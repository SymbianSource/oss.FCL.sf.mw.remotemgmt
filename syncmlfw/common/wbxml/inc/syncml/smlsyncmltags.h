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
* Description:  Enumerations for SyncML tags and document public IDs.
*
*/


#ifndef __SYNCMLTAGS_H__
#define __SYNCMLTAGS_H__

// ------------------------------------------------------------------------------------------------
// Includes
// ------------------------------------------------------------------------------------------------
#include <e32base.h>
#include <nsmldefines.h>

// ------------------------------------------------------------------------------------------------
// Constants
// ------------------------------------------------------------------------------------------------
const TUint8 KNSmlSyncMLVersion = 0x02;
const TUint32 KNSmlSyncMLUTF8 = 0x6a;

#ifdef __NSML_DEBUG_SYNCML10__
const TInt32 KNSmlSyncMLPublicId = 0xfd1;
_LIT8(KNSmlSyncMLPublicIdStr, "-//SYNCML//DTD SyncML 1.0//EN");
_LIT8(KNSmlMetInfPublicIdStr, "-//SYNCML//DTD MetInf 1.0//EN");
#else
const TInt32 KNSmlSyncMLPublicId = 0xfd3;
_LIT8(KNSmlSyncMLPublicIdStr, "-//SYNCML//DTD SyncML 1.1//EN");
_LIT8(KNSmlMetInfPublicIdStr, "-//SYNCML//DTD MetInf 1.1//EN");
#endif // __NSML_DEBUG_SYNCML10__

const TInt32 KNSmlSyncMLPublicId12 = 0x1201;

enum TNSmlSyncMLCodepages
	{
	ECodePageSyncML = 0,
	ECodePageMetInf = 1,
    ECodePageDevInf = 2
	};

// ------------------------------------------------------------------------------------------------
// TNSmlSyncMLTags
// ------------------------------------------------------------------------------------------------

enum TNSmlSyncMLTags
	{
	EAdd = 5,
	EAlert,
	EArchive,
	EAtomic,
	EChal,
	ECmd,
	ECmdID,
	ECmdRef,
	ECopy,
	ECred,
	EData,
	EDelete,
	EExec,
	EFinal,
	EGet,
	EItem,
	ELang,
	ELocName,
	ELocURI,
	EMap,
	EMapItem,
	EMeta,
	EMsgID,
	EMsgRef,
	ENoResp,
	ENoResults,
	EPut,
	EReplace,
	ERespURI,
	EResults,
	ESearch,
	ESequence,
	ESessionID,
	ESftDel,
	ESource,
	ESourceRef,
	EStatus,
	ESync,
	ESyncBody,
	ESyncHdr,
	ESyncML,
	ETarget,
	ETargetRef,
	EReserved_1,
	EVerDTD,
	EVerProto, 
	ENumberOfChanges,
	EMoreData,
// 1.2 CHANGES: new tags
	EField,
	EFilter,
	ERecord,
	EFilterType,
	ESourceParent,
	ETargetParent,
	EMove,	
// Changes end
	// FOTA
	ECorrelator
	//FOTA end
	};

#endif // __SYNCMLTAGS_H__