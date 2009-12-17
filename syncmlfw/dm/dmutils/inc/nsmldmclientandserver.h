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
* Description:  Common constants between DM client and DM host server
*
*/


#ifndef __NSMLDMCLIENTANDSERVER_H__ 
#define __NSMLDMCLIENTANDSERVER_H__ 

#include <e32base.h>

// ------------------------------------------------------------------------------------------------
// Constants
// ------------------------------------------------------------------------------------------------

_LIT ( KNSmlDmHostServer1Name, 		"nsmldmhostserver1");
_LIT ( KNSmlDmHostServer2Name, 		"nsmldmhostserver2" );
_LIT ( KNSmlDmHostServer3Name, 		"nsmldmhostserver3" );
_LIT ( KNSmlDmHostServer4Name, 		"nsmldmhostserver4" );
_LIT ( KNSmlDmHostServerExecPath, 	"Z:\\Sys\\Bin\\" );
_LIT ( KNSmlDmSemaphoreName, 		"DmHostServer" );

_LIT ( KNSmlDmHostServer1Exe, 		"Z:nsmldmhostserver1.exe" );
_LIT ( KNSmlDmHostServer2Exe, 		"Z:nsmldmhostserver2.exe" );
_LIT ( KNSmlDmHostServer3Exe, 		"Z:nsmldmhostserver3.exe" );
_LIT ( KNSmlDmHostServer4Exe, 		"Z:nsmldmhostserver4.exe" );

const TInt KNSmlDmHostServerVerMajor = 1;
const TInt KNSmlDmHostServerVerMinor = 0;
const TInt KNSmlDmHostServerVerBuild = 0;


const TInt KNSmlDmHostChunkMinSize = 32;
const TInt KNSmlDmHostChunkMaxSize = 76800;
const TInt KNSmlDmMaxUriLength = 256;
// ------------------------------------------------------------------------------------------------
// Enumerations
// ------------------------------------------------------------------------------------------------
enum TNSmlDmHostOperationCodes
	{
	ENSmlDMResourceCountMarkStart = 1,
	ENSmlDMResourceCountMarkEnd,
	ENSmlDMResourceCount,
	ENSmlDMCloseSession,
	ENSmlDMGetDDF,
	ENSmlDMGetDDFSize,
	ENSmlDMUpdateLeaf,
	ENSmlDMUpdateLeafLarge,
	ENSmlDMAddNode,
	ENSmlDMExecute,
	ENSmlDMExecuteLarge,
	ENSmlDMCopy,
	ENSmlDMDelete,
	ENSmlDMStartAtomic,
	ENSmlDMCommitAtomic,
	ENSmlDMRollbackAtomic,
	ENSmlDMCompleteCommands,
	ENSmlDMFetchLeaf,
	ENSmlDMFetchLink,
	ENSmlDMFetchLeafSize,
	ENSmlDMChildUriList,
	ENSmlDMChunkHandle,
	ENSmlDMGetMappings,
	ENSmlDMGetStatuses,
	ENSmlDMGetResults,
	ENSmlDMGetMore,
// FOTA	
	ENSmlDMGenericAlertsSent
// FOTA end
	};


enum TNSmlDmHostCallbackType
	{
	ENSmlDmCallbackNone = 0,
	ENSmlDmCallbackFetchLink,
	ENSmlDmCallbackGetLuid,
	ENSmlDmCallbackMappings,
	ENSmlDmCallbackStatuses,
	ENSmlDmCallbackResults,
	ENSmlDmCallbackChunkFull
	};

#endif // __NSMLDMCLIENTANDSERVER_H__
