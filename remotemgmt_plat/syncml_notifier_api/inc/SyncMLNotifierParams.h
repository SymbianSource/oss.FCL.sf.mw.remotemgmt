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
* Description:  Partial declaration of the notifier API.
*
*/



#ifndef SYNCMLNOTIFIERPARAMS_H
#define SYNCMLNOTIFIERPARAMS_H

//  INCLUDES

// CONSTANTS
const TUint KSyncMLMaxServerMsgLength = 200;
const TUint KSyncMLMaxProfileNameLength = 50;
const TUint KSyncMLMaxDefaultResponseMsgLength = 300;
const TUint KSyncMLMaxAlertResultLength = 150;
const TUint KSyncMLChoiceItemsLengthBuffer = 400;
const TUint KSyncMLMaxChoiceItemLength = 100;
const TUint KSyncMLChunknameLength = 25;

// MACROS

// FORWARD DECLARATIONS
class TSyncMLAppLaunchNotifParams;
class TSyncMLDlgNotifParams;
class TSyncMLAppLaunchNotifRetVal;
class TSyncMLDlgNotifRetVal;
class TSyncMLFwUpdNotifParams;
class TSyncMLFwUpdNotifRetVal;
class TSyncMLDlgNotifReturnParams;


// DATA TYPES
typedef TPckgBuf<TSyncMLAppLaunchNotifParams> TSyncMLAppLaunchNotifParamsPckg;
typedef TPckgBuf<TSyncMLDlgNotifParams>       TSyncMLDlgNotifParamsPckg;
typedef TPckgBuf<TSyncMLFwUpdNotifParams>     TSyncMLFwUpdNotifParamsPckg;
typedef TPckgBuf<TSyncMLAppLaunchNotifRetVal> TSyncMLAppLaunchNotifRetValPckg;
typedef TPckgBuf<TSyncMLDlgNotifRetVal>       TSyncMLDlgNotifRetValPckg;
typedef TPckgBuf<TSyncMLFwUpdNotifRetVal>     TSyncMLFwUpdNotifRetValPckg;
typedef TPckgBuf<TSyncMLDlgNotifReturnParams>    TSyncMLDlgNotifReturnParamsPckg;


enum TSyncMLSessionTypes
    {
    ESyncMLUnknownSession,
    ESyncMLSyncSession,
    ESyncMLMgmtSession,
    };

enum TSyncMLDlgNoteTypes
    {
    ESyncMLInfoNote,
    ESyncMLErrorNote,
    ESyncMLOkQuery,
    ESyncMLYesNoQuery,
    ESyncMLQueryNote,  // Deprecated. Use ESyncMLOkQuery or ESyncMLYesNoQuery instead
    ESyncMLInputQuery, //For 1102 User Input alert 	
	ESyncMLSingleChoiceQuery, //1103
    ESyncMLMultiChoiceQuery  //1104
    };

enum TSyncMLFwUpdNoteTypes
    {
    ESyncMLFwUpdUnknown,
    ESyncMLFwUpdErrorNote,
    ESyncMLFwUpdResultNote,
    ESyncMLFwUpdStartQuery,
    ESyncMLFwUpdRebootNote,
    ESyncMLFwUpdNoReminder,       //Enum for No Reminder & Install Later note
    ESyncMLFwUpdLargeNotEnoughMemory,  //Enum for large object No Enough memory situation
    ESyncMLFwUpdOmaDLNotEnoughMemory, //Enum for Oma Download No Enough memory situation
    ESyncMLFwUpdOmaDLUserCancel,	//Enum for Oma Download User cancel situation
    ESyncMLFwUpdOmaDLPostponed,	//Enum for Oma Download postponed
    ESyncMLFwUpdOmaDLCancelled, //Enum for Oma Download cancelled
    ESyncMLFwUpdOmaDLResume,        //Enum for Oma Download Resume
    ESyncMLFwUpdStartQueryEnc      //Enum for starting installation on encrypted memory

    };


enum TSyncMLFwUpdNoteInfo
    {
    KErrCommsLineFail2, //Enum for Oma Download connection failure with resume supported
    KErrGeneralResume, //Enum for Oma Download general error with resume supported
    KErrGeneralNoResume, //Enum for Oma Download general error without resume supported
    KErrBadUsbPower,      //Enum for low battery error when USB charging is supported 
    KErrBadPowerForEnc,     //Enum for low battery error during encryption
    KErrDeviceMemoryBusy    //Enum to show drive busy error when encryption or decryption is in progress
    };


enum TSyncMLSANSupport
    {
    ESANNotSpecified,
    ESANSilent,
    ESANUserInformative,
    ESANUserInteractive
    };
    
enum TSyncMLUserInputTypes
    {
    ESyncMLInputTypeAlphaNumeric,
    ESyncMLInputTypeNumeric,
    ESyncMLInputTypeDate,
    ESyncMLInputTypeTime,
    ESyncMLInputTypePhoneNumber,
    ESyncMLInputTypeIPAddress	
    };
    
 enum TSyncMLUserInputEchoTypes
    {
    ESyncMLEchoTypeText,
    ESyncMLEchoTypePassword
    };   

    
// CLASS DECLARATION

/**
* Struct to allow the sending of parameters to SyncML application
* starter notifier plugin.
*
*  @lib SyncMLNotifier
*  @since Series 60 3.0
*/
class TSyncMLAppLaunchNotifParams
	{
    public:
        // Type of the SyncML session to be initiated. 
	    TSyncMLSessionTypes iSessionType;
        // Identifier of the job requested.
	    TInt iJobId;
	    // Identifier of the profile used
	    TInt iProfileId;
       // Server alert Ui mode
	    TInt iUimode;
	};

/**
* Struct to allow the delivery of return values from SyncML application
* starter notifier plugin.
*
*  @lib SyncMLNotifier
*  @since Series 60 3.0
*/
class TSyncMLAppLaunchNotifRetVal
	{
    public:
        // Identifier of the session requested.
	    TUid iSecureId;
	};


/**
* Struct to allow the sending of parameters to SyncML server
* dialog notifier plugin.
*
*  @lib SyncMLNotifier
*  @since Series 60 3.0
*/
class TSyncMLDlgNotifParams
	{
    public:
        // Type of the query or note to be shown.
	    TSyncMLDlgNoteTypes iNoteType;
        // Message to be shown on the screen.
	    TBufC<KSyncMLMaxServerMsgLength> iServerMsg;
        // Timeout of the note (in seconds). 0 = No timeout.
	    TInt iMaxTime;
	    //Response message in case of 1102 alert
	    TBuf<KSyncMLMaxDefaultResponseMsgLength> iDR;		
	    //Input type
	    TInt iIT;
	    //Echo type
	    TInt iET;
	    //Max length
	    TInt iMaxLength;	    
	    //For choice alerts	 
	    TBufC<KSyncMLChoiceItemsLengthBuffer> iItemLength;
		TBufC<KSyncMLChunknameLength> iChunkName;
		//Number of choice items
	    TInt iNumberOfItems;			   
	};
	


//For 1102 server alert
class TSyncMLDlgNotifReturnParams
   {
   public:
   TInt iretval; //which stores the yes/no/end key pressed
   TBuf<KSyncMLMaxAlertResultLength> irettext;	
   };


/**
* Struct to allow the delivery of return values from SyncML server
* dialog notifier plugin.
*
*  @lib SyncMLNotifier
*  @since Series 60 3.0
*/
class TSyncMLDlgNotifRetVal
	{
	// Included for future use.
	};

/**
* Struct to allow the sending of parameters to SyncML firmware
* update notifier plugin.
*
*  @lib SyncMLNotifier
*  @since Series 60 3.1
*/
class TSyncMLFwUpdNotifParams
	{
    public:
        // Type of the SyncML session to be initiated. 
	    TSyncMLFwUpdNoteTypes iNoteType;
	    
        // Generic integer parameter.
        // Usage:
        // ESyncMLFwUpdErrorNote:  Symbian error code.
        // ESyncMLFwUpdResultNote: Symbian error code.
        // ESyncMLFwUpdStartQuery: Firmware update profile identifier.
	    TInt iIntParam;
	    TInt iMemoryNeeded;// Parameter to hold how much memory is to be
	                       // freed for downloading the package.
	    TBool iEncryptReq; //To tell if encrypt query should be shown to user.
	};

/**
* Struct to allow the delivery of return values from SyncML firmware
* update notifier plugin.
*
*  @lib SyncMLNotifier
*  @since Series 60 3.1
*/
class TSyncMLFwUpdNotifRetVal
	{
	// Included for future use.
	};

#endif      // SYNCMLNOTIFIERPARAMS_H   

// End of File
