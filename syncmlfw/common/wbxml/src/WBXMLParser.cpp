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
* Description:  Generic WBXML parser class implementation.
*
*/


// ------------------------------------------------------------------------------------------------
// Includes
// ------------------------------------------------------------------------------------------------
#include <s32mem.h>

#include "WBXMLParser.h"
#include "WBXMLAttributes.h"

// ------------------------------------------------------------------------------------------------
// Constants
// ------------------------------------------------------------------------------------------------
const TInt KNSmlBufferGranularity( 32 );

// ------------------------------------------------------------------------------------------------
// Local functions
// ------------------------------------------------------------------------------------------------
LOCAL_D void doPanic( const TDesC& aMsg, TInt aReason )
	{
	User::Panic(aMsg, aReason);
	}

// ------------------------------------------------------------------------------------------------
// CWBXMLParser
// ------------------------------------------------------------------------------------------------
EXPORT_C CWBXMLParser* CWBXMLParser::NewL()
	{
	CWBXMLParser* self = new (ELeave) CWBXMLParser();
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(); // self
	return self;
	}

// ------------------------------------------------------------------------------------------------
void CWBXMLParser::ConstructL()
	{
	iBuffer = CBufFlat::NewL(KNSmlBufferGranularity);
	}

// ------------------------------------------------------------------------------------------------
EXPORT_C CWBXMLParser::~CWBXMLParser()
	{
	delete iBuffer;
	delete iStringTable;
	iStack.Reset();
	}

// ------------------------------------------------------------------------------------------------
EXPORT_C void CWBXMLParser::SetDocumentHandler( MWBXMLDocumentHandler* aHandler )
	{
	iDocHandler = aHandler;
	}

// ------------------------------------------------------------------------------------------------
EXPORT_C void CWBXMLParser::SetExtensionHandler( MWBXMLExtensionHandler* aHandler )
	{
	iExtHandler = aHandler;
	}

// ------------------------------------------------------------------------------------------------
EXPORT_C void CWBXMLParser::SetDocumentL( RReadStream& aInput )
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
EXPORT_C TWBXMLParserError CWBXMLParser::ParseL()
	{
	__ASSERT_ALWAYS(iDocHandler, doPanic(KWBXMLNoDocHandler, KErrGeneral));
	__ASSERT_ALWAYS(iExtHandler, doPanic(KWBXMLNoExtHandler, KErrGeneral));
	if( !iDocHdrParsed )
		{
		TWBXMLParserError result(KWBXMLParserErrorOk);
		TRAPD(err, 	result = DoParseDocumentHeaderL());
		if( err == KErrEof )
			{
			return KWBXMLParserErrorEofTooEarly;
			}
		return result;
		}
	return DoParseDocumentBodyL();
	}

// ------------------------------------------------------------------------------------------------
TWBXMLParserError CWBXMLParser::DoParseDocumentHeaderL()
	{
	iDocHdrParsed = ETrue;

	// Version
	TUint8 version = ReadUint8L();
	
	// PublicId
	TInt32 publicId = ReadMUint32L();

	if( publicId == 0 )
		{
		publicId = ReadMUint32L();
		}
	else
		{
		publicId = -publicId;
		}

	// Charset
	TUint32 charSet = ReadMUint32L();

	// String table
	ReadStringTableL();

	// Document begings now
	if( publicId < 0 )
		{
		iDocHandler->StartDocumentL(version, -publicId, charSet);
		}
	else
		{
		iDocHandler->StartDocumentL(version, StringTableString(publicId), charSet);
		}

	return KWBXMLParserErrorOk;
	}

// ------------------------------------------------------------------------------------------------
TWBXMLParserError CWBXMLParser::DoParseDocumentBodyL()
	{
	TUint8 id(0);
	
	TRAPD(err, id = ReadUint8L());

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
			TUint8 cp = ReadUint8L();
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
			TWBXMLStackItem si(iStack.operator[](iStack.Count() - 1));
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
			TUint8 b = ReadUint8L();
			iDocHandler->CharactersL(TPtrC8(&b, 1));
			}
			break;

		case STR_I: 
			{
			iDocHandler->CharactersL(ReadStrIL());
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
			HandleExtensionsL(id);
			break;

		case STR_T: 
			iDocHandler->CharactersL(StringTableString(ReadMUint32L()));
			break;

		case PI:
			User::Leave(KErrNotSupported);
			break;

		default: 
			HandleElementL(id);
			break;
	    }

	return KWBXMLParserErrorOk;
	}

// ------------------------------------------------------------------------------------------------
TUint32 CWBXMLParser::ReadMUint32L()
	{
	TUint32 result = 0;
	TUint8 c;
	
	do 	{
		c = ReadUint8L();
		result = (result << 7) | (c & 0x7f);
		} while ( c & 0x80 );

	return result;
	}

// ------------------------------------------------------------------------------------------------
TUint8 CWBXMLParser::ReadUint8L()
	{
	return iInput.ReadUint8L();
	}

// ------------------------------------------------------------------------------------------------
TPtrC8 CWBXMLParser::ReadStrIL()
	{
	iBuffer->Reset();
	RBufWriteStream bws(*iBuffer);
	TUint8 c;
	while( (c = ReadUint8L()) != 0 )
		{
		bws.WriteUint8L(c);
		}
	bws.CommitL();
	return iBuffer->Ptr(0);
	}

// ------------------------------------------------------------------------------------------------
TPtrC8 CWBXMLParser::ReadOpaqueL()
	{
	iBuffer->Reset();
	RBufWriteStream bws(*iBuffer);
	bws.WriteL(iInput, ReadMUint32L());
	bws.CommitL();
	return iBuffer->Ptr(0);
	}

// ------------------------------------------------------------------------------------------------
TPtrC8 CWBXMLParser::StringTableString( TUint32 aIndex )
	{
	TPtrC8 temp(iStringTable->Mid(aIndex));
	TInt pos = temp.Find(KWBXMLNull());
	if( pos != KErrNotFound )
		{
		temp.Set(temp.Left(pos));
		}
	return temp;
	}

// ------------------------------------------------------------------------------------------------
void CWBXMLParser::ReadStringTableL()
	{
	delete iStringTable;
	iStringTable = 0;
	TUint32 strTblLen = ReadMUint32L();
	if( strTblLen > 0 )
		{
		iStringTable = HBufC8::NewL(strTblLen);
		TPtr8 ptr = iStringTable->Des();
		iInput.ReadL(ptr, strTblLen);
		}
	}

// ------------------------------------------------------------------------------------------------
void CWBXMLParser::HandleExtensionsL( TUint8 aId )
	{
	switch( aId ) 
		{
		case EXT_I_0: 
		case EXT_I_1: 
		case EXT_I_2:     
			iExtHandler->Ext_IL(TUint8(aId - EXT_I_0), ReadStrIL());
			break;
			
		case EXT_T_0: 
		case EXT_T_1: 
		case EXT_T_2:
			iExtHandler->Ext_TL(TUint8(aId - EXT_T_0), ReadMUint32L());
			break;
			
		case EXT_0:   
		case EXT_1: 
		case EXT_2:
			iExtHandler->ExtL(TUint8(aId - EXT_0));
			break;
			
		case OPAQUE: 
			iExtHandler->OpaqueL(ReadOpaqueL());
			break;

		default:
			User::Leave( KWBXMLParserErrorInvalidTag );
		}
	}

// ------------------------------------- -----------------------------------------------------------
void CWBXMLParser::HandleElementL( TUint8 aId )
	{
	TUint8 aTag = TUint8(aId & 0x3f);

	CWBXMLAttributes* attr = CWBXMLAttributes::NewLC();
	
	if( aId & 0x80 )
		{
		ReadAttributesL(attr);
		}

	iDocHandler->StartElementL(aTag, *attr);

	if( aId & 0x40 ) 
		{
		TWBXMLStackItem si(aTag, iCodePage);
	    iStack.Append(si);
		}
	else 
		{
	    iDocHandler->EndElementL(aTag);
		}

	CleanupStack::PopAndDestroy(); // attr
	}

// ------------------------------------------------------------------------------------------------
void CWBXMLParser::ReadAttributesL( CWBXMLAttributes* aAttributes )
	{
	TUint8 id = ReadUint8L();
	while( id != END ) 
		{	    
		iBuffer->Reset();
		RBufWriteStream bws(*iBuffer);
		TUint8 name = id;
	    id = ReadUint8L();
	    while( id > 0x80 || id == ENTITY || id == STR_I || id == STR_T || (id >= EXT_I_0 && id <= EXT_I_2) || (id >= EXT_T_0 && id <= EXT_T_2) ) 
			{		
			switch (id) 
				{
				case ENTITY: 
					bws.WriteUint8L(ReadUint8L());
					break;

				case STR_I: 
					bws.WriteL(ReadStrIL());
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
					HandleExtensionsL(id);
					break;
 
				case STR_T: 
					bws.WriteL(StringTableString(ReadUint8L()));
					break;
			   
				default:
					bws.WriteUint8L(id);
					break;
				}
			id = ReadUint8L();
			}
		bws.CommitL();
		aAttributes->AddAttributeL(name, iBuffer->Ptr(0));
		}
	}

//End of File


