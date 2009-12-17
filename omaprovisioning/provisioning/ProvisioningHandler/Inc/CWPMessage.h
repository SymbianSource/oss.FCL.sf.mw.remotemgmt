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
* Description: 
*     Main class for handling provisioning message
*
*/


#ifndef CWPMESSAGE_H
#define CWPMESSAGE_H

// INCLUDES
#include <e32base.h>
#include <msvapi.h>

// FORWARD DECLARATIONS
class CWPPushMessage;
class MWPPhone;
class CPushMessage;
class CDesC16Array;

// CLASS DECLARATION

/**
 * CWPMessage handles a provisioning message.
 * @library ProvisioningHandler.dll
 */ 
class CWPMessage : public CBase,
    private MMsvSessionObserver
    {
    private:
        enum TAuthResult { EFailed, EMACSucceeded, EInitiatorSucceeded };

    public:  // Constructors and destructor

        /**
        * Two-phased constructor.
        * @param aMessage The push message to use as base
        * @return The created CWPMessage object
        */
        static CWPMessage* NewL( CPushMessage& aMessage );

        /**
        * Destructor.
        */
        virtual ~CWPMessage();

    public:
        /**
        * Process the message.
        */
        void ProcessL();

    private: // From MMsvSessionObserver

        void HandleSessionEventL(TMsvSessionEvent aEvent, TAny* aArg1, TAny* aArg2, TAny* aArg3);

    private:
        /**
        * C++ default constructor.
        */
        CWPMessage();

        /**
        * By default Symbian 2nd phase constructor is private.
        * @param aMessage The push message to use as base
        */
        void ConstructL( CPushMessage& aMessage );

    private:

        /**
        * Retrieves the UID of the BIO message type.
        * @param aMessage The message whose content type to look
        */
        void BioMessageTypeL( CPushMessage& aMessage );

        /**
        * Stores the message into Messaging Server.
        */
        void StoreMsgL();

        /**
        * Stores an SMS message into Messaging Server.
        * @param aResource The text to use
        */
        void StoreMsgL( TInt aResource );

        /**
        * Stores an SMS message into Messaging Server based on Class0.
        * @param aResource The text to use
        */
        void StoreMsgclass0L( TInt aResource );
        
        /**
        * Prepares a TMsvEntry for saving. Leaves a string
        * to CleanupStack.
        * @param aEntry Entry to prepare
        */
        void PrepareEntryLC( TMsvEntry& aEntry );

        /**
        * Completes processing a message entry.
        * @param aCEntry The CMsvEntry to change
        * @param aTEntry The corresponding TMsvEntry
        */
        void PostprocessEntryL( CMsvEntry& aCEntry, TMsvEntry& aTEntry );

        /**
        * Loads a string from resource file.
        * @param aResource The resource ID
        */
        HBufC* LoadStringLC( TInt aResourceId );

    private:  // Data
        /// ETel interface
        MWPPhone* iPhone;

        /// Session to Messaging Server. Owns.
        CMsvSession* iSession;

        /// Sender address. Owns.
        HBufC8* iSender;

        /// ETrue if the message contains bootstrap characteristic
        TBool iBootstrap;

        /// Push message header and contents. Refs.
        CWPPushMessage* iMessage;

        /// BIO uid
        TUid iBioUID;
    };

#endif  // CWPMESSAGE_H
            
// End of File
