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
* Description:  Sources
*
*/


// INCLUDE FILES
#include <s32strm.h>
#include <nsmlchangefinder.h>


// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CNSmlDataItemUidSet::CNSmlDataItemUidSet
// Constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CNSmlDataItemUidSet::CNSmlDataItemUidSet()
    {
    }

// -----------------------------------------------------------------------------
// CNSmlDataItemUidSet::~CNSmlDataItemUidSet
// Destructor.
// -----------------------------------------------------------------------------
//
CNSmlDataItemUidSet::~CNSmlDataItemUidSet()
    {
    Reset();
    }

// -----------------------------------------------------------------------------
// CNSmlDataItemUidSet::AddItem
// Adds item to UID set.
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CNSmlDataItemUidSet::AddItem( TSmlDbItemUid aItemId )
	{
    if ( iUidSet.Find( aItemId ) != KErrNotFound )
        {
        return KErrAlreadyExists;
        }
    return iUidSet.Append( aItemId );
	}

// -----------------------------------------------------------------------------
// CNSmlDataItemUidSet::Reset
// Resets UID set.
// -----------------------------------------------------------------------------
//
EXPORT_C void CNSmlDataItemUidSet::Reset()
	{
	iUidSet.Reset();
	}
	
// -----------------------------------------------------------------------------
// CNSmlDataItemUidSet::InternalizeL
// This method internalizes class from stream.
// -----------------------------------------------------------------------------
//
EXPORT_C void CNSmlDataItemUidSet::InternalizeL( RReadStream& aStream )
	{
    TInt count( aStream.ReadInt32L() );
    for ( TInt i = 0; i < count; i++ )
        {
        AddItem( aStream.ReadInt32L() );	
        }	
	}

// -----------------------------------------------------------------------------
// CNSmlDataItemUidSet::DoItemCount
// Returns the number of items in the set.
// -----------------------------------------------------------------------------
//
TInt CNSmlDataItemUidSet::DoItemCount() const
	{
	return iUidSet.Count();
	}
	
// -----------------------------------------------------------------------------
// CNSmlDataItemUidSet::DoItemIndex
// Returns the index of UID.
// -----------------------------------------------------------------------------
//
TInt CNSmlDataItemUidSet::DoItemIndex( TSmlDbItemUid aItemId ) const
	{
	return iUidSet.Find( aItemId );
	}
	
// -----------------------------------------------------------------------------
// CNSmlDataItemUidSet::DoItemAt
// Retuns the item UID.
// -----------------------------------------------------------------------------
//
TSmlDbItemUid CNSmlDataItemUidSet::DoItemAt( TInt aIndex ) const
	{
	return iUidSet[aIndex];
	}
	
// -----------------------------------------------------------------------------
// CNSmlDataItemUidSet::DoExternalizeL
// This method externalizes class to stream.
// -----------------------------------------------------------------------------
//
void CNSmlDataItemUidSet::DoExternalizeL( RWriteStream& aStream ) const
	{
	aStream.WriteInt32L( iUidSet.Count() );
	for ( TInt i = 0; i < iUidSet.Count(); i++ )
		{
		aStream.WriteInt32L( iUidSet[i] );
		}
	}

// End of File
