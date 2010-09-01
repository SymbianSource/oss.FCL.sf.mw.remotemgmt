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
* Description:  Common interface to use engine
*
*/


#ifndef DIAGENGINECOMMON_H
#define DIAGENGINECOMMON_H

// SYSTEM INCLUDES
#include <e32std.h>                     // TDesC, TTimeIntervalMicroSeconds32
#include <DiagCommonDialog.h>           // TDiagCommonDialog
#include <DiagAppCommand.h>             // TDiagAppCommand
#include <DiagEngineWatchdogTypes.h>    // TDiagEngineWatchdogTypes

// FORWARD DECLARATION
class MDiagPluginExecPlan;
class MDiagPlugin;
class CAknViewAppUi;
class RDiagResultsDatabaseRecord;
class CDiagPluginPool;
class CAknDialog;

/**
*  Diagnostics Framework Engine Common Interface
*
*  This interface is provided for Test plug-ins, Suite plug-ins and
*  any other clients of CDiagEngine.
*
*  @since S60 v5.0
*/
class MDiagEngineCommon
    {
public: // Data Structures
    
    /** 
    * Possible cancellation mode.
    */
    enum TCancelMode
        {
        ESkip,        // Cancel current test only. Continue rest.
        ECancelAll      // Cancel current test and skip rest of execution.
        };

    /**
    *  Text List based config options
    */
    enum TConfigListType
        { 
        /**
        * EConfigListCallIngore - List of phone numbers to ignore. 
        *   If a phone number is added to this list, engine will ignore the number
        *   a new MO/MT call is created by the phone, and will not suspend
        *   the engine execution when new call is made.
        */
        EConfigListCallIngore = 0
        };

public: // public methods

    /**
    * Returns current execution plan.
    * If test is not being executed, it will leave with KErrNotReady.
    *
    * @return Reference to current execution plan.
    */
    virtual const MDiagPluginExecPlan& ExecutionPlanL() const = 0;

    /**
    * Stop current execution of tests.
    * If test is not being executed, it will leave with KErrNotReady.
    *
    * @param aCancelMode   ESkip       - Skip current test only. Rest will continue.
    *                      ECancelAll  - Cancels current test and skips the rest of
    *                                  execution.
    */
    virtual void ExecutionStopL( TCancelMode aCancelMode ) = 0;

    /**
    * Resets test execution watchdog timer.  It will use the last given watchdog
    * timer value for currently executing plug-in.
    */
    virtual void ResetWatchdog() = 0;

    /**
    * Resets test execution watchdog timer.  
    *   Watchog timer value will be determined by the type of watchdog requested.
    *   Given value is valid until next plug-in is executed, or another
    *   ResetWachdogL() or TestProgressL() is called.
    *
    * @param aWatchdogType Type of watchdog timer vaule to use.
    *   @see TDiagEngineWatchdogTypes
    */
    virtual void ResetWatchdog( TDiagEngineWatchdogTypes aWatchdogType ) = 0;

    /**
    * Resets test execution watchdog timer.  
    *   Engine will allow specified amount of time. 
    *   Given value is valid until next plug-in is executed, or another
    *   ResetWachdogL() or TestProgressL() is called.
    * 
    * @param aTimeToWait - Amount of time to wait before watchdog is expired.
    */
    virtual void ResetWatchdog( TTimeIntervalMicroSeconds32 aExpectedTimeToComplete ) = 0;

    /**
    * Get App Ui reference that launched Diagnostics Engine.
    * AppUi reference can be used for tests that requires AppUi for view switching.
    * 
    * @return Reference to CAknViewAppUi
    */
    virtual CAknViewAppUi& ViewAppUi() = 0;

    /**
    * Get App Ui reference that launched Diagnostics Engine.
    * AppUi reference can be used for tests that requires AppUi for view switching.
    * 
    * @return Reference to CAknViewAppUi
    */
    virtual const CAknViewAppUi& ViewAppUi() const = 0;

    /**
    * Get Database Subsession for record open for writing.
    * 
    * @return Reference to RDiagResultsDatabaseRecord
    */
    virtual RDiagResultsDatabaseRecord& DbRecord() = 0;

    /**
    * Get Database Subsession for record open for writing.
    * 
    * @return Reference to RDiagResultsDatabaseRecord
    */
    virtual const RDiagResultsDatabaseRecord& DbRecord() const = 0;

    /**
    * Get reference to Plugin Pool
    * 
    * @return Reference to CDiagPluginPool
    */
    virtual CDiagPluginPool& PluginPool() = 0;

    /**
    * Get reference to Plugin Pool
    * 
    * @return Reference to CDiagPluginPool
    */
    virtual const CDiagPluginPool& PluginPool() const = 0;

    /**
    * Add a text to configuraiton list option. 
    * 
    * @param aListType - List type to add the configuration option to. 
    *   @see TConfigListType
    * @param aText - text to add to the list
    */
    virtual void AddToConfigListL( TConfigListType aListType, const TDesC& aText ) = 0;

    /**
    * Remove a text from configuration list.
    * If configuration item text does not exist in the list, this function will 
    * leave with KErrNotFound.
    * 
    * @param aListType - List type to remove the item from. 
    *   @see TConfigListType
    * @param aText - text for the option
    */
    virtual void RemoveFromConfigListL( TConfigListType aListType, const TDesC& aText ) = 0;

    /**
    * Create a common dialog.
    * Plug-ins can use this factory method to create a common dialog.
    * For available dialog types, @see DiagCommonDialog.h
    * 
    * @param aDialogType - Type of dialog to create.
    * @param aInitData - Initialization parameter. Ownership is passed to engine.
    * @return a pointer to newly created dialog. Ownership is passed to caller.
    */
    virtual CAknDialog* CreateCommonDialogLC( TDiagCommonDialog aDialogType,
                                              TAny* aInitData ) = 0;

    /**
    * Stop watchdog activity temporarily. Watchdog will stay inactive until:
    *   a ) TestProgressL() is called.
    *   b ) ResetWatchdogL() is called.
    *   c ) Plug-in is cancelled and the next plug-in starts to execute.
    */
    virtual void StopWatchdogTemporarily() = 0;

    /**
    * Check if current engine is being executed with dependency disabled.
    *   @return TBool - ETrue if dependency is disabled.
    *                   EFalse if depencenty is enabled.
    */
    virtual TBool IsDependencyDisabled() const = 0;

    /**
    * Get custom parameter passed during engine creation.
    *   @return TAny* Custom parameter.  Ownership is not transferred.
    */
    virtual TAny* CustomParam() const = 0;

    /**
    * Send a command to application to execute.. This can be used by the plug-in to
    * request certain behavior from application. For more information about
    * types of commands possible, @see TDiagAppCommand.
    * If the command is not supported by the application, this function will 
    * leave with error code KErrNotSupported.
    *
    * Memory Ownership of Parameters.
    *   Note that in general, parameters should be owned by the caller. This is 
    *   because if a particular operation is not supported, application 
    *   will not know how to deallocate the parameter, causing memory leak.
    *   
    *   In cases where ownership must be passed, caller must check if
    *   ExecuteAppCommandL() leaves KErrNotSupported and deallocate
    *   parameters.
    *
    * @param aCommand - Type of command to execute. @see TDiagAppCommand
    * @param aParam1 - Data for the command. Value of this parameter depends
    *   on the type of command. Ownership not transferred. @see TDiagAppCommand
    * @param aParam2 - Data for the command. Value of this parameter depends
    *   on the type of command. Ownership not transferred. @see TDiagAppCommand
    *   
    */
    virtual void ExecuteAppCommandL( TDiagAppCommand aCommand, 
                                     TAny* aParam1,
                                     TAny* aParam2 ) = 0;
    };


#endif // DIAGENGINECOMMON_H

// End of File

