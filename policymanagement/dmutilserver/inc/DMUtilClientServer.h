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
// DMUtilClientServer.h

#ifndef __DMUTIL_CLIENTSERVER_H__
#define __DMUTIL_CLIENTSERVER_H__

#include <e32std.h>



_LIT(KDMUtilServerName,"dmutilserver");
_LIT(KDMUtilServerImg,"DMUtilServer");         // DLL/EXE name

const TUid KDMUtilServerUid={0x10207843};


enum TDMUtilServerMessages
	{
    	EGetDMSessionCertInfo,
    	EGetDMSessionServerId,
    	EPerformDMUtilRFS,
    	ERemoveACL, 
    	ESetACL,
    	EAddACL,
    	EMngSessionCertificate,
    	ENewDMSession,
    	EFlush,
    	EUpdatePolicyMngStatusFlags,
    	EMarkMMCWipe    	
	};

//values can be used as a mask values....
enum TAclCommands
{
	EACLAdd = 1,		//000001b
	EACLDelete = 2,		//000010b
	EACLExec = 4,		//000100b
	EACLGet = 8,		//001000b
	EACLReplace = 16,	//010000b
	EACLAll = 32		//100000b
};

//defines destination of ACL operation
enum TACLDestination
{
	EForNode,
	EForChildrens,
	EForBoth
};

//values to be used is policy management state updates
enum KPolicyMngStatusFlags
{
	EPolicyChanged = 0,
	ETerminalSecurityActive,
	ETerminalSecurityDeactive
};


#endif// __DMUTIL_CLIENTSERVER_H__
