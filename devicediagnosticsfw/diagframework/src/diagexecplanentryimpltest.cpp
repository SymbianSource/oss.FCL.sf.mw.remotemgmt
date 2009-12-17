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
* Description:  Class definition of CDiagExecPlanEntryImplTest
*
*/


// CLASS DECLARATION
#include "diagexecplanentryimpltest.h"

// SYSTEM INCLUDE FILES
#include <DiagPlugin.h>                     // MDiagPlugin
#include <DiagTestPlugin.h>                 // MDiagTestPlugin
#include <DiagFrameworkDebug.h>             // LOGSTRING
#include <DiagResultsDbItemBuilder.h>       // CDiagResultsDbItemBuilder
#include <DiagTestExecParam.h>              // TDiagTestExecParam
#include <DiagResultsDatabase.h>            // RDiagResultsDatabaseRecord

// USER INCLUDE FILES
#include "diagframework.pan"                // Panic Codes
#include "diagexecplanentryimplobserver.h"  // MDiagExecPlanEntryImplObserver
#include "diagengineconfig.h"               // TDiagEngineConfig


// DATA

// MACROS

// LOCAL DATA TYPES


// ======== LOCAL FUNCTIONS ========

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// CDiagExecPlanEntryImplTest::NewL
// ---------------------------------------------------------------------------
//
CDiagExecPlanEntryImplTest* CDiagExecPlanEntryImplTest::NewL(
        MDiagEngineCommon& aEngine,
        const TDiagEngineConfig& aEngineConfig,
        MDiagExecPlanEntryImplObserver& aObserver,
        MDiagTestPlugin& aPlugin,
        TBool aAsDependency,
        CDiagResultsDatabaseItem::TResult aResult )
    {
    CDiagExecPlanEntryImplTest* self = NewLC( aEngine,
                                              aEngineConfig,
                                              aObserver,
                                              aPlugin,
                                              aAsDependency,
                                              aResult );
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// CDiagExecPlanEntryImplTest::NewLC
// ---------------------------------------------------------------------------
//
CDiagExecPlanEntryImplTest* CDiagExecPlanEntryImplTest::NewLC(
        MDiagEngineCommon& aEngine,
        const TDiagEngineConfig& aEngineConfig,
        MDiagExecPlanEntryImplObserver& aObserver,
        MDiagTestPlugin& aPlugin,
        TBool aAsDependency,
        CDiagResultsDatabaseItem::TResult aResult )
    {
    CDiagExecPlanEntryImplTest* self = 
        new ( ELeave ) CDiagExecPlanEntryImplTest ( aEngine,
                                                    aEngineConfig,
                                                    aObserver,
                                                    aPlugin, 
                                                    aAsDependency,
                                                    aResult );
    CleanupStack::PushL( self );
    return self;
    }


// ---------------------------------------------------------------------------
// CDiagExecPlanEntryImplTest::CDiagExecPlanEntryImplTest
// ---------------------------------------------------------------------------
//
CDiagExecPlanEntryImplTest::CDiagExecPlanEntryImplTest( 
        MDiagEngineCommon& aEngine,
        const TDiagEngineConfig& aEngineConfig,
        MDiagExecPlanEntryImplObserver& aObserver,
        MDiagTestPlugin& aPlugin,
        TBool aAsDependency,
        CDiagResultsDatabaseItem::TResult aResult )
    :   CDiagExecPlanEntryImpl( 
            aEngine,
            aEngineConfig,
            aObserver,
            aPlugin,
            aAsDependency,
            ETypeTestExec ),
        iResult( aResult )
    {
    }

// ---------------------------------------------------------------------------
// CDiagExecPlanEntryImplTest::~CDiagExecPlanEntryImplTest
// ---------------------------------------------------------------------------
//
CDiagExecPlanEntryImplTest::~CDiagExecPlanEntryImplTest()
    {
    StopAll();  // deletes timer as well.
    if ( CDiagExecPlanEntryImpl::State() != EStateCompleted )
        {
        // TRAP is needed since this is being called in destructor.
        TRAP_IGNORE( StopExecutionAndIgnoreResultL() )
        }
    delete iCachedResult;
    iCachedResult = NULL;
    }

// ---------------------------------------------------------------------------
// CDiagExecPlanEntryImplTest::Result
// ---------------------------------------------------------------------------
//
CDiagResultsDatabaseItem::TResult CDiagExecPlanEntryImplTest::Result() const
    {
    return iResult;
    }

// ---------------------------------------------------------------------------
// CDiagExecPlanEntryImplTest::SetResult
// ---------------------------------------------------------------------------
//
void CDiagExecPlanEntryImplTest::SetResult( CDiagResultsDatabaseItem::TResult aResult )
    {
    iResult = aResult;
    }

// ---------------------------------------------------------------------------
// CDiagExecPlanEntryImplTest::TestPlugin
// ---------------------------------------------------------------------------
//
MDiagTestPlugin& CDiagExecPlanEntryImplTest::TestPlugin()
    {
    // It is safe to typecast here, since CDiagExecPlanEntryImplTest::NewL
    // accepts only MDiagTestPlugin&.
    return static_cast< MDiagTestPlugin& >( Plugin() );
    }

// ---------------------------------------------------------------------------
// From CDiagExecPlanEntryImpl
// CDiagExecPlanEntryImplTest::ExecuteL
// ---------------------------------------------------------------------------
//
void CDiagExecPlanEntryImplTest::ExecuteL()
    {
    #ifdef _DEBUG
    HBufC* pluginName = Plugin().GetPluginNameL( 
        MDiagPlugin::ENameLayoutListSingle );
    
    LOGSTRING3( "---- TEST BEGIN ---- [ 0x%08x  %S ] ---- TEST BEGIN ---- {",
        Plugin().Uid().iUid,
        pluginName )
    delete pluginName;
    pluginName = NULL;
    #endif // _DEBUG

    __ASSERT_DEBUG( State() == EStateQueued,
                    Panic( EDiagFrameworkCorruptStateMachine ) );

    
    // Case 1) Item was already executed. (Resume scenario)
    // Just go straight to EStateCompleted and let engine know that
    // it was completed.
    if ( Result() != CDiagResultsDatabaseItem::EQueuedToRun )
        {
        // it was already executed. Report result immediately.
        ChangeStateL( EStateCompleted );
        Observer().ExecPlanEntryExecutedL( *this );
        return;
        }

    // Notify observer that progress is 0 so that application will know that
    // plug-in execution has now started.
    Observer().ExecPlanEntryProgressL( *this, 0, TestPlugin().TotalSteps() );
        
    // Case 2) Initial delay was specified. Start delay timer.
    if ( EngineConfig().TestPluginInitialDelay().Int() > 0 )
        {
        // timer value is specified. Start timer.
        ChangeStateL( EStateInitDelay );
        StopInitDelayTimer();
        iInitDelayTimer = CPeriodic::NewL( CActive::EPriorityStandard );
        iInitDelayTimer->Start( EngineConfig().TestPluginInitialDelay(),   // initial delay 
                                EngineConfig().TestPluginInitialDelay(),   // repeat
                                TCallBack( HandleDelayTimerExpiredL, this ) );

        return;
        }

    // Case 3) Start test immediately.
    DoExecuteTestPluginL();
    }



// ---------------------------------------------------------------------------
// CDiagExecPlanEntryImplTest::DoExecuteTestPluginL
// ---------------------------------------------------------------------------
//
void CDiagExecPlanEntryImplTest::DoExecuteTestPluginL()
    {
    #ifdef _DEBUG
    HBufC* pluginName = Plugin().GetPluginNameL( 
        MDiagPlugin::ENameLayoutListSingle );
    
    LOGSTRING3( "CDiagExecPlanEntryImplTest::DoExecuteTestPluginL() "
        L"Starting test plugin Uid 0x%x (%S)", 
        Plugin().Uid().iUid,
        pluginName )
    delete pluginName;
    pluginName = NULL;
    #endif // _DEBUG
        
    // Real test starts now, so reset watchdog timer.
    if ( TestPlugin().RunMode() == MDiagTestPlugin::EAutomatic )
        {
        ResetWatchdog( EDiagEngineWatchdogTypeAutomatic );
        }
    else
        {
        ResetWatchdog( EDiagEngineWatchdogTypeInteractive );
        }

    ChangeStateL( EStateRunning );


    // Create a result in case test fails during RunTestL().
    // It needs to be created at this point to make sure that 
    // test start time is recorded correctly, since RunTestL() may
    // take some time to finish.
    CDiagResultsDbItemBuilder* resultBuilder = 
        CDiagResultsDbItemBuilder::NewLC( Plugin().Uid(), AsDependency() );

    TDiagTestExecParam* execParam = 
        new ( ELeave ) TDiagTestExecParam( *this,   // aObserver
                                           Engine() /* aEngine */ );

    // Trap when calling test. This makes sure that if a test fails during
    // its RunTestL(), it will automatically be marked as failed.
    TRAPD( err, TestPlugin().RunTestL( execParam,      // ownership changed
                                       Engine().IsDependencyDisabled(),
                                       AsDependency(),
                                       Engine().CustomParam() ) );
    execParam = NULL;  //lint !e423 execParam ownership changed.
    
    if ( err != KErrNone )
        {
        // Test failure. Call stop plug-in so that it may have a chance to 
        // clean up, but result is not needed. Discard it and replace it with
        // CDiagResultsDatabaseItem::EFailed
        LOGSTRING2( "CDiagExecPlanEntryImplTest::DoExecuteTestPluginL() "
            L"RunTestL() for 0x%08x failed! Mark it as FAILED and continue",
            Plugin().Uid().iUid )

        StopExecutionAndIgnoreResultL();
        resultBuilder->SetTestCompleted( CDiagResultsDatabaseItem::EFailed );

        CDiagResultsDatabaseItem* result = resultBuilder->ToResultsDatabaseItemL(); 

        // Call test execution observer as if the real test is completed.
        TestExecutionCompletedL( TestPlugin(), result );
        result = NULL;  // ownership transferred above.
        }

    CleanupStack::PopAndDestroy( resultBuilder );
    resultBuilder = NULL;
    }

// ---------------------------------------------------------------------------
// From MDiagTestObserver
// CDiagExecPlanEntryImplTest::TestProgressL
// ---------------------------------------------------------------------------
//
void CDiagExecPlanEntryImplTest::TestProgressL(
        const MDiagTestPlugin& aSender,
        TUint aCurrentStep )
    {
    __ASSERT_ALWAYS( aSender.Uid() == Plugin().Uid(), Panic( EDiagFrameworkPlugin ) );
    __ASSERT_ALWAYS( aCurrentStep <= TestPlugin().TotalSteps(), Panic( EDiagFrameworkPlugin ) );

    // TestExecutionProgressL is accepted only if we were running.
    if ( State() != EStateRunning )
        {
        __ASSERT_DEBUG( 0, Panic( EDiagFrameworkCorruptStateMachine ) );
        return;
        }

    ResetWatchdog();
    Observer().ExecPlanEntryProgressL( *this, aCurrentStep, TestPlugin().TotalSteps() );
    }

// ---------------------------------------------------------------------------
// From MDiagTestObserver
// CDiagExecPlanEntryImplTest::TestExecutionCompletedL
// ---------------------------------------------------------------------------
//
void CDiagExecPlanEntryImplTest::TestExecutionCompletedL( 
        const MDiagTestPlugin& aSender,
        CDiagResultsDatabaseItem* aTestResult )
    {
    __ASSERT_ALWAYS( aSender.Uid() == Plugin().Uid(), Panic( EDiagFrameworkPlugin ) );
    __ASSERT_ALWAYS( aTestResult, Panic( EDiagFrameworkNullTestResult ) );

    // TestExecutionCompletedL is accepted only if we were running.
    if ( State() == EStateCompleted )
        {
        __ASSERT_DEBUG( 0, Panic( EDiagFrameworkCorruptStateMachine ) );
        return;
        }

    // For view-switching plugins, ask the application to switch the view
    // back.  This allows the application to be in a known state.  This must
    // be done before logging the test result, because that may fail.
    if ( TestPlugin().RunMode() == MDiagTestPlugin::EInteractiveView )
        {
        TRAP_IGNORE( Engine().ExecuteAppCommandL(
            EDiagAppCommandSwitchToMainView,
            NULL,
            NULL ) )
        }
    // Writing to db is a non interactive step.
    ResetWatchdog( EDiagEngineWatchdogTypeAutomatic );
    ChangeStateL( EStateStopped );


    // update cache. Cached result will be available until ResultsDbItem()
    // is called.
    delete iCachedResult;
    iCachedResult = aTestResult;
    aTestResult = NULL;

    iResult = iCachedResult->TestResult();

    Engine().DbRecord().LogTestResult( iStatus, *iCachedResult );
    SetActive();
    }

// ---------------------------------------------------------------------------
// From CActive
// CDiagExecPlanEntryImplTest::RunL
// ---------------------------------------------------------------------------
//
void CDiagExecPlanEntryImplTest::RunL()
    {
    // Currently, this object only uses one request, which is
    // Engine().DbRecord().LogTestResult(), which is sent only when state is
    // EStateStopped
    User::LeaveIfError( iStatus.Int() );

    ChangeStateL( EStateCompleted );

    #ifdef _DEBUG
    HBufC* pluginName = Plugin().GetPluginNameL( 
        MDiagPlugin::ENameLayoutListSingle );

    LOGSTRING4( "} ---- TEST END ---- [ 0x%08x  %S %S ] ---- TEST END ----",
        Plugin().Uid().iUid,
        pluginName,
        &TestResultString( iResult ) )

    delete pluginName;
    pluginName = NULL;
    #endif // _DEBUG

    // Notify engine that test is completed.
    Observer().ExecPlanEntryExecutedL( *this );
    }

// ---------------------------------------------------------------------------
// From CActive
// CDiagExecPlanEntryImplTest::DoCancel
// ---------------------------------------------------------------------------
//
void CDiagExecPlanEntryImplTest::DoCancel()
    {
    // Currently, DbRecord().LogTestResult() is the only request this uses.
    Engine().DbRecord().CancelLogTestResult();
    }

// ---------------------------------------------------------------------------
// From CActive
// CDiagExecPlanEntryImplTest::RunError
// ---------------------------------------------------------------------------
//
TInt CDiagExecPlanEntryImplTest::RunError( TInt aError )
    {
    Observer().ExecPlanEntryCriticalError( aError );
    return KErrNone;
    }

// ---------------------------------------------------------------------------
// CDiagExecPlanEntryImplTest::StopExecutionByWatchdogL
// ---------------------------------------------------------------------------
//
void CDiagExecPlanEntryImplTest::StopExecutionByWatchdogL()
    {
    CDiagResultsDatabaseItem* result = StopTestPluginL( MDiagTestPlugin::EWatchdog );
    
    if ( result == NULL )
        {
        // It was not originally running, so no real result. Create
        // a default that makes sense.
        result = CDiagResultsDbItemBuilder::CreateSimpleDbItemL(
            Plugin().Uid(),
            AsDependency(),
            CDiagResultsDatabaseItem::EWatchdogCancel );
        }

    TestExecutionCompletedL( TestPlugin(), result ); // ownership transferred.
    result = NULL;
    }

// ---------------------------------------------------------------------------
// CDiagExecPlanEntryImplTest::DoStopExecutionByClientL
// ---------------------------------------------------------------------------
//
void CDiagExecPlanEntryImplTest::DoStopExecutionByClientL( 
        MDiagEngineCommon::TCancelMode aCancelMode )
    {
    MDiagTestPlugin::TStopReason stopReason;

    switch ( aCancelMode )
        {
        case MDiagEngineCommon::ECancelAll:
            stopReason = MDiagTestPlugin::ECancelAll;
            break;

        case MDiagEngineCommon::ESkip:
            stopReason = MDiagTestPlugin::ESkip;
            break;

        default:
            __ASSERT_DEBUG( 0, Panic( EDiagFrameworkInternal ) );
            stopReason = MDiagTestPlugin::ESkip;
            break;
        }

    CDiagResultsDatabaseItem* result = StopTestPluginL( stopReason );
    
    if ( result == NULL )
        {
        // it was not originally running, so no result was given by
        // the plugin. Create one for them.
        result = CDiagResultsDbItemBuilder::CreateSimpleDbItemL( 
            Plugin().Uid(),
            AsDependency(),
            CDiagResultsDatabaseItem::ESkipped );
        }

    TestExecutionCompletedL( TestPlugin(), result ); // ownership transferred.
    result = NULL;
    }

// ---------------------------------------------------------------------------
// CDiagExecPlanEntryImplTest::StopExecutionAndIgnoreResultL
// ---------------------------------------------------------------------------
//
void CDiagExecPlanEntryImplTest::StopExecutionAndIgnoreResultL()
    {
    CDiagResultsDatabaseItem* result = StopTestPluginL( MDiagTestPlugin::ESkip );
    delete result;
    }

// ---------------------------------------------------------------------------
// CDiagExecPlanEntryImplTest::StopTestPluginL
// ---------------------------------------------------------------------------
//
CDiagResultsDatabaseItem* CDiagExecPlanEntryImplTest::StopTestPluginL( 
        MDiagTestPlugin::TStopReason aReason )
    {
    StopAll();

    CDiagResultsDatabaseItem* result = NULL;

    switch ( State() )
        {
        case EStateQueued:          // fall through
        case EStateInitDelay:
            // Test was not actually running in these casese.
            LOGSTRING3( "CDiagExecPlanEntryImplTest::StopTestPluginL()"
                L"Plugin 0x%x. State = %d",
                Plugin().Uid().iUid,
                State() )
            break;

        case EStateStopped:
            // Test was already completed. Return last result.
            LOGSTRING3( "CDiagExecPlanEntryImplTest::StopTestPluginL()"
                L"Plugin 0x%x. State = %d",
                Plugin().Uid().iUid,
                State() )
            result = GetLastTestResultL();
            break;
            
        case EStateRunning:     // fall through
        case EStateSuspended:
            // Test was actually running in this case.
            // Need to call execution stop and use result from plug-in.
            LOGSTRING2( "CDiagEngineImpl::StopTestPluginL: "
                L"Plugin 0x%x was in ERunning or ESuspended. Call Stop",
                Plugin().Uid().iUid )
            result = TestPlugin().ExecutionStopL( aReason );
            __ASSERT_ALWAYS( result, Panic( EDiagFrameworkNullTestResult ) );
            break;

        case EStateCompleted:
        default:
            LOGSTRING3( "CDiagEngineImpl::StopTestPluginL: "
                L"Plugin 0x%x = INVALID STATE! %d",
                Plugin().Uid().iUid,
                State() )
            __ASSERT_DEBUG( 0, Panic( EDiagFrameworkCorruptStateMachine ) );

            // Use result from cache or DB.
            result = GetLastTestResultL();
            break;
        }

    return result;
    }

// ---------------------------------------------------------------------------
// CDiagExecPlanEntryImplTest::SuspendL
// ---------------------------------------------------------------------------
//
void CDiagExecPlanEntryImplTest::SuspendL()
    {
    StopAll();

    switch ( State() )
        {
        case EStateQueued:          // fall through
        case EStateInitDelay:       // fall through
            // nothing to do. 
            LOGSTRING3( "CDiagExecPlanEntryImplTest::SuspendL(): "
                L"Plugin 0x%08x state = %d. Was not running. Change to EQueued",
                Plugin().Uid().iUid,
                State() )

            ChangeStateL( EStateQueued );
            break;

        case EStateRunning:
            {
            // it is currently running. suspend
            LOGSTRING2( "CDiagExecPlanEntryImplTest::SuspendL(): "
                L"Plugin 0x%08x = ERunning. Trying to stop",
                Plugin().Uid().iUid )
            TRAPD( err, TestPlugin().SuspendL() )
            if ( err == KErrNone )
                {
                LOGSTRING2( "CDiagExecPlanEntryImplTest::SuspendL(): "
                    L"Plugin 0x%08x Stop successful. To ESuspended",
                    Plugin().Uid().iUid )

                ChangeStateL( EStateSuspended );
                }
            else 
                {
                // Suspend probably not supported. Try Stop
                LOGSTRING2( "CDiagExecPlanEntryImplTest::SuspendL(): "
                    L"Plugin 0x%x Stop not successful. Try to Stop",
                    Plugin().Uid().iUid )

                CDiagResultsDatabaseItem* result = 
                    TestPlugin().ExecutionStopL( MDiagTestPlugin::ESkip );
                __ASSERT_ALWAYS( result, Panic( EDiagFrameworkNullTestResult ) );
                delete result;   // not interested in result.
                // force reset the plugin execution to queued so that
                // it will be restarted
                ChangeStateL( EStateQueued );
                }
            }
            break;

        case EStateStopped:
            LOGSTRING2( "CDiagExecPlanEntryImplTest::SuspendL(): "
                L"Plugin 0x%08x = EStateStopped. Do nothing.",
                Plugin().Uid().iUid )
            // do nothing, When resumed, it will store temporary result to db
            break;

        case EStateSuspended:
            LOGSTRING2( "CDiagExecPlanEntryImplTest::SuspendL(): "
                L"Plugin 0x%08x = ESuspended. Already suspended..",
                Plugin().Uid().iUid )
            // do nothing.
            break;

        case EStateCompleted:
            // result already logged. do nothing.
            LOGSTRING2( "CDiagExecPlanEntryImplTest::SuspendL(): "
                L"Plugin 0x%08x = ECompleted. Stay completed.",
                Plugin().Uid().iUid )
            break;

        default:
            __ASSERT_DEBUG( 0, Panic( EDiagFrameworkCorruptStateMachine ) );
        }
    }

// ---------------------------------------------------------------------------
// CDiagExecPlanEntryImplTest::ResumeL
// ---------------------------------------------------------------------------
//
void CDiagExecPlanEntryImplTest::ResumeL()
    {
    LOGSTRING3( "CDiagExecPlanEntryImplTest::ResumeL(): "
        L"Plugin 0x%08x, State = %d", Plugin().Uid().iUid, State() )

    // resumable states are: EStateQueued, EStateSuspended, EStateStopped

    switch ( State() )
        {
        case EStateQueued:
            ExecuteL();
            break;

        case EStateSuspended:
            {
            ChangeStateL( EStateRunning );
            
            TestPlugin().ResumeL();
            }
            break;

        case EStateStopped:
            {
            // There was a result not saved in DB yet.
            // If test was suspended in EStopped state, there MUST BE a
            // saved test result.
            __ASSERT_DEBUG( iCachedResult, Panic( EDiagFrameworkInternal ) );

            if ( iCachedResult ) //lint !e774 Will be evaluated in non DEBUG build.
                {
                // Simulate TestExecutionCompletedL from plug-in.
                // Unset iCachedResult, since TestExecutionCompletedL() will try 
                // to deallocate iCachedResult.
                CDiagResultsDatabaseItem* result = iCachedResult;
                iCachedResult = NULL;
                TestExecutionCompletedL( TestPlugin(), result ); // ownership passed
                result = NULL;
                }
            }
            break;

        default:
            // already running. ignored.
            break;
        }
    }

    
// ---------------------------------------------------------------------------
// CDiagExecPlanEntryImplTest::GetLastTestResultL
// ---------------------------------------------------------------------------
//
CDiagResultsDatabaseItem* CDiagExecPlanEntryImplTest::GetLastTestResultL()
    {
    // Currently, cache is implemented such that calling 
    // GetLastTestResultL() once after test is completed will not 
    // require a request to DB. Any subsequent request will have to access
    // database. 
    // 
    // This should satisfy most common usage of execution plan entry.
    // It could be expanded to store for longer than that, however, it
    // may take up too much memory if there are lots of tests in the 
    // execution plan. At this point, this should be sufficient.
    
    CDiagResultsDatabaseItem* result = NULL;

    if ( iCachedResult )
        {
        if ( State() != EStateStopped )
            {
            LOGSTRING2( "CDiagExecPlanEntryImplTest::GetLastTestResultL() "
                L"Plugin 0x%08x. Cache hit",
                Plugin().Uid().iUid )
            result = iCachedResult;
            iCachedResult = NULL;
            }
        else
            {
            LOGSTRING2( "CDiagExecPlanEntryImplTest::GetLastTestResultL() "
                L"Plugin 0x%08x. Cache hit, but need to duplicate.",
                Plugin().Uid().iUid )
            // If we were in STOPPED state, make a copy, since it
            // may not been fully written to db yet.
            // Cached copy should be deleted only after it is written to db.
            const CBufFlat* detailData = iCachedResult->DetailsData();
            CBufFlat* detailDataCopy = NULL;

            if ( detailData )
                {
                detailDataCopy = CBufFlat::NewL( detailData->Size() );
                CleanupStack::PushL( detailDataCopy );

                TPtr8 ptr = detailDataCopy->Ptr( 0 );
                detailData->Read( 0, ptr );
                }

            result = CDiagResultsDatabaseItem::NewL(
                iCachedResult->TestUid(),
                iCachedResult->WasDependency(),
                iCachedResult->TestResult(),
                iCachedResult->TimeStarted(),
                iCachedResult->TimeCompleted(),
                detailDataCopy );

            if ( detailDataCopy )
                {
                CleanupStack::Pop( detailDataCopy );
                }
            }
        }
    else
        {
        LOGSTRING2( "CDiagExecPlanEntryImplTest::GetLastTestResultL() "
            L"Plugin 0x%08x. Cache miss",
            Plugin().Uid().iUid )
        // result was not avaiable in cache. Fetch it from database.
        User::LeaveIfError( Engine().DbRecord().GetTestResult( Plugin().Uid(), result ) );
        }

    return result;
    }

// ---------------------------------------------------------------------------
// CDiagExecPlanEntryImplTest::HandleDelayTimerExpiredL
// ---------------------------------------------------------------------------
//
TInt CDiagExecPlanEntryImplTest::HandleDelayTimerExpiredL( TAny* aData )
    {
    CDiagExecPlanEntryImplTest* self = static_cast< CDiagExecPlanEntryImplTest* >( aData );
    
    self->StopInitDelayTimer();
    self->DoExecuteTestPluginL();
    return 0;
    }

// ---------------------------------------------------------------------------
// CDiagExecPlanEntryImplTest::StopAll
// ---------------------------------------------------------------------------
//
void CDiagExecPlanEntryImplTest::StopAll()
    {
    Cancel();
    StopInitDelayTimer();
    StopWatchdogTemporarily();
    }

// ---------------------------------------------------------------------------
// CDiagExecPlanEntryImplTest::StopInitDelayTimer
// ---------------------------------------------------------------------------
//
void CDiagExecPlanEntryImplTest::StopInitDelayTimer()
    {
    if ( iInitDelayTimer )
        {
        iInitDelayTimer->Cancel();
        delete iInitDelayTimer;
        iInitDelayTimer = NULL;
        }
    }

// ---------------------------------------------------------------------------
// CDiagExecPlanEntryImplTest::TestResultString
// ---------------------------------------------------------------------------
//
#ifdef _DEBUG
const TDesC& CDiagExecPlanEntryImplTest::TestResultString( 
        CDiagResultsDatabaseItem::TResult aResult )
    {
    _LIT( KSuccess, "ESuccess" );
    _LIT( KFailed, "EFailed" );
    _LIT( KSkipped, "ESkipped" );
    _LIT( KCancelled, "ECancelled" );
    _LIT( KInterrupted, "EInterrupted" );
    _LIT( KNotPerformed, "ENotPerformed" );
    _LIT( KDependencyFailed, "EDependencyFailed" );
    _LIT( KWatchdogCancel, "EWatchdogCancel" );
    _LIT( KSuspended, "ESuspended" );
    _LIT( KQueuedToRun, "EQueuedToRun" );

    switch( aResult )
        {
        case CDiagResultsDatabaseItem::ESuccess:
            return KSuccess();

        case CDiagResultsDatabaseItem::EFailed:
            return KFailed();

        case CDiagResultsDatabaseItem::ESkipped:
            return KSkipped();

        case CDiagResultsDatabaseItem::ECancelled:
            return KCancelled();

        case CDiagResultsDatabaseItem::EInterrupted:
            return KInterrupted();

        case CDiagResultsDatabaseItem::ENotPerformed:
            return KNotPerformed();

        case CDiagResultsDatabaseItem::EDependencyFailed:
            return KDependencyFailed();

        case CDiagResultsDatabaseItem::EWatchdogCancel:
            return KWatchdogCancel();

        case CDiagResultsDatabaseItem::ESuspended:
            return KSuspended();

        case CDiagResultsDatabaseItem::EQueuedToRun:
            return KQueuedToRun();

        default:
            _LIT( KUnknown, "* Unknown *" );
            return KUnknown();
        }
    }

#else   // #if _DEBUG

// non-debug version.
const TDesC& CDiagExecPlanEntryImplTest::TestResultString( 
        CDiagResultsDatabaseItem::TResult /* aResult */ )
    {
    _LIT( KNonDebugResultName, "?" );
    return KNonDebugResultName();
    }

#endif // #else _DEBUG

// End of File

