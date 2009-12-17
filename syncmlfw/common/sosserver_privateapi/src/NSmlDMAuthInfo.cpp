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
* Description:  
*
*/


#include "nsmldmauthinfo.h"
#include "nsmlsosserverdefs.h"

// ---------------------------------------------------------
// CNSmlDMAuthInfo::CNSmlDMAuthInfo()
// Constructor
// ---------------------------------------------------------
//	
EXPORT_C CNSmlDMAuthInfo::CNSmlDMAuthInfo()
	{
	}

// ---------------------------------------------------------
// CNSmlDMAuthInfo::CNSmlDMAuthInfo()
// Destructor
// ---------------------------------------------------------
//	
EXPORT_C CNSmlDMAuthInfo::~CNSmlDMAuthInfo()
	{
	
	if ( iServerNonce )
		{
		delete iServerNonce;
		}
	
	if ( iClientNonce )
		{
		delete iClientNonce;
		}
	
	}

// ---------------------------------------------------------
// TInt CNSmlDMAuthInfo::DataSize()
// Returns the size of data
// ---------------------------------------------------------
//	
TInt CNSmlDMAuthInfo::DataSize() const
	{
	TInt size(0);
	
	size += KSizeofTInt32;  	// Server nonce length
	size += iServerNonce->Length()+1;
	size += KSizeofTInt32; 		// Client nonce length
	size += iClientNonce->Length()+1;
	size += KSizeofTInt32; 		// Auth required
	
	return size;
	}
	
// ---------------------------------------------------------
// void CNSmlDMAuthInfo::InternalizeL( RReadStream& aStream )
// From base class. Reads data from given stream.
// ---------------------------------------------------------
//	
void CNSmlDMAuthInfo::InternalizeL( RReadStream& aStream )
	{
	TInt length(0);
	
	length = aStream.ReadInt32L();
	iServerNonce = HBufC8::NewL( aStream, length );
	
	length = aStream.ReadInt32L();
	iClientNonce = HBufC8::NewL( aStream, length );
	
	iAuthPref = aStream.ReadInt32L();
	}

// ---------------------------------------------------------
// void CNSmlDMAuthInfo::ExternalizeL( RWriteStream& aStream ) const
// From base class. Writes data to given stream.
// ---------------------------------------------------------
//	
void CNSmlDMAuthInfo::ExternalizeL( RWriteStream& aStream ) const
	{
	aStream.WriteInt32L( iServerNonce->Size() );
	aStream << *iServerNonce;
	
	aStream.WriteInt32L( iClientNonce->Size() );
	aStream << *iClientNonce;
	
	aStream.WriteInt32L( iAuthPref );
	aStream.CommitL();
	}

