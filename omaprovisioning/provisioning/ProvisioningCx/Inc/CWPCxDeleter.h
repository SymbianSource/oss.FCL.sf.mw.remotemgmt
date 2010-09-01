/*
* Copyright (c) 2002-2006 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Settings deleter with progress note.
*
*/


#ifndef CWPCXDELETER_H
#define CWPCXDELETER_H

// INCLUDES
#include <aknprogressdialog.h>
#include <activeapdb.h>
#include <favouritesdbobserver.h>
#include <FavouritesDb.h>

// FORWARD DECLARATIONS
class CWPEngine;
class CActiveFavouritesDbNotifier;

// CLASS DECLARATION

/**
 * Helper class for deleting Provisioning settings.
 */
class CWPCxDeleter : public CActive, 
                     private MProgressDialogCallback, 
                     private MActiveApDbObserver, 
                     private MFavouritesDbObserver
    {
    public:
        /**
        * C++ default constructor.
        * @param aEngine Engine to be used for saving
        */
        CWPCxDeleter( CWPEngine& aEngine, TUint32 aContext );

        /**
        * Destructor.
        */
        ~CWPCxDeleter();

    public:
        /**
        * Prepares the object for saving.
        */
        void PrepareLC();

        /**
        * Executes save with a progress note. Ownership of the
        * CWPSaver object is transferred.
        * @param aNumDeleted When returns, contains number of settings saved.
        * @return Status code. >= 0 if saving was completed
        */
        TInt ExecuteLD( TInt& aNumDeleted );

    protected: // From CActive

        void DoCancel();
        void RunL();
        TInt RunError( TInt aError );

    private: // From MProgressDialogCallback

        void DialogDismissedL( TInt aButtonId );

    private: // from MActiveApDbObserver

        void HandleApDbEventL( TEvent anEvent );

    private: // from MFavouritesDbObserver

        void HandleFavouritesDbEventL( RDbNotifier::TEvent aEvent );

    private:
        /**
        * Complete the request so that RunL() gets called.
        */
        void CompleteRequest();

        /**
        * Complete the request so that Timeout() gets called after
        * a delay.
        */
        void DelayedCompleteRequestL();

        /**
        * Retry save now.
        */
        void Retry();

        /**
        * Timer timed-out.
        */
        static TInt Timeout(TAny* aSelf);

    private:
        // The engine used for performing the save. Refs.
        CWPEngine& iEngine;

        // The context being deleted
        TUint32 iContext;

        // The progress dialog. Owns.
        CAknProgressDialog* iDialog;

        // The item to be saved next
        TInt iCurrentItem;

        // Contains result to be passed to the called of ExecuteLD
        TInt iResult;

        // Active AP database
        CActiveApDb* iApDbNotifier;

        // Active Favourites Database
        CActiveFavouritesDbNotifier* iFavouritesNotifier;

        // Contains ETrue if commsdb is being waited on
        TBool iWaitCommsDb;

        // Contains ETrue if favourites db is being waited on
        TBool iWaitFavourites;

        // Active scheduler.
        CActiveSchedulerWait iWait;

        // Timer for retry
        CPeriodic* iRetryTimer;

        // Retry count
        TInt iRetryCount;
        
        RFavouritesDb       iBookmarkDb;
        RFavouritesSession  iSession;
    };


#endif // CWPCXDELETER_H

// End of File
