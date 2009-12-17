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
* Description:  Class declaration for CDiagExecPlanEntryImplTest
*
*/


#ifndef DIAGEXECPLANENTRYIMPLTEST_H
#define DIAGEXECPLANENTRYIMPLTEST_H

// SYSTEM INCLUDE FILES
#include <DiagResultsDatabaseItem.h>    // CDiagResultsDatabaseItem::TResult
#include <DiagTestObserver.h>           // MDiagTestObserver
#include <DiagTestPlugin.h>             // MDiagTestPlugin::TStopReason

// USER INCLUDE FILES
#include "diagexecplanentryimpl.h"      // CDiagExecPlanEntryImpl

// FORWARD DECLARATION
class MDiagTestPlugin;

/**
* Diagnostics Plug-in Execution Plan Entry Implementation for test plugin
*
* This class implements CDiagExecPlanEntryImplTest. It is meant to be private
*   to Diagnostics Framework and not exported.
*
* @since S60 v5.0
*
*/
NONSHARABLE_CLASS( CDiagExecPlanEntryImplTest ): public CDiagExecPlanEntryImpl,
                                                 public MDiagTestObserver
    {
public: // Public constructors and destructors
    /**
    * Symbian two-phased constructor
    *
    * @param aEngine - Diagnostics Engine
    * @param aEngineConig - Engine configuration.
    * @param aObserver - Execution plan entry observer.
    * @param aPlugin - Test plug-in that this plan entry represents.
    * @param aAsDependency - Whether this item is being executed as dependency or not.
    * @param aResult - Initial result. 
    *
    * @return a new instance of CDiagExecPlanEntryImplTest
    */
    static CDiagExecPlanEntryImplTest* NewL( MDiagEngineCommon& aEngine,
                                             const TDiagEngineConfig& aEngineConfig,
                                             MDiagExecPlanEntryImplObserver& aObserver,
                                             MDiagTestPlugin& aPlugin,
                                             TBool aAsDependency,
                                             CDiagResultsDatabaseItem::TResult aResult );

    /**
    * Symbian two-phased constructor.
    * This will put an entry in cleanup stack.
    *
    * @param aEngine - Diagnostics Engine
    * @param aEngineConig - Engine configuration.
    * @param aObserver - Execution plan entry observer.
    * @param aPlugin - Test plug-in that this plan entry represents.
    * @param aAsDependency - Whether this item is being executed as dependency or not.
    * @param aResult - Initial result. 
    *
    * @return a new instance of CDiagExecPlanEntryImplTest
    */
    static CDiagExecPlanEntryImplTest* NewLC( MDiagEngineCommon& aEngine,
                                              const TDiagEngineConfig& aEngineConfig,
                                              MDiagExecPlanEntryImplObserver& aObserver,
                                              MDiagTestPlugin& aPlugin,
                                              TBool aAsDependency,
                                              CDiagResultsDatabaseItem::TResult aResult );

    /**
    * C++ destructor
    */
    virtual ~CDiagExecPlanEntryImplTest();

public:     // new public api
    /**
    * Get test result
    *   @return Test result enum.
    */
    CDiagResultsDatabaseItem::TResult Result() const;

    /**
    * Set test result
    *   @param aResult - Test result.
    */
    void SetResult( CDiagResultsDatabaseItem::TResult aResult );

    /**
    * Get result database item. If cached value is available, it will use
    * cached value. Otherwise, it will retrieve from database.
    *   @return Test result item from database. Ownership is transferred.
    */
    CDiagResultsDatabaseItem* GetLastTestResultL();

    /**
    * Get test plugin associated.
    * @return a reference to test plug-in that this entry represents.
    */
    MDiagTestPlugin& TestPlugin();

private:    // from CActive
    /*
    * @see CActive::RunL
    */
    virtual void RunL();

    /*
    * @see CActive::DoCancel
    */
    virtual void DoCancel();

    /*
    * @see CActive::RunError
    */
    virtual TInt RunError( TInt aError );

private:    // from CDiagExecPlanEntryImpl
    /**
    * @see CDiagExecPlanEntryImpl::ExecuteL
    */
    virtual void ExecuteL();

    /**
    * @see CDiagExecPlanEntryImpl::StopExecutionByWatchdogL
    */
    virtual void StopExecutionByWatchdogL();

    /**
    * @see CDiagExecPlanEntryImpl::DoStopExecutionByClientL
    */
    virtual void DoStopExecutionByClientL( MDiagEngineCommon::TCancelMode aCancelMode );

    /**
    * @see CDiagExecPlanEntryImpl::SuspendL
    */
    virtual void SuspendL();

    /**
    * @see CDiagExecPlanEntryImpl::ResumeL
    */
    virtual void ResumeL();

private:    // from MDiagTestObserver
    /**
    * Notify engine of test plug-in execution progress.
    * @see MDiagTestObserver::TestProgressL
    */
    virtual void TestProgressL( const MDiagTestPlugin& aSender, 
                                TUint aCurrentStep );

    /**
    * Notify engine of test plug-in execution completion.
    * @see MDiagTestObserver::TestExecutionCompletedL
    */
    virtual void TestExecutionCompletedL( const MDiagTestPlugin& aSender, 
                                          CDiagResultsDatabaseItem* aTestResult );
    
    
private:    // Private methods
    /**
    * c++ constructor.
    *
    * @param aEngine - Diagnostics Engine
    * @param aEngineConig - Engine configuration.
    * @param aObserver - Execution plan entry observer.
    * @param aPlugin - Test plug-in that this plan entry represents.
    * @param aAsDependency - Whether this item is being executed as dependency or not.
    * @param aResult - Initial result. 
    */
    CDiagExecPlanEntryImplTest( MDiagEngineCommon& aEngine,
                                const TDiagEngineConfig& aEngineConfig,
                                MDiagExecPlanEntryImplObserver& aObserver,
                                MDiagTestPlugin& aPlugin,
                                TBool aAsDependency,
                                CDiagResultsDatabaseItem::TResult aResult );
    
    /**
    * Starts executing test.
    */
    void DoExecuteTestPluginL();

    /**
    * Stop test plugin
    */
    CDiagResultsDatabaseItem* StopTestPluginL( MDiagTestPlugin::TStopReason aReason );

    /**
    * Handler function for initial delay timer expiration.
    */
    static TInt HandleDelayTimerExpiredL( TAny* aData );

    /**
    * Stop initial delay timer.
    */
    void StopInitDelayTimer();

    /**
    * Stop all active requests and timers.
    */
    void StopAll();

    /**
    * Stops test execution, but test result is not reported.
    */
    void StopExecutionAndIgnoreResultL();

    /**
    * Get human readable name for test result. This is meant for debugging
    * purpose only. In non-debug build, it will return empty string.
    */
    static const TDesC& TestResultString( CDiagResultsDatabaseItem::TResult aResult );


private:    // private data
    /**
    * Test result
    */
    CDiagResultsDatabaseItem::TResult iResult;

    /**
    * Plugin test initial delay timer.
    * Ownership - this
    */
    CPeriodic* iInitDelayTimer;

    /**
    * Cached test result. This stores test result temporarily, so that database access
    * could be avoided. Currently, cache will remain only for one request after
    * test is completed.
    * Ownership - this
    */
    CDiagResultsDatabaseItem* iCachedResult;

    };

#endif // DIAGEXECPLANENTRYIMPLTEST_H

// End of File

