/*
* Copyright (c) 2002-2004 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Resource reader for DM-settings 
*
*/


#include "NSmlDMResourceProfile.h"
#include "nsmldmsettings.h"


// ----------------------------------------------------------
// CNSmlDMResourceProfile implementation 
// ----------------------------------------------------------

// ----------------------------------------------------------
// CNSmlDMResourceProfile::~CNSmlDMResourceProfile()
// ----------------------------------------------------------
CNSmlDMResourceProfile::~CNSmlDMResourceProfile()
	{
	delete iProfileDisplayName;
	delete iUserName;
	delete iPassWord;
	delete iServerURL;
	delete iServerId;
	delete iClientPassword;
	delete iHttpAuthUsername;
	delete iHttpAuthPassword;
	}

// ----------------------------------------------------------
// CNSmlDMResourceProfile* CNSmlDMResourceProfile::NewLC()
// ----------------------------------------------------------
CNSmlDMResourceProfile* CNSmlDMResourceProfile::NewLC( )
	{
	CNSmlDMResourceProfile* self = new (ELeave) CNSmlDMResourceProfile;
	CleanupStack::PushL( self );
	return self;
	}

// ----------------------------------------------------------
// CNSmlDMResourceProfile* CNSmlDMResourceProfile::NewLC( TResourceReader& aReader )
// ----------------------------------------------------------
CNSmlDMResourceProfile* CNSmlDMResourceProfile::NewLC( TResourceReader& aReader )
	{
	CNSmlDMResourceProfile* self = new (ELeave) CNSmlDMResourceProfile;
	CleanupStack::PushL( self );
	self->ConstructL( aReader );
	return self;
	}

// ----------------------------------------------------------
// TBool CNSmlDMResourceProfile::SaveProfileL()
// ----------------------------------------------------------
TBool CNSmlDMResourceProfile::SaveProfileL( CNSmlDMProfile* aProfile )
	{
	if(iProfileDisplayName)
		{
		aProfile->SetStrValue( EDMProfileDisplayName, iProfileDisplayName->Des() );
		}
	aProfile->SetIntValue( EDMProfileIAPId, iIapId );
	aProfile->SetIntValue( EDMProfileTransportId, iTransportId);
	aProfile->SetIntValue( EDMProfileSessionId, iDMSessionId );
	if(iServerId)
		{
		aProfile->SetStrValue( EDMProfileServerId, iServerId->Des() );
		}	
	if(iUserName)
		{
		aProfile->SetStrValue( EDMProfileServerUsername, iUserName->Des() );
		}
	if(iPassWord)
		{
		aProfile->SetStrValue( EDMProfileServerPassword, iPassWord->Des() );
		}
	if(iServerURL)
		{	
		aProfile->SetStrValue( EDMProfileServerURL, iServerURL->Des() );
		}
	if(iClientPassword)
		{
		aProfile->SetStrValue( EDMProfileClientPassword, iClientPassword->Des() );
		}	
	aProfile->SetIntValue( EDMProfileServerAlertAction, iServerAlertAction );
	aProfile->SetIntValue( EDMProfileHidden, iProfileHidden );
	aProfile->SetIntValue( EDMProfileAuthenticationRequired, iAuthenticationRequired );
	aProfile->SetIntValue( EDMProfileCreatorId, iCreatorId );
	aProfile->SetIntValue( EDMProfileDeleteAllowed, iDeleteAllowed );
	
	aProfile->SetIntValue( EDMProfileHttpAuthUsed, iHttpAuthUsed );
	
	if ( iHttpAuthUsername )
		{
		aProfile->SetStrValue( EDMProfileHttpAuthUsername, iHttpAuthUsername->Des() );
		}
	
	if ( iHttpAuthPassword )
		{
		aProfile->SetStrValue( EDMProfileHttpAuthPassword, iHttpAuthPassword->Des() );
		}	
	aProfile->SetIntValue( EDMProfileLock, iProfileLock );

	return ETrue;
	}

// ----------------------------------------------------------
// void CNSmlDMResourceProfile::ConstructL( TResourceReader& aReader )
// ----------------------------------------------------------
void CNSmlDMResourceProfile::ConstructL( TResourceReader& aReader )
	{
	AddDataL( aReader );
	}

// ----------------------------------------------------------
// HBufC* CNSmlDMResourceProfile::GetServerId()
// ----------------------------------------------------------
HBufC* CNSmlDMResourceProfile::GetServerId()
	{
		return iServerId;
	}

// ----------------------------------------------------------
// void CNSmlDMResourceProfile::AddDataL( TResourceReader& aReader )
// ----------------------------------------------------------
void CNSmlDMResourceProfile::AddDataL( TResourceReader& aReader )
	{
	iProfileDisplayName = aReader.ReadHBufC16L();
	iIapId = aReader.ReadInt16();
	iTransportId = aReader.ReadInt32();
	iUserName = aReader.ReadHBufC16L();
	iPassWord = aReader.ReadHBufC16L();
	iClientPassword = aReader.ReadHBufC16L();
	iServerURL = aReader.ReadHBufC16L();
	iServerId = aReader.ReadHBufC16L();
	iDMSessionId = aReader.ReadInt32();
	iServerAlertAction = aReader.ReadInt8();
	iAuthenticationRequired = aReader.ReadInt8();
	iCreatorId = aReader.ReadInt32();
	iDeleteAllowed = aReader.ReadInt8();
	iProfileHidden = aReader.ReadInt8();
	iHttpAuthUsed = aReader.ReadInt8();
	iHttpAuthUsername = aReader.ReadHBufC16L();
	iHttpAuthPassword = aReader.ReadHBufC16L();
    iProfileLock = aReader.ReadInt8();
	}

