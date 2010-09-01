/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description: 
*     Sortable pair of strings.
*
*/


// INCLUDE FILES
#include "CWPCodeQueryDialog.h"

// ============================ MEMBER FUNCTIONS ===============================


// ----------------------------------------------------------------------------
// CCodeQueryDialog::CCodeQueryDialog()
// C++ constructor
// ----------------------------------------------------------------------------
//
CWPCodeQueryDialog::CWPCodeQueryDialog( TDes& aDataText )
                                    : CAknTextQueryDialog( aDataText, ENoTone )
	{
	}


// ----------------------------------------------------------------------------
// Destructor
// ----------------------------------------------------------------------------
//
CWPCodeQueryDialog::~CWPCodeQueryDialog()
	{
	}

// ----------------------------------------------------------------------------
// CWPCodeQueryDialog::OfferKeyEventL
// called by framework when any key is pressed
// ----------------------------------------------------------------------------
//
TKeyResponse CWPCodeQueryDialog::OfferKeyEventL( const TKeyEvent& aKeyEvent,
                                                 TEventCode aType )
	{
	// '#' key 
	if( aKeyEvent.iScanCode == EStdKeyHash  && aType == EEventKeyUp)
		{ 
		TryExitL( EEikBidOk );
		return EKeyWasConsumed;
		}
	
	// '*' key
	if ( aKeyEvent.iCode == '*' )
		{
		return EKeyWasConsumed;
		}

	return CAknTextQueryDialog::OfferKeyEventL( aKeyEvent, aType );
	}

// ----------------------------------------------------------------------------
// CWPCodeQueryDialog::NeedToDismissQueryL()
// Handles '#' key called by CAknTextQueryDialog::OfferKeyEventL()
// ----------------------------------------------------------------------------
//
TBool CWPCodeQueryDialog::NeedToDismissQueryL( const TKeyEvent& /*aKeyEvent*/ )
	{
	return EFalse;
	}

// ----------------------------------------------------------------------------
// CWPCodeQueryDialog::OkToExitL()
// called by framework when the Softkey is pressed
// ----------------------------------------------------------------------------
//
TBool CWPCodeQueryDialog::OkToExitL( TInt aButtonId )
	{
	return CAknTextQueryDialog::OkToExitL( aButtonId );
	}
	
//  End of File
