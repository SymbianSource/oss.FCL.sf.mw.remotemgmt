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


#ifndef SCPSESSION_H
#define SCPSESSION_H

//  INCLUDES
#include <bldvariant.hrh>
#include <etelmm.h>
#include <rmmcustomapi.h>

#include "SCPServerInterface.h"
#include "SCPTimer.h"
#include <SCPParamObject.h>
#include <TerminalControl3rdPartyParamsPlugin.h>
// FORWARD DECLARATIONS
class CSCPServer;
class CSCPLockNotificationEventHandler;
class CSCPLockSettingEventHandler;
class RTelServer;
class RMobilePhone;

// CLASS DECLARATION

/**
*  The class corresponding to a single SCP server session.
*/
class CSCPSession : public CSession2, public MSCPTimeoutHandler
	{       
	public:  // Methods

        // Constructors and destructor

        /**
        * Static constructor.
        */
        static CSCPSession* NewL( CSCPServer& aServer );
        
        /**
        * Static constructor, that leaves the pointer to the cleanup stack.
        */
        static CSCPSession* NewLC( CSCPServer& aServer );

        /**
        * Destructor.
        */
        virtual ~CSCPSession();

        // New methods
        /**
        * Check if the given command is in progress
        * @param aCommand The command that is to be checked for
        * @return TBool: The command status
        * <BR><B>Name of return value:</B> Operation state
        * <BR><B>Type of return value:</B> TBool
        * <BR><B>Range of return value:</B> ETrue/EFalse
        * <BR><B>Contents of return value:</B> A truth value indicating 
        * <BR>the operation state.
        */
        TBool IsOperationInProgress( TSCPAdminCommand aCommand );
        
        /**
        * Acknowledges the given operation, and returns if it was awaited
        * @param aCommand The command that was acknowledged
        * @return TBool: The wait status
        * <BR><B>Name of return value:</B> Wait status
        * <BR><B>Type of return value:</B> TBool
        * <BR><B>Range of return value:</B> ETrue/EFalse
        * <BR><B>Contents of return value:</B> A truth value indicating if the 
        * <BR> acknowledgement was awaited.  
        */
        TBool AcknowledgeOperation( TSCPAdminCommand aCommand ); 
        
        /**
        * Callback method, indicates the lock operation is complete              
        */
        void LockOperationCompleted();
        
        /**
        * Informs that a lock operation is pending, waiting for an acknowledgement.
        * @param aCommand The command that is pending
        * @param aStatus The RequestStatus-variable that is to be signalled on completion.        
        */
        void LockOperationPending(  TSCPAdminCommand aCommand, 
                                    TRequestStatus* aStatus );
        
        /**
        * Changes the DOS side lock setting utilizing the password stored on the server.
        * @param aLocked The new state for the DOS lock setting
        * @param aAutolock ETrue if Autolock should be activated on success
        */
        void SetDOSLockSettingL( TBool aLocked,
                                 TBool aAutolock, 
                                 const RMessage2 &aMessage );        
        
        /**
        * Returns the state of the SMS Lock component
        * @return TBool: ETrue if SMS Lock is active, otherwise EFalse
        * <BR><B>Name of return value:</B> SMS Lock status
        * <BR><B>Type of return value:</B> TBool
        * <BR><B>Range of return value:</B> ETrue/EFalse
        * <BR><B>Contents of return value:</B> A truth value indicating the 
        * <BR> state of the component
        */
        static TBool IsSMSLockActiveL();
        
        /**
        * Returns the state of the Autolock component (is the timeout active)
        * @return TBool: ETrue if Autolock is active, otherwise EFalse
        * <BR><B>Name of return value:</B> Autolock status
        * <BR><B>Type of return value:</B> TBool
        * <BR><B>Range of return value:</B> ETrue/EFalse
        * <BR><B>Contents of return value:</B> A truth value indicating the 
        * <BR>state of the component
        */
        TBool IsAutolockActive();
        
        // Methods from base classes

        /**
        * From CSession The standard service entry point
        * @param aMessage The message received from the client        
        */
        void ServiceL( const RMessage2 &aMessage );
        
        /**
        * From MSCPTimeoutHandler Timeout handler function
        * @param aParam The data given to the timer object when initialized.        
        */
        void Timeout( TAny* aParam );
                
        
    private: //Methods

        // New Methods        
                
        /**
        * C++ default constructor.
        */
        CSCPSession( CSCPServer& aServer );
                
        /**
        * Panic the client
        * @param ?arg1 ?description
        * @return ?description
        */
    	void PanicClient( TInt aPanic ) const;

        /**
        * Handles the get code message
        * @param aMessage The message received from the client   
        */
        void HandleGetCodeMessageL( const RMessage2 &aMessage );
        
        /**
        * Handles the store code message
        * @param aMessage The message received from the client      
        */
        void HandleSetCodeMessageL( const RMessage2 &aMessage );
        
        /**
        * Handles the change code message
        * @param aMessage The message received from the client
        */
        void HandleChangeCodeMessageL( const RMessage2 &aMessage );                     
        
        /**
        * Handles the set phone lock message
        * @param aMessage The message received from the client
        */
        void HandleSetLockStateMessageL( const RMessage2 &aMessage );    
        
        /**
        * Handles the query admin command message.
        * @param aMessage The message received from the client
        */        
        void HandleQueryMessageL( const RMessage2 &aMessage );
        
        /**
        * Handles the get lock state message.
        * @param aMessage The message received from the client
        */  
        void HandleGetLockStateMessageL( const RMessage2 &aMessage );
        
        /**
        * Handles get parameter value messages.
        * @param aMessage The message received from the client
        */  
        void HandleGetParamMessageL( const RMessage2 &aMessage );        
        
        /**
        * Handles set parameter value messages.
        * @param aMessage The message received from the client
        */  
        void HandleSetParamMessageL( const RMessage2 &aMessage );   
        
        /**
        * Handles check configuration messages.
        * @param aMessage The message received from the client
        */          
        void HandleCheckConfigMessageL( const RMessage2 &aMessage );
        
//#ifdef __SAP_DEVICE_LOCK_ENHANCEMENTS        

        /**
        * Handles the authentication messages
        * @param aMessage The message received from the client
        */  
        void HandleAuthenticationMessageL( const RMessage2 &aMessage );
        
        /**
        * Handles change code requests from the client
        * @param aMessage The message received from the client
        */  
        void HandleChangeEnhCodeMessageL( const RMessage2 &aMessage );
                
        /**
        * Handles change code allowed query -messages from the client.
        * @param aMessage The message received from the client
        */         
        void HandleQueryChangeMessageL( const RMessage2 &aMessage );        

        /**
        * Handles cleanup (on application uninstallation)
        * @param aMessage The message received from the client
        * @return KErrNone if successful, error status otherwise
        */         
        TInt HandleCleanupL( const RMessage2 &aMessage );
        
        /**
        * Handles get on policies
        * @param aMessage The message received from the client
        * @return KErrNone if successful, error status otherwise
        */         
        void HandleGetPoliciesL( const RMessage2 &aMessage );
		
        TInt HandleSetALPeriodL( const RMessage2 &aMessage );
//#endif // __SAP_DEVICE_LOCK_ENHANCEMENTS               
        
        /**
        * Distributes the messages to the current handler function
        * @param aMessage The message received from the client
        */        
        void DispatchSynchronousMessageL( const RMessage2 &aMessage );
        
        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();
        
        /**
        * Notifies all the stakeholders of a change in parameter value(s) of the specified parameter(s).
        * This function is called either from PerformCleanupL() or SetParameterValueL() 
        * 
        * @params   aParamIDArray: The ID(s) of the parameter that needs to be set
        *           aParamValueArray: The value(s) of the parameter(s)
        *           
        *           NOTE: aParamValueArray.Count() SHOULD BE EQUAL TO aParamIDArray.Count()
        *           
        * @return TInt: KErrNone if successful, otherwise a system error code
        */        
        TInt NotifyAllStakeHoldersL( const RArray<const TParamChange>& aChange, TUint32 aCallerID );
        
        /**
        * Notifies all the stakeholders of a change in parameter value(s) of the specified parameter(s).
        * This function is called either from SetParameterValueL() or LockOperationCompleted()
        * 
        * @params   aParamID: The ID(s) of the parameter that needs to be set
        *           aParamValue: The value(s) of the parameter(s)
        *           
        * @return none
        */
        void NotifyChangeL( TInt aParamID, const TDesC8 aParamVal, TUint32 aCallerID );        

        void ReadFailedPoliciestoMsgBufferL(CSCPParamObject*& aParamObject,const RMessage2 &aMessage,TInt aSlotNumber );
		
        void ValidateLockcodeAgainstPoliciesL(const RMessage2 &aMessage);
    private:   // Data
       // Possible states for iLockCommandState
       enum TSCPLockCmdState
            {
            ESCPLockCmdStateIdle,
            ESCPLockCmdStateInProgress,
            ESCPLockCmdStateInProgressAcknowledged,
            ESCPLockCmdStateFinishedAckPending
            };      
       
        /** A reference to the server */
        CSCPServer&		                   iServer;      
        /** A pointer to the notification handler AO, owned */
        CSCPLockNotificationEventHandler*  iNotificationHandler;
        /** A pointer to the setting handler AO, owned */
        CSCPLockSettingEventHandler*       iSettingHandler;
        /** The current lock command state */
        TSCPLockCmdState                   iLockCommandState;
        /** The current unlock command state */
        TBool                              iUnlockMessageSent;
        /** The timeout handler ptr, if created. Owned, */
        CSCPTimer*                         iLockOperationTimer;
        /** A pointer to the request status for a pending call, not owned */
        TRequestStatus*                    iPendingCallStatus; 
        /** An integer variable to define the input mode of the lock code query */
        TInt def_mode;
    	/** A character variable to get the very first character of the lock code */
    	TChar ch;
    	
    	/* Parameter ID (wil always be either ESCPAutolockPeriod or 
    	 * ESCPMaxAutolockPeriod) that will be referred to notify the 
    	 * stakeholders in case of async notification
    	 */
    	TInt iALParamID;
    	
    	/* Parameter value corresponding to iALParamID that
    	 * will be referred to notify the stakeholders in case 
    	 * of async notification. The memory for this is allocated in 
    	 * HandleSetParamMessageL() and deallocated in LockOperationCompleted()
    	 * or the destructor.
    	 */
    	HBufC8* iALParamValue;

    	/* The SID of the app that set the above two (iALParamID, iALParamValue) fields
    	 */
    	TUint32 iALParamCallerID;
    };

#endif      // SCPSESSION_H   
            
// End of File

