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
* Description:  Class definition of CDiagExecPlanEntryImplSuite
*
*/


// CLASS DECLARATION
#include "diagexecplanentryimplsuite.h"

// SYSTEM INCLUDE FILES
#include <DiagPlugin.h>                     // MDiagPlugin
#include <DiagSuitePlugin.h>                // MDiagSuitePlugin
#include <DiagFrameworkDebug.h>             // LOGSTRING
#include <DiagSuiteExecParam.h>             // TDiagSuiteExecParam

// USER INCLUDE FILES
#include "diagframework.pan"                // Panics
#include "diagexecplanentryimplobserver.h"  // MDiagExecPlanEntryImplObserver

// DATA

// MACROS

// LOCAL DATA TYPES


// ======== LOCAL FUNCTIONS ========

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// CDiagExecPlanEntryImplSuite::NewL
// ---------------------------------------------------------------------------
//
CDiagExecPlanEntryImplSuite* CDiagExecPlanEntryImplSuite::NewL(
        MDiagEngineCommon& aEngine,
        const TDiagEngineConfig& aEngineConfig,
        MDiagExecPlanEntryImplObserver& aObserver,
        MDiagSuitePlugin& aPlugin,
        TBool aAsDependency,
        CDiagExecPlanEntryImpl::TType aType )
    {
    CDiagExecPlanEntryImplSuite* self = NewLC( aEngine,
                                               aEngineConfig,
                                               aObserver,
                                               aPlugin,
                                               aAsDependency,
                                               aType );
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// CDiagExecPlanEntryImplSuite::NewLC
// ---------------------------------------------------------------------------
//
CDiagExecPlanEntryImplSuite* CDiagExecPlanEntryImplSuite::NewLC(
        MDiagEngineCommon& aEngine,
        const TDiagEngineConfig& aEngineConfig,
        MDiagExecPlanEntryImplObserver& aObserver,
        MDiagSuitePlugin& aPlugin,
        TBool aAsDependency,
        CDiagExecPlanEntryImpl::TType aType )
    {
    CDiagExecPlanEntryImplSuite* self = 
        new ( ELeave ) CDiagExecPlanEntryImplSuite ( aEngine,
                                                     aEngineConfig,
                                                     aObserver,
                                                     aPlugin, 
                                                     aAsDependency,
                                                     aType );
    CleanupStack::PushL( self );
    return self;
    }


// ---------------------------------------------------------------------------
// CDiagExecPlanEntryImplSuite::CDiagExecPlanEntryImplSuite
// ---------------------------------------------------------------------------
//
CDiagExecPlanEntryImplSuite::CDiagExecPlanEntryImplSuite( 
        MDiagEngineCommon& aEngine,
        const TDiagEngineConfig& aEngineConfig,
        MDiagExecPlanEntryImplObserver& aObserver,
        MDiagSuitePlugin& aPlugin,
        TBool aAsDependency,
        CDiagExecPlanEntryImpl::TType aType )
    :   CDiagExecPlanEntryImpl( 
            aEngine,
            aEngineConfig,
            aObserver,
            aPlugin,
            aAsDependency,
            aType  )
    {
    }

// ---------------------------------------------------------------------------
// CDiagExecPlanEntryImplSuite::~CDiagExecPlanEntryImplSuite
// ---------------------------------------------------------------------------
//
CDiagExecPlanEntryImplSuite::~CDiagExecPlanEntryImplSuite()
    {
    Cancel();

    if ( CDiagExecPlanEntryImpl::State() == EStateRunning )
        {
        TRAP_IGNORE( SuitePlugin().ExecutionStopL( MDiagSuitePlugin::ESkip ) )
        }
    }

// ---------------------------------------------------------------------------
// CDiagExecPlanEntryImplSuite::SuitePlugin
// ---------------------------------------------------------------------------
//
MDiagSuitePlugin& CDiagExecPlanEntryImplSuite::SuitePlugin()
    {
    // It is safe to typecast here, since CDiagExecPlanEntryImplSuite::NewL
    // accepts only MDiagSuitePlugin&.
    return static_cast< MDiagSuitePlugin& >( Plugin() );
    }

// ---------------------------------------------------------------------------
// From CDiagExecPlanEntryImpl
// CDiagExecPlanEntryImplSuite::ExecuteL
// ---------------------------------------------------------------------------
//
void CDiagExecPlanEntryImplSuite::ExecuteL()
    {
    #ifdef _DEBUG
    HBufC* pluginName = Plugin().GetPluginNameL( 
        MDiagPlugin::ENameLayoutListSingle );
    
    LOGSTRING4( "---- SUITE BEGIN ---- [ 0x%08x %S / %s ] ---- SUITE BEGIN ---- {",
        Plugin().Uid().iUid,
        pluginName,
        ( Type() == ETypeSuitePrepare ) ? L"PREPARE" : L"FINALIZE" )

    delete pluginName;
    pluginName = NULL;
    #endif // _DEBUG

    __ASSERT_DEBUG( State() == EStateQueued,
                    Panic( EDiagFrameworkCorruptStateMachine ) );

    ResetWatchdog( EDiagEngineWatchdogTypeAutomatic );
    ChangeStateL( EStateRunning );

    // Notify observer that progress is 0 of 1 since suite does not have progress.
    Observer().ExecPlanEntryProgressL( *this, 0, 1 );

    TDiagSuiteExecParam* execParam = new ( ELeave ) TDiagSuiteExecParam( *this, Engine() );
    if ( Type() == CDiagExecPlanEntryImpl::ETypeSuitePrepare )
        {
        SuitePlugin().PrepareChildrenExecutionL( execParam,  // owership change
                                                 Engine().IsDependencyDisabled(),
                                                 AsDependency() );
        }
    else
        {
        SuitePlugin().FinalizeChildrenExecutionL( execParam, // owership change
                                                  Engine().IsDependencyDisabled(),
                                                  AsDependency() );
        }

    execParam = NULL; //lint !e423 execParam ownership changed.
    }

// ---------------------------------------------------------------------------
// From CDiagExecPlanEntryImpl
// CDiagExecPlanEntryImplSuite::StopExecutionByWatchdogL
// ---------------------------------------------------------------------------
//
void CDiagExecPlanEntryImplSuite::StopExecutionByWatchdogL()
    {
    StopSuitePluginL( MDiagSuitePlugin::EWatchdog );
    }

// ---------------------------------------------------------------------------
// From CDiagExecPlanEntryImpl
// CDiagExecPlanEntryImplSuite::StopExecutionByClientL
// ---------------------------------------------------------------------------
//
void CDiagExecPlanEntryImplSuite::DoStopExecutionByClientL( 
        MDiagEngineCommon::TCancelMode aCancelMode )
    {
    MDiagSuitePlugin::TStopReason stopReason;

    switch ( aCancelMode )
        {
        case MDiagEngineCommon::ECancelAll:
            stopReason = MDiagSuitePlugin::ECancelAll;
            break;

        case MDiagEngineCommon::ESkip:
            stopReason = MDiagSuitePlugin::ESkip;
            break;

        default:
            __ASSERT_DEBUG( 0, Panic( EDiagFrameworkInternal ) );
            stopReason = MDiagSuitePlugin::ESkip;
            break;
        }

    StopSuitePluginL( stopReason );
    }

// ---------------------------------------------------------------------------
// CDiagExecPlanEntryImplSuite::StopSuitePluginL
// ---------------------------------------------------------------------------
//
void CDiagExecPlanEntryImplSuite::StopSuitePluginL(
        MDiagSuitePlugin::TStopReason aStopReason )
    {
    LOGSTRING3( "CDiagExecPlanEntryImplSuite::StopSuitePluginL(): "
        L"Calling plugin uid = 0x%08x SuitePlugin::ExecutionStopL( %d )",
        Plugin().Uid().iUid,
        aStopReason )

    Cancel();

    switch ( State() )
        {
        case EStateQueued:      // fall through
        case EStateInitDelay:   // fall through
        case EStateStopped:     // fall through
        case EStateSuspended:   // fall through
        case EStateCompleted:
            LOGSTRING3( "CDiagExecPlanEntryImplSuite::DoStopSuitePluginL(): "
                L"Plugin 0x%08x = %d. Do nothing.",
                Plugin().Uid().iUid,
                State() )
            break;

        case EStateRunning:
            LOGSTRING2( "CDiagExecPlanEntryImplSuite::DoStopSuitePluginL(): "
                L"Plugin 0x%08x was in ERunning or ESuspended. Call Stop",
                Plugin().Uid().iUid )
            SuitePlugin().ExecutionStopL( aStopReason );
            break;

        default:
            LOGSTRING3( "CDiagExecPlanEntryImplSuite::DoStopSuitePluginL(): "
                L"Plugin 0x%08x = INVALID STATE! %d",
                Plugin().Uid().iUid,
                State() )
            __ASSERT_DEBUG( 0, Panic( EDiagFrameworkCorruptStateMachine ) );
            break;
        }

    // Notify observer as if plug-in itself would have done.
    ContinueExecutionL( SuitePlugin() );
    }

// ---------------------------------------------------------------------------
// CDiagExecPlanEntryImplSuite::SuspendL
// ---------------------------------------------------------------------------
//
void CDiagExecPlanEntryImplSuite::SuspendL()
    {
    Cancel();

    switch ( State() )
        {
        case EStateQueued:      // fall through
        case EStateInitDelay:   // fall through
            LOGSTRING3( "CDiagExecPlanEntryImplSuite::SuspendL(): "
                L"Plugin 0x%08x = %d. Reset to Queued state",
                Plugin().Uid().iUid,
                State() )
            // force reset the execution state to queued so that
            // it will be restarted
            ChangeStateL( EStateQueued );
            break;

        case EStateRunning:
            LOGSTRING2( "CDiagExecPlanEntryImplSuite::SuspendL(): "
                L"Plugin 0x%08x was in ERunning. Calling Stop",
                Plugin().Uid().iUid )

            // Suites do not have suspend. Stop with cancel as reason.
            SuitePlugin().ExecutionStopL( MDiagSuitePlugin::ESkip );
            // force reset the plugin execution to queued so that
            // it will be restarted
            ChangeStateL( EStateQueued );
            break;

        case EStateStopped:
            LOGSTRING2( "CDiagExecPlanEntryImplSuite::SuspendL(): "
                L"Plugin 0x%08x = Stopped.",
                Plugin().Uid().iUid )
            // plug-in is already stopped.
            ChangeStateL( EStateSuspended );
            break;

        case EStateSuspended:
            LOGSTRING2( "CDiagExecPlanEntryImplSuite::SuspendL(): "
                L"Plugin 0x%08x = Suspended.",
                Plugin().Uid().iUid )
            // already suspneded. nothing to do.
            break;

        case EStateCompleted:
            LOGSTRING2( "CDiagExecPlanEntryImplSuite::SuspendL(): "
                L"Plugin 0x%08x was already in completed state. No change",
                Plugin().Uid().iUid )
            break;

        default:
            LOGSTRING3( "CDiagExecPlanEntryImplSuite::SuspendL(): "
                L"Plugin 0x%08x = INVALID STATE! %d",
                Plugin().Uid().iUid,
                State() )
            __ASSERT_DEBUG( 0, Panic( EDiagFrameworkCorruptStateMachine ) );
            break;
        }

    // Postcondition. Check acceptble states.
    __ASSERT_DEBUG( State() == EStateQueued ||
                    State() == EStateSuspended ||
                    State() == EStateCompleted,
                    Panic( EDiagFrameworkCorruptStateMachine ) );
    }

// ---------------------------------------------------------------------------
// CDiagExecPlanEntryImplSuite::ResumeL
// ---------------------------------------------------------------------------
//
void CDiagExecPlanEntryImplSuite::ResumeL()
    {
    LOGSTRING3( "CDiagExecPlanEntryImplTest::ResumeL(): "
        L"Plugin 0x%08x, State = %d", Plugin().Uid().iUid, State() )

    __ASSERT_DEBUG( State() == EStateQueued ||
                    State() == EStateSuspended,
                    Panic( EDiagFrameworkCorruptStateMachine ) );

    if ( State() == EStateQueued )
        {
        ExecuteL();
        }
    else
        {
        ContinueExecutionL( SuitePlugin() );
        }
    }

// ---------------------------------------------------------------------------
// From MDiagSuiteObserver
// CDiagExecPlanEntryImplSuite::ContinueExecutionL
// ---------------------------------------------------------------------------
//
void CDiagExecPlanEntryImplSuite::ContinueExecutionL( const MDiagSuitePlugin& aSender )
    {
    __ASSERT_ALWAYS( aSender.Uid() == Plugin().Uid(), Panic( EDiagFrameworkPlugin ) );

    // Need to do an empty request complete since by default, suite plug-ins
    // do nothing in prepare/finalize steps and just call ContinueExecutionL.
    // Doing empty request complete ensures that Plug-in call back does not
    // directly call application TestExecutionCompletedL() method.

    ChangeStateL( EStateStopped );

    TRequestStatus* status = &iStatus;
    User::RequestComplete( status, KErrNone );
    SetActive();
    }

// ---------------------------------------------------------------------------
// From CActive
// CDiagExecPlanEntryImplSuite::RunL
// ---------------------------------------------------------------------------
//
void CDiagExecPlanEntryImplSuite::RunL()
    {
    ChangeStateL( EStateCompleted );

    #ifdef _DEBUG
    HBufC* pluginName = Plugin().GetPluginNameL( 
        MDiagPlugin::ENameLayoutListSingle );
    
    LOGSTRING4( "} ---- SUITE END ---- [ 0x%08x %S / %s ] ---- SUITE END ----",
        Plugin().Uid().iUid,
        pluginName,
        ( Type() == ETypeSuitePrepare ) ? L"PREPARE" : L"FINALIZE" )

    delete pluginName;
    pluginName = NULL;
    #endif // _DEBUG

    Observer().ExecPlanEntryExecutedL( *this );
    }


// ---------------------------------------------------------------------------
// From CActive
// CDiagExecPlanEntryImplSuite::DoCancel
// ---------------------------------------------------------------------------
//
void CDiagExecPlanEntryImplSuite::DoCancel()
    {
    // nothing to do
    }


// End of File

