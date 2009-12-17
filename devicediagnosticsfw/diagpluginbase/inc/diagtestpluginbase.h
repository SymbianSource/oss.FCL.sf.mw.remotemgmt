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
* Description:  Diagnostics Test Plug-in  Base class
*
*/


#ifndef DIAGTESTPLUGINBASE_H
#define DIAGTESTPLUGINBASE_H

// INCLUDES
#include <DiagTestPlugin.h>     // MDiagTestPlugin
#include <ConeResLoader.h>      // RConeResourceLoader
#include <DiagResultsDatabaseItem.h>    // CDiagResultsDatabaseItem::TResult
#include <DiagEngineWatchdogTypes.h>    // TDiagEngineWatchdogTypes

// FORWARD DECLARATIONS
class CDiagResultsDbItemBuilder;
class TDiagTestPluginBasePrivateData;
class CDiagPluginConstructionParam;
class CAknDialog;

/**
*  Diagnostics Framework Test Plugin  Base Class
*
*  This class provides further simplification of test execution.
*  It handles execution parameter and reporting results to engine
*
*  @since S60 v5.0
*/
class CDiagTestPluginBase : public CActive,
                            public MDiagTestPlugin
    {
public: // C++ Destructor
    /**
    * Destructor.
    */
    IMPORT_C virtual ~CDiagTestPluginBase();

protected: // new API for derived class
    /**
    * C++ constructor
    * 
    *   Note that BaseConstructL() must be called to complete object construction.
    * @param aConstructionParam Construction parameters.
    */
    IMPORT_C CDiagTestPluginBase( CDiagPluginConstructionParam* aConstructionParam );
    
    /**
    * 2nd phase Base class constructor
    * This initializes CDiagTestPluginBase class. Derived class must call this
    * method in its ConstructL() method.
    *  
    * @param aResourceFileName Drive and name of resource file in format
    *                          <path>:<rsc_file_name>
    */
    IMPORT_C void BaseConstructL( const TDesC& aResourceFileName );

    /**
    * Verify that all dependencies are satisfied. Also returns a list of
    * failed dependencies.
    *
    * @param aEngine. Reference to engine, which provides references to
    *   database session and plug-in pool.
    * @param aFailedUids. If ETrue is returned, the parameter is NULL.
    *           if EFalse is returned, the parameter points to an array that
    *           can be used to query UIDs of failed dependencies. Client is
    *           responsible of closing the array.
    * @return ETrue if all dependencies are executed successfully.
    *         EFalse if one or more dependencies are failed.
    */
    IMPORT_C virtual TBool AreDependenciesSatisfiedL( MDiagEngineCommon& aEngine,
                                            RArray<TUid>*& aFailedUids ) const;

    /**
    * Check result of dependencies. This will check each dependency and
    * determine the best overall result of all the dependencies.
    *
    * @param aEngine. Reference to engine, which provides references to
    *   database session and plug-in pool.
    * @param aFailedUids. If ESuccess is returned, the parameter is NULL.
    *           Otherwise the parameter points to an array that
    *           can be used to query UIDs of failed dependencies. Client is
    *           responsible of closing the array.
    * @return Result of the dependency check. If multiple depencies or suites
    *   are included, it will return the most appropreate result.
    */
    IMPORT_C virtual CDiagResultsDatabaseItem::TResult VerifyDependenciesL( 
        MDiagEngineCommon& aEngine,
        RArray<TUid>*& aFailedUids ) const;

    /**
    * Returns execution parameter. 
    *   Note that this function should be called only during test execution. 
    *   If called in other states, it will Panic with EDiagPluginBasePanicInvalidState
    *
    * @return Execution parameter.
    */
    IMPORT_C TDiagTestExecParam& ExecutionParam();

    /**
    * Check if current test session has dependency check skip flag set.
    *   Note that this function should be called only during test execution. 
    *   If called in other states, it will Panic with EDiagPluginBasePanicInvalidState
    * 
    * @return ETrue if dependency check is disabled for this execution sesison.
    *   EFalse if dependency check is enabled.
    */
    IMPORT_C TBool IsDependencyCheckSkipped() const;

    /**
    * Check if current test is being executed to satisfy dependency or
    *   if it is being executed explicitly.
    *   Note that this function should be called only during test execution. 
    *   If called in other states, it will Panic with EDiagPluginBasePanicInvalidState
    *
    * @return ETrue - If it is to satisfy dependency.
    *   EFalse - If it is being executed explicitly.
    */
    IMPORT_C TBool IsDependencyExecution() const;

    /**
    * Get custom parameter passed to from the engine
    *   Note that this function should be called only during test execution. 
    *   If called in other states, it will Panic with EDiagPluginBasePanicInvalidState
    * 
    * @return Custom parameter. Ownership is not transferred.
    */
    IMPORT_C TAny* CustomParam() const;

    /**
    * Get currently result db item builder.
    *   Note that this function should be called only during test execution. 
    *   If called in other states, it will Panic with EDiagPluginBasePanicInvalidState
    *
    * @return Reference to current CDiagResultsDbItemBuilder.
    */
    IMPORT_C CDiagResultsDbItemBuilder& ResultsDbItemBuilder();

    /**
    * Report test progress test observer.
    *   This function will call engine's TestProgressL method with watchdog
    *   type value that is based on plug-in RunMode.
    *
    *   If EAutomatic plug-in, it will use EDiagEngineWatchdogTypeNonInteractive.
    *   If EInteractiveView or EInteractiveDialog, EDiagEngineWatchdogTypeInteractive. 
    *
    *   When watchdog is timed out, it will stop test with test result
    *   CDiagResultsDatabaseItem::EWatchdogCancel.
    *
    *   Note that this function should be called only during test execution. 
    *   If called in other states, it will Panic with EDiagPluginBasePanicInvalidState
    *
    * @param aCurrentStep - Current execution step
    */
    IMPORT_C void ReportTestProgressL( TUint aCurrentStep );

    /**
    * Reset watchdog value and watchdog failure result type.
    *   When watchdog is timed out, it will use the given result type as
    *   value to report back to engine.  
    *
    *   Note that this function should be called only during test execution. 
    *   If called in other states, it will Panic with EDiagPluginBasePanicInvalidState
    *
    * @param aWatchdogType - Type of watchdog requested.
    * @param aResultType - Result to report when wachdog is failed.
    */
    IMPORT_C void ResetWatchdog( TDiagEngineWatchdogTypes aWatchdogType,
                                 CDiagResultsDatabaseItem::TResult aResultType );

    /**
    * Reset watchdog value and watchdog failure result type.
    *   For most plug-ins, the other ResetWatchdog should be better. 
    *   This should be used only if specific time must be given for the step.
    *
    *   When watchdog is timed out, it will use the given result type as
    *   value to report back to engine.  
    *
    *   Note that this function should be called only during test execution. 
    *   If called in other states, it will Panic with EDiagPluginBasePanicInvalidState
    *
    * @param aTimeToCompletion - Time needed to complete current operation.
    *   Unit is in microseconds.
    * @param aResultType - Result to report when wachdog is failed.
    */
    IMPORT_C void ResetWatchdog( 
        TInt aTimeToCompletion,
        CDiagResultsDatabaseItem::TResult aResultType );

    /**
    * Reset watchdog value and watchdog failure result type to default values.
    *   This will also reset watchdog on engine.
    *
    *   Note that this function should be called only during test execution. 
    *   If called in other states, it will Panic with EDiagPluginBasePanicInvalidState
    *
    */
    IMPORT_C void ResetWatchdogToDefault();

    /**
    * Report test result to test observer. 
    *   Note that this function should be called only during test execution. 
    *   If called in other states, it will Panic with EDiagPluginBasePanicInvalidState
    *
    *   This function will also call StopAndCleanupL(), which will call 
    *   DoStopAndCleanupL(). After CompleteTestL() is called, ExecutionParam() and
    *   other test state specific API will fail.
    *
    * @param aResult - Test result.
    */
    IMPORT_C void CompleteTestL( CDiagResultsDatabaseItem::TResult aResult );

    /**
    * Stop current execution and free any data associated with the test.
    *   This method will call DoStopAndCleanupL() function to allow
    *   derived class to clean up.
    *   
    *   It will also call DismissWaitingDialog() and CActive::Cancel().
    *
    *   If derived class overrides ExecutionStopL() function, this function
    *   must be called by derived version of ExecutionStopL() to allow
    *   proper clean up of CDiagTestPluginBase.
    *
    *   @see CDiagTestPluginBase::ExecutionStopL
    */
    IMPORT_C void StopAndCleanupL();

    /**
    * Run a dialog that waits for response. It is highly recommended that
    * all plug-ins use this function to display dialogs since it can detect
    * deletion of dialogs.
    *
    * The difference from normal dialog RunLD is that this function returns 
    * ETrue if dialog exited due to user response and 
    * EFalse if it was by other means, such as object deletion or
    * by DismissWaitingDialog() method. 
    * 
    *   !!!! NOTE THAT PLUG-IN MUST RETURN IMMEDIATELY WITHOUT ACCESSING  !!!!
    *   !!!! LOCAL VARIABLE OR LOCAL FUNCITONS WHEN THIS FUNCTION RETURNS !!!!
    *   !!!! EFalse VALUE BECAUSE "THIS" POINTER MAY BE FREED ALREADY.    !!!!
    * 
    * The normal dialog response is returned via aDialogResponse reference.
    * This function can only display one dialog at a time. Calling it again 
    * while it has not been returned will cause panic.
    *
    * @param aDialog - Pointer to dialog to run. If the dialog does not have
    *   EEikDialogFlagWait flag set, it will panic with EDiagPluginBaseBadArgument
    * @param aDialogResponse - Response from the dialog.  
    *   For detailed values @see avkon.hrh "CBA constants". E.g. EAknSoftkeyYes
    *   The only exception is that if cancel is pressed, it will be 0.
    * @return ETrue if dialog is exiting due to user response.
    *   Efalse if dialog is dismissed withou user input. 
    *   If EFalse is returned, plug-in MUST NOT act on the response.
    *   Also, since plug-in may have been deleted,
    */
    IMPORT_C TBool RunWaitingDialogL( CAknDialog* aDialog, TInt& aDialogResponse );

    /**
    * Dismiss the waiting dialog. This will cause the RunDialogLD() function to return with
    * aIsUserResponse = Efalse. If nothing is being displayed, this function
    * does nothing.
    */
    IMPORT_C void DismissWaitingDialog();

    /**
    * CCoeEnv
    *   @return Reference to CCoeEnv.
    */
    IMPORT_C CCoeEnv& CoeEnv();

    /**
    * Check if current test is being executed as a single plugin or 
    * as a part of a suite
    *   Note that this function should be called only during test execution. 
    *   If called in other states, it will Panic with EDiagPluginBasePanicInvalidState
    *
    * @return ETrue - If run as a single plugin
    *   EFalse - If run as a part of a suite
    */
    IMPORT_C TBool SinglePluginExecution() const;
    
protected:  // to be implemented by derived classes
    /**
    * Execute test. This is called by CDiagTestPluginBase::RunTestL()
    *   Note that CDiagTestPluginBase takes ownership of all parameters
    *   given in RunTestL() function. If specific parameter is needed
    *   use the following set of functions:
    *
    *   Parameters are available via the following APIs.
    *       Observer:               ExecutionParam().Observer();
    *       Engine:                 ExecutionParam().Engine();
    *       Dependency Execution:   IsDependencyExecution();
    *       Skip Depdency:          IsDependencyCheckSkipped();
    *       Custom Parameter:       CustomParam();
    *
    *   Also, a new instance of CDiagResultsDbItemBuilder is created and
    *   accessible via:
    *
    *       Result Db Item Builder: ResultsDbItemBuilder();
    */
    virtual void DoRunTestL() = 0;

    /**
    * Stop current execution and free any data associated with the test.
    *   This is called by StopAndCleanupL
    */
    virtual void DoStopAndCleanupL() = 0;

    /**
    * This is called by CDiagTestPluginBase::ExecutionStopL() to allow 
    *   the derived class to provide additional behavior in ExecutionStopL().
    *   Default implementation does nothing. For more information about
    *   how this is called, @see CDiagTestPluginBase::ExecutionStopL()
    *
    * @param aReason - Reason why ExecutionStopL() is being called.
    * @param aTestResult - Test result to write to in database.
    *   This is IN/OUT parameter. By default, it contains default result based on
    *   aReason. Plug-in can provide different test result, assign new result
    *   to this variable.
    *
    */
    IMPORT_C virtual void DoExecutionStopL( MDiagTestPlugin::TStopReason aReason,
                                            CDiagResultsDatabaseItem::TResult& aTestResult );

protected:  // From CActive
    /**
    * RunError. Handle leaves from test. If test is currently running, 
    * CDiagTestPluginBase will handle the error by calling 
    * CompleteTestL( CDiagResultsDatabaseItem::EFailed ). If test was
    * not running, it will return the error back to active scheduler.
    * 
    * If this behavior is not desired, derived class should override this
    * method.
    * 
    * For parameters and return value, 
    * @see CActive::RunError
    */
    IMPORT_C virtual TInt RunError( TInt aError );

protected: // from MDiagTestPlugin
    /**
    * Handle execution stop.
    *   @see MDiagTestPlugin::ExecutionStopL()
    * 
    *   This method does the following:
    *       a) Call DoExecutionStopL()   
    *       b) Call StopAndCleanupL()  -> Calls DoStopAndCleanupL()
    *       c) Return either EWatchdogCancel or ECancelled depending on
    *           the stop reason. 
    *   
    *   Note: If derived class wants override this function, be sure to 
    *   call StopAndCleanupL() to allow proper clean up of 
    *   CDiagTestPluginBase class. StopAndCleanupL() will call
    *   DoStopAndCleanupL(). 
    *
    *   @see DoExecutionStopL()
    *   @see StopAndCleanupL() 
    */
    IMPORT_C virtual CDiagResultsDatabaseItem* ExecutionStopL( TStopReason aReason );

protected:    // from MDiagTestPlugin
    /**
    * Get the name of the service that the plug-in provides.
    *
    * @return The name of the service.
    */
    IMPORT_C virtual const TDesC& ServiceLogicalName() const;

    /**
    * Get logical dependencies. One plug-in can have multiple dependencies to 
    * other plug-ins.
    *
    * @param aArray An array of logical names.
    * @see ServiceLogicalNameL
    */
    IMPORT_C virtual void GetLogicalDependenciesL( CPtrCArray& aArray ) const;

    /**
    * Return the type of the plug-in. 
    *
    * @return The type.
    * @see TPluginType.
    */
    IMPORT_C virtual TPluginType Type() const;

    /**
    * Create an icon that represents the plug-in.
    *
    * @return An icon. 
    */
    IMPORT_C virtual CGulIcon* CreateIconL() const;

    /**
    * @see MDiagPlugin::IsSupported
    */
    IMPORT_C virtual TBool IsSupported() const;

    /**
    * Get the order number that this plug-in should appear in its parent list.
    *
    * @return TUint order number.
    */
    IMPORT_C virtual TUint Order() const;

    /**
    * Get UID of the parent.
    *
    * @return The parent UID.
    */
    IMPORT_C virtual TUid ParentUid() const;

    /**
    * @see MDiagPlugin::SetDTorIdKey()
    */
    IMPORT_C virtual void SetDtorIdKey( TUid aDtorIdKey );

    /**
    * Get title of the plugin. Default implementation in CDiagTestPluginBase 
    *   will leave with KErrNotSupported. If plug-in has a title, plug-ins 
    *   must override this method.
    * @see MDiagPlugin::GetTitleL() 
    */
    IMPORT_C virtual HBufC* GetTitleL() const;

    /**
    * Get description of the plugin. Default implementation in 
    *   CDiagTestPluginBase will leave with KErrNotSupported. If plug-in 
    *   has a description, plug-in must override this method.
    * @see MDiagPlugin::GetDescriptionL()
    */
    IMPORT_C virtual HBufC* GetDescriptionL() const;

    /**
    * Reserved for future use/plugin's custom functionality. 
    *
    * @param aUid   Unique identifier of the operation.
    * @param aParam Custom parameter. 
    * @return TAny pointer. Custom data.
    */
    IMPORT_C virtual TAny* CustomOperationL( TUid aUid, TAny* aParam );

    /**
    * Reserved for future use/plugin's custom functionality. 
    *
    * @param aUid   Unique identifier of the property
    * @param aParam Custom parameter.
    * @return TAny pointer. Custom data. 
    */
    IMPORT_C virtual TAny* GetCustomL( TUid aUid, TAny* aParam );

    /**
    * Initialization Step. This method is called before any plugin are executed.
    * This can be used to clean up any left over data from previous execution 
    * sessions. All plug-ins in execution plan will have a chance to clean 
    * up before any plug-ins are run.  This is a synchrouns method.
    *
    * @param aEngine - Reference to engine.
    * @param aSkipDependencyCheck - If ETrue, plug-in will be executed
    *   even if dependencies are not executed.
    * @param aCustomParams Custom parameters for plug-ins. 
    *   It can used to pass arbitrary data from application to the plug-ins.
    *   Owership is not transferred and plug-in must not delete
    *   this parameter.
    */
    IMPORT_C virtual void TestSessionBeginL( MDiagEngineCommon& aEngine,
                                             TBool aSkipDependencyCheck,
                                             TAny* aCustomParams );

    /**
    * Cleanup Step. This method is called after all plug-ins in the 
    * execution plan is completed to clean up any left over data from 
    * current sesison. This can be used to clean up any data that
    * provides dependent service created for its dependencies.
    * This is a synchrouns method.
    *
    * @param aEngine - Reference to engine.
    * @param aSkipDependencyCheck - If ETrue, plug-in as executed
    *   even if dependencies are not executed.
    * @param aCustomParams Custom parameters for plug-ins. 
    *   It can used to pass arbitrary data from application to the plug-ins.
    *   Owership is not transferred and plug-in must not delete
    *   this parameter.
    */
    IMPORT_C virtual void TestSessionEndL( MDiagEngineCommon& aEngine,
                                           TBool aSkipDependencyCheck,
                                           TAny* aCustomParams );


    /**
    * Execute diagnostics test.
    * @see MDiagTestPlugin::RunTestL
    */
    IMPORT_C virtual void RunTestL( TDiagTestExecParam* aExecParam,
                                    TBool aSkipDependencyCheck,
                                    TBool aDependencyExecution,
                                    TAny* aCustomParams );

    /**
    * Suspend test.  
    * @see MDiagTestPlugin::SuspendL
    */
    IMPORT_C virtual void SuspendL();

    /**
    * Resume test. 
    * @see MDiagTestPlugin::ResumeL
    */
    IMPORT_C virtual void ResumeL();

    /**
    * Create test result detail.
    * @see CDiagTestPluginBase::CreateDetailL
    */
    IMPORT_C virtual MDiagResultDetail* CreateDetailL( 
        const CDiagResultsDatabaseItem& aResult ) const;

private:    // private functions
    /**
    * Stop and clean up CDiagTestPluginBase
    */
    void BaseStopAndCleanup();

    /**
    * Utility function to get all dependent test plug-ins. If suite is found,
    * it will be expanded to test plug-ins.

    * @param aEngine. Reference to engine, which provides references to
    *   database session and plug-in pool.
    * @param aPluginList - Output array. Upon completion, it will hold
    *   list of test plug-ins. If this plug-in does not depend on any tests, 
    *   it will be empty.
    *   Ownership of individual items are not trasferred, so ResetAndDestroy()
    *   must not be called.
    */
    void GetAllDependentTestsL( MDiagEngineCommon& aEngine,
                                RPointerArray< MDiagTestPlugin >& aPluginList ) const;

    /**
    * Utility function to check results of tests in array and come up with
    * best overall result of tests. 
    * @param aPluginList - List of test plug-ins.
    * @param aFailedUidList - Output array. Upon completion, it will contain
    *   list of uids that failed.
    * @return Overall result of dependent tets.
    */
    CDiagResultsDatabaseItem::TResult SummarizeOverallTestResultsL( 
        MDiagEngineCommon& aEngine,
        const RPointerArray< MDiagTestPlugin >& aPluginList,
        RArray< TUid >& aFailedUidList ) const;


private:    // Private Data Type
    // Forward declaration.
    class TPrivateData;

private:    // Private Data
    /**
    * Plug-in constructor parameter.
    * Ownership: this
    */
    CDiagPluginConstructionParam* iConstructionParam;

    /**
    * CCoeEnv. This is needed for handling resources. 
    */
    CCoeEnv& iCoeEnv;

    /**
    * Plug-in base's private data
    */
    TPrivateData* iData;
    };

#endif // DIAGTESTPLUGINBASE_H

// End of File

