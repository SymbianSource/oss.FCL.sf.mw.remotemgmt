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
* Description:  Provides execution parameters for test plug-ins
*
*/



#ifndef DIAGTESTEXECPARAM_H
#define DIAGTESTEXECPARAM_H


// SYSTEM INCLUDES
#include <e32def.h>             // IMPORT_C / EXPORT_C

// FORWARD DECLARATIONS
class MDiagTestObserver;
class MDiagEngineCommon;

/**
* Diagnostics test plug-in Execution Parameters
*
* This class provides the execution environment for test plug-ins.
*
* @since S60 v5.0
**/
NONSHARABLE_CLASS( TDiagTestExecParam )
    {
public: // constructor
    /**
    * C++ Constructor
    *
    * @param aObserver - Observer to notify test execution progress.
    * @param aEngine - Reference to engine that provides necessary environment
    *   for test execution, such as CDiagPluginPool, or CAknViewAppUi.
    */
    IMPORT_C TDiagTestExecParam( MDiagTestObserver& aObserver,
                                 MDiagEngineCommon& aEngine );
    
public: // new functions

    /**
    * Get the test plug-in execution observer.
    *
    * @return MDiagTestObserver - Observer of test-plugin. Test progress
    *   and completion should be notified via this object.
    **/
    IMPORT_C MDiagTestObserver& Observer() const;

    /**
    * Get execution engine.
    *
    * @return MDiagEngineCommon - Engine that executes the plug-in.
    *   Engine provides various functionalities needed for executing
    *   plug-ins.
    **/
    IMPORT_C MDiagEngineCommon& Engine() const;
    
private: // data
    /**
    * iObserver - Test execution observer.
    */
    MDiagTestObserver&  iObserver;

    /**
    * iEngine - Common Engine interface
    */
    MDiagEngineCommon&  iEngine;
    };

#endif // DIAGTESTEXECPARAM_H

// End of File

