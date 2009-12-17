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
* Description:  Test Plugin Abstract Interface
*
*/


#ifndef DIAGTESTPLUGIN_H
#define DIAGTESTPLUGIN_H

// INCLUDES
#include    <DiagPlugin.h>              // MDiagPlugin


// FORWARD DECLARATION
class CDiagPluginExecPlan;
class MDiagTestObserver;
class MDiagEngineCommon;
class CDiagPluginPool;
class TDiagTestExecParam;
class CDiagResultsDatabaseItem;
class MDiagResultDetail;

/**
* Diagnostics test plug-in abstract interface.
*
* @since S60 v5.0
**/
class MDiagTestPlugin: public MDiagPlugin
    {
public: 

    /**
    * Run mode defines what kind of behaviour is expected from this plug-in.
    **/
    enum TRunMode
        {
        EInteractiveView,       // plugin with a view
        EInteractiveDialog,     // interactive, but with dialogs
        EAutomatic              // automatic tests
        };

    /**
    * Reason for execution stop.
    **/
    enum TStopReason
        {
        ESkip,
        ECancelAll,
        EWatchdog
        };

    /**
    * Execute a diagnostics test. This is an asynchronous method. The plug-in
    * must call MDiagTestObserver:TestExecutionCompletedL when test has finished.
    *
    * @param aExecParams   Parameters for running the test.
    * @param aSkipDependencyCheck - If ETrue, plug-in should execute even if 
    *   dependencies are not executed.
    * @param aDependencyExecution - If ETrue, this is being executed to 
    *   satisfy dependency.
    * @param aCustomParams Custom parameters for plug-ins. 
    *   These should be used if TDiagTestExecParam is not enough. 
    *   It can used to pass arbitrary data from application to the plug-ins.
    *   Owership is not transferred and plug-in must not delete
    *   this parameter. Ownership is not transferred since
    *   client does not always know the type of pointer.
    **/
    virtual void RunTestL( TDiagTestExecParam* aExecParam,
                           TBool aSkipDependencyCheck,
                           TBool aDependencyExecution,
                           TAny* aCustomParams ) = 0;

    /**
    * Describes the run mode of the test plug-in.
    *
    * @return Run mode.
    * @see TDiagRunMode.
    **/
    virtual TRunMode RunMode() const = 0;

    /**
    * Cancels testing. Cancellation is expected to be synchronous.
    * Test plug-in must return the result immediately, and it must not call
    * TestExecutionCompletedL(). Ownership of the CDiagResultsDatabaseItem is 
    * transferred to the caller.
    *
    * @param aReason - Reason why ExecutionStopL() is being called.
    * @return Returns result object.  Test must not call TestExecutionCompletedL.
    **/
    virtual CDiagResultsDatabaseItem* ExecutionStopL( TStopReason aReason ) = 0;

    /**
    * Suspend test.  If test cannot be suspended, it will leave with
    * KErrNotSupported.
    **/
    virtual void SuspendL() = 0;

    /**
    * Resume test. If test cannot be resumed, it will leave with 
    * KErrNotSupported.
    **/
    virtual void ResumeL() = 0;

    /**
    * Get the number of steps that is needed to execute tests. 
    * This can be used to calculate progress information. 
    *
    * @return The number of steps.
    **/
    virtual TUint TotalSteps() const = 0;

    /**
    * Create detailed information object from test result.
    * Ownership is transferred to caller.
    *
    * @param aTestResult - Base test result.
    * @return Instance of MDiagResultDetail* that can provide detailed 
    *   information about the last test.
    **/
    virtual MDiagResultDetail* CreateDetailL( 
                        const CDiagResultsDatabaseItem& aResult ) const = 0;
    };

#endif // DIAGTESTPLUGIN_H

// End of File

