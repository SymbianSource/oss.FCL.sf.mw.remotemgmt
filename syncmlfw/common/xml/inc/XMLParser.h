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
* Description:  XML parser
*
*/


#ifndef __XMLPARSER_H__
#define __XMLPARSER_H__

// ------------------------------------------------------------------------------------------------
// Includes
// ------------------------------------------------------------------------------------------------
#include <e32base.h>
#include <s32strm.h>

#include "WBXMLDefs.h"
#include "WBXMLHandler.h"
#include "WBXMLParserError.h"

// ------------------------------------------------------------------------------------------------
// Constants
// ------------------------------------------------------------------------------------------------
_LIT(KXMLNoExtHandler, "No extension handler");
_LIT(KXMLNoDocHandler, "No document handler");

_LIT8(KXMLNull, "\0x00");

// ------------------------------------------------------------------------------------------------
// Forwards
// ------------------------------------------------------------------------------------------------
class CXMLAttributes;

// ------------------------------------------------------------------------------------------------
// TXMLStackItem
// ------------------------------------------------------------------------------------------------
struct TXMLStackItem
	{
public:
	inline TXMLStackItem( TUint8 aTag, TUint8 aCodePage );
	inline TUint8 Tag() const;
	inline TUint8 CodePage() const;

private:
	TUint8 iTag;
	TUint8 iCodePage;
	};

inline TXMLStackItem::TXMLStackItem( TUint8 aTag, TUint8 aCodePage ) : iTag(aTag), iCodePage(aCodePage)
	{
	}

inline TUint8 TXMLStackItem::Tag() const
	{
	return iTag;
	}

inline TUint8 TXMLStackItem::CodePage() const
	{
	return iCodePage;
	}

// ------------------------------------------------------------------------------------------------
// CXMLParser
// ------------------------------------------------------------------------------------------------
class CXMLParser : public CBase
	{
public:
	IMPORT_C static CXMLParser* NewL();
	IMPORT_C ~CXMLParser();
	IMPORT_C void SetDocumentHandler( MWBXMLDocumentHandler* aHandler );
	IMPORT_C void SetExtensionHandler( MWBXMLExtensionHandler* aHandler );

	IMPORT_C void SetDocumentL( RReadStream& aInput );
	IMPORT_C TWBXMLParserError ParseL();

protected:
	TWBXMLParserError doParseDocumentHeaderL();
	TWBXMLParserError doParseDocumentBodyL();

	TUint32 readMUint32L();
	TUint8 readUint8L();
	TPtrC8 readStrIL();
	TPtrC8 readOpaqueL();
	TPtrC8 stringTableString( TUint32 aIndex );
	void readAttributesL( CWBXMLAttributes* aAttributes );
	void readStringTableL();
	void handleExtensionsL( TUint8 aId );
	void handleElementL( TUint8 aId );

private:
	void ConstructL();

private:
	MWBXMLDocumentHandler* iDocHandler;
	MWBXMLExtensionHandler* iExtHandler;
	HBufC8* iStringTable;
	RReadStream iInput;
	RArray<TXMLStackItem> iStack;
	CBufBase* iBuffer;
	TUint8 iCodePage;
	TBool iDocHdrParsed;
	};

#endif // __XMLPARSER_H__