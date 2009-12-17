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
* Description:  XML generator
*
*/


// ------------------------------------------------------------------------------------------------
// Includes
// ------------------------------------------------------------------------------------------------
#include "WBXMLDefs.h"
#include "XMLGenerator.h"
#include "XMLStaticWorkspace.h"
#include "XMLDynamicWorkspace.h"
#include "WBXMLGeneratorError.h"

_LIT8(KWBXMLTagStart, "<");
_LIT8(KWBXMLTagStartEndTag, "</");
_LIT8(KWBXMLTagEndNoContent, "/>");
_LIT8(KWBXMLTagEnd, ">");
_LIT8(KWBXMLElemenentSeparator, "|");
_LIT8(KNamespaceBegin, " xmlns='");
_LIT8(KNamespaceEnd, "'");
_LIT8(KIndent, "   ");
_LIT8(KNewLine, "\r\n");

// ------------------------------------------------------------------------------------------------
// CXMLGenerator
// ------------------------------------------------------------------------------------------------
EXPORT_C TPtrC8 CXMLGenerator::Document()
	{
	return Workspace()->Buffer();
	}

// ------------------------------------------------------------------------------------------------
EXPORT_C void CXMLGenerator::Reset()
	{
	iElemStack.Reset();
	if( iOwnsWorkspace )
		{
		Workspace()->Reset();
		}
	iNSAppended = EFalse;
	}

// ------------------------------------------------------------------------------------------------
EXPORT_C void CXMLGenerator::CreateStaticWorkspaceL( TInt aBufferSize )
	{
	FreeWorkspace();
	iWorkspace = CXMLStaticWorkspace::NewL(aBufferSize);
	iOwnsWorkspace = ETrue;
	}

// ------------------------------------------------------------------------------------------------
EXPORT_C void CXMLGenerator::CreateDynamicWorkspaceL()
	{
	FreeWorkspace();
	iWorkspace = CXMLDynamicWorkspace::NewL();
	iOwnsWorkspace = ETrue;
	}

// ------------------------------------------------------------------------------------------------
EXPORT_C CXMLWorkspace* CXMLGenerator::Workspace() const
	{
	return iWorkspace;
	}

// ------------------------------------------------------------------------------------------------
EXPORT_C void CXMLGenerator::SetWorkspace( CXMLWorkspace* aBuffer, TBool aTakeOwnership )
	{	
	FreeWorkspace();
	iOwnsWorkspace = aTakeOwnership;
	iWorkspace = aBuffer;
	}

// ------------------------------------------------------------------------------------------------
void CXMLGenerator::FreeWorkspace()
	{
	if( iOwnsWorkspace )
		{
		delete iWorkspace;
		iWorkspace = 0;
		}
	}

// ------------------------------------------------------------------------------------------------
EXPORT_C CXMLGenerator::CXMLGenerator()
	{
	}

// ------------------------------------------------------------------------------------------------
EXPORT_C CXMLGenerator::~CXMLGenerator()
	{
	delete iTable;
	delete iNSName;
	FreeWorkspace();
	iElemStack.Reset();
	}

// ------------------------------------------------------------------------------------------------
EXPORT_C void CXMLGenerator::BeginDocumentL( TUint8 /*aVersion*/, TInt32 /*aPublicId*/, TUint32 /*aCharset*/, const TDesC8& /*aStringTbl*/ )
	{
	}

// ------------------------------------------------------------------------------------------------
EXPORT_C void CXMLGenerator::BeginElementL( TUint8 aElement, TBool aHasContent, TBool aHasAttributes )
	{
	TBool addToElemStack(ETrue);
	IndentL();
	Workspace()->WriteL(KWBXMLTagStart());

	Workspace()->WriteL(TranslateElement(aElement));

	if( ((iNSAppendType == CXMLGenerator::EAppendToFirstElementOnly) && !(iNSAppended)) || (iNSAppendType == CXMLGenerator::EAppendToEveryElement) )
		{
		Workspace()->WriteL(KNamespaceBegin());
		Workspace()->WriteL(*iNSName);
		Workspace()->WriteL(KNamespaceEnd());
		iNSAppended = ETrue;
		}
	
	if( aHasAttributes )
		{
		// attributes
		}

	if( aHasContent )
		{
		Workspace()->WriteL(KWBXMLTagEnd());
		}
	else
		{
		Workspace()->WriteL(KWBXMLTagEndNoContent());
		addToElemStack = EFalse;
		}
	
	if( !iDontNewLine )
		{
		Workspace()->WriteL(KNewLine());
		}
	iDontNewLine = EFalse;

	if( addToElemStack )
		{
		iElemStack.Insert(aElement, 0);
		}
	}

// ------------------------------------------------------------------------------------------------
EXPORT_C void CXMLGenerator::SetTranslateTableL( const TDesC8& aTable )
	{
	delete iTable;
	iTable = 0;
	iTable = aTable.AllocL();
	}

// ------------------------------------------------------------------------------------------------
EXPORT_C void CXMLGenerator::SetNamespaceNameL( const TDesC8& aName, TNSmlNamespaceAppendType aAppendType )
	{
	iNSAppendType = aAppendType;
	delete iNSName;
	iNSName = 0;
	iNSName = aName.AllocL();
	}

// ------------------------------------------------------------------------------------------------
EXPORT_C void CXMLGenerator::EndElementL()
	{
	TUint8 elem = iElemStack[0];
	iElemStack.Remove(0);
	if( !iDontIndent )
		{
		IndentL();
		}
	iDontIndent = EFalse;
	Workspace()->WriteL(KWBXMLTagStartEndTag());
	Workspace()->WriteL(TranslateElement(elem));
	Workspace()->WriteL(KWBXMLTagEnd());
	Workspace()->WriteL(KNewLine());
	}

// ------------------------------------------------------------------------------------------------
EXPORT_C void CXMLGenerator::AddElementL( TUint8 aElement, const TDesC8& aContent, const TXMLContentFormat aFormat )
	{
	iDontNewLine = ETrue;
	BeginElementL(aElement, ETrue);
	if( aFormat == EXMLContentFormatOpaque )
		{
		WriteOpaqueDataL(aContent);
		}
	else
		{
		WriteInlineStringL(aContent);
		}
	iDontIndent = ETrue;
	EndElementL();
	}

// ------------------------------------------------------------------------------------------------
EXPORT_C void CXMLGenerator::AddElementL( TUint8 /*aElement*/, const TDesC8& /*aContent*/, const CWBXMLAttributes& /*aAttributes*/, const TXMLContentFormat /*aFormat*/ )
	{
	}

// ------------------------------------------------------------------------------------------------
EXPORT_C void CXMLGenerator::AddEntityL( TUint32 /*aEntity*/ )
	{
	}

// ------------------------------------------------------------------------------------------------
EXPORT_C void CXMLGenerator::AddExt_IL( TInt /*aExtNum*/, const TDesC8& /*aContent*/ )
	{
	}

// ------------------------------------------------------------------------------------------------
EXPORT_C void CXMLGenerator::AddExt_TL( TInt /*aExtNum*/, TUint32 /*aContent*/ )
	{
	}

// ------------------------------------------------------------------------------------------------
EXPORT_C void CXMLGenerator::AddExtL( TInt /*aExtNum*/ )
	{
	}

// ------------------------------------------------------------------------------------------------
EXPORT_C void CXMLGenerator::SwitchCodePageL( TUint8 /*aNewCodePage*/ )
	{
	}

// ------------------------------------------------------------------------------------------------
EXPORT_C TInt CXMLGenerator::HandleResult( TInt aResult, TInt aTreshold )
	{
	switch( aResult )
		{
		case KErrNone:
			if( Workspace()->FreeSize() < aTreshold )
				{				
				Workspace()->Rollback();
				return KWBXMLGeneratorBufferFull;
				}
			Workspace()->Commit();
			return KWBXMLGeneratorOk;

		case KErrTooBig:
			Workspace()->Rollback();
			return KWBXMLGeneratorBufferFull;

		default:
		    return KErrGeneral;
		}
	}

// ------------------------------------------------------------------------------------------------
EXPORT_C TInt CXMLGenerator::IndentLevel() const
	{
	return iElemStack.Count();
	}

// ------------------------------------------------------------------------------------------------
EXPORT_C void CXMLGenerator::SetIndentLevel( TInt aLevel )
	{
	iInitialIndentLevel = aLevel;
	}

// ------------------------------------------------------------------------------------------------
void CXMLGenerator::WriteMUint32L( TUint32 /*aValue*/ )
	{
	}

// ------------------------------------------------------------------------------------------------
void CXMLGenerator::WriteOpaqueDataL( const TDesC8& aData )
	{
	Workspace()->WriteL(aData);
	}

// ------------------------------------------------------------------------------------------------
void CXMLGenerator::WriteInlineStringL( const TDesC8& aData )
	{
	WriteOpaqueDataL(aData);
	}

// ------------------------------------------------------------------------------------------------
TPtrC8 CXMLGenerator::TranslateElement( TUint8 aElement )
	{
	TPtrC8 buf(*iTable);
	while( aElement-- )
		{
		TInt pos = buf.Find(KWBXMLElemenentSeparator());
		if( pos == KErrNotFound )
			{
			return TPtrC8();
			}
		buf.Set(buf.Right(buf.Length() - pos - 1));
		}

	TInt pos = buf.Find(KWBXMLElemenentSeparator());
	
	if( pos != KErrNotFound )
		{
		buf.Set(buf.Left(pos));
		}

	return buf;
	}

// ------------------------------------------------------------------------------------------------
void CXMLGenerator::IndentL()
	{
	for( TInt i = 0; i < iElemStack.Count() + iInitialIndentLevel; i++ )
		{
		Workspace()->WriteL(KIndent());
		}
	}
