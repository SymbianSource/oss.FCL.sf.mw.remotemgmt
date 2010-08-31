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
* Description:  SyncML DS Error reporting API CenRep key definitions
*
*/

#ifndef NSMLOPERATORERRORCRKEYS_H
#define NSMLOPERATORERRORCRKEYS_H

#include <e32std.h>

/**
 * SyncML DS error reporting keys
 *
 */
const TUid KCRUidOperatorDatasyncErrorKeys = { 0x2001FDF1 };

/**
 * KNsmlOpDsSyncErrorCode
 * Holds error code of latest DS sync (0 if no error occured).
 * Storing of error codes is configured in KCRUidOperatorDatasyncInternalKeys
 * (nsmloperatordatacrkeys.h).
 */
const TUint32 KNsmlOpDsSyncErrorCode = 0x00000001;

/**
 * KNsmlOpDsSyncProfId
 * Holds sync profile Id used in last DS sync
 */
const TUint32 KNsmlOpDsSyncProfId = 0x00000002;

/**
 * KNsmlOpDsSyncType
 * Holds sync type used in last DS sync
 */
const TUint32 KNsmlOpDsSyncType = 0x00000003;

/**
 * KNsmlOpDsSyncInitiation
 * Holds sync initiation used in last DS sync
 */
const TUint32 KNsmlOpDsSyncInitiation = 0x00000004;

#endif //NSMLOPERATORERRORCRKEYS_H
