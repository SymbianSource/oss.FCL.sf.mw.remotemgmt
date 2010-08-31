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
* Description:  DM-settings 
*
*/



// INCLUDE FILES
#include "nsmldmsettings.h"
#include "NSmlDMResourceProfile.h"
#include <featmgr.h>
const TInt KNSmlDmBluetoothType = 0x101F99F1;
// ----------------------------------------------------------
// CNSmlDMResourceProfiles implementation 
// ----------------------------------------------------------

// ----------------------------------------------------------
// CNSmlDMResourceProfiles::~CNSmlDMResourceProfiles()
// ----------------------------------------------------------
CNSmlDMResourceProfiles::~CNSmlDMResourceProfiles()
	{
	if(iProfileArray)
		{
		iProfileArray->ResetAndDestroy();			
		}
	delete iProfileArray;
	FeatureManager::UnInitializeLib();
	}

// ----------------------------------------------------------
// CNSmlDMResourceProfiles* CNSmlDMResourceProfiles::NewLC( TResourceReader& aReader, CNSmlDMSettings& aSettings )
// ----------------------------------------------------------
CNSmlDMResourceProfiles* CNSmlDMResourceProfiles::NewLC( TResourceReader& aReader, CNSmlDMSettings* aSettings )
	{
	CNSmlDMResourceProfiles* self = new (ELeave) CNSmlDMResourceProfiles;
	CleanupStack::PushL( self );
	self->ConstructL( aReader, aSettings );
	return self;
	}
// ----------------------------------------------------------
// void CNSmlDMResourceProfiles::RemoveDuplicateEntry()
// ----------------------------------------------------------
void CNSmlDMResourceProfiles::RemoveDuplicateEntry()
{
	TInt count = iProfileArray->Count();
	for ( TInt i = 0; i < count; i++ )
	{
		for(TInt j = i+1; j <count; j++)
		{
			if((iProfileArray->At(i)->GetServerId()->Des().Compare(iProfileArray->At(j)->GetServerId()->Des()) == 0) &&
			(iProfileArray->At(i)->GetServerId()->Length() == iProfileArray->At(j)->GetServerId()->Length()))
			{
				iProfileArray->Delete(j);
				iProfileArray->Compress();
				j--;
				count--;
			}
		}
	}
}
// ----------------------------------------------------------
// void CNSmlDMResourceProfiles::SaveProfilesL()
// ----------------------------------------------------------
void CNSmlDMResourceProfiles::SaveProfilesL()
	{
	RemoveDuplicateEntry();
	RemoveObexEntryL();
	for ( TInt i = 0; i < iProfileArray->Count(); i++ )
		{
		CNSmlDMProfile* profile = iSettings->CreateProfileL();
		CleanupStack::PushL( profile );
		iProfileArray->At(i)->SaveProfileL( profile );
		profile->SaveL();
		CleanupStack::PopAndDestroy(); // profile
		}
	}

// ----------------------------------------------------------
// void CNSmlDMResourceProfiles::ConstructL( TResourceReader& aReader )
// ----------------------------------------------------------
void CNSmlDMResourceProfiles::ConstructL( TResourceReader& aReader, CNSmlDMSettings* aSettings )
	{
	iSettings = aSettings;
	iProfileArray = new (ELeave) CArrayPtrFlat<CNSmlDMResourceProfile> (3);
	FeatureManager::InitializeLibL();
	AddDataL( aReader );
	}

// ----------------------------------------------------------
// void CNSmlDMResourceProfiles::AddDataL( TResourceReader& aReader )
// ----------------------------------------------------------
void CNSmlDMResourceProfiles::AddDataL( TResourceReader& aReader )
	{
	TInt index;
	TInt number = aReader.ReadInt16();
	for ( index = 0; index < number; index++ )
		{
		CNSmlDMResourceProfile* profile = CNSmlDMResourceProfile::NewLC( aReader );
		iProfileArray->AppendL( profile );
		CleanupStack::Pop();		
		}
	}

// ----------------------------------------------------------
// void CNSmlDMResourceProfiles::RemoveObexEntryL( )
// ----------------------------------------------------------	
void CNSmlDMResourceProfiles::RemoveObexEntryL()
 {   
    TInt count = iProfileArray->Count();
    for ( TInt i = 0; i < count; i++ )
    {
        if((! FeatureManager::FeatureSupported ( KFeatureIdSyncMlDmObex )) &&
		   ( iProfileArray->At(i)->iTransportId == KNSmlDmBluetoothType))
         {
            iProfileArray->Delete(i);
            iProfileArray->Compress();
            i--;
            count--;            
         }               
    } 
 }
//  End of File
