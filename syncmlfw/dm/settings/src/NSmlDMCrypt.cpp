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


//------------------------------------------------------------------------------
//
//		CNSmlDMCrypt
//
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//		CNSmlDMCrypt::CNSmlDMCrypt()
//		Constructor.
//------------------------------------------------------------------------------
EXPORT_C CNSmlDMCrypt::CNSmlDMCrypt()
	{
	}

//------------------------------------------------------------------------------
//		CNSmlDMCrypt::~CNSmlDMCrypt()
//		Destructor.
//------------------------------------------------------------------------------
EXPORT_C CNSmlDMCrypt::~CNSmlDMCrypt()
	{
	delete iOutput;
	}

//------------------------------------------------------------------------------
//		CNSmlDMCrypt::EncryptedL()
//		Encrypts 
//------------------------------------------------------------------------------
EXPORT_C TDesC& CNSmlDMCrypt::EncryptedL( const TDesC& aInput )
	{
	delete iOutput;
	iOutput = 0;

	iOutput = aInput.AllocL();

	TPtr ptrOutput( iOutput->Des() );
	
	// Key definitions to be used in encryption
	TUint16 key1 = 0xa576;
	TUint16 key2 = 0x02f9;

	for (TInt i = 0; i < iOutput->Length(); i++ )
		{
		key1 %= key2;
		key1 <<= 7;
		key1 ^= key2;
		key2 %= key1;
		key2 <<= 3;
		key2 ^= key1;
		ptrOutput[i] ^= key1;  
		}

	return *iOutput;
	}

//------------------------------------------------------------------------------
//		CNSmlDMCrypt::DecryptedL()
//		Destructor.
//------------------------------------------------------------------------------
EXPORT_C TDesC& CNSmlDMCrypt::DecryptedL( const TDesC& aInput )
	{
	return EncryptedL( aInput );
	}

//  End of File
