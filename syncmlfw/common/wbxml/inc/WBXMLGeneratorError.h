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
* Description:  Error codes for WBXML generator.
*
*/


#ifndef __WBXMLGENERATORERROR_H__
#define __WBXMLGENERATORERROR_H__

// ------------------------------------------------------------------------------------------------
// Generator error base
// ------------------------------------------------------------------------------------------------

#define GERRBASE 0x3000

enum TWBXMLGeneratorError
	{
	KWBXMLGeneratorOk = 0,
	KWBXMLGeneratorBufferFull = GERRBASE
	};

#endif __WBXMLGENERATORERROR_H__