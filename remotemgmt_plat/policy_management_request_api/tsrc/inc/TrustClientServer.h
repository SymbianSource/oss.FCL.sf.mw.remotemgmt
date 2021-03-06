/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description: definition of dm constants/exported methods
* 	This is part of remotemgmt_plat.
*
*/
 
#ifndef __Trust_CLIENTSERVER_H__
#define  __Trust_CLIENTSERVER_H__

#include <e32std.h>
#include <e32base.h>

// server name

_LIT(KTrustServerName,"TrustServer");
_LIT(KTrustServerImg,"TrustServer");//can be removed

const TUid KTrustServerUid={0x101F9A02}; 

enum TTrustServerMessages
{
	EAddTrust,
	EGetElementList,
	EGetElement,
	EGetXACMLDescription
};

#endif