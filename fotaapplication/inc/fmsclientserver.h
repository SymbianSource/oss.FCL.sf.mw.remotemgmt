/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description: Implementation of fotaserver component
* 	This is part of fotaapplication.
*
*/

// FMSClientServer.h
//
//
#ifndef __FMS_CLIENTSERVER_H__
#define __FMS_CLIENTSERVER_H__

#include <e32std.h>
#include <e32base.h>
#include <f32file.h>
// server name

_LIT(KFMSServerName,"FMSServer");

//const TUid KFMSServerUid={0x200100C8}; //To be moved to fotaconst.h

enum TFmsIpcCommands
{
    ENoInterrupt = 0, // Not to be used for IPC.
    //For starting of the FMS monitoring during phone startup
    EFotaStartUpPlugin = 1, //as continuation to upper enums
    //For all download related interrupts
    EDLUserInterrupt = 10,
	EDLNetworkInterrupt,
	EDLMemoryInterrupt,
	EDLGeneralInterrupt,
	
	//For all update related interrupts
	EUpdMonitorbattery = 20, 
    EUpdPhoneCallActive,
	EUpdMonitorPhoneCallEnd,
	
	//For canceling any outstanding request
	ECancelOutstandingRequest = 30,
};
#endif
