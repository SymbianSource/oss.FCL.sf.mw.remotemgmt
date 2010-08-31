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
* Description:  ?Description
*
*/



#ifndef TESTPROFILEUTIL_H
#define TESTPROFILEUTIL_H

//  INCLUDES
#include <StifLogger.h>
#include <TestScripterInternal.h>
#include <StifTestModule.h>
#include <TestclassAssert.h>
#include <centralrepository.h>
#include <CoreApplicationUIsSDKCRKeys.h>

//SyncML Includes

#include <SyncMLClient.h>
#include <SyncMLClientDS.h>
#include <SyncMLClientDM.h>
#include <SyncMLObservers.h>
#include <SyncMLErr.h>


// CONSTANTS
//const ?type ?constant_var = ?constant;

// MACROS
//#define ?macro ?macro_def
#define TEST_CLASS_VERSION_MAJOR 0
#define TEST_CLASS_VERSION_MINOR 0
#define TEST_CLASS_VERSION_BUILD 0

// Logging path
_LIT( KtestprofileutilLogPath, "\\logs\\testframework\\testprofileutil\\" ); 
// Log file
_LIT( KtestprofileutilLogFile, "testprofileutil.txt" ); 
_LIT( KtestprofileutilLogFileWithTitle, "testprofileutil_[%S].txt" );

// FUNCTION PROTOTYPES
//?type ?function_name(?arg_list);

// FORWARD DECLARATIONS
//class ?FORWARD_CLASSNAME;
class Ctestprofileutil;

// DATA TYPES
//enum ?declaration
//typedef ?declaration
//extern ?data_type;

// CLASS DECLARATION

/**
*  Ctestprofileutil test class for STIF Test Framework TestScripter.
*  ?other_description_lines
*
*  @lib ?library
*  @since ?Series60_version
*/
NONSHARABLE_CLASS(Ctestprofileutil) : public CScriptBase
    {
    public:  // Constructors and destructor

        /**
        * Two-phased constructor.
        */
        static Ctestprofileutil* NewL( CTestModuleIf& aTestModuleIf );

        /**
        * Destructor.
        */
        virtual ~Ctestprofileutil();

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
        Ctestprofileutil( CTestModuleIf& aTestModuleIf );

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
        
        //Sync Related Methods
        
        //syncml sessions
        TInt OpenSyncSessionL( CStifItemParser& aItem );
        TInt CloseSyncSession( CStifItemParser& aItem );
                
        //DS proflie creation
        TInt CreateDSProfileL( CStifItemParser& aItem );
		TInt SetRemoteDSProfileDataL(CStifItemParser& aItem);
		TInt UpdateDSProfileL( CStifItemParser& aItem );
        TInt CloseDSProfile( CStifItemParser& aItem );
		TInt OpenDSProfileByIdL( CStifItemParser& aItem );
		TSmlProfileId	OpenDSProfileByNameL( TSmlProfileId aProfileId);
		void ListProfilesL( TSmlUsageType type );
		
		//Connection apis
	    TInt OpenConnectionL( CStifItemParser& aItem );
        TInt CreateConnectionL( CStifItemParser& aItem );
        TInt CloseConnection( CStifItemParser& aItem );
        void SetConnectionProfileL( CStifItemParser& aItem );
        TInt UpdateConnectionL( CStifItemParser& aItem );
        TSmlUsageType UsageType( CStifItemParser& aItem );
        
        //Task apis
        TInt OpenTaskL( CStifItemParser& aItem );
        TInt CreateTaskL( CStifItemParser& aItem );
        TInt SetTaskDataL( CStifItemParser& aItem );
		TInt CloseTask( CStifItemParser& aItem );
		TInt UpdateTaskL( CStifItemParser& aItem );
		
		TInt OffLineL( CStifItemParser& aItem);
		TInt GeneralL( CStifItemParser& aItem);
        TInt SyncL(CStifItemParser& aItem);
        TInt LaunchAutoRestartL(CStifItemParser& aItem);
        TInt CallDelay(CStifItemParser& aItem);

        //ProfileUtil Apis
        TInt ReadSettingsDBL( CStifItemParser& aItem );
        TInt ReadAndWriteCenrepKeysL(CStifItemParser& aItem);
              
        /**
        * Example test method.
        * @since ?Series60_version
        * @param aItem Script line containing parameters.
        * @return Symbian OS error code.
        */
        virtual TInt ExampleL( CStifItemParser& aItem );
        
        /**
         * Method used to log version of test class
         */
        void SendTestClassVersion();

        

    public:     // Data
        // ?one_line_short_description_of_data
        //?data_declaration;

    protected:  // Data
        // ?one_line_short_description_of_data
        //?data_declaration;

    private:    // Data
        
        // ?one_line_short_description_of_data
        //?data_declaration;
          RSyncMLSession iSyncSession;
          RSyncMLDataSyncProfile	iDSProfile;
          TSmlTaskId				iTaskId;
          RSyncMLTask				iTask;
          RArray<TSmlProfileId>	iProfiles;
          TSmlProfileId			iProfileId;
          RSyncMLConnection		iConnection;
          RSyncMLProfileBase*		iConnectionProfile;
          TSmlTransportId			iConnectionId;
          TSmlDataProviderId		iDataProviderId;
          
          // Reserved pointer for future extension
        //TAny* iReserved;

    public:     // Friend classes
        //?friend_class_declaration;
    protected:  // Friend classes
        //?friend_class_declaration;
    private:    // Friend classes
        //?friend_class_declaration;

    };
    
    
enum TSyncStates {   ERegister = 0, 
                     EMonitor, 
                     ESyncOk,
                     EInitSync,
                     ECancel,
                     EComplete };
                     
class CSync : public CActive,
			  public MSyncMLEventObserver,
              public MSyncMLProgressObserver
              
{
	public:
	    
	    /**
		 * Two phase construction
		 */
		static CSync* NewL(TInt aParam);
		
		/**
		 * Destructor
		 */
		~CSync();
	protected:
	
	    // From base class CActive

		/**
		 * Implements cancellation of an outstanding request
		 */
		void DoCancel();

		/**
		 * Handles an active objects request completion event.
		 */
		void RunL();

		/**
		 * If the RunL function leaves, then the active scheduler call
		 * RunError() to handle the leave.
		 * @param aError - The error code
		 */
		TInt RunError ( TInt aError );
   private:
         
        	/**
		 * Constructor
		 */
		CSync();
		
		/**
		 * Second phase constructor
		 */
		void ConstructL(TInt aParam);

		/**
		 * Resgiter for events
		 */
		void RegisterL();

		/**
		 * Trigger sync
		 */
		void StartSyncL();

		/** 
		 * Close the server
		 */
		void Complete();
		
		void SyncCancel();

		/**
		 * Set active
		 */
		void IssueRequest(); 
		
		void OnSyncMLSessionEvent(TEvent aEvent, 
                                  TInt aIdentifier, 
                                  TInt aError, 
                                  TInt aAdditionalData); 
        void OnSyncMLSyncError(TErrorLevel aErrorLevel, TInt aError, TInt aTaskId, TInt aInfo1, TInt aInfo2);
        
        void OnSyncMLSyncProgress(TStatus aStatus, TInt aInfo1, TInt aInfo2);
        
        void OnSyncMLDataSyncModifications(TInt aTaskId, const TSyncMLDataSyncModifications& aClientModifications, const TSyncMLDataSyncModifications& aServerModifications);
        
        void OpenSyncSessionL();
		
		void CloseSyncSession();
		
		void OfflineModeL();
				
 private:
        
        TInt iInterruptStage;
         
        TSyncStates iState; 
        
        RSyncMLSession iSyncSession;
        
        RSyncMLDataSyncJob iSyncJob;
        
        TStatus iPrgPrevStatus;
		
		TStatus iPrgCurStatus;
            
};

#endif      // TESTPROFILEUTIL_H

// End of File
