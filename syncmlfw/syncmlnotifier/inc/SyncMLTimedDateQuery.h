/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Date query with timeout.
*
*/



#ifndef CSYNCMLTIMEDDATEQUERY_H
#define CSYNCMLTIMEDDATEQUERY_H

//  INCLUDES
#include <AknQueryDialog.h>

#include "SyncMLQueryTimer.h"

// CONSTANTS
const TInt  KSyncMLNNoTimeoutDate  = 0; // The query does not time out

// CLASS DECLARATION

/**
*  Date query with timeout.
*
*  @lib SyncMLNotifier
*  @since Series 60 3.2
*/
NONSHARABLE_CLASS ( CSyncMLTimedDateQuery ) : public CAknTimeQueryDialog,
                                                 public MSyncMLQueryTimerObserver
    {
    public:  // Constructors and destructor
        
        /**
        * Two-phased constructor.
        */
        static CSyncMLTimedDateQuery* NewL(
            TTime& aDate,const TDesC& aPromptText, TBool& aKeypress,
                  TInt   aTimeout = KSyncMLNNoTimeoutDate );
          
        CSyncMLTimedDateQuery(TTime& aDate, TBool& aKeypress,TInt aTimeout = KSyncMLNNoTimeoutDate );
        /**
        * Destructor.
        */
        virtual ~CSyncMLTimedDateQuery();

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
        
        /** 
        * Called by OfferkeyEventL(), gives a change to dismiss the query even with 
        * keys different than Enter of Ok. 
        */ 
        TBool NeedToDismissQueryL(const TKeyEvent& aKeyEvent);
        
        /**
        * Called by NeedToDismissQueryL(), gives a change to either accept or reject
        * the query. Default implementation is to accept the query if the Left soft
        * key is displayed and reject it otherwise. Left softkey is only displayed if
        * the query has valid data into it.
        */
        void DismissQueryL();
        
    private:    // Data
        // Number of seconds before the query is timed out. 0 = no timeout.
        TInt               iTimeout;

        // Pointer to the timer instance
        CSyncMLQueryTimer*  iTimer;
    public:    
        //key press detection
        TBool iKeypress;
    };

#endif      // CSYNCMLTIMEDMESSAGEQUERY_H   
            
// End of File
