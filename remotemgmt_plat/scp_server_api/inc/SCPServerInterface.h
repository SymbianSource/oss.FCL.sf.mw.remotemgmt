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
* Description: 
*       Contains the definitions needed for communication with the SCP server.
*
*
*/


#ifndef SCPSERVER_INTEFFACE_H
#define SCPSERVER_INTEFFACE_H

//  INCLUDES
#include "SCPPServerPluginDefs.hrh"

typedef TBuf<8> TSCPSecCode;
const TInt KSCPMaxEnhCodeLen = 256;

// New DataType to store encrypted information of maximum size 128.
typedef TBuf<128> TSCPCryptoCode;

// LOCAL CONSTANTS
_LIT( KSCPServerName, "!SCPServer" );
_LIT( KSCPServerFileName, "SCPServer" );
_LIT( KSCPServerSemaphoreName, "SCPServerSem" );

// The available functions
enum TSCPServRqst 
	{
	ESCPServGetCode = 0,
	ESCPServChangeCode,
	ESCPServSetPhoneLock,
	ESCPServGetLockState,
	ESCPServSetParam,
	ESCPServSetCode = 10,
	ESCPServQueryAdminCmd = 20,
	ESCPServGetParam = 30,
	ESCPServAuthenticateS60 = 40,
	ESCPServChangeEnhCode,
	ESCPServCodeChangeQuery,
	ESCPServValidateLockcode,
	ESCPServCheckConfig = 50,
	ESCPApplicationUninstalled = 60,
	ESCPServUISetAutoLock = 70
	};

// The admin commands that can be queried for
enum TSCPAdminCommand
    {
    ESCPCommandLockPhone,
    ESCPCommandUnlockPhone
    };

// The parameters available from the server
enum TSCPParameterID
    {
    ESCPAutolockPeriod,
    ESCPMaxAutolockPeriod,
    ESCPCodeChangePolicy
    };

enum TDevicelockPolicies {
    //Autolock timeout. Values in minutes
    EDeviceLockAutolockperiod = 0,
    // Maximum autolock timeout value, Values in minutes
    EDeviceLockMaxAutolockPeriod,
    //specifies the minimum lock code length
    EDeviceLockMinlength,
    //Allowed maxim lock code length
    EDeviceLockMaxlength,
    // 0 = No restriction,
    //1 = Both upper and lower case letters are required in the password
    EDeviceLockRequireUpperAndLower,
    // 0 = No restriction,
    // 1 = Both characters and numbers are required in the password
    EDeviceLockRequireCharsAndNumbers,
    // 0 = No restriction,
    // 1-4 = A single character cannot be used more than X times in the password
    EDeviceLockAllowedMaxRepeatedChars,
    // 0 = No restriction,
    // 1-X = The new password cannot match the previous X passwords
    EDeviceLockHistoryBuffer,
    // 0 = No restriction,
    // 1-365 = The password expires after X days and must be changed by the user
    // -1 = password expires immediately
    EDeviceLockPasscodeExpiration,
    // The user can change the password only X times before EPasscodeMinChangeInterval //hours have passed (default 0).
    EDeviceLockMinChangeTolerance,
    // 0 = No restriction,
    // 1-1000 = The user cannot change the password more than EPasscodeMinChangeTolerance // times before X hours have passed since the previous change
    EDeviceLockMinChangeInterval,
    // disallow the specific string or strings given.
    EDeviceLockDisallowSpecificStrings,
    // 0 = No restriction,
    // 3-100 = The device is "hard" reset after the user has consecutively failed X times // to answer the password query
    EDeviceLockAllowedMaxAtempts,
    // 0 = No restriction, 1 = The password cannot contain two consecutive numbers.
    EDeviceLockConsecutiveNumbers,
    // 0 = No restriction,
    // 1-255 = The password should contain at least X number of special characters.
    EDeviceLockMinSpecialCharacters,
    // 0 = No restriction,
    // 1 = Single character repeat not allowed (ex: 222222, aaaaaa etc) are not allowed
    EDeviceLockSingleCharRepeatNotAllowed,
    // 0 = No restriction,
    // 1 = lock code shouldn’t consist of consecutive characters (ex: 12345, abcde etc) are notallowed
    EDevicelockConsecutiveCharsNotAllowed,

    EDevicelockTotalPolicies
};

// The version number of this client-server package
const TUint KSCPServMajorVersionNumber=0;
const TUint KSCPServMinorVersionNumber=9;
const TUint KSCPServBuildVersionNumber=0;

// Absolute minimum and maximum lengths of the passcode
const TInt KSCPPasscodeMinLength = 4;
const TInt KSCPPasscodeMaxLength = 255;

// Max text length of a TInt (-1*2^31)
const TInt KSCPMaxIntLength( 11 );
// Max text length of a TUInt64 (2^64)
const TInt KSCPMaxInt64Length( 21 );

// Max buffer length for an MD5 digest
const TInt KSCPMaxHashLength( 32 );
// MD5 digest length
const TInt KSCPMD5HashLen( 16 ); // 128 bits

// Max ISA code length
const TInt KSCPCodeMaxLen( 5 );

// Default parameters
_LIT( KSCPDefaultSecCode, "12345" );
_LIT( KSCPDefaultEnchSecCode, "12345" );

// Lengths for the TARM notifier param buffer
const TInt KSCPTARMSuffixLen = 4;
const TInt KSCPMaxTARMNotifParamLen = KSCPTARMSuffixLen + 512; 
// 512 chars through the event and 4 for the prefix

// Max length for the prompts for UI actions
const TInt KSCPMaxPromptTextLen = 100; 

// New Tarm event
const TUint KSCPTARMSecEvent = SCP_TARM_SEC_EVENT_UID;
const TUint KSCPTARMSecEventReply = SCP_TARM_SEC_EVENT_REP_UID;
const TUid KSecUIDObserver   = {SCP_SECOBS_UID};

// The interface UID for SCP interest objects
const TUid KSCPPluginInterfaceUID = { SCP_PLUGIN_IF_UID };

// SCP server UID
const TUid KSCPServerUid = { SCP_SERVER_UID };

// Autolock's SID
const TUid KSCPSIDAutolock = {0x100059B5};

// Values for TARM admin flag
const TInt KSCPFlagResyncQuery = 1; // For SecUi to show the correct query when out-of-sync
const TInt KSCPFlagAdminLock = 2; // For Autolock to lock the phone also when in a call
const TInt KSCPConfigCheckTimeout = 500000; // 0.5s

// Code query flags
const TInt KSCPEtelRequest = 1;

// Parameters for checking the configuration in CheckConfiguration call
const TInt KSCPInitial = 1;
const TInt KSCPComplete = 2;

#endif      // SCPSERVER_INTEFFACE_H   
            
// End of File

