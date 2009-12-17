/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies). 
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:   Client for fotaserver
*
*/




// INCLUDES
#include <bldvariant.hrh>
#include "fotaengine.h"
#include "FotaIPCTypes.h"
#include "fotaConst.h"
#include "fotaenginedebug.h"
#include <apgcli.h>
#include <AknServerApp.h>

// CONSTANTS
_LIT(KServerNameFormat, "%08x_%08x_AppServer");        

// =================== LOCAL FUNCTIONS ========================================

// ---------------------------------------------------------------------------
// IsClientFota()   Checks if client is another fotaserver
// ---------------------------------------------------------------------------
TBool IsClientFota()
    {
    RThread thread;
    TUid    fota,dlmgr;
    fota.iUid           = KFotaServerUid;
    dlmgr.iUid			= KDLMgrServerUid;
    if (thread.SecureId() == fota.iUid || thread.SecureId() == dlmgr.iUid )
        {
        return ETrue;
        }
    return EFalse;
    }


// ---------------------------------------------------------------------------
// RFotaEngineSession::StartServerL() 
// Start application server
// ---------------------------------------------------------------------------
//
void RFotaEngineSession::StartApplicationL( const TUid& aNameUid
                                            , const TUid& aAppServerUid )
    {
    TInt err;
    FLOG(_L("RFotaEngineSession::StartApplicationL >>"));
    RApaLsSession apa;
    err = apa.Connect();
    User::LeaveIfError(err);
    CleanupClosePushL(apa);

    // Get application information
    TApaAppInfo info;
    err=0;
    for(TInt i = 20; ((err = apa.GetAppInfo(info, 
	    aAppServerUid)) == RApaLsSession::EAppListInvalid) && i > 0; i--)
        {
        User::After(500000);
        }
    User::LeaveIfError(err);

    // Start aplication server
    CApaCommandLine* cmdLine = CApaCommandLine::NewLC();
    cmdLine->SetExecutableNameL(info.iFullName);
    cmdLine->SetServerRequiredL( aNameUid.iUid );
    cmdLine->SetCommandL(EApaCommandBackground);
    TThreadId   srvid;
    err = apa.StartApp(*cmdLine, srvid);
    User::LeaveIfError(err);

    // Wait until server is running.

    // Rendezvous() is not reliable for synchronising with the new server
    // in this case as we may not be able
    // to open the server thread before it has reached its rendezvous
    // point, in which case we hang.
    // So, the standby algorithm is to poll for server existence (yuk)
	const TInt maxPoll = 100;
	const TInt waitDelay = 100000;	// 0.1 seconds
    TFullName serverName;
    serverName.Format(KServerNameFormat, aNameUid, aAppServerUid);
	for (TInt ii = 0; ii < maxPoll; ii++)
		{
		// look for the server name
		TFindServer find(serverName);
		TFullName fullName;
        err = find.Next(fullName);
		if ( err == KErrNone)
			{
			break;		// found the server, so return
			}
		User::After(waitDelay);			// wait before trying again
		}
	User::LeaveIfError(err);	// failed to find the server, bomb out

    CleanupStack::PopAndDestroy(2, &apa);	// cmdLine and apa
    FLOG(_L("RFotaEngineSession::StartApplicationL <<"));
    }


// ---------------------------------------------------------------------------
// RFotaEngineSession::StartServerL() 
// Connect to existing server
// ---------------------------------------------------------------------------
//
void RFotaEngineSession::ConnectToServerL( const TUid& aNameUid
                                            , const TUid& aAppServerUid )
    {
    RWsSession  ws;
    TInt        err;

    FLOG(_L("RFotaEngineSession::ConnectToServerL >>"));
    if (aAppServerUid == KNullUid)
        {
        User::Leave(KErrGeneral);
        }

	// Connect to server
    TFullName serverName;
	serverName.Format(KServerNameFormat, 
	aNameUid, aAppServerUid);
    TRAP(err, ConnectExistingByNameL(serverName) );
    if(err)
        {
        TVersion vers(0,0,1);
        err = CreateSession (serverName, vers);
        User::LeaveIfError(err);
        }
    FLOG(_L("       3"));

    FLOG(_L("RFotaEngineSession::ConnectToServerL <<"));
    }
    
// ---------------------------------------------------------------------------
// RFotaEngineSession::OpenL( )
// Opens session to fotaserver. 
// ---------------------------------------------------------------------------
EXPORT_C void RFotaEngineSession::OpenL( )
{
    RProcess pr; TFullName fn = pr.FullName(); 
    TInt err(0);
    TUid diff1 = TUid::Uid(KUikonUidPluginInterfaceNotifiers);
    FLOG(_L("[RFotaEngineSession] OpenL\tcalled by '%S' >>"),&fn );

    // -------------------------------------------- V
    err = KErrNotFound;
    // If client is fotaserver - MUST create new server
    if( !IsClientFota() ) 
        {
        TRAP(err, ConnectToServerL( diff1 , TUid::Uid(KFotaServerUid)) );
        }
    else
        {
        diff1 = TUid::Uid(KUikonUidPluginInterfaceNotifiers+1);
        }

    if(err!=KErrNone)
        {
        StartApplicationL( diff1 , TUid::Uid(KFotaServerUid));
        ConnectToServerL( diff1 , TUid::Uid(KFotaServerUid));
        }
    FLOG(_L("[RFotaEngineSession]\tconnected <<") );
}


// ---------------------------------------------------------------------------
// RFotaEngineSession::Close( )
// Closes session to fotaserver
// ---------------------------------------------------------------------------
EXPORT_C void RFotaEngineSession::Close( )
{
    RProcess pr; TFullName fn = pr.FullName(); 
    FLOG(_L("[RFotaEngineSession] RFotaEngineSession::Close() >> called by '%S'"),&fn );
        
    if ( iStream )
        {
        iStream->Close(); // this uses iChunk
        delete iStream;  iStream=0;
        }
    iChunk.Close();

    // Tell server that generic alert is sent for this pkg, so state is 
    // cleaned up     
    if ( iGenericAlertSentPkgID != -1 )
        {
        TInt err = SendReceive( EGenericAlertSentForPackage
                                    , TIpcArgs(iGenericAlertSentPkgID) );
        }

    REikAppServiceBase::Close();
    FLOG(_L("[RFotaEngineSession] RFotaEngineSession::Close() <<") );
}


// ---------------------------------------------------------------------------
// RFotaEngineSession::Download
// Starts download of upd package.
// ---------------------------------------------------------------------------
EXPORT_C TInt RFotaEngineSession::Download(const TInt aPkgId
    , const TDesC8& aPkgURL, const TSmlProfileId aProfileId
    , const TDesC8& aPkgName, const TDesC8& aPkgVersion)
{
    TInt err;
    TDownloadIPCParams          ipcparam;
    ipcparam.iPkgId             = aPkgId;
    ipcparam.iProfileId         = aProfileId;
    ipcparam.iPkgName.Copy      (aPkgName);
    ipcparam.iPkgVersion.Copy   (aPkgVersion);
    TPckg<TDownloadIPCParams>   pkg(ipcparam);
    err = SendReceive ( EFotaDownload ,  TIpcArgs(&pkg, &aPkgURL));
    return err;
}

// ---------------------------------------------------------------------------
// RFotaEngineSession::DownloadAndUpdate
// Starts download and update of update pakcage
// ---------------------------------------------------------------------------
EXPORT_C TInt RFotaEngineSession::DownloadAndUpdate(const TInt aPkgId
        ,const TDesC8& aPkgURL, const TSmlProfileId aProfileId
        ,const TDesC8& aPkgName, const TDesC8& aPkgVersion)
{
    TInt err;
    TDownloadIPCParams          ipcparam;
    ipcparam.iPkgId             = aPkgId;
    ipcparam.iProfileId         = aProfileId;
    ipcparam.iPkgName.Copy      (aPkgName);
    ipcparam.iPkgVersion.Copy   (aPkgVersion);
    TPckg<TDownloadIPCParams>   pkg(ipcparam);
    err = SendReceive( EFotaDownloadAndUpdate,  TIpcArgs(&pkg, &aPkgURL));
    return err;
}

// ---------------------------------------------------------------------------
// RFotaEngineSession::Update
// Starts update of update package
// ---------------------------------------------------------------------------
EXPORT_C TInt RFotaEngineSession::Update(const TInt aPkgId
        ,const TSmlProfileId aProfileId, const TDesC8& aPkgName
        ,const TDesC8& aPkgVersion)
{
    TInt err;
    TDownloadIPCParams          ipcparam;
    ipcparam.iPkgId             = aPkgId;
    ipcparam.iProfileId         = aProfileId;
    ipcparam.iPkgName.Copy      (aPkgName);
    ipcparam.iPkgVersion.Copy   (aPkgVersion);
    TPckg<TDownloadIPCParams>   pkg(ipcparam);
    err = SendReceive ( EFotaUpdate ,  TIpcArgs(&pkg));
    return err;
}


// ---------------------------------------------------------------------------
// RFotaEngineSession::IsPackageStoreSizeAvailable
// Checks if there's space enough for update package
// ---------------------------------------------------------------------------
EXPORT_C TBool RFotaEngineSession::IsPackageStoreSizeAvailable(
                                                    const TInt aSize)
    {
//    // -------------------------------------------- v 
//    if( IsClientFota() ) 
//        {
//        FLOG(_L("     IsPackageStoreSizeAvailable  NOT CONNECTING"));
//        return ETrue;
//        }
//    // -------------------------------------------- ^ 
    TInt            err;
    TBool           available;
    TPckg<TBool>    pavailable(available);
    err = SendReceive ( EIsPackageStoreSizeAvailable, TIpcArgs(aSize
                                                        , &pavailable ) );
    if ( err ) 
        {   
        FLOG(_L("RFotaEngineSession::IsPackageStoreSizeAvailable error %d")
                    ,err);
        }
    return available;
    }


// ---------------------------------------------------------------------------
// RFotaEngineSession::OpenUpdatePackageStore
// OPens update package storage for writing.
// ---------------------------------------------------------------------------
EXPORT_C TInt RFotaEngineSession::OpenUpdatePackageStore(const TInt aPkgId
                                                 ,RWriteStream*& aPkgStore)
	{
    FLOG(_L( "RFotaEngineSession::OpenUpdatePackageStore >> pkgid %d " )
                    ,aPkgId );
    TInt err;
    err = iChunk.CreateGlobal( KNullDesC, KFotaChunkMinSize, KFotaChunkMaxSize );
    if(err) return err;
    iStream = new RFotaWriteStream();
    iStream->iFotaEngineSession = this;    
    TRAP( err, iStream->OpenL(aPkgId) );
    aPkgStore = iStream;
    if(err) return err;

    TIpcArgs args;
    args.Set(0,aPkgId);
    args.Set(1,iChunk );
	err = SendReceive( EFotaOpenUpdatePackageStore, args );
    FLOG(_L( "RFotaEngineSession::OpenUpdatePackageStore << err  %d" ),err );
    return err;
    }
// ---------------------------------------------------------------------------
// RFotaEngineSession::GetDownloadUpdatePackageSize
// Gets the downloaded and full size of the update package. 
// Implementation is not ready and will be provided later.
// ---------------------------------------------------------------------------
EXPORT_C TInt RFotaEngineSession::GetDownloadUpdatePackageSize(const TInt aPkgId, TInt& aDownloadedSize, TInt& aTotalSize)
	{
	FLOG(_L("RFotaEngineSession::GetDownloadUpdatePackageSize, aPkgId=%d >>"),aPkgId);
	TInt err (KErrNone);
    TPckg<TInt> pkg1(aDownloadedSize);
    TPckg<TInt>	pkg2(aTotalSize);
	err = SendReceive( EFotaGetDownloadUpdatePackageSize,TIpcArgs(aPkgId,&pkg1, &pkg2)  );
	FLOG(_L("RFotaEngineSession::GetDownloadUpdatePackageSize << err = %d, aDownloadedSize = %d, aTotalSize = %d" ), err, aDownloadedSize, aTotalSize);
	return err;
	}

// ---------------------------------------------------------------------------
// RFotaEngineSession::TryResumeDownload
// Requests to resume the suspended download of the update package. 
// Implementation is not ready and will be provided later.
// ---------------------------------------------------------------------------
EXPORT_C TInt RFotaEngineSession::TryResumeDownload()
	{
	FLOG(_L("RFotaEngineSession::TryResumeDownload >>"));

	TInt err = KErrNone;
	
	err = SendReceive( EFotaTryResumeDownload );
	
	FLOG(_L("RFotaEngineSession::TryResumeDownload << err = %d" ),err);
	return err;
	}

// ---------------------------------------------------------------------------
// RFotaEngineSession::UpdatePackageDownloadComplete
// Ends update pkg storing. Closes resources.
// ---------------------------------------------------------------------------
EXPORT_C void RFotaEngineSession::UpdatePackageDownloadComplete(
                                                          const TInt aPkgId)
    {
    FLOG(_L("RFotaEngineSession::UpdatePackageDownloadComplete >> id %d")
                            ,aPkgId);
    if ( iStream )
        {
        iStream->Close();
        delete iStream;
        iStream=0;
        }
    TInt err = SendReceive(EUpdatePackageDownloadComplete, TIpcArgs(aPkgId) );
    FLOG(_L("RFotaEngineSession::UpdatePackageDownloadComplete << error %d ")
                            ,err);
    }


// ---------------------------------------------------------------------------
// RFotaEngineSession::GetState
// ---------------------------------------------------------------------------
EXPORT_C RFotaEngineSession::TState RFotaEngineSession::GetState(
                                                         const TInt aPkgId)
    {
    FLOG(_L("RFotaEngineSession::GetState"));
    TInt err(0);
    RFotaEngineSession::TState          state;
    TPckg<RFotaEngineSession::TState>   pkgstate(state);
    err = SendReceive ( EGetState , TIpcArgs(aPkgId, &pkgstate));
    if ( err ) 
        {
        FLOG(_L("RFotaEngineSession::GetState error %d"),err);
        }
    return state;
    }


// ---------------------------------------------------------------------------
// RFotaEngineSession::GetResult    
// ---------------------------------------------------------------------------
EXPORT_C TInt RFotaEngineSession::GetResult(const TInt aPkgId)
    {
    FLOG(_L("RFotaEngineSession::GetResult"));
    TInt            result;
    TPckg<TInt>     pkgresult(result);
    SendReceive ( EGetResult , TIpcArgs(aPkgId, &pkgresult));
    return result;
    }


// ---------------------------------------------------------------------------
// RFotaEngineSession::DeleteUpdatePackage
// ---------------------------------------------------------------------------
EXPORT_C TInt RFotaEngineSession::DeleteUpdatePackage(const TInt aPkgId)
{
    TInt err = SendReceive(EDeletePackage, TIpcArgs(aPkgId) );
    return err;
}


// ---------------------------------------------------------------------------
// RFotaEngineSession::LastUpdate
// ---------------------------------------------------------------------------
EXPORT_C TInt RFotaEngineSession::LastUpdate(TTime& aUpdates)
    {
    TInt err;
    FLOG(_L("RFotaEngineSession::LastUpdate >>"));
    TBuf<15>                    timestamp;
    err = SendReceive ( EGetUpdateTimestamp, TIpcArgs(&timestamp) );

    if ( timestamp.Length() > 0 )
        {
        TInt year   = timestamp[0];
        TInt month  = timestamp[1];
        TInt day    = timestamp[2];
        TInt hour   = timestamp[3];
        TInt minute = timestamp[4];
        aUpdates = TDateTime (year,(TMonth)month,day,hour,minute,0,0 );
        }
    else
        {
        aUpdates.Set( _L( "19900327:101010.000000" ) ); 
        err = KErrUnknown;
        }
    FLOG(_L("RFotaEngineSession::LastUpdate <<"));
    return err;
    }


// ---------------------------------------------------------------------------
// RFotaEngineSession::CurrentVersion
// ---------------------------------------------------------------------------
EXPORT_C TInt RFotaEngineSession::CurrentVersion(TDes& aSWVersion)
    {
    aSWVersion.Copy(_L("1.0"));  
    return KErrNone;
    }


// ---------------------------------------------------------------------------
// RFotaEngineSession::ExtensionInterface
// ---------------------------------------------------------------------------
EXPORT_C void RFotaEngineSession::ExtensionInterface(TUid /*aInterfaceId*/
                                              ,TAny*& /*aImplementation*/)
    {
    RProcess pr; TFullName fn = pr.FullName(); 
    FLOG(_L("RFotaEngineSession::ExtensionInterface called by %S"), &fn);
    }


// ---------------------------------------------------------------------------
// RFotaEngineSession::RFotaEngineSession
// ---------------------------------------------------------------------------
EXPORT_C RFotaEngineSession::RFotaEngineSession() : iStream(0)
                    , iGenericAlertSentPkgID(-1)
    {
    FLOG(_L("RFotaEngineSession::RFotaEngineSession() >>"));
    FLOG(_L("RFotaEngineSession::RFotaEngineSession() <<"));
    }


// ---------------------------------------------------------------------------
// RFotaEngineSession::GetUpdatePackageIds
// Gets ids of the update packages present in the system.
// ---------------------------------------------------------------------------
EXPORT_C TInt RFotaEngineSession::GetUpdatePackageIds(TDes16& aPackageIdList)
    {
    TInt err;
    FLOG(_L("RFotaEngineSession::GetUpdatePackageIds >>"));
    TBuf<10> b; b.Copy(_L("dkkd"));
    TPkgIdList                  pkgids;
    TPckg<TPkgIdList>           pkgids_pkg(pkgids);
    TIpcArgs                    args ( &pkgids_pkg);
    err = SendReceive ( EGetUpdatePackageIds, args);
    aPackageIdList.Copy(pkgids);
    FLOG(_L("RFotaEngineSession::GetUpdatePackageIds <<"));
    return err;
    }



// ---------------------------------------------------------------------------
// RFotaEngineSession::GenericAlertSentL
// marks genereic alert being sent
// ---------------------------------------------------------------------------
EXPORT_C void RFotaEngineSession::GenericAlertSentL ( const TInt aPackageID )
    {
    iGenericAlertSentPkgID = aPackageID;
    }

// ---------------------------------------------------------------------------
// RFotaEngineSession::ScheduledUpdateL
// Update fw
// ---------------------------------------------------------------------------
EXPORT_C TInt RFotaEngineSession::ScheduledUpdateL ( const TFotaScheduledUpdate aSchedule )
    {
  	TInt err(KErrNotSupported);

    TPckg<TFotaScheduledUpdate> p(aSchedule);
    err = SendReceive( EScheduledUpdate, TIpcArgs(&p) );

    return err;
    }

// ---------------------------------------------------------------------------
// RFotaEngineSession::ServiceUid
// Apparc asks which session class to create in server side
// ---------------------------------------------------------------------------
TUid RFotaEngineSession::ServiceUid() const
	{
    RProcess pr; TFullName fn = pr.FullName(); 
    FLOG(_L( "RFotaEngineSession::ServiceUid() >> called by: %S" ), &fn );
    FLOG(_L( "RFotaEngineSession::ServiceUid() << ret: 0x%X" ),
                                            KFotaServiceUid );
	return TUid::Uid( KFotaServiceUid );
	}


// ---------------------------------------------------------------------------
// RFotaEngineSession::SendChunkL
// Tells server to read chunk contnets
// ---------------------------------------------------------------------------
void RFotaEngineSession::SendChunkL(const TUint8* aP1, const TUint8* aP2)
    {
    TInt writecount = aP2-aP1;
    TInt err = SendReceive(EFotaSendChunk, TIpcArgs(writecount) );

    if ( err )
        {   
        FLOG(_L("RFotaEngineSession::SendChunkL error %d"),err);
        }
    User::LeaveIfError ( err );
    }


// ---------------------------------------------------------------------------
// RFotaEngineSession::ReleaseChunkHandle()
// Releases server's handle to the chuhnk
// ---------------------------------------------------------------------------
TInt RFotaEngineSession::ReleaseChunkHandle()
	{
	return Send( EFotaReleaseChunkHandle);
	}
