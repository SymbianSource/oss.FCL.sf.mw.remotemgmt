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
* Description:  WBXML attribute classes implementation.
*
*/


// ------------------------------------------------------------------------------------------------
// Includes
// ------------------------------------------------------------------------------------------------
#include "WBXMLAttributes.h"

// ------------------------------------------------------------------------------------------------
// CWBXMLAttribute
// ------------------------------------------------------------------------------------------------
CWBXMLAttribute* CWBXMLAttribute::NewL( TWBXMLTag aName, const TDesC8& aValue )
	{
	CWBXMLAttribute* self = new (ELeave) CWBXMLAttribute();
	CleanupStack::PushL(self);
	self->ConstructL(aName, aValue);
	CleanupStack::Pop(); // self
	return self;
	}

// ------------------------------------------------------------------------------------------------
CWBXMLAttribute::~CWBXMLAttribute()
	{
	delete iValue;
	}

// ------------------------------------------------------------------------------------------------
void CWBXMLAttribute::ConstructL( TWBXMLTag aName, const TDesC8& aValue )
	{
	iName = aName;
	iValue = aValue.AllocL();
	}

// ------------------------------------------------------------------------------------------------
EXPORT_C TWBXMLTag CWBXMLAttribute::Name() const
	{
	return iName;
	}

// ------------------------------------------------------------------------------------------------
EXPORT_C TPtrC8 CWBXMLAttribute::Value() const
	{
	return *iValue;
	}

// ------------------------------------------------------------------------------------------------
// CWBXMLAttributes
// ------------------------------------------------------------------------------------------------
EXPORT_C CWBXMLAttributes* CWBXMLAttributes::NewL()
	{
	CWBXMLAttributes* self = NewLC();
	CleanupStack::Pop();
	return self;
	}

// ------------------------------------------------------------------------------------------------
EXPORT_C CWBXMLAttributes* CWBXMLAttributes::NewLC()
	{
	CWBXMLAttributes* self = new (ELeave) CWBXMLAttributes();
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

// ------------------------------------------------------------------------------------------------
CWBXMLAttributes::~CWBXMLAttributes()
	{
	if ( iAttributeList )
		{
		iAttributeList->ResetAndDestroy();
		delete iAttributeList;
		}
	}

// ------------------------------------------------------------------------------------------------
void CWBXMLAttributes::ConstructL()
	{
	iAttributeList = new (ELeave) CArrayPtrFlat<CWBXMLAttribute>(2);
	}

// ------------------------------------------------------------------------------------------------
EXPORT_C TInt CWBXMLAttributes::Count() const
	{
	return iAttributeList->Count();
	}

// ------------------------------------------------------------------------------------------------
EXPORT_C const CWBXMLAttribute& CWBXMLAttributes::Attribute( TInt aIndex ) const
	{
	return *(iAttributeList->operator[](aIndex));
	}

// ------------------------------------------------------------------------------------------------
EXPORT_C void CWBXMLAttributes::AddAttributeL( TWBXMLTag aName, const TDesC8& aValue )
	{
	CWBXMLAttribute* attr = CWBXMLAttribute::NewL(aName, aValue);
	iAttributeList->AppendL(attr);
	}
