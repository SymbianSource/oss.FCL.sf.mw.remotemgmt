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
Data field class
Holds data defining a property value or property parameter value.
This includes Name, display name, and data type - including any enumerations.
*/

// INCLUDE FILES
#include <SmlDataFormat.h>
#include <stringpool.h>

// ===================================== MEMBER FUNCTIONS ========================================

// ------------------------------------------------------------------------------------------------
// CSmlDataField::NewLC()
// Class constructor.
// ------------------------------------------------------------------------------------------------
EXPORT_C CSmlDataField* CSmlDataField::NewLC()
{
	CSmlDataField* self = new( ELeave ) CSmlDataField;
	CleanupStack::PushL( self );
	
	return self;
}
// ------------------------------------------------------------------------------------------------
// CSmlDataField::NewLC()
// Class constructor from resource
// ------------------------------------------------------------------------------------------------
EXPORT_C CSmlDataField* CSmlDataField::NewLC( const RStringPool& aStringPool, TResourceReader& aReader )
{
	CSmlDataField* self = new( ELeave ) CSmlDataField;
	CleanupStack::PushL( self );
	
	HBufC8* tmpName;
	HBufC8* tmpDatatype;	

	self->iDisplayName = aReader.ReadHBufCL(); 	// LTEXT display_name
	
	tmpName = aReader.ReadHBufC8L();				// LTEXT8 name
	if( tmpName )				
		{
		CleanupStack::PushL( tmpName );
		self->iName = aStringPool.OpenStringL( *tmpName );
		CleanupStack::PopAndDestroy( tmpName );		
		}
	else
		{
		tmpName = KNullDesC8().AllocLC();
		self->iName = aStringPool.OpenStringL( *tmpName );
		CleanupStack::PopAndDestroy( tmpName );		
		}
	
	tmpDatatype = aReader.ReadHBufC8L();			// LTEXT8 data_type
	if( tmpDatatype )				
		{
		CleanupStack::PushL( tmpDatatype );
		self->iDataType = aStringPool.OpenStringL( *tmpDatatype );
		CleanupStack::PopAndDestroy( tmpDatatype );		
		}
	else
		{
		tmpDatatype = KNullDesC8().AllocLC();
		self->iDataType = aStringPool.OpenStringL( *tmpDatatype );
		CleanupStack::PopAndDestroy();		
		}
	
	// The first WORD contains the number of DATA structs within the resource
	TInt number = aReader.ReadInt16();

	for ( TInt index = 0; index < number ; index++ )
		{
		// enum values as RStrings
		HBufC8* tmpEnum = aReader.ReadHBufC8L();
		CleanupStack::PushL( tmpEnum );
		RString tmprstr( aStringPool.OpenStringL( *tmpEnum ) );
		CleanupClosePushL( tmprstr );
		self->iEnumValues.AppendL( tmprstr );
		CleanupStack::Pop(); // tmprstr
		CleanupStack::PopAndDestroy( tmpEnum );
		}		

	return self;	

}
// ------------------------------------------------------------------------------------------------
// CSmlDataField::NewLC()
// Class constructor from stream
// ------------------------------------------------------------------------------------------------
EXPORT_C CSmlDataField* CSmlDataField::NewLC( const RStringPool& aStringPool, RReadStream& aStream )
{
	CSmlDataField* self = new( ELeave ) CSmlDataField;
	CleanupStack::PushL( self );

	TInt tmpLength = aStream.ReadUint32L();
	self->iDisplayName = HBufC::NewL( aStream, tmpLength );
		
	tmpLength = aStream.ReadUint32L();
	HBufC8* tmpName = HBufC8::NewLC( aStream, tmpLength );
	self->iName = aStringPool.OpenStringL( *tmpName );
	CleanupStack::PopAndDestroy( tmpName ); 
	
	tmpLength = aStream.ReadUint32L();
	HBufC8* tmpDatatype = HBufC8::NewLC( aStream, tmpLength );
	self->iDataType = aStringPool.OpenStringL( *tmpDatatype );
	CleanupStack::PopAndDestroy( tmpDatatype );
			
	TInt number = aStream.ReadInt16L();

	for ( TInt index = 0; index < number ; index++ )
		{
		// enum values as RStrings
		tmpLength = aStream.ReadUint32L();
		HBufC8* tmpEnumvalue = HBufC8::NewLC( aStream, tmpLength );
		RString tmprstr( aStringPool.OpenStringL( *tmpEnumvalue ) );
		CleanupClosePushL( tmprstr );
		self->iEnumValues.AppendL ( tmprstr );
		CleanupStack::Pop();
		CleanupStack::PopAndDestroy( tmpEnumvalue );
		}		
	
	return self;
}


// ------------------------------------------------------------------------------------------------
// CSmlDataField::~CSmlDataField()
// Destructor
// ------------------------------------------------------------------------------------------------
EXPORT_C CSmlDataField::~CSmlDataField()
{
	iName.Close();
	iDataType.Close();
	delete iDisplayName;
	
	for( TInt i = 0; i < iEnumValues.Count(); i++)
		{
		iEnumValues[i].Close();
		}
	iEnumValues.Reset();
}
// ------------------------------------------------------------------------------------------------
// CSmlDataField::ExternalizeL()
// Puts the class contents to the write stream
// ------------------------------------------------------------------------------------------------
EXPORT_C void CSmlDataField::ExternalizeL(RWriteStream& aStream) const
	{

	if ( iDisplayName )
		{
		aStream.WriteUint32L( iDisplayName->Length() );
		aStream << *iDisplayName;
		}
	else
		{
		aStream.WriteInt32L( 0 );
		aStream << KNullDesC;
		}
	
	aStream.WriteUint32L( iName.DesC().Length() );
	aStream << iName.DesC();	
	
	aStream.WriteUint32L( iDataType.DesC().Length() );
	aStream << iDataType.DesC();	
	aStream.WriteInt16L( iEnumValues.Count() );	
	
	for ( TInt index = 0; index < iEnumValues.Count() ; index++ )	
		{		
		aStream.WriteUint32L( iEnumValues[index].DesC().Length() );
		aStream << iEnumValues[index].DesC();
		}
	}
// ------------------------------------------------------------------------------------------------
// CSmlDataField::Name()
// Returns name
// ------------------------------------------------------------------------------------------------
EXPORT_C RString CSmlDataField::Name() const
{
	return iName;
}

// ------------------------------------------------------------------------------------------------
// CSmlDataField::SetNameL()
// Sets name for data field
// ------------------------------------------------------------------------------------------------
EXPORT_C void CSmlDataField::SetNameL( RString& aName )
{
	iName.Close();
	iName = aName.Copy();
	aName.Close();
}

// ------------------------------------------------------------------------------------------------
// CSmlDataField::DisplayName()
// Returns displayname
// ------------------------------------------------------------------------------------------------
EXPORT_C const TDesC& CSmlDataField::DisplayName() const
{
	return *iDisplayName;
}

// ------------------------------------------------------------------------------------------------
// CSmlDataField::SetDisplayNameL()
// Sets display name for data type
// ------------------------------------------------------------------------------------------------
EXPORT_C void CSmlDataField::SetDisplayNameL( TDesC& aDisplayName )
{
	delete iDisplayName;
	iDisplayName = NULL;
	iDisplayName = aDisplayName.AllocL();
}

// ------------------------------------------------------------------------------------------------
// CSmlDataField::DataType()
// Returns data type.
// ------------------------------------------------------------------------------------------------
EXPORT_C RString CSmlDataField::DataType() const
{
	return iDataType;
}

// ------------------------------------------------------------------------------------------------
// CSmlDataField::SetDataTypeL()
// Sets data type for data field
// ------------------------------------------------------------------------------------------------
EXPORT_C void CSmlDataField::SetDataTypeL( RString& aDataType )
{
	iDataType.Close();
	iDataType = aDataType.Copy();
	aDataType.Close();
}

// ------------------------------------------------------------------------------------------------
// CSmlDataField::EnumValueCount()
// Returns number of enumeration values.
// ------------------------------------------------------------------------------------------------
EXPORT_C TInt CSmlDataField::EnumValueCount() const
{
	return iEnumValues.Count();
}

// ------------------------------------------------------------------------------------------------
// CSmlDataField::EnumValue()
// Returns the indexed enumeration.
// ------------------------------------------------------------------------------------------------
EXPORT_C RString CSmlDataField::EnumValue( TInt aIndex ) const
{
	return iEnumValues[aIndex];
}

// ------------------------------------------------------------------------------------------------
// CSmlDataField::SetEnumValuesL()
// Sets enumeration values
// ------------------------------------------------------------------------------------------------
EXPORT_C void CSmlDataField::SetEnumValuesL( RArray<RString>& aEnumValues )
{
	for ( TInt i = 0; i < iEnumValues.Count(); i++ )
		{
		iEnumValues[i].Close();
		}
	iEnumValues.Reset();
	iEnumValues.Close();
	for ( TInt j = 0; j < aEnumValues.Count(); j++ )
		{
		iEnumValues.AppendL( aEnumValues[j].Copy() );
		}
	for ( TInt k = 0; k < aEnumValues.Count(); k++ )
		{
		aEnumValues[k].Close();
		}
	aEnumValues.Reset();
	aEnumValues.Close();
}

// End of file
