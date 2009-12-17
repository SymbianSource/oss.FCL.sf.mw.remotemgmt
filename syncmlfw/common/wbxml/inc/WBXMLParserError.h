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
* Description:  Error codes for WBXML parsers.
*
*/


#ifndef __WBXMLPARSERERROR_H__
#define __WBXMLPARSERERROR_H__

// ------------------------------------------------------------------------------------------------
// Parser error base
// ------------------------------------------------------------------------------------------------

#define PERRBASE 0x2000

enum TWBXMLParserError
	{
	KWBXMLParserErrorOk = 0,
	KWBXMLParserErrorEof = PERRBASE,
	KWBXMLParserErrorEofTooEarly,
	KWBXMLParserErrorInvalidTag,
	KWBXMLParserErrorWrongWBXMLVersion,
	KWBXMLParserErrorWrongPublicId,
	KErrParserErrorInvalidDocument
	};

#endif // __WBXMLPARSERERROR_H__
