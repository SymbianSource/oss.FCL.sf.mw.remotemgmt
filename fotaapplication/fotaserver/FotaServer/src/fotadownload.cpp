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
* Description:   downloads upd pkg
*
*/



// INCLUDES
#ifdef SYMBIAN_ENABLE_SPLIT_HEADERS
#include <es_enum_internal.h>
#endif
#include <centralrepository.h>
#include <f32file.h>
#include <AknWaitDialog.h>
#include <fotaserver.rsg>
#include <eikprogi.h>
#include <eikenv.h>
#include <apgtask.h>
#include <apgwgnam.h>
#ifndef SYMBIAN_ENABLE_SPLIT_HEADERS
#include <es_sock.h>
#else
#include <es_sock.h>
#include <es_sock_partner.h>
#endif
#include <es_enum.h>
#include <NSmlDMSync.rsg>	//OMA DM UI for reading resource
#include <bautils.h>
#include <StringLoader.h>
#include <DevManInternalCRKeys.h> //for reading san feature
#include <e32property.h>
#include "fotastorage.h"
#include "FotaSrvApp.h"
#include "fotadownload.h"
#include "fotaserverPrivatePSKeys.h"
#include "FotaNetworkRegStatus.h"
#include <startupdomainpskeys.h>	//globalrfsstates
#define __LEAVE_IF_ERROR(x) if(KErrNone!=x) {FLOG(_L("LEAVE in %s: %d"), __FILE__, __LINE__); User::Leave(x); }
#define __LEAVE(x) {FLOG(_L("LEAVE in %s: %d"), __FILE__, __LINE__); User::Leave(x); }


// --------------------------------------------------------------------------
// CFotaDownload::NewL 
// --------------------------------------------------------------------------
CFotaDownload* CFotaDownload::NewL (CFotaServer* aServer)
	{
	__ASSERT_ALWAYS( aServer, User::Panic(KFotaPanic, KErrArgument) );
	CFotaDownload* ao = new (ELeave) CFotaDownload();
	ao->iFotaServer = aServer;
	return ao;
	}


// --------------------------------------------------------------------------
// CFotaDownload::RunL
// Do operations started in HandleDmgrEvent
// --------------------------------------------------------------------------
void CFotaDownload::RunL()
	{
	FLOG(_L( "[FotaServer] CFotaDownload::RunL() >>"));
	CancelDownload( ETrue );
	CreateDownloadL();
	FLOG(_L( "[FotaServer] CFotaDownload::RunL() <<" ) );
	}


// --------------------------------------------------------------------------
// CFotaDownload::DoCancel()
// Cancel notifier request
// --------------------------------------------------------------------------
//
void CFotaDownload::DoCancel()
	{
	FLOG(_L("CFotaDownload::DoCancel() >>"));
	FLOG(_L("CFotaDownload::DoCancel() <<"));
	}


// --------------------------------------------------------------------------
// CFotaDownload::RunError(TInt aError)
// Handle active object run error.
// --------------------------------------------------------------------------
//
TInt CFotaDownload::RunError(TInt aError)
	{
	if(aError)
		{
		FLOG(_L(" CFotaDownload::RunError err %d"), aError );
		}
	return aError;
	}


// --------------------------------------------------------------------------
// CFotaDownload::CFotaDownload()
// --------------------------------------------------------------------------
//
CFotaDownload::CFotaDownload() : CActive(EPriorityNormal)
		, iUpdateAfterDownload (EFalse), iUrl( NULL ),
		iDownload (NULL),
		iDownloadedContentSize(0),
		iPreviousContentSize ( 0 ), iShowOMACDUI(EFalse), iShowDLPrgUI(EFalse),
		iDownloadResumable (EFalse),				
		iFMSEnabled (EFalse),
		iDownloadActive (EFalse),
		iFreshDL(ETrue),
		iUserInitiatedResume(EFalse),		
		iSanValue(KErrNotFound),
		iDownloadFinalized(EFalse)		
	{
	CActiveScheduler::Add( this ); 
	FLOG(_L("CFotaDownload::CFotaDownload()"));
	}


// --------------------------------------------------------------------------
// CFotaDownload::~CFotaDownload()
// --------------------------------------------------------------------------
//
CFotaDownload::~CFotaDownload()
	{
	FLOG(_L("CFotaDownload::~CFotaDownload() >>"));
	Cancel();
	
	//Sets download state inactive if accidently closed. This is anyhow taken care in FinalizeDownload in Server.
	SetDownloadActive(EFalse);
	CancelDownload( ETrue );
	if ( iUrl )
		{
		delete iUrl; iUrl=NULL;
		}
/*	if ( iNotifHandler ) Deleted at server
		{
		iNotifHandler->Cancel();
		delete iNotifHandler;        
		}*/
	if ( iDLProgressDlg )
		{
		delete iDLProgressDlg; iDLProgressDlg=NULL;
		}

	if (iDownloadMgr.Handle())
		iDownloadMgr.Close();
	//don't delete iFotaServer as it doesn't belong here.
	
	FLOG(_L("CFotaDownload::~CFotaDownload() <<"));
}


// --------------------------------------------------------------------------
// CFotaDownload::CancelDownload
// Cancel download
// --------------------------------------------------------------------------
//
void CFotaDownload::CancelDownload( const TBool aCancelProgressBar )
	{
	FLOG(_L("CFotaDownload::CancelDownload() >>"));
	
	if( iDownloadMgr.Handle() )
		{
		iDownloadMgr.Close();
		}

	if ( aCancelProgressBar  && iDLProgressDlg )
		{
		TRAPD(err, iDLProgressDlg->ProcessFinishedL() ) ;// deletes
		if(err); // remove compiler warning
		delete iDLProgressDlg;
		iDLProgressDlg = NULL;
		}
	FLOG(_L("CFotaDownload::CancelDownload() <<"));
	}

// --------------------------------------------------------------------------
// CFotaDownload::CreateDownloadL
// Create  download
// --------------------------------------------------------------------------
//
void CFotaDownload::CreateDownloadL()
	{
	FLOG(_L("CFotaDownload::CreateDownloadL() >>") );
	__ASSERT_ALWAYS( iDownloadMgr.Handle()==0, User::Leave(KErrAlreadyExists) );
	RHttpDownload*  d(NULL);
	iDLNeedsReset = EFalse;
	TBool       created;
	iDownloadMgr.ConnectL( TUid::Uid(KFotaServerUid) , *this, EFalse); 
	FLOG(_L("CFotaDownload::CreateDownloadL, Stage 1") );
	iDownloadMgr.DeleteAll();

	TRAPD(err, d = &iDownloadMgr.CreateDownloadL( *iUrl, created ) );
	RFs fs;	
	if(fs.Connect() == KErrNone)
	    {
	    RFile file;    
	    CleanupClosePushL( fs );
	    CleanupClosePushL( file );
	    err=fs.MkDir(KDDDirPath);
	    FLOG(_L("creating directory err as %d"),err);
	    err=fs.SetSessionPath(KDDDirPath);
	    FLOG(_L("setting session path err as %d"),err);
	    err=fs.ShareProtected();
	    FLOG(_L("ShareProtected err as %d"),err);
	    err = file.Replace(fs,KDDFilename, EFileWrite|EFileShareAny);	
	    FLOG(_L("creating rfile err as %d"),err);	
	    err = d->SetFileHandleAttribute(file);
	    FLOG(_L("setting dlmgr destfile attrib err as %d"),err);
	    CleanupStack::PopAndDestroy( &file );
	    CleanupStack::PopAndDestroy( &fs );
	    }	
	FLOG(_L("CFotaDownload::CreateDownloadL, Stage 2. err %d"),err);
	if ( err  == KErrArgument ) User::Leave( KErrNotFound );
	else if( err !=KErrNone)    User::Leave( err );

	if ( iDLState.iIapId > KErrNotFound )
		{
		FLOG(_L("   Using internet access point iapid: %d"), iDLState.iIapId);
		err = iDownloadMgr.SetIntAttribute( EDlMgrIap, iDLState.iIapId); // IAP id
		User::LeaveIfError( err );        
		//At this point the iIapId used in the SyncML Session is presumed working. Hence
		//the same is stored in database for use during download.
		FLOG(_L("Setting IAPID=%d is FotaState"),iDLState.iIapId);
		iFotaServer->iDatabase->OpenDBL();
		iFotaServer->iDatabase->SetStateL(iDLState,KNullDesC8, EFDBIapId );
		iFotaServer->iDatabase->CloseAndCommitDB();
		}
	FLOG(_L("CFotaDownload::CreateDownloadL, Stage 3") );
	User::LeaveIfError( d->SetIntAttribute(EDlAttrFotaPckgId ,iDLState.iPkgId) );
	FLOG(_L("CFotaDownload::CreateDownloadL, Stage 4") );
	User::LeaveIfError( d->SetBoolAttribute(EDlAttrNoContentTypeCheck,ETrue) );

	// If ui hidden or download restarting, do not show descriptor
	if ( !iShowOMACDUI || iRestartCounter > 0 )
		{
		FLOG(_L("setting DD to hidden"));
		User::LeaveIfError(iDownloadMgr.SetBoolAttribute( EDlMgrSilentMode,ETrue )) ;
		}
	//It's a fresh download
	iFreshDL = ETrue;

	// Start download 
	User::LeaveIfError(iDownloadMgr.StartAll()); 
	FLOG(_L("CFotaDownload::CreateDownloadL, Stage 5") );
	iDLProgress = EStarted;

	//This P&S Key is used to notify DM UI on any download event. key=0 for idle and key=1 for download
	err = RProperty::Set( TUid::Uid(KOmaDMAppUid),
			KFotaDMRefresh,
			ETrue );
	FLOG(_L("RProperty KFotaDMRefresh Set Etrue, err = %d"), err);

	SetDownloadActive(ETrue);

	User::LeaveIfError(err);
	FLOG(_L("[FotaServer]  CFotaDownload::CreateDownloadL <<"));
}

	// --------------------------------------------------------------------------
	// CFotaDownload::DownloadL
	// Start download of swupd package.
	// --------------------------------------------------------------------------
	//
void CFotaDownload::DownloadL(TDownloadIPCParams aParams,const TDesC8& aPkgURL
		,TBool aUpdateAfterDownload,const TInt aIapid,const TInt aRestartDownload)
	{
	FLOG(_L("[FotaServer]  CFotaDownload::DownloadL >> restart %d"),aRestartDownload);

	TInt err = RProperty::Define( TUid::Uid(KOmaDMAppUid),
			KFotaDMRefresh,
			RProperty::EInt,KReadPolicy,KWritePolicy);
	FLOG(_L("RProperty KFotaDMRefresh Define, err = %d"), err);        
	if (err != KErrAlreadyExists )
		{
		User::LeaveIfError(err);

		err = RProperty::Set( TUid::Uid(KOmaDMAppUid),
				KFotaDMRefresh,
				KErrNotFound );

		FLOG(_L("RProperty KFotaDMRefresh Set KErrNotFound, err = %d"), err);
		User::LeaveIfError(err);
		}

	if ( iUrl ) {delete iUrl; iUrl=NULL;}
	iUrl = aPkgURL.Alloc();
	iUpdateAfterDownload = aUpdateAfterDownload;
	iRestartCounter = aRestartDownload;

	iDLState        = aParams;
	FLOG(_L("CFotaDownload::DownloadL, Stage 1") );

	// Set state -------------------------------------------------------------
	iDLState.iState = RFotaEngineSession::EStartingDownload;

	iDLState.iResult = KErrNotFound;
	iDLState.iUpdateLtr = aUpdateAfterDownload;
	iFotaServer->iDatabase->OpenDBL();
	iFotaServer->iDatabase->SetStateL( iDLState, *iUrl,EFDBState|EFDBResult
			| EFDBProfileId|EFDBPkgUrl|EFDBPkgName|EFDBVersion|EFDBUpdateLtr);
	iFotaServer->iDatabase->CloseAndCommitDB();
	FLOG(_L("CFotaDownload::DownloadL, Stage 2, pkgid:%d"),iDLState.iPkgId);

	// 2. Get Iap Id to use for download. This would be set in iDLState.iIapId.
	SetIapToUseL(aParams, aIapid);

	// 3. Determine whether download should be visible or not
	// Autoaccepted profile?

	SetDownloadUiBehavior(ETrue);

	CancelDownload( ETrue );

	FLOG(_L("Setting SessionType=%d in FotaState"),iDLState.iSessionType);
	iFotaServer->iDatabase->OpenDBL();
	iFotaServer->iDatabase->SetStateL(iDLState,KNullDesC8, EFDBSessionType );
	iFotaServer->iDatabase->CloseAndCommitDB();
	FLOG(_L("CFotaDownload::DownloadL, Stage 3") );
	//Called to read the download variations in Fota (resume support & FMS)
	CheckDownloadVariations();

	//Create the download
	CreateDownloadL( );
	FLOG(_L("[FotaServer]  CFotaDownload::DownloadL <<"));
	}

// ---------------------------------------------------------------------------
// CFotaDownload::HandleDLProgressDialogExitL
// Handle canceling of download
// ---------------------------------------------------------------------------
TBool CFotaDownload::HandleDLProgressDialogExitL( TInt aButtonId ) 
	{
	FLOG(_L("CFotaDownload::HandleDLProgressDialogExitL %d, note = %d >>"),aButtonId, iDLProgressDlg->iNoteType);
	// Cancel clicked during download
	if ( aButtonId == KErrNotFound )
		{
		iRestartCounter = -1;
		FLOG(_L("[FotaServer]  CFotaDownload::HandleDLProgressDialogExitL Hiding UI"));
		iFotaServer->iParentApp->SetUIVisibleL ( EFalse, ETrue );
		iUpdateAfterDownload = EFalse;

		iDLState.iResult = RFotaEngineSession::EResUserCancelled;

		if (iDLProgressDlg->iNoteType == EConnectingNote)
			{
			iDLState.iState = RFotaEngineSession::EDownloadProgressingWithResume;

			TInt active (KErrNotFound);
			RProperty::Get( TUid::Uid(KFotaServerUid), KFotaDownloadActive, active );
			FLOG(_L("active = %d"),active);
			
			if (active) //Resume operation has just begun and download request is submitted to dlmgr. Hence need to suspend.
				{
				RunDownloadSuspendL(RFotaEngineSession::EResUserCancelled, ETrue);
				}
			else //Resume operation has just begun, but download request is not submitted to dlmgr
				{
				LaunchNotifierL( ESyncMLFwUpdOmaDLPostponed, EFalse, EFalse );
				iFotaServer->FinalizeDownloadL( iDLState );
				}
			}
		else if (iDLProgressDlg->iNoteType == EDownloadingNote)
			{
			if (iDownloadResumable)
				{
				//Resume is supported. Query user whether to postpone or cancel download.
				FLOG(_L("User pressed cancel. Resume is supported; hence pause download and query user if download has to be postponed or cancelled permanently."));
				RunDownloadSuspendL(RFotaEngineSession::EResUserCancelled);
				}
			else
				{
				//Resume is not supported. Download is cancelled
				FLOG(_L("User pressed cancel. Resume is not supported & hence download has to be cancelled"));
				RunDownloadCancelL(RFotaEngineSession::EResUserCancelled);
				}
			}
		}
	FLOG(_L("CFotaDownload::HandleDLProgressDialogExitL <<"));
	return ETrue;
	}


// --------------------------------------------------------------------------
// CFotaDownload::DoHandleDMgrEventL
// Handles download events. Updates package state accordingly
// --------------------------------------------------------------------------
void CFotaDownload::DoHandleDMgrEventL( RHttpDownload* aDownload
		, THttpDownloadEvent aEvent )
	{
	__ASSERT_ALWAYS( aDownload, User::Panic(KFotaPanic, KErrArgument) );
	TInt32                      sysErrorId( KErrNone );
	TInt32                      contentsize;
	TInt32                      downloadedsize;

	TInt32                      tmp;
	THttpDownloadMgrError       dlErrorId;
	iDownload = aDownload;
	// If DL is in final state (complete/fail), there's an error id available.
	User::LeaveIfError( aDownload->GetIntAttribute( EDlAttrGlobalErrorId
			,sysErrorId ) );
	User::LeaveIfError( aDownload->GetIntAttribute( EDlAttrDownloadedSize
			,downloadedsize ) );
	User::LeaveIfError( aDownload->GetIntAttribute( EDlAttrLength
			,contentsize ) );
	User::LeaveIfError( aDownload->GetIntAttribute( EDlAttrErrorId
			,tmp ) );
	dlErrorId = (THttpDownloadMgrError)tmp;


	// Update progress dlg progress
	if ( !iDLProgressDlg && iRestartCounter>0)
		{
		FLOG(_L("reshowing progress..."));
		iDLProgressDlg = new(ELeave) CFotaDLProgressDlg((CEikDialog** ) &iDLProgressDlg , EFalse, this, EDownloadingNote );
		iDLProgressDlg->PrepareLC( R_FOTASERVER_DL_PROGRESS_DIALOG );
		iDLProgressDlg->RunLD();
		}
	if (iDLProgressDlg)
		{
		CEikProgressInfo* progressInfo = iDLProgressDlg->GetProgressInfoL();
		__ASSERT_ALWAYS( progressInfo, User::Panic(KFotaPanic, KErrBadHandle) );
		if ( iPreviousContentSize!=contentsize )
			{
			iPreviousContentSize = contentsize;
			if ( contentsize > 0 )
				{
				progressInfo->SetFinalValue( contentsize );
				}
			}
		TInt incr = downloadedsize - iDownloadedContentSize;
		if (incr<=0) incr=0; //Protection
		progressInfo->IncrementAndDraw(incr);
		iDownloadedContentSize = downloadedsize;
		}
	FLOG(_L("CFotaDownload::HandleDMgrEventL() >>   DLstate:%d  ProgressSt:%d\
	globErr: %d DLError: %d (%d/%d)"),aEvent.iDownloadState
	,aEvent.iProgressState,sysErrorId,dlErrorId,downloadedsize,contentsize );

	if ( sysErrorId <= -25000)  { FLOG(_L("  error %d interpreted as http \
	error %d") , sysErrorId, sysErrorId + 25000 ); }

	switch ( aEvent.iDownloadState )
	{
	case EHttpDlCreated:		// 1
		{
		FLOG(_L("Download State: EHttpDlCreated"));
		}
		break;
	case EHttpDlInprogress:     // 2
		{
		FLOG(_L("Download State: EHttpDlProgress"));


		if (aEvent.iProgressState == EHttpProgCodDescriptorDownloaded)
			{
			if (iShowOMACDUI || iShowDLPrgUI)
				{
				iFotaServer->iParentApp->SetUIVisibleL ( ETrue, EFalse);
				}
			}

		// Is ECodLoadEnd optimized out?
		if ( aEvent.iProgressState == EHttpProgCodLoadEnd ) // 2503
			{
			TInt    err;
			err = aDownload->Start();  // PPAR-6FRHFY fixed
			if(err)
				{
				FLOG(_L(" ERROR when restarting DL %d"),err);
				User::Leave(err);
				}
			FLOG( _L("          DL restarted") );
			}

		// Should tell user to free disk space. But how do we know that disk space is about be exceeded?

		if ( iDLNeedsReset )
			{
			FLOG(_L("aDownload->Reset();"));
			iDLNeedsReset = EFalse;
			iStatus = KRequestPending;
			SetActive();
			TRequestStatus* status = &iStatus;
			User::RequestComplete( status, KErrNone );
			User::LeaveIfError( aDownload->Reset() );
			}

		// Body data of descriptor or payload pkg
		if ( aEvent.iProgressState == EHttpProgResponseBodyReceived && iDLProgress==EDescriptorDownloaded/*2500*/)
			{
			// Now pkg size can be retrieved. must check size.
			iDLState.iPkgSize = contentsize;
			FLOG(_L("Setting PkgSize=%d in FotaState"),contentsize);
			iFotaServer->iDatabase->OpenDBL();
			iFotaServer->iDatabase->SetStateL(iDLState,KNullDesC8, EFDBPkgSize );
			iFotaServer->iDatabase->CloseAndCommitDB();
			

			TInt contentsize2 = contentsize;
			
			TInt dlsize (KErrNone);
			TInt tlsize (KErrNone);

			TRAPD(err, iFotaServer->GetDownloadUpdatePackageSizeL(iDLState.iPkgId, dlsize,tlsize));
			if(err); // remove compiler warning
			contentsize2 -= dlsize;
			FLOG(_L("check pkg size = %d"),contentsize2 );

			CFotaStorage::TFreeSpace avail = iFotaServer->StoragePluginL()->IsPackageStoreSizeAvailableL(contentsize2);

			iDLProgress = ESizeChecked;

			if ( avail==CFotaStorage::EDoesntFitToFileSystem )
				{
				FLOG(_L("CFotaStorage::EDoesntFitToFileSystem"));
				if (iFreshDL)
					{
					if ( iShowDLPrgUI )
						{
						FLOG(_L("iDLNeedsReset = ETrue;"));
						iDLNeedsReset = ETrue;
						LaunchNotifierL( ESyncMLFwUpdOmaDLNotEnoughMemory, KErrDiskFull,contentsize2 );
						}
					else
						{
						iDLState.iResult = RFotaEngineSession::EResDLFailDueToDeviceOOM;
						iDLState.iState  = RFotaEngineSession::EDownloadFailed;
						iUpdateAfterDownload = EFalse;
						LaunchNotifierL( ESyncMLFwUpdErrorNote, KErrGeneral, contentsize2,ETrue );
						}
					}
				else //resuming download
					{
					FLOG(_L("Memory need in order to resume. Notify user..."));
					LaunchNotifierL( ESyncMLFwUpdOmaDLNotEnoughMemory, KErrDiskFull,contentsize2 );
					RunDownloadSuspendL(RFotaEngineSession::EResDLFailDueToDeviceOOM);
					}
					
				}
			}

		// OMA DD download completed , must hide progress
		if ( aEvent.iProgressState == EHttpProgContentTypeChanged ) // 2055
			{
			if ( iDLProgressDlg )
				{
				iDLProgressDlg->ProcessFinishedL(); // deletes itself
				delete iDLProgressDlg;
				iDLProgressDlg = NULL;
				}
			}
		// OMA DD accepted,show progress
		if ( aEvent.iProgressState == EHttpProgCodDescriptorAccepted ) // 2502

			{
			iDLProgress = EDescriptorDownloaded;
			if ( iShowDLPrgUI )
				{
				FLOG(_L("SHOW PROGRESS"));
				if ( iDLProgressDlg )
					{
					FLOG(_L("Removing Connecting progress note..."));
					iDLProgressDlg->ProcessFinishedL();
					delete iDLProgressDlg;
					iDLProgressDlg = NULL;
					}
				if (!iDLProgressDlg)
					{
					
					FLOG(_L("Creating iDLProgressDlg"));
					iDLProgressDlg = new(ELeave) CFotaDLProgressDlg((CEikDialog** ) &iDLProgressDlg , EFalse, this, EDownloadingNote );
					iDLProgressDlg->PrepareLC( R_FOTASERVER_DL_PROGRESS_DIALOG );
					iDLProgressDlg->RunLD();
					}
				CEikProgressInfo* progressInfo = iDLProgressDlg->GetProgressInfoL();
				progressInfo->SetFinalValue( 500 );
				iPreviousContentSize = -1;
				iDownloadedContentSize = 0;
				iFotaServer->iParentApp->SetUIVisibleL ( ETrue , EFalse );
				}
			if (iDownloadResumable)
				{
				FLOG(_L("Setting download as resumable"));
				iDLState.iState = RFotaEngineSession::EDownloadProgressingWithResume;
		    	iFotaServer->SetStartupReason(EFotaDownloadInterrupted);
				}
			else
				{
				FLOG(_L("Setting download as non resumable"));
				iDLState.iState = RFotaEngineSession::EDownloadProgressing;
				iFotaServer->SetStartupReason(EFotaPendingGenAlert);
				}
			iFotaServer->iDatabase->OpenDBL();
			iFotaServer->iDatabase->SetStateL(iDLState,KNullDesC8, EFDBState );
			iFotaServer->iDatabase->CloseAndCommitDB();


			if (iDLState.iIapId <= 0) //Read the IAP ID when not known, and set it into db.
				{
				TInt32 usedapid (KErrNotFound);
				TInt er = iDownloadMgr.GetIntAttribute( EDlMgrIap, usedapid);
				iDLState.iIapId = usedapid;
				FLOG(_L("Setting IAPID=%d is FotaState"),iDLState.iIapId);
				iFotaServer->iDatabase->OpenDBL();
				iFotaServer->iDatabase->SetStateL(iDLState,KNullDesC8, EFDBIapId );
				iFotaServer->iDatabase->CloseAndCommitDB();
				}
			}
		}
		break;
	case EHttpDlCompleted:      // 4
		{
		FLOG(_L("Download State: EHttpDlCompleted"));

		iRestartCounter = -1;
		if ( iDLProgressDlg )
			{
			iDLProgressDlg->ProcessFinishedL(); // deletes itself
			delete iDLProgressDlg;
			iDLProgressDlg = NULL;
			}
		iDLState.iState     = RFotaEngineSession::EDownloadComplete;

		if (iUpdateAfterDownload)
			iDLState.iResult = KErrNotFound;
		else
			iDLState.iResult    = RFotaEngineSession::EResSuccessful;
		iFotaServer->FinalizeDownloadL( iDLState );
		}
		break;
	case EHttpDlPaused: //Event thrown for any resumable download
	case EHttpDlFailed: //Event thrown for any non-resumable download or critical error on resumable download
		{
		
		if (aEvent.iProgressState != EHttpProgNone) //Accepting only events from COD
			return;

		if (aEvent.iDownloadState == EHttpDlPaused)
			{
			FLOG(_L("Download State: EHttpDlPaused"));
			if (iDownloadResumable)
				{
				iDLState.iState     = RFotaEngineSession::EDownloadProgressingWithResume;
				}
			else 
				{
				// iDownloadResumable remains 0 even for resumable download.
				iDLState.iState     = RFotaEngineSession::EDownloadFailed;
				}
			}
		else //aEvent.iDownloadState = EHttpDlFailed
			{
			FLOG(_L("Download State: EHttpDlFailed"));
			iDLState.iState     = RFotaEngineSession::EDownloadFailed;
			iDownloadResumable = EFalse;
			}
		//Remove the download progress bar
		if ( iDLProgressDlg && (iDLState.iResult != RFotaEngineSession::EResUserCancelled ))
			{
			FLOG(_L("Shd not come in EResUserCancelled"));
			iDLProgressDlg->ProcessFinishedL(); // deletes itself
			delete iDLProgressDlg;
			iDLProgressDlg = NULL;
			}
		// This is restarted download => decrement counter
		if ( iRestartCounter > 0 )
			{
			--iRestartCounter;
			if ( iRestartCounter <=0 ) iRestartCounter = -1;
			FLOG(_L("   iRestartCounter to %d"),iRestartCounter );
			}
		/*****************************/
		//Handling all errors now...//
		/****************************/
		TInt notifType (KErrNotFound);
		TInt notifParam (KErrNotFound);
		//Handling User Cancel of Download Descriptor
		if ( dlErrorId == EGeneral && (sysErrorId == KErrAbort || sysErrorId == KErrCancel ))
			{
			FLOG(_L("Reason: User cancelled download descriptor"));
			//LaunchNotifierL(ESyncMLFwUpdOmaDLCancelled, EFalse, EFalse);
			iRestartCounter = -1;
			iDLState.iResult = RFotaEngineSession::EResUserCancelled;
			notifType = ESyncMLFwUpdOmaDLCancelled;
			notifParam = KErrNone;
			}
		//Handling User Cancel
		else if ( iDLState.iResult == RFotaEngineSession::EResUserCancelled ) //This variable is set in RunDownloadCancelL or RunDownloadSuspendL
			{
			iRestartCounter = -1;
			//If initial pause is successful, query user if he wants to postpone or cancel permanently.
			iFotaServer->iDatabase->OpenDBL();
			iFotaServer->iDatabase->SetStateL(iDLState,KNullDesC8, EFDBState);
			iFotaServer->iDatabase->CloseAndCommitDB();

			if (iDownloadResumable)
				{
				if (!iSilentOpn)
					{
					FLOG(_L("Download has been paused successful. Query user if he wants to postpone or cancel permanently"));
					LaunchNotifierL(ESyncMLFwUpdOmaDLUserCancel, KErrNone, KErrNone);
					break;
					}
				else
					{
					notifType = ESyncMLFwUpdOmaDLPostponed;
					notifParam = KErrNone;
					}
				}
			else 
				{
				FLOG(_L("Error while initial pausing...%d Hence download has to be cancelled permanently!"));
				RunDownloadCancelL(RFotaEngineSession::EResUserCancelled);
        notifType = ESyncMLFwUpdOmaDLCancelled;
        notifParam = KErrNone;
				}
			}
		else if ( iDLState.iResult == RFotaEngineSession::EResDLFailDueToDeviceOOM )
			{
			FLOG(_L("Reason: Not enough memory to perform resume"));
			iRestartCounter = -1;

			if (iDownloadResumable)
				iDLState.iState = RFotaEngineSession::EDownloadProgressingWithResume;
			else
				iDLState.iState = RFotaEngineSession::EDownloadFailed;
			}
		//Handling user cancel IAP selection popup
		else if ( dlErrorId == EConnectionFailed && sysErrorId == KErrCancel)
			{
			FLOG(_L("Reason: User canceled IAP selection popup"));
			iDLState.iResult = RFotaEngineSession::EResUserCancelled;
			iRestartCounter = -1;

			notifType = ESyncMLFwUpdOmaDLPostponed;
			notifParam = KErrNone;
			}
		//Handling temproary network timeout. This may either try resume or restart based on download.
/*		else if ( dlErrorId == EGeneral && sysErrorId == KErrTimedOut )
			{
			FLOG(_L("Reason: Network timeout"));
			// No restart issued yet, this is 1st download attempt
			if ( iRestartCounter == 0 )
				{
				iRestartCounter = KFotaDownloadTimeoutRestartCount;
				iDLState.iState     = RFotaEngineSession::EIdle;
				FLOG(_L("Trying to restart download (iRestartCounter=%d)"),iRestartCounter );
				}
			
			}*/
		//Handling all Network interrupts
		else if ( (dlErrorId == EConnectionFailed && sysErrorId == KErrCommsLineFail)|| 
				  dlErrorId == ETransactionFailed || 
				  (dlErrorId == KErrCodWapConnectionDropped && sysErrorId == KErrGeneral) ||
				  (dlErrorId == EGeneral && sysErrorId == KErrTimedOut) ||
				  (dlErrorId == EConnectionFailed && sysErrorId == KErrGprsServicesNotAllowed ) ||
				  (dlErrorId == EConnectionFailed && sysErrorId == KErrGsmMMNetworkFailure ) ||
				  (dlErrorId == EConnectionFailed && sysErrorId == KErrWlanNetworkNotFound )
				  )
			{
			FLOG(_L("Reason: Network breakage"));
			iRestartCounter = -1;
			notifType = ESyncMLFwUpdErrorNote;
			if (iDownloadResumable)
				{
				notifParam = KErrCommsLineFail2;
				iDLState.iState = RFotaEngineSession::EDownloadProgressingWithResume;
				iDLState.iResult = RFotaEngineSession::EResDLFailDueToNWIssues;
				}
			else
				{
				notifParam = KErrCommsLineFail;
				iDLState.iState = RFotaEngineSession::EDownloadFailed;
				iDLState.iResult = RFotaEngineSession::EResDLFailDueToNWIssues;
				}
			}
		//Handling Out Of Memory interrupt
		else if ((dlErrorId == EGeneral && sysErrorId == KErrDiskFull ) || sysErrorId == KErrCodInsufficientSpace)
			{
			FLOG(_L("Reason: Disk full"));
			iRestartCounter = -1;
			notifType = KErrNotFound;
			notifParam = KErrNotFound; // don't show anything
			iDLState.iResult = RFotaEngineSession::EResDLFailDueToDeviceOOM;
			}
		//Handle unclassified (general) interrupts
		else
			{
			iRestartCounter = -1;

			
			RProperty prop;
			TInt val = KErrNone;
		
			TInt err = prop.Get(KPSUidStartup, KPSGlobalSystemState, val);
			if (err==KErrNone && val!=ESwStateShuttingDown)
				{
				notifType = ESyncMLFwUpdErrorNote;				
				}
			else
				{
				FLOG(_L("Phone is powering down..."));
				}

			if (iDownloadResumable)
				{
				notifParam = KErrGeneralResume;
				iDLState.iState = RFotaEngineSession::EDownloadProgressingWithResume;
				}
			else
				{
				notifParam = KErrGeneralNoResume;
				iDLState.iState = RFotaEngineSession::EDownloadFailed;
				}


        SetDLResultdlErrorId(dlErrorId ,sysErrorId  );
			

			}
		if ( iRestartCounter <= 0)
			{
			iUpdateAfterDownload = EFalse;
			SetDownloadActive(EFalse);
			if ( iDLState.iSessionType!= ESanSilent && notifType!=KErrNotFound )
				{
				if ( iShowDLPrgUI )
					LaunchNotifierL( (TSyncMLFwUpdNoteTypes)notifType, notifParam,0 );
				else
					LaunchNotifierL( (TSyncMLFwUpdNoteTypes)notifType, notifParam,0, ETrue);
				}
			FLOG(_L("Should  come here for nonpausable "));	
			iDownloadFinalized = ETrue;
			iFotaServer->FinalizeDownloadL( iDLState );
			}
		else
			{
			if (iDownloadResumable)
				{
				FLOG(_L("Resuming download..."));
				iDownload->Start();
				}
			else
				{
				FLOG(_L("Restarting download..."));
				iFotaServer->FinalizeDownloadL( iDLState );
				}
			}
		}
		break;

	case EHttpDlPausable:
		{
		FLOG(_L("Download State: EHttpDlPausable"));
		if (iDownloadResumable) //This is initially set by IsDownloadSuspendResumeSupported()
			{
			FLOG(_L("Download Suspend/Resume is supported!"));       				
			}
		else
			{
			FLOG(_L("Download Server support Suspend/Resume, but Fota doesn't!"));
			//retain the state iDownloadResumable = EOmaDLResumeNotSupported;
			}
		}
		break;

	case EHttpDlNonPausable:
		{
		FLOG(_L("Download State: EHttpDlNonPausable"));
		if (iDownloadResumable) //This is initially set by CheckDownloadVariations()
			{
			FLOG(_L("Download Server doesn't support Suspend/Resume, but Fota does!"));
			//Reset the state to not supported.
			iDownloadResumable = EFalse;
			}
		else
			{
			FLOG(_L("Download Server doesn't support Suspend/Resume, neither does Fota!"));
			}
		}
		break;
	case EHttpDlDeleting:	//13
		{
		FLOG(_L("Download State: EHttpDlDeleting"));
		}
		break;
	case EHttpDlDeleted:	//11
		{
		FLOG(_L("Download State: EHttpDlDeleted"));
		}
		break;
	default:
		{
		FLOG(_L("Default. No action performed for this Download State"));
		}
		break;
	}
	FLOG(_L("CFotaDownload::HandleDMgrEventL()  <<"));
	}



// --------------------------------------------------------------------------
// CFotaDownload::SetDLResultdlErrorId
// Sets the appropriate error
// --------------------------------------------------------------------------
//

void CFotaDownload::SetDLResultdlErrorId( THttpDownloadMgrError adlErrorId, TInt32 asysErrorId)
{
      if ( adlErrorId == EInternal)
				{
				FLOG(_L("Reason:   error EInternal"));
				iDLState.iResult = RFotaEngineSession::EResUndefinedError;
				}
			else if ( adlErrorId == EContentFileIntegrity)
				{
				FLOG(_L("Reason:   error EContentFileIntegrity"));
				iDLState.iResult = RFotaEngineSession::EResUndefinedError;
				}
			else if ( adlErrorId == EMMCRemoved)
				{
				FLOG(_L("Reason:   error EMMCRemoved,"));
				iDLState.iResult = RFotaEngineSession::EResUndefinedError;
				}
			else if ( adlErrorId == EBadUrl) 
				{
				FLOG(_L("Reason:   error EBadUrl"));
				iDLState.iResult = RFotaEngineSession::EResMalformedOrBadURL;
				}
			else if ( adlErrorId == EHttpUnhandled)
				{
				FLOG(_L("Reason:   error EHttpUnhandled, check global error id!"));
				iDLState.iResult = RFotaEngineSession::EResUndefinedError;
				}
			else if ( adlErrorId == EHttpAuthenticationFailed)
				{
				FLOG(_L("Reason:   error EHttpAuthenticationFailed"));
				iDLState.iResult = RFotaEngineSession::EResUndefinedError;
				}
			else if ( adlErrorId == EObjectNotFound)
				{
				FLOG(_L("Reason:   error EObjectNotFound"));
				iDLState.iResult = RFotaEngineSession::EResAlternateDLServerUnavailable;
				}
			else if ( adlErrorId == EPartialContentModified)
				{
				FLOG(_L("Reason:   error EPartialContentModified"));
				iDLState.iResult = RFotaEngineSession::EResUndefinedError;
				}
			else if ( adlErrorId == EContentExpired)
				{
				FLOG(_L("Reason:   error EContentExpired"));
				iDLState.iResult = RFotaEngineSession::EResUndefinedError;
				}
			else if ( adlErrorId == EHttpRestartFailed)
				{
				FLOG(_L("Reason:   error EHttpRestartFailed"));
				iDLState.iResult = RFotaEngineSession::EResUndefinedError;
				}
			else if ( asysErrorId == KErrCodInvalidDescriptor )
				{
				FLOG(_L("Reason:   error KErrCodInvalidDescriptor"));
				iDLState.iResult  = RFotaEngineSession::EResMalformedOrBadURL;
				}
			else
				{
				FLOG(_L("Reason: unknown, check dlError!"));
				iDLState.iResult = RFotaEngineSession::EResUndefinedError;
				}

}

// --------------------------------------------------------------------------
// CFotaDownload::HandleDMgrEventL
// --------------------------------------------------------------------------
//
void CFotaDownload::HandleDMgrEventL( RHttpDownload& aDownload
		, THttpDownloadEvent aEvent )
	{   
	DoHandleDMgrEventL(&aDownload,aEvent);
	}


// --------------------------------------------------------------------------
// CFotaDownload::LaunchNotifierL
// Launches notifier. This method is also used to finalize erronous DL
// --------------------------------------------------------------------------
//
void CFotaDownload::LaunchNotifierL( const TSyncMLFwUpdNoteTypes aNotetype
		,const TInt aIntParam
		,const TInt aMemoryNeeded
		,TBool aFinishOnly )
	{
	if ( iNotifHandler ) iNotifHandler->Cancel();
	else				 iNotifHandler = CFotaDownloadNotifHandler::NewL(this);
	iNotifHandler->LaunchNotifierL( aNotetype,aIntParam,aMemoryNeeded
			,aFinishOnly );
	}

// --------------------------------------------------------------------------
// CFotaDownload::CheckDownloadVariations
// Reads download variations - fota suspend & resume bahavior & FMS from cenrep keys
// --------------------------------------------------------------------------
//
void CFotaDownload::CheckDownloadVariations()
	{
	FLOG(_L("CFotaDownload::CheckDownloadVariations >>"));

	CRepository* centrep( NULL);
	TRAPD(err, centrep = CRepository::NewL( KCRUidFotaServer ) );
	if (err) FLOG(_L("Error reading FotaServer cenrep... %d"),err);
	TInt supported(KErrNone);
	if (centrep ) 
		{
		err = centrep->Get( KSuspendResumeFeatureSupported, supported );
		if (err) FLOG(_L("Error reading cenrep key... %d"),err);
		iDownloadResumable = (supported==1)? ETrue:EFalse;

		supported = KErrNone;
		centrep->Get( KFotaMonitoryServiceEnabled, supported );
		if (err) FLOG(_L("Error reading cenrep key... %d"),err);
		iFMSEnabled = (supported > 0)? ETrue:EFalse;

		delete centrep;
		centrep = NULL;
		}

	FLOG(_L("Susp&Resume feature supported = %d, FMS feature supported = %d"),iDownloadResumable,iFMSEnabled);
	FLOG(_L("CFotaDownload::CheckDownloadVariations <<"));
	}

// --------------------------------------------------------------------------
// CFotaDownload::RunDownloadCancelL
// Starts to cancel the download operation
// --------------------------------------------------------------------------
//
void CFotaDownload::RunDownloadCancelL(const TInt aReason, TBool aSilent)
	{
	FLOG(_L("CFotaDownload::RunDownloadCancelL, reason = %d, silent = %d >>"), aReason, aSilent);

	iDLState.iState     = RFotaEngineSession::EDownloadFailed;
	iDLState.iResult    = aReason;
	iSilentOpn = aSilent;
	
	iRestartCounter = -1;
	iUpdateAfterDownload = EFalse;
	
	if (!iDownload)
		{
		//iDownload is null when user cancels the download permanently when prompted to resume.
		//hence retrieve the download object first 
		iDownload = RetrieveDownloadL();
		}
	TInt err = iDownload->Delete(); //or Cancel or Reset?
	FLOG(_L("Error = %d"),err);
	
	if(iDownloadFinalized == EFalse)
	{
	iFotaServer->FinalizeDownloadL( iDLState );
	FLOG(_L("Should not come here fro nopausable "));
	if (aReason == RFotaEngineSession::EResUserCancelled)
		LaunchNotifierL(ESyncMLFwUpdOmaDLCancelled, EFalse, EFalse);
	else if (aReason == RFotaEngineSession::EResUndefinedError)
		LaunchNotifierL(ESyncMLFwUpdErrorNote, KErrGeneralNoResume, EFalse );
  }
	FLOG(_L("CFotaDownload::RunDownloadCancelL <<"));
	}

// --------------------------------------------------------------------------
// CFotaDownload::RunDownloadSuspendL
// Starts to suspend the download operation
// --------------------------------------------------------------------------
//
void CFotaDownload::RunDownloadSuspendL(const TInt aReason, TBool aSilent)
	{
	FLOG(_L("CFotaDownload::RunDownloadSuspendL, reason = %d, silent = %d >>"), aReason, aSilent);

	iDLState.iState     = RFotaEngineSession::EDownloadProgressingWithResume;
	iDLState.iResult    = aReason;
	iSilentOpn = aSilent;
	//First pause download, and later ask for user action.
	TInt err = iDownload->Pause();

	if (err)
	{
	//initial pausing becomes a problem.
	FLOG(_L("Error while initial pausing...%d Hence download has to be cancelled permanently!"),err);
	RunDownloadCancelL(aReason);
	}

	FLOG(_L("CFotaDownload::RunDownloadSuspendL <<"));
	}

// --------------------------------------------------------------------------
// CFotaDownload::TryResumeDownloadL
// Tries to resume the download operation
// --------------------------------------------------------------------------
//
void CFotaDownload::TryResumeDownloadL(TBool aUserInitiated)
	{
	FLOG(_L("CFotaDownload::TryResumeDownloadL >>"));

	iUserInitiatedResume = aUserInitiated;
	TInt err = RProperty::Define( TUid::Uid(KOmaDMAppUid),
			KFotaDMRefresh,
			RProperty::EInt,KReadPolicy,KWritePolicy);
	FLOG(_L("RProperty KFotaDMRefresh Define, err = %d"), err);        
	if (err != KErrAlreadyExists )
		{
		User::LeaveIfError(err);
		}
	//This P&S Key is used to notify DM UI on any download event. key=0 for idle and key=1 for download
	err = RProperty::Set( TUid::Uid(KOmaDMAppUid),
			KFotaDMRefresh,
			ETrue );
	FLOG(_L("RProperty KFotaDMRefresh Set ETrue, err = %d"), err);    

	//Called to read the download variations in Fota (resume support & FMS)
	CheckDownloadVariations();	
	if (IsFMSEnabled() && iUserInitiatedResume)
		iFotaServer->CancelFmsL();

	//Query based on the type of DM session used earlier.
	if (iUserInitiatedResume || iDLState.iSessionType != ESanSilent)
		{
		//Query user for resume
		FLOG(_L("Quering user for resume..."));
		if (iFotaServer->iNotifHandler)
			{
			iFotaServer->iNotifHandler->Cancel();
			delete iFotaServer->iNotifHandler;
			iFotaServer->iNotifHandler = NULL;
			}
		iNotifHandler = CFotaDownloadNotifHandler::NewL(this);

		iNotifHandler->LaunchNotifierL(ESyncMLFwUpdOmaDLResume, EFalse, EFalse);
		}
	else //Only FMS initiated Silent Session download
		{
		//Resume directly
		FLOG(_L("Resuming automatically..."));
		ResumeDownloadL();
		}
	FLOG(_L("CFotaDownload::TryResumeDownloadL <<"));
	}

// --------------------------------------------------------------------------
// CFotaDownload::ResumeDownloadL
// Resume the download operation
// --------------------------------------------------------------------------
//
void CFotaDownload::ResumeDownloadL()
	{
	FLOG(_L("CFotaDownload::ResumeDownloadL >>"));
	TBool toresetdl (EFalse);
	TBool downloadiapvalid (ETrue);
	//Find the download ui behavior from fota cenrep
	TInt behavior(KErrNotFound);
	CRepository* centrep = NULL;
	TRAPD(err, centrep = CRepository::NewL( KCRUidFotaServer ) );
	if (( centrep ) && ( err == KErrNone))
	    {
	    centrep->Get( KSilentOmaDlUIBehavior, behavior );
	    delete centrep;
	    }
	centrep = NULL;

	if (iUserInitiatedResume || 
	        (behavior == EOmacdOffdlprgOn || behavior == EOmacdOndlprgOn))
		{
		FLOG(_L("Showing Connecting progress note..."));
		iFotaServer->iParentApp->SetUIVisibleL ( ETrue , EFalse );
		
		if ( !iDLProgressDlg )
			{
			iDLProgressDlg = new(ELeave) CFotaDLProgressDlg((CEikDialog** ) &iDLProgressDlg , EFalse, this, EConnectingNote );
			iDLProgressDlg->PrepareLC( R_CONNECTION_NOTE );
			}
		// Load the resource files
	    TInt err = KErrNone;
	       
	    // Localize the file name, and load the SCP resources
	    TFileName resFile;
	    resFile.Copy( KDriveZ );
	    resFile.Append( KSCPResourceFilename );
    	    
	    BaflUtils::NearestLanguageFile( CCoeEnv::Static()->FsSession(), resFile );    

	    TInt Res1(KErrNone);
	    TRAP( err, Res1 =CCoeEnv::Static()->AddResourceFileL( resFile ) );        

	    HBufC* buf = StringLoader::LoadLC(R_SYNCSTATUS_SYNC_CONN);
	
	    if( Res1 )
	    	{
	        iFotaServer->GetEikEnv()->DeleteResourceFile( Res1 );
	    	}
	    FLOG(_L("Reading txt as: %S"), &buf->Des());
		iDLProgressDlg->SetTextL(buf->Des());
		CleanupStack::PopAndDestroy(buf);
	
		iDLProgressDlg->RunLD();
		}
	FLOG(_L("CFotaDownload::ResumeDownloadL, Stage 1"));
	
	iDownload = RetrieveDownloadL();
	if (iDownload)
		{
		FLOG(_L("CFotaDownload::ResumeDownloadL, Stage 2: Successfully retrieved download object >>"));

		SetIapToUseL(iDLState, iDLState.iIapId);

		FLOG(_L("CFotaDownload::ResumeDownloadL, Stage 3: Using iapid %d for resume..."), iDLState.iIapId);
		downloadiapvalid = iFotaServer->CheckIapExistsL(iDLState.iIapId);
		if (iDLState.iIapId > 0 && downloadiapvalid)
			{
			if (!iMonitor)
				{
				iMonitor = CFotaNetworkRegStatus::NewL (iFotaServer);
				}
			TBool val = iMonitor->IsConnectionPossibleL(iDLState.iIapId);
			FLOG(_L("CFotaDownload::ResumeDownloadL, Stage 4"));
			delete iMonitor; iMonitor = NULL;
			
#if defined(__WINS__)
			val =ETrue;
#endif
			if (!val)
				{
				FLOG(_L("CFotaDownload::ResumeDownloadL, Stage 5.1: Disallowing resume operation as connection not possible"));
				
				if ( !iShowDLPrgUI && iDLProgressDlg )
					{
					FLOG(_L("Removing Connecting progress note..."));

					iDLProgressDlg->ProcessFinishedL();
					delete iDLProgressDlg;
					iDLProgressDlg = NULL;
					}
				if (iUserInitiatedResume || iDLState.iSessionType!=ESanSilent)
					LaunchNotifierL(ESyncMLFwUpdErrorNote, KErrCommsLineFail2,0);
				iDLState.iState = RFotaEngineSession::EDownloadProgressingWithResume;
				iDLState.iResult = RFotaEngineSession::EResDLFailDueToNWIssues;
				iUpdateAfterDownload = EFalse;
				iFotaServer->FinalizeDownloadL(iDLState);
				return;
				}
			}

		if (iDLState.iIapId == KErrNotFound || !downloadiapvalid )
		    {
		    iDLState.iIapId = KErrNotFound;
			User::LeaveIfError( iDownloadMgr.SetIntAttribute( EDlMgrIap, 0)); // Download mgr interprets 0 for Always Ask
		    }
		else
			User::LeaveIfError( iDownloadMgr.SetIntAttribute( EDlMgrIap, iDLState.iIapId)); // IAP id

		FLOG(_L("CFotaDownload::ResumeDownloadL, Stage 5.2"));
		SetDownloadUiBehavior(EFalse);		

		if ( !iShowDLPrgUI && iDLProgressDlg )
			{
			FLOG(_L("Removing Connecting progress note..."));

			iDLProgressDlg->ProcessFinishedL();
			delete iDLProgressDlg;
			iDLProgressDlg = NULL;
			}
		TInt dlsize, tlsize;
		FLOG(_L("CFotaDownload::ResumeDownloadL, Stage 6"));
		iFotaServer->GetDownloadUpdatePackageSizeL(iDLState.iPkgId, dlsize,tlsize);
		iDownloadedContentSize = dlsize; //for progress bar update only
		iUpdateAfterDownload = iDLState.iUpdateLtr; //should update after reboot?
		iRestartCounter = 0;
		
		//Set right states in fota db
		iDLState.iState = RFotaEngineSession::EDownloadProgressingWithResume;
		iDLState.iResult = KErrNotFound;
		
		iFotaServer->iDatabase->OpenDBL();
		iFotaServer->iDatabase->SetStateL( iDLState, KNullDesC8, EFDBState|EFDBResult);
		iFotaServer->iDatabase->CloseAndCommitDB();
		iFreshDL = EFalse;
		FLOG(_L("CFotaDownload::ResumeDownloadL, Stage 6: Starting to resume download now..."));
		TInt err = iDownload->Start();
		if (err == KErrNone)
			{
			//Set the P&S Key to active...
			FLOG(_L("Resume operation is success!"));
			SetDownloadActive(ETrue);
			}
		else
			{
			FLOG(_L("An error occured during resume, err = %d"),err);
			toresetdl = ETrue;
			}
		}
	else
		{
		FLOG(_L("Failed to get the paused download object"));
		if ( iDLProgressDlg )
			{
			FLOG(_L("Removing Connecting progress note..."));
			iDLProgressDlg->ProcessFinishedL(); // deletes itself
			delete iDLProgressDlg;
			iDLProgressDlg = NULL;
			}
		toresetdl = ETrue;
		}
      ReSetDownloadL(	toresetdl);
	
	FLOG(_L("CFotaDownload::ResumeDownloadL <<"));
	}
// --------------------------------------------------------------------------
// CFotaDownload::ReSetDownloadL
// Sets the download state to failed
// --------------------------------------------------------------------------
//
void CFotaDownload::ReSetDownloadL(TBool atoresetdl)
{
if (atoresetdl)
		{
		FLOG(_L("Resetting fota download !!!"));
		if (iUserInitiatedResume || iDLState.iSessionType!=ESanSilent)
			LaunchNotifierL(ESyncMLFwUpdErrorNote, KErrGeneralNoResume,0);
		iDLState.iState = RFotaEngineSession::EDownloadFailed;
		iDLState.iResult = RFotaEngineSession::EResUndefinedError;
		iUpdateAfterDownload = EFalse;
		iFotaServer->FinalizeDownloadL(iDLState);
		}
}



// --------------------------------------------------------------------------
// CFotaDownload::IsFMSEnabled
// Returns FMS enabled state
// --------------------------------------------------------------------------
//
TBool CFotaDownload::IsFMSEnabled()
	{
	FLOG(_L("CFotaDownload::IsFMSEnabled, iFMSEnabled = %d <<"), iFMSEnabled);
	return iFMSEnabled;
	}

// --------------------------------------------------------------------------
// CFotaDownload::IsDownloadResumable
// Returns whether the download is resumable or not.
// --------------------------------------------------------------------------
//
TBool CFotaDownload::IsDownloadResumable()
	{
	FLOG(_L("CFotaDownload::IsDownloadResumable, iDownloadResumable = %d <<"), iDownloadResumable);
	return iDownloadResumable;
	}

// --------------------------------------------------------------------------
// CFotaDownload::SetIapToUseL
// Sets the IAP ID to use. This menthod is used in fresh and resume download.
// --------------------------------------------------------------------------
//
void CFotaDownload::SetIapToUseL(TPackageState aParams, const TInt aIapid)
	{
	FLOG(_L("CFotaDownload::SetIapToUseL >>"));

	RSyncMLSession sml;
	TInt profIapid( KErrNotFound );
	TInt sockIapid( KErrNotFound );
	TSmlServerAlertedAction saa(ESmlConfirmSync);

	CleanupClosePushL( sml );
	sml.OpenL();
	iFotaServer->GetProfileDataL(&sml,aParams.iProfileId, profIapid, saa
			,iIsjobStartedFromUI, iSanValue );
	CleanupStack::PopAndDestroy( &sml );

	// 2/3 GET IAP FROM ESOCK ----------------------------------------------
	// resolve which accespoint is used for current DM session
	if ( (profIapid == KErrNotFound || profIapid == KErrGeneral)  
			&& aIapid==KErrNotFound)
		{
		RSocketServ serv;
		CleanupClosePushL( serv );
		User::LeaveIfError( serv.Connect() );

		RConnection conn;
		CleanupClosePushL( conn );
		User::LeaveIfError( conn.Open( serv ) );

		TUint count( 0 );
		User::LeaveIfError( conn.EnumerateConnections ( count ) );
		// enumerate connections
		for( TUint idx=1; idx<=count; ++idx )
			{
			TConnectionInfo connectionInfo;
			TConnectionInfoBuf connInfo( connectionInfo );

			TInt err = conn.GetConnectionInfo( idx, connInfo ); // iapid
			if( err != KErrNone )
				{
				CleanupStack::PopAndDestroy( 2 ); // conn, serv
				User::Leave( err );
				}
			// enumerate connectionclients
			TConnectionEnumArg	conArg;
			conArg.iIndex = idx;
			TConnEnumArgBuf conArgBuf(conArg);
			err=conn.Control(KCOLConnection,KCoEnumerateConnectionClients
					,conArgBuf);
			if( err != KErrNone )
				{
				CleanupStack::PopAndDestroy( 2 ); // conn, serv
				User::Leave( err );
				}
			TInt cliCount = conArgBuf().iCount;
			for ( TUint j=1; j<=cliCount ;++j )
				{
				TConnectionGetClientInfoArg conCliInfo;
				conCliInfo.iIndex = j;
				TConnGetClientInfoArgBuf conCliInfoBuf(conCliInfo);
				err=conn.Control(KCOLConnection, KCoGetConnectionClientInfo
						, conCliInfoBuf);

				if( err != KErrNone )
					{
					CleanupStack::PopAndDestroy( 2 ); // conn, serv
					User::Leave( err );
					}				
				TConnectionClientInfo conCliInf = conCliInfoBuf().iClientInfo;
				TUid uid = conCliInf.iUid;
				if ( uid == TUid::Uid(KSosServerUid) )
					{
					sockIapid = connInfo().iIapId;
					FLOG(_L("[FotaServer]  IAP found %x"),sockIapid);
					}

				FLOG(_L("[FotaServer]  CFotaDownload::DownloadL uid %x")
						,uid.iUid);
				}
			}
		CleanupStack::PopAndDestroy( 2 ); // conn, serv		
		}
	TInt newIapid( KErrNotFound );
	if ( sockIapid!=KErrNotFound  ) newIapid = sockIapid;
	if ( profIapid!=KErrNotFound && profIapid != KErrGeneral ) newIapid = profIapid;
	if ( aIapid   !=KErrNotFound ) newIapid = aIapid;

	//Do some intelligent work during resume
	/* Some rules..
	 * 1. If IAP is always ask in DM profile, use the earlier set during FMS trigger
	 * 2. If IAP is always ask in DM profile, query user for IAP during user trigger
	 * 3. If IAP has changed in DM profile after suspend, use the newer one during resume
	 * 4. 
	 */
	TInt active (KErrNotFound);
	RProperty::Get(TUid::Uid(KFotaServerUid), KFotaDownloadActive, active );
	FLOG(_L("active = %d"),active);
	if (active==EFalse && iDLState.iState == RFotaEngineSession::EDownloadProgressingWithResume)
		{
		//Actual resume only
		if (!iUserInitiatedResume && (profIapid == KErrNotFound || profIapid == KErrGeneral  ) )
			{
			//FMS triggered resume, apply Rule 1
			newIapid = iDLState.iIapId;
			}
		else if (profIapid == KErrNotFound || profIapid == KErrGeneral )
			{
			//User triggered resume, apply Rule 2
			newIapid = KErrNotFound;
			}
		else if (profIapid != iDLState.iIapId && profIapid > KErrNone)
			{
			//IAP has changed in DM profile, apply Rule 3
			newIapid = profIapid;
			}
		}
	iDLState.iIapId = newIapid;


	FLOG(_L("CFotaDownload::SetIapToUseL, iap = %d <<"), iDLState.iIapId);
	}

// --------------------------------------------------------------------------
// CFotaDownload::SetDownloadUiBehavior
// Sets Download UI behavior. ex: DD display and Progress note
// --------------------------------------------------------------------------
//
void CFotaDownload::SetDownloadUiBehavior(TBool aFreshDL)
	{
	FLOG(_L("CFotaDownload::SetDownloadUiBehavior, aFreshDL = %d >>"),aFreshDL);

	if (aFreshDL) //Fresh download
		{
		iDLState.iSessionType = iSanValue;
		}
	else //resuming download
		{
		iSanValue = iDLState.iSessionType;
		}

	if ( !iIsjobStartedFromUI &&  iRestartCounter==0 ) //iRestartCounter to ensure that the UI behavior is read only once.
		{
		TInt behavior(KErrNotFound);
		TInt sansupported(KErrNone);

		//Find whether SAN is supported or not
	    CRepository* centrep = NULL;

	    TRAPD( err, centrep = CRepository::NewL( KCRUidDeviceManagementInternalKeys) );    
	    if( centrep ) centrep->Get( KDevManSANUIBitVariation, sansupported );
	    delete centrep; centrep = NULL;
	    FLOG(_L("...1..."));
	    if (sansupported != 1) sansupported = 0;
	    
		//Find the download ui behavior from fota cenrep
		TRAP(err, centrep = CRepository::NewL( KCRUidFotaServer ) );
		if ( centrep ) 	centrep->Get( KSilentOmaDlUIBehavior, behavior );
		delete centrep; centrep = NULL;

		FLOG(_L("Values are... SAN enabled: %d, session type: %d, downloadbehavior: %d"),sansupported, iDLState.iSessionType,behavior);

		if (sansupported) //SAN is supported
			{
				
			DetermineUISANON(behavior)	;
			
			}
		else //SAN not supported
			{
				DetermineUISANOFF(behavior);
		
			}
		}
	else// if (iRestartCounter==0) //Started from DM UI, but not retry
		{
		iShowOMACDUI = ETrue;
		iShowDLPrgUI = ETrue;
		if (aFreshDL)
			iDLState.iSessionType = KErrNotFound;
		}

TInt value (EFotaUpdateDM);
TInt err = RProperty::Get( KPSUidNSmlDMSyncApp, KNSmlCurrentFotaUpdateAppName, value );
if (!err && ((value == EFotaUpdateNSC) || (value == EFotaUpdateNSCBg)))
{
			iShowOMACDUI = EFalse;		
			iShowDLPrgUI = ETrue;
			FLOG(_L("CFotaDownload::DownloadL UI will not be shown!"));
}
	FLOG(_L("UI Behavior: OMA DD Display = %d, Download Progress = %d"),iShowOMACDUI,iShowDLPrgUI);

	FLOG(_L("CFotaDownload::SetDownloadUiBehavior <<"));
	}


// --------------------------------------------------------------------------
// CFotaDownload::DetermineUISANON
// Determine UI when SAN is ON
// --------------------------------------------------------------------------
//
void CFotaDownload::DetermineUISANON(TInt aBehavior)
{
				switch (aBehavior)
					{
					case EOmacdOffdlprgOff: //0
						{
						iShowOMACDUI = EFalse;
						iShowDLPrgUI = EFalse;
						}
						break;
					case EOmacdOffdlprgOn: //1
						{
						iShowOMACDUI = EFalse;
						iShowDLPrgUI = ETrue;
						}
						break;
					case EOmacdOndlprgOff: //2
						{
						iShowOMACDUI = ETrue;
						iShowDLPrgUI = EFalse;
						}
						break;
					case EOmacdOndlprgOn: //3
						{
						iShowOMACDUI = ETrue;
						iShowDLPrgUI = ETrue;
						}
						break;
					default: //includes EChooseFromSan
						{
						if(iSanValue == ESanSilent)
							{
							iShowOMACDUI = EFalse;
							iShowDLPrgUI = EFalse;
							}
						else if (iSanValue == ESanInformative)
							{
							iShowOMACDUI = EFalse;
							iShowDLPrgUI = ETrue;
							}
						else //includes ESanInteractive, ESanNotSpecified
							{
							iShowOMACDUI = ETrue;
							iShowDLPrgUI = ETrue;
							}
						}
						break;
					}
	
	
}

// --------------------------------------------------------------------------
// CFotaDownload::DetermineUISANOFF
// Determine UI when SAN is OFF
// --------------------------------------------------------------------------
//

void CFotaDownload::DetermineUISANOFF(TInt aBehavior)
{
			if (iSanValue==EAcceptedYes) //Accepted is Yes in Profile
				{
				switch (aBehavior)
					{
					case EOmacdOffdlprgOff: //0
						{
						iShowOMACDUI = EFalse;
						iShowDLPrgUI = EFalse;
						}
						break;
					case EOmacdOffdlprgOn: //1
						{
						iShowOMACDUI = EFalse;
						iShowDLPrgUI = ETrue;
						}
						break;
					case EOmacdOndlprgOff: //2
						{
						iShowOMACDUI = ETrue;
						iShowDLPrgUI = EFalse;
						}
						break;
					default: //includes EOmacdOndlprgOn
						{
						iShowOMACDUI = ETrue;
						iShowDLPrgUI = ETrue;
						}
						break;
					}
				}
			else // includes iSanValue = EAcceptedNo (Accepted is No in profile)
				{
				iShowOMACDUI = ETrue;
				iShowDLPrgUI = ETrue;
				}
}







// --------------------------------------------------------------------------
// CFotaDownload::RetrieveDownloadL
// Retrieves the RHttpDownload object from download manager during resume
// --------------------------------------------------------------------------
//
RHttpDownload* CFotaDownload::RetrieveDownloadL()
	{
	FLOG(_L("CFotaDownload::RetrieveDownloadL >>"));
	//Retrieve RHttpDownload object for the suspended download from Download Mgr.
	if (!iDownloadMgr.Handle())
		iDownloadMgr.ConnectL( TUid::Uid(KFotaServerUid) , *this, EFalse);
	const CDownloadArray& downloads = iDownloadMgr.CurrentDownloads();
	
	TBuf8<KMaxPath> url8;
	TInt32 val (KErrNone);
	
	RHttpDownload* aDownload (NULL);
	for (TInt i = 0; i < downloads.Count(); i++ )
		{
		User::LeaveIfError( downloads[i]->GetStringAttribute( EDlAttrCurrentUrl, url8 ));
		User::LeaveIfError( downloads[i]->GetIntAttribute( EDlAttrFotaPckgId, val));
	
		FLOG(_L("Current download(s) as per download mgr: %d, url = %S, pkgid = %d"), i+1, &url8, val);
	
		if (iDLState.iPkgId ==  val)
			{
			FLOG(_L("Successfully found the paused download object"));
			aDownload = downloads[i];
			if ( iUrl ) {delete iUrl; iUrl=NULL;}
			iUrl = url8.Alloc();
	
			}
		}
	FLOG(_L("CFotaDownload::RetrieveDownloadL <<"));
	return aDownload;
	}

// --------------------------------------------------------------------------
// CFotaDownload::SetDownloadActive
// Sets the download activity state P&S key
// --------------------------------------------------------------------------
//
void CFotaDownload::SetDownloadActive(TBool aValue)
	{
	FLOG(_L("CFotaDownload::SetDownloadActive, aValue = %d"),aValue);
	iDownloadActive = aValue;
	TBool val (EFalse);
	TInt err = RProperty::Get(TUid::Uid(KFotaServerUid), KFotaDownloadActive, val );

	if (err == KErrNone && val != aValue)
		{
		err = RProperty::Set(TUid::Uid(KFotaServerUid), KFotaDownloadActive, aValue );
		FLOG(_L("RProperty KFotaDownloadActive Set %d, err = %d"), aValue, err);
		}
	
	FLOG(_L("CFotaDownload::SetDownloadActive <<"));
	}

// --------------------------------------------------------------------------
// IsDownloadActive
// Returns the activeness of the download
// --------------------------------------------------------------------------
//

TBool CFotaDownload::IsDownloadActive()
	{
	FLOG(_L("CFotaDownload::IsDownloadActive, value = %d"),iDownloadActive);
	return iDownloadActive;
	}


// --------------------------------------------------------------------------
// CFotaDownloadNotifHandler::NewL 
// --------------------------------------------------------------------------
//
CFotaDownloadNotifHandler* CFotaDownloadNotifHandler::NewL (CFotaDownload* aDownload)
	{
	FLOG(_L("CFotaDownloadNotifHandler::NewL"));
	__ASSERT_ALWAYS( aDownload, User::Panic(KFotaPanic, KErrArgument) );	
	CFotaDownloadNotifHandler* h=new (ELeave)CFotaDownloadNotifHandler;
	h->iDownload = aDownload;
	h->iDownload->iFotaServer->iNotifHandler = h;
	return h;
	}

// --------------------------------------------------------------------------
CFotaDownloadNotifHandler::CFotaDownloadNotifHandler() : CActive(EPriorityNormal)
	{
	FLOG(_L("CFotaDownloadNotifHandler::CFotaDownloadNotifHandler()"));
	CActiveScheduler::Add( this ); 
	}

// --------------------------------------------------------------------------
CFotaDownloadNotifHandler::~CFotaDownloadNotifHandler()
	{
	FLOG(_L("CFotaDownloadNotifHandler::~CFotaDownloadNotifHandler >>"));
	iNotifier.Close();
	Cancel();
	FLOG(_L("CFotaDownloadNotifHandler::~CFotaDownloadNotifHandler <<"));
	}

// --------------------------------------------------------------------------
void CFotaDownloadNotifHandler::RunL()
	{
	FLOG(_L("CFotaDownloadNotifHandler::RunL() note:%d, param:%d" ),iNotifParams.iNoteType, iNotifParams.iIntParam);

	TBool reset (ETrue);

	iNotifier.Close();

	iDownload->SetDownloadActive(EFalse);
	if ( iNotifParams.iNoteType == ESyncMLFwUpdErrorNote  ) // 1 
		{
		//do nothing here
		}
	if ( iNotifParams.iNoteType == ESyncMLFwUpdOmaDLNotEnoughMemory ) // 7
		{
		//To be active as Flexible mem is in action.
		if (iDownload->iFreshDL)
			iDownload->SetDownloadActive(ETrue);
		}
	if ( iNotifParams.iNoteType == ESyncMLFwUpdOmaDLUserCancel )
		{
		//At this point Download Mgr has already paused the download successfully.
		if (iStatus.Int() == KErrCancel) //RSK Press
			{
			//User wants to cancel download permanently.
			FLOG(_L("User choose to cancel download permanently!"));
			iDownload->RunDownloadCancelL(RFotaEngineSession::EResUserCancelled);
			}
		else
			{
			//Other keys pressed, like LSK or Call termination key
			FLOG(_L("User choose to resume download later, key press = %d"),iStatus.Int());
			iDownload->iFotaServer->FinalizeDownloadL( iDownload->iDLState );
			if (iStatus.Int() == KErrNone) //RSK is pressed
				{
				LaunchNotifierL(ESyncMLFwUpdOmaDLPostponed, EFalse, EFalse);
				reset = EFalse;
				}
			else
				iDownload->iFotaServer->ShutApp();
			}
		}

	if ( iNotifParams.iNoteType == ESyncMLFwUpdOmaDLPostponed )
		{
		//Nothing to do
		}

	if ( iNotifParams.iNoteType == ESyncMLFwUpdOmaDLCancelled )
		{
		//Nothing to do
		}

	if ( iNotifParams.iNoteType == ESyncMLFwUpdOmaDLResume )
		{
		if (iStatus.Int() == KErrNone) // LSK Press
			{
			FLOG(_L("User choose to continue download now."));
			iNotifParams.iNoteType = ESyncMLFwUpdUnknown;
			iNotifParams.iIntParam = 0;
			iDownload->ResumeDownloadL();
			reset = EFalse;
			}
		else	//RSK or any other key
			{
			FLOG(_L("User cancelled to continue download now, key press = %d."),iStatus.Int());
			if (iStatus.Int() == KErrCancel) //RSK is pressed
				{
				iDownload->iDLState.iResult = RFotaEngineSession::EResUserCancelled;
				LaunchNotifierL(ESyncMLFwUpdOmaDLUserCancel, EFalse, EFalse);
				reset = EFalse;
				}
			else
				iDownload->iFotaServer->ShutApp();
			}
		}

	if (reset)
		{
		iNotifParams.iNoteType = ESyncMLFwUpdUnknown;
		iNotifParams.iIntParam = 0;
		}
	}

// --------------------------------------------------------------------------
TInt CFotaDownloadNotifHandler::RunError(TInt aError)
	{	
	FLOG(_L("CFotaDownloadNotifHandler::RunError %d"),aError);
	return aError;
	}

// --------------------------------------------------------------------------
void CFotaDownloadNotifHandler::DoCancel()
	{
	FLOG(_L("CFotaDownloadNotifHandler::DoCancel >>"));
	if (iNotifier.Handle())
		{
		iNotifier.CancelNotifier( KSyncMLFwUpdNotifierUid );
		iNotifier.Close();
		}
		FLOG(_L("CFotaDownloadNotifHandler::DoCancel <<"));
	}

// --------------------------------------------------------------------------
void CFotaDownloadNotifHandler::LaunchNotifierL( const TSyncMLFwUpdNoteTypes aNotetype
		,const TInt aIntParam
		, const TInt aMemoryNeeded
		,TBool aFinishOnly)
	{
	FLOG(_L("CFotaDownloadNotifHandler::LaunchNotifierL, aNoteType=%d, aParam=%d, aMemoryNeeded=%d aFinishOnly=%d >>  "), aNotetype, aIntParam, aMemoryNeeded, aFinishOnly?1:0);
	
	__ASSERT_ALWAYS( iNotifier.Handle()==0, User::Leave(KErrAlreadyExists) );
	__ASSERT_ALWAYS( aMemoryNeeded>=0, User::Leave(KErrArgument) );


	// Launch notifier
	TSyncMLFwUpdNotifParams         params;
	FLOG(_L("           1"));
	params.iNoteType        = aNotetype;
	params.iIntParam        = aIntParam;

	TInt rem = 0;
	rem = aMemoryNeeded % 1024 ;
	if (rem)
		{
		params.iMemoryNeeded = (aMemoryNeeded /1024) + 1;   	
		}
	else
		{  
		params.iMemoryNeeded = (aMemoryNeeded /1024) ;
		}

	//    #endif    
	TSyncMLFwUpdNotifParamsPckg     pckg(params);
	iNotifParams.iNoteType  = params.iNoteType;
	iNotifParams.iIntParam  = params.iIntParam;

	iNotifParams.iMemoryNeeded = params.iMemoryNeeded;

	iDummyResponsePckg = TSyncMLFwUpdNotifRetValPckg();
	FLOG(_L("           2"));
	if ( !aFinishOnly )
		{
		User::LeaveIfError( iNotifier.Connect() );
		iNotifier.StartNotifierAndGetResponse( iStatus, KSyncMLFwUpdNotifierUid
				, pckg, iDummyResponsePckg );
		SetActive();
		}
	else
		{
		iStatus = KRequestPending;
		SetActive();
		TRequestStatus* status = &iStatus;
		User::RequestComplete( status, KErrNone );
		}
	FLOG(_L("           3  %d"),iStatus.Int());
	FLOG(_L("CFotaDownloadNotifHandler::LaunchNotifierL() <<"));	
	}

TBool CFotaDownloadNotifHandler::IsOpen()
	{
	FLOG(_L("Handle = %d"), iNotifier.Handle());
	return (iNotifier.Handle())? ETrue:EFalse;
	}
