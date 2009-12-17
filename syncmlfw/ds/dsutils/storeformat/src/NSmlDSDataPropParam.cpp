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
* Description:  DS Utils Store format
*
*/

/**
Data property Parameter class
Property Parameter.
Sub-type of property.
*/

// INCLUDE FILES
#include <SmlDataFormat.h>
#include <stringpool.h>

// CONSTANTS
const TInt KVersionValue = 1;

// ===================================== MEMBER FUNCTIONS ========================================

// ------------------------------------------------------------------------------------------------
// CSmlDataPropertyParam::NewLC()
// Class constructor
// ------------------------------------------------------------------------------------------------	
EXPORT_C CSmlDataPropertyParam* CSmlDataPropertyParam::NewLC()
{
	CSmlDataPropertyParam* self = new( ELeave ) CSmlDataPropertyParam;
	CleanupStack::PushL( self );
		
	return self;
}

// ------------------------------------------------------------------------------------------------
// CSmlDataPropertyParam::NewLC()
// Class constructor from resource. 
// ------------------------------------------------------------------------------------------------
EXPORT_C CSmlDataPropertyParam* CSmlDataPropertyParam::NewLC( const RStringPool& aStringPool, TResourceReader& aReader )
{
	CSmlDataPropertyParam* self = new( ELeave ) CSmlDataPropertyParam;
	CleanupStack::PushL( self );
	
	TInt version( aReader.ReadInt8() );
	self->iField = CSmlDataField::NewLC( aStringPool, aReader );	// display_name, name, data_type, enum_values	
	CleanupStack::Pop( self->iField ); 
	
	return self;
}

// ------------------------------------------------------------------------------------------------
// CSmlDataPropertyParam::NewLC()
// Class constructor from stream.
// ------------------------------------------------------------------------------------------------
EXPORT_C CSmlDataPropertyParam* CSmlDataPropertyParam::NewLC( const RStringPool& aStringPool, RReadStream& aStream )
{
	CSmlDataPropertyParam* self = new( ELeave ) CSmlDataPropertyParam;
	CleanupStack::PushL( self );
	
	TInt version( aStream.ReadInt8L() );
	
	self->iField = CSmlDataField::NewLC( aStringPool, aStream );	// display_name, name, data_type, enum_values	
	CleanupStack::Pop( self->iField ); 
	
	return self;
}

// ------------------------------------------------------------------------------------------------
// CSmlDataPropertyParam::~CSmlDataPropertyParam()
// Destructor.
// ------------------------------------------------------------------------------------------------
EXPORT_C CSmlDataPropertyParam::~CSmlDataPropertyParam()
{
	delete iField;
}

// ------------------------------------------------------------------------------------------------
// CSmlDataPropertyParam::ExternalizeL()
// Puts the class contents to the write stream
// ------------------------------------------------------------------------------------------------
EXPORT_C void CSmlDataPropertyParam::ExternalizeL( RWriteStream& aStream ) const
	{
	aStream.WriteInt8L( KVersionValue ); // version
	iField->ExternalizeL( aStream );
	}

// ------------------------------------------------------------------------------------------------
// CSmlDataPropertyParam::Field()
// Returns field.
// ------------------------------------------------------------------------------------------------
EXPORT_C const CSmlDataField& CSmlDataPropertyParam::Field() const
{
	return *iField;
}

// ------------------------------------------------------------------------------------------------
// CSmlDataPropertyParam::SetDataFieldL()
// Sets data field
// ------------------------------------------------------------------------------------------------
EXPORT_C void CSmlDataPropertyParam::SetDataFieldL( CSmlDataField& aDataField )
{
	delete iField;
	iField = NULL;
	iField = &aDataField;
}

// End of file
