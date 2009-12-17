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
* Description:  Class declaration for CDiagExecPlanEntryImpl
*
*/


#ifndef DIAGEXECPLANENTRYIMPL_H
#define DIAGEXECPLANENTRYIMPL_H

// SYSTEM INCLUDE FILES
#include <e32base.h>                // CBase
#include <DiagExecPlanEntry.h>      // MDiagExecPlanEntry
#include <DiagEngineCommon.h>       // MDiagEngineCommon::TCancelMode

class MDiagExecPlanEntryImplObserver;
class TDiagEngineConfig;

/**
* Diagnostics Plug-in Execution Plan Entry Implementation.
*
* This class implements MDiagExecPlanEntry. It is meant to be private
*   to Diagnostics Framework and not exported.
*
* @since S60 v5.0
*
*/
NONSHARABLE_CLASS( CDiagExecPlanEntryImpl ) :  public CActive,
                                               public MDiagExecPlanEntry
    {
public:     // Data Types
    /**
    * TType - Execution plan entry type. 
    *   ETypeTestExec           - Test plugin execution.
    *   ETypeSuitePrepare       - Suite prepare execution
    *   ETypeSuiteFinalize      - Suite finalize execution
    *   ETypeSuiteUnexpanded    - Suite before being expanded into 
    *                               ETypeSuitePrepare or ETypeSuiteFinalize
    */
    enum TType
        {
        ETypeTestExec = 0,      
        ETypeSuitePrepare,      
        ETypeSuiteFinalize,     
        ETypeSuiteUnexpanded,   
        };

public:     // Public API
    /**
    * C++ destructor 
    */
    virtual ~CDiagExecPlanEntryImpl();

    /**
    * Plugin associated with the entry
    *   @return Reference to the plugin.
    */
    MDiagPlugin& Plugin();      //lint !e1411  This is proper overload

    /**
    * Get item type
    *   @return current item type
    */
    TType Type() const;

    /**
    * Update item type
    *   @param aType - new type.
    */
    void SetType( TType aType );

    /**
    * Update dependency
    *   @param aAsDependency - ETrue if item is being executed to satisfy 
    *       dependency. EFalse otherwise.
    */
    void SetAsDependency( TBool aAsDependency );

    /**
    * Handle cancel execution by client. Calling this will case IsStoppedByClient()
    * to return ETrue.
    *
    * @param aCancelMode - Cancellation mode.
    */
    void StopExecutionByClientL( MDiagEngineCommon::TCancelMode aCancelMode );

    /**
    * Check if test was stopped by client.
    *
    * @return ETrue if it was stopped by StopExecutionByClientL call. 
    *   EFalse otherwise.
    */
    TBool IsStoppedByClient() const;

    /**
    * Reset Watchdog to last known value.  
    * If watchdog was suspended, it will resume. 
    * If watchdog was not running, this does nothing.
    */
    void ResetWatchdog();

    /**
    * Reset Watchdog to one of the default based on watchdog type
    * If watchdog was suspended, it will resume. 
    * If watchdog was not running, this does nothing.
    * @param aWatchdogType - type of watchdog to use.
    */
    void ResetWatchdog( TDiagEngineWatchdogTypes aWatchdogType );

    /**
    * Reset watchdog to a specific value.
    * If watchdog was suspended, it will resume. 
    * If watchdog was not running, this does nothing.
    * @param aTimer value - watchdog timer value. 
    */
    void ResetWatchdog( TTimeIntervalMicroSeconds32 aWatchdogValue );

    /**
    * Stop watchdog temporarily.
    */
    void StopWatchdogTemporarily();

public:     // APIs to be implemented by derived class
    /**
    * Execute plan entry. To cancel request, call Cancel()
    */
    virtual void ExecuteL() = 0;

    /**
    * Suspend execution.
    */
    virtual void SuspendL() = 0;

    /**
    * Resume suspended execution.
    */
    virtual void ResumeL() = 0;

public:    // From MDiagExecPlanEntry
    /**
    * Plugin associated with the entry
    *   @see MDiagExecPlanEntry::Plugin
    */
    const MDiagPlugin& Plugin() const;

    /**
    * Returns whether item is being executed to satisfy dependency or not.
    *   @see MDiagExecPlanEntry::AsDepndent
    */
    TBool AsDependency() const;

    /**
    * Item State
    *   @see MDiagExecPlanEntry::State
    */
    TState State() const;


protected:  // API for derived class
    /**
    * C++ Constructor
    *   This is protected since it should not be created directly.
    *   Use one of the derived class version.
    *
    *   @param aEngine - Reference to the engine.
    *   @param aEngineConifg - Reference to the engine configuration.
    *   @param aPlugin - Reference to the plug-in.
    *   @param aAsDependent - Whether it is being executed as dependent test.
    *   @param aType - Type of entry. @see CDiagExecPlanEntryImpl::TType
    */
    CDiagExecPlanEntryImpl( MDiagEngineCommon& aEngine,
                            const TDiagEngineConfig& aEngineConfig,
                            MDiagExecPlanEntryImplObserver& aObserver,
                            MDiagPlugin& aPlugin,
                            TBool aAsDependency,
                            TType aType );

    /**
    * Get Engine reference
    * 
    * @return reference to engine.
    */
    MDiagEngineCommon& Engine();

    /**
    * Get Engine configuration reference
    * 
    * @return Reference to engine configuration.
    */
    const TDiagEngineConfig& EngineConfig() const;

    /**
    * Get observer.
    * 
    * @return Reference to plan entry observer.
    */
    MDiagExecPlanEntryImplObserver& Observer();

    /**
    * Change current state to a new state.  
    *   @param aState - new state to set to.
    */
    void ChangeStateL( TState aState );

protected:  // API to be implemented by derived class.
    /**
    * Handle cancel execution by client.
    *
    * @param aCancelMode - Cancellation mode.
    */
    virtual void DoStopExecutionByClientL( MDiagEngineCommon::TCancelMode aCancelMode ) = 0;

    /**
    * Watchdog timeout stop.
    */
    virtual void StopExecutionByWatchdogL() = 0;


private:    // private methods
    /**
    * Watchdog timer timeout handler.
    * @param aPtr - pointer to *this* object.
    */
    static TInt WatchdogTimerExpiredL( TAny* aPtr );

private:    // Private data
    /**
    * Engine
    */
    MDiagEngineCommon& iEngine;

    /**
    * Engine Configuration.
    */
    const TDiagEngineConfig& iEngineConfig;

    /**
    * Observer
    */
    MDiagExecPlanEntryImplObserver& iObserver;

    /**
    * Plug-in
    */
    MDiagPlugin& iPlugin;

    /**
    * Indicates whether it is being executed as dependent test.
    */
    TBool iAsDependency;

    /**
    * Indicates whether execution is stopped by client.
    */
    TBool iStoppedByClient;

    /**
    * Current item state
    *   @see MDiagExecPlanEntry::TState
    */
    TState iState;

    /**
    * Current item type
    *   @see CDiagExecPlanEntryImpl::TType
    */
    TType iType;
    
    /**
    * Watchdog timer value. It is a microsecond value to be used with 
    * watchdog timer.
    */
    TTimeIntervalMicroSeconds32 iWatchdogValue;

    /**
    * Watchdog timer
    * Owership: this.
    */
    CPeriodic* iWatchdogTimer;

    };

#endif // DIAGEXECPLANENTRY_H

// End of File

