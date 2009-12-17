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
* Description:  Dynamic workspace class implementation.
*
*/


// ------------------------------------------------------------------------------------------------
// Includes
// ------------------------------------------------------------------------------------------------
#include <s32mem.h>

#include "XMLDynamicWorkspace.h"

// ------------------------------------------------------------------------------------------------
// CXMLDynamicWorkspace
// ------------------------------------------------------------------------------------------------
CXMLDynamicWorkspace::CXMLDynamicWorkspace()
	{
	}

// ------------------------------------------------------------------------------------------------
EXPORT_C CXMLDynamicWorkspace* CXMLDynamicWorkspace::NewL()
	{
	CXMLDynamicWorkspace* self = new (ELeave) CXMLDynamicWorkspace();
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(); // self
	return self;
	}

// ------------------------------------------------------------------------------------------------
void CXMLDynamicWorkspace::ConstructL()
	{
	iBuffer = CBufFlat::NewL(32);
	CXMLWorkspace::ConstructL();
	}

// ------------------------------------------------------------------------------------------------
EXPORT_C CXMLDynamicWorkspace::~CXMLDynamicWorkspace()
	{
	delete iBuffer;
	}

// ------------------------------------------------------------------------------------------------
EXPORT_C void CXMLDynamicWorkspace::Reset()
	{
	iBuffer->Reset();
	}

// ------------------------------------------------------------------------------------------------
EXPORT_C void CXMLDynamicWorkspace::WriteL( const TDesC8& aData )
	{
	TRAPD(err, iBuffer->InsertL(iBuffer->Size(), aData));
	if( err != KErrNone )
		{
		User::Leave(KErrTooBig);
		}
	}

// ------------------------------------------------------------------------------------------------
EXPORT_C void CXMLDynamicWorkspace::WriteL( const TUint8 aData )
	{
	WriteL(TPtrC8(&aData, 1));
	}

// ------------------------------------------------------------------------------------------------
EXPORT_C TPtrC8 CXMLDynamicWorkspace::Buffer() const
	{
	return iBuffer->Ptr(0);
	}

// ------------------------------------------------------------------------------------------------
EXPORT_C TInt CXMLDynamicWorkspace::FreeSize() const
	{
	return (KMaxTInt32 - Size());
	}

// ------------------------------------------------------------------------------------------------
EXPORT_C TInt CXMLDynamicWorkspace::MaxSize() const
	{
	return KMaxTInt32;
	}

// ------------------------------------------------------------------------------------------------
EXPORT_C TInt CXMLDynamicWorkspace::Size() const
	{
	return iBuffer->Size();
	}

// ------------------------------------------------------------------------------------------------
void CXMLDynamicWorkspace::Delete( TInt aPos, TInt aLength )
	{
	iBuffer->Delete(aPos, aLength);
	}
