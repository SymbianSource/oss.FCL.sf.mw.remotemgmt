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
* Description:  XML element class implementation.
*
*/


// ------------------------------------------------------------------------------------------------
// Includes
// ------------------------------------------------------------------------------------------------

#include "xmlelement.h"
#include "smldtd.h"

// ------------------------------------------------------------------------------------------------
// TElementParams
// ------------------------------------------------------------------------------------------------
EXPORT_C TXMLElementParams::TXMLElementParams( TAny* aCallbacks, CNSmlStack<CXMLElement>* aCmdStack, CNSmlStack<CXMLElement>* aCleanupStack ) :
	iCallbacks(aCallbacks), iCmdStack(aCmdStack), iCleanupStack(aCleanupStack)
	{
	}

// ------------------------------------------------------------------------------------------------
// CXMLElement
// ------------------------------------------------------------------------------------------------
EXPORT_C CXMLElement::CXMLElement()
	{
	}

EXPORT_C CXMLElement::~CXMLElement()
	{
	}

EXPORT_C CXMLElement* CXMLElement::BeginElementL( TUint8 /*aTag*/, const TXMLElementParams& /*Params*/ )
	{
	return 0;
	}

// ------------------------------------------------------------------------------------------------
EXPORT_C CXMLElement::TAction CXMLElement::EndElementL( TAny* /*aCallbacks*/, TUint8 /*aTag*/ )
	{
	return EPop;
	}


// ------------------------------------------------------------------------------------------------
EXPORT_C TBool CXMLElement::NeedsCleanup() const
	{
	return EFalse;
	}

// ------------------------------------------------------------------------------------------------
EXPORT_C void CXMLElement::SetDataL( const TDesC8& /*aData*/ )
	{
	}

// ------------------------------------------------------------------------------------------------
EXPORT_C TPtrC8 CXMLElement::Data() const
	{
	return TPtrC8();
	}

// ------------------------------------------------------------------------------------------------
EXPORT_C TBool CXMLElement::Validate()
	{
	return ETrue;
	}
