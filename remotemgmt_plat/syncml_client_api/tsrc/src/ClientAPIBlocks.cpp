/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description: definition of dm constants/exported methods
* 	This is part of remotemgmt_plat.
*
*/



// INCLUDE FILES
#include <e32svr.h>
#include <StifParser.h>
#include <Stiftestinterface.h>

#include <SyncMLDef.h>
#include <SyncMLClient.h>
#include <SyncMLClientDS.h>
#include <SyncMLClientDM.h>
#include <SyncMLTransportProperties.h>

#include "ClientAPI.h"
#include "nsmlconstants.h"

// EXTERNAL DATA STRUCTURES
//extern  ?external_data;

// EXTERNAL FUNCTION PROTOTYPES  
//extern ?external_function( ?arg_type,?arg_type );


// CONSTANTS
const TSmlServerAlertedAction 	KDSProfileSAN 		= ESmlConfirmSync;
const TSmlProtocolVersion 		KDSProfileProtocol 	= ESmlVersion1_2;
const TInt 						KDSProfileCreatorId = 10;

const TSmlServerAlertedAction 	KDMProfileSAN 		= ESmlConfirmSync;
const TSmlProtocolVersion 		KDMProfileProtocol 	= ESmlVersion1_1_2;
const TInt 						KDMProfileCreatorId = 15;

const TInt						KTaskCreatorId		= 100;
const TSmlSyncType				KTaskSyncType		= ESmlSlowSync;
const TBool						KTaskEnabled		= ETrue;

_LIT( 	KDSProfileDisplayName, 		"data sync profile" );
_LIT8( 	KDSProfileUserName, 		"ds username" );
_LIT8( 	KDSProfilePassword, 		"ds passwd" );
_LIT8( 	KDSProfileServerId, 		"ds server id" );
_LIT8( 	KDSProfileServerPasswd, 	"ds server passwd" );

_LIT( 	KDMProfileDisplayName, 		"dev man profile" );
_LIT8( 	KDMProfileUserName, 		"dm username" );
_LIT8( 	KDMProfilePassword, 		"dm passwd" );
_LIT8( 	KDMProfileServerId, 		"dm server id" );
_LIT8( 	KDMProfileServerPasswd, 	"dm server passwd" );

_LIT( 	KTaskServerDataSource, 		"server\\data.db" );
_LIT( 	KTaskClientDataSource, 		"c:\\system\\data\\calendar" );
_LIT(	KTaskDisplayName,			"task name on display" );


// MACROS
//#define ?macro ?macro_def

// LOCAL CONSTANTS AND MACROS
//const ?type ?constant_var = ?constant;
//#define ?macro_name ?macro_def

// MODULE DATA STRUCTURES
//enum ?declaration
//typedef ?declaration

// LOCAL FUNCTION PROTOTYPES
//?type ?function_name( ?arg_type, ?arg_type );

// FORWARD DECLARATIONS
//class ?FORWARD_CLASSNAME;

// ============================= LOCAL FUNCTIONS ===============================

//
// Event thread functionality
//

LOCAL_C TInt EventThreadFunction(TAny* aData)
	{
	RSemaphore& started= *(RSemaphore*) aData;
	
	CTrapCleanup* cleanup = CTrapCleanup::New(); // get clean-up stack
	CActiveScheduler* scheduler = new (ELeave) CActiveScheduler();
	CActiveScheduler::Install( scheduler );

	EventCallback* callback = new (ELeave) EventCallback();
//	TRAPD( err, iSyncSession.RequestEventL( *callback ) );
	
	CStifLogger* event_logger;
	TInt err;
    TRAP( err, event_logger = CStifLogger::NewL( KClientAPILogPath, 
                          _L("events.txt"),
                          CStifLogger::ETxt,
                          CStifLogger::EFile,
                          EFalse, ETrue, ETrue, EFalse, EFalse, EFalse ) );
	
	// print one empty line to log between test cases
    event_logger->Log( _L("") );
    
	started.Signal();
	
	CActiveScheduler::Start();

	delete event_logger;
    delete callback;
    delete scheduler;
	delete cleanup;
	
	return err;
	}
	
LOCAL_C TInt StartEventThread()
	{
	RSemaphore signal;
    signal.CreateLocal(0);

	RThread thread;
		
    TRAPD(err,thread.Create(
        _L("EventThread"), // name of thread
        EventThreadFunction, // thread function
        KDefaultStackSize,NULL, // default size for stack and same heap with main thread
        &signal // semaphore as a parameter to thread function
        ));
	if ( err != KErrNone )
		{
		thread.Close();
		signal.Close();
		return KErrGeneral;
		}
	
	// start thread
	thread.SetPriority( EPriorityMuchMore ); // set priority
	thread.Resume(); 
	thread.Close(); 

    signal.Wait(); 
	signal.Close();
	
	return KErrNone;
	}
	
//
// Progress event thread functionality
//

LOCAL_C TInt ProgressThreadFunction(TAny* aData)
	{
	RSemaphore& started= *(RSemaphore*) aData;
	
	CTrapCleanup* cleanup = CTrapCleanup::New(); // get clean-up stack
	CActiveScheduler* scheduler = new (ELeave) CActiveScheduler();
	CActiveScheduler::Install( scheduler );

	ProgressCallback* callback = new (ELeave) ProgressCallback();

	CStifLogger* progress_logger;
	TInt err;
    TRAP( err, progress_logger = CStifLogger::NewL( KClientAPILogPath, 
                          _L("progress.txt"),
                          CStifLogger::ETxt,
                          CStifLogger::EFile,
                          EFalse, ETrue, ETrue, EFalse, EFalse, EFalse ) );
                          
	// print one empty line to log between test cases
    progress_logger->Log( _L("") );
    
	started.Signal();
	
	CActiveScheduler::Start();

	delete progress_logger;
    delete callback;
    delete scheduler;
	delete cleanup;
	
	return err;
	}
	
LOCAL_C TInt StartProgressThread()
	{
	RSemaphore signal;
    signal.CreateLocal(0);

	RThread thread;
	TRAPD(err,thread.Create(
	        _L("ProgressThread"), // name of thread
	        ProgressThreadFunction, // thread function
	        KDefaultStackSize, NULL, // default size for stack and same heap with main thread
	        &signal // semaphore as a parameter to thread function
	        ));
	if ( err != KErrNone )
		{
		thread.Close();
		signal.Close();
		return KErrGeneral;
		}
	
	// start thread
	thread.SetPriority( EPriorityMuchMore ); // set priority
	thread.Resume(); 
	thread.Close(); 

    signal.Wait(); 
	signal.Close();
	
	return KErrNone;
	}
	



// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CClientAPI::Delete
// Delete here all resources allocated and opened from test methods. 
// Called from destructor. 
// -----------------------------------------------------------------------------
//
void CClientAPI::Delete() 
    {
    iProfiles.Close();
    iTransports.Close();
    iTasks.Close();
    
    delete iEventCallback2;
    delete iProgressCallback2;
    }
    
// -----------------------------------------------------------------------------
// CClientAPI::RunMethodL
// Run specified method. Contains also table of test mothods and their names.
// -----------------------------------------------------------------------------
//
TInt CClientAPI::RunMethodL( CStifItemParser& aItem ) 
    {

	// functions mapped for script
	static TStifFunctionInfo const KFunctions[] =
        {  
        ENTRY( "InitSettings", 				CClientAPI::InitSettingsL ),
        // session
        ENTRY( "OpenSyncSession", 			CClientAPI::OpenSyncSessionL ),
        ENTRY( "OpenSecondSyncSession", 	CClientAPI::OpenSecondSyncSessionL ),
        ENTRY( "CloseSyncSession", 			CClientAPI::CloseSyncSession ),
        ENTRY( "RequestEvent", 				CClientAPI::RequestEvent ),
        ENTRY( "CancelEvent", 				CClientAPI::CancelEvent ),
        ENTRY( "ChangeEvent", 				CClientAPI::ChangeEventL ),
        ENTRY( "RequestProgress", 			CClientAPI::RequestProgress ),
        ENTRY( "ChangeProgress", 			CClientAPI::ChangeProgressL ),
        ENTRY( "CancelProgress", 			CClientAPI::CancelProgress ),
        ENTRY( "CurrentJob", 				CClientAPI::CurrentJobL ),
        ENTRY( "ListQueuedJobs",			CClientAPI::ListQueuedJobsL ),
        ENTRY( "ListTransports",			CClientAPI::ListTransportsL ),
        ENTRY( "ListDataProviders",			CClientAPI::ListDataProvidersL ),
        ENTRY( "ListProfiles",				CClientAPI::ListProfilesL ),
        ENTRY( "ListProtocolVersions",		CClientAPI::ListProtocolVersionsL ),
        ENTRY( "DeleteProfile",				CClientAPI::DeleteProfileL ),
        ENTRY( "DeleteDSProfile",			CClientAPI::DeleteDSProfileL ),
        ENTRY( "DeleteDMProfile",			CClientAPI::DeleteDMProfileL ),
        // ds profile
        ENTRY( "CreateDSProfile", 			CClientAPI::CreateDSProfileL ),
        ENTRY( "OpenDSProfile",	 			CClientAPI::OpenDSProfileL ),
        ENTRY( "OpenDSProfileById",			CClientAPI::OpenDSProfileByIdL ),
        ENTRY( "OpenDSProfileByIdReadOnly",	CClientAPI::OpenDSProfileByIdReadOnlyL ),
        ENTRY( "OpenSameDSProfile",	 		CClientAPI::OpenSameDSProfileL ),
        ENTRY( "UpdateDSProfile",	 		CClientAPI::UpdateDSProfileL ),
        ENTRY( "CloseDSProfile", 			CClientAPI::CloseDSProfile ),
        ENTRY( "SetDSProfileData",			CClientAPI::SetDSProfileDataL ),
        ENTRY( "CheckInitialDSProfileData",	CClientAPI::CheckInitialDSProfileDataL ),
        ENTRY( "CheckDSProfileData",		CClientAPI::CheckDSProfileDataL ),
        ENTRY( "CheckDSProfileId",			CClientAPI::CheckDSProfileId ),
        ENTRY( "CheckDSProfileDelete",		CClientAPI::CheckDSProfileDeletion ),
        ENTRY( "DeleteDSConnection",		CClientAPI::DeleteDSConnectionL ),
        ENTRY( "DeleteDSConnectionById",		CClientAPI::DeleteDSConnectionByIdL ),        
        ENTRY( "ListTasksEmpty",			CClientAPI::ListTasksEmptyL ),
        ENTRY( "ListTasksNotEmpty",			CClientAPI::ListTasksNotEmptyL ),
        ENTRY( "DeleteTask",				CClientAPI::DeleteTaskL ),
        ENTRY( "ChangeProtocolVer",			CClientAPI::ChangeProtocolVerL ),
        // dm profile
        ENTRY( "CreateDMProfile", 			CClientAPI::CreateDMProfileL ),
        ENTRY( "OpenDMProfile",	 			CClientAPI::OpenDMProfileL ),
        ENTRY( "OpenDMProfileById",			CClientAPI::OpenDMProfileByIdL ),
        ENTRY( "OpenSameDMProfile",	 		CClientAPI::OpenSameDMProfileL ),
        ENTRY( "UpdateDMProfile",	 		CClientAPI::UpdateDMProfileL ),
        ENTRY( "CloseDMProfile", 			CClientAPI::CloseDMProfile ),
        ENTRY( "SetDMProfileData",			CClientAPI::SetDMProfileDataL ),
       ENTRY( "SetInvDMProfileProtocol",	CClientAPI::SetInvDMProfileProtocolL ),
        ENTRY( "CheckInitialDMProfileData",	CClientAPI::CheckInitialDMProfileDataL ),
        ENTRY( "CheckDMProfileData",		CClientAPI::CheckDMProfileDataL ),
        ENTRY( "CheckDMProfileId",			CClientAPI::CheckDMProfileId ),
        ENTRY( "CheckDMProfileDelete",		CClientAPI::CheckDMProfileDeletion ),
        ENTRY( "DeleteDMConnection",		CClientAPI::DeleteDMConnectionL ),
        // transport
        ENTRY( "OpenTransport",		        CClientAPI::OpenTransportL ),
        ENTRY( "OpenTransportInv",		    CClientAPI::OpenTransportInvL ),
        ENTRY( "ReadTransportValues",       CClientAPI::ReadTransportValues ),
        ENTRY( "ReadTransportProperties",   CClientAPI::ReadTransportProperties ),
        ENTRY( "ReadTransportPropertiesInv",CClientAPI::ReadTransportPropertiesInv ),
        ENTRY( "StartListening",            CClientAPI::TransportStartListeningL ),
        ENTRY( "StopListening",             CClientAPI::TransportStopListeningL ),
        ENTRY( "CloseTransport",	        CClientAPI::CloseTransport ),
        // data provider
        ENTRY( "OpenDataProvider",		    CClientAPI::OpenDataProviderL ),
        ENTRY( "OpenDataProviderInv",	    CClientAPI::OpenDataProviderInvL ),
        ENTRY( "ReadDataProviderValues",    CClientAPI::ReadDataProviderValues ),
        ENTRY( "CheckDataProviderFilters",	CClientAPI::CheckDataProviderFiltersL ),
        ENTRY( "CloseDataProvider",	        CClientAPI::CloseDataProvider ),
        // connection
        ENTRY( "OpenConnection",		    CClientAPI::OpenConnectionL ),
        ENTRY( "OpenConnectionById",		CClientAPI::OpenConnectionByIdL ),
        ENTRY( "CreateConnection",		    CClientAPI::CreateConnectionL ),
        ENTRY( "CreateConnNewProf",		    CClientAPI::CreateConnectionWithNewProfileL ),
        ENTRY( "CheckInitialConnectionData",CClientAPI::CheckInitialConnectionDataL ),
        ENTRY( "CheckConnectionProps",		CClientAPI::CheckConnectionPropertiesL ),
        ENTRY( "CheckDMConnectionProps",	CClientAPI::CheckDMConnectionPropertiesL ),
        ENTRY( "CheckConnectionData",		CClientAPI::CheckConnectionDataL ),
        ENTRY( "SetConnectionData",		    CClientAPI::SetConnectionDataL ),
        ENTRY( "SetConnectionProps",		CClientAPI::SetConnectionPropertiesL ),
        ENTRY( "SetInvConnectionProp",		CClientAPI::SetInvConnectionPropertyL ),
        ENTRY( "GetInvConnectionProp",		CClientAPI::GetInvConnectionPropertyL ),
        ENTRY( "UpdateConnection",		    CClientAPI::UpdateConnectionL ),
        ENTRY( "CloseConnection",		    CClientAPI::CloseConnection ),
        ENTRY( "LeaveIfConnNotReadOnly",	CClientAPI::LeaveIfConnectionNotReadOnlyL ),
        // task
        ENTRY( "OpenTask",				    CClientAPI::OpenTaskL ),
        ENTRY( "OpenTaskById",				CClientAPI::OpenTaskByIdL ),
        ENTRY( "CreateTask",			    CClientAPI::CreateTaskL ),
        ENTRY( "CheckInitialTaskData",	    CClientAPI::CheckInitialTaskDataL ),
        ENTRY( "CheckTaskData",			    CClientAPI::CheckTaskDataL ),
        ENTRY( "CheckTaskId",			    CClientAPI::CheckTaskId ),
        ENTRY( "CheckTaskFilters",		    CClientAPI::CheckTaskFiltersL ),
        ENTRY( "SetTaskData",			    CClientAPI::SetTaskDataL ),
        ENTRY( "UpdateTask",			    CClientAPI::UpdateTaskL ),
        ENTRY( "CloseTask",				    CClientAPI::CloseTask ),
        ENTRY( "LeaveIfTaskNotReadOnly",	CClientAPI::LeaveIfTaskNotReadOnlyL ),
        // history log
        ENTRY( "OpenHistoryLog",		    CClientAPI::OpenHistoryLogL ),
        ENTRY( "OpenEmptyHistoryLog",		CClientAPI::OpenEmptyHistoryLogL ),
        ENTRY( "OpenHistoryLogInv",		    CClientAPI::OpenHistoryLogInvL ),
        ENTRY( "ReadHistoryLogValues",      CClientAPI::ReadHistoryLogValues ),
        ENTRY( "CloseHistoryLog",	        CClientAPI::CloseHistoryLog ),
        // ds job
        ENTRY( "OpenDSJob",			        CClientAPI::OpenDSJobL ),
        ENTRY( "OpenDSJobById",		        CClientAPI::OpenDSJobByIdL ),
        ENTRY( "StopDSJob",			        CClientAPI::StopDSJobL ),
        ENTRY( "CreateDSJobProf",			CClientAPI::CreateDSJobForProfileL ),
        ENTRY( "CreateDSJobTrans",			CClientAPI::CreateDSJobForTransportL ),
        ENTRY( "CreateDSJobTransInv",		CClientAPI::CreateDSJobForTransportInvL ),
        ENTRY( "CreateDSJobProfST",			CClientAPI::CreateDSJobForProfileSTL ),
        ENTRY( "CreateDSJobTransST",		CClientAPI::CreateDSJobForTransportSTL ),
        ENTRY( "CreateDSJobTasks",			CClientAPI::CreateDSJobForTasksL ),
        ENTRY( "CreateDSJobTransTasks",		CClientAPI::CreateDSJobForTransportAndTasksL ),
        ENTRY( "CreateDSJobTasksST",		CClientAPI::CreateDSJobForTasksSTL ),
        ENTRY( "CreateDSJobTransTasksST",	CClientAPI::CreateDSJobForTransportAndTasksSTL ),
        ENTRY( "CreateDSJobs",				CClientAPI::CreateDSJobsL ),
        ENTRY( "CheckDSJobData",			CClientAPI::CheckDSJobDataL ),
        ENTRY( "CheckDSJobDataTasks",		CClientAPI::CheckDSJobDataWithTasksL ),
        ENTRY( "CloseDSJob",				CClientAPI::CloseDSJob ),
        ENTRY( "DSJobError",				CClientAPI::DSJobError ),
        // dm job
        ENTRY( "OpenDMJob",			        CClientAPI::OpenDMJobL ),
        ENTRY( "OpenDMJobById",				CClientAPI::OpenDMJobByIdL ),
        ENTRY( "StopDMJob",			        CClientAPI::StopDMJobL ),
        ENTRY( "CreateDMJobProf",			CClientAPI::CreateDMJobL ),
        ENTRY( "CreateDMJobs",				CClientAPI::CreateDMJobsL ),
        ENTRY( "CreateDMJobTrans",			CClientAPI::CreateDMJobForTransportL ),
        ENTRY( "CreateDMJobTransInv",		CClientAPI::CreateDMJobForTransportInvL ),
        ENTRY( "CheckDMJobData",			CClientAPI::CheckDMJobDataL ),
        ENTRY( "CloseDMJob",				CClientAPI::CloseDMJob ),
        // settings
        ENTRY( "OpenSettings",				CClientAPI::OpenSettingsL ),
        ENTRY( "GetSettingsValue",			CClientAPI::GetSettingsValueL ),
        ENTRY( "SetSettingsValue",			CClientAPI::SetSettingsValueL ),
        ENTRY( "CloseSettings",				CClientAPI::CloseSettings ),
        // devman
        ENTRY( "OpenDevMan",				CClientAPI::OpenDevManL ),
        ENTRY( "ClearDevMan",				CClientAPI::ClearDevManL ),
        ENTRY( "GetDevMan",					CClientAPI::GetDevManL ),
        ENTRY( "SetDevMan",					CClientAPI::SetDevManL ),
        ENTRY( "CloseDevMan",				CClientAPI::CloseDevManL ),
        // misc
        ENTRY( "CapabilityTests", 			CClientAPI::CapabilityTestsL ),
        ENTRY( "LargeTests", 				CClientAPI::LargeTestsL ),
        ENTRY( "LargeTestsWithWait",		CClientAPI::LargeTestsWithWaitL ),
        //filters
        //filter transfers + matchtype + supportsuserselectable
        ENTRY( "FilterCreateTaskAndUpdate",		CClientAPI::FilterCreateTaskAndUpdateL), //Create + update task
        ENTRY( "FilterOpenTaskAndUpdate",		CClientAPI::FilterOpenTaskAndUpdateL), //Open + update task
        ENTRY( "FilterOpenTaskAndCreate",		CClientAPI::FilterOpenTaskAndCreateL), //Open + delete + create + update task       
        ENTRY( "FilterMatchTypeChange",		CClientAPI::FilterMatchTypeChangeL), //Open + delete + create + update task       
        
        };

    const TInt count = sizeof( KFunctions ) / 
                        sizeof( TStifFunctionInfo );

    return RunInternalL( KFunctions, count, aItem );

    }    

// -----------------------------------------------------------------------------
// CClientAPI::ExampleL
// Example test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CClientAPI::ExampleL( CStifItemParser& aItem )
    {
    
    TInt addr = 0;
    aItem.GetNextInt( addr );
    
    iLog->Log( _L("In ExampleL received integer %d"), addr );
    return KErrNone;
  
    }
    

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
TInt CClientAPI::InitSettingsL( CStifItemParser& /*aItem*/ )
	{
	RSyncMLSession session;
	session.OpenL();
	CleanupClosePushL( session );
	
	RSyncMLDataSyncProfile dsProf;
	dsProf.CreateL( session );
	CleanupClosePushL( dsProf );
	dsProf.UpdateL();

	RSyncMLTask task;
	task.CreateL( dsProf, 270495198, KTaskServerDataSource, KTaskClientDataSource ); // agenda
	CleanupClosePushL( task );
	task.UpdateL();
	dsProf.UpdateL();
	
	CleanupStack::PopAndDestroy(); // task
	CleanupStack::PopAndDestroy(); // dsProf
	
	RSyncMLDevManProfile dmProf;
	dmProf.CreateL( session );
	CleanupClosePushL( dmProf );
	dmProf.UpdateL();
	
	CleanupStack::PopAndDestroy(); // dmProf
	CleanupStack::PopAndDestroy(); // session
	
	return KErrNone;
	}
	
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
// Session
// -----------------------------------------------------------------------------

TInt CClientAPI::OpenSyncSessionL( CStifItemParser& /*aItem*/ )
	{
	iSyncSession.OpenL();
	iSyncSession.ShareAuto(); // share automatically with other threads (notifiers)
	return KErrNone;
	}
	
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
TInt CClientAPI::OpenSecondSyncSessionL( CStifItemParser& /*aItem*/ )
	{
	RSyncMLSession session;
	session.OpenL();
	session.Close();
	return KErrNone;
	}
	
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
TInt CClientAPI::CloseSyncSession( CStifItemParser& /*aItem*/ )
	{
	iSyncSession.Close();
	return KErrNone;
	}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
TInt CClientAPI::CurrentJobL( CStifItemParser& aItem )
	{
	TInt jobId = 0;
	TSmlUsageType type = UsageType( aItem );
	iSyncSession.CurrentJobL( jobId, type );
	
	if ( jobId <= 0 )
		return KErrNone;
	}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
TInt CClientAPI::ListQueuedJobsL( CStifItemParser& aItem )
	{
	TSmlUsageType type = UsageType( aItem );
	TInt count = 0;
	aItem.GetNextInt( count );
	
	RArray<TSmlJobId> array;
	CleanupClosePushL( array );
	iSyncSession.ListQueuedJobsL( array, type );
	
	TInt ret = KErrNone;
	if ( count == 0 && count != array.Count() )
		{
		ret = KErrGeneral;
		}
	else if ( count != 0 && array.Count() <= 0 )
		{
		ret = KErrGeneral;
		}
	
	CleanupStack::PopAndDestroy(); // array
	
	return ret;
	}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
TInt CClientAPI::ListTransportsL( CStifItemParser& /*aItem*/ )
	{
	RArray<TSmlTransportId> transports;
	CleanupClosePushL( transports );
	
	TInt ret = KErrNone;
	iSyncSession.ListTransportsL( transports );
	
	// there is no way to know which transports should be in the list, 
	// so check with debugger. Here is just amount check (at least internet
	// should be supported).
	if ( transports.Count() <= 0 )
		{
		ret = KErrGeneral;
		}
	
	CleanupStack::PopAndDestroy(); // transports
	return ret;
	}
	
// -----------------------------------------------------------------------------
// Lists data providers and checks that there are enough of them. Give 
// the count as parameter.
// -----------------------------------------------------------------------------
TInt CClientAPI::ListDataProvidersL( CStifItemParser& /*aItem*/ )
	{
	RArray<TSmlDataProviderId> adapters;
	CleanupClosePushL( adapters );
	
	iSyncSession.ListDataProvidersL( adapters );
	
	TInt ret = KErrNone;
	if ( adapters.Count() <= 0 )
		{
		// not enough adapters found, something wrong
		ret = KErrGeneral;
		}
	
	CleanupStack::PopAndDestroy(); // adapters
	
	return ret;
	}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
TInt CClientAPI::ListProfilesL( CStifItemParser& aItem )
	{
	TSmlUsageType type = UsageType( aItem );
	
	RArray<TSmlProfileId> profiles;
	CleanupClosePushL( profiles );
	
	iSyncSession.ListProfilesL( profiles, type );
	
	TInt ret = KErrNone;
	if ( profiles.Count() <= 0 )
		{
		ret = KErrGeneral;
		}
	
	CleanupStack::PopAndDestroy();
	return ret;
	}
	
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
TInt CClientAPI::ListProtocolVersionsL( CStifItemParser& aItem )
	{
	TSmlUsageType type = UsageType( aItem );
	
	RArray<TSmlProtocolVersion> versions;
	CleanupClosePushL( versions );
	
	TSmlProtocolVersion dVersion = iSyncSession.ListProtocolVersionsL( type, versions );
	
	TInt ret = KErrNone;
	switch( type )
		{
		case ESmlDataSync: // default 1.2, in the list 1.2 and 1.1.2
			{
			if ( dVersion != ESmlVersion1_2 || versions[0] != ESmlVersion1_2 
				|| versions[1] != ESmlVersion1_1_2 || versions.Count() > 2)
				{
				ret = KErrGeneral;
				}
			}
			break;
		case ESmlDevMan: // default 1.1.2, in the list 1.1.2
			{
			if ( dVersion != ESmlVersion1_2 || versions[0] != ESmlVersion1_2 
			                || versions[1] != ESmlVersion1_1_2 || versions.Count() > 2)
				{
				ret = KErrGeneral;
				}
			}
			break;
		default:
			ret = KErrGeneral;
			break;
		}
	
	CleanupStack::PopAndDestroy(); // versions
	
	return ret;
	}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
TInt CClientAPI::DeleteProfileL( CStifItemParser& aItem )
	{
	TInt id = 0;
	aItem.GetNextInt( id );
	
	iSyncSession.DeleteProfileL( id );
	
	return KErrNone;
	}
	
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
TInt CClientAPI::DeleteDSProfileL( CStifItemParser& /*aItem*/ )
	{
	TInt ret = KErrGeneral;
	RArray<TSmlProfileId> profiles;
	CleanupClosePushL( profiles );
	
	iSyncSession.ListProfilesL( profiles, ESmlDataSync );
	
	TBool profileFound = EFalse;
	for ( TInt i = 0; i < profiles.Count(); ++i )
		{
		if ( profiles[i] == iProfileId )
			profileFound = ETrue;
		}
	
	if ( profileFound )
		{
		iSyncSession.DeleteProfileL( iProfileId );
		ret = KErrNone;
		}
	
	CleanupStack::PopAndDestroy(); // profiles
	
	return ret;
	}
	
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
TInt CClientAPI::DeleteDMProfileL( CStifItemParser& /*aItem*/ )
	{
	TInt ret = KErrGeneral;
	RArray<TSmlProfileId> profiles;
	CleanupClosePushL( profiles );
	
	iSyncSession.ListProfilesL( profiles, ESmlDevMan );
	
	TBool profileFound = EFalse;
	for ( TInt i = 0; i < profiles.Count(); ++i )
		{
		if ( profiles[i] == iProfileId )
			profileFound = ETrue;
		}
	
	if ( profileFound )
		{
		iSyncSession.DeleteProfileL( iProfileId );
		ret = KErrNone;
		}
	
	CleanupStack::PopAndDestroy(); // profiles
	
	return ret;
	}
	
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
TInt CClientAPI::RequestEvent( CStifItemParser& /*aItem*/ )
	{
	return StartEventThread();
	}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
TInt CClientAPI::ChangeEventL( CStifItemParser& /*aItem*/ )
	{
	iEventCallback2 = new (ELeave) EventCallback2();
	TRAPD(err, iSyncSession.RequestEventL( *iEventCallback2 ));
	iLog->Log(_L("Err valur: %d"),err);
	return KErrNone;
	}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
TInt CClientAPI::CancelEvent( CStifItemParser& /*aItem*/ )
	{
	iSyncSession.CancelEvent();
	return KErrNone;
	}
	
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
TInt CClientAPI::RequestProgress( CStifItemParser& /*aItem*/ )
	{
	return StartProgressThread();
	}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
TInt CClientAPI::ChangeProgressL( CStifItemParser& /*aItem*/ )
	{
	iProgressCallback2 = new (ELeave) ProgressCallback2();
	iSyncSession.RequestProgressL( *iProgressCallback2 );
	return KErrNone;
	}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
TInt CClientAPI::CancelProgress( CStifItemParser& /*aItem*/ )
	{
	iSyncSession.CancelProgress();
	return KErrNone;
	}
	

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
// DS profile
// -----------------------------------------------------------------------------

TInt CClientAPI::CreateDSProfileL( CStifItemParser& aItem )
	{
	TInt protocol = 0;
	aItem.GetNextInt( protocol );
	
	iDSProfile.CreateL( iSyncSession );
	iDSProfile.SetProtocolVersionL((TSmlProtocolVersion) protocol );
	return KErrNone;
	}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
TInt CClientAPI::OpenDSProfileL( CStifItemParser& aItem )
	{
	TSmlOpenMode mode = OpenMode( aItem );
	
	RArray<TSmlProfileId> profiles;
	CleanupClosePushL( profiles );
	
	TRAPD( err, iSyncSession.ListProfilesL( profiles, ESmlDataSync ) );
	iLog->Log(_L("ListProfilesL, err value: %d"),err);
	
	//iDSProfile.OpenL( iSyncSession, profiles[profiles.Count()-1], mode );
	TRAPD( err1, iDSProfile.OpenL( iSyncSession, profiles[profiles.Count()-1], mode ) );
	iLog->Log(_L("OpenL, err value: %d"),err1);
	
	CleanupStack::PopAndDestroy(); // profiles
	
	return KErrNone;
	}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
TInt CClientAPI::OpenDSProfileByIdL( CStifItemParser& aItem )
	{
	TInt id = -2;
	aItem.GetNextInt( id );
	
	if ( id == -2 )
		{
		iDSProfile.OpenL( iSyncSession, iProfileId, ESmlOpenReadWrite );
		}
	else
		{
		iDSProfile.OpenL( iSyncSession, id, ESmlOpenReadWrite );
		}
	
	return KErrNone;
	}
	
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
TInt CClientAPI::OpenDSProfileByIdReadOnlyL( CStifItemParser& /*aItem*/ )
	{
	iDSProfile.OpenL( iSyncSession, iProfileId, ESmlOpenRead );
	return KErrNone;
	}
	
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
TInt CClientAPI::OpenSameDSProfileL( CStifItemParser& /*aItem*/ )
	{
	RSyncMLDataSyncProfile prof;
	prof.OpenL( iSyncSession, iDSProfile.Identifier(), ESmlOpenReadWrite );
	prof.Close();
	return KErrNone;
	}
	
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
TInt CClientAPI::UpdateDSProfileL( CStifItemParser& /*aItem*/ )
	{
	iDSProfile.UpdateL();
	iProfileId = iDSProfile.Identifier();
	return KErrNone;
	}
	
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
TInt CClientAPI::CloseDSProfile( CStifItemParser& /*aItem*/ )
	{
	iDSProfile.Close();
	return KErrNone;
	}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
TInt CClientAPI::SetDSProfileDataL( CStifItemParser& /*aItem*/ )
	{
	iDSProfile.SetCreatorId( KDSProfileCreatorId );
	iDSProfile.SetDisplayNameL( KDSProfileDisplayName );
	iDSProfile.SetUserNameL( KDSProfileUserName );
	iDSProfile.SetPasswordL( KDSProfilePassword );
	iDSProfile.SetServerIdL( KDSProfileServerId );
	iDSProfile.SetServerPasswordL( KDSProfileServerPasswd );
	iDSProfile.SetSanUserInteractionL( KDSProfileSAN );
	iDSProfile.SetProtocolVersionL( KDSProfileProtocol );
	
	return KErrNone;
	}

// -----------------------------------------------------------------------------
// Calls DS profiles each getter and checks that data is in its initial state.
// -----------------------------------------------------------------------------
TInt CClientAPI::CheckInitialDSProfileDataL( CStifItemParser& /*aItem*/ )
	{
	TL( iDSProfile.Identifier() == -1 );
	TL( iDSProfile.Type() == ESmlDataSync );
	TL( iDSProfile.CreatorId() ==  -1 );
	TL( iDSProfile.DisplayName() == _L("") );
	TL( iDSProfile.UserName() == _L8("") );
	TL( iDSProfile.Password() == _L8("") );
	TL( iDSProfile.ServerId() == _L8("") );
	TL( iDSProfile.ServerPassword() == _L8("") );
	TL( iDSProfile.SanUserInteraction() == ESmlConfirmSync );
	TL( iDSProfile.ProtocolVersion() == ESmlVersion1_1_2 );
	TL( iDSProfile.IsReadOnly() == EFalse );
	TL( iDSProfile.DeleteAllowed() );
	
	return KErrNone;
	}
	
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
TInt CClientAPI::CheckDSProfileDataL( CStifItemParser& aItem )
	{
	TSmlOpenMode mode = OpenMode( aItem );
	
	TL( iDSProfile.Identifier() >= 0 );
	TL( iDSProfile.Type() == ESmlDataSync );
	TL( iDSProfile.CreatorId() == KDSProfileCreatorId );
	TL( iDSProfile.DisplayName() == KDSProfileDisplayName );
	TL( iDSProfile.UserName() == KDSProfileUserName );
	TL( iDSProfile.Password() == KDSProfilePassword );
	TL( iDSProfile.ServerId() == KDSProfileServerId );
	TL( iDSProfile.ServerPassword() == _L8("") );
	TL( iDSProfile.SanUserInteraction() == KDSProfileSAN );
	TL( iDSProfile.ProtocolVersion() == KDSProfileProtocol );
	
	if ( iDSProfile.Identifier() <= 1 )
		{ // PC suite
		TL( !iDSProfile.DeleteAllowed() );
		}
	else
		{ // other
		TL( iDSProfile.DeleteAllowed() );
		}

	if ( mode == ESmlOpenRead )	
		{
		TL( iDSProfile.IsReadOnly() ); // opened in read mode
	}
	else if ( mode == ESmlOpenReadWrite )
		{
		TL( !iDSProfile.IsReadOnly() ); // opened in read/write mode
	}
		
	return KErrNone;
	}
	
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
TInt CClientAPI::CheckDSProfileId( CStifItemParser& /*aItem*/ )
	{
	if ( iDSProfile.Identifier() <= 0 )
		{
		return KErrGeneral;
		}
		
	return KErrNone;
	}
	
// -----------------------------------------------------------------------------
// Returns an error id DS profiles deletion is allowed.
// -----------------------------------------------------------------------------
TInt CClientAPI::CheckDSProfileDeletion( CStifItemParser& /*aItem*/ )
	{
	if ( iDSProfile.DeleteAllowed() )
		{
		return KErrGeneral;
		}
		
	return KErrNone;
	}
	
// -----------------------------------------------------------------------------
// deleting connections should not be supported.
// -----------------------------------------------------------------------------
TInt CClientAPI::DeleteDSConnectionByIdL( CStifItemParser& /*aItem*/ )
	{
	//Deleting connection is not supported so it leave with KErrNotSupported, Calling 
	//'DeleteConnectionByConnectionIdL' function calls 'DeleteConnectionL' func internally
	TRAPD(err, iDSProfile.DeleteConnectionByConnectionIdL( 0 ));
	if ( err == KErrNotSupported )
	{
		return KErrNone;
	}
	else
	{
		return KErrGeneral;
	}
	}
TInt CClientAPI::DeleteDSConnectionL( CStifItemParser& /*aItem*/ )
	{
	TRAPD(err, iDSProfile.DeleteConnectionL( 0 ));
	if ( err == KErrNotSupported )
	{
		return KErrNone;
	}
	else
	{
		return KErrGeneral;
	}
	}	
	
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
TInt CClientAPI::ListTasksEmptyL( CStifItemParser& /*aItem*/ )
	{
	RArray<TSmlTaskId> tasks;
	CleanupClosePushL( tasks );
	
	iDSProfile.ListTasksL( tasks );
	TL( tasks.Count() == 0 );
	
	CleanupStack::PopAndDestroy();
	
	return KErrNone;
	}
	
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
TInt CClientAPI::ListTasksNotEmptyL( CStifItemParser& /*aItem*/ )
	{
	RArray<TSmlTaskId> tasks;
	CleanupClosePushL( tasks );
	
	iDSProfile.ListTasksL( tasks );
	TL( tasks.Count() > 0 );
	
	CleanupStack::PopAndDestroy();
	
	return KErrNone;
	}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
TInt CClientAPI::DeleteTaskL( CStifItemParser& aItem )
	{
	TInt id = -2;
	aItem.GetNextInt( id );
	
	if ( id == -2 )
		{
		iDSProfile.DeleteTaskL( iTaskId );
		}
	else
		{
		iDSProfile.DeleteTaskL( id );
		}
	
	return KErrNone;
	}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
TInt CClientAPI::ChangeProtocolVerL( CStifItemParser& aItem )
	{
	TSmlOpenMode mode = OpenMode( aItem );
	
	RArray<TSmlProfileId> profiles;
	CleanupClosePushL( profiles );
	
	iSyncSession.ListProfilesL( profiles, ESmlDataSync );
	
	iDSProfile.OpenL( iSyncSession, profiles[profiles.Count()-1], mode );
	
	TInt protocol = 1;
	iDSProfile.SetProtocolVersionL((TSmlProtocolVersion) protocol ); 
	
	iDSProfile.UpdateL();
	
	CleanupStack::PopAndDestroy(); // profiles
	
	return KErrNone;
	}
	


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
// DM profile
// -----------------------------------------------------------------------------

TInt CClientAPI::CreateDMProfileL( CStifItemParser& /*aItem*/ )
	{
	iDMProfile.CreateL( iSyncSession );
	return KErrNone;
	}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
TInt CClientAPI::OpenDMProfileL( CStifItemParser& aItem )
	{
	TSmlOpenMode mode = OpenMode( aItem );
	
	RArray<TSmlProfileId> profiles;
	CleanupClosePushL( profiles );
	
	iSyncSession.ListProfilesL( profiles, ESmlDevMan );
	
	iDMProfile.OpenL( iSyncSession, profiles[profiles.Count()-1], mode );
	
	CleanupStack::PopAndDestroy(); // profiles
	
	return KErrNone;
	}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
TInt CClientAPI::OpenDMProfileByIdL( CStifItemParser& aItem )
	{
	TInt id = 0;
	aItem.GetNextInt( id );
	
	iDMProfile.OpenL( iSyncSession, id, ESmlOpenReadWrite );
	
	return KErrNone;
	}
	
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
TInt CClientAPI::OpenSameDMProfileL( CStifItemParser& /*aItem*/ )
	{
	RSyncMLDevManProfile prof;
	prof.OpenL( iSyncSession, iDMProfile.Identifier(), ESmlOpenReadWrite );
	prof.Close();
	return KErrNone;
	}
	
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
TInt CClientAPI::UpdateDMProfileL( CStifItemParser& /*aItem*/ )
	{
	iDMProfile.UpdateL();
	iProfileId = iDMProfile.Identifier();
	return KErrNone;
	}
	
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
TInt CClientAPI::CloseDMProfile( CStifItemParser& /*aItem*/ )
	{
	iDMProfile.Close();
	return KErrNone;
	}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
TInt CClientAPI::SetDMProfileDataL( CStifItemParser& /*aItem*/ )
	{
	iDMProfile.SetCreatorId( KDMProfileCreatorId );
	iDMProfile.SetDisplayNameL( KDMProfileDisplayName );
	iDMProfile.SetUserNameL( KDMProfileUserName );
	iDMProfile.SetPasswordL( KDMProfilePassword );
	iDMProfile.SetServerIdL( KDMProfileServerId );
	iDMProfile.SetServerPasswordL( KDMProfileServerPasswd );
	iDMProfile.SetSanUserInteractionL( KDMProfileSAN );
	iDMProfile.SetProtocolVersionL( KDMProfileProtocol );
	
	return KErrNone;
	}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
TInt CClientAPI::SetInvDMProfileProtocolL( CStifItemParser& /*aItem*/ )
	{
	iDMProfile.SetProtocolVersionL( ESmlVersion1_2 ); // should leave with KErrNotSupported
	return KErrNone;
	}
	
// -----------------------------------------------------------------------------
// Calls DS profiles each getter and checks that data is in its initial state.
// -----------------------------------------------------------------------------
TInt CClientAPI::CheckInitialDMProfileDataL( CStifItemParser& /*aItem*/ )
	{
	TL( iDMProfile.Identifier() == -1 );
	TL( iDMProfile.Type() == ESmlDevMan );
	TL( iDMProfile.CreatorId() ==  -1 );
	TL( iDMProfile.DisplayName() == _L("") );
	TL( iDMProfile.UserName() == _L8("") );
	TL( iDMProfile.Password() == _L8("") );
	TL( iDMProfile.ServerId() == _L8("") );
	TL( iDMProfile.ServerPassword() == _L8("") );
	TL( iDMProfile.SanUserInteraction() == ESmlConfirmSync );
	TL( iDMProfile.ProtocolVersion() == ESmlVersion1_1_2 );
	TL( iDMProfile.IsReadOnly() == EFalse );
	TL( iDMProfile.DeleteAllowed() );
	
	return KErrNone;
	}
	
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
TInt CClientAPI::CheckDMProfileDataL( CStifItemParser& aItem )
	{
	TSmlOpenMode mode = OpenMode( aItem );
	
	TL( iDMProfile.Identifier() > 0 );
	TL( iDMProfile.Type() == ESmlDevMan );
	TL( iDMProfile.CreatorId() == KDMProfileCreatorId );
	TL( iDMProfile.DisplayName() == KDMProfileDisplayName );
	TL( iDMProfile.UserName() == KDMProfileUserName );
	TL( iDMProfile.Password() == KDMProfilePassword );
	TL( iDMProfile.ServerId() == KDMProfileServerId );
	TL( iDMProfile.ServerPassword() == KDMProfileServerPasswd );
	TL( iDMProfile.SanUserInteraction() == KDMProfileSAN );
	TL( iDMProfile.ProtocolVersion() == KDMProfileProtocol );
	TL( iDMProfile.DeleteAllowed() );

	if ( mode == ESmlOpenRead )	
		{ TL( iDMProfile.IsReadOnly() ); // opened in read mode 
			}
	else if ( mode == ESmlOpenReadWrite )
		{ TL( !iDMProfile.IsReadOnly() ); // opened in read/write mode 
			}
		
	return KErrNone;
	}
	
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
TInt CClientAPI::CheckDMProfileId( CStifItemParser& /*aItem*/ )
	{
	if ( iDMProfile.Identifier() <= 0 )
		{
		return KErrGeneral;
		}
		
	return KErrNone;
	}
	
// -----------------------------------------------------------------------------
// Returns an error id DS profiles deletion is allowed.
// -----------------------------------------------------------------------------
TInt CClientAPI::CheckDMProfileDeletion( CStifItemParser& /*aItem*/ )
	{
	if ( iDMProfile.DeleteAllowed() )
		{
		return KErrGeneral;
		}
		
	return KErrNone;
	}
	
// -----------------------------------------------------------------------------
// deleting connections should not be supported.
// -----------------------------------------------------------------------------
TInt CClientAPI::DeleteDMConnectionL( CStifItemParser& /*aItem*/ )
	{
	TRAPD(err, iDMProfile.DeleteConnectionByConnectionIdL( 0 ));
	if ( err == KErrNotSupported )
	{
		return KErrNone;
	}
	else
	{
		return KErrGeneral;
	}	}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
// Transport
// -----------------------------------------------------------------------------

TInt CClientAPI::OpenTransportL( CStifItemParser& /*aItem*/ )
	{
	RArray<TInt> transports;
	CleanupClosePushL( transports );
	
	iSyncSession.ListTransportsL( transports );	
	iConnectionId = transports[0];
	iTransport.OpenL( iSyncSession, iConnectionId );
	
	CleanupStack::PopAndDestroy(); // transports	
	return KErrNone;
	}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
TInt CClientAPI::OpenTransportInvL( CStifItemParser& /*aItem*/ )
	{
	iTransport.OpenL( iSyncSession, 0 );
	return KErrNone;
	}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
TInt CClientAPI::ReadTransportValues( CStifItemParser& /*aItem*/ )
	{
	TL( iTransport.Identifier() == iConnectionId );
    TL( iTransport.DisplayName() == _L("Internet") );
    TL( iTransport.SupportsCapability(ESmlCapCanListen) == EFalse );
	return KErrNone;
	}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
TInt CClientAPI::ReadTransportProperties( CStifItemParser& /*aItem*/ )
	{
	if ( iTransport.DisplayName() == _L("Internet") )
		{
	    TL( iTransport.Properties().Count() == 8 );
		}
	else
		{
	    TL( iTransport.Properties().Count() == 0 );
		}
		
	return KErrNone;
	}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
TInt CClientAPI::ReadTransportPropertiesInv( CStifItemParser& /*aItem*/ )
	{
    TL( iTransport.Properties().Count() == 0 );
	return KErrNone;
	}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
TInt CClientAPI::TransportStartListeningL( CStifItemParser& /*aItem*/ )
	{
    iTransport.StartListeningL();
	return KErrNone;
	}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
TInt CClientAPI::TransportStopListeningL( CStifItemParser& /*aItem*/ )
	{
    iTransport.StopListeningL();
	return KErrNone;
	}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
TInt CClientAPI::CloseTransport( CStifItemParser& /*aItem*/ )
	{
	iTransport.Close();
	return KErrNone;
	}
	


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
// Data provider
// -----------------------------------------------------------------------------
TInt CClientAPI::OpenDataProviderL( CStifItemParser& aItem )
	{
	TInt id = 0;
	aItem.GetNextInt( id );
	
	if ( !id )
		{
		RArray<TSmlDataProviderId> dataProviders;
		CleanupClosePushL( dataProviders );
		iSyncSession.ListDataProvidersL( dataProviders );
		id = dataProviders[0];
		CleanupStack::PopAndDestroy(); // dataProviders	
		}
		
	iDataProvider.OpenL( iSyncSession, id );
    TL( iDataProvider.Identifier() == id );
	
	return KErrNone;
	}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
TInt CClientAPI::OpenDataProviderInvL( CStifItemParser& /*aItem*/ )
	{
	iDataProvider.OpenL( iSyncSession, 0 );
	return KErrNone;
	}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
TInt CClientAPI::ReadDataProviderValues( CStifItemParser& /*aItem*/ )
	{
    TPtrC dName = iDataProvider.DisplayName();
    TL( dName != _L("") );
    
    for ( TInt i = 0; i < iDataProvider.MimeTypeCount(); ++i )
    	{
	    TPtrC type = iDataProvider.MimeType( i );
	    TL( type != _L("") );
    	TPtrC ver = iDataProvider.MimeVersion( i );
    	TL( ver != _L("") );
    	}
    	
    TPtrC name = iDataProvider.DefaultDataStoreName();
    TL( name != _L("") );
    
    TBool allowMultiple = iDataProvider.AllowsMultipleDataStores();

    CDesCArray* dsArray = new (ELeave) CDesCArrayFlat(1);
    iDataProvider.GetDataStoreNamesL(*dsArray);
    for ( TInt j = 0; j < dsArray->Count(); ++j )
    	{
    	TPtrC ds = (*dsArray)[j];
	    TL( ds != _L("") );
    	}
    delete dsArray;
    iDataProvider.RequiredProtocolVersionL();
    TVersion version = iDataProvider.Version();
    version.iMajor;
    version.iMinor;
    version.iBuild;
	return KErrNone;
	}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
TInt CClientAPI::CheckDataProviderFiltersL( CStifItemParser& /*aItem*/ )
	{
	//iDataProvider.SupportedServerFiltersL();
	return KErrNotSupported;
	}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
TInt CClientAPI::CloseDataProvider( CStifItemParser& /*aItem*/ )
	{
	iDataProvider.Close();
	return KErrNone;
	}
	


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
// Connection
// -----------------------------------------------------------------------------

TInt CClientAPI::OpenConnectionL( CStifItemParser& aItem )
	{
	SetConnectionProfileL( aItem );
	
	RArray<TSmlTransportId> conns;
	//iConnectionProfile->ListConnectionsL( conns );
	iConnectionProfile->ListConnectionsByConnectionIdL( conns );
	CleanupClosePushL( conns );
	
	iConnectionId = conns[0];
	iConnection.OpenL( *iConnectionProfile, iConnectionId );
	
	CleanupStack::PopAndDestroy(); // conns
	
	return KErrNone;
	}
	
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
TInt CClientAPI::OpenConnectionByIdL( CStifItemParser& aItem )
	{
	SetConnectionProfileL( aItem );

	TInt id = 0;
	aItem.GetNextInt( id );
	iConnectionId = id;
	
	iConnection.OpenByConnectionIdL( *iConnectionProfile, id );
	
	return KErrNone;
	}
	
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
TInt CClientAPI::CreateConnectionL( CStifItemParser& aItem )
	{
	TInt id = 0;
	aItem.GetNextInt( id );
	iConnectionId = id;
	
	iConnection.CreateL( *iConnectionProfile, id );
	
	return KErrNone;
	}
	
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
TInt CClientAPI::CreateConnectionWithNewProfileL( CStifItemParser& aItem )
	{
	TSmlUsageType type = UsageType( aItem );
	
	if ( type == ESmlDataSync )
		{
		RSyncMLDataSyncProfile prof;
		prof.CreateL( iSyncSession );
		CleanupClosePushL( prof );
		
		iConnection.CreateL( prof, 0 );
		
		CleanupStack::PopAndDestroy(); // prof
		}
	else
		{
		RSyncMLDevManProfile prof;
		prof.CreateL( iSyncSession );
		CleanupClosePushL( prof );
		
		iConnection.CreateL( prof, 0 );
		
		CleanupStack::PopAndDestroy(); // prof
		}
	
	return KErrNone;
	}
	
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
TInt CClientAPI::CheckInitialConnectionDataL( CStifItemParser& /*aItem*/ )
	{
	TSmlUsageType type = iConnectionProfile->Type();
	
	TL( iConnection.Identifier() == iConnectionId );
	TL( iConnection.ConnectionId() == iConnectionId );
	TL( iConnection.Profile() == iConnectionProfile->Identifier() );
	TL( iConnection.Priority() == 0 );
	TL( iConnection.RetryCount() == 0 );
	TL( iConnection.ServerURI() == _L8("") );
	TL( iConnection.IsReadOnly() == iConnectionProfile->IsReadOnly() );
	
	RSyncMLTransport transport;
	transport.OpenL( iSyncSession, iConnectionId );
	CleanupClosePushL( transport );
	const CSyncMLTransportPropertiesArray& props = transport.Properties();
	
	if ( type == ESmlDataSync )
		{ // DS
		for ( TInt i = 0; i < props.Count(); ++i )
			{
			const TSyncMLTransportPropertyInfo info = props.At( i );
			if ( info.iDataType == EDataTypeNumber || info.iDataType == EDataTypeBoolean )
				{
				TL ( iConnection.GetPropertyL( info.iName ) == _L8("0") );
				}
			else
				{
				TL ( iConnection.GetPropertyL( info.iName ) == _L8("") );
				}
			}
		}
	else
		{ // DM, only iapid should be found
		TL( iConnection.GetPropertyL( _L8("NSmlIapId") ) == _L8("0") );
		for ( TInt i = 1; i < props.Count(); ++i )
			{
			const TSyncMLTransportPropertyInfo info = props.At( i );
			TRAPD(err, iConnection.GetPropertyL( info.iName ) );
			if ( err != KErrNotFound )
				User::Leave( KErrGeneral );
			}
		}
		
	CleanupStack::PopAndDestroy(); // transport
	
	return KErrNone;
	}
	
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
TInt CClientAPI::CheckConnectionPropertiesL( CStifItemParser& /*aItem*/ )
	{
	RSyncMLTransport transport;
	transport.OpenL( iSyncSession, iConnectionId );
	CleanupClosePushL( transport );
	const CSyncMLTransportPropertiesArray& props = transport.Properties();
	
	for ( TInt i = 0; i < props.Count(); ++i )
		{
		const TSyncMLTransportPropertyInfo info = props.At( i );
		if ( info.iDataType == EDataTypeNumber || info.iDataType == EDataTypeBoolean )
			{
			TL ( iConnection.GetPropertyL( info.iName ) == _L8("1") );
			}
		else
			{
			TL ( iConnection.GetPropertyL( info.iName ) == _L8("prop") );
			}
		}
	
	CleanupStack::PopAndDestroy(); // transport
	
	return KErrNone;
	}
	
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
TInt CClientAPI::CheckDMConnectionPropertiesL( CStifItemParser& /*aItem*/ )
	{
	RSyncMLTransport transport;
	transport.OpenL( iSyncSession, iConnectionId );
	CleanupClosePushL( transport );
	const CSyncMLTransportPropertiesArray& props = transport.Properties();
	
	for ( TInt i = 1; i < props.Count(); ++i ) // start from second, every get should leave
		{
		const TSyncMLTransportPropertyInfo info = props.At( i );
		TRAPD(err, iConnection.GetPropertyL( info.iName ) );
		if ( err != KErrNotFound )
			User::Leave( KErrGeneral );
		}
	
	CleanupStack::PopAndDestroy(); // transport
	
	TL( iConnection.GetPropertyL( _L8("NSmlIapId") ) == _L8("100") );
	
	return KErrNone;
	}
	
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
TInt CClientAPI::CheckConnectionDataL( CStifItemParser& /*aItem*/ )
	{
	TL( iConnection.Identifier() == iConnectionId );
	TL( iConnection.ConnectionId() == iConnectionId );
	TL( iConnection.Profile() == iConnectionProfile->Identifier() );
	TL( iConnection.Priority() == 0 );
	TL( iConnection.RetryCount() == 0 );
	TL( iConnection.ServerURI() == _L8("http:\\www.sync.server.fi:80") );
	TL( iConnection.IsReadOnly() == iConnectionProfile->IsReadOnly() );
	
	return KErrNone;
	}
	
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
TInt CClientAPI::SetConnectionDataL( CStifItemParser& /*aItem*/ )
	{
	iConnection.SetPriority( 100 );
	iConnection.SetRetryCount( 22 );
	iConnection.SetServerURIL( _L8("http:\\www.sync.server.fi:80") );
	
	return KErrNone;
	}
	
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
TInt CClientAPI::SetConnectionPropertiesL( CStifItemParser& aItem )
	{
	TSmlUsageType type = UsageType( aItem );
	
	RSyncMLTransport transport;
	transport.OpenL( iSyncSession, iConnectionId );
	CleanupClosePushL( transport );
	const CSyncMLTransportPropertiesArray& props = transport.Properties();
	
	if ( type == ESmlDataSync )
		{ // DS
		for ( TInt i = 0; i < props.Count(); ++i )
			{
			const TSyncMLTransportPropertyInfo info = props.At( i );
			if ( info.iDataType == EDataTypeNumber || info.iDataType == EDataTypeBoolean )
				{
				iConnection.SetPropertyL( info.iName, _L8("1") );
				}
			else
				{
				iConnection.SetPropertyL( info.iName, _L8("prop") );
				}
			}
		}
	else
		{ // DM
		iConnection.SetPropertyL( _L8("NSmlIapId"), _L8("100") );
		for ( TInt i = 1; i < props.Count(); ++i ) // start from second, every set should leave
			{
			const TSyncMLTransportPropertyInfo info = props.At( i );
			TRAPD(err, iConnection.SetPropertyL( info.iName, _L8("") ) );
			if ( err != KErrNotFound )
				User::Leave( KErrGeneral );
			}
		}
		
	CleanupStack::PopAndDestroy(); // transport
	
	return KErrNone;
	}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
TInt CClientAPI::SetInvConnectionPropertyL( CStifItemParser& /*aItem*/ )
	{
	iConnection.SetPropertyL( _L8("inv"), _L8("inv") );
	return KErrNone;
	}
	
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
TInt CClientAPI::GetInvConnectionPropertyL( CStifItemParser& /*aItem*/ )
	{
	iConnection.GetPropertyL( _L8("inv") );
	return KErrNone;
	}
	
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
TInt CClientAPI::UpdateConnectionL( CStifItemParser& /*aItem*/ )
	{
	iConnection.UpdateL();
	return KErrNone;
	}
	
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
TInt CClientAPI::CloseConnection( CStifItemParser& /*aItem*/ )
	{
	iConnection.Close();
	return KErrNone;
	}
	
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
TInt CClientAPI::LeaveIfConnectionNotReadOnlyL( CStifItemParser& /*aItem*/ )
	{
	if ( !iConnection.IsReadOnly() )
		User::Leave( KErrGeneral );
	
	return KErrNone;
	}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
// History log
// -----------------------------------------------------------------------------

TInt CClientAPI::OpenHistoryLogL( CStifItemParser& aItem )
	{
	TInt id;
	aItem.GetNextInt( id );
	
	RArray<TInt> profiles;
	CleanupClosePushL( profiles );
	
	if ( id == -2 )
		{
		iSyncSession.ListProfilesL( profiles, ESmlDataSync );	
		iHistoryLog.OpenL( iSyncSession, profiles[0] );
    	TL( iHistoryLog.Profile() == profiles[0] );	
		}
	else
		{
		iHistoryLog.OpenL( iSyncSession, id );
    	TL( iHistoryLog.Profile() == id );	
		}
		
	CleanupStack::PopAndDestroy(); // profiles	
	return KErrNone;
	}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
TInt CClientAPI::OpenEmptyHistoryLogL( CStifItemParser& /*aItem*/ )
	{
	RSyncMLDataSyncProfile prof;
	prof.CreateL( iSyncSession );
	prof.UpdateL();
	
	TInt id = prof.Identifier();
	prof.Close();

	iHistoryLog.OpenL( iSyncSession, id );
    TL( iHistoryLog.Profile() == id );	
	
	return KErrNone;
	}
	
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
TInt CClientAPI::OpenHistoryLogInvL( CStifItemParser& /*aItem*/ )
	{
	iHistoryLog.OpenL( iSyncSession, -1 );
	return KErrNone;
	}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
TInt CClientAPI::ReadHistoryLogValues( CStifItemParser& /*aItem*/ )
	{
    iHistoryLog.SortEntries( CSyncMLHistoryEntry::ESortByTime );

    for ( TInt i(0) ; i < iHistoryLog.Count() ; i++ )
        {
       	iLog->Log(_L("B4Caling Entry"));
        iHistoryLog.Entry(i);
        iLog->Log(_L("B4Caling Entry"));
        }

    iHistoryLog.SortEntries( CSyncMLHistoryEntry::ESortByType );   

    for ( TInt i(0) ; i < iHistoryLog.Count() ; i++ )
        {
        iHistoryLog.operator[](i);
        }

    iHistoryLog.DeleteAllEntriesL();
    TL(iHistoryLog.Count() == 0 );
    iHistoryLog.DeleteAllEntriesL();
    iLog->Log(_L("At the end of readhistory"));
    
	return KErrNone;
	}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
TInt CClientAPI::CloseHistoryLog( CStifItemParser& /*aItem*/ )
	{
	iHistoryLog.Close();
	return KErrNone;
	}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
// Task
// -----------------------------------------------------------------------------

TInt CClientAPI::OpenTaskL( CStifItemParser& /*aItem*/ )
	{
	RArray<TSmlTaskId> tasks;
	CleanupClosePushL( tasks );
	
	iDSProfile.ListTasksL( tasks );
	iTask.OpenL( iDSProfile, tasks[0] ); // open the first in list
	
	CleanupStack::PopAndDestroy(); // tasks
	
	return KErrNone;
	}
	
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
TInt CClientAPI::OpenTaskByIdL( CStifItemParser& aItem )
	{
	TInt id = 0;
	aItem.GetNextInt( id );
	
	iTask.OpenL( iDSProfile, id );
	
	return KErrNone;
	}
	
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
TInt CClientAPI::CreateTaskL( CStifItemParser& aItem )
	{
	TInt dataProvider = 0;
	aItem.GetNextInt( dataProvider );
	
	if ( dataProvider == 0 ) // not given as parameter, get from session
		{
		RArray<TSmlDataProviderId> adapters;
		CleanupClosePushL( adapters );
		iSyncSession.ListDataProvidersL( adapters );
		dataProvider = adapters[0];
		CleanupStack::PopAndDestroy(); // adapters
		}
	
	iDataProviderId = dataProvider;
	
	iTask.CreateL( iDSProfile, dataProvider, KTaskServerDataSource, KTaskClientDataSource );
	
	return KErrNone;
	}
	
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
TInt CClientAPI::CheckInitialTaskDataL( CStifItemParser& /*aItem*/ )
	{
	TL( iTask.Identifier() == -1 );
	TL( iTask.Profile() == iDSProfile.Identifier() );
	TL( iTask.CreatorId() == -1 );
	TL( iTask.DisplayName() == _L("") );
	TL( iTask.ServerDataSource() == KTaskServerDataSource );
	TL( iTask.ClientDataSource() == KTaskClientDataSource );
	TL( iTask.DataProvider() == iDataProviderId );
	TL( iTask.DefaultSyncType() == ESmlTwoWay );
	TL( iTask.Enabled() );
	TL( ! iTask.IsReadOnly() );
	
	return KErrNone;
	}
	
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
TInt CClientAPI::CheckTaskDataL( CStifItemParser& /*aItem*/ )
	{
	TL( iTask.Identifier() > 0 );
	TL( iTask.Profile() == iDSProfile.Identifier() );
	TL( iTask.CreatorId() == KTaskCreatorId );
	TL( iTask.DisplayName() == KTaskDisplayName );
	TL( iTask.ServerDataSource() == KTaskServerDataSource );
	TL( iTask.ClientDataSource() == KTaskClientDataSource );
	TL( iTask.DataProvider() == iDataProviderId );
	TL( iTask.DefaultSyncType() == KTaskSyncType );
	TL( iTask.Enabled() == KTaskEnabled );
	TL( iTask.IsReadOnly() == iDSProfile.IsReadOnly() );
	
	return KErrNone;
	}
	
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
TInt CClientAPI::CheckTaskId( CStifItemParser& /*aItem*/ )
	{
	if ( iTask.Identifier() <= 0 )
		{
		return KErrGeneral;
		}
		
	return KErrNone;
	}
	
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
TInt CClientAPI::CheckTaskFiltersL( CStifItemParser& /*aItem*/ )
	{
	/*TRAPD( err, iTask.GetSupportedServerFiltersL( 0 ) );
	ASSERT( err == KErrNotSupported );
	ASSERT( iTask.ServerFilterCount() == 0 );
	ASSERT( iTask.FilterMatchType() == ESyncMLMatchDefault );
	
	CSyncMLFilter* filter = NULL;
	
	TRAP( err, iTask.ReplaceServerFilterL( 0, *filter ) );
	ASSERT( err == KErrNotSupported );
	
	TRAP( err, iTask.AddServerFilterL( *filter ) );
	ASSERT( err == KErrNotSupported );
	
	TRAP( err, iTask.DeleteServerFilterL( 0 ) );
	ASSERT( err == KErrNotSupported );
	
	TRAP( err, iTask.DeleteAllServerFiltersL() );
	ASSERT( err == KErrNotSupported );
	
	TRAP( err, iTask.SetServerFilterEnabledL( 0, ETrue ) );
	ASSERT( err == KErrNotSupported );
	
	TRAP( err, iTask.SetFilterMatchTypeL( ESyncMLMatchDefault ) );
	ASSERT( err == KErrNotSupported );
	*/
	return KErrNone;
	}
	
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
TInt CClientAPI::SetTaskDataL( CStifItemParser& /*aItem*/ )
	{
	iTask.SetCreatorId( KTaskCreatorId );
	iTask.SetDisplayNameL( KTaskDisplayName );
	iTask.SetDefaultSyncTypeL( KTaskSyncType );
	iTask.SetEnabledL( KTaskEnabled );
	return KErrNone;
	}
	
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
TInt CClientAPI::UpdateTaskL( CStifItemParser& /*aItem*/ )
	{
	iTask.UpdateL();
	iTaskId = iTask.Identifier();
	return KErrNone;
	}
	
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
TInt CClientAPI::CloseTask( CStifItemParser& /*aItem*/ )
	{
	iTask.Close();
	return KErrNone;
	}
	
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
TInt CClientAPI::LeaveIfTaskNotReadOnlyL( CStifItemParser& /*aItem*/ )
	{
	if ( ! iTask.IsReadOnly() )
		User::Leave( KErrGeneral );
	
	return KErrNone;
	}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
// DS Job
// -----------------------------------------------------------------------------

TInt CClientAPI::OpenDSJobL( CStifItemParser& /*aItem*/ )
	{
	TInt ret = KErrNone;
	
	RArray<TSmlJobId> jobs;
	CleanupClosePushL( jobs );
	
	iSyncSession.ListQueuedJobsL( jobs, ESmlDataSync );
	
	if ( !jobs.Count() )
		{
		ret = KErrGeneral;
		}
	else
		{
		iJobId = jobs[ jobs.Count()-1 ];
		iDSJob.OpenL( iSyncSession, iJobId );
		}
		
	CleanupStack::PopAndDestroy(); // jobs
	
	return ret;
	}
	
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
TInt CClientAPI::OpenDSJobByIdL( CStifItemParser& aItem )
	{
	TInt id = 0;
	aItem.GetNextInt( id );
	
	if ( id )
		{
		iJobId = id;
		}
	
	iDSJob.OpenL( iSyncSession, iJobId );
	
	return KErrNone;
	}
	
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
TInt CClientAPI::StopDSJobL( CStifItemParser& /*aItem*/ )
	{
	iDSJob.StopL();
	return KErrNone;
	}
	
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
TInt CClientAPI::CreateDSJobForProfileL( CStifItemParser& aItem )
	{
	TInt id = -2;
	aItem.GetNextInt( id );
	
	ListProfilesL( ESmlDataSync );
	if ( id == -2 )
		{
		iDSJob.CreateL( iSyncSession, iProfiles[0] );
		iJobId = iDSJob.Identifier();
		}
	else
		{
		iDSJob.CreateL( iSyncSession, id );
		iJobId = iDSJob.Identifier();
		}
	
/*	TInt i = 0;
	for ( i = 0; i < iProfiles.Count() && i < 5; ++i )
		{
		RSyncMLDataSyncJob j1;
		j1.CreateL( iSyncSession, iProfiles[i] );
		j1.Close();
		}*/
	
	return KErrNone;
	}
	
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
TInt CClientAPI::CreateDSJobForTransportL( CStifItemParser& aItem )
	{
	TInt id = 0;
	aItem.GetNextInt( id );
	
	ListProfilesL( ESmlDataSync );
	ListTransportsL();
	if ( id == 0 )
		{
		iDSJob.CreateL( iSyncSession, iProfiles[0], iTransports[0] );
		iJobId = iDSJob.Identifier();
		}
	else
		{
		iDSJob.CreateL( iSyncSession, id, iTransports[0] );
		iJobId = iDSJob.Identifier();
		}
	
	return KErrNone;
	}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
TInt CClientAPI::CreateDSJobForTransportInvL( CStifItemParser& /*aItem*/ )
	{
	ListProfilesL( ESmlDataSync );
	iDSJob.CreateL( iSyncSession, iProfiles[0], -2 );
	iJobId = iDSJob.Identifier();
	
	return KErrNone;
	}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
TInt CClientAPI::CreateDSJobForProfileSTL( CStifItemParser& aItem )
	{
	TInt id = 0;
	aItem.GetNextInt( id );
	
	ListProfilesL( ESmlDataSync );
	if ( id == 0 )
		{
		iDSJob.CreateL( iSyncSession, iProfiles[0], ESmlSlowSync );
		iJobId = iDSJob.Identifier();
		}
	else
		{
		iDSJob.CreateL( iSyncSession, id, ESmlSlowSync );
		iJobId = iDSJob.Identifier();
		}
	
	return KErrNone;
	}
	
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
TInt CClientAPI::CreateDSJobForTransportSTL( CStifItemParser& aItem )
	{
	TInt id = 0;
	aItem.GetNextInt( id );
	
	ListProfilesL( ESmlDataSync );
	ListTransportsL();
	if ( id == 0 )
		{
		iDSJob.CreateL( iSyncSession, iProfiles[0], iTransports[0], ESmlSlowSync );
		iJobId = iDSJob.Identifier();
		}
	else
		{
		iDSJob.CreateL( iSyncSession, id, iTransports[0], ESmlSlowSync );
		iJobId = iDSJob.Identifier();
		}
	
	return KErrNone;
	}
	
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
TInt CClientAPI::CreateDSJobForTasksL( CStifItemParser& aItem )
	{
	TInt id = 0;
	aItem.GetNextInt( id );
	
	ListProfilesL( ESmlDataSync );
	ListTasksL();
	if ( id == 0 )
		{
		iDSJob.CreateL( iSyncSession, iProfiles[0], iTasks );
		iJobId = iDSJob.Identifier();
		}
	else
		{
		iDSJob.CreateL( iSyncSession, id, iTasks );
		iJobId = iDSJob.Identifier();
		}
	
	return KErrNone;
	}
	
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
TInt CClientAPI::CreateDSJobForTransportAndTasksL( CStifItemParser& aItem )
	{
	TInt id = 0;
	aItem.GetNextInt( id );
	
	ListProfilesL( ESmlDataSync );
	ListTransportsL();
	ListTasksL();
	if ( id == 0 )
		{
		iDSJob.CreateL( iSyncSession, iProfiles[0], iTransports[0], iTasks );
		iJobId = iDSJob.Identifier();
		}
	else
		{
		iDSJob.CreateL( iSyncSession, id, iTransports[0], iTasks );
		iJobId = iDSJob.Identifier();
		}
	
	return KErrNone;
	}
	
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
TInt CClientAPI::CreateDSJobForTasksSTL( CStifItemParser& aItem )
	{
	TInt id = 0;
	aItem.GetNextInt( id );
	
	ListProfilesL( ESmlDataSync );
	ListTasksL();
	if ( id == 0 )
		{
		iDSJob.CreateL( iSyncSession, iProfiles[0], ESmlSlowSync, iTasks );
		iJobId = iDSJob.Identifier();
		}
	else
		{
		iDSJob.CreateL( iSyncSession, id, ESmlSlowSync, iTasks );
		iJobId = iDSJob.Identifier();
		}
	
	return KErrNone;
	}
	
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
TInt CClientAPI::CreateDSJobForTransportAndTasksSTL( CStifItemParser& aItem )
	{
	TInt id = 0;
	aItem.GetNextInt( id );
	
	ListProfilesL( ESmlDataSync );
	ListTransportsL();
	ListTasksL();
	if ( id == 0 )
		{
		iDSJob.CreateL( iSyncSession, iProfiles[0], iTransports[0], ESmlSlowSync, iTasks );
		iJobId = iDSJob.Identifier();
		}
	else
		{
		iDSJob.CreateL( iSyncSession, id, iTransports[0], ESmlSlowSync, iTasks );
		iJobId = iDSJob.Identifier();
		}
	
	return KErrNone;
	}
	
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
TInt CClientAPI::CreateDSJobsL( CStifItemParser& /*aItem*/ )
	{
	iProfiles.Reset();
	ListProfilesL( ESmlDataSync );
	
	for ( TInt i = 0; i < iProfiles.Count() && i < 3; ++i )
		{
		RSyncMLDataSyncJob dsJob;
		dsJob.CreateL( iSyncSession, iProfiles[i] );
		dsJob.Close();
		}
		
	return KErrNone;
	}
	
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
TInt CClientAPI::CheckDSJobDataL( CStifItemParser& /*aItem*/ )
	{
	// tasks
	RArray<TSmlTaskId> tasks = iDSJob.TaskIDs();
	TL( tasks.Count() == 0 );
	
	// id
	TL( iDSJob.Identifier() == iJobId );
	
	// profile
	TL( iDSJob.Profile() == iProfiles[0] );
	
	return KErrNone;
	}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
TInt CClientAPI::CheckDSJobDataWithTasksL( CStifItemParser& /*aItem*/ )
	{
	// tasks
	RArray<TSmlTaskId> tasks = iDSJob.TaskIDs();
	for ( TInt i = 0; i < tasks.Count(); ++i )
		{
		TInt t1 = iTasks[i];
		TInt t2 = tasks[i];
		TL( iTasks[i] == tasks[i] );
		}
	
	// id
	TL( iDSJob.Identifier() == iJobId );
	
	// profile
	TL( iDSJob.Profile() == iProfiles[0] );
	
	return KErrNone;
	}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
TInt CClientAPI::CloseDSJob( CStifItemParser& /*aItem*/ )
	{
	iDSJob.Close();
	return KErrNone;
	}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
TInt CClientAPI::DSJobError( CStifItemParser& /*aItem*/ )
	{
	//creating profile
	_LIT( KTaskClientErrDataSource, "c:Kalendar" );
	TInt retError(KErrNone);
	ProgressCallbackErrorTest *progressCallbackET = new (ELeave) ProgressCallbackErrorTest();
	iSyncSession.RequestProgressL( *progressCallbackET );
	
	RSyncMLDataSyncProfile dsProf;
	dsProf.CreateL( iSyncSession );
	CleanupClosePushL( dsProf );
	dsProf.UpdateL();

	//creating task
	RSyncMLTask task;
	task.CreateL( dsProf, 270495198, KTaskServerDataSource, KTaskClientErrDataSource ); // agenda
	CleanupClosePushL( task );
	task.UpdateL();
	dsProf.UpdateL();
	
	dsProf.ListTasksL( iTasks );
	
	iDSJob.CreateL( iSyncSession, dsProf.Identifier(), iTasks );
	iJobId = iDSJob.Identifier();
	
	dsProf.DeleteTaskL( task.Identifier() );
	CleanupStack::PopAndDestroy(); // task
	CleanupStack::PopAndDestroy(); // dsProf
	
	iSyncSession.CancelProgress();
	if ( progressCallbackET->iCaseFailed )
		{
		retError = KErrGeneral;
		}
		
	delete progressCallbackET;
	
	return retError;
	}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
// DM Job
// -----------------------------------------------------------------------------

TInt CClientAPI::OpenDMJobL( CStifItemParser& /*aItem*/ )
	{
	TInt ret = KErrNone;
	
	RArray<TSmlJobId> jobs;
	CleanupClosePushL( jobs );
	
	iSyncSession.ListQueuedJobsL( jobs, ESmlDevMan );
	
	if ( !jobs.Count() )
		{
		ret = KErrGeneral;
		}
	else
		{
		iJobId = jobs[0];
		iDMJob.OpenL( iSyncSession, iJobId );
		}
		
	CleanupStack::PopAndDestroy(); // jobs
	
	return KErrNone;
	}
	
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
TInt CClientAPI::OpenDMJobByIdL( CStifItemParser& aItem )
	{
	TInt id = 0;
	aItem.GetNextInt( id );
	
	if ( id )
		{
		iJobId = id;
		}
	
	iDMJob.OpenL( iSyncSession, iJobId );
	
	return KErrNone;
	}
	
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
TInt CClientAPI::StopDMJobL( CStifItemParser& /*aItem*/ )
	{
	iDMJob.StopL();
	return KErrNone;
	}
	
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
TInt CClientAPI::CreateDMJobL( CStifItemParser& aItem )
	{
	TInt id = 0;
	aItem.GetNextInt( id );
	
	ListProfilesL( ESmlDevMan );
	
	if ( id == 0 )
		{
		iDMJob.CreateL( iSyncSession, iProfiles[0] );
		iJobId = iDMJob.Identifier();
		}
	else 
		{
		iDMJob.CreateL( iSyncSession, id );
		iJobId = iDMJob.Identifier();
		}
	
	return KErrNone;
	}
	
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
TInt CClientAPI::CreateDMJobsL( CStifItemParser& /*aItem*/ )
	{
	iProfiles.Reset();
	ListProfilesL( ESmlDevMan );
	
	for ( TInt i = 0; i < iProfiles.Count() && i < 3; ++i )
		{
		RSyncMLDevManJob dmJob;
		dmJob.CreateL( iSyncSession, iProfiles[i] );
		dmJob.Close();
		}
		
	return KErrNone;
	}
	
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
TInt CClientAPI::CreateDMJobForTransportL( CStifItemParser& aItem )
	{
	TInt id = 0;
	aItem.GetNextInt( id );
	
	ListProfilesL( ESmlDevMan );
	ListTransportsL();
	
	if ( id == 0 )
		{
		iDMJob.CreateL( iSyncSession, iProfiles[0], iTransports[0] );
		iJobId = iDMJob.Identifier();
		}
	else 
		{
		iDMJob.CreateL( iSyncSession, id, iTransports[0] );
		iJobId = iDMJob.Identifier();
		}
	
	return KErrNone;
	}
	
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
TInt CClientAPI::CreateDMJobForTransportInvL( CStifItemParser& /*aItem*/ )
	{
	ListProfilesL( ESmlDevMan );
	iDMJob.CreateL( iSyncSession, iProfiles[0], -2 );
	
	return KErrNone;
	}
	
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
TInt CClientAPI::CheckDMJobDataL( CStifItemParser& /*aItem*/ )
	{
	// id
	TL( iDMJob.Identifier() == iJobId );
	
	// profile
	TL( iDMJob.Profile() == iProfiles[0] );
	
	return KErrNone;
	}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
TInt CClientAPI::CloseDMJob( CStifItemParser& /*aItem*/ )
	{
	iDMJob.Close();
	return KErrNone;
	}
	

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
// DevMan
// -----------------------------------------------------------------------------

TInt CClientAPI::OpenDevManL( CStifItemParser& /*aItem*/ )
	{
	iDevMan.OpenL( iSyncSession );
	return KErrNone;
	}
	
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
TInt CClientAPI::ClearDevManL( CStifItemParser& /*aItem*/ )
	{
	iDevMan.ClearRootAclL();
	return KErrNone;
	}
	
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
TInt CClientAPI::SetDevManL( CStifItemParser& /*aItem*/ )
	{
	iDevMan.SetUserInteractionNotifierTimeoutL( 0 );
	return KErrNone;
	}
	
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
TInt CClientAPI::GetDevManL( CStifItemParser& /*aItem*/ )
	{
	TL( iDevMan.UserInteractionNotifierTimeout() == 0 );
	return KErrNone;
	}
	
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
TInt CClientAPI::CloseDevManL( CStifItemParser& /*aItem*/ )
	{
	iDevMan.Close();
	return KErrNone;
	}
	


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
// Settings
// -----------------------------------------------------------------------------

TInt CClientAPI::OpenSettingsL( CStifItemParser& /*aItem*/ )
	{
	TRAPD( err, iSettings.OpenL( iSyncSession ) );
	//Source code is returning 'KErrNotSupported' as RSyncMLSettings is not supported
	if( err == KErrNotSupported )
	{
		return KErrNone;
	}
	else
	{
		return KErrGeneral;
	}
}
	
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
TInt CClientAPI::GetSettingsValueL( CStifItemParser& /*aItem*/ )
	{
	TRAPD(err,iSettings.GetValueL( ESmlSettingDisplayBgResult ));
	//Source code is returning 'KErrNotSupported' as RSyncMLSettings is not supported
	if( err == KErrNotSupported )
	{
		return KErrNone;
	}
	else
	{
		return KErrGeneral;
	}
	}
	
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
TInt CClientAPI::SetSettingsValueL( CStifItemParser& /*aItem*/ )
	{
	TRAPD(err,iSettings.SetValueL( ESmlSettingDisplayBgResult, 0 ));
	//Source code is returning 'KErrNotSupported' as RSyncMLSettings is not supported
	if( err == KErrNotSupported )
	{
		return KErrNone;
	}
	else
	{
		return KErrGeneral;
	}
	}
	
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
TInt CClientAPI::CloseSettings( CStifItemParser& /*aItem*/ )
	{
	iSettings.Close();
	return KErrNone;
	}
	
        
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
// Misc
// -----------------------------------------------------------------------------

TInt CClientAPI::CapabilityTestsL( CStifItemParser& /*aItem*/ )
	{
	// DS tests
	RSyncMLDataSyncProfile dsProf;
	dsProf.CreateL( iSyncSession );
	CleanupClosePushL( dsProf );
	
	dsProf.UpdateL();
	
	TInt dsId = dsProf.Identifier();
	
	CleanupStack::PopAndDestroy(); // dsProf
	
	dsProf.OpenL( iSyncSession, dsId, ESmlOpenRead );
	dsProf.Close();
	
	dsProf.OpenL( iSyncSession, dsId, ESmlOpenReadWrite );
	dsProf.Close();
	
	iSyncSession.DeleteProfileL( dsId );
	
	// DM tests
	RSyncMLDevManProfile dmProf;
	dmProf.CreateL( iSyncSession );
	CleanupClosePushL( dmProf );
	
	dmProf.UpdateL();
	
	TInt dmId = dmProf.Identifier();
	
	CleanupStack::PopAndDestroy(); // dmProf
	
	dmProf.OpenL( iSyncSession, dmId, ESmlOpenRead );
	dmProf.Close();
	
	dmProf.OpenL( iSyncSession, dmId, ESmlOpenReadWrite );
	dmProf.Close();
	
	iSyncSession.DeleteProfileL( dmId );
	
	return KErrNone;
	}

TInt CClientAPI::LargeTestsWithWaitL( CStifItemParser& aItem )
	{
	TInt loopCount = 0;
	aItem.GetNextInt( loopCount );
	
	RSyncMLSession session;
	session.OpenL();
	CleanupClosePushL( session );
	
	for ( TInt i = 0; i < loopCount; ++i )
		{
		iDSProfile.CreateL( session );
		SetDSProfileDataL( aItem );
		UpdateDSProfileL( aItem );
		CloseDSProfile( aItem );
		}
	
	User::After( 30000000 );
	CleanupStack::PopAndDestroy(); // session
	
	return KErrNone;
	}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
TInt CClientAPI::LargeTestsL( CStifItemParser& aItem )
	{
	iLog->Log( _L("CClientAPI::LargeTestsL started") );
	
	TInt loopCount = 0;
	aItem.GetNextInt( loopCount );
	
	iLog->Log( _L("\tAmount of rounds: %d"), loopCount);
	
	RSyncMLSession session;
	session.OpenL();
	CleanupClosePushL( session );
	
	for ( TInt i = 0; i < loopCount; ++i )
		{
		iDSProfile.CreateL( session );
		SetDSProfileDataL( aItem );
		UpdateDSProfileL( aItem );
		TSmlProfileId id = iDSProfile.Identifier();
		CloseDSProfile( aItem );
		iDSProfile.OpenL( session, id, ESmlOpenRead );
		CloseDSProfile( aItem );
		}
	
	CleanupStack::PopAndDestroy(); // session
	
	iLog->Log( _L("CClientAPI::LargeTestsL ended") );
	return KErrNone;
	}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
TSmlUsageType CClientAPI::UsageType( CStifItemParser& aItem )
	{
	TInt type = -1;
	aItem.GetNextInt( type );
	
	switch ( type )
		{
		case 0:
			return ESmlDataSync;
			break;
		case 1:
			return ESmlDevMan;
			break;
		default:
			User::Leave( KErrArgument );
			break;
		}
		
	return ESmlDataSync;
	}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
TSmlOpenMode CClientAPI::OpenMode( CStifItemParser& aItem )
	{
	TInt mode = -1;
	aItem.GetNextInt( mode );
	
	switch ( mode )
		{
		case 0:
			return ESmlOpenRead;
			break;
		case 1:
			return ESmlOpenReadWrite;
			break;
		default:
			User::Leave( KErrArgument );
			break;
		}
		
	return ESmlOpenReadWrite;
	}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void CClientAPI::SetConnectionProfileL( CStifItemParser& aItem )
	{
	TSmlUsageType type = UsageType( aItem );
	
	if ( type == ESmlDataSync ) 
		iConnectionProfile = &iDSProfile;
	else if ( type == ESmlDevMan ) 
		iConnectionProfile = &iDMProfile;
	else 
		User::Leave( KErrArgument );
	}
	

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void CClientAPI::ListProfilesL( TSmlUsageType type )
	{
	TRAPD( err, iSyncSession.ListProfilesL( iProfiles, type ) );
	if ( err == KErrNotSupported ) // ds sync not supported, use dummy values
		{
		iProfiles.Append( 1 );
		}
	}
	

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void CClientAPI::ListTransportsL()
	{
	iSyncSession.ListTransportsL( iTransports );
	}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void CClientAPI::ListTasksL()
	{
	RSyncMLDataSyncProfile profile;
	CleanupClosePushL( profile );
	
	profile.OpenL( iSyncSession, iProfiles[0], ESmlOpenReadWrite );
	profile.ListTasksL( iTasks );
	
	if ( iTasks.Count() == 0 )
		{
		RSyncMLTask task;
		CleanupClosePushL( task );
		task.CreateL( profile, 270495197, KTaskServerDataSource, KTaskClientDataSource );
		task.UpdateL();
		profile.UpdateL();
		CleanupStack::PopAndDestroy(); // task
		iTasks.Reset();
		profile.ListTasksL( iTasks );
		}
	
	RSyncMLTask task;
	CleanupClosePushL( task );
	task.OpenL( profile, iTasks[0] );
	task.SetDefaultSyncTypeL( ESmlTwoWay );
	task.UpdateL();
	profile.UpdateL();
	CleanupStack::PopAndDestroy(); // task
		
	CleanupStack::PopAndDestroy(); // profile
	}

// -----------------------------------------------------------------------------
// Filter : Create + update task
// -----------------------------------------------------------------------------
TInt CClientAPI::FilterCreateTaskAndUpdateL( CStifItemParser& aItem )
	{
	TInt dataProvider = 0;
	aItem.GetNextInt( dataProvider );
	
	RSyncMLTask task;
	task.CreateL( iDSProfile, dataProvider, _L("server/calendar"), _L("Calendar") );
	CleanupClosePushL( task );
	
	TSyncMLFilterChangeInfo changeInfo;
	RPointerArray<CSyncMLFilter>& array = task.SupportedServerFiltersL( changeInfo );

	TInt filterCount = 0;
	aItem.GetNextInt( filterCount );
	
	//ASSERT(array.Count() == 0);
	task.UpdateL();
	array.Close();
	
	//ASSERT(changeInfo == ESyncMLDefault);
	//ASSERT(task.FilterMatchType() == ESyncMLMatchDisabled );
	//ASSERT(task.SupportsUserSelectableMatchType() == EFalse );
	CleanupStack::PopAndDestroy(); // task
	return KErrNone;
	}
    

// -----------------------------------------------------------------------------
// Filter : Open + update task
// -----------------------------------------------------------------------------
TInt CClientAPI::FilterOpenTaskAndUpdateL( CStifItemParser& /*aItem*/ )
	{
	RArray<TInt> providers;
	iDSProfile.ListTasksL( providers );
	TInt prov = providers[0];
	providers.Reset();
	providers.Close();
	
	RSyncMLTask task;
	task.OpenL( iDSProfile, prov );
	CleanupClosePushL( task );
	
	TSyncMLFilterChangeInfo changeInfo;
	RPointerArray<CSyncMLFilter>& array = task.SupportedServerFiltersL( changeInfo );

	array.Close();
	task.UpdateL();
	
	TL(changeInfo == ESyncMLDefault);
	TL(task.FilterMatchType() == ESyncMLMatchDisabled );
	TL(task.SupportsUserSelectableMatchType() == EFalse );
	CleanupStack::PopAndDestroy(); // task
	return KErrNone;
	}

// -----------------------------------------------------------------------------
// Filter :  Open + delete + create + update task       
// -----------------------------------------------------------------------------
TInt CClientAPI::FilterOpenTaskAndCreateL( CStifItemParser& aItem )
	{
	TInt dataProvider = 0;
	aItem.GetNextInt( dataProvider );
	
	RArray<TInt> providers;
	iDSProfile.ListTasksL( providers );
	TInt prov = providers[0];
	providers.Reset();
	providers.Close();
	
	RSyncMLTask task;
	task.OpenL( iDSProfile, prov );
	CleanupClosePushL( task );
	
	TSyncMLFilterChangeInfo changeInfo;
	RPointerArray<CSyncMLFilter>& array = task.SupportedServerFiltersL( changeInfo );
	TInt id = task.Identifier();
	CleanupStack::PopAndDestroy(); // task
	
	iDSProfile.DeleteTaskL(prov);
	
	RSyncMLTask task2;
	CleanupClosePushL( task2 );
	task2.CreateL(  iDSProfile, dataProvider, _L("server/calendar"), _L("Calendar"), array );
	task2.UpdateL();
	
	
	TL(changeInfo == ESyncMLDefault);
	TL(task2.FilterMatchType() == ESyncMLMatchDisabled );
	TL(task2.SupportsUserSelectableMatchType() == EFalse );
	
	CleanupStack::PopAndDestroy(); //task2
		
	
	return KErrNone;
	}


// -----------------------------------------------------------------------------
// Filter : 
// -----------------------------------------------------------------------------
TInt CClientAPI::FilterMatchTypeChangeL( CStifItemParser& /*aItem*/ )
	{
	const TDesC d(_L("F"));
	
	RArray<TInt> providers;
	iDSProfile.ListTasksL( providers );
	TInt prov = providers[0];
	providers.Reset();
	providers.Close();
	
	RSyncMLTask task;
	task.OpenL( iDSProfile, prov );
	CleanupClosePushL( task );
	task.SetFilterMatchTypeL( ESyncMLMatchDisabled );
	task.UpdateL();
	iDSProfile.UpdateL();
	CleanupStack::PopAndDestroy(); //task

	task.OpenL( iDSProfile, prov );
	CleanupClosePushL( task );
	TL( task.FilterMatchType() == ESyncMLMatchDisabled);
	TL( task.SupportsUserSelectableMatchType() == EFalse );
	task.SetFilterMatchTypeL( ESyncMLMatchNotSelected );
	task.UpdateL();
	iDSProfile.UpdateL();
	CleanupStack::PopAndDestroy(); //task

	return KErrNone;
	}
	
// ========================== OTHER EXPORTED FUNCTIONS =========================
// None

//  End of File
