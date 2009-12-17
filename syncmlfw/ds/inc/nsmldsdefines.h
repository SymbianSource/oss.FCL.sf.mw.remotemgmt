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
* Description:  Defines for SyncML DS
*
*/


#ifndef __NSMLDSDEFINES_H__
#define __NSMLDSDEFINES_H__

// ------------------------------------------------------------------------------------------------
//
// This header contains DS related defines.
// 
// ------------------------------------------------------------------------------------------------

// ------------------------------------------------------------------------------------------------
// Includes
// ------------------------------------------------------------------------------------------------


#include "nsmldefines.h"
#include "nsmldsconstants.h"
#include "nsmldbcaps.h"

// ------------------------------------------------------------------------------------------------
// Constacts
// ------------------------------------------------------------------------------------------------

// These versions are used in DS adapter Version() function. Defines version of SyncML DS.
const TInt KNSmlDSVerMajor = 1;
const TInt KNSmlDSVerMinor = 2;

struct TNSmlContentSpecificSyncType
	{
	TInt iTaskId;
	CNSmlDbCaps::ENSmlSyncTypes iType;
	};
	
#endif // __NSMLDSDEFINES_H__
