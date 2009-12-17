/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description: Implementation of policymanagement components
*
*/
// CentRepToolClientServer.h

#ifndef __CENTREPTOOLSERVER_H__
#define __CENTREPTOOLSERVER_H__

#include <e32std.h>



_LIT(KCentRepToolServerName,"centreptoolserver");
_LIT(KCentRepToolServerImg,"centreptoolserver");			// DLL/EXE name

const TUid KCentRepToolServerUid={0x80000005};




enum TAccessType
{
	EWriteAccess = 0,
	EReadAccess,
	EAccessBoth
};

enum TCentRepMessages
	{
	ECreateRepositorySubSession,
	ECloseRepositorySubSession,
	
	EInitRepositorySession,
	
	ESetSIDWRForSetting,
	ERestoreSetting,

	ESetSIDWRForMask,
	ERestoreMask,

	ESetSIDWRForRange,
	ERestoreRange,
	
	EAddSIDWRForDefaults,
	ERestoreDefaults,
	
	ERemoveBackupFlagForRange,
	ERestoreBackupFlagForRange,

	ERemoveBackupFlagForMask,
	ERestoreBackupFlagForMask,

	ERemoveBackupFlagForDefaults,
	ERestoreBackupFlagForDefaults,

	EFlushRepository,
	ECheckCommitState,
	
	EPerformCentRepToolRFS,

	ECreateCheckAccessSession,
	ECloseCheckAcceessSession,
	ECheckAccess
	};

#endif// __CENTREPTOOLSERVER_H__
