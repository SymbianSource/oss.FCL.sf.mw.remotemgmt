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



#ifndef CSYNCMLQUERYTIMER_H
#define CSYNCMLQUERYTIMER_H

//  INCLUDES
#include <e32base.h>

// CLASS DECLARATION

/**
*  Timer observer class for SyncML notifier queries.
*
*  @lib SyncMLNotifier
*  @since Series 60 3.0
*/
NONSHARABLE_CLASS ( MSyncMLQueryTimerObserver )
    {
    public: // New functions
        /**
        * Is called when the timer expires.
        * @since Series 60 3.0
        * @param None
        * @return None
        */
        virtual void TimerExpired() = 0;
    };

/**
*  Timer class for SyncML notifier queries.
*
*  @lib SyncMLNotifier
*  @since Series 60 3.0
*/
NONSHARABLE_CLASS ( CSyncMLQueryTimer ) : public CTimer
    {
    public:  // Constructors and destructor
        
        /**
        * Two-phased constructor.
        */
        static CSyncMLQueryTimer* NewL( MSyncMLQueryTimerObserver* aObserver );
        
        /**
        * Destructor.
        */
        virtual ~CSyncMLQueryTimer();

    public: // Functions from base classes

        /**
        * From CActive Is called when an event is received.
        * @since Series 60 3.0
        * @param None
        * @return None
        */
        void RunL();
        
    private:

        /**
        * C++ default constructor.
        */
        CSyncMLQueryTimer( MSyncMLQueryTimerObserver* aObserver );

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();

    private:    // Data
        // Pointer to the timer observer instance.
        MSyncMLQueryTimerObserver*  iObserver;
         
    };

#endif      // CSYNCMLQUERYTIMER_H   
            
// End of File
