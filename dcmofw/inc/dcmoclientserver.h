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
* Description:  header file for DCMO ClientServer 
*
*/

#ifndef __DCMOCLIENTSERVER_H__
#define __DCMOCLIENTSERVER_H__

#include <e32std.h>

#define KStarterUid		0x2000D75B

_LIT(KDCMOServerName, "dcmoserver");
const TUid KDCMOServerUid={0x200100CA};

const TUint KDCMOServerMajorVersionNumber=1;
const TUint KDCMOServerMinorVersionNumber=0;
const TUint KDCMOServerBuildVersionNumber=0;
const TUint KDCMOServerMessageSlotNumber=2;

enum TDCMOOperation
	{
	EDcmoGetAttrInt,   // any client can Get	
	EDcmoGetAttrStr,
	EDcmoSetAttrInt,   // Set only for DiskAdmin
	EDcmoSetAttrStr,
	EDcmoSearchAdapter
	};

#endif// __DCMOCLIENTSERVER_H__
