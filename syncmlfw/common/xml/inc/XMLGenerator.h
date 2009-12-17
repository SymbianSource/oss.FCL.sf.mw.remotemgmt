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


#ifndef __XMLGENERATOR_H__
#define __XMLGENERATOR_H__

// ------------------------------------------------------------------------------------------------
// Includes
// ------------------------------------------------------------------------------------------------
#include <e32base.h>

#include "WBXMLAttributes.h"

// ------------------------------------------------------------------------------------------------
// Class forwards
// ------------------------------------------------------------------------------------------------

class CXMLWorkspace;

// ------------------------------------------------------------------------------------------------
// TXMLContentFormat
// ------------------------------------------------------------------------------------------------
enum TXMLContentFormat
	{
	EXMLContentFormatOpaque,
	EXMLContentFormatInlineString
	};

// ------------------------------------------------------------------------------------------------
// CXMLGenerator
// ------------------------------------------------------------------------------------------------
class CXMLGenerator : public CBase
	{
public:
	IMPORT_C TPtrC8 Document();
	IMPORT_C void Reset();
	IMPORT_C CXMLWorkspace* Workspace() const;
	IMPORT_C void SetWorkspace( CXMLWorkspace* aBuffer, TBool aTakeOwnership = EFalse );
	IMPORT_C TInt IndentLevel() const;
	IMPORT_C void SetIndentLevel( TInt aLevel );

protected:
	enum TNSmlNamespaceAppendType
		{
		EAppendToEveryElement,
		EAppendToFirstElementOnly
		};

	IMPORT_C CXMLGenerator();
	IMPORT_C virtual ~CXMLGenerator();

	IMPORT_C void CreateStaticWorkspaceL( TInt aBufferSize );
	IMPORT_C void CreateDynamicWorkspaceL();

	// XML
	IMPORT_C void BeginDocumentL( TUint8 aVersion, TInt32 aPublicId, TUint32 aCharset, const TDesC8& aStringTbl = TPtrC8() ); // Negative PublicId = offset to string table

	IMPORT_C void BeginElementL( TUint8 aElement, TBool aHasContent = EFalse, TBool aHasAttributes = EFalse );
	IMPORT_C void EndElementL();

	IMPORT_C void AddElementL( TUint8 aElement, const TDesC8& aContent, const TXMLContentFormat aFormat = EXMLContentFormatOpaque );
	IMPORT_C void AddElementL( TUint8 aElement, const TDesC8& aContent, const CWBXMLAttributes& aAttributes, const TXMLContentFormat aFormat = EXMLContentFormatOpaque );

	IMPORT_C void AddEntityL( TUint32 aEntity );

	IMPORT_C void AddExt_IL( TInt aExtNum, const TDesC8& aContent );
	IMPORT_C void AddExt_TL( TInt aExtNum, TUint32 aContent );
	IMPORT_C void AddExtL( TInt aExtNum );

	IMPORT_C void SwitchCodePageL( TUint8 aNewCodePage );

	IMPORT_C TInt HandleResult( TInt aResult, TInt aTreshold = 0 );

	IMPORT_C void SetTranslateTableL( const TDesC8& aTable );
	IMPORT_C void SetNamespaceNameL( const TDesC8& aName, TNSmlNamespaceAppendType aAppendType = EAppendToFirstElementOnly );

private:
	TPtrC8 TranslateElement( TUint8 aElement );
	void WriteMUint32L( TUint32 aValue );
	void WriteOpaqueDataL( const TDesC8& aData );
	void WriteInlineStringL( const TDesC8& aData );
	void FreeWorkspace();
	void ConstructL( const TDesC8& aElements );
	void IndentL();

private:
	RArray<TUint8> iElemStack;
	HBufC8* iTable;
	HBufC8* iNSName;
	CXMLWorkspace* iWorkspace;
	TBool iOwnsWorkspace;
	TBool iNSAppended;
	TBool iDontNewLine;
	TBool iDontIndent;
	TNSmlNamespaceAppendType iNSAppendType;
	TInt iInitialIndentLevel;
	};

#endif // __XMLGENERATOR_H__
