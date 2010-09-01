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
* Description:  Timer for queries that have timeout.
*
*/



// INCLUDE FILES
#include    "SyncMLQueryTimer.h"

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CSyncMLQueryTimer::CSyncMLQueryTimer
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CSyncMLQueryTimer::CSyncMLQueryTimer( MSyncMLQueryTimerObserver* aObserver )
    : CTimer( EPriorityStandard ), iObserver( aObserver )
    {
    }

// -----------------------------------------------------------------------------
// CSyncMLQueryTimer::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CSyncMLQueryTimer::ConstructL()
    {
	CTimer::ConstructL();
	CActiveScheduler::Add( this );
    }

// -----------------------------------------------------------------------------
// CSyncMLQueryTimer::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CSyncMLQueryTimer* CSyncMLQueryTimer::NewL( MSyncMLQueryTimerObserver* aObserver )
    {
    CSyncMLQueryTimer* self = new( ELeave ) CSyncMLQueryTimer( aObserver );
    
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );

    return self;
    }

    
// Destructor
CSyncMLQueryTimer::~CSyncMLQueryTimer()
    {
    
    }


// -----------------------------------------------------------------------------
// CSyncMLQueryTimer::RunL
// -----------------------------------------------------------------------------
//
void CSyncMLQueryTimer::RunL()
    {
    if ( iObserver )
        {
        iObserver->TimerExpired();
        }
    }

//  End of File  
