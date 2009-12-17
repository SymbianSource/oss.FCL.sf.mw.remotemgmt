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
* Description:  Generic WBXML generator.
*
*/


#ifndef __WBXMLGENERATOR_H__
#define __WBXMLGENERATOR_H__

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
// TWBXMLContentFormat
// ------------------------------------------------------------------------------------------------
enum TWBXMLContentFormat
	{
	EWBXMLContentFormatOpaque,
	EWBXMLContentFormatInlineString
	};

// ------------------------------------------------------------------------------------------------
// CWBXMLGenerator
// ------------------------------------------------------------------------------------------------
class CWBXMLGenerator : public CBase
	{
public:
	IMPORT_C TPtrC8 Document();
	IMPORT_C void Reset();

protected:
	IMPORT_C CWBXMLGenerator();
	IMPORT_C virtual ~CWBXMLGenerator();

	IMPORT_C void CreateStaticWorkspaceL( TInt aBufferSize );
	IMPORT_C void CreateDynamicWorkspaceL();

	IMPORT_C CXMLWorkspace* Workspace() const;
	IMPORT_C void SetWorkspace( CXMLWorkspace* aBuffer, TBool aTakeOwnership = EFalse );

	// WBXML
	IMPORT_C void BeginDocumentL( TUint8 aVersion, TInt32 aPublicId, TUint32 aCharset, const TDesC8& aStringTbl = TPtrC8() ); // Negative PublicId = offset to string table

	IMPORT_C void BeginElementL( TUint8 aElement, TBool aHasContent = EFalse, TBool aHasAttributes = EFalse );
	IMPORT_C void EndElementL();

	IMPORT_C void AddElementL( TUint8 aElement, const TDesC8& aContent, const TWBXMLContentFormat aFormat = EWBXMLContentFormatOpaque );
	IMPORT_C void AddElementL( TUint8 aElement, const TDesC8& aContent, const CWBXMLAttributes& aAttributes, const TWBXMLContentFormat aFormat = EWBXMLContentFormatOpaque );

	IMPORT_C void AddEntityL( TUint32 aEntity );

	IMPORT_C void AddExt_IL( TInt aExtNum, const TDesC8& aContent );
	IMPORT_C void AddExt_TL( TInt aExtNum, TUint32 aContent );
	IMPORT_C void AddExtL( TInt aExtNum );

	IMPORT_C void SwitchCodePageL( TUint8 aNewCodePage );

	IMPORT_C TInt HandleResult( TInt aResult, TInt aTreshold = 0 );

private:
	void WriteMUint32L( TUint32 aValue );
	void WriteOpaqueDataL( const TDesC8& aData );
	void WriteInlineStringL( const TDesC8& aData );
	void FreeWorkspace();

private:
	CXMLWorkspace* iWorkspace;
	TBool iOwnsWorkspace;
	};

#endif // __WBXMLGENERATOR_H__
