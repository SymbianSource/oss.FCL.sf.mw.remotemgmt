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
* Description: definition of dm constants/exported methods
* 	This is part of remotemgmt_plat.
*
*/


#ifndef __POLICYENGINECLIENTSERVER_H__
#define __POLICYENGINECLIENTSERVER_H__

// INCLUDES
#include <e32std.h>


// CONSTANTS
_LIT(KPolicyEngineServerName,"policyengineserver");
_LIT(KPolicyEngineServerImg,"policyengineserver");		// DLL/EXE name

const TUid KPolicyEngineServerUid={0x10207814};

const TInt KMaxReturnMessageLength = 150;
const TInt KElementIdMaxLength = 100;
const TInt KMessageDelimiterChar = '|';


// DATA TYPES

typedef TBuf8<KElementIdMaxLength> TElementId;

enum TElementType
{
	EPolicySets,
	EPolicies,
	ERules
};

// FUNCTION PROTOTYPES
// CLASS DECLARATION

class TElementInfoHelpPack
{
	public:
		TInt iDescriptionLength;
		TInt iChildListLength;
};

	
enum TRole
{
	ECorporateRole,
	ENoRole
};	

#endif// __POLICYENGINECLIENTSERVER_H__
