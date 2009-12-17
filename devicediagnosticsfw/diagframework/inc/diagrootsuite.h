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
* Description:  Class declaration for CDiagRootSuite
*
*/


#ifndef DIAGROOTSUITE_H
#define DIAGROOTSUITE_H


// SYSTEM INCLUDES
#include <DiagSuitePlugin.h>        // MDiagSuitePlugin

// USER INCLUDES

// fORWARD DECLARATIONS

namespace DiagFwInternal
    {
/**
*  Diagnostics Root Suite
*
*  This class implements root suite. This provides a container for ALL suites
*  that can be loaded via Plug-in Pool.
*
*  @since S60 v5.0
*/
NONSHARABLE_CLASS( CDiagRootSuite ) : public CBase,
                                      public MDiagSuitePlugin
    {
public:

    /**
    * Two-phased constructors.
    *
    * @return New instance of CDiagRootSuite*
    */
    static CDiagRootSuite* NewL();
    static CDiagRootSuite* NewLC();
    
    /**
    * Destructor.
    */
    virtual ~CDiagRootSuite();

public: // from MDiagPlugin
    /**
    * Get name of the plug-in
    * @see MDiagPlugin::GetPluginNameL
    */
    virtual HBufC* GetPluginNameL( TNameLayoutType aLayoutType ) const;

    /**
    * Get name of the service it provides.
    * @see MDiagPlugin::ServiceLogicalName
    */
    virtual const TDesC& ServiceLogicalName() const;

    /**
    * Get list af dependencies it requires.
    * @see MDiagPlugin::GetLogicalDependenciesL
    */
    virtual void GetLogicalDependenciesL( CPtrCArray& aArray ) const;

    /**
    * Get Type of plug-in
    * @see MDiagPlugin::Type
    */
    virtual TPluginType Type() const;

    /**
    * Create an icon for this plug-in.
    * @see MDiagPlugin::CreateIconL
    */
    virtual CGulIcon* CreateIconL() const;

    /**
    * Whether this plug-in should be visible to user or not.
    * @see MDiagPlugin::IsVisible
    */
    virtual TBool IsVisible() const;

    /**
    * Whether this plug-in is supported in this build or not.
    * @see MDiagPlugin::IsSupported
    */
    virtual TBool IsSupported() const;

    /**
    * Get localized title of the plug-in
    * @see MDiagPlugin::GetTitleL
    */
    virtual HBufC* GetTitleL() const;

    /**
    * Get localized description of the plug-in
    * @see MDiagPlugin::GetDescriptionL
    */
    virtual HBufC* GetDescriptionL() const;

    /**
    * Uid of the plug-in
    * @see MDiagPlugin::Uid
    */
    virtual TUid Uid() const;

    /**
    * Uid of the parent plug-in
    * @see MDiagPlugin::ParentUid
    */
    virtual TUid ParentUid() const;

    /**
    * Set destructor key for ECOM plug-in
    * @see MDiagPlugin::SetDtorIdKey
    */
    virtual void SetDtorIdKey( TUid aDtorIdKey );

    /**
    * Custom operation
    * @see MDiagPlugin::CustomOperationL
    */
    virtual TAny* CustomOperationL( TUid aUid, TAny* aParam );

    /**
    * Custom operation
    * @see MDiagPlugin::GetCustomL
    */
    virtual TAny* GetCustomL( TUid aUid, TAny* aParam );

    /**
    * Order to appear in the parent
    * @see MDiagPlugin::Order
    */
    virtual TUint Order() const;

    /**
    * Initialize the plug-in for execution.
    * @see MDiagPlugin::TestSessionBeginL
    */
    virtual void TestSessionBeginL( MDiagEngineCommon& aEngine,
                                    TBool aSkipDependencyCheck,
                                    TAny* aCustomParams );

    /**
    * Cleanup the plug-in after execution.
    * @see MDiagPlugin::TestSessionEndL
    */
    virtual void TestSessionEndL( MDiagEngineCommon& aEngine,
                                  TBool aSkipDependencyCheck,
                                  TAny* aCustomParams );

public: // from MDiagSuitePlugin
    /**
    * Get list of children in the specified order.
    * @see MDiagSuitePlugin::GetChildrenL
    */
    virtual void GetChildrenL( RPointerArray<MDiagPlugin>& aChildren,
                               TSortOrder aOrder ) const;

    /**
    * Add child 
    * @see MDiagSuitePlugin::AddChildL
    */
    virtual void AddChildL( MDiagPlugin* aChild );

    /**
    * Prepare suite entering one of its children.
    * @see MDiagSuitePlugin::PrepareExecutionL
    */
    virtual void PrepareChildrenExecutionL( TDiagSuiteExecParam* aParam,
                                            TBool aSkipDependencyCheck,
                                            TBool aDependencyExecution );

    /**
    * Finalize suite for exiting one of its children.
    * @see MDiagSuitePlugin::FinalizeExecutionL
    */
    virtual void FinalizeChildrenExecutionL( TDiagSuiteExecParam* aParam,
                                             TBool aSkipDependencyCheck,
                                             TBool aDependencyExecution );

    /**
    * Get list of uids of its children it the specified order.
    * @see MDiagSuitePlugin::GetChildrenUidsL
    */
    virtual void GetChildrenUidsL( RArray<TUid>& aUids,
                                   TSortOrder aOrder ) const;

    /**
    * Stop execution of PrepareExecutionL or FinalizeExecutionL
    * @see MDiagSuitePlugin::ExecutionStopL
    */
    virtual void ExecutionStopL( TStopReason aReason );


private: // private constructors

    /**
    * C++ Constructor.
    **/
    CDiagRootSuite();
    
    /**
    * 2nd phase constructor
    */
    void ConstructL();


private: // data
    
    /**
    * iChildren - list of plug-ins it owns.
    */
    RPointerArray<MDiagPlugin> iChildren;
    };
    } // namespace DiagFwInternal
#endif // DIAGROOTSUITE_H

// End of File

