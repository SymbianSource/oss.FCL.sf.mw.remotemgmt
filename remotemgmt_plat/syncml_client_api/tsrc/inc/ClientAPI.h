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



#ifndef CLIENTAPI_H
#define CLIENTAPI_H


//  INCLUDES
#include <StifLogger.h>
#include <TestScripterInternal.h>
#include <StifTestModule.h>

#define SYNCML_V3

#include <SyncMLClient.h>
#include <SyncMLClientDS.h>
#include <SyncMLClientDM.h>
#include <SyncMLObservers.h>
#include <SyncMLErr.h>

// CONSTANTS
//const ?type ?constant_var = ?constant;

// MACROS
//#define ?macro ?macro_def
// Logging path
_LIT( KClientAPILogPath, "\\logs\\testframework\\ClientAPI\\" ); 
// Log file
_LIT( KClientAPILogFile, "ClientAPI.txt" ); 

// FUNCTION PROTOTYPES
//?type ?function_name(?arg_list);
LOCAL_C TInt EventThreadFunction(TAny* aData);
LOCAL_C TInt StartEventThread();

// FORWARD DECLARATIONS
//class ?FORWARD_CLASSNAME;
class CClientAPI;
class EventCallback;
class EventCallback2;
class ProgressCallback2;

// DATA TYPES
//enum ?declaration
//typedef ?declaration
//extern ?data_type;


// GLOBALS
// session as global so that callback threads are able to access it
//RSyncMLSession sync_session;
//CStifLogger* event_logger; // logger for event callback
//CStifLogger* progress_logger; // logger for progress event callback

// CLASS DECLARATION

/**
*  CClientAPI test class for STIF Test Framework TestScripter.
*  ?other_description_lines
*
*  @lib ?library
*  @since ?Series60_version
*/
class CClientAPI : public CScriptBase
    {
    public:  // Constructors and destructor
        
        /**
        * Two-phased constructor.
        */
        static CClientAPI* NewL( CTestModuleIf& aTestModuleIf );
        
        /**
        * Destructor.
        */
        virtual ~CClientAPI();

    public: // New functions
        
        /**
        * ?member_description.
        * @since ?Series60_version
        * @param ?arg1 ?description
        * @return ?description
        */
        //?type ?member_function( ?type ?arg1 );

    public: // Functions from base classes

        /**
        * From CScriptBase Runs a script line.
        * @since ?Series60_version
        * @param aItem Script line containing method name and parameters
        * @return Symbian OS error code
        */
        virtual TInt RunMethodL( CStifItemParser& aItem );
                
    protected:  // New functions
        
        /**
        * ?member_description.
        * @since ?Series60_version
        * @param ?arg1 ?description
        * @return ?description
        */
        //?type ?member_function( ?type ?arg1 );

    protected:  // Functions from base classes
        
        /**
        * From ?base_class ?member_description
        */
        //?type ?member_function();

    private:

        /**
        * C++ default constructor.
        */
        CClientAPI( CTestModuleIf& aTestModuleIf );

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();

        // Prohibit copy constructor if not deriving from CBase.
        // ?classname( const ?classname& );
        // Prohibit assigment operator if not deriving from CBase.
        // ?classname& operator=( const ?classname& );
    
        /**
        * Frees all resources allocated from test methods.
        * @since ?Series60_version
        */
        void Delete();
        
        /**
        * Test methods are listed below. 
        */
        
        /**
        * Example test method.
        * @since ?Series60_version
        * @param aItem Script line containing parameters.
        * @return Symbian OS error code.
        */
		TInt ExampleL( CStifItemParser& aItem );
		
		TInt InitSettingsL( CStifItemParser& aItem );
		
		// session -->
		TInt OpenSyncSessionL( CStifItemParser& aItem );
		TInt OpenSecondSyncSessionL( CStifItemParser& aItem );
		TInt CloseSyncSession( CStifItemParser& aItem );
		
		TInt CurrentJobL( CStifItemParser& aItem );
		TInt ListQueuedJobsL( CStifItemParser& aItem );
		TInt ListTransportsL( CStifItemParser& aItem );
		TInt ListDataProvidersL( CStifItemParser& aItem );
		TInt ListProfilesL( CStifItemParser& aItem );
		TInt ListProtocolVersionsL( CStifItemParser& aItem );
		
		TInt DeleteProfileL( CStifItemParser& aItem );
		TInt DeleteDSProfileL( CStifItemParser& aItem );
		TInt DeleteDMProfileL( CStifItemParser& aItem );
		
		TInt RequestEvent( CStifItemParser& aItem );
		TInt ChangeEventL( CStifItemParser& aItem );
		TInt CancelEvent( CStifItemParser& aItem );
		
		TInt RequestProgress( CStifItemParser& aItem );
		TInt ChangeProgressL( CStifItemParser& aItem );
		TInt CancelProgress( CStifItemParser& aItem );
		
		// ds profile -->
		TInt CreateDSProfileL( CStifItemParser& aItem );
		TInt OpenDSProfileL( CStifItemParser& aItem );
		TInt OpenDSProfileByIdL( CStifItemParser& aItem );
		TInt OpenDSProfileByIdReadOnlyL( CStifItemParser& aItem );
		TInt OpenSameDSProfileL( CStifItemParser& aItem );
		TInt UpdateDSProfileL( CStifItemParser& aItem );
		TInt CloseDSProfile( CStifItemParser& aItem );
		TInt SetDSProfileDataL( CStifItemParser& aItem );
		TInt CheckInitialDSProfileDataL( CStifItemParser& aItem );
		TInt CheckDSProfileDataL( CStifItemParser& aItem );
		TInt CheckDSProfileId( CStifItemParser& aItem );
		TInt CheckDSProfileDeletion( CStifItemParser& aItem );
		TInt DeleteDSConnectionL( CStifItemParser& aItem );
		TInt ListTasksEmptyL( CStifItemParser& aItem );
		TInt ListTasksNotEmptyL( CStifItemParser& aItem );
		TInt DeleteTaskL( CStifItemParser& aItem );
		TInt ChangeProtocolVerL( CStifItemParser& aItem );
		
		// dm profile -->
		TInt CreateDMProfileL( CStifItemParser& aItem );
		TInt OpenDMProfileL( CStifItemParser& aItem );
		TInt OpenDMProfileByIdL( CStifItemParser& aItem );
		TInt OpenSameDMProfileL( CStifItemParser& aItem );
		TInt UpdateDMProfileL( CStifItemParser& aItem );
		TInt CloseDMProfile( CStifItemParser& aItem );
		TInt SetDMProfileDataL( CStifItemParser& aItem );
		TInt SetInvDMProfileProtocolL( CStifItemParser& aItem );
		TInt CheckInitialDMProfileDataL( CStifItemParser& aItem );
		TInt CheckDMProfileDataL( CStifItemParser& aItem );
		TInt CheckDMProfileId( CStifItemParser& aItem );
		TInt CheckDMProfileDeletion( CStifItemParser& aItem );
		TInt DeleteDMConnectionL( CStifItemParser& aItem );
		
		// transport -->
        TInt OpenTransportL( CStifItemParser& aItem );
        TInt OpenTransportInvL( CStifItemParser& aItem );
        TInt ReadTransportValues( CStifItemParser& aItem );
        TInt ReadTransportProperties( CStifItemParser& aItem );
        TInt ReadTransportPropertiesInv( CStifItemParser& aItem );
        TInt TransportStartListeningL( CStifItemParser& aItem );
        TInt TransportStopListeningL( CStifItemParser& aItem );
		TInt CloseTransport( CStifItemParser& aItem );

		// data provider -->
        TInt OpenDataProviderL( CStifItemParser& aItem );
        TInt OpenDataProviderInvL( CStifItemParser& aItem );
        TInt ReadDataProviderValues( CStifItemParser& aItem );
        TInt CheckDataProviderFiltersL( CStifItemParser& aItem );
        TInt CloseDataProvider( CStifItemParser& aItem );

		// connection -->
        TInt OpenConnectionL( CStifItemParser& aItem );
        TInt OpenConnectionByIdL( CStifItemParser& aItem );
        TInt CreateConnectionL( CStifItemParser& aItem );
        TInt CreateConnectionWithNewProfileL( CStifItemParser& aItem );
		TInt CheckInitialConnectionDataL( CStifItemParser& aItem );
		TInt CheckConnectionPropertiesL( CStifItemParser& aItem );
		TInt CheckDMConnectionPropertiesL( CStifItemParser& aItem );
		TInt CheckConnectionDataL( CStifItemParser& aItem );
		TInt SetConnectionDataL( CStifItemParser& aItem );
		TInt SetConnectionPropertiesL( CStifItemParser& aItem );
		TInt SetInvConnectionPropertyL( CStifItemParser& aItem );
		TInt GetInvConnectionPropertyL( CStifItemParser& aItem );
		TInt UpdateConnectionL( CStifItemParser& aItem );
        TInt CloseConnection( CStifItemParser& aItem );
        TInt LeaveIfConnectionNotReadOnlyL( CStifItemParser& aItem );
        TInt DeleteDSConnectionByIdL( CStifItemParser& aItem );

		// history log -->
        TInt OpenHistoryLogL( CStifItemParser& aItem );
        TInt OpenEmptyHistoryLogL( CStifItemParser& aItem );
        TInt OpenHistoryLogInvL( CStifItemParser& aItem );
        TInt ReadHistoryLogValues( CStifItemParser& aItem );
        TInt CloseHistoryLog( CStifItemParser& aItem );
        
        // task -->
        TInt OpenTaskL( CStifItemParser& aItem );
        TInt OpenTaskByIdL( CStifItemParser& aItem );
        TInt CreateTaskL( CStifItemParser& aItem );
		TInt CheckInitialTaskDataL( CStifItemParser& aItem );
		TInt CheckTaskDataL( CStifItemParser& aItem );
		TInt CheckTaskId( CStifItemParser& aItem );
		TInt CheckTaskFiltersL( CStifItemParser& aItem );
		TInt SetTaskDataL( CStifItemParser& aItem );
		TInt UpdateTaskL( CStifItemParser& aItem );
        TInt CloseTask( CStifItemParser& aItem );
        TInt LeaveIfTaskNotReadOnlyL( CStifItemParser& aItem );
		
        // ds job -->
        TInt OpenDSJobL( CStifItemParser& aItem );
        TInt OpenDSJobByIdL( CStifItemParser& aItem );
        TInt StopDSJobL( CStifItemParser& aItem );
        TInt CreateDSJobForProfileL( CStifItemParser& aItem );
        TInt CreateDSJobForTransportL( CStifItemParser& aItem );
        TInt CreateDSJobForTransportInvL( CStifItemParser& aItem );
        TInt CreateDSJobForProfileSTL( CStifItemParser& aItem );
        TInt CreateDSJobForTransportSTL( CStifItemParser& aItem );
        TInt CreateDSJobForTasksL( CStifItemParser& aItem );
        TInt CreateDSJobForTransportAndTasksL( CStifItemParser& aItem );
        TInt CreateDSJobForTasksSTL( CStifItemParser& aItem );
        TInt CreateDSJobForTransportAndTasksSTL( CStifItemParser& aItem );
        TInt CreateDSJobsL( CStifItemParser& aItem );
		TInt CheckDSJobDataL( CStifItemParser& aItem );
		TInt CheckDSJobDataWithTasksL( CStifItemParser& aItem );
        TInt CloseDSJob( CStifItemParser& aItem );
        TInt DSJobError( CStifItemParser& aItem );
        
        // dm job -->
        TInt OpenDMJobL( CStifItemParser& aItem );
        TInt OpenDMJobByIdL( CStifItemParser& aItem );
        TInt StopDMJobL( CStifItemParser& aItem );
        TInt CreateDMJobL( CStifItemParser& aItem );
        TInt CreateDMJobsL( CStifItemParser& aItem );
        TInt CreateDMJobForTransportL( CStifItemParser& aItem );
        TInt CreateDMJobForTransportInvL( CStifItemParser& aItem );
		TInt CheckDMJobDataL( CStifItemParser& aItem );
        TInt CloseDMJob( CStifItemParser& aItem );
        
        // settings -->
        TInt OpenSettingsL( CStifItemParser& aItem );
        TInt GetSettingsValueL( CStifItemParser& aItem );
        TInt SetSettingsValueL( CStifItemParser& aItem );
        TInt CloseSettings( CStifItemParser& aItem );
        
        // devman -->
        TInt OpenDevManL( CStifItemParser& aItem );
        TInt ClearDevManL( CStifItemParser& aItem );
        TInt SetDevManL( CStifItemParser& aItem );
        TInt GetDevManL( CStifItemParser& aItem );
        TInt CloseDevManL( CStifItemParser& aItem );
        
		// misc -->
		TInt CapabilityTestsL( CStifItemParser& aItem );
		TInt LargeTestsWithWaitL( CStifItemParser& aItem );
		TInt LargeTestsL( CStifItemParser& aItem );
		
		//filter		
		TInt FilterCreateTaskAndUpdateL( CStifItemParser& aItem );
		TInt FilterOpenTaskAndUpdateL( CStifItemParser& aItem );	
		TInt FilterOpenTaskAndCreateL( CStifItemParser& aItem );
		TInt FilterMatchTypeChangeL( CStifItemParser& aItem );
		
	private:
		TSmlUsageType UsageType( CStifItemParser& aItem );
		TSmlOpenMode OpenMode( CStifItemParser& aItem );
		void SetConnectionProfileL( CStifItemParser& aItem );
		
		void ListProfilesL( TSmlUsageType type );
		void ListTransportsL();
		void ListTasksL();
		
    public:     // Data
        // ?one_line_short_description_of_data
        //?data_declaration;
    
    protected:  // Data
        // ?one_line_short_description_of_data
        //?data_declaration;

    private:    // Data
		RSyncMLDataSyncProfile	iDSProfile;
		RSyncMLDevManProfile 	iDMProfile;
		RSyncMLTransport		iTransport;
        RSyncMLDataProvider		iDataProvider;
        RSyncMLHistoryLog		iHistoryLog;
        RSyncMLSettings			iSettings;
        RSyncMLDevMan			iDevMan;
        
        RSyncMLConnection		iConnection;
        RSyncMLProfileBase*		iConnectionProfile;
        TSmlTransportId			iConnectionId;
        
        RSyncMLTask				iTask;
        TSmlTaskId				iTaskId;
        TSmlDataProviderId		iDataProviderId;
        
        RSyncMLDataSyncJob		iDSJob;
        RSyncMLDevManJob		iDMJob;
        TSmlJobId				iJobId;
        RArray<TSmlProfileId>	iProfiles;
        RArray<TSmlTransportId>	iTransports;
        RArray<TSmlTaskId>		iTasks;
        
        TSmlProfileId			iProfileId;
        
        EventCallback2*			iEventCallback2;
        ProgressCallback2*		iProgressCallback2;
        
        RSyncMLSession iSyncSession;
        
    public:     // Friend classes
        //?friend_class_declaration;
    protected:  // Friend classes
        //?friend_class_declaration;
    private:    // Friend classes
        //?friend_class_declaration;

    };


// class for listening to events
class EventCallback : public MSyncMLEventObserver
	{
	public:
		EventCallback() {}
		~EventCallback() {}
		virtual void OnSyncMLSessionEvent( TEvent aEvent, TInt aIdentifier, TInt aError, TInt aAdditionalData );
	};


// another class for listening to events
class EventCallback2 : public MSyncMLEventObserver
	{
	public:
		EventCallback2() {}
		~EventCallback2() {}
		virtual void OnSyncMLSessionEvent( TEvent aEvent, TInt aIdentifier, TInt aError, TInt aAdditionalData );
	};

// class for listening to progress events
class ProgressCallback : public MSyncMLProgressObserver
	{
	public:
		ProgressCallback() {}
		~ProgressCallback() {}
		
		virtual void OnSyncMLSyncError( TErrorLevel aErrorLevel, TInt aError, TInt aTaskId, TInt aInfo1, TInt aInfo2 );
		virtual void OnSyncMLSyncProgress( TStatus aStatus, TInt aInfo1, TInt aInfo2 );
		virtual void OnSyncMLDataSyncModifications( TInt aTaskId,
										const TSyncMLDataSyncModifications& aClientModifications,
										const TSyncMLDataSyncModifications& aServerModifications );
	};

// another class for listening to progress events
class ProgressCallback2 : public MSyncMLProgressObserver
	{
	public:
		ProgressCallback2() {}
		~ProgressCallback2() {}
		
		virtual void OnSyncMLSyncError( TErrorLevel aErrorLevel, TInt aError, TInt aTaskId, TInt aInfo1, TInt aInfo2 );
		virtual void OnSyncMLSyncProgress( TStatus aStatus, TInt aInfo1, TInt aInfo2 );
		virtual void OnSyncMLDataSyncModifications( TInt aTaskId,
										const TSyncMLDataSyncModifications& aClientModifications,
										const TSyncMLDataSyncModifications& aServerModifications );
	};
	
//class for testing error conversion.
class ProgressCallbackErrorTest : public MSyncMLProgressObserver
	{
	public:
		ProgressCallbackErrorTest() : iCaseFailed(EFalse) {}
		~ProgressCallbackErrorTest() {}
		
		virtual void OnSyncMLSyncError( TErrorLevel aErrorLevel, TInt aError, TInt aTaskId, TInt aInfo1, TInt aInfo2 );
		virtual void OnSyncMLSyncProgress( TStatus aStatus, TInt aInfo1, TInt aInfo2 );
		virtual void OnSyncMLDataSyncModifications( TInt aTaskId,
										const TSyncMLDataSyncModifications& aClientModifications,
										const TSyncMLDataSyncModifications& aServerModifications );
										
	public:
		TBool iCaseFailed;
	};	



#endif      // CLIENTAPI_H
            
// End of File
