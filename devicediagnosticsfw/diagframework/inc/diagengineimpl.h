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
* Description:  Class declaration for CDiagEngineImpl
*
*/


#ifndef DIAGENGINEIMPL_H
#define DIAGENGINEIMPL_H


// system includes
#include <e32base.h>                    // CActive
#include <DiagEngineCommon.h>           // MDiagEngineCommon
#include <DiagEngineObserver.h>         // MDiagEngineObserver::TSuspendReason
#include <DiagResultsDatabase.h>        // RDiagResultsDatabase, RDiagResultsDatabaseRecord
#include <DiagTestPlugin.h>             // MDiagTestPlugin::TStopReason
#include <DiagSuitePlugin.h>            // MDiagSuitePlugin::TStopReason

// user includes.
#include "diagenginestatemachineobserver.h"     // DiagFwInternal::MStateMachineObserver
#include "diagenginestates.h"                   // DiagFwInternal::TState
#include "diagengineevents.h"                   // DiagFwInternal::TEvent
#include "diagenginecallhandlerobserver.h"      // MDiagEngineCallHandlerObserver
#include "diagexecplanentryimplobserver.h"      // MDiagExecPlanEntryImplObserver
#include "diagengineconfig.h"                   // TDiagEngineConfig


// Forward declarations
class MDiagTestPlugin;
class MDiagSuitePlugin;
class CDiagPluginPool;
class CAknViewAppUi;
class CDiagEngineCallHandler;
class CDiagPluginExecPlanImpl;
class CDiagExecPlanEntryImpl;
class CDiagExecPlanEntryImplTest;

namespace DiagFwInternal
    {
    class CStateMachine;
    class CEventBasic;
    class CEventCancel;
    class CEventSendCompletion;
    class CEventSuiteContinue;
    class CEventTestCompleted;
    class CEventTestProgress;
    class CEventWatchdog;
    }

/**
*  Diagnostics Framework Engine Implementation
*
*  This class is the actual class that implements CDiagEngine.
*  All actual work is done by CDiagEngineImpl class.
*
*/
NONSHARABLE_CLASS( CDiagEngineImpl ) : public CActive,
                                       public MDiagEngineCommon,
                                       public MDiagExecPlanEntryImplObserver,
                                       public DiagFwInternal::MStateMachineObserver,
                                       public MDiagEngineCallHandlerObserver
    {
public:

    /**
    * Two-phased constructor.
    * @param aViewAppUi    View App Ui that is using this engine.
    * @param aObserver     Engine observer.
    * @param aDbSession    Session to RDiagResultDatabase to create record.
    * @param aPluginPool   Initialized plug-in pool that holds plug-ins.
    * @param aDisableDependency    if ETrue, plug-ins should execute
    *                      event if dependencies are not met.
    * @param aExecutionBatch - array of test uids to execute
    *
    * @return new instance of CDiagEngineImpl*
    */
    static CDiagEngineImpl* NewL( CAknViewAppUi&          aViewAppUi, 
                                  MDiagEngineObserver&    aObserver,
                                  RDiagResultsDatabase&   aDbSession,
                                  CDiagPluginPool&        aPluginPool,
                                  TBool                   aDisableDependency,
                                  const RArray< TUid >&   aExecutionBatch );

    /**
    * Two-phased constructor. This function is identical to NewL, 
    * except that it pushs created object in clean up stack.
    *
    * @param aViewAppUi    View App Ui that is using this engine.
    * @param aObserver     Engine observer.
    * @param aDbSession    Session to RDiagResultDatabase to create record.
    * @param aPluginPool   Initialized plug-in pool that holds plug-ins.
    * @param aDisableDependency    if ETrue, plug-ins should execute
    *                      event if dependencies are not met.
    * @param aExecutionBatch - array of test uids to execute
    *
    * @return new instance of CDiagEngineImpl*
    */
    static CDiagEngineImpl* NewLC( CAknViewAppUi&          aViewAppUi, 
                                   MDiagEngineObserver&    aObserver,
                                   RDiagResultsDatabase&   aDbSession,
                                   CDiagPluginPool&        aPluginPool,
                                   TBool                   aDisableDependency,
                                   const RArray< TUid >&   aExecutionBatch );
    

    /**
    * Two-phased constructor.
    * @param aViewAppUi    View App Ui that is using this engine.
    * @param aObserver     Engine observer.
    * @param aDbSession    Session to RDiagResultDatabase to create record.
    * @param aPluginPool   Initialized plug-in pool that holds plug-ins.
    * @param aIncompleteRecordUid - Uid to the record to resume.
    *
    * @return new instance of CDiagEngineImpl*
    */
    static CDiagEngineImpl* NewL( CAknViewAppUi&          aViewAppUi, 
                                  MDiagEngineObserver&    aObserver,
                                  RDiagResultsDatabase&   aDbSession,
                                  CDiagPluginPool&        aPluginPool,
                                  TUid                    aIncompleteRecordUid );
    /**
    * Two-phased constructor. This function is identical to NewL, 
    * except that it pushs created object in clean up stack.
    *
    * @param aViewAppUi    View App Ui that is using this engine.
    * @param aObserver     Engine observer.
    * @param aDbSession    Session to RDiagResultDatabase to create record.
    * @param aPluginPool   Initialized plug-in pool that holds plug-ins.
    * @param aIncompleteRecordUid - Uid to the record to resume.
    *
    * @return new instance of CDiagEngineImpl*
    */
    static CDiagEngineImpl* NewLC( CAknViewAppUi&          aViewAppUi, 
                                   MDiagEngineObserver&    aObserver,
                                   RDiagResultsDatabase&   aDbSession,
                                   CDiagPluginPool&        aPluginPool,
                                   TUid                    aIncompleteRecordUid );
    

    /**
    * Destructor.
    */
    virtual ~CDiagEngineImpl();

    /**
    * Execute tests. This implements CDiagEngine::ExecuteL
    *
    * @see CDiagEngine::ExecuteL
    */
    void ExecuteL();

    /**
    * Set custom execution parameter.
    *
    * @see CDiagEngine::SetCustomParam
    */
    void SetCustomParam( TAny* aCustomParams );

    /**
    * Suspend execution.  
    * 
    * @see CDiagEngine::SuspendL
    */
    void SuspendL();

    /**
    * Resume suspended execution. This implements CDiagEngine::ResumeL
    *
    * @see CDiagEngine::ResumeL
    */
    void ResumeL();

public: // from MDiagEngineCommon
    /**
    * Returns current execution plan.
    * 
    * @see MDiagEngineCommon::ExecutionPlanL
    */
    virtual const MDiagPluginExecPlan& ExecutionPlanL() const;

    /**
    * Stop current execution of tests.
    * 
    * @see MDiagEngineCommon::ExecutionStopL
    */
    virtual void ExecutionStopL( TCancelMode aCancelMode );
    
    /**
    * Resets test execution watchdog timer.
    * 
    * @see MDiagEngineCommon::ResetWatchdog
    */
    virtual void ResetWatchdog();
    
    /**
    * Resets test execution watchdog timer.
    * @see MDiagEngineCommon::ResetWatchdog
    */
    virtual void ResetWatchdog( TDiagEngineWatchdogTypes aWatchdogType );
    
    /**
    * Resets test execution watchdog timer.
    * @see MDiagEngineCommon::ResetWatchdog
    */
    virtual void ResetWatchdog( TTimeIntervalMicroSeconds32 aExpectedTimeToComplete );
    
    /**
    * Get App Ui reference that launched Diagnostics Engine.
    * 
    * @see MDiagEngineCommon::ViewAppUi()
    */
    virtual CAknViewAppUi& ViewAppUi();
    
    /**
    * Get App Ui reference that launched Diagnostics Engine.
    * 
    * @see MDiagEngineCommon::ViewAppUi()
    */
    virtual const CAknViewAppUi& ViewAppUi() const;

    /**
    * Get Database Subsession for record open for writing.
    * 
    * @see MDiagEngineCommon::DbRecord
    */
    virtual RDiagResultsDatabaseRecord& DbRecord();

    /**
    * Get Database Subsession for record open for writing.
    * 
    * @see MDiagEngineCommon::DbRecord
    */
    virtual const RDiagResultsDatabaseRecord& DbRecord() const;

    /**
    * Get reference to Plugin Pool
    * 
    * @see MDiagEngineCommon::PluginPool
    */
    virtual CDiagPluginPool& PluginPool();

    /**
    * Get reference to Plugin Pool
    * 
    * @see MDiagEngineCommon::PluginPool
    */
    virtual const CDiagPluginPool& PluginPool() const;

    /**
    * Add a text to configuraiton list option. 
    * 
    * @see MDiagEngineCommon::AddToConfigListL
    */
    virtual void AddToConfigListL( TConfigListType aListType, const TDesC& aText );

    /**
    * Remove a text from configuration list.
    * 
    * @see MDiagEngineCommon::RemoveFromConfigListL
    */
    virtual void RemoveFromConfigListL( TConfigListType aListType, const TDesC& aText );

    /**
    * Create a common dialog.
    * @see MDiagEngineCommon::CreateCommonDialogL
    */
    virtual CAknDialog* CreateCommonDialogLC( TDiagCommonDialog aDialogType,
                                              TAny* aInitData );

    /**
    * Stop watchdog activity temporarily.
    * @see MDiagEngineCommon::StopWatchdogTemporarily
    */
    virtual void StopWatchdogTemporarily();

    /**
    * Check if current engine is being executed with dependency disabled.
    * @see MDiagEngineCommon::IsDependencyDisabled
    */
    virtual TBool IsDependencyDisabled() const;

    /**
    * Get custom execution parameter.
    * @see MDiagEngineCommon::CustomParam
    */
    virtual TAny* CustomParam() const;

    /**
    * Send a command to application.
    *   @see MDiagEngineCommon::ExecuteAppCommandL
    */
    virtual void ExecuteAppCommandL( TDiagAppCommand aCommand, 
                                     TAny* aParam1,
                                     TAny* aParam2 );

private: // from MDiagExecPlanEntryImplObserver
    /**
    * Observe plugin execution progress
    * @see MDiagExecPlanEntryImplObserver::ExecPlanEntryProgressL
    */
    virtual void ExecPlanEntryProgressL( CDiagExecPlanEntryImpl& aSender,
                                         TUint aCurrentStep,
                                         TUint aTotalSteps );
    /**
    * Observe plugin execution completion
    * @see MDiagExecPlanEntryImplObserver::ExecPlanEntryExecutedL
    */
    virtual void ExecPlanEntryExecutedL( CDiagExecPlanEntryImpl& aSender );

    /**
    * Observer critical error from plug-in execution entry.
    * @see MDiagExecPlanEntryImplObserver::ExecPlanEntryCriticalError
    */
    virtual void ExecPlanEntryCriticalError( TInt aError );

private: // from DiagFwInternal::MStateMachineObserver
    /**
    * Handle state transition
    * 
    * @see MDiagEngineStateMachineObserver::HandleStateChangedL
    */
    virtual void HandleStateChangedL( DiagFwInternal::TState aPreviousState, 
                                      DiagFwInternal::TState aCurrentState,
                                      const DiagFwInternal::CEventBasic& aEventPreview );

    /**
    * Handle event.
    * 
    * @see MDiagEngineStateMachineObserver::HandleEventL
    */
    virtual void HandleEventL( DiagFwInternal::CEventBasic& aEvent );

    /**
    * Handle error.
    * 
    * @see MDiagEngineStateMachineObserver::HandleError
    */
    virtual DiagFwInternal::TState HandleError( DiagFwInternal::TState aCurrentState, 
                                                TInt aError );

private:    // From MDiagEngineCallHandlerObserver
    /**
    * Handle call handling state change.
    * 
    * @see MDiagEngineCallHandlerObserver::CallHandlerStateChangedL
    */
    virtual void CallHandlerStateChangedL( TDiagEngineCallHandlerState aState );

private:    // from CActive
    /**
    * RunL from CActive
    * 
    * @see CActive::RunL
    */
    virtual void RunL();

    /**
    * DoCancel from CActive
    * 
    * @see CActive::DoCancel
    */
    virtual void DoCancel();

    /**
    * Handle error 
    * 
    * @see CActive::RunError
    */
    virtual TInt RunError( TInt aError );

private:    // Private Constructors
    /**
    * C++ Constrctor
    *
    * @param aViewAppUi - Reference to Appui.
    * @param aObserver - Reference to engine observer. (Generall app)
    * @param aDbSession - Session to the database.
    * @param aPluginPool - Reference to plug-in pool.
    * @param aDisableDependency - Whether to disable dependency check.
    * @param aIncompleteRecordUid - Uid to the record to resume.
    *   If this UID is aIncompleteRecordUid == TUid::Null(), it will create
    *   a new record. 
    */
    CDiagEngineImpl( CAknViewAppUi&          aViewAppUi,
                     MDiagEngineObserver&    aObserver,
                     RDiagResultsDatabase&   aDbSession,
                     CDiagPluginPool&        aPluginPool );
    
    /**
    * Second phase constructor for creating a new record.
    *
    * @param aDisableDependency - Whether to disable dependency check
    * @param aExecutionBatch - Array of plug-in uids to execute
    */
    void ConstructNewRecordL( TBool aDisableDependency, 
                              const RArray< TUid >& aExecutionBatch );

    /**
    * Second phase constructor for resuming incomplete record.
    * 
    * @param aIncompleteRecordUid - Uid of DB record to resume from.
    */
    void ConstructIncompleteRecordL( TUid aIncompleteRecordUid );
    
    /**
    * Second phase constructor common.
    * Initializes items that are common to both new session and resuming session.
    */
    void ConstructCommonL();

private:    // Event handlers
    /**
    * State handler function for EStateCreatingPlan
    *
    * @param aEvent - Event to handle.
    */
    void HandleEventInCreatingPlanStateL( DiagFwInternal::CEventBasic& aEvent );

    /**
    * State handler function for EStateRunning
    *
    * @param aEvent - Event to handle.
    */
    void HandleEventInRunningStateL( DiagFwInternal::CEventBasic& aEvent );

    /**
    * State handler function for EStateCancelAll
    *
    * @param aEvent - Event to handle.
    */
    void HandleEventInCancelAllStateL( DiagFwInternal::CEventBasic& aEvent );

    /**
    * State handler function for EStateSuspended
    *
    * @param aEvent - Event to handle.
    */
    void HandleEventInSuspendedStateL( DiagFwInternal::CEventBasic& aEvent );

    /**
    * State handler function for EStateFinalizing
    *
    * @param aEvent - Event to handle.
    */
    void HandleEventInFinalizingStateL( DiagFwInternal::CEventBasic& aEvent );

    /**
    * State handler function for EStateStopped
    *
    * @param aEvent - Event to handle.
    */
    void HandleEventInStoppedStateL( DiagFwInternal::CEventBasic& aEvent );

private:    // Other private functions
    /**
    * Start creating execution plan
    */
    void StartCreateExecutionPlanL();

    /**
    * Handle Plan created event
    */
    void HandlePlanCreatedL();

    /**
    * Notify test progress event to engine observer
    *
    * @param aEvent - test progress event.
    */
    void NotifyTestProgressL( DiagFwInternal::CEventTestProgress& aEvent );

    /**
    * Report plug-in execution result to observer and continue to the next test.
    *
    * @param aError - Error code to report to observer.
    * @param aResult - result to report to observer.
    */
    void NotifyResultAndContinueL( TInt aError, CDiagResultsDatabaseItem* aResult );

    /**
    * Commit db record to DB. 
    *
    */
    void CommitResultsToDb();

    /**
    * Notify observer that last plugin is completed and engine is now stopped.
    * This could also mean that execution has failed.
    *
    * @param aEvent - last completion event
    */
    void NotifyLastPluginCompletedL( DiagFwInternal::CEventSendCompletion& aEvent );

    /**
    * Execute next plugin in the plan
    *
    */
    void ExecuteNextPluginL();

    /**
    * Suspend engine
    * 
    * @param aReason - Reason for suspending.
    */
    void DoSuspendL( MDiagEngineObserver::TSuspendReason aReason );

    /**
    * Add event to state machine.
    *   This functions makes sure that correct resume event is added to 
    *   the state machine depending on the previous state.
    * @param aReason - Resume reason.
    */
    void AddResumeEventL( MDiagEngineObserver::TResumeReason aReason );

    /**
    * Resume engine
    *
    */
    void DoResumeL();

    /**
    * Handle Skip
    *
    */
    void HandleSkipL();

    /**
    * Handle cancelling of all plug-ins
    *
    */
    void HandleCancelAllL();

    /**
    * Start delay timer for test plugin
    *
    * @param aPlanEntry - test plugin entry in plan to execute.
    */
    void StartTimerForDelayingTestL( CDiagExecPlanEntryImpl& aPlanEntry );

    /**
    * Start Executing a test plug-in
    *
    * @param aPlanEntry - test plugin entry in plan to execute.
    */
    void DoExecuteTestPluginL( CDiagExecPlanEntryImpl& aPlanEntry );

    /**
    * Start Executing a suite plug-in
    *
    * @param aPlanEntry - suite plugin entry in plan to execute.
    */
    void DoExecuteSuitePluginL( CDiagExecPlanEntryImpl& aPlanEntry );

    /**
    * Callback function for plugin delay timer expiration.
    *
    * @param aPtr - pointer to "this"
    */
    static TInt PluginDelayTimerExpiredL( TAny* aPtr );

    /**
    * Callback function for watchdog timer expiration. 
    *
    * @param aPtr - pointer to "this"
    */
    static TInt WatchdogTimerExpiredL( TAny* aPtr );

    /**
    * Handle watchdog time expiration. 
    *
    * @param aEvent - Watchdog event.
    */
    void HandleWatchdogTimeoutL( DiagFwInternal::CEventWatchdog& aEvent );

    /**
    * Function to stop currently executing plugin
    *
    * @param aStopMode - Reason for stopping the execution.
    * @param aIsWatchdog - ETrue if this is because of watchdog timeout.
    * @return Test Result. NULL if plug-in was a suite. Ownership transferred
    *   to caller.
    */
    CDiagResultsDatabaseItem* StopCurrentPluginL( TCancelMode aStopMode, 
                                                  TBool aIsWatchdog );

    /**
    * Function to stop currently executing test plugin
    *
    * @param aStopMode - Reason for stopping the execution.
    * @param aIsWatchdog - ETrue if this is because of watchdog timeout.
    * @return Test Result. NULL if plug-in was a suite. Ownership transferred
    *   to caller.
    */
    CDiagResultsDatabaseItem* StopCurrentTestPluginL( TCancelMode aStopMode, 
                                                      TBool aIsWatchdog );

    /**
    * Function to stop currently executing suite plugin
    *
    * @param aStopMode - Reason for stopping the execution.
    * @param aIsWatchdog - ETrue if this is because of watchdog timeout.
    */
    void StopCurrentSuitePluginL( TCancelMode aStopMode, TBool aIsWatchdog );

    /**
    * Function to create test stop reason based on input parameter
    *
    * @param aStopMode - Reason for stopping the execution.
    * @param aIsWatchdog - ETrue if this is because of watchdog timeout.
    * @return Test stop reason
    */
    MDiagTestPlugin::TStopReason TranslateTestStopReason( TCancelMode aStopMode, 
                                                          TBool aIsWatchdog );
        
    /**
    * Function to create suite stop reason based on input parameter
    *
    * @param aStopMode - Reason for stopping the execution.
    * @param aIsWatchdog - ETrue if this is because of watchdog timeout.
    * @return Suite stop reason
    */
    MDiagSuitePlugin::TStopReason TranslateSuiteStopReason( TCancelMode aStopMode, 
                                                            TBool aIsWatchdog );
        
    /**
    * Function to create database result type based on input parameter
    *
    * @param aStopMode - Reason for stopping the execution.
    * @param aIsWatchdog - ETrue if this is because of watchdog timeout.
    * @return Database result type.
    */
    CDiagResultsDatabaseItem::TResult ConvertToDbResult( TCancelMode aStopMode, 
                                                         TBool aIsWatchdog );     
        
    /**
    * Create database result item with just result.
    *
    * @param aCurrentItem - Currently executing item
    * @param aResult - Result type
    * @return Database result item. Ownership transferred to caller.
    */
    CDiagResultsDatabaseItem* CreateDbItemL( 
            CDiagExecPlanEntryImpl& aCurrentItem,
            CDiagResultsDatabaseItem::TResult aResultType ) const;
        
    /**
    * Call TestSessionEndL on all plug-ins.
    *
    */
    void NotifyPluginsOfTestSessionEnd();

    /**
    * Stops all active requests
    */
    void StopAllRequests();

    /**
    * Cleans up incomplete test session during engine destruction.
    * This function should be called only from destructor.
    */
    void CleanupIncompleteTestSession();

    /**
    * Finalizes test session. This will notify plug-ins, finalize DB record and
    * notify application.
    */
    void FinalizeTestSessionL();

private:    // private data
    /**
    * iViewAppUi - Reference to View App Ui. Passed to plug-ins to allow
    * view switching.
    */
    CAknViewAppUi& iViewAppUi;

    /**
    * iObserver - reference to engine observer.
    */
    MDiagEngineObserver& iObserver;

    /**
    * iDbSession - Database session.
    */
    RDiagResultsDatabase& iDbSession;

    /**
    * iDbRecord - New record created for current test session.
    */
    RDiagResultsDatabaseRecord iDbRecord;

    /**
    * iRecordId - New record id created for current test session.
    */
    TUid iRecordId;

    /**
    * iPluginPool - Reference to the plug-in pool.
    */
    CDiagPluginPool& iPluginPool;

    /**
    * iCustomParam - Custom parameter.
    * Ownership - Client.
    */
    TAny* iCustomParam;

    /**
    * iBatch - list of items to execute.
    * Ownership - Individual elements are owned by plug-in pool. 
    */
    RPointerArray< MDiagPlugin > iBatch;

    /**
    * iWatchdogTimer - Timer for watchdog.
    * Ownership - this
    */
    CPeriodic* iWatchdogTimer;

    /**
    * iLastWatchdogValue - Last watchdog value set.
    *   This will be used when ResetWatchdog() is called without
    *   any value specified.
    */
    TInt iLastWatchdogValue;

    /**
    * iPlan - Execution plan
    * Ownership - this
    */
    CDiagPluginExecPlanImpl* iPlan;

    /**
    * iStateMachine - State machine.
    * Ownership - this
    */
    DiagFwInternal::CStateMachine* iStateMachine;

    /**
    * iSuspendedPrevState - previous state just before suspended.
    * This is used to resume to correct state.
    */
    DiagFwInternal::TState iSuspendedPrevState;

    /**
    * iSuspendReason - Reason why engine was suspended.
    */
    MDiagEngineObserver::TSuspendReason iSuspendReason;

    /**
    * iResumeReason - Reason why engine is resumed.
    */
    MDiagEngineObserver::TResumeReason iResumeReason;

    /**
    * iSuspendedResult - Temporary result from test plug-in when engine is 
    * suspended. When test is suspended just before being able to save the
    * test result, its result is saved here, so that when resumed
    * it will use this result instead of re-executing.
    * Ownership - this
    */
    CDiagResultsDatabaseItem* iSuspendedResult;
    
    /**
    * iCallHandler - Handles call states
    * Ownership - this
    */
    CDiagEngineCallHandler* iCallHandler;
    
    /**
    * iContinueIncompleteRecord - Indicates whether engine is supposed to
    *   continue from an incomplete record.
    *   ETrue if engine is working with incomplete record.
    *   EFalse if engine is working with a new record.
    */
    TBool iContinueIncompleteRecord;

    /**
    * iEngineConfig - Confiugration information about engine.
    */
    TDiagEngineConfig iEngineConfig;

    /**
    * iEngineError - Error code of engine.
    */
    TInt iEngineError;
    };

#endif // DIAGENGINEIMPL_H

// End of File

