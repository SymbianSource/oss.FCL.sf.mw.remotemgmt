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
* Description:  SyncMLNotifier Domain CenRep key definitions
*
*/

#ifndef NSMLOPERATORERRORCRKEYS_H
#define NSMLOPERATORERRORCRKEYS_H

#include <e32std.h>

/**
 * SyncML framework operator settings
 *
 */
const TUid KCRUidOperatorDatasyncErrorKeys = { 0x2001FDF1 };

/**
 * KNsmlOpDsErrorCode
 * Define operator specific SyncML error code
 *
 */
const TUint32 KNsmlOpDsErrorCode = 0x00000001;

/**
 * KNsmlOpDsSyncProfUid
 * Defines operator sync profile uid
 */
const TUint32 KNsmlOpDsSyncProfUid = 0x00000002;

/**
 * KNsmlOpDsSyncType
 * Define sync type
 *
 */
const TUint32 KNsmlOpDsSyncType = 0x00000003;

/**
 * KNsmlOpDsSyncInitiation
 * Define sync initiation
 *
 */
const TUint32 KNsmlOpDsSyncInitiation = 0x00000004;

#endif //NSMLOPERATORERRORCRKEYS_H
