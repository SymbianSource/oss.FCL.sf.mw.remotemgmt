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


// ------------------------------------------------------------------------------------------------
// Includes
// ------------------------------------------------------------------------------------------------
#include <s32mem.h>

#include "XMLParser.h"
#include "WBXMLAttributes.h"

// ------------------------------------------------------------------------------------------------
// Local functions
// ------------------------------------------------------------------------------------------------
LOCAL_D void doPanic( const TDesC& aMsg, TInt aReason )
	{
	User::Panic(aMsg, aReason);
	}

// ------------------------------------------------------------------------------------------------
// CXMLParser
// ------------------------------------------------------------------------------------------------
EXPORT_C CXMLParser* CXMLParser::NewL()
	{
	CXMLParser* self = new (ELeave) CXMLParser();
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(); // self
	return self;
	}

// ------------------------------------------------------------------------------------------------
void CXMLParser::ConstructL()
	{
	iBuffer = CBufFlat::NewL(32);
	}

// ------------------------------------------------------------------------------------------------
EXPORT_C CXMLParser::~CXMLParser()
	{
	delete iBuffer;
	delete iStringTable;
	iStack.Reset();
	}

// ------------------------------------------------------------------------------------------------
EXPORT_C void CXMLParser::SetDocumentHandler( MWBXMLDocumentHandler* aHandler )
	{
	iDocHandler = aHandler;
	}

// ------------------------------------------------------------------------------------------------
EXPORT_C void CXMLParser::SetExtensionHandler( MWBXMLExtensionHandler* aHandler )
	{
	iExtHandler = aHandler;
	}

// ------------------------------------------------------------------------------------------------
EXPORT_C void CXMLParser::SetDocumentL( RReadStream& aInput )
	{
	iInput = aInput;
	iStack.Reset();
	iDocHdrParsed = EFalse;
	if( iDocHandler )
		{
		iDocHandler->DocumentChangedL();
		}
	}

// ------------------------------------------------------------------------------------------------
EXPORT_C TWBXMLParserError CXMLParser::ParseL()
	{
	__ASSERT_ALWAYS(iDocHandler, doPanic(KXMLNoDocHandler, KErrGeneral));
	__ASSERT_ALWAYS(iExtHandler, doPanic(KXMLNoExtHandler, KErrGeneral));
	if( !iDocHdrParsed )
		{
		TWBXMLParserError result(KWBXMLParserErrorOk);
		TRAPD(err, 	result = doParseDocumentHeaderL());
		if( err == KErrEof )
			{
			return KWBXMLParserErrorEofTooEarly;
			}
		return result;
		}
	return doParseDocumentBodyL();
	}

// ------------------------------------------------------------------------------------------------
TWBXMLParserError CXMLParser::doParseDocumentHeaderL()
	{
	iDocHdrParsed = ETrue;

	// Version
	TUint8 version = readUint8L();
	
	// PublicId
	TInt32 publicId = readMUint32L();

	if( publicId == 0 )
		{
		publicId = readMUint32L();
		}
	else
		{
		publicId = -publicId;
		}

	// Charset
	TUint32 charSet = readMUint32L();

	// String table
	readStringTableL();

	// Document begings now
	if( publicId < 0 )
		{
		iDocHandler->StartDocumentL(version, -publicId, charSet);
		}
	else
		{
		iDocHandler->StartDocumentL(version, stringTableString(publicId), charSet);
		}

	return KWBXMLParserErrorOk;
	}

// ------------------------------------------------------------------------------------------------
TWBXMLParserError CXMLParser::doParseDocumentBodyL()
	{
	TUint8 id(0);
	
	TRAPD(err, id = readUint8L());

	if( err != KErrNone )
		{
		if( err == KErrEof )
			{
			iDocHandler->EndDocumentL();
			if( iStack.Count() > 0 )
				{
				return KWBXMLParserErrorEofTooEarly;
				}
			return KWBXMLParserErrorEof;
			}
		User::Leave(err);
		}
	
	switch (id) 
		{
		case SWITCH_PAGE: 
			{
			TUint8 cp = readUint8L();
			if( cp != iCodePage )
				{
				iDocHandler->CodePageSwitchL(cp);
				iCodePage = cp;
				}
			}
			break;
	
		case END:
			{
			if( !iStack.Count() )
				{
				User::Leave(KErrParserErrorInvalidDocument);
				}
			TXMLStackItem si(iStack.operator[](iStack.Count() - 1));
			if( si.CodePage() != iCodePage )
				{
				iDocHandler->CodePageSwitchL(si.CodePage());
				iCodePage = si.CodePage();
				}
			iDocHandler->EndElementL(si.Tag());
			iStack.Remove(iStack.Count() - 1);
			}
			break;
	
		case ENTITY:
			{
			TUint8 b = readUint8L();
			iDocHandler->CharactersL(TPtrC8(&b, 1));
			}
			break;

		case STR_I: 
			{
			iDocHandler->CharactersL(readStrIL());
			}
			break;

		case EXT_I_0: 
		case EXT_I_1: 
		case EXT_I_2:
		case EXT_T_0: 
		case EXT_T_1: 
		case EXT_T_2:
		case EXT_0:   
		case EXT_1:   
		case EXT_2:
		case OPAQUE:
			handleExtensionsL(id);
			break;

		case STR_T: 
			iDocHandler->CharactersL(stringTableString(readMUint32L()));
			break;

		case PI:
			User::Leave(KErrNotSupported);
			break;

		default: 
			handleElementL(id);
			break;
	    }

	return KWBXMLParserErrorOk;
	}

// ------------------------------------------------------------------------------------------------
TUint32 CXMLParser::readMUint32L()
	{
	TUint32 result = 0;
	TUint8 c;
	
	do 	{
		c = readUint8L();
		result = (result << 7) | (c & 0x7f);
		} while ( c & 0x80 );

	return result;
	}

// ------------------------------------------------------------------------------------------------
TUint8 CXMLParser::readUint8L()
	{
	return iInput.ReadUint8L();
	}

// ------------------------------------------------------------------------------------------------
TPtrC8 CXMLParser::readStrIL()
	{
	iBuffer->Reset();
	RBufWriteStream bws(*iBuffer);
	TUint8 c;
	while( (c = readUint8L()) != 0 )
		{
		bws.WriteUint8L(c);
		}
	bws.CommitL();
	return iBuffer->Ptr(0);
	}

// ------------------------------------------------------------------------------------------------
TPtrC8 CXMLParser::readOpaqueL()
	{
	iBuffer->Reset();
	RBufWriteStream bws(*iBuffer);
	bws.WriteL(iInput, readMUint32L());
	bws.CommitL();
	return iBuffer->Ptr(0);
	}

// ------------------------------------------------------------------------------------------------
TPtrC8 CXMLParser::stringTableString( TUint32 aIndex )
	{
	TPtrC8 temp(iStringTable->Mid(aIndex));
	TInt pos = temp.Find(KXMLNull());
	if( pos != KErrNotFound )
		{
		return temp.Mid(0, pos);
		}
	return temp;
	}

// ------------------------------------------------------------------------------------------------
void CXMLParser::readStringTableL()
	{
	delete iStringTable;
	iStringTable = 0;
	TUint32 strTblLen = readMUint32L();
	if( strTblLen > 0 )
		{
		iStringTable = HBufC8::NewL(strTblLen);
		TPtr8 ptr = iStringTable->Des();
		iInput.ReadL(ptr, strTblLen);
		}
	}

// ------------------------------------------------------------------------------------------------
void CXMLParser::handleExtensionsL( TUint8 aId )
	{
	switch( aId ) 
		{
		case EXT_I_0: 
		case EXT_I_1: 
		case EXT_I_2:     
			iExtHandler->Ext_IL(TUint8(aId - EXT_I_0), readStrIL());
			break;
			
		case EXT_T_0: 
		case EXT_T_1: 
		case EXT_T_2:
			iExtHandler->Ext_TL(TUint8(aId - EXT_T_0), readMUint32L());
			break;
			
		case EXT_0:   
		case EXT_1: 
		case EXT_2:
			iExtHandler->ExtL(TUint8(aId - EXT_0));
			break;
			
		case OPAQUE: 
			iExtHandler->OpaqueL(readOpaqueL());
			break;

		default:
			User::Leave( KWBXMLParserErrorInvalidTag );
		}
	}

// ------------------------------------- -----------------------------------------------------------
void CXMLParser::handleElementL( TUint8 aId )
	{
	TUint8 aTag = TUint8(aId & 0x3f);

	CWBXMLAttributes* attr = CWBXMLAttributes::NewLC();
	
	if( aId & 0x80 )
		{
		readAttributesL(attr);
		}

	iDocHandler->StartElementL(aTag, *attr);

	if( aId & 0x40 ) 
		{
		TXMLStackItem si(aTag, iCodePage);
	    iStack.Append(si);
		}
	else 
		{
	    iDocHandler->EndElementL(aTag);
		}

	CleanupStack::PopAndDestroy(); // attr
	}

// ------------------------------------------------------------------------------------------------
void CXMLParser::readAttributesL( CWBXMLAttributes* aAttributes )
	{
	TUint8 id = readUint8L();
	while( id != END ) 
		{	    
		iBuffer->Reset();
		RBufWriteStream bws(*iBuffer);
		TUint8 name = id;
	    id = readUint8L();
	    while( id > 0x80 || id == ENTITY || id == STR_I || id == STR_T || (id >= EXT_I_0 && id <= EXT_I_2) || (id >= EXT_T_0 && id <= EXT_T_2) ) 
			{		
			switch (id) 
				{
				case ENTITY: 
					bws.WriteUint8L(readUint8L());
					break;

				case STR_I: 
					bws.WriteL(readStrIL());
					break;
			
				case EXT_I_0: 
				case EXT_I_1: 
				case EXT_I_2:     
				case EXT_T_0: 
				case EXT_T_1: 
				case EXT_T_2:
				case EXT_0:   
				case EXT_1: 
				case EXT_2:
				case OPAQUE: 
					handleExtensionsL(id);
					break;
 
				case STR_T: 
					bws.WriteL(stringTableString(readUint8L()));
					break;
			   
				default:
					bws.WriteUint8L(id);
					break;
				}
			id = readUint8L();
			}
		bws.CommitL();
		aAttributes->AddAttributeL(name, iBuffer->Ptr(0));
		}
	}

//End of File


