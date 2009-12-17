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


#ifndef __PLATFORM_SECURITY_POLICIES__
#define __PLATFORM_SECURITY_POLICIES__

#include <e32base.h>
#include "CentRepToolClientServer.h"


const TUint KCentRepToolRangeCount = 4;

const TInt KCentRepToolRanges[KCentRepToolRangeCount] = 
        {
        // First range
        0, 
        //Second range
        EPerformCentRepToolRFS,
        //Third range
        ECreateCheckAccessSession,
        // ENotSupported
        ECheckAccess + 1  
        }; 
       
const TUint8 KCentRepToolSecurityElementsIndex[KCentRepToolRangeCount] = 
        {
        0,  //applies to 1st range
        1,	//applies to 2nd range
        2,	//applies to 3rd range
        CPolicyServer::ENotSupported  
        };

//capability checks (Policy engine is only component which can use CentRepTool!!)
const CPolicyServer::TPolicyElement KCentRepToolSecurityElements[] = 
        {
        //Policy Engine security UID
        {_INIT_SECURITY_POLICY_S0( 0x10207815),  CPolicyServer::EPanicClient}, 	 
        {_INIT_SECURITY_POLICY_S0( 0x102073EA /*RFS SID*/),  CPolicyServer::EPanicClient}, 	 
        {_INIT_SECURITY_POLICY_C1(ECapabilityReadDeviceData),  CPolicyServer::EPanicClient} 	 
        };

//Package all the above together into a policy
CPolicyServer::TPolicy CentRepToolSecurityPolicy;

#endif