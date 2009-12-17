/*
* Copyright (c) 2002-2005 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Common definitions for dedicated clients.
*
*/

#ifndef AHLEDEDICATED_H
#define AHLEDEDICATED_H

#include <e32def.h>
#include <e32cmn.h>
#include "ahlegeneric.h"

_LIT(KAHLEItemDefaultDescription, "");

enum TAHLEClientType
{
    EAHLEGeneric,
    EAHLEBrowser,
    EAHLEPhonebook,
    EAHLEMessaging,
    EAHLELogs,
    EAHLEEmail
};

enum TAHLEState
{
    EAHLEAdaptiveSiteList,
    EAHLEAdaptiveSiteDetails,
    EAHLEAdaptiveAutoComplete
};


#endif      // AHLEDEDICATED_H

//  End of File
