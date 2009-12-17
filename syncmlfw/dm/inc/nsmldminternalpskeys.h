/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  DM publish & subscribe keys 
*
*/



#ifndef __NSMLDMINTERNALPSKEYS_H
#define __NSMLDMINTERNALPSKEYS_H

// CONSTANTS

_LIT_SECURITY_POLICY_C1( KReadPolicy, ECapabilityReadDeviceData );
_LIT_SECURITY_POLICY_C1( KWritePolicy, ECapabilityWriteDeviceData );

// Dm Sync agent Uid
const TUid KPSUidNSmlDMSyncAgent = {0x101f9a0a};

// PubSub key used to share information about adding the leaf node in the
// current DM session.
// Contains value as -1, when the DM command from server is other than Add
// Contains value as 1,when the command from DM server is Add(may be node/leaf)
// Contains value as 2, when the command from DM server is Add on existing
// leaf node
const TUint32 KNSmlDMCmdAddOnExistingNodeorLeafKey = 0x0000000A;

// PubSub key used to share information about node adding in current Dm session
// For success value is 1, set by DM Host session
const TUint32 KNSmlDMCmdAddNodeSuccess = 0x0000000B;

// Enum for the P&S key KNSmlDMCmdAddOnExistingLeafKey
enum TNSmlDmSyncCmdOnLeafNode
	{
	EAddCmd = 1, //DM cmd is Add (on leaf/internal node)
	EAddOnExistingNode
	};
	
// Enum for the P&S key KNSmlDMCmdAddNodeSuccess
enum TNSmlDmSyncAddCmdOnNode
	{
	ENotAdded = 0, //buffered
	EAdded,
	EFailed
	};		
#endif // __NSMLDMINTERNALPSKEYS_H
