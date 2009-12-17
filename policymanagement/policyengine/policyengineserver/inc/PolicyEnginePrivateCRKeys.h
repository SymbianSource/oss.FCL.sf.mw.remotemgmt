/*
* Copyright (c) 2002-2004 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description: 
*     SyncML Device Management internal CenRep key definitions
*
*/


#ifndef POLICY_ENGINE_PRIVATE_CR_KEYS_H
#define POLICY_ENGINE_PRIVATE_CR_KEYS_H

/**
* SyncML Device Management Internal Keys UID
*/
const TUid KPolicyEngineRepositoryID = {0x10207815};
const TUid KPolicyEngineBackupRepositoryID = {0x10207816};

/**
* CenRep key for Next free ID:
* Holds the next free if for policy component
*
* Possible integer values:
* positive integer values 
*
* Default value: -1 (not defined)
*
* Note: Only for Policy Engine private use
*
*/
const TUint32 KNextFreeId = 0x00000001;

/**
* CenRep key for commit flag:
* Determines when policy operation is ready.If something goes wrong (phone is turned off during operation),
* this flag indicates in the next boot restoring backup is required.
*
* Possible integer values:
* 0 or 1. 0 indicates that the operations is not commited. 1 indicates that the operation is committed
* and policy management is in the valid state.
*
* Default value: -1 (not defined)
*
* Note: This flag is written by the policy engine. Other policy management components can read a flag.
*
*/
const TUint32 KCommitFlag = 0x00000003;

/**
* CenRep key for enforcement state keys:
* This range is reserved for policy engine private use and it keeps track about enforcement state.
*
* Possible integer values:
*
* Default value: -1 (not defined)
*
* Note: This flag is start of the range which is dedicated for enforcement states
*
*/
const TUint32 KEnforcementStates = 0x00000100;

#endif // POLICY_ENGINE_PRIVATE_CR_KEYS_H

