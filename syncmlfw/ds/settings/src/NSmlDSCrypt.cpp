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
* Description:  DS-settings 
*
*/



// INCLUDE FILES
#include "nsmldssettings.h"


//=============================================
//
//		CNSmlDSCrypt
//
//=============================================

//=============================================
//		CNSmlDSCrypt::CNSmlDSCrypt()
//		Constructor.
//=============================================
EXPORT_C CNSmlDSCrypt::CNSmlDSCrypt()
	{
	}

//=============================================
//		CNSmlDSCrypt::~CNSmlDSCrypt()
//		Destructor.
//=============================================
EXPORT_C CNSmlDSCrypt::~CNSmlDSCrypt()
	{
	delete iOutput;
	}

//=============================================
//		CNSmlDSCrypt::EncryptedL()
//		Destructor.
//=============================================
EXPORT_C TDesC& CNSmlDSCrypt::EncryptedL( const TDesC& aInput )
	{
	delete iOutput;
	iOutput = 0;

	iOutput = aInput.AllocL();

	TPtr ptrOutput( iOutput->Des() );
	
	// Keys to be used in encryption / decryption.
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

//=============================================
//		CNSmlDSCrypt::DecryptedL()
//		Destructor.
//=============================================
EXPORT_C TDesC& CNSmlDSCrypt::DecryptedL( const TDesC& aInput )
	{
	return EncryptedL( aInput );
	}

//  End of File
