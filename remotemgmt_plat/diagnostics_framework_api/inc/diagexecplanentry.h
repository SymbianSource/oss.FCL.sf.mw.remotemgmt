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
* Description:  Class declaration for CDiagExecPlanEntry
*
*/


#ifndef DIAGEXECPLANENTRY_H
#define DIAGEXECPLANENTRY_H


// SYSTEM INCLUDE FILES
#include <e32def.h>         // TBool

// FORWARD DECLARATION
class MDiagPlugin;

/**
* Diagnostics Plug-in Execution Plan Entry.
*
* This class provides the interface for test execution plan entries.
*
* @since S60 v5.0
*
*/
class MDiagExecPlanEntry
    {
public: // data types
    /**
    * Item execution state.
    */
    enum TState
        {
        EStateQueued     = 0,   // Execution has not begun.
        EStateInitDelay,        // Waiting for initial delay timer to expire
        EStateRunning,          // RunTestL() is called. Waiting for completion.
        EStateSuspended,        // Execution is suspended
        EStateStopped,          // Plugin is stopped, but result not logged yet.
        EStateCompleted         // Result is logged
        };

public:
    /**
    * Plugin associated with the entry
    *   @return Reference to the plugin.
    */
    virtual const MDiagPlugin& Plugin() const = 0;

    /**
    * Returns whether item is being executed to satisfy dependency or not.
    *   @return ETrue if item is being executed to satisfy dependency.
    *       EFalse if item is being executed explicitly.
    */
    virtual TBool AsDependency() const = 0;

    /**
    * Item State
    *   @return Current state of execution plan item
    */
    virtual TState State() const = 0;

    };

#endif // DIAGEXECPLANENTRY_H

// End of File

