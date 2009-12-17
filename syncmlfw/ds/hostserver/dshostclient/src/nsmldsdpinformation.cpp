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
#include <badesca.h>

#include "nsmldsdpinformation.h"
	
// ================================= MEMBER FUNCTIONS =============================================

// ------------------------------------------------------------------------------------------------
// CNSmlDPInformation::NewL
// Creates new instance of CNSmlDPInformation based class.
// ------------------------------------------------------------------------------------------------
EXPORT_C CNSmlDPInformation* CNSmlDPInformation::NewL()
	{	
	CNSmlDPInformation* self = NewLC();
	CleanupStack::Pop();
	return self;
	}
	
// ------------------------------------------------------------------------------------------------
// CNSmlDPInformation::NewLC
// Creates new instance of CNSmlDPInformation based class.
// ------------------------------------------------------------------------------------------------
EXPORT_C CNSmlDPInformation* CNSmlDPInformation::NewLC()
	{
	CNSmlDPInformation* self = new (ELeave) CNSmlDPInformation();
	CleanupStack::PushL( self );
	self->ConstructL();
	return self;
	}

// ------------------------------------------------------------------------------------------------
// CNSmlDPInformation::CNSmlDPInformation
// C++ Destructor.
// ------------------------------------------------------------------------------------------------
EXPORT_C CNSmlDPInformation::~CNSmlDPInformation()
	{
	if ( iInfo )
		{
		delete iInfo->iDisplayName;
		delete iInfo->iMimeTypes;
		delete iInfo->iMimeVersions;
		delete iInfo;
		}
	}

// ------------------------------------------------------------------------------------------------
// CNSmlDPInformation::SetId
// ------------------------------------------------------------------------------------------------
EXPORT_C void CNSmlDPInformation::SetId(TSmlDataProviderId aId) const
	{
	iInfo->iId = aId;
	}
// ------------------------------------------------------------------------------------------------
// CNSmlDPInformation::DisplayName
// ------------------------------------------------------------------------------------------------
EXPORT_C const HBufC* CNSmlDPInformation::DisplayName() const
	{
	return iInfo->iDisplayName;
	}

// ------------------------------------------------------------------------------------------------
// CNSmlDPInformation::SetDisplayName
// ------------------------------------------------------------------------------------------------
EXPORT_C void CNSmlDPInformation::SetDisplayNameL( const TDesC& aDisplayName )
	{
	delete iInfo->iDisplayName;
	iInfo->iDisplayName = NULL;
	iInfo->iDisplayName = aDisplayName.AllocL();
	}
	
// ------------------------------------------------------------------------------------------------
// CNSmlDPInformation::MimeType
// ------------------------------------------------------------------------------------------------
EXPORT_C const CDesC8Array* CNSmlDPInformation::MimeTypes() const
	{
	return iInfo->iMimeTypes;
	}

// ------------------------------------------------------------------------------------------------
// CNSmlDPInformation::SetMimeTypeL
// ------------------------------------------------------------------------------------------------		
EXPORT_C void CNSmlDPInformation::SetMimeTypesL( CDesC8Array* aMimeTypes )
	{
	delete iInfo->iMimeTypes;
	iInfo->iMimeTypes = aMimeTypes;
	}

// ------------------------------------------------------------------------------------------------
// CNSmlDPInformation::MimeVer
// ------------------------------------------------------------------------------------------------
EXPORT_C const CDesC8Array* CNSmlDPInformation::MimeVers() const
	{
	return iInfo->iMimeVersions;
	}

// ------------------------------------------------------------------------------------------------
// CNSmlDPInformation::SetMimeVerL
// ------------------------------------------------------------------------------------------------		
EXPORT_C void CNSmlDPInformation::SetMimeVersL( CDesC8Array* aMimeVersions )
	{
	delete iInfo->iMimeVersions;
	iInfo->iMimeVersions = aMimeVersions;
	}

// ------------------------------------------------------------------------------------------------
// CNSmlDPInformation::ProtocolVersion
// ------------------------------------------------------------------------------------------------
EXPORT_C TSmlProtocolVersion CNSmlDPInformation::ProtocolVersion() const
	{
	return iInfo->iProtocolVer;
	}

// ------------------------------------------------------------------------------------------------
// CNSmlDPInformation::SetProtocolVersion
// ------------------------------------------------------------------------------------------------
EXPORT_C void CNSmlDPInformation::SetProtocolVersion( TSmlProtocolVersion aProtocol )
	{
	iInfo->iProtocolVer = aProtocol;
	}

// ------------------------------------------------------------------------------------------------
// CNSmlDPInformation::Version
// ------------------------------------------------------------------------------------------------
EXPORT_C const TVersion& CNSmlDPInformation::Version() const
	{
	return iInfo->iVersion;
	}

// ------------------------------------------------------------------------------------------------
// CNSmlDPInformation::SetVersion
// ------------------------------------------------------------------------------------------------
EXPORT_C void CNSmlDPInformation::SetVersion( const TVersion& aVersion )
	{
	iInfo->iVersion = aVersion;
	}

// ------------------------------------------------------------------------------------------------
// CNSmlDPInformation::InformationD
// ------------------------------------------------------------------------------------------------
EXPORT_C TNSmlDPInformation* CNSmlDPInformation::InformationD()
	{
	TNSmlDPInformation* ret = iInfo;
	iInfo = NULL;
	delete this;
	return ret;
	}

// ------------------------------------------------------------------------------------------------
// CNSmlDPInformation::ExternalizeL
// ------------------------------------------------------------------------------------------------
EXPORT_C void CNSmlDPInformation::ExternalizeL( RWriteStream& aStream ) const
	{
	TPckgC<TSmlDataProviderId> id( iInfo->iId );
	aStream.WriteL( id );
	
	//version
	aStream.WriteInt32L( iInfo->iVersion.iMajor );
	aStream.WriteInt32L( iInfo->iVersion.iMinor );
	aStream.WriteInt32L( iInfo->iVersion.iBuild );
	
	aStream.WriteInt32L( iInfo->iProtocolVer );
	
	aStream.WriteUint32L( iInfo->iDisplayName->Length() );
	aStream << *iInfo->iDisplayName;
	
	TInt mimetypesCount( iInfo->iMimeTypes->Count() );
	aStream.WriteUint32L( mimetypesCount );
	
	for (TInt i = 0; i < mimetypesCount; i++ )
		{
		const TDesC8& mimetype = (*iInfo->iMimeTypes)[i];
		aStream.WriteUint32L( mimetype.Length() );
		aStream << mimetype;
		}
		
	TInt mimeversCount( iInfo->iMimeVersions->Count() );
	aStream.WriteUint32L( mimeversCount );
	
	for (TInt i = 0; i < mimeversCount; i++ )
		{
		const TDesC8& mimever = (*iInfo->iMimeVersions)[i];
		aStream.WriteUint32L( mimever.Length() );
		aStream << mimever;
		}
	}

// ------------------------------------------------------------------------------------------------
// CNSmlDPInformation::InternalizeL
// ------------------------------------------------------------------------------------------------
EXPORT_C void CNSmlDPInformation::InternalizeL( RReadStream& aStream )
	{
	TPckg<TSmlDataProviderId> id( iInfo->iId );
	aStream.ReadL( id );
	
	iInfo->iVersion.iMajor = aStream.ReadInt32L();
	iInfo->iVersion.iMinor = aStream.ReadInt32L();
	iInfo->iVersion.iBuild = aStream.ReadInt32L();
	
	iInfo->iProtocolVer = static_cast<TSmlProtocolVersion>( aStream.ReadInt32L() );
	
	delete iInfo->iDisplayName;
	iInfo->iDisplayName = NULL;
	iInfo->iDisplayName = HBufC::NewL( aStream, aStream.ReadUint32L() );
	
	delete iInfo->iMimeTypes;
	iInfo->iMimeTypes = NULL;
	TInt mimetypesCount = aStream.ReadUint32L();
	CDesC8Array* mimetypes = new (ELeave) CDesC8ArrayFlat( Max<TInt>( 2, mimetypesCount ) );
	CleanupStack::PushL( mimetypes );
	
	for (TInt i = 0 ; i < mimetypesCount; i++ )
		{
		HBufC8* tempBuf8 = HBufC8::NewLC( aStream, aStream.ReadUint32L() );
		mimetypes->AppendL( *tempBuf8 );
		CleanupStack::PopAndDestroy(); // tempBuf8
		}
	
	delete iInfo->iMimeVersions;
	iInfo->iMimeVersions = NULL;
	TInt mimeversCount = aStream.ReadUint32L();
	CDesC8Array* mimevers = new (ELeave) CDesC8ArrayFlat( Max<TInt>( 2, mimeversCount ) );
	CleanupStack::PushL( mimevers );
	
	for (TInt i = 0 ; i < mimeversCount; i++ )
		{
		HBufC8* tempBuf8 = HBufC8::NewLC( aStream, aStream.ReadUint32L() );
		mimevers->AppendL( *tempBuf8 );
		CleanupStack::PopAndDestroy(); // tempBuf8
		}
		
	iInfo->iMimeTypes = mimetypes;
	iInfo->iMimeVersions = mimevers;
	
	CleanupStack::Pop(2); // mimetypes, mimevers
	}

// ------------------------------------------------------------------------------------------------
// CNSmlDPInformation::CNSmlDPInformation
// ------------------------------------------------------------------------------------------------
CNSmlDPInformation::CNSmlDPInformation() : iInfo( NULL )
	{
	
	}

// ------------------------------------------------------------------------------------------------
// CNSmlDPInformation::ConstructL
// ------------------------------------------------------------------------------------------------
void CNSmlDPInformation::ConstructL()
	{
	iInfo = new ( ELeave ) TNSmlDPInformation;
	iInfo->iId = 0;
	iInfo->iProtocolVer = ESmlVersion1_1_2;
	iInfo->iDisplayName = NULL;
	iInfo->iMimeTypes = NULL;
	iInfo->iMimeVersions = NULL;
	
	iInfo->iDisplayName = HBufC::NewL( 0 );
	iInfo->iMimeTypes = new ( ELeave ) CDesC8ArrayFlat( 2 );
	iInfo->iMimeVersions = new ( ELeave ) CDesC8ArrayFlat( 2 );
	}
		
// End of File
