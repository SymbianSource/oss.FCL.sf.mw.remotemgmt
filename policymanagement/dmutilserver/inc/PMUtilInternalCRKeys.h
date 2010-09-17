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


#ifndef PM_UTIL_PRIVATE_CR_KEYS_H
#define PM_UTIL_PRIVATE_CR_KEYS_H

/**
* SyncML Device Management Internal Keys UID
*/
const TUid KCRUidPolicyManagementUtilInternalKeys = {0x10207843};

/**
* CenRep key for Session Certificate ID:
* Contains DM session certificate
*
* Possible binary values:
* Certificate in a "TPckg<TCertInfo>"-format
*
* Default value: -1 (not defined)
*
*
*/
const TUint32 KSyncMLSessionCertificate = 0x00000001;

/**
* CenRep key for policy change counter:
* Indicates changes in policy structure.
*
* Possible integer values:
* Positive numbers
*
* Default value: -1 (not defined)
*
*
*/
const TUint32 KPolicyChangeCounter = 0x00000002;


/**
* CenRep key for terminal security state key:
* Indicates terminal security is turned on or off
*
* Possible integer values:
* 0 terminal security is not active, 1 terminal security is active
*
* Default value: 0
*
*
*/
const TUint32 KTerminalSecurityStateKey = 0x00000003;


/**
* CenRep key for RFS counter:
* Indicates that RFS is performed
*
* Possible integer values:
* Positive numbers
*
* Default value: -1 (not defined)
*
*
*/
const TUint32 KRFSCounterKey = 0x00000004;

/**
* CenRep key for RFS counter:
* Indicates that RFS is performed
*
* Possible integer values:
* Positive numbers
*
* Default value: -1 (not defined)
*
*
*/


/**
* CenRep key for SyncML session parameters:
* Used to delivery session parameters in silent DM mode
*
* Possible binary values:
* Parameters in a "TPckg<TSyncMLAppLaunchNotifParams>"-format
*
* Default value: -1 (not defined)
*
*
*/
const TUint32 KSyncMLSessionParamsKey = 0x00000005;

/**
* CenRep key for SyncML forced certificate check:
* Indicates is certificate check needed when connection is established (needed in silent DM mode)
*
* Possible binary values:
* 0 certificate check is off, 1 certificate check is on.
*
* Default value: -1 (not defined)
*
*
*/
const TUint32 KSyncMLForcedCertificateCheckKey = 0x00000006;

/**
* CenRep key for certificate label counter:
* Indicates current label value of certificate.
*
* Possible integer values:
* Positive numbers
*
* Default value: 0 (not defined)
*
*
*/
const TUint32 KCertificateCounter = 0x00000007;

/**
* CenRep key for server id:
* Indicates current trusted server id.
*
* Possible string values:
* some string names
*
* Default value: NULL(not defined)
*
*
*/
const TUint32 KTrustedServerId = 0x00000008;


#endif // PM_UTIL_PRIVATE_CR_KEYS_H

