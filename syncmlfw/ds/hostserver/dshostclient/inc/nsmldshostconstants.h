/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Constants for DS Loader Framework
*
*/


#ifndef __NSMLDSHOSTCONSTANTS_H__ 
#define __NSMLDSHOSTCONSTANTS_H__ 

//#define __HOST_SERVER_MTEST__

//------------------------------------------------
// Constants
//------------------------------------------------
_LIT( KNSmlDSHostServer1Name, 	"dshostserver1" );
_LIT( KNSmlDSHostServer2Name, 	"dshostserver2" );

_LIT( KNSmlDSHostServer1Exe, 	"Z:nsmldshostserver1.exe" );
_LIT( KNSmlDSHostServer2Exe, 	"Z:nsmldshostserver2.exe" );

const TInt KNSmlDSHostServerVersionMajor = 1;
const TInt KNSmlDSHostServerVersionMinor = 0;
const TInt KNSmlDSHostServerVersionBuild = 0;

const TInt KNSmlDSHostChunkMinSize = 1024;
const TInt KNSmlDSHostChunkMaxSize = 786432;

// ------------------------------------------------------------------------------------------------
// TNSmlDSHostServerErrors
// ------------------------------------------------------------------------------------------------
enum TNSmlDSHostServerErrors
	{
	ENSmlDSHostServerErrNone,
	ENSmlDSHostServerErrDPNotCreated,
	ENSmlDSHostServerErrDSNotOpened,
	ENSmlDSHostServerErrAsynchCallOutstanding
	};
// ------------------------------------------------------------------------------------------------
// enum TNSmlHostServers
// ------------------------------------------------------------------------------------------------
enum TNSmlHostServers
	{
	ENSmlHostServer1,
	ENSmlHostServer2,
	//additional servers here. 
	ENSmlHostServerEnd
	};

// ------------------------------------------------------------------------------------------------
// enum TNSmlHostOperationCodes
// ------------------------------------------------------------------------------------------------
enum TNSmlHostOperationCodes
	{
	ENSmlDPOpen = 1,
	ENSmlDPOpenExcept,
	ENSmlHandleChunk,
	ENSmlHandleStringPool,
	ENSmlDPInformation,
	ENSmlDPSupportsOperation,
	ENSmlDPStoreFormat,
	ENSmlDPlListStores,
	ENSmlDPDefaultStore,
	ENSmlDSOpen,
	ENSmlDSCancelRequest,
	ENSmlServerFilters,
	ENSmlCheckSupportedServerFilters,
	ENSmlUpdateServerFilters,
	ENSmlFilters,
	ENSmlRecordQuery,
	ENSmlFieldQuery,
	ENSmlDSBeginTransaction,
	ENSmlDSCommitTransaction,
	ENSmlDSRevertTransaction,
	ENSmlDSBeginBatch,
	ENSmlDSCommitBatch,
	ENSmlDSCancelBatch,
	ENSmlDSSetDataStoreFormat,
	ENSmlDSRemoteMaxObjSize,
	ENSmlMaxObjSize,
	ENSmlItemOpen,
	ENSmlItemCreate,
	ENSmlItemReplace,
	ENSmlItemRead,
	ENSmlItemWrite,
	ENSmlItemCommit,
	ENSmlItemClose,
	ENSmlItemMove,
	ENSmlItemDelete,
	ENSmlItemSoftDelete,
	ENSmlItemDeleteAll,
	ENSmlDSSyncHistory,
	ENSmlDSItemsAdded,
	ENSmlDSItemsDeleted,
	ENSmlDSItemsSofDeleted,
	ENSmlDSItemsModified,
	ENSmlDSItemsMoved,
	ENSmlDSItemsAll,
	ENSmlDSResetChangeInfo,
	ENSmlDSCommitChanges,
	ENSmlDSCommitAllChanges,
	ENSmlDSUpdateServerId
	};

// ------------------------------------------------------------------------------------------------
// enum TNSmlHostMode
// ------------------------------------------------------------------------------------------------	
enum TNSmlHostMode
	{
	ENSmlNormalMode = 1,
	ENSmlBatchMode,
	ENSmlTransactionMode
	};

#endif // __NSMLDSHOSTCONSTANTS_H__

//End of File

