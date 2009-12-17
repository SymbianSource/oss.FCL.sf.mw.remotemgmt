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
#include "nsmldshostitem.h"

// ================================= MEMBER FUNCTIONS =============================================

// ------------------------------------------------------------------------------------------------
// CNSmlDSHostItem::NewL
// Creates new instance of CNSmlDSHostItem based class.
// ------------------------------------------------------------------------------------------------
EXPORT_C CNSmlDSHostItem* CNSmlDSHostItem::NewL()
	{	
	CNSmlDSHostItem* self = NewLC();
	CleanupStack::Pop();
	return self;
	}
	
// ------------------------------------------------------------------------------------------------
// CNSmlDSHostItem::NewLC
// Creates new instance of CNSmlDSHostItem based class.
// ------------------------------------------------------------------------------------------------
EXPORT_C CNSmlDSHostItem* CNSmlDSHostItem::NewLC()
	{
	CNSmlDSHostItem* self = new (ELeave) CNSmlDSHostItem();
	CleanupStack::PushL( self );
	self->ConstructL();
	return self;
	}

// ------------------------------------------------------------------------------------------------
// CNSmlDSHostItem::~CNSmlDSHostItem
// C++ Destructor.
// ------------------------------------------------------------------------------------------------
EXPORT_C CNSmlDSHostItem::~CNSmlDSHostItem()
	{
	delete iMimeType;
	delete iMimeVer;
	}

// ------------------------------------------------------------------------------------------------
// CNSmlDSHostItem::ExternalizeL
// This method externalizes class to stream.
// ------------------------------------------------------------------------------------------------
EXPORT_C void CNSmlDSHostItem::ExternalizeL( RWriteStream& aStream ) const
	{
	aStream.WriteInt32L( iUid );
	aStream.WriteInt32L( iFieldChange );
	aStream.WriteInt32L( iSize );
	aStream.WriteInt32L( iParentUid );
	aStream.WriteUint32L( iMimeType->Length() );
	aStream << *iMimeType;
	aStream.WriteUint32L( iMimeVer->Length() );
	aStream << *iMimeVer;
	}
		
// ------------------------------------------------------------------------------------------------
// CNSmlDSHostItem::InternalizeL
// This method internalizes class from stream.
// ------------------------------------------------------------------------------------------------
EXPORT_C void CNSmlDSHostItem::InternalizeL( RReadStream& aStream )
	{
	iUid = aStream.ReadInt32L();
	iFieldChange = aStream.ReadInt32L();
	iSize = aStream.ReadInt32L();
	iParentUid = aStream.ReadInt32L();

	delete iMimeType;
	iMimeType = NULL;
	delete iMimeVer;
	iMimeVer = NULL;
	
	iMimeType = HBufC8::NewL( aStream, aStream.ReadUint32L() );
	iMimeVer = HBufC8::NewL( aStream, aStream.ReadUint32L() );
	}
		
// ------------------------------------------------------------------------------------------------
// CNSmlDSHostItem::Uid
// Returns UID of item.
// ------------------------------------------------------------------------------------------------
EXPORT_C TSmlDbItemUid CNSmlDSHostItem::Uid() const
	{
	return iUid;
	}

// ------------------------------------------------------------------------------------------------
// CNSmlDSHostItem::SetUid
// Sets UID of item.
// ------------------------------------------------------------------------------------------------
EXPORT_C void CNSmlDSHostItem::SetUid( const TSmlDbItemUid aItemId )
	{
	iUid = aItemId;
	}
	
// ------------------------------------------------------------------------------------------------
// CNSmlDSHostItem::FieldChange
// Returns fieldChange info.
// ------------------------------------------------------------------------------------------------
EXPORT_C TBool CNSmlDSHostItem::FieldChange() const
	{
	return iFieldChange;
	}
		
// ------------------------------------------------------------------------------------------------
// CNSmlDSHostItem::SetFieldChange
// Sets fieldChange info.
// ------------------------------------------------------------------------------------------------
EXPORT_C void CNSmlDSHostItem::SetFieldChange( const TBool aFieldChange )
	{
	iFieldChange = aFieldChange;
	}

// ------------------------------------------------------------------------------------------------
// CNSmlDSHostItem::Size
// Returns size of item.
// ------------------------------------------------------------------------------------------------
EXPORT_C TInt CNSmlDSHostItem::Size() const
	{
	return iSize;
	}
		
// ------------------------------------------------------------------------------------------------
// CNSmlDSHostItem::SetSize
// Sets size of item.
// ------------------------------------------------------------------------------------------------
EXPORT_C void CNSmlDSHostItem::SetSize( const TInt aSize )
	{
	iSize = aSize;
	}	

// ------------------------------------------------------------------------------------------------
// CNSmlDSHostItem::ParentUid
// Returns UID of parent of item.
// ------------------------------------------------------------------------------------------------
EXPORT_C TSmlDbItemUid CNSmlDSHostItem::ParentUid() const
	{
	return iParentUid;
	}
		
// ------------------------------------------------------------------------------------------------
// CNSmlDSHostItem::SetParentUid
// Sets UID of parent of item.
// ------------------------------------------------------------------------------------------------
EXPORT_C void CNSmlDSHostItem::SetParentUid( const TSmlDbItemUid aParentId )
	{
	iParentUid = aParentId;
	}			

// ------------------------------------------------------------------------------------------------
// CNSmlDSHostItem::MimeTypeL
// Returns mime type of item.
// ------------------------------------------------------------------------------------------------
EXPORT_C const HBufC8* CNSmlDSHostItem::MimeType() const
	{
	return iMimeType;
	}
		
// ------------------------------------------------------------------------------------------------
// CNSmlDSHostItem::SetMimeTypeL
// Sets mime type of item.
// ------------------------------------------------------------------------------------------------
EXPORT_C void CNSmlDSHostItem::SetMimeTypeL( const TDesC8& aMimeType )
	{
	delete iMimeType;
	iMimeType = NULL;
	iMimeType = aMimeType.AllocL();
	}		

// ------------------------------------------------------------------------------------------------
// CNSmlDSHostItem::MimeVerL
// Returns mime version of item.
// ------------------------------------------------------------------------------------------------
EXPORT_C const HBufC8* CNSmlDSHostItem::MimeVer() const
	{
	return iMimeVer;
	}
		
// ------------------------------------------------------------------------------------------------
// CNSmlDSHostItem::SetMimeVerL
// Sets mime version of item.
// ------------------------------------------------------------------------------------------------
EXPORT_C void CNSmlDSHostItem::SetMimeVerL( const TDesC8& aMimeVer )
	{
	delete iMimeVer;
	iMimeVer = NULL;
	iMimeVer = aMimeVer.AllocL();
	}

// ------------------------------------------------------------------------------------------------
// CNSmlDSHostItem::CNSmlDSHostItem
// C++ constructor.
// ------------------------------------------------------------------------------------------------
EXPORT_C CNSmlDSHostItem::CNSmlDSHostItem() : 
		iUid( 0 ),iFieldChange( EFalse ), iSize( 0 ), iParentUid( KNullDataItemId ),
		iMimeType( 0 ), iMimeVer( 0 )
	{
	}

// ------------------------------------------------------------------------------------------------
// CNSmlDSHostItem::ConstrucL
// ConstructL
// ------------------------------------------------------------------------------------------------
EXPORT_C void CNSmlDSHostItem::ConstructL()
	{
	iMimeType = HBufC8::NewL( 0 );
	iMimeVer = HBufC8::NewL( 0 );
	}
	
// End of File
