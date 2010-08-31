/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Orbit specific constants for the device dialog.
*
*/

#ifndef DEVICEDIALOGCONSTS_H_
#define DEVICEDIALOGCONSTS_H_

#include <e32def.h>
#include <qstring.h>

/// Number of retries for PIN
 const TInt KPinRetries = 3;
 
 // PIN Max Length
 const TInt KCPPinMaxLength = 20;
 
//Strings for CP PIN Query Dialog
const QString pinquery = "omacppinquery";

const char devicedialogtype[] = "com.nokia.hb.devicemanagementdialog/1.0";
_LIT(KDeviceDialogType, "com.nokia.hb.devicemanagementdialog/1.0");

// Keys for the QVariantMap

const QString keydialog = "Dialog";
const QString keyparam1 = "Param1";
const QString keyparam2 = "Param2";
const QString keyparam3 = "Param3";
const QString keyparam4 = "Param4";
const QString returnkey = "returnkey";

_LIT(KKeyDialog, "Dialog");
_LIT(KKeyParam1, "Param1");
_LIT(KKeyParam2, "Param2");
_LIT(KKeyParam3, "Param3");
_LIT(KKeyParam4, "Param4");
_LIT(KResult,"returnkey");

// Enumerations to show the type of dialog to be shown from FOTA. The Enumeration ranges from 1 to 100.

enum TFwUpdNoteTypes
    {
    //Download related notes
    EFwDLNeedMoreMemory = 100,
    EFwDLConnectionFailure,
    EFwDLGeneralFailure,
    EFwDLNonResumableFailure,
    
    //Updated related notes	
    EFwUpdNotEnoughBattery,
    EFwUpdDeviceBusy,
		EFwUpdSuccess,
		EFwUpdNotCompatible,
    EFwUpdRebootNote,
    
		//Download related query
		EFwUpdResumeDownload,
		
		//Update related query    
    EFwUpdResumeUpdate
    
    };
 
 enum TOmaCPPinDialogTypes
    {
    //Download related notes
    EOmaCPPinDialog = 200
    
		};  
 

enum THbNotifierKeys 
    {
    EHbNotifierStatus = 10,
    EHbNotifierStatusReturn // Return the content of actual status value accepted from UI
    };

enum TSoftkeys
    {
    EHbLSK = 1,
    EHbMSK,
    EHbRSK
    };



#endif /* DEVICEDIALOGCONSTS_H_ */
