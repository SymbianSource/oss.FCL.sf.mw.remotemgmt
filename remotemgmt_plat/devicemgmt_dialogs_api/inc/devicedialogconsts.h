/*
 * devicedialogconsts.h
 *
 *  Created on: Apr 20, 2010
 *      Author: gnataraj
 */

#ifndef DEVICEDIALOGCONSTS_H_
#define DEVICEDIALOGCONSTS_H_


/// Number of retries for PIN
 const TInt KPinRetries = 3;
 
 // PIN Max Length
 const int KCPPinMaxLength = 20;
 
//Strings for CP PIN Query Dialog
const QString pinquery = "omacppinquery";
const QString returnkey = "returnkey";

const char devicedialogtype[] = "com.nokia.hb.devicemanagementdialog/1.0";
_LIT(KDeviceDialogType, "com.nokia.hb.devicemanagementdialog/1.0");

// Keys for the QVariantMap

const QString keydialog = "Dialog";
const QString keyparam1 = "Param1";
const QString keyparam2 = "Param2";
const QString keyparam3 = "Param3";

// Enumerations to show the type of dialog to be shown from FOTA. The Enumeration ranges from 1 to 100.

enum TFwUpdNoteTypes
    {
    //Download related notes
    EFwDLNeedMoreMemory = 100,
    EFwDLConnectionFailure,
    EFwDLGeneralFailure,
    
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
