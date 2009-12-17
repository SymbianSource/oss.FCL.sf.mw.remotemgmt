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
* Description: This header contains the constants for cenrep keys.
*
*/
#ifndef __DMEVENTNOTIFIERCRKEYS_H__
#define __DMEVENTNOTIFIERCRKEYS_H__

//UID3 of this executable
#define KAppUidDmEventNotifier 0x20026F5E

//Cenrep key to store the scheduled status (enabled/disabled)
const TUint32 KDmEventNotifierEnabled = 0x00000001;

//Cenrep key to know whether SCP server should be notified upon events
const TUint32 KSCPNotifyEvent = 0x00000002;

//Cenrep key to know whether AM server should be notified upon events
const TUint32 KAMNotifyEvent = 0x00000003;


//Cenrep key to have the memory status
const TUint32 KMMCStatus = 0x00000004;

#endif //__DMEVENTNOTIFIERCRKEYS_H__
// End of File
