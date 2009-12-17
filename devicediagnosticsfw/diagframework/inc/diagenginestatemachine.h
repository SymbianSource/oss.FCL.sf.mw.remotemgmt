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
* Description:  Class declaration for DiagFwInternal::CStateMachine
*
*/


#ifndef DIAGENGINESTATEMACHINE_H
#define DIAGENGINESTATEMACHINE_H

// system include
#include <e32base.h>

// user include
#include "diagenginestates.h"   // DiagFwInternal::TState
#include "diagengineevents.h"   // DiagFwInternal::TEvent


namespace DiagFwInternal
    {
// forward declaration
class CEventBasic;
class MStateMachineObserver;

/**
*  Diagnostics Framework Engine State Machine
*
*  This class implements state machine for CDiagEngine.
*
*/
NONSHARABLE_CLASS( CStateMachine ) : public CActive
    {
public:
    /**
    * Two-phased constructor.
    * 
    * @param aObserver     Engine observer.
    * @return New instance of CStateMachine
    */
    static CStateMachine* NewL( MStateMachineObserver& aObserver );
    static CStateMachine* NewLC( MStateMachineObserver& aObserver );

    /**
    * Queue a new event into event queue. 
    * 
    * @param aEvent  New event. CStateMachine will own
    *              the object from this point on.
    *              It will be freed by CStateMachine when
    *              the event is processed.
    */
    void AddEventL( CEventBasic* aEvent );

    /**
    * Queue a new event into event queue.
    * 
    * @param aEventId  Event Id of new event. CEventBasic will be created.
    */
    void AddEventL( TEvent aEventId );

    /**
    * Current State.
    * 
    * @return Current state.
    */
    TState CurrentState() const;

    /**
    * Handle error. This should be called when an abnormal error
    * occurs (e.g. leave that cannot be handled.)
    * 
    * @param aError - Error number.
    */
    void HandleError( TInt aError );

    /**
    * State Name. 
    * Debugging purpose only. In release build, this function will
    * return empty string.
    * 
    * @param aState - a state 
    * @return Name of the state
    */
    const TDesC& StateName( TState aState ) const;

    /**
    * C++ Destructor
    */
    ~CStateMachine();

protected: // from CActive
    /**
    * @see CActive::RunL
    */
    virtual void RunL();
    
    /**
    * @see CActive::DoCancel
    */
    virtual void DoCancel();

    /**
    * @see CActive::RunError
    */
    virtual TInt RunError( TInt aError );

private:    // private constructors
    /**
    * C++ constructor
    */
    CStateMachine( MStateMachineObserver& iObserver );

    /**
    * 2nd phase constructor
    *
    */
    virtual void ConstructL();

private:    // private functions
    /**
    * Match states in state table.
    */
    TState CheckStateTable( TState aCurrState, TEvent aEvent ) const;

    /**
    * Complete request and set itself to be active so that the next
    * item in the queue can be executed.
    */
    void ReactivateQueue();

    
private: // private data
    /**
    * iObserver - Observer.
    */
    MStateMachineObserver&          iObserver;      

    /**
    * iEventQueue - array of events to process.
    */
    RPointerArray<CEventBasic>      iEventQueue;

    /**
    * iState - Current state.
    */
    TState                          iState;
    };
    } // end of namespace DiagFwInternal
#endif // DIAGENGINESTATEMACHINE_H

// End of File

