/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Synchronisation server alert notifier.
*
*/



#ifndef CSYNCMLDLGNOTIFIER_H
#define CSYNCMLDLGNOTIFIER_H

//  INCLUDES
#include <SyncMLNotifierParams.h>
#include "SyncMLNotifierBase.h"    // Base class
#include <aknselectionlist.h> 
#include <aknlists.h>
#include <AknIconArray.h> 
// FORWARD DECLARATIONS
class CSyncMLAppLaunchNotifier;

// CLASS DECLARATION

/**
*  Notifier for information received from synchronisation server.
*
*  @lib SyncMLNotifier
*  @since Series 60 3.0
*/
NONSHARABLE_CLASS( CSyncMLDlgNotifier ) : public CSyncMLNotifierBase
    {
    public:  // Constructors and destructor
        
        /**
        * Two-phased constructor.
        */
        static CSyncMLDlgNotifier* NewL(
            CSyncMLAppLaunchNotifier* aAppLaunchNotif );
        
        /**
        * Destructor.
        */
        virtual ~CSyncMLDlgNotifier();

    protected:  // New functions
        
        /**
        * Tries to enable or disable the sync progress notes.
        * @since Series 60 3.0
        * @param aEnable Should the progress notes be enabled.
        * @return None
        */
        void EnableSyncProgressNoteL( TBool aEnable );

    private: // Functions from base classes

        /**
        * From CSyncMLNotifierBase Called when a notifier is first loaded.        
        * @param None.
        * @return A structure containing priority and channel info.
        */
        TNotifierInfo   RegisterL();

        /**
        * From CSyncMLNotifierBase Used in asynchronous notifier launch to 
        * store received parameters into members variables and 
        * make needed initializations.
        * @param aBuffer A buffer containing received parameters
        * @param aReturnVal The return value to be passed back.
        * @param aMessage Should be completed when the notifier is deactivated.
        * @return None.
        */
        void GetParamsL( const TDesC8&       aBuffer,
                               TInt          aReplySlot,
                         const RMessagePtr2& aMessage );

        /**
        * From CSyncMLNotifierBase The notifier has been deactivated 
        * so resources can be freed and outstanding messages completed.
        * @param None.
        * @return None.
        */
        void Cancel();

        /**
        * From CSyncMLNotifierBase Gets called when a request completes.
        * @param None.
        * @return None.
        */
        void RunL();

    private:

        /**
        * C++ default constructor.
        */
        CSyncMLDlgNotifier( CSyncMLAppLaunchNotifier* aAppLaunchNotif );
        /**
        * Action for the Alphanumeric input.
        * @param result.
        * @return None.
        */
				void InputTypeAlphaNumericL(TInt & result);
				/**
        * Action for the Numeric input.
        * @param result.
        * @return None.
        */
				void InputTypeNumericL(TInt & result);
				/**
        * Action for the Date input.
        * @param result.
        * @return None.
        */
				void InputTypeDateL(TInt & result);
				/**
        * Action for the Time input.
        * @param result.
        * @return None.
        */
				void InputTypeTimeL(TInt & result);
				/**
        * Action for the Phone number input.
        * @param result.
        * @return None.
        */
				void InputTypePhoneNumberL(TInt & result);
				
        /**
         * Set icons on markable list
         * @param aListBox, pointer to listbox.
         * @return None.
         */
        void SetIconsL(CEikFormattedCellListBox* aListBox);
		
    private: // Data
        // The type of the note requested.
        TSyncMLDlgNoteTypes             iNoteType;
        
        // Server message from the parameters
        TBuf<KSyncMLMaxServerMsgLength> iServerMsg;
        
        // Maximum time the query is shown. (In seconds)
        TInt                            iMaxTime;
        //Maximum length of the server message
        TInt iMaxlen;
		//Default response of the alert
        TBuf<KSyncMLMaxDefaultResponseMsgLength> iDR;
        //Input type of user input alert
        TInt iIT;
		//Echo type of user input alert
        TInt iET;
        //Number of choice items in user choice alerts
        TInt iNumberOfItems;
        // Pointer to the application launcher notifier instance.
        CSyncMLAppLaunchNotifier*       iAppLaunchNotif;
		//array of user choice alert items
        CDesCArrayFlat* iListItems;
		//Default selection item in user choice alerts
        TInt iDefindex;
		//reply buffer
        TSyncMLDlgNotifReturnParams reply;
		//Member variable to identify end key press
        TBool EndKeyPress;
		//Chunk to retrieve the user choice alert items
        RChunk iChunk;
    };


#endif      // CSYNCMLDLGNOTIFIER_H   
            
// End of File
