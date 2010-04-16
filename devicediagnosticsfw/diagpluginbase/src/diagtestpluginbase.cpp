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
* Description:  Class definition of CDiagTestPluginBase
*
*/


// CLASS DECLARATION
#include <DiagTestPluginBase.h>

// SYSTEM INCLUDE FILES
#include <coemain.h>                        // CCoeEnv::Static()
#include <DiagTestExecParam.h>              // TDiagTestExecParam
#include <DiagResultsDbItemBuilder.h>       // CDiagResultsDbItemBuilder
#include <DiagTestObserver.h>               // MDiagTestObserver
#include <DiagPluginConstructionParam.h>    // CDiagPluginConstructionParam
#include <DiagFrameworkDebug.h>             // LOGSTRING
#include <DiagResultDetailBasic.h>          // CDiagResultDetailBasic
#include <DiagEngineCommon.h>               // MDiagEngineCommon
#include <DiagPluginPool.h>                 // CDiagPluginPool
#include <DiagResultsDatabase.h>            // RDiagResultsDatabase
#include <DiagPluginWaitingDialogWrapper.h> // CDiagPluginWaitingDialogWrapper
#include <DiagPluginExecPlan.h>             // MDiagPluginExecPlan
#include <DiagSuitePlugin.h>                // MDiagSuitePlugin
// ADO & Platformization Changes
#include <avkon.hrh>						// EAknSoftkeyYes

// USER INCLUDE FILES
#include "diagpluginbaseutils.h"            // DiagPluginBaseUtils
#include "diagpluginbase.pan"               // Panic Codes

// CONSTANTS

// priority of result value. Entries that are earlier have higher priority
static const CDiagResultsDatabaseItem::TResult KResultPriority[] = 
    {
    CDiagResultsDatabaseItem::EQueuedToRun,
    CDiagResultsDatabaseItem::EDependencyFailed,
    CDiagResultsDatabaseItem::EFailed,       
    CDiagResultsDatabaseItem::ECancelled,    
    CDiagResultsDatabaseItem::ESkipped,      
    CDiagResultsDatabaseItem::EInterrupted,  
    CDiagResultsDatabaseItem::EWatchdogCancel,
    CDiagResultsDatabaseItem::ENotPerformed, 
    CDiagResultsDatabaseItem::ESuspended,    
    CDiagResultsDatabaseItem::ESuccess
    };

static const TInt KResultPriorityCount = 
    sizeof( KResultPriority ) / sizeof( CDiagResultsDatabaseItem::TResult );

// LOCAL TYPES
class CDiagTestPluginBase::TPrivateData
    {
public:     // Constructor
    // Note that this is not a C-class, and ALL member must be explicitly initialized.
    TPrivateData( CCoeEnv& aCoeEnv )
        :   iDtorIdKey( TUid::Null() ),
            iPluginResourceLoader( aCoeEnv ),
            iWaitingDialogWrapper( NULL ),
            iResultBuilder( NULL ),
            iExecParam( NULL ),
            iCustomParam( NULL ),
            iDependencyCheckSkipped( EFalse ),
            iDependencyExecution( EFalse ),
            iSinglePluginExecution( EFalse ),
            iWatchdogResultType( CDiagResultsDatabaseItem::EWatchdogCancel )
        {}
    
public:
    /**
    * ECOM Destructor key.
    */
    TUid iDtorIdKey;

    /**
    * Resource loader for derived class resource.
    */
    RConeResourceLoader iPluginResourceLoader;

    /**
    * Wrapper class for displaying waiting dialogs.
    * Ownership: Shared. Normally, dialog will dismiss itself. However,
    * if plug-in is being deleted, it can be deleted by the plug-in as well.
    */
    CDiagPluginWaitingDialogWrapper* iWaitingDialogWrapper;

    /**
    * Results Db Item Builder. This will be used to build the result db item.
    * Owneship: this.
    */
    CDiagResultsDbItemBuilder*  iResultBuilder;

    /**
    * Test execution parameter.
    * Ownership: this.
    */
    TDiagTestExecParam* iExecParam;

    /**
    * Custom test execution parameter
    * Ownership: Does not own. Owned by application.
    */
    TAny* iCustomParam;

    /**
    * This indicates whether this test session has skipped dependency check.
    */
    TBool iDependencyCheckSkipped;

    /**
    * This indicates whether this test is being executed to satisfy dependency.
    */
    TBool iDependencyExecution;

    /**
    * Indicates if the plugin is run as a single plugin
    * or as a part of suite
    */
    TBool iSinglePluginExecution;

    /**
    * Result type to use when watchdog is timed out.
    */
    CDiagResultsDatabaseItem::TResult iWatchdogResultType;
    };

// ======== LOCAL FUNCTIONS ========

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// CDiagTestPluginBase::CDiagTestPluginBase
// ---------------------------------------------------------------------------
//
EXPORT_C CDiagTestPluginBase::CDiagTestPluginBase( 
        CDiagPluginConstructionParam* aParam )
    :   CActive( EPriorityStandard ),
        iConstructionParam( aParam ),
        iCoeEnv ( *CCoeEnv::Static() )
    {
    __ASSERT_ALWAYS( aParam, Panic( EDiagPluginBasePanicBadArgument ) );

    CActiveScheduler::Add(this);
    }

// ---------------------------------------------------------------------------
// CDiagTestPluginBase::BaseConstructL
// ---------------------------------------------------------------------------
//
EXPORT_C void CDiagTestPluginBase::BaseConstructL( const TDesC& aResourceFileName )
    {
    iData = new ( ELeave ) TPrivateData( CoeEnv() );
        
    DiagPluginBaseUtils::OpenResourceFileL(
        aResourceFileName, iData->iPluginResourceLoader, CoeEnv().FsSession() );
    }


// ---------------------------------------------------------------------------
// CDiagTestPluginBase::~CDiagTestPluginBase
// ---------------------------------------------------------------------------
//
EXPORT_C CDiagTestPluginBase::~CDiagTestPluginBase()
    {
    if ( iData )
        {
        // In case test was still running, clean up.
        // It takes care of:
        //  iData->iWaitingDialog
        //  iData->iIsDialogDismessedByUserResponsePtr
        //  iData->iResultBuilder
        //  iData->iExecParam
        // It must be trapped, since it is part of object destruction.
        TRAPD( err, StopAndCleanupL() );
        if ( err != KErrNone )
            {
            /*LOGSTRING3( "CDiagTestPluginBase::~CDiagTestPluginBase(). "
                L"StopAndCleaupL() failed Uid = 0x%08x , err = %d",
                Uid(),
                err );*/
            }

        // Call DestroyedImplementation only if it was set.
        // SetDtorIdKey may not have been called if it is not actually an
        // ECOM plug-in.
        if ( iData->iDtorIdKey != TUid::Null() )
            {
            REComSession::DestroyedImplementation( iData->iDtorIdKey );
            }

        iData->iPluginResourceLoader.Close();

        delete iData;
        iData = NULL;
        }

    delete iConstructionParam;
    iConstructionParam = NULL;
    }

// ---------------------------------------------------------------------------
// From CActive
// CDiagTestPluginBase::RunError
// ---------------------------------------------------------------------------
//
EXPORT_C TInt CDiagTestPluginBase::RunError( TInt aError )
    {
    LOGSTRING2( "CDiagTestPluginBase::RunError( %d )", aError )
    
    // Check if we are executing a test.
    if ( iData->iExecParam != NULL )
        {
        // Catch error from CompleteTestL. If there is an error while calling
        // CompleteTestL(), return the error to active scheduler.
        aError = KErrNone;
        TRAP( aError, CompleteTestL( CDiagResultsDatabaseItem::EFailed ) );
        }

    return aError;
    }

// ---------------------------------------------------------------------------
// From MDiagTestPlugin
// CDiagTestPluginBase::GetServiceLogicalNameL
// ---------------------------------------------------------------------------
//
EXPORT_C const TDesC& CDiagTestPluginBase::ServiceLogicalName() const
    {
    return iConstructionParam->ServiceProvided();
    }


// ---------------------------------------------------------------------------
// From MDiagTestPlugin
// CDiagTestPluginBase::GetLogicalDependenciesL
// ---------------------------------------------------------------------------
//
EXPORT_C void CDiagTestPluginBase::GetLogicalDependenciesL( 
        CPtrCArray& aArray ) const
    {       
    aArray.CopyL( iConstructionParam->ServicesRequired() );
    }


// ---------------------------------------------------------------------------
// From MDiagTestPlugin
// CDiagTestPluginBase::Type
// ---------------------------------------------------------------------------
//
EXPORT_C MDiagPlugin::TPluginType CDiagTestPluginBase::Type() const
    {
    return ETypeTestPlugin;
    }

// ---------------------------------------------------------------------------
// From MDiagTestPlugin
// CDiagTestPluginBase::CreateIconL
// ---------------------------------------------------------------------------
//
EXPORT_C CGulIcon* CDiagTestPluginBase::CreateIconL() const
    {
    return NULL;
    }

// ---------------------------------------------------------------------------
// From MDiagTestPlugin
// CDiagTestPluginBase::IsSupported
// ---------------------------------------------------------------------------
//
EXPORT_C TBool CDiagTestPluginBase::IsSupported() const
    {
    return ETrue;
    }

// ---------------------------------------------------------------------------
// From MDiagTestPlugin
// CDiagTestPluginBase::ParentUid
// ---------------------------------------------------------------------------
//
EXPORT_C TUid CDiagTestPluginBase::ParentUid() const
    {
    return iConstructionParam->ParentUid();
    }

// ---------------------------------------------------------------------------
// From MDiagTestPlugin
// CDiagTestPluginBase::Order
// ---------------------------------------------------------------------------
//
EXPORT_C TUint CDiagTestPluginBase::Order() const
    {
    return iConstructionParam->Order();
    }


// ---------------------------------------------------------------------------
// From MDiagTestPlugin
// CDiagTestPluginBase::SetDtorIdKey
// ---------------------------------------------------------------------------
//
EXPORT_C void CDiagTestPluginBase::SetDtorIdKey( TUid aDtorIdKey )
    {
    __ASSERT_ALWAYS( iData, Panic( EDiagPluginBasePanicInvalidState ) );
    LOGSTRING3( "CDiagTestPluginBase::SetDtorIdKey: Old=0x%x, New=0x%x",
        iData->iDtorIdKey.iUid, aDtorIdKey.iUid )
    iData->iDtorIdKey = aDtorIdKey;
    }

// ---------------------------------------------------------------------------
// From MDiagTestPlugin
// CDiagTestPluginBase::GetTitleL
// ---------------------------------------------------------------------------
//
EXPORT_C HBufC* CDiagTestPluginBase::GetTitleL() const
    {
    // Currently, GetTitleL() is not supported.
    User::Leave( KErrNotSupported );
    return NULL;
    }

// ---------------------------------------------------------------------------
// From MDiagTestPlugin
// CDiagTestPluginBase::GetDescriptionL
// ---------------------------------------------------------------------------
//
EXPORT_C HBufC* CDiagTestPluginBase::GetDescriptionL() const
    {
    // Currently, GetDescriptionL() is not supported.
    User::Leave( KErrNotSupported );
    return NULL;
    }

// ---------------------------------------------------------------------------
// From MDiagTestPlugin
// CDiagTestPluginBase::CustomOperationL
// ---------------------------------------------------------------------------
//
EXPORT_C TAny* CDiagTestPluginBase::CustomOperationL( TUid /* aUid */, 
        TAny* /* aParam */ ) 
    {
    LOGSTRING( "CDiagTestPluginBase::CustomOperationL: KErrNotSupported" )
    User::Leave( KErrNotSupported );
    return NULL;
    }



// ---------------------------------------------------------------------------
// From MDiagTestPlugin
// CDiagTestPluginBase::GetCustomL
// ---------------------------------------------------------------------------
//
EXPORT_C TAny* CDiagTestPluginBase::GetCustomL( TUid /* aUid*/,
                                                TAny* /* aParam */ )
    {
    LOGSTRING( "CDiagTestPluginBase::GetCustomL: KErrNotSupported" )
    User::Leave( KErrNotSupported );
    return NULL;
    }


// ---------------------------------------------------------------------------
// From MDiagTestPlugin
// CDiagTestPluginBase::TestSessionBeginL
// ---------------------------------------------------------------------------
//
EXPORT_C void CDiagTestPluginBase::TestSessionBeginL( 
        MDiagEngineCommon& /* aEngine */,
        TBool /* aSkipDependencyCheck */,
        TAny* /* aCustomParams */ )
    {
    }

// ---------------------------------------------------------------------------
// From MDiagTestPlugin
// CDiagTestPluginBase::TestSessionEndL
// ---------------------------------------------------------------------------
//
EXPORT_C void CDiagTestPluginBase::TestSessionEndL( 
        MDiagEngineCommon& /* aEngine */,
        TBool /* aSkipDependencyCheck */,
        TAny* /* aCustomParams */ )
    {
    }

// ---------------------------------------------------------------------------
// From CDiagTestPluginBase
// CDiagTestPluginBase::RunTestL
// ---------------------------------------------------------------------------
//
EXPORT_C void CDiagTestPluginBase::RunTestL( 
        TDiagTestExecParam* aExecParam,
        TBool aSkipDependencyCheck,
        TBool aDependencyExecution,
        TAny* aCustomParam ) 
    {
    __ASSERT_ALWAYS( iData, Panic( EDiagPluginBasePanicInvalidState ) );
    __ASSERT_DEBUG( iData->iExecParam == NULL, Panic( EDiagPluginBasePanicInternal ) );
    __ASSERT_DEBUG( iData->iResultBuilder == NULL, Panic( EDiagPluginBasePanicInternal ) );
    delete iData->iExecParam;
    delete iData->iResultBuilder;

    iData->iExecParam = aExecParam;
    iData->iCustomParam = aCustomParam;
    iData->iDependencyCheckSkipped = aSkipDependencyCheck;
    iData->iDependencyExecution = aDependencyExecution;
    iData->iResultBuilder = CDiagResultsDbItemBuilder::NewL( Uid(), aDependencyExecution );
    ResetWatchdogToDefault();

    // checking if this is a single test execution or not 
    iData->iSinglePluginExecution = 
        ( aExecParam->Engine().ExecutionPlanL().TestCount( EFalse ) == 1 );

    // verify that dependencies are satisfied.
    RArray<TUid>* failedUids = NULL;
    CDiagResultsDatabaseItem::TResult dependencyResult = CDiagResultsDatabaseItem::ESuccess;

    if ( !aSkipDependencyCheck )
        {
        dependencyResult =  VerifyDependenciesL( 
            aExecParam->Engine(),
            failedUids );
        }
	
    if(dependencyResult == CDiagResultsDatabaseItem::ESuccess)
	LOGSTRING( "This is to remove compiler warning");
	

    if ( failedUids )
        {
        failedUids->Reset();
        failedUids->Close();
        delete failedUids;
        failedUids = NULL;
        }
    DoRunTestL();
    /*if ( dependencyResult == CDiagResultsDatabaseItem::ESuccess )
        {
        
        }
    else
        {
        // Create dependency failed test result and send it to engine.
        // Note that we can't actually call CompleteTestL(), or StopAndCleanupL() 
        // here because we haven't actually called the derived class.
        // So, reporting and clean up must be done manually.

        // Map dependent test result to what we should report.
        switch ( dependencyResult )
            {
            case CDiagResultsDatabaseItem::EFailed:
                dependencyResult = CDiagResultsDatabaseItem::EDependencyFailed;
                break;
            
            case CDiagResultsDatabaseItem::EWatchdogCancel:
            case CDiagResultsDatabaseItem::ESkipped:
            case CDiagResultsDatabaseItem::EInterrupted:
            case CDiagResultsDatabaseItem::ENotPerformed:
                dependencyResult = CDiagResultsDatabaseItem::EDependencySkipped;
                break;

            default:
                // no change is needed.
                break;
            }

        iData->iResultBuilder->SetTestCompleted( dependencyResult );

        iData->iExecParam->Observer().TestExecutionCompletedL( *this,
            iData->iResultBuilder->ToResultsDatabaseItemL() );

        BaseStopAndCleanup();
        }*/
    }

// ---------------------------------------------------------------------------
// CDiagTestPluginBase::ExecutionParam
// ---------------------------------------------------------------------------
//
EXPORT_C TDiagTestExecParam& CDiagTestPluginBase::ExecutionParam()
    {
    __ASSERT_ALWAYS( iData, Panic( EDiagPluginBasePanicInvalidState ) );
    __ASSERT_ALWAYS( iData->iExecParam, Panic( EDiagPluginBasePanicInvalidState ) );
    return *(iData->iExecParam);
    }

// ---------------------------------------------------------------------------
// CDiagTestPluginBase::IsDependencyCheckSkipped
// ---------------------------------------------------------------------------
//
EXPORT_C TBool CDiagTestPluginBase::IsDependencyCheckSkipped() const
    {
    __ASSERT_ALWAYS( iData, Panic( EDiagPluginBasePanicInvalidState ) );
    __ASSERT_ALWAYS( iData->iExecParam, Panic( EDiagPluginBasePanicInvalidState ) );
    return iData->iDependencyCheckSkipped;
    }

// ---------------------------------------------------------------------------
// CDiagTestPluginBase::IsDependencyExecution
// ---------------------------------------------------------------------------
//
EXPORT_C TBool CDiagTestPluginBase::IsDependencyExecution() const
    {
    __ASSERT_ALWAYS( iData, Panic( EDiagPluginBasePanicInvalidState ) );
    __ASSERT_ALWAYS( iData->iExecParam, Panic( EDiagPluginBasePanicInvalidState ) );
    return iData->iDependencyExecution;
    }

// ---------------------------------------------------------------------------
// CDiagTestPluginBase::CustomParam
// ---------------------------------------------------------------------------
//
EXPORT_C TAny* CDiagTestPluginBase::CustomParam() const
    {
    __ASSERT_ALWAYS( iData, Panic( EDiagPluginBasePanicInvalidState ) );
    __ASSERT_ALWAYS( iData->iExecParam, Panic( EDiagPluginBasePanicInvalidState ) );
    return iData->iCustomParam;
    }

// ---------------------------------------------------------------------------
// CDiagTestPluginBase::ResultsDbItemBuilder
// ---------------------------------------------------------------------------
//
EXPORT_C CDiagResultsDbItemBuilder& CDiagTestPluginBase::ResultsDbItemBuilder()
    {
    __ASSERT_ALWAYS( iData, Panic( EDiagPluginBasePanicInvalidState ) );
    __ASSERT_ALWAYS( iData->iResultBuilder, Panic( EDiagPluginBasePanicInvalidState ) );
    return *(iData->iResultBuilder);
    }

// ---------------------------------------------------------------------------
// CDiagTestPluginBase::ReportTestProgressL
// ---------------------------------------------------------------------------
//
EXPORT_C void CDiagTestPluginBase::ReportTestProgressL( TUint aCurrentStep )
    {
    __ASSERT_ALWAYS( iData, Panic( EDiagPluginBasePanicInvalidState ) );
    __ASSERT_ALWAYS( iData->iExecParam, Panic( EDiagPluginBasePanicInvalidState ) );

    iData->iExecParam->Observer().TestProgressL( *this, aCurrentStep );
    }

// ---------------------------------------------------------------------------
// CDiagTestPluginBase::ResetWatchdog
// ---------------------------------------------------------------------------
//
EXPORT_C void CDiagTestPluginBase::ResetWatchdog(
        TDiagEngineWatchdogTypes aWatchdogType,
        CDiagResultsDatabaseItem::TResult aWatchdogResultType )
    {
    __ASSERT_ALWAYS( iData, Panic( EDiagPluginBasePanicInvalidState ) );
    __ASSERT_ALWAYS( iData->iExecParam, Panic( EDiagPluginBasePanicInvalidState ) );

    LOGSTRING3( "CDiagTestPluginBase::ResetWatchdog() Type = %d, result = %d",
        aWatchdogType, aWatchdogResultType )

    iData->iWatchdogResultType = aWatchdogResultType;
    iData->iExecParam->Engine().ResetWatchdog( aWatchdogType );
    }

// ---------------------------------------------------------------------------
// CDiagTestPluginBase::ResetWatchdog
// ---------------------------------------------------------------------------
//
EXPORT_C void CDiagTestPluginBase::ResetWatchdog(
        TInt aTimeToCompletion,
        CDiagResultsDatabaseItem::TResult aWatchdogResultType )
    {
    __ASSERT_ALWAYS( iData, Panic( EDiagPluginBasePanicInvalidState ) );
    __ASSERT_ALWAYS( iData->iExecParam, Panic( EDiagPluginBasePanicInvalidState ) );

    LOGSTRING3( "CDiagTestPluginBase::ResetWatchdog() time = %d, result = %d",
        aTimeToCompletion, aWatchdogResultType )

    iData->iWatchdogResultType = aWatchdogResultType;
    iData->iExecParam->Engine().ResetWatchdog( aTimeToCompletion );
    }

// ---------------------------------------------------------------------------
// CDiagTestPluginBase::ResetWatchdogToDefault
// ---------------------------------------------------------------------------
//
EXPORT_C void CDiagTestPluginBase::ResetWatchdogToDefault()
    {
    __ASSERT_ALWAYS( iData, Panic( EDiagPluginBasePanicInvalidState ) );
    __ASSERT_ALWAYS( iData->iExecParam, Panic( EDiagPluginBasePanicInvalidState ) );

    if ( RunMode() == EAutomatic )
        {
        ResetWatchdog( EDiagEngineWatchdogTypeAutomatic, 
                       CDiagResultsDatabaseItem::EWatchdogCancel );
        }
    else
        {
        ResetWatchdog( EDiagEngineWatchdogTypeInteractive, 
                       CDiagResultsDatabaseItem::EWatchdogCancel );
        }
    }

// ---------------------------------------------------------------------------
// CDiagTestPluginBase::CompleteTestL
// ---------------------------------------------------------------------------
//
EXPORT_C void CDiagTestPluginBase::CompleteTestL( 
        CDiagResultsDatabaseItem::TResult aResult )
    {
    __ASSERT_ALWAYS( iData, Panic( EDiagPluginBasePanicInvalidState ) );
    __ASSERT_ALWAYS( iData->iResultBuilder, Panic( EDiagPluginBasePanicInvalidState ) );
    __ASSERT_ALWAYS( iData->iExecParam, Panic( EDiagPluginBasePanicInvalidState ) );

    iData->iResultBuilder->SetTestCompleted( aResult );
    iData->iExecParam->Observer().TestExecutionCompletedL( *this, 
        iData->iResultBuilder->ToResultsDatabaseItemL() );

    StopAndCleanupL();
    }

// ---------------------------------------------------------------------------
// CDiagTestPluginBase::StopAndCleanupL
// ---------------------------------------------------------------------------
//
EXPORT_C void CDiagTestPluginBase::StopAndCleanupL()
    {
    __ASSERT_ALWAYS( iData, Panic( EDiagPluginBasePanicInvalidState ) );
    // Cancel active request
    Cancel();

    // Clean up only if it was already running.
    if ( iData->iExecParam != NULL || iData->iResultBuilder != NULL )
        {
        // allow derived class to clean up first.
        DoStopAndCleanupL();

        BaseStopAndCleanup();
        }
    }

// ---------------------------------------------------------------------------
// from MDiagTestPlugin
// CDiagTestPluginBase::ExecutionStopL
// ---------------------------------------------------------------------------
//
EXPORT_C CDiagResultsDatabaseItem* CDiagTestPluginBase::ExecutionStopL( 
        MDiagTestPlugin::TStopReason aReason )
    {
    __ASSERT_ALWAYS( iData, Panic( EDiagPluginBasePanicInvalidState ) );
    __ASSERT_ALWAYS( iData->iResultBuilder, Panic( EDiagPluginBasePanicInvalidState ) );
    __ASSERT_ALWAYS( iData->iExecParam, Panic( EDiagPluginBasePanicInvalidState ) );

    // Determine default reason.
    CDiagResultsDatabaseItem::TResult result;

    if ( aReason == MDiagTestPlugin::EWatchdog )
        {
        result = iData->iWatchdogResultType;
        }
    else
        {
        result = CDiagResultsDatabaseItem::EInterrupted;
        }

    // Allow derived class to provide custom behavor
    DoExecutionStopL( aReason, result );

    iData->iResultBuilder->SetTestCompleted( result );

    CDiagResultsDatabaseItem* dbItem = iData->iResultBuilder->ToResultsDatabaseItemL();

    // Stop and clean up before returning to the engine.
    CleanupStack::PushL( dbItem );
    StopAndCleanupL();
    CleanupStack::Pop( dbItem );
    
    return dbItem;  // ownership transfer
    }

// ---------------------------------------------------------------------------
// From MDiagTestPlugin
// CDiagTestPluginBase::SuspendL
// ---------------------------------------------------------------------------
//
EXPORT_C void CDiagTestPluginBase::SuspendL()
    {
    LOGSTRING( "CDiagTestPluginBase::SuspendL: KErrNotSupported" )
    User::Leave( KErrNotSupported );
    }

// ---------------------------------------------------------------------------
// From MDiagTestPlugin
// CDiagTestPluginBase::ResumeL
// ---------------------------------------------------------------------------
//
EXPORT_C void CDiagTestPluginBase::ResumeL()
    {
    LOGSTRING( "CDiagTestPluginBase::ResumeL: KErrNotSupported" )
    User::Leave( KErrNotSupported );
    }

// ---------------------------------------------------------------------------
// From MDiagTestPlugin
// CDiagTestPluginBase::CreateDetailL
// ---------------------------------------------------------------------------
EXPORT_C MDiagResultDetail* CDiagTestPluginBase::CreateDetailL(
            const CDiagResultsDatabaseItem& aResult ) const        
    {
    // Use basic version by default
    return new( ELeave )CDiagResultDetailBasic( aResult.TestResult() ); 
    }


// ---------------------------------------------------------------------------
// CDiagTestPluginBase::DoExecutionStopL
// ---------------------------------------------------------------------------
//
EXPORT_C void CDiagTestPluginBase::DoExecutionStopL( 
        MDiagTestPlugin::TStopReason /* aReason */,
        CDiagResultsDatabaseItem::TResult& /* aTestResult */ )
    {
    // default implementation. Does nothing.
    }

// ---------------------------------------------------------------------------
// CDiagTestPluginBase::AreDependenciesSatisfiedL
// ---------------------------------------------------------------------------
//
EXPORT_C TBool CDiagTestPluginBase::AreDependenciesSatisfiedL(
            MDiagEngineCommon& aEngine,
            RArray<TUid>*& aFailedUids ) const
    {
    CDiagResultsDatabaseItem::TResult result = VerifyDependenciesL( aEngine, aFailedUids );

    return result == CDiagResultsDatabaseItem::ESuccess;
    }

// ---------------------------------------------------------------------------
// CDiagTestPluginBase::VerifyDependenciesL
// ---------------------------------------------------------------------------
//
EXPORT_C CDiagResultsDatabaseItem::TResult CDiagTestPluginBase::VerifyDependenciesL(
        MDiagEngineCommon& aEngine,
        RArray< TUid >*& aFailedUids ) const
    {
    // Create a failed uid list array
    RArray<TUid>* failedUids = new( ELeave )RArray<TUid>();
    CleanupStack::PushL( failedUids );  // to delete array itself.
    CleanupClosePushL( *failedUids );   // to call close.

    // First, convert dependency list to a plugin list
    RPointerArray< MDiagTestPlugin > pluginList;
    CleanupClosePushL( pluginList );    // destroy not needed since elements are not owned.

    // Expand suites.
    GetAllDependentTestsL( aEngine, pluginList );

    // Analyze result of each plug-in.
    CDiagResultsDatabaseItem::TResult result = 
        SummarizeOverallTestResultsL( aEngine, pluginList, *failedUids );

    CleanupStack::PopAndDestroy( &pluginList ); // pluginList.Close()

    if ( result == CDiagResultsDatabaseItem::ESuccess )
        {
        __ASSERT_DEBUG( failedUids->Count() == 0,
            Panic( EDiagPluginBasePanicInternal ) );
        CleanupStack::PopAndDestroy( failedUids ); // call close.
        CleanupStack::PopAndDestroy( failedUids ); // delete RArray
        failedUids = NULL;
        }
    else
        {
        __ASSERT_DEBUG( failedUids->Count() > 0,
            Panic( EDiagPluginBasePanicInternal ) );
        CleanupStack::Pop( failedUids ); // call close.
        CleanupStack::Pop( failedUids ); // delete RArray
        }

    aFailedUids = failedUids;

    return result;
    }

// ---------------------------------------------------------------------------
// CDiagTestPluginBase::GetAllDependentTestsL
// ---------------------------------------------------------------------------
//
void CDiagTestPluginBase::GetAllDependentTestsL( 
        MDiagEngineCommon& aEngine,
        RPointerArray< MDiagTestPlugin >& aPluginList ) const
    {
    TInt i = 0;

    // first create an array of dependent plug-ins.
    RPointerArray< MDiagPlugin > mixedPluginList;
    CleanupClosePushL( mixedPluginList );   // to call close.

    CPtrCArray* dependencyList = new( ELeave )CPtrCArray( 1 );
    CleanupStack::PushL( dependencyList );
    GetLogicalDependenciesL( *dependencyList );

    TInt count = dependencyList->Count();
    for ( i = 0;  i < count; i++ )
        {
        MDiagPlugin* plugin = NULL;
        User::LeaveIfError( aEngine.PluginPool().FindPlugin( ( *dependencyList )[i], plugin ) );
        mixedPluginList.AppendL( plugin );
        }

    CleanupStack::PopAndDestroy( dependencyList );
    dependencyList = NULL;

    // expand suites into tests.
    i = 0;
    while ( i < mixedPluginList.Count() )
        {
        if ( mixedPluginList[i]->Type() == ETypeTestPlugin )
            {
            //  test plug-in. move to next item.
            aPluginList.AppendL( static_cast< MDiagTestPlugin* >( mixedPluginList[i] ) );
            i++;
            }
        else
            {
            // suite. remove and replace them with its children.
            MDiagSuitePlugin& suitePlugin = 
                static_cast< MDiagSuitePlugin& >( *mixedPluginList[i] );

            mixedPluginList.Remove( i );
            
            RPointerArray<MDiagPlugin> children;
            CleanupClosePushL( children ); // destroy not needed since ownership not transferred.

            suitePlugin.GetChildrenL( children, MDiagSuitePlugin::ESortByPosition );

            TInt childCount = children.Count();
            for ( TInt childIndex = 0; childIndex < childCount; childIndex++ )
                {
                mixedPluginList.InsertL( children[childIndex], i + childIndex );
                }

            CleanupStack::PopAndDestroy( &children ); // children.Close()
            // do not increment 'i' to allow re-examination of item just inserted.
            }
        }

    CleanupStack::PopAndDestroy( &mixedPluginList ); // mixedPluginList.Close()
    }

// ---------------------------------------------------------------------------
// CDiagTestPluginBase::SummarizeOverallTestResultsL
// ---------------------------------------------------------------------------
//
CDiagResultsDatabaseItem::TResult CDiagTestPluginBase::SummarizeOverallTestResultsL(
        MDiagEngineCommon& aEngine,
        const RPointerArray< MDiagTestPlugin >& aPluginList,
        RArray< TUid >& aFailedUidList ) const
    {
    // Check result of each item.
    // and the result will override it.
    // by default, we start with ESuccess.
    CDiagResultsDatabaseItem::TResult result = CDiagResultsDatabaseItem::ESuccess;
    TInt pluginListCount = aPluginList.Count();
    for ( TInt i = 0; i < pluginListCount; i++ )
        {
        MDiagTestPlugin& plugin = *( aPluginList[i] );

        CDiagResultsDatabaseItem* resultItem = NULL;
        TInt err = aEngine.DbRecord().GetTestResult( plugin.Uid(), resultItem );

        // KErrNotFound is acceptable.
        if ( err != KErrNone && err != KErrNotFound )
            {
            delete resultItem;
            User::Leave( err );
            }

        CDiagResultsDatabaseItem::TResult newResult;
        if ( resultItem == NULL )
            {
            newResult = CDiagResultsDatabaseItem::EQueuedToRun; 
            }
        else
            {
            newResult = resultItem->TestResult();
            }
        
        delete resultItem;
        resultItem = NULL;

        // add items to failed list if not successful.
        if ( newResult != CDiagResultsDatabaseItem::ESuccess )
            {
            LOGSTRING3( "CDiagTestPluginBase::SummarizeOverallTestResultsL(): "
                L"Failed test 0x%08x, result = %d", 
                plugin.Uid().iUid,
                newResult )
            aFailedUidList.AppendL( plugin.Uid() );
            }

        // update to new result based on result priority.
        // check for condition where newResult == result. If they are equal
        // there is no reason to run the loop.
        for ( TInt priorityIndex = 0; 
              priorityIndex < KResultPriorityCount && newResult != result;
              priorityIndex++ )
            {
            if ( KResultPriority[priorityIndex] == newResult )
                {
                result = newResult;
                }
            }
        }

    LOGSTRING3( "CDiagTestPluginBase::SummarizeOverallTestResultsL(): "
        L"Plugin 0x%08x Overall depencency result = %d", 
        Uid().iUid,
        result )
    return result;
    }


// ---------------------------------------------------------------------------
// CDiagTestPluginBase::RunWaitingDialogL
// ---------------------------------------------------------------------------
EXPORT_C TBool CDiagTestPluginBase::RunWaitingDialogL( CAknDialog* aDialog, 
        TInt& aDialogResponse )
    {
    __ASSERT_ALWAYS( iData, Panic( EDiagPluginBasePanicInvalidState ) );

    // this function can display only one dialog at a time.
    __ASSERT_ALWAYS( iData->iWaitingDialogWrapper == NULL,
                     Panic( EDiagPluginBasePanicDialogAlreadyUp ) );

    iData->iWaitingDialogWrapper = CDiagPluginWaitingDialogWrapper::NewL( aDialog );

    TBool isUserResonse = iData->iWaitingDialogWrapper->RunLD( aDialogResponse );

    if ( isUserResonse )
        {
        // local variable can be accessed only if it was returned due to
        // user response.
        iData->iWaitingDialogWrapper = NULL;
        }

    return isUserResonse;
    }

// ---------------------------------------------------------------------------
// CDiagTestPluginBase::DismissWaitingDialog
// ---------------------------------------------------------------------------
EXPORT_C void CDiagTestPluginBase::DismissWaitingDialog()
    {
    __ASSERT_ALWAYS( iData, Panic( EDiagPluginBasePanicInvalidState ) );

    if ( iData->iWaitingDialogWrapper )
        {
        LOGSTRING( "CDiagTestPluginBase::DismissWaitingDialog() Dialog dismissed." )
        delete iData->iWaitingDialogWrapper;
        iData->iWaitingDialogWrapper = NULL;
        }
    }

// ---------------------------------------------------------------------------
// CDiagTestPluginBase::CoeEnv
// ---------------------------------------------------------------------------
//
EXPORT_C CCoeEnv& CDiagTestPluginBase::CoeEnv() 
    {
    return iCoeEnv;
    }

// ---------------------------------------------------------------------------
// CDiagTestPluginBase::SinglePluginExecution
// ---------------------------------------------------------------------------
//
EXPORT_C TBool CDiagTestPluginBase::SinglePluginExecution() const
    {
    __ASSERT_ALWAYS( iData, Panic( EDiagPluginBasePanicInvalidState ) );
    return iData->iSinglePluginExecution;
    }

// ---------------------------------------------------------------------------
// CDiagTestPluginBase::BaseStopAndCleanup
// ---------------------------------------------------------------------------
//
void CDiagTestPluginBase::BaseStopAndCleanup()
    {
    DismissWaitingDialog();

    delete iData->iResultBuilder;
    iData->iResultBuilder = NULL;

    delete iData->iExecParam;
    iData->iExecParam = NULL;

    iData->iCustomParam = NULL;
    }

// ADO & Platformization Changes
/*
    
// ---------------------------------------------------------------------------
// CDiagSpeakerPlugin::AskCancelExecutionL
// ---------------------------------------------------------------------------
//
EXPORT_C TInt CDiagTestPluginBase::AskCancelExecutionL( TInt& aButtonId )
    {
    LOGSTRING( "CDiagTestPluginBase::AskCancelExecutionL() IN" )

    CAknDialog* dialog;
    TBool       result;

    // set softkey for single execution
    if ( !SinglePluginExecution() )
        {
    	// Create common dialog by invoking Engine
    	dialog = ExecutionParam().Engine().
             CreateCommonDialogLC( EDiagCommonDialogConfirmCancelAll, NULL );

    	// Launch dialog and get result from it
    	result = RunWaitingDialogL( dialog, aButtonId );
	}	
    else 
       {
       CompleteTestL( CDiagResultsDatabaseItem::ECancelled );	
       aButtonId = EAknSoftkeyYes;
       return ETrue;
       }
    
    LOGSTRING3( "CDiagTestPluginBase::AskCancelExecutionL() OUT aButtonId=%d result=%d", aButtonId, result )
    return result;
    }
    */
// End of File




