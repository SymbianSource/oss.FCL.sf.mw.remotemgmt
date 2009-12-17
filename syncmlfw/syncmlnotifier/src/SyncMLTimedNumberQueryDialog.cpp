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
* Description:  Implementation of the query dialog with timeout timer.
*
*/



// INCLUDE FILES
#include "SyncMLTimedNumberQueryDialog.h"
#include "SyncMLNotifDebug.h"

// CONSTANTS
const TInt  KSyncMLuSecsInSec   = 1000000; // Microseconds in a second

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CSyncMLTimedNumberQueryDialog::CSyncMLTimedNumberQueryDialog
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CSyncMLTimedNumberQueryDialog::CSyncMLTimedNumberQueryDialog( TReal& aValue, TBool& aKeypress,TInt aTimeout ):
CAknFloatingPointQueryDialog( aValue, CAknQueryDialog::EConfirmationTone )
   
    {
     iTimeout = aTimeout ;
     iKeypress = aKeypress;
    }
    
// -----------------------------------------------------------------------------
// CSyncMLTimedNumberQueryDialog::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CSyncMLTimedNumberQueryDialog* CSyncMLTimedNumberQueryDialog::NewL( TReal& aValue,                                                              
                                const TDesC& aMessage,TBool& aKeypress, TInt  aTimeout )                                                             
    {
    CSyncMLTimedNumberQueryDialog* self =
        new( ELeave ) CSyncMLTimedNumberQueryDialog( aValue, aKeypress, aTimeout ) ;
    
    CleanupStack::PushL( self );
    
    	self->SetPromptL( aMessage );
    CleanupStack::Pop( self );

    return self;
    }

  
// -----------------------------------------------------------------------------
// CSyncMLTimedNumberQueryDialog::~CSyncMLTimedNumberQueryDialog
// Destructor.
// -----------------------------------------------------------------------------
//
CSyncMLTimedNumberQueryDialog::~CSyncMLTimedNumberQueryDialog()
    {
    if ( iTimer )
        {
        iTimer->Cancel();
        delete iTimer;
        iTimer = NULL;
        }
    }

// -----------------------------------------------------------------------------
// CSyncMLTimedNumberQueryDialog::OkToExitL
// -----------------------------------------------------------------------------
//
TBool CSyncMLTimedNumberQueryDialog::OkToExitL( TInt aButtonId )
    {
    if ( aButtonId == KErrTimedOut )
        {
        return ETrue;
        }
    return CAknFloatingPointQueryDialog::OkToExitL( aButtonId );
    }

// -----------------------------------------------------------------------------
// CSyncMLTimedNumberQueryDialog::TimerExpired
// -----------------------------------------------------------------------------
//
void CSyncMLTimedNumberQueryDialog::TimerExpired()
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
            _L("[SmlNotif]\t CSyncMLTimedNumberQueryDialog::TimerExpired() \
             TryExitL returned %d"), err ) );
        }
    }
    
// -----------------------------------------------------------------------------
// CSyncMLTimedNumberQueryDialog::PostLayoutDynInitL
// -----------------------------------------------------------------------------
//
void CSyncMLTimedNumberQueryDialog::PostLayoutDynInitL()
    {
    if ( iTimeout > KSyncMLNNoTimeout )
        {
        iTimer = CSyncMLQueryTimer::NewL( this );
        iTimer->After( iTimeout * KSyncMLuSecsInSec );
        }
    CAknFloatingPointQueryDialog::PostLayoutDynInitL();
    }
    
// -----------------------------------------------------------------------------
// CSyncMLTimedNumberQueryDialog::NeedToDismissQueryL
// -----------------------------------------------------------------------------
//
TBool CSyncMLTimedNumberQueryDialog::NeedToDismissQueryL( const TKeyEvent& aKeyEvent )
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
// CSyncMLTimedNumberQueryDialog::DismissQueryL
// -----------------------------------------------------------------------------
//
void CSyncMLTimedNumberQueryDialog::DismissQueryL()
	{
	 TryExitL(EEikBidCancel);
	}


//  End of File  
