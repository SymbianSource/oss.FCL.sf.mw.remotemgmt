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
* Description:  Provides execution parameters for Suite plug-ins
*
*/



#ifndef DIAGSUITEEXECPARAM_H
#define DIAGSUITEEXECPARAM_H

// SYSTEM INCLUDES
#include <e32def.h>             // IMPORT_C / EXPORT_C


// FORWARD DECLARATIONS
class MDiagSuiteObserver;
class MDiagEngineCommon;

/**
* Diagnostics Suite plug-in Execution Parameters
*
* This class provides the execution environment for Suite plug-ins.
*
* @since S60 v5.0
**/
NONSHARABLE_CLASS( TDiagSuiteExecParam )
    {
public:
    /**
    * C++ Constructor
    *
    * @param aObserver - Observer to notify continuation. 
    * @param aEngine - Reference to engine that provides necessary environment
    *   for suite execution, such as CDiagPluginPool, CAknViewAppUi and
    *   CDiagPluginExecPlan.
    */
    IMPORT_C TDiagSuiteExecParam( MDiagSuiteObserver& aObserver,
                                  MDiagEngineCommon& aEngine );
    
public: // new functions

    /**
    * Get the Suite plug-in execution observer.
    *
    * @return MDiagSuiteObserver - Observer of Suite-plugin.
    **/
    IMPORT_C MDiagSuiteObserver& Observer() const;

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
    * iObserver  - Suite execution observer.
    */
    MDiagSuiteObserver&  iObserver;

    /**
    * iEngine - Engine common interface.
    */
    MDiagEngineCommon&  iEngine;
    };

#endif // DIAGSUITEEXECPARAM_H

// End of File

