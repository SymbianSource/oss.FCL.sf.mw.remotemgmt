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
 
#ifndef  __DMCERTAPI_CLIENTSERVER_H__
#define  __DMCERTAPI_CLIENTSERVER_H__

#include <e32std.h>
#include <e32base.h>

// server name

_LIT(KDMSSServerName,"dmcertapiServer");
_LIT(KDMSSServerImg,"dmcertapiServer");//can be removed

const TUid KDMSSServerUid={0x101F99FB}; 
const TInt KSSInfoBufferLength = 1000;
enum TSSServerMessages
{
	EGetCertificate
};

#endif
// end of file