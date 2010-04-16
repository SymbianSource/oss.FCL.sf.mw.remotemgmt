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


#ifndef SYNCML_DEVMAN_INTERNAL_CR_KEYS_H
#define SYNCML_DEVMAN_INTERNAL_CR_KEYS_H

/**
* SyncML Device Management Internal Keys UID
*/
const TUid KCRUidDeviceManagementInternalKeys = {0x101F9A0A};

/**
* CenRep key for Client Initiated Firmware Update:
* Device Management profile id corresponding the device management
* server where firmware update request should be sent.
* Key is reseted (set to default) by device management client after request 
* has been sent.
*
* Possible integer values:
* positive integer values that match existing device management profile id
* in the device management settings database.
*
* Default value: -1 (not defined)
*
* Note: setting this key does not automatically initiate device management session,
* caller should start the session itself with appropriate profile id using SyncML
* Client API from SyncML Common subsystem.
*
*/
const TUint32 KDevManClientInitiatedFwUpdateId = 0x00000001;

/*
* CenRep key for the session type whether the session is FOTA/ DM Configuration session
* Default value: 0 
*
*/
const TUint32 KDevManSessionType = 0x00000002;

/**
* Cenrep key for SANUI Bit Variation
*/
const TUint32 KDevManSANUIBitVariation =  0x00000003;

/**
* Cenrep key for application launch notifier timeout
*/
const TUint32 KDevManServerAlertTimeout = 0x00000004;

/** 
* Cenrep for variating the user interaction alerts 
**/
const TUint32 KDevManUserInteractionAlerts = 0x00000005;

/**
* Cenrep for providing path of the dm profiles XML file
*/
 const TUint32 KDevManProfilesXMLFile = 0x00000006;
 
 /**
* Cenrep key for defining FUMO predefined node
*/

const TUint32  KDevManFUMOPredefinedNodes = 0x00000007;

/*
* This flag defines if the feature to enable  the customized UI notes for Package 0 informative ,
* interacttrive and note between firware install and update.
*/
const TUint32 KDevManUINotesCustomization = 0x00000008;
/*
* This key is used to enable DM session time out feature.
* Default value : -1 
* Allowed values.
* 1 - 30 .
* This unit is in minutes .
* If value is -1 or any other value  then this feature OFF. And default behavior.
* Value :  1-30, session times out withthin this time E.g If value is 5 min,
* then a call comes which gets high priorioty than GPRS , Dm session waits for 5 min and times out if Call doesn't end. 
*/

const TUint32 KDevManDMSessionTimeout = 0x00000009; 

/*
* This key stores the server ID of current DM Session. This server ID is used by DMNetMon
* to start DM Session after network is up
*/

const TUint32 KDevManServerIdKey = 0x0000000A; 

/*
* This key stores the IAP ID of current DM Session. This IAP ID is used by DMNetMon
* to start DM Session after network is up
*/

const TUint32 KDevManIapIdKey = 0x0000000B; 

/*
* This key checks whether Network Monitoring can be enabled by DM Sync Agent.
*/

const TUint32 KDevManEnableDMNetworkMon = 0x0000000C;

/*
* This key checks whether Network Monitoring can be in roaming.
*/

const TUint32 KDevManEnableDMNetworkMonInRoaming = 0x0000000D;

/* 
* This key is used to indicate shut down time for DM NetMOn.
*/

const TUint32 KDevManNetMonTimeoutKey = 0x0000000E;
/*
* This key is used to override profilelock feature
* Default value : 0 
* Allowed values.
* 0 or 1 
* If value is 0 then locked profile values can not be repalced or deleted And default behavior.
* If value is 1  then locked profile values can changed.
* 
*/
const TUint32 KDevManEnableHbNotifier = 0x0000000F; 

/*
* This key is used to store the External memory card status for DCMO
* Default value : 1 
* Allowed values.
* 0 or 1 
* If value is 0 then the card is dismounted.
* If value is 1  then the card is mounted and is the defaulf value.
* 
*/
const TUint32 KDevManMemCardCtrl = 0x00000010; 

#endif // SYNCML_DEVMAN_INTERNAL_CR_KEYS_H

