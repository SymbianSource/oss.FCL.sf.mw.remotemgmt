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
* Description: Engine server constants definition. 
*
*/


#ifndef __NSMLSOSSERVERDEFS_H__
#define __NSMLSOSSERVERDEFS_H__

_LIT( KServerNameAndDir, "Z:\\sys\\bin\\nsmlsosserver.exe");
_LIT( KServerDir, "Z:\\sys\\bin\\");
_LIT( KServerExeName, "nsmlsosserver.exe");

_LIT(KSOSServerName,"SymbianOSServer");
_LIT(KSOSServerThread,"SOSServer");

_LIT(KNSmlLaunch,"NSmlLaunch");
_LIT(KSmlDMAgentLoaderLibName, "nsmldmagent.dll");
_LIT(KSmlDSAgentLoaderLibName, "nsmldsagent.dll");

_LIT( KNSmlSOSServerMutexName, "NSmlSOSServerMutex" );

const TInt KNSmlSOSServerVersionMinor = 0;
const TInt KNSmlSOSServerVersionMajor = 1;

const TInt KDefaultNSmlBufferGranularity = 500;
const TInt KSizeofTInt32 = sizeof(TInt32);
const TInt KSizeofTInt8 = sizeof(TInt8);
const TInt KMaxIAPCount = 4;
const TInt KBufferGranularity = 500;
const TInt KMaxArrayCount = 10;
const TInt KMaxJobDataSize = 64;

const TInt KNotifierTimeout = 300000000; // 5 min timeout

const TUint KDefaultHeapSize=0x400000;
const TUint KNSmlSOSServerStackSize=0x3000;

const TUid KUidNotifier = { 0x101F8769 };
const TUid KUidDMServer = { 0x101F9A02 }; // nsmldmhostserver1
const TUid KUidWAPPush  = { 0x00000001 }; // NOT KNOWN YET
const TUid KUidSRCS     = { 0x101F7C87 };
const TUid KUidDmFotaServer = { 0x102072C4  }; // fotaserver

enum TNSmlRequestType
    {
    ENSmlTypeEvent,
    ENSmlTypeProgressEvent
    };

enum TNSmlProgressEventType
    {
    ENSmlSyncError,
    ENSmlSyncProgress,
    ENSmlModifications
    };

enum TNSmlCommands
    {
    ECmdEventRequest,
    ECmdEventRequestCancel,
    ECmdProgressRequest,
    ECmdProgressRequestCancel,
    ECmdJobListCurrent,
    ECmdJobListStart,
    ECmdJobListNext,
    ECmdProfileListStart,
    ECmdProfileListNext,
    ECmdProfileDelete,
    ECmdProfileCreateDS, 
    ECmdProfileCreateDM, 
    ECmdProfileOpenDS,   
    ECmdProfileOpenDM,   
    ECmdProfileGet,
    ECmdProfileSet,
    ECmdProfileClose,
    ECmdTaskListStart,
    ECmdTaskListNext,
    ECmdTaskDelete,
    ECmdConnList,
    ECmdConnCreate, 
    ECmdConnOpen, 
    ECmdConnGet,
    ECmdConnSet,
    ECmdConnClose, 
    ECmdCreateTask,
    ECmdTaskOpen, 
    ECmdTaskGet,
    ECmdTaskFilterBufferSize,
    ECmdTaskSupportedFilters,
    ECmdTaskSet,
    ECmdTaskClose, 
    ECmdDataSyncJobCreateForProfile, 
    ECmdDataSyncJobCreateForProfileST, 
    ECmdDataSyncJobCreateForTasks, 
    ECmdDataSyncJobCreateForTasksST, 
    ECmdDataSyncJobCreateForTransport, 
    ECmdDataSyncJobCreateForTransportST, 
    ECmdDataSyncJobCreateForTransportAndTasks,
    ECmdDataSyncJobCreateForTransportAndTasksST, 
    ECmdDevManJobCreate, 
    ECmdDevManJobCreateForTransport, 
    ECmdJobOpen,  
    ECmdJobGet,
    ECmdJobStop, 
    ECmdJobClose, 
    ECmdHistoryLogOpen, 
    ECmdHistoryLogGet,
    ECmdHistoryLogReset,
    ECmdHistoryLogClose, 
    ECmdDataProviderOpen, 
    ECmdDataProviderGet,
    ECmdDataProviderClose, 
    ECmdServerAlert,
    ECmdInitDMAuthInfo,
    ECmdGetDMAuthInfo,
    ECmdSetDMAuthInfo,
    // FOTA
    ECmdAddDMGenericAlert,
    // FOTA end
    // SCOMO
    ECmdAddGenericAlert,
    // SCOMO End
	ECmdContactSuiteProgressRequest,
	ECmdContactSuiteProgressRequestCancel,
	ECmdContactSuiteRegisterObserversWithProfileId
    };

enum TNSmlJobType
    {
    EDSJobProfile,
    EDSJobProfileST,
    EDSJobTasks,
    EDSJobTasksST,
    EDSJobTransport,
    EDSJobTransportST,
    EDSJobTaskTrans,
    EDSJobTaskTransST,
    EDMJobProfile,
    EDMJobTransport
    };

#endif