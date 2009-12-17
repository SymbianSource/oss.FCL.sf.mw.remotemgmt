/*
* Copyright (c) 2004 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Device information tag enums and document public IDs.
*
*/


#ifndef __DEVINFTAGS_H__
#define __DEVINFTAGS_H__

// ------------------------------------------------------------------------------------------------
// Includes
// ------------------------------------------------------------------------------------------------
#include <e32base.h>

// ------------------------------------------------------------------------------------------------
// Constants
// ------------------------------------------------------------------------------------------------
const TUint8 KNSmlDevInfVersion = 0x02;
const TUint32 KNSmlDevInfUTF8 = 0x6a;

#ifdef __NSML_DEBUG_SYNCML10__
const TInt32 KNSmlDevInfPublicId = 0xfd2;
_LIT8(KNSmlDevInfPublicIdStr, "-//SYNCML//DTD DevInf 1.0//EN");
#else
const TInt32 KNSmlDevInfPublicId = 0xfd4;
_LIT8(KNSmlDevInfPublicIdStr11, "-//SYNCML//DTD DevInf 1.1//EN");
#endif // __NSML_DEBUG_SYNCML10__

// 1.2 CHANGES: added public id string for version 1.2
const TInt32 KNSmlDevInfPublicId12 = 0x1203;
_LIT8(KNSmlDevInfPublicIdStr12, "-//SYNCML//DTD DevInf 1.2//EN");

// versions of Device info DTD
#define KDtDVersion12 12
#define KDtDVersion11 11

// changes end


// ------------------------------------------------------------------------------------------------
// TNSmlDevInfTags
// ------------------------------------------------------------------------------------------------

enum TNSmlDevInfTags
	{
    EDevCTCap = 5,
    EDevCTType,
    EDevDataStore,
    EDevDataType,
    EDevDevID,
    EDevDevInf,
    EDevDevTyp,
    EDevDisplayName,
    EDevDSMem,
    EDevExt,
    EDevFwV,
    EDevHwV,
    EDevMan,
    EDevMaxGUIDSize,
    EDevMaxID,
    EDevMaxMem,
    EDevMod,
    EDevOEM,
    EDevParamName,
    EDevPropName,
    EDevRx,
    EDevRxPref,
    EDevSharedMem,
 // 1.2 CHANGES: renamed from EDevSize
	EDevMaxSize, 
// changes end
    EDevSourceRef,
    EDevSwV,
    EDevSyncCap,
    EDevSyncType,
    EDevTx,
    EDevTxPref,
    EDevValEnum,
    EDevVerCT,
    EDevVerDTD,
    EDevXNam,
    EDevXVal,  
    EDevUTC,
    EDevSupportNumberOfChanges,
    EDevSupportLargeObjs,
//1.2 CHANGES: new elements 
    EDevProperty,
    EDevPropParam,
    EDevMaxOccur,
    EDevNoTruncate,
	EDevNull,  // not used in DTD 
    EDevFilterRx,
    EDevFilterCap,
    EDevFilterKeyword,
    EDevFieldLevel,
    EDevSupportHierarchicalSync
// Changes end
	};

#endif // __DEVINFTAGS_H__
