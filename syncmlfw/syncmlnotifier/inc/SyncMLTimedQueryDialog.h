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
* Description:  Query dialog with timeout.
*
*/



#ifndef CSYNCMLTIMEDQUERYDIALOG_H
#define CSYNCMLTIMEDQUERYDIALOG_H

//  INCLUDES
#include <AknQueryDialog.h>

#include "SyncMLQueryTimer.h"
#include "SyncMLTimedMessageQuery.h"

// CONSTANTS
const TInt KSyncMLNotifierTimeout = 300; // 5 min timeout

// CLASS DECLARATION

/**
*  Message query with timeout.
*
*  @lib SyncMLNotifier
*  @since Series 60 3.0
*/
NONSHARABLE_CLASS ( CSyncMLTimedQueryDialog ) : public CAknQueryDialog,
                                                 public MSyncMLQueryTimerObserver
    {
    public:  // Constructors and destructor
        
        /**
        * Two-phased constructor.
        */
        static CSyncMLTimedQueryDialog* NewL(
            const TDesC& aMessage,
                  TInt   aTimeout = KSyncMLNNoTimeout );
        
        /**
        * Destructor.
        */
        virtual ~CSyncMLTimedQueryDialog();

    public: // Functions from base classes

        /**
        * From MSyncMLQueryTimerObserver Is called when the timer
        * runs out.
        * @since Series 60 3.0
        * @param None
        * @return None
        */
        void TimerExpired();
        
    protected:  // Functions from base classes
        
        /**
        * From CEikDialog Starts the dialog timer.
        * Called after the dialog is laid out.
        * @since Series 60 3.0
        * @param None
        * @return None
        */
        void PostLayoutDynInitL();

        /**
        * From CEikDialog Determines if the dialog can be closed.
        * @since Series 60 3.0
        * @param aButtonId Identifier of the button pressed.
        * @return None
        */
        TBool OkToExitL( TInt aButtonId );
        
    private:

        /**
        * C++ default constructor.
        */
        CSyncMLTimedQueryDialog( TInt aTimeout = KSyncMLNNoTimeout );

    private:    // Data
        // Number of seconds before the query is timed out. 0 = no timeout.
        TInt               iTimeout;

        // Pointer to the timer instance
        CSyncMLQueryTimer*  iTimer;
    };

#endif      // CSYNCMLTIMEDQUERYDIALOG_H   
            
// End of File
