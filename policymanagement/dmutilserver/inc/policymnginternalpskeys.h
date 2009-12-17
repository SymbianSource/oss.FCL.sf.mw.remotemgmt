/*
* Copyright (c) 2000 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description: Implementation of policymanagement components
*
*/


#ifndef __POLICYMNGPROPERTIES_H__
#define __POLICYMNGPROPERTIES_H__

// INCLUDES

#include <e32std.h>

// MACROS
// CONSTANTS

const TUid KPolicyMngProperty = { 0x10207843 };

enum KPolicyMngPropertyKeys 
{
	KPolicyChangedCounter = 0,
	KFactoryResetCounter
};

// DATA TYPES
// FUNCTION PROTOTYPES
// FORWARD DECLARATIONS
// CLASS DECLARATION

#endif //__POLICYMNGPROPERTIES_H__