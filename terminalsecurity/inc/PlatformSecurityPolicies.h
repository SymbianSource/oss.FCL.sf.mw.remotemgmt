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
* Description: Implementation of terminalsecurity components
*
*/


#ifndef __PLATFORM_SECURITY_POLICIES__
#define __PLATFORM_SECURITY_POLICIES__

#include <e32base.h>

const TUint policyEngineRangeCount = 3;

const TInt policyEngineRanges[policyEngineRangeCount] = 
        {
        0, // For delivery...
        100, // For request
        101  // ENotSupported
        }; 
       
const TUint8 policyEngineSecurityElementsIndex[policyEngineRangeCount] = 
        {
        0,  //applies to 1st range
        CPolicyServer::ECustomCheck,  
        CPolicyServer::ENotSupported  
        };

//capability checks
const CPolicyServer::TPolicyElement policyEngineSecurityElements[] = 
        {
        {_INIT_SECURITY_POLICY_C1(ECapabilityReadDeviceData),  CPolicyServer::EFailClient} 	 
        };

//Package all the above together into a policy
CPolicyServer::TPolicy policyEngineSecurityPolicy;
/*        0,	//iSpare --> must be 0
        helloWorldRanges,	//ranges array
        helloWorldElementsIndex,	//elements<->ranges index
        helloWorldElements,		//array of elements
        };
*/

#endif