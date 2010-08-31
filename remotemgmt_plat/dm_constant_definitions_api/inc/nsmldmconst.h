/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Constants for SyncML DM
*
*/


#ifndef __NSMLDMCONST_H__
#define __NSMLDMCONST_H__

// SyncML DM software engine version, which is sent to a DM server within DmV element.
// New plug-in versions or changes in existing ones have not effect to the version.  
#include <e32base.h>
_LIT8( KNSmlDMDmV, "1.0");
_LIT8( KNSmlDMDmV12, "1.2");
// SyncML notification MIME type
_LIT( KNSmlDMNotificationMIMEType, "application/vnd.syncml.notification" );
// FOTA
const TUid KPSUidNSmlSOSServerKey = {0x101F99FB};
const TUint32 KNSmlDMSilentJob = 0x0000005;
enum TDmSessionType
{
ESilent = 1,
ENonSilent
};
const TInt KNSmlDMResetGenAlerts = -2;
const TInt KNSmlMaxInt32Length = 10;
// FOTA end

#endif //__NSMLDMCONST_H__