/*
* Copyright (c) 2002-2006 Nokia Corporation and/or its subsidiary(-ies). 
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
*     SyncML Notifier  private CenRep key definitions
*
*/


#ifndef SYNCML_NOTIF_PRIVATE_CR_KEYS_H
#define SYNCML_NOTIF_PRIVATE_CR_KEYS_H


/**
* SyncML Notifier Private Keys UID
*/
const TUid KCRUidNSmlNotifierPrivateKeys = {0x101F8769};


// Device manager key UID
const TUid KPSUidNSmlDMSyncApp = {0x101f6de5};



//PubSub key used to mark which application is handling the process of download
//Sets by DM UI, NSC or NSCBackground
const TUint32 KNSmlCurrentFotaUpdateAppName = 0x0000000A;


//PubSub key used to define should "Install Now?" query be shown or not
//Sets by DM UI or NSC
const TUint32 KDevManShowInstallNowQuery = 0x0000000B;


/**
* CenRep key for defining whether DM session blocked in roaming or not.
* 
*
* Possible integer values:
* 0 or 1
* 0 means that feature is disabled
* 1 means feature is enabled
* Default value: 0 (not enabled)
*
* If 1 then if the phone is in roaming server alert message is not 
* processed hence DM session doesnot get started
*/
//const TUint32 KNSmlDMBlockedInRoaming = 0x00000001;

#endif // SYNCML_NOTIF_PRIVATE_CR_KEYS_H

