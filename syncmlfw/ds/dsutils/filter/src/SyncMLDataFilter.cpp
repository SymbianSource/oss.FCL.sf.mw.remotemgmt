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
#include <SyncMLDataFilter.h>
#include <SyncMLDataFilter.hrh>


// CONSTANTS
const TInt KSizeofTInt32 = sizeof(TInt32);
const TInt KSizeofTInt16 = sizeof(TInt16);
const TInt KSizeofTInt8 = sizeof(TInt8);


// ============================ MEMBER FUNCTIONS ===============================
// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
// CSyncMLFilter::NewLC
// Symbian two-phased constructor.
// -----------------------------------------------------------------------------
EXPORT_C CSyncMLFilter* CSyncMLFilter::NewLC()
	{
	CSyncMLFilter* self = new (ELeave) CSyncMLFilter();
	CleanupStack::PushL( self );

	self->iFilterId = 0;
	self->iDisplayName = KNullDesC().AllocL();
	self->iDisplayDescription = KNullDesC().AllocL();
	
	return self;
	}

// -----------------------------------------------------------------------------
// CSyncMLFilter::NewLC
// Symbian two-phased constructor.
// -----------------------------------------------------------------------------
EXPORT_C CSyncMLFilter* CSyncMLFilter::NewLC( TResourceReader& aReader )
	{
	CSyncMLFilter* self = new (ELeave) CSyncMLFilter();
	CleanupStack::PushL( self );
	
	// Create filter from resource file
	// BYTE version
	TInt version( aReader.ReadInt8() );
	
	// WORD filter ID
	self->iFilterId = aReader.ReadInt16();
	
	// LTEXT display name
	self->iDisplayName = aReader.ReadHBufC16L();
	
	// LTEXT display description
	self->iDisplayDescription = aReader.ReadHBufC16L();
	
	// BYTE enabled
	if ( aReader.ReadInt8() > 0 )
		{
		self->iEnabled = ETrue;
		}
	
	// BYTE supported by server
	if ( aReader.ReadInt8() > 0 )
		{
		self->iSupportedByServer = ETrue;
		}
	
	// BYTE match type supported
	if ( aReader.ReadInt8() > 0 )
		{
		self->iMatchTypeSupported = ETrue;
		}
	
	// STRUCT filter properties
	TInt number = aReader.ReadInt16();
	for ( TInt i = 0; i < number; i++)
		{
		CSyncMLFilterProperty* property = CSyncMLFilterProperty::NewLC( aReader );
		self->iProperties.AppendL( property );
		CleanupStack::Pop(); // property
		}
	
	return self;
	}
	
// -----------------------------------------------------------------------------
// CSyncMLFilter::NewLC
// Symbian two-phased constructor.
// -----------------------------------------------------------------------------
EXPORT_C CSyncMLFilter* CSyncMLFilter::NewLC( RReadStream& aStream )
	{
	CSyncMLFilter* self = new (ELeave) CSyncMLFilter();
	CleanupStack::PushL( self );
		
	// Create filter from stream
	// filter ID
	self->iFilterId = aStream.ReadInt16L();

    // display name	
	TInt length( aStream.ReadInt32L() );
	self->iDisplayName = HBufC::NewL( aStream, length );
	
	// display description
	length = aStream.ReadInt32L();
	self->iDisplayDescription = HBufC::NewL( aStream, length );	
	
	// BYTE enabled
	if ( aStream.ReadInt8L() > 0 )
		{
		self->iEnabled = ETrue;
		}
	
	// BYTE supported by server
	if ( aStream.ReadInt8L() > 0 )
		{
		self->iSupportedByServer = ETrue;
		}
	
	// BYTE match type supported
	if ( aStream.ReadInt8L() > 0 )
		{
		self->iMatchTypeSupported = ETrue;
		}
	
	// STRUCT filter properties
	TInt number = aStream.ReadInt32L();
	
	for ( TInt i = 0; i < number; i++)
		{
		CSyncMLFilterProperty* property = CSyncMLFilterProperty::NewLC( aStream );
		self->iProperties.AppendL( property );
		CleanupStack::Pop(); // property
		}
	
	return self;
	}

// -----------------------------------------------------------------------------
// CSyncMLFilter::~CSyncMLFilter
// Destructor.
// -----------------------------------------------------------------------------
EXPORT_C CSyncMLFilter::~CSyncMLFilter()
	{
	delete iDisplayName;
  	delete iDisplayDescription;
	iProperties.ResetAndDestroy();
	iProperties.Close();
	}

// -----------------------------------------------------------------------------
// CSyncMLFilter::ExternalizeL
// Externalizes filter.
// -----------------------------------------------------------------------------
EXPORT_C void CSyncMLFilter::ExternalizeL( RWriteStream& aStream ) const
	{
	// filter ID
	aStream.WriteInt16L( iFilterId );
	
	// display name
	if ( iDisplayName )
		{
		aStream.WriteInt32L( iDisplayName->Length() );
		aStream << iDisplayName->Des();
		}
	else
		{
		aStream.WriteInt32L( 0 );
		aStream << KNullDesC;
		}
		
	// display description
	if ( iDisplayDescription )
		{
		aStream.WriteInt32L( iDisplayDescription->Length() );
		aStream << iDisplayDescription->Des();
		}
	else
		{
		aStream.WriteInt32L( 0 );
		aStream << KNullDesC;
		}
		
	// enabled
	aStream.WriteInt8L( iEnabled );
	
	// supported by server
	aStream.WriteInt8L( iSupportedByServer );
	
	// match type supported
	aStream.WriteInt8L( iMatchTypeSupported );
	
	// filter properties
	aStream.WriteInt32L( iProperties.Count() );
	for ( TInt i = 0; i < iProperties.Count(); i++)
		{
		iProperties[i]->ExternalizeL( aStream );
		}
	
	}

// -----------------------------------------------------------------------------
// CSyncMLFilter::FilterId
// ID of filter.
// -----------------------------------------------------------------------------
EXPORT_C TUint CSyncMLFilter::FilterId() const
	{
	return iFilterId;
	}
	
// -----------------------------------------------------------------------------
// CSyncMLFilter::DisplayName
// Name of filter.
// -----------------------------------------------------------------------------
EXPORT_C const TDesC& CSyncMLFilter::DisplayName() const
	{
	return *iDisplayName;
	}
	
// -----------------------------------------------------------------------------
// CSyncMLFilter::DisplayDescription
// Description of filter.
// -----------------------------------------------------------------------------
EXPORT_C const TDesC& CSyncMLFilter::DisplayDescription() const
	{
	return *iDisplayDescription;
	}

// -----------------------------------------------------------------------------
// CSyncMLFilter::Enabled
// Filter enable/disable information.
// -----------------------------------------------------------------------------
EXPORT_C TBool CSyncMLFilter::Enabled() const
	{
	return iEnabled;
	}
	
// -----------------------------------------------------------------------------
// CSyncMLFilter::SetEnabledL
// Set filter enable/disable information.
// -----------------------------------------------------------------------------
EXPORT_C void CSyncMLFilter::SetEnabledL( const TBool aEnabled )
	{
	iEnabled = aEnabled;
	}

// -----------------------------------------------------------------------------
// CSyncMLFilter::SupportedByServer
// Information about server capabilities to support this filter.
// -----------------------------------------------------------------------------
EXPORT_C TBool CSyncMLFilter::SupportedByServer() const
	{
	return iSupportedByServer;
	}

// -----------------------------------------------------------------------------
// CSyncMLFilter::SetSupportedByServerL
// Set information about server capabilities to support this filter.
// -----------------------------------------------------------------------------
EXPORT_C void CSyncMLFilter::SetSupportedByServerL( const TBool aSupported )
	{
	iSupportedByServer = aSupported;
	}

// -----------------------------------------------------------------------------
// CSyncMLFilter::MatchTypeSupported
// Information about match type effect to filter.
// -----------------------------------------------------------------------------
EXPORT_C TBool CSyncMLFilter::MatchTypeSupported() const
	{
	return iMatchTypeSupported;
	}

// -----------------------------------------------------------------------------
// CSyncMLFilter::SetMatchTypeSupported
// Set information about match type effect to filter.
// -----------------------------------------------------------------------------
EXPORT_C void CSyncMLFilter::SetMatchTypeSupported( const TBool aSupported )
	{
	iMatchTypeSupported = aSupported;
	}

// -----------------------------------------------------------------------------
// CSyncMLFilter::FilterProperties
// Properties of filter.
// -----------------------------------------------------------------------------
EXPORT_C RPointerArray<CSyncMLFilterProperty>& CSyncMLFilter::FilterProperties()
	{
	return iProperties;
	}

// -----------------------------------------------------------------------------
// CSyncMLFilter::DataSize
// Data size of filter.
// -----------------------------------------------------------------------------
EXPORT_C TInt CSyncMLFilter::DataSize()
	{
	TInt size(0);
	
	// iFilterId
	size += KSizeofTInt16;
	
	// length of DisplayName
	size += KSizeofTInt32;
	
	if ( iDisplayName )
		{
		// header information
		size += iDisplayName->Length() + 1;
		// header information is needed for every 64 part
		size += iDisplayName->Length() / 64;
		}
		
	// length of DisplayDescription
	size += KSizeofTInt32;
	
	if ( iDisplayDescription )
		{
		// header information
		size += iDisplayDescription->Length() + 1;
		// header information is needed for every 64 part
		size += iDisplayDescription->Length() / 64;
		}
	
	// iEnabled
	size += KSizeofTInt8;
	
	// iSupportedByServer
	size += KSizeofTInt8;
	
	// iMatchTypeSupported
	size += KSizeofTInt8;
		
	//count of properties for this filter
	size += KSizeofTInt32;
	for (TInt i = 0; i < FilterProperties().Count(); i++ )
		{
		size += iProperties[i]->DataSize();
		}
		
	return size;
	}
// -----------------------------------------------------------------------------
// CSyncMLFilter::CSyncMLFilter
// Basic constructor.
// -----------------------------------------------------------------------------
CSyncMLFilter::CSyncMLFilter() :
    iFilterId( 0 ),
    iEnabled( EFalse ),
    iSupportedByServer( EFalse )
	{
	}
	
// End of file
