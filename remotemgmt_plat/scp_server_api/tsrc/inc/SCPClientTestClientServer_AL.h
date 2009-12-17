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
 
#ifndef  __SCPCLIENTTESTCLIENTSERVER_AL_H__
#define  __SCPCLIENTTESTCLIENTSERVER_AL_H__

#include <e32std.h>
#include <e32base.h>

_LIT(KDMSSServerName,"SCPClientTestServer_AL");
_LIT(KDMSSServerImg,"SCPClientTestServer_AL");//can be removed

const TUid KDMSSServerUid={0x100059B5}; 
const TInt KSSInfoBufferLength = 1000;
enum TSSServerMessages
{
	ESCPClientTestQueryAdminCmd,
	ESCPClientTestGetParam,
	ESCPClientTestCheckConfig
};

#endif

//end of file