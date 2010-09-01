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
* Description:  Class definition of CDiagEngineImpl
*
*/


// CLASS DECLARATION
#include "diagengineimpl.h"                 // CDiagEngineImpl

// SYSTEM INCLUDE FILES
#include <DiagPluginPool.h>                 // CDiagPluginPool
#include <DiagFrameworkDebug.h>             // LOGSTRING
#include <DiagPlugin.h>                     // MDiagPlugin
#include <DiagTestExecParam.h>              // TDiagTestExecParam
#include <DiagSuiteExecParam.h>             // TDiagSuiteExecParam
#include <DiagResultsDbItemBuilder.h>       // CDiagResultsDbItemBuilder
#include <DiagResultsDbRecordEngineParam.h> // CDiagResultsDbRecordEngineParam

// USER INCLUDE FILES
#include "diagpluginexecplanimpl.h"         // CDiagPluginExecPlanImpl
#include "diagexecplanentryimpltest.h"      // CDiagExecPlanEntryImplTest
#include "diagenginestatemachine.h"         // DiagFwInternal::CStateMachine
#include "diagenginecallhandler.h"          // CDiagEngineCallHandler
#include "diagframework.pan"                // Panic Codes
#include "diagengineconfig.h"               // TDiagEngineConfig

// EVENT INCLUDE FILES
#include "diagengineeventbasic.h"           // DiagFwInternal::CEventBasic
#include "diagengineeventtestprogress.h"    // DiagFwInternal::CEventTestProgress

using namespace DiagFwInternal;

// DATA

// ======== LOCAL FUNCTIONS ========

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// CDiagEngineImpl::NewL()
// ---------------------------------------------------------------------------
//
CDiagEngineImpl* CDiagEngineImpl::NewL( 
        CAknViewAppUi&          aViewAppUi, 
        MDiagEngineObserver&    aObserver,
        RDiagResultsDatabase&   aDbSession,
        CDiagPluginPool&        aPluginPool,
        TBool                   aDisableDependency,
        const RArray< TUid >&   aExecutionBatch )
    {
    CDiagEngineImpl* self = CDiagEngineImpl::NewLC( aViewAppUi,
                                                    aObserver,
                                                    aDbSession,
                                                    aPluginPool,
                                                    aDisableDependency,
                                                    aExecutionBatch );
    CleanupStack::Pop( self );
    return self;
    }


// ---------------------------------------------------------------------------
// CDiagEngineImpl::NewLC()
// ---------------------------------------------------------------------------
//
CDiagEngineImpl* CDiagEngineImpl::NewLC( 
        CAknViewAppUi&          aViewAppUi, 
        MDiagEngineObserver&    aObserver,
        RDiagResultsDatabase&   aDbSession,
        CDiagPluginPool&        aPluginPool,
        TBool                   aDisableDependency,
        const RArray< TUid >&   aExecutionBatch )
    {
    CDiagEngineImpl* self = new ( ELeave ) CDiagEngineImpl( aViewAppUi,
                                                            aObserver,
                                                            aDbSession,
                                                            aPluginPool );

    CleanupStack::PushL( self );
    self->ConstructNewRecordL( aDisableDependency, aExecutionBatch );
    return self;
    }

// ---------------------------------------------------------------------------
// CDiagEngineImpl::NewL()
// ---------------------------------------------------------------------------
//
CDiagEngineImpl* CDiagEngineImpl::NewL( CAknViewAppUi&          aViewAppUi, 
                                        MDiagEngineObserver&    aObserver,
                                        RDiagResultsDatabase&   aDbSession,
                                        CDiagPluginPool&        aPluginPool,
                                        TUid                    aIncompleteRecordUid )
    {
    CDiagEngineImpl* self = CDiagEngineImpl::NewLC( aViewAppUi,
                                                    aObserver,
                                                    aDbSession,
                                                    aPluginPool,
                                                    aIncompleteRecordUid );
    CleanupStack::Pop( self );
    return self;
    }


// ---------------------------------------------------------------------------
// CDiagEngineImpl::NewLC()
// ---------------------------------------------------------------------------
//
CDiagEngineImpl* CDiagEngineImpl::NewLC( CAknViewAppUi&    aViewAppUi, 
                                   MDiagEngineObserver&    aObserver,
                                   RDiagResultsDatabase&   aDbSession,
                                   CDiagPluginPool&        aPluginPool,
                                   TUid                    aIncompleteRecordUid )
    {
    CDiagEngineImpl* self = new ( ELeave ) CDiagEngineImpl( aViewAppUi,
                                                            aObserver,
                                                            aDbSession,
                                                            aPluginPool );

    CleanupStack::PushL( self );
    self->ConstructIncompleteRecordL( aIncompleteRecordUid );
    return self;
    }

// ---------------------------------------------------------------------------
// CDiagEngineImpl::CDiagEngineImpl
// ---------------------------------------------------------------------------
//
CDiagEngineImpl::CDiagEngineImpl( CAknViewAppUi&          aViewAppUi,
                                  MDiagEngineObserver&    aObserver,
                                  RDiagResultsDatabase&   aDbSession,
                                  CDiagPluginPool&        aPluginPool )
    :   CActive( EPriorityStandard ),
        iViewAppUi( aViewAppUi ),
        iObserver( aObserver ),
        iDbSession( aDbSession ),
        iPluginPool( aPluginPool )
    {
    LOGSTRING( "---- DIAG ENGINE BEGIN ---- DIAG ENGINE BEGIN ----{" )
    CActiveScheduler::Add( this );
    }

// ---------------------------------------------------------------------------
// CDiagEngineImpl::ConstructNewRecordL
// ---------------------------------------------------------------------------
//
void CDiagEngineImpl::ConstructNewRecordL( TBool aDisableDependency,
                                           const RArray< TUid >& aExecutionBatch )
    {
    ConstructCommonL();

    LOGSTRING( "CDiagEngineImpl::ConstructNewRecordL()" )

    iContinueIncompleteRecord = EFalse;
    iEngineConfig.SetDependencyDisabled( aDisableDependency );

    // convert uid array to plugin reference array.
    // Also, make a copy of the plug-in UID array so that it can be
    // passed to database.
    TInt batchCount = aExecutionBatch.Count();

    // uidList must be allocated in heap since it database will own the data later.
    RArray< TUid >* uidList = new( ELeave )RArray< TUid >();
    CleanupStack::PushL( uidList );     // to delete array itself.
    CleanupClosePushL( *uidList );      // to call close.

    uidList->ReserveL( batchCount );
    
    MDiagPlugin* plugin = NULL;

    for ( TInt i = 0; i < batchCount ; i++ )
        {
        LOGSTRING2( "CDiagEngineImpl::ConstructNewRecordL() "
            L"Add plugin 0x%08x", aExecutionBatch[i].iUid )
            
        iPluginPool.FindPlugin( aExecutionBatch[i], plugin );
        iBatch.AppendL( plugin );
        plugin = NULL;

        uidList->AppendL( aExecutionBatch[i] );
        }

    CDiagResultsDbRecordEngineParam* engineParam = 
        CDiagResultsDbRecordEngineParam::NewL(
            uidList,                                // ownership transfer
            !iEngineConfig.IsDependencyDisabled()   /* isDependencyExecution */ );

    CleanupStack::Pop( uidList );   // CleanupClosePushL( *uidList )
    CleanupStack::Pop( uidList );   // CleanupStack::PushL( uidList )
    uidList = NULL;

    CleanupStack::PushL( engineParam );
    User::LeaveIfError( 
        iDbRecord.CreateNewRecord( iDbSession, iRecordId, *engineParam ) );
    CleanupStack::PopAndDestroy( engineParam );
    engineParam = NULL;

    iPlan = CDiagPluginExecPlanImpl::NewL( *this, iEngineConfig, *this );
    }

// ---------------------------------------------------------------------------
// CDiagEngineImpl::ConstructIncompleteRecordL
// ---------------------------------------------------------------------------
//
void CDiagEngineImpl::ConstructIncompleteRecordL( TUid aIncompleteRecordUid )
    {
    ConstructCommonL();

    LOGSTRING2( "CDiagEngineImpl::ConstructIncompleteRecordL() "
        L"RecordId = 0x%08x", aIncompleteRecordUid.iUid )
        
    iRecordId = aIncompleteRecordUid;
    iContinueIncompleteRecord = ETrue;

    User::LeaveIfError( 
        iDbRecord.Connect( iDbSession, 
                           iRecordId,
                           EFalse   /* aReadOnly */ ) );

    // make sure that record is open for writing.
    TBool isTestCompleted = EFalse;
    User::LeaveIfError( iDbRecord.IsTestCompleted( isTestCompleted ) );
    if ( isTestCompleted )
        {
        User::Leave( KErrLocked );
        }

    // recover original parameter
    CDiagResultsDbRecordEngineParam* engineParam = NULL;
    User::LeaveIfError(
        iDbRecord.GetEngineParam( engineParam ) );

    CleanupStack::PushL( engineParam );

    // Recover original batch.
    MDiagPlugin* plugin = NULL;
    iEngineConfig.SetDependencyDisabled( !engineParam->DependencyExecution() );
    const RArray< TUid >& uidArray = engineParam->ExecutionsUidArray();
    TInt count = uidArray.Count();
    for ( TInt i = 0; i < count; i++ )
        {
        LOGSTRING2( "CDiagEngineImpl::ConstructNewRecordL() "
            L"Add plugin 0x%08x", uidArray[i].iUid )
            
        User::LeaveIfError( 
            iPluginPool.FindPlugin( uidArray[i], plugin ) );
        iBatch.AppendL( plugin );
        plugin = NULL;
        }
    
    CleanupStack::PopAndDestroy( engineParam );
    engineParam = NULL;

    iPlan = CDiagPluginExecPlanImpl::NewL( *this, iEngineConfig, *this );
    }

// ---------------------------------------------------------------------------
// CDiagEngineImpl::ConstructCommonL()
// ---------------------------------------------------------------------------
//
void CDiagEngineImpl::ConstructCommonL()
    {
    LOGSTRING( "CDiagEngineImpl::ConstructCommonL." )

    // Read Cenrep key
    iEngineConfig.ReadCenrepKeysL();

    // Create state machine.
    iStateMachine = CStateMachine::NewL( *this );

    // Create call handler.
    iCallHandler = CDiagEngineCallHandler::NewL( *this );

    }

// ---------------------------------------------------------------------------
// CDiagEngineImpl::~CDiagEngineImpl
// ---------------------------------------------------------------------------
//
CDiagEngineImpl::~CDiagEngineImpl()
    {
    LOGSTRING( "CDiagEngineImpl::~CDiagEngineImpl() Destructor" )

    StopAllRequests();

    CleanupIncompleteTestSession();

    // always delete plan before record is closed.
    delete iPlan;
    iPlan = NULL;

    iDbRecord.Close();

    iBatch.Close(); // MUST NOT destroy since we don't own elements within it

    delete iStateMachine;
    iStateMachine = NULL;

    delete iSuspendedResult;
    iSuspendedResult = NULL;

    delete iCallHandler;
    iCallHandler = NULL;

    LOGSTRING( "} ---- DIAG ENGINE END ---- DIAG ENGINE END ----" )
    }

// ---------------------------------------------------------------------------
// CDiagEngineImpl::CleanupIncompleteTestSession
// ---------------------------------------------------------------------------
//
void CDiagEngineImpl::CleanupIncompleteTestSession()
    {
    if ( iStateMachine == NULL || 
            iStateMachine->CurrentState() == EStateNotReady ||
            iStateMachine->CurrentState() == EStateStopped )
        {
        // Test is never started or already finished. Nothing to clean up here.
        return;
        }

    // At this point, we can either suspend or complete the test record.
    // Normally, since state machine is not in EStateStopped state,
    // record should suspended.
    //
    // However, even if it is not completely done executing the plan, 
    // if last test is already completed, engine will report TestCompleted( ETrue ).
    // Otherwise, when examining db record for resuming, the record will appear
    // incomplete, but in reality there is no more test to execute and resuming the
    // record will immediately finish the session, which is meaningless.
    // Hence, when last test plug-in is executed, engine will record test completed,
    // instead of suspend, even if there are suite items left in plan.

    LOGSTRING2( "CDiagEngineImpl::CleanupIncompleteTestSession() State = %d", 
        iStateMachine->CurrentState() )

    switch( iStateMachine->CurrentState() )
        {
        case EStateStopped:
            // all completed. Nothing to finalize.
            break;

        case EStateNotReady:        // fall through
        case EStateCreatingPlan:
            // Plan was not fully created.
            // Mark the record as incomplete and let execution plan
            // to figure out how to resume later.
            // No need to call NotifyPluginsOfTestSessionEnd() since 
            // TestSessionBeginL() was never called. 
            iDbRecord.Suspend();    // in destructor. Error ignored.
            break;
        
        case EStateCancelAll:
            {
            // Test session was being cancelled. Since we were in cancel all mode,
            // it cannot be resumed. Mark the record completed.
            NotifyPluginsOfTestSessionEnd();

            TInt err =  iDbRecord.TestCompleted( EFalse /* aFullyComplete */ );
            LOGSTRING2( "CDiagEngineImpl::CleanupIncompleteTestSession() "
                L" TestCompleted( EFalse ) err = %d", err )
            // in destructor. Error ignored.
            }
            break;

        case EStateFinalizing:
            {
            // Engine was about to finalize, but never got a chance to.
            // In this state, db record should always be closed with TestCompleted
            // since it is a normal execution finish scenario and it cannot
            // be resumed later.
            NotifyPluginsOfTestSessionEnd();

            TBool isFullyCompleted = ( iEngineError == KErrNone );
            TInt err = iDbRecord.TestCompleted( isFullyCompleted );
            LOGSTRING3( "CDiagEngineImpl::CleanupIncompleteTestSession() "
                L"iDbRecord.TestCompleted( %d ) err = %d", isFullyCompleted, err )
            
            // We are in destructor. Ignore errors from db.
            }
            break;

        default:
            // In other states, engine was still executing some items.
            // Check if it shoud be marked as suspended or not.
            {
            TBool allTestsCompleted = EFalse;
            for ( TInt i = iPlan->CurrentIndex(); i < iPlan->Count(); i++ )
                {
                if ( (*iPlan)[i].Plugin().Type() == MDiagPlugin::ETypeTestPlugin &&
                     (*iPlan)[i].State() != MDiagExecPlanEntry::EStateCompleted )
                    {
                    // Found a test entry that is not completed.
                    allTestsCompleted = EFalse;
                    break; //lint !e960  break OK here.
                    }
                }

            NotifyPluginsOfTestSessionEnd();

            if ( allTestsCompleted )
                {
                TInt err =  iDbRecord.TestCompleted( ETrue /* aFullyComplete */ );
                LOGSTRING2( "CDiagEngineImpl::CleanupIncompleteTestSession() "
                    L" iDbRecord.TestCompleted( ETrue ) err = %d", err )
                }
            else
                {
                TInt err = iDbRecord.Suspend();
                LOGSTRING2( "CDiagEngineImpl::CleanupIncompleteTestSession(). "
                    L"iDbRecord.Suspend() err = %d", err )
                }
            
            }
            break;

        }
    }

// ---------------------------------------------------------------------------
// CDiagEngineImpl::ExecuteL
// ---------------------------------------------------------------------------
//
void CDiagEngineImpl::ExecuteL()
    {
    __ASSERT_ALWAYS( iStateMachine->CurrentState() == EStateNotReady,
                     Panic( EDiagFrameworkInvalidState ) );

    iStateMachine->AddEventL( EEventExecute );
    }

// ---------------------------------------------------------------------------
// CDiagEngineImpl::SetCustomParam
// ---------------------------------------------------------------------------
//
void CDiagEngineImpl::SetCustomParam( TAny* aCustomParams )
    {
    __ASSERT_ALWAYS( aCustomParams, Panic( EDiagFrameworkBadArgument ) );

    // Custom parameter can be set only when engine is not running.
    __ASSERT_ALWAYS( iStateMachine->CurrentState() == EStateNotReady, 
                     Panic( EDiagFrameworkInvalidState ) );

    iCustomParam = aCustomParams;
    }

// ---------------------------------------------------------------------------
// CDiagEngineImpl::SuspendL
// ---------------------------------------------------------------------------
//
void CDiagEngineImpl::SuspendL()
    {
    DiagFwInternal::TState state = iStateMachine->CurrentState();

    // Suspendable states:
    __ASSERT_ALWAYS( ( state == EStateRunning ||
                       state == EStateCreatingPlan ||
                       state == EStateFinalizing ||
                       state == EStateStopped ),
                     Panic( EDiagFrameworkInvalidState ) );

    if ( state == EStateFinalizing || state == EStateStopped )
        {
        // ignore suspend request. All tests are already completed and
        // we are just waiting to finalize db record and report final
        // result.
        return;
        }

    StopAllRequests();

    LOGSTRING( "CDiagEngineImpl::SuspendL(). Adding suspend event" )
    iStateMachine->AddEventL( EEventSuspend );
    }

// ---------------------------------------------------------------------------
// CDiagEngineImpl::ResumeL
// ---------------------------------------------------------------------------
//
void CDiagEngineImpl::ResumeL()
    {
    __ASSERT_ALWAYS( iStateMachine->CurrentState() == EStateSuspended,
                     Panic( EDiagFrameworkInvalidState ) );

    LOGSTRING( "CDiagEngineImpl::SuspendL(). Adding resume event" )
    AddResumeEventL( MDiagEngineObserver::EResumedByClient );
    }

// ---------------------------------------------------------------------------
// From class MDiagEngineCommon
// CDiagEngineImpl::ExecutionPlanL
// ---------------------------------------------------------------------------
//
const MDiagPluginExecPlan& CDiagEngineImpl::ExecutionPlanL() const
    {
    return *iPlan;
    }


// ---------------------------------------------------------------------------
// From class MDiagEngineCommon
// CDiagEngineImpl::ExecutionStopL
// ---------------------------------------------------------------------------
//
void CDiagEngineImpl::ExecutionStopL( TCancelMode aCancelMode )
    {
    DiagFwInternal::TState state = iStateMachine->CurrentState();

    // Acceptable states for execution stop.
    __ASSERT_ALWAYS( state == EStateCreatingPlan ||
                     state == EStateRunning ||
                     state == EStateSuspended ||
                     state == EStateFinalizing ||
                     state == EStateStopped,
                     Panic( EDiagFrameworkInvalidState ) );

    if ( state == EStateFinalizing || state == EStateStopped )
        {
        // ignore cancel request. All tests are already completed and
        // we are just waiting to finalize db record and report final
        // result.
        return;
        }

    StopAllRequests();
    
    if ( aCancelMode == ESkip )
        {
        iStateMachine->AddEventL( EEventSkip );
        }
    else
        {
        iStateMachine->AddEventL( EEventCancelAll );
        }
    }

// ---------------------------------------------------------------------------
// From class MDiagEngineCommon
// CDiagEngineImpl::ResetWatchdog
// ---------------------------------------------------------------------------
//
void CDiagEngineImpl::ResetWatchdog()
    {
    iPlan->CurrentExecutionItem().ResetWatchdog();
    }

// ---------------------------------------------------------------------------
// From class MDiagEngineCommon
// CDiagEngineImpl::ResetWatchdog
// ---------------------------------------------------------------------------
//
void CDiagEngineImpl::ResetWatchdog( TDiagEngineWatchdogTypes aWatchdogType )
    {
    iPlan->CurrentExecutionItem().ResetWatchdog( aWatchdogType );
    }

// ---------------------------------------------------------------------------
// From class MDiagEngineCommon
// CDiagEngineImpl::ResetWatchdog
// ---------------------------------------------------------------------------
//
void CDiagEngineImpl::ResetWatchdog( TTimeIntervalMicroSeconds32 aExpectedTimeToComplete )
    {
    iPlan->CurrentExecutionItem().ResetWatchdog( aExpectedTimeToComplete );
    }

// ---------------------------------------------------------------------------
// From class MDiagEngineCommon
// CDiagEngineImpl::ViewAppUi
// ---------------------------------------------------------------------------
//
CAknViewAppUi& CDiagEngineImpl::ViewAppUi()
    {
    return iViewAppUi;
    }

// ---------------------------------------------------------------------------
// From class MDiagEngineCommon
// CDiagEngineImpl::ViewAppUi
// ---------------------------------------------------------------------------
//
const CAknViewAppUi& CDiagEngineImpl::ViewAppUi() const
    {
    return iViewAppUi;
    }

// ---------------------------------------------------------------------------
// From class MDiagEngineCommon
// CDiagEngineImpl::DbRecord
// ---------------------------------------------------------------------------
//
RDiagResultsDatabaseRecord& CDiagEngineImpl::DbRecord()
    {
    return iDbRecord;
    }

// ---------------------------------------------------------------------------
// From class MDiagEngineCommon
// CDiagEngineImpl::DbRecord
// ---------------------------------------------------------------------------
//
const RDiagResultsDatabaseRecord& CDiagEngineImpl::DbRecord() const
    {
    return iDbRecord;
    }

// ---------------------------------------------------------------------------
// From class MDiagEngineCommon
// CDiagEngineImpl::PluginPool
// ---------------------------------------------------------------------------
//
CDiagPluginPool& CDiagEngineImpl::PluginPool()
    {
    return iPluginPool;
    }

// ---------------------------------------------------------------------------
// From class MDiagEngineCommon
// CDiagEngineImpl::PluginPool
// ---------------------------------------------------------------------------
//
const CDiagPluginPool& CDiagEngineImpl::PluginPool() const
    {
    return iPluginPool;
    }

// ---------------------------------------------------------------------------
// From class MDiagEngineCommon
// CDiagEngineImpl::AddToConfigListL
// ---------------------------------------------------------------------------
//
void CDiagEngineImpl::AddToConfigListL(
        MDiagEngineCommon::TConfigListType aListType, 
        const TDesC& aText )
    {
    switch ( aListType )
        {
        case MDiagEngineCommon::EConfigListCallIngore:
            iCallHandler->AddIgnoreNumberL( aText );
            break;

        default:
            LOGSTRING2( "CDiagEngineImpl::AddToConfigListL(). Invalid ListType %d",
                aListType )
            Panic( EDiagFrameworkBadArgument );
            break;
        }
    }

// ---------------------------------------------------------------------------
// From class MDiagEngineCommon
// CDiagEngineImpl::RemoveFromConfigListL
// ---------------------------------------------------------------------------
//
void CDiagEngineImpl::RemoveFromConfigListL(
        MDiagEngineCommon::TConfigListType aListType,
        const TDesC& aText )
    {
    switch ( aListType )
        {
        case MDiagEngineCommon::EConfigListCallIngore:
            iCallHandler->RemoveIgnoreNumberL( aText );
            break;

        default:
            LOGSTRING2( "CDiagEngineImpl::RemoveFromConfigListL(). Invalid ListType %d",
                aListType )
            Panic( EDiagFrameworkBadArgument );
            break;
        }
    }

// ---------------------------------------------------------------------------
// From class MDiagEngineCommon
// CDiagEngineImpl::CreateCommonDialogLC
// ---------------------------------------------------------------------------
//
CAknDialog* CDiagEngineImpl::CreateCommonDialogLC( TDiagCommonDialog aDialogType,
                                                   TAny* aInitData )
    {
    return iObserver.CreateCommonDialogLC( aDialogType, aInitData );
    }

// ---------------------------------------------------------------------------
// From class MDiagEngineCommon
// CDiagEngineImpl::ExecuteAppCommandL
// ---------------------------------------------------------------------------
//
void CDiagEngineImpl::ExecuteAppCommandL( TDiagAppCommand aCommand, 
                                          TAny* aParam1,
                                          TAny* aParam2 )
    {
    iObserver.ExecuteAppCommandL( aCommand, aParam1, aParam2 );
    }

// ---------------------------------------------------------------------------
// From class MDiagEngineCommon
// CDiagEngineImpl::StopWatchdogTemporarily
// ---------------------------------------------------------------------------
//
void CDiagEngineImpl::StopWatchdogTemporarily()
    {
    if ( iStateMachine == NULL || iPlan == NULL )
        {
        return;
        }

    if ( iStateMachine->CurrentState() == EStateRunning )
        {
        iPlan->CurrentExecutionItem().StopWatchdogTemporarily();
        }
    }

// ---------------------------------------------------------------------------
// From class MDiagEngineCommon
// CDiagEngineImpl::IsDependencyDisabled
// ---------------------------------------------------------------------------
//
TBool CDiagEngineImpl::IsDependencyDisabled() const
    {
    return iEngineConfig.IsDependencyDisabled();
    }

// ---------------------------------------------------------------------------
// From class MDiagEngineCommon
// CDiagEngineImpl::CustomParam
// ---------------------------------------------------------------------------
//
TAny* CDiagEngineImpl::CustomParam() const
    {
    return iCustomParam;
    }   //lint !e1763 Custom param is just passed along. Does not actually change engine.

// ---------------------------------------------------------------------------
// From class MDiagExecPlanEntryImplObserver
// CDiagEngineImpl::ExecPlanEntryProgressL
// ---------------------------------------------------------------------------
//
void CDiagEngineImpl::ExecPlanEntryProgressL( 
        CDiagExecPlanEntryImpl& aSender,
        TUint aCurrentStep, 
        TUint aTotalSteps )
    {
    LOGSTRING4( "CDiagEngineImpl::ExecPlanEntryProgressL: Plugin = 0x%08x, (%d / %d) ",
        aSender.Plugin().Uid().iUid,
        aCurrentStep, 
        aTotalSteps )

    if ( aSender.Plugin().Uid() == iPlan->CurrentExecutionItem().Plugin().Uid() )
        {
        CEventTestProgress* event = new ( ELeave ) CEventTestProgress(
                aSender.Plugin(), aCurrentStep, aTotalSteps );

        iStateMachine->AddEventL( event ); // ownership changed.
        }
    else
        {
        // probably timing issue. Ignore event.
        __ASSERT_DEBUG( 0, Panic( EDiagFrameworkInternal ) );
        }
    }

// ---------------------------------------------------------------------------
// From class MDiagExecPlanEntryImplObserver
// CDiagEngineImpl::ExecPlanEntryExecutedL
// ---------------------------------------------------------------------------
//
void CDiagEngineImpl::ExecPlanEntryExecutedL( CDiagExecPlanEntryImpl& aSender )
    {
    LOGSTRING2( "CDiagEngineImpl::ExecPlanEntryExecutedL: Plugin = 0x%08x",
        aSender.Plugin().Uid().iUid )

    if ( aSender.Plugin().Uid() != iPlan->CurrentExecutionItem().Plugin().Uid() )
        {
        // probably timing issue. Ignore event.
        __ASSERT_DEBUG( 0, Panic( EDiagFrameworkInternal ) );
        }
    else
        {
        TInt error = KErrNone;
        if ( aSender.IsStoppedByClient() )
            {
            error = KErrCancel;
            }

        CDiagResultsDatabaseItem* result = NULL;

        if ( aSender.Plugin().Type() == MDiagPlugin::ETypeTestPlugin )
            {
            // Test was completed. Get test result.
            CDiagExecPlanEntryImplTest& testEntry = 
                static_cast< CDiagExecPlanEntryImplTest& >( aSender );

            result = testEntry.GetLastTestResultL();

            __ASSERT_ALWAYS( result != NULL, Panic( EDiagFrameworkNullTestResult ) );
            }

        NotifyResultAndContinueL( error, result );
        }
    }

// ---------------------------------------------------------------------------
// From class MDiagExecPlanEntryImplObserver
// CDiagEngineImpl::ExecPlanEntryCriticalError
// ---------------------------------------------------------------------------
//
void CDiagEngineImpl::ExecPlanEntryCriticalError( TInt aError )
    {
    LOGSTRING2( "CDiagEngineImpl::ExecPlanEntryCriticalError: "
        L"Critical failure %d", aError )

    // Unrecoverable error has occered. e.g. out of memory, disk full etc.
    iStateMachine->HandleError( aError );
    }


// ---------------------------------------------------------------------------
// From class MDiagEngineStateMachineObserver
// CDiagEngineImpl::HandleStateChangedL
// ---------------------------------------------------------------------------
//
void CDiagEngineImpl::HandleStateChangedL( 
        TState aPreviousState, 
        TState aCurrentState,
        const CEventBasic& aEventPreview )
    {
    // NOTE: This method is called by state machine as soon as an event that
    // changes states is called. This means that it could execute within caller's
    // call stack. (e.g. it could be still within application or plug-in's 
    // call stack.) It is best to limit amount of implementation here to
    // things that must be done as soon as an event occurs.
    // 
    // Suspend and Resume are exceptions since they require immediate notification
    // to observer. Otherwise, observer might have mismatch in notifications.

    LOGSTRING3( "CDiagEngineImpl::HandleStateChangedL(): "
            L"Entering State : %d( %S )",
            aCurrentState, &iStateMachine->StateName( aCurrentState ) )

    switch ( aCurrentState )
        {
        case EStateSuspended:
            // keep track of state befor suspended, so that we can resume
            // to correct state.
            iSuspendedPrevState = aPreviousState;

            MDiagEngineObserver::TSuspendReason reason;
            if ( aEventPreview.GetType() == EEventSuspend )
                {
                reason = MDiagEngineObserver::ESuspendByClient;
                }
            else
                {
                reason = MDiagEngineObserver::ESuspendByPhoneCall;
                }

            // This will also notify iObserver.
            DoSuspendL( reason );
            break;

        case EStateRunning: 
            if ( aEventPreview.GetType() == EEventResumeToRunning )
                {
                LOGSTRING( "CDiagEngineImpl::HandleStateChangedL(). Notify resume" )
                iObserver.TestExecutionResumedL( iResumeReason );
                }
            break;

        case EStateCreatingPlan:
            if ( aEventPreview.GetType() == EEventResumeToCreatingPlan )
                {
                LOGSTRING( "CDiagEngineImpl::HandleStateChangedL(). Notify resume" )
                iObserver.TestExecutionResumedL( iResumeReason );
                }
            break;
        
        default:
            // Do nothing.
            break;
        }
    }

// ---------------------------------------------------------------------------
// From class MDiagEngineStateMachineObserver
// CDiagEngineImpl::HandleEventL
// ---------------------------------------------------------------------------
//
void CDiagEngineImpl::HandleEventL( CEventBasic& aEvent )
    {
    LOGSTRING5( "CDiagEngineImpl::HandleEventL(): State=%d(%S), Event=%d(%S)",
        iStateMachine->CurrentState(), 
        &iStateMachine->StateName( iStateMachine->CurrentState() ),
        aEvent.GetType(), &aEvent.ToString() )
            
    switch ( iStateMachine->CurrentState() )
        {
        case EStateNotReady:
            LOGSTRING( "CDiagEngineImpl::HandleEventL: "
                L"ERROR! Cannot accept events in EStateNotReady State" )
            __ASSERT_DEBUG( 0, Panic( EDiagFrameworkCorruptStateMachine ) );
            break;

        case EStateCreatingPlan:
            HandleEventInCreatingPlanStateL( aEvent );
            break;

        case EStateRunning:
            HandleEventInRunningStateL( aEvent );
            break;

        case EStateCancelAll:
            HandleEventInCancelAllStateL( aEvent );
            break;

        case EStateSuspended:
            HandleEventInSuspendedStateL( aEvent );
            break;

        case EStateFinalizing:
            HandleEventInFinalizingStateL( aEvent );
            break;

        case EStateStopped:
            HandleEventInStoppedStateL( aEvent );
            break;

        case EStateAny:     // fall through
        default:
            __ASSERT_DEBUG( 0, Panic( EDiagFrameworkCorruptStateMachine ) );
            break;
        }
    LOGSTRING( "CDiagEngineImpl::HandleEventL(): return" )
    }

// ---------------------------------------------------------------------------
// CDiagEngineImpl::HandleEventInCreatingPlanStateL
// ---------------------------------------------------------------------------
//
void CDiagEngineImpl::HandleEventInCreatingPlanStateL( CEventBasic& aEvent )
    {
    switch ( aEvent.GetType() )
        {
        case EEventExecute:                 // fall through
        case EEventResumeToCreatingPlan:
            StartCreateExecutionPlanL();
            break;
        
        default:
            LOGSTRING2( "CDiagEngineImpl::HandleEventInCreatingPlanStateL: "
                        L"Invalid event = %d", aEvent.GetType() )
            // Ignored.
            break;
        }
    }

// ---------------------------------------------------------------------------
// CDiagEngineImpl::HandleEventInRunningStateL
// ---------------------------------------------------------------------------
//
void CDiagEngineImpl::HandleEventInRunningStateL( CEventBasic& aEvent )
    {
    switch ( aEvent.GetType() )
        {
        case EEventPlanCreated:         
            // Normal case. Plan is created normally.
            HandlePlanCreatedL();
            break;
        
        case EEventExecuteNext:
            ExecuteNextPluginL();
            break;

        case EEventSkip:
            HandleSkipL();
            break;

        case EEventTestProgress:
            NotifyTestProgressL( static_cast<CEventTestProgress&>( aEvent ) );
            break;

        case EEventResumeToRunning:
            DoResumeL();
            break;

        default:
            LOGSTRING2( "CDiagEngineImpl::HandleEventInRunningStateL "
                        L"Invalid event = %d", aEvent.GetType() )
            break;
        }
    }

// ---------------------------------------------------------------------------
// CDiagEngineImpl::HandleEventInCancelAllStateL
// ---------------------------------------------------------------------------
//
void CDiagEngineImpl::HandleEventInCancelAllStateL( CEventBasic& aEvent )
    {
    switch ( aEvent.GetType() )
        {
        case EEventCancelAll:
        case EEventExecuteNext:
            HandleCancelAllL();
            break;

        case EEventSuspend:                 // fall through
        case EEventVoiceCallActive:
            // suspend is always handed in its own state
            __ASSERT_DEBUG( 0, Panic( EDiagFrameworkCorruptStateMachine ) );
            break;

        default:
            // other events are ingored( e.g. progress ), since
            // cancel has been requested
            LOGSTRING2( "CDiagEngineImpl::HandleEventInCancelAllStateL "
                        L"Invalid event = %d", aEvent.GetType() )
            break;
        }
    }

// ---------------------------------------------------------------------------
// CDiagEngineImpl::HandleEventInSuspendedStateL
// ---------------------------------------------------------------------------
//
void CDiagEngineImpl::HandleEventInSuspendedStateL( CEventBasic& aEvent )
    {
    switch ( aEvent.GetType() )
        {
        case EEventSuspend:
            // nothing to do. Suspend is immediate, so it is handled
            // in HandleStateChangedL 
            break;

        default:
            // suspended. ignore.
            LOGSTRING2( "CDiagEngineImpl::HandleEventInSuspendedStateL "
                        L"Invalid event = %d", aEvent.GetType() )
            break;
        }
    }

// ---------------------------------------------------------------------------
// CDiagEngineImpl::HandleEventInFinalizingStateL
// ---------------------------------------------------------------------------
//
void CDiagEngineImpl::HandleEventInFinalizingStateL( CEventBasic& aEvent )
    {
    switch ( aEvent.GetType() )
        {
        case EEventAllPluginsCompleted:
            FinalizeTestSessionL();
            break;

        default:
            // ignore all others. This is because if the state machine
            // gets here due to error, there may be left over events in
            // event queue. Those should be ignored.
            LOGSTRING2( "CDiagEngineImpl::HandleEventInFinalizingStateL "
                        L"Invalid event = %d", aEvent.GetType() )
            break;
        }
    }

// ---------------------------------------------------------------------------
// CDiagEngineImpl::HandleEventInStoppedStateL
// ---------------------------------------------------------------------------
//
void CDiagEngineImpl::HandleEventInStoppedStateL( CEventBasic& /* aEvent */ )
    {
    // ignore all events. This is because if the state machine
    // gets here due to error, there may be left over events in
    // event queue. Those should be ignored.
    }


// ---------------------------------------------------------------------------
// From class MDiagEngineStateMachineObserver
// CDiagEngineImpl::HandleError
// ---------------------------------------------------------------------------
//
TState CDiagEngineImpl::HandleError( TState aCurrentState, TInt aError )
    {
    LOGSTRING4( "CDiagEngineImpl::HandleError() State %d(%S), ERROR %d",
        aCurrentState,
        &iStateMachine->StateName( aCurrentState ),
        aError )

    switch ( aCurrentState )
        {
        case EStateNotReady:    // fall through.
        case EStateStopped:
            // was not running.. Nothing to do.
            return aCurrentState;

        default:
            {
            iEngineError = aError;

            // Database record is open. Suspend the record so that
            // user can retry later.
            iDbRecord.Suspend(); // error ignored

            // Ignore error from application in this case since we are already
            // handling error.
            TRAP_IGNORE( iObserver.TestExecutionCompletedL( aError ) )
            }
            return EStateStopped;
        }
    }

// ---------------------------------------------------------------------------
// CDiagEngineImpl::StartCreateExecutionPlanL
//
// ---------------------------------------------------------------------------
//
void CDiagEngineImpl::StartCreateExecutionPlanL()
    {
    LOGSTRING( "CDiagEngineImpl::StartCreateExecutionPlanL() Start creating plan" )
    if ( iContinueIncompleteRecord )
        {
        iPlan->InitializeL( iStatus );
        }
    else
        {
        iPlan->InitializeL( iStatus, iBatch );
        }
    SetActive();
    }

// ---------------------------------------------------------------------------
// CDiagEngineImpl::HandleEventExecute
//
// ---------------------------------------------------------------------------
//
void CDiagEngineImpl::HandlePlanCreatedL()
    {
    if ( iPlan->Count() == 0 )
        {
        // Error. There were no items in the plan.
        iEngineError = KErrArgument;
        iStateMachine->AddEventL( EEventAllPluginsCompleted );
        return;
        }

    // First initialize each plugins for execution.
    // Note that if initialization step fails, it will be caught by
    // CStateMachine's active object, and will call ::HandleError()
    LOGSTRING( "CDiagEngineImpl::HandlePlanCreatedL() : Phase I - Initaliaze all plugins" )
    for ( TInt i = 0; i < iPlan->Count(); i++ )
        {
        MDiagPlugin& plugin = (*iPlan)[i].Plugin();
        plugin.TestSessionBeginL( *this, iEngineConfig.IsDependencyDisabled(), iCustomParam );
        }

    LOGSTRING( "CDiagEngineImpl::HandlePlanCreatedL() : Phase II - Execution Begins" )
    iPlan->ResetExecutionCursor();
    iStateMachine->AddEventL( EEventExecuteNext );
    iObserver.TestExecutionBeginL();
    }

// ---------------------------------------------------------------------------
// CDiagEngineImpl::ExecuteNextPluginL
//
// ---------------------------------------------------------------------------
//
void CDiagEngineImpl::ExecuteNextPluginL()
    {
    // first, check if cursor needs to be advanced
    if ( iPlan->CurrentExecutionItem().State() == MDiagExecPlanEntry::EStateCompleted )
        {
        TBool moved = iPlan->MoveCursorToNext();
        if ( !moved )
            {
            // cursor should always move. This is because execute next event
            // should have never been created by NotifyResultAndContinueL
            __ASSERT_DEBUG( 0, Panic( EDiagFrameworkInternal ) );
            return;
            }
        }

    // Check if we are in call. If so, suspend current execution immediately.
    if ( iCallHandler->CurrentState() == EDiagEngineCallHandlerStateBusy )
        {
        LOGSTRING( "CDiagEngineImpl::ExecuteNextPluginL() Call in progress" )
        iStateMachine->AddEventL( EEventVoiceCallActive );
        return;
        }

    LOGSTRING2( "CDiagEngineImpl::ExecuteNextPluginL() : "
        L"Executing plugin 0x%08x", 
        iPlan->CurrentExecutionItem().Plugin().Uid().iUid )

    iPlan->CurrentExecutionItem().ExecuteL();
    }


// ---------------------------------------------------------------------------
// CDiagEngineImpl::NotifyTestProgressL
//
// ---------------------------------------------------------------------------
//
void CDiagEngineImpl::NotifyTestProgressL( CEventTestProgress& aEvent )
    {
    // make sure that we are reporting progress on currently executing test.
    CDiagExecPlanEntryImpl& currItem = iPlan->CurrentExecutionItem();

    if ( currItem.Plugin().Uid() == aEvent.Sender().Uid() &&
         currItem.State() != MDiagExecPlanEntry::EStateCompleted )
        {
        LOGSTRING3( "CDiagEngineImpl::NotifyTestProgressL. "
            L"Calling TestExecutionProgressL( %d, %d )", 
            aEvent.CurrStep(), 
            aEvent.TotalSteps() )
        iObserver.TestExecutionProgressL( aEvent.CurrStep(), aEvent.TotalSteps() );
        }
    }

// ---------------------------------------------------------------------------
// CDiagEngineImpl::NotifyResultAndContinueL
//
// ---------------------------------------------------------------------------
//
void CDiagEngineImpl::NotifyResultAndContinueL( 
        TInt aError,
        CDiagResultsDatabaseItem* aResult )
    {
    // aResult can be NULL if it was executing suite.
    if ( aResult )
        {
        CleanupStack::PushL( aResult );
        }
    
    if ( aResult )
        {
        CleanupStack::Pop( aResult );
        }

    iObserver.TestExecutionPluginExecutedL( aError, aResult ); 
    aResult = NULL; // aResult ownership transferred above.

    if ( iPlan->IsLastPlugin() )
        {
        User::After(2000000);
        iStateMachine->AddEventL( EEventAllPluginsCompleted );
        }
    else
        {
        iStateMachine->AddEventL( EEventExecuteNext );
        }
    }

// ---------------------------------------------------------------------------
// CDiagEngineImpl::HandleSkipL
//
// ---------------------------------------------------------------------------
//
void CDiagEngineImpl::HandleSkipL()
    {
    // if plan was empty, ( e.g. before any execution has actually started.
    // do nothing.
    if ( iPlan->Count() == 0 )
        {
        iEngineError = KErrArgument;
        iStateMachine->AddEventL( EEventAllPluginsCompleted );
        return;
        }

    // Stop execution.  While plug-ins are required to stop immediately,
    // writing to results db is async. Completion is notified is
    // ExecPlanEntryExecutedL()
    iPlan->CurrentExecutionItem().StopExecutionByClientL( ESkip );
    }

// ---------------------------------------------------------------------------
// CDiagEngineImpl::HandleCancelAllL
//
// ---------------------------------------------------------------------------
//
void CDiagEngineImpl::HandleCancelAllL()
    {
    iEngineError = KErrCancel;

    if ( iPlan->Count() == 0 )
        {
        // If plan was empty, ( e.g. before any execution has actually started ),
        // do nothing and send back completed with KErrCancel.
        iStateMachine->AddEventL( EEventAllPluginsCompleted );
        return;
        }

    if ( iPlan->CurrentExecutionItem().State() == MDiagExecPlanEntry::EStateCompleted )
        {
        TBool moved = iPlan->MoveCursorToNext();
        if ( !moved )
            {
            // cursor should always move. This is because execute next event
            // should have never been created by NotifyResultAndContinueL
            __ASSERT_DEBUG( 0, Panic( EDiagFrameworkInternal ) );
            return;
            }
        }

    iPlan->CurrentExecutionItem().StopExecutionByClientL( ECancelAll );
    // Continue to ::ExecPlanEntryExecutedL
    }

// ---------------------------------------------------------------------------
// CDiagEngineImpl::DoSuspendL
//
// ---------------------------------------------------------------------------
//
void CDiagEngineImpl::DoSuspendL( MDiagEngineObserver::TSuspendReason aReason )
    {
    if ( iSuspendedPrevState == EStateNotReady ||
         iSuspendedPrevState == EStateCreatingPlan ||
         iSuspendedPrevState == EStateStopped )
        {
        // not much to do here.
        }
    else
        {
        StopAllRequests();

        iSuspendReason = aReason;

        iPlan->CurrentExecutionItem().SuspendL();
        }

    LOGSTRING( "CDiagEngineImpl::DoSuspendL: Engine Suspended" )
    iObserver.TestExecutionSuspendedL( aReason );
    }

// ---------------------------------------------------------------------------
// CDiagEngineImpl::AddResumeEventL
//
// ---------------------------------------------------------------------------
//
void CDiagEngineImpl::AddResumeEventL( MDiagEngineObserver::TResumeReason aReason )
    {
    iResumeReason = aReason;
    if ( iSuspendedPrevState == EStateNotReady ||
         iSuspendedPrevState == EStateCreatingPlan )
        {
        iStateMachine->AddEventL( EEventResumeToCreatingPlan );
        }
    else
        {
        iStateMachine->AddEventL( EEventResumeToRunning );
        }
    }

// ---------------------------------------------------------------------------
// CDiagEngineImpl::DoResumeL
//
// ---------------------------------------------------------------------------
//
void CDiagEngineImpl::DoResumeL()
    {
    LOGSTRING( "CDiagEngineImpl::DoResumeL: Resuming Engine..." )
    CDiagExecPlanEntryImpl& entry = iPlan->CurrentExecutionItem();

    if ( entry.State() == MDiagExecPlanEntry::EStateCompleted )
        {
        // already completed. nothing to resume.
        ExecuteNextPluginL();
        }
    else
        {
        entry.ResumeL();
        }
    }


// ---------------------------------------------------------------------------
// CDiagEngineImpl::CreateDbItem
//
// ---------------------------------------------------------------------------
//
CDiagResultsDatabaseItem* CDiagEngineImpl::CreateDbItemL(
        CDiagExecPlanEntryImpl& aCurrItem,
        CDiagResultsDatabaseItem::TResult aResultType ) const
    {
    return CDiagResultsDbItemBuilder::CreateSimpleDbItemL( aCurrItem.Plugin().Uid(),
                                                           aCurrItem.AsDependency(),
                                                           aResultType );
    }

// ---------------------------------------------------------------------------
// CDiagEngineImpl::NotifyPluginsOfTestSessionEnd
//
// ---------------------------------------------------------------------------
//
void CDiagEngineImpl::NotifyPluginsOfTestSessionEnd()
    {
    LOGSTRING( "CDiagEngineImpl::NotifyPluginsOfTestSessionEnd() : "
        L"Phase III - Cleaning up stage" )
    // unlike initialization stage, it will TRAP all errors so that 
    // every plug-ins will have a chance to run its clean up code.
    for ( TInt index = 0; index < iPlan->Count(); index++ )
        {
        MDiagPlugin& plugin = (*iPlan)[index].Plugin();
        TRAPD( err, plugin.TestSessionEndL( 
            *this, iEngineConfig.IsDependencyDisabled(), iCustomParam ) );
        if ( err != KErrNone )
            {
            LOGSTRING3( "CDiagEngineImpl::NotifyPluginsOfTestSessionEnd(): "
                L"Plug-in Uid %d CleanupL failed with error %d", 
                plugin.Uid().iUid, err )
            }
        }
    }

// ---------------------------------------------------------------------------
// CDiagEngineImpl::StopAllRequests
//
// ---------------------------------------------------------------------------
//
void CDiagEngineImpl::StopAllRequests()
    {
    Cancel();
    StopWatchdogTemporarily();
    }

// ---------------------------------------------------------------------------
// CDiagEngineImpl::FinalizeTestSessionL
//
// ---------------------------------------------------------------------------
//
void CDiagEngineImpl::FinalizeTestSessionL()
    {
    LOGSTRING( "CDiagEngineImpl::FinalizeTestSessionL() " )
    
    // This causes state to change to EStateStopped
    //iStateMachine->AddEventL( EEventFinalized );

    // Call TestSessionEnd on all plug-ins
    NotifyPluginsOfTestSessionEnd();

    // Mark DB completed.
    TBool isFullyCompleted = ( iEngineError == KErrNone );
    
    TInt err = iDbRecord.TestCompleted( isFullyCompleted );
    if ( err != KErrNone )
        {
        LOGSTRING3( "CDiagEngineImpl::FinalizeTestSessionL() "
            L"iDbRecord.TestCompleted( %d ) return err %d", 
            isFullyCompleted, 
            err )
        iEngineError = err;
        }
    
    // Let observer know that test is completed.
    iObserver.TestExecutionCompletedL( iEngineError );
    }

// ---------------------------------------------------------------------------
// From class MDiagEngineCallHandlerObserver
// CDiagEngineImpl::CallHandlerStateChangedL
// ---------------------------------------------------------------------------
//
void CDiagEngineImpl::CallHandlerStateChangedL( TDiagEngineCallHandlerState aState )
    {
    if ( aState == EDiagEngineCallHandlerStateBusy && 
            ( iStateMachine->CurrentState() == EStateRunning) )
        {
        iStateMachine->AddEventL( EEventVoiceCallActive );
        }
    else if ( aState == EDiagEngineCallHandlerStateIdle && 
            iStateMachine->CurrentState() == EStateSuspended  &&
            iSuspendReason == MDiagEngineObserver::ESuspendByPhoneCall )
        {
        AddResumeEventL( MDiagEngineObserver::EAutoResumedByCallHangup );
        }
    else
        {
        // Ignored
        }
    }


// ---------------------------------------------------------------------------
// From class CActive
// CDiagEngineImpl::RunL
// ---------------------------------------------------------------------------
//
void CDiagEngineImpl::RunL()
    {
    LOGSTRING2( "CDiagEngineImpl::RunL() error %d", iStatus.Int() )
    
    User::LeaveIfError( iStatus.Int() );
    
    switch ( iStateMachine->CurrentState()  )
        {
        case EStateCreatingPlan:
            // plan created successfully.
            iStateMachine->AddEventL( EEventPlanCreated );
            break;

        default:
            __ASSERT_DEBUG( 0, Panic( EDiagFrameworkCorruptStateMachine ) );
            break;
        }
    }

// ---------------------------------------------------------------------------
// From class CActive
// CDiagEngineImpl::DoCancel
// ---------------------------------------------------------------------------
//
void CDiagEngineImpl::DoCancel()
    {
    switch ( iStateMachine->CurrentState() )
        {
        case EStateCreatingPlan:
            iPlan->Cancel();
            break;

        default:
            // Nothing to do
            break;
        }
    }

// ---------------------------------------------------------------------------
// From class CActive
// CDiagEngineImpl::RunError
// ---------------------------------------------------------------------------
//
TInt CDiagEngineImpl::RunError( TInt aError )
    {
    if ( iStateMachine )
        {
        iStateMachine->HandleError( aError );
        }

    return KErrNone;
    }

// End of File

