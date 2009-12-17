/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Static workspace class implementation.
*
*/


// ------------------------------------------------------------------------------------------------
// Includes
// ------------------------------------------------------------------------------------------------
#include "XMLStaticWorkspace.h"

// ------------------------------------------------------------------------------------------------
// CXMLStaticWorkspace
// ------------------------------------------------------------------------------------------------
CXMLStaticWorkspace::CXMLStaticWorkspace()
	{
	}

// ------------------------------------------------------------------------------------------------
EXPORT_C CXMLStaticWorkspace* CXMLStaticWorkspace::NewL( TInt aBufferSize )
	{
	CXMLStaticWorkspace* self = new (ELeave) CXMLStaticWorkspace();
	CleanupStack::PushL(self);
	self->ConstructL(aBufferSize);
	CleanupStack::Pop(); // self
	return self;
	}

// ------------------------------------------------------------------------------------------------
void CXMLStaticWorkspace::ConstructL( TInt aBufferSize )
	{
	iMaxLength = aBufferSize;
	iBuffer = HBufC8::NewL(aBufferSize);
	CXMLWorkspace::ConstructL();
	}

// ------------------------------------------------------------------------------------------------
EXPORT_C CXMLStaticWorkspace::~CXMLStaticWorkspace()
	{
	delete iBuffer;
	}

// ------------------------------------------------------------------------------------------------
EXPORT_C void CXMLStaticWorkspace::Reset()
	{
	Ptr().Zero();
	}

// ------------------------------------------------------------------------------------------------
EXPORT_C void CXMLStaticWorkspace::WriteL( const TDesC8& aData )
	{
	if( FreeSize() < aData.Length() )
		{
		User::Leave(KErrTooBig);
		}
	Ptr().Append(aData);
	}

// ------------------------------------------------------------------------------------------------
EXPORT_C void CXMLStaticWorkspace::WriteL( const TUint8 aData )
	{
	WriteL(TPtrC8(&aData, 1));
	}

// ------------------------------------------------------------------------------------------------
EXPORT_C TPtrC8 CXMLStaticWorkspace::Buffer() const
	{
	return *iBuffer;
	}

// ------------------------------------------------------------------------------------------------
EXPORT_C TInt CXMLStaticWorkspace::FreeSize() const
	{
	return iMaxLength - Ptr().Length();
	}

// ------------------------------------------------------------------------------------------------
EXPORT_C TInt CXMLStaticWorkspace::MaxSize() const
	{
	return iMaxLength;
	}

// ------------------------------------------------------------------------------------------------
EXPORT_C TInt CXMLStaticWorkspace::Size() const
	{
	return Ptr().Length();
	}

// ------------------------------------------------------------------------------------------------
void CXMLStaticWorkspace::Delete( TInt aPos, TInt aLength )
	{
	Ptr().Delete(aPos, aLength);
	}

// End of file
