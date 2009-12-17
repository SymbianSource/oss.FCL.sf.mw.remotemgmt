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
* Description:  Interface for UID information exchange.
*
*/


#include <SmlDataProvider.h>	
#include <SyncMLDataFilter.h>

// ------------------------------------------------------------------------------------------------
// MSmlDataItemUidSet::ItemCount
// Returns the number of items in the set.
// ------------------------------------------------------------------------------------------------
EXPORT_C TInt MSmlDataItemUidSet::ItemCount() const
{
	return DoItemCount();
}

// ------------------------------------------------------------------------------------------------
// MSmlDataItemUidSet::ItemIndex
// Returns the index of the specified item UID in the set, or -1 if the item UID is not present.
// ------------------------------------------------------------------------------------------------
EXPORT_C TInt MSmlDataItemUidSet::ItemIndex( TSmlDbItemUid aItemId ) const
{
	return DoItemIndex( aItemId );
}

// ------------------------------------------------------------------------------------------------
// MSmlDataItemUidSet::ItemAt
// Retuns the item UID at the specified index in the set.
// ------------------------------------------------------------------------------------------------
EXPORT_C TSmlDbItemUid MSmlDataItemUidSet::ItemAt( TInt aIndex ) const
{
	return DoItemAt( aIndex );
}

// ------------------------------------------------------------------------------------------------
// MSmlDataItemUidSet::ExternalizeL
// Externalizes UIDs from set.
// ------------------------------------------------------------------------------------------------
EXPORT_C void MSmlDataItemUidSet::ExternalizeL( RWriteStream& aStream ) const
{
	return DoExternalizeL( aStream );
}

// End of File
