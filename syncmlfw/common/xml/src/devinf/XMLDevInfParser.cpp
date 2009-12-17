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
* Description:  XML device information DTD parser
*
*/


#include "XMLDevInfParser.h"
#include "WBXMLParserError.h"

#include "smlmetinfdtd.h"
#include "xmlelement.h"

// ------------------------------------------------------------------------------------------------
// CXMLDevInfDocHandler
// ------------------------------------------------------------------------------------------------
EXPORT_C CXMLDevInfDocHandler* CXMLDevInfDocHandler::NewL( MWBXMLDevInfCallbacks* aCallbacks )
	{
	CXMLDevInfDocHandler* self = new (ELeave) CXMLDevInfDocHandler(aCallbacks);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(); // self
	return self;
	}

// ------------------------------------------------------------------------------------------------
EXPORT_C CXMLDevInfDocHandler::~CXMLDevInfDocHandler()
	{
	if(iCmdStack)
		{
		iCmdStack->Reset();
		}
	
	delete iCmdStack;
	delete iCleanupStack;
	}

// ------------------------------------------------------------------------------------------------
void CXMLDevInfDocHandler::StartDocumentL( TUint8 /*aVersion*/, TInt32 /*aPublicId*/, TUint32 /*aCharset*/ )
	{
	}

// ------------------------------------------------------------------------------------------------
void CXMLDevInfDocHandler::StartDocumentL( TUint8 /*aVersion*/, const TDesC8& /*aPublicIdStr*/, TUint32 /*aCharset*/ )
	{
	}

// ------------------------------------------------------------------------------------------------
void CXMLDevInfDocHandler::EndDocumentL()
	{
	}

// ------------------------------------------------------------------------------------------------
void CXMLDevInfDocHandler::StartElementL( TWBXMLTag aTag, const CWBXMLAttributes& /*aAttributes*/ )
	{
	if( iCmdStack->Top() != 0 )
		{
		AddElementL(iCmdStack->Top()->BeginElementL(aTag, TXMLElementParams(iCallbacks, iCmdStack, iCleanupStack)));
		}
	else
		{
		if( aTag == EDevDevInf )
			{
			AddElementL(new (ELeave) SmlDevInfDevInf_t());
			}
		else
			{
			User::Leave(KWBXMLParserErrorInvalidTag);
			}
		}
	}

// ------------------------------------------------------------------------------------------------
void CXMLDevInfDocHandler::AddElementL( CXMLElement* aElement )
	{
	if( aElement )
		{
		iCmdStack->Push(aElement);
		if( aElement->NeedsCleanup() )
			{
			iCleanupStack->Push(aElement);
			}
		}
	}

// ------------------------------------------------------------------------------------------------
void CXMLDevInfDocHandler::EndElementL( TWBXMLTag aTag )
	{
	if( iCmdStack->Top() != 0 )
		{
		CXMLElement::TAction action = iCmdStack->Top()->EndElementL(iCallbacks, aTag);
		if( action != CXMLElement::ENone )
			{
			CXMLElement* elem = iCmdStack->Pop();
			if( iCleanupStack->Top() == elem )
				{
				iCleanupStack->Pop();
				}
			if( action == CXMLElement::EPopAndDestroy )
				{
				delete elem;
				}
			}
		}
	}

// ------------------------------------------------------------------------------------------------
void CXMLDevInfDocHandler::CodePageSwitchL( TUint8 /*aPage*/ )
	{
	}

// ------------------------------------------------------------------------------------------------
void CXMLDevInfDocHandler::CharactersL( const TDesC8& aBuffer )
	{
	if( iCmdStack->Top() != 0 )
		{
		iCmdStack->Top()->SetDataL(aBuffer);
		}
	else
		{
		User::Leave(KWBXMLParserErrorInvalidTag);
		}
	}

// ------------------------------------------------------------------------------------------------
void CXMLDevInfDocHandler::DocumentChangedL()
	{
	iCmdStack->Reset();
	}

// ------------------------------------------------------------------------------------------------
void CXMLDevInfDocHandler::Ext_IL( TWBXMLTag /*aTag*/, const TDesC8& /*aData*/ )
	{
	}

// ------------------------------------------------------------------------------------------------
void CXMLDevInfDocHandler::Ext_TL( TWBXMLTag /*aTag*/, TUint32 /*aData*/ )
	{
	}

// ------------------------------------------------------------------------------------------------
void CXMLDevInfDocHandler::ExtL( TWBXMLTag /*aTag*/ )
	{
	}

// ------------------------------------------------------------------------------------------------
void CXMLDevInfDocHandler::OpaqueL( const TDesC8& aData )
	{
	CharactersL(aData);
	}

// ------------------------------------------------------------------------------------------------
void CXMLDevInfDocHandler::ConstructL()
	{
	iCmdStack = CNSmlStack<CXMLElement>::NewL();
	iCleanupStack = CNSmlStack<CXMLElement>::NewL();
	}

// ------------------------------------------------------------------------------------------------
CXMLDevInfDocHandler::CXMLDevInfDocHandler( MWBXMLDevInfCallbacks* aCallbacks ) : iCallbacks(aCallbacks)
	{
	}
