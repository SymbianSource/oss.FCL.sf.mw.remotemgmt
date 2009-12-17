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
* Description:   Fota server update and download functionality 
*
*/



// INCLUDE FILES
#include <aknenv.h>
#include <apgtask.h>
#include <apgwgnam.h>
#include <schtime.h>
#include <csch_cli.h>
#include <e32property.h>
#include <ApUtils.h> 
#include <commdb.h>
#include <DevManInternalCRKeys.h>
#include <nsmlconstants.h>
#include <centralrepository.h>
#include <sysversioninfo.h>
#include "FotaSrvApp.h"
#include "FotaServer.h"
#include "FotaSrvDebug.h"
#include "FotasrvSession.h"
#include "fotaConst.h"
#include "nsmldmtreedbclient.h"
#include "fotaserverPrivatePSKeys.h"
#include "FotaNetworkRegStatus.h"
_LIT(KSizePass, "contentsize");
#include "DevEncController.h"
#define __LEAVE_IF_ERROR(x) if(KErrNone!=x) {FLOG(_L("LEAVE in %s: %d"), __FILE__, __LINE__); User::Leave(x); }
// ============================= LOCAL FUNCTIONS =============================

// ---------------------------------------------------------------------------
// ToDesC16LC
// Converts to 16bit desc
// ---------------------------------------------------------------------------
HBufC* ToDesC16LC( const TDesC8& a )
    {
    HBufC* bf = HBufC::NewLC( a.Length() );
    bf->Des().Copy(a);
    return bf;
    }
// ---------------------------------------------------------------------------
// GetPredefinedNodeL
// function to get preconfigured node for FUMO
// ---------------------------------------------------------------------------
void GetPredefinedNodeL(TDes8& aNode)
	{
	FLOG(_L("GetPredefinedNodeL() Begin"));
    CRepository* centrep( NULL);
	aNode.Zero();
   	centrep = CRepository::NewLC( TUid::Uid(0x101F9A0A) );

    if ( centrep )
	    {
	    FLOG(_L("centralrepository found End"));
    	TFullName temp;
		
    	if (centrep->Get( KDevManFUMOPredefinedNodes, temp )==KErrNone && temp.Length() )
			{
			temp.Trim();
			aNode.Copy(temp);
			}
		CleanupStack::PopAndDestroy(centrep);
	    }
	FLOG(_L("GetPredefinedNodeL() End"));	    
	}
// ---------------------------------------------------------------------------
// DeleteFUMOTreeL
// function to get preconfigured node for FUMO
// ---------------------------------------------------------------------------
void DeleteFUMOTreeL ()
    {
    FLOG(_L("DeleteFUMOTreeL() Begin"));
    const TUint32 KNSmlFotaAdapterUid = 0x101F9A09;
    _LIT8( KNSmlFumoPath, "FUMO" );
    _LIT8( KNSmlFumoSeparator, "/" );
    const TInt KGranularity = 10;
    TBuf8<KMaxFullName> temp;
	GetPredefinedNodeL(temp);
    RNSmlDMCallbackSession session;
    User::LeaveIfError(session.Connect());
    CleanupClosePushL(session);
    
    CBufBase *emptyList = CBufFlat::NewL(0);
    CleanupStack::PushL(emptyList);
    CArrayFixFlat <TSmlDmMappingInfo>* UriSegList;
    UriSegList = new (ELeave) CArrayFixFlat <TSmlDmMappingInfo> (KGranularity);
    
    CleanupStack::PushL(UriSegList);
			
    session.GetUriSegListL(KNSmlFotaAdapterUid,KNSmlFumoPath,*UriSegList);
    
    for ( TUint16 i=0;i<UriSegList->Count();i++)
        {
        	if (temp.Length() && UriSegList->At(i).iURISeg.Find( temp ) != KErrNotFound)
            {
              FLOG(_L("DeleteFUMOTreeL predefined match =%S"), &temp);
              continue;
            }
       
       	TBuf8<KMaxFullName> tempFumo;
       	tempFumo.Append(KNSmlFumoPath);
       	tempFumo.Append(KNSmlFumoSeparator);
       	tempFumo.Append(UriSegList->At(i).iURISeg);
       	FLOG(_L("DeleteFUMOTreeL() call update mapping info for node %S"),&tempFumo);
        session.UpdateMappingInfoL(KNSmlFotaAdapterUid,tempFumo,*emptyList);
        
        }
    // session.UpdateMappingInfoL(KNSmlFotaAdapterUid,KNSmlFumoPath,*emptyList);
	CleanupStack::PopAndDestroy(UriSegList); 
    CleanupStack::PopAndDestroy(emptyList); 
    CleanupStack::PopAndDestroy(&session); //session
    FLOG(_L("DeleteFUMOTreeL() End"));
    }

// ---------------------------------------------------------------------------
// CFotaServer::GetProfileDataL
// ---------------------------------------------------------------------------
void CFotaServer::GetProfileDataL( RSyncMLSession* aSyncml
    ,const TSmlProfileId aProf, TInt& aIapId, TSmlServerAlertedAction& aSrvAA
    ,TBool& aUIJob, TInt& aSanVal)
	{
    FLOG(_L("CFotaServer::GetProfileDataL prof %d"),aProf);
    __ASSERT_ALWAYS( aSyncml, User::Panic(KFotaPanic, KErrArgument) );
    __ASSERT_ALWAYS( aSyncml->Handle(), User::Panic(KFotaPanic, KErrBadHandle) );
    // GET IAP FROM PROFILE ----------------------------------------------
    TSmlJobId               jobid( KErrNotFound );
    TInt                    jobidFromUI( KErrNotFound );
    RSyncMLDevManProfile    smlprof;
    RArray<TSmlTransportId> connections;
    TSmlTransportId         transport;
    RSyncMLConnection       connection;
                   
    CleanupClosePushL(smlprof);
    CleanupClosePushL(connections);
    CleanupClosePushL(connection);
    FLOG(_L("[FotaServer]   1"));
    
    FLOG(_L("[FotaServer]   1.1 opening syncml \
                        profileid %d "), aProf);
    smlprof.OpenL(*aSyncml, aProf, ESmlOpenRead);
    FLOG(_L("[FotaServer]  1.1"));
    smlprof.ListConnectionsL(connections);
    FLOG(_L("[FotaServer]  1.3"));
    transport = connections[0];
    connection.OpenL(smlprof,transport);
    TBuf8<20> iapid2 = connection.GetPropertyL ( KNSmlIAPId );
    TLex8 iapid3(iapid2);
    iapid3.Val(aIapId);

    // 1. Profile's "accepted" field.
    aSrvAA = smlprof.SanUserInteraction ();
    FLOG(_L("[FotaServer]  1.4 \
                    SanUserInteraction:%d"),aSrvAA );

    // 2. Has job been started from omadmappui. That is, did user start the job
    TSmlUsageType dmusagetype(ESmlDevMan);
    aSyncml->CurrentJobL( jobid,dmusagetype );
    RProperty::Get( TUid::Uid(KOmaDMAppUid), KNsmlCurrentJobId, jobidFromUI);
    aUIJob = ( jobidFromUI == jobid && jobid!=KErrNotFound );
    
    aSanVal = KErrNotFound; 
    if ( aUIJob )
    	{
    	//First for not server alert or SAN not supported value is -1
    	RProperty::Get( TUid::Uid(KOmaDMAppUid), KNsmlSanUIMode, aSanVal);
    	if ( aSanVal != -1 ) //SAN Supported
    	    {
   	    	aUIJob = 0;
    	    }
    	}
    FLOG(_L("  jobidFromUI: %d jobid: %d, aUIJob: %d, aSanVal: %d"),jobidFromUI,jobid,aUIJob,aSanVal);
    CleanupStack::PopAndDestroy (&connection); 
    CleanupStack::PopAndDestroy (&connections);
    CleanupStack::PopAndDestroy (&smlprof); 
	}

// --------------------------------------------------------------------------
// CreateDeviceManagementSessionL       
// Creates DM session 
// --------------------------------------------------------------------------
void CFotaServer::CreateDeviceManagementSessionL( TPackageState& aState )
    {
    FLOG(_L("[cfotasever]   CreateDeviceManagementSessionL dms >> profid %d\
    %d counts left ,sml handle %d, iNetworkAvailable=%d"), aState.iProfileId, aState.iSmlTryCount 
    , iSyncMLSession.Handle(),iNetworkAvailable);
	
    SetStartupReason(EFotaPendingGenAlert);

	if (!iNetworkAvailable)
		{
		iRetryingGASend = ETrue;
		iStoredState = aState;
		StartNetworkMonitorL();
		}
	else 
		{

	    TBool dbAlreadyOpen = iDatabase->IsOpen();
	    TBool triesLeft( EFalse );
 		
 		iNetworkAvailable = EFalse; // to check network again when GA is sent next time
 		
	    if ( iSyncMLSession.Handle() )
	        {
	        User::Leave( KErrAlreadyExists );
	        }

	    if ( !dbAlreadyOpen ) iDatabase->OpenDBL();
	    // There is still tries left
	    if ( aState.iSmlTryCount > 0 )
	        {
	        triesLeft = ETrue;
	        FLOG(_L(" decrementing the retry count"));
	        aState.iSmlTryCount = aState.iSmlTryCount - 1;
	        iDatabase->SetStateL( aState, KNullDesC8, EFDBSmlTryCount );
	        }
	    // out of tries, set state to idle
	    else
	        {
	        triesLeft = EFalse;
	        FLOG(_L("   out of tries, resetting pkg state"));
	        
	        SetStartupReason(EFotaDefault);
	        aState.iState = RFotaEngineSession::EIdle;
	        aState.iResult = KErrNotFound;
	        iDatabase->SetStateL( aState, KNullDesC8, EFDBState|EFDBResult );
	        }
	    if ( !dbAlreadyOpen ) iDatabase->CloseAndCommitDB();

	    if ( triesLeft )
	        {
	        TInt iapid;
	        TSmlServerAlertedAction tmp;
	        TBool                   tmp2;
	        TInt					tmp3;
	        iSyncMLSession.OpenL(); 
	        GetProfileDataL( &iSyncMLSession,aState.iProfileId,iapid,tmp,tmp2,tmp3);
	        FLOG(_L("Using IAP: %d to send GA"),iapid);
	        FLOG(_L("From Db Using IAP: %d to send GA"),aState.iIapId);
	        RSyncMLDevManJob    dmJob;
	        TBuf<10> genalertap,temp;
	        genalertap.Zero();
	        temp.Zero();	  
	        genalertap.Append(KNSmlDMJobIapPrefix);
	        temp.Num(aState.iIapId);//Decimal Iap
	        if( temp.Length() <= KNSmlHalfTransportIdLength && 
	            aState.iIapId > KErrNotFound && CheckIapExistsL(aState.iIapId) )
	        	{
	        	genalertap.AppendFill('0',KNSmlHalfTransportIdLength-temp.Length());
	        	genalertap.Append(temp);
	        	TLex gavalue(genalertap);
	        	gavalue.Val(iIapId);
	        	dmJob.CreateL( iSyncMLSession, aState.iProfileId,iIapId);	
	        	iSyncMLAttempts = KSyncmlAttemptCount;
	        	}
	        else 
	        	{
	        	iSyncMLAttempts = 0;
	        	dmJob.CreateL( iSyncMLSession, aState.iProfileId);
	        	}

			// If there's no iapid defined, sml will pop up connection dialog.
			// In that case, only one connection attempt is allowed.
	        iSyncJobId   = dmJob.Identifier();
	        iSyncProfile = aState.iProfileId;
	        dmJob.Close();
	        iSyncMLSession.RequestEventL( *this ); 
	        }
	}

    FLOG(_L("[cfotasever]   CreateDeviceManagementSessionL dms << profid %d")
                            ,aState.iProfileId);
    }


// ============================= MEMBER FUNCTIONS ============================

// --------------------------------------------------------------------------
// CFotaServer::CFotaServer()   
// --------------------------------------------------------------------------
//
CFotaServer::CFotaServer() : iInitialized(EFalse), iNeedToClose(0), iParentApp(0)
, iDatabase(0),iDownloader(0), iUpdater(0)
 , iDownloadFinalizer(0), iUpdateFinalizer(0), iTimedExecuteResultFile(0)
 , iTimedSMLSessionClose(0), iStorage(0), iAppShutter(0), iMonitor (NULL), iSyncJobId(-1), iRetryingGASend (EFalse), iNetworkAvailable (EFalse)
 , iIapId(KErrNotFound),iSessMode(0),iUserResume(KErrNotFound), iInstallupdClient(EFalse), iDEController(NULL)
	{
    RProcess pr; TFullName fn = pr.FullName();  TUint prid = pr.Id();
	FLOG(_L( "CFotaServer::CFotaServer process(id %d)%S. this 0x%x"),prid,&fn
        ,this);

	}


// --------------------------------------------------------------------------
// CFotaServer::DoExecuteResultFileL
// Interprets result of update (file update.resp)
// --------------------------------------------------------------------------
//
void CFotaServer::DoExecuteResultFileL()
    {
    FLOG(_L("CFotaServer::DoExecuteResultFileL() >>"));

    if ( iTimedExecuteResultFile ) 
        {
        iTimedExecuteResultFile->Cancel();
        delete iTimedExecuteResultFile;
        iTimedExecuteResultFile = NULL;
        }
    iUpdater = CFotaUpdate::NewL(this);
    iUpdater->ExecuteUpdateResultFileL( iFs );
    FLOG(_L("CFotaServer::DoExecuteResultFileL() <<"));
    }

// --------------------------------------------------------------------------
// StaticDoExecuteResultFile
// --------------------------------------------------------------------------
//
static TInt StaticDoExecuteResultFile(TAny *aPtr)
    {
    FLOG(_L("[cfotaserver] StaticDoExecuteResultFile() >>"));
    __ASSERT_ALWAYS( aPtr, User::Panic(KFotaPanic, KErrBadHandle) );
    CFotaServer* srv = (CFotaServer*) aPtr;
    TRAPD( err,  srv->DoExecuteResultFileL() );
    if(err) 
        {
        FLOG(_L("[cfotaserver] StaticDoExecuteResultFile ERROR %d"),err);
        TRAP_IGNORE(srv->DoDeleteUpdateResultFileL());
        }

    FLOG(_L("[cfotaserver] StaticDoExecuteResultFile() <<"));
    return err;
    }


// --------------------------------------------------------------------------
// CFotaServer::CFotaServer()   
// Constructor. Can't do all constructing since fotaserver might call itself
// recursively (via downloadmgr). 
// --------------------------------------------------------------------------
//
void CFotaServer::ConstructL(const TDesC &aFixedServerName)
    {
    FLOG(_L("CFotaServer::ConstructL() >> name=%S"), &aFixedServerName);
    TBool   updated (EFalse);
    TInt    err;
    CAknAppServer::ConstructL( aFixedServerName );

    User::LeaveIfError( iFs.Connect() );
    
    err = iFs.CreatePrivatePath(EDriveC);
    if ( err != KErrNone && err != KErrAlreadyExists ) { User::Leave (err); }
    User::LeaveIfError( iFs.SetSessionToPrivate( EDriveC ) );

    if ( !iDatabase )
        {
        TRAPD( err,iDatabase = CFotaDB::NewL() );
        if ( err ) 
            {
            FLOG(_L("CFotaServer::  DB creationg error %d"), err);
            User::Leave( err );
            }
        }

    updated = CFotaUpdate::CheckUpdateResults( iFs );

    // Update has happened, and result file is in place
    if ( updated )
        {
        FLOG(_L("scheduling update result file execution"));
        if ( iTimedExecuteResultFile ) 
            {
            iTimedExecuteResultFile->Cancel();
            delete iTimedExecuteResultFile;
            iTimedExecuteResultFile = NULL;
            }
        iTimedExecuteResultFile = CPeriodic::NewL (EPriorityNormal) ;
        iTimedExecuteResultFile->Start (
            TTimeIntervalMicroSeconds32(KDownloadFinalizerWaitTime)
            , TTimeIntervalMicroSeconds32(KDownloadFinalizerWaitTime)
            , TCallBack(StaticDoExecuteResultFile,this) ) ;
        }

	iEikEnv = CEikonEnv::Static();
    FLOG(_L("CFotaServer::ConstructL() <<"));
    }


// --------------------------------------------------------------------------
// CFotaServer::ClientAwareConstructL
// Does rest of constructing, if not done. If parent of this fotaserver 
// instance is another fotaserver, skip maintenance operations (since parent 
// takes care of them). DmHOstserver triggers cleanup for states: 20,70,80
// ,90,100 , must handle state 60 here
// --------------------------------------------------------------------------
void CFotaServer::ClientAwareConstructL( const RMessage2 &aMessage )
    {
    FLOG(_L("CFotaServer::ClientAwareConstructL >>"));
    if ( iInitialized ) return;
    CFotaSrvSession::TClient client 
                    = CFotaSrvSession::CheckClientSecureIdL ( aMessage );
	// temporary fix to keep FS from polluting FSW in case it was opened from omadmappui
    if ( client == CFotaSrvSession::EOMADMAppUi || client == CFotaSrvSession::EFotaServer ||
       client == CFotaSrvSession::ESoftwareChecker || client == CFotaSrvSession::ESoftwareCheckerBackground  ) 
        {
        //Loading the storage plugin (during DM UI launch) to adjust the reserved memory accordingly.
        StoragePluginL();
        iParentApp->SetUIVisibleL ( EFalse, ETrue ); // 
        }

    // Do constructing if client is other that fotaserver
    if ( client != CFotaSrvSession::EFotaServer )
        {
        RArray<TInt>    states;
        TPackageState   state;
        CleanupClosePushL (states);
        iDatabase->OpenDBL();
        iDatabase->GetAllL ( states );
    	TBool value (EFalse);
    	if (iDownloader)
    		{
    		value = iDownloader->IsDownloadActive();
    		}
    	else
    		{
    		TInt err = RProperty::Get(TUid::Uid(KOmaDMAppUid), KFotaDownloadActive, value );
    		FLOG(_L("value of KFotaDownloadActive & err as %d,%d "), (TInt)value,err);
    		}
    	FLOG(_L("Download active value is:%d "), (TInt)value);
        // Loop states. 
        for(TInt i = 0; i < states.Count(); ++i ) 
            {
            TPackageState   tmp;
            TBool           toIdle(EFalse);
            tmp = iDatabase->GetStateL(  states[i]  );
            FLOG(_L("   1 got state id:%d state:%d result:%d"), tmp.iPkgId
                , tmp.iState, tmp.iResult);
            
             if ( tmp.iState == RFotaEngineSession::EStartingUpdate )
             
             {
             	TBool ispkgvalid= ETrue;
             	TRAPD(err1,ispkgvalid= CheckSWVersionL()   )
              if (!ispkgvalid && err1 == KErrNone)
              {
              StoragePluginL()->DeleteUpdatePackageL (  tmp.iPkgId );	
             	tmp.iState    = RFotaEngineSession::EUpdateFailed;
              tmp.iResult   = RFotaEngineSession::EResPackageMismatch;
              iDatabase->SetStateL( tmp,KNullDesC8, EFDBState | EFDBResult );
   
             }	
             }
            // Update has been started (60)
            // If there is no result file, means that update agent failed
            // to run. Must get back to 50 to allow user to try again. 
            if ( tmp.iState == RFotaEngineSession::EUpdateProgressing 
                && iTimedExecuteResultFile==NULL )
                {
                FLOG(_L("     State 60 found, UA didnt run! id %d "), tmp.iPkgId);
                tmp.iState    = RFotaEngineSession::EStartingUpdate;
                iDatabase->SetStateL( tmp,KNullDesC8, EFDBState);
                }

            // These states need must trigger generic alert! (70+ )
            if ( tmp.iState >= RFotaEngineSession::EUpdateFailed  )
                {
                if ( iSyncMLSession.Handle() == NULL )
                    {
                    iDatabase->CloseAndCommitDB();
                    CreateDeviceManagementSessionL( tmp );
                    iDatabase->OpenDBL();
                    }
                }

            if (tmp.iState == RFotaEngineSession::EDownloadComplete && tmp.iResult == RFotaEngineSession::EResSuccessful )
            	{
                CreateDeviceManagementSessionL( tmp );
            	}

            if (value != 1) //if download is not active, EStartingDownload should be reset to EDownloadFailed
            	{
            	if (tmp.iState == RFotaEngineSession::EStartingDownload ||
            			tmp.iState == RFotaEngineSession::EDownloadProgressing )
            		{
            		FLOG(_L("Resetting state %d to 20..."), tmp.iState);
            		tmp.iState = RFotaEngineSession::EDownloadFailed;
                    iDatabase->SetStateL( tmp,KNullDesC8, EFDBState ) ;
                    iDatabase->CloseAndCommitDB();
                    iDatabase->OpenDBL();
            		}
            	}

            if ( tmp.iState == RFotaEngineSession::EDownloadFailed )
                {
                StoragePluginL()->DeleteUpdatePackageL( tmp.iPkgId ); 
                tmp.iResult   = RFotaEngineSession::EResDLFailDueToNWIssues;
                iDatabase->SetStateL( tmp,KNullDesC8, EFDBResult ) ;
                iDatabase->CloseAndCommitDB();
                CreateDeviceManagementSessionL( tmp );
                iDatabase->OpenDBL();
                }

            // Reset package state to idle
            if ( toIdle )
                {
                FLOG(_L("   2 resetting state %d to idle"), tmp.iPkgId);
                tmp.iState    = RFotaEngineSession::EIdle;
                iDatabase->SetStateL( tmp,KNullDesC8, EFDBState ) ;
                DeleteFUMOTreeL();
                }
            }
            iDatabase->CloseAndCommitDB();
            CleanupStack::PopAndDestroy( &states );
        }
    else // client is child fotaserver
        {
        }
    iInitialized = ETrue;
    FLOG(_L("CFotaServer::ClientAwareConstructL <<"));
    }


// ---------------------------------------------------------------------------
// CFotaServer::StoragePluginL
// Getter function for iStorage. If it doesn't exist, load it.
// ---------------------------------------------------------------------------
//
CFotaStorage* CFotaServer::StoragePluginL()
    {
    if ( iStorage == NULL )
        {
        LoadStoragePluginL();
        }
    return iStorage;
    }


// --------------------------------------------------------------------------
// CFotaServer::LoadStoragePluginL 
// Load update storage plugin via ecom framework.
// --------------------------------------------------------------------------
//
void CFotaServer::LoadStoragePluginL ()
    {
    FLOG(_L("CFotaServer::LoadStoragePluginL"));
    if(iStorage)
        {
        UnLoadStoragePluginL ();
        }
    TUid                if_uid;
    if_uid.iUid = ( KStorageIFUid );
    RImplInfoPtrArray   pluginArray;
    REComSession::ListImplementationsL( if_uid, pluginArray );
    CleanupClosePushL( pluginArray );

    if( pluginArray.Count() )
        {
        for( TInt i = 0; i < pluginArray.Count(); i++ )
            {
            CImplementationInformation* info = pluginArray[ i ];
            TUid id = info->ImplementationUid();
            delete info;
            info = NULL;
            iStorage =(CFotaStorage*) REComSession::CreateImplementationL( id
                                        ,iStorageDtorKey); 
            }
        }
    else
        {
        FLOG(_L("  storage plugin not found, ERROR"));
        User::Leave ( KErrNotFound );
        }
    CleanupStack::PopAndDestroy( &pluginArray );
    }

// --------------------------------------------------------------------------
// CFotaServer::UnLoadStoragePluginL
// Unloads storage plugin
// --------------------------------------------------------------------------
//
void CFotaServer::UnLoadStoragePluginL ()
    {
    FLOG(_L("CFotaServer::UnLoadStoragePluginL"));
    if ( iStorage ) 
        {
        delete iStorage;
        iStorage=NULL;
        }
    FLOG(_L("    destroying"));
    REComSession::DestroyedImplementation(iStorageDtorKey); // zeroes referenc
                                                            // count of plugin
    FLOG(_L("    finalclosing"));
    REComSession::FinalClose();
    }


// --------------------------------------------------------------------------
// StaticDoFinalizeUpdate
// Intermediate function
// --------------------------------------------------------------------------
static TInt StaticDoFinalizeUpdate(TAny *aPtr)
    {
    FLOG(_L("[cfotaserver] StaticDoFinalizeUpdate() >>"));
    CFotaServer* srv = (CFotaServer*) aPtr;
    TRAPD( err,  srv->DoFinalizeUpdateL() );
    if(err)
        {
        FLOG(_L("[cfotaserver] StaticDoFinalizeUpdate ERROR %d"),err);
        }

    FLOG(_L("[cfotaserver] StaticDoFinalizeUpdate() <<"));
    return err;
    }


// --------------------------------------------------------------------------
// CFotaServer::FinalizeUpdate()
// Initialize finalization of updater
// --------------------------------------------------------------------------
//
void CFotaServer::FinalizeUpdateL()
    {
    FLOG(_L("CFotaServer::FinalizeUpdate() >>"));
    if ( iUpdateFinalizer )
        {
        iUpdateFinalizer->Cancel();
        delete iUpdateFinalizer;
        iUpdateFinalizer=NULL;
        }
    iUpdateFinalizer = CPeriodic::NewL (EPriorityNormal) ;
    iUpdateFinalizer->Start (
                TTimeIntervalMicroSeconds32(KDownloadFinalizerWaitTime)
                ,TTimeIntervalMicroSeconds32(KDownloadFinalizerWaitTime)
                ,TCallBack(StaticDoFinalizeUpdate,this) ) ;
    FLOG(_L("CFotaServer::FinalizeUpdate() <<"));
    }


// --------------------------------------------------------------------------
// CFotaServer::DoFinalizeUpdateL
// Finalize update. Free resources
// --------------------------------------------------------------------------
//
void CFotaServer::DoFinalizeUpdateL()
    {
    FLOG(_L("CFotaServer::DoFinalizeUpdateL() >>"));

    // Free resources
    FLOG(_L("iupdater = %d"), iUpdater);
    if ( iUpdater ) 
        {
        FLOG(_L("Deleting iupdater..."));
        iUpdater->Cancel();
        delete iUpdater; 
        iUpdater=NULL;
        }
    
    if ( iUpdateFinalizer ) 
        {
        iUpdateFinalizer->Cancel();
        }
    // Hide UI
    iParentApp->SetUIVisibleL ( EFalse, ETrue );
    FLOG(_L("CFotaServer::DoFinalizeUpdateL() <<"));
    }


// --------------------------------------------------------------------------
// StaticDoFinalizeDownload
// Intermediate function
// --------------------------------------------------------------------------
static TInt StaticDoFinalizeDownload(TAny *aPtr)
    {
    FLOG(_L("[cfotaserver] StaticDoFinalizeDownload() >>"));
    CFotaServer* srv = (CFotaServer*) aPtr;
    TRAPD( err,  srv->DoFinalizeDownloadL() );
    if(err)
        {
        FLOG(_L("[cfotaserver] StaticDoFinalizeDownload ERROR %d"),err);
        }

    FLOG(_L("[cfotaserver] StaticDoFinalizeDownload() <<"));
    return err;
    }


// --------------------------------------------------------------------------
// CFotaServer::FinalizeDownload
// Initialize finalization of download
// --------------------------------------------------------------------------
//
void CFotaServer::FinalizeDownloadL( const TPackageState& aDLState )
    {
    FLOG(_L("CFotaServer::FinalizeDownload() >> state:%d result:%d")
                ,aDLState.iState, aDLState.iResult);
    __ASSERT_ALWAYS( iDownloader, User::Panic(KFotaPanic, KErrBadHandle ));
    
    SetStartupReason(EFotaDefault);
          
    if ( iDownloadFinalizer )
        {
        iDownloadFinalizer->Cancel();
        delete iDownloadFinalizer;
        iDownloadFinalizer=NULL;
        }
    iDownloadFinalizer = CPeriodic::NewL (EPriorityMuchMore) ;
    iDLFinalizeState = aDLState;
    
    // Not restarting,quick finalize
    if ( iDownloader->iRestartCounter<=0 )
        {
        iDownloadFinalizer->Start(
                        TTimeIntervalMicroSeconds32(KDownloadFinalizerWaitTime)
                        ,TTimeIntervalMicroSeconds32(KDownloadFinalizerWaitTime)
                        ,TCallBack(StaticDoFinalizeDownload,this) ) ;
        }
    // Restarting, wait some time
    else
        {
        iDownloadFinalizer->Start(
                        TTimeIntervalMicroSeconds32(2000000)
                        ,TTimeIntervalMicroSeconds32(2000000)
                        ,TCallBack(StaticDoFinalizeDownload,this) ) ;
        }
    FLOG(_L("CFotaServer::FinalizeDownload() <<"));
    }


// --------------------------------------------------------------------------
// CFotaServer::DoFinalizeDownloadL
// Finalize download. Free resources
// --------------------------------------------------------------------------
//
void CFotaServer::DoFinalizeDownloadL()
    {
    FLOG(_L("CFotaServer::DoFinalizeDownloadL() >> state:%d result:%d")
            ,iDLFinalizeState.iState, iDLFinalizeState.iResult );
    __ASSERT_ALWAYS( iDownloader, User::Panic(KFotaPanic, KErrBadHandle ));
    
    if ( iDownloadFinalizer ) 
        {
        iDownloadFinalizer->Cancel();
        }

    if (iDownloader->iDLState.iState == RFotaEngineSession::EDownloadFailed)
        StoragePluginL()->DeleteUpdatePackageL ( iDownloader->iDLState.iPkgId );
        
    // Download not to be restarted, delete
    if ( iDownloader->iRestartCounter<=0 )
        {
        // Set downloader's ending state to DB
        iDatabase->OpenDBL();
        iDatabase->SetStateL(iDLFinalizeState
                            ,KNullDesC8,EFDBState|EFDBResult);
        iDatabase->CloseAndCommitDB();

        iDownloader->SetDownloadActive(EFalse);
        iParentApp->SetUIVisibleL( EFalse,ETrue);

        // Initiate update 
        if ( iDownloader && iDownloader->iUpdateAfterDownload )
            {
            UpdateL( iDownloader->iDLState );  
            }
        // Report status to DM server
        else
            {
            if (iDownloader->iDLState.iState == RFotaEngineSession::EDownloadProgressingWithResume )
            	{
            	if (iDownloader->IsFMSEnabled())
                	{
                	FLOG(_L("Invoking FMS..."));
                	InvokeFmsL();
                	SetStartupReason(EFotaDownloadInterrupted);
                	}
            	}
            else
            	{
            	CreateDeviceManagementSessionL( iDownloader->iDLState );
            	}
            }

        // Free resources
        if ( iDownloader ) 
            {
            iDownloader->Cancel();
			iDownloader->CancelDownload (ETrue);
            delete iDownloader; 
            iDownloader=NULL;
            }
        TInt val (EFalse);
        TInt err = RProperty::Get(TUid::Uid(KOmaDMAppUid), KFotaDMRefresh, val );
        if (val!=EFalse)
        	err = RProperty::Set(TUid::Uid(KOmaDMAppUid), KFotaDMRefresh, EFalse );
        FLOG(_L("RProperty KFotaDMRefresh EFalse, err = %d"), err);
        }
    // Download to be restarted
    else
        {
        __ASSERT_ALWAYS( iDownloader->iUrl
                ,User::Panic(KFotaPanic, KErrBadHandle ));
        FLOG(_L(" Restarting download(iRestartCounter %d)"),iDownloader->iRestartCounter);

        // Reuse url and iapid
        HBufC8* url = iDownloader->iUrl->AllocLC();
        TInt    iapid = iDownloader->iDLState.iIapId;

        TBool   update= iDownloader->iUpdateAfterDownload;
        TInt    restart=iDownloader->iRestartCounter;
        
        iDownloader->CancelDownload( EFalse ); //Doesn't actually deletes downloads when S&R feature is supported.
      
        iDownloader->DownloadL( iDLFinalizeState , *url,update,iapid, restart);
        CleanupStack::PopAndDestroy( url );
        }


    FLOG(_L("CFotaServer::DoFinalizeDownloadL() <<"));
    }


// --------------------------------------------------------------------------
// StaticDoCloseSMLSession 
// Intermediate function
// --------------------------------------------------------------------------
static TInt StaticDoCloseSMLSession ( TAny *aPtr )
    {
    FLOG(_L("[cfotaserver] StaticDoCloseSMLSession() >>"));
    CFotaServer* srv = (CFotaServer*) aPtr;
    TRAPD( err,  srv->DoCloseSMLSessionL() );
    if(err)
        {
        FLOG(_L("[cfotaserver] StaticDoCloseSMLSession ERROR %d"),err);
        }

    FLOG(_L("[cfotaserver] StaticDoCloseSMLSession() <<"));
    return err;
    }

// --------------------------------------------------------------------------
// CFotaServer::DoCloseSMLSessionL
// Close syncml session, or resync
// --------------------------------------------------------------------------
//
void CFotaServer::DoCloseSMLSessionL()
    {
    FLOG(_L("CFotaServer::DoCloseSMLSessionL() >>"));

    // Must still try to sync
    if ( iSyncMLAttempts > 0 )
        {
        FLOG(_L("   trycount %d => creating new job"),iSyncMLAttempts);
	    RSyncMLDevManJob    dmJob;
    	if(iIapId > KErrNotFound)
    		{
    		FLOG(_L("DoCloseSMLSessionL new job uses iap from fotadb %d"),
    				iIapId);
    		dmJob.CreateL( iSyncMLSession, iSyncProfile,iIapId );
    		}
    	else
    		{
    		FLOG(_L("DoCloseSMLSessionL new job uses iap from profile"));
    		dmJob.CreateL( iSyncMLSession, iSyncProfile );
    		}
			iSyncMLAttempts--;
			iSyncJobId = dmJob.Identifier();
	    dmJob.Close();
        }
    else 
    // We ran out of attempts, close sml
        {
        if ( iSyncMLSession.Handle() ) 
            {
            FLOG(_L("   Closing syncml session"));
            iSyncMLSession.CancelEvent();
            iSyncMLSession.Close();
            }
        } 

    if ( iTimedSMLSessionClose ) 
        {
        FLOG(_L("   closing smlsession timer"));
        iTimedSMLSessionClose->Cancel();
        delete iTimedSMLSessionClose;
        iTimedSMLSessionClose = NULL;
        }

    FLOG(_L("CFotaServer::DoCloseSMLSessionL() <<"));
    }


// --------------------------------------------------------------------------
// CFotaServer::ReadChunkL
// Read data from chunk created by client.
// --------------------------------------------------------------------------
//
void CFotaServer::ReadChunkL( const TInt aSize,TInt aError )
	{
    __ASSERT_ALWAYS( aSize>=0 , User::Panic(KFotaPanic, KErrArgument) );
    TUint8* b = iChunk.Base();
    TPtr8   data(b, aSize, aSize );

    iStorage->iBytesWritten += aSize;
    FLOG(_L("     CFotaServer::ReadChunkL  >>  reading %d bytes"),aSize);
    if (iStorageStream && aError !=KErrServerTerminated &&
    		aError !=KErrDiskFull )
       {FLOG(_L("     CFotaServer::ReadChunkL  >>  inside if(iStorageStream)"));
       TRAPD ( err, iStorageStream->WriteL(data) );

       if ( !err ) 
           {
           iStorageStream->CommitL();
           }
       else
    	  {
          FLOG(_L("Error when writing pkg %d"), err);
          iStorageStream->Close();
          iStorageStream = NULL;
          User::Leave ( err );
          }
       }  
    FLOG(_L("     CFotaServer::ReadChunkL  << "));
    }


// --------------------------------------------------------------------------
// CFotaServer::OpenUpdatePackageStoreL 
// Opens package store. Calling chain in case of oma DL:  
// fotaserver->downloadmgr->codhandler->fotaserver  (subject to change, 
// depending on downloadmgr&codhandler changes)
// Fotaadapter calls this function in direct way: fotaadapter->fotaserver
//
// --------------------------------------------------------------------------
//
void CFotaServer::OpenUpdatePackageStoreL ( const RMessage2& aMessage )
    {
    FLOG(_L("CFotaServer::OpenUpdatePackageStoreL  >>"));

    // If chunk is already open, bail out
    THandleInfo     info;
    TInt            pkgid;
    iChunk.HandleInfo(&info);
    
     
    if(info.iNumOpenInProcess>0)
        {
        FLOG(_L("CFotaServer::SetChunkHandleL  chunk is open %d times by \
            current process (%d times in thread)"),info.iNumOpenInProcess
            ,info.iNumOpenInThread);
        User::Leave(KErrInUse);
        }

    // Set package state 
    pkgid = aMessage.Int0();
    iStorageDownloadPackageId = pkgid;

    TPackageState   state = GetStateL(pkgid);

    // AS  16.02.05: must be in failed state until complete

    FLOG(_L("CFotaServer::OpenUpdatePackageStoreL  opening update pkg storage\
                ffor pkgid: %d "),pkgid);

    TInt size2 = state.iPkgSize;
    // Use storage api to save update pkg 
    StoragePluginL()->OpenUpdatePackageStoreL(pkgid,size2, iStorageStream);

    User::LeaveIfError( iChunk.Open(aMessage,1,EFalse) );
	TInt err = RProperty::Define( TUid::Uid(KFotaServerUid),
					KFotaLrgObjDl,RProperty::EInt,
					KReadPolicy,  KWritePolicy );
	TInt    err1 = RProperty::Set( TUid::Uid(KFotaServerUid),
			KFotaLrgObjDl, KErrNotFound );
	FLOG(_L("CFotaServer::OpenUpdatePackageStoreL err for KFotaLrgObjDl define is %d err is %d"),
			err,err1 );  
	err = RProperty::Define( TUid::Uid(KFotaServerUid),
				KFotaLrgObjProfileId,RProperty::EInt,
				KReadPolicy,KWritePolicy );
	err1 = RProperty::Set( TUid::Uid(KFotaServerUid),
						KFotaLrgObjProfileId, KErrNotFound );
	FLOG(_L("CFotaServer::OpenUpdatePackageStoreL err for KFotaLrgObjProfileId define is %d err is %d"),
			err,err1 );  
    FLOG(_L("CFotaServer::OpenUpdatePackageStoreL  <<"));
    }

// --------------------------------------------------------------------------
// CFotaServer::GetDownloadUpdatePackageSizeL 
// Gets the downloaded update package size in bytes
// --------------------------------------------------------------------------
//
void CFotaServer::GetDownloadUpdatePackageSizeL (const TInt aPkgId, TInt& aDownloadedSize, TInt& aTotalSize)
	{
	FLOG(_L("CFotaServer::GetDownloadUpdatePackageSize, aPkgId = %d  >>"),aPkgId);
	TPackageState temp;

	//Getting the total package size from database.
	iDatabase->OpenDBL();
	temp = iDatabase->GetStateL(aPkgId);
	iDatabase->CloseAndCommitDB();
	
	FLOG(_L("fota state for given pkg id is = %d"),temp.iState);
	if (temp.iState == RFotaEngineSession::EDownloadProgressing
			|| temp.iState == RFotaEngineSession::EDownloadProgressingWithResume
			|| temp.iState == RFotaEngineSession::EDownloadComplete
			|| temp.iState == RFotaEngineSession::EStartingUpdate)
		{
		aTotalSize = temp.iPkgSize;
		//Getting the downloaded update package size from storage pluggin.
		StoragePluginL()->GetDownloadedUpdatePackageSizeL(aPkgId, aDownloadedSize);
		}
	else
		{
		FLOG(_L("No progressing/suspended/completed download corresponding to the given pkgid (%d)"),aPkgId);
		User::Leave (KErrNotFound);
		}

	
	FLOG(_L("CFotaServer::GetDownloadUpdatePackageSize, aDownloadedSize = %d, aTotalSize = %d  <<"),aDownloadedSize, aTotalSize);
	}

// --------------------------------------------------------------------------
// CFotaServer::TryResumeDownloadL 
// Tries to resume the download operation
// --------------------------------------------------------------------------
//
void CFotaServer::TryResumeDownloadL(TBool aUserInitiated)
	{
	FLOG(_L("CFotaServer::TryResumeDownloadL, aUserInitiated = %d  >>"),aUserInitiated);
	//Check whether there is a paused resume actually.
	if(aUserInitiated)
	    iUserResume = ETrue;
	else
	    iUserResume = EFalse;
	TPackageState temp = GetStateL(-1); //Gets the state of the current/last fota download
	iSessMode = temp.iSessionType;
	FLOG(_L("iSessMode = %d ,temp.iSessionType=%d  "),iSessMode,
	            temp.iSessionType);	
	if ( temp.iState != RFotaEngineSession::EDownloadProgressingWithResume )
		{
		FLOG(_L("There are no paused downloads currently; hence leaving with KErrNotFound..."));
		User::Leave (KErrNotFound);
		}

	//Resume download now
    if(!iDownloader)
        {
        FLOG(_L("Creating new idownloader"));
        iDownloader = CFotaDownload::NewL( this );
        }
    if (iDownloader->IsDownloadActive())
    	{
    	FLOG(_L("Another download is already active, hence returning..."));
    	User::Leave (KErrAlreadyExists);
    	}
    iDownloader->iDLState = temp;
	SetStartupReason(EFotaDownloadInterrupted);
    iDownloader->TryResumeDownloadL(aUserInitiated);
	
	FLOG(_L("CFotaServer::TryResumeDownloadL  <<"));
	}

// --------------------------------------------------------------------------
// CFotaServer::InvokeFmsL 
// Starts Fota Monitory Service with relevant parameters for monitoring.
// --------------------------------------------------------------------------
//
void CFotaServer::InvokeFmsL()
	{
	FLOG(_L("CFotaServer::InvokeFmsL  >>"));
	//Collect all information needed to invoke FMS.
	TPackageState temp = GetStateL(iDownloader->iDLState.iPkgId);
	FLOG(_L("State as recorded in fota db:"));
	FLOG(_L("iPkgId = %d"),temp.iPkgId);
	FLOG(_L("iProfileId = %d"),temp.iProfileId);
	FLOG(_L("iPkgName = %S"),&temp.iPkgName);
	FLOG(_L("iPkgVersion = %S"),&temp.iPkgVersion);
	FLOG(_L("iSendAlert = %d"),temp.iSendAlert);
	FLOG(_L("iIapId = %d"),temp.iIapId);
	FLOG(_L("iPkgSize = %d"),temp.iPkgSize);
	FLOG(_L("iSessionType = %d"),temp.iSessionType);
	FLOG(_L("iState = %d"),temp.iState);
	FLOG(_L("iResult = %d"),temp.iResult);
	
	
	//Finding the reason for download interrupt
	TOmaDLInterruptReason reason (EGeneralInterrupt); // 3
	
	switch (iDownloader->iDLState.iResult)
	{
	case RFotaEngineSession::EResUserCancelled:
		{
		reason = EUserInterrupt; //0
		break;
		}
	case RFotaEngineSession::EResDLFailDueToNWIssues:
		{
		reason = ENetworkInterrupt; //1
		break;
		}
	case RFotaEngineSession::EResDLFailDueToDeviceOOM:
		{
		reason = EMemoryInterrupt;   //2
		break;
		}
	default:
		{
		//reason is already EGeneralInterrupt
		break;
		}
	}
	//FotaState has the last successfully worked IAP. Hence use this for FMS monitoring.
//	TInt apid = iDownloader->iDLState.iIapId;
	TInt apid = temp.iIapId;
	
	//Finding the drive number
	TBuf8<KMaxPath> path8;
	path8.Zero();
	StoragePluginL()->GetUpdatePackageLocationL(iDownloader->iDLState.iPkgId, path8);
	TPath path16;
	path16.Copy(path8);


    TInt drive (EDriveC ); //Default drive is Phone Memory
	TParse p;
    if (!p.Set(path16,NULL,NULL))
    	{
    	TDriveName drivename(p.Drive());
    	TDriveUnit driveunit(drivename);
    	if (iFs.IsValidDrive((TInt) driveunit))
    		drive = driveunit;
    	}
    else
    	{
    	FLOG(_L("Error while parsing for drive number! defaulting to Phone Memory (C)"));
    	}

    TInt dlsize, tsize;
    GetDownloadUpdatePackageSizeL(iDownloader->iDLState.iPkgId, dlsize, tsize);
	TInt neededsize = tsize - dlsize;
	FLOG(_L("Launching FMS with params... reason = %d, iapid = %d, drive = %d, neededsize = %d"),reason, apid, drive, neededsize);
	iFMSClient.OpenL();
	iFMSClient.NotifyForResumeL( reason, apid, (TDriveNumber)drive, neededsize );
	iFMSClient.Close();
	
	FLOG(_L("CFotaServer::InvokeFmsL  <<"));
	}

// --------------------------------------------------------------------------
// CFotaServer::CancelFmsL 
// Cancels any outstanding monitoring requests in Fota Monitory Service
// --------------------------------------------------------------------------
//
void CFotaServer::CancelFmsL()
	{
	FLOG(_L("CFotaServer::CancelFmsL  >>"));
	
	iFMSClient.OpenL();
	iFMSClient.CancelNotifyForResume();
	iFMSClient.Close();
	
	FLOG(_L("CFotaServer::CancelFmsL  <<"));
	}

// --------------------------------------------------------------------------
// CFotaServer::SetStartupReason 
// Sets the startup reason for Fota. This is used in Fota Startup pluggin.
// --------------------------------------------------------------------------
//
void CFotaServer::SetStartupReason(TInt aReason)
	{
	FLOG(_L("CFotaServer::SetStartupReason, aReason = %d  >>"), aReason);
	
	CRepository* centrep = NULL;
    TRAPD( err, centrep = CRepository::NewL( KCRUidFotaServer ) )
    if (err==KErrNone ) 
    	{
         centrep->Set(  KFotaUpdateState, aReason );
        }
    delete centrep;
    centrep = NULL;

	FLOG(_L("CFotaServer::SetStartupReason  <<"));
	}

// --------------------------------------------------------------------------
// CFotaServer::~CFotaServer()
// Frees database, download, chunk, filewriter, etc resources
// --------------------------------------------------------------------------
//
CFotaServer::~CFotaServer()
	{
    FLOG(_L("CFotaServer::~CFotaServer  >>"));

    if(iDatabase)
        {
        iDatabase->CloseAndCommitDB();
        delete iDatabase;
        iDatabase = NULL;
        }

    if(iDownloader)
        {
        iDownloader->Cancel();
        delete iDownloader;
        iDownloader = NULL;
        }

    if(iUpdater)
        {
        iUpdater->Cancel();
        delete iUpdater;
        iUpdater = NULL;
        }

    if(iDownloadFinalizer) 
        {
        iDownloadFinalizer->Cancel();
        delete iDownloadFinalizer;
        iDownloadFinalizer = NULL;
        }

    if(iUpdateFinalizer)
        {
        iUpdateFinalizer->Cancel();
        delete iUpdateFinalizer;
        iUpdateFinalizer = NULL;
        }

    if ( iTimedExecuteResultFile ) 
        {
        iTimedExecuteResultFile->Cancel();
        delete iTimedExecuteResultFile;
        iTimedExecuteResultFile = NULL;
        }

    if ( iAppShutter )
        {
        iAppShutter->Cancel();
        delete iAppShutter;
        iAppShutter=NULL;
        }
        
    if ( iMonitor )    
    	{
    	delete iMonitor;
    	iMonitor = NULL;
    	}
    //don't delete iEikEnv, fw will take care.
    if (iChunk.Handle())
    	iChunk.Close();
    
    if (iStorageStream) {
    	TRAP_IGNORE(iStorageStream->CommitL());
    	iStorageStream->Close();
    	iStorageStream = NULL;
    }
    iSyncMLSession.Close();
    TRAP_IGNORE( UnLoadStoragePluginL () );
    iFs.Close();
    
    
    if (iFMSClient.Handle())
    	iFMSClient.Close();

	if ( iNotifHandler ) 
		{
		iNotifHandler->Cancel();
		delete iNotifHandler;        
		}
	TInt err = RProperty::Delete(TUid::Uid(KFotaServerUid),
    		KFotaLrgObjDl);
    FLOG( _L( "CFotaServer::~CFotaServer err for KFotaLrgObjDl is %d " ),err );  
    err = RProperty::Delete(TUid::Uid(KFotaServerUid),
    		KFotaLrgObjDl);
    FLOG( _L( "CFotaServer::~CFotaServer err for KFotaLrgObjProfileId is %d " ),err );  
    err = RProperty::Delete(TUid::Uid(KFotaServerUid),
    		KFotaLrgObjProfileId);    
    
    if (iDEController)
        {
        delete iDEController;
        iDEController = NULL;
        }
 
	FLOG(_L("CFotaServer::~CFotaServer  <<"));  
	}


// ---------------------------------------------------------------------------
// CFotaServer::UpdatePackageDownloadCompleteL
// Sets state and package streaming related objects. If this is child
// fotaserver (not largeobject), don't set state since parent fotaserver does
// it.
// ---------------------------------------------------------------------------
void CFotaServer::UpdatePackageDownloadCompleteL(TBool aLargObj
                                                        , const TInt aPkgId)
    {
    FLOG(_L("CFotaServer::UpdatePackageDownloadCompleteL %d >>"),aPkgId);
    TInt        pkgid(aPkgId);

    // Set state in case of lrg obj download 
    // If not lrg ob download, calling app sets state
    if (aLargObj)
        {
        TPackageState s;
        s.iPkgId = aPkgId;
        s.iState = RFotaEngineSession::EStartingUpdate;
        iDatabase->OpenDBL();
        iDatabase->SetStateL( s,KNullDesC8,EFDBState );
        iDatabase->CloseAndCommitDB();
        //To refresh in DM UI for OMA DM large object
        TInt err = RProperty::Define( TUid::Uid(KOmaDMAppUid),
				KFotaDMRefresh,
				RProperty::EInt,KReadPolicy,KWritePolicy);
		FLOG(_L("CFotaServer::UpdatePackageDownloadCompleteL KFotaDMRefresh Define, err = %d")
														, err);        
		if (err != KErrAlreadyExists && err != KErrNone)
			{
			User::LeaveIfError(err);
			}
		//notify DM UI, EFalse indicates no Download going on 
		err = RProperty::Set( TUid::Uid(KOmaDMAppUid),
				KFotaDMRefresh,
				EFalse );
		FLOG(_L("RProperty KFotaDMRefresh Set ETrue, err = %d"), err);           
        }
    // Free resources
    iChunk.Close();
    iStorage->UpdatePackageDownloadCompleteL(pkgid);
    FLOG(_L("CFotaServer::UpdatePackageDownloadCompleteL %d <<"),aPkgId);
    }


// ---------------------------------------------------------------------------
// CFotaServer::DeletePackageL 
// Deletes update package from db
// ---------------------------------------------------------------------------
void CFotaServer::DeletePackageL ( const TInt aPkgId)
    {
    FLOG(_L("CFotaServer::DeletePackageL  >> id %d"),aPkgId );
   // TInt err;

    StoragePluginL()->DeleteUpdatePackageL ( aPkgId );
//    User::LeaveIfError( err );
    FLOG(_L("CFotaServer::DeletePackageL  <<") );
    }


// ---------------------------------------------------------------------------
//  CFotaServer::DownloadL
//  Create package downloader and download update package.
// ---------------------------------------------------------------------------
void CFotaServer::DownloadL(TDownloadIPCParams aParams, const TDesC8& aPkgURL)
    {
    FLOG(_L("[FotaServer]  Download >>"));

    // If download already in progress, delete it
    if ( iDownloader )
        {
        FLOG(_L("CFotaServer::DownloadL already downloading!"));
        User::Leave (KErrAlreadyExists);
        }
    
    if(!iDownloader)
        {
        iDownloader = CFotaDownload::NewL( this );
        }
        
        TBuf<KSysVersionInfoTextLength> temp;
    if (GetSoftwareVersion(temp) == KErrNone)
        {
        
          RFileWriteStream    wstr;
        	CleanupClosePushL ( wstr );
        	TInt err1=wstr.Replace( iFs,KSWversionFile, EFileWrite  );
        	if(err1==KErrNone)
        	{
           HBufC16*             swv;
           swv = HBufC16::NewLC ( temp.Length() );
           swv->Des().Copy( temp );
           wstr.WriteInt16L( swv->Des().Length()); // length  
           wstr.WriteL( swv->Des() );
           wstr.WriteInt16L( 0 );
           CleanupStack::PopAndDestroy( swv );
               
          }                           
                                        
        	CleanupStack::PopAndDestroy( &wstr ); // wstr      
        }
    
    iDownloader->DownloadL(aParams,aPkgURL,EFalse);
    FLOG(_L("[FotaServer]  Download <<"));
    }


// --------------------------------------------------------------------------
// CFotaServer::DownloadAndUpdateL
// Downloads update package and updates fw
// --------------------------------------------------------------------------
//
void CFotaServer::DownloadAndUpdateL(TDownloadIPCParams aParams 
                                            ,const TDesC8& aPkgURL)
    {
    FLOG(_L("CFotaServer::DownloadAndUpdateL >>"));

    // If download already in progress, delete it
    if ( iDownloader )
        {
        FLOG(_L("CFotaServer::DownloadAndUpdateL already downloading!"));
        User::Leave (KErrAlreadyExists);
        }
    if (!iDownloader)
        {
        iDownloader = CFotaDownload::NewL(this);
        }
        TBuf<KSysVersionInfoTextLength> temp;
    if (GetSoftwareVersion(temp) == KErrNone)
        {
        	RFileWriteStream    wstr;
        	CleanupClosePushL ( wstr );
          TInt err = wstr.Replace( iFs ,KSWversionFile, EFileWrite );
         if(err ==KErrNone) 
          {
          HBufC16*             swv;
          swv = HBufC16::NewLC ( temp.Length( ) );
          swv->Des().Copy( temp );
          wstr.WriteInt16L( swv->Des().Length()); // length  
          wstr.WriteL( swv->Des() );
          wstr.WriteInt16L( 0 );
          CleanupStack::PopAndDestroy( swv );
                
          }       
                                 
         CleanupStack::PopAndDestroy( &wstr ); // wstr                                   
        	
        }
    iDownloader->DownloadL( aParams,aPkgURL,ETrue );
    FLOG(_L("CFotaServer::DownloadAndUpdateL <<"));
    }

// --------------------------------------------------------------------------
// CFotaServer::UpdateL
// Start update 
// --------------------------------------------------------------------------
//
void CFotaServer::UpdateL( const TDownloadIPCParams &aParams )
    {
    FLOG(_L("CFotaServer::UpdateL  >>"));
    
     TBool isPkgvalid(ETrue);
     isPkgvalid = CheckSWVersionL();
      
    if (isPkgvalid)
        {
        	
        FLOG(_L("CFotaServer::UpdateL  package is valid >>"));	
        if(!iUpdater)
            {
            iUpdater = CFotaUpdate::NewL(this);
            }
        iUpdater->StartUpdateL( aParams );
        }
    else
        {
        FLOG(_L("IMPORTANT:: Firmware version mismatch! Resetting fota state"));
        ResetFotaStateL(aParams);
        }
       
        FLOG(_L("CFotaServer::UpdateL  <<"));  
    }



// --------------------------------------------------------------------------
// CFotaServer::CheckSWVersionL
// Check the s/w version
// --------------------------------------------------------------------------
//

TBool CFotaServer::CheckSWVersionL()

{
	
	
	  FLOG(_L("CFotaServer::CheckSWVersionL  >>"));  
	 TBuf<KSysVersionInfoTextLength> temp;
    HBufC16*            message16=NULL;
    TBool isPkgvalid(ETrue);
    if (GetSoftwareVersion(temp) == KErrNone)
        {
          
         //TBuf<KSysVersionInfoTextLength>swvfromfile;
        //Fetch the software version ...
        RFileReadStream     rstr;
        TInt err1=rstr.Open(iFs,KSWversionFile ,EFileRead);
        if(err1== KErrNone)
        {
         CleanupClosePushL( rstr );
         TInt msglen = rstr.ReadInt16L(); 
         if(msglen > 0)
         { 
          	message16         = HBufC16::NewLC(msglen + 1);
        	 TPtr16 tempswv = message16->Des();
        	 TRAPD(err, rstr.ReadL(tempswv,msglen ));
        	        
           if ( err != KErrNone && err != KErrEof)
            {
             FLOG(_L("  file read err %d"),err); //User::Leave( err ); 
              msglen =0;
            }
           else
           { 
            FLOG(_L("  msglen  %d"),msglen); 
            TPtr swvfromfile = message16->Des();
                     
            FLOG(_L("  swvfromfile=%S"),message16); 
            
             //Compare the software versions to decide whether the download is still valid or not.
              if (msglen != temp.Length() || temp.Compare(tempswv)!=KErrNone)
              {
        	     isPkgvalid = EFalse;
        	      FLOG(_L("CFotaServer::software not matching  >>"));
        	
              }
            } 
          
             CleanupStack::PopAndDestroy( message16 );
         }    
            CleanupStack::PopAndDestroy( &rstr ); 
            
            
        }
        
       } 
       
        FLOG(_L("CFotaServer::CheckSWVersionL  <<"));  
       return isPkgvalid;
	
	
}	


// --------------------------------------------------------------------------
// CFotaServer::ScheduledUpdateL
// Update, triggered by scheduler
// --------------------------------------------------------------------------
void CFotaServer::ScheduledUpdateL( TFotaScheduledUpdate aUpdate )
    {
    FLOG(_L("CFotaServer::ScheduledUpdateL"));
    TPackageState s = GetStateL( aUpdate.iPkgId );

    // If update is in progress, do not start new one (multiple popups)
    if ( iUpdater )
    	{
    	FLOG(_L("\t\tupdate in progress"));
		return;
    	}
	else
		{
		
		//Check to find whether the user has already installed the package before the
		//reminder could expire
		
		RFs aRfs;
		RDir aDir;
		TEntryArray anArray;
		User::LeaveIfError(aRfs.Connect());
		
		
		TBuf16<KMaxFileName> temp;
		temp.Zero();
		temp.Copy(KSwupdPath8);
		
		if(aDir.Open(aRfs,temp,KEntryAttNormal)==KErrNone)
		{
			TInt error = aDir.Read(anArray);	
		}
		aDir.Close();
		aRfs.Close();
	
		temp.Zero();
		temp.Copy(KSwupdFileExt8);
		
		TBool aStartUpdate(EFalse);
		
		for (TInt i=0; i<anArray.Count();i++)
			{
			if (anArray[i].iName.Find(temp)!=KErrNotFound)
				{
				aStartUpdate = ETrue;
				break;
				}
			}
		
		if (aStartUpdate)
			{
			FLOG(_L("\t\tReminder expired and update packages found on dir"));
			iUpdater = CFotaUpdate::NewL(this);
	    	iUpdater->StartUpdateL( s );
			}
		else
			{
			FLOG(_L("\t\tReminder expired, but no update package is found on dir; skipping"));
			}
		}
    }

// --------------------------------------------------------------------------
// CFotaServer::DoConnect
// From CServer2. Initializes class members. 
// --------------------------------------------------------------------------
void CFotaServer::DoConnect(const RMessage2 &aMessage)
    {
    FLOG(_L("CFotaServer::DoConnect(const RMessage2 &aMessage) >>") );

    // In case shutdown is in progress, cancel it.
    if ( iAppShutter )
        {
        iAppShutter->Cancel();
        delete iAppShutter;
        iAppShutter=NULL;
        }

    if ( iInitialized == EFalse )
        {
        TRAPD( err, ClientAwareConstructL( aMessage ) );
        if ( err ) FLOG(_L("   ClientAwareConstructL err %d"),err);
        }

    CAknAppServer::DoConnect( aMessage );
    FLOG(_L("CFotaServer::DoConnect(const RMessage2 &aMessage) <<"));
    }   


// --------------------------------------------------------------------------
// CFotaServer::GetStateL
// Get state of a download package
// --------------------------------------------------------------------------
TPackageState CFotaServer::GetStateL( const TInt aPkgId)
    {
    TPackageState s=RFotaEngineSession::EIdle;

    if (aPkgId >= 0) // Used by all clients
    	{
    	iDatabase->OpenDBL();
    	s = iDatabase->GetStateL( aPkgId );
   	    iDatabase->CloseAndCommitDB();
    	}
    else if (aPkgId == -2) //Used by DM UI to check if fota ui has to be in foreground
    	{
    	TBool value (EFalse);
    	/** This P&S Key is used to notify DM UI on any download event. key=0 for idle and key=1 for download */     	
    	TInt err = RProperty::Get(TUid::Uid(KOmaDMAppUid),
                           KFotaDMRefresh,
                           value);
        FLOG(_L("RProperty KFotaDMRefresh Get, err = %d, val = %d"), err,value);
		if (err==KErrNone && value)
			{
			FLOG(_L("Ongoing download operation detected!"));
			s=RFotaEngineSession::EDownloadProgressing;
			}
    	}
    else if ( aPkgId == -1) //Used by DM UI to get the state of last fota operation
    	{
		//Read status from fotastate last entry

    	iDatabase->OpenDBL();

		RArray<TInt>    states;
        CleanupClosePushL (states);
        iDatabase->GetAllL ( states );
        // Loop states. 
        for(TInt i = 0; i < states.Count(); ++i ) 
            {
            TPackageState   tmp;
            tmp = iDatabase->GetStateL(  states[i]  );
            FLOG(_L("***Package: %d, State = %d"),states[i],(TInt) tmp.iState);
            if (tmp.iState!=RFotaEngineSession::EIdle)
	            {
            	s=tmp;
    	        }
            }
			FLOG(_L("Status of current operation is %d"),(TInt)s.iState);
		
		CleanupStack::PopAndDestroy( &states );
   	    iDatabase->CloseAndCommitDB();
    	}
    return s;
    }


// --------------------------------------------------------------------------
// CFotaServer::IsPackageStoreSizeAvailable 
// Checks if update package fits into storage
// --------------------------------------------------------------------------
//
TBool CFotaServer::IsPackageStoreSizeAvailableL ( const TInt aSize)
    {
    FLOG(_L("CFotaServer::IsPackageStoreSizeAvailableL >>"));
    TInt size=aSize;

		// Write content size for later use (flexible memory usage)
		RFileWriteStream str;	
		TUint32 				size2 = aSize;
		User::LeaveIfError(str.Replace(iFs, KSizePass, EFileWrite)); 
		CleanupClosePushL(str);
		str.WriteUint32L(size2);
		CleanupStack::PopAndDestroy(1);

    
    CFotaStorage::TFreeSpace avail = StoragePluginL()->IsPackageStoreSizeAvailableL(size);
    FLOG(_L("CFotaServer::IsPackageStoreSizeAvailableL <<"));
    return avail==CFotaStorage::EDoesntFitToFileSystem ? EFalse : ETrue;
    }

// --------------------------------------------------------------------------
// CFotaServer::OnSyncMLSessionEvent
// --------------------------------------------------------------------------
//
void CFotaServer::OnSyncMLSessionEvent(TEvent aEvent, TInt aIdentifier
                                       , TInt aError, TInt /*aAdditionalData*/)
    {
    if ( iSyncJobId != aIdentifier ) return;
    FLOG(_L("CFotaServer::OnSyncMLSessionEvent %d err:%d (id %d==%d?)")
            , aEvent,aError,aIdentifier,iSyncJobId);

    if ( iSyncJobId == aIdentifier )
	    {
        TTimeIntervalMicroSeconds32 close(0);
        TBool end ( EFalse );
	    switch( aEvent )
		    {
            //EJobStart = 0
            case EJobStartFailed: // 1 E
                {
                end = ETrue;
                }
			    break;
            case EJobStop: // 2 E
                {
                end = ETrue;
                // Sync ok => do not try anymore
                if ( aError == KErrNone ) 
                    {
                    iSyncMLAttempts=0;
                    }
                }
			    break;
            case EJobRejected: // 3 E
                {
                end = ETrue;
                }
			    break;
            // ETransportTimeout , // 7
    	    default:
                {
                }
			    break;
            }


        // sml session OK,close it 
        if ( end && iSyncMLAttempts == 0 ) 
            {
            FLOG(_L("   Sml OK, scheduling close"));
        	if( aError == KErrNone ) //always ask session successful
    		  {
    		  TInt val = KErrNotFound;
    		  TInt    err1 = RProperty::Get( TUid::Uid(KFotaServerUid),
    				  KFotaLrgObjDl, val );
    		  FLOG( _L( "CFotaSrvDocument::OnSyncMLSessionEvent KFotaLrgObjDl val & err is %d, %d" )
    		  														,val,err1 );  
    				  if( val == EOmaDmLrgObjDlFail ) //if large object
    					{
    					err1 = RProperty::Set( TUid::Uid(KFotaServerUid),
    							KFotaLrgObjDl, KErrNotFound );
    					FLOG( _L( "CFotaSrvDocument::OnSyncMLSessionEvent err for KFotaLrgObjDl is %d" )
    							,err1 );  
    					err1 = RProperty::Set( TUid::Uid(KFotaServerUid),
    							KFotaLrgObjProfileId, KErrNotFound );
    					FLOG( _L( "CFotaSrvDocument::OnSyncMLSessionEvent err for KFotaLrgObjProfileId is %d")
    																							 ,err1 );  
    							FLOG( _L( "[FotaServer] CFotaSrvDocument::OnSyncMLSessionEvent pkgid is %d" )
    									,iStorageDownloadPackageId );  
    							TRAP_IGNORE(GenericAlertSentL( iStorageDownloadPackageId ));
    					}
    		  }
            close = TTimeIntervalMicroSeconds32( 100000 );
            }
        // sml session NOK, retry
        if ( end && iSyncMLAttempts>0)
            {
            FLOG(_L("   Sml OK, scheduling retry"));
            close = TTimeIntervalMicroSeconds32( KSyncmlSessionRetryInterval);
            }

        if ( close > TTimeIntervalMicroSeconds32(0) )
            {
            if ( iTimedSMLSessionClose ) 
                {
                FLOG(_L("   closing smlsession timer"));
                iTimedSMLSessionClose->Cancel();
                delete iTimedSMLSessionClose;
                iTimedSMLSessionClose = NULL;
                }
            FLOG(_L("   starting smlsession timer"));
            TRAPD( err2, iTimedSMLSessionClose = CPeriodic::NewL (EPriorityNormal) );
            if ( !err2 )
            	{
							iTimedSMLSessionClose->Start (
                close
                , TTimeIntervalMicroSeconds32( KSyncmlSessionRetryInterval )
                , TCallBack( StaticDoCloseSMLSession,this ) ) ;
							}
							else FLOG(_L(" iTimedSMLSessionClose err %d"),err2);
            }
        }
    }


// --------------------------------------------------------------------------
// CFotaServer::TryToShutDownFotaServer()
// Try to shut down. After last client left, this is tried periodically.
// --------------------------------------------------------------------------
//
TInt CFotaServer::TryToShutDownFotaServer()
    {
    RProcess pr; TFullName fn = pr.FullName();  TUint prid = pr.Id();
	FLOG(_L( "CFotaServer::TryToShutDownFotaServer process(id %d)%S. this 0x%x")
	                ,prid,&fn,this);
    FLOG(_L("CFotaServer::TryToShutDownFotaServer()"));
    FLOG(_L("iSessMode:%d,iUserResume:%d,iNeedToClose:%d"),iSessMode,
                        iUserResume,iNeedToClose);
    TBool val (EFalse);
    if (iNotifHandler)
    	val = iNotifHandler->IsOpen();
    if( !iDownloader  && !iUpdater && !iTimedExecuteResultFile 
        && !iSyncMLSession.Handle() && !iRetryingGASend  && !val
        && !( iDownloader 
             && iDownloader->IsDownloadActive() && iUserResume == EFalse /*FMS*/ 
             && !iNeedToClose ))
        {
        
        FLOG(_L("   shutting down fotaserver"));
        if (iDownloader && iDownloader->IsDownloadActive() )
            {
            FLOG(_L("Shutting down active in TryToShutDownFotaServer..."));
            StopDownload(RFotaEngineSession::EResUndefinedError);
            }      
       	CAknAppServer::HandleAllClientsClosed();
       	return 1;
        }
    else
        {
        FLOG(_L("   shutdownwait:%d.%d.%d.%d.%d"), iDownloader,iUpdater
            , iTimedExecuteResultFile,iSyncMLSession.Handle(),val);
        }
    return 0;
    }


// ---------------------------------------------------------------------------
// StaticApplicationShutter
// Intermediate function
// ---------------------------------------------------------------------------
static TInt StaticApplicationShutter(TAny *aPtr)
    {
    __ASSERT_ALWAYS( aPtr, User::Panic(KFotaPanic, KErrArgument) );
    CFotaServer* srv = (CFotaServer*) aPtr;
    srv->TryToShutDownFotaServer();
    return KErrNone;
    }


// ---------------------------------------------------------------------------
// CFotaServer::HandleAllClientsClosed()
// Tries to shut down fotaserver. If unsuccesfull, periodically try it again
// and again and again ...
// ---------------------------------------------------------------------------
void CFotaServer::HandleAllClientsClosed()
    {   
    FLOG(_L("CFotaServer::HandleAllClientsClosed() >>"));

    if ( TryToShutDownFotaServer() == 0) 
        {
        FLOG(_L("   starting application shutdown" ));
        if ( iAppShutter )
            {
            iAppShutter->Cancel();
            delete iAppShutter;
            iAppShutter=NULL;
            }
        TRAPD ( err , iAppShutter = CPeriodic::NewL (EPriorityNormal) );
        __ASSERT_ALWAYS( err == KErrNone , User::Panic(KFotaPanic, err) );
        FLOG(_L("iSessMode = %d iUserResume:%d,iNeedToClose:%d"),
                iSessMode,iUserResume,iNeedToClose);
        if (iDownloader && iDownloader->IsDownloadActive() &&
                !(iUserResume == EFalse /*FMS*/ 
                  && !iNeedToClose ) )
        	{
            FLOG(_L("Shutting down active..."));
        	StopDownload(RFotaEngineSession::EResUndefinedError);
        	}

        iAppShutter->Start( KFotaTimeShutDown , KFotaTimeShutDown
                , TCallBack(StaticApplicationShutter,this) ) ;
        }

    FLOG(_L("CFotaServer::HandleAllClientsClosed() <<"));
    }

// --------------------------------------------------------------------------
// CFotaServer::GetUpdateTimeStampL 
// Gets time of last update. It is stored in a file.
// --------------------------------------------------------------------------
void CFotaServer::GetUpdateTimeStampL (TDes16& aTime)
    {
    FLOG(_L("CFotaServer::GetUpdateTimeStampL  >>"));
    TInt                err;

    RFileReadStream     rstr;
    err = rstr.Open( iFs, _L("updatetimestamp"), EFileRead );

    if ( err == KErrNone) 
        {
        FLOG(_L(" update time stamp file found,reading"));
        CleanupClosePushL (rstr);
        TInt year   =  rstr.ReadInt32L();
        TInt month  =  rstr.ReadInt32L();    
        TInt day    =  rstr.ReadInt32L();
        TInt hour   =  rstr.ReadInt32L();  
        TInt minute =  rstr.ReadInt32L();
        TInt year16   = year;
        TInt month16  = month; 
        TInt day16    = day;   
        TInt hour16   = hour;  
        TInt minute16 = minute;
        CleanupStack::PopAndDestroy( &rstr );
        aTime.Append (year16);
        aTime.Append (month16);
        aTime.Append (day16);
        aTime.Append (hour16);
        aTime.Append (minute16);
        }
    else if ( err != KErrNotFound )
        {
        User::Leave ( err );
        }

    if ( err == KErrNotFound ) 
        {
        FLOG(_L(" update time stamp not found "));
        }

    FLOG(_L("CFotaServer::GetUpdateTimeStampL  <<"));
    }

// --------------------------------------------------------------------------
// CFotaServer::GetUpdatePackageIdsL
// --------------------------------------------------------------------------
//
void CFotaServer::GetUpdatePackageIdsL(TDes16& aPackageIdList)
    {
    FLOG(_L("CFotaServer::GetUpdatePackageIdsL()"));
    StoragePluginL()->GetUpdatePackageIdsL( aPackageIdList );
    }



// --------------------------------------------------------------------------
// CFotaServer::GenericAlertSentL
// Generic alert sent, do cleanup. FUMO spec specifies cleanup need to have 
// for states 20,70,80,90,100. Called by syncml framework when it has sent
// generic alert
// --------------------------------------------------------------------------
//
void CFotaServer::GenericAlertSentL( const TInt aPackageID )
    {
    FLOG(_L("CFotaServer::GenericAlertSentL %d"), aPackageID);
    TPackageState   state;
    TBool           toidle(EFalse);
    TBool           deletepkg(EFalse);

    iDatabase->OpenDBL();
    state = iDatabase->GetStateL( aPackageID );

	switch( state.iState  )
		{
        case RFotaEngineSession::EDownloadFailed:
            {
            toidle = ETrue;
            deletepkg = ETrue;
            }
			break;
        case RFotaEngineSession::EUpdateFailed:
            {
            toidle = ETrue;
            deletepkg = ETrue;
            }
			break;
        case RFotaEngineSession::EUpdateFailedNoData:
            {
            toidle = ETrue;
            }
			break;
        case RFotaEngineSession::EUpdateSuccessful:
            {
            toidle = ETrue;
            deletepkg = ETrue;
            }
			break;
        case RFotaEngineSession::EUpdateSuccessfulNoData:
            {
            toidle = ETrue;
            }
			break;
        case RFotaEngineSession::EDownloadComplete:
        	{
        	state.iState = RFotaEngineSession::EStartingUpdate;
        	state.iResult = KErrNotFound;
            iDatabase->SetStateL( state,KNullDesC8, EFDBState );
        	toidle = EFalse;
        	}
        	break;
    	default:
            {
            FLOG(_L(" pkg %d (state:%d) doesnt need cleanup"), aPackageID
                        ,state.iState );
            }
			break;
        }
    
    if ( toidle )
        {
        state.iState = RFotaEngineSession::EIdle;
        state.iResult = KErrNotFound;
        iDatabase->SetStateL( state,KNullDesC8, EFDBState|EFDBResult );
        DeleteFUMOTreeL();
        }


   	SetStartupReason(EFotaDefault);

    iDatabase->CloseAndCommitDB();

    if ( deletepkg )
        {
        StoragePluginL()->DeleteUpdatePackageL( aPackageID );
        }

    // this should already be done when user was notified 
    // about update result
    DoDeleteUpdateResultFileL();
    }

// --------------------------------------------------------------------------
// CFotaServer::DoDeleteUpdateResultFileL
// Deletes the update resule file
// --------------------------------------------------------------------------
void CFotaServer::DoDeleteUpdateResultFileL()
	{
    CFotaUpdate::DeleteUpdateResultFileL( iFs );
	}

// --------------------------------------------------------------------------
// CFotaServer::CreateServiceL
// Creates session object
// --------------------------------------------------------------------------
CApaAppServiceBase* CFotaServer::CreateServiceL( TUid aServiceType ) const
	{
    FLOG(_L( "CFotaServer::CreateServiceL 0x%x " ), aServiceType.iUid );
    if ( aServiceType.iUid == KFotaServiceUid )
        {
	    return ((CApaAppServiceBase*) (new (ELeave) CFotaSrvSession));
        }
    else
        {
        return CAknAppServer::CreateServiceL ( aServiceType );
        }
	}

// --------------------------------------------------------------------------
// CFotaServer::GetEikEnv
// Gets the EikonEnv object
// --------------------------------------------------------------------------
CEikonEnv* CFotaServer::GetEikEnv()
	{
	return iEikEnv;
	}
	
// --------------------------------------------------------------------------
// CFotaServer::StartNetworkMonitorL       
// Starts Network Monitoring operation for defined interval and retries (FotaNetworkRegMonitor.h)
// --------------------------------------------------------------------------
void CFotaServer::StartNetworkMonitorL()
	{
	FLOG(_L("CFotaServer::StartNetworkMonitorL >>"));
	if (!iMonitor)
		iMonitor = CFotaNetworkRegStatus::NewL (this);
	iMonitor->StartMonitoringL();
	
	FLOG(_L("CFotaServer::StartNetworkMonitorL <<"));
	}
	
// --------------------------------------------------------------------------
// CFotaServer::ReportNetworkStatus       
// called by CFotaNetworkRegStatus for reporting status
// --------------------------------------------------------------------------
void CFotaServer::ReportNetworkStatus(TBool status)
	{
	FLOG(_L("CFotaServer::ReportNetworkStatus, status = %d >>"),status);
	iRetryingGASend = EFalse;
	iNetworkAvailable = status;
	
	if (iNetworkAvailable)
		{
		TRAPD (err, CreateDeviceManagementSessionL (iStoredState));
		if (err!=KErrNone)
			{
			FLOG(_L("Error %d occured while sending GA after retries"),err);
			}
		}
	//No need of iMonitor anymore	
    if ( iMonitor )    
    	{
    	delete iMonitor;
    	iMonitor = NULL;
    	}

	FLOG(_L("CFotaServer::ReportNetworkStatus >>"));
	}

// --------------------------------------------------------------------------
// CFotaServer::ShutApp 
// Shuts the DM App ui. This is used when End key is pressed during fota operation.
// --------------------------------------------------------------------------
//
void CFotaServer::ShutApp()
	{
	FLOG(_L("CFotaServer::ShutApp >>"));
	FLOG(_L("Ending DM UI...."));
	TApaTaskList taskList(GetEikEnv()->WsSession());
	TApaTask task=taskList.FindApp(TUid::Uid(KOmaDMAppUid));
	if(task.Exists())
		{
		task.EndTask();
		}
	FLOG(_L("CFotaServer::ShutApp <<"));
	}

// --------------------------------------------------------------------------
// CFotaServer::StopDownload 
// Stops any ongoing download operation. Depending on the property of download, the later is either paused
// or cancelled.
// --------------------------------------------------------------------------
//
void CFotaServer::StopDownload(TInt aReason)
	{
	FLOG(_L("CFotaServer::StopDownload, aReason = %d >>"), aReason);
	iUserResume = KErrNotFound;
	iSessMode = KErrNotFound;
    if (iDownloader)
    	{
        FLOG(_L("stopping fota download"));
        iNeedToClose  = EFalse;
        if (iDownloader->IsDownloadResumable())
    		{
    		TRAP_IGNORE(iDownloader->RunDownloadSuspendL(aReason));
    		}
   		else
   			{
   			TRAP_IGNORE(iDownloader->RunDownloadCancelL(aReason));
   			}
    	}	
	FLOG(_L("CFotaServer::StopDownload <<"));
	}

// --------------------------------------------------------------------------
// CFotaServer::UpdateDBdataL 
// For OMA DM large object download failure, this method updates the database
// and sends the generic alert.
// --------------------------------------------------------------------------
//
void CFotaServer::UpdateDBdataL()
	{	
	TInt val(KErrNotFound),UserCancel(KErrNotFound);
	TInt err = RProperty::Get( TUid::Uid(KFotaServerUid),
			KFotaLrgObjDl, val);
	FLOG(_L("CFotaServer::UpdateDBdata KFotaLrgObjDl value & err is %d, %d "),val,err);	
	//val = 1 means LrgObj Download & its failed , -1 means not lrg object download / stream commited
	if( val == EOmaDmLrgObjDlFail ) 
		{
		err = RProperty::Get( TUid::Uid(KOmaDMAppUid),
				KDmJobCancel, UserCancel);
		FLOG(_L("CFotaServer::UpdateDBdataL KDmJobCancel value & err is %d, %d "),UserCancel,err);
		if( UserCancel == KErrCancel )
			{			
			TInt ProfId(KErrNotFound);
			err = RProperty::Get( TUid::Uid(KFotaServerUid),
					KFotaLrgObjProfileId, ProfId);
			FLOG(_L("CFotaServer::UpdateDBdataL KFotaLrgObjProfileId value & err is %d, %d,pkgid is %d "),
					ProfId,err,iStorageDownloadPackageId);
			if(iStorageDownloadPackageId > KErrNotFound && ProfId > KErrNotFound )
				{
				iDatabase->OpenDBL();
				FLOG(_L("CFotaServer::UpdateDBdataL after iDatabase->OpenDBL() "));
				TPackageState state;
				state.iPkgId = iStorageDownloadPackageId;
				state.iProfileId  = ProfId;
				state.iState = RFotaEngineSession::EDownloadFailed;
				state.iResult = RFotaEngineSession::EResUserCancelled;				
				iDatabase->SetStateL( state,KNullDesC8, EFDBState | EFDBResult ) ;
				FLOG(_L("CFotaServer::UpdateDBdataL after iDatabase->SetStateL"));
				iDatabase->CloseAndCommitDB();		
				FLOG(_L("CFotaServer::UpdateDBdataL after iDatabase->CloseAndCommitDB "));			    
				// Free resources
				iChunk.Close();
				FLOG(_L("CFotaServer::UpdateDBdataL ,chunk released "));
				iStorage->UpdatePackageDownloadCompleteL(iStorageDownloadPackageId);		    				
				CreateDeviceManagementSessionL(state);
				}

			}
		}	
	}
// --------------------------------------------------------------------------
// CFotaServer::MonitorBatteryL()
// Monitors for the battery 
// 
// --------------------------------------------------------------------------
//
void CFotaServer::MonitorBattery(TInt aLevel)
{   FLOG(_L("CFotaServer::MonitorBatteryL(), level = %d >>"), aLevel);
    SetStartupReason(EFotaUpdateInterrupted);
	RFMSClient fmsclient;
	TRAPD(err,fmsclient.OpenL());
	if(err == KErrNone)
	{ FLOG(_L("CFotaServer::going into FMS client side MonitorBatteryL() >>"));
		//fmsclient.Cancel();
	  TRAPD(err1,fmsclient.MonitorForBatteryL(aLevel));
	   if(err1)
	   {
	   	FLOG(_L("CFotaServer:: MonitorBatteryL() left with error %d  >>"), err1);
	   }
	  fmsclient.Close();
	
	}
	
	FLOG(_L("CFotaServer::MonitorBatteryL() <<"));
  	
	
}

// --------------------------------------------------------------------------
// CFotaServer::CheckIapExistsL 
// Checks for IAP Id exists or not in commsdb
// IAP Id used for resuming the download or for sending Generic alert
// --------------------------------------------------------------------------
//
TBool CFotaServer::CheckIapExistsL(TUint32 aIapId)
    {
    FLOG(_L("CFotaServer::CheckIapExistsL >>"));
    CCommsDatabase* commDb = CCommsDatabase::NewL( EDatabaseTypeIAP );
    CleanupStack::PushL( commDb );
    CApUtils* aputils = CApUtils::NewLC(*commDb);
    TBool exists = aputils->IAPExistsL( aIapId);      
    CleanupStack::PopAndDestroy( aputils );
    CleanupStack::PopAndDestroy( commDb );
    FLOG(_L("CFotaServer::CheckIapExistsL <<"));
    return exists;
    }

// --------------------------------------------------------------------------
// CFotaServer::GetSoftwareVersion
// Gets the software version
// 
// --------------------------------------------------------------------------
//
TInt CFotaServer::GetSoftwareVersion(TDes& aVersion)
    {
    FLOG(_L("CFotaServer::GetSoftwareVersion >>"));
    aVersion.Zero();
    
    SysVersionInfo::TVersionInfoType what = SysVersionInfo::EFWVersion;
    TInt error (KErrNone);
    error = SysVersionInfo::GetVersionInfo(what,aVersion);
    FLOG(_L("CFotaServer::GetSoftwareVersion,SwV=%S <<"),&aVersion);
    return error;
    }
    
// --------------------------------------------------------------------------
// CFotaServer::ResetFotaStateL
// Resets the Fotastate
// 
// --------------------------------------------------------------------------
//    
void CFotaServer::ResetFotaStateL(const TDownloadIPCParams& aParams)
    {
    FLOG(_L("CFotaServer::ResetFotaStateL >>"));

    TPackageState state;
    if (!iDatabase->IsOpen()) iDatabase->OpenDBL();
    //Fetch the software version that was before download from db.
    state = iDatabase->GetStateL(aParams.iPkgId);
    state.iState = RFotaEngineSession::EUpdateFailed;
    state.iResult = RFotaEngineSession::EResPackageMismatch;
    iDatabase->SetStateL( state,KNullDesC8, EFDBState | EFDBResult );
    iDatabase->CloseAndCommitDB();
    
    StoragePluginL()->DeleteUpdatePackageL ( aParams.iPkgId );
    
    CreateDeviceManagementSessionL(state);
    
    FLOG(_L("CFotaServer::ResetFotaStateL <<"));
    }

// --------------------------------------------------------------------------
// CFotaServer::SetInstallUpdateClientL 
// Set's who is the client triggering the update
// --------------------------------------------------------------------------
//
void CFotaServer::SetInstallUpdateClientL(TInt aClient)
    {    
    iInstallupdClient = aClient;
    FLOG(_L("CFotaServer::SetInstallUpdateClientL() client:%d<<"),iInstallupdClient);
    }

// --------------------------------------------------------------------------
// CFotaServer::GetInstallUpdateClientL 
// returns the client who triggered the update
// --------------------------------------------------------------------------
//
TInt CFotaServer::GetInstallUpdateClientL()
    {
    FLOG(_L("CFotaServer::GetInstallUpdateClientL() client:%d<<"),iInstallupdClient);
    return iInstallupdClient ;
    }

// CFotaServer::NeedToDecryptL
// This method is called to check if decryption is needed.
// 
// --------------------------------------------------------------------------
// 
TBool CFotaServer::NeedToDecryptL(const TInt &aPkgId, TDriveNumber &aDrive)
    {
    FLOG(_L("CFotaServer::NeedToDecryptL >>"));
    
    TBool ret (EFalse);

    //Finding the drive number
    TBuf8<KMaxPath> path8;
    path8.Zero();
    StoragePluginL()->GetUpdatePackageLocationL(aPkgId, path8);
    TPath path16;
    path16.Copy(path8);


    TDriveNumber drive (EDriveC ); //Default drive is Phone Memory
    TParse p;
    if (!p.Set(path16,NULL,NULL))
        {
        TDriveName drivename(p.Drive());
        TDriveUnit driveunit(drivename);
        if (iFs.IsValidDrive((TInt) driveunit))
            {
            drive =  (TDriveNumber) driveunit.operator TInt();
            iStorageDrive = drive;
            }
        }
    else
        {
        FLOG(_L("Error while parsing for drive number! defaulting to Phone Memory (C)"));
        }
    FLOG(_L("Package storage drive is %d"), (TInt) drive);

    if (!iDEController)
        {
        TRAPD(err, iDEController = CDevEncController::NewL(this));
        if (err == KErrNotSupported)
            {
            //Encryption feature is not on.
            return EFalse;
            }
        else
            {
            __LEAVE_IF_ERROR(err);
            }
        }
    TRAPD(err, ret = iDEController->NeedToDecryptL(drive));
    
    delete iDEController; iDEController = NULL;
    
    if (err == KErrNotSupported)
        {
        //Encryption feature is ON, but the encryption adaptation is note present.
        ret = EFalse;
        }
    else
        {
        __LEAVE_IF_ERROR(err);
        }
   
    //Set the appropriate drive when ret is true
    if (ret)
        {
        aDrive = drive;
        }
    
    FLOG(_L("CFotaServer::NeedToDecryptL ret = %d, drive = %d <<"), ret, drive);
    return ret;

    }

// --------------------------------------------------------------------------
// CFotaServer::DoStartDecryptionL
// This method is called to start the decryption operation.
// 
// --------------------------------------------------------------------------
// 
void CFotaServer::DoStartDecryptionL()
    {
    FLOG(_L("CFotaServer::DoStartDecryptionL >>"));
    
    if (!iDEController)
        iDEController = CDevEncController::NewL(this);
    
    iDEController->DoStartDecryptionL(iStorageDrive);
    
    FLOG(_L("CFotaServer::DoStartDecryptionL <<"));
    }

// --------------------------------------------------------------------------
// CFotaServer::HandleDecryptionCompleteL
// This method is called to when decryption operation is complete.
// 
// --------------------------------------------------------------------------
// 
void CFotaServer::HandleDecryptionCompleteL(TInt aResult, TInt aValue)
    {
    FLOG(_L("CFotaServer::HandleDecryptionCompleteL, result = %d, value = %d >>"), aResult, aValue);
    
    if (aResult == KErrNone)
        {
        //Skip battery test as it is already performed before decryption
        iUpdater->HandleUpdateAcceptStartL(ETrue);
        }
    else
        {
        FLOG(_L("Can't start update because of error %d"), aResult);
        iUpdater->HandleUpdateErrorL(aResult, aValue);
        }
    
    if (iDEController)
        {
        delete iDEController; iDEController = NULL;
        }
    
    FLOG(_L("CFotaServer::HandleDecryptionCompleteL <<"));
    }


// --------------------------------------------------------------------------
// CFotaServer::NeedToEncryptL
// This method is called to check if encryption is needed.
// 
// --------------------------------------------------------------------------
// 
TBool CFotaServer::NeedToEncryptL(TDriveNumber &aDrive)
    {
    FLOG(_L("CFotaServer::NeedToEncryptL >>"));
    
    TDriveNumber drive;
    if (!iDEController)
        {
        TRAPD(err, iDEController = CDevEncController::NewL(this));
        if (err == KErrNotSupported)
            {
            return EFalse;
            }
        else
            {
            __LEAVE_IF_ERROR(err);
            }
        }
    
    TBool ret = iDEController->NeedToEncryptL(drive);
    delete iDEController; iDEController = NULL;
    
    if (ret)
        {
        aDrive = drive;
        iStorageDrive = drive;
        }
    
    FLOG(_L("CFotaServer::NeedToEncryptL, ret = %d drive = %d << "), ret, aDrive);
    return ret;
    }

// --------------------------------------------------------------------------
// CFotaServer::DoStartEncryptionL
// This method is called to start the encryption operation.
// 
// --------------------------------------------------------------------------
// 
void CFotaServer::DoStartEncryptionL()
    {
    FLOG(_L("CFotaServer::DoStartEncryptionL >>"));
    
    if (!iDEController)
        iDEController = CDevEncController::NewL(this);
    
    iDEController->DoStartEncryptionL(iStorageDrive);
    
    FLOG(_L("CFotaServer::DoStartEncryptionL <<"));
    }

// --------------------------------------------------------------------------
// CFotaServer::HandleEncryptionCompleteL
// This method is called when the encryption operation is complete.
// 
// --------------------------------------------------------------------------
// 
void CFotaServer::HandleEncryptionCompleteL(TInt aResult, TInt aValue)
    {
    FLOG(_L("CFotaServer::HandleEncryptionCompleteL, result = %d, value = %d >>"), aResult, aValue);
    
    if (aResult == KErrNone)
        {
        //Do nothing
        }
    else
        {
        FLOG(_L("Can't start update because of error %d"), aResult);
        iUpdater->HandleEncryptionErrorL(aResult);
        }
    
    if (iDEController)
        {
        delete iDEController; iDEController = NULL;
        }
    
    FLOG(_L("CFotaServer::HandleEncryptionCompleteL <<"));
    }


// --------------------------------------------------------------------------
// CFotaServer::GetDEOperation
// This method returns the device encryption operation.
// 
// --------------------------------------------------------------------------
// 
TInt CFotaServer::GetDEOperation()
    {
    FLOG(_L("CFotaServer::GetDEOperation >>"));
    TInt ret (EIdle);

    if (iDEController)
        ret = iDEController->GetDEOperation();
    
    FLOG(_L("CFotaServer::GetDEOperation, ret = %d <<"), ret);
    return ret;
    }

