/*
* Copyright (c) 2002-2004 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  DM-settings 
*
*/



// INCLUDE FILES
#include <nsmlconstants.h>
#include "nsmldmsettings.h"


//------------------------------------------------------------------------------
//
//		CNSmlDMProfileListItem
//
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//		CNSmlDMProfileListItem::NewL()
//		Creates a new instance of CNSmlDMProfileListItem object. 
//------------------------------------------------------------------------------

EXPORT_C CNSmlDMProfileListItem* CNSmlDMProfileListItem::NewL( const TInt aId )
	{
	CNSmlDMProfileListItem* self = CNSmlDMProfileListItem::NewLC( aId );
	CleanupStack::Pop();
	return self;
	}

//------------------------------------------------------------------------------
//		CNSmlDMProfileListItem::NewLC()
//		Creates a new instance of CNSmlDMProfileListItem object. 
//		Pushes and leaves new instance into CleanupStack.
//------------------------------------------------------------------------------
	
EXPORT_C CNSmlDMProfileListItem* CNSmlDMProfileListItem::NewLC( const TInt aId )
	{
	CNSmlDMProfileListItem* self = new( ELeave ) CNSmlDMProfileListItem;
	CleanupStack::PushL( self );
	self->ConstructL();
	self->iId = aId;
	return self;
	}

//------------------------------------------------------------------------------
//		CNSmlDMProfileListItem::ConstructL()
//		Second phase constructor.
//------------------------------------------------------------------------------

void CNSmlDMProfileListItem::ConstructL()
	{
	}

//------------------------------------------------------------------------------
//		CNSmlDMProfileListItem::~CNSmlDMProfileListItem()
//		Destructor.
//------------------------------------------------------------------------------

CNSmlDMProfileListItem::~CNSmlDMProfileListItem()
	{
	}

//------------------------------------------------------------------------------
//		CNSmlDMProfileListItem::AssignL()
//		Assigns values given in parameter to this object.
//------------------------------------------------------------------------------
EXPORT_C void CNSmlDMProfileListItem::AssignL( const CNSmlDMProfileListItem* aSourceItem )
	{
	iId = aSourceItem->iId;
	}


//------------------------------------------------------------------------------
//		CNSmlDMProfileListItem::IntValue()
//		Returns a given integer value.
//------------------------------------------------------------------------------

EXPORT_C TInt CNSmlDMProfileListItem::IntValue( TNSmlDMProfileData aProfileItem ) const
	{	
	TInt result( 0 );
	switch ( aProfileItem )
		{
		case ( EDMProfileId ) :
			{
			result = iId;
			}
		break;

		default:
			{
			User::Panic( KNSmlIndexOutOfBoundStr, KNSmlPanicIndexOutOfBound );
			}
		}
	return result;
	}



//  End of File
