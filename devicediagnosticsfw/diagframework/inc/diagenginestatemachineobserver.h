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
* Description:  Class declaration for MStateMachineObserver
*
*/


#ifndef DIAGENGINESTATEMACHINEOBSERVER_H
#define DIAGENGINESTATEMACHINEOBSERVER_H

// system include

// user include
#include "diagenginestates.h"   // DiagFwInternal::TState
#include "diagengineevents.h"   // DiagFwInternal::TEvent

namespace DiagFwInternal
    {
// forward declaration
class CEventBasic;

/**
 *  Diagnostics Framework Engine State Machine Observer
 *
 *  This interface is called by CDiagEngineStateMachine to notify
 *  its client of state changes and events.
 *
 *  @since S60 v5.0
 */
class MStateMachineObserver
    {
public:
    /**
    * Notify state machine client that state machine has entered a new state.
    *
    * @param aPreviousState     Previous state.
    * @param aCurrentState      Current state. (new state.)
    * @param aEventPreview  Event that caused state change. Note that
    *   this is just a preview.  HandleEventL() will be called with the same
    *   event later on.
    */
    virtual void HandleStateChangedL( TState aPreviousState, 
                                      TState aCurrentState,
                                      const CEventBasic& aEventPreview ) = 0;

    /**
    * Notify state machine client that a new event is received.
    *
    * @param aEvent        New event.
    */
    virtual void HandleEventL( CEventBasic& aEvent ) = 0;

    /**
    * Notify state machine client that an abnormal error has occured.
    * Client can handle the error and return appropriate error state.
    *
    * @param aCurrentState Current state.
    * @param aError        Error number.
    * @return TState       New state to go to.
    */
    virtual TState HandleError( TState aCurrentState, TInt aError ) = 0;
    };
    } // end of namespace DiagFwInternal
    
#endif // DIAGENGINESTATEMACHINEOBSERVER_H

// End of File

