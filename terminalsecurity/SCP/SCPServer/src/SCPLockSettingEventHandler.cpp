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
#include <e32base.h>
#include <e32debug.h>

#include <etelmm.h>
#include <rmmcustomapi.h>



#include    <e32property.h>
#include    <PSVariables.h>   // Property values
#include 	<coreapplicationuisdomainpskeys.h>


 
#include <mmtsy_names.h>
#include <w32std.h>
#include <apgtask.h>

#include "SCPLockSettingEventHandler.h"
#include "SCPSession.h"

// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------
// void CSCPLockSettingEventHandler::CSCPLockSettingEventHandler()
// C++ default constructor
// 
// Status : Approved
// ---------------------------------------------------------

CSCPLockSettingEventHandler::CSCPLockSettingEventHandler( 
            const RMessage2& aLockMessage, 
            CSCPSession* aSession,
            TBool aState,
            TBool aAutolock,
            CSCPServer* aServer
            )
    :CSCPLockEventHandler( aServer ),
    iLockMessage( aLockMessage ),
    iSession( aSession ),
    iState( aState ),
    iAutolock( aAutolock ),
    iUnlockSent( EFalse ),
    iMessageStatus( KErrNone )
	{
	Dprint( (_L("--> CSCPLockSettingEventHandler::CSCPLockSettingEventHandler()") ));
	
	Dprint( (_L("<-- CSCPLockSettingEventHandler::CSCPLockSettingEventHandler()") ));
	}


// ---------------------------------------------------------
// void CSCPLockSettingEventHandler::ConstructL()
// Symbian 2nd phase constructor
// 
// Status : Approved
// ---------------------------------------------------------

void CSCPLockSettingEventHandler::ConstructL()
    {
    Dprint( (_L("--> CSCPLockSettingEventHandler::ConstructL()") ));
   
    BaseConstructL();         		
            
    Dprint( (_L("<-- CSCPLockSettingEventHandler::ConstructL()") ));
    }


// ---------------------------------------------------------
// void CSCPLockSettingEventHandler::StartL()
// Initiates the lock/unlock operation.
// 
// Status : Approved
// ---------------------------------------------------------

void CSCPLockSettingEventHandler::StartL()
    {
#ifndef __WINS__

    // If this is an unlock call, we must check whether SMS Lock or Autolock is active
    // before disabling the DOS side lock.
    if ( ( !iState ) && 
	     ( ( CSCPSession::IsSMSLockActiveL() ) || 
	     ( iSession->IsAutolockActive() ) ) 
	     )
        {
        Dprint( (_L("CSCPLockSettingEventHandler::ConstructL(): AL or SMS lock active,\
	         leaving DOS lock on.") ));
	    
        // Just unlock Symbian side, if Autolock handling specified
        if ( iAutolock )
            {        
            TRAPD( err, SetAutolockStateL( EFalse ) );
	    
            if ( err == KErrNone )
                {
                 // Inform the session-class that an unlock-message has been sent
                 // (the server won't respond to Autolock until the handler finishes)
                 iSession->LockOperationPending( ESCPCommandUnlockPhone, &iStatus );
                 iUnlockSent = ETrue;	                	            
                 SetActive(); // Wait for the session-class to complete this call
                }
            else 
                {
                // Failed to signal autolock, this call is complete
                iLockMessage.Complete( err );    
                iSession->LockOperationCompleted();                  
                }	        
            }
        }
    else // Set both the DOS side and the Symbian side locks, starting with SetLockSetting
        {
        RMobilePhone::TMobilePhoneLock lockType = RMobilePhone::ELockPhoneDevice;
    
        // Run the SetLockSetting-command, either to activate or to disable the lock
        Dprint( (_L("CSCPLockSettingEventHandler::ConstructL():\
          Running SetLockSetting") ));

        if ( iState )
            {
            iPhone->SetLockSetting( iStatus, lockType, RMobilePhone::ELockSetEnabled );	
            }
        else
            {
            iPhone->SetLockSetting( iStatus, lockType, RMobilePhone::ELockSetDisabled );	
            }	    

        Dprint( (_L("CSCPLockSettingEventHandler::ConstructL(): Running SetActive") ));	   
                        
        SetActive();	
        }

#else // __WINS__. In emulator we only care about Autolock
	
    if ( iAutolock )
        {
        // Autolock activation/deactivation was requested
        TRAPD( err, SetAutolockStateL( iState ) );

        if ( err != KErrNone )
            {
            // If the call failed, send the result as the message status
            iMessageStatus = err;
            }
        else if ( !iState )
            {
            // Inform the session-class that an unlock-message has been sent
            // (the server won't respond to Autolock until the handler finishes)                    
            iSession->LockOperationPending( ESCPCommandUnlockPhone, &iStatus );
            iUnlockSent = ETrue;	                	          
            SetActive(); // Wait for the session-class to complete this call
            }
        else
            {
            // This call is complete, Autolock activated
            iLockMessage.Complete( KErrNone );    
            iSession->LockOperationCompleted();
            }	
        }
	      
#endif // __WINS__    
    }


// ---------------------------------------------------------
// void CSCPLockSettingEventHandler::NewL()
// Static constructor.
// 
// Status : Approved
// ---------------------------------------------------------

CSCPLockSettingEventHandler* CSCPLockSettingEventHandler::NewL(
        const RMessage2& aLockMessage, 
        CSCPSession* aSession,
        TBool aState,
        TBool aAutolock,
        CSCPServer* aServer )
    {
    Dprint( (_L("--> CSCPLockSettingEventHandler::NewL()") ));

    CSCPLockSettingEventHandler* self = CSCPLockSettingEventHandler::NewLC( aLockMessage,
	      aSession, aState, aAutolock, aServer );
    CleanupStack::Pop( self ); 
	
    Dprint( (_L("<-- CSCPLockSettingEventHandler::NewL()") ));
    return self;	
    }
	

// ---------------------------------------------------------
// void CSCPLockSettingEventHandler::NewLC()
// Static constructor, leaves object pointer to the cleanup stack.
// 
// Status : Approved
// ---------------------------------------------------------

CSCPLockSettingEventHandler* CSCPLockSettingEventHandler::NewLC(
        const RMessage2& aLockMessage, 
        CSCPSession* aSession,
        TBool aState,
        TBool aAutolock,
        CSCPServer* aServer )
    {
    Dprint( (_L("--> CSCPLockSettingEventHandler::NewLC()") ));

    CSCPLockSettingEventHandler* self = new (ELeave) CSCPLockSettingEventHandler( 
	      aLockMessage, aSession, aState, aAutolock, aServer );

    CleanupStack::PushL( self );
    self->ConstructL();	
    
    Dprint( (_L("<-- CSCPLockSettingEventHandler::NewLC()") ));
    return self;	
    }

    
// ---------------------------------------------------------
// void CSCPLockSettingEventHandler::AckReceived()
// Destructor
// 
// Status : Approved
// ---------------------------------------------------------

CSCPLockSettingEventHandler::~CSCPLockSettingEventHandler()
    {
    Dprint( (_L("--> CSCPLockSettingEventHandler::~CSCPLockSettingEventHandler()") ));
         
    if ( IsActive() )
        {
        Dprint( (_L("CSCPLockSettingEventHandler::\
               CSCPLockSettingEventHandler(): Cancelling call") ));
        Cancel();
        }
           
    Dprint( (_L("<-- CSCPLockSettingEventHandler::~CSCPLockSettingEventHandler()") ));
    }


// ---------------------------------------------------------
// void CSCPLockSettingEventHandler::SetAutolockStateL( TBool aActive )
// Activates Autolock by signalling the SA/PubSub event, or
// deactivates it by sending it the deactivation message.
// 
// Status : Approved
// ---------------------------------------------------------

void CSCPLockSettingEventHandler::SetAutolockStateL( TBool aActive )
    {
    if ( aActive ) 
        {
        // Activation call, set the admin flag and signal the lock event        

        TInt tarmFlag = 0;        
        TInt tRet = RProperty::Get( KSCPSIDAutolock, SCP_TARM_ADMIN_FLAG_UID, tarmFlag );
        
        if ( tRet == KErrNone )
            {
            tarmFlag |= KSCPFlagAdminLock;
            tRet = RProperty::Set( KSCPSIDAutolock, SCP_TARM_ADMIN_FLAG_UID, tarmFlag );
            }

        if ( tRet != KErrNone )
            {
            #if defined(_DEBUG)
            RDebug::Print(_L("CSCPLockSettingEventHandler::SetAutolockStateL():\
                FAILED to set TARM Admin Flag"));
            #endif
            }                    
#ifdef RD_REMOTELOCK        
			// ERemoteLocked is meant for Remote Locking using SMS. Mfe/USB charging needed the lock status to be Manual.
	        tRet = RProperty::Set(KPSUidCoreApplicationUIs, KCoreAppUIsAutolockStatus, EManualLocked );
	 
#else //RD_REMOTELOCK
		
	        tRet = RProperty::Set(KPSUidCoreApplicationUIs, KCoreAppUIsAutolockStatus, EAutolockOn );
	                                                 
#endif//RD_REMOTELOCK	 	
      
   	    if ( tRet == KErrNone )
   	        {
       	    Dprint( (_L("CSCPLockSettingEventHandler::SetAutolockStateL():\
   	            Autolock signalled OK") ));   	        
   	        }
   	    else
   	        {
       	    Dprint( (_L("CSCPLockSettingEventHandler::SetAutolockStateL():\
   	            FAILED to signal Autolock: %d"), tRet ));
   	        }   	    
	    }
    else
        {
        // Deactivation call, send the deactivation message to Autolock
        RWsSession wsSession;
        if ( wsSession.Connect() != KErrNone )
           {
           Dprint( (_L("CSCPLockSettingEventHandler::SetAutolockStateL():\
               WsSession connection failed") ));        
           User::Leave( KErrGeneral );
           }
        CleanupClosePushL( wsSession );

        TApaTaskList taskList( wsSession );
        TApaTask task = taskList.FindApp( TUid::Uid( KSCPServerSIDAutolock ) ); // Autolock

        if ( task.Exists() )
            {
            Dprint( (_L("CSCPLockSettingEventHandler::SetAutolockStateL():\
               Autolock task found, sending msg") ));        
            // No parameters, just a dummy buffer
            TBuf8<8> buf;
            buf.Zero();            
            const TPtrC8& message = buf;                      
            User::LeaveIfError( 
               task.SendMessage( TUid::Uid( SCP_CMDUID_UNLOCK ), message ) 
               );           
            }
        else
            {
            Dprint( (_L("CSCPLockSettingEventHandler::SetAutolockStateL():\
               Autolock task not found") ));
            User::Leave( KErrNotFound );
            }
                  
        CleanupStack::PopAndDestroy(); // wsSession	                
        }    
    }


// ---------------------------------------------------------
// void CSCPLockSettingEventHandler::RunL()
// Checks the status, and activates/deactivates autolock on success.
// 
// Status : Approved
// ---------------------------------------------------------
//
void CSCPLockSettingEventHandler::RunL()
    {
    TInt ret = iStatus.Int();
    TBool finalCall = ETrue;
    TBool startFinalPhase = EFalse;
    
    Dprint( (_L("--> CSCPLockSettingEventHandler::RunL()") ));
     
    if ( iUnlockSent ) // Received an acknowledgement for the unlock-message
        {
        if ( ret == KErrNone ) 
            {
            Dprint( (_L("CSCPLockSettingEventHandler::RunL(): Unlock ack received") ));
            }
        else if ( ret == KErrCancel ) 
            {
            Dprint( (_L("CSCPLockSettingEventHandler::RunL(): Unlock call TIMEOUT") ));
            iMessageStatus = ret;
            }
        }    
    else // Received an acknowledgement for the set lock setting call
        {
        if ( ret == KErrNone ) 
            {
            Dprint( (_L("CSCPLockSettingEventHandler::RunL():\
                Acknowledgement received") ));            
            startFinalPhase = ETrue;
            }
        else if ( ret == KErrCancel ) 
            {
            Dprint( (_L("CSCPLockSettingEventHandler::RunL():\
                Lock setting call TIMEOUT") ));
            iMessageStatus = ret;
            }            
        }
	   
    if ( ( startFinalPhase ) && ( iAutolock ) )
        {
        // Autolock activation/deactivation was requested
        TRAPD( err, SetAutolockStateL( iState ) );
    
        if ( err != KErrNone )
            {
            // If the call failed, send the result as the message status
            iMessageStatus = err;
            }
        else if ( !iState )
            {
            // Inform the session-class that an unlock-message has been sent
            // (the server won't respond to Autolock until the handler finishes)                    
            iSession->LockOperationPending( ESCPCommandUnlockPhone, &iStatus );
            iUnlockSent = ETrue;	                
            finalCall = EFalse;
            SetActive(); // Wait for the session-class to complete this call
            }
        }
	   
    if ( finalCall ) 
        {
        Dprint( (_L("CSCPLockSettingEventHandler::RunL():\
            Completing set lock state call, status: %d"), ret ));         			
        
        iLockMessage.Complete( iMessageStatus ); 	    
        // Inform the SessionBase class that this operation is complete
        iSession->LockOperationCompleted();
        }          
   
    Dprint( (_L("<-- CSCPLockSettingEventHandler::RunL()") ));
    }
    
// ---------------------------------------------------------
// void CSCPLockSettingEventHandler::DoCancel()
// Cancel function, aborts the TSY request
// 
// Status : Approved
// ---------------------------------------------------------
//
void CSCPLockSettingEventHandler::DoCancel()
    {
    Dprint( (_L("--> CSCPLockSettingEventHandler::DoCancel()") ));

    iPhone->CancelAsyncRequest( EMobilePhoneSetLockSetting );

    iLockMessage.Complete( KErrCancel );    
    iSession->LockOperationCompleted();  

    Dprint( (_L("<-- CSCPLockSettingEventHandler::DoCancel()") ));
    }
   
   
// ================= OTHER EXPORTED FUNCTIONS ==============

//  End of File  

