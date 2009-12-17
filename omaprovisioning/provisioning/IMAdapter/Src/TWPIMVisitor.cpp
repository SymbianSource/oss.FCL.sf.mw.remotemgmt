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
* Description:  TWPIMVisitor crawls a logical proxy
*
*/



// INCLUDE FILES
#include <e32base.h>
#include <CWPCharacteristic.h>
#include <CWPParameter.h>
#include "TWPIMVisitor.h"

// CONSTANTS
const TInt KNumNames = 1;

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// TWPIMVisitor::TWPIMVisitor
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
TWPIMVisitor::TWPIMVisitor()
	{
	iNapDef = NULL;
	iName.Set( KNullDesC );
	}

// -----------------------------------------------------------------------------
// TWPIMVisitor::VisitL
// -----------------------------------------------------------------------------
//
void TWPIMVisitor::VisitL( CWPParameter& aElement )
	{
	switch( aElement.ID() )
		{
		case EWPParameterName:
			if( iName == KNullDesC )
				{
				iName.Set( aElement.Value() );
				}
			break;

		default:
			break;
		}
	}
	
// -----------------------------------------------------------------------------
// TWPIMVisitor::VisitL
// -----------------------------------------------------------------------------
//
void TWPIMVisitor::VisitL( CWPCharacteristic& aElement )
	{ 
	if( !iNapDef && aElement.Type() == KWPPxPhysical ) 
		{ 
		aElement.AcceptL( *this );
		} 
	}

// -----------------------------------------------------------------------------
// TWPIMVisitor::VisitLinkL
// -----------------------------------------------------------------------------
//
void TWPIMVisitor::VisitLinkL( CWPCharacteristic& aLink )
	{ 
	if( !iNapDef && aLink.Type() == KWPNapDef ) 
		{ 
		iNapDef = &aLink; 
		} 
	};



//  End of File  
