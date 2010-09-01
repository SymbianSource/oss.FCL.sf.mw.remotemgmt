/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Observer interface to use with CDiagEngine
*
*/


#ifndef DIAGENGINEOBSERVER_H
#define DIAGENGINEOBSERVER_H


// SYSTEM INCLUDES
#include <e32def.h>                 // TUint
#include <DiagCommonDialog.h>       // TDiagCommonDialog
#include <DiagAppCommand.h>         // TDiagAppCommand

// FORWARD DECLARATION
class CDiagResultsDatabaseItem;
class CAknDialog;

/**
*  Diagnostics Framework Engine Observer
*
*  This interface is called by CDiagEngine to notify its client
*  of progress of requests made to engine.
*
* @since S60 v5.0
*/
class MDiagEngineObserver
    {
public: // Public Data types
    enum TSuspendReason
        {
        ESuspendByClient = 0,
        ESuspendByPhoneCall
        };

    enum TResumeReason
        {
        EResumedByClient = 0,
        EAutoResumedByCallHangup
        };

public: // Public interface

    /**
    * Notify client of test execution begin. Clients can get testplan using
    * CDiagEngine::ExecutionPlanL. 
    */
    virtual void TestExecutionBeginL() = 0;

    /**
    * Notify client of test execution progress.
    *
    * @param aCurrentItemStep       - Progress in current entry.
    * @param aCurrentItemTotalSteps - Total steps to execute in current entry.
    */
    virtual void TestExecutionProgressL( TUint aCurrentItemStep,
                                         TUint aCurrentItemTotalSteps ) = 0;

    /**
    * Notify client of execution completion of one plug-in.
    *   This can be either test plug-in or suite plug-in.
    *
    * @param aError  - KErrNone - Success.
    *   KErrNoMemory - Out of memory.
    *   KErrCancel   - Cancelled due to ExecutionStopL()
    *
    * @param aResult - Result of the test. Ownership is transferred here. 
    *  Client must deallocate aResult to avoid memory leak. aResult is Null 
    *  if the plug-in being executed is a suite pre/post method.
    */
    virtual void TestExecutionPluginExecutedL( TInt aError,
                                               CDiagResultsDatabaseItem* aResult ) = 0;

    /**
    * Notify client of execution suspend.
    *
    * @param aSuspendReason - Why execution is suspended.
    */
    virtual void TestExecutionSuspendedL( TSuspendReason aSuspendReason ) = 0;

    /**
    * Notify client of execution resume.
    *
    * @param aResumeReason - Why execution is being resumed
    */
    virtual void TestExecutionResumedL( TResumeReason aResumeReason ) = 0;

    /**
    * Notify client of engine stopping.
    *
    * This indicates that engine execution has stopped. There will not be any
    * further messages from engine. This could be called because all
    * tests are completed, or an unrecoverable error occured during execution.
    *
    * Note that if cancel is called during plan creation or if plan creation
    * fails, TestExecutionStoppedL() may be called withing first calling
    * TestExecutionBeginL(). 
    *
    * @param aError - Reason for engine stopping. 
    *   a) KErrNone - All tests are successfully completed.
    *   b) KErrCancel - ExecutionStopL is called with ECancelAll. 
    *       Test session cannot be resumed later.
    *   c) KErrArgument - Parameters passed to engine are invalid.
    *   d) Others - Other critical that could not be recovered occured during
    *       test execution.  Test may be resumed later in this case.
    */
    virtual void TestExecutionCompletedL( TInt aError ) = 0;

    /**
    * Create a common Dialog.
    * Application should create an instance of requested dialog.
    * For available dialog types, @see DiagCommonDialog.h
    * 
    * @param aDialogType - Type of dialog to create.
    * @param aData - Initialization parameter. Ownership is passed to engine.
    * @return a pointer to newly created dialog. Ownership is passed to caller.
    */
    virtual CAknDialog* CreateCommonDialogLC( TDiagCommonDialog aDialogType,
                                              TAny* aInitData ) = 0;

    /**
    * Execute a command from plug-in. Parameters are identical to 
    * MDiagEngineCommon::ExecuteAppCommandL(). 
    * @see MDiagEngineCommon::ExecuteAppCommandL()
    * @see TDiagAppCommand
    */
    virtual void ExecuteAppCommandL( TDiagAppCommand aCommand, 
                                     TAny* aParam1,
                                     TAny* aParam2 ) = 0;
    };


#endif // DIAGENGINEOBSERVER_H

// End of File

