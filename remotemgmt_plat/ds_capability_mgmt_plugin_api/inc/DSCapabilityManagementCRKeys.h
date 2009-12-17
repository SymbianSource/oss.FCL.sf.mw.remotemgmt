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
* Description:  header file for DesktopSync plug-in adapter CR 
* 		keys for DCMO
*
*/

#ifndef DSCAPABILITYMANAGEMENTCRKEYS_H
#define DSCAPABILITYMANAGEMENTCRKEYS_H

#include <e32std.h>


const TUid KCRUidDSDCMOConfig = { 0x20022FC0 };

// CENREP key "KNsmlDesktopSync" can take only 2 values 0 and 1 
// i.e., 0 to disable and 1 to enable
const TUint32 KNsmlDesktopSync = 0x00000000;

#endif