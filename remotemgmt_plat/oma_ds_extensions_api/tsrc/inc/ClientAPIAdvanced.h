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



#ifndef CLIENTAPIADVANCED_H
#define CLIENTAPIADVANCED_H

//  INCLUDES
#include <StifLogger.h>
#include <TestScripterInternal.h>
#include <StifTestModule.h>

#include <SyncMLClient.h>
#include <SyncMLClientDS.h>
#include <SyncMLClientDM.h>
#include <SyncMLObservers.h>
#include <SyncMLErr.h>

// phonebook  header files

#include <cntdef.h>
#include <cntdb.h>
#include <cntitem.h>
#include <cntfldst.h>
#include <cntview.h>
#include <cpbkcontactengine.h>
#include <cpbkcontactitem.h> 
#include <cpbkfieldinfo.h> 
#include <cpbkfieldsinfo.h> 
#include <pbkfields.hrh>

// CONSTANTS
//const ?type ?constant_var = ?constant;

// MACROS
//#define ?macro ?macro_def
// Logging path
_LIT( KClientAPIAdvancedLogPath, "\\logs\\testframework\\ClientAPIAdvanced\\" ); 
// Log file
_LIT( KClientAPIAdvancedLogFile, "ClientAPIAdvanced.txt" ); 

// FUNCTION PROTOTYPES
//?type ?function_name(?arg_list);

// FORWARD DECLARATIONS
//class ?FORWARD_CLASSNAME;
class CClientAPIAdvanced;

// DATA TYPES
//enum ?declaration
//typedef ?declaration
//extern ?data_type;

// CLASS DECLARATION

/**
*  CClientAPIAdvanced test class for STIF Test Framework TestScripter.
*  ?other_description_lines
*
*  @lib ?library
*  @since ?Series60_version
*/
//contact fields

class CPhonebookParameters : public CBase
    {
public:
    HBufC*  iContactFirstName;
    HBufC*  iContactLastName;
    HBufC*  iContactCompanyName;
    HBufC*  iContactJobTitle;
    HBufC*  iContactPhoneNumberGeneral;     
    HBufC*  iContactURL;                    
    HBufC*  iContactEmailAddress;           
    HBufC*  iContactSyncType;           


   TInt iNumberOfPhoneNumberFields;
   TInt iNumberOfURLFields; 
   TInt iNumberOfEmailAddressFields;

public:
    CPhonebookParameters();
    ~CPhonebookParameters();
    };


enum TSyncStates {   ERegister = 0, 
                     EMonitor, 
                     ESyncOk,
                     EInitSync,
                     ECancel,
                     EComplete };


NONSHARABLE_CLASS(CClientAPIAdvanced) : public CScriptBase
    {
    public:  // Constructors and destructor

        /**
        * Two-phased constructor.
        */
        static CClientAPIAdvanced* NewL( CTestModuleIf& aTestModuleIf );

        /**
        * Destructor.
        */
        virtual ~CClientAPIAdvanced();

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
        CClientAPIAdvanced( CTestModuleIf& aTestModuleIf );

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
        
        //syncml sessions
        TInt OpenSyncSessionL( CStifItemParser& aItem );
        TInt CloseSyncSession( CStifItemParser& aItem );
        
        //ds job
        TInt CreateDSJobForProfileL( CStifItemParser& aItem );
        TInt CloseDSJob( CStifItemParser& aItem );
        TInt CreateDSJobForTasksL( CStifItemParser& aItem );
        // ds profile -->
		TInt CreateDSProfileL( CStifItemParser& aItem );
		TInt SetRemoteDSProfileDataL(CStifItemParser& aItem);
		TInt UpdateDSProfileL( CStifItemParser& aItem );
		TInt CloseDSProfile( CStifItemParser& aItem );
		TInt OpenDSProfileByIdL( CStifItemParser& aItem );
		TSmlProfileId	OpenDSProfileByNameL( TSmlProfileId aProfileId,TDesC& aProfilename);

		// connection -->
        TInt OpenConnectionL( CStifItemParser& aItem );
        TInt CreateConnectionL( CStifItemParser& aItem );
        TInt UpdateConnectionL( CStifItemParser& aItem );
        TInt CloseConnection( CStifItemParser& aItem );
        //ADD NEW METHOD DEC HERE
   
        //task------>
        TInt OpenTaskL( CStifItemParser& aItem );
        TInt CreateTaskL( CStifItemParser& aItem );
        TInt SetTaskDataL( CStifItemParser& aItem );
        TInt UpdateTaskL( CStifItemParser& aItem );
        TInt CloseTask( CStifItemParser& aItem );
        
         TInt Wait( CStifItemParser& /*aItem*/ );
        virtual TInt ExampleL( CStifItemParser& aItem );
        //for creating,deleting and updating contacts in contact database.
        
        TInt CreateContactL(CStifItemParser& aItem);
        TInt DeleteContactL(CStifItemParser& aItem);
        TInt UpdateContactL(CStifItemParser& aItem);
        void AddFieldToContactItemL(CPbkContactItem& aItem, TPbkFieldId aFieldId, const TDesC& aFieldText);
        
        
        //starting the sync sessions
        TInt SyncL(CStifItemParser& aItem);
        
    private:
        TSmlUsageType UsageType( CStifItemParser& aItem );
        void SetConnectionProfileL( CStifItemParser& aItem );
        void ListProfilesL( TSmlUsageType type );
        void ListTasksL(TInt aId,TDesC& aProfilename);
    public:     // Data
        // ?one_line_short_description_of_data
        //?data_declaration;

    protected:  // Data
        // ?one_line_short_description_of_data
        //?data_declaration;

    private:    // Data
        
        // ?one_line_short_description_of_data
        //?data_declaration;
         RSyncMLDataSyncProfile	iDSProfile;
         RSyncMLSession iSyncSession;
         RSyncMLProfileBase*		iConnectionProfile;
         RSyncMLConnection		iConnection;
         RSyncMLTask				iTask;
         RSyncMLDataSyncJob		iDSJob;
         TSmlJobId				iJobId;
         TSmlDataProviderId		iDataProviderId;
         TSmlProfileId			iProfileId;
         TSmlTransportId			iConnectionId;
         TSmlTaskId				iTaskId;
         RArray<TSmlProfileId>	iProfiles;
         RArray<TSmlTransportId>	iTransports;
         RArray<TSmlTaskId>		iTasks;
         CPbkContactEngine* iContactDBEngine;
        // Reserved pointer for future extension
        //TAny* iReserved;

    public:     // Friend classes
        //?friend_class_declaration;
    protected:  // Friend classes
        //?friend_class_declaration;
    private:    // Friend classes
        //?friend_class_declaration;

    };


class CSync : public CActive,
			  public MSyncMLEventObserver,
              public MSyncMLProgressObserver
              
{
	public:
	    
	    /**
		 * Two phase construction
		 */
		static CSync* NewL(TSmlProfileId aProfileId);
		
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
		
  private:
        
        TSmlProfileId iProfileId;
              
        TSyncStates iState; 
        
        RSyncMLSession iSyncSession;
        
        RSyncMLDataSyncJob iSyncJob;
        
               
};






#endif      // CLIENTAPIADVANCED_H

// End of File
