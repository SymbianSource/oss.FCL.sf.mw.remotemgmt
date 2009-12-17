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
* Description:  Implementation of the message query with timeout timer.
*
*/



// INCLUDE FILES
#include "SyncMLTimedMessageQuery.h"
#include "SyncMLNotifDebug.h"

// CONSTANTS
const TInt  KSyncMLuSecsInSec   = 1000000; // Microseconds in a second

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CSyncMLTimedMessageQuery::CSyncMLTimedMessageQuery
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CSyncMLTimedMessageQuery::CSyncMLTimedMessageQuery( TInt aTimeout )
    : iTimeout( aTimeout )
    {
    }

// -----------------------------------------------------------------------------
// CSyncMLTimedMessageQuery::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CSyncMLTimedMessageQuery* CSyncMLTimedMessageQuery::NewL( const TDesC& aMessage,
                                                                TInt  aTimeout )
    {
    CSyncMLTimedMessageQuery* self =
        new( ELeave ) CSyncMLTimedMessageQuery ( aTimeout );
    
    CleanupStack::PushL( self );
    self->SetMessageTextL( aMessage );
    CleanupStack::Pop( self );

    return self;
    }

    
// Destructor
CSyncMLTimedMessageQuery::~CSyncMLTimedMessageQuery()
    {
    if ( iTimer )
        {
        iTimer->Cancel();
        delete iTimer;
        iTimer = NULL;
        }
    }

// -----------------------------------------------------------------------------
// CSyncMLTimedMessageQuery::OkToExitL
// -----------------------------------------------------------------------------
//
TBool CSyncMLTimedMessageQuery::OkToExitL( TInt aButtonId )
    {
    if ( aButtonId == KErrTimedOut )
        {
        return ETrue;
        }
    return CAknQueryDialog::OkToExitL( aButtonId );
    }

// -----------------------------------------------------------------------------
// CSyncMLTimedMessageQuery::TimerExpired
// -----------------------------------------------------------------------------
//
void CSyncMLTimedMessageQuery::TimerExpired()
    {
    if ( iTimer )
        {
        iTimer->Cancel();
        delete iTimer;
        iTimer = NULL;
        iTimeout = KSyncMLNNoTimeout;
        }
    TRAPD( err, TryExitL( KErrTimedOut ) );
    
    if ( err != KErrNone )
        {
        FTRACE( FPrint(
            _L("[SmlNotif]\t CSyncMLTimedMessageQuery::TimerExpired() TryExitL returned %d"),
            err ) );
        }
    }
    
// -----------------------------------------------------------------------------
// CSyncMLTimedMessageQuery::PostLayoutDynInitL
// -----------------------------------------------------------------------------
//
void CSyncMLTimedMessageQuery::PostLayoutDynInitL()
    {
    if ( iTimeout > KSyncMLNNoTimeout )
        {
        iTimer = CSyncMLQueryTimer::NewL( this );
        iTimer->After( iTimeout * KSyncMLuSecsInSec );
        }
    }


//  End of File  
