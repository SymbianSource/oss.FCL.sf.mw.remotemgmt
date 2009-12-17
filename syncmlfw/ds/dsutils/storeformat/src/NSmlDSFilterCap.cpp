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
Filter Capability class
The format and capabilities of a filter
*/

// INCLUDE FILES
#include <SmlDataFormat.h>
#include <stringpool.h>

// CONSTANTS
const TInt KVersionValue = 1;

// ===================================== MEMBER FUNCTIONS ========================================

// ------------------------------------------------------------------------------------------------
// CSmlFilterCapability::NewLC()
// Class constructor
// ------------------------------------------------------------------------------------------------
EXPORT_C CSmlFilterCapability* CSmlFilterCapability::NewLC()
	{
	CSmlFilterCapability* self = new ( ELeave ) CSmlFilterCapability();
	CleanupStack::PushL( self );
	return self;
	}

// ------------------------------------------------------------------------------------------------
// CSmlFilterCapability::NewLC()
// Class constructor from resource file
// ------------------------------------------------------------------------------------------------
EXPORT_C CSmlFilterCapability* CSmlFilterCapability::NewLC( const RStringPool& aStringPool, TResourceReader& aReader )
	{
	CSmlFilterCapability* self = new ( ELeave ) CSmlFilterCapability();
	CleanupStack::PushL( self );
	
	HBufC8* tmpMimetype;
	HBufC8* tmpMimeversion;

	TInt version = aReader.ReadInt8();					// BYTE version

	tmpMimetype = aReader.ReadHBufC8L();			// LTEXT8 mime_type
	if( tmpMimetype )				
		{
		CleanupStack::PushL( tmpMimetype );
		self->iMimeType = aStringPool.OpenFStringL( *tmpMimetype );
		CleanupStack::PopAndDestroy( tmpMimetype );		
		}
	else
		{
		tmpMimetype = KNullDesC8().AllocLC();
		self->iMimeType = aStringPool.OpenFStringL( *tmpMimetype );
		CleanupStack::PopAndDestroy( tmpMimetype );	 	
		}

	tmpMimeversion = aReader.ReadHBufC8L();			// LTEXT8 mime_ver
	if( tmpMimeversion )				
		{
		CleanupStack::PushL( tmpMimeversion );
		self->iMimeVersion = aStringPool.OpenFStringL( *tmpMimeversion );
		CleanupStack::PopAndDestroy( tmpMimeversion );		
		}
	else
		{
		tmpMimeversion = KNullDesC8().AllocLC();
		self->iMimeVersion = aStringPool.OpenFStringL( *tmpMimeversion );
		CleanupStack::PopAndDestroy( tmpMimeversion );			
		}

	TInt number = aReader.ReadInt16();		// Keyword array

	for ( TInt index = 0; index < number ; index++ )
		{
		HBufC8* tmpKeyword = aReader.ReadHBufC8L();
		CleanupStack::PushL( tmpKeyword );
		RString tmprstr( aStringPool.OpenStringL( *tmpKeyword ) );
		CleanupClosePushL( tmprstr );
		self->iKeywordList.AppendL( tmprstr );
		CleanupStack::Pop();
		CleanupStack::PopAndDestroy( tmpKeyword );
		}		

	number = aReader.ReadInt16();		// PropertyList array
	for ( TInt index2 = 0; index2 < number ; index2++ )
		{
		HBufC8* tmpProperty = aReader.ReadHBufC8L();
		CleanupStack::PushL( tmpProperty );
		RString tmprstr( aStringPool.OpenStringL( *tmpProperty ) );
		CleanupClosePushL( tmprstr );
		self->iPropertyList.AppendL( tmprstr );
		CleanupStack::Pop();
		CleanupStack::PopAndDestroy( tmpProperty );
		}		
	
	return self;
	}

// ------------------------------------------------------------------------------------------------
// CSmlFilterCapability::NewLC()
// Class constructor from stream
// ------------------------------------------------------------------------------------------------
EXPORT_C CSmlFilterCapability* CSmlFilterCapability::NewLC(const RStringPool& aStringPool, RReadStream& aStream)
	{
	CSmlFilterCapability* self = new (ELeave) CSmlFilterCapability();
	CleanupStack::PushL( self );
		
	TInt version = aStream.ReadInt8L();

	TInt tmpLength = aStream.ReadUint32L();
	HBufC8* tmpMimetype = HBufC8::NewLC( aStream, tmpLength );
	self->iMimeType = aStringPool.OpenFStringL( *tmpMimetype );
	CleanupStack::PopAndDestroy(); // tmpMimetype	
	
	tmpLength = aStream.ReadUint32L();
	HBufC8* tmpMimeversion = HBufC8::NewLC( aStream, tmpLength );
	self->iMimeVersion = aStringPool.OpenFStringL( *tmpMimeversion );
	CleanupStack::PopAndDestroy(); // tmpMimeversion	
	
	TInt number = aStream.ReadInt16L();

	for ( TInt index = 0; index < number ; index++ )
		{
		tmpLength = aStream.ReadUint32L();
		HBufC8* tmpKeyword = HBufC8::NewLC( aStream, tmpLength );
		RString tmprstr( aStringPool.OpenStringL( *tmpKeyword ) );
		CleanupClosePushL( tmprstr );
		self->iKeywordList.AppendL( tmprstr );
		CleanupStack::Pop(); //tmprstr
		CleanupStack::PopAndDestroy( tmpKeyword );
		}		
	
	number = aStream.ReadInt16L();
	for ( TInt index2 = 0; index2 < number ; index2++ )
		{
		tmpLength = aStream.ReadUint32L();
		HBufC8* tmpProperty = HBufC8::NewLC( aStream, tmpLength );
		RString tmprstr( aStringPool.OpenStringL( *tmpProperty ) );
		CleanupClosePushL( tmprstr );
		self->iPropertyList.AppendL( tmprstr );
		CleanupStack::Pop(); //tmprstr
		CleanupStack::PopAndDestroy( tmpProperty );
		}
	
	return self;
	}
	
// ------------------------------------------------------------------------------------------------
// CSmlFilterCapability::~CSmlFilterCapability()
// Destructor.
// ------------------------------------------------------------------------------------------------
EXPORT_C CSmlFilterCapability::~CSmlFilterCapability()
	{
	iMimeType.Close();
	iMimeVersion.Close();
	for ( TInt i = 0; i < iKeywordList.Count(); i++ )
		{
		iKeywordList[i].Close();
		}
	iKeywordList.Reset();
	
	for ( TInt j = 0; j < iPropertyList.Count(); j++ )
		{
		iPropertyList[j].Close();
		}
	iPropertyList.Reset();
	}

// ------------------------------------------------------------------------------------------------
// CSmlFilterCapability::ExternalizeL()
// Puts the class contents to the write stream
// ------------------------------------------------------------------------------------------------
EXPORT_C void CSmlFilterCapability::ExternalizeL( RWriteStream& aStream ) const
	{
	aStream.WriteInt8L( KVersionValue ); //version
	aStream.WriteUint32L( iMimeType.DesC().Length() );		
	aStream << iMimeType.DesC();	
	
	aStream.WriteUint32L( iMimeVersion.DesC().Length() );	
	aStream << iMimeVersion.DesC();
	
	aStream.WriteInt16L( iKeywordList.Count() );
		
	for ( TInt index = 0; index < iKeywordList.Count() ; index++ )	
		{		
		aStream.WriteUint32L( iKeywordList[index].DesC().Length() );	
		aStream << iKeywordList[index].DesC();					
		}	
	
	aStream.WriteInt16L( iPropertyList.Count() );
	for ( TInt index2 = 0; index2 < iPropertyList.Count() ; index2++ )	
		{		
		aStream.WriteUint32L( iPropertyList[index2].DesC().Length() );	
		aStream << iPropertyList[index2].DesC();					
		}	
	}

// ------------------------------------------------------------------------------------------------
// CSmlFilterCapability::MimeType()
// Returns mime type
// ------------------------------------------------------------------------------------------------
EXPORT_C RStringF CSmlFilterCapability::MimeType() const
	{
	return iMimeType;
	}
	
// ------------------------------------------------------------------------------------------------
// CSmlFilterCapability::SetMimeTypeL()
// Sets mime type.
// ------------------------------------------------------------------------------------------------
EXPORT_C void CSmlFilterCapability::SetMimeTypeL( RStringF& aMimeType )
	{
	iMimeType.Close();
	iMimeType = aMimeType.Copy();
	aMimeType.Close();
	}

// ------------------------------------------------------------------------------------------------
// CSmlFilterCapability::MimeVersion()
// Returns mime version
// ------------------------------------------------------------------------------------------------
EXPORT_C RStringF CSmlFilterCapability::MimeVersion() const
	{
	return iMimeVersion;
	}

// ------------------------------------------------------------------------------------------------
// CSmlFilterCapability::SetMimeVersionL()
// Sets mime version.
// ------------------------------------------------------------------------------------------------
EXPORT_C void CSmlFilterCapability::SetMimeVersionL( RStringF& aMimeVersion )
	{
	iMimeVersion.Close();
	iMimeVersion = aMimeVersion.Copy();
	aMimeVersion.Close();
	}	

// ------------------------------------------------------------------------------------------------
// CSmlFilterCapability::KeywordCount()
// Retruns the number of keywords.
// ------------------------------------------------------------------------------------------------
EXPORT_C TInt CSmlFilterCapability::KeywordCount() const
	{
	return iKeywordList.Count();
	}

// ------------------------------------------------------------------------------------------------
// CSmlFilterCapability::Keyword()
// Returns the indexed keyword.
// ------------------------------------------------------------------------------------------------
EXPORT_C RString CSmlFilterCapability::Keyword( TInt aIndex ) const
	{
	return iKeywordList[aIndex];	
	}

// ------------------------------------------------------------------------------------------------
// CSmlFilterCapability::SetKeyWordListL()
// Sets keywords.
// ------------------------------------------------------------------------------------------------
EXPORT_C void CSmlFilterCapability::SetKeyWordListL( RArray<RString>& aKeywordList )
	{
	for ( TInt i = 0; i < iKeywordList.Count(); i++ )
		{
		iKeywordList[i].Close();
		}
	iKeywordList.Reset();
	iKeywordList.Close();
	for ( TInt j = 0; j < aKeywordList.Count(); j++ )
		{
		iKeywordList.AppendL( aKeywordList[j].Copy() );
		}
	for ( TInt k = 0; k < aKeywordList.Count(); k++ )
		{
		aKeywordList[k].Close();
		}
	aKeywordList.Reset();
	aKeywordList.Close();
	}

// ------------------------------------------------------------------------------------------------
// CSmlFilterCapability::PropertyCount()
// Returns the number of properties.
// ------------------------------------------------------------------------------------------------
EXPORT_C TInt CSmlFilterCapability::PropertyCount() const
	{
	return iPropertyList.Count();
	}

// ------------------------------------------------------------------------------------------------
// CSmlFilterCapability::PropertyName()
// Returns the property name.
// ------------------------------------------------------------------------------------------------
EXPORT_C RString CSmlFilterCapability::PropertyName( TInt aIndex ) const
	{
	return iPropertyList[aIndex];
	}

// ------------------------------------------------------------------------------------------------
// CSmlFilterCapability::SetPropertiesListL()
// Sets properties.
// ------------------------------------------------------------------------------------------------
EXPORT_C void CSmlFilterCapability::SetPropertiesListL( RArray<RString>& aPropertyList )
	{
	for ( TInt i = 0; i < iPropertyList.Count(); i++ )
		{
		iPropertyList[i].Close();
		}
	iPropertyList.Reset();
	iPropertyList.Close();
	for ( TInt j = 0; j < aPropertyList.Count(); j++ )
		{
		iPropertyList.AppendL( aPropertyList[j].Copy() );
		}
	for ( TInt k = 0; k < aPropertyList.Count(); k++ )
		{
		aPropertyList[k].Close();
		}
	aPropertyList.Reset();
	aPropertyList.Close();
	}

// End of File
