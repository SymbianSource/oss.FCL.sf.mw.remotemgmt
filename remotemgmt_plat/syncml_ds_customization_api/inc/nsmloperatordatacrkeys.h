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
* Description:  SyncML DS customization API CenRep key definitions
*
*/

#ifndef NSMLOPERATORDATACRKEYS_H
#define NSMLOPERATORDATACRKEYS_H

#include <e32std.h>

/**
 * SyncML framework operator settings
 *
 */
const TUid KCRUidOperatorDatasyncInternalKeys = { 0x2001E2E1 };

/**
 * KNsmlOpDsOperatorSyncServerURL
 * Define operator server URL
 *
 */
const TUint32 KNsmlOpDsOperatorSyncServerURL = 0x00000001;

/**
 * KNsmlOpDsOperatorAdapterUid
 * Define operator specific contact adapter uid
 *
 */
const TUint32 KNsmlOpDsOperatorAdapterUid = 0x00000002;

/**
 * KNsmlOpDsProfileAdapterUid
 * Define profile contact adapter uid
 *
 */
const TUint32 KNsmlOpDsProfileAdapterUid = 0x00000003;

/**
 * KNsmlOpDsDevInfoSwVValue
 * Define Software version for operator devinfo
 *
 */
const TUint32 KNsmlOpDsDevInfoSwVValue = 0x00000004;

/**
 * KNsmlOpDsDevInfoModValue
 * Define Mod value for operator devinfo
 *
 */
const TUint32 KNsmlOpDsDevInfoModValue = 0x00000005;

/**
 * KNsmlOpDsSyncProfileVisibility
 * Define visibility setting for operator specific sync profile
 *
 */
const TUint32 KNsmlOpDsSyncProfileVisibility = 0x00000006;

/**
 * KNsmlOpDsOperatorSyncServerId
 * Define operator server Id
 *
 */
const TUint32 KNsmlOpDsOperatorSyncServerId = 0x00000007;

/**
* KNsmlOpDsDevInfoManValue
* Define Man value for operator devinfo
*
*/
const TUint32 KNsmlOpDsDevInfoManValue = 0x00000008;

/**
 * KNsmlOpDsSyncMLStatusCodeList
 * Define list of SyncML status codes for error reporting
 *
 */
const TUint32 KNsmlOpDsSyncMLStatusCodeList = 0x00000009;

/**
 * KNsmlOpDsHttpErrorReporting
 * Define if HTTP status codes are reported
 *
 */
const TUint32 KNsmlOpDsHttpErrorReporting = 0x0000000A;

/**
 * KNsmlOpDsSyncMLErrorReporting
 * Define if SyncML status codes are reported
 *
 */
const TUint32 KNsmlOpDsSyncMLErrorReporting = 0x0000000B;

#endif //NSMLOPERATORDATACRKEYS_H
