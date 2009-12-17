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
* Description:  Diagnostics Suite Plug-in Base class
*
*/


#ifndef DIAGSUITEPLUGINBASE_H
#define DIAGSUITEPLUGINBASE_H

// INCLUDES
#include <e32cmn.h>             // TUid
#include <DiagSuitePlugin.h>    // MDiagSuitePlugin
#include <ConeResLoader.h>      // RConeResourceLoader

// FORWARD DECLARATIONSn
class CDiagPluginConstructionParam;
class CAknDialog;

/**
*  Diagnostics Framework Suite Plugin Base Class
*
*  This base class provides common implementation of Suite Plugin.
*
*  @since S60 v5.0
*/
class CDiagSuitePluginBase : public CActive,
                             public MDiagSuitePlugin
    {
public:
    /**
    * Destructor. 
    */
    IMPORT_C virtual ~CDiagSuitePluginBase();

protected:  // new API for derived class.
    /**
    * C++ constructor
    * 
    * @param aConstructionParam Construction parameters.
    */
    IMPORT_C CDiagSuitePluginBase( CDiagPluginConstructionParam* aParam );
    
    /**
    * Base class constructor
    * This initializes CDiagSuitePluginBase class. Derived class must call this
    * method in its ConstructL() method.
    *  
    * @param aResourceFileName Drive and name of resource file in format
    *                          <path>:<rsc_file_name>
    */
    IMPORT_C void BaseConstructL( const TDesC& aResourceFileName );

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
    *   EEikDialogFlagWait flag set, it will panic with EDiagPluginBasePanicBadArgument
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

public: // from MDiagPlugin
    /**
    * Get the name of the service that the plug-in provides.
    *
    * @return The name of the service.
    **/
    IMPORT_C virtual const TDesC& ServiceLogicalName() const;

    /**
    * Get logical dependencies. One plug-in can have multiple dependencies 
    * to other plug-ins.
    *
    * @param aArray An array of logical names.
    * @see ServiceLogicalName.
    **/
    IMPORT_C virtual void GetLogicalDependenciesL( CPtrCArray& aArray ) const;

    /**
    * Return the type of the plug-in. 
    *
    * @return The type.
    * @see TPluginType.
    **/
    IMPORT_C virtual TPluginType Type() const;

    /**
    * Create an icon that represents the plug-in.
    *
    * @return An icon. 
    **/
    IMPORT_C virtual CGulIcon* CreateIconL() const;

    /**
    * Get the order number that this plug-in should appear in its parent list.
    *
    * @return TUint order number.
    **/
    IMPORT_C virtual TUint Order() const;

    /**
    * @see MDiagPlugin::IsSupported
    **/
    IMPORT_C virtual TBool IsSupported() const;

    /**
    * Get UID of the parent.
    *
    * @return The parent UID.
    **/
    IMPORT_C virtual TUid ParentUid() const;

    /**
    * @see MDiagPlugin::SetDTorIdKey()
    */
    IMPORT_C virtual void SetDtorIdKey( TUid aDtorIdKey );

    /**
    * Get title of the plugin. Default implementation in CDiagSuitePluginBase
    *   will leave with KErrNotSupported. If plug-in has a title, plug-ins 
    *   must override this method.
    * @see MDiagPlugin::GetTitleL() 
    */
    IMPORT_C virtual HBufC* GetTitleL() const;

    /**
    * Get description of the plugin. Default implementation in 
    *   CDiagSuitePluginBase will leave with KErrNotSupported. If plug-in 
    *   has a description, plug-ins must override this method.
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
    **/
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
    **/
    IMPORT_C virtual void TestSessionEndL( MDiagEngineCommon& aEngine,
                                           TBool aSkipDependencyCheck,
                                           TAny* aCustomParams );


public: // from MDiagSuitePlugin

    /**
    * Get children of this plug-in. The pointer array is guaranteed to 
    * be sorted defined by TSortOrder.
    *
    * @param aChildren Children are appended into this array.
    * @param aOrder Sorting algorithm.
    **/
    IMPORT_C virtual void GetChildrenL( RPointerArray<MDiagPlugin>& aChildren,
                                        TSortOrder aOrder ) const;

    /**
    * Add one child. Child can be either a test suite or a test plug-in.
    *
    * @param aChild - Child to be added. However, ownership is not
    *               transferred here. 
    **/
    IMPORT_C virtual void AddChildL( MDiagPlugin* aChild );

    /**
    * Get the Uids. The uid is used for database access.
    * Test suites return a list of their childrens' uids.
    *
    * @param aUids  An UID array.
    * @param aOrder Sorting algorithm.
    **/
    IMPORT_C virtual void GetChildrenUidsL( RArray<TUid>& aUids,
                                    TSortOrder aOrder ) const;

    /**
    * Called before one of its test plug-in is executed.  Note that it will
    * not be called if two of its children plug-ins are executed in 
    * sequence.
    *
    * @param aSkipDependencyCheck - If ETrue dependency is disabled 
    *   for this test session.
    * @param aDependencyExecution - If ETrue, this suite is being
    *   executed to satisfy dependency.
    **/
    IMPORT_C virtual void PrepareChildrenExecutionL( TDiagSuiteExecParam* aParam,
                                                     TBool aSkipDependencyCheck,
                                                     TBool aDependencyExecution );

    /**
    * Called before test execution switches to another test suite.
    *
    * @param aExecParams Parameters for suite post execution
    * @param aSkipDependencyCheck - If ETrue dependency is disabled 
    *   for this test session.
    * @param aDependencyExecution - If ETrue, this suite is being
    *   executed to satisfy dependency.
    **/
    IMPORT_C virtual void FinalizeChildrenExecutionL( TDiagSuiteExecParam* aParam,
                                                      TBool aSkipDependencyCheck,
                                                      TBool aDependencyExecution );

    /**
    * Cancels pre/post execution. Cancellation is expected to be synchronous.
    * Suite plug-in must not call ContinueExecutionL().
    *
    * @param aReason - Reason why ExecutionStopL() is being called.
    **/
    IMPORT_C virtual void ExecutionStopL( TStopReason aReason );

private:    // Private Data Types
    class TPrivateData;

private:    // Private data
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

#endif // DIAGSUITEPLUGINBASE_H

// End of File

