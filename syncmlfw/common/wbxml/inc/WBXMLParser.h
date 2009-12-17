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
* Description:  Generic WBXML parser class.
*
*/


#ifndef __WBXMLPARSER_H__
#define __WBXMLPARSER_H__

// ------------------------------------------------------------------------------------------------
// Includes
// ------------------------------------------------------------------------------------------------
#include <e32base.h>
#include <s32strm.h>

#include "WBXMLHandler.h"
#include "WBXMLDefs.h"
#include "WBXMLParserError.h"

// ------------------------------------------------------------------------------------------------
// Constants
// ------------------------------------------------------------------------------------------------
_LIT(KWBXMLNoExtHandler, "No extension handler");
_LIT(KWBXMLNoDocHandler, "No document handler");

_LIT8(KWBXMLNull, "\x00");

// ------------------------------------------------------------------------------------------------
// Forwards
// ------------------------------------------------------------------------------------------------
class CWBXMLAttributes;

// ------------------------------------------------------------------------------------------------
// TWBXMLStackItem
// ------------------------------------------------------------------------------------------------
struct TWBXMLStackItem
	{
public:
	inline TWBXMLStackItem( TUint8 aTag, TUint8 aCodePage );
	inline TUint8 Tag() const;
	inline TUint8 CodePage() const;

private:
	TUint8 iTag;
	TUint8 iCodePage;
	TUint16 iDummy;
	};

#include "WBXMLParser.inl"

// ------------------------------------------------------------------------------------------------
// CWBXMLParser
// ------------------------------------------------------------------------------------------------
class CWBXMLParser : public CBase
	{
public:
	IMPORT_C static CWBXMLParser* NewL();
	IMPORT_C ~CWBXMLParser();
	IMPORT_C void SetDocumentHandler( MWBXMLDocumentHandler* aHandler );
	IMPORT_C void SetExtensionHandler( MWBXMLExtensionHandler* aHandler );

	IMPORT_C void SetDocumentL( RReadStream& aInput );
	IMPORT_C TWBXMLParserError ParseL();

private:
	TWBXMLParserError DoParseDocumentHeaderL();
	TWBXMLParserError DoParseDocumentBodyL();

	TUint32 ReadMUint32L();
	TUint8 ReadUint8L();
	TPtrC8 ReadStrIL();
	TPtrC8 ReadOpaqueL();
	TPtrC8 StringTableString( TUint32 aIndex );
	void ReadAttributesL( CWBXMLAttributes* aAttributes );
	void ReadStringTableL();
	void HandleExtensionsL( TUint8 aId );
	void HandleElementL( TUint8 aId );

	void ConstructL();

private:
	MWBXMLDocumentHandler* iDocHandler;
	MWBXMLExtensionHandler* iExtHandler;
	HBufC8* iStringTable;
	RReadStream iInput;
	RArray<TWBXMLStackItem> iStack;
	CBufBase* iBuffer;
	TUint8 iCodePage;
	TBool iDocHdrParsed;
	};

#endif // __WBXMLPARSER_H__