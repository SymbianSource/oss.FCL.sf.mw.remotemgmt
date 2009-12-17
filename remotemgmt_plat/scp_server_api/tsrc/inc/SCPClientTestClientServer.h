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
 
#ifndef  __SCPCLIENTTEST_CLIENTSERVER_H__
#define  __SCPCLIENTTEST_CLIENTSERVER_H__

#include <e32std.h>
#include <e32base.h>

// server name

_LIT(KDMSSServerName,"SCPClientTestServer");
_LIT(KDMSSServerImg,"SCPClientTestServer");//can be removed

const TUid KDMSSServerUid={0x102073EA}; 
const TInt KSSInfoBufferLength = 1000;
enum TSSServerMessages
{
	ESCPClientTestConstructor,
	ESCPClientTestConnect,
	ESCPClientTestVersion,
	ESCPClientTestGetCode,
	ESCPClientTestSetCode,
	ESCPClientTestChangeCode,
	ESCPClientTestSetPhoneLock,
	ESCPClientTestGetLockState,
	ESCPClientTestQueryAdminCmd,
	ESCPClientTestSetParam,
	ESCPClientTestGetParam,
	ESCPClientTestAuthenticateS60,
	ESCPClientTestCodeChangeQuery,
    ESCPClientTestPerformCleanUp
};

#endif
// end of file