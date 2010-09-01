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
* Description:  Message query with timeout.
*
*/



#ifndef CSYNCMLTIMEDMESSAGEQUERY_H
#define CSYNCMLTIMEDMESSAGEQUERY_H

//  INCLUDES
#include <aknmessagequerydialog.h>

#include "SyncMLQueryTimer.h"

// CONSTANTS
const TInt  KSyncMLNNoTimeout  = 0; // The query does not time out

// CLASS DECLARATION

/**
*  Message query with timeout.
*
*  @lib SyncMLNotifier
*  @since Series 60 3.0
*/
NONSHARABLE_CLASS ( CSyncMLTimedMessageQuery ) : public CAknMessageQueryDialog,
                                                 public MSyncMLQueryTimerObserver
    {
    public:  // Constructors and destructor
        
        /**
        * Two-phased constructor.
        */
        static CSyncMLTimedMessageQuery* NewL(
            const TDesC& aMessage,
                  TInt   aTimeout = KSyncMLNNoTimeout );
        
        /**
        * Destructor.
        */
        virtual ~CSyncMLTimedMessageQuery();

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
        CSyncMLTimedMessageQuery( TInt aTimeout = KSyncMLNNoTimeout );

    private:    // Data
        // Number of seconds before the query is timed out. 0 = no timeout.
        TInt               iTimeout;

        // Pointer to the timer instance
        CSyncMLQueryTimer*  iTimer;
    };

#endif      // CSYNCMLTIMEDMESSAGEQUERY_H   
            
// End of File
