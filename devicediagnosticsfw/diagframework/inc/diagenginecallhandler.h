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
* Description:  Class declaration for DiagFwInternal::TEvent
*
*/


#ifndef DIAGENGINECALLHANDLER_H
#define DIAGENGINECALLHANDLER_H

// SYSTEM INCLUDES
#include <etel3rdparty.h>       // CTelephony and its T classes
#include <badesca.h>            // CDesCArrayFlat - needed since its a typedef

// USER INCLUDES
#include "diagenginecallhandlerstates.h"    // TDiagEngineCallHandlerState

// FORWARD DECLARATION
class MDiagEngineCallHandlerObserver;


/**
*  Call Handler.
*  
*   This component is responsible for checking both MO and MT voice calls.
*   If new calls are made, this component will let the observer know that
*   call status has changed.
*
*   It also maintains a list of acceptable phone numbers that can be ignored.
*   The list can be changed dynamically.
*   
*  @since S60 v5.0
*/
NONSHARABLE_CLASS( CDiagEngineCallHandler ) : public CActive
    {
public:
    /**
    * Two-phased constructor
    * 
    * @param aObserver - Reference to object interested in voice call events.
    * @return New instance of CDiagEngineCallHandler
    */
    static CDiagEngineCallHandler* NewL( MDiagEngineCallHandlerObserver& aObserver );

    /**
    * C++ Destructor
    * 
    */
    virtual ~CDiagEngineCallHandler();

    /**
    * Add a new phone number to a ignore list. If a new call is made, and
    * it is in the ignore list, state chaneg will not occur.
    *
    * Note that if the number specified is already in-call, adding the 
    * numebr will not cause the state to be changed. Only new calls
    * are watched.
    * 
    * @param aNumber - Phone number to ignore
    */
    void AddIgnoreNumberL( const TDesC& aNumber );

    /**
    * Remove a number from the ignore list. Number must match exactly to
    * be successful. If entry is not found, this function will leave
    * vith KErrNotFound.
    *
    * Note that if the number specified is already in call, removing 
    * the number will not generate state change event. Only new
    * calls are handled.
    * 
    * @param aNumber - Phone number to ignore
    */
    void RemoveIgnoreNumberL( const TDesC& aNumber );

    /**
    * Returns current call handler state.
    *
    * @return Current call handler state.
    */
    TDiagEngineCallHandlerState CurrentState() const;

private:    // from CActive
    /**
    * @see CActive::RunL()
    */
    virtual void RunL();

    /**
    * @see CActive::DoCancel()
    */
    virtual void DoCancel();

private:    // private functions
    /**
    * C++ Constructor
    *
    * @param aObserver - reference to observer.
    */
    CDiagEngineCallHandler( MDiagEngineCallHandlerObserver& aObserver );

    /**
    * Second phase constructor.
    *
    */
    void ConstructL();

    /**
    * Request from the telephony to receive phone status change indication.
    * This function must be called every time status update happens in
    * order to continue to receive new state change.
    *
    */
    void RequestNotify();

    /**
    * Handle CTelephony::EStatusIdle case.
    *
    * @return ETrue if state is changed, and observer should be notified.
    */
    TBool HandleIdle();

    /**
    * Handle CTelephony::EStatusDialling and CTelephony::EStatusRinning case.
    *
    * @return ETrue if state is changed, and observer should be notified.
    */
    TBool HandleCall();

    /**
    * Debugging function that logs a human readable text of current 
    * call status.
    *
    * This function does not do anything in DEBUG builds.
    */
    void LogCallStatus() const;

private:
    /**
    * iObserver - Observers call handle status.
    */
    MDiagEngineCallHandlerObserver& iObserver;          

    /**
    * iTelephony - Used to receive call status changed indication.
    * Owns.
    */
    CTelephony* iTelephony;

    /**
    * iCallStatus - This is where call status data is updated.
    */
    CTelephony::TCallStatusV1 iCallStatus;

    /**
    * iCallStatusPckg - packaged version of iCallStatus
    */
    CTelephony::TCallStatusV1Pckg iCallStatusPckg;

    /**
    * iCallIgnoreList - List of numbers to ignore.
    * Owns.
    */
    CDesCArrayFlat* iCallIgnoreList;    

    /**
    * iState - internal state.
    */
    TDiagEngineCallHandlerState iState;
    };

#endif // DIAGENGINECALLHANDLER_H

// End of File

