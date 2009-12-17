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
* Description:  Device information parser class implementation.
*
*/


#include "WBXMLDevInfParser.h"
#include "WBXMLParserError.h"

#include "smlmetinfdtd.h"
#include "xmlelement.h"

// ------------------------------------------------------------------------------------------------
// CWBXMLDevInfDocHandler
// ------------------------------------------------------------------------------------------------
EXPORT_C CWBXMLDevInfDocHandler* CWBXMLDevInfDocHandler::NewL( MWBXMLDevInfCallbacks* aCallbacks )
	{
	CWBXMLDevInfDocHandler* self = new (ELeave) CWBXMLDevInfDocHandler(aCallbacks);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(); // self
	return self;
	}

// ------------------------------------------------------------------------------------------------
EXPORT_C CWBXMLDevInfDocHandler::~CWBXMLDevInfDocHandler()
	{
	if(iCmdStack)
		{
		iCmdStack->Reset();
		}
	delete iCmdStack;

	if(iCleanupStack)
		{
		iCleanupStack->ResetAndDestroy();
		}
	delete iCleanupStack;
	}

// ------------------------------------------------------------------------------------------------
void CWBXMLDevInfDocHandler::StartDocumentL( TUint8 /*aVersion*/, TInt32 /*aPublicId*/, TUint32 /*aCharset*/ )
	{
	}

// ------------------------------------------------------------------------------------------------
void CWBXMLDevInfDocHandler::StartDocumentL( TUint8 /*aVersion*/, const TDesC8& /*aPublicIdStr*/, TUint32 /*aCharset*/ )
	{
	}

// ------------------------------------------------------------------------------------------------
void CWBXMLDevInfDocHandler::EndDocumentL()
	{
	}

// ------------------------------------------------------------------------------------------------
void CWBXMLDevInfDocHandler::StartElementL( TWBXMLTag aTag, const CWBXMLAttributes& /*aAttributes*/ )
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
void CWBXMLDevInfDocHandler::AddElementL( CXMLElement* aElement )
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
void CWBXMLDevInfDocHandler::EndElementL( TWBXMLTag aTag )
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
void CWBXMLDevInfDocHandler::CodePageSwitchL( TUint8 /*aPage*/ )
	{
	}

// ------------------------------------------------------------------------------------------------
void CWBXMLDevInfDocHandler::CharactersL( const TDesC8& aBuffer )
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
void CWBXMLDevInfDocHandler::DocumentChangedL()
	{
	iCmdStack->Reset();
	}

// ------------------------------------------------------------------------------------------------
void CWBXMLDevInfDocHandler::Ext_IL( TWBXMLTag /*aTag*/, const TDesC8& /*aData*/ )
	{
	}

// ------------------------------------------------------------------------------------------------
void CWBXMLDevInfDocHandler::Ext_TL( TWBXMLTag /*aTag*/, TUint32 /*aData*/ )
	{
	}

// ------------------------------------------------------------------------------------------------
void CWBXMLDevInfDocHandler::ExtL( TWBXMLTag /*aTag*/ )
	{
	}

// ------------------------------------------------------------------------------------------------
void CWBXMLDevInfDocHandler::OpaqueL( const TDesC8& aData )
	{
	CharactersL(aData);
	}

// ------------------------------------------------------------------------------------------------
void CWBXMLDevInfDocHandler::ConstructL()
	{
	iCmdStack = CNSmlStack<CXMLElement>::NewL();
	iCleanupStack = CNSmlStack<CXMLElement>::NewL();
	}

// ------------------------------------------------------------------------------------------------
CWBXMLDevInfDocHandler::CWBXMLDevInfDocHandler( MWBXMLDevInfCallbacks* aCallbacks ) : iCallbacks(aCallbacks)
	{
	}
