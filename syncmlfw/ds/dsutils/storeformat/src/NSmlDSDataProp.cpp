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
Data property class
Property.
A field in the Data Store.
*/

// INCLUDE FILES
#include <SmlDataFormat.h>
#include <SmlDataFormat.hrh>
#include <stringpool.h>

// CONSTANTS
const TInt KVersionValue = 1;

// ===================================== MEMBER FUNCTIONS ========================================

// ------------------------------------------------------------------------------------------------
// CSmlDataProperty::NewLC()
// Class constructor.
// ------------------------------------------------------------------------------------------------
EXPORT_C CSmlDataProperty* CSmlDataProperty::NewLC()
{
	CSmlDataProperty* self = new (ELeave) CSmlDataProperty();
	CleanupStack::PushL( self );
	return( self );
}

// ------------------------------------------------------------------------------------------------
// CSmlDataProperty::NewLC()
// Class constructor from resource
// ------------------------------------------------------------------------------------------------
EXPORT_C CSmlDataProperty* CSmlDataProperty::NewLC( const RStringPool& aStringPool, TResourceReader& aReader )
{
	CSmlDataProperty* self = new( ELeave ) CSmlDataProperty;
	CleanupStack::PushL( self );

	TInt version = aReader.ReadInt8();					// BYTE version	
	
	self->iField = CSmlDataField::NewLC( aStringPool, aReader );	// display_name, name, data_type, enum_values
	CleanupStack::Pop(); // self->iField

	self->iFlags = aReader.ReadUint32();				// LONG flags
	self->iMaxSize = aReader.ReadUint16();				// WORD max_size
	self->iMaxOccur = aReader.ReadUint16();				// max_occur

	// The first WORD contains the number of DATA structs within the resource
	TInt number = aReader.ReadInt16();		

	for ( TInt index = 0; index < number ; index++ )
		{
		CSmlDataPropertyParam *temppoint = CSmlDataPropertyParam::NewLC( aStringPool, aReader );	// STRUCT params[] (SML_DATA_PROPERTY)
		self->iParams.AppendL( temppoint );
		CleanupStack::Pop( temppoint ); // temppoint
		}

	return self;
}

// ------------------------------------------------------------------------------------------------
// CSmlDataProperty::NewLC()
// Class constructor from stream
// ------------------------------------------------------------------------------------------------
EXPORT_C CSmlDataProperty* CSmlDataProperty::NewLC( const RStringPool& aStringPool, RReadStream& aStream )
{
	CSmlDataProperty* self = new( ELeave ) CSmlDataProperty;
	CleanupStack::PushL( self );
	
	TInt version( aStream.ReadInt8L() );
	self->iMaxSize = aStream.ReadInt16L();
	self->iMaxOccur = aStream.ReadInt16L();
	self->iFlags = aStream.ReadInt32L();
	
	self->iField = CSmlDataField::NewLC( aStringPool, aStream );	// display_name, name, data_type, enum_values
	CleanupStack::Pop( self->iField );
	
	TInt number( aStream.ReadInt16L() ); // The first WORD contains the number of DATA structs within the resource

	for (TInt index = 0; index < number ; index++)
		{
		CSmlDataPropertyParam *temppoint = CSmlDataPropertyParam::NewLC( aStringPool, aStream );	// STRUCT params[] (SML_DATA_PROPERTY)
		self->iParams.AppendL( temppoint );
		CleanupStack::Pop( temppoint ); // temppoint
		}
	
	return self;	
}

// ------------------------------------------------------------------------------------------------
// CSmlDataProperty::~CSmlDataProperty()
// Destructor
// ------------------------------------------------------------------------------------------------
EXPORT_C CSmlDataProperty::~CSmlDataProperty()
{
	delete iField;
	iParams.ResetAndDestroy();
}

// ------------------------------------------------------------------------------------------------
// CSmlDataProperty::ExternalizeL()
// Externalize class to stream.
// ------------------------------------------------------------------------------------------------
EXPORT_C void CSmlDataProperty::ExternalizeL( RWriteStream& aStream ) const
{
	aStream.WriteInt8L( KVersionValue ); // version
	aStream.WriteInt16L( iMaxSize );
	aStream.WriteInt16L( iMaxOccur );	
	aStream.WriteInt32L( iFlags );
	
	iField->ExternalizeL( aStream);
		
	aStream.WriteInt16L( iParams.Count() );
	for ( TInt index= 0; index < iParams.Count() ; index++ )
		{
		iParams[index]->ExternalizeL( aStream );
		}
}

// ------------------------------------------------------------------------------------------------
// CSmlDataProperty::Field()
// Returns field.
// ------------------------------------------------------------------------------------------------
EXPORT_C const CSmlDataField& CSmlDataProperty::Field() const
{
	return *iField;
}

// ------------------------------------------------------------------------------------------------
// CSmlDataProperty::SetDataFieldL()
// Sets field.
// ------------------------------------------------------------------------------------------------
EXPORT_C void CSmlDataProperty::SetDataFieldL( CSmlDataField& aDataField )
{
	delete iField;
	iField = NULL;
	iField = &aDataField;
}

// ------------------------------------------------------------------------------------------------
// CSmlDataProperty::ParamCount()
// Returns the number of parameters.
// ------------------------------------------------------------------------------------------------
EXPORT_C TInt CSmlDataProperty::ParamCount() const
{
	return iParams.Count();	
}

// ------------------------------------------------------------------------------------------------
// CSmlDataProperty::Param()
// Returns the indexed parameter.
// ------------------------------------------------------------------------------------------------
EXPORT_C const CSmlDataPropertyParam& CSmlDataProperty::Param( TInt aIndex ) const
{
	return *iParams[aIndex];
}

// ------------------------------------------------------------------------------------------------
// CSmlDataProperty::SetPropertyParamsL()
// Sets property parameters
// ------------------------------------------------------------------------------------------------
EXPORT_C void CSmlDataProperty::SetPropertyParamsL( RPointerArray<CSmlDataPropertyParam>& aParams )
{
	iParams.ResetAndDestroy();
	for ( TInt i = 0; i < aParams.Count(); i++ )
		{
		iParams.AppendL( aParams[i] );
		}
	aParams.Reset();
	aParams.Close();
}

// ------------------------------------------------------------------------------------------------
// CSmlDataProperty::IsSupported()
// Check is the option supported.
// ------------------------------------------------------------------------------------------------
EXPORT_C TBool CSmlDataProperty::IsSupported( TOption aOption ) const
{
	switch ( aOption )
		{
		case EOptionHasMaxSize:
			{
			if( KSmlDataProperty_HasMaxSize & iFlags  )
				{
				return ETrue;
				}

			break;
			}
		case EOptionHasMaxOccur:
			{
			if( KSmlDataProperty_HasMaxOccur & iFlags )
				{
				return ETrue;
				}

			break;
			}
		case EOptionNoTruncate:
			{
			if( KSmlDataProperty_NoTruncate & iFlags )
				{
				return ETrue;
				}

			break;
			}
		default:
			break;					
		}
		
	return EFalse;
}

// ------------------------------------------------------------------------------------------------
// CSmlDataProperty::SetSupportedOptions()
// Sets supported flags
// ------------------------------------------------------------------------------------------------
EXPORT_C void CSmlDataProperty::SetSupportedOptions( TUint32 aFlags )
{
	iFlags = aFlags;
}

// ------------------------------------------------------------------------------------------------
// CSmlDataProperty::HasMaxSize()
// Returns information is max size used
// ------------------------------------------------------------------------------------------------
EXPORT_C TBool CSmlDataProperty::HasMaxSize() const
{
	if ( iMaxSize != 0 )
		{
		return ETrue;
		}
		
	return EFalse;
}
// ------------------------------------------------------------------------------------------------
// CSmlDataProperty::HasMaxOccur()
// Returns information is max occur used
// ------------------------------------------------------------------------------------------------
EXPORT_C TBool CSmlDataProperty::HasMaxOccur() const
{
	if ( iMaxOccur != 0 )
		{
		return ETrue;
		}
		
	return EFalse;
}
// ------------------------------------------------------------------------------------------------
// CSmlDataProperty::MaxSize()
// Returns max size.
// ------------------------------------------------------------------------------------------------
EXPORT_C TInt CSmlDataProperty::MaxSize() const
{
	return iMaxSize;
}

// ------------------------------------------------------------------------------------------------
// CSmlDataProperty::MaxOccur()
// Returns max occur
// ------------------------------------------------------------------------------------------------
EXPORT_C TInt CSmlDataProperty::MaxOccur() const
{
	return iMaxOccur;
}

// ------------------------------------------------------------------------------------------------
// CSmlDataProperty::SetMaxSize()
// Sets max size
// ------------------------------------------------------------------------------------------------
EXPORT_C void CSmlDataProperty::SetMaxSize( TUint16 aMaxSize )
{
	iMaxSize = aMaxSize;
}

// ------------------------------------------------------------------------------------------------
// CSmlDataProperty::SetMaxOccur()
// Sets max occurrence
// ------------------------------------------------------------------------------------------------
EXPORT_C void CSmlDataProperty::SetMaxOccur( TUint16 aMaxOccur )
{
	iMaxOccur = aMaxOccur;
}

// End of file
