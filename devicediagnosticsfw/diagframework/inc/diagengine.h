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
* Description:  Class declaration for CDiagEngine
*
*/


#ifndef DIAGENGINE_H
#define DIAGENGINE_H


// system includes
#include <e32base.h>                // CBase

// user includes.
#include <DiagEngineCommon.h>       // MDiagEngineCommon

// Forward declarations
class MDiagEngineObserver;
class RDiagResultsDatabase;
class CDiagPluginPool;
class CDiagEngineImpl;


/**
*  Diagnostics Framework Engine 
*
*  This class is used by the client to execute tests and load plug-ins.
*
*  @since S60 v5.0
*/
NONSHARABLE_CLASS( CDiagEngine ) : public CBase,
                                   public MDiagEngineCommon
    {
public:

    /**
    * Two-phased constructors. 
    *
    * @param aViewAppUi    View App Ui that is using this engine.
    * @param aObserver     Engine observer.
    * @param aDbSession    Session to RDiagResultDatabase to create record.
    * @param aPluginPool   Initialized plug-in pool that holds plug-ins.
    * @param aDisableDependency    if ETrue, plug-ins should execute
    *                      event if dependencies are not met.
    * @param aExecutionBatch array of plug-ins to execute.
    * @return New instance of CDiagEngine*.
    */
    IMPORT_C static CDiagEngine* NewL( CAknViewAppUi&          aViewAppUi, 
                                       MDiagEngineObserver&    aObserver,
                                       RDiagResultsDatabase&   aDbSession,
                                       CDiagPluginPool&        aPluginPool,
                                       TBool                   aDisableDependency,
                                       const RArray< TUid >&   aExecutionBatch );

    /**
    * Two-phased constructors. This is identical to NewL, except that it will 
    * keep newl instance in clean up stack.
    *
    * @param aViewAppUi    View App Ui that is using this engine.
    * @param aObserver     Engine observer.
    * @param aDbSession    Session to RDiagResultDatabase to create record.
    * @param aPluginPool   Initialized plug-in pool that holds plug-ins.
    * @param aDisableDependency    if ETrue, plug-ins should execute
    *                      event if dependencies are not met.
    * @param aExecutionBatch array of plug-ins to execute.
    * @return New instance of CDiagEngine*.
    */
    IMPORT_C static CDiagEngine* NewLC( CAknViewAppUi&          aViewAppUi, 
                                        MDiagEngineObserver&    aObserver,
                                        RDiagResultsDatabase&   aDbSession,
                                        CDiagPluginPool&        aPluginPool,
                                        TBool                   aDisableDependency,
                                        const RArray< TUid >&   aExecutionBatch );
    /**
    * Two-phased constructors. 
    *
    * @param aViewAppUi    View App Ui that is using this engine.
    * @param aObserver     Engine observer.
    * @param aDbSession    Session to RDiagResultDatabase to create record.
    * @param aPluginPool   Initialized plug-in pool that holds plug-ins.
    * @param aIncompleteRecordUid - Uid to the record to resume.
    *   
    * @return New instance of CDiagEngine*.
    */
    IMPORT_C static CDiagEngine* NewL( CAknViewAppUi&          aViewAppUi, 
                                       MDiagEngineObserver&    aObserver,
                                       RDiagResultsDatabase&   aDbSession,
                                       CDiagPluginPool&        aPluginPool,
                                       TUid                    aIncompleteRecordUid );

    /**
    * Two-phased constructors. This is identical to NewL, except that it will 
    * keep newl instance in clean up stack.
    *
    * @param aViewAppUi    View App Ui that is using this engine.
    * @param aObserver     Engine observer.
    * @param aDbSession    Session to RDiagResultDatabase to create record.
    * @param aPluginPool   Initialized plug-in pool that holds plug-ins.
    * @param aIncompleteRecordUid - Uid to the record to resume.
    *
    * @return New instance of CDiagEngine*.
    */
    IMPORT_C static CDiagEngine* NewLC( CAknViewAppUi&          aViewAppUi, 
                                        MDiagEngineObserver&    aObserver,
                                        RDiagResultsDatabase&   aDbSession,
                                        CDiagPluginPool&        aPluginPool,
                                        TUid                    aIncompleteRecordUid );
    
    /**
    * Destructor.
    */
    virtual ~CDiagEngine();

    /**
    * Execute tests. This is a asynchronous method. 
    * First, execution plan is sent via MDiagEngineObserver::TestExecutionBeginL(). 
    * Then, during the execution, MDiagEngineObserver::TestExecutionProgressL() 
    * may be called multiple times to notify execution progress.  
    * When execution is completed, 
    * MDiagEngineObserver::TestExecutionCompletedL() is called.
    */
    IMPORT_C void ExecuteL();

    /**
    * Set custom execution parameter.
    *
    * @param aCustomParams - Custom data. Owership is not transefrred
    *   so, the client must delete it once test is completed.
    *   Desipte it being a pointer, ownership cannot be transferred 
    *   because engine does not know the type of pointer. 
    */
    IMPORT_C void SetCustomParam( TAny* aCustomParams );

    /**
    * Suspend execution.  This can be used to suspend test execution.
    * If currently executing test is suspendable, it will be suspended and resumed.
    * Otherwise, it will be stopped. When CDiagEngine::ResumeL is called, 
    * the stopped test will be restarted from the beginning. Already executed
    * tests are not affected. 
    * 
    * After engine is suspended, MDiagEngineObserver::TestExecutionSuspendedL() is called.
    *
    */
    IMPORT_C void SuspendL();

    /**
    * Resume suspended execution. This function can be used to resume 
    * previously suspended execution. If execution was suspended on a test
    * that cannot be suspended, the test will restart. Previously executed tests
    * will not be re-run. Also, dependent tests will not be re-run.  If 
    * the suspended test fails to resume (e.g. leaves), it will be marked as
    * failure.
    *
    * After engine is resumed, MDiagEngineObserver::TestExecutionResumedL() is called.
    */
    IMPORT_C void ResumeL();
    
    
    // ADO & Platformization Changes
    ///@@@KSR: changes for Codescanner error val = High
    //IMPORT_C TBool GetPluginDependency();
    IMPORT_C TBool GetPluginDependencyL();

public: // from MDiagEngineCommon

    /**
    * Returns current execution plan.
    * @see MDiagEngineCommon::ExecutionPlanL
    */
    virtual const MDiagPluginExecPlan& ExecutionPlanL() const;

    /**
    * Stop current execution of tests.
    * @see MDiagEngineCommon::ExecutionStopL
    */
    virtual void ExecutionStopL( TCancelMode aCancelMode );
    
    /**
    * Resets test execution watchdog timer.
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
    * @see MDiagEngineCommon::ViewAppUi
    */
    virtual CAknViewAppUi& ViewAppUi();

    /**
    * Get App Ui reference that launched Diagnostics Engine.
    * @see MDiagEngineCommon::ViewAppUi
    */
    virtual const CAknViewAppUi& ViewAppUi() const;

    /**
    * Get Database Subsession for record open for writing.
    * @see MDiagEngineCommon::DbRecord
    */
    virtual RDiagResultsDatabaseRecord& DbRecord();

    /**
    * Get Database Subsession for record open for writing.
    * @see MDiagEngineCommon::DbRecord
    */
    virtual const RDiagResultsDatabaseRecord& DbRecord() const;

    /**
    * Get reference to Plugin Pool
    * @see MDiagEngineCommon::PluginPool
    */
    virtual CDiagPluginPool& PluginPool();

    /**
    * Get reference to Plugin Pool
    * @see MDiagEngineCommon::PluginPool
    */
    virtual const CDiagPluginPool& PluginPool() const;

    /**
    * Add a text to configuraiton list option. 
    * @see MDiagEngineCommon::AddToConfigListL
    */
    virtual void AddToConfigListL( TConfigListType aListType, const TDesC& aText );

    /**
    * Remove a text from configuration list.
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
    *  @see MDiagEngineCommon::IsDependencyDisabled
    */
    virtual TBool IsDependencyDisabled() const;

    /**
    * Get custom param
    *  @see MDiagEngineCommon::CustomParam
    */
    virtual TAny* CustomParam() const;

    /**
    * Execute an application command. 
    *   @see MDiagEngineCommon::ExecuteAppCommandL
    */
    virtual void ExecuteAppCommandL( TDiagAppCommand aCommand, 
                                     TAny* aParam1,
                                     TAny* aParam2 );

private:    // private data
    CDiagEngineImpl* iEngineImpl;
    };


#endif // DIAGENGINE_H

// End of File

