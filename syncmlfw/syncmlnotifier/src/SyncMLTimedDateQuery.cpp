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
* Description:  Implementation of the message query with timeout timer.
*
*/



// INCLUDE FILES
#include "SyncMLTimedDateQuery.h"
#include "SyncMLNotifDebug.h"

// CONSTANTS
const TInt  KSyncMLuSecsInSec   = 1000000; // Microseconds in a second

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CSyncMLTimedDateQuery::CSyncMLTimedDateQuery
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CSyncMLTimedDateQuery::CSyncMLTimedDateQuery( TTime& aDate, TBool& aKeypress,TInt aTimeout )
    : CAknTimeQueryDialog(aDate, ENoTone),iTimeout( aTimeout ),iKeypress( aKeypress )
    {
    }

// -----------------------------------------------------------------------------
// CSyncMLTimedDateQuery::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CSyncMLTimedDateQuery* CSyncMLTimedDateQuery::NewL( TTime& aDataText, const TDesC& aPromptText,
                                                     TBool& aKeypress, TInt  aTimeout )
    {
    CSyncMLTimedDateQuery* self =
        new( ELeave ) CSyncMLTimedDateQuery ( aDataText, aKeypress, aTimeout );
    
    CleanupStack::PushL( self );
    self->SetPromptL( aPromptText );//chenge to set prompt
    CleanupStack::Pop( self );

    return self;
    }

    
// -----------------------------------------------------------------------------
// CSyncMLTimedDateQuery::~CSyncMLTimedDateQuery()
// Destructor.
// -----------------------------------------------------------------------------
//
CSyncMLTimedDateQuery::~CSyncMLTimedDateQuery()
    {
    if ( iTimer )
        {
        iTimer->Cancel();
        delete iTimer;
        iTimer = NULL;
        }
    }

// -----------------------------------------------------------------------------
// CSyncMLTimedDateQuery::OkToExitL
// -----------------------------------------------------------------------------
//
TBool CSyncMLTimedDateQuery::OkToExitL( TInt aButtonId )
    {
    if ( aButtonId == KErrTimedOut )
        {
        return ETrue;
        }
    return CAknTimeQueryDialog::OkToExitL( aButtonId );
    }

// -----------------------------------------------------------------------------
// CSyncMLTimedDateQuery::TimerExpired
// -----------------------------------------------------------------------------
//
void CSyncMLTimedDateQuery::TimerExpired()
    {
    if ( iTimer )
        {
        iTimer->Cancel();
        delete iTimer;
        iTimer = NULL;
        iTimeout = KSyncMLNNoTimeoutDate;
        }
    TRAPD( err, TryExitL( KErrTimedOut ) );
    
    if ( err != KErrNone )
        {
        FTRACE( FPrint(
            _L("[SmlNotif]\t CSyncMLTimedDateQuery::TimerExpired() TryExitL returned %d"),
            err ) );
        }
    }
    
// -----------------------------------------------------------------------------
// CSyncMLTimedDateQuery::PostLayoutDynInitL
// -----------------------------------------------------------------------------
//
void CSyncMLTimedDateQuery::PostLayoutDynInitL()
    {
    if ( iTimeout > KSyncMLNNoTimeoutDate )
        {
        iTimer = CSyncMLQueryTimer::NewL( this );
        iTimer->After( iTimeout * KSyncMLuSecsInSec );
        }
    }

// -----------------------------------------------------------------------------
// CSyncMLTimedDateQuery::NeedToDismissQueryL
// -----------------------------------------------------------------------------
//
TBool CSyncMLTimedDateQuery::NeedToDismissQueryL( const TKeyEvent& aKeyEvent )
    {
    if (aKeyEvent.iCode == EKeyNo)
        {
            iKeypress = ETrue;
            DismissQueryL();
            return ETrue;
            
        }
    return EFalse;
    }

// -----------------------------------------------------------------------------
// CSyncMLTimedDateQuery::DismissQueryL
// -----------------------------------------------------------------------------
//
void CSyncMLTimedDateQuery::DismissQueryL()
	{
	 TryExitL(EEikBidCancel);
	}


//  End of File  
