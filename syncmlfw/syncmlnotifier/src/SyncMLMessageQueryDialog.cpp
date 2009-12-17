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
* Description:  Message Query Dialogue
*
*/



// INCLUDE FILES
#include "SyncMLMessageQueryDialog.h"
#include "SyncMLNotifDebug.h"


// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CSyncMLMessageQueryDialog::CSyncMLMessageQueryDialog
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CSyncMLMessageQueryDialog::CSyncMLMessageQueryDialog(TBool& aKeyPress  ):iKeyPress(aKeyPress)
    
    {
    }

// -----------------------------------------------------------------------------
// CSyncMLMessageQueryDialog::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CSyncMLMessageQueryDialog* CSyncMLMessageQueryDialog::NewL(TBool& aKeyPress )
    {
    CSyncMLMessageQueryDialog* self =
        new( ELeave ) CSyncMLMessageQueryDialog (aKeyPress  );
    
    return self;
    }

    
// -----------------------------------------------------------------------------
// CSyncMLMessageQueryDialog::~CSyncMLMessageQueryDialog
// Destructor.
// -----------------------------------------------------------------------------
//
CSyncMLMessageQueryDialog::~CSyncMLMessageQueryDialog()
    {
    
    }

// -----------------------------------------------------------------------------
// CSyncMLMessageQueryDialog::OkToExitL
// Determines if the dialog can be closed..
// -----------------------------------------------------------------------------
//
TBool CSyncMLMessageQueryDialog::OkToExitL( TInt aButtonId )
    {
    FLOG(_L("[SmlNotif]\t CSyncMLMessageQueryDialog::OkToExitL()"));
    return CAknQueryDialog::OkToExitL( aButtonId );
    }

// -----------------------------------------------------------------------------
// CSyncMLMessageQueryDialog::NeedToDismissQueryL
// Called by OfferkeyEventL(), gives a change to dismiss the query even with
// keys different than Enter of Ok.
// -----------------------------------------------------------------------------
//    
TBool CSyncMLMessageQueryDialog::NeedToDismissQueryL(const TKeyEvent& aKeyEvent)
    {
    FLOG(_L("[SmlNotif]\t CSyncMLMessageQueryDialog::NeedToDismissQueryL()"));
    if (aKeyEvent.iCode == EKeyNo)
        {
        FLOG(_L("[SmlNotif]\t CSyncMLMessageQueryDialog::NeedToDismissQueryL()--EKeyNo"));
        iKeyPress=ETrue;
		DismissQueryL();
        return ETrue;
        }
    return EFalse;
    }

// -----------------------------------------------------------------------------
// CSyncMLMessageQueryDialog::ExitL
// -----------------------------------------------------------------------------
//
void CSyncMLMessageQueryDialog::ExitL()
	{
	FLOG(_L("[SmlNotif]\t CSyncMLMessageQueryDialog::ExitL()"));
	OkToExitL( EKeyNo );
	}

//  End of File  
