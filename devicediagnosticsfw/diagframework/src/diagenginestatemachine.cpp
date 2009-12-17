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
* Description:  Class definition of CStateMachine
*
*/


// CLASS DECLARATION
#include "diagenginestatemachine.h"         // DiagFwInternal::CStateMachine

// SYSTEM INCLUDE FILES
#include <DiagFrameworkDebug.h>             // LOGSTRING

// USER INCLUDE FILES
#include "diagenginestatemachineobserver.h" // DiagFwInternal::MStateMachineObserver
#include "diagengineeventbasic.h"           // DiagFwInternal::CEventBasic
#include "diagframework.pan"                // Panics


namespace DiagFwInternal
    {
// ======== LOCAL DATA ==========
struct TStateTableEntry
    {
    TState  iInputState;
    TEvent  iEventType;
    TState  iOutputState;
    };


static const TStateTableEntry  KStateTable[] = 
    {
        // current state    input event                 output state
        {
        EStateNotReady,     EEventExecute,              EStateCreatingPlan
        },
        {
        EStateCreatingPlan, EEventPlanCreated,          EStateRunning
        },
        {
        EStateSuspended,    EEventResumeToRunning,      EStateRunning
        },
        {
        EStateSuspended,    EEventResumeToCreatingPlan, EStateCreatingPlan
        },
        {
        EStateFinalizing,   EEventFinalized,            EStateStopped,
        },
        // ALL EStateAny MUST be listed in below.
        // This makes sure that more specific transition happens before
        // any state transitions occur.
        {
        EStateAny,          EEventAllPluginsCompleted,  EStateFinalizing
        },
        {
        EStateAny,          EEventCancelAll,            EStateCancelAll
        },
        {
        EStateAny,          EEventSuspend,              EStateSuspended
        },
        {
        EStateAny,          EEventVoiceCallActive,      EStateSuspended
        }
    };

static const TInt KStateTableSize = sizeof( KStateTable )/sizeof( TStateTableEntry );


// ======== LOCAL FUNCTIONS ========


// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// CStateMachine::ConstructL()
// ---------------------------------------------------------------------------
//
void CStateMachine::ConstructL()
    {
    }


// ---------------------------------------------------------------------------
// CStateMachine::NewL()
// ---------------------------------------------------------------------------
//
CStateMachine* CStateMachine::NewL( MStateMachineObserver& aObserver )
    {
    CStateMachine* self = CStateMachine::NewLC( aObserver );
    CleanupStack::Pop( self );
    return self;
    }


// ---------------------------------------------------------------------------
// CStateMachine::NewLC()
// ---------------------------------------------------------------------------
//
CStateMachine* CStateMachine::NewLC( MStateMachineObserver& aObserver )
    {
    CStateMachine* self = new( ELeave )CStateMachine( aObserver );
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

// ---------------------------------------------------------------------------
// CStateMachine::CStateMachine
// ---------------------------------------------------------------------------
//
CStateMachine::CStateMachine( MStateMachineObserver& aObserver )
    :   CActive( EPriorityStandard ),
        iObserver( aObserver ),
        iEventQueue(),
        iState( EStateNotReady )
    {
    CActiveScheduler::Add( this );
    }


// ---------------------------------------------------------------------------
// CStateMachine::~CStateMachine
// ---------------------------------------------------------------------------
//
CStateMachine::~CStateMachine() 
    {
    Cancel();
    iEventQueue.ResetAndDestroy();
    iEventQueue.Close();
    }
    

// ---------------------------------------------------------------------------
// CStateMachine::AddEventL
// ---------------------------------------------------------------------------
//
void CStateMachine::AddEventL( CEventBasic* aEvent )
    {
    __ASSERT_ALWAYS( aEvent, Panic( EDiagFrameworkBadArgument ) );

    LOGSTRING2( "CStateMachine::AddEventL: Type = %S", &( aEvent->ToString() ) )

    CleanupStack::PushL( aEvent );
    iEventQueue.AppendL( aEvent );     // owership transfer
    CleanupStack::Pop( aEvent );
    // do not set aEvent to NULL, since it is used later

    TState nextState = CheckStateTable( iState, aEvent->GetType() );

    if ( nextState != EStateAny && nextState != iState )
        {
        // state change happened.
        TState prevState = iState;
        iState = nextState;

        iObserver.HandleStateChangedL( prevState, iState, *aEvent );
        }

    ReactivateQueue();
    }


// ---------------------------------------------------------------------------
// CStateMachine::AddEventL
// ---------------------------------------------------------------------------
//
void CStateMachine::AddEventL( TEvent aEventId )
    {
    // simple event. Create a basic event type
    AddEventL( new( ELeave )CEventBasic( aEventId ) );
    }


// ---------------------------------------------------------------------------
// CStateMachine::CurrentState
// ---------------------------------------------------------------------------
//
TState CStateMachine::CurrentState() const
    {
    return iState;
    }

// ---------------------------------------------------------------------------
// CStateMachine::HandleError
// ---------------------------------------------------------------------------
//
void CStateMachine::HandleError( TInt aError )
    {
    iState = iObserver.HandleError( iState, aError );
    }

// ---------------------------------------------------------------------------
// From CActive
// CStateMachine::RunL
// ---------------------------------------------------------------------------
//
void CStateMachine::RunL() 
    {
    // Note that ReactivateQueue() must always be called before observer method
    // is called. This is because observer may delete state machine
    // during the callback. To prevent crashes, callback MUST BE the
    // last funciton to call before returning.
    if ( iEventQueue.Count() > 0 )
        {
        // there is an item in the event queue.
        // Pop and execute the event.
        CEventBasic* event = iEventQueue[0];
        iEventQueue.Remove( 0 );
        
        // always reactivate the queue before calling observer
        ReactivateQueue();

        CleanupStack::PushL( event );
        iObserver.HandleEventL( *event );
        CleanupStack::PopAndDestroy( event );

        // must return immediately to make sure that no member variables
        // are accessed after observer is called.
        return;
        }
    else
        {
        LOGSTRING( "CStateMachine::RunL(). Called for no reason?" )
        __ASSERT_DEBUG( 0, Panic( EDiagFrameworkCorruptStateMachine ) );
        }
    }

// ---------------------------------------------------------------------------
// From CActive
// CStateMachine::DoCancel
// ---------------------------------------------------------------------------
//
void CStateMachine::DoCancel() 
    {
    // Nothing to do here..
    }

// ---------------------------------------------------------------------------
// From CActive
// CStateMachine::RunError
// ---------------------------------------------------------------------------
//
TInt CStateMachine::RunError( TInt aError ) 
    {
    HandleError( aError );
    return KErrNone;
    }


// ---------------------------------------------------------------------------
// CStateMachine::CheckStateTable
// ---------------------------------------------------------------------------
//
TState CStateMachine::CheckStateTable( TState aCurrState, TEvent aEvent ) const
    {
    TState outputState = EStateAny;
    TBool isFound = EFalse;

    for ( TInt i = 0; i < KStateTableSize && !isFound; i++ )
        {
        if ( ( KStateTable[i].iInputState == EStateAny || 
                    aCurrState == KStateTable[i].iInputState ) &&
                aEvent == KStateTable[i].iEventType )
            {
            outputState = KStateTable[i].iOutputState;
            isFound = ETrue;
            }
        }
    return outputState;
    }

// ---------------------------------------------------------------------------
// CStateMachine::ReactivateQueue
// ---------------------------------------------------------------------------
//
void CStateMachine::ReactivateQueue()
    {
    if ( !IsActive() && iEventQueue.Count() > 0 )
        {
        // reactivate only if it is not already active, and there is something
        // in the queue
        TRequestStatus* stat = &iStatus;
        User::RequestComplete( stat, KErrNone );
        SetActive();
        }
    }

// ---------------------------------------------------------------------------
// CStateMachine::StateName
// ---------------------------------------------------------------------------
//

#if _DEBUG
const TDesC& CStateMachine::StateName( TState aState ) const
    {
    _LIT( KStateAny,         "EStateAny" );
    _LIT( KStateNotReady,    "EStateNotReady" );
    _LIT( KStateCreatingPlan,"EStateCreatingPlan" );
    _LIT( KStateRunning,     "EStateRunning" );
    _LIT( KStateStopped,     "EStateStopped" );
    _LIT( KStateCancelAll,   "EStateCancelAll" );
    _LIT( KStateSuspended,   "EStateSuspended" );
    _LIT( KStateFinalizing,  "EStateFinalizing" );

    switch ( aState )
        {
        case EStateAny:
            return KStateAny();

        case EStateNotReady:
            return KStateNotReady();

        case EStateCreatingPlan:
            return KStateCreatingPlan();

        case EStateRunning:
            return KStateRunning();

        case EStateStopped:
            return KStateStopped();

        case EStateCancelAll:
            return KStateCancelAll();

        case EStateSuspended:
            return KStateSuspended();

        case EStateFinalizing:
            return KStateFinalizing();
        
        default:
            _LIT( KUnknownState, "* Unknown State *");
            return KUnknownState(); 
        }
    }

#else   // #if _DEBUG

// non-debug version.
const TDesC& CStateMachine::StateName( TState /* aState */ ) const
    {
    _LIT( KNonDebugStateName, "?" );
    return KNonDebugStateName();
    }

#endif // #else _DEBUG

    } // end of namespace DiagFwInternal

// End of File

