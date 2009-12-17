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
*     SyncML Data Synchronization internal P&S key definitions
*
*/


#ifndef SYNCML_DATASYNC_INTERNAL_PS_KEYS_H
#define SYNCML_DATASYNC_INTERNAL_PS_KEYS_H

/**
* SyncML Data Synchronization Internal P&S Keys UID
*/
const TUid KPSUidDataSynchronizationInternalKeys = {0x101F9A0B};

/**
* P&S key for Data Synchronization status.
*
* Possible integer values:
* 0 = Data sync is not running (EDataSyncNotRunning)
* 1 = Data sync is running, using OMA DS protocol version 1.1 (EDataSyncRunning)
* 2 = Data sync is running, using OMA DS protocol version 1.2 (EDataSyncRunning12)
*
* Default value: 0 (EDataSyncNotRunning)
*
* Note: Key is created when Data Sync is run first time. Before that it does not exist.
* If you want just to see whether Data sync is running and you do not care about the
* OMA DS protocol version, just compare if key value is greater than zero (>0) 
*
*/
const TUint32 KDataSyncStatus = 0x00000001;

// Enumeration for Data Sync status
enum TDataSyncStatus
	{
	EDataSyncNotRunning,
	EDataSyncRunning,
	EDataSyncRunning12
	};

#endif // SYNCML_DATASYNC_INTERNAL_PS_KEYS_H

