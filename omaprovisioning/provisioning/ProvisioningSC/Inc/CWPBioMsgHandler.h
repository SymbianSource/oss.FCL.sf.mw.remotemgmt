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
* Description:  Stores provisioning messages into inbox
*
*/



#ifndef CWPBIOMSGHANDLER_H
#define CWPBIOMSGHANDLER_H

//  INCLUDES
#include	<e32base.h>
#include	<msvapi.h>

// FORWARD DECLARATIONS
class CClientMtmRegistry;
class CPushMessage;
class CWPPushMessage;

/**
*  CWPBioMsgHandler manages the storage of the imsi numbers
*  of processed SIM cards.
*  @since 2.5
*/
class CWPBioMsgHandler : public CBase, public MMsvSessionObserver
    {
    public:  // Constructors and destructor
        
        /**
        * Two-phased constructor.
        */
        static CWPBioMsgHandler* NewL();

		/**
        * Two-phased constructor.
        */
        static CWPBioMsgHandler* NewLC();
        
        /**
        * Destructor.
        */
        virtual ~CWPBioMsgHandler();

	public: // from MMsvSessionObserver
		void HandleSessionEventL( TMsvSessionEvent aEvent, TAny* aArg1,
								TAny* aArg2, TAny* aArg3);

    public: // New functions


		/**
        * Creates an intermediary message for storing settings
		* @param provisioning document containing message body
		* @return the message
		* @since 2.6
        */
		CWPPushMessage* CreateMessageLC( TDesC8& aProvisioningDoc );
        
		/**
        * Stores SIM smart card provisioned settings
		* into Inbox
        * @param the intermediary message to be stored
		* @since 2.6
        */
		void StoreMessageL( CWPPushMessage& aMessage );

		/**
        * Stores an information note into Inbox
        * @param a resource id
		* @param the intermediary message
		* @since 2.6
        */
		void StoreMessageL( TInt aResource, CWPPushMessage& aMessage );


    private:

        /**
        * C++ default constructor.
        */
        CWPBioMsgHandler();

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();

        /**
        * Creates CPushmessage
		* @param provisioning wbxml document for message body
		* @return message with a new header and given body
        */
		CPushMessage* CreatePushMessageL( TDesC8& aProvisioningDoc );


		/**
        * Sets entry parameters
		* @param entry to be set
        */
		void PrepareEntryL( TMsvEntry& aEntry );

		/**
        * Stores a given entry into inbox
		* @param push message to be stored
		* @param entry to be stored
        */
		void StoreEntryL( CWPPushMessage& aPushMsg, TMsvEntry& aEntry );

		/**
        * Sets the biouid member from message's bio type.
        * @param aMessage The message whose content type to look
        */
        void BioMessageTypeL( CPushMessage& aMessage );

    private:    // Data

	// client session on the message server
    CMsvSession*	iSession; // owned
	// Message Type Module
    CBaseMtm*		iMtm; // owned
	// Mtm client registry for creating new mtms  
    CClientMtmRegistry* iMtmRegistry; // owned
	/// Push message header and contents. 
	CWPPushMessage* iMessage; // owned
	TUid iBioUid;
    };

#endif      // CWPBioMsgHandler_H   
// End of File
