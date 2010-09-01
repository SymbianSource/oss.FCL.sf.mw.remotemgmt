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
* Description:  Class declaration for CDiagPluginExecPlan
*
*/


#ifndef DIAGPLUGINEXECPLAN_H
#define DIAGPLUGINEXECPLAN_H

// INCLUDES
#include <e32base.h>            // CBase

// FORWARD DECLARATION
class MDiagPlugin;
class MDiagExecPlanEntry;

/**
*  Diagnostics Plugin Execution Plan
*
*  This class provides interface to access execution plan.
*  It also provides interface to modify execution plan.
*  A cursor to currently executing item is also maintained.
*
*/
class MDiagPluginExecPlan
    {
public: 
    /**
    * Access an item in plan.
    *
    * @param aIndex    - index 
    * @return Reference to plan entry at given index.
    */
    virtual const MDiagExecPlanEntry& operator[]( TInt aIndex ) const = 0;

    /**
    * Get current execution plan item.
    *
    * @return Reference to currently executing plan item.
    */
    virtual const MDiagExecPlanEntry& CurrentExecutionItem() const = 0;

    /**
    * Returns index of execution cursor. Note that this index 
    * includes both suite plug-in and test plug-in.
    *
    * If only interested about test plug-ins. use CurrentTestIndex().
    *
    * @return Index to current execution cursor.
    */
    virtual TInt CurrentIndex() const = 0;

    /**
    * Returns total number of items in the plan. Note that this count
    * includes both suite plug-in and test plug-in.

    * If only interested only test plug-ins. use TestCount().
    *
    * @return Number of items in the test plan.
    */
    virtual TInt Count() const = 0;

    /**
    * Get current test index.
    *
    * Note that only test plug-ins (MDiagTestPlugin) are counted in this
    * case.  Suite pre/post execution are not. You cannot use this value
    * with [] operator since this is not a real index.
    * 
    * To get real cursor index, use GetCurrentIndex().
    *
    * @param aIncludeDependency  If ETrue, test index will also include
    *                            plugins that are included as dependency.
    * @return current test index.
    */
    virtual TInt CurrentTestIndex( TBool aIncludeDependency ) const = 0;

    /**
    * Get total test count.
    *
    * Note that only test plug-ins (MDiagTestPlugin) are counted in this
    * case.  Suite pre/post execution are not. You cannot use this value
    * with [] operator since this is not a real count.
    * 
    * To get the real number of items in the plan, use GetCount().
    *
    * @param aIncludeDependency  If ETrue, test index will also include
    *                            plug-ins that are included as dependency.
    * @return current test index.
    */
    virtual TInt TestCount( TBool aIncludeDependency ) const = 0;

    /**
    * Check if this is the last test being executed.
    *
    * Note that only test plug-ins (MDiagTestPlugin) are counted in this
    * case.  Suite pre/post execution are not.
    * If you are interested in checking if it is currently running
    * the last plug-in, including test and suites, @see IsLastPlugin()
    *
    * @return ETrue plan is executing the last test plug-in.
    */
    virtual TBool IsLastTest() const = 0;

    /**
    * Check if this is the last plug-in being executed.
    *
    * Note that this includes both test plug-ins and suite plug-ins.
    * If you are interested in checking the progress on tests only,
    * @see IsLastTest()
    *
    * @return ETrue plan is executing the last test or suite plug-in.
    */
    virtual TBool IsLastPlugin() const = 0;

    /**
    * Returns the plug-in index that plan is resuming from.
    * If this session is not a resume session, this will return 0 (first index).
    * Otherwise, this index will point the first real execution item.
    *
    * Note that this value will not change as test session progresses. Even
    * as test progresses, this value will still point to the index that
    * this session is being resumed from.
    *
    * @return Index of which session is being resumed from.
    */
    virtual TInt ResumeIndex() const = 0;
    };

#endif // DIAGPLUGINEXECPLAN_H

// End of File

