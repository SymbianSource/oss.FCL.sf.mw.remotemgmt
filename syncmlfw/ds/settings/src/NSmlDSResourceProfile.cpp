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
// CNSmlDSResourceProfile implementation 
// ----------------------------------------------------------

// ----------------------------------------------------------
// CNSmlDSResourceProfile::~CNSmlDSResourceProfile()
// ----------------------------------------------------------
CNSmlDSResourceProfile::~CNSmlDSResourceProfile()
	{
	if (iContentTypeArray)
		{
		iContentTypeArray->ResetAndDestroy();
		delete iContentTypeArray;
		}

	delete iProfileDisplayName;
	delete iServerUserName;
	delete iServerPassWord;
	delete iServerURL;

	delete iServerId;
	delete iVisibilityStr;
	
	delete iHttpAuthUsername;
	delete iHttpAuthPassword;
	}

// ----------------------------------------------------------
// CNSmlDSResourceProfile* CNSmlDSResourceProfile::NewLC( TResourceReader& aReader )
// ----------------------------------------------------------
CNSmlDSResourceProfile* CNSmlDSResourceProfile::NewLC( TResourceReader& aReader )
	{
	CNSmlDSResourceProfile* self = new (ELeave) CNSmlDSResourceProfile;
	CleanupStack::PushL( self );
	self->ConstructL( aReader );
	return self;
	}

// ----------------------------------------------------------
// TBool CNSmlDSResourceProfile::SaveProfileL( CNSmlDSProfile* aProfile )
// ----------------------------------------------------------
TBool CNSmlDSResourceProfile::SaveProfileL( CNSmlDSProfile* aProfile )
	{
	for ( TInt i = 0; i < iContentTypeArray->Count(); i++ )
		{
		iContentTypeArray->At(i)->SaveContentL( aProfile );
		}
	if(iProfileDisplayName)
		{
		aProfile->SetStrValue( EDSProfileDisplayName, iProfileDisplayName->Des() );
		}
	aProfile->SetIntValue( EDSProfileIAPId, iIapId );
	aProfile->SetIntValue( EDSProfileTransportId, iTransportId );
	if(iServerUserName)
		{
		aProfile->SetStrValue( EDSProfileSyncServerUsername, iServerUserName->Des() );
		}
	if(iServerPassWord)
		{
		aProfile->SetStrValue( EDSProfileSyncServerPassword, iServerPassWord->Des() );
		}
	if(iServerURL)
		{
		aProfile->SetStrValue( EDSProfileServerURL, iServerURL->Des() );
		}
	if(iServerId)
		{
		aProfile->SetStrValue( EDSProfileServerId, iServerId->Des() );
		}
	aProfile->SetIntValue( EDSProfileServerAlertedAction, iServerAlertedAction );
	aProfile->SetIntValue( EDSProfileDeleteAllowed, iDeleteAllowed );
	aProfile->SetIntValue( EDSProfileHidden, iProfileHidden );
	if(iVisibilityStr)
		{
		aProfile->InitVisibilityArray( iVisibilityStr->Des() );
		}
	aProfile->SetIntValue( EDSProfileProtocolVersion, iProtocolVersion );
	aProfile->SetIntValue( EDSProfileHttpAuthUsed, iHttpAuthUsed );
	if( iHttpAuthPassword )
		{
		aProfile->SetStrValue( EDSProfileHttpAuthPassword, *iHttpAuthPassword );
		}
	if( iHttpAuthUsername )
		{
		aProfile->SetStrValue( EDSProfileHttpAuthUsername, *iHttpAuthUsername );
		}
	aProfile->SetIntValue( EDSProfileCreatorId, iCreatorId );
	
	return ETrue;
	}

// ----------------------------------------------------------
// void CNSmlDSResourceProfile::ConstructL( TResourceReader& aReader )
// ----------------------------------------------------------
void CNSmlDSResourceProfile::ConstructL( TResourceReader& aReader )
	{
	iContentTypeArray = new (ELeave) CArrayPtrFlat<CNSmlDSResourceContentType> (3);
	AddDataL( aReader );
	}

// ----------------------------------------------------------
// void CNSmlDSResourceProfile::AddDataL( TResourceReader& aReader )
// ----------------------------------------------------------
void CNSmlDSResourceProfile::AddDataL( TResourceReader& aReader )
	{
	TInt number = aReader.ReadInt16();
	for ( TInt index = 0; index < number; index++ )
		{
		CNSmlDSResourceContentType* contentType = CNSmlDSResourceContentType::NewLC( aReader );
		iContentTypeArray->AppendL( contentType );
		CleanupStack::Pop();		
		}
	iProfileDisplayName = aReader.ReadHBufC16L();

	iIapId = aReader.ReadInt16();
	iTransportId = aReader.ReadInt32();

	iServerUserName = aReader.ReadHBufC16L();
	iServerPassWord = aReader.ReadHBufC16L();
	iServerURL = aReader.ReadHBufC16L();
	iServerId = aReader.ReadHBufC16L();
	
	iServerAlertedAction = aReader.ReadInt16();
	iDeleteAllowed = aReader.ReadInt8();
	iProfileHidden = aReader.ReadInt8();

	iVisibilityStr = aReader.ReadHBufC16L();

	iProtocolVersion = aReader.ReadInt16();
	
	iCreatorId = aReader.ReadInt32();
	iHttpAuthUsed = aReader.ReadInt8();
	iHttpAuthPassword = aReader.ReadHBufC16L();
	iHttpAuthUsername = aReader.ReadHBufC16L();		
	
	}
// End of File
