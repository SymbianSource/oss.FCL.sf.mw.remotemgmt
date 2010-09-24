/*
* Copyright (c) 2000 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description: Implementation of terminalsecurity components
*
*/


#ifndef SCPSERVER_H
#define SCPSERVER_H

// Define this so the precompiler in CW 3.1 won't complain about token pasting,
// the warnings are not valid
#pragma warn_illtokenpasting off

#define SCP_USE_POLICY_ENGINE
#define SCP_ENFORCE_SECURITY

#ifdef __REMOTE_LOCK
#define SCP_SMS_LOCK_AVAILABLE
#endif // __REMOTE_LOCK

// Note: Copy the definitions also to SCPServer.mmp

//  INCLUDES
#include <e32base.h>
#include <bldvariant.hrh>
#include <etelmm.h>
#include <rmmcustomapi.h>
#include <f32file.h>

#include "SCPServerInterface.h"
#include "SCPConfiguration.h"
#include "SCPTimer.h"

#include "SCPParamDB.h"
#include    <settingsinternalcrkeys.h>
#include    <centralrepository.h>

#include "SCPDebug.h"
#include <TerminalControl3rdPartyParamsPlugin.h>
/*#ifdef _DEBUG
#define __SCP_DEBUG
#endif // _DEBUG

#ifdef __SCP_DEBUG
#define Dprint(a) RDebug::Print ## a
#else
#define Dprint(a)
#endif // _DEBUG*/


// LOCAL CONSTANTS
const TInt KSCPConfigUnknown = 0;
const TInt KSCPConfigOK = 1;
const TInt KSCPConfigInvalid = 2;

const TUint KSCPServerShutdownTimeout( 10000000 ); // 10 seconds

const TUint KSCPLockOperationTimeout( 10000000 ); // 10 seconds
const TUint KSCPAutolockPeriodMaximum( 1440 ); // Autolock period max. value

const TInt KSCPCodeBlockLimit( 5 ); // The fifth attempt will lock the code
const TInt KSCPCodeBlockedTimeMin( 5 ); // Block the code for 5min
_LIT( KSCPNotBlocked, "NB" );

const TInt KSCPErrCodeBlockStarted = -102;

// Policy engine -related definitions. Used to retrieve and set the code change -policy.
_LIT8( KSCPOKResponse, "Operation Ok");

_LIT8( KSCPCodePolicyDisallowOperation, 
    "<Package><Operation action_id = \"Add\" target_id = \"device_lock_passcode_visibility_policy\"><Data><![CDATA[<Rule RuleId=\"device_lock_passcode_visibility_policy_rule2\" Effect=\"Deny\"><Target></Target></Rule>]]></Data></Operation></Package>");
        
_LIT8( KSCPCodePolicyAllowOperation, 
    "<Package><Operation action_id = \"Remove\" target_id = \"device_lock_passcode_visibility_policy_rule2\"></Operation></Package>");        
    
_LIT8( KCodePolicySubject, "subject" );
_LIT8( KCodePolicySubjectValue, "autolock" );
_LIT8( KCodePolicyAction, "action" );
_LIT8( KCodePolicyActionValue, "lock" );
_LIT8( KCodePolicyTypeId, "http://www.w3.org/2001/XMLSchema#string" );

// Allowed SIDs for the functions
const TUint32 KSCPSTIF( 0x102073E4 ); // STIF exe
const TUint32 KSCPServerSIDTelephone( 0x100058B3 ); // Terminal Control Server
const TUint32 KSCPServerSIDLog( 0x101f4cd5 ); // Terminal Control Server
const TUint32 KSCPServerSIDTerminalControl( 0x10207825 ); // Terminal Control Server

//const TUint32 KSCPServerSIDGeneralSettings( 0x100058EC ); // General Settings
const TUint32 KSCPServerSIDGeneralSettings( 0X20025FD9 ); // General Settings

const TUint32 KSCPSecuiDialogNotifierSrv( 0x10000938 ); // SecUI Notified Server
const TUint32 KSecUiTest( 0xEE89E3CF ); // SecUI Test App

//const TUint32 KSCPServerSIDSecurityObserver( 0x10000938 ); // Security Observer
const TUint32 KAknNfySrvUid( 0x10281EF2 ); // Security Notifier
const TUint32 KSCPServerSIDAutolock( 0x100059B5 ); // Autolock
const TUint32 KSCPEvntHndlrUid( 0x20026F5C ); // UID of the DMEventNotifier -> SCPEventHandler
const TUint32 KDevEncUiUid( 0x2000259A ); // SID of Device Encryption UI
const TUint32 KSCPServerSIDSysAp (0x100058F3);

#ifdef SCP_ENFORCE_SECURITY
// Policy server elements
const CPolicyServer::TPolicyElement SCPPolicyElements[]=
    {
        {
        _INIT_SECURITY_POLICY_S0( KSCPServerSIDTerminalControl ),
            CPolicyServer::EFailClient
        },
        {
        _INIT_SECURITY_POLICY_C1( ECapability_None ),
            CPolicyServer::EFailClient
        },
        {
        _INIT_SECURITY_POLICY_C1( ECapability_None ),
            CPolicyServer::EFailClient
        // Access to the query-functionality will be checked in the service code
        },
        {
        _INIT_SECURITY_POLICY_C1( ECapability_None ),
            CPolicyServer::EFailClient
        // Access to the get-functionality will be checked in the service code
        },
        {
        _INIT_SECURITY_POLICY_C1( ECapability_None ),
            CPolicyServer::EFailClient
        // Anyone can access the authentication calls
        },
        {
        _INIT_SECURITY_POLICY_S0( KSCPServerSIDAutolock ),
            CPolicyServer::EFailClient
        // Only Autolock allowed for the Check configuration call
        },
        {
        _INIT_SECURITY_POLICY_S0( KSCPEvntHndlrUid ),
            CPolicyServer::EFailClient
        },
        {
        _INIT_SECURITY_POLICY_C1( ECapabilityAllFiles ),
            CPolicyServer::EFailClient
        }                                                 
    };
#else
const CPolicyServer::TPolicyElement SCPPolicyElements[]=
    {
        {
        _INIT_SECURITY_POLICY_C1( ECapability_None ),
            CPolicyServer::EFailClient
        },
        {
        _INIT_SECURITY_POLICY_C1( ECapability_None ),
            CPolicyServer::EFailClient
        },
        {
        _INIT_SECURITY_POLICY_C1( ECapability_None ),
            CPolicyServer::EFailClient
        },
        {
        _INIT_SECURITY_POLICY_C1( ECapability_None ),
            CPolicyServer::EFailClient
        },
        {
        _INIT_SECURITY_POLICY_C1( ECapability_None ),
            CPolicyServer::EFailClient
        },
        {
        _INIT_SECURITY_POLICY_C1( ECapability_None ),
            CPolicyServer::EFailClient        
        },        
        {
        _INIT_SECURITY_POLICY_C1( ECapability_None ),
            CPolicyServer::EFailClient        
        },
        {
        _INIT_SECURITY_POLICY_C1( ECapability_None ),
            CPolicyServer::EFailClient        
        }
    };
#endif // SCP_ENFORCE_SECURITY

// Policy server ranges
const TInt SCPPolicyRanges[] = 
    {
    0, // Functional calls
    10, // Store call
    20, // Query call
    30, // Getparam call
    40, // Authentication calls
    50, // CheckConfig call
    60, // App cleanup call
    70 // SetAutoLock period from UI 
    };
    
// Policy server elements index
const TUint8 SCPPolicyElementsIndex[] =
    {
    0,
    1,
    2,
    3,
    4,
    5,
    6,
    7
    };
    
// Actual policy definition
const CPolicyServer::TPolicy CSCPServerPolicy =
    {
    CPolicyServer::EAlwaysPass, // Connection policy
    8,  // Range count
    SCPPolicyRanges,
    SCPPolicyElementsIndex,
    SCPPolicyElements
    };      

const TInt KSCPDefaultMaxTO( 1440 );

// reasons for server panic
enum TSCPServPanic
	{
	ESvrCreateServer,
	ESvrStartServer,
	ECreateTrapCleanup,
	EBadRequest,
	EBadHashDigest
	};


// DATA TYPES

// FORWARD DECLARATIONS
class CSCPSessionbase;

//#ifdef __SAP_DEVICE_LOCK_ENHANCEMENTS
class CSCPPluginManager;
class CSCPPluginEventHandler;
class CSCPParamObject;
class MSCPPluginEventHandler;
//#endif // __SAP_DEVICE_LOCK_ENHANCEMENTS

// function to panic the server
void PanicServer( TSCPServPanic aPanic );

// FORWARD CLASS DECLARATION
class CSCPParamDBController;

/**
*  The class which contains the actual SCP server implementation
*/
class CSCPServer : public CPolicyServer, public MSCPTimeoutHandler
	{
	
	public:  // Methods

        enum { EPriority = CActive::EPriorityStandard }; 
    	
        // Constructors and destructor      

        /**
        * Static constructor.
        */
   	    static CSCPServer* NewL();

        /**
        * Destructor.
        */
        virtual ~CSCPServer();

        // New methods
        
        /**
        * The initial entry point to the actual server code
        * @param aNone Not used
        * @return Always success, may panic if the server or 
        * the cleanup stack cannot be created
        */
        static TInt ThreadFunction(TAny* aNone);    	   	        
        
        /**
        * A method to retrieve the code stored on the server
        * @param aCode A descriptor buffer that is to contain the code
        * @return TInt: A generic status value
        * <BR><B>Name of return value:</B> Status
        * <BR><B>Type of return value:</B> TInt
        * <BR><B>Range of return value:</B> A system error code
        * <BR><B>Contents of return value:</B> The status code
        */
        TInt GetCode( TDes& aCode );
        
        /**
        * A method to store a new ISA code value to the server
        * @param aCode A descriptor buffer that contains the new code
        * @return TInt: A generic status value
        * <BR><B>Name of return value:</B> Status
        * <BR><B>Type of return value:</B> TInt
        * <BR><B>Range of return value:</B> A system error code
        * <BR><B>Contents of return value:</B> The status code        
        */
        TInt StoreCode( TDes& aCode );
        
        /**
        * Tries to change the ISA-side code using the stored old code.
        * @param aNewPassword The new password
        */        
        void ChangeISACodeL( RMobilePhone::TMobilePassword& aNewPassword );
        
//#ifdef __SAP_DEVICE_LOCK_ENHANCEMENTS
        
        /**
        * Get the 5-digit ISA code value from the given hash-buffer.
        * @param aHashBuf A descriptor buffer that contains a MD5 digest
        * @return TInt: The hashed DOS code
        * <BR><B>Name of return value:</B> DOS code
        * <BR><B>Type of return value:</B> TInt
        * <BR><B>Range of return value:</B> 10000-99999
        * <BR><B>Contents of return value:</B> The hashed code        
        */
        static TInt HashISACode( TDes& aHashBuf );
        
        /**
        * A method to store a new enhanced code value to the server
        * @param aCode A descriptor buffer that contains the new code
        * @param aNewDOScode A ptr to a descriptor buffer, which will contain the new
        * DOS code after a successful call
        * @return TInt: A generic status value
        * <BR><B>Name of return value:</B> Status
        * <BR><B>Type of return value:</B> TInt
        * <BR><B>Range of return value:</B> A system error code
        * <BR><B>Contents of return value:</B> The status code        
        */
        TInt StoreEnhCode( TDes& aCode, TSCPSecCode* aNewDOSCode = NULL );
//#endif // __SAP_DEVICE_LOCK_ENHANCEMENTS
             
        /**
        * A method for checking if a session has the given operation in progress
        * @param aCommand The command to check for
        * @return The status of the given command
        * <BR><B>Name of return value:</B> Status
        * <BR><B>Type of return value:</B> TBool
        * <BR><B>Range of return value:</B> ETrue/EFalse
        * <BR><B>Contents of return value:</B> The status code        
        */
        TBool IsOperationInProgress( TSCPAdminCommand aCommand );
        
        /**
        * A method for acknowledging commands of a given type, also returns if a 
        * session was waiting for the acknowledgement
        * @param aCommand The command the acknowledgement is for
        * @return ETrue if a session was waiting, otherwise EFalse
        * <BR><B>Name of return value:</B> Status
        * <BR><B>Type of return value:</B> TBool
        * <BR><B>Range of return value:</B> ETrue/EFalse
        * <BR><B>Contents of return value:</B> The status code        
        */
        TBool AcknowledgementReceived( TSCPAdminCommand aCommand );
        
        /**
        * Handles get parameter value messages.
        * @param aMessage The message received from the client
        * <BR><B>Name of return value:</B> Status
        * <BR><B>Type of return value:</B> TInt
        * <BR><B>Range of return value:</B> A system error code
        * <BR><B>Contents of return value:</B> The status code        
        */  
        TInt SetParameterValueL( TInt aID, const TDesC& aValue, TUint32 aCallerIdentity );       
        
        /**
        * Handles set parameter value messages.
        * @param aMessage The message received from the client
        * <BR><B>Name of return value:</B> Status
        * <BR><B>Type of return value:</B> TInt
        * <BR><B>Range of return value:</B> A system error code
        * <BR><B>Contents of return value:</B> The status code        
        */  
        TInt GetParameterValueL( TInt aID, TDes& aValue, TUint32 aCallerIdentity );
        
        /**
        * Checks the validity of the given ISA code string.
        * @param aCode The code whose validity is to be checked.
        * <BR><B>Name of return value:</B> Code validity state
        * <BR><B>Type of return value:</B> TBool
        * <BR><B>Range of return value:</B> ETrue/EFalse
        * <BR><B>Contents of return value:</B> A truth value indicating the validity
        */  
        static TBool IsValidISACode( TDes& aCode );                        
             
        /**
        * Sets the autolock period.
        * @param aValue The new value for the period
        * <BR><B>Name of return value:</B> Status
        * <BR><B>Type of return value:</B> TInt
        * <BR><B>Range of return value:</B> A system error code
        * <BR><B>Contents of return value:</B> The status code 
        */  
        TInt SetAutolockPeriodL( TInt aValue );
        
        /**
        * Gets the autolock period.
        * @param aValue A reference that will contain the value on success
        * <BR><B>Name of return value:</B> Status
        * <BR><B>Type of return value:</B> TInt
        * <BR><B>Range of return value:</B> A system error code
        * <BR><B>Contents of return value:</B> The status code 
        */  
        TInt GetAutolockPeriodL( TInt& aValue );  
        
        /**
        * Informs the server that a session has been closed.
        */  
        void SessionClosed();
        
        /**
        * Informs the server that a session has been opened.
        */  
        void SessionOpened();        
        
        /**
        * Check if the given DOS code is correct.
        */           
        void CheckISACodeL( RMobilePhone::TMobilePassword aCode );  
        
        /**
        * Check if the server configuration is valid and in-sync with the DOS.
        * @param aMode The check mode, initial or complete. The initial mode only checks if the
        * configuration has already been validated.
        */        
        void ValidateConfigurationL( TInt aMode );
        
        /**
        * Fetches the server's handles to Etel sever and RMobilePhone API. Opens
        * if required.
        * @param aTelServ A handle ptr to receive the Etel API handle ptr
        * @param aPhone A handle ptr to receive the RMobilePhone API handle ptr
        */
        void GetEtelHandlesL( RTelServer** aTelServ = NULL, RMobilePhone** aPhone = NULL );                      
        
//#ifdef __SAP_DEVICE_LOCK_ENHANCEMENTS

        /**
        * Checks the validity of the given enhanced code string.
        * @param aCode The code whose validity is to be checked.
        * <BR><B>Name of return value:</B> Code validity state
        * <BR><B>Type of return value:</B> TBool
        * <BR><B>Range of return value:</B> ETrue/EFalse
        * <BR><B>Contents of return value:</B> A truth value indicating the validity
        */  
        static TBool IsValidEnhCode( TDes& aCode );
        
        /**
        * Check if the code is blocked
        * @return The status
        */          
        TBool IsCodeBlocked();
        
        /**
        * Send a DOS code known to be invalid. Makes sure the DOS code isn't blocked.
        */          
        void SendInvalidDOSCode( RMobilePhone::TMobilePassword& aCodeToSend );

        /**
        * The end-point method for authentication calls. Checks the given code and returns
        * the correct/incorrect ISA code.
        */         
        TInt CheckCodeAndGiveISAL( TDes& aCodeToCheck, 
                                  TDes& aISACodeToReturn,
                                  CSCPParamObject*& aRetParams,
                                  TInt aFlags ); 
        
        /**
        * The end-point method for the change security code -call. Checks the given code, and 
        * changes the code, if it was correct.
        */          
        TInt CheckAndChangeEnhCodeL( TDes& aOldPass,
                            TDes& aNewPass,
                            CSCPParamObject*& aRetParams,
                            TSCPSecCode& aNewDOSCode );
                            
        /**
        * Retrieves the plugin event handler -object to be used.
        */          
        MSCPPluginEventHandler* GetEventHandlerL();  
        
        /**
        * Checks from the plugins, if the password is allowed to be changed at this time.
        */           
        TInt IsPasswordChangeAllowedL( CSCPParamObject*& aRetParams );
            
		/**
		* This method is called to remove the parameters set by the application(s) provided as arguments.
		*/
        TInt PerformCleanupL( HBufC8* aAppIDBuffer, RArray<const TParamChange>& aChangeArray, RPointerArray<HBufC8>& aParamValArray );
        
        /**
        * This method returns the values for the policies maintained at the server
        */
        void GetPoliciesL(HBufC8* aAppIDBuffer, TUint32 aCallerIdentity);
//#endif //  __SAP_DEVICE_LOCK_ENHANCEMENTS 
        
        // Methods from base classes
        
        /**
        * From CPolicyServer The session creation function
        * @param aVersion The version of the requesting client
        * @param aMsg The connection message
        * @return A pointer to a new session
        * <BR><B>Name of return value:</B> New session pointer
        * <BR><B>Type of return value:</B> CSession2*
        * <BR><B>Range of return value:</B> A valid pointer to a new session
        * <BR><B>Contents of return value:</B> A valid pointer to a new session
        */
        CSession2* NewSessionL(const TVersion &aVersion, const RMessage2& aMsg ) const;
        
        /**
        * From MSCPTimeoutHandler Timeout callback
        * @param aParam The parameter passed to the timer object
        */        
        void Timeout( TAny* aParam );
		
        TInt ValidateLockcodeAgainstPoliciesL(TDes& aLockcode,CSCPParamObject*& aRetParams);
        
    private: //Methods
    
        // New Methods
        
        /**
        * C++ default constructor.
        */
        CSCPServer( TInt aPriority );
        
        /**
        * The actual worker method for the server        
        * @return TInt: KErrNone is successful
        */
        static TInt ThreadFunctionStage2L(); 

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();  
        
        /**
        * Set the change policy for the security code
        * @return TInt: KErrNone is successful, otherwise a system error code
        */        
        TInt ChangeCodePolicy( const TDesC& aValue );
        
		/**
		* Sets the best policy for the given parameter
		* @return TInt: KErrNone if successful, otherwise a system wide error
		*/
        TInt SetBestPolicyL( TInt aID, const TDesC& aValue, TUint32 aCallerIdentity, CSCPParamDBController* aParamDB );
		
        /**
        * Retrieve the change policy for the security code
        * @return TInt: KErrNone is successful, otherwise a system error code
        */
        TInt GetCodePolicy( TDes& aValue );
        
        /**
        * Check if Restore Factory Settings has been run, and reset the server
        * configuration if it has.
        */
        void CheckIfRfsPerformedL();        
        
        
       // #ifdef __SAP_DEVICE_LOCK_ENHANCEMENTS        
        
        /**
        * Retrieve the change policy for the security code
        * @return TInt: KErrNone is successful, otherwise a system error code
        */  
        TInt IsCorrectEnhCode( TDes& aCode, TInt aFlags );
        
        //#endif //  __SAP_DEVICE_LOCK_ENHANCEMENTS        
        
        /**
        * Checks if device memory is encrypted or not. The check is only done for phone memory.
        * @return TBool. ETrue if phone memory is encrypted, otherwise EFalse.
        */  
        TBool IsDeviceMemoryEncrypted();
    
    private:   // Data
        /** An object that contains the configuration data for the server */
        TSCPConfiguration iConfiguration;                
        
        #ifndef SCP_USE_POLICY_ENGINE
        /** Used to store the state of the code change policy */
        TInt iCodePolicy;
        #endif // SCP_USE_POLICY_ENGINE
            
        /** Used for retrieving and changing the Autolock period, owned. */
        CRepository* iALPeriodRep;  
        
        /** Pointer to a timer-object used to shut down the server after
        all sessions have been closed and a specified time has elapsed. Owned.*/
        CSCPTimer* iShutdownTimer; 
        
        /** A handle to ETel server */
        RTelServer iTelServ;
        /** A handle to the multimode TSY */
        RMobilePhone iPhone;
        
        /** A handle to the File Server */
        RFs iRfs;  
        
       // #ifdef __SAP_DEVICE_LOCK_ENHANCEMENTS
        
        /** A pointer to the plugin manager object, owned */
        CSCPPluginManager* iPluginManager;     
        
        /** A pointer to the plugin event handler object, owned */
        CSCPPluginEventHandler* iPluginEventHandler;           
        
        /** An integer variable to define the input mode of the lock code query */
        TInt def_mode;
        
    	/** A character variable to get the very first character of the lock code */
    	TChar ch;      
        
    	TBool iOverrideForCleanup;
       // #endif // __SAP_DEVICE_LOCK_ENHANCEMENTS
    };

#endif      // SCPSERVER_H   
            
// End of File

