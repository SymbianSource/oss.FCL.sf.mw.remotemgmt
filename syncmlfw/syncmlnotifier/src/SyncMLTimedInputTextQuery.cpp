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
* Description:  Implementation of the text input query with timeout timer.
*
*/



// INCLUDE FILES
#include "SyncMLTimedInputTextQuery.h"
#include "SyncMLNotifDebug.h"

// CONSTANTS
const TInt  KSyncMLuSecsInSec   = 1000000; // Microseconds in a second

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CSyncMLTimedInputTextQuery::CSyncMLTimedInputTextQuery
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CSyncMLTimedInputTextQuery::CSyncMLTimedInputTextQuery( TDes& aDataText, TBool& aKeypress, TInt aTimeout )
    : CAknTextQueryDialog(aDataText, ENoTone),iTimeout( aTimeout ),iKeypress(aKeypress)
    {
    
    }

// -----------------------------------------------------------------------------
// CSyncMLTimedInputTextQuery::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CSyncMLTimedInputTextQuery* CSyncMLTimedInputTextQuery::NewL( TDes& aDataText, const TDesC& aPromptText,
                                                              TBool& aKeypress, TInt  aTimeout )
    {
    CSyncMLTimedInputTextQuery* self =
        new( ELeave ) CSyncMLTimedInputTextQuery ( aDataText, aKeypress, aTimeout );
    
    CleanupStack::PushL( self );
    self->SetPromptL( aPromptText );//chenge to set prompt
    CleanupStack::Pop( self );

    return self;
    }

    
// -----------------------------------------------------------------------------
// CSyncMLTimedInputTextQuery::CSyncMLTimedInputTextQuery()
// Destructor.
// -----------------------------------------------------------------------------
//
CSyncMLTimedInputTextQuery::~CSyncMLTimedInputTextQuery()
    {
    if ( iTimer )
        {
        iTimer->Cancel();
        delete iTimer;
        iTimer = NULL;
        }
    }
 
// -----------------------------------------------------------------------------
// CSyncMLTimedInputTextQuery::OkToExitL
// -----------------------------------------------------------------------------
//
TBool CSyncMLTimedInputTextQuery::OkToExitL( TInt aButtonId )
    {
    if ( aButtonId == KErrTimedOut )
        {
        return ETrue;
        }
    return CAknTextQueryDialog::OkToExitL( aButtonId );
    }

// -----------------------------------------------------------------------------
// CSyncMLTimedInputTextQuery::TimerExpired
// -----------------------------------------------------------------------------
//
void CSyncMLTimedInputTextQuery::TimerExpired()
    {
    if ( iTimer )
        {
        iTimer->Cancel();
        delete iTimer;
        iTimer = NULL;
        iTimeout = KSyncMLNNoAlphanumTimeout;
        }
    TRAPD( err, TryExitL( KErrTimedOut ) );
    
    if ( err != KErrNone )
        {
        FTRACE( FPrint(
            _L("[SmlNotif]\t CSyncMLTimedInputTextQuery::TimerExpired() TryExitL returned %d"),
            err ) );
        }
    }
    
// -----------------------------------------------------------------------------
// CSyncMLTimedInputTextQuery::PostLayoutDynInitL
// -----------------------------------------------------------------------------
//
void CSyncMLTimedInputTextQuery::PostLayoutDynInitL()
    {
    if ( iTimeout > KSyncMLNNoAlphanumTimeout )
        {
        iTimer = CSyncMLQueryTimer::NewL( this );
        iTimer->After( iTimeout * KSyncMLuSecsInSec );
        }
    }

// -----------------------------------------------------------------------------
// CSyncMLTimedInputTextQuery::NeedToDismissQueryL
// -----------------------------------------------------------------------------
//
TBool CSyncMLTimedInputTextQuery::NeedToDismissQueryL(const TKeyEvent& aKeyEvent)
    {
    if (aKeyEvent.iCode == EKeyNo)
        {
        //CAknQueryControl* control = QueryControl();
        //if (control && control->QueryType() == EPinLayout)
          //  {
          iKeypress = ETrue;
            DismissQueryL();
            return ETrue;
            //}
        }
    return EFalse;
    }

// -----------------------------------------------------------------------------
// CSyncMLTimedInputTextQuery::DismissQueryL
// -----------------------------------------------------------------------------
//
void CSyncMLTimedInputTextQuery::DismissQueryL()
	{
	 TryExitL(EEikBidCancel);
	}

// -----------------------------------------------------------------------------
// CSyncMLTimedInputTextQuery::HandleQueryEditorStateEventL
// -----------------------------------------------------------------------------
//
TBool CSyncMLTimedInputTextQuery::HandleQueryEditorStateEventL(CAknQueryControl* aQueryControl, TQueryControlEvent aEventType, TQueryValidationStatus aStatus)
	{
 	if ( aQueryControl )
  {
 		if( EPhoneLayout == aQueryControl->QueryType() )
 	 	{
			if (aEventType == EEmergencyCallAttempted)
        	{
        	TryExitL(EEikBidCancel);
        	}
    	else
        	{
        	
         	TBuf<KSyncMLMaxDefaultResponseMsgLength> PhoneNo;
         	aQueryControl->GetText( PhoneNo );
         	TInt posplus = PhoneNo.LocateReverse('+');
          	if(posplus==0 || posplus==KErrNotFound )
           	{
           	CAknQueryDialog::HandleQueryEditorStateEventL(aQueryControl,aEventType,aStatus);	
           	}
          	else
           	{      
          	MakeLeftSoftkeyVisible(EFalse);
           	}
         	}  
  	 }
  }
 	else //For all other layouts 
 	 {
 	CAknQueryDialog::HandleQueryEditorStateEventL(aQueryControl,aEventType,aStatus);	
 	 }
	return EFalse;
	}

//  End of File  
