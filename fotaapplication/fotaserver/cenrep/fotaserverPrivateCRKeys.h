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

// DM UI Silent feature variation
const TUint32 KSilentOmaDlUIBehavior = 0x00000002;

//Flag to enable/disable the  feature ,.."Send Generic alert after device reboots"
//Allowed values   1  , 0 
const TUint32 KGenericAlertResendAfterBoot = 0x00000003;

//Flag to configure number of retries for sending GA in failure cases.

const TUint32 KGenericAlertRetries = 0x00000004;

//Determines state of fota update.
//0 - Default , no action taken. 
//1 - Firmware update/GA 
//2. Download Interrupted

const TUint32 KFotaUpdateState   = 0x00000005;

//Determines whether the Fota Suspend and Resume Feature is supported or not.
//0 (default) - Feature OFF 
//1   - Feature ON

const TUint32 KSuspendResumeFeatureSupported = 0x00000006;

//Determines whether the Fota Monitory Service is enabled or not.
//0 (default ) - Feature OFF
//1   - Feature ON

const TUint32  KFotaMonitoryServiceEnabled = 0x00000007;

//This key is to configure  the reminder options .
//Format :
//1h:4h:1d:3m:NROFF
//h - hour/s. 
//d- day/s
//m - minute/s
//":" Is the separator.
//Currently only first 4 options are read to show reminder duration.
//5th option can have value NROFF which means that "No reminder" option  will not be shown.
//if 5th option has any other value other than NROFF , "No reminder" option will be shown
//Default value : NULL.  Which means current options  1h:4h:1d:3d are taken 
//into consideration along with No Reminder option.

const TUint32 KFotaOptionsForReminderDuration = 0x00000008;

//This key is to enable feature to reset the timer after device reboot  .
//If value is 0 : Feature is OFF. - Default value
//If value is 1  : Feature is ON.
//Any other value its feature OFF
//If feature is ON then if there was a reminder  set once 
//device reboots timer should be reset and note to install Firmware update is shown .

const TUint32 KFotaReminderTimerReset = 0x00000009;
//This key is to enable feature to reset the timer after device reboot  .
//If value is 0 :  Dynamo charger
//If value is 1  : USB charger
//Any other value its feature OFF

const TUint32 KFotaUSBChargerEnabled  = 0x0000000A; 

//This key stores the drive id to encrypt after update.
const TUint32 KDriveToEncrypt  = 0x0000000B;

//Determines whether FOTA Install Update note to be displayed or not,
//when a phone call is active
//0 (default) - Install update note not displayed when a call is active 
//1   - Install update note will be displayed when a call is active
const TUint32 KFotaUpdateNoteDisplayAlways= 0x0000000C;

/*
* This key is used to determine the maximum number of postpones allowed for a FOTA Update.
* Default value : 3
*/

const TUint32 KFOTAMaxPostponeCount = 0x0000000D; 


/*
* This key is used to determine the number of postpones done by the user for a particular FOTA Update.
* Default value : 0
*/

const TUint32 KFOTAUserPostponeCount = 0x0000000E; 


/*
* This flag defines if the feature to enable  the customized UI notes for interacttrive and note between firware install and update.
* Default value : 0
*/

const TUint32 KFOTAUINotesCustomization = 0x0000000F; 

#endif // FOTASERVER_PRIVATE_KEYS_H
            
// End of File
