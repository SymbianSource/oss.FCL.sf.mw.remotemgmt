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

#ifndef SCPLOCKNOTIFICATIONEVENTHANDLER_H
#define SCPLOCKNOTIFICATIONEVENTHANDLER_H

//  INCLUDES
#include <bldvariant.hrh>
#include "SCPLockEventHandler.h"

// FORWARD DECLARATIONS
class CSCPSession;
// CLASS DECLARATION

/**
*  A class, which handles the notification and verification event in phone locking
*/
class CSCPLockNotificationEventHandler : public CSCPLockEventHandler
	{		
	public:  // Methods

        // Constructors and destructor                

        /**
        * Static constructor.
        */
   	    static CSCPLockNotificationEventHandler* NewL( CSCPServer* aServer, CSCPSession* aSession );
        
        /**
        * Static constructor, that leaves the pointer to the cleanup stack.
        */
        static CSCPLockNotificationEventHandler* NewLC( CSCPServer* aServer, CSCPSession* aSession );

        /**
        * Destructor.
        */
        virtual ~CSCPLockNotificationEventHandler();
       
		// Ack from session after queryadmincmd
		void AckReceived();
    protected:  // Methods   
        
        // Methods from base classes
        
        /**
        * Registration method for reveiving notifications        
        * @return TInt: A generic status code
        */
        TInt RegisterListener();
        
        /**
        * From CActive The active object working method.
        */
        void RunL();
        
        /**
        * From CActive The active object request cancellation method.
        */
        void DoCancel();

        // send passcode to ISA
        void VerifyPass();

    private: //Methods
    
        /**
        * C++ default constructor.
        */
        CSCPLockNotificationEventHandler( CSCPServer* aServer, CSCPSession* aSession );

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();          

    private:   // Data    
        
        // Possible processing states
        enum TSCPLNQueryState
            {
            ESCPLNQueryStateNotification,
            ESCPLNQueryStateVerification,
            ESCPLNQueryStateQueryAdmin
            };
                        
        /** The current processing state. */
        TSCPLNQueryState                        iQueryState;                 
        /** A placeholder for the event received from the TSY */
        RMobilePhone::TMobilePhoneSecurityEvent iEvent;        
        // Ack from queryadmin
        TBool iAckReceived;
        /** The parent session pointer */
        CSCPSession* iSession;
    };

#endif      // SCPLOCKNOTIFICATIONEVENTHANDLER_H   
            
// End of File

