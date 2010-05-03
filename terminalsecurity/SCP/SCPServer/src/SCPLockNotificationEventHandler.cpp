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


 
#include <mmtsy_names.h>

#include "SCPLockNotificationEventHandler.h"
#include "SCPServer.h"
#include "SCPSession.h"

// ================= MEMBER FUNCTIONS =======================

// C++ default constructor can NOT contain any code, that
// might leave.
//
CSCPLockNotificationEventHandler::CSCPLockNotificationEventHandler(                        
            CSCPServer* aServer,
            CSCPSession* aSession
            )
	        : CSCPLockEventHandler( aServer ),
	        iQueryState(ESCPLNQueryStateNotification),
	        iAckReceived (EFalse),
			iSession (aSession)
    {
    Dprint( (_L("--> CSCPLockNotificationEventHandler::\
	      CSCPLockNotificationEventHandler()") ));
	
    Dprint( (_L("<-- CSCPLockNotificationEventHandler::\
	     CSCPLockNotificationEventHandler()") ));
    }


// Symbian 2nd phase constructor can leave.
void CSCPLockNotificationEventHandler::ConstructL()
    {
    Dprint( (_L("--> CSCPLockNotificationEventHandler::ConstructL()") ));
   
    BaseConstructL();   
   
    RegisterListener();   

    Dprint( (_L("<-- CSCPLockNotificationEventHandler::ConstructL()") ));
    }


// Static constructor.
CSCPLockNotificationEventHandler* CSCPLockNotificationEventHandler::NewL(
        CSCPServer* aServer,
        CSCPSession* aSession
        )
    {
	  Dprint( (_L("--> CSCPLockNotificationEventHandler::NewL()") ));

	  CSCPLockNotificationEventHandler* self = 
	      CSCPLockNotificationEventHandler::NewLC( aServer, aSession );
	        
    CleanupStack::Pop( self );
	
    Dprint( (_L("<-- CSCPLockNotificationEventHandler::NewL()") ));
    return self;	
    }
	

// Static constructor, leaves object pointer to the cleanup stack.
CSCPLockNotificationEventHandler* CSCPLockNotificationEventHandler::NewLC(
        CSCPServer* aServer,
        CSCPSession* aSession
        )
    {
    Dprint( (_L("--> CSCPLockNotificationEventHandler::NewLC()") ));

    CSCPLockNotificationEventHandler* self = 
        new (ELeave) CSCPLockNotificationEventHandler( aServer, aSession );

    CleanupStack::PushL( self );
    self->ConstructL();	
	
    Dprint( (_L("<-- CSCPLockNotificationEventHandler::NewLC()") ));
    return self;	
    }

    
// Destructor
CSCPLockNotificationEventHandler::~CSCPLockNotificationEventHandler()
    {
    Dprint( (_L("--> CSCPLockNotificationEventHandler::\
      ~CSCPLockNotificationEventHandler()") ));
      
    if ( IsActive() )
        {
        Dprint( (_L("CSCPLockNotificationEventHandler::\
            ~CSCPLockNotificationEventHandler(): Cancelling call") ));
        Cancel();
        }
    
    Dprint( (_L("<-- CSCPLockNotificationEventHandler::\
	    ~CSCPLockNotificationEventHandler()") ));
    }



// ---------------------------------------------------------
// TInt CSCPLockNotificationEventHandler::RegisterListener()
// Registers this object for receiving security events via
// NotifyOnSecurityEvent.
// 
// Status : Approved
// ---------------------------------------------------------
//
TInt CSCPLockNotificationEventHandler::RegisterListener()
    {
    TInt ret;
   
    // Register for notifications for security events
    Dprint( (_L("CSCPLockNotificationEventHandler::\
    RegisterListener(): Running NotifySecurityEvent") ));
	
    iPhone->NotifySecurityEvent( iStatus, iEvent );
	    
    iQueryState = ESCPLNQueryStateNotification;
     	
    Dprint( (_L("CSCPLockNotificationEventHandler::\
          RegisterListener(): Running SetActive") ));		   
    SetActive();      
    ret = KErrNone;
       
    return ret;
    }

// ---------------------------------------------------------
// void CSCPLockNotificationEventHandler::AckReceived()
// Sets the ackReceived member to indicate that the call has
// already been acknowledged.
// 
// Status : Approved
// ---------------------------------------------------------

void CSCPLockNotificationEventHandler::AckReceived()
    {
    Dprint( (_L("--> CSCPLockNotificationEventHandler::AckReceived()") ));
    iAckReceived = ETrue;    
    }

// ---------------------------------------------------------
// void CSCPLockNotificationEventHandler::VerifyPass()
// Verify password to the ISA
// 
// Status : Approved
// ---------------------------------------------------------
void CSCPLockNotificationEventHandler::VerifyPass()
    {
    Dprint( (_L("CSCPLockNotificationEventHandler::VerifyPass():\
        EPhonePasswordRequired event received") ));

    RMobilePhone::TMobilePhoneSecurityCode secCodeType = 
        RMobilePhone::ESecurityCodePhonePassword;
        
    RMobilePhone::TMobilePassword password;
    RMobilePhone::TMobilePassword required_fourth;
    required_fourth.Zero();
    iServer->GetCode( password );                

    iPhone->VerifySecurityCode(iStatus, secCodeType, 
        password, required_fourth);

    // Start waiting for verification response
    iQueryState = ESCPLNQueryStateVerification;
    SetActive(); 
    Dprint( (_L("<-- CSCPLockNotificationEventHandler::VerifyPass()") ));    
    }

// ---------------------------------------------------------
// void CSCPLockNotificationEventHandler::RunL()
// When the correct security event is received, the code query is
// verified via VerifySecurityCode. This method also handles the
// response for this call.
// 
// Status : Approved
// ---------------------------------------------------------
//
void CSCPLockNotificationEventHandler::RunL()
    {
    Dprint( (_L("--> CSCPLockNotificationEventHandler::RunL()") ));
   
    switch ( iQueryState )
        {
        case ( ESCPLNQueryStateNotification ):
            {                        
            // Event received from Session, queryAdminCmd
             if (iAckReceived)
                 {
                 VerifyPass();
                 }
             else if ( iEvent == RMobilePhone::EPhonePasswordRequired )
                 {
                 Dprint( (_L("--> CSCPLockNotificationEventHandler::EPhonePasswordRequired") ));
                 iQueryState = ESCPLNQueryStateQueryAdmin;
                 iSession->LockOperationPending( ESCPCommandLockPhone, &iStatus );
                 SetActive();
                 }
             else
                 {
                 Dprint( (_L("CSCPLockNotificationEventHandler::RunL():\
                     Invalid event received") ));
                 // Re-run registration, this event is not the one we're waiting for
                 RegisterListener();
                 }
             break;
            }
        // Event received from Session for LockOpPending, queryAdminCmd
        case (ESCPLNQueryStateQueryAdmin):
            {             
            Dprint( (_L("--> CSCPLockNotificationEventHandler::ESCPLNQueryStateQueryAdmin") ));
            VerifyPass();
            break;
            }

        case ( ESCPLNQueryStateVerification ):
            {                        
            // Verification response
            if ( iStatus.Int() == KErrNone )
                {
                Dprint( (_L("CSCPLockNotificationEventHandler::RunL():\
                    VerifySecurityCode OK") ));
                }
            else
                {
                Dprint( (_L("CSCPLockNotificationEventHandler::RunL():\
                    VerifySecurityCode ERROR %d"), iStatus.Int() ));
                }
            break;            
            }                               
        }
   
   
    Dprint( (_L("<-- CSCPLockNotificationEventHandler::RunL()") ));
    }

// ---------------------------------------------------------
// void CSCPLockNotificationEventHandler::DoCancel()
// Cancels the currently ongoing request.
// 
// Status : Approved
// ---------------------------------------------------------
//
void CSCPLockNotificationEventHandler::DoCancel()
    {
    Dprint( (_L("--> CSCPLockNotificationEventHandler::DoCancel()") ));
   
    switch ( iQueryState )
        {
        case ( ESCPLNQueryStateNotification ):
            iPhone->CancelAsyncRequest( EMobilePhoneNotifySecurityEvent );
        break;

        case ( ESCPLNQueryStateVerification ):
            iPhone->CancelAsyncRequest( EMobilePhoneVerifySecurityCode );
        break;
        }
        
    Dprint( (_L("<-- CSCPLockNotificationEventHandler::DoCancel()") ));
    }
   

// ================= OTHER EXPORTED FUNCTIONS ==============

//  End of File  

