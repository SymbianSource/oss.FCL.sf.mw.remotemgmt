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
* Description:  Class definition of CDiagExecPlanEntryImpl
*
*/


// CLASS DECLARATION
#include "diagexecplanentryimpl.h"

// SYSTEM INCLUDE FILES
#include <DiagFrameworkDebug.h>             // LOGSTRING

// USER INCLUDE FILES
#include "diagengineconfig.h"               // TDiagEngineConfig
#include "diagframework.pan"                // Panic Codes

// DATA

// MACROS

// LOCAL DATA TYPES


// ======== LOCAL FUNCTIONS ========

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// CDiagExecPlanEntryImpl::CDiagExecPlanEntryImpl
// ---------------------------------------------------------------------------
//
CDiagExecPlanEntryImpl::CDiagExecPlanEntryImpl( 
        MDiagEngineCommon& aEngine,
        const TDiagEngineConfig& aEngineConfig,
        MDiagExecPlanEntryImplObserver& aObserver,
        MDiagPlugin& aPlugin,
        TBool aAsDependency,
        TType aType )
    :   CActive( EPriorityStandard ),
        iEngine( aEngine ),
        iEngineConfig( aEngineConfig ),
        iObserver( aObserver ),
        iPlugin( aPlugin ),
        iAsDependency( aAsDependency ),
        iState ( EStateQueued ),
        iType ( aType )
    {
    CActiveScheduler::Add( this );
    }

// ---------------------------------------------------------------------------
// CDiagExecPlanEntryImpl::~CDiagExecPlanEntryImpl
// ---------------------------------------------------------------------------
//
CDiagExecPlanEntryImpl::~CDiagExecPlanEntryImpl()
    {
    if ( iWatchdogTimer )
        {
        iWatchdogTimer->Cancel();
        delete iWatchdogTimer;
        iWatchdogTimer = NULL;
        }
    }

// ---------------------------------------------------------------------------
// CDiagExecPlanEntryImpl::Plugin
// ---------------------------------------------------------------------------
//
MDiagPlugin& CDiagExecPlanEntryImpl::Plugin()
    {
    return iPlugin;
    }

// ---------------------------------------------------------------------------
// CDiagExecPlanEntryImpl::Plugin
// ---------------------------------------------------------------------------
//
const MDiagPlugin& CDiagExecPlanEntryImpl::Plugin() const
    {
    return iPlugin;
    }

// ---------------------------------------------------------------------------
// CDiagExecPlanEntryImpl::State
// ---------------------------------------------------------------------------
//
MDiagExecPlanEntry::TState CDiagExecPlanEntryImpl::State() const
    {
    return iState;
    }

// ---------------------------------------------------------------------------
// CDiagExecPlanEntryImpl::ChangeStateL
// ---------------------------------------------------------------------------
//
void CDiagExecPlanEntryImpl::ChangeStateL( TState aState )
    {
    iState = aState;

    TBool watchdogNeeded = EFalse;

    // determine if we need a watchdog or not.
    switch( aState )
        {
        case EStateRunning:
        case EStateStopped:
            watchdogNeeded = ETrue;
            break;

        case EStateQueued:
        case EStateInitDelay:
        case EStateCompleted:
        case EStateSuspended:
        default:
            watchdogNeeded = EFalse;
            break;
        }

    if ( watchdogNeeded )
        {
        if ( iWatchdogTimer == NULL )
            {
            LOGSTRING( "CDiagExecPlanEntryImpl::ChangeStateL() Create watchdog timer" )
            iWatchdogTimer = CPeriodic::NewL( EPriorityStandard );
            }
        ResetWatchdog();
        }
    else
        {
        if ( iWatchdogTimer )
            {
            LOGSTRING( "CDiagExecPlanEntryImpl::ChangeStateL() delete watchdog timer" )
            iWatchdogTimer->Cancel();
            delete iWatchdogTimer;
            iWatchdogTimer = NULL;
            }
        }
    }

// ---------------------------------------------------------------------------
// CDiagExecPlanEntryImpl::Type
// ---------------------------------------------------------------------------
//
CDiagExecPlanEntryImpl::TType CDiagExecPlanEntryImpl::Type() const
    {
    return iType;
    }

// ---------------------------------------------------------------------------
// CDiagExecPlanEntryImpl::SetType
// ---------------------------------------------------------------------------
//
void CDiagExecPlanEntryImpl::SetType( TType aType )
    {
    iType = aType;
    }

// ---------------------------------------------------------------------------
// CDiagExecPlanEntryImpl::AsDependency
// ---------------------------------------------------------------------------
//
TBool CDiagExecPlanEntryImpl::AsDependency() const
    {
    return iAsDependency;
    }

// ---------------------------------------------------------------------------
// CDiagExecPlanEntryImpl::SetAsDependency
// ---------------------------------------------------------------------------
//
void CDiagExecPlanEntryImpl::SetAsDependency( TBool aAsDependency )
    {
    iAsDependency = aAsDependency;
    }

// ---------------------------------------------------------------------------
// CDiagExecPlanEntryImpl::IsStoppedByClient
// ---------------------------------------------------------------------------
//
TBool CDiagExecPlanEntryImpl::IsStoppedByClient() const
    {
    return iStoppedByClient;
    }

// ---------------------------------------------------------------------------
// CDiagExecPlanEntryImpl::Engine
// ---------------------------------------------------------------------------
//
MDiagEngineCommon& CDiagExecPlanEntryImpl::Engine()
    {
    return iEngine;
    }

// ---------------------------------------------------------------------------
// CDiagExecPlanEntryImpl::EngineConfig
// ---------------------------------------------------------------------------
//
const TDiagEngineConfig& CDiagExecPlanEntryImpl::EngineConfig() const
    {
    return iEngineConfig;
    }

// ---------------------------------------------------------------------------
// CDiagExecPlanEntryImpl::Observer
// ---------------------------------------------------------------------------
//
MDiagExecPlanEntryImplObserver& CDiagExecPlanEntryImpl::Observer()
    {
    return iObserver;
    }


// ---------------------------------------------------------------------------
// CDiagExecPlanEntryImpl::StopExecutionByClientL
// ---------------------------------------------------------------------------
//
void CDiagExecPlanEntryImpl::StopExecutionByClientL( 
        MDiagEngineCommon::TCancelMode aCancelMode )
    {
    iStoppedByClient = ETrue;

    DoStopExecutionByClientL( aCancelMode );
    }

// ---------------------------------------------------------------------------
// CDiagExecPlanEntryImpl::ResetWatchdog
// ---------------------------------------------------------------------------
//
void CDiagExecPlanEntryImpl::ResetWatchdog()
    {
    if ( iWatchdogTimer )
        {
        LOGSTRING2( "CDiagExecPlanEntryImpl::ResetWatchdog() "
            L"Timout Value = %d microseconds", iWatchdogValue.Int() )
        iWatchdogTimer->Cancel();
        iWatchdogTimer->Start( iWatchdogValue,
                               iWatchdogValue,
                               TCallBack( WatchdogTimerExpiredL, this ) );
        }
    else    
        {
        LOGSTRING( "CDiagExecPlanEntryImpl::ResetWatchdog() "
            L"Watchdog was not running. Reset request ignored." )
        }
    }


// ---------------------------------------------------------------------------
// CDiagExecPlanEntryImpl::ResetWatchdog
// ---------------------------------------------------------------------------
//
void CDiagExecPlanEntryImpl::ResetWatchdog( TDiagEngineWatchdogTypes aWatchdogType )
    {
    switch ( aWatchdogType )
        {
        case EDiagEngineWatchdogTypeAutomatic:
            iWatchdogValue = iEngineConfig.WatchdogTimeoutValueAutomatic();
            break;

        case EDiagEngineWatchdogTypeInteractive:
            iWatchdogValue = iEngineConfig.WatchdogTimeoutValueInteractive();
            break;

        default:
            __ASSERT_ALWAYS( 0, Panic( EDiagFrameworkBadArgument ) );
            break;
        }

    ResetWatchdog();
    }

// ---------------------------------------------------------------------------
// CDiagExecPlanEntryImpl::ResetWatchdog
// ---------------------------------------------------------------------------
//
void CDiagExecPlanEntryImpl::ResetWatchdog( TTimeIntervalMicroSeconds32 aWatchdogValue )
    {
    iWatchdogValue = aWatchdogValue;
    ResetWatchdog();
    }

// ---------------------------------------------------------------------------
// CDiagExecPlanEntryImpl::StopWatchdogTemporarily
// ---------------------------------------------------------------------------
//
void CDiagExecPlanEntryImpl::StopWatchdogTemporarily()
    {
    LOGSTRING( "CDiagExecPlanEntryImpl::StopWatchdogTemporarily() " )
    if ( iWatchdogTimer )
        {
        iWatchdogTimer->Cancel();
        }
    }


// ---------------------------------------------------------------------------
// CDiagEngineImpl::WatchdogTimerExpiredL
//
// ---------------------------------------------------------------------------
//
TInt CDiagExecPlanEntryImpl::WatchdogTimerExpiredL( TAny* aPtr )
    {
    LOGSTRING( "CDiagExecPlanEntryImpl::WatchdogTimerExpiredL()" )
    
    CDiagExecPlanEntryImpl* thisPtr = static_cast< CDiagExecPlanEntryImpl* >( aPtr );

    // once watchdog expires, it should not be restarted, since it is end of test.
    thisPtr->iWatchdogTimer->Cancel();
    delete thisPtr->iWatchdogTimer;
    thisPtr->iWatchdogTimer = NULL;

    thisPtr->StopExecutionByWatchdogL();
    
    return 0;
    }



// End of File

