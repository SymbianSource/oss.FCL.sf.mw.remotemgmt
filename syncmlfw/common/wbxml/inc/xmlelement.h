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
* Description:  Base class for every element struct.
*
*/


#ifndef __XMLELEMENT_H__
#define __XMLELEMENT_H__

// ------------------------------------------------------------------------------------------------
// Includes
// ------------------------------------------------------------------------------------------------

#include <e32base.h>
#include "NSmlStack.h"

// ------------------------------------------------------------------------------------------------
// Class forwards
// ------------------------------------------------------------------------------------------------
class MWBXMLSyncMLCallbacks;
struct TXMLElementParams;

// ------------------------------------------------------------------------------------------------
// CXMLElement
// ------------------------------------------------------------------------------------------------
class CXMLElement : public CBase
	{
public:
	enum TAction 
		{
		ENone,
		EPop,
		EPopAndDestroy // Pop from stack, callback, delete
		};

	IMPORT_C CXMLElement();
	IMPORT_C virtual ~CXMLElement();

	IMPORT_C virtual void SetDataL( const TDesC8& aData );
	IMPORT_C virtual TPtrC8 Data() const;
	IMPORT_C virtual TBool Validate();
	
	IMPORT_C virtual TBool NeedsCleanup() const;
	IMPORT_C virtual CXMLElement* BeginElementL( TUint8 aTag, const TXMLElementParams& aParams );
	IMPORT_C virtual TAction EndElementL( TAny* aCallbacks, TUint8 aTag );
	};

// ------------------------------------------------------------------------------------------------
// TElementParams
// ------------------------------------------------------------------------------------------------
struct TXMLElementParams
	{
public:
	inline TXMLElementParams();
	IMPORT_C TXMLElementParams( TAny* aCallbacks, CNSmlStack<CXMLElement>* aCmdStack = 0, CNSmlStack<CXMLElement>* aCleanupStack = 0 );

	inline TAny* Callbacks() const;
	inline CNSmlStack<CXMLElement>* CmdStack() const;
	inline CNSmlStack<CXMLElement>* CleanupStack() const;
private:
	TAny* iCallbacks;
	CNSmlStack<CXMLElement>* iCmdStack;
	CNSmlStack<CXMLElement>* iCleanupStack;
	};

#include "xmlelement.inl"

#endif // __XMLELEMENT_H__