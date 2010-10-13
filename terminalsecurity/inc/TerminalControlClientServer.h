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
* Description: Implementation of terminalsecurity components
*
*/


#ifndef __TERMINALCONTROLCLIENTSERVER_H__
#define __TERMINALCONTROLCLIENTSERVER_H__

#include <e32std.h>

_LIT(KTerminalControlServerName, "terminalcontrolserver");
_LIT(KTerminalControlServerImg,  "TerminalControl");  // DLL/EXE name

const TUid KTerminalControlServerUid={0x10207825};

const TUint KTerminalControlServerMajorVersionNumber=0;
const TUint KTerminalControlServerMinorVersionNumber=1;
const TUint KTerminalControlServerBuildVersionNumber=1;


#ifndef MAX_NUMBER_OF_DIGITS_IN_10BASE_INT64
#define MAX_NUMBER_OF_DIGITS_IN_10BASE_INT64 21
#endif

const TInt KMaxLengthOfPasscodeDisallowString = 1024;
enum TTerminalControlMessages
	{
	ECreateSubSession = 0,
	ECloseSubSession,
	EFileScan,
	EFileScanResults,
	EFileScanResultsSize,
	EFileDelete,
	EDeviceLock_AutoLockPeriod_Set,
	EDeviceLock_AutoLockPeriod_Get,
	EDeviceLock_MaxAutoLockPeriod_Set,
	EDeviceLock_MaxAutoLockPeriod_Get,
	EDeviceLock_LockLevel_Set,
	EDeviceLock_LockLevel_Get,
	EDeviceLock_LockCode_Set,
	EDeviceLock_LockCode_Get,
	EDeviceWipe,
// --------- Enhanced features BEGIN ------------------
//#ifdef __SAP_DEVICE_LOCK_ENHANCEMENTS
	EDeviceLock_PasscodeMinLength_Set,
	EDeviceLock_PasscodeMinLength_Get,	
	EDeviceLock_PasscodeMaxLength_Set,
	EDeviceLock_PasscodeMaxLength_Get,	
	EDeviceLock_PasscodeRequireUpperAndLower_Set,
	EDeviceLock_PasscodeRequireUpperAndLower_Get,
	EDeviceLock_PasscodeRequireCharsAndNumbers_Set,
	EDeviceLock_PasscodeRequireCharsAndNumbers_Get,
	EDeviceLock_PasscodeMaxRepeatedCharacters_Set,
	EDeviceLock_PasscodeMaxRepeatedCharacters_Get,
	EDeviceLock_PasscodeHistoryBuffer_Set,
	EDeviceLock_PasscodeHistoryBuffer_Get,	
	EDeviceLock_PasscodeExpiration_Set,
	EDeviceLock_PasscodeExpiration_Get,	
	EDeviceLock_PasscodeMinChangeTolerance_Set,
	EDeviceLock_PasscodeMinChangeTolerance_Get,
	EDeviceLock_PasscodeMinChangeInterval_Set,
	EDeviceLock_PasscodeMinChangeInterval_Get,	
	EDeviceLock_PasscodeCheckSpecificStrings_Set,
	EDeviceLock_PasscodeCheckSpecificStrings_Get,
	EDeviceLock_PasscodeDisallowSpecific,
	EDeviceLock_PasscodeAllowSpecific,
	EDeviceLock_PasscodeClearSpecificStrings,
	EDeviceLock_PasscodeMaxAttempts_Set,
	EDeviceLock_PasscodeMaxAttempts_Get,
	EDeviceLock_PasscodeConsecutiveNumbers_Set,
	EDeviceLock_PasscodeConsecutiveNumbers_Get,
    EDeviceLock_PasscodeMinSpecialCharacters_Set,
    EDeviceLock_PasscodeMinSpecialCharacters_Get,
    EDeviceLock_PasscodeDisallowSimple_Set,
    EDeviceLock_PasscodeDisallowSimple_Get,    
//#endif
// --------- Enhanced features END --------------------
	EProcesses,
	EProcessesSize,
	EProcessData,
	EReboot,
	ELocalOperation_Start,
	ELocalOperation_Stop,
	E3rdPartySetterGet,
	E3rdPartySetterGetSize,
	E3rdPartySetterSet,
	};

#endif// __TERMINALCONTROLCLIENTSERVER_H__
