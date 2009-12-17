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
* Description:  Implementation of the query dialog with timeout timer.
*
*/



// INCLUDE FILES
#include "SyncMLTimedQueryDialog.h"
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
CSyncMLTimedQueryDialog::CSyncMLTimedQueryDialog( TInt aTimeout )
    : iTimeout( aTimeout )
    {
    iTone = EConfirmationTone;
    }

// -----------------------------------------------------------------------------
// CSyncMLTimedMessageQuery::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CSyncMLTimedQueryDialog* CSyncMLTimedQueryDialog::NewL( const TDesC& aMessage,
                                                                TInt  aTimeout )
    {
    CSyncMLTimedQueryDialog* self =
        new( ELeave ) CSyncMLTimedQueryDialog ( aTimeout );
    
    CleanupStack::PushL( self );
    self->SetPromptL( aMessage );
    CleanupStack::Pop( self );

    return self;
    }

    
// Destructor
CSyncMLTimedQueryDialog::~CSyncMLTimedQueryDialog()
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
TBool CSyncMLTimedQueryDialog::OkToExitL( TInt aButtonId )
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
void CSyncMLTimedQueryDialog::TimerExpired()
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
            _L("[SmlNotif]\t CSyncMLTimedQueryDialog::TimerExpired() \
             TryExitL returned %d"), err ) );
        }
    }
    
// -----------------------------------------------------------------------------
// CSyncMLTimedMessageQuery::PostLayoutDynInitL
// -----------------------------------------------------------------------------
//
void CSyncMLTimedQueryDialog::PostLayoutDynInitL()
    {
    if ( iTimeout > KSyncMLNNoTimeout )
        {
        iTimer = CSyncMLQueryTimer::NewL( this );
        iTimer->After( iTimeout * KSyncMLuSecsInSec );
        }
    CAknQueryDialog::PostLayoutDynInitL();
    }


//  End of File  
