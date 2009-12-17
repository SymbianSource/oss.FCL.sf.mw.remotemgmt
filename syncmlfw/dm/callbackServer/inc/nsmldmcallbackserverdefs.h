/*
* Copyright (c) 2004 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  DM callback client and server IPC commands
*
*/


#ifndef __NSMLDMCALLBACKSERVERDEFS_H__
#define __NSMLDMCALLBACKSERVERDEFS_H__

_LIT( KServerNameAndDir, "Z:\\sys\\bin\\NSmlDmCallbackServer.exe");
_LIT( KServerDir, "Z:\\sys\\bin\\");

_LIT(KNSmlDmCallbackServerName,"NSmlDmCallbackServer");
_LIT(KNSmlDmCallbackServerThread,"NSmlDmCallbackThread");

_LIT(KNSmlDmCallbackLaunch,"NSmlDmCallbackLaunch");


_LIT(KNSmlDMCallBackGlobal, "NSmlDmCallbackGlobal");
_LIT(KNSmlDMCallbackExeName, "NSmlDmCallbackServer.exe");


const TInt KNSmlDmCallbackServerVersionMinor = 0;
const TInt KNSmlDmCallbackServerVersionMajor = 1;
const TUint KNSmlDmCallbackDefaultHeapSize=0x140000;



enum TSmlDmCAllbackCommands
    {
    ECmdDMCallbackSetServer,
    ECmdDMCallbackGetLuid,
    ECmdDMCallbackGetSegmentList,
    ECmdDMCallbackGetLuidLength,
    ECmdDMCallbackGetSegmentListLength,
    ECmdDMCallbackAddMappingInfo,
    ECmdDMCallbackRemoveMappingInfo,
    ECmdDMCallbackRenameMapping,
    ECmdDMCallbackUpdateMapping,
    ECmdDMCallbackDeleteAcl,
    ECmdDMCallbackUpdateAcl,
    ECmdDMCallbackGetAcl,
    ECmdDMCallbackGetAclLength,
    ECmdDMCallbackCheckAcl,
    ECmdDMCallbackDefaultAclsToServer,
    ECmdDMCallbackDefaultAclsToChildren,
    ECmdDMCallbackEraseServerId,
    ECmdDMCallbackStartAtomic,
    ECmdDMCallbackCommitAtomic,
    ECmdDMCallbackRollbackAtomic,
    ECmdDMCallbackCloseSession,
    ECmdDMCallbackResourceCount
    };


#endif