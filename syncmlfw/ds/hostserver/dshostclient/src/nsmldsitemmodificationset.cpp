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
* Description:  Utilities for DS Loader Framework
*
*/


// INCLUDE FILES
#ifndef __HOST_SERVER_MTEST__
#include <SmlDataProvider.h>
#endif

#include "nsmldsitemmodificationset.h"

// ================================= MEMBER FUNCTIONS =============================================

// ------------------------------------------------------------------------------------------------
// RNSmlDbItemModificationSet::AddItem
// Adds item to modification set.
// ------------------------------------------------------------------------------------------------
EXPORT_C TInt RNSmlDbItemModificationSet::AddItemL(const TNSmlDbItemModification& aItem )
	{
	if ( iItemSet.Find( aItem ) != KErrNotFound )
		{
		return KErrAlreadyExists;
		}
	else
		{
		iItemSet.AppendL( aItem );
		}
		
	return KErrNone;
	}

// ------------------------------------------------------------------------------------------------
// RNSmlDbItemModificationSet::RemoveItem
// Removes the item at a specified position from the modification set.
// ------------------------------------------------------------------------------------------------
EXPORT_C void RNSmlDbItemModificationSet::RemoveItem( TInt aIndex )
	{
	iItemSet.Remove( aIndex );
	}

// ------------------------------------------------------------------------------------------------
// RNSmlDbItemModificationSet::AddGroup
// Adds group of items to modification set.
// ------------------------------------------------------------------------------------------------
EXPORT_C void RNSmlDbItemModificationSet::AddGroupL( const MSmlDataItemUidSet& aUidSet, TNSmlDbItemModification::TNSmlDbItemModificationType aModType )
	{
	for ( TInt i = 0; i < aUidSet.ItemCount(); i++ )
		{
		TNSmlDbItemModification tempItem( aUidSet.ItemAt( i ), aModType );
		if ( iItemSet.Find( tempItem ) == KErrNotFound )
			{
			iItemSet.AppendL( tempItem );
			}
		}
	}

// ------------------------------------------------------------------------------------------------
// RNSmlDbItemModificationSet::ItemCount
// Returns the number of items in the set.
// ------------------------------------------------------------------------------------------------
EXPORT_C TInt RNSmlDbItemModificationSet::ItemCount() const
	{
	return iItemSet.Count();
	}
	
// ------------------------------------------------------------------------------------------------
// RNSmlDbItemModificationSet::ItemIndex
// Returns the index of modification item.
// ------------------------------------------------------------------------------------------------
EXPORT_C TInt RNSmlDbItemModificationSet::ItemIndex( const TSmlDbItemUid aItemId ) const
	{
	for ( TInt i = 0; i < iItemSet.Count(); i++ )
		{
		if ( iItemSet[i].ItemId() == aItemId )
			{
			return i;
			}
		}
	return KErrNotFound;
	}

// ------------------------------------------------------------------------------------------------
// RNSmlDbItemModificationSet::ItemAt
// Retuns the modifcation item.
// ------------------------------------------------------------------------------------------------
EXPORT_C TNSmlDbItemModification RNSmlDbItemModificationSet::ItemAt( TInt aIndex ) const
	{
	return iItemSet[aIndex];
	}

// ------------------------------------------------------------------------------------------------
// RNSmlDbItemModificationSet::Reset
// Resets modification set.
// ------------------------------------------------------------------------------------------------
EXPORT_C void RNSmlDbItemModificationSet::Reset()
	{
	iItemSet.Reset();
	}

// ------------------------------------------------------------------------------------------------
// RNSmlDbItemModificationSet::Reset
// Closes modification set.
// ------------------------------------------------------------------------------------------------	
EXPORT_C void RNSmlDbItemModificationSet::Close()
	{
	Reset();
	}

// ------------------------------------------------------------------------------------------------
// RNSmlDbItemModificationSet::ExternalizeL
// This method externalizes class to stream.
// ------------------------------------------------------------------------------------------------
EXPORT_C void RNSmlDbItemModificationSet::ExternalizeL( RWriteStream& aStream ) const
	{
	aStream.WriteInt32L( iItemSet.Count() );
	for ( TInt i = 0; i < iItemSet.Count(); i++ )
		{
		TPckgBuf<TSmlDbItemUid> b( ( iItemSet[i].ItemId() ) );
		aStream << b;
		TPckgBuf<TNSmlDbItemModification::TNSmlDbItemModificationType> c( iItemSet[i].ModificationType() );
		aStream << c;
		}
	}

// ------------------------------------------------------------------------------------------------
// RNSmlDbItemModificationSet::InternalizeL
// This method internalizes class from stream.
// ------------------------------------------------------------------------------------------------
EXPORT_C void RNSmlDbItemModificationSet::InternalizeL( RReadStream& aStream )
	{
	TInt count( aStream.ReadInt32L() );
	for ( TInt i = 0; i < count; i++ )
		{
		TPckgBuf<TSmlDbItemUid> packageA;
		aStream >> packageA;
		TSmlDbItemUid tempId = packageA();
		
		TPckgBuf<TNSmlDbItemModification::TNSmlDbItemModificationType> packageB;
		aStream >> packageB;
		TNSmlDbItemModification::TNSmlDbItemModificationType tempModType = packageB();

		TNSmlDbItemModification tempItem( tempId, tempModType );
		AddItemL( tempItem );	
		}
	};

// ------------------------------------------------------------------------------------------------
// RNSmlDbItemModificationSet::StreamSize
// calculates size needed for stream when externalized.
// ------------------------------------------------------------------------------------------------
EXPORT_C TInt RNSmlDbItemModificationSet::StreamSize() const
	{
	return ( sizeof( TInt32 ) + ( sizeof( TNSmlDbItemModification ) + 2 ) * ItemCount() );
	}
	
// End of File
