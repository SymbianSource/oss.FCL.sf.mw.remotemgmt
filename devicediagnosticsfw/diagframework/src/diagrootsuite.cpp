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
* Description:  Class definition of CDiagRootSuite
*
*/


// CLASS DECLARATION
#include "diagrootsuite.h"

// SYSTEM INCLUDE FILES
#include <DiagSuiteExecParam.h>     // TDiagSuiteExecParam
#include <DiagSuiteObserver.h>      // MDiagSuiteObserver

// USER INCLUDE FILES
#include "diagframework.pan"        // panic codes.
#include "diagrootsuite.h"          // CDiagRootSuite

using namespace DiagFwInternal;

// ======== LOCAL FUNCTIONS ========
/**
* Compares two plug-ins by order.  Used for RArray  InsertInOrderAllowRepeats
* @param aFirst - first plug-in
* @param aSecond - second plug-in
* @return   positive if aFirst > aSecond
*           0 if same.
*           negative if aFirst < aSecond
*/
static TInt ComparePluginByOrder( const MDiagPlugin& aFirst,
                                   const MDiagPlugin& aSecond )
    {
    return aFirst.Order() - aSecond.Order();
    }


// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// CDiagRootSuite::ConstructL()
// ---------------------------------------------------------------------------
//
void CDiagRootSuite::ConstructL()
    {
    }


// ---------------------------------------------------------------------------
// CDiagRootSuite::NewL()
// ---------------------------------------------------------------------------
//
CDiagRootSuite* CDiagRootSuite::NewL()
    {
    CDiagRootSuite* self = CDiagRootSuite::NewLC();
    CleanupStack::Pop( self );
    return self;
    }


// ---------------------------------------------------------------------------
// CDiagRootSuite::NewLC()
// ---------------------------------------------------------------------------
//
CDiagRootSuite* CDiagRootSuite::NewLC()
    {
    CDiagRootSuite* self = new( ELeave )CDiagRootSuite();
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

// ---------------------------------------------------------------------------
// CDiagRootSuite::CDiagRootSuite
// ---------------------------------------------------------------------------
//
CDiagRootSuite::CDiagRootSuite()
    :   iChildren()
    {
    }

// ---------------------------------------------------------------------------
// CDiagRootSuite::~CDiagRootSuite
// ---------------------------------------------------------------------------
//
CDiagRootSuite::~CDiagRootSuite()
    {
    iChildren.Close();
    }


// ---------------------------------------------------------------------------
// from MDiagPlugin
// CDiagRootSuite::GetPluginNameL
// ---------------------------------------------------------------------------
//
HBufC* CDiagRootSuite::GetPluginNameL( TNameLayoutType /* aLayoutType */) const
    {
    _LIT( KRootSuitePluginName, "*Root Suite" );
    return KRootSuitePluginName().AllocL();
    }

// ---------------------------------------------------------------------------
// from MDiagPlugin
// CDiagRootSuite::ServiceLogicalName
// ---------------------------------------------------------------------------
//
const TDesC& CDiagRootSuite::ServiceLogicalName() const
    {
    _LIT( KRootSuiteServiceName, "" );
    return KRootSuiteServiceName();
    }

// ---------------------------------------------------------------------------
// From MDiagPlugin
// CDiagRootSuite::GetLogicalDependenciesL
// ---------------------------------------------------------------------------
//
void CDiagRootSuite::GetLogicalDependenciesL( CPtrCArray& /* aArray */ ) const
    {
    }


// ---------------------------------------------------------------------------
// From MDiagPlugin
// CDiagRootSuite::Type
// ---------------------------------------------------------------------------
//
MDiagPlugin::TPluginType CDiagRootSuite::Type() const
    {
    return ETypeSuitePlugin;
    }

// ---------------------------------------------------------------------------
// From MDiagPlugin
// CDiagRootSuite::CreateIconL
// ---------------------------------------------------------------------------
//
CGulIcon* CDiagRootSuite::CreateIconL() const
    {
    return NULL;
    }


// ---------------------------------------------------------------------------
// From MDiagPlugin
// CDiagRootSuite::IsVisible
// ---------------------------------------------------------------------------
//
TBool CDiagRootSuite::IsVisible() const
    {
    return EFalse;
    }

// ---------------------------------------------------------------------------
// From MDiagPlugin
// CDiagRootSuite::IsSupported
// ---------------------------------------------------------------------------
//
TBool CDiagRootSuite::IsSupported() const
    {
    return ETrue;
    }

// ---------------------------------------------------------------------------
// From MDiagPlugin
// CDiagRootSuite::GetTitleL
// ---------------------------------------------------------------------------
//
HBufC* CDiagRootSuite::GetTitleL() const
    {
    // This should never be displayed to the end user.
    _LIT( KRootSuitePluginTitle, "*Root Suite Title" );
    return KRootSuitePluginTitle().AllocL();
    }

// ---------------------------------------------------------------------------
// From MDiagPlugin
// CDiagRootSuite::GetDescriptionL
// ---------------------------------------------------------------------------
//
HBufC* CDiagRootSuite::GetDescriptionL() const
    {
    // This should never be displayed to the end user.
    _LIT( KRootSuitePluginDesc, "*Root Suite Desc" );
    return KRootSuitePluginDesc().AllocL();
    }

// ---------------------------------------------------------------------------
// From MDiagPlugin
// CDiagRootSuite::Uid
// ---------------------------------------------------------------------------
//
TUid CDiagRootSuite::Uid() const
    {
    return TUid::Uid( 0 );
    }



// ---------------------------------------------------------------------------
// From MDiagPlugin
// CDiagRootSuite::ParentUid
// ---------------------------------------------------------------------------
//
TUid CDiagRootSuite::ParentUid() const
    {
    return TUid::Uid( 0 );
    }


// ---------------------------------------------------------------------------
// From MDiagPlugin
// CDiagRootSuite::SetDtorIdKey
// ---------------------------------------------------------------------------
//
void CDiagRootSuite::SetDtorIdKey( TUid /* aDtorIdKey */ )
    {
    }


// ---------------------------------------------------------------------------
// From MDiagPlugin
// CDiagRootSuite::CustomOperationL
// ---------------------------------------------------------------------------
//
TAny* CDiagRootSuite::CustomOperationL( TUid /*aUid*/, TAny* /*aParam*/ )
    {
    User::Leave( KErrNotSupported );
    return NULL;
    }

// ---------------------------------------------------------------------------
// From MDiagPlugin
// CDiagRootSuite::GetCustomL
// ---------------------------------------------------------------------------
//
TAny* CDiagRootSuite::GetCustomL( TUid /* aUid */, TAny* /*aParam*/ )
    {
    User::Leave( KErrNotSupported );
    return NULL;
    }


// ---------------------------------------------------------------------------
// From MDiagTestPlugin
// CDiagRootSuite::TestSessionBeginL
// ---------------------------------------------------------------------------
//
void CDiagRootSuite::TestSessionBeginL( MDiagEngineCommon& /* aEngine */,
                                        TBool /* aSkipDependencyCheck */,
                                        TAny* /* aCustomParams */ )
    {
    // nothing to do
    }

// ---------------------------------------------------------------------------
// From MDiagTestPlugin
// CDiagRootSuite::CleanupL
// ---------------------------------------------------------------------------
//
void CDiagRootSuite::TestSessionEndL( MDiagEngineCommon& /* aEngine */,
                                      TBool /* aSkipDependencyCheck */,
                                      TAny* /* aCustomParams */ )
    {
    // nothing to do
    }



// ---------------------------------------------------------------------------
// From MDiagPlugin
// CDiagRootSuite::Order
// ---------------------------------------------------------------------------
//
TUint CDiagRootSuite::Order() const
    {
    return 0;
    }


// ---------------------------------------------------------------------------
// From MDiagSuitePlugin
// CDiagRootSuite::GetChildrenL
// ---------------------------------------------------------------------------
//
void CDiagRootSuite::GetChildrenL( RPointerArray<MDiagPlugin>& aChildren,
                                   TSortOrder aOrder ) const
    {
    if ( aOrder != ENotSorted && aOrder != ESortByPosition )
        User::Leave( KErrArgument );

    for ( TInt i = 0; i < iChildren.Count(); i++ )
        {
        aChildren.AppendL( iChildren[i] );
        }
    }

// ---------------------------------------------------------------------------
// From MDiagSuitePlugin
// CDiagRootSuite::AddChildL
// ---------------------------------------------------------------------------
//
void CDiagRootSuite::AddChildL( MDiagPlugin* aChild )
    {
    TLinearOrder<MDiagPlugin> order( *ComparePluginByOrder );
    User::LeaveIfError( iChildren.InsertInOrderAllowRepeats( aChild, order ) );
    }

// ---------------------------------------------------------------------------
// From MDiagSuitePlugin
// CDiagRootSuite::PrepareChildrenExecutionL
// ---------------------------------------------------------------------------
//
void CDiagRootSuite::PrepareChildrenExecutionL( TDiagSuiteExecParam* aParam,
                                                TBool /* aSkipDependencyCheck */,
                                                TBool /* aDependencyExecution */ )
    {
    aParam->Observer().ContinueExecutionL( *this );
    delete aParam;
    }

// ---------------------------------------------------------------------------
// From MDiagSuitePlugin
// CDiagRootSuite::FinalizeExecutionL
// ---------------------------------------------------------------------------
//
void CDiagRootSuite::FinalizeChildrenExecutionL( TDiagSuiteExecParam* aParam,
                                                 TBool /* aSkipDependencyCheck */,
                                                 TBool /* aDependencyExecution */ )
    {
    aParam->Observer().ContinueExecutionL( *this );
    delete aParam;
    }


// ---------------------------------------------------------------------------
// From MDiagSuitePlugin
// CDiagRootSuite::GetChildrenUidsL
// ---------------------------------------------------------------------------
//
void CDiagRootSuite::GetChildrenUidsL( RArray<TUid>& aUids,
                                        TSortOrder aOrder ) const
    {
    if ( aOrder != ENotSorted && aOrder != ESortByPosition )
        User::Leave( KErrArgument );

    for ( TInt i = 0; i < iChildren.Count(); i++ )
        {
        aUids.AppendL( iChildren[i]->Uid() );
        }
    }

// ---------------------------------------------------------------------------
// From MDiagSuitePlugin
// CDiagRootSuite::ExecutionStopL
// ---------------------------------------------------------------------------
//
void CDiagRootSuite::ExecutionStopL( TStopReason /* aReason */ )
    {
    }

// End of File

