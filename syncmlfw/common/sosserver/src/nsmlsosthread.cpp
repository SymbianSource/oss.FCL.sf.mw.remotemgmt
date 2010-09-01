/*
* Copyright (c) 2002-2005 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Symbian OS Server source.
*
*/

#include <e32math.h>
#include <DevManInternalCRKeys.h>
#include <centralrepository.h>
#include <nsmldebug.h>

#include "nsmlsosthread.h"
#include <DataSyncInternalPSKeys.h>

#include <PolicyEngineClient.h>
#include <centralrepository.h>
#include "PMUtilInternalCRKeys.h"
#include <featmgr.h>
// --------------------------------------------------------------------------
// EXPORT_C TInt ThreadFunction( TAny* )
// --------------------------------------------------------------------------
//
TInt ThreadFunction( TAny* aStarted )
	{
	#ifdef __CLIENT_API_MT_
	 __UHEAP_MARK;
	#endif
	
	CNSmlThreadParams* params = (CNSmlThreadParams*) aStarted;
	
	CTrapCleanup* cleanup = CTrapCleanup::New();
	_DBG_FILE("CNSmlSOSServer : Session thread started");	
	
	CActiveScheduler* scheduler = new CActiveScheduler;
	__ASSERT_ALWAYS( scheduler, User::Panic( KSOSServerThread, EMainSchedulerError ) );
    	
	CActiveScheduler::Install( scheduler );
	
	TInt endStatus(KErrNone);
	CNSmlThreadEngine * threadEngine = 0;
	
	params->iCurrentJob.iSilentJob = EFalse;

	TRAPD(err,
		threadEngine = CNSmlThreadEngine::NewL( *params );
	
	if ( params->iSyncInit == EServerAlerted && params->iCurrentJob.iPending )
		{
		if ( threadEngine->SilentModeL( params ))
			{
			//in silent mode DM client UI is not opened
			params->iCurrentJob.iPending = EFalse;
		    params->iCurrentJob.iSilentJob = ETrue;
			threadEngine->SaveSessionInfoL();
			threadEngine->ForcedCertificateCheckL( ETrue );
			threadEngine->StartJobSessionL();		
			}
		else
			{
			//if not a silent mode behave as in a normal DM session
			threadEngine->ForcedCertificateCheckL( EFalse );
			threadEngine->VerifyJobFromNotifierL();		
			}
		}
	else
		{
		//when sync session is not alerted behave as in a normal DM session
		threadEngine->ForcedCertificateCheckL( EFalse );
		threadEngine->StartJobSessionL();		
		}
	);
	
	endStatus = err;
    
    if ( err != KErrNone )    
 		{
		if ( params->iCurrentJob.TransportId() != KUidNSmlMediumTypeInternet.iUid )
			{
			// Disconnect if local connection
			TRAP_IGNORE( params->iSOSHandler->DoDisconnectL() );		
			}		
		}	
	
	// start scheduler after succesfull initialization 
	if ( threadEngine )
	    {
	    if ( threadEngine->IsActive() )
	        {
	        CActiveScheduler::Start();                
	        }	    
	    }	
	
	delete scheduler;
	
	if ( endStatus == KErrNone )
		{
		endStatus = threadEngine->FinishedStatus();	
		}
	
	if ( threadEngine )	
	    {
	    delete threadEngine;        
	    }
	
	_DBG_FILE("SosServer Job Thread ended");
	delete cleanup;
	
	#ifdef __CLIENT_API_MT_
	__UHEAP_MARKEND;
	#endif
	
    return endStatus;
	}

// --------------------------------------------------------------------------
// CNSmlThreadEngine::NewL( CNSmlThreadParams& aParams )
// Two phase Constructor
// --------------------------------------------------------------------------
//
CNSmlThreadEngine* CNSmlThreadEngine::NewL( CNSmlThreadParams& aParams )
	{
	CNSmlThreadEngine* self = new (ELeave) CNSmlThreadEngine( aParams );
	CleanupStack::PushL( self );
	self->ConstructL();
	CleanupStack::Pop();
	return self;
	}

// --------------------------------------------------------------------------
// CNSmlThreadEngine::ConstructL()
// 2nd Constructor
// --------------------------------------------------------------------------
//
void CNSmlThreadEngine::ConstructL()
	{
		FeatureManager::InitializeLibL();
	iThreadParams.iThreadEngine = this;
	iContentArray = new(ELeave) CArrayFixFlat<TNSmlContentSpecificSyncType>(1);
	// Fix for cancel not happening when cancel key is
	// pressed .
	iSyncCancelled = EFalse;
	if ( iThreadParams.iCSArray )
		{
		for (TInt i = 0; i < iThreadParams.iCSArray->Count(); i++)
			{
			TNSmlContentSpecificSyncType type;
			type.iTaskId = iThreadParams.iCSArray->At( i ).iTaskId;
			type.iType = iThreadParams.iCSArray->At( i ).iType;
			iContentArray->AppendL( type );
			}
		}		
	}

// --------------------------------------------------------------------------
// CNSmlThreadEngine( TNSmlThreadParams& aParams ) 
// Constructor
// --------------------------------------------------------------------------
//
CNSmlThreadEngine::CNSmlThreadEngine( CNSmlThreadParams& aParams ) 
: CActive(EPriorityLow), iThreadParams(aParams)
	{
	CActiveScheduler::Add(this);
	}

// --------------------------------------------------------------------------
// CNSmlThreadEngine::~CNSmlThreadEngine()
// Destructor
// --------------------------------------------------------------------------
//
CNSmlThreadEngine::~CNSmlThreadEngine()
	{
	delete iContentArray;
	delete iNotifierObserver;
	Cancel();
	RThread().Close();
	delete iDSAgent;
	delete iDMAgent;
	delete iCancelTimeout;
	FeatureManager::UnInitializeLib();
	}

// --------------------------------------------------------------------------
// CNSmlThreadEngine::SilentModeL()
// Check is current serverid mentioned to work in silent mode (TARM feature)
// --------------------------------------------------------------------------
//
TBool CNSmlThreadEngine::SilentModeL( const CNSmlThreadParams* aParams )
    {   
    _DBG_FILE("CNSmlThreadEngine::SilentModeL() : start");	
    TInt retVal = ETrue;
    
    if ( aParams->iCurrentJob.UsageType() == ESmlDataSync )
        {
        return EFalse;               
        }
 if(FeatureManager::FeatureSupported(KFeatureIdSapPolicyManagement))
 {
	//get DM profile -> get profile serverid
	CNSmlDMSettings* settings = CNSmlDMSettings::NewLC();
    CNSmlDMProfile* prof = settings->ProfileL( 
            aParams->iCurrentJob.ProfileId() );
    CleanupStack::PushL( prof );

	//connect to policy engine
	RPolicyEngine policyEngine;
	User::LeaveIfError( policyEngine.Connect() );
	CleanupClosePushL( policyEngine );
		
	RPolicyManagement policyManagement;
	User::LeaveIfError( policyManagement.Open( policyEngine ) );

	//check is the specific server id allowed to work in silent mode
	TInt err = policyManagement.IsAllowedServerId( 
	        prof->StrValue( EDMProfileServerId ) );
			
	policyManagement.Close();

	CleanupStack::PopAndDestroy( 3, settings );
	
	if ( err == KErrNone )
	    {
	    retVal = ETrue;    
	    }
	else
	    {
	    retVal = EFalse;    
	    }	
}
else
{
	//if policy management is not installed....
	retVal = EFalse;
}

    if( retVal )
        {
        _DBG_FILE("CNSmlThreadEngine::SilentModeL() : end, return ETrue");
        }
    else
        {
        _DBG_FILE("CNSmlThreadEngine::SilentModeL() : end, return EFalse");
        }

    return retVal;
    }

// --------------------------------------------------------------------------
// CNSmlThreadEngine::SaveSessionInfo()
// Save session info which is needed later in DM agent to launch notifier
// --------------------------------------------------------------------------
//
void CNSmlThreadEngine::SaveSessionInfoL()
    {    
 if(FeatureManager::FeatureSupported(KFeatureIdSapPolicyManagement))
 {
	CRepository * rep = CRepository::NewLC( 
	                    KCRUidPolicyManagementUtilInternalKeys );

	TSyncMLAppLaunchNotifParams params;	
	
	if ( iThreadParams.iCurrentJob.UsageType() == ESmlDataSync )	
	    {
	    params.iSessionType = ESyncMLSyncSession;    
	    }
	else
	    {
	    params.iSessionType = ESyncMLMgmtSession;    
	    }
   
	params.iJobId = iThreadParams.iCurrentJob.JobId();
	params.iProfileId = iThreadParams.iCurrentJob.ProfileId(); 

    TPckg<TSyncMLAppLaunchNotifParams> data( params );
   
	TInt err = rep->Set( KSyncMLSessionParamsKey, data );
	
	if ( err == KErrNotFound )
	    {
		User::LeaveIfError( rep->Create( KSyncMLSessionParamsKey, data ) );
    	}
	else
	    {
        _DBG_FILE("CNSmlThreadEngine::SaveSessionInfoL TInt err = \
                    rep->Set( KParamsKey, data );");
	    User::LeaveIfError( err );
	    }

	CleanupStack::PopAndDestroy( rep );
}
    }

// --------------------------------------------------------------------------
// CNSmlThreadEngine::ForcedCertificateCheckL( TBool aForcedState )
// Launch job session in thread
// --------------------------------------------------------------------------
//
void CNSmlThreadEngine::ForcedCertificateCheckL( TBool aForcedState )
    {
    if(FeatureManager::FeatureSupported(KFeatureIdSapPolicyManagement))
    {	
	CRepository * rep = CRepository::NewLC( KCRUidPolicyManagementUtilInternalKeys );

	TInt err = rep->Set( KSyncMLForcedCertificateCheckKey, aForcedState );
	
	if ( err == KErrNotFound )
	    {
		User::LeaveIfError( rep->Create( KSyncMLForcedCertificateCheckKey, aForcedState ));
    	}

	CleanupStack::PopAndDestroy( rep );
		}
    }

// --------------------------------------------------------------------------
// CNSmlThreadEngine::StartJobSessionL()
// Launch job session in thread
// --------------------------------------------------------------------------
//
void CNSmlThreadEngine::StartJobSessionL()
	{
	_DBG_FILE("CNSmlThreadStart::StartJobSession() : Begin");	
		
	if ( !IsActive() )
		{
		SetActive();
		}
		
	TInt status( KErrNone );			
	if ( iThreadParams.iCurrentJob.UsageType() == ESmlDevMan )
		{
		CRepository* centrep = NULL;
		TRAPD( err, centrep = CRepository::NewL(KCRUidDeviceManagementInternalKeys));  
		TInt phoneLock(0);
		TInt factoryProfileID(0);
		if (err==KErrNone ) 
		{
			TInt err = centrep->Get( KLAWMOPhoneLock , phoneLock );
			err = centrep->Get( KLAWMOfactoryDmProfileID , factoryProfileID );
			delete centrep;
			centrep = NULL;
		}
		if(phoneLock != 30)
		{
			_DBG_FILE("CNSmlThreadEngine phonelock != 30");
			if(factoryProfileID>0)   
			{   

				TInt profileId = iThreadParams.iCurrentJob.ProfileId();    
	
				if(profileId!= factoryProfileID) 
					{
					   _DBG_FILE("CNSmlThreadEngine profid doesnt match with factory");
					   TRequestStatus* stat = &iStatus;
					   User::RequestComplete( stat, KErrNone );
					   return;
					}   
			
			}
			else
				{
				    _DBG_FILE("CNSmlThreadEngine factoryprofileid<0");
				    TRequestStatus* stat = &iStatus;
					User::RequestComplete( stat, KErrNone );
					return;	
				}
		}
		_DBG_FILE("CNSmlThreadEngine startDMSessionL");
		TRAP( status, StartDMSessionL() );
		}
	else
		{
		// Scenario 1:
		// Fix for cancel not happening when cancel key is
		// pressed .
        if(!iSyncCancelled)
            {
            TRAP( status, StartDSSessionL() );	
            }
        else
            {
			// Sync is cancelled from the UI before the
			// the job session has started.
			// Fix for cancel not happening when cancel key is
			// pressed .
            status = KErrCancel;
            iSyncCancelled = EFalse;
            }
		}
	
	if ( status != KErrNone )
	    {
	   	TRequestStatus* sStatus = &iStatus;
		User::RequestComplete( sStatus, status );    
	    }
	_DBG_FILE("CNSmlThreadStart::StartJobSession() : End");	
	}

// --------------------------------------------------------------------------
// CNSmlThreadEngine::CancelJob()
// Cancel current running job
// --------------------------------------------------------------------------
//
void CNSmlThreadEngine::CancelJob()
	{
	// Fix for cancel not happening when cancel key is
	// pressed .
    // job has been created but it is not running and from ui
    // Cancel has been called
    iSyncCancelled = ETrue;
	if ( iCancelTimeout )
	    {
		iCancelTimeout->SetJobCancelled( iThreadParams.iCurrentJob.UsageType() );
	    }	
	}

// --------------------------------------------------------------------------
// CNSmlThreadEngine::StartDMSessionL()
// Start DM job
// --------------------------------------------------------------------------
//	
void CNSmlThreadEngine::StartDMSessionL()
	{
	TInt result = iSessionLib.Load( KSmlDMAgentLoaderLibName );
    if ( result != KErrNone )
    	{
	    _DBG_FILE("CNSmlSOSHandler::StartDMSyncL(): loading agent failed !");
    	User::Leave( result );
    	}
    TNSmlCreateDMAgentFunc createDMAgentL = (TNSmlCreateDMAgentFunc)iSessionLib.Lookup(1);
    iDMAgent =  createDMAgentL(iThreadParams.iSOSHandler);    
    
    iCancelTimeout = new (ELeave) CNSmlCancelTimeOut( *iDMAgent, *iDSAgent );
    
	iDMAgent->Configure(iStatus, iThreadParams.iCurrentJob.ProfileId(), iThreadParams.iCurrentJob.TransportId(), (TNSmlSyncInitiation)iThreadParams.iSyncInit,
			iThreadParams.iCurrentJob.GetJobIapId());
	}

// --------------------------------------------------------------------------
// CNSmlThreadEngine::StartDSSessionL()
// Start DS job
// --------------------------------------------------------------------------
//		
void CNSmlThreadEngine::StartDSSessionL()
	{
	
	TInt result = iSessionLib.Load( KSmlDSAgentLoaderLibName );
	if ( result != KErrNone )
		{
	    _DBG_FILE("CNSmlSOSHandler::StartDSSyncL(): loading agent failed!");
		User::Leave( result );
		}
	
	TNSmlCreateDSAgentFunc createDSAgentL = (TNSmlCreateDSAgentFunc)iSessionLib.Lookup(1);
	iDSAgent = createDSAgentL( iThreadParams.iSOSHandler );
	
	iCancelTimeout = new (ELeave) CNSmlCancelTimeOut( *iDMAgent, *iDSAgent );
	
	// Select correct sync method and start sync
	TNSmlSyncInitiation syncInit = (TNSmlSyncInitiation)iThreadParams.iSyncInit;
	// Scenario 2:
	// Fix for cancel not happening when cancel key is
	// pressed .
	if(iSyncCancelled)
	    {
	    iSyncCancelled = EFalse;
        User::Leave( KErrCancel );
	    }

    switch ( iThreadParams.iCurrentJob.JobType() )
        {
        case EDSJobProfile:
            iDSAgent->Synchronise( iStatus, iThreadParams.iCurrentJob.ProfileId(), KNSmlNullId, NULL, syncInit );
            break;           
        case EDSJobProfileST:
            iDSAgent->Synchronise( iStatus, iThreadParams.iCurrentJob.ProfileId(), KNSmlNullId, iContentArray, syncInit ); // + syncType
            break;            
        case EDSJobTasks:
            iDSAgent->Synchronise( iStatus, iThreadParams.iCurrentJob.ProfileId(), KNSmlNullId, iContentArray, syncInit ); // + tasks
            break;
        case EDSJobTasksST:
            iDSAgent->Synchronise( iStatus, iThreadParams.iCurrentJob.ProfileId(), KNSmlNullId, iContentArray, syncInit ); // + tasks & syncType
            break;
        case EDSJobTransport:
            iDSAgent->Synchronise( iStatus, iThreadParams.iCurrentJob.ProfileId(), iThreadParams.iCurrentJob.TransportId(), NULL, syncInit ); // + transport
            break;
        case EDSJobTransportST:
            iDSAgent->Synchronise( iStatus, iThreadParams.iCurrentJob.ProfileId(), iThreadParams.iCurrentJob.TransportId(), iContentArray, syncInit ); // + transport & syncType
            break;
        case EDSJobTaskTrans:
            iDSAgent->Synchronise( iStatus, iThreadParams.iCurrentJob.ProfileId(), iThreadParams.iCurrentJob.TransportId(), iContentArray, syncInit ); // + tasks & transport
            break;
        case EDSJobTaskTransST:
            iDSAgent->Synchronise( iStatus, iThreadParams.iCurrentJob.ProfileId(), iThreadParams.iCurrentJob.TransportId(), iContentArray, syncInit, iThreadParams.iCurrentJob.iSessionId, iThreadParams.iCurrentJob.iPackage ); // + tasks & transport & syncType & server alert
            break;
        default:
            break;
        }
	}

// --------------------------------------------------------------------------
// CNSmlThreadEngine::VerifyJobFromNotifierL()
// Launches notifier and waits for 
// --------------------------------------------------------------------------
//
TInt CNSmlThreadEngine::VerifyJobFromNotifierL()
	{
	_DBG_FILE("CNSmlThreadEngine::VerifyJobFromNotifierL() : Begin");
	iNotifierObserver = new (ELeave) CNSmlNotifierObserver(iStatus, iThreadParams);
	SetActive();
	
	TSyncMLAppLaunchNotifParams params;
	params.iSessionType = ( iThreadParams.iCurrentJob.UsageType() == ESmlDataSync ) ? ESyncMLSyncSession : ESyncMLMgmtSession;
	params.iJobId = iThreadParams.iCurrentJob.JobId();
	params.iProfileId = iThreadParams.iCurrentJob.ProfileId();
    params.iUimode = iThreadParams.iCurrentJob.iUimode;	
	iNotifierObserver->ConnectToNotifierL( params );
	_DBG_FILE("CNSmlThreadEngine::VerifyJobFromNotifierL() : End");
    return KErrNone;
	}

// --------------------------------------------------------------------------
// CNSmlThreadEngine::DMAgentL()
// Returns dm agent if it is created, otherwise leaves with KErrNotFound.
// --------------------------------------------------------------------------
//
CNSmlDMAgent* CNSmlThreadEngine::DMAgentL() const
	{
	if ( !iDMAgent )
		{
		User::Leave( KErrNotFound );
		}
	
	return iDMAgent;
	}

// --------------------------------------------------------------------------
// CNSmlThreadEngine::DSAgentL()
// Returns ds agent if it is created, otherwise leaves with KErrNotFound.
// --------------------------------------------------------------------------
//
CNSmlDSAgent* CNSmlThreadEngine::DSAgentL() const
	{
	if ( !iDSAgent )
		{
		User::Leave( KErrNotFound );
		}
	
	return iDSAgent;
	}

// --------------------------------------------------------------------------
// CNSmlThreadEngine::DoCancel()
// From base class
// --------------------------------------------------------------------------
//
void CNSmlThreadEngine::DoCancel()
	{
	_DBG_FILE("CNSmlThreadStart::DoCancel() : Begin");	
	_DBG_FILE("CNSmlThreadStart::DoCancel() : End");	
	}

// --------------------------------------------------------------------------
// CNSmlThreadEngine::RunL()
// From base class : 
// --------------------------------------------------------------------------
//
void CNSmlThreadEngine::RunL()
	{
	_DBG_FILE("CNSmlThreadEngine::RunL() : Begin");
	TBool resume( EFalse );
	TRAP_IGNORE( resume = DoRunL() );
	
	if ( resume )
	    {
	    return;
	    }
	_DBG_FILE("CNSmlThreadEngine::RunL() : End");	
	CActiveScheduler::Stop();	
	}

// --------------------------------------------------------------------------
// CNSmlThreadEngine::DoRunL()
// Method to perform RunL, that can be trapped.
// --------------------------------------------------------------------------
//
TBool CNSmlThreadEngine::DoRunL()
    {
    _DBG_FILE("CNSmlThreadEngine::DoRunL() : Begin");
    
    if ( iThreadParams.iCurrentJob.iPending != EFalse )	
		{
		iThreadParams.iCurrentJob.iPending = EFalse;
		if ( iStatus.Int() == KErrNone )
			{
			StartJobSessionL();
			return ETrue;
			}
		else
			{
			if ( iThreadParams.iCurrentJob.TransportId() != KUidNSmlMediumTypeInternet.iUid )
				{
				// Disconnect local connection.
				TRAP_IGNORE( iThreadParams.iSOSHandler->DoDisconnectL() );				
				}
			
			WriteHistoryLogL( iStatus.Int() );
			}
		}
	_DBG_FILE("CNSmlThreadEngine::DoRunL() : Begin");    
	return EFalse;	
    }

// --------------------------------------------------------------------------
// CNSmlThreadEngine::WriteHistoryLogL( TInt aErrorCode )
// Writes history log when notifier start 
// --------------------------------------------------------------------------
//
void CNSmlThreadEngine::WriteHistoryLogL( TInt aErrorCode )
	{
    RArray<TInt> tasks;
   	       	
    if ( iThreadParams.iCurrentJob.UsageType() == ESmlDataSync )
    	{
    	iThreadParams.iCurrentJob.TaskIds( tasks );	
    	}       	
   	CleanupClosePushL(tasks);
   	
    CSyncMLHistoryJob* historyLog = CSyncMLHistoryJob::NewL( iThreadParams.iCurrentJob.ProfileId(), tasks );
    CleanupStack::PushL( historyLog );             
    
	TTime time;
	time.UniversalTime();
	
	historyLog->SetResult( time, aErrorCode );

	if ( iThreadParams.iCurrentJob.UsageType() == ESmlDataSync )
		{
		CNSmlDSSettings* settings = CNSmlDSSettings::NewLC();
		CNSmlDSProfile* prof = settings->ProfileL( iThreadParams.iCurrentJob.ProfileId() );
		CleanupStack::PushL( prof );
		CNSmlHistoryArray* array = CNSmlHistoryArray::NewL();
		CleanupStack::PushL( array );
		array->SetOwnerShip( ETrue );
	
		RReadStream& readStream = prof->LogReadStreamL();
		CleanupClosePushL( readStream );
		
		if ( readStream.Source()->SizeL() > 0 )
			{
			array->InternalizeL( readStream );
			}
			
		CleanupStack::PopAndDestroy(); // readStream
	
		array->AppendEntryL( historyLog );
		RWriteStream& writeStream = prof->LogWriteStreamL();
		CleanupClosePushL( writeStream );
		array->ExternalizeL( writeStream );
		CleanupStack::PopAndDestroy(); // writeStream
		prof->WriteStreamCommitL();
	
		CleanupStack::PopAndDestroy(array);
		CleanupStack::PopAndDestroy(prof);
		CleanupStack::PopAndDestroy(settings);
		}
	else	// DevMan
		{
		CNSmlDMSettings* settings = CNSmlDMSettings::NewLC();
		CNSmlDMProfile* prof = settings->ProfileL( iThreadParams.iCurrentJob.ProfileId() );
		CleanupStack::PushL( prof );
		CNSmlHistoryArray* array = CNSmlHistoryArray::NewL();
		CleanupStack::PushL( array );
		array->SetOwnerShip( ETrue );
	
		RReadStream& readStream = prof->LogReadStreamL();
		CleanupClosePushL( readStream );
		
		if ( readStream.Source()->SizeL() > 0 )
			{
			array->InternalizeL( readStream );
			}
			
		CleanupStack::PopAndDestroy(); // readStream
	
		array->AppendEntryL( historyLog );
		RWriteStream& writeStream = prof->LogWriteStreamL();
		CleanupClosePushL( writeStream );
		array->ExternalizeL( writeStream );
		CleanupStack::PopAndDestroy(); // writeStream
		prof->WriteStreamCommitL();
	
		CleanupStack::PopAndDestroy(array);
		CleanupStack::PopAndDestroy(prof);
		CleanupStack::PopAndDestroy(settings); 				
		}
		
	CleanupStack::Pop(historyLog);
	CleanupStack::PopAndDestroy(&tasks);					
	}
// --------------------------------------------------------------------------
// TNSmlThreadLauncher::RunJobSession( CNSmlThreadParams& aParams, CNSmlThreadObserver& aObserver )
// Launches a new thread for job session
// --------------------------------------------------------------------------
//
TInt TNSmlThreadLauncher::RunJobSession( CNSmlThreadParams& aParams, CNSmlThreadObserver& aObserver )
	{
	_DBG_FILE("TNSmlThreadLauncher::RunJobSession() : Begin");	
	RThread thread;
	
	TName threadName ( KNSmlJobThread );
    threadName.Append ( _L("_") );
    threadName.AppendNum ( Math::Random(), EHex );
	
	TInt status = thread.Create(threadName, ThreadFunction, KNSmlSOSServerStackSize, NULL, &aParams, EOwnerProcess);
	
	if (status != KErrNone)
		{
		thread.Close();
		return status;
		}
		
	thread.SetPriority( EPriorityNormal ); // set priority
	thread.Resume();
	
	//Request notification when DM session thread ends.
	aObserver.LogonToThread( thread );

	thread.Close(); 
	_DBG_FILE("TNSmlThreadLauncher::RunJobSession() : End");	
	return status;
	}


// --------------------------------------------------------------------------
// CNSmlThreadObserver::CNSmlThreadObserver()
// Constructor
// --------------------------------------------------------------------------
//
CNSmlThreadObserver::CNSmlThreadObserver(TRequestStatus& aStatus)
:CActive(0), iSosStatus(aStatus)
	{
	CActiveScheduler::Add(this);
	}

// --------------------------------------------------------------------------
// CNSmlThreadObserver::~CNSmlThreadObserver()
// Constructor
// --------------------------------------------------------------------------
//
CNSmlThreadObserver::~CNSmlThreadObserver()
	{
	Cancel();
	}

// --------------------------------------------------------------------------
// CNSmlThreadObserver::LogonToThread( RThread& aThread )
// Logs on to given thread. Wakes the server scheduler
// --------------------------------------------------------------------------
//	
void CNSmlThreadObserver::LogonToThread( RThread& aThread )
	{
	_DBG_FILE("CNSmlThreadObserver::LogonToThread() : Begin");	
	if ( !IsActive() )
		{
		SetActive();
		}
		
	aThread.Logon( iStatus );
	_DBG_FILE("CNSmlThreadObserver::LogonToThread() : End");	
	}

// --------------------------------------------------------------------------
// CNSmlThreadObserver::DoCancel()
// From base class
// --------------------------------------------------------------------------
//
void CNSmlThreadObserver::DoCancel()
	{
	
	}

// --------------------------------------------------------------------------
// CNSmlThreadObserver::RunL()
// Logs on to given thread. Called when thread finishes.
// --------------------------------------------------------------------------
//
void CNSmlThreadObserver::RunL()
	{
	DBG_FILE_CODE(iStatus.Int(), _S8("CNSmlThreadObserver::RunL() : Begin"));	
	
	TInt err;
	TInt dsStatus = KErrNotFound;
	
	err = RProperty::Set( KPSUidDataSynchronizationInternalKeys, KDataSyncStatus, dsStatus );
	
	if(err == KErrNone) 
	{
		if(dsStatus != EDataSyncNotRunning )	
		{
			RProperty::Set( KPSUidDataSynchronizationInternalKeys, KDataSyncStatus, EDataSyncNotRunning );	
		}
	
	}
	//Notify thread completion status code to caller.
	TRequestStatus* sStatus = &iSosStatus;
	User::RequestComplete( sStatus, iStatus.Int() );
	
	_DBG_FILE("CNSmlThreadObserver::LogonToThread() : End");	
	}

// --------------------------------------------------------------------------
// CNSmlNotifierObserver::CNSmlNotifierObserver(TRequestStatus& aStatus, CNSmlThreadEngine* aEngine, CNSmlThreadParams& aParams)
// Constructor
// --------------------------------------------------------------------------
//	
CNSmlNotifierObserver::CNSmlNotifierObserver(TRequestStatus& aStatus, CNSmlThreadParams& aParams)
: CActive(0), iCallerStatus(aStatus), iThreadParams(aParams)
	{
	CActiveScheduler::Add(this);
	}

// --------------------------------------------------------------------------
// CNSmlNotifierObserver::~CNSmlNotifierObserver()
// Destructor
// --------------------------------------------------------------------------
//	
CNSmlNotifierObserver::~CNSmlNotifierObserver()
	{
	
	// StartNotifier called to avoid Notifier server panic, if 
	// notifier does not exist anymore.
	TBuf8<1> dummy;	
	iNotifier.StartNotifier(KNullUid, dummy, dummy); // KNullUid should do also..
	
	iNotifier.CancelNotifier( KUidNotifier );
	iNotifier.Close();
	Cancel();
	}

// --------------------------------------------------------------------------
// CNSmlNotifierObserver::ConnectToNotifierL( const TSyncMLAppLaunchNotifParams& aParam )
// Starts connection with notifier
// --------------------------------------------------------------------------
//		
void CNSmlNotifierObserver::ConnectToNotifierL( const TSyncMLAppLaunchNotifParams& aParam )
	{
	_DBG_FILE( "CNSmlNotifierObserver::ConnectToNotifierL:Begin" );
	iTimeOut = EFalse;
	
	if ( !IsActive() )
		{
		SetActive();
		}
		
	   
    TSyncMLAppLaunchNotifParamsPckg data( aParam );
    
    if ( !KNSmlWaitNotifierForEver )
        {
        iNotifierTimeOut.LaunchNotifierTimer( this );
        }
        
    TInt err = iNotifier.Connect();
    if ( err == KErrNone )
        {
        iNotifier.StartNotifierAndGetResponse( iStatus, KUidNotifier, data, iResBuf );                
        }
    else
        {
        // Stop job. Error connecting to notifier.
        TRequestStatus* sStatus = &iStatus;
		User::RequestComplete( sStatus, err );            
        }
     _DBG_FILE("CNSmlNotifierObserver::ConnectToNotifierL:End");   
	}

// --------------------------------------------------------------------------
// CNSmlNotifierObserver::NotifierTimeOut()
// Starts notifier time out timer
// --------------------------------------------------------------------------
//
void CNSmlNotifierObserver::NotifierTimeOut()
	{
	iTimeOut = ETrue;
	
	// StartNotifier called to avoid Notifier server panic, if 
	// notifier does not exist anymore.
	TBuf8<1> dummy;
	iNotifier.StartNotifier(KNullUid, dummy, dummy); // KNullUid should do also..
	iNotifier.CancelNotifier( KUidNotifier );
	}

// --------------------------------------------------------------------------
// CNSmlNotifierObserver::DoCancel()
// From base class
// --------------------------------------------------------------------------
//	
void CNSmlNotifierObserver::DoCancel()
	{
	}

// --------------------------------------------------------------------------
// CNSmlNotifierObserver::RunL()
// From base class
// --------------------------------------------------------------------------
//	
void CNSmlNotifierObserver::RunL()
	{
	    
    TInt ret = iStatus.Int();
    if ( ret == KErrCancel && iTimeOut )
    	{
    	
    	}
    else
    if ( ret == KErrNone )
    	{	   
        
	  	TInt sid = iResBuf().iSecureId.iUid; // read secure id from notifier.
	   	
	    // Check the response and error code. If there is a fail, dump the job.        
	    // Also compare sid to creator id saved for current job to secure that listener owns the job.
	    if ( ret != KErrNone || sid != iThreadParams.iCurrentJob.CreatorId() )
	        {
	        if ( ret == KErrNone && sid != iThreadParams.iCurrentJob.CreatorId() )
	        	{
#ifndef __WINS__	        	
	        	ret = KErrPermissionDenied;
#endif	        	
	        	}
	        }
	    	
    	}
    
	TRequestStatus* sStatus = &iCallerStatus;
	User::RequestComplete( sStatus, ret );
	}

// --------------------------------------------------------------------------
// CNSmlNotifierTimeOut::~CNSmlNotifierTimeOut()
// Constructor
// --------------------------------------------------------------------------
//
CNSmlNotifierTimeOut::CNSmlNotifierTimeOut()
:CActive(0)
	{
	CActiveScheduler::Add(this);
	}

// --------------------------------------------------------------------------
// CNSmlNotifierTimeOut::~CNSmlNotifierTimeOut()
// Destructor
// --------------------------------------------------------------------------
//
CNSmlNotifierTimeOut::~CNSmlNotifierTimeOut()
	{
	Cancel();
	iTimeOutTimer.Close();
	}

// --------------------------------------------------------------------------
// CNSmlNotifierTimeOut::LaunchNotifierTimer( CNSmlNotifierObserver*  aObserver )
// Starts notifier time out timer
// --------------------------------------------------------------------------
//
void CNSmlNotifierTimeOut::LaunchNotifierTimer( CNSmlNotifierObserver*  aObserver )
	{
	
	if ( !IsActive() )
		{
		SetActive();
		}
		
	iTimeOutTimer.CreateLocal();
	iTimeOutTimer.After( iStatus, KNotifierTimeout );
	iObserver = aObserver;
	}
	
// --------------------------------------------------------------------------
// CNSmlNotifierTimeOut::DoCancel()
// From base class 
// --------------------------------------------------------------------------
//
void CNSmlNotifierTimeOut::DoCancel()
	{
	iTimeOutTimer.Cancel();
	}

// --------------------------------------------------------------------------
// CNSmlNotifierTimeOut::RunL()
// From base class 
// --------------------------------------------------------------------------
//
void CNSmlNotifierTimeOut::RunL()
	{
	if ( iStatus.Int() != KErrCancel )
		{
		iObserver->NotifierTimeOut();	
		}
	}

// --------------------------------------------------------------------------
// CNSmlCancelTimeOut::CNSmlCancelTimeOut()
// Constructor
// --------------------------------------------------------------------------
//
CNSmlCancelTimeOut::CNSmlCancelTimeOut( CNSmlDMAgent& aDMAgent, CNSmlDSAgent& aDSAgent )
: CActive(0), iDMAgent(aDMAgent), iDSAgent(aDSAgent)
	{
	CActiveScheduler::Add(this);
	iTimeOutTimer.CreateLocal();
	iDoCancel = EFalse;
	if ( !IsActive() )
		{
		SetActive();
		}
		
	iTimeOutTimer.After( iStatus, 10000 );	
	}

// --------------------------------------------------------------------------
// CNSmlCancelTimeOut::~CNSmlCancelTimeOut()
// Destructor
// --------------------------------------------------------------------------
//	
CNSmlCancelTimeOut::~CNSmlCancelTimeOut()
	{
	Cancel();
	iTimeOutTimer.Close();	
	}

// --------------------------------------------------------------------------
// CNSmlCancelTimeOut::SetJobCancelled()
// Flags job to be cancelled
// --------------------------------------------------------------------------
//			
void CNSmlCancelTimeOut::SetJobCancelled( TSmlUsageType aUsageType )
	{
	iDoCancel = ETrue;
	iUsageType = aUsageType;	
	}

// --------------------------------------------------------------------------
// CNSmlCancelTimeOut::DoCancel()
// From base class 
// --------------------------------------------------------------------------
//	
void CNSmlCancelTimeOut::DoCancel()
	{
	iTimeOutTimer.Cancel();	
	}

// --------------------------------------------------------------------------
// CNSmlCancelTimeOut::RunL()
// From base class 
// --------------------------------------------------------------------------
//
void CNSmlCancelTimeOut::RunL()
	{
	if ( iDoCancel )
		{
		if ( iUsageType == ESmlDevMan )
			{
			iDMAgent.Cancel();					
			}
		else
			{
			iDSAgent.Cancel();					
			}
		}
	else
		{
		if ( !IsActive() )
			{
			SetActive();
			}
		iTimeOutTimer.After( iStatus, 10000 );		
		}
	}

// --------------------------------------------------------------------------
// CNSmlThreadParams::~CNSmlThreadParams()
// Destructor
// --------------------------------------------------------------------------
//
CNSmlThreadParams::~CNSmlThreadParams()
	{
	delete iCSArray;
	iCSArray = 0;
	}
