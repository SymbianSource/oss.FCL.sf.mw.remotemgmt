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
* Description:  WBXML attribute classes.
*
*/


#ifndef __WBXMLATTRIBUTES_H__
#define __WBXMLATTRIBUTES_H__

// ------------------------------------------------------------------------------------------------
// Includes
// ------------------------------------------------------------------------------------------------

#include <e32base.h>

#include "WBXMLDefs.h"

// ------------------------------------------------------------------------------------------------
// CWBXMLAttribute
// ------------------------------------------------------------------------------------------------

class CWBXMLAttribute : CBase
	{
public:
	static CWBXMLAttribute* NewL( TWBXMLTag aName, const TDesC8& aValue );
	~CWBXMLAttribute();
	IMPORT_C TWBXMLTag Name() const;
	IMPORT_C TPtrC8 Value() const;

private:
	void ConstructL( TWBXMLTag aName, const TDesC8& aValue );

private:
	TWBXMLTag iName;
	HBufC8* iValue;
	};

// ------------------------------------------------------------------------------------------------
// CWBXMLAttributes
// ------------------------------------------------------------------------------------------------

class CWBXMLAttributes : public CBase
	{
public:
	IMPORT_C static CWBXMLAttributes* NewL();
	IMPORT_C static CWBXMLAttributes* NewLC();
	~CWBXMLAttributes();

	IMPORT_C TInt Count() const;
	IMPORT_C const CWBXMLAttribute& Attribute( TInt aIndex ) const;
	IMPORT_C void AddAttributeL( TWBXMLTag aName, const TDesC8& aValue );

private:
	void ConstructL();

private:
	CArrayPtr<CWBXMLAttribute>* iAttributeList;
	};

#endif // __WBXMLATTRIBUTES_H__