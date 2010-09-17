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

// INCLUDES

#include <e32base.h>

#include <PolicyEngineClientServer.h>
#include "PolicyEngineClientServerDefs.h"
// CONSTANTS

const TUint KPolicyEngineRangeCount = 7;

const TInt KPolicyEngineRanges[KPolicyEngineRangeCount] = 
        {
        ECreateManagementSubSession,

//		First range containes following function ids
//		ECreateManagementSubSession
//		ECloseManagementSubSession
//		EExecuteOperation


		EGetElementListLength,
//		Second range function ids (Only for DM)
//		EGetElementListLength
//		EReadElementList
//		EGetElementDescriptionAndChildListLength
//		EReadElementAndChildList
//		EGetElementXACMLLength
//		EReadElementXACML
//		EAddSessionTrust,

		EPerformPMRFS,
//		third range, only RFS

		EIsServerIdValid,
//		4th range, for DM components (Get functions)...
//		ECertificateRole

	
		EPolicyRequest,
//		5th range function ids
//		EPolicyRequest
//		ECreateRequestSubSession
//		ECloseRequestSubSessio
	
		EServerCertAddRequest,
//      6th range function ids
//      EServerCertRemoveRequest,
		
		EServerCertRemoveRequest+1
        //ENotSupported
        }; 
        
        
       
const TUint8 KPolicyEngineSecurityElementsIndex[ KPolicyEngineRangeCount] = 
        {
        CPolicyServer::ECustomCheck,  		//applies to 1st range 
        0,  								//applies to 2nd range
        1,									//applies to 3rd range
        2,									//applies to 4rd range
        3,									//applies to 5rd range
        4,                                  //applies to 6rd range
        CPolicyServer::ENotSupported  
        };

//capability checks
const CPolicyServer::TPolicyElement KPolicyEngineSecurityElements[] = 
        {
        {_INIT_SECURITY_POLICY_S2( 0x101f9a02, ECapabilityWriteDeviceData, ECapabilityReadDeviceData),  CPolicyServer::EPanicClient}, 	 
        {_INIT_SECURITY_POLICY_S0( 0x102073EA /*RFS SID*/),  CPolicyServer::EPanicClient}, 	 
        {_INIT_SECURITY_POLICY_C2(ECapabilityReadDeviceData, ECapabilityWriteDeviceData),  CPolicyServer::EPanicClient}, 	 
        {_INIT_SECURITY_POLICY_C2(ECapabilityReadDeviceData, ECapabilityWriteDeviceData),  CPolicyServer::EPanicClient} ,
        {_INIT_SECURITY_POLICY_C1( ECapability_None ), CPolicyServer::EFailClient}
        };

//Package all the above together into a policy
const CPolicyServer::TPolicy KPolicyEngineSecurityPolicy =
{
	CPolicyServer::ECustomCheck,
	KPolicyEngineRangeCount,
	KPolicyEngineRanges,
	KPolicyEngineSecurityElementsIndex,
	KPolicyEngineSecurityElements
};

// MACROS
// DATA TYPES
// FUNCTION PROTOTYPES
// FORWARD DECLARATIONS
// CLASS DECLARATION

#endif