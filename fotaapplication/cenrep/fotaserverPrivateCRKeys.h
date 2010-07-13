/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:   	CR keys for fotaserver
*
*/



#ifndef FOTASERVER_PRIVATE_KEYS_H
#define FOTASERVER_PRIVATE_KEYS_H

// CONSTANTS

// FS key UID
const TUid KCRUidFotaServer = { 0x102072C4 };

// Device manager key UID
const TUid KPSUidNSmlDMSyncApp = {0x101f6de5};
// Whether update agent should be simulated by FS
const TUint32 KSimulateUpdateAgent = 0x00000001;

//Flag to enable/disable the  feature ,.."Send Generic alert after device reboots"
//Allowed values   1  , 0 
const TUint32 KGenericAlertResendAfterBoot = 0x00000002;

//Flag to configure number of retries for sending GA in failure cases.

const TUint32 KGenericAlertRetries = 0x00000003;

//Determines state of fota update.
//0 - Default , no action taken. 
//1 - Firmware update/GA 
//2. Download Interrupted

const TUint32 KFotaUpdateState   = 0x00000004;

//Determines whether the Fota Monitory Service is enabled or not.
//0 (default ) - Feature OFF
//1   - Feature ON

const TUint32  KFotaMonitoryServiceEnabled = 0x00000005;

/*
* This key is used to determine the maximum number of postpones allowed for a FOTA Update.
* Default value : 3
*/

const TUint32 KFOTAMaxPostponeCount = 0x00000006; 


/*
* This key is used to determine the number of postpones done by the user for a particular FOTA Update.
* Default value : 0
*/

const TUint32 KFOTAUserPostponeCount = 0x00000007; 

const TUint32 KUpdateRequesterUid = 0x0000008;

const TUint32 KFOTADownloadRestartCount = 0x0000009;

#endif // FOTASERVER_PRIVATE_KEYS_H
            
// End of File
