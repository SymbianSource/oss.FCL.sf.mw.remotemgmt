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
* Description:  Symbian specific constants for the device dialog.
*
*/



#ifndef DEVICEDIALOGSYMBIAN_H
#define DEVICEDIALOGSYMBIAN_H

//  INCLUDES

// CONSTANTS
const TUint KSyncMLMaxServerMsgLength = 200;

// MACROS

// FORWARD DECLARATIONS
class TSyncMLAppLaunchNotifParams;
class TSyncMLDlgNotifParams;

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
    ESyncMLYesNoQuery
  };
  
enum TSyncMLSANSupport
    {
    ESANNotSpecified,
    ESANSilent,
    ESANUserInformative,
    ESANUserInteractive
    };

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
	    //Max length
	    TInt iMaxLength;	    
	};
	

#endif      // DEVICEDIALOGSYMBIAN_H   

// End of File
