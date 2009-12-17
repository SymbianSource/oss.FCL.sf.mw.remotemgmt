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
* Description:  XML syncml DTD parser
*
*/


// ------------------------------------------------------------------------------------------------
// Includes
// ------------------------------------------------------------------------------------------------
#include "XMLSyncMLParser.h"
#include "WBXMLParserError.h"

#include "smlsyncmltags.h"
#include "xmlelement.h"
#include "smlmetinfdtd.h"
#include "smldevinfdtd.h"

// ------------------------------------------------------------------------------------------------
// CXMLSyncMLDocHandler
// ------------------------------------------------------------------------------------------------
EXPORT_C CXMLSyncMLDocHandler* CXMLSyncMLDocHandler::NewL( MWBXMLSyncMLCallbacks* aCallbacks )
	{
	CXMLSyncMLDocHandler* self = new (ELeave) CXMLSyncMLDocHandler(aCallbacks);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(); // self
	return self;
	}

// ------------------------------------------------------------------------------------------------
EXPORT_C CXMLSyncMLDocHandler::~CXMLSyncMLDocHandler()
	{
	if(iCmdStack)
		{
		iCmdStack->Reset();
		}
	delete iCmdStack;

	if(iCleanupStack)
		{
		iCleanupStack->Reset();
		}
	delete iCleanupStack;
	}

// ------------------------------------------------------------------------------------------------
void CXMLSyncMLDocHandler::StartDocumentL( TUint8 /*aVersion*/, TInt32 /*aPublicId*/, TUint32 /*aCharset*/ )
	{
	}

// ------------------------------------------------------------------------------------------------
void CXMLSyncMLDocHandler::StartDocumentL( TUint8 /*aVersion*/, const TDesC8& /*aPublicIdStr*/, TUint32 /*aCharset*/ )
	{
	}

// ------------------------------------------------------------------------------------------------
void CXMLSyncMLDocHandler::EndDocumentL()
	{
	}

// ------------------------------------------------------------------------------------------------
void CXMLSyncMLDocHandler::StartElementL( TWBXMLTag aTag, const CWBXMLAttributes& /*aAttributes*/ )
	{
	if( iCmdStack->Top() != 0 )
		{
		AddElementL(iCmdStack->Top()->BeginElementL(aTag, TXMLElementParams(iCallbacks)));
		}
	else
		{
		if( aTag == ESyncML )
			{
			AddElementL(new (ELeave) SmlSyncML_t());
			}
		else
			{
			User::Leave(KWBXMLParserErrorInvalidTag);
			}
		}
	}

// ------------------------------------------------------------------------------------------------
void CXMLSyncMLDocHandler::AddElementL( CXMLElement* aElement )
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
void CXMLSyncMLDocHandler::EndElementL( TWBXMLTag aTag )
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
void CXMLSyncMLDocHandler::CodePageSwitchL( TUint8 aPage )
	{
	switch( aPage )
		{
		case 0x00:
            {
			iCmdStack->Pop();
			break;
            }
		case 0x01:
            {
			SmlPcdataPtr_t top = (SmlPcdataPtr_t)iCmdStack->Top();
			// If top element already contains content then push it into
			// command stack, create new otherwise.
			if( !top->content )
				{
				top->length = 0;
				top->contentType = SML_PCDATA_EXTENSION;
				top->extension = SML_EXT_METINF;
				top->content = new (ELeave) SmlMetInfMetInf_t();
				iCmdStack->Push(SmlMetInfMetInfPtr_t(top->content));
				}
			else
				{
				iCmdStack->Push(SmlMetInfMetInfPtr_t(top->content));
				}
			break;
            }
        case 0x02:
            {
			SmlPcdataPtr_t top = (SmlPcdataPtr_t)iCmdStack->Top();
			// If top element already contains content then push it into
			// command stack, create new otherwise.
			if( !top->content )
				{
				top->length = 0;
				top->contentType = SML_PCDATA_EXTENSION;
				top->extension = SML_EXT_DEVINFPROP;
				top->content = new (ELeave) SmlDevInfCtCap_t();
				iCmdStack->Push((SmlDevInfCtCapPtr_t) top->content);
				}
			else
				{
				iCmdStack->Push((SmlDevInfCtCapPtr_t) top->content);
				}
			break;
            }
		default:
			User::Leave( KWBXMLParserErrorInvalidTag );
		}
	}

// ------------------------------------------------------------------------------------------------
void CXMLSyncMLDocHandler::CharactersL( const TDesC8& aBuffer )
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
void CXMLSyncMLDocHandler::DocumentChangedL()
	{
	iCmdStack->Reset();
	}

// ------------------------------------------------------------------------------------------------
void CXMLSyncMLDocHandler::Ext_IL( TWBXMLTag /*aTag*/, const TDesC8& /*aData*/ )
	{
	}

// ------------------------------------------------------------------------------------------------
void CXMLSyncMLDocHandler::Ext_TL( TWBXMLTag /*aTag*/, TUint32 /*aData*/ )
	{
	}

// ------------------------------------------------------------------------------------------------
void CXMLSyncMLDocHandler::ExtL( TWBXMLTag /*aTag*/ )
	{
	}

// ------------------------------------------------------------------------------------------------
void CXMLSyncMLDocHandler::OpaqueL( const TDesC8& aData )
	{
	CharactersL(aData);
	}

// ------------------------------------------------------------------------------------------------
void CXMLSyncMLDocHandler::ConstructL()
	{
	iCmdStack = CNSmlStack<CXMLElement>::NewL();
	iCleanupStack = CNSmlStack<CXMLElement>::NewL();
	}

// ------------------------------------------------------------------------------------------------
CXMLSyncMLDocHandler::CXMLSyncMLDocHandler( MWBXMLSyncMLCallbacks* aCallbacks ) : iCallbacks(aCallbacks)
	{
	}

