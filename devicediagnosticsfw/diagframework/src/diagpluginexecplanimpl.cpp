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
* Description:  Class definition of CDiagPluginExecPlanImpl
*
*/


// CLASS DECLARATION
#include "diagpluginexecplanimpl.h"

// SYSTEM INCLUDE FILES
#include <e32def.h>
#include <cstack.h>                     // CStack
#include <DiagPlugin.h>                 // MDiagPlugin
#include <DiagSuitePlugin.h>            // MDiagSuitePlugin
#include <DiagTestPlugin.h>             // MDiagTestPlugin
#include <DiagPluginPool.h>             // CDiagPluginPool
#include <DiagResultsDatabase.h>        // RDiagResultsDatabaseRecord
#include <DiagResultsDbItemBuilder.h>   // CDiagResultsDbItemBuilder
#include <DiagResultDetail.h>           // MDiagResultDetail
#include <DiagFrameworkDebug.h>         // For debug log
#include <DiagResultsDbRecordEngineParam.h> // CDiagResultsDbRecordEngineParam

// USER INCLUDE FILES
#include "diagframework.pan"            // panic codes
#include "diagexecplanentryimpl.h"      // CDiagExecPlanEntryImpl
#include "diagexecplanentryimpltest.h"  // CDiagExecPlanEntryImplTest
#include "diagexecplanentryimplsuite.h" // CDiagExecPlanEntryImplSuite
#include "diagcleanupresetanddestroy.h" // CleanupRPointerArrayPushL
#include "diagengineconfig.h"           // TDiagEngineConfig


// DATA

// MACROS
// Uncomment the line below to enable more plan creation log.
// #define _DEBUG_EXEC_PLAN


// LOCAL DATA TYPES
/**
* Used for keeping track of suite level stack.
* It is used in InsertSuiteTransitionsL() function. ( STEP_5 )
*/
struct TTransitionStackEntry
    {
    /**
    * iLeavelUid - Uid of the current level. 
    */
    TUid  iLevelUid;
    
    /**
    * iPrepareIndex - Index in the plan where ETypeSuitePrepare entry is. 
    *   This information is used to update the suite iAsDependent field.
    */
    TInt    iPrepareIndex;         
    };    


// ======== LOCAL FUNCTIONS ========


// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// CDiagPluginExecPlanImpl::NewL
// ---------------------------------------------------------------------------
//
CDiagPluginExecPlanImpl* CDiagPluginExecPlanImpl::NewL( 
        MDiagEngineCommon& aEngine,
        const TDiagEngineConfig& aEngineConfig,
        MDiagExecPlanEntryImplObserver& aEntryObserver )
    {
    CDiagPluginExecPlanImpl* self = new ( ELeave ) CDiagPluginExecPlanImpl( 
        aEngine,
        aEngineConfig,
        aEntryObserver );

    return self;
    }

// ---------------------------------------------------------------------------
// CDiagPluginExecPlanImpl::CDiagPluginExecPlanImpl
// ---------------------------------------------------------------------------
//
CDiagPluginExecPlanImpl::CDiagPluginExecPlanImpl( 
        MDiagEngineCommon& aEngine,
        const TDiagEngineConfig& aEngineConfig,
        MDiagExecPlanEntryImplObserver& aEntryObserver )
    :   CActive( EPriorityLow ),
        iEngine( aEngine ),
        iEngineConfig( aEngineConfig ),
        iPlanEntryObserver( aEntryObserver )
    {
    CActiveScheduler::Add( this );
    }

// ---------------------------------------------------------------------------
// CDiagPluginExecPlanImpl::~CDiagPluginExecPlanImpl
// ---------------------------------------------------------------------------
//
CDiagPluginExecPlanImpl::~CDiagPluginExecPlanImpl()
    {
    Cancel();
    iPlan.ResetAndDestroy();
    iPlan.Close();
    iExecutedEntries.ResetAndDestroy();
    iExecutedEntries.Close();
    }


// ---------------------------------------------------------------------------
// CDiagPluginExecPlanImpl::InitaliazeL
//      Creating a fresh session.
// ---------------------------------------------------------------------------
//
void CDiagPluginExecPlanImpl::InitializeL( TRequestStatus& aStatus, 
                                           const RPointerArray< MDiagPlugin >& aBatch )
    {
    LOGSTRING( "CDiagPluginExecPlanImpl::InitializeL: new session")

    __ASSERT_ALWAYS( iState == EStateIdle, Panic( EDiagFrameworkInternal ) );

    aStatus = KRequestPending;
    iClientStatus = &aStatus;

    iExecutedEntries.ResetAndDestroy();
    iPlan.ResetAndDestroy();
    iResumeIndex = 0;

    // pre-step execution step. copy the argument to a local plan
    TInt i;
    for ( i = 0; i < aBatch.Count();  i++ )
        {
        __ASSERT_ALWAYS( aBatch[i] != NULL, Panic( EDiagFrameworkBadArgument ) );

        CDiagExecPlanEntryImpl* newEntry = CreateDefaultPlanEntryLC(
            *(aBatch[i]),
            EFalse );   // aAsDependency

        iPlan.AppendL( newEntry );  // ownership transferred.
        CleanupStack::Pop( newEntry ); 
        newEntry = NULL;
        }

    LOGSTRING( "CDiagPluginExecPlanImpl::InitializeL: Initial Batch" )
    LogPlanL();

    ChangeState( EStateExpandDependencyAndSuites );
    }

// ---------------------------------------------------------------------------
// CDiagPluginExecPlanImpl::InitializeL
//      Continuing from incomplete session.
// ---------------------------------------------------------------------------
//
void CDiagPluginExecPlanImpl::InitializeL( TRequestStatus& aStatus )
    {
    LOGSTRING( "CDiagPluginExecPlanImpl::InitaliazeL: continue" )

    // validate input.
    __ASSERT_ALWAYS( iState == EStateIdle, Panic( EDiagFrameworkInternal ) );

    // if we are resuming incomplete session, it must incomplete.
    TBool isRecordCompleted = EFalse;
    User::LeaveIfError( iEngine.DbRecord().IsTestCompleted( isRecordCompleted ) );
    __ASSERT_ALWAYS( !isRecordCompleted, Panic( EDiagFrameworkBadArgument ) );

    aStatus = KRequestPending;
    iClientStatus = &aStatus;
    iResumeIndex = 0;

    iExecutedEntries.ResetAndDestroy();
    iPlan.ResetAndDestroy();

    // Retrieve records from database and use it as base for creating 
    // a new plan.
    RPointerArray< CDiagResultsDatabaseItem > previousResults;
    DiagFwInternal::CleanupRPointerArrayPushL< CDiagResultsDatabaseItem >( &previousResults );

    User::LeaveIfError( 
        iEngine.DbRecord().GetTestResults( previousResults ) );

    TInt resultCount = previousResults.Count();
    for( TInt i = 0; i < resultCount; i++ )
        {
        // Get the test plug-in.
        // Note that FindPlugin does not transfer ownership and hence testPlugin
        // and does not need to be deallocated.
        MDiagPlugin& testPlugin = 
            iEngine.PluginPool().FindPluginL( previousResults[i]->TestUid() );

        // This should have been a test plug-in
        __ASSERT_DEBUG( testPlugin.Type() == MDiagPlugin::ETypeTestPlugin,
            Panic( EDiagFrameworkInternal ) );

        CDiagExecPlanEntryImplTest* testEntry = 
            CDiagExecPlanEntryImplTest::NewLC( 
                iEngine,
                iEngineConfig,
                iPlanEntryObserver,
                static_cast< MDiagTestPlugin& >( testPlugin ),
                previousResults[i]->WasDependency(),
                previousResults[i]->TestResult() );

        if ( testEntry->Result() == CDiagResultsDatabaseItem::EQueuedToRun )
            {
            // it was queued to be executed.
            iPlan.AppendL( testEntry ); // ownership transferred.
            }
        else
            {
            // it was already executed. 
            iExecutedEntries.AppendL( testEntry ); // ownership transferred.
            }
        CleanupStack::Pop( testEntry );
        testEntry = NULL;
        }

    CleanupStack::PopAndDestroy( &previousResults );

    iResumeIndex = iExecutedEntries.Count();

    if ( iResumeIndex != 0 )
        {
        LOGSTRING( "CDiagPluginExecPlanImpl::InitializeL: Queued Items" )
        LogPlanL();

        ChangeState( EStateExpandDependencyAndSuites );
        }
    else
        {
        LOGSTRING( "CDiagPluginExecPlanImpl::InitializeL: "
            L"Items in DB may be invalid. Fully recreate plan" )

        // If iResumeIndex is 0, it means that all items in the db are 
        // marked as EQueuedToRun. Because of the async nature of db,
        // it could also indicate that not previous plan was not fully
        // written to the database. In this case, read the engine parameter
        // and try to recreate the plan from scratch.
        //
        // If it was already fully written but hadn't had a chance to really
        // execute anything, this will not cause any ill effects since
        // plan created should be identical and simply reset 
        // all results in db to EQueuedToRun.
        iExecutedEntries.ResetAndDestroy();
        iPlan.ResetAndDestroy();

        RPointerArray< MDiagPlugin > batch;
        CleanupClosePushL( batch );    // items are not owned, so no need for "Destroy"

        CDiagResultsDbRecordEngineParam* engineParam = NULL;
        User::LeaveIfError( 
            iEngine.DbRecord().GetEngineParam( engineParam ) );

        CleanupStack::PushL( engineParam );

        const RArray< TUid >& batchUids = engineParam->ExecutionsUidArray();

        // Read original Uids from previous engine param.
        for( TInt i = 0; i < batchUids.Count(); i++ )
            {
            MDiagPlugin* testPlugin = NULL;
            User::LeaveIfError( 
                iEngine.PluginPool().FindPlugin( batchUids[i], testPlugin ) );

            __ASSERT_DEBUG( testPlugin != NULL, Panic( EDiagFrameworkInternal ) );
            if ( testPlugin )  //lint !e774 This will be evaluated on non-debug build.
                {
                batch.AppendL( testPlugin );
                }
            }

        CleanupStack::PopAndDestroy( engineParam );
        engineParam = NULL;

        // Call the normal InitializeL() method as if it is a new session.
        InitializeL( aStatus, batch );

        CleanupStack::PopAndDestroy( &batch );  // calls Close()
        }
    }

// ---------------------------------------------------------------------------
// CDiagPluginExecPlanImpl::operator[]
// ---------------------------------------------------------------------------
//
CDiagExecPlanEntryImpl& CDiagPluginExecPlanImpl::operator[]( TInt aIndex )
    {
    return *iPlan[aIndex];
    }

// ---------------------------------------------------------------------------
// CDiagPluginExecPlanImpl::CurrentExecutionItem
// ---------------------------------------------------------------------------
//
CDiagExecPlanEntryImpl& CDiagPluginExecPlanImpl::CurrentExecutionItem() 
    {
    return *iPlan[iExecutionCursor];
    }

// ---------------------------------------------------------------------------
// CDiagPluginExecPlanImpl::ResetExecutionCursor
// ---------------------------------------------------------------------------
//
void CDiagPluginExecPlanImpl::ResetExecutionCursor()
    {
    iExecutionCursor = 0;
    }

// ---------------------------------------------------------------------------
// CDiagPluginExecPlanImpl::MoveCursorToNext
// ---------------------------------------------------------------------------
//
TBool CDiagPluginExecPlanImpl::MoveCursorToNext()
    {
    TBool value = EFalse;

    if ( iExecutionCursor < iPlan.Count() - 1 )
        {
        iExecutionCursor++;
        value = ETrue;
        }
    else
        {
        // cannot move beyond the last item.
        }

    return value;
    }

// ======== From CActive =========
// ---------------------------------------------------------------------------
// From CActive
// CDiagPluginExecPlanImpl::RunL
// ---------------------------------------------------------------------------
//
void CDiagPluginExecPlanImpl::RunL()
    {
    /*----------------------------------------------------------------------
       Create execution plan:
       Overview:
       
            STEP_1. Expand Dependency 
            STEP_2. Expand Suites.
            STEP_3. Repeat STEP_1 & STEP_2 until no changes are made to the plan
            STEP_4. Check and remove empty suites
            STEP_5. Add Prepare/Finalize suite execution.
            STEP_6. Store execution plan to results db. 
            STEP_7. Append items that were executed in the previous test
                    to the beginning of the test.
       
       Details:
        STEP_1. Expand Dependency
            In this step, each item is checked for dependency and 
            dependent items will be inserted into plan.
            Note that if dependent item also depends on something else, those
            will be added immediately.
       
            All items inserted during this phase is inserted as dependent.
       
        STEP_2. Expand suites
            Suites are expandeded to individual tests or suites. Suite
            item will be expanded between parent suite's Prepare and 
            Finalize items. 
            
            iAsDependent value will be inherited from the parent suite
            that it expanded from. E.g. if suite was dependent, then so will
            all the items that included from that suite.
        
            Note that it does not remove the original suite entry in the plan. 
            This is to make sure that dependencies inserted will be grouped
            within the same group as the plug-ins that depended on it.
            If they are removed, and re-inserted later in STEP_5, it will
            cause the dependent items to be outside of the original grouping.
                e.g.  S1 = {P1}
                      S2 = {P2, P3}

                      and P2 depends on P1

                When dependency and suites are expanded with original
                suite removed, after STEP_3, the plan will look like this:
                    (* indicates iAsDependent == ETrue)

                      0     1    2
                      *P1   P2   P3

                When it tries to add suite transition back in to the plan,
                it will look like this:
                    (* indicates iAsDependent == ETrue)
                    (< indicates suite prepare execution.)
                    (> indicates suite finalize execution.)

                      0    1    2    3    4   5   6
                      <S1  *P1  S1>  <S2  P2  P3  S2>

                which is somewhat incorrect since *P1 is added due to 
                dependency. 
                
                Instead, if original suite grouping is kept, after STEP_3

                    0    1    2    3    4
                    <S2  *P1  P2   P3   S2>

                After STEP_5

                    0    1    2    3    4   5   6
                    <S2  <S1  *P1  S1>  P2  P3  S2>
                             
                which is more correct.
            
            Note that this will be done in breadth first style. If there is a
            nested suites, it will not be expanded until next cycle. 
            This is to allow dependencies of the suite to be expanded before
            being removed.
       
        STEP_3. Repeat STEP_1 & STEP_2 until no changes are made.
            This step ensures that suites are expanded, and all dependent
            items are added to the plan.
       
        STEP_4. Remove Empty suites
            Because dependent items can be moved, it is possible that plan ends
            up with empty suites. This section will check for 
            suite prepare/finalize that has nothing inside, and remove them.

        STEP_5. Add Prepare/Finalize steps for suites.
            As a final step, it will navigate all the entries in the plan, and
            insert proper prepare/finalize steps. This is based on stack,
            where every suite prepare is pushed into a stack, and poped when
            suite finalize is called. e.g. consider following plug-ins:

                S1 = { P1, P2 }
                S2 = { P3, P4 }

                And P3 depends on P2.

            And resulting plan after STEP_4 looked like this:

                0    1     2    3    4
                <S2  *P2   P3   P4   S2>

            After STEP_5, it should look like this:

                0    1    2    3    4    5    6
                <S2  <S1  *P2  S1>  P3   P4   S2>

        STEP_6. Store execution plan to results db. 
            This allows continuation of the incomplete results.
            Only test plug-ins are written to the database. 

        STEP_7. Append items that were executed in the previous test
            to the beginning of the test.

        Other Notes:

        NOTE_1) This logic currently does not check for circular dependencies.
           If circular dependency exists, the program will go into an
           infinite loop in ExpandDependenciesL().

        NOTE_2) Inserting duplicate items.
            Due to dependencies, duplicate entries may exist in the plan.

            When items are inserted during STEP_1 - STEP_2, it will be checked
            for duplicates.
       
            Duplicate removal policy is as follows:
            RULE_A) If an item with "iAsDependent == ETrue" is being inserted,
                AND if an item already exist in the plan before current index,
                then it will not be inserted. 

            RULE_B) Any items inserted with "iAsDependent == EFalse" will 
               always be inserted at the requested index.
               This is because they are considered to be user input, and
               plan will not try to re-order the execution order that 
               client or user has specified.

            RULE_C) The first instance of an item has higher priority than later
               instances. Later instace of duplicate will be removed, unless
               it violates RULE_B. (e.g. item has "iAsDependent == EFalse".)

            This has a side effect: item may execute twice in certain cases.
            E.g. original plan has two plug-ins:  P1, P2, P3
            P1 depends on P2, and P3 also depends ond P2

            Original Input:   0   1   2
                              P1  P2  P3

            After STEP_3, it should look like this. 
                    (* indicates iAsDependent == ETrue)

                              0    1    2   3    4
                              *P2  P1   P2  *P2  P3

            During STEP_4, duplicates are removed, and it should look like this:
                    (* indicates iAsDependent == ETrue)

                              0    1    2   4
                              *P2  P1   P2  P3

            Note that there are two P2s in the plan; 
            at 0 with iAsDependent == ETrue and at 2 with iAsDependent == EFalse

            There may be some discussions on whether this is how it should be.
            However, at this point, it seems that it is more important that
            execution appears to the user in the order specified. 

            To make sure that user sees that P2 is executed only once, plug-in
            developer may need to make sure that if a plug-in provides services,
            it should also handle cases where it is executed twice.

            In reality, such cases can be avoided by grouping the plug-ins 
            in the correct order, hence it should not be an issue.

    ----------------------------------------------------------------------*/

    // First, error handling.
    LOGSTRING3( "CDiagPluginExecPlanImpl::RunL: State = %d, Err = %d",
        iState, iStatus.Int() )

    User::LeaveIfError( iStatus.Int() );

    switch ( iState )
        {
        case EStateExpandDependencyAndSuites:
            {
            LOGSTRING( "CDiagPluginExecPlanImpl::RunL: STEP_1")
            // STEP_1. Expand dependency. 
            if ( !iEngineConfig.IsDependencyDisabled() )
                {
                ExpandDependenciesL();
                }

            LOGSTRING( "CDiagPluginExecPlanImpl::RunL: STEP_2")
            // STEP_2. Expand suites.
            if ( ExpandSuitesL() )
                {
                // STEP_3. Repeat STEP_1, STEP_2 until no changes are made.
                // Do a self transition to repeat STEP_1 and STEP_2
                ChangeState( EStateExpandDependencyAndSuites );
                }
            else
                {
                // no changes are made to the plan. Move to STEP_4
                ChangeState( EStateRemoveEmptySuites );
                }
            }
            break;

        case EStateRemoveEmptySuites:
            LOGSTRING( "CDiagPluginExecPlanImpl::RunL: STEP_4")
            // STEP_4. Remove empty suites. This can happen because of duplicate
            // dependencies moving later dependent test to earlier position.
            RemoveEmptySuites();

            #ifdef _DEBUG_EXEC_PLAN
            LOGSTRING( "CDiagPluginExecPlanImpl:CreatePlanL: "
                L"Before Inserting Suite Transtions" )
            LogPlanL();
            #endif // _DEBUG_EXEC_PLAN

            // Continue to STEP_5
            ChangeState( EStateInsertSuiteTransitions );
            break;

        case EStateInsertSuiteTransitions:
            LOGSTRING( "CDiagPluginExecPlanImpl::RunL: STEP_5 - Insert Suite Transtions")
            // STEP_5. Add pre/post suite execution
            InsertSuiteTransitionsL();

            // STEP_6. Store plan to db.
            LOGSTRING( "CDiagPluginExecPlanImpl::RunL: STEP_6 - Store to db")
            // this time, manually update state, since storing 
            // to database needs to use iStatus
            iState = EStateStoreToDb;

            // Move cursor to one before so that it can start checking from
            // the beginning. StoreNextTestPluginToDbL() will move the cursor
            // as soon as it enters.
            iExecutionCursor = -1;
            StoreNextTestPluginToDbL();
            
            break;

        case EStateStoreToDb:
            LOGSTRING2( "CDiagPluginExecPlanImpl::RunL: STEP_6 - "
                L"item 0x%08x stored.",
                iPlan[iExecutionCursor]->Plugin().Uid().iUid )

            StoreNextTestPluginToDbL();
            break;

        case EStatePlanCreated:
            // STEP_7. Prepend the items that were executed in last session
            // to the beginning of the execution plan.
            LOGSTRING( "CDiagPluginExecPlanImpl::RunL: STEP_7 - prepend executed items" )
            PrependExecutedItemsL();

            LOGSTRING( "CDiagPluginExecPlanImpl::RunL: Final Plan" )
            LogPlanL();
            
            LOGSTRING2( "CDiagPluginExecPlanImpl::RunL: Resume at index %d", iResumeIndex )

            LogPlanInRecordL();

            ResetExecutionCursor();

            ReportResult( KErrNone );
            break;

        case EStateIdle:
        default:
            __ASSERT_DEBUG( 0, Panic( EDiagFrameworkInternal ) );
            break;
        }
    }

// ---------------------------------------------------------------------------
// From CActive
// CDiagPluginExecPlanImpl::DoCancel
// ---------------------------------------------------------------------------
//
void CDiagPluginExecPlanImpl::DoCancel()
    {
    iPlan.ResetAndDestroy();
    iExecutedEntries.ResetAndDestroy();
    iState = EStateIdle;

    ReportResult( KErrCancel );
    }

// ---------------------------------------------------------------------------
// From CActive
// CDiagPluginExecPlanImpl::RunError
// ---------------------------------------------------------------------------
//
TInt CDiagPluginExecPlanImpl::RunError( TInt aError )
    {
    iPlan.ResetAndDestroy();
    iExecutedEntries.ResetAndDestroy();
    iState = EStateIdle;

    ReportResult( aError );

    return KErrNone;
    }

// ---------------------------------------------------------------------------
// From MDiagPluginExecPlan
// CDiagPluginExecPlanImpl::CurrentIndex
// ---------------------------------------------------------------------------
//
TInt CDiagPluginExecPlanImpl::CurrentIndex() const
    {
    return iExecutionCursor;
    }


// ---------------------------------------------------------------------------
// From MDiagPluginExecPlan
// CDiagPluginExecPlanImpl::CurrentTestIndex
// ---------------------------------------------------------------------------
//
TInt CDiagPluginExecPlanImpl::CurrentTestIndex( TBool aIncludeDependency ) const
    {
    TInt count = 0;

    for ( TInt i = 0; i < iPlan.Count() && i < iExecutionCursor; i++ )
        {
        if ( iPlan[i]->Plugin().Type() == MDiagPlugin::ETypeTestPlugin )
            {
            // if caller wants to include both explicit and dependent
            // or if plug-in is not depentent test, count.
            if ( aIncludeDependency || ( !iPlan[i]->AsDependency() ) )
                {
                count++;
                }
            }
        }
    
    // subtract 1 if we only went past the last test.
    if ( count >= TestCount( aIncludeDependency ) )
        {
        count--;
        }
    return count;
    }


// ---------------------------------------------------------------------------
// From MDiagPluginExecPlan
// CDiagPluginExecPlanImpl::TestCount
// ---------------------------------------------------------------------------
//
TInt CDiagPluginExecPlanImpl::TestCount( TBool aIncludeDependency ) const
    {
    TInt count = 0;

    for ( TInt i = 0; i < iPlan.Count(); i++ )
        {
        if ( iPlan[i]->Plugin().Type() == MDiagPlugin::ETypeTestPlugin )
            {
            // if caller wants to include both explicit and dependent
            // or if plug-in is not depentent test, count.
            if ( aIncludeDependency || ( !iPlan[i]->AsDependency() ) )
                count++;
            }
        }
    return count;
    }

// ---------------------------------------------------------------------------
// From MDiagPluginExecPlan
// CDiagPluginExecPlanImpl::operator[] () const
// ---------------------------------------------------------------------------
//
const MDiagExecPlanEntry& CDiagPluginExecPlanImpl::operator[] ( TInt aIndex ) const
    {
    __ASSERT_ALWAYS( aIndex >= 0 && aIndex < iPlan.Count(), 
                     Panic( EDiagFrameworkArrayBounds ) );
    return *( static_cast< MDiagExecPlanEntry* >( iPlan[aIndex] ) );
    }

// ---------------------------------------------------------------------------
// From MDiagPluginExecPlan
// CDiagPluginExecPlanImpl::CurrentExecutionItem const
// ---------------------------------------------------------------------------
//
const MDiagExecPlanEntry& CDiagPluginExecPlanImpl::CurrentExecutionItem() const
    {
    __ASSERT_ALWAYS( iExecutionCursor >= 0 && iExecutionCursor < iPlan.Count(), 
                     Panic( EDiagFrameworkArrayBounds ) );
    return *( static_cast< MDiagExecPlanEntry* >( iPlan[iExecutionCursor] ) );
    }


// ---------------------------------------------------------------------------
// From MDiagPluginExecPlan
// CDiagPluginExecPlanImpl::IsLastTest
// ---------------------------------------------------------------------------
//
TBool CDiagPluginExecPlanImpl::IsLastTest() const
    {
    if ( TestCount( ETrue ) == 0 )
        {
        // there was no test in the plan.. 
        // In this case, it is always ETrue.
        return ETrue;
        }

    return ( CurrentTestIndex( ETrue ) == TestCount( ETrue ) - 1 );
    }

// ---------------------------------------------------------------------------
// From MDiagPluginExecPlan
// CDiagPluginExecPlanImpl::IsLastPlugin
// ---------------------------------------------------------------------------
//
TBool CDiagPluginExecPlanImpl::IsLastPlugin() const
    {
    if ( iPlan.Count() == 0 )
        {
        // if plan was empty, always consider it to be the last.
        return ETrue;
        }

    return iExecutionCursor == ( iPlan.Count() -1 );
    }


// ---------------------------------------------------------------------------
// From MDiagPluginExecPlan
// CDiagPluginExecPlanImpl::Count
// ---------------------------------------------------------------------------
//
TInt CDiagPluginExecPlanImpl::Count() const
    {
    return iPlan.Count();
    }

// ---------------------------------------------------------------------------
// From MDiagPluginExecPlan
// CDiagPluginExecPlanImpl::ResumeIndex
// ---------------------------------------------------------------------------
//
TInt CDiagPluginExecPlanImpl::ResumeIndex() const
    {
    return iResumeIndex;
    }

// ---------------------------------------------------------------------------
// CDiagPluginExecPlanImpl::ChangeState
// ---------------------------------------------------------------------------
//
void CDiagPluginExecPlanImpl::ChangeState( TState aState )
    {
    LOGSTRING3( "CDiagPluginExecPlanImpl::ChangeState: state change %d -> %d",
        iState, aState )

    iState = aState;

    TRequestStatus* stat = &iStatus;
    User::RequestComplete( stat, KErrNone );
    SetActive();
    }

// ---------------------------------------------------------------------------
// CDiagPluginExecPlanImpl::ExpandDependenciesL
// ---------------------------------------------------------------------------
//
TBool CDiagPluginExecPlanImpl::ExpandDependenciesL()
    {
    // STEP_1. Expand dependency.
    // Before modifying this function, please see STEP_1 comments in CreatePlanL()
    TInt planIdx = 0;
    TBool planChanged( EFalse );
    while ( planIdx < iPlan.Count() )
        {
        TBool itemAdded = EFalse;

        // we are only interested in type ETypeTestExec or ETypeSuiteUnexpanded. 
        // If it is ETypeSuitePrepare or ETypeSuiteFinalize, that means 
        // that it is a suite and it has been already been expanded to 
        // prepare/finalize. Since dependencies are resolved before suites 
        // are expanded, we are not interested in re-evaluating expanded tests.
        if ( iPlan[planIdx]->Type() == CDiagExecPlanEntryImpl::ETypeTestExec ||
             iPlan[planIdx]->Type() == CDiagExecPlanEntryImpl::ETypeSuiteUnexpanded )
            {
            // dependencies should be rare. So granuality 1 should be fine.
            CPtrCArray* depList = new( ELeave )CPtrCArray( 1 );
            CleanupStack::PushL( depList );
            iPlan[planIdx]->Plugin().GetLogicalDependenciesL( *depList );

            // iterate through the dependiencies BACKWARDS to make sure
            // that dependencies are inserted in the same order 
            // specified. If it is not added backwards, developers may 
            // be surprised that dependencies are executed 
            // in reverse order of what is specified in the XML.
            for ( TInt depIdx = depList->Count() - 1; depIdx >= 0; depIdx-- )
                {
                // If dependency is specified, but the dependent plug-in is 
                // not found in plug-in pool, this probably means 
                // plug-in database is corrupted.
                MDiagPlugin& plugin = iEngine.PluginPool().FindPluginL( ( *depList )[ depIdx ] );

                if ( InsertPluginL( plugin, 
                                    ETrue, 
                                    planIdx ) )
                    {
                    itemAdded = ETrue;
                    }
                }
            CleanupStack::PopAndDestroy( depList );
            depList = NULL;
            }

        // Advance to next item in plan only if no new item is added.
        // This ensures that items just added are re-evaluated for
        // dependency, and expand if it is an unexpanded suite.
        if ( !itemAdded )
            {
            planIdx++;
            }
        else
            {
            planChanged = ETrue;
            }
        }

    return planChanged;
    }


// ---------------------------------------------------------------------------
// CDiagPluginExecPlanImpl::ExpandSuitesL
// ---------------------------------------------------------------------------
//
TBool CDiagPluginExecPlanImpl::ExpandSuitesL() 
    {
    // STEP_2. Expand suites.
    // Before modifying this function, please see STEP_1 comments in CreatePlanL()
    TBool planChanged = EFalse;
    TInt i = 0;
    while ( i < iPlan.Count() )
        {
        if ( iPlan[i]->Type() == CDiagExecPlanEntryImpl::ETypeSuiteUnexpanded )
            {
            // If it is unexpanded, it must be a suite.
            __ASSERT_DEBUG( iPlan[i]->Plugin().Type() == MDiagPlugin::ETypeSuitePlugin,
                            Panic( EDiagFrameworkInternal ) );
            planChanged = ETrue;

            // Change type from ETestExec to ETypeSuitePrepare. This indicates
            // that the suite item in the plan has been examined and
            // expanded.
            iPlan[i]->SetType( CDiagExecPlanEntryImpl::ETypeSuitePrepare );

            // dependency is inherited
            TBool asDependency = iPlan[i]->AsDependency();
            
            // Get Children from the plug-in
            MDiagSuitePlugin& suite = static_cast<MDiagSuitePlugin&>( iPlan[i]->Plugin() );
            RPointerArray<MDiagPlugin> children;
            CleanupClosePushL( children );

            suite.GetChildrenL( children, MDiagSuitePlugin::ESortByPosition );

            i++; // insert rest after current item.

            TInt childIdx = 0;
            while ( childIdx < children.Count() )
                {
                if ( InsertPluginL( *( children[childIdx] ), 
                                    asDependency, 
                                    i ) )
                    {
                    // new item is added. Move to next
                    i++;
                    }
                childIdx++;
                }

            children.Reset();   // children pointers are not owned 
            CleanupStack::PopAndDestroy( &children );

            // insert suite finalize entry into plan
            CDiagExecPlanEntryImplSuite* finalizeEntry = 
                CDiagExecPlanEntryImplSuite::NewLC(
                    iEngine,
                    iEngineConfig,
                    iPlanEntryObserver,
                    suite,
                    asDependency,
                    CDiagExecPlanEntryImpl::ETypeSuiteFinalize );

            iPlan.InsertL( static_cast< CDiagExecPlanEntryImpl* >( finalizeEntry ), i );   
            CleanupStack::Pop( finalizeEntry ); // owership transferred
            finalizeEntry = NULL;
            i++;
            }
        else
            {
            // this one is not suite, so examine the next one.
            i++;    
            }
        }

    return planChanged;
    }

// Looking for STEP_3?  It is in RunL() .. 

// ---------------------------------------------------------------------------
// CDiagPluginExecPlanImpl::RemoveEmptySuites
// ---------------------------------------------------------------------------
//
void CDiagPluginExecPlanImpl::RemoveEmptySuites()
    {
    // STEP_4. Remove empty suites
    // Before modifying this function, please see STEP_1 comments in CreatePlanL()

    // Checking for duplicate is done by checking if suite prepare/finalize
    // is in the plan next to each other.
    //
    // NOTE_4
    // After a empty suite is removed, step back one index.
    // so that the previous item will be re-evaluated, in case removing them cased 
    // the parent suite to be empty as well.
    //      e.g
    //      i == 1
    //      index:   0   [1]  2   3
    //               <A  <B   B>  A> 
    //  after removal, now prepare/finalize A becomes next to each other
    //
    //      i == 1
    //      index:   0   [1] 
    //               <A  A> 
    //
    // in order to make sure that above case is handled, decrement i and
    // re-evaluate from index 0
    //
    TInt i = 0;
    while ( i < iPlan.Count() - 2 )  // no need to go beyond the last item
        {
        if ( iPlan[i]->Type() == CDiagExecPlanEntryImpl::ETypeSuitePrepare &&
                iPlan[i+1]->Type() == CDiagExecPlanEntryImpl::ETypeSuiteFinalize )
            {
            // if these two do not match, then there may have been
            // a problem during plan creation.
            __ASSERT_DEBUG( iPlan[i]->Plugin().Uid() == iPlan[i+1]->Plugin().Uid(),
                            Panic( EDiagFrameworkCorruptPlugin ) );


            CDiagExecPlanEntryImpl* entry = iPlan[ i + 1 ];
            iPlan.Remove( i+1 );
            delete entry;
            entry = NULL;

            entry = iPlan[i];
            iPlan.Remove( i );
            delete entry;
            entry = NULL;
            
            // Wondering why step back by one? See NOTE_4
            if ( i > 0 )
                {
                i--;
                }
            }
        else
            {
            // it's not empty. Examine next item.
            i++;
            }
        }
    }

// ---------------------------------------------------------------------------
// CDiagPluginExecPlanImpl::InsertSuiteTransitionsL
// ---------------------------------------------------------------------------
//
void CDiagPluginExecPlanImpl::InsertSuiteTransitionsL()
    {
    // STEP_5. Add pre/post suite execution based on test parent change.
    // Before modifying this function, please see STEP_1 comments in CreatePlanL()
    
    // if plan is empty, nothing to do.
    if ( iPlan.Count() == 0 )
        {
        return;
        }
    
    // Create a new CStack. Second template parameter ETrue means that
    // CStack owns the stack entries.
    CStack<TTransitionStackEntry, ETrue>* stack = 
        new( ELeave )CStack<TTransitionStackEntry, ETrue>;
    
    CleanupStack::PushL( stack );

    // create a root entry
    AddRootSuiteIfMissingL();
    
    // In this loop, iPlan.Count() cannot be cached because new item may be
    // added within the loop
    TInt i = 0;
    while ( i < iPlan.Count() )
        {
        #ifdef _DEBUG_EXEC_PLAN
        LOGSTRING2( "CDiagPluginExecPlanImpl::InsertSuiteTransitionsL: ------ %d", i )
        LogPlanL();
        #endif // _DEBUG_EXEC_PLAN
            
        // Case 1
        // If current item is a suite prepare, push a new level into stack.
        if ( iPlan[i]->Type() == CDiagExecPlanEntryImpl::ETypeSuitePrepare )
            {
            // we are entering a new suite. Push current plan item into stack.
            PushNewLevelL( *stack, i );
            i++;
            continue; //lint !e960  continue OK. examine next item.
            }

        // Case 2
        // If current item is a suite finalize, pop a level from stack.
        if ( iPlan[i]->Type() == CDiagExecPlanEntryImpl::ETypeSuiteFinalize && 
                stack->Head() != NULL &&
                stack->Head()->iLevelUid == iPlan[i]->Plugin().Uid() )
            {
            // we are leaving a suite. Pop top item from suite.
            PopLevel( *stack );
            i++;
            continue; //lint !e960 : continue OK. examine next item.
            }

        // it was neither prepare or finalize. Examine item as current level.
        const TTransitionStackEntry& level = *(stack->Head()); // Peek

        TUid newLevelUid = iPlan[i]->Plugin().ParentUid();

        // Case 3
        if ( newLevelUid == level.iLevelUid )
            {
            // still in the same suite level.  No need to add new items.

            // check if current item is non-dependent.
            // If so, update the iAsDependent to reflect that it includes
            // a non-dependent item.
            if ( !(iPlan[i]->AsDependency()) )
                {
                // update the prepare item, indicating that it has
                // an non-dependent item
                iPlan[level.iPrepareIndex]->SetAsDependency( EFalse );
                }
            i++;
            continue;   //lint !e960 : continue OK. examine next item.
            }

        // If parent UID is differnt from current level uid, 
        // it can mean one of the following:
        //  A ) We are entering a new suite.
        //  B ) We leaving a suite.
        // Case B ) can be deteced by checking whether the new parent is 
        // already in the stack somewhere (stack will be popped until we are
        // in the same level), or if there is no more item of the same
        // level is in the plan, which means it is not needed.
        // If case B ) fails, assume case A.
        if ( IsLevelInStack( *stack, newLevelUid ) ||
             !IsThisSuiteNeededAfterThisIndex( stack->Head()->iLevelUid, i ) )
            {
            // Case B) Insert a new Finalize item.
            AddSuiteFinalizeL( level.iPrepareIndex, i );
            }
        else
            {
            // Case A) Insert a new prepare item.
            AddSuitePrepareL( newLevelUid, i );
            }
        }

    // When all done, stack must be empty.
    __ASSERT_DEBUG( stack->IsEmpty(), Panic( EDiagFrameworkInternal ) );

    CleanupStack::PopAndDestroy( stack );
    stack = NULL;
    }

// ---------------------------------------------------------------------------
// CDiagPluginExecPlanImpl::AddRootSuiteIfMissingL
// ---------------------------------------------------------------------------
//
void CDiagPluginExecPlanImpl::AddRootSuiteIfMissingL()
    {
    // Check to see if root element is already in the plan.
    if ( iPlan[0]->Plugin().Uid() != KDiagRootSuiteUid )
        {
        // root( uid 0 ) does not exist in the plan.
        // Add prepare and finalize root into the plan.
        MDiagPlugin& rootPlugin = iEngine.PluginPool().FindPluginL( KDiagRootSuiteUid );

        CDiagExecPlanEntryImplSuite* rootEntry = 
            CDiagExecPlanEntryImplSuite::NewLC(
                iEngine,
                iEngineConfig,
                iPlanEntryObserver,
                static_cast< MDiagSuitePlugin& >( rootPlugin ),
                ETrue,              // iAsDependent == ETrue. Updated as later.
                CDiagExecPlanEntryImpl::ETypeSuitePrepare );

        iPlan.InsertL( static_cast< CDiagExecPlanEntryImpl* >( rootEntry ), 0 );  
        CleanupStack::Pop( rootEntry ); // ownership transferred above.
        rootEntry = NULL;

        rootEntry = CDiagExecPlanEntryImplSuite::NewLC(
            iEngine,
            iEngineConfig,
            iPlanEntryObserver,
            static_cast< MDiagSuitePlugin& >( rootPlugin ),
            ETrue,              // iAsDependent == ETrue. Updated as later.
            CDiagExecPlanEntryImpl::ETypeSuiteFinalize );

        // insert same entry with finalize as type
        iPlan.AppendL( static_cast< CDiagExecPlanEntryImpl* >( rootEntry ) );
        CleanupStack::Pop( rootEntry ); // ownership transferred above
        rootEntry = NULL;
        }
    }

// ---------------------------------------------------------------------------
// CDiagPluginExecPlanImpl::PushNewLevelL
// ---------------------------------------------------------------------------
//
void CDiagPluginExecPlanImpl::PushNewLevelL( 
        CStack<TTransitionStackEntry, ETrue>& aStack, 
        TInt aHeadIndex )
    {
    TTransitionStackEntry* level = new( ELeave )TTransitionStackEntry;

    __ASSERT_ALWAYS( aHeadIndex >= 0 && aHeadIndex < iPlan.Count(),
                     Panic( EDiagFrameworkInternal ) );

    level->iLevelUid = iPlan[aHeadIndex]->Plugin().Uid();
    level->iPrepareIndex = aHeadIndex;

    #ifdef _DEBUG_EXEC_PLAN
    LOGSTRING2( "CDiagPluginExecPlanImpl:: Push 0x%08x", level->iLevelUid.iUid )
    #endif // _DEBUG_EXEC_PLAN

    aStack.PushL( level ); // owership transferred.
    level = NULL; //lint !e423 Ownership transferred. No leak here.
    }

// ---------------------------------------------------------------------------
// CDiagPluginExecPlanImpl::PopLevel
// ---------------------------------------------------------------------------
//
void CDiagPluginExecPlanImpl::PopLevel( CStack<TTransitionStackEntry, ETrue>& aStack )
    {
    TTransitionStackEntry* level = aStack.Pop();
    #ifdef _DEBUG_EXEC_PLAN
    LOGSTRING2( "CDiagPluginExecPlanImpl:: Pop 0x%08x", level->iLevelUid.iUid )
    #endif // _DEBUG_EXEC_PLAN
    delete level;
    level = NULL;
    }

// ---------------------------------------------------------------------------
// CDiagPluginExecPlanImpl::IsLevelInStack
// ---------------------------------------------------------------------------
//
TBool CDiagPluginExecPlanImpl::IsLevelInStack( 
        CStack<TTransitionStackEntry, ETrue>& aStack,
        TUid aLevelUid ) const
    {
    for ( TInt i = 0; i < aStack.Count(); i++ )
        {
        const TTransitionStackEntry& currEntry = *aStack[i];
        if ( currEntry.iLevelUid == aLevelUid )
            {
            return ETrue;
            }
        }

    return EFalse;
    }

// ---------------------------------------------------------------------------
// CDiagPluginExecPlanImpl::IsThisSuiteNeededAfterThisIndex
// ---------------------------------------------------------------------------
//
TBool CDiagPluginExecPlanImpl::IsThisSuiteNeededAfterThisIndex(
        TUid aSuiteUid,
        TInt aIndex ) const
    {
    for ( TInt i = aIndex; i < iPlan.Count(); i++ )
        {
        if ( iPlan[i]->Plugin().Uid() == aSuiteUid ||  // finalize already exists
             iPlan[i]->Plugin().ParentUid() == aSuiteUid )
            {
            return ETrue;
            }
        }

    return EFalse;
    }

// ---------------------------------------------------------------------------
// CDiagPluginExecPlanImpl::AddSuitePrepareL
// ---------------------------------------------------------------------------
//
void CDiagPluginExecPlanImpl::AddSuitePrepareL( TUid aLevelUid, TInt aAt )
    {
    MDiagPlugin& suitePlugin = iEngine.PluginPool().FindPluginL( aLevelUid );

    CDiagExecPlanEntryImplSuite* prepareEntry =
        CDiagExecPlanEntryImplSuite::NewLC( 
            iEngine,
            iEngineConfig,
            iPlanEntryObserver,
            static_cast< MDiagSuitePlugin& >( suitePlugin ),
            ETrue, // iAsDependent. Updated as later if not true.
            CDiagExecPlanEntryImpl::ETypeSuitePrepare );

    iPlan.InsertL( static_cast< CDiagExecPlanEntryImpl* >( prepareEntry ), aAt );
    CleanupStack::Pop( prepareEntry ); // ownership transfer
    prepareEntry = NULL;

    #ifdef _DEBUG_EXEC_PLAN
    LOGSTRING2( "CDiagPluginExecPlanImpl:: InsertPrepare 0x%08x", aLevelUid )
    #endif // _DEBUG_EXEC_PLAN
    }

// ---------------------------------------------------------------------------
// CDiagPluginExecPlanImpl::AddSuiteFinalizeL
// ---------------------------------------------------------------------------
//
void CDiagPluginExecPlanImpl::AddSuiteFinalizeL( TInt aPrepareIndex, TInt aAt )
    {
    // It is identical to the prepare entry, except that type is ETypeSuiteFinalize
    __ASSERT_ALWAYS( aPrepareIndex >= 0 && aPrepareIndex < iPlan.Count(),
                     Panic( EDiagFrameworkInternal ) );

    CDiagExecPlanEntryImplSuite& prepareEntry = 
        static_cast< CDiagExecPlanEntryImplSuite& > ( *(iPlan[aPrepareIndex]) );

    CDiagExecPlanEntryImplSuite* finalizeEntry = 
        CDiagExecPlanEntryImplSuite::NewLC( 
            iEngine,
            iEngineConfig,
            iPlanEntryObserver,
            prepareEntry.SuitePlugin(),
            prepareEntry.AsDependency(),
            CDiagExecPlanEntryImpl::ETypeSuiteFinalize );

    iPlan.InsertL( static_cast< CDiagExecPlanEntryImpl* >( finalizeEntry ), aAt );
    CleanupStack::Pop( finalizeEntry );  // ownership transmitted above
    finalizeEntry = NULL;

    #ifdef _DEBUG_EXEC_PLAN
    LOGSTRING2( "CDiagPluginExecPlanImpl:: InsertFinalize 0x%08x", prepareEntry.Plugin().Uid().iUid );
    #endif // _DEBUG_EXEC_PLAN
    }

// ---------------------------------------------------------------------------
// CDiagPluginExecPlanImpl::StoreNextTestPluginToDbL
// ---------------------------------------------------------------------------
//
void CDiagPluginExecPlanImpl::StoreNextTestPluginToDbL()
    {
    // STEP_6. Store plan to DB. 
    // In this step, all test entries must be stored in db with EQueuedToRun
    // status. Since DbRecord().LogTestResult() is an async call, only one
    // item can be written at a time. 
    //
    // To do this, this function will search for the next "TEST" plug-in that
    // needs to be logged to DB. Once it is found, it will call async function
    // LogTestResult() and wait for RunL() to execute again.
    // In RunL(), StoreNextTestPluginToDbL() is called again, and it will look for the
    // next test plugin to store until it loops through all items in plan.
    while ( MoveCursorToNext() )
        {
        if ( CurrentExecutionItem().Plugin().Type() == MDiagPlugin::ETypeTestPlugin )
            {
            LOGSTRING2( "CDiagPluginExecPlanImpl::RunL: STEP_6 - storing item 0x%08x",
                CurrentExecutionItem().Plugin().Uid().iUid )

            CDiagResultsDatabaseItem* resultItem = 
                CDiagResultsDbItemBuilder::CreateSimpleDbItemL( 
                    CurrentExecutionItem().Plugin().Uid(),
                    CurrentExecutionItem().AsDependency(),
                    CDiagResultsDatabaseItem::EQueuedToRun );

            // Record initial test result to db.
            // StoreNextTestPluginToDbL() will be called again later from RunL()
            iEngine.DbRecord().LogTestResult( iStatus, *resultItem );
            SetActive();

            delete resultItem;
            resultItem = NULL;

            // Exit here since we must wait for LogTestResult() to complete
            // before moving on to the next item.
            return;
            }
        }

    // All items are stored now.
    ChangeState( EStatePlanCreated );
    }

// ---------------------------------------------------------------------------
// CDiagPluginExecPlanImpl::PrependExecutedItemsL
// ---------------------------------------------------------------------------
//
void CDiagPluginExecPlanImpl::PrependExecutedItemsL()
    {
    #ifdef _DEBUG_EXEC_PLAN
    LOGSTRING( "CDiagExecPlanEntryImpl::PrependExecutedItemsL(). "
        L" Before prepending executed entries" );
    LogPlanL();
    #endif // _DEBUG_EXEC_PLAN

    for ( TInt lastIndex = iExecutedEntries.Count() - 1;
          lastIndex >= 0 ;
          lastIndex-- )
        {
        // Insert last entry from the executed list to the beginning.
        iPlan.InsertL( iExecutedEntries[ lastIndex ], 0 );
        iExecutedEntries.Remove( lastIndex );
        }
    }

// ---------------------------------------------------------------------------
// CDiagPluginExecPlanImpl::ReportResult
// ---------------------------------------------------------------------------
//
void CDiagPluginExecPlanImpl::ReportResult( TInt aError )
    {
    User::RequestComplete( iClientStatus, aError );
    iClientStatus = NULL;
    }

// ---------------------------------------------------------------------------
// CDiagPluginExecPlanImpl::InsertPluginL
// ---------------------------------------------------------------------------
//
TBool CDiagPluginExecPlanImpl::InsertPluginL( MDiagPlugin& aPlugin,
                                              TBool aAsDependency,
                                              TInt aAt )
    {
    #ifdef _DEBUG_EXEC_PLAN
    LOGSTRING4( "CDiagPluginExecPlanImpl::InsertPluginL:"
            L"Id = 0x%x, Type = %s, At %d", 
            aPlugin.Uid().iUid,  
            ( aPlugin.Type() == MDiagPlugin::ETypeTestPlugin ? L"Test" : L"Suite" ),
            aAt )
    #endif // _DEBUG_EXEC_PLAN

    __ASSERT_ALWAYS( aAt >= 0 && aAt <= iPlan.Count(), Panic( EDiagFrameworkArrayBounds ) );

    // First, examine items before given index.
    // If new item has iAsDependent == ETrue, and also found before the given index,
    //   --> it is okay to not insert the item, since it will just be duplicates.
    // if new item has iAsDependent == EFalse, 
    //   --> no need to check for items prior to current position, since
    //   explicit
    // where it was requested.
    TInt idx = 0;
    if ( aAsDependency )
        {
        // check if it was already executed in the previous session.
        for ( idx = 0; idx < iExecutedEntries.Count(); idx++ )
            {
            if ( iExecutedEntries[idx]->Plugin().Uid() == aPlugin.Uid() )
                {
                // matching item found.
                // No need to instert a new item.
                return EFalse;
                }
            }

        // check for items in current plan.
        for ( idx = 0; idx < aAt && idx < iPlan.Count(); idx++ )
            {
            if ( iPlan[idx]->Plugin().Uid() == aPlugin.Uid() )
                {
                // Matching item found. 
                // No change to plan made.
                return EFalse;
                }
            }
        }

    // Duplicate item not found before given index.
    // An item will be inserted at the given index. Now, we search for
    // duplicates after current index, and see if we can remove them.
    idx = aAt;
    while ( idx < iPlan.Count() )
        {
        if ( iPlan[idx]->Plugin().Uid() == aPlugin.Uid() && iPlan[idx]->AsDependency() )
            {
            // Item is found, and it was for dependency.
            // We can move this item to current position. 
            // For now, we just need to remove it, since it will be added at
            // current position when for loop is done.
            CDiagExecPlanEntryImpl* entry = iPlan[idx];
            iPlan.Remove( idx );
            delete entry;
            entry = NULL;

            // no need to increment idx, since current item is removed.
            // idx should already be at the next item.
            }
        else
            {
            idx++;     // check next item.
            }
        }

    // Either plug-in is not found, or we found one that we can move.
    // Add to current position.
    CDiagExecPlanEntryImpl* newEntry = CreateDefaultPlanEntryLC(
        aPlugin,
        aAsDependency );

    iPlan.InsertL( newEntry, aAt );
    CleanupStack::Pop( newEntry );
    newEntry = NULL;

    return ETrue;
    }

// ---------------------------------------------------------------------------
// CDiagPluginExecPlanImpl::CreateDefaultPlanEntryL
//
// ---------------------------------------------------------------------------
//
CDiagExecPlanEntryImpl* CDiagPluginExecPlanImpl::CreateDefaultPlanEntryLC(
        MDiagPlugin& aPlugin,
        TBool aAsDependency ) const
    {
    CDiagExecPlanEntryImpl* newEntry = NULL;

    if ( aPlugin.Type() == MDiagPlugin::ETypeTestPlugin )
        {
        newEntry = static_cast< CDiagExecPlanEntryImpl* >(
            CDiagExecPlanEntryImplTest::NewLC(
                iEngine,
                iEngineConfig,
                iPlanEntryObserver,
                static_cast< MDiagTestPlugin& >( aPlugin ),
                aAsDependency,
                CDiagResultsDatabaseItem::EQueuedToRun ) );
        }
    else
        {
        newEntry = static_cast< CDiagExecPlanEntryImpl* >(
            CDiagExecPlanEntryImplSuite::NewLC(
                iEngine,
                iEngineConfig,
                iPlanEntryObserver,
                static_cast< MDiagSuitePlugin& >( aPlugin ),
                aAsDependency,
                CDiagExecPlanEntryImpl::ETypeSuiteUnexpanded ) );
        }

    return newEntry;
    }

// ---------------------------------------------------------------------------
// CDiagPluginExecPlanImpl::LogPlan
// ---------------------------------------------------------------------------
//
void CDiagPluginExecPlanImpl::LogPlanL() const
    {
    #ifdef _DEBUG
    LOGSTRING( "CDiagPluginExecPlanImpl::LogPlan(): Plan Dump." )
    for ( TInt i = 0; i < iPlan.Count(); i++ )
        {            
        HBufC* pluginName = iPlan[i]->Plugin().GetPluginNameL( 
            MDiagPlugin::ENameLayoutListSingle );

        LOGSTRING5( "Plan Entry: Id = 0x%08x, %s, Dep=[%s], Name = %S",
            iPlan[i]->Plugin().Uid().iUid,  
            ( iPlan[i]->Type() == CDiagExecPlanEntryImpl::ETypeSuitePrepare ? L"<" : 
                ( iPlan[i]->Type() == CDiagExecPlanEntryImpl::ETypeSuiteFinalize ? L">" : L" " ) ),
            ( iPlan[i]->AsDependency() ? L"*" : L" " ),
            pluginName )

        delete pluginName;
        }
    #endif // _DEBUG
    }

// ---------------------------------------------------------------------------
// CDiagPluginExecPlanImpl::LogPlanInRecordL
// ---------------------------------------------------------------------------
//
void CDiagPluginExecPlanImpl::LogPlanInRecordL() const
    {
    #ifdef _DEBUG

    LOGSTRING( "CDiagPluginExecPlanImpl::LogPlanInRecord()" )

    RPointerArray< CDiagResultsDatabaseItem > results;
    DiagFwInternal::CleanupRPointerArrayPushL< CDiagResultsDatabaseItem >( &results );
    User::LeaveIfError(
        iEngine.DbRecord().GetTestResults( results ) );

    TInt resultCount = results.Count();

    LOGSTRING2( "  Result Count = %d", resultCount )

    for ( TInt i = 0; i < resultCount; i++ )
        {
        LOGSTRING4( "   Test UID = 0x%08x, Result = %d, Dep = %d", 
            results[i]->TestUid().iUid,
            results[i]->TestResult(),
            results[i]->WasDependency() )
        }

    CleanupStack::PopAndDestroy( &results );

    #endif // _DEBUG
    }

// End of File

