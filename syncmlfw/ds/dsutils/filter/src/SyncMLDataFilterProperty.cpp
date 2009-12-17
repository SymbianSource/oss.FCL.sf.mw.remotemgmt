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
* Description:  Data Filter library
*
*/



// INCLUDES
#include <e32base.h>
#include <s32strm.h>
#include <barsread.h>
#include <stringpool.h>
#include <SyncMLDataFilter.h>
#include <SyncMLDataFilter.hrh>


// CONSTANTS
const TInt KSizeOfTInt64 = sizeof(TReal64);
const TInt KSizeofTInt32 = sizeof(TInt32);
const TInt KSizeofTInt8 = sizeof(TInt8);
const TInt KMaxTextLength = 256;


// ============================ MEMBER FUNCTIONS ===============================
// -----------------------------------------------------------------------------
// CSyncMLFilterProperty::NewLC
// Symbian two-phased constructor.
// -----------------------------------------------------------------------------
EXPORT_C CSyncMLFilterProperty* CSyncMLFilterProperty::NewLC()
	{
	CSyncMLFilterProperty* self = new (ELeave) CSyncMLFilterProperty();
	CleanupStack::PushL( self );
	
	self->iDisplayName = KNullDesC().AllocL();
	self->iQueryValueText16 = NULL;
	self->iQueryValueText8 = NULL;
		
	return self;
	}
	
// -----------------------------------------------------------------------------
// CSyncMLFilterProperty::NewLC
// Symbian two-phased constructor.
// -----------------------------------------------------------------------------
EXPORT_C CSyncMLFilterProperty*
                CSyncMLFilterProperty::NewLC( TResourceReader& aReader )
	{
	CSyncMLFilterProperty* self = new (ELeave) CSyncMLFilterProperty();
	CleanupStack::PushL( self );
	
	// Create filter property from resource file
	// BYTE version
	TInt version( aReader.ReadInt8() );
	
	// LTEXT name
	self->iDisplayName = aReader.ReadHBufC16L();

    // LONG data type for default values
	TInt32 tmpDataTypeForDefaultValues = aReader.ReadInt32();
	switch( tmpDataTypeForDefaultValues )
		{
		case KSyncMLFilterDataTypeNumber:
			{
			self->iDataTypeForDefaultValues = ESyncMLDataTypeNumber;
			break;
			}
		case KSyncMLFilterDataTypeBoolean:
			{
			self->iDataTypeForDefaultValues = ESyncMLDataTypeBool;
			break;
			}
		case KSyncMLFilterDataTypeDate:
			{
			self->iDataTypeForDefaultValues = ESyncMLDataTypeDate;
			break;
			}
		case KSyncMLFilterDataTypeTime:
			{
			self->iDataTypeForDefaultValues = ESyncMLDataTypeTime;
			break;
			}			
		case KSyncMLFilterDataTypeText8:
			{
			self->iDataTypeForDefaultValues = ESyncMLDataTypeText8;
			break;
			}
		case KSyncMLFilterDataTypeText16:
			{
			self->iDataTypeForDefaultValues = ESyncMLDataTypeText16;
			break;
			}
		default:
			{
			self->iDataTypeForDefaultValues = ESyncMLDataTypeNull;
			break;
			}
		}
	
	// LONG data type for query value
	TInt32 tmpDataTypeForQueryValue = aReader.ReadInt32();
	switch( tmpDataTypeForQueryValue )
		{
		case KSyncMLFilterDataTypeNumber:
			{
			self->iDataTypeForQueryValue = ESyncMLDataTypeNumber;
			break;
			}
		case KSyncMLFilterDataTypeBoolean:
			{
			self->iDataTypeForQueryValue = ESyncMLDataTypeBool;
			break;
			}
		case KSyncMLFilterDataTypeDate:
			{
			self->iDataTypeForQueryValue = ESyncMLDataTypeDate;
			break;
			}
		case KSyncMLFilterDataTypeTime:
			{
			self->iDataTypeForQueryValue = ESyncMLDataTypeTime;
			break;
			}			
		case KSyncMLFilterDataTypeText8:
			{
			self->iDataTypeForQueryValue = ESyncMLDataTypeText8;
			break;
			}
		case KSyncMLFilterDataTypeText16:
			{
			self->iDataTypeForQueryValue = ESyncMLDataTypeText16;
			break;
			}
		default:
			{
			self->iDataTypeForQueryValue = ESyncMLDataTypeNull;
			break;
			}
		}
	
	// WORD max text length
	self->iMaxTextLength = aReader.ReadInt16();

    // LTEXT[] default text16 values
	TInt number = aReader.ReadInt16();
	for ( TInt i = 0; i < number; i++ )
		{
		HBufC16* text = aReader.ReadHBufC16L();
		CleanupStack::PushL( text );
		self->iDefaultValueText16List.AppendL( text );
		CleanupStack::Pop(); // text
		}
	
	// LTEXT[] default text8 values
	number = aReader.ReadInt16();
	for ( TInt i = 0; i < number; i++ )
		{
		HBufC8* text = aReader.ReadHBufC8L();
		CleanupStack::PushL( text );
		self->iDefaultValueText8List.AppendL( text );
		CleanupStack::Pop(); // text
		}
	
	// WORD[] default integer/boolean values
	number = aReader.ReadInt16();
	for ( TInt i = 0; i < number; i++ )
		{
		self->iDefaultValueIntOrBoolList.AppendL( aReader.ReadInt16() );
		}
	
	// LTEXT default time values
	number = aReader.ReadInt16();
	for ( TInt i = 0; i < number; i++ )
		{
		HBufC* timeStr = aReader.ReadHBufCL();
		CleanupStack::PushL( timeStr );
		TTime* temp = new (ELeave) TTime( *timeStr );
		CleanupStack::PopAndDestroy(); //timeStr
		CleanupDeletePushL( temp );
		TDateTime dt( temp->DateTime() );
		
		switch( self->iDataTypeForDefaultValues )
			{
			case ESyncMLDataTypeDate:
				dt.SetHour(0);
				dt.SetMinute(0);
				dt.SetSecond(0);
				dt.SetMicroSecond(0);
				break;
			case ESyncMLDataTypeTime:
				dt.SetDay(0);
				dt.SetMonth(EJanuary);
				dt.SetYear(0);
				break;
			default:
				break;
			}
		*temp = dt;

		self->iDefaultValueDateOrTimeList.AppendL( temp );
		CleanupStack::Pop(); // temp
		}
	
	// BYTE[] default value selection
	number = aReader.ReadInt16();
	for ( TInt i = 0; i < number; i++ )
		{
		self->iDefaultValueSelected.AppendL( aReader.ReadInt8() );
		}
	
	// LTEXT query text16
	self->iQueryValueText16 = aReader.ReadHBufC16L();
	
	// LTEXT query text8
	self->iQueryValueText8 = aReader.ReadHBufC8L();
	
	// WORD query integer/boolean
	self->iQueryValueIntOrBool = aReader.ReadInt16();
	
	// LTEXT query time
	HBufC* timeStr = aReader.ReadHBufCL();
	
	if ( timeStr )
		{
		CleanupStack::PushL( timeStr );
		
		self->iQueryValueDateOrTime.Set( *timeStr );
		TDateTime dt( self->iQueryValueDateOrTime.DateTime() );
		
		switch( self->iDataTypeForQueryValue )
			{
			case ESyncMLDataTypeDate:
				dt.SetHour(0);
				dt.SetMinute(0);
				dt.SetSecond(0);
				dt.SetMicroSecond(0);
				break;
			case ESyncMLDataTypeTime:
				dt.SetDay(0);
				dt.SetMonth(EJanuary);
				dt.SetYear(0);
				break;
			default:
				break;
			}
		self->iQueryValueDateOrTime = dt;
		CleanupStack::PopAndDestroy(); //timeStr
		}

    // BYTE query value selected
	if ( aReader.ReadInt8() > 0 )
		{
		self->iQueryValueSelected = ETrue;
		}
	
	// BYTE can use query
	if ( aReader.ReadInt8() > 0 )
		{
		self->iCanUseQueryValue = ETrue;
		}
	
	// BYTE multiple selection
	if ( aReader.ReadInt8() > 0 )
		{
		self->iSupportsMultpleSelection = ETrue;
		}
		
	// BYTE empty selection
	if ( aReader.ReadInt8() > 0 )
		{
		self->iSupportsEmptySelection = ETrue;
		}

	return self;
	}

// -----------------------------------------------------------------------------
// CSyncMLFilterProperty::NewLC
// Symbian two-phased constructor.
// -----------------------------------------------------------------------------
EXPORT_C CSyncMLFilterProperty*
                CSyncMLFilterProperty::NewLC( RReadStream& aStream )
	{
	CSyncMLFilterProperty* self = new (ELeave) CSyncMLFilterProperty();
	CleanupStack::PushL( self );
	
	self->iQueryValueText16 = NULL;
	self->iQueryValueText8 = NULL;
	
	// Create filter from stream
	//  name
	TInt tempLength( aStream.ReadUint32L() );
	if ( tempLength )
		{
		// name
		self->iDisplayName = HBufC::NewL( aStream, tempLength );
		}
	
	// data type for default values
	self->iDataTypeForDefaultValues = 
	                ( TSyncMLFilterPropertyDataType )aStream.ReadInt8L();
	
	// data type for query value
	self->iDataTypeForQueryValue = 
	                ( TSyncMLFilterPropertyDataType )aStream.ReadInt8L();

    // max text length
	self->iMaxTextLength = aStream.ReadUint32L();
	
	// default values
	switch( self->iDataTypeForDefaultValues )
		{
		case ESyncMLDataTypeBool:
		case ESyncMLDataTypeNumber:
			{
			TInt count = aStream.ReadInt32L();
			for ( TInt i = 0; i < count; i++ )
				{
				self->iDefaultValueIntOrBoolList.AppendL( 
				                aStream.ReadInt32L() );
				}
			break;
			}
		case ESyncMLDataTypeTime:
		case ESyncMLDataTypeDate:
			{
			TInt count = aStream.ReadInt32L();
			for ( TInt i = 0; i < count; i++ )
				{
				TTime* temp = new ( ELeave ) TTime( aStream.ReadReal64L() );
				CleanupDeletePushL( temp );
				self->iDefaultValueDateOrTimeList.AppendL( temp );
				CleanupStack::Pop(); // temp
				}
			break;
			}
			
		case ESyncMLDataTypeText8:
			{
			TInt count = aStream.ReadInt32L();
			for ( TInt i = 0; i < count; i++ )
				{
				TInt length( aStream.ReadUint32L() );
				if ( length )
					{
					self->iDefaultValueText8List.AppendL( 
					                HBufC8::NewLC( aStream, length ) );
					CleanupStack::Pop(); // text
					}
				}
			break;
			}
		case ESyncMLDataTypeText16:
			{
			TInt count = aStream.ReadInt32L();
			for ( TInt i = 0; i < count; i++ )
				{
				TInt length( aStream.ReadUint32L() );
				if ( length )
					{
					self->iDefaultValueText16List.AppendL( 
					                HBufC::NewLC( aStream, length ) );
					CleanupStack::Pop(); // text
					}
				}
			break;
			}
		case ESyncMLDataTypeNull:
			{
			aStream.ReadInt32L();
			break;
			}
		default:
			{
			break;
			}
		}
		
	// query values
	switch( self->iDataTypeForQueryValue )
		{
		case ESyncMLDataTypeBool:
		case ESyncMLDataTypeNumber:
			{
			self->iQueryValueIntOrBool = aStream.ReadInt32L();
			break;
			}
		case ESyncMLDataTypeTime:	
		case ESyncMLDataTypeDate:
			{	
			TTime tempTime( aStream.ReadReal64L() );
			self->iQueryValueDateOrTime = tempTime;
			break;
			}
			
		case ESyncMLDataTypeText8:
			{
			TInt length( aStream.ReadUint32L() );
			if (length)
				{
				self->iQueryValueText8 = HBufC8::NewL( aStream, length );
				}
			break;
			}
		case ESyncMLDataTypeText16:
			{
			TInt length( aStream.ReadUint32L() );
			if (length)
				{
				self->iQueryValueText16 = HBufC::NewL( aStream, length );
				}
				
			break;
			}
		case ESyncMLDataTypeNull:
			{
			aStream.ReadInt32L();
			break;
			}
		default:
			{
			break;
			}
		}

    // default value selection
	tempLength = aStream.ReadInt32L();
	for ( TInt i = 0; i < tempLength; i++ )
		{
		self->iDefaultValueSelected.AppendL( aStream.ReadInt32L() );
		}

    // can use query
	if ( aStream.ReadInt8L() > 0 )
		{
		self->iCanUseQueryValue = ETrue;
		}
	
	// supports multiple selection
	if ( aStream.ReadInt8L() > 0 )
		{
		self->iSupportsMultpleSelection = ETrue;
		}
	
	// supports empty selection
	if ( aStream.ReadInt8L() > 0 )
		{
		self->iSupportsEmptySelection = ETrue;
		}
	
	// query value selected
	if ( aStream.ReadInt8L() > 0 )
		{
		self->iQueryValueSelected = ETrue;
		}

	return self;
	}

// -----------------------------------------------------------------------------
// CSyncMLFilterProperty::~CSyncMLFilterProperty
// Destructor.
// -----------------------------------------------------------------------------
EXPORT_C CSyncMLFilterProperty::~CSyncMLFilterProperty()
	{
	delete iDisplayName;
	
	iDefaultValueText16List.ResetAndDestroy();
	iDefaultValueText16List.Close();
	iDefaultValueText8List.ResetAndDestroy();
	iDefaultValueText8List.Close();
	iDefaultValueIntOrBoolList.Reset();
	iDefaultValueIntOrBoolList.Close();
	iDefaultValueDateOrTimeList.ResetAndDestroy();
	iDefaultValueDateOrTimeList.Close();
	
	iDefaultValueSelected.Reset();
	iDefaultValueSelected.Close();
	
	delete iQueryValueText16;
	delete iQueryValueText8;
	}

// -----------------------------------------------------------------------------
// CSyncMLFilterProperty::ExternalizeL
// Externalizes filter property.
// -----------------------------------------------------------------------------
EXPORT_C void CSyncMLFilterProperty::ExternalizeL( RWriteStream& aStream ) const
	{
	
	if ( iDisplayName )
		{
		aStream.WriteUint32L( iDisplayName->Length() );
		aStream << iDisplayName->Des();
		}
	else
		{
		aStream.WriteUint32L( 0 );	
		}
	
	// data type for default values
	aStream.WriteInt8L( ( TInt8 )iDataTypeForDefaultValues );
	
	// data type for query value
	aStream.WriteInt8L( ( TInt8 )iDataTypeForQueryValue );
	
	// max text length
	aStream.WriteUint32L( iMaxTextLength );
	
	// default values
	switch( iDataTypeForDefaultValues )
		{
		case ESyncMLDataTypeBool:
		case ESyncMLDataTypeNumber:
			{
			aStream.WriteInt32L( iDefaultValueIntOrBoolList.Count() );
			for ( TInt i = 0; i < iDefaultValueIntOrBoolList.Count(); i++ )
				{
				aStream.WriteInt32L( iDefaultValueIntOrBoolList[i] );
				}
			break;
			}
		case ESyncMLDataTypeTime:	
		case ESyncMLDataTypeDate:
			{
			aStream.WriteInt32L( iDefaultValueDateOrTimeList.Count() );
			for ( TInt i = 0; i < iDefaultValueDateOrTimeList.Count(); i++ )
				{
				aStream.WriteReal64L( 
				                ( iDefaultValueDateOrTimeList[i] )->Int64() );
				}
			break;
			}
			
		case ESyncMLDataTypeText8:
			{
			aStream.WriteInt32L( iDefaultValueText8List.Count() );
			for ( TInt i = 0; i < iDefaultValueText8List.Count(); i++ )
				{
				if ( iDefaultValueText8List[i] )
					{
					aStream.WriteUint32L( 
					                iDefaultValueText8List[i]->Length() );
					aStream << *iDefaultValueText8List[i];
					}
				else
					{
					aStream.WriteInt32L( 0 );
					}
				}
			break;
			}
			
		case ESyncMLDataTypeText16:
			{
			aStream.WriteInt32L( iDefaultValueText16List.Count() );
			for ( TInt i = 0; i < iDefaultValueText16List.Count(); i++ )
				{
				if ( iDefaultValueText16List[i] )
					{
					aStream.WriteUint32L( 
					                iDefaultValueText16List[i]->Length() );
					aStream << *iDefaultValueText16List[i];
					}
				else
					{
					aStream.WriteInt32L( 0 );
					}
				}
			break;
			}
		default:
			{
			aStream.WriteInt32L( 0 );
			break;
			}
		}
		
	// default values and query values
	switch( iDataTypeForQueryValue )
		{
		case ESyncMLDataTypeBool:
		case ESyncMLDataTypeNumber:
			{
			aStream.WriteInt32L( iQueryValueIntOrBool );
			break;
			}
		case ESyncMLDataTypeTime:	
		case ESyncMLDataTypeDate:
			{
			aStream.WriteReal64L( iQueryValueDateOrTime.Int64() );
			break;
			}
			
		case ESyncMLDataTypeText8:
			{
			if ( iQueryValueText8 )
				{
				aStream.WriteUint32L( iQueryValueText8->Length() );
				aStream << iQueryValueText8->Des();
				}	
			else
				{
				aStream.WriteUint32L( 0 );
				}	
			break;
			}
			
		case ESyncMLDataTypeText16:
			{
			if ( iQueryValueText16 )
				{
				aStream.WriteUint32L( iQueryValueText16->Length() );	
				aStream << iQueryValueText16->Des();
				}
			else
				{
				aStream.WriteUint32L( 0 );
				}			
			break;
			}
		default:
			{
			aStream.WriteInt32L( 0 );
			break;
			}
		}
	
	// default value selection
	aStream.WriteInt32L( iDefaultValueSelected.Count() );
	for ( TInt i = 0; i < iDefaultValueSelected.Count(); i++ )
		{
		aStream.WriteInt32L( iDefaultValueSelected[i] );
		}
		
	// can use query value
	aStream.WriteInt8L( iCanUseQueryValue );
	
	// supports multiple selection
	aStream.WriteInt8L( iSupportsMultpleSelection );
	
	// supports empty selection
	aStream.WriteInt8L( iSupportsEmptySelection );
	
	// query value selected
	aStream.WriteInt8L( iQueryValueSelected );
	
	}

// -----------------------------------------------------------------------------
// CSyncMLFilterProperty::Name
// Name of filter property.
// -----------------------------------------------------------------------------
EXPORT_C const TDesC& CSyncMLFilterProperty::DisplayName() const
	{
	return *iDisplayName;
	}
	
// -----------------------------------------------------------------------------
// CSyncMLFilterProperty::DefaultValueDataType
// Data type of default values for filter property.
// -----------------------------------------------------------------------------
EXPORT_C TSyncMLFilterPropertyDataType
                CSyncMLFilterProperty::DefaultValueDataType() const
	{
	return iDataTypeForDefaultValues;
	}
	
// -----------------------------------------------------------------------------
// CSyncMLFilterProperty::QueryValueDataType
// Data type of query value for filter property.
// -----------------------------------------------------------------------------
EXPORT_C TSyncMLFilterPropertyDataType
                CSyncMLFilterProperty::QueryValueDataType() const
	{
	return iDataTypeForQueryValue;
	}

// -----------------------------------------------------------------------------
// CSyncMLFilterProperty::MaxTextLength
// Maximum text length of filter property.
// This is useful only if data type is text16 or text8.
// -----------------------------------------------------------------------------
EXPORT_C TUint CSyncMLFilterProperty::MaxTextLength() const
	{
	return iMaxTextLength;
	}

// -----------------------------------------------------------------------------
// CSyncMLFilterProperty::DefaultValueCount
// Number of default values.
// -----------------------------------------------------------------------------
EXPORT_C TInt CSyncMLFilterProperty::DefaultValueCount() const
	{
	switch( iDataTypeForDefaultValues )
		{
		case ESyncMLDataTypeBool:
		case ESyncMLDataTypeNumber:
			{
			return iDefaultValueIntOrBoolList.Count();
			}
		case ESyncMLDataTypeTime:
		case ESyncMLDataTypeDate:
			{
			return iDefaultValueDateOrTimeList.Count();
			}
		case ESyncMLDataTypeText8:
			{
			return iDefaultValueText8List.Count();
			}
		case ESyncMLDataTypeText16:
			{
			return iDefaultValueText16List.Count();
			}
		default:
			{
			return 0;
			}
		}
	}
	
// -----------------------------------------------------------------------------
// CSyncMLFilterProperty::DefaultValueText16L
// Default value from given index for text16 property.
// This is useful only if data type is text16.
// -----------------------------------------------------------------------------
EXPORT_C const TDesC16&
                CSyncMLFilterProperty::DefaultValueText16L( TInt aIndex ) const
	{
	if ( iDataTypeForDefaultValues != ESyncMLDataTypeText16 )
		{
		User::Leave( KErrNotSupported );
		}
	if ( iDefaultValueText16List.Count() < aIndex || aIndex < 0 )
		{
		User::Leave( KErrArgument );
		}
	return *( iDefaultValueText16List[ aIndex ] );
	}

// -----------------------------------------------------------------------------
// CSyncMLFilterProperty::DefaultValueText8L
// Default value from given index for text8 property.
// This is useful only if data type is text8.
// -----------------------------------------------------------------------------
EXPORT_C const TDesC8&
                CSyncMLFilterProperty::DefaultValueText8L( TInt aIndex ) const
	{
	if ( iDataTypeForDefaultValues != ESyncMLDataTypeText8 )
		{
		User::Leave( KErrNotSupported );
		}
	if ( iDefaultValueText8List.Count() < aIndex || aIndex < 0 )
		{
		User::Leave( KErrArgument );
		}
	return *( iDefaultValueText8List[ aIndex ] );
	}
	
// -----------------------------------------------------------------------------
// CSyncMLFilterProperty::DefaultValueIntL
// Default value from given index for number property.
// This is useful only if data type is number.
// -----------------------------------------------------------------------------
EXPORT_C TInt CSyncMLFilterProperty::DefaultValueIntL( TInt aIndex ) const
	{
	if ( iDataTypeForDefaultValues != ESyncMLDataTypeNumber )
		{
		User::Leave( KErrNotSupported );
		}
	if ( iDefaultValueIntOrBoolList.Count() < aIndex || aIndex < 0 )
		{
		User::Leave( KErrArgument );
		}
	return iDefaultValueIntOrBoolList[ aIndex ];
	}
	
// -----------------------------------------------------------------------------
// CSyncMLFilterProperty::DefaultValueBoolL
// Default value from given index for boolean property.
// This is useful only if data type is boolean.
// -----------------------------------------------------------------------------
EXPORT_C TBool CSyncMLFilterProperty::DefaultValueBoolL( TInt aIndex ) const
	{
	if ( iDataTypeForDefaultValues != ESyncMLDataTypeBool )
		{
		User::Leave( KErrNotSupported );
		}
	if ( iDefaultValueIntOrBoolList.Count() < aIndex || aIndex < 0 )
		{
		User::Leave( KErrArgument );
		}
	return iDefaultValueIntOrBoolList[ aIndex ];
	}
	
// -----------------------------------------------------------------------------
// CSyncMLFilterProperty::DefaultValueDateL
// Default value from given index for date property.
// This is useful only if data type is date.
// -----------------------------------------------------------------------------
EXPORT_C const TTime& 
                CSyncMLFilterProperty::DefaultValueDateL( TInt aIndex ) const
	{
	if ( iDataTypeForDefaultValues != ESyncMLDataTypeDate )
		{
		User::Leave( KErrNotSupported );
		}
	if ( iDefaultValueDateOrTimeList.Count() < aIndex || aIndex < 0 )
		{
		User::Leave( KErrArgument );
		}
	return *( iDefaultValueDateOrTimeList[ aIndex ] );
	}

// -----------------------------------------------------------------------------
// CSyncMLFilterProperty::DefaultValueTimeL
// Default value from given index for time property.
// This is useful only if data type is time.
// -----------------------------------------------------------------------------
EXPORT_C const TTime&
                CSyncMLFilterProperty::DefaultValueTimeL( TInt aIndex ) const
	{
	if ( iDataTypeForDefaultValues != ESyncMLDataTypeTime )
		{
		User::Leave( KErrNotSupported );
		}
	if ( iDefaultValueDateOrTimeList.Count() < aIndex || aIndex < 0 )
		{
		User::Leave( KErrArgument );
		}
	return *( iDefaultValueDateOrTimeList[ aIndex ] );
	}
	

// -----------------------------------------------------------------------------
// CSyncMLFilterProperty::ClearDefaultValuesL
// Clears all default values and value selections.
// -----------------------------------------------------------------------------
EXPORT_C void CSyncMLFilterProperty::ClearDefaultValuesL()
	{
	iDefaultValueText16List.ResetAndDestroy();
	iDefaultValueText8List.ResetAndDestroy();
	iDefaultValueIntOrBoolList.Reset();
	iDefaultValueDateOrTimeList.ResetAndDestroy();
	}
	
// -----------------------------------------------------------------------------
// CSyncMLFilterProperty::AddDefaultValueText16L
// Adds default value for text16 property.
// This is useful only if data type is text16.
// -----------------------------------------------------------------------------
EXPORT_C void CSyncMLFilterProperty::AddDefaultValueText16L( 
                TDesC16& aNewDefaultValue, const TBool aSelected )
	{
	if ( iDataTypeForDefaultValues != ESyncMLDataTypeText16 )
		{
		User::Leave( KErrNotSupported );
		}
	if ( aNewDefaultValue.Length() > iMaxTextLength )
		{
		User::Leave( KErrArgument );
		}
	iDefaultValueText16List.AppendL( aNewDefaultValue.AllocLC() );
	CleanupStack::Pop(); // aNewDefaultValue.AllocLC()
	iDefaultValueSelected.AppendL( aSelected );
	}
	
// -----------------------------------------------------------------------------
// CSyncMLFilterProperty::AddDefaultValueText8L
// Adds default value for text8 property.
// This is useful only if data type is text8.
// -----------------------------------------------------------------------------
EXPORT_C void CSyncMLFilterProperty::AddDefaultValueText8L(
                TDesC8& aNewDefaultValue, const TBool aSelected )
	{
	if ( iDataTypeForDefaultValues != ESyncMLDataTypeText8 )
		{
		User::Leave( KErrNotSupported );
		}
	if ( aNewDefaultValue.Length() > iMaxTextLength )
		{
		User::Leave( KErrArgument );
		}
	iDefaultValueText8List.AppendL( aNewDefaultValue.AllocLC() );
	CleanupStack::Pop(); // aNewDefaultValue.AllocLC()
	iDefaultValueSelected.AppendL( aSelected );
	}
	
// -----------------------------------------------------------------------------
// CSyncMLFilterProperty::AddDefaultValueIntL
// Adds default value for number property.
// This is useful only if data type is number.
// -----------------------------------------------------------------------------
EXPORT_C void CSyncMLFilterProperty::AddDefaultValueIntL( 
                const TInt aNewDefaultValue, const TBool aSelected )
	{
	if ( iDataTypeForDefaultValues != ESyncMLDataTypeNumber )
		{
		User::Leave( KErrNotSupported );
		}
	iDefaultValueIntOrBoolList.AppendL( aNewDefaultValue );
	iDefaultValueSelected.AppendL( aSelected );
	}
	
// -----------------------------------------------------------------------------
// CSyncMLFilterProperty::AddDefaultValueBoolL
// Adds default value for boolean property.
// This is useful only if data type is boolean.
// -----------------------------------------------------------------------------
EXPORT_C void CSyncMLFilterProperty::AddDefaultValueBoolL( 
                const TBool aNewDefaultValue, const TBool aSelected )
	{
	if ( iDataTypeForDefaultValues != ESyncMLDataTypeBool )
		{
		User::Leave( KErrNotSupported );
		}
	iDefaultValueIntOrBoolList.AppendL( aNewDefaultValue );
	iDefaultValueSelected.AppendL( aSelected );
	}
	
// -----------------------------------------------------------------------------
// CSyncMLFilterProperty::AddDefaultValueDateL
// Adds default value for date property.
// This is useful only if data type is date.
// -----------------------------------------------------------------------------
EXPORT_C void CSyncMLFilterProperty::AddDefaultValueDateL(
                const TTime& aNewDefaultValue, const TBool aSelected )
	{
	if ( iDataTypeForDefaultValues != ESyncMLDataTypeDate )
		{
		User::Leave( KErrNotSupported );
		}
		
	TDateTime date( aNewDefaultValue.DateTime() );
	date.SetHour(0);
	date.SetMinute(0);
	date.SetSecond(0);
	date.SetMicroSecond(0);
		
	TTime* temp = new (ELeave) TTime( date );
	CleanupDeletePushL( temp );
	iDefaultValueDateOrTimeList.AppendL( temp );
	CleanupStack::Pop(); // temp
	iDefaultValueSelected.AppendL( aSelected );
	}
	
// -----------------------------------------------------------------------------
// CSyncMLFilterProperty::AddDefaultValueTimeL
// Adds default value for time property.
// This is useful only if data type is time.
// -----------------------------------------------------------------------------
EXPORT_C void CSyncMLFilterProperty::AddDefaultValueTimeL(
                const TTime& aNewDefaultValue, const TBool aSelected )
	{
	if ( iDataTypeForDefaultValues != ESyncMLDataTypeTime )
		{
		User::Leave( KErrNotSupported );
		}
	
	TDateTime time( aNewDefaultValue.DateTime() );
	time.SetDay(0);
	time.SetMonth(EJanuary);
	time.SetYear(0);

	TTime* temp = new (ELeave) TTime( time );
	CleanupDeletePushL( temp );
	iDefaultValueDateOrTimeList.AppendL( temp );
	CleanupStack::Pop(); // temp
	iDefaultValueSelected.AppendL( aSelected );
	}

// -----------------------------------------------------------------------------
// CSyncMLFilterProperty::CanUseQueryValue
// Can use query value with default values or not.
// -----------------------------------------------------------------------------
EXPORT_C TBool CSyncMLFilterProperty::CanUseQueryValue() const
	{
	return iCanUseQueryValue;
	}

// -----------------------------------------------------------------------------
// CSyncMLFilterProperty::SupportsMultipleSelection
// Does filter property support multiple selection.
// -----------------------------------------------------------------------------
EXPORT_C TBool CSyncMLFilterProperty::SupportsMultipleSelection() const
	{
	return iSupportsMultpleSelection;
	}
	
// -----------------------------------------------------------------------------
// CSyncMLFilterProperty::SupportsEmptySelection
// Does filter property support empty selection.
// -----------------------------------------------------------------------------
EXPORT_C TBool CSyncMLFilterProperty::SupportsEmptySelection() const
	{
	return iSupportsEmptySelection;
	}

// -----------------------------------------------------------------------------
// CSyncMLFilterProperty::IsDefaultValueSelected
// Returns information about which default values are selected.
// -----------------------------------------------------------------------------
EXPORT_C TBool 
              CSyncMLFilterProperty::IsDefaultValueSelected( TInt aIndex ) const
	{
	return iDefaultValueSelected[ aIndex ];
	}
	
// -----------------------------------------------------------------------------
// CSyncMLFilterProperty::IsQueryValueSelected
// Returns information about query value selection.
// This is useful only if query value can be used.
// -----------------------------------------------------------------------------
EXPORT_C TBool CSyncMLFilterProperty::IsQueryValueSelected() const
	{
	return iQueryValueSelected;
	}

// -----------------------------------------------------------------------------
// CSyncMLFilterProperty::SelectDefaultValueL
// Selects default value from given index.
// -----------------------------------------------------------------------------
EXPORT_C void CSyncMLFilterProperty::SelectDefaultValueL( TInt aIndex )
	{
	iDefaultValueSelected[aIndex] = ETrue;
	}
	
// -----------------------------------------------------------------------------
// CSyncMLFilterProperty::UnselectDefaultValueL
// Unselects default value from given index.
// -----------------------------------------------------------------------------
EXPORT_C void CSyncMLFilterProperty::UnselectDefaultValueL( TInt aIndex )
	{
	iDefaultValueSelected[aIndex] = EFalse;
	}
		
// -----------------------------------------------------------------------------
// CSyncMLFilterProperty::QueryValueText16L
// Returns query value for text16 property.
// This is useful only if data type is text16 and if query value can be used.
// -----------------------------------------------------------------------------
EXPORT_C const TDesC16& CSyncMLFilterProperty::QueryValueText16L() const
	{
	if ( iDataTypeForQueryValue != ESyncMLDataTypeText16 || !iCanUseQueryValue )
		{
		User::Leave( KErrNotSupported );
		}
	User::LeaveIfNull( iQueryValueText16 );
	
	return *iQueryValueText16;
	}
	
// -----------------------------------------------------------------------------
// CSyncMLFilterProperty::QueryValueText8L
// Returns query value for text8 property.
// This is useful only if data type is text8 and if query value can be used.
// -----------------------------------------------------------------------------
EXPORT_C const TDesC8& CSyncMLFilterProperty::QueryValueText8L() const
	{
	if ( iDataTypeForQueryValue != ESyncMLDataTypeText8 || !iCanUseQueryValue )
		{
		User::Leave( KErrNotSupported );
		}
	User::LeaveIfNull( iQueryValueText8 );
	
	return *iQueryValueText8;
	}
	
// -----------------------------------------------------------------------------
// CSyncMLFilterProperty::QueryValueIntL
// Returns query value for number property.
// This is useful only if data type is number and if query value can be used.
// -----------------------------------------------------------------------------
EXPORT_C TInt CSyncMLFilterProperty::QueryValueIntL() const
	{
	if ( iDataTypeForQueryValue != ESyncMLDataTypeNumber || !iCanUseQueryValue )
		{
		User::Leave( KErrNotSupported );
		}
	return iQueryValueIntOrBool;
	}
	
// -----------------------------------------------------------------------------
// CSyncMLFilterProperty::QueryValueBoolL
// Returns query value for boolean property.
// This is useful only if data type is boolean and if query value can be used.
// -----------------------------------------------------------------------------
EXPORT_C TBool CSyncMLFilterProperty::QueryValueBoolL() const
	{
	if ( iDataTypeForQueryValue != ESyncMLDataTypeBool || !iCanUseQueryValue )
		{
		User::Leave( KErrNotSupported );
		}
	return iQueryValueIntOrBool;
	}
	
// -----------------------------------------------------------------------------
// CSyncMLFilterProperty::QueryValueDateL
// Returns query value for date property.
// This is useful only if data type is date and if query value can be used.
// -----------------------------------------------------------------------------
EXPORT_C const TTime& CSyncMLFilterProperty::QueryValueDateL() const
	{
	if ( iDataTypeForQueryValue != ESyncMLDataTypeDate || !iCanUseQueryValue )
		{
		User::Leave( KErrNotSupported );
		}
	return iQueryValueDateOrTime;
	}
	
// -----------------------------------------------------------------------------
// CSyncMLFilterProperty::QueryValueTimeL
// Returns query value for time property.
// This is useful only if data type is time and if query value can be used.
// -----------------------------------------------------------------------------
EXPORT_C const TTime& CSyncMLFilterProperty::QueryValueTimeL() const
	{
	if ( iDataTypeForQueryValue != ESyncMLDataTypeTime || !iCanUseQueryValue )
		{
		User::Leave( KErrNotSupported );
		}
	return iQueryValueDateOrTime;
	}
	

// -----------------------------------------------------------------------------
// CSyncMLFilterProperty::UnselectQueryValueL
// Unselects query value. This is useful only if query value can be used.
// -----------------------------------------------------------------------------
EXPORT_C void CSyncMLFilterProperty::UnselectQueryValueL()
	{
	if ( !iCanUseQueryValue )
		{
		User::Leave( KErrNotSupported );
		}
	iQueryValueSelected = EFalse;
	}
	
// -----------------------------------------------------------------------------
// CSyncMLFilterProperty::SetQueryValueText16L
// Sets query value for text16 property.
// This is useful only if data type is text16 and if query value can be used.
// This method also selects query value.
// -----------------------------------------------------------------------------
EXPORT_C void
            CSyncMLFilterProperty::SetQueryValueText16L( const TDesC16& aValue )
	{
	if ( iDataTypeForQueryValue != ESyncMLDataTypeText16 || !iCanUseQueryValue )
		{
		User::Leave( KErrNotSupported );
		}
	if ( aValue.Length() > iMaxTextLength )
		{
		User::Leave( KErrArgument );
		}

	delete iQueryValueText16;
	iQueryValueText16 = NULL;
	iQueryValueText16 = aValue.AllocL();
	iQueryValueSelected = ETrue;
	}
	
// -----------------------------------------------------------------------------
// CSyncMLFilterProperty::SetQueryValueText8L
// Sets query value for text8 property.
// This is useful only if data type is text8 and if query value can be used.
// This method also selects query value.
// -----------------------------------------------------------------------------
EXPORT_C void CSyncMLFilterProperty::SetQueryValueText8L( const TDesC8& aValue )
	{
	if ( iDataTypeForQueryValue != ESyncMLDataTypeText8 || !iCanUseQueryValue )
		{
		User::Leave( KErrNotSupported );
		}
	if ( aValue.Length() > iMaxTextLength )
		{
		User::Leave( KErrArgument );
		}
		
	delete iQueryValueText8;
	iQueryValueText8 = NULL;
	iQueryValueText8 = aValue.AllocL();
	iQueryValueSelected = ETrue;
	}
	
// -----------------------------------------------------------------------------
// CSyncMLFilterProperty::SetQueryValueIntL
// Sets query value for number property.
// This is useful only if data type is number and if query value can be used.
// This method also selects query value.
// -----------------------------------------------------------------------------
EXPORT_C void CSyncMLFilterProperty::SetQueryValueIntL( TInt aValue )
	{
	if ( iDataTypeForQueryValue != ESyncMLDataTypeNumber || !iCanUseQueryValue )
		{
		User::Leave( KErrNotSupported );
		}
	iQueryValueIntOrBool = aValue;
	iQueryValueSelected = ETrue;
	}
	
// -----------------------------------------------------------------------------
// CSyncMLFilterProperty::SetQueryValueBoolL
// Sets query value for boolean property.
// This is useful only if data type is boolean and if query value can be used.
// This method also selects query value.
// -----------------------------------------------------------------------------
EXPORT_C void CSyncMLFilterProperty::SetQueryValueBoolL( TBool aValue )
	{
	if ( iDataTypeForQueryValue != ESyncMLDataTypeBool || !iCanUseQueryValue )
		{
		User::Leave( KErrNotSupported );
		}
	iQueryValueIntOrBool = aValue;
	iQueryValueSelected = ETrue;
	}
	
// -----------------------------------------------------------------------------
// CSyncMLFilterProperty::SetQueryValueDateL
// Sets query value for date property.
// This is useful only if data type is date and if query value can be used.
// This method also selects query value.
// -----------------------------------------------------------------------------
EXPORT_C void CSyncMLFilterProperty::SetQueryValueDateL( const TTime& aValue )
	{
	if ( iDataTypeForQueryValue != ESyncMLDataTypeDate || !iCanUseQueryValue )
		{
		User::Leave( KErrNotSupported );
		}
		
	TDateTime date( aValue.DateTime() );
	date.SetHour(0);
	date.SetMinute(0);
	date.SetSecond(0);
	date.SetMicroSecond(0);
			
	iQueryValueDateOrTime = date;
	iQueryValueSelected = ETrue;
	}
	
// -----------------------------------------------------------------------------
// CSyncMLFilterProperty::SetQueryValueTimeL
// Sets query value for time property.
// This is useful only if data type is time and if query value can be used.
// This method also selects query value.
// -----------------------------------------------------------------------------
EXPORT_C void CSyncMLFilterProperty::SetQueryValueTimeL( const TTime& aValue )
	{
	if ( iDataTypeForQueryValue != ESyncMLDataTypeTime || !iCanUseQueryValue )
		{
		User::Leave( KErrNotSupported );
		}
		
	TDateTime time( aValue.DateTime() );
	time.SetDay(0);
	time.SetMonth(EJanuary);
	time.SetYear(0);

	iQueryValueDateOrTime = time;
	iQueryValueSelected = ETrue;
	}

// -----------------------------------------------------------------------------
// CSyncMLFilterProperty::DataSize()
// Returns the size of filter property
// -----------------------------------------------------------------------------
EXPORT_C TInt CSyncMLFilterProperty::DataSize()
	{
	TInt size(0);
	
	// Length of Name
	size += KSizeofTInt32;
	
	if ( iDisplayName )
		{
		// header information
		size += iDisplayName->Length() + 1;
		// header information is needed for every 64 part
		size += iDisplayName->Length() / 64;
		}
	
	// iDataType for default values
	size += KSizeofTInt8;
	
	// iDataType for query value
	size += KSizeofTInt8;
	
	// iMaxTextLength
	size += KSizeofTInt32;
	
	// default values
	switch ( iDataTypeForDefaultValues )
		{
		case ESyncMLDataTypeNull:
			{
			size += KSizeofTInt32;
			}
		break;
		
		case ESyncMLDataTypeBool :
		case ESyncMLDataTypeNumber:
			{
			// default value count
			size += KSizeofTInt32;
			size += DefaultValueCount() * KSizeofTInt32;
			}
		break;

		case ESyncMLDataTypeTime:		
		case ESyncMLDataTypeDate:
			{
			// default value count
			size += KSizeofTInt32;
			size += KSizeOfTInt64 * DefaultValueCount();
			}
		break;
		
		case ESyncMLDataTypeText8:
			{
			// default value count
			size += KSizeofTInt32;
			// total length of strings
			size += DefaultValueCount() * KSizeofTInt32;
			
			for (TInt i = 0; i < DefaultValueCount(); i++)
				{
				if ( iDefaultValueText8List[i] )
					{
					// iDefaultValueText8List + header information
					size += iDefaultValueText8List[i]->Length() + 1;
					// header information is needed for every 64 part
					size += iDefaultValueText8List[i]->Length() / 64;
					}
				}
			}
		break;
		
		case ESyncMLDataTypeText16:
			{
			// default value count
			size += KSizeofTInt32;
			// total length of strings
			size += DefaultValueCount() * KSizeofTInt32;
			
			for (TInt i = 0; i < DefaultValueCount(); i++)
				{
				if ( iDefaultValueText16List[i] )
					{
					// iDefaultValueText16List + header information
					size += iDefaultValueText16List[i]->Length() + 1;
					// header information is needed for every 64 part
					size += iDefaultValueText16List[i]->Length() / 64;
					}
				}
			}
		break;
		
		default:
			{
			User::Panic( _L("Out of range"), KErrArgument );
			}
			
		break;

		}
		
	// query values
	switch ( iDataTypeForQueryValue )
		{
		case ESyncMLDataTypeNull:
			{
			size += KSizeofTInt32;
			}
		break;
		
		case ESyncMLDataTypeBool :
		case ESyncMLDataTypeNumber:
			{
			// queryValue
			size += KSizeofTInt32;
			}
		break;

		case ESyncMLDataTypeTime:
		case ESyncMLDataTypeDate:
			{
			// queryValue
			size += KSizeOfTInt64;
			}
		break;
		
		case ESyncMLDataTypeText8:
			{
			// queryValue
			size += KSizeofTInt32;
			if ( iQueryValueText8 )
				{
				// + header information
				size += iQueryValueText8->Des().Length() + 1;
				// header information is needed for every 64 part
				size += iQueryValueText8->Des().Length() / 64;
				}
											
			}
		break;
		
		case ESyncMLDataTypeText16:
			{
			// queryValue
			size += KSizeofTInt32;
			if ( iQueryValueText16 )
				{
				// + header information
				size += iQueryValueText16->Des().Length() + 1;
				// header information is needed for every 64 part
				size += iQueryValueText16->Des().Length() / 64;
				}
			}
		break;
		
		default:
			{
			User::Panic( _L("Out of range"), KErrArgument );
			}
			
		break;

		}
	
	// default value selected array count
	size += KSizeofTInt32;
	
	// iDefaultValueSelected
	size += DefaultValueCount() * KSizeofTInt32;
	
	// iCanUseQueryValue
	size += KSizeofTInt8;
	
	// iSupportsMultpleSelection
	size += KSizeofTInt8;
	
	// iSupportsEmptySelection
	size += KSizeofTInt8;
	
	// iQueryValueSelected
	size += KSizeofTInt8;
	
	return size;
	}
	
// -----------------------------------------------------------------------------
// CSyncMLFilterProperty::CSyncMLFilterProperty
// Basic constructor.
// -----------------------------------------------------------------------------
CSyncMLFilterProperty::CSyncMLFilterProperty() : 
	iDataTypeForDefaultValues( ESyncMLDataTypeNull ),
	iDataTypeForQueryValue( ESyncMLDataTypeNull ),
	iMaxTextLength( KMaxTextLength ),
	iCanUseQueryValue( EFalse ),
	iSupportsMultpleSelection( EFalse ),
	iSupportsEmptySelection( EFalse ),
	iQueryValueSelected( EFalse ),
	iQueryValueIntOrBool( 0 ),
  	iQueryValueDateOrTime( 0 )
	{
	}

// End of file
