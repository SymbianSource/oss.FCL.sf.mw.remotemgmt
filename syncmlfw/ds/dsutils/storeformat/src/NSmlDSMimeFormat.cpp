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


// INCLUDE FILES
#include <SmlDataFormat.h>
#include <stringpool.h>

#include <centralrepository.h> 

// CONSTANTS
const TInt KNsmlDsGeoField = 0x0000;  //  defined in DS cenrep
const TUid KCRUidGeoFieldInternalKeys = { 0x20022EE1 };  // 0x20022EE1 defined in DS cenrep

 _LIT16(KGEO,"GEO Address");
// ===================================== MEMBER FUNCTIONS ========================================

// ------------------------------------------------------------------------------------------------
// CSmlMimeFormat::NewLC()
// Class constructor.
// ------------------------------------------------------------------------------------------------
EXPORT_C CSmlMimeFormat* CSmlMimeFormat::NewLC()
	{
	CSmlMimeFormat* self = new( ELeave ) CSmlMimeFormat;
	CleanupStack::PushL( self );
	return self;
	}

// ------------------------------------------------------------------------------------------------
// CSmlMimeFormat::NewLC()
// Class constructor from resource
// ------------------------------------------------------------------------------------------------
EXPORT_C CSmlMimeFormat* CSmlMimeFormat::NewLC( const RStringPool& aStringPool, TResourceReader& aReader )
	{
	CSmlMimeFormat* self = new( ELeave ) CSmlMimeFormat;
	CleanupStack::PushL( self );

	TInt version( aReader.ReadInt8() );					// BYTE version

	HBufC8* tmpMimetype = aReader.ReadHBufC8L();		// LTEXT8 mime_type
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
	
	HBufC8* tmpMimeversion = aReader.ReadHBufC8L();		// LTEXT8 mime_ver
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
	
	// The first WORD contains the number of DATA structs within the resource
	TInt number( aReader.ReadInt16() );		
	for ( TInt index = 0 ; index < number ; index++ )
		{
		CSmlDataProperty *temppoint = CSmlDataProperty::NewLC( aStringPool, aReader );	// STRUCT properties[] (SML_DATA_PROPERTY)
          
	   if ( temppoint->Field().DisplayName().Compare(KGEO()) == 0 ) //Check For Feature ON/OFF For GEO/GEO-HOME/GEO-WORK
	        {
	        // Read from cenrep	
	        CRepository* repository = CRepository::NewLC( KCRUidGeoFieldInternalKeys );
	        TBool geoFeatureValue(1); 
	        // Get the geo feature value..
	        TInt err = repository->Get( KNsmlDsGeoField, geoFeatureValue );
	        User::LeaveIfError(err);
	        if (!geoFeatureValue)
	            {
	            CleanupStack::PopAndDestroy(repository);
	            CleanupStack::PopAndDestroy(temppoint);
	            temppoint = NULL;
	            }
          else
              {
	            CleanupStack::PopAndDestroy(repository);
              }
	        }
             
     if ( temppoint != NULL)
		     {
		       self->iProperties.AppendL( temppoint );
		       CleanupStack::Pop( temppoint );
		     }
		}
		
	self->iFieldLevel = aReader.ReadInt8();					// BYTE field_level
		
	return self;
	}

// ------------------------------------------------------------------------------------------------
// CSmlMimeFormat::NewLC()
// Class constructor from stream
// ------------------------------------------------------------------------------------------------
EXPORT_C CSmlMimeFormat* CSmlMimeFormat::NewLC( const RStringPool& aStringPool, RReadStream& aStream )
	{
	CSmlMimeFormat* self = new( ELeave ) CSmlMimeFormat;
	CleanupStack::PushL( self );
	
	TInt version( aStream.ReadInt8L() );

	TInt32 tmpLength = aStream.ReadUint32L();
	HBufC8* tmpMimetype = HBufC8::NewLC( aStream, tmpLength );	// mimetype
	self->iMimeType = aStringPool.OpenFStringL( *tmpMimetype );
	CleanupStack::PopAndDestroy( tmpMimetype );
	
	tmpLength = aStream.ReadUint32L();
	HBufC8* tmpMimeversion = HBufC8::NewLC( aStream, tmpLength ); // mimeversion
	self->iMimeVersion = aStringPool.OpenFStringL( *tmpMimeversion );
	CleanupStack::PopAndDestroy( tmpMimeversion );
	
	//properties
	TInt number( aStream.ReadInt16L() );
	
	for (TInt index = 0; index < number ; index++)
		{
		CSmlDataProperty *temppoint = CSmlDataProperty::NewLC( aStringPool, aStream );
		self->iProperties.AppendL( temppoint );
		CleanupStack::Pop( temppoint ); 
		}
	
	//field level
	self->iFieldLevel = aStream.ReadInt8L();
	return self;
	}

// ------------------------------------------------------------------------------------------------
// CSmlMimeFormat::~CSmlMimeFormat()
// Destructor
// ------------------------------------------------------------------------------------------------
EXPORT_C CSmlMimeFormat::~CSmlMimeFormat()
	{
	iMimeType.Close();
	iMimeVersion.Close();
	iProperties.ResetAndDestroy();
	}

// ------------------------------------------------------------------------------------------------
// CSmlMimeFormat::ExternalizeL()
// Puts the class contents to the write stream
// ------------------------------------------------------------------------------------------------
EXPORT_C void CSmlMimeFormat::ExternalizeL( RWriteStream& aStream ) const
	{
	aStream.WriteInt8L( 1 );	//version
	
	aStream.WriteUint32L( iMimeType.DesC().Length() );		// mimetype
	aStream << iMimeType.DesC();	
	
	aStream.WriteUint32L( iMimeVersion.DesC().Length() );	// mimeversion?
	aStream << iMimeVersion.DesC();	
	
	aStream.WriteInt16L( iProperties.Count() );
	
	//properties
	for ( TInt index = 0; index < iProperties.Count() ; index++ )
		{
		iProperties[index]->ExternalizeL( aStream );
		}
	
	//field level
	aStream.WriteInt8L( iFieldLevel );	//field level
	}
	
// ------------------------------------------------------------------------------------------------
// CSmlMimeFormat::MimeType()
// Returns the stored Mime type
// ------------------------------------------------------------------------------------------------
EXPORT_C RStringF CSmlMimeFormat::MimeType() const
	{
	return iMimeType;
	}
	
// ------------------------------------------------------------------------------------------------
// CSmlMimeFormat::SetMimeTypeL()
// Stores Mime type
// ------------------------------------------------------------------------------------------------
EXPORT_C void CSmlMimeFormat::SetMimeTypeL( RStringF& aMimeType )
	{
	iMimeType.Close();
	iMimeType = aMimeType.Copy();
	aMimeType.Close();
	}

// ------------------------------------------------------------------------------------------------
// CSmlMimeFormat::MimeVersion()
// Returns the stored Mime version
// ------------------------------------------------------------------------------------------------
EXPORT_C RStringF CSmlMimeFormat::MimeVersion() const
	{
	return iMimeVersion;
	}

// ------------------------------------------------------------------------------------------------
// CSmlMimeFormat::SetMimeVersionL()
// Stores Mime version
// ------------------------------------------------------------------------------------------------
EXPORT_C void CSmlMimeFormat::SetMimeVersionL( RStringF& aMimeVersion )
	{
	iMimeVersion.Close();
	iMimeVersion = aMimeVersion.Copy();
	aMimeVersion.Close();
	}

// ------------------------------------------------------------------------------------------------
// CSmlMimeFormat::PropertyCount()
// Number of properties in the array
// ------------------------------------------------------------------------------------------------
EXPORT_C TInt CSmlMimeFormat::PropertyCount() const
	{
	return iProperties.Count();
	}

// ------------------------------------------------------------------------------------------------
// CSmlMimeFormat::Property()
// Returns the indexed property
// ------------------------------------------------------------------------------------------------
EXPORT_C const CSmlDataProperty& CSmlMimeFormat::Property( TInt aIndex ) const
	{
	return *iProperties[aIndex]; 
	}
	
// ------------------------------------------------------------------------------------------------
// CSmlMimeFormat::SetDataPropertiesL()
// Sets properties
// ------------------------------------------------------------------------------------------------
EXPORT_C void CSmlMimeFormat::SetDataPropertiesL( RPointerArray<CSmlDataProperty>& aProperties )
	{
	iProperties.ResetAndDestroy();
	for ( TInt i = 0; i < aProperties.Count(); i++ )
		{
		iProperties.AppendL( aProperties[i] );
		}
	aProperties.Reset();
	aProperties.Close();
	}

// ------------------------------------------------------------------------------------------------
// CSmlMimeFormat::FieldLevel()
// Returns the fieldlevel
// ------------------------------------------------------------------------------------------------
EXPORT_C TBool CSmlMimeFormat::FieldLevel() const
	{
	return iFieldLevel;
	}

// ------------------------------------------------------------------------------------------------
// CSmlMimeFormat::SetFieldLevel()
// Sets the fieldlevel
// ------------------------------------------------------------------------------------------------
EXPORT_C void CSmlMimeFormat::SetFieldLevel( TBool aFieldLevel )
	{
	iFieldLevel = aFieldLevel;
	}

// End of file
