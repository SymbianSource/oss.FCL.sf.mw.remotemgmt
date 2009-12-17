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
* Description:  Definitions for WBXML.
*
*/


#ifndef __WBXMLDEFS_H__
#define __WBXMLDEFS_H__

// ------------------------------------------------------------------------------------------------
// Includes
// ------------------------------------------------------------------------------------------------

#include <e32base.h>

// ------------------------------------------------------------------------------------------------
// Typedefs
// ------------------------------------------------------------------------------------------------

typedef TUint8 TWBXMLTag;

// ------------------------------------------------------------------------------------------------
// Constants
// ------------------------------------------------------------------------------------------------

const TWBXMLTag SWITCH_PAGE = 0;
const TWBXMLTag END = 1;
const TWBXMLTag ENTITY = 2;
const TWBXMLTag STR_I = 3;
const TWBXMLTag LITERAL = 4;
const TWBXMLTag EXT_I_0 = 0x40;
const TWBXMLTag EXT_I_1 = 0x41;
const TWBXMLTag EXT_I_2 = 0x42;
const TWBXMLTag PI = 0x43;
const TWBXMLTag LITERAL_C = 0x44;
const TWBXMLTag EXT_T_0 = 0x80;
const TWBXMLTag EXT_T_1 = 0x81;
const TWBXMLTag EXT_T_2 = 0x82;
const TWBXMLTag STR_T = 0x83;
const TWBXMLTag LITERAL_A = 0x84;
const TWBXMLTag EXT_0 = 0xc0;
const TWBXMLTag EXT_1 = 0xc1;
const TWBXMLTag EXT_2 = 0xc2;
const TWBXMLTag OPAQUE = 0xc3; 
const TWBXMLTag LITERAL_AC = 0xc4;

const TUint8 KWBXMLHasAttributes = (1<<7);
const TUint8 KWBXMLHasContent = (1<<6);

#endif // __WBXMLDEFS_H__