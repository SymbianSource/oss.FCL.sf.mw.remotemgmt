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
* Description:  Implementation of the query dialog.
*
*/



// INCLUDE FILES
#include "SyncMLQueryDialog.h"
#include "SyncMLNotifDebug.h"


// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CSyncMLQueryDialog::CSyncMLQueryDialog
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CSyncMLQueryDialog::CSyncMLQueryDialog(TBool& aKeyPress  ):iKeyPress(aKeyPress)
    
    {
    }

// -----------------------------------------------------------------------------
// CSyncMLQueryDialog::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CSyncMLQueryDialog* CSyncMLQueryDialog::NewL(TBool& aKeyPress )
    {
    CSyncMLQueryDialog* self =
        new( ELeave ) CSyncMLQueryDialog (aKeyPress  );
    
    return self;
    }

    
// -----------------------------------------------------------------------------
// CSyncMLQueryDialog::~CSyncMLQueryDialog
// Destructor.
// -----------------------------------------------------------------------------
//
CSyncMLQueryDialog::~CSyncMLQueryDialog()
    {
    }

// -----------------------------------------------------------------------------
// CSyncMLQueryDialog::OkToExitL
// Determines if the dialog can be closed.
// -----------------------------------------------------------------------------
//
TBool CSyncMLQueryDialog::OkToExitL( TInt aButtonId )
    {
    return CAknQueryDialog::OkToExitL( aButtonId );
    }

// -----------------------------------------------------------------------------
// CSyncMLQueryDialog::NeedToDismissQueryL
// Called by OfferkeyEventL(), gives a change to dismiss the query even with
// keys different than Enter of Ok.
// -----------------------------------------------------------------------------
// 
TBool CSyncMLQueryDialog::NeedToDismissQueryL(const TKeyEvent& aKeyEvent)
    {
    if (aKeyEvent.iCode == EKeyNo)
        {
        iKeyPress=ETrue;
		DismissQueryL();
        return ETrue;
        }
    return EFalse;
    }

//  End of File  
