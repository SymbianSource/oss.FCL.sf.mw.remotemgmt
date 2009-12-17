/*
* Copyright (c) 2007-2007 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Contains common datatypes.
*
*/


#ifndef DIAG_RESULTS_DATABASE_COMMON_H
#define DIAG_RESULTS_DATABASE_COMMON_H

#include <e32std.h>             // TTime
#include <flogger.h>

// CONSTANTS
_LIT( KDiagLogFolder,  "DiagFramework" );
_LIT( KDiagLogFile1,    "LOG1.TXT" );

#define LOGME( aFormat )                                \
    {                                                       \
    _LIT( KFormat, aFormat );                               \
    RFileLogger::Write( KDiagLogFolder(),          \
                        KDiagLogFile1(),            \
                        EFileLoggingModeAppend,             \
                        KFormat() );                        \
    }
    
#define LOGME1( aFormat, a )                            \
    {                                                       \
    _LIT( KFormat, aFormat );                               \
    RFileLogger::WriteFormat( KDiagLogFolder(),    \
                              KDiagLogFile1(),      \
                              EFileLoggingModeAppend,       \
                              TRefByValue<const TDesC>( KFormat() ), \
                              a );                          \
    }
    
// FORWARD DECLARATIONS

// CONSTANTS
const TUint KDiagResultsDatabaseServerMajor(1);
const TUint KDiagResultsDatabaseServerMinor(1);
const TUint KDiagResultsDatabaseServerBuild(0);

_LIT(KDiagResultsDatabaseServerName,"diagresultsdatabaseserver");
_LIT(KDiagResultsDatabaseServerExe,"diagresultsdatabaseserver.exe");

/**
* Namespace for results database common datatypes.
*
* @since S60 v5.0
*/
namespace DiagResultsDbCommon
    {

    // Server start function
    TInt StartServer();

    // ENUMERATIONS
    enum TServerOpCodes
        {
        EConnect,
        EClose,
        EGetRecordCount,
        EConnectSubsession,
        ECloseSubsession,
        EGetLastRecord,
        EGetLastNotCompletedRecord,
        EGetRecordList,
        EGetRecordInfoList,
        EInitiateGetLastResults,
        ECancelInitiateGetLastResults,
        EGetLastResults,
        EInitiateGetSingleLastResult,
        EGetSingleLastResult,
        
        ESubsessionGetTestRecordId,
        ESubsessionTestCompleted,
        ESubsessionSuspend,
        ESubsessionIsTestCompleted,
        ESubsessionIsSuspended,
        ESubsessionCreateNewRecord,
        ESubsessionGetRecordInfo,
        ESubsessionGetTestUids,
        ESubsessionGetEngineParam,
        ESubsessionLogTestResult,
        ESubsessionCancelLogTestResult,
        ESubsessionGetStatus,
        ESubsessionGetTestResult,
        ESubsessionGetTestResults,
        ENotSupported
        };
        
    // reasons for server panic
    enum TPanics
	    {
	    EBadRequest,
	    EBadDescriptor,
	    EMainSchedulerError,
	    ESvrCreateServer,
	    ESvrStartServer,
	    ESvrCreateSubsession,
	    EServerCorruptFileDelete,
	    EServerFileCreationError,
	    EServerStoreRevertError,
	    ECreateTrapCleanup,
	    EDatabaseCompact,
	    EDatabaseCompactCommit,
	    EGetLastResultsMismatch,
	    ECentralRepositoryFailure,
	    EDoCompleteTestResultL,
	    EDoCompleteTestRecordL,
	    EStoreTestResultRevertLError,
	    EStoreRecordCompleteRevertLError,
	    EStoreNullPointer,
	    EGetStatusPanic,
	    EUnknownLastResultState,
	    EIncorrectExtendNumberForLastResults,
	    EIncorrectStreamId,
	    EUnableToDelete,
	    EMaximumFileSizeExceeded,
	    EUnknownDeletionAlgorithm,
	    ENotImplementedYet,
	    };

    }
	
#endif //DIAG_RESULTS_DATABASE_COMMON_H

