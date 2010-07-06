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
#include <fotaengine.h>
#include "FotaIPCTypes.h"
#include "fotaConst.h"
#include "fotaenginedebug.h"
#include <apgcli.h>

       

// =================== LOCAL FUNCTIONS ========================================


// ---------------------------------------------------------------------------
// StartServerL()  
//  Starts Fota Downlaod Interrupt Monitory server
// ---------------------------------------------------------------------------
TInt StartServerL()
    {
    FLOG(_L("StartServerLt Started"));
    TInt res = KErrNone;
    
    RProcess server;
    RSemaphore sem;
    res = sem.CreateGlobal(KFotaServerScem, EOwnerProcess);

    res=server.Create(KFotaServerName,KNullDesC);
    FLOG(_L("StartServerL-- create server error as %d"),res);
    if (res!=KErrNone)
        {       
        return res;
        }

    server.Resume();    // logon OK - start the server
    
    sem.Wait();
    sem.Close();
    server.Close();
    
    FLOG(_L("StartServerL-- server.ExitType() returns %d"),res);

    return res;
    }

/**
 * Connects to the  server using 4 message slots.
 */
TBool RFotaEngineSession::connectToHbServer()
{
    FLOG(_L("RFotaEngineSession::connectToHbServer >>"));

    TInt error = CreateSession(KFotaServerName,TVersion(1,0,0),2);

    if (KErrNone != error) {
        FLOG(_L("RFotaEngineSession::Error: %d"), error);
    }
    FLOG(_L("RFotaEngineSession::connectToHbServer <<"));
    return (KErrNone == error); 
}

// ---------------------------------------------------------------------------
// RFotaEngineSession::OpenL( )
// Opens session to fotaserver. 
// ---------------------------------------------------------------------------
EXPORT_C void RFotaEngineSession::OpenL( )
{
    RProcess pr; TFullName fn = pr.FullName(); 

    TInt res = KErrNone;
    FLOG(_L("[RFotaEngineSession] OpenL\tcalled by '%S' >>"),&fn );
    
    res = connectToHbServer();
    if(!res)
        {
    FLOG(_L("Creation of session failed with error: %d; starting the server now..."), res);
        res = StartServerL();
        
        FLOG(_L("Starting of the server, error = %d"), res);
        User::LeaveIfError( res );

        res = CreateSession( KFotaServerName,TVersion(1,0,0),2 );
        FLOG(_L("Creation of session,  error: %d; "), res);
        User::LeaveIfError (res);
    
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
        
    // Tell server that generic alert is sent for this pkg, so state is 
    // cleaned up     
    FLOG(_L("iGenericAlertSentPkgID = %d"), iGenericAlertSentPkgID);
    if ( iGenericAlertSentPkgID != -1 )
        {
        TInt err = SendReceive( EGenericAlertSentForPackage
                                    , TIpcArgs(iGenericAlertSentPkgID) );
        }

    RSessionBase::Close();
    FLOG(_L("[RFotaEngineSession] RFotaEngineSession::Close() <<") );
}


// ---------------------------------------------------------------------------
// RFotaEngineSession::Download
// Starts download of upd package.
// ---------------------------------------------------------------------------
EXPORT_C TInt RFotaEngineSession::Download(const TInt aPkgId
    , const TDesC8& aPkgURL, const TSmlProfileId aProfileId
    , const TDesC8& aPkgName, const TDesC8& aPkgVersion, TBool aSilent)
{
    TInt err;
    TDownloadIPCParams          ipcparam;
    ipcparam.iPkgId             = aPkgId;
    ipcparam.iProfileId         = aProfileId;
    ipcparam.iPkgName.Copy      (aPkgName);
    ipcparam.iPkgVersion.Copy   (aPkgVersion);
    TPckg<TDownloadIPCParams>   pkg(ipcparam);
    err = SendReceive ( EFotaDownload ,  TIpcArgs(&pkg, &aPkgURL, aSilent));
    return err;
}

// ---------------------------------------------------------------------------
// RFotaEngineSession::DownloadAndUpdate
// Starts download and update of update pakcage
// ---------------------------------------------------------------------------
EXPORT_C TInt RFotaEngineSession::DownloadAndUpdate(const TInt aPkgId
        ,const TDesC8& aPkgURL, const TSmlProfileId aProfileId
        ,const TDesC8& aPkgName, const TDesC8& aPkgVersion, TBool aSilent)
{
    TInt err;
    TDownloadIPCParams          ipcparam;
    ipcparam.iPkgId             = aPkgId;
    ipcparam.iProfileId         = aProfileId;
    ipcparam.iPkgName.Copy      (aPkgName);
    ipcparam.iPkgVersion.Copy   (aPkgVersion);
    TPckg<TDownloadIPCParams>   pkg(ipcparam);
    err = SendReceive( EFotaDownloadAndUpdate,  TIpcArgs(&pkg, &aPkgURL, aSilent));
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

// --------------------------------------------------------------------------
// RFotaEngineSession::PauseDownload
// To pause the download
// ---------------------------------------------------------------------------
EXPORT_C TInt RFotaEngineSession::PauseDownload ( )
    {
    TInt err(KErrNotSupported);

    err = SendReceive( EPauseDownload );

    return err;
    }

// ---------------------------------------------------------------------------
// RFotaEngineSession::TryResumeDownload
// Requests to resume the suspended download of the update package. 
// Implementation is not ready and will be provided later.
// ---------------------------------------------------------------------------
EXPORT_C TInt RFotaEngineSession::TryResumeDownload(TBool aSilent)
	{
	FLOG(_L("RFotaEngineSession::TryResumeDownload >>"));

	TInt err = KErrNone;
	
	err = SendReceive( EFotaTryResumeDownload, TIpcArgs( aSilent) );
	
	FLOG(_L("RFotaEngineSession::TryResumeDownload << err = %d" ),err);
	return err;
	}


// ---------------------------------------------------------------------------
// RFotaEngineSession::GetState
// ---------------------------------------------------------------------------
EXPORT_C RFotaEngineSession::TState RFotaEngineSession::GetState(
                                                         const TInt aPkgId)
    {
    FLOG(_L("RFotaEngineSession::GetState >>"));

    RFotaEngineSession::TState          state;
    TPckg<RFotaEngineSession::TState>   pkgstate(state);
    SendReceive ( EGetState , TIpcArgs(aPkgId, &pkgstate));
    FLOG(_L("RFotaEngineSession::GetState <<"));

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
// RFotaEngineSession::RFotaEngineSession
// ---------------------------------------------------------------------------
EXPORT_C RFotaEngineSession::RFotaEngineSession() : iGenericAlertSentPkgID(-1)
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


EXPORT_C TVersion RFotaEngineSession::Version() const
    {
    TVersion version (1,0,0);
      
    return version;
    }

// ---------------------------------------------------------------------------
// RFotaEngineSession::GenericAlertSentL
// marks genereic alert being sent
// ---------------------------------------------------------------------------
EXPORT_C void RFotaEngineSession::GenericAlertSentL ( const TInt aPackageID )
    {
    FLOG(_L("RFotaEngineSession::GenericAlertSentL, aPackageID = %d >>"),aPackageID);
    iGenericAlertSentPkgID = aPackageID;
    FLOG(_L("RFotaEngineSession::GenericAlertSentL, iGenericAlertSentPkgID = %d <<"), iGenericAlertSentPkgID);
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

EXPORT_C void RFotaEngineSession::GetCurrentFirmwareDetailsL(TDes8& aName, TDes8& aVersion, TInt& aSize)
    {
    FLOG(_L("RFotaEngineSession::GetCurrentFirmwareDetailsL >>"));
    
    TPckg<TInt>     pkgsize(aSize);
    
    __ASSERT_ALWAYS(aName.MaxLength() >= KFotaMaxPkgNameLength, User::Leave(KErrArgument) );
    __ASSERT_ALWAYS(aName.MaxLength() >= KFotaMaxPkgVersionLength, User::Leave(KErrArgument) );
    
    TInt err = SendReceive(EGetCurrFwDetails, TIpcArgs(&aName, &aVersion, &pkgsize));
    
    FLOG(_L("RFotaEngineSession::GetCurrentFirmwareDetailsL, err = %d <<"), err);
    User::LeaveIfError(err);
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
	
/*
 * The following exported functions are not supported in SF4
 * They would be removed once stakeholder remove dependency with them.
 * Polonium CR: 530
 */ 
	
EXPORT_C TBool RFotaEngineSession::IsPackageStoreSizeAvailable(
        const TInt aSize)
    {
    return EFalse;
    }

EXPORT_C TInt RFotaEngineSession::OpenUpdatePackageStore(const TInt aPkgId,
        RWriteStream*& aPkgStore)
    {
    return KErrNotSupported;
    }

EXPORT_C TInt RFotaEngineSession::GetDownloadUpdatePackageSize(
        const TInt aPkgId, TInt& aDownloadedSize, TInt& aTotalSize)
    {
    return KErrNotSupported;
    }

EXPORT_C void RFotaEngineSession::UpdatePackageDownloadComplete(
        const TInt aPkgId)
    {
    
    }

