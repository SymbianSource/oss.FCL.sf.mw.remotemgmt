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


#ifndef __POLICYENGINECLIENTSERVERDEFS_H__
#define __POLICYENGINECLIENTSERVERDEFS_H__

// INCLUDES
#include <e32std.h>


enum TPolicyEngineMessages
	{
	//Management operations
	ECreateManagementSubSession = 0,
	ECloseManagementSubSession,
	
	EExecuteOperation,
	
	EGetElementListLength,
	EReadElementList,

	EGetElementDescriptionAndChildListLength,
	EReadElementAndChildList,
		
	EGetElementXACMLLength,
	EReadElementXACML,
	
	EAddSessionTrust,
	EPerformPMRFS,
	
	EIsServerIdValid,
	ECertificateRole,
	
	//Policy request operations	
	EPolicyRequest,
	ECreateRequestSubSession,
	ECloseRequestSubSessio,
	

	EServerCertAddRequest,
	EServerCertRemoveRequest
	};
	

#endif// __POLICYENGINECLIENTSERVERDEFS_H__
