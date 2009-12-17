/*
* Copyright (c) 2004 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  DS-settings 
*
*/



// INCLUDE FILES
#include <nsmlconstants.h>
#include "nsmldssettings.h"


//=============================================
//
//		CNSmlDSProfileListItem
//
//=============================================

//=============================================
//		CNSmlDSProfileListItem::NewL()
//		Creates a new instance of CNSmlDSProfileListItem object. 
//=============================================

EXPORT_C CNSmlDSProfileListItem* CNSmlDSProfileListItem::NewL( const TInt aId )
	{
	CNSmlDSProfileListItem* self = CNSmlDSProfileListItem::NewLC( aId );
	CleanupStack::Pop();
	return self;
	}

//=============================================
//		CNSmlDSProfileListItem::NewLC()
//		Creates a new instance of CNSmlDSProfileListItem object. 
//		Pushes and leaves new instance into CleanupStack.
//=============================================
	
EXPORT_C CNSmlDSProfileListItem* CNSmlDSProfileListItem::NewLC( const TInt aId )
	{
	CNSmlDSProfileListItem* self = new( ELeave ) CNSmlDSProfileListItem;
	CleanupStack::PushL( self );
	self->ConstructL();
	self->iId = aId;
	return self;
	}

//=============================================
//		CNSmlDSProfileListItem::ConstructL()
//		Second phase constructor.
//=============================================

void CNSmlDSProfileListItem::ConstructL()
	{
	}

//=============================================
//		CNSmlDSProfileListItem::~CNSmlDSProfileListItem()
//		Destructor.
//=============================================

CNSmlDSProfileListItem::~CNSmlDSProfileListItem()
	{
	}

//=============================================
//		CNSmlDSProfileListItem::AssignL()
//		Assigns values given in parameter to this object.
//=============================================
EXPORT_C void CNSmlDSProfileListItem::AssignL( const CNSmlDSProfileListItem* aSourceItem )
	{
	iId = aSourceItem->iId;
	}


//=============================================
//		CNSmlDSProfileListItem::IntValue()
//		Returns a given integer value.
//=============================================

EXPORT_C TInt CNSmlDSProfileListItem::IntValue( TNSmlDSProfileData aProfileItem ) const
	{	
	TInt result( 0 );
	switch ( aProfileItem )
		{
		case ( EDSProfileId ) :
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
