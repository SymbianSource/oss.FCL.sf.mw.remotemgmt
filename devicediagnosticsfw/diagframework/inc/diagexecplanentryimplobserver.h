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
* Description:  Class declaration for MDiagExecPlanEntryImplObserver.h
*
*/


#ifndef DIAGEXECPLANENTRYIMPLOBSERVER_H
#define DIAGEXECPLANENTRYIMPLOBSERVER_H


class CDiagExecPlanEntryImpl;

/**
* Diagnostics Plug-in Execution Plan Entry Observer Interface
*
* This interface is called by CDiagExecPlanEntryImpl and its derived classes
* to notify progress of plugin execution.
*
* @since S60 v5.0
*
*/
class MDiagExecPlanEntryImplObserver
    {
public:
    /**
    * Notify plugin execution progress
    * @param aSender        - Reference to the plan entry that caused this event.
    * @param aCurrentStep   - current execution step.
    * @param aTotalSteps    - Total number of steps to execute.
    */
    virtual void ExecPlanEntryProgressL( CDiagExecPlanEntryImpl& aSender,
                                         TUint aCurrentStep,
                                         TUint aTotalSteps ) = 0;

    /**
    * Notify plugin execution completion. 
    *   This is called only when it is completed normally.
    *   It will not becalled if execution is interrupted by watchdog or by client.
    * @param aSender        - Reference to the plan entry that caused this event.
    */
    virtual void ExecPlanEntryExecutedL( CDiagExecPlanEntryImpl& aSender ) = 0;

    /**
    * Notify that a critical error has occured.
    *   This is called when there is a critical execution error
    *   that cannot be recovered. Engine should not execute any
    *   more plug-ins after this point.
    * @param aError - Error number.
    */
    virtual void ExecPlanEntryCriticalError( TInt aError ) = 0;
    };

#endif // DIAGEXECPLANENTRYIMPLOBSERVER_H

// End of File

