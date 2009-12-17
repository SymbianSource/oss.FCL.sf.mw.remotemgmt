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
* Description:  Interface to observe test plug-ins
*
*/


#ifndef DIAGTESTOBSERVER_H
#define DIAGTESTOBSERVER_H

// INCLUDES
#include <e32def.h>                         // TUint
#include <DiagEngineWatchdogTypes.h>        // TDiagEngineWatchdogTypes

// FORWARD DECLARATION
class CDiagResultsDatabaseItem;
class MDiagTestPlugin;


/**
* Diagnostics Framework Test Plug-in Observer
*
* This interface is called by Test Plug-ins to notify the engine
* of its test execution progress or completion.
*
* @since S60 v3.2
*/
class MDiagTestObserver
    {
public:
    /**
    * Notify engine of test plug-in execution progress.
    *
    * @param aSender - Reference to sender object.
    * @param aCurrentStep - Current execution step.
    */
    virtual void TestProgressL( const MDiagTestPlugin& aSender, 
                                TUint aCurrentStep ) = 0;


    /**
    * Notify engine of test plug-in execution completion.
    *
    * @param aSender       Reference to sender object.
    * @param aTestResult   Result of test execution. Ownership is transferred
    *                      from the plug-in to engine.
    */
    virtual void TestExecutionCompletedL( const MDiagTestPlugin& aSender, 
                                          CDiagResultsDatabaseItem* aTestResult ) = 0;
    };


#endif // DIAGTESTOBSERVER_H

// End of File

