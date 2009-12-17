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


/*
Data Store Format.

Used by Data Provider implementations to specify the format and capabilities of their Data Stores.
Used by the Sync Engine to generate SyncML DevInf.
See also SmlDataFormat.rh and SmlDataFormat.hrh for resource definitions.

Data store class
The format and capabilities of Data Stores of a specific type.
*/

// INCLUDE FILES
#include <SmlDataFormat.h>
#include <SmlDataFormat.hrh>
#include <stringpool.h>
#include "NSmlDSDataStoreFormatUtils.h"

// CONSTANTS
const TInt KVersionValue = 1;

// ===================================== MEMBER FUNCTIONS ========================================

// ------------------------------------------------------------------------------------------------
// CSmlDataStoreFormat::NewLC()
// Empty construction
// ------------------------------------------------------------------------------------------------
EXPORT_C CSmlDataStoreFormat* CSmlDataStoreFormat::NewLC()
{
	CSmlDataStoreFormat* self = new (ELeave) CSmlDataStoreFormat();
	CleanupStack::PushL( self );
	self->iDisplayName = NULL;
	return( self );
}

// ------------------------------------------------------------------------------------------------
// CSmlDataStoreFormat::NewLC()
// Class constructor from resource file
// ------------------------------------------------------------------------------------------------
EXPORT_C CSmlDataStoreFormat* CSmlDataStoreFormat::NewLC( const RStringPool& aStringPool, TResourceReader& aReader )
{
	CSmlDataStoreFormat* self = new( ELeave ) CSmlDataStoreFormat;
	CleanupStack::PushL( self );

	TInt version = aReader.ReadInt8();				// BYTE version
	self->iDisplayName = aReader.ReadHBufC16L(); 	// LTEXT display_name
	TUint16 tmpSynctypes( aReader.ReadInt16() );	// WORD sync_types

	TInt number( aReader.ReadInt16() );		// The first WORD contains the number of DATA structs within the resource
	
	// STRUCT mime_format[] (SML_DATA_PROPERTY)
	for ( TInt index = 0 ; index < number ; index++ )
		{
		CSmlMimeFormat* temppoint = CSmlMimeFormat::NewLC( aStringPool, aReader );
		self->iMimeFormats.AppendL( temppoint );
		CleanupStack::Pop( temppoint );
		}
		
	self->iTxPref = aReader.ReadInt16();	// WORD mime_format_tx_pref     
  	self->iRxPref = aReader.ReadInt16();	// WORD mime_format_rx_pref     		

	// STRUCT folder_properties[] (SML_DATA_PROPERTY)
	number = aReader.ReadInt16();
	for ( TInt index2 = 0; index2 < number ; index2++ )
		{
		CSmlDataProperty *temppoint = CSmlDataProperty::NewLC( aStringPool, aReader );
		self->iFolderProperties.AppendL( temppoint );
		CleanupStack::Pop( temppoint ); 
		}

	// STRUCT filter_capabilities[] (SML_FILTER_CAPABILITY)
	number = aReader.ReadInt16();
	for ( TInt index3 = 0; index3 < number ; index3++ )
		{
		CSmlFilterCapability *temppoint = CSmlFilterCapability::NewLC( aStringPool, aReader );
		self->iFilterCapabilities.AppendL( temppoint );
		CleanupStack::Pop( temppoint );
		}
		
	self->iMaxSize = aReader.ReadUint16();				// WORD max_size
	self->iMaxItems = aReader.ReadUint16();				// WORD max_items
	self->iFlags = aReader.ReadUint32();				// LONG flags
	
	TSmlDataStoreFormatUtils::SetSyncTypeMaskFromResource( self->iSyncTypeMask, tmpSynctypes );
	return self;	

}

// ------------------------------------------------------------------------------------------------
// CSmlDataStoreFormat::NewLC()
// Class constructor from stream
// ------------------------------------------------------------------------------------------------
EXPORT_C CSmlDataStoreFormat* CSmlDataStoreFormat::NewLC( const RStringPool& aStringPool, RReadStream& aStream )
{
	CSmlDataStoreFormat* self = new( ELeave ) CSmlDataStoreFormat;
	CleanupStack::PushL( self );
	
	TInt version( aStream.ReadInt8L() );
		
	TInt tmpLength ( aStream.ReadUint32L() );		// the length of desc
	self->iDisplayName = HBufC::NewL( aStream, tmpLength ); //display name
	self->iFlags = aStream.ReadInt32L(); 			//flags
	self->iMaxSize = aStream.ReadUint16L();			//maxsize
	self->iMaxItems = aStream.ReadUint16L();		//max items
	TUint16 tmpSynctypes ( aStream.ReadInt16L() );	//sync types

	TInt number( aStream.ReadInt16L() );

	//mime formats
	for ( TInt index = 0; index < number ; index++ )
		{
		CSmlMimeFormat *temppoint = CSmlMimeFormat::NewLC( aStringPool, aStream );
		self->iMimeFormats.AppendL( temppoint );
		CleanupStack::Pop( temppoint ); 
		}

	self->iTxPref = aStream.ReadUint16L();	// WORD mime_format_tx_pref     
  	self->iRxPref = aStream.ReadUint16L();	// WORD mime_format_rx_pref  

	//folder properties
	number = aStream.ReadInt16L();
	for ( TInt index2 = 0; index2 < number ; index2++ )
		{
		CSmlDataProperty *temppoint = CSmlDataProperty::NewLC( aStringPool, aStream );
		self->iFolderProperties.AppendL( temppoint );
		CleanupStack::Pop( temppoint ); 
		}
		
	//filter capabilities
	number = aStream.ReadInt16L();
	for ( TInt index3 = 0; index3 < number ; index3++ )
		{
		CSmlFilterCapability *temppoint = CSmlFilterCapability::NewLC( aStringPool, aStream );
		self->iFilterCapabilities.AppendL( temppoint );
		CleanupStack::Pop( temppoint ); 
		}

	TSmlDataStoreFormatUtils::SetSyncTypeMaskFromResource( self->iSyncTypeMask, tmpSynctypes );
	return self;
}

// ------------------------------------------------------------------------------------------------
// CSmlDataStoreFormat::~CSmlDataStoreFormat()
// Destructor
// ------------------------------------------------------------------------------------------------
EXPORT_C CSmlDataStoreFormat::~CSmlDataStoreFormat()
{
	delete iDisplayName;
	iMimeFormats.ResetAndDestroy();
	iFolderProperties.ResetAndDestroy();
	iFilterCapabilities.ResetAndDestroy();	
}

// ------------------------------------------------------------------------------------------------
// CSmlDataStoreFormat::ExternalizeL()
// Puts the class contents to the write stream
// ------------------------------------------------------------------------------------------------
EXPORT_C void CSmlDataStoreFormat::ExternalizeL( RWriteStream& aStream ) const
	{
	aStream.WriteInt8L( KVersionValue );	//version
	
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

	aStream.WriteInt32L( iFlags );					//flags
	aStream.WriteInt16L( iMaxSize );				//maxsize
	aStream.WriteInt16L( iMaxItems );				//maxitems

	TUint16 tmpSynctypes(0);
	
	if ( iSyncTypeMask.IsSupported( ESmlTwoWay ) )
		{
		tmpSynctypes |= KSmlSyncType_TwoWay;
		}
		
	if ( iSyncTypeMask.IsSupported( ESmlOneWayFromServer ) )
		{
		tmpSynctypes |= KSmlSyncType_OneWayFromSvr;
		}
		
	if ( iSyncTypeMask.IsSupported( ESmlOneWayFromClient ) )
		{
		tmpSynctypes |= KSmlSyncType_OneWayFromClnt;
		}
		
	if ( iSyncTypeMask.IsSupported( ESmlSlowSync ) )
		{
		tmpSynctypes |= KSmlSyncType_SlowSync;
		}
		
	if ( iSyncTypeMask.IsSupported( ESmlRefreshFromServer ) )
		{
		tmpSynctypes |= KSmlSyncType_RefreshFromSvr;
		}
		
	if ( iSyncTypeMask.IsSupported( ESmlRefreshFromClient ) )
		{
		tmpSynctypes |= KSmlSyncType_RefreshFromClnt;
		}
		
	aStream.WriteInt16L( tmpSynctypes );			//sync types

	//mime formats
	aStream.WriteInt16L( iMimeFormats.Count() );
	for ( TInt index = 0; index < iMimeFormats.Count() ; index++ )
		{
		iMimeFormats[index]->ExternalizeL( aStream );
		}

	aStream.WriteInt16L( iTxPref );	// WORD mime_format_tx_pref     
  	aStream.WriteInt16L( iRxPref );	// WORD mime_format_rx_pref  

	//folder properties
	aStream.WriteInt16L( iFolderProperties.Count() );
	for ( TInt index2 = 0; index2 < iFolderProperties.Count() ; index2++ )
		{
		iFolderProperties[index2]->ExternalizeL( aStream );
		}

	//filter cababilities
	aStream.WriteInt16L( iFilterCapabilities.Count() );
	for ( TInt index3 = 0; index3 < iFilterCapabilities.Count() ; index3++ )
		{
		iFilterCapabilities[index3]->ExternalizeL( aStream );
		}
	}

// ------------------------------------------------------------------------------------------------
// CSmlDataStoreFormat::DisplayName()
// Returns display name.
// ------------------------------------------------------------------------------------------------
EXPORT_C const TDesC& CSmlDataStoreFormat::DisplayName() const
	{
	return *iDisplayName;
	}
	
// ------------------------------------------------------------------------------------------------
// CSmlDataStoreFormat::SetDisplayNameL()
// Sets display name.
// ------------------------------------------------------------------------------------------------
EXPORT_C void CSmlDataStoreFormat::SetDisplayNameL( TDesC& aDisplayName )
	{
	delete iDisplayName;
	iDisplayName = NULL;
	iDisplayName = aDisplayName.AllocL();
	}

// ------------------------------------------------------------------------------------------------
// CSmlDataStoreFormat::SyncTypes()
// Returns the stored sync types
// ------------------------------------------------------------------------------------------------
EXPORT_C TSmlSyncTypeMask CSmlDataStoreFormat::SyncTypes() const
	{
	return iSyncTypeMask;
	}

// ------------------------------------------------------------------------------------------------
// CSmlDataStoreFormat::SetSyncTypeMask()
// Stores sync types.
// ------------------------------------------------------------------------------------------------
EXPORT_C void CSmlDataStoreFormat::SetSyncTypeMask( TSmlSyncTypeMask aSyncTypeMask )
	{
	iSyncTypeMask = aSyncTypeMask;
	}

// ------------------------------------------------------------------------------------------------
// CSmlDataStoreFormat::MimeFormatCount()
// Returns number of supported mime types.
// ------------------------------------------------------------------------------------------------
EXPORT_C TInt CSmlDataStoreFormat::MimeFormatCount() const
	{
	return iMimeFormats.Count();
	}
	
// ------------------------------------------------------------------------------------------------
// CSmlDataStoreFormat::MimeFormat()
// Returns indexed mime format
// ------------------------------------------------------------------------------------------------
EXPORT_C const CSmlMimeFormat& CSmlDataStoreFormat::MimeFormat( TInt aIndex ) const
	{
	return *iMimeFormats[aIndex];
	}
	
// ------------------------------------------------------------------------------------------------
// CSmlDataStoreFormat::SetMimeFormatsL()
// Sets mime formats.
// ------------------------------------------------------------------------------------------------
EXPORT_C void CSmlDataStoreFormat::SetMimeFormatsL( RPointerArray<CSmlMimeFormat>& aMimeFormats )
	{
	iMimeFormats.ResetAndDestroy();
	for ( TInt i = 0; i < aMimeFormats.Count(); i++ )
		{
		iMimeFormats.AppendL( aMimeFormats[i] );
		}
	aMimeFormats.Reset();
	aMimeFormats.Close();
	}
	
// ------------------------------------------------------------------------------------------------
// CSmlDataStoreFormat::MimeFormatRxPref()
// Returns RxPref.
// ------------------------------------------------------------------------------------------------
EXPORT_C TInt CSmlDataStoreFormat::MimeFormatRxPref() const
	{
	return iRxPref;
	}

// ------------------------------------------------------------------------------------------------
// CSmlDataStoreFormat::SetMimeFormatRxPref()
// Sets rx preference.
// ------------------------------------------------------------------------------------------------
EXPORT_C void CSmlDataStoreFormat::SetMimeFormatRxPref( TInt aRxPref )
	{
	iRxPref = aRxPref;
	}

// ------------------------------------------------------------------------------------------------
// CSmlDataStoreFormat::MimeFormatTxPref()
// Returns TxPref.
// ------------------------------------------------------------------------------------------------
EXPORT_C TInt CSmlDataStoreFormat::MimeFormatTxPref() const
	{
	return iTxPref;
	}

// ------------------------------------------------------------------------------------------------
// CSmlDataStoreFormat::SetMimeFormatTxPref()
// Sets tx preference.
// ------------------------------------------------------------------------------------------------
EXPORT_C void CSmlDataStoreFormat::SetMimeFormatTxPref( TInt aTxPref )
	{
	iTxPref = aTxPref;
	}

// ------------------------------------------------------------------------------------------------
// CSmlDataStoreFormat::IsSupported()
// Check is the option supported.
// ------------------------------------------------------------------------------------------------
EXPORT_C TBool CSmlDataStoreFormat::IsSupported( TOption aOption ) const
	{
	switch ( aOption )
		{
		case EOptionHasMaxSize:
			{
			if( KSmlDataStore_HasMaxSize & iFlags )
				{
				return ETrue;
				}

			break;
			}
		case EOptionHasMaxItems:
			{
			if( KSmlDataStore_HasMaxItems & iFlags )
				{
				return ETrue;
				}

			break;
			}
		case EOptionHierarchial:
			{
			if( KSmlDataStore_Hierarchical & iFlags )
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
// CSmlDataStoreFormat::SetSupportedOptions()
// Set supported options.
// ------------------------------------------------------------------------------------------------
EXPORT_C void CSmlDataStoreFormat::SetSupportedOptions( TUint32 aFlags )
	{
	iFlags = aFlags;
	}

// ------------------------------------------------------------------------------------------------
// CSmlDataStoreFormat::FolderPropertyCount()
// Returns the number of properties.
// ------------------------------------------------------------------------------------------------
EXPORT_C TInt CSmlDataStoreFormat::FolderPropertyCount() const
	{
	return iFolderProperties.Count();	
	}

// ------------------------------------------------------------------------------------------------
// CSmlDataStoreFormat::FolderProperty()
// Returns the indexed folder property.
// ------------------------------------------------------------------------------------------------
EXPORT_C const CSmlDataProperty& CSmlDataStoreFormat::FolderProperty( TInt aIndex ) const
	{
	return *iFolderProperties[aIndex];	
	}
	
// ------------------------------------------------------------------------------------------------
// CSmlDataStoreFormat::SetFolderPropertiesL()
// Sets folder properties.
// ------------------------------------------------------------------------------------------------
EXPORT_C void CSmlDataStoreFormat::SetFolderPropertiesL( RPointerArray<CSmlDataProperty>& aFolderProperties )
	{
	iFolderProperties.ResetAndDestroy();
	for ( TInt i = 0; i < aFolderProperties.Count(); i++ )
		{
		iFolderProperties.AppendL( aFolderProperties[i] );
		}
	aFolderProperties.Reset();
	aFolderProperties.Close();
	}

// ------------------------------------------------------------------------------------------------
// CSmlDataStoreFormat::FilterCapabilityCount()
// Number of properties in the array
// ------------------------------------------------------------------------------------------------
EXPORT_C TInt CSmlDataStoreFormat::FilterCapabilityCount() const
	{
	return iFilterCapabilities.Count();		
	}
	
// ------------------------------------------------------------------------------------------------
// CSmlDataStoreFormat::FilterCapability()
// Returns the indexed filter capability.
// ------------------------------------------------------------------------------------------------
EXPORT_C const CSmlFilterCapability& CSmlDataStoreFormat::FilterCapability( TInt aIndex ) const
	{
	return *iFilterCapabilities[aIndex];
	}
	
// ------------------------------------------------------------------------------------------------
// CSmlDataStoreFormat::SetFilterCapabilitiesL()
// Sets filter capabilities.
// ------------------------------------------------------------------------------------------------
EXPORT_C void CSmlDataStoreFormat::SetFilterCapabilitiesL( RPointerArray<CSmlFilterCapability>& aFilterCapabilities )
	{
	iFilterCapabilities.ResetAndDestroy();
	for ( TInt i = 0; i < aFilterCapabilities.Count(); i++ )
		{
		iFilterCapabilities.AppendL( aFilterCapabilities[i] );
		}
	aFilterCapabilities.Reset();
	aFilterCapabilities.Close();
	}
	
// ------------------------------------------------------------------------------------------------
// CSmlDataStoreFormat::HasMaxSize()
// Returns information is max size used 
// ------------------------------------------------------------------------------------------------
EXPORT_C TBool CSmlDataStoreFormat::HasMaxSize() const
	{		
	if ( iMaxSize != 0 )
		{
		return ETrue;
		}

	return EFalse;
	}
	
// ------------------------------------------------------------------------------------------------
// CSmlDataStoreFormat::HasMaxItems()
// Returns information are max items used.
// ------------------------------------------------------------------------------------------------
EXPORT_C TBool CSmlDataStoreFormat::HasMaxItems() const
	{
	if ( iMaxItems != 0 )
		{
		return ETrue;
		}
		
	return EFalse;
	}

// ------------------------------------------------------------------------------------------------
// CSmlDataStoreFormat::MaxSize()
// Returns max size.
// ------------------------------------------------------------------------------------------------
EXPORT_C TInt CSmlDataStoreFormat::MaxSize() const
	{
	return iMaxSize;
	}

// ------------------------------------------------------------------------------------------------
// CSmlDataStoreFormat::MaxItems()
// Returns the max items count.
// ------------------------------------------------------------------------------------------------
EXPORT_C TInt CSmlDataStoreFormat::MaxItems() const
	{
	return iMaxItems;
	}
	
// ------------------------------------------------------------------------------------------------
// CSmlDataStoreFormat::SetMaxSize()
// ------------------------------------------------------------------------------------------------
EXPORT_C void CSmlDataStoreFormat::SetMaxSize( TUint16 aMaxSize )
	{
	iMaxSize = aMaxSize;
	}
	
// ------------------------------------------------------------------------------------------------
// CSmlDataStoreFormat::SetMaxItems()
// ------------------------------------------------------------------------------------------------
EXPORT_C void CSmlDataStoreFormat::SetMaxItems( TUint16 aMaxItems )
	{
	iMaxItems = aMaxItems;
	}

// End of File
