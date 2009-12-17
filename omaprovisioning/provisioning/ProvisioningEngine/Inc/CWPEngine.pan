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
* Description:  Engine panics.
*
*/


#ifndef CWPENGINE_PAN
#define CWPENGINE_PAN

// DATA TYPES
enum TWPPanic
    {
    EWPIndexOverflow,
    EWPNullElement,
    EWPIllegalLink,
    EWPNullMessage,
    EWPIllegalCharacteristic,
    EWPIllegalParameter,
    EWPIllegalElement
    };

// FUNCTION PROTOTYPES

// ----------------------------------------------------
// Panic handler
// ----------------------------------------------------
//
GLREF_C void Panic(TWPPanic aPanic);

#endif /* CWPENGINE_PAN */
