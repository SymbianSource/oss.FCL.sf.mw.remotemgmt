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
* Description:  SyncML document parser class implementation.
*
*/


// ------------------------------------------------------------------------------------------------
// Includes
// ------------------------------------------------------------------------------------------------
#include "WBXMLSyncMLParser.h"
#include "WBXMLParserError.h"

#include "smlsyncmltags.h"
#include "xmlelement.h"
#include "smlmetinfdtd.h"
#include "smldevinfdtd.h"

// ------------------------------------------------------------------------------------------------
// CWBXMLSyncMLDocHandler
// ------------------------------------------------------------------------------------------------
EXPORT_C CWBXMLSyncMLDocHandler* CWBXMLSyncMLDocHandler::NewL( MWBXMLSyncMLCallbacks* aCallbacks )
	{
	CWBXMLSyncMLDocHandler* self = new (ELeave) CWBXMLSyncMLDocHandler(aCallbacks);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(); // self
	return self;
	}

// ------------------------------------------------------------------------------------------------
EXPORT_C CWBXMLSyncMLDocHandler::~CWBXMLSyncMLDocHandler()
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
void CWBXMLSyncMLDocHandler::StartDocumentL( TUint8 /*aVersion*/, TInt32 /*aPublicId*/, TUint32 /*aCharset*/ )
	{
	}

// ------------------------------------------------------------------------------------------------
void CWBXMLSyncMLDocHandler::StartDocumentL( TUint8 /*aVersion*/, const TDesC8& /*aPublicIdStr*/, TUint32 /*aCharset*/ )
	{
	}

// ------------------------------------------------------------------------------------------------
void CWBXMLSyncMLDocHandler::EndDocumentL()
	{
	}

// ------------------------------------------------------------------------------------------------
void CWBXMLSyncMLDocHandler::StartElementL( TWBXMLTag aTag, const CWBXMLAttributes& /*aAttributes*/ )
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
void CWBXMLSyncMLDocHandler::AddElementL( CXMLElement* aElement )
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
void CWBXMLSyncMLDocHandler::EndElementL( TWBXMLTag aTag )
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
void CWBXMLSyncMLDocHandler::CodePageSwitchL( TUint8 aPage )
	{
	switch( aPage )
		{
		// SyncML
		case 0x00:
			iCmdStack->Pop();
			break;

		// Meta Information
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
		case 0x02:	// 1.2 CHANGES: Properties
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
			} // Changes end
		default: // 1.2 CHANGES: case default added
			{
			User::Leave(KErrParserErrorInvalidDocument);
			}// changes end
		}
	}

// ------------------------------------------------------------------------------------------------
void CWBXMLSyncMLDocHandler::CharactersL( const TDesC8& aBuffer )
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
void CWBXMLSyncMLDocHandler::DocumentChangedL()
	{
	iCmdStack->Reset();
	}

// ------------------------------------------------------------------------------------------------
void CWBXMLSyncMLDocHandler::Ext_IL( TWBXMLTag /*aTag*/, const TDesC8& /*aData*/ )
	{
	}

// ------------------------------------------------------------------------------------------------
void CWBXMLSyncMLDocHandler::Ext_TL( TWBXMLTag /*aTag*/, TUint32 /*aData*/ )
	{
	}

// ------------------------------------------------------------------------------------------------
void CWBXMLSyncMLDocHandler::ExtL( TWBXMLTag /*aTag*/ )
	{
	}

// ------------------------------------------------------------------------------------------------
void CWBXMLSyncMLDocHandler::OpaqueL( const TDesC8& aData )
	{
	CharactersL(aData);
	}

// ------------------------------------------------------------------------------------------------
void CWBXMLSyncMLDocHandler::ConstructL()
	{
	iCmdStack = CNSmlStack<CXMLElement>::NewL();
	iCleanupStack = CNSmlStack<CXMLElement>::NewL();
	}

// ------------------------------------------------------------------------------------------------
CWBXMLSyncMLDocHandler::CWBXMLSyncMLDocHandler( MWBXMLSyncMLCallbacks* aCallbacks ) : iCallbacks(aCallbacks)
	{
	}

