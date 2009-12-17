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
// CNSmlDSResourceProfiles implementation 
// ----------------------------------------------------------

// ----------------------------------------------------------
// CNSmlDSResourceProfiles::~CNSmlDSResourceProfiles()
// ----------------------------------------------------------
CNSmlDSResourceProfiles::~CNSmlDSResourceProfiles()
	{
	if(iProfileArray)
		{
		iProfileArray->ResetAndDestroy();
		}

	delete iProfileArray;
	}


// ----------------------------------------------------------
// CNSmlDSResourceProfiles* CNSmlDSResourceProfiles::NewLC( TResourceReader& aReader, CNSmlDSSettings& aSettings )
// ----------------------------------------------------------
CNSmlDSResourceProfiles* CNSmlDSResourceProfiles::NewLC( TResourceReader& aReader, CNSmlDSSettings* aSettings )
	{
	CNSmlDSResourceProfiles* self = new (ELeave) CNSmlDSResourceProfiles;
	CleanupStack::PushL( self );
	self->ConstructL( aReader, aSettings );
	return self;
	}

// ----------------------------------------------------------
// void CNSmlDSResourceProfiles::SaveProfilesL()
// ----------------------------------------------------------
void CNSmlDSResourceProfiles::SaveProfilesL(CNSmlProfileArray* aArray)
	{
	for ( TInt i = 0; i < iProfileArray->Count(); i++ )
		{
		CNSmlDSProfile* profile = iSettings->CreateProfileL();
		CleanupStack::PushL( profile );
		iProfileArray->At(i)->SaveProfileL( profile );
		profile->SaveL();
		CNSmlDSProfile* tempProfile = iSettings->CreateProfileL();
		tempProfile->SetStrValue(EDSProfileServerId, profile->StrValue(EDSProfileServerId));
		aArray->AppendL(tempProfile);
		CleanupStack::PopAndDestroy(); // profile
		}
	}

// ----------------------------------------------------------
// void CNSmlDSResourceProfiles::ConstructL( TResourceReader& aReader, CNSmlDSSettings* aSettings  )
// ----------------------------------------------------------
void CNSmlDSResourceProfiles::ConstructL( TResourceReader& aReader, CNSmlDSSettings* aSettings )
	{
	iSettings = aSettings;
	iProfileArray = new (ELeave) CArrayPtrFlat<CNSmlDSResourceProfile> (3);
	AddDataL( aReader );
	}

// ----------------------------------------------------------
// void CNSmlDSResourceProfiles::AddDataL( TResourceReader& aReader )
// ----------------------------------------------------------
void CNSmlDSResourceProfiles::AddDataL( TResourceReader& aReader )
	{
	TInt number = aReader.ReadInt16();
	for ( TInt index = 0; index < number; index++ )
		{
		CNSmlDSResourceProfile* profile = CNSmlDSResourceProfile::NewLC( aReader );
		iProfileArray->AppendL( profile );
		CleanupStack::Pop();		
		}
	}

//  End of File
