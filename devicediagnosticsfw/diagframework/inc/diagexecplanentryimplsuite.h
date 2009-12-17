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
* Description:  Class declaration for CDiagExecPlanEntryImplSuite
*
*/


#ifndef DIAGEXECPLANENTRYIMPLSUITE_H
#define DIAGEXECPLANENTRYIMPLSUITE_H

// SYSTEM INCLUDE FILES
#include <DiagSuitePlugin.h>            // MDiagSuitePlugin::TStopReason
#include <DiagSuiteObserver.h>          // MDiagSuiteObserver

// USER INCLUDE FILES
#include "diagexecplanentryimpl.h"      // CDiagExecPlanEntryImpl

// FORWARD DECLARATION
class MDiagSuitePlugin;

/**
* Diagnostics Plug-in Execution Plan Entry Implementation for Suite plugin
*
* This class implements CDiagExecPlanEntryImplSuite. It is meant to be private
*   to Diagnostics Framework and not exported.
*
* @since S60 v5.0
*
*/
NONSHARABLE_CLASS( CDiagExecPlanEntryImplSuite ) : public CDiagExecPlanEntryImpl,
                                                   public MDiagSuiteObserver
    {
public: // Public constructors and destructors
    /**
    * Symbian two-phased constructor
    *   
    * @param aEngine - Diagnostics Engine
    * @param aEngineConfig - Engine configuration.
    * @param aObserver - Execution plan entry observer.
    * @param aPlugin - Suite plug-in that this plan entry represents.
    * @param aAsDependency - Whether this item is being executed as dependency or not.
    * @param aType - Type of execution. @see CDiagExecPlanEntryImpl::TType
    *
    * @return a new instance of CDiagExecPlanEntryImplSuite
    */
    static CDiagExecPlanEntryImplSuite* NewL( MDiagEngineCommon& aEngine,
                                              const TDiagEngineConfig& aEngineConfig,
                                              MDiagExecPlanEntryImplObserver& aObserver,
                                              MDiagSuitePlugin& aPlugin,
                                              TBool aAsDependency,
                                              TType aType );

    /**
    * Symbian two-phased constructor
    *  This will leave a entry in clean up stack.
    *
    * @param aEngine - Diagnostics Engine
    * @param aEngineConfig - Engine configuration.
    * @param aObserver - Execution plan entry observer.
    * @param aPlugin - Suite plug-in that this plan entry represents.
    * @param aAsDependency - Whether this item is being executed as dependency or not.
    * @param aType - Type of execution. @see CDiagExecPlanEntryImpl::TType
    *
    * @return a new instance of CDiagExecPlanEntryImplSuite
    */
    static CDiagExecPlanEntryImplSuite* NewLC( MDiagEngineCommon& aEngine,
                                               const TDiagEngineConfig& aEngineConfig,
                                               MDiagExecPlanEntryImplObserver& aObserver,
                                               MDiagSuitePlugin& aPlugin,
                                               TBool aAsDependency,
                                               TType aType );

    /**
    * C++ destructor
    */
    virtual ~CDiagExecPlanEntryImplSuite();

public:     // new public api
    /**
    * Get suite plugin associated.
    */
    MDiagSuitePlugin& SuitePlugin();

private:    // from CActive
    /*
    * @see CActive::RunL
    */
    virtual void RunL();

    /*
    * @see CActive::DoCancel
    */
    virtual void DoCancel();
    

private:     // from CDiagExecPlanEntryImpl
    /**
    * @see CDiagExecPlanEntryImpl::ExecuteL
    */
    virtual void ExecuteL();

    /**
    * @see CDiagEXecPlanEntryImpl::StopExecutionByWatchdogL
    */
    virtual void StopExecutionByWatchdogL();

    /**
    * @see CDiagEXecPlanEntryImpl::StopExecutionByClientL
    */
    virtual void DoStopExecutionByClientL( MDiagEngineCommon::TCancelMode aCancelMode );

    /**
    * @see CDiagEXecPlanEntryImpl::SuspendL
    */
    virtual void SuspendL();

    /**
    * @see CDiagEXecPlanEntryImpl::ResumeL
    */
    virtual void ResumeL();


private: // from MDiagSuiteObserver
    /**
    * Notify engine of suite plug-in execution completion.
    * 
    * @see MDiagSuitePlugin::ContinueExecutionL
    */
    virtual void ContinueExecutionL( const MDiagSuitePlugin& aSender );


private:    // private methods
    /**
    * C++ constructor
    *   
    * @param aEngine - Diagnostics Engine
    * @param aEngineConfig - Engine configuration.
    * @param aObserver - Execution plan entry observer.
    * @param aPlugin - Suite plug-in that this plan entry represents.
    * @param aAsDependency - Whether this item is being executed as dependency or not.
    * @param aType - Type of execution. @see CDiagExecPlanEntryImpl::TType
    */
    CDiagExecPlanEntryImplSuite( MDiagEngineCommon& aEngine,
                                 const TDiagEngineConfig& aEngineConfig,
                                 MDiagExecPlanEntryImplObserver& aObserver,
                                 MDiagSuitePlugin& aPlugin,
                                 TBool aAsDependency,
                                 TType aType );

    /**
    * Stop suite plug-in.
    * 
    * @param aStopReason - Stop reason to pass to suite.
    */
    void StopSuitePluginL( MDiagSuitePlugin::TStopReason aStopReason );
    };

#endif // DIAGEXECPLANENTRYIMPLSUITE_H

// End of File

