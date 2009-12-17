/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description: definition of dm constants/exported methods
* 	This is part of remotemgmt_plat.
*
*/
 
#ifndef __Temp_CLIENTSERVER_H__
#define  __Temp_CLIENTSERVER_H__

#include <e32std.h>
#include <e32base.h>

// server name

_LIT(KTempServerName,"PMRFSServer");
_LIT(KTempServerImg,"PMRFSServer");//can be removed

const TUid KPMRFSServerUid={0x102073EA}; 

enum TTempServerMessages
{
	EPerformRFS
};

#endif