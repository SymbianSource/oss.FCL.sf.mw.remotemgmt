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
* Description:  This is a test suite i.e. contains collection of tests. 
*
*/



#ifndef DIAGSUITEPLUGIN_H
#define DIAGSUITEPLUGIN_H

// INCLUDES
#include    <DiagPlugin.h>          // MDiagPlugin

// FORWARD DECLARATIONS
class MDiagSuiteObserver;
class MDiagEngineCommon;
class CDiagPluginPool;
class TDiagSuiteExecParam;

/**
* Test suite plug-in abstract interface. 
*
* @since S60 v5.0
**/
class MDiagSuitePlugin: public MDiagPlugin
    {
public: //DATA STRUCTURES

    /**
    * Defines sorting algorithm.
    **/
    enum TSortOrder 
    {
        ENotSorted,
        ESortByPosition
    };

    /**
    * Reason for execution stop.
    **/
    enum TStopReason
    {
        ESkip,
        ECancelAll,
        EWatchdog
    };

public:    // new API

    /**
    * Get children of this plug-in. The pointer array is guaranteed to 
    * be sorted defined by TSortOrder.
    * @param aChildren Children are appended into this array.
    * @param aOrder Sorting algorithm.
    **/
    virtual void GetChildrenL( RPointerArray<MDiagPlugin>& aChildren,
                               TSortOrder aOrder ) const = 0;

    /**
    * Add one child. Child can be either a test suite or a test plug-in.
    * @param aChild - Child to be added. Ownership is transferred to
    *               MDiagPluginSuite.
    **/
    virtual void AddChildL( MDiagPlugin* aChild ) = 0;

    /**
    * Called before one of its test plug-in is executed.  Note that it will
    * not be called if two of its children plug-ins are executed in 
    * sequence.
    *
    * @param aExecParams Parameters for suite pre execution
    * @param aSkipDependencyCheck - If ETrue dependency is disabled 
    *   for this test session.
    * @param aDependencyExecution - If ETrue, this suite is being
    *   executed to satisfy dependency.
    **/
    virtual void PrepareChildrenExecutionL( TDiagSuiteExecParam* aParam,
                                            TBool aSkipDependencyCheck,
                                            TBool aDependencyExecution ) = 0;

    /**
    * Called before test execution switches to another test suite.
    *
    * @param aExecParams Parameters for suite post execution
    * @param aSkipDependencyCheck - If ETrue dependency is disabled 
    *   for this test session.
    * @param aDependencyExecution - If ETrue, this suite is being
    *   executed to satisfy dependency.
    **/
    virtual void FinalizeChildrenExecutionL( TDiagSuiteExecParam* aParam,
                                             TBool aSkipDependencyCheck,
                                             TBool aDependencyExecution ) = 0;

    /**
    * Get the Uids. The uid is used for database access.
    * Test suites return a list of their childrens' uids.
    *
    * @param aUids An UID array.
    * @param aOrder Sorting algorithm.
    **/
    virtual void GetChildrenUidsL( RArray<TUid>& aUids,
                                   TSortOrder aOrder ) const = 0;

    /**
    * Cancels pre/post execution. Cancellation is expected to be synchronous.
    * Suite plug-in must not call ContinueExecutionL().
    *
    * @param aReason - Reason why ExecutionStopL() is being called.
    **/
    virtual void ExecutionStopL( TStopReason aReason ) = 0;
    };

#endif // DIAGSUITEPLUGIN_H

// End of File

