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
* Description:  Number Query dialog with timeout.
*
*/



#ifndef CSYNCMLTIMEDNUMBERQUERYDIALOG_H
#define CSYNCMLTIMEDNUMBERQUERYDIALOG_H

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
*  @since Series 60 3.2
*/
NONSHARABLE_CLASS ( CSyncMLTimedNumberQueryDialog ) : public CAknFloatingPointQueryDialog,
                                                 public MSyncMLQueryTimerObserver
    {
    public:  // Constructors and destructor
        
        /**
        * Two-phased constructor.
        */
        static CSyncMLTimedNumberQueryDialog* NewL(
            TReal& aValue, const TDesC& aIptext,TBool& aKeypress, 
                  TInt   aTimeout = KSyncMLNNoTimeout );
        
        /**
        * Destructor.
        */
        virtual ~CSyncMLTimedNumberQueryDialog();
       

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
        
    private:

        /**
        * C++ default constructor.
        */
        CSyncMLTimedNumberQueryDialog( TReal& aValue, TBool& aKeypress, TInt aTimeout = KSyncMLNNoTimeout );

    private:    // Data
        // Number of seconds before the query is timed out. 0 = no timeout.
        TInt               iTimeout;

        // Pointer to the timer instance
        CSyncMLQueryTimer*  iTimer;
     public:   
        //Detecting the key press
         TBool iKeypress;
    };

#endif      // CSYNCMLTIMEDQUERYDIALOG_H   
            
// End of File
