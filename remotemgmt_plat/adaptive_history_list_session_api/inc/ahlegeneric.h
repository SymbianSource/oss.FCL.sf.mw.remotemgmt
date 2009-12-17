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
* Description:  Common definitions for generic clients.
*
*/

#ifndef AHLEGENERIC_H
#define AHLEGENERIC_H

_LIT(KAHLEItemDefaultName, "");
_LIT(KAHLENoMatch, "");

const TUint KAHLEMaxItemLength = 1024;
const TUint KAHLEMaxDescriptionLength = 256;

const static TReal32 KAHLEDefaultWeight = 1;

enum TAHLESortOrder
    {
    EAHLEAscending,
    EAHLEDescending
    };

#endif      // AHLEGENERIC_H

//  End of File
