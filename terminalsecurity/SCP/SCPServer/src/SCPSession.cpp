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


// INCLUDE FILES
#include <e32debug.h>
#include <e32math.h>
#include <e32cmn.h>



#include    <e32property.h>
#include    <PSVariables.h>   // Property values
#include 	<coreapplicationuisdomainpskeys.h>

 
#include "SCPServer.h"
#include "SCPSession.h"
#include "SCPLockSettingEventHandler.h"
#include "SCPLockNotificationEventHandler.h"

// For Central Repository
#include <centralrepository.h>
#include "SCPCodePrivateCRKeys.h"
#include "SCPLockCode.h"

#ifdef SCP_SMS_LOCK_AVAILABLE
#include <RemoteLockSettings.h>
#endif // SCP_SMS_LOCK_AVAILABLE

//#ifdef __SAP_DEVICE_LOCK_ENHANCEMENTS
#include <SCPParamObject.h>
//#endif // __SAP_DEVICE_LOCK_ENHANCEMENTS
#include <featmgr.h>
#include "SCPDebug.h"
#include <TerminalControl3rdPartyAPI.h>
#include <TerminalControl3rdPartyParamsPlugin.h>
#include <utf.h>
// ================= MEMBER FUNCTIONS =======================

// C++ default constructor can NOT contain any code, that
// might leave.
//
CSCPSession::CSCPSession( CSCPServer& aServer )
	: CSession2(),
	iServer( aServer ),
	iLockCommandState( ESCPLockCmdStateIdle ),
	iUnlockMessageSent( EFalse ),
	iLockOperationTimer( NULL ),
	iPendingCallStatus( NULL ),
	iALParamValue(NULL)
	{
	}

// Symbian 2nd phase constructor can leave.
void CSCPSession::ConstructL()
    {
    iServer.SessionOpened();
    }

// Static constructor.
CSCPSession* CSCPSession::NewL( CSCPServer& aServer )
	{
	CSCPSession* self = CSCPSession::NewLC( aServer );
	CleanupStack::Pop( self );
	
	return self;	
	}

// Static constructor, leaves object pointer to the cleanup stack.
CSCPSession* CSCPSession::NewLC( CSCPServer& aServer )
    {
    CSCPSession* self = new(ELeave) CSCPSession( aServer );
    
    CleanupStack::PushL( self );
    self->ConstructL();

    return self;
    }

    
// Destructor
CSCPSession::~CSCPSession()
    {
    Dprint( (_L("--> CSCPSession::~CSCPSession()")) );
     
    delete iLockOperationTimer;
    iLockOperationTimer = NULL;
    
    delete iNotificationHandler; 
    iNotificationHandler = NULL;   
    
    delete iSettingHandler;    
    iSettingHandler = NULL;
    
    iPendingCallStatus = NULL;
    
    iServer.SessionClosed();
    
    if(iALParamValue) {
		delete iALParamValue;		
    }
        
    Dprint( (_L("<-- CSCPSession::~CSCPSession()")) );   
    }



// ---------------------------------------------------------
// TBool CSCPSession::IsOperationInProgress( TSCPAdminCommand aCommand )
// Checks from the state variable, if the given command is actually in progress
// 
// Status : Approved
// ---------------------------------------------------------
//
TBool CSCPSession::IsOperationInProgress( TSCPAdminCommand aCommand )
    {    
    Dprint( (_L("--> CSCPSession::IsOperationInProgress( %d )"), aCommand ));
    
    TBool ret = EFalse;
    
    if ( aCommand == ESCPCommandLockPhone )
        {
        if (    ( iLockCommandState == ESCPLockCmdStateInProgress )
            ||  ( iLockCommandState == ESCPLockCmdStateInProgressAcknowledged )
           )
            {
            ret = ETrue; 
            }
        }
        
    if ( aCommand == ESCPCommandUnlockPhone )
        {
        if ( iUnlockMessageSent )
            {
            ret = ETrue;
            }
        }
           
    Dprint( (_L("<-- CSCPSession::IsOperationInProgress(): %d"), ret ));
    
    return ret;
    }
    
// ---------------------------------------------------------
// TBool CSCPSession::AcknowledgeOperation( TSCPAdminCommand aCommand )
// Set the state variable accordingly, if it was set in a waiting state
// and return the status value respectively.
// 
// Status : Approved
// ---------------------------------------------------------
//
TBool CSCPSession::AcknowledgeOperation( TSCPAdminCommand aCommand )
    {
    Dprint( (_L("--> CSCPSession::AcknowledgeOperation( %d )"), aCommand ));
    
    // The return parameter, set this if the ack was awaited
    TBool ret = EFalse;
    
    if ( aCommand == ESCPCommandLockPhone )        
        {
        switch ( iLockCommandState )
            {
            case ( ESCPLockCmdStateInProgress ):
                {                                                      
                // Acknowledgement received, now just wait for the actual 
                // call to finish
                Dprint( (_L("CSCPSession::AcknowledgeOperation( %d ): \
                    Lock-op acknowledged"), aCommand ));
                iLockCommandState = ESCPLockCmdStateInProgressAcknowledged;
                iNotificationHandler->AckReceived();
                                
                ret = ETrue;
                break;
                }
            
            case ( ESCPLockCmdStateFinishedAckPending ):                
                {
                // OK, the operation is completed
                Dprint( (_L("CSCPSession::AcknowledgeOperation( %d ): \
                    Lock-op acknowledged"), aCommand ));
                iLockCommandState = ESCPLockCmdStateIdle;
                iUnlockMessageSent = EFalse;                    
                
                Dprint( (_L("CSCPSession::AcknowledgeOperation( %d ): \
                    Completing wait"), aCommand ));
                User::RequestComplete( iPendingCallStatus, KErrNone );
                iPendingCallStatus = NULL;
                                    
                ret = ETrue;                    
                break;
                }                
                
            case ( ESCPLockCmdStateIdle ): 
            // Flow through, nothing here
            
            case ( ESCPLockCmdStateInProgressAcknowledged ):
            // Flow through, nothing here  
            
            default:
            // No action          
            break;                
            }
        }
        
    if ( aCommand == ESCPCommandUnlockPhone )
        {
        if ( iUnlockMessageSent )
            {
            Dprint( (_L("CSCPSession::AcknowledgeOperation( %d ): \
                Completing Unlock-event"), aCommand ));
            
            ret = ETrue;
            // Complete the call
            iUnlockMessageSent = EFalse;            
            User::RequestComplete( iPendingCallStatus, KErrNone );
            iPendingCallStatus = NULL;
            }
        }        
    
    Dprint( (_L("<-- CSCPSession::AcknowledgeOperation(): %d"), ret ));    
    return ret;
    }        

// ---------------------------------------------------------
// void CSCPSession::HandleGetCodeMessageL( const RMessage2 &aMessage )
// Gets the stored code from the server and returns it to the
// client thread.
// 
// Status : Approved
// ---------------------------------------------------------
//
void CSCPSession::HandleGetCodeMessageL( const RMessage2 &aMessage )
    {    
    Dprint( (_L("--> CSCPSession::HandleGetCodeMessageL()") ));
   
    TSCPSecCode code;
    code.Zero();
   
    if ( iServer.GetCode( code ) != KErrNone )
        {
        User::Leave( KErrGeneral );
        }   
      
    aMessage.WriteL( 0, code );    
   
    Dprint( (_L("<-- CSCPSession::HandleGetCodeMessageL()") ));  
    }

// ---------------------------------------------------------
// void CSCPSession::HandleSetCodeMessageL( const RMessage2 &aMessage )
// Reads the code from the client buffer, and sends it to the server.
// 
// Status : Approved
// ---------------------------------------------------------
//
void CSCPSession::HandleSetCodeMessageL( const RMessage2 &aMessage )
    {
    Dprint( (_L("--> CSCPSession::HandleSetCodeMessageL()") ));
    RMobilePhone::TMobilePassword code;
      
    aMessage.ReadL( 0, code );    
   
    TSCPSecCode curCode;
    iServer.GetCode( curCode );
   
    if ( curCode.Compare( code ) == 0 )
        {
        TInt retErrorNone = KErrNone;
        // The codes are already the same.
        User::Leave( retErrorNone );
        }
   
#ifndef __WINS__    
    // No access to the ISA side in emulator
       
    // Check the given code, leave on error
    iServer.CheckISACodeL( code );
#endif //  __WINS__

    TInt ret = iServer.StoreCode( code ); 
    if ( ret != KErrNone )
        {
        User::Leave( ret );
        }
      
    Dprint( (_L("<-- CSCPSession::HandleSetCodeMessageL()") ));
    }
   
   
//#ifndef __SAP_DEVICE_LOCK_ENHANCEMENTS 

// ---------------------------------------------------------
// void CSCPSession::HandleChangeCodeMessageL( const RMessage2 &aMessage )
// Fetches the stored code from the server, reads the new code
// from the client side, and calls the method to change the code.
// 
// Status : Approved
// ---------------------------------------------------------
//
void CSCPSession::HandleChangeCodeMessageL( const RMessage2 &aMessage )
    {
    if(!FeatureManager::FeatureSupported(KFeatureIdSapDeviceLockEnhancements))
    {
    Dprint( (_L("--> CSCPSession::HandleChangeCodeMessageL()") ));                

    RMobilePhone::TMobilePassword newPassword;
    
    aMessage.ReadL( 0, newPassword );    
      
	// Possible leave will result in an error status for the client
	iServer.ChangeISACodeL( newPassword );
	
	Dprint( (_L("<-- CSCPSession::HandleChangeCodeMessageL()") ));	
	}	
	else

/*
#else // __SAP_DEVICE_LOCK_ENHANCEMENTS active

// ---------------------------------------------------------
// void CSCPSession::HandleChangeCodeMessageL( const RMessage2 &aMessage )
// Send the code in slot 0 to the server for storage. 
//
// 
// Status : Approved
// ---------------------------------------------------------
//
void CSCPSession::HandleChangeCodeMessageL( const RMessage2 &aMessage )
*/
    {
    Dprint( (_L("--> CSCPSession::HandleChangeCodeMessageL()") ));
    (void)aMessage;
    
    HBufC* codeHBuf = HBufC::NewLC( KSCPMaxEnhCodeLen + 1 );
    TPtr codeBuf = codeHBuf->Des();
    aMessage.ReadL( 0, codeBuf );
    
    if ( !iServer.IsValidEnhCode( codeBuf ) )
        {
        Dprint( (_L("CSCPSession::HandleChangeCodeMessageL(): ERROR:\
            Invalid code format") ));
        CleanupStack::PopAndDestroy( codeHBuf );
        User::Leave( KErrArgument );
        }
    
    iServer.StoreEnhCode( codeBuf );
    
    CleanupStack::PopAndDestroy( codeHBuf );
	
	Dprint( (_L("<-- CSCPSession::HandleChangeCodeMessageL()") ));	
	}
    }
//#endif // __SAP_DEVICE_LOCK_ENHANCEMENTS
	
	

// ---------------------------------------------------------
// void CSCPSession::HandleSetLockStateMessageL( const RMessage2 &aMessage )
// Sets the DOS lock setting to enabled/disabled, 
// and activates or deactivates Autolock via SetDOSLockSetting.
// 
// Status : Approved
// ---------------------------------------------------------
//
void CSCPSession::HandleSetLockStateMessageL( const RMessage2 &aMessage )
    {
    Dprint( (_L("--> CSCPSession::HandleSetLockStateMessageL()") ));
       
    TInt lockState = aMessage.Int0();
    
    Dprint( (_L("CSCPSession::HandleSetLockStateMessageL(), lock command: %d"),
        lockState ));
    
    SetDOSLockSettingL( lockState, ETrue, aMessage );
	
    Dprint( (_L("<-- CSCPSession::HandleSetLockStateMessageL()") ));
    }
	
	
// ---------------------------------------------------------
// void CSCPSession::SetDOSLockSettingL( TBool aLocked, const RMessage2 &aMessage )
// Checks whether a lock operation is already in progress, if not, 
// starts the operation by creating the two active objects responsible for handling
// the call, and sets the state variable accordingly.
// 
// Status : Approved
// ---------------------------------------------------------

void CSCPSession::SetDOSLockSettingL( TBool aLocked, 
                                      TBool aAutolock,
                                      const RMessage2 &aMessage )
    {
    // Only one lock/unlock phone command can be in progress at a time
    if (    ( IsOperationInProgress( ESCPCommandLockPhone ) )
            || ( IsOperationInProgress( ESCPCommandUnlockPhone ) )
            || ( iServer.IsOperationInProgress( ESCPCommandLockPhone ) )
            || ( iServer.IsOperationInProgress( ESCPCommandUnlockPhone ) )
       )
        {
        User::Leave( KErrInUse );
        }
   
    // Fetch the stored password from the server
    TSCPSecCode storedCode;
    if ( iServer.GetCode( storedCode ) != KErrNone )
        {
        Dprint( (_L("CSCPSession::HandleLockPhoneMessageL():\
            FAILED to get stored code!") ));
        User::Leave( KErrGeneral );
        }
     
    
    // Create a timer object for this call
    iLockOperationTimer = CSCPTimer::NewL( KSCPLockOperationTimeout, NULL, this );
     
    // Set the state parameter for this session, a lock command is now in progress
    iLockCommandState = ESCPLockCmdStateInProgress;
    
#ifndef __WINS__ // The notification handler is not used in emulator

    // Don't create the notification handler if it won't be used 
    // (because DOS lock won't be disabled)
    if ( ( aLocked ) || ( ( !IsSMSLockActiveL() ) && ( !IsAutolockActive() ) ) )
        {
        iNotificationHandler = CSCPLockNotificationEventHandler::NewL( &iServer, this );
        }
        
#endif // WINS
        
    iSettingHandler = CSCPLockSettingEventHandler::NewL( 
        aMessage, this, aLocked, aAutolock, &iServer );
    iSettingHandler->StartL();
    
    }


// ---------------------------------------------------------
// void CSCPSession::HandleQueryMessageL( const RMessage2 &aMessage )
// Checks if the given operation is in progress, and acknowledges the
// message. The return value is computed based on the calls, if either
// is true, so is the return value.
// 
// Status : Approved
// ---------------------------------------------------------
//
void CSCPSession::HandleQueryMessageL( const RMessage2 &aMessage )
    {
    Dprint( (_L("--> CSCPSession::HandleQueryMessageL()") ));
    
    #ifdef SCP_ENFORCE_SECURITY
    // Check the process permissions for this call
    switch ( (TSCPAdminCommand)aMessage.Int0() )
        {
        case ( ESCPCommandLockPhone ):
            {
            // Only Security Observer allowed
            if ( aMessage.SecureId() != KAknNfySrvUid /*KSCPServerSIDSecurityObserver*/ )
                {
                Dprint( (_L("CSCPSession::HandleQueryMessageL():\
                    ERROR: Permission denied") ));
                User::Leave( KErrPermissionDenied );
                }
            break;
            }
        
        case ( ESCPCommandUnlockPhone ):
            {
            // Only Autolock allowed
            if ( aMessage.SecureId() != KSCPServerSIDAutolock )
                {
                Dprint( (_L("CSCPSession::HandleSetParamMessageL():\
                    ERROR: Permission denied") ));
                User::Leave( KErrPermissionDenied );
                }
            break;
            }
                    
        default: 
            {                        
            Dprint( (_L("CSCPSession::HandleQueryMessageL():DEFAULT:\
                Permission denied") ));
            User::Leave( KErrPermissionDenied );
            }
        break;        
        }
    #endif
    
    // Check from the server, if another session has initiated a lock operation
    // Also if an acknowledgement was awaited for this operation, the return
    // value should be true
    TSCPAdminCommand commandQueried = (TSCPAdminCommand)aMessage.Int0();
        
    TBool ret1 = iServer.IsOperationInProgress( commandQueried );
    
    TBool ret2 = iServer.AcknowledgementReceived( commandQueried );
    
    TBool ret = ret1 || ret2;
    
    // Return the reply        
    TInt retWrap = (TInt)ret;
    
    TPckg<TInt> retPackage(retWrap);
    aMessage.WriteL(1, retPackage );
   
    Dprint( (_L("<-- CSCPSession::HandleQueryMessageL()") ));
    }
	
	
	
	
// ---------------------------------------------------------
// void CSCPSession::HandleGetLockStateMessageL( const RMessage2 &aMessage )
// Retrieves the Autolock status and returns it as the lock state.
// 
// Status : Approved
// ---------------------------------------------------------
//
void CSCPSession::HandleGetLockStateMessageL( const RMessage2 &aMessage )
    {
    Dprint( (_L("--> CSCPSession::HandleGetLockStateMessageL()") ));
    
    // Check the autolock variable
    
    TInt autolockState; // the state
    TInt checkFor; // the "active state"
    
    TInt err( KErrGeneral );    
   
    RProperty Property;    

#ifdef RD_REMOTELOCK
	    err = Property.Get(KPSUidCoreApplicationUIs, KCoreAppUIsAutolockStatus, autolockState);       
	    checkFor = EAutolockOff;    
#else //RD_REMOTELOCK

	err = Property.Get(KPSUidCoreApplicationUIs, KCoreAppUIsAutolockStatus, autolockState);       
    checkFor = EAutolockOn;    
       
#endif//RD_REMOTELOCK	       
    
    User::LeaveIfError( err );        
    
    TBool ret = EFalse;
#ifdef RD_REMOTELOCK    
    if ( autolockState > checkFor )
        {
        ret = ETrue;
        }
#else//RD_REMOTELOCK
    if ( autolockState == checkFor )
        {
        ret = ETrue;
        }
#endif //RD_REMOTELOCK      
    TPckg<TBool> retpackage(ret);
    
    aMessage.WriteL( 0, retpackage );
   
    Dprint( (_L("<-- CSCPSession::HandleGetLockStateMessageL()") ));
    }	
		
void CSCPSession :: HandleGetPoliciesL(const RMessage2 &aMessage) {
    #ifdef SCP_ENFORCE_SECURITY
    // Check the access for this parameter
    if((aMessage.SecureId() != KSCPServerSIDTerminalControl) &&
       (aMessage.SecureId() != KSCPServerSIDGeneralSettings) &&
       (aMessage.SecureId() != KDevEncUiUid) && 
       (aMessage.SecureId() != KSCPSecuiDialogNotifierSrv) &&
       (aMessage.SecureId() != KSecUiTest )) {
       
        Dprint((_L("CSCPSession::HandleSetParamMessageL(): ERROR: Permission denied")));
        User :: Leave( KErrPermissionDenied);
    }
    #endif // SCP_ENFORCE_SECURITY
    
    // Init a local buffer to store the return value
    HBufC8* lBuffer = HBufC8 :: NewLC(aMessage.GetDesMaxLength(1));

    iServer.GetPoliciesL(lBuffer, aMessage.SecureId().iId);
    Dprint((_L("[CSCPSession]-> iServer.GetPoliciesL complete...")));
    
    TPtr8 bufPtr = lBuffer->Des();
    // OK, send the data to the client side
    aMessage.WriteL(1, bufPtr);
    CleanupStack :: PopAndDestroy(1); // lBuffer
}
		
// ---------------------------------------------------------
// void CSCPSession::HandleSetParamMessageL( const RMessage2 &aMessage )
// Extract the parameters and send them to the server, then pack the response.
// 
// Status : Approved
// ---------------------------------------------------------
//    
void CSCPSession::HandleSetParamMessageL( const RMessage2 &aMessage )
    {    
    Dprint((_L("[CSCPSession]-> HandleSetParamMessageL() >>>")));
    
    TBool oldALState = EFalse;    
        
#ifndef __WINS__ // No need to check for lock setting changes in emulator

    if ( ( (TSCPParameterID)aMessage.Int0() == ESCPAutolockPeriod ) ||
         ( (TSCPParameterID)aMessage.Int0() == ESCPMaxAutolockPeriod ) )    
        {
        // Save the old Autolock state, the lock setting will have to be
        // changed, if the period is changed from 0 -> X or from X -> 0.        
        oldALState = IsAutolockActive();               
        }

#endif // __WINS__
    
    // Copy the client data into a local buffer
    HBufC* setBuffer = HBufC::NewLC( aMessage.GetDesLength(1) );
    TPtr bufPtr = setBuffer->Des();
    aMessage.ReadL( 1, bufPtr );
    
    TBool lNotifyChange = EFalse;
    TPckgBuf<TUint32> lCallerIDBuf;
    aMessage.ReadL(2, lCallerIDBuf);
    TBool lSetParamRequired = ETrue;
    const TSCPParameterID lParamID = (TSCPParameterID)aMessage.Int0();

    /* Check if the new value is the same as the old one, call SCPServer 
    * only if the values are different (or old value was not found)...
    */
    switch(lParamID) {
        case ESCPMaxAutolockPeriod:
        case RTerminalControl3rdPartySession :: EPasscodeRequireUpperAndLower:
        case RTerminalControl3rdPartySession :: EPasscodeRequireCharsAndNumbers:
        case RTerminalControl3rdPartySession :: EPasscodeExpiration:
        case RTerminalControl3rdPartySession :: EPasscodeMaxRepeatedCharacters:
        case RTerminalControl3rdPartySession :: EPasscodeMinChangeTolerance:
        case RTerminalControl3rdPartySession :: EPasscodeMinChangeInterval:
        case RTerminalControl3rdPartySession :: EPasscodeCheckSpecificStrings:
        case RTerminalControl3rdPartySession :: EPasscodeMaxAttempts:
        case RTerminalControl3rdPartySession :: EPasscodeConsecutiveNumbers:
        case RTerminalControl3rdPartySession :: EPasscodeHistoryBuffer:
        case RTerminalControl3rdPartySession :: EPasscodeMinSpecialCharacters:
        case RTerminalControl3rdPartySession :: EPasscodeMinLength:
        case RTerminalControl3rdPartySession :: EPasscodeMaxLength:
        case RTerminalControl3rdPartySession :: EPasscodeDisallowSimple: {
			HBufC* lOldValue = HBufC :: NewLC(25);
			TPtr lOldValPtr = lOldValue->Des();
			Dprint(_L("[CSCPSession]-> Initiating GetParameterValueL(), fetching earlier values..."));
			TInt ret = iServer.GetParameterValueL(lParamID, lOldValPtr, aMessage.SecureId());
			Dprint(_L("[CSCPSession]-> GetParameterValueL() completed, ret=%d"), ret);
			switch(ret) {
			case KErrNone:
				// If the old and the new values are same, no need to set them in SCPServer
				if(0 == (lOldValue->Des().CompareC(setBuffer->Des()))) {
					Dprint(_L("[CSCPSession]-> INFO: Old and New values are the same..."));
					lSetParamRequired = EFalse;
					lNotifyChange = EFalse;
				}
				else {
					lNotifyChange = ETrue;
				}
				break;
			case KErrNotFound:
				lNotifyChange = ETrue;
				break;
			default:
				User :: LeaveIfError(ret);
			}
		    CleanupStack :: PopAndDestroy(lOldValue);
        }
        break;
        default:
        	break;
    }
              
    if(lSetParamRequired) {
        Dprint(_L("[CSCPSession]-> Initiating SetParameterValueL()..."));
        TInt ret = iServer.SetParameterValueL(lParamID, *setBuffer, lCallerIDBuf());        
        Dprint(_L("[CSCPSession]-> SetParameterValueL() returned, ret=%d"), ret);
    User::LeaveIfError( ret );
    }
    
    // Should the message be completed immediatelly?
    TBool completeRequest = ETrue;    
        
#ifndef __WINS__ // No need to check for lock setting changes in emulator    
    
    if ( ( lParamID == ESCPAutolockPeriod ) ||
         ( lParamID == ESCPMaxAutolockPeriod ) )    
        {
        // Autolock state may have changed, check if the lock setting must be changed.        
        TBool currentALState = IsAutolockActive();                   
        
        if ( ( !oldALState ) && ( currentALState ) )
            {
            // Autolock activated, enable lock
            Dprint( (_L("CSCPSession::HandleSetParamMessageL(): Enabling DOS lock") ));
            SetDOSLockSettingL( ETrue, EFalse, aMessage );
            completeRequest = EFalse;
            }
        else if ( ( oldALState ) && ( !currentALState ) )
            {
            //Autolock deactivated, disable lock, if SMS Lock is not active
            if ( !IsSMSLockActiveL() )
                {
                Dprint( (_L("CSCPSession::HandleSetParamMessageL():\
                    Disabling DOS lock") ));
                SetDOSLockSettingL( EFalse, EFalse, aMessage );
                completeRequest = EFalse;                
                }
            else
                {
                Dprint( (_L("CSCPSession::HandleSetParamMessageL(): SMS Lock active,\
                    leaving DOS lock on") ));
                }
            }
        else
            {
            Dprint( (_L("CSCPSession::HandleSetParamMessageL(): Autolock (Max.) period\
                changed, no lock change") ));
            }
        }
        
#endif // __WINS__        
       
    
    // Ok, complete the message here
    if(completeRequest) {
		Dprint(_L("[CSCPSession]-> aMessage.Complete()..."));
        aMessage.Complete( KErrNone );
        
        if(lNotifyChange) {
            HBufC8* lTmpBuff = HBufC8 :: NewLC(setBuffer->Length());
            lTmpBuff->Des().Copy(setBuffer->Des());

            // No need to return an error condition if notification (subsequent) fails!
			TRAPD(lErr, NotifyChangeL(lParamID, lTmpBuff->Des(), lCallerIDBuf()));
			
			if(KErrNone != lErr) {
				Dprint(_L("[CSCPSession]-> INFO: NotifyAllStakeHoldersL() could not complete, lErr=%d"), lErr);
			}
			
            CleanupStack :: PopAndDestroy(); // lTmpBuff
        }
    }
    else {
		/* Control gets here only if lParamID == ESCPMaxAutolockPeriod or
		 * if lParamID == ESCPAutolockPeriod and if the 
		*/
		iALParamID = lParamID;
		iALParamCallerID = lCallerIDBuf();
		
		if(iALParamValue) {
			delete iALParamValue;
			iALParamValue = NULL;
		}
		
		// No need to return an error condition if notification (subsequent) fails!
		TRAPD(lErr, iALParamValue = HBufC8 :: NewL(setBuffer->Length()));
		
		if(lErr == KErrNone) {
			iALParamValue->Des().Copy(setBuffer->Des());
		}
    }
    
    CleanupStack :: PopAndDestroy(setBuffer);
    Dprint((_L("[CSCPSession]-> HandleSetParamMessageL() <<<"))); 
}    
     
// ---------------------------------------------------------
// void CSCPSession::HandleGetParamMessageL( const RMessage2 &aMessage )
// Check the access permissions, then extract the parameters and send 
// them to the server, then pack the response.
// 
// Status : Approved
// ---------------------------------------------------------
//    
void CSCPSession::HandleGetParamMessageL( const RMessage2 &aMessage )
    {
    Dprint( (_L("--> CSCPSession::HandleGetParamMessageL()") ));
    
#ifdef SCP_ENFORCE_SECURITY
    // Check the access for this parameter
    switch ( (TSCPParameterID)aMessage.Int0() ) {
        default:        
            // For the current parameters, we allow Gs and Terminal Control Server
            if(( aMessage.SecureId() != KSCPServerSIDTerminalControl ) &&
               ( aMessage.SecureId() != KSCPServerSIDGeneralSettings ) &&
               ( aMessage.SecureId() != KDevEncUiUid ) &&
               ( aMessage.SecureId() != KSCPSecuiDialogNotifierSrv ) &&
               ( aMessage.SecureId() != KSecUiTest ) ) {
                    Dprint( (_L("CSCPSession::HandleSetParamMessageL(): ERROR:\
                        Permission denied") ));
                    User :: Leave( KErrPermissionDenied );
            }
        break;        
        }
#endif // SCP_ENFORCE_SECURITY
    
    // Init a local buffer to store the return value
    HBufC* getBuffer = HBufC::NewLC( aMessage.GetDesMaxLength(1) );
    TPtr bufPtr = getBuffer->Des();    
    
    TInt ret = iServer.GetParameterValueL( 
            (TSCPParameterID)aMessage.Int0(), 
            bufPtr,
            aMessage.SecureId()
        );
        
    User::LeaveIfError( ret );
    
    // OK, send the data to the client side
    aMessage.WriteL( 1, bufPtr );
    
    CleanupStack::PopAndDestroy( getBuffer );
    
    Dprint( (_L("<-- CSCPSession::HandleGetParamMessageL()") ));         
    }   
		
		
		
		
// ---------------------------------------------------------
// void CSCPSession::HandleCheckConfigMessageL( const RMessage2 &aMessage )
// Propagate the check config -call to the server.
// 
// Status : Approved
// ---------------------------------------------------------
//  		
void CSCPSession::HandleCheckConfigMessageL( const RMessage2 &aMessage )
    {
    // Check if the server configuration is in sync
    TInt valerr = KErrNone;
    
    TRAP( valerr, iServer.ValidateConfigurationL( aMessage.Int0() ) );
    Dprint( (_L("CSCPServer::ConstructL(): Validation returned: %d"), valerr ));
    
    // Write the return value to slot 1
    TPckg<TInt> retPackage(valerr);
    
    aMessage.WriteL( 1, retPackage );    
    }




//#ifdef __SAP_DEVICE_LOCK_ENHANCEMENTS

// ---------------------------------------------------------
// void CSCPSession::HandleAuthenticationMessageL()
// Copy the given data into a local buffer, and propagate the call
// to the server.
//
// Status : Approved
// ---------------------------------------------------------
//
void CSCPSession :: HandleAuthenticationMessageL( const RMessage2 &aMessage ) {

    if(!FeatureManager::FeatureSupported(KFeatureIdSapDeviceLockEnhancements))
	{	
		FeatureManager::UnInitializeLib();
		User::Leave(KErrNotSupported);
	}
    
    TSecureId id = aMessage.SecureId();
	
    switch(id.iId) {
        case KSCPSTIF:
        case KSCPServerSIDAutolock:
        case KAknNfySrvUid:
        case KSCPServerSIDGeneralSettings:
        case KSCPServerSIDSysAp:
        case KSCPServerSIDTerminalControl:
        case KSCPServerSIDTelephone:
        case KSCPServerSIDLog:
        case KSCPSecuiDialogNotifierSrv:
        case KSecUiTest:
            break;
        default: {
            Dprint( (_L("[CSCPSession]-> ERROR: Permission denied") ));
            User::Leave( KErrPermissionDenied );
        }
    };
	
    Dprint( (_L("--> CSCPSession::HandleAuthenticationMessage()") ));
    
    HBufC* servBuf = NULL;
    TInt status = KErrNone;
    
    // The slot 0 contains the given password
    TInt passwordLen = aMessage.GetDesLength(0);
    if ( passwordLen > 0 )
        {
        servBuf = HBufC::NewLC( passwordLen );
        }
    else
        {
        // We cannot accept an empty code
        Dprint( (_L("<-- ERROR: CSCPSession::HandleAuthenticationMessage():\
            returning KErrArgument, buffer empty") ));
        User::Leave( KErrArgument );
        }
        
    TPtr servBufPtr = servBuf->Des();
    
    TInt ret = aMessage.Read( 0, servBufPtr );
    
    // Try to get the ISA code from the server, giving the password and flags
    TSCPSecCode isaCodeToReturn;
    CSCPParamObject* addParams = NULL;
    TInt flags = aMessage.Int3();
    if ( ret == KErrNone )
        {        
        status = iServer.CheckCodeAndGiveISAL( servBufPtr,
                                            isaCodeToReturn,
                                            addParams,
                                            flags );
        }    
    
    servBufPtr.Zero();
    CleanupStack::PopAndDestroy( servBuf );
    
    if ( ret != KErrNone )
        {
        // Fail the call
        Dprint( (_L("<-- ERROR: CSCPSession::HandleAuthenticationMessage():\
            returning %d"), ret ));
        User::Leave( ret );            
        }
    // read failed polices to msg buffer
    if (addParams != NULL)
        {
        ReadFailedPoliciestoMsgBufferL(addParams,aMessage,2);
        }
        delete addParams;
    // OK, we either have the correct ISA code or don't, return it to the client in slot 1
    User::LeaveIfError( aMessage.Write( 1, isaCodeToReturn ) );    
            
    Dprint( (_L("<-- CSCPSession::HandleAuthenticationMessage(): %d"), status ));    
    User::LeaveIfError( status );
    }
    
    
// ---------------------------------------------------------
// void CSCPSession::HandleChangeEnhCodeMessageL()
// Copy the given data into local buffers, and propagate the call
// to the server.
//
// Status : Approved
// ---------------------------------------------------------
//
void CSCPSession :: HandleChangeEnhCodeMessageL(const RMessage2 &aMessage) {
    
    Dprint(_L("[CSCPSession]-> HandleChangeEnhCodeMessage() >>>"));
    
    if(!FeatureManager :: FeatureSupported(KFeatureIdSapDeviceLockEnhancements)) {	
		FeatureManager :: UnInitializeLib();
		User :: Leave(KErrNotSupported);
	}
    
    TInt oldPasswordLen = aMessage.GetDesLength(0);
    TInt newPasswordLen = aMessage.GetDesLength(1);
    
    if ((newPasswordLen == 0) || (oldPasswordLen == 0)) {
        // We cannot accept an empty code
        Dprint(_L("[CSCPSession]-> ERROR: Input buffers are empty!!"));
        User :: Leave(KErrArgument);
    }

    TInt lErr = KErrNone;
    HBufC* oldPassBuf = HBufC :: NewLC(oldPasswordLen);
    HBufC* newPassBuf = HBufC :: NewLC(newPasswordLen);
    
    TPtr oldPassPtr = oldPassBuf->Des();
    TPtr newPassPtr = newPassBuf->Des();
    
    // Read the strings, slot 0 = old password, slot 1 = new password  
    aMessage.ReadL(0, oldPassPtr);
    aMessage.ReadL(1, newPassPtr);
        
    // A buffer for the updated DOS password
    TSCPSecCode newDosCode;
    newDosCode.Zero();
    
    CSCPParamObject* addParams = NULL;    
    lErr = iServer.CheckAndChangeEnhCodeL(oldPassPtr, newPassPtr, addParams, newDosCode);    
    CleanupStack :: PushL(addParams);
    
    if (addParams != NULL) {
        ReadFailedPoliciestoMsgBufferL(addParams, aMessage, 2);
    }
    
    CleanupStack :: PopAndDestroy(1); //addParams

    if(lErr == KErrNone) {
        /* Get the very first character of the new lock code and set the default input mode of the
        lock code query on the basis of the first character. */
        ch = newPassPtr[0];
        def_mode = ch.IsDigit() ? 0 : 1;
    
        CRepository* repository = CRepository :: NewLC(KCRUidSCPParameters);
        User :: LeaveIfError(repository->Set(KSCPLockCodeDefaultInputMode, def_mode));
        CleanupStack :: PopAndDestroy(repository);
        
        /* Set the value in the cenrep that the default lock code has been changed if it is not 
         * already set
         * */
        repository = CRepository :: NewLC(KCRUidSCPLockCode);
        User :: LeaveIfError(repository->Set(KSCPLockCodeDefaultLockCode, 0));
        CleanupStack :: PopAndDestroy(repository);
    }
    
    CleanupStack :: PopAndDestroy(2); //newPassBuf, oldPassBuf
    
    Dprint(_L("[CSCPSession]-> HandleChangeEnhCodeMessage() lErr=%d <<<"), lErr);
    User :: LeaveIfError(lErr);
}   
    
// ---------------------------------------------------------
// void CSCPSession::HandleQueryChangeMessageL()
// Check from the server, if the password is allowed to be changed.
//
// Status : Approved
// ---------------------------------------------------------
//
    
void CSCPSession::HandleQueryChangeMessageL( const RMessage2 &aMessage )
    {
    Dprint( (_L("--> CSCPSession::HandleQueryChangeMessage()") ));
    if(!FeatureManager::FeatureSupported(KFeatureIdSapDeviceLockEnhancements))
	{	
        Dprint( (_L("--> CSCPSession::HandleQueryChangeMessage() .. Feature not Supported !!") ));
		FeatureManager::UnInitializeLib();
		User::Leave(KErrNotSupported);
	}
    
    CSCPParamObject* addParams = NULL;
    
    TInt ret = iServer.IsPasswordChangeAllowedL( addParams );        
    if (ret != KErrNone)
        {
        ReadFailedPoliciestoMsgBufferL(addParams,aMessage,0);
        }
    if (addParams != NULL)
        delete addParams;
    
    Dprint( (_L("<-- CSCPSession::HandleQueryChangeMessage()") ));    
    }

    
//#endif // __SAP_DEVICE_LOCK_ENHANCEMENTS


		
// ---------------------------------------------------------
// void CSCPSession::LockOperationCompleted()
// The method is called from iSettinghandler, when the call has
// been completed. The active objects are deleted, and the state
// is set to reflect the situation.
// 
// Status : Approved
// ---------------------------------------------------------
//
void CSCPSession::LockOperationCompleted()
   {
   Dprint( (_L("--> CSCPSession::LockOperationCompleted()") ));
   
   // Delete the timer, the call is complete
   if ( iLockOperationTimer )
       {
       delete iLockOperationTimer;
       iLockOperationTimer = NULL;
       }
      
   if ( iNotificationHandler )
       {
       delete iNotificationHandler;    
       iNotificationHandler = NULL;   
       }

   if ( iSettingHandler )
       {
       delete iSettingHandler;   
       iSettingHandler = NULL;
       }
       
   iPendingCallStatus = NULL;
   
   iLockCommandState = ESCPLockCmdStateIdle;
   iUnlockMessageSent = EFalse;   
       
   if(iALParamValue) {
	   TRAPD(lErr, NotifyChangeL(iALParamID, iALParamValue->Des(), iALParamCallerID));
	   if(lErr == KErrNone) {
		   Dprint( (_L("[CSCPSession]-> INFO: Notification complete...") ));
	   }
	   else {
		   Dprint( (_L("[CSCPSession]-> INFO: Unable to complete notification...") ));	   
	   }
	   delete iALParamValue;
	   iALParamValue = NULL;
   }
   Dprint( (_L("<-- CSCPSession::LockOperationCompleted()") ));
   }


// ---------------------------------------------------------
// void CSCPSession::Timeout( TAny* aParam )
// This method will be called in case the lock operation times
// out.
// 
// Status : Approved
// ---------------------------------------------------------
//
void CSCPSession::Timeout( TAny* aParam )
    {
    Dprint( (_L("--> CSCPSession::Timeout()") ));  
    (void)aParam;  
                                
    // Since the timeout should only occur if the system is
    // somehow in an invalid state, reset all the state information
    // here.
    
    if ( ( iUnlockMessageSent ) || 
         ( iLockCommandState == ESCPLockCmdStateFinishedAckPending ) 
       )
        {
        // Signal timeout by completing the request                
        User::RequestComplete( iPendingCallStatus, KErrCancel ); 
        iPendingCallStatus = NULL;           
        }
    else
        {
        if ( iNotificationHandler )
            {
            iNotificationHandler->Cancel();
            }
        iSettingHandler->Cancel();
        }  
        
    iUnlockMessageSent = EFalse;                      
    iLockCommandState = ESCPLockCmdStateIdle;
       
    Dprint( (_L("<-- CSCPSession::Timeout()") ));
    }

// ---------------------------------------------------------
// void CSCPSession::LockOperationPending( TSCPAdminCommand aCommand,
//                                         TRequestStatus* aStatus )
// Saves the status ptr of the pending call, and sets the relevant
// state variable.
// 
// Status : Approved
// ---------------------------------------------------------
//
void CSCPSession::LockOperationPending( TSCPAdminCommand aCommand, 
                                        TRequestStatus* aStatus )
    {
    Dprint( (_L("--> CSCPSession::LockOperationPending()") ));
    iPendingCallStatus = aStatus;
    *iPendingCallStatus = KRequestPending;    
        
    switch ( aCommand )
        {
        case ( ESCPCommandUnlockPhone ) :
            {
            iUnlockMessageSent = ETrue;
            break;
            }

        case ( ESCPCommandLockPhone ) :
            {
            iLockCommandState = ESCPLockCmdStateFinishedAckPending;
            break;
            }                                
        }    
    
    Dprint( (_L("<-- CSCPSession::LockOperationPending()") ));
    }


// ---------------------------------------------------------
// TBool CSCPSession::IsSMSLockActiveL()
// Retrieves the SMS Lock status using the Remote Lock 
// Settings API.
// 
// Status : Approved
// ---------------------------------------------------------
//
TBool CSCPSession::IsSMSLockActiveL()
    {   
    TBool smsLockState = EFalse;
    
    #ifdef SCP_SMS_LOCK_AVAILABLE    
    
    CRemoteLockSettings* smsLockSettings = CRemoteLockSettings::NewLC();
        
    if ( !smsLockSettings->GetEnabled( smsLockState ) )
        {
        smsLockState = EFalse;
        }
    
    CleanupStack::PopAndDestroy( smsLockSettings );
    
    #endif // SCP_SMS_LOCK_AVAILABLE
    
    return smsLockState;
    }
                
   
// ---------------------------------------------------------
// TBool CSCPSession::IsAutolockActive()
// Retrieves the Autolock status using by retrieving
// the period from the server.
// 
// Status : Approved
// ---------------------------------------------------------
//
TBool CSCPSession::IsAutolockActive()
    {   
    TBool ret = EFalse;    
    
    TInt autolockPeriod = 0;
    TInt servRet = KErrNone;       
    TRAPD( err, servRet = iServer.GetAutolockPeriodL( autolockPeriod ) );    
    
    if ( ( err == KErrNone ) && ( servRet == KErrNone ) && ( autolockPeriod > 0 ) )
        {
        ret = ETrue;
        }
        
    return ret;
    }
    
                
// ---------------------------------------------------------
// void CSCPSession::ServiceL( const RMessage2 &aMessage )
// The initial entry point for a service call, propagates the
// call based on the function.
// 
// Status : Approved
// ---------------------------------------------------------
//
void CSCPSession::ServiceL( const RMessage2 &aMessage )
	{
	switch ( aMessage.Function() )
	    {
	    case ( ESCPServSetPhoneLock ):
	        {
	        TRAPD( err, HandleSetLockStateMessageL( aMessage ) );
	        if ( err != KErrNone )
	            {
	            aMessage.Complete( err );
	            }
	        }
	    break;
	   
        case ( ESCPServSetParam ):
            {
            TRAPD( err, HandleSetParamMessageL( aMessage ) );
	        if ( err != KErrNone )
	            {
	            aMessage.Complete( err );
	            }            
            }            
        break; 	   
	   
        case ESCPApplicationUninstalled: {
            TInt lErr = KErrNone;
            TRAP(lErr, HandleCleanupL(aMessage));
            if(lErr != KErrNone) {
                aMessage.Complete(lErr);
            }
        }        
        break;
        case ESCPServUISetAutoLock: {
            TInt lErr = KErrNone;
            TRAP(lErr, HandleSetALPeriodL(aMessage));
            if(lErr != KErrNone) {
                aMessage.Complete(lErr);
            }
        }
        break;
	    default:
	        {
	        TRAPD( err, DispatchSynchronousMessageL(aMessage) );
	        aMessage.Complete( err );
	        }
	        break;	   
	    }
    }


// ---------------------------------------------------------
// void CSCPSession::DispatchSynchronousMessageL( const RMessage2 &aMessage )
// A dispatch function for synchronous calls, calls the function dependent
// handler method.
// 
// Status : Approved
// ---------------------------------------------------------
//
void CSCPSession::DispatchSynchronousMessageL( const RMessage2 &aMessage )
	{
	Dprint( (_L("--> CSCPSession::DispatchSynchronousMessageL()") ));

	switch ( aMessage.Function() )
        {
		case ( ESCPServGetCode ):
		    {
		    HandleGetCodeMessageL( aMessage );			
		    break;	
		    }		    		

//#ifndef __SAP_DEVICE_LOCK_ENHANCEMENTS  ..recheck
        // No longer supported for enhanced security code		
		case ( ESCPServSetCode ):
		    {
		    if(!FeatureManager::FeatureSupported(KFeatureIdSapDeviceLockEnhancements))
			{
		    	HandleSetCodeMessageL( aMessage );			
			}
			else
		    {
		    	User::Leave(KErrNotSupported);
		    }
		    break;	
		    }		
//#endif // __SAP_DEVICE_LOCK_ENHANCEMENTS		    
		
        case ( ESCPServChangeCode ):
            {
            HandleChangeCodeMessageL( aMessage );
            break;    
            }
      
        case ( ESCPServQueryAdminCmd ):
            {
            HandleQueryMessageL( aMessage );
            break;      		
            }                   
		
        case ( ESCPServGetLockState ):
            {
            HandleGetLockStateMessageL( aMessage );
            break; 
            }
                    
        case ( ESCPServGetParam ):
            {
                if(aMessage.Int0() == -1) {
                    HandleGetPoliciesL(aMessage);
                }
                else {
                    HandleGetParamMessageL(aMessage);
                }
                
            break; 
            }
            
        case ( ESCPServCheckConfig ):
            {
            HandleCheckConfigMessageL( aMessage );
            break; 
            }
                        
//#ifdef __SAP_DEVICE_LOCK_ENHANCEMENTS
        
        case ( ESCPServAuthenticateS60 ):
            {
            if(FeatureManager::FeatureSupported(KFeatureIdSapDeviceLockEnhancements))
            {
            	HandleAuthenticationMessageL( aMessage );	
            }
            else
		    {
		    	User::Leave(KErrNotSupported);
		    }
            
            break;
            }
            
        case ( ESCPServChangeEnhCode ):
            {
            if(FeatureManager::FeatureSupported(KFeatureIdSapDeviceLockEnhancements))
            {
           	 HandleChangeEnhCodeMessageL( aMessage );
            }
            else
		    {
		    	User::Leave(KErrNotSupported);
		    }
            break;
            }
            
        case ( ESCPServCodeChangeQuery ):
            {
            if(FeatureManager::FeatureSupported(KFeatureIdSapDeviceLockEnhancements))
            {
            	HandleQueryChangeMessageL( aMessage );
            }
            else
		    {
		    	User::Leave(KErrNotSupported);
		    }
            break;
            }
        case ESCPServValidateLockcode:
            {
            ValidateLockcodeAgainstPoliciesL(aMessage);
            break;
            }
            
//#endif // __SAP_DEVICE_LOCK_ENHANCEMENTS         
                           		
		default:
		    {
		    User::Leave( KErrNotSupported );
		    break;
		    }		   	    
        }

	Dprint( (_L("<-- CSCPSession::DispatchSynchronousMessageL()") ));
	}

//  End of File  

TInt CSCPSession :: NotifyAllStakeHoldersL(const RArray<const TParamChange>& aChange, TUint32 aCallerID) {
    Dprint(_L("[CSCPSession]-> NotifyAllStakeHoldersL() >>>"));
   
    if(aChange.Count() < 1) {
        return KErrNone;
    }
    
    RImplInfoPtrArray implArray;
    CleanupClosePushL(implArray);
    
    CTC3rdPartyParamsEcomIF :: ListAllImplementationsL(implArray);
    TInt numberOfPlugins = implArray.Count();
    
    Dprint(_L("[CSCPSession]-> numberOfPlugins=%d"), numberOfPlugins);
    for(TInt i = 0; i < numberOfPlugins; i++) {
        CImplementationInformation& info = *implArray[i];
        TUid implementation = info.ImplementationUid();
        HBufC* text = HBufC :: NewMaxLC(info.OpaqueData().Length() );
        TPtr opaqueData(text->Des());
        opaqueData.Copy(info.OpaqueData());

        TUint pluginUID;
        TLex lex(* text);
        lex.Val(pluginUID, EHex);
        TUid id(TUid :: Uid((TInt)pluginUID));

        if(TUint32(id.iUid) != aCallerID) {
            CTC3rdPartyParamsEcomIF* plugin = CTC3rdPartyParamsEcomIF :: NewL(implementation);
            CleanupStack :: PushL(plugin);
            TRAP_IGNORE( plugin->DeviceLockParamChangedL(aChange));
            CleanupStack :: PopAndDestroy(); // plugin 
        }

        CleanupStack :: PopAndDestroy(text);
    }

    implArray.ResetAndDestroy();
    CleanupStack :: PopAndDestroy(&implArray);
    Dprint(_L("[CSCPSession]-> NotifyAllStakeHoldersL() <<<"));
    return KErrNone;
}

TInt CSCPSession :: HandleCleanupL(const RMessage2& aMessage) {
    // Copy the client data into a local buffer
    TInt32 lCount = aMessage.GetDesLength(1);
    
/*    // If the caller is not SCPEventHandler the deny access
    if(aMessage.SecureId() != KSCPEvntHndlrUid) {
        return KErrPermissionDenied;
    }*/
    
    // Atleast one application id has to be present in the received message (atleast 8 bytes)
    if(lCount < sizeof(TInt32)) {
        return KErrArgument;
    }
    
    RArray<const TParamChange> lChangeArray;
    CleanupClosePushL(lChangeArray);
    
    RPointerArray<HBufC8> lParamValArray;
    CleanupClosePushL(lParamValArray);
    
    HBufC8* lBuffer = HBufC8 :: NewLC(lCount);
    TPtr8 bufPtr = lBuffer->Des();
    aMessage.ReadL(1, bufPtr);
    
    TInt lStatus = KErrNone;
    
    TRAPD(lErr, lStatus = iServer.PerformCleanupL(lBuffer, lChangeArray, lParamValArray));
    
    if(lErr != KErrNone) {
        Dprint(_L("[CSCPSession]-> ERROR: SCPServer was unable to complete the operation lErr=%d"), lErr);
        lParamValArray.ResetAndDestroy();
        User :: Leave(lErr);
    }
    
    TInt32 lCallerID = aMessage.Identity().iUid;
    aMessage.Complete(lStatus);
    
    Dprint(_L("[CSCPSession]-> Total parameters changed=%d "), lChangeArray.Count());
    if(lChangeArray.Count() > 0) {
        TRAP(lErr, NotifyAllStakeHoldersL(lChangeArray, lCallerID));
        
        if(lErr != KErrNone) {
            Dprint(_L("[CSCPSession]-> ERROR:  NotifyAllStakeHoldersL() could not complete, lErr=%d"), lErr);
        }
    }
    
    lParamValArray.ResetAndDestroy();
    CleanupStack :: PopAndDestroy(3); // lParamIDArray lParamValArray lBuffer    
    return lStatus;
}

TInt CSCPSession :: HandleSetALPeriodL( const RMessage2& aMessage ) {
    Dprint((_L("[CSCPSession]-> HandleSetParamMessageL() >>>")));
    TBool oldALState = EFalse;
    
    #ifndef __WINS__ // No need to check for lock setting changes in emulator
    if ( ( (TSCPParameterID)aMessage.Int0() == ESCPAutolockPeriod ) ||
         ( (TSCPParameterID)aMessage.Int0() == ESCPMaxAutolockPeriod ) )    
        {
        oldALState = IsAutolockActive();               
        }
    #endif // __WINS__
    TPckgBuf<TInt> lALPeriod;
    aMessage.ReadL(0, lALPeriod);
    User :: LeaveIfError(iServer.SetAutolockPeriodL(lALPeriod()));
    TBool completeRequest = ETrue;    
#ifndef __WINS__ // No need to check for lock setting changes in emulator    
    TBool currentALState = IsAutolockActive();                   
    if ( ( !oldALState ) && ( currentALState ) )
        {
        Dprint( (_L("CSCPSession::HandleSetParamMessageL(): Enabling DOS lock") ));
        SetDOSLockSettingL( ETrue, EFalse, aMessage );
        completeRequest = EFalse;
        }
    else if ( ( oldALState ) && ( !currentALState ) )
        {
        if ( !IsSMSLockActiveL() )
            {
            Dprint( (_L("CSCPSession::HandleSetParamMessageL():\
                Disabling DOS lock") ));
            SetDOSLockSettingL( EFalse, EFalse, aMessage );
            completeRequest = EFalse;                
            }
        else
            {
            Dprint( (_L("CSCPSession::HandleSetParamMessageL(): SMS Lock active,\
                leaving DOS lock on") ));
            }
        }
    else
        {
        Dprint( (_L("CSCPSession::HandleSetParamMessageL(): Autolock (Max.) period\
            changed, no lock change") ));
        }
#endif // __WINS__        
    if(completeRequest) {
        Dprint(_L("[CSCPSession]-> aMessage.Complete()..."));
        aMessage.Complete( KErrNone );
    }
    else {
    }
    Dprint((_L("[CSCPSession]-> HandleSetParamMessageL() <<<")));
    return KErrNone;
}

void CSCPSession :: NotifyChangeL( TInt aParamID, const TDesC8 aParamVal, TUint32 aCallerID ) {
	RArray<const TParamChange> lChangeArray;
	CleanupClosePushL(lChangeArray);
	
	const TParamChange lChange(aParamID, aParamVal);
	TInt ret = lChangeArray.Append(lChange);
	
	if(KErrNone != ret) {
		CleanupStack :: PopAndDestroy(); //lChangeArray
		return;
	}
	
	Dprint(_L("[CSCPSession]->INFO: Initiating notification to all the StakeHolders..."));
	TRAP_IGNORE( NotifyAllStakeHoldersL(lChangeArray, aCallerID));
	Dprint(_L("[CSCPSession]->INFO: Notification to all the StakeHolders complete..."));
	CleanupStack :: PopAndDestroy(); //lChangeArray
}

void CSCPSession :: ReadFailedPoliciestoMsgBufferL(CSCPParamObject*& aParamObject, const RMessage2& aMessage, TInt aSlotNumber) {
    Dprint((_L("[CSCPSession]-> ReadFailedPoliciestoMsgBufferL() >>>")));
    //get failed polices array from param object
    const RArray<TInt>& failedPolicesArray = aParamObject->GetFailedPolices();
    
    // extra one for failed policies count
    HBufC8* failedPoliciesBuf = HBufC8 :: NewLC((EDevicelockTotalPolicies+1) * sizeof(TInt32));
    TPtr8 failedpoliciesBufPtr = failedPoliciesBuf->Des();
    
    RDesWriteStream writeStream(failedpoliciesBufPtr);
    CleanupClosePushL(writeStream);
    
    writeStream.WriteInt32L(failedPolicesArray.Count());
    
    for(int count =0; count < failedPolicesArray.Count(); count++) {
        writeStream.WriteInt32L(failedPolicesArray[count]);
    }
    
    writeStream.CommitL();
    aMessage.WriteL(aSlotNumber, failedPoliciesBuf->Des());
    CleanupStack :: PopAndDestroy(2); //writeStream, failedPoliciesBuf
    Dprint((_L("[CSCPSession]-> ReadFailedPoliciestoMsgBufferL() <<<")));
}

void CSCPSession :: ValidateLockcodeAgainstPoliciesL(const RMessage2& aMessage) {
    Dprint((_L("[CSCPSession]-> ValidateLockcodeAgainstPolicies() <<<")));
    TInt lRet = KErrNone;
    HBufC* lockcodeBuf = NULL;
    CSCPParamObject* addParams = NULL;
    TInt lockcodeLen = aMessage.GetDesLength(0);
    Dprint( (_L("CSCPSession :: ValidateLockcodeAgainstPoliciesL: lockcodeLen ->%d"), lockcodeLen ));
    if (lockcodeLen == 0) {
        User :: Leave(KErrArgument);
    }
    else {
        lockcodeBuf = HBufC :: NewLC(lockcodeLen);
    }
    
    TPtr lockcodeptr = lockcodeBuf->Des();
    TRAP( lRet, aMessage.ReadL( 0, lockcodeptr ) );
    User :: LeaveIfError(lRet);
    
    lRet = iServer.ValidateLockcodeAgainstPoliciesL(lockcodeptr, addParams);
    
    if(addParams) {
        CleanupStack :: PushL(addParams);
    }
    
    if (lRet != KErrNone) {
        ReadFailedPoliciestoMsgBufferL(addParams, aMessage, 1);
    }
    
    if(addParams) {
        CleanupStack :: PopAndDestroy(addParams);
    }
    
    CleanupStack :: PopAndDestroy(1); // lockcodeBuf
    Dprint((_L("[CSCPSession]-> ValidateLockcodeAgainstPolicies() <<<")));
    User :: LeaveIfError(lRet);
}
