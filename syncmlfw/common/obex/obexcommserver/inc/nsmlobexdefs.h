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
* Description:  SyncML Obex server internal server module
*
*/



#ifndef __NSMLOBEXDEFS_H__
#define __NSMLOBEXDEFS_H__

// ------------------------------------------------------------------------------------------------
// Includes
// ------------------------------------------------------------------------------------------------
#include <e32base.h>

// ------------------------------------------------------------------------------------------------
// Constants
// ------------------------------------------------------------------------------------------------
_LIT( KNSmlDSObexCommServerName, "NSmlDSObexCommServer");
_LIT( KNSmlDMObexCommServerName, "NSmlDMObexCommServer");

const TInt KNSmlObexCommServerVersionMajor = 1;
const TInt KNSmlObexCommServerVersionMinor = 0;

// ------------------------------------------------------------------------------------------------
// TNSmlObexCommCommands 
// ------------------------------------------------------------------------------------------------
enum TNSmlObexCommCommands 
	{
	// Client side commands
	ENSmlObexCommCommandDisconnect,
	ENSmlObexCommCommandSend,
	ENSmlObexCommCommandReceive,
	ENSmlObexCommCommandCancelReceive,
	// Server side commands
	ENSmlObexCommCommandSetReceivePacket,
	ENSmlObexCommCommandGetSendPacket,
	ENSmlObexCommCommandCancelGetSendPacket,
	ENSmlObexCommCommandListenDisconnect,
	ENSmlObexCommCommandCancelListenDisconnect
	};

#endif __NSMLOBEXDEFS_H__