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
* Description:  Implementation for transport handler and related classes
*
*/



// INCLUDE FILES
#include <s32strm.h>

#include "NSmlTransportHandler.h"


// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CSyncMLTransportPropertiesArray::CSyncMLTransportPropertiesArray
// Constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CSyncMLTransportPropertiesArray::CSyncMLTransportPropertiesArray( TInt aGranularity )
	: iElements( aGranularity )
	{
	}
	
// -----------------------------------------------------------------------------
// CSyncMLTransportPropertiesArray::InternalizeL
// Internalizes the properties from the stream.
// -----------------------------------------------------------------------------
//
EXPORT_C void CSyncMLTransportPropertiesArray::InternalizeL( RReadStream& aStream )
	{
	iElements.Reset();
	
	TInt count = aStream.ReadUint16L(); // how many props
	
	TSyncMLTransportPropertyInfo prop;
	for ( TInt i = 0; i < count; ++i )
		{
		prop.InternalizeL( aStream );
		AddL( prop );
		}
	}
	
// -----------------------------------------------------------------------------
// CSyncMLTransportPropertiesArray::ExternalizeL
// Externalizes the properties to the stream.
// -----------------------------------------------------------------------------
//
EXPORT_C void CSyncMLTransportPropertiesArray::ExternalizeL( RWriteStream& aStream ) const
	{
	// first write count
	aStream.WriteUint16L( Count() );

	// write each property
	for ( TInt i = 0; i < Count(); ++i )
		{
		iElements[i].ExternalizeL( aStream );
		}
	}
	
// -----------------------------------------------------------------------------
// CSyncMLTransportPropertiesArray::AddL
// Appends the given property to the array and returns the index 
// of the added property.
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CSyncMLTransportPropertiesArray::AddL( const TPropertyInfo& aProperty )
	{
	iElements.AppendL( aProperty );
	return iElements.Count() - 1 ;
	}
	
// -----------------------------------------------------------------------------
// CSyncMLTransportPropertiesArray::Delete
// Deletes a property from the given index.
// -----------------------------------------------------------------------------
//
EXPORT_C void CSyncMLTransportPropertiesArray::Delete( TInt aIndex )
	{
	iElements.Delete( aIndex );
	}
	
// -----------------------------------------------------------------------------
// CSyncMLTransportPropertiesArray::Find
// Returns the index of the given property relative to 1, if it 
// is found. Otherwise returns 0.
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CSyncMLTransportPropertiesArray::Find( const TDesC8& aPropertyName ) const
	{
	for ( TInt i = 0; i < iElements.Count(); ++i )
		{
		if ( iElements[i].iName == aPropertyName )
			{
			return i+1;
			}
		}
	
	return 0;
	}
	
// -----------------------------------------------------------------------------
// CSyncMLTransportPropertiesArray::Count
// Returns the count of properties in array.
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CSyncMLTransportPropertiesArray::Count() const
	{
	return iElements.Count();
	}
	
// -----------------------------------------------------------------------------
// CSyncMLTransportPropertiesArray::At
// Returns a reference to the indexed property in the array. 
// Does not do any index checking.
// -----------------------------------------------------------------------------
//
EXPORT_C CSyncMLTransportPropertiesArray::TPropertyInfo& CSyncMLTransportPropertiesArray::At( TInt aIndex )
	{
	return iElements.At( aIndex );
	}
	
// -----------------------------------------------------------------------------
// CSyncMLTransportPropertiesArray::At
// Returns a const reference to the indexed property in the array. 
// Does not do any index checking.
// -----------------------------------------------------------------------------
//
EXPORT_C const CSyncMLTransportPropertiesArray::TPropertyInfo& CSyncMLTransportPropertiesArray::At( TInt aIndex ) const
	{
	return iElements.At( aIndex );
	}
	


