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
* Description:  Class declaration for CDiagPluginExecPlanImpl
*
*/


#ifndef DIAGPLUGINEXECPLANIMPL_H
#define DIAGPLUGINEXECPLANIMPL_H

// SYSTEM INCLUDE FILES
#include <e32base.h>                    // CBase
#include <DiagPluginExecPlan.h>         // MDiagPluginExecPlan
#include <DiagResultsDatabaseItem.h>    // CDiagResultsDatabaseItem::TResult

// USER INCLUDE FILES
#include "diagexecplanentryimpl.h"      // TDiagPluginExecPlanImpl::TType

// FORWARD DECLARATION
class MDiagPlugin;
class MDiagSuitePlugin;
class MDiagEngineCommon;
class CDiagPluginPool;
class RDiagResultsDatabaseRecord;
class TDiagEngineConfig;
struct TTransitionStackEntry;

template <class T, TBool StackOwnsEntry>
class CStack;

/**
*  Diagnostics Plugin Execution Plan
*
*  This class provides interface to access execution plan.
*  It also provides interface to modify execution plan.
*  A cursor to currently executing item is also maintained.
*
* @since S60 v5.0
*
*/
NONSHARABLE_CLASS( CDiagPluginExecPlanImpl ) : public CActive,
                                               public MDiagPluginExecPlan
    {
public: 
    /**
    * Two-phase constructor
    *
    * @param aEngine - Reference to diagnostics engine.
    * @param aEngineConfig - Engine configuration information.
    * @param aEntryObserver - Reference to entry observer.
    * @return new instance of CDiagPluginExecPlanImpl
    */
    static CDiagPluginExecPlanImpl* NewL( MDiagEngineCommon& aEngine,
                                          const TDiagEngineConfig& aEngineConfig,
                                          MDiagExecPlanEntryImplObserver& aEntryObserver );

    /**
    * Destructor.
    */
    virtual ~CDiagPluginExecPlanImpl();

public:     // New API
    /**
    * Initialize execution plan
    *
    * @param aStatus - Request status to set when plan creatation is completed.
    * @param aBatch - list of plug-ins to execute
    */
    void InitializeL( TRequestStatus& aStatus, 
                      const RPointerArray< MDiagPlugin >& aBatch );

    /**
    * Initialize execution plan from db.
    *   This will read current data from results database and reconstruct
    *   the execution plan.
    *
    *   This should be used to continue an incomplete session.
    *
    * @param aStatus - Request status to set when plan creatation is completed.
    */
    void InitializeL( TRequestStatus& aStatus );

    /**
    * Access an item in plan. 
    *
    * @param aIndex    - index 
    * @return Reference to plan entry at given index.
    */
    CDiagExecPlanEntryImpl& operator[]( TInt aIndex );      //lint !e1411  This is proper overload

    /**
    * Get current execution plan item.
    *
    * @return Reference to currently executing plan item.
    */
    CDiagExecPlanEntryImpl& CurrentExecutionItem();         //lint !e1411  This is proper overload

    /**
    * Reset execution cursor
    * 
    * Resets execution cursor to the beginning.
    */
    void ResetExecutionCursor();

    /**
    * Moves execution cursor to next item.
    *
    * @return ETrue if cursor is updated.
    *         EFalse if cursor is already at last item.
    */
    TInt MoveCursorToNext();

private:     // from CActive
    /**
    * Handle Active Object completion event.
    * @see CActive::RunL()
    */
    virtual void RunL();

    /**
    * Cancel outstanding request
    * @see CActive::RunL()
    */
    virtual void DoCancel();

    /**
    * Handle errors
    * @see CActive::RunError()
    */
    virtual TInt RunError( TInt aError );

public:     // from MDiagPluginExecPlan
    /**
    * Access an item in plan.
    *   @see MDiagPluginExecPlan::operator[]
    */
    virtual const MDiagExecPlanEntry& operator[]( TInt aIndex ) const;

    /**
    * Get current execution plan item.
    *   @see MDiagPluginExecPlan::CurrentExecutionItem
    */
    virtual const MDiagExecPlanEntry& CurrentExecutionItem() const;

    /**
    * Returns index of execution cursor. 
    *   @see MDiagPluginExecPlan::CurrentIndex
    */
    virtual TInt CurrentIndex() const;

    /**
    * Returns total number of items in the plan.
    *   @see MDiagPluginExecPlan::Count
    */
    virtual TInt Count() const;

    /**
    * Get current test index.
    *
    *   @see MDiagPluginExecPlan::CurrentTestIndex
    */
    virtual TInt CurrentTestIndex( TBool aIncludeDependency ) const;

    /**
    * Get total test count.
    *
    *   @see MDiagPluginExecPlan::TestCount
    */
    virtual TInt TestCount( TBool aIncludeDependency ) const;

    /**
    * Check if this is the last test being executed.
    *
    *   @see MDiagPluginExecPlan::IsLastTest
    */
    virtual TBool IsLastTest() const;

    /**
    * Check if this is the last plug-in being executed.
    *
    *   @see MDiagPluginExecPlan::IsLastPlugin
    */
    virtual TBool IsLastPlugin() const;

    /**
    * Returns the plug-in index that plan is resuming from.
    *
    *   @see MDiagPluginExecPlan::ResumeIndex
    */
    virtual TInt ResumeIndex() const;

private: // private data types
    /**
    * State
    */
    enum TState
        {
        EStateIdle  = 0,
        EStateExpandDependencyAndSuites,
        EStateRemoveEmptySuites,
        EStateInsertSuiteTransitions,
        EStateStoreToDb,
        EStatePlanCreated
        };

private: // private functions
    /**
    * C++ Constructor
    * @param aEngine - reference to engine.
    * @param aEngineConfig - reference to engine configuration information.
    * @param aEntryObserver - Reference to observer for each plug-in
    */
    CDiagPluginExecPlanImpl( MDiagEngineCommon& aEngine,
                             const TDiagEngineConfig& aEngineConfig,
                             MDiagExecPlanEntryImplObserver& aEntryObserver );

    /**
    * Symbian 2nd phase constructor
    */
    void ConstructL();

    /**
    * Change state
    *   @param aState - new state.
    */    
    void ChangeState( TState aState );

    /**
    * Plan creation phase 1. Expand dependencies
    *
    * @return ETrue if any changes are made to the plan.
    */
    TBool ExpandDependenciesL();

    /**
    * Plan creation phase 2. Expand suites to plug-ins
    *
    * @return ETrure if any changes are made to the plan
    */
    TBool ExpandSuitesL();

    /**
    * Plan creation phase 3. Remove empty suites. Delete plan entries that
    * just has prepare and finalize without any tests in between.
    *
    */
    void RemoveEmptySuites();

    /**
    * Plan creation phase 4. Insert suite transition based on parent
    *
    */
    void InsertSuiteTransitionsL();

    /**
    * Store next test plan item to database. Part of STEP_6.
    *
    */
    void StoreNextTestPluginToDbL();
    
    /**
    * Plan creation step 7. Prepend items from last session to new plan.
    */
    void PrependExecutedItemsL();

    /**
    * Report result to client.
    * 
    * @param aErr - Result to report back to engine. KErrNone if successful.
    *   
    */
    void ReportResult( TInt aErr );

    /**
    * Insert plugin
    *
    * This function will attempt to add a plugin into plan. 
    * If item already exists in the plan prior to the given index, it will
    * not add a new item. 
    * If item exists after the given index, and the item is a dependent item, it will
    * move it up in the plan, so that it will be executed earlier.
    * If item exists after the given index, and the item is not a dependent item, 
    * it will create a duplicate entry, to make sure that client specified order
    * is not changed.
    *
    * @param aPlugin - Plugin to insert
    * @param aAsDependent - whether new item is to satify dependency or not.
    * @param aAt - index to insert the dependent item to.
    * @return ETrue if plan is updated. EFalse if no changes to plan is made.
    */
    TBool InsertPluginL( MDiagPlugin& aPlugin,
                         TBool aAsDependent, 
                         TInt aAt );

    /**
    * Utility function to create a default execution plan entry
    *   For test plug-ins, it will create an entry with EQueueToRun as status.
    *   For suite plug-ins, it will create an entry with ETypeSuiteUnexpanded as type.
    *
    * @param aPlugin - Plugin to create default entry for. Either test or suite plug-in.
    * @param aAsDependency - Whether the entry should be created as dependency or not.
    *
    * @return New execution plan entry. Ownership is transferred.
    */
    CDiagExecPlanEntryImpl* CreateDefaultPlanEntryLC(
            MDiagPlugin& aPlugin,
            TBool aAsDependency ) const;

    /**
    * Utility function to add root suite if it does not alredy exists 
    * in the plan. 
    * 
    * NOTE: This should be called only from InsertSuiteTransitionsL()
    */
    void AddRootSuiteIfMissingL();

    /**
    * Utility function to push new transition into stack.
    *
    * NOTE: This should be called only from InsertSuiteTransitionsL()
    *
    * @param aStack - Stack to push new transition to.
    * @Param aHeadIndex - index in execution plan to head entry.
    */
    void PushNewLevelL( CStack<TTransitionStackEntry, ETrue>& aStack, TInt aHeadIndex );

    /**
    * Utility function to pop a level from stack. This will remove top entry
    *
    * NOTE: This should be called only from InsertSuiteTransitionsL()
    *
    * @param aStack - Stack to pop the head item from.
    */
    void PopLevel( CStack<TTransitionStackEntry, ETrue>& aStack );

    /**
    * Utility function to check if given level exists in stack
    *
    * NOTE: This should be called only from InsertSuiteTransitionsL()
    *
    * @param aStack - Stack to check levels in.
    * @param aLevelUid - Uid of leven to check in stack.
    * 
    * @return ETrue if item exists in stack. EFalse if not found.
    */
    TBool IsLevelInStack( CStack<TTransitionStackEntry, ETrue>& aStack, TUid aLevelUid ) const;

    /**
    * Utility function to check if there is more of items of the same parent
    * exists in the plan.
    *
    * NOTE: This should be called only from InsertSuiteTransitionsL()
    * 
    * @param aParentUid - Uid of suite to search for.
    * @param aIndex - Index to execution plan to start searching for.
    * @return ETrue if given suite is still needed.
    *   EFalse otherwise.
    */
    TBool IsThisSuiteNeededAfterThisIndex( TUid aSuiteUid, TInt aIndex ) const;

    /**
    * Utility function to add a SuitePrepare entry into plan.
    *
    * NOTE: This should be called only from InsertSuiteTransitionsL()
    *
    * @param aLevelUid - Uid of suite to add in plan.
    * @param aAt - Index in plan to add new prepare entry to.
    */
    void AddSuitePrepareL( TUid aLevelUid, TInt aAt );

    /**
    * Utility function to add a SuiteFinalize entry into plan.
    *
    * NOTE: This should be called only from InsertSuiteTransitionsL()
    *
    * @param aPrepareIndex - location in plan where prepare is at. Some information
    *   from prepare item will be copied to finalize.
    * @param aAt - Index in execution plan to add finalize entry to.
    */
    void AddSuiteFinalizeL( TInt aPrepareIndex, TInt aAt );
    
    /**
    * Log plan information to debug log. This function does nothing in release build.
    */
    void LogPlanL() const;

    /**
    * Debug function that validate that entries are written to db correctly.
    * This function does nothing in release build.
    */
    void LogPlanInRecordL() const;

private: // data

    /**
    * iPlan - holds plan entries
    * Individual entries are owned by the array.
    */
    RPointerArray<CDiagExecPlanEntryImpl> iPlan;

    /**
    * iExecutedEntries  - Items that are already executed in the
    * previous session.
    * Individual entries are owned by the array.
    */
    RPointerArray<CDiagExecPlanEntryImpl> iExecutedEntries;

    /**
    * iExecutionCursor - currently executing item.
    */
    TInt iExecutionCursor;

    /**
    * iResumeIndex - Index which session is being resumed from.
    */
    TInt iResumeIndex;

    /**
    * iEngine - Reference to diag engine.
    */
    MDiagEngineCommon& iEngine;

    /**
    * iEngineConfig - Enine configuration information.
    */
    const TDiagEngineConfig& iEngineConfig;

    /**
    * iPlanEntryObserver - Plan entry observer. Passed to each plan entry
    *    when it is created.
    */
    MDiagExecPlanEntryImplObserver& iPlanEntryObserver;

    /**
    * iClientStatus - TRequestStatus of the client. Used to notify
    * completion and error.
    * Ownership - Does not own.
    */
    TRequestStatus* iClientStatus;

    /**
    * iState - state of the execution plan creation.
    *   @see TState
    */
    TState iState;
    };

#endif // DIAGPLUGINEXECPLANIMPL_H


// End of File

