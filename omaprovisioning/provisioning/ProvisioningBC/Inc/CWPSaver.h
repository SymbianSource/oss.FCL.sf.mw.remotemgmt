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
* Description:  Settings saver with progress note.
*
*/


#ifndef CWPSAVER_H
#define CWPSAVER_H

// INCLUDES
#include <AknProgressDialog.h>
#include <ActiveApDb.h>
#include <FavouritesDbObserver.h>
#include <FavouritesDb.h>


// FORWARD DECLARATIONS
class CWPEngine;
class CActiveFavouritesDbNotifier;

// CLASS DECLARATION

/**
 * Helper class for saving Provisioning settings. Provides a progress note.
 * @since 2.0
 */
class CWPSaver : public CActive, 
                 private MProgressDialogCallback,
                 private MActiveApDbObserver, 
                 private MFavouritesDbObserver
    {
    public:
        /**
        * C++ default constructor.
        * @param aEngine Engine to be used for saving
        * @param aSetAsDefault EFalse => call CWPEngine::SaveL, ETrue
        *        => call CWPEngine::SetAsDefaultL
        */
        CWPSaver( CWPEngine& aEngine, TBool aSetAsDefault );

        /**
        * Destructor.
        */
        virtual ~CWPSaver();

    public:
        /**
        * Prepares the object for saving.
        */
        void PrepareLC();

        /**
        * Executes save with a progress note. Ownership of the
        * CWPSaver object is transferred.
        * @param aNumSaved When returns, contains number of settings saved.
        * @return Status code. >= 0 if saving was completed
        */
        TInt ExecuteLD( TInt& aNumSaved );

        TInt GetNumAccessDenied();
        void SetNumAccessDenied(TInt aNumAccessDenied );
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
        * a delay. Leaves with KErrTimeout if retry count is 
        * exceeded.
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
        /// The engine used for performing the save. Refs.
        CWPEngine& iEngine;

        /// ETrue if setting as default
        TBool iSetAsDefault;

        /// The progress dialog. Owns.
        CAknProgressDialog* iDialog;

        /// The item to be saved next
        TInt iCurrentItem;

        /// Contains result to be passed to the called of ExecuteLD
        TInt iResult;

        /// Active AP database. Owns.
        CActiveApDb* iApDbNotifier;

        /// Active Favourites Database. Owns.
        CActiveFavouritesDbNotifier* iFavouritesNotifier;

        /// Contains ETrue if commsdb is being waited on
        TBool iWaitCommsDb;

        /// Contains ETrue if favourites db is being waited on
        TBool iWaitFavourites;

        /// Active scheduler.
        CActiveSchedulerWait iWait;

        /// Timer for retry. Owns.
        CPeriodic* iRetryTimer;

        /// Retry counter
        TInt iRetryCount;
        
        RFavouritesDb       iBookmarkDb;
        RFavouritesSession  iSession;       
   	TInt iNumAccessDenied;
    };


#endif // CWPSAVER_H

// End of File
