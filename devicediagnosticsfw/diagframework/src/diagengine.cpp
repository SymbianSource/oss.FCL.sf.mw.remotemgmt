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
* Description:  Class definition of CDiagEngine
*
*/


// CLASS DECLARATION
#include "diagengineimpl.h"

// SYSTEM INCLUDE FILES
#include <DiagEngine.h>

// USER INCLUDE FILES
#include "diagframework.pan"        // For Panics codes.

#include "diagpluginexecplan.h"   // ADO & Platformization Changes
#include "diagexecplanentryimpltest.h"   // ADO & Platformization Changes
// ======== LOCAL FUNCTIONS ========


// ======== MEMBER FUNCTIONS ========


// ---------------------------------------------------------------------------
// CDiagEngine::NewL()
// ---------------------------------------------------------------------------
//
EXPORT_C CDiagEngine* CDiagEngine::NewL( CAknViewAppUi&         aViewAppUi, 
                                         MDiagEngineObserver&   aObserver,
                                         RDiagResultsDatabase&  aDbSession,
                                         CDiagPluginPool&       aPluginPool,
                                         TBool                  aDisableDependency,
                                         const RArray< TUid >&  aExecutionBatch )
    {
    CDiagEngine* self = CDiagEngine::NewLC( aViewAppUi,
                                            aObserver,
                                            aDbSession,
                                            aPluginPool,
                                            aDisableDependency,
                                            aExecutionBatch );

    CleanupStack::Pop( self );
    return self;
    }


// ---------------------------------------------------------------------------
// CDiagEngine::NewLC()
// ---------------------------------------------------------------------------
//
EXPORT_C CDiagEngine* CDiagEngine::NewLC( CAknViewAppUi&         aViewAppUi, 
                                          MDiagEngineObserver&   aObserver,
                                          RDiagResultsDatabase&  aDbSession,
                                          CDiagPluginPool&       aPluginPool,
                                          TBool                  aDisableDependency,
                                          const RArray< TUid >&  aExecutionBatch )
    {
    CDiagEngine* self = new ( ELeave ) CDiagEngine();
    CleanupStack::PushL( self );

    self->iEngineImpl = CDiagEngineImpl::NewL( aViewAppUi,
                                               aObserver,
                                               aDbSession,
                                               aPluginPool,
                                               aDisableDependency,
                                               aExecutionBatch );
    return self;
    }

// ---------------------------------------------------------------------------
// CDiagEngine::NewL()
// ---------------------------------------------------------------------------
//
EXPORT_C CDiagEngine* CDiagEngine::NewL( CAknViewAppUi&         aViewAppUi, 
                                         MDiagEngineObserver&   aObserver,
                                         RDiagResultsDatabase&  aDbSession,
                                         CDiagPluginPool&       aPluginPool,
                                         TUid                   aIncompleteRecordUid )
    {
    CDiagEngine* self = CDiagEngine::NewLC( aViewAppUi,
                                            aObserver,
                                            aDbSession,
                                            aPluginPool,
                                            aIncompleteRecordUid );

    CleanupStack::Pop( self );
    return self;
    }


// ---------------------------------------------------------------------------
// CDiagEngine::NewLC()
// ---------------------------------------------------------------------------
//
EXPORT_C CDiagEngine* CDiagEngine::NewLC( CAknViewAppUi&         aViewAppUi, 
                                          MDiagEngineObserver&   aObserver,
                                          RDiagResultsDatabase&  aDbSession,
                                          CDiagPluginPool&       aPluginPool,
                                          TUid                   aIncompleteRecordUid )
    {
    CDiagEngine* self = new ( ELeave ) CDiagEngine();
    CleanupStack::PushL( self );

    self->iEngineImpl = CDiagEngineImpl::NewL( aViewAppUi,
                                               aObserver,
                                               aDbSession,
                                               aPluginPool,
                                               aIncompleteRecordUid );
    return self;
    }


// ---------------------------------------------------------------------------
// CDiagEngine::~CDiagEngine
// ---------------------------------------------------------------------------
//
EXPORT_C CDiagEngine::~CDiagEngine()
    {
    delete iEngineImpl;
    iEngineImpl = NULL;
    }


// ---------------------------------------------------------------------------
// CDiagEngine::ExecuteL
// ---------------------------------------------------------------------------
//
EXPORT_C void CDiagEngine::ExecuteL()
    {
    iEngineImpl->ExecuteL();
    }

// ---------------------------------------------------------------------------
// CDiagEngine::SetCustomParam
// ---------------------------------------------------------------------------
//
EXPORT_C void CDiagEngine::SetCustomParam( TAny* aCustomParams )
    {
    __ASSERT_ALWAYS( aCustomParams, Panic( EDiagFrameworkBadArgument ) );
    iEngineImpl->SetCustomParam( aCustomParams );
    }

// ---------------------------------------------------------------------------
// CDiagEngine::SuspendL
// ---------------------------------------------------------------------------
//
EXPORT_C void CDiagEngine::SuspendL()
    {
    iEngineImpl->SuspendL();
    }

// ---------------------------------------------------------------------------
// CDiagEngine::ResumeL
// ---------------------------------------------------------------------------
//
EXPORT_C void CDiagEngine::ResumeL()
    {
    iEngineImpl->ResumeL();
    }



// ---------------------------------------------------------------------------
// From class MDiagEngineCommon
// CDiagEngine::ExecutionPlanL
// ---------------------------------------------------------------------------
//
const MDiagPluginExecPlan& CDiagEngine::ExecutionPlanL() const
    {
    return iEngineImpl->ExecutionPlanL();
    }


// ---------------------------------------------------------------------------
// From class MDiagEngineCommon
// CDiagEngine::ExecutionStopL
// ---------------------------------------------------------------------------
//
void CDiagEngine::ExecutionStopL( TCancelMode aCancelMode )
    {
    iEngineImpl->ExecutionStopL( aCancelMode );
    }


// ---------------------------------------------------------------------------
// From class MDiagEngineCommon
// CDiagEngine::ResetWatchdog
// ---------------------------------------------------------------------------
//
void CDiagEngine::ResetWatchdog()
    {
    iEngineImpl->ResetWatchdog();
    }


// ---------------------------------------------------------------------------
// From class MDiagEngineCommon
// CDiagEngine::ResetWatchdog
// ---------------------------------------------------------------------------
//
void CDiagEngine::ResetWatchdog( TDiagEngineWatchdogTypes aWatchdogType )
    {
    iEngineImpl->ResetWatchdog( aWatchdogType );
    }


// ---------------------------------------------------------------------------
// From class MDiagEngineCommon
// CDiagEngine::ResetWatchdog
// ---------------------------------------------------------------------------
//
void CDiagEngine::ResetWatchdog( TTimeIntervalMicroSeconds32 aExpectedTimeToComplete )
    {
    iEngineImpl->ResetWatchdog( aExpectedTimeToComplete );
    }

// ---------------------------------------------------------------------------
// From class MDiagEngineCommon
// CDiagEngine::ViewAppUi
// ---------------------------------------------------------------------------
//
CAknViewAppUi& CDiagEngine::ViewAppUi()
    {
    return iEngineImpl->ViewAppUi();
    }

// ---------------------------------------------------------------------------
// From class MDiagEngineCommon
// CDiagEngine::ViewAppUi
// ---------------------------------------------------------------------------
//
const CAknViewAppUi& CDiagEngine::ViewAppUi() const
    {
    return iEngineImpl->ViewAppUi();
    }


// ---------------------------------------------------------------------------
// From class MDiagEngineCommon
// CDiagEngine::DbRecord
// ---------------------------------------------------------------------------
//
RDiagResultsDatabaseRecord& CDiagEngine::DbRecord()
    {
    return iEngineImpl->DbRecord();
    }

// ---------------------------------------------------------------------------
// From class MDiagEngineCommon
// CDiagEngine::DbRecord
// ---------------------------------------------------------------------------
//
const RDiagResultsDatabaseRecord& CDiagEngine::DbRecord() const
    {
    return iEngineImpl->DbRecord();
    }

// ---------------------------------------------------------------------------
// From class MDiagEngineCommon
// CDiagEngine::DbRecord
// ---------------------------------------------------------------------------
//
CDiagPluginPool& CDiagEngine::PluginPool()
    {
    return iEngineImpl->PluginPool();
    }

// ---------------------------------------------------------------------------
// From class MDiagEngineCommon
// CDiagEngine::DbRecord
// ---------------------------------------------------------------------------
//
const CDiagPluginPool& CDiagEngine::PluginPool() const
    {
    return iEngineImpl->PluginPool();
    }

// ---------------------------------------------------------------------------
// From class MDiagEngineCommon
// CDiagEngine::AddToConfigListL
// ---------------------------------------------------------------------------
//
void CDiagEngine::AddToConfigListL( MDiagEngineCommon::TConfigListType aListType,
                                     const TDesC&                       aText )
    {
    return iEngineImpl->AddToConfigListL( aListType, aText );
    }

// ---------------------------------------------------------------------------
// From class MDiagEngineCommon
// CDiagEngine::RemoveFromConfigListL
// ---------------------------------------------------------------------------
//
void CDiagEngine::RemoveFromConfigListL( MDiagEngineCommon::TConfigListType aListType,
                                          const TDesC&                       aText )
    {
    return iEngineImpl->RemoveFromConfigListL( aListType, aText );
    }

// ---------------------------------------------------------------------------
// From class MDiagEngineCommon
// CDiagEngine::CreateCommonDialogLC
// ---------------------------------------------------------------------------
//
CAknDialog* CDiagEngine::CreateCommonDialogLC( TDiagCommonDialog aDialogType,
                                               TAny* aInitData )
    {
    return iEngineImpl->CreateCommonDialogLC( aDialogType, aInitData );
    }

// ---------------------------------------------------------------------------
// From class MDiagEngineCommon
// CDiagEngine::ExecuteAppCommandL
// ---------------------------------------------------------------------------
//
void CDiagEngine::ExecuteAppCommandL( TDiagAppCommand aCommand, 
                                      TAny* aParam1,
                                      TAny* aParam2 )
    {
    iEngineImpl->ExecuteAppCommandL( aCommand, aParam1, aParam2 );
    }

// ---------------------------------------------------------------------------
// From class MDiagEngineCommon
// CDiagEngine::StopWatchdogTemporarily
// ---------------------------------------------------------------------------
//
void CDiagEngine::StopWatchdogTemporarily()
    {
    iEngineImpl->StopWatchdogTemporarily();
    }

// ---------------------------------------------------------------------------
// From class MDiagEngineCommon
// CDiagEngine::IsDependencyDisabled
// ---------------------------------------------------------------------------
//
TBool CDiagEngine::IsDependencyDisabled() const
    {
    return iEngineImpl->IsDependencyDisabled();
    }

// ---------------------------------------------------------------------------
// From class MDiagEngineCommon
// CDiagEngine::CustomParam
// ---------------------------------------------------------------------------
//
TAny* CDiagEngine::CustomParam() const
    {
    return iEngineImpl->CustomParam();
    }
    
    
  // ADO & Platformization Changes
    
///@@@KSR: changes for Codescanner error val = High
//EXPORT_C TBool CDiagEngine::GetPluginDependency()
EXPORT_C TBool CDiagEngine::GetPluginDependencyL()
{
	return iEngineImpl->ExecutionPlanL().CurrentExecutionItem().AsDependency();
}

// End of File

