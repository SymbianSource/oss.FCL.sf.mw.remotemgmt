/*
* Copyright (c) 2004 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Resource reader for DS-settings 
*
*/



// INCLUDE FILES
#include "NSmlDSResourceProfile.h"



// ----------------------------------------------------------
// CNSmlDSResourceContentType implementation 
// ----------------------------------------------------------

// ----------------------------------------------------------
// CNSmlDSResourceContentType::~CNSmlDSResourceContentType()
// ----------------------------------------------------------
CNSmlDSResourceContentType::~CNSmlDSResourceContentType()
	{
	delete iClientDataSource;
	delete iServerDataSource;
	delete iFilterVisibility;
	delete iSyncTypeVisibility;
	}

// ----------------------------------------------------------
// CNSmlDSResourceContentType* CNSmlDSResourceContentType::NewLC( TResourceReader& aReader )
// ----------------------------------------------------------
CNSmlDSResourceContentType* CNSmlDSResourceContentType::NewLC( TResourceReader& aReader )
	{
	CNSmlDSResourceContentType* self = new (ELeave) CNSmlDSResourceContentType;
	CleanupStack::PushL( self );
	self->ConstructL( aReader );
	return self;
	}

// ----------------------------------------------------------
// TBool CNSmlDSResourceContentType::SaveContentL( CNSmlDSProfile* iProfile )
// ----------------------------------------------------------
TBool CNSmlDSResourceContentType::SaveContentL( CNSmlDSProfile* iProfile )
	{
	// visibilities of filters and sync type
	if ( iFilterVisibility && iFilterVisibility->Length() == 1)
		{
		TNSmlDSFieldVisibility filtersVisibility = iProfile->CharToVisibility( (*iFilterVisibility)[0] );
		}
	if ( iSyncTypeVisibility && iSyncTypeVisibility->Length() == 1)
		{
		TNSmlDSFieldVisibility syncTypeVisibility = iProfile->CharToVisibility( (*iSyncTypeVisibility)[0] );
		}

	iProfile->AddContentTypeL( iImplementationId, *iClientDataSource, *iServerDataSource );
	CNSmlDSContentType* content = iProfile->ContentType( iImplementationId );
	content->SetIntValue( EDSAdapterSyncType, iSyncType );
	content->SetIntValue( EDSAdapterCreatorId, iCreatorId);
	content->SetIntValue( EDSAdapterEnabled, iEnabled);

	return ETrue;
	}

// ----------------------------------------------------------
// void CNSmlDSResourceContentType::ConstructL( TResourceReader& aReader )
// ----------------------------------------------------------
void CNSmlDSResourceContentType::ConstructL( TResourceReader& aReader )
	{
	TRAPD( error, AddDataL( aReader ) );
	if ( error )
		{
		User::Leave( error );
		}
	}

// ----------------------------------------------------------
// void CNSmlDSResourceContentType::AddDataL( TResourceReader& aReader )
// ----------------------------------------------------------
void CNSmlDSResourceContentType::AddDataL( TResourceReader& aReader )
	{
	iImplementationId = aReader.ReadInt32();

	iClientDataSource = aReader.ReadHBufC16L();
	
	if ( iClientDataSource == NULL )
		{
		iClientDataSource = KNullDesC().AllocL();	
		}
	iServerDataSource = aReader.ReadHBufC16L();

	iSyncType = aReader.ReadInt16();
	
	iFilterVisibility = aReader.ReadHBufC16L();
	iSyncTypeVisibility = aReader.ReadHBufC16L();
	
	iCreatorId = aReader.ReadInt32();
	iEnabled = aReader.ReadInt8();
	}

//  End of File
