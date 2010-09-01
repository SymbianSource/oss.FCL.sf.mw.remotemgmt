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
* Description:  Class definition of CDiagSuitePluginBase
*
*/


// CLASS DECLARATION
#include <DiagSuitePluginBase.h>            // CDiagSuitePluginBase

// SYSTEM INCLUDE FILES
#include <ecom.h>                           // REComSession
#include <coemain.h>                        // CCoeEnv::Static()
#include <StringLoader.h>                   // StringLoader
#include <DiagPluginConstructionParam.h>    // CDiagPluginConstructionParam
#include <DiagSuiteExecParam.h>             // TDiagSuiteExecParam
#include <DiagSuiteObserver.h>              // MDiagSuiteObserver
#include <DiagFrameworkDebug.h>             // LOGSTRING
#include <DiagPluginWaitingDialogWrapper.h> // CDiagPluginWaitingDialogWrapper

// USER INCLUDE FILES
#include "diagpluginbaseutils.h"            // DiagPluginBaseUtils
#include "diagpluginbase.pan"               // Panic Codes


// LOCAL TYPES
/**
* Internal private data class for CDiagSuitePluginBase
*/
class CDiagSuitePluginBase::TPrivateData
    {
public:
    /** 
    * C++ Constructor
    * All variables must be initialized here since this does not derive from
    * CBase.
    * Note that this class does not manage memory. Instead it is left to
    * CDiagSuitePluginBase class to allocate/free memory.
    */
    TPrivateData( CCoeEnv& aCoeEnv )
        :   iDtorIdKey( TUid::Null() ),
            iWaitingDialogWrapper( NULL ),
            iChildren(),
            iPluginResourceLoader( aCoeEnv )
        {
        }

public: // data
    /**
    * ECOM Destructor key.
    */
    TUid iDtorIdKey;

    /**
    * Wrapper class for displaying waiting dialogs.
    * Ownership: Shared. Normally, dialog will dismiss itself. However,
    * if plug-in is being deleted, it can be deleted by the plug-in as well.
    */
    CDiagPluginWaitingDialogWrapper* iWaitingDialogWrapper;

    /**
    * Array of children.
    */
    RPointerArray<MDiagPlugin> iChildren;

    /**
    * Resource loader for derived class resource.
    */
    RConeResourceLoader iPluginResourceLoader;
    };


// ======== LOCAL FUNCTIONS ========
// ---------------------------------------------------------------------------
// Compares two plug-ins by order number.
// ---------------------------------------------------------------------------
//
static TInt ComparePluginByOrder( const MDiagPlugin& aFirst,
                                   const MDiagPlugin& aSecond )
    {
    return aFirst.Order() - aSecond.Order();
    }


// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// CDiagSuitePluginBase::CDiagSuitePluginBase
// ---------------------------------------------------------------------------
//
EXPORT_C CDiagSuitePluginBase::CDiagSuitePluginBase( 
        CDiagPluginConstructionParam* aParam )
    :   CActive( EPriorityStandard ),
        iConstructionParam( aParam ),
        iCoeEnv( *CCoeEnv::Static() )
    {
    }

// ---------------------------------------------------------------------------
// CDiagSuitePluginBase::BaseConstructL
// ---------------------------------------------------------------------------
//
EXPORT_C void CDiagSuitePluginBase::BaseConstructL( const TDesC& aResourceFileName )
    {
    iData = new ( ELeave ) TPrivateData( CoeEnv() );

    DiagPluginBaseUtils::OpenResourceFileL(
        aResourceFileName, iData->iPluginResourceLoader, CoeEnv().FsSession() );
    }


// ---------------------------------------------------------------------------
// CDiagSuitePluginBase::~CDiagSuitePluginBase
// ---------------------------------------------------------------------------
//
EXPORT_C CDiagSuitePluginBase::~CDiagSuitePluginBase()
    {
    if ( iData )
        {
        DismissWaitingDialog();
        iData->iChildren.Close();

        if ( iData->iDtorIdKey != TUid::Null() )
            {
            REComSession::DestroyedImplementation( iData->iDtorIdKey );
            }

        iData->iPluginResourceLoader.Close();

        delete iData;
        iData = NULL;
        }

    delete iConstructionParam;
    iConstructionParam = NULL;
    }

// ---------------------------------------------------------------------------
// from MDiagPlugin
// CDiagSuitePluginBase::ServiceLogicalName
// ---------------------------------------------------------------------------
//
EXPORT_C const TDesC& CDiagSuitePluginBase::ServiceLogicalName() const
    {
    return iConstructionParam->ServiceProvided();
    }

// ---------------------------------------------------------------------------
// From MDiagPlugin
// CDiagSuitePluginBase::GetLogicalDependenciesL
// ---------------------------------------------------------------------------
//
EXPORT_C void CDiagSuitePluginBase::GetLogicalDependenciesL( 
        CPtrCArray& aArray ) const
    {
    aArray.CopyL( iConstructionParam->ServicesRequired() );
    }


// ---------------------------------------------------------------------------
// From MDiagPlugin
// CDiagSuitePluginBase::Type
// ---------------------------------------------------------------------------
//
EXPORT_C MDiagPlugin::TPluginType CDiagSuitePluginBase::Type() const
    {
    return ETypeSuitePlugin;
    }

// ---------------------------------------------------------------------------
// From MDiagPlugin
// CDiagSuitePluginBase::CreateIconL
// ---------------------------------------------------------------------------
//
EXPORT_C CGulIcon* CDiagSuitePluginBase::CreateIconL() const
    {
        // Currently, GetTitleL() is not supported.
        User::Leave( KErrNotSupported );
        return NULL;
    }


// ---------------------------------------------------------------------------
// From MDiagTestPlugin
// CDiagSuitePluginBase::IsSupported
// ---------------------------------------------------------------------------
//
EXPORT_C TBool CDiagSuitePluginBase::IsSupported() const
    {
    return ETrue;
    }

// ---------------------------------------------------------------------------
// From MDiagPlugin
// CDiagSuitePluginBase::ParentUid
// ---------------------------------------------------------------------------
//
EXPORT_C TUid CDiagSuitePluginBase::ParentUid() const
    {
    return iConstructionParam->ParentUid();
    }


// ---------------------------------------------------------------------------
// From MDiagPlugin
// CDiagSuitePluginBase::SetDtorIdKey
// ---------------------------------------------------------------------------
//
EXPORT_C void CDiagSuitePluginBase::SetDtorIdKey( TUid aDtorIdKey )
    {
    LOGSTRING2( "CDiagTestPluginBase::SetDtorIdKey: New=0x%x",
        aDtorIdKey.iUid )

    __ASSERT_ALWAYS( iData, Panic( EDiagPluginBasePanicInvalidState ) );

    iData->iDtorIdKey = aDtorIdKey;
    }

// ---------------------------------------------------------------------------
// From MDiagSuitePlugin
// CDiagSuitePluginBase::GetTitleL
// ---------------------------------------------------------------------------
//
EXPORT_C HBufC* CDiagSuitePluginBase::GetTitleL() const
    {
    // Currently, GetTitleL() is not supported.
    User::Leave( KErrNotSupported );
    return NULL;
    }

// ---------------------------------------------------------------------------
// From MDiagSuitePlugin
// CDiagSuitePluginBase::GetDescriptionL
// ---------------------------------------------------------------------------
//
EXPORT_C HBufC* CDiagSuitePluginBase::GetDescriptionL() const
    {
    // Currently, GetDescriptionL() is not supported.
    User::Leave( KErrNotSupported );
    return NULL;
    }

// ---------------------------------------------------------------------------
// From MDiagPlugin
// CDiagSuitePluginBase::CustomOperationL
// ---------------------------------------------------------------------------
//
EXPORT_C TAny* CDiagSuitePluginBase::CustomOperationL( TUid /*aUid*/, 
                                                       TAny* /*aParam*/ )
    {
    LOGSTRING( "CDiagTestPluginBase::CustomOperationL: KErrNotSupported" )
    User::Leave( KErrNotSupported );
    return NULL;
    }

// ---------------------------------------------------------------------------
// From MDiagPlugin
// CDiagSuitePluginBase::GetCustomL
// ---------------------------------------------------------------------------
//
EXPORT_C TAny* CDiagSuitePluginBase::GetCustomL( TUid /* aUid */,
                                                 TAny* /*aParam*/ )
    {
    LOGSTRING( "CDiagTestPluginBase::CustomOperationL: KErrNotSupported" )
    User::Leave( KErrNotSupported );
    return NULL;
    }

// ---------------------------------------------------------------------------
// From MDiagPlugin
// CDiagSuitePluginBase::Order
// ---------------------------------------------------------------------------
//
EXPORT_C TUint CDiagSuitePluginBase::Order() const
    {
    return iConstructionParam->Order();
    }


// ---------------------------------------------------------------------------
// From MDiagSuitePlugin
// CDiagSuitePluginBase::GetChildrenL
// ---------------------------------------------------------------------------
//
EXPORT_C void CDiagSuitePluginBase::GetChildrenL( RPointerArray<MDiagPlugin>& aChildren,
                                                  TSortOrder aOrder ) const
    {
    __ASSERT_ALWAYS( iData, Panic( EDiagPluginBasePanicInvalidState ) );
    __ASSERT_ALWAYS( (aOrder == ENotSorted || aOrder == ESortByPosition),
                     Panic( EDiagPluginBasePanicBadArgument ) );
                      
    TInt count = iData->iChildren.Count();
    for ( TInt i = 0; i < count; i++ )
        {
        aChildren.AppendL( iData->iChildren[i] );
        }
    }

// ---------------------------------------------------------------------------
// From MDiagSuitePlugin
// CDiagSuitePluginBase::AddChildL
// ---------------------------------------------------------------------------
//
EXPORT_C void CDiagSuitePluginBase::AddChildL( MDiagPlugin* aChild )
    {
    __ASSERT_ALWAYS( iData, Panic( EDiagPluginBasePanicInvalidState ) );

    TLinearOrder<MDiagPlugin> order( *ComparePluginByOrder );
    User::LeaveIfError( iData->iChildren.InsertInOrderAllowRepeats( aChild, order ) );
    }

// ---------------------------------------------------------------------------
// From MDiagTestPlugin
// CDiagSuitePluginBase::TestSessionBeginL
// ---------------------------------------------------------------------------
//
EXPORT_C void CDiagSuitePluginBase::TestSessionBeginL( 
        MDiagEngineCommon& /* aEngine */,
        TBool /* aSkipDependencyCheck */,
        TAny* /* aCustomParams */ )
    {
    }

// ---------------------------------------------------------------------------
// From MDiagTestPlugin
// CDiagSuitePluginBase::TestSessionEndL
// ---------------------------------------------------------------------------
//
EXPORT_C void CDiagSuitePluginBase::TestSessionEndL( 
        MDiagEngineCommon& /* aEngine */,
        TBool /* aSkipDependencyCheck */,
        TAny* /* aCustomParams */ )
    {
    }

// ---------------------------------------------------------------------------
// From MDiagSuitePlugin
// CDiagSuitePluginBase::PrepareExecutionL
// ---------------------------------------------------------------------------
//
EXPORT_C void CDiagSuitePluginBase::PrepareChildrenExecutionL( 
        TDiagSuiteExecParam* aParam,
        TBool /* aSkipDependencyCheck */,
        TBool /* aDependencyExecution */)
    {
    CleanupStack::PushL( aParam );
    aParam->Observer().ContinueExecutionL( *this );
    CleanupStack::PopAndDestroy ( aParam );
    }

// ---------------------------------------------------------------------------
// From MDiagSuitePlugin
// CDiagSuitePluginBase::FinalizeExecutionL
// ---------------------------------------------------------------------------
//
EXPORT_C void CDiagSuitePluginBase::FinalizeChildrenExecutionL( 
        TDiagSuiteExecParam* aParam,
        TBool /* aSkipDependencyCheck */,
        TBool /* aDependencyExecution */ )
    {
    CleanupStack::PushL( aParam );
    aParam->Observer().ContinueExecutionL( *this );
    CleanupStack::PopAndDestroy ( aParam );
    }


// ---------------------------------------------------------------------------
// From MDiagSuitePlugin
// CDiagSuitePluginBase::GetChildrenUidsL
// ---------------------------------------------------------------------------
//
EXPORT_C void CDiagSuitePluginBase::GetChildrenUidsL( RArray<TUid>& aUids,
        TSortOrder aOrder ) const
    {
    __ASSERT_ALWAYS( iData, Panic( EDiagPluginBasePanicInvalidState ) );
    __ASSERT_ALWAYS( (aOrder == ENotSorted || aOrder == ESortByPosition),
                     Panic( EDiagPluginBasePanicBadArgument ) );

    TInt count = iData->iChildren.Count();
    for ( TInt i = 0; i < count; i++ )
        {
        aUids.AppendL( iData->iChildren[i]->Uid() );
        }
    }

// ---------------------------------------------------------------------------
// From MDiagSuitePlugin
// CDiagSuitePluginBase::ExecutionStopL
// ---------------------------------------------------------------------------
//
EXPORT_C void CDiagSuitePluginBase::ExecutionStopL( TStopReason /* aReason */ )
    {
        // Currently, GetTitleL() is not supported.
        User::Leave( KErrNotSupported );
    }


// ---------------------------------------------------------------------------
// CDiagSuitePluginBase::RunWaitingDialogLD
// ---------------------------------------------------------------------------
EXPORT_C TBool CDiagSuitePluginBase::RunWaitingDialogL( CAknDialog* aDialog, 
        TInt& aDialogResponse )
    {
    __ASSERT_ALWAYS( iData, Panic( EDiagPluginBasePanicInvalidState ) );

    // this function can display only one dialog at a time.
    __ASSERT_ALWAYS( iData->iWaitingDialogWrapper == NULL,
                     Panic( EDiagPluginBasePanicDialogAlreadyUp ) );

    iData->iWaitingDialogWrapper = CDiagPluginWaitingDialogWrapper::NewL( aDialog );

    TBool isUserResonse = iData->iWaitingDialogWrapper->RunLD( aDialogResponse );

    if ( isUserResonse )
        {
        // local variable can be accessed only if it was returned due to
        // user response.
        iData->iWaitingDialogWrapper = NULL;
        }

    return isUserResonse;
    }

// ---------------------------------------------------------------------------
// CDiagSuitePluginBase::DismissWaitingDialog
// ---------------------------------------------------------------------------
EXPORT_C void CDiagSuitePluginBase::DismissWaitingDialog()
    {
    __ASSERT_ALWAYS( iData, Panic( EDiagPluginBasePanicInvalidState ) );

    if ( iData->iWaitingDialogWrapper )
        {
        LOGSTRING( "CDiagTestPluginBase::DismissWaitingDialog() Dialog dismissed." )
        delete iData->iWaitingDialogWrapper;
        iData->iWaitingDialogWrapper = NULL;
        }
    }

// ---------------------------------------------------------------------------
// CDiagSuitePluginBase::CoeEnv
// ---------------------------------------------------------------------------
//
EXPORT_C CCoeEnv& CDiagSuitePluginBase::CoeEnv() 
    {
    return iCoeEnv;
    }

// End of File

