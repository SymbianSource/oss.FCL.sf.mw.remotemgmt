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
* Description:  wbxml to xml converter
*
*/


#include <s32mem.h>

#include "wbxml2xmlconverter.h"
#include "smlsyncmltags.h"
#include "smlmetinftags.h"

EXPORT_C CWbxml2XmlConverter* CWbxml2XmlConverter::NewL()
	{
	CWbxml2XmlConverter* self = NewLC();
	CleanupStack::Pop(); // self
	return self;
	}

EXPORT_C CWbxml2XmlConverter* CWbxml2XmlConverter::NewLC()
	{
	CWbxml2XmlConverter* self = new (ELeave) CWbxml2XmlConverter();
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

EXPORT_C void CWbxml2XmlConverter::ConvertL( const TUint8* aPtr, TInt32 aLength )
	{
	RMemReadStream mrs(aPtr, aLength);
	doConvertL(mrs);
	}

EXPORT_C void CWbxml2XmlConverter::ConvertL( RReadStream& aSource )
	{
	doConvertL(aSource);
	}

void CWbxml2XmlConverter::doConvertL( RReadStream& aSource )
	{
	iGenerator->Reset();
	CWBXMLParser* parser = CWBXMLParser::NewL();
	CleanupStack::PushL(parser);
	CWBXMLSyncMLDocHandler* dh = CWBXMLSyncMLDocHandler::NewL(this);
	CleanupStack::PushL(dh);
	parser->SetDocumentHandler(dh);
	parser->SetExtensionHandler(dh);
	parser->SetDocumentL(aSource);
	while( parser->ParseL() == KWBXMLParserErrorOk )
		;
	CleanupStack::PopAndDestroy(2); // parser, dh
	}

EXPORT_C TPtrC8 CWbxml2XmlConverter::Document() const
	{
	return iGenerator->Document();
	}

EXPORT_C CWbxml2XmlConverter::~CWbxml2XmlConverter()
	{
	delete iGenerator;
	}

void CWbxml2XmlConverter::ConstructL()
	{
	iGenerator = new (ELeave) CXMLSyncMLGenerator();
	iGenerator->CreateWorkspaceL();
	iGenerator->InitializeL();
	}

Ret_t CWbxml2XmlConverter::smlStartMessageFuncL(SmlSyncHdrPtr_t pContent)
	{
	if( iGenerator )
		{
		iGenerator->smlStartMessage(pContent);
		}
	return KErrNone;
	}

Ret_t CWbxml2XmlConverter::smlEndMessageFuncL(Boolean_t final)
	{
	if( iGenerator )
		{
		iGenerator->smlEndMessage(final);
		}
	return KErrNone;
	}

Ret_t CWbxml2XmlConverter::smlStartSyncFuncL(SmlSyncPtr_t pContent)
	{
	if( iGenerator )
		{
		iGenerator->smlStartSync(pContent);
		}
	return KErrNone;
	}

Ret_t CWbxml2XmlConverter::smlEndSyncFuncL()
	{
	if( iGenerator )
		{
		iGenerator->smlEndSync();
		}
	return KErrNone;
	}

Ret_t CWbxml2XmlConverter::smlStartAtomicFuncL(SmlAtomicPtr_t pContent)
	{
	if( iGenerator )
		{
		iGenerator->smlStartAtomic(pContent);
		}
	return KErrNone;
	}

Ret_t CWbxml2XmlConverter::smlEndAtomicFuncL()
	{
	if( iGenerator )
		{
		iGenerator->smlEndAtomic();
		}
	return KErrNone;
	}

Ret_t CWbxml2XmlConverter::smlStartSequenceFuncL(SmlSequencePtr_t pContent)
	{
	if( iGenerator )
		{
		iGenerator->smlStartSequence(pContent);
		}
	return KErrNone;
	}

Ret_t CWbxml2XmlConverter::smlEndSequenceFuncL()
	{
	if( iGenerator )
		{
		iGenerator->smlEndSequence();
		}
	return KErrNone;
	}

Ret_t CWbxml2XmlConverter::smlAddCmdFuncL(SmlAddPtr_t pContent)
	{
	iGenerator->smlAddCmd(pContent);
	return KErrNone;
	}

Ret_t CWbxml2XmlConverter::smlReplaceCmdFuncL(SmlReplacePtr_t pContent)
	{
	iGenerator->smlReplaceCmd(pContent);
	return KErrNone;
	}

Ret_t CWbxml2XmlConverter::smlAlertCmdFuncL(SmlAlertPtr_t pContent)
	{
	if( iGenerator )
		{
		iGenerator->smlAlertCmd(pContent);
		}
	return KErrNone;
	}

Ret_t CWbxml2XmlConverter::smlDeleteCmdFuncL(SmlDeletePtr_t pContent)
	{
	if( iGenerator )
		{
		iGenerator->smlDeleteCmd(pContent);
		}
	return KErrNone;
	}

Ret_t CWbxml2XmlConverter::smlGetCmdFuncL(SmlGetPtr_t pContent)
	{
	if( iGenerator )
		{
		iGenerator->smlGetCmd(pContent);
		}
	return KErrNone;
	}

Ret_t CWbxml2XmlConverter::smlPutCmdFuncL(SmlPutPtr_t pContent)
	{
	if( iGenerator )
		{
		iGenerator->smlPutCmd(pContent);
		}
	return KErrNone;
	}

Ret_t CWbxml2XmlConverter::smlMapCmdFuncL(SmlMapPtr_t pContent)
	{
	if( iGenerator )
		{
		iGenerator->smlMapCmd(pContent);
		}
	return KErrNone;
	}

Ret_t CWbxml2XmlConverter::smlResultsCmdFuncL(SmlResultsPtr_t pContent)
	{
	if( iGenerator )
		{
		iGenerator->smlResultsCmd(pContent);
		}
	return KErrNone;
	}

Ret_t CWbxml2XmlConverter::smlStatusCmdFuncL(SmlStatusPtr_t pContent)
	{
	if( iGenerator )
		{
		iGenerator->smlStatusCmd(pContent);
		}
	return KErrNone;
	}

Ret_t CWbxml2XmlConverter::smlCopyCmdFuncL(SmlCopyPtr_t pContent)
	{
	if( iGenerator )
		{
		iGenerator->smlCopyCmd(pContent);
		}
	return KErrNone;
	}

Ret_t CWbxml2XmlConverter::smlMoveCmdFuncL(SmlMovePtr_t pContent)
	{
	if( iGenerator )
		{
		iGenerator->smlMoveCmd(pContent);
		}
	return KErrNone;
	}

Ret_t CWbxml2XmlConverter::smlExecCmdFuncL(SmlExecPtr_t pContent)
	{
	// FOTA
	if( iGenerator )
		{
		iGenerator->smlExecCmd(pContent);
		}
	// FOTA end	
	return KErrNone;
	}

Ret_t CWbxml2XmlConverter::smlSearchCmdFuncL(SmlSearchPtr_t /*pContent*/)
	{
	return KErrNone;
	}





