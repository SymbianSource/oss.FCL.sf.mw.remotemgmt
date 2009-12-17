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
* Description:  Implementation of Diagnostics Plug-in Pool
*
*/


// CLASS DECLARATION
#include "diagpluginpoolimpl.h"

// SYSTEM INCLUDE FILES
#include <ecom.h>
#include <DiagPlugin.h>
#include <DiagPluginPoolObserver.h>
#include <DiagPluginConstructionParam.h>
#include <DiagPluginPoolObserver.h>
#include <DiagFrameworkDebug.h>
#include <mmf/common/mmfcontrollerpluginresolver.h> // CleanupResetAndDestroyPushL

// USER INCLUDE FILES
#include "diagpluginloaderdependencyparser.h"
#include "diagrootsuite.h"

using namespace DiagPluginPool;
using namespace DiagFwInternal;

_LIT( KPanicCode, "PluginPool" );

// ===========================================================================
// PUBLIC MEMBER FUNCTIONS
// ===========================================================================    

// ---------------------------------------------------------------------------
// NewL
// ---------------------------------------------------------------------------
//
CDiagPluginPoolImpl* CDiagPluginPoolImpl::NewL( MDiagPluginPoolObserver& aObserver )
    {
    LOGSTRING("");
    CDiagPluginPoolImpl* self = CDiagPluginPoolImpl::NewLC( aObserver );
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// NewLC
// ---------------------------------------------------------------------------
//
CDiagPluginPoolImpl* CDiagPluginPoolImpl::NewLC( MDiagPluginPoolObserver& aObserver )
    {
    CDiagPluginPoolImpl* self = new( ELeave )CDiagPluginPoolImpl( aObserver );
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }        

// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
CDiagPluginPoolImpl::~CDiagPluginPoolImpl()
    {
    LOGSTRING("");
    Cancel();
    iPlugins.ResetAndDestroy();
    iPlugins.Close();
    REComSession::FinalClose();
    }
    
// ---------------------------------------------------------------------------
// LoadAsyncL
// ---------------------------------------------------------------------------
//
void CDiagPluginPoolImpl::LoadAsyncL()
    {
    LOGSTRING( "CDiagPluginPoolImpl::LoadAsyncL" )

    ASSERT ( !iPluginsLoaded );
    ASSERT ( !iPluginsLoading );

    iPluginsLoading = ETrue;       
       
    iConstructionParamArray = GeneratePluginListL();
    
    iTotalPluginsFound = iConstructionParamArray->Count();

    // No plug-ins found
    if ( ! iTotalPluginsFound )
        {
        DestroyConstructionParams();
        User::Leave( KErrNotFound );
        }                
            
    // Create root suite to define the hierarchy
    MDiagSuitePlugin* rootSuite = CDiagRootSuite::NewL();    
    CleanupStack::PushL( rootSuite );
    iPlugins.AppendL( rootSuite ); // ownership transfered        
    CleanupStack::Pop( rootSuite );

    SetNextIteration();
    }
    
// ---------------------------------------------------------------------------
// CreatePluginL
// ---------------------------------------------------------------------------
//
MDiagPlugin* CDiagPluginPoolImpl::CreatePluginL( const TUid aUid )
    {
    TInt errorCode = KErrNone;

    LOGSTRING2( "CDiagPluginPoolImpl::CreatePluginL %d\n", aUid.iUid )

    // Create construction parameters list
    RConstructionParamArray* constructionParamsArray = GeneratePluginListL();
    CleanupStack::PushL( constructionParamsArray );
    CleanupResetAndDestroyPushL( *constructionParamsArray );
    
    MDiagPlugin* plugin = NULL;
    
    // Find plug-in with matching UID        
    for ( TInt i=0; i<constructionParamsArray->Count(); i++ )
        {
        if ( ( *constructionParamsArray )[i]->Uid() == aUid )
            {            
            plugin = CreatePluginFromConstructionParamsL( ( *constructionParamsArray )[i] );
            
            // Construction params owned by plug-in so remove from list
            constructionParamsArray->Remove(i);
            break;  //lint !e960 ok to break from for loop     
            }
        }
        
    if ( ! plugin )
        {                
        errorCode = KErrNotFound;                
        }
       
    CleanupStack::PopAndDestroy();        // *implInfoArray
    CleanupStack::PopAndDestroy( constructionParamsArray );
    
    User::LeaveIfError( errorCode );
        
    return plugin;    
    }

// ---------------------------------------------------------------------------
// FindPlugin
// ---------------------------------------------------------------------------
//
TInt CDiagPluginPoolImpl::FindPlugin( TUid aUid, MDiagPlugin*& aPlugin ) const
    {
    LOGSTRING2( "CDiagPluginPoolImpl::FindPlugin 0x%x\n", aUid.iUid )

    aPlugin = NULL;

    // Find plug-in with matching UID in pool
    for ( TInt i=0; i<iPlugins.Count(); i++ )
        {           
        if ( iPlugins[i]->Uid() == aUid )           
            {
            aPlugin = iPlugins[i];            
            return KErrNone;
            }
        }
        
    return KErrNotFound;        
    }

// ---------------------------------------------------------------------------
// FindPlugin
// ---------------------------------------------------------------------------
//
TInt CDiagPluginPoolImpl::FindPlugin( const TDesC& aServiceName,
                                        MDiagPlugin *& aPlugin ) const
    {    
    LOGSTRING2( "CDiagPluginPoolImpl::FindPlugin \"%S\"\n", &aServiceName )

    aPlugin = NULL;
    
    // Find plug-in with matching service name in pool
    for ( TInt i=0; i<iPlugins.Count(); i++ )
        {           
        if ( iPlugins[i]->ServiceLogicalName() == aServiceName )           
            {
            aPlugin = iPlugins[i];            
            return KErrNone;
            }
        }
        
    return KErrNotFound;
    }


// ===========================================================================
// PRIVATE CONSTRUCTORS
// ===========================================================================

// ---------------------------------------------------------------------------
// CDiagPluginPool
// ---------------------------------------------------------------------------
//
CDiagPluginPoolImpl::CDiagPluginPoolImpl( MDiagPluginPoolObserver& aObserver )
        // Priority is idle to allow plug-ins to load in background regardless of
        // what UI priority is set to.
    :   CActive( EPriorityIdle ),
        iObserver( aObserver ),
        iErrorCode( KErrNone )
    {
    }


// ---------------------------------------------------------------------------
// ConstructL
// ---------------------------------------------------------------------------
//
void CDiagPluginPoolImpl::ConstructL()
    {
    CActiveScheduler::Add( this );
    }        
    

// ===========================================================================
// PRIVATE STATIC FUNCTIONS
// ===========================================================================

// ---------------------------------------------------------------------------
// ObtainImplementationInfoL
// ---------------------------------------------------------------------------
//
CDiagPluginPoolImpl::RConstructionParamArray* CDiagPluginPoolImpl::GeneratePluginListL()
    {     

    // Get list of plug-in implementation implementationInfo
    RImplInfoPtrArray* implInfoArray = new( ELeave )RImplInfoPtrArray;
    
    REComSession::ListImplementationsL( KDiagPluginInterfaceUid, *implInfoArray );
        
    // Leave if no matching plug-ins found        
    if ( ! implInfoArray->Count() )
        {
        delete implInfoArray;
        User::Leave( KErrNotFound );
        }
        
    CleanupStack::PushL( implInfoArray );
    CleanupResetAndDestroyPushL( *implInfoArray );         
        
    RConstructionParamArray* constructionParamsArray = new(ELeave)RConstructionParamArray;
    CleanupStack::PushL( constructionParamsArray );
    CleanupResetAndDestroyPushL( *constructionParamsArray );
    CDependencyParser* parser = CDependencyParser::NewLC();
    
    // Iterate through implementation information array and create construction data for each
    // plug-in.  Insert into array by order number.
    for ( TInt pluginIndex=0; pluginIndex < implInfoArray->Count(); pluginIndex++ )
        {
        CImplementationInformation* pluginImplementationInfo = ( *implInfoArray )[pluginIndex];

        if ( !pluginImplementationInfo->RomBased() )
            {
#ifdef _DEBUG
            LOGSTRING2( "CDiagPluginPoolImpl::GeneratePluginListL() "
                L" Loading non-rom plug-in 0x%08x", 
                pluginImplementationInfo->ImplementationUid() );
#else   // #ifdef _DEBUG
            // In non _DEBUG build, do not load plug-in that are not in ROM
            // for security reasons. Continue to next plug-in.
            //continue;
#endif  // #else _DEBUG
            }

        // Obtain depenency and other information from opaque data field
        parser->ParseL( pluginImplementationInfo->OpaqueData(), 
                        pluginImplementationInfo->DataType() );
        
        CDiagPluginConstructionParam* constructionParams = 
            GenerateConstructionParamsLC( pluginImplementationInfo );
                
        TInt orderNumber = constructionParams->Order();        
        
        // Find correct index to insert plug-in
        TInt insertIndex;
        for ( insertIndex=0; insertIndex<constructionParamsArray->Count(); insertIndex++ )
            {
            if ( orderNumber < (*constructionParamsArray)[insertIndex]->Order() )
                {                
                break;    //lint !e960 break OK here.
                }
            } 
                        
        // Insert in order by order number
        constructionParamsArray->InsertL(constructionParams,insertIndex);            
        CleanupStack::Pop(constructionParams);
        }
                    
    // Clean up                    
    CleanupStack::PopAndDestroy(parser);              
    CleanupStack::Pop(); // constructionParamsArray
    CleanupStack::Pop(constructionParamsArray);            
    CleanupStack::PopAndDestroy(); // implInfoArray
    CleanupStack::PopAndDestroy(implInfoArray);                  
        
    return constructionParamsArray;
    }
    
// ---------------------------------------------------------------------------
// GenerateConstructionParamsL
// ---------------------------------------------------------------------------
//     
CDiagPluginConstructionParam* CDiagPluginPoolImpl::GenerateConstructionParamsLC(
                                    const CImplementationInformation* aInfo )
    {
    CDependencyParser* parser = CDependencyParser::NewLC();
    
    // Obtain depenency and other information from opaque data field
    parser->ParseL( aInfo->OpaqueData(), aInfo->DataType() );
    
    CDiagPluginConstructionParam* params = CDiagPluginConstructionParam::NewL
                                    (
                                    parser->GetServiceProvided(),
                                    parser->GetServicesRequired(),
                                    parser->GetOrderNumber(),
                                    aInfo->ImplementationUid(),
                                    parser->GetParentUid()
                                    );
        
    CleanupStack::PopAndDestroy( parser );
    CleanupStack::PushL( params );
    
    return params;
    }

// ---------------------------------------------------------------------------
// CreatePluginFromImplementationInfoL
// ---------------------------------------------------------------------------
//    
MDiagPlugin* CDiagPluginPoolImpl::CreatePluginFromConstructionParamsL(
                                    const CDiagPluginConstructionParam* aParams )
    {    
    MDiagPlugin* newPlugin = NULL;
    TUid destructorIdKey;

    // Call ECOM to load next plug-in
    TAny* object = REComSession::CreateImplementationL( aParams->Uid(), 
                                                        destructorIdKey, 
                                                        ( TAny* )aParams );
    if (!object)
    User::Leave(KErrNotFound);
    
    newPlugin = static_cast<MDiagPlugin*>( object );
    newPlugin->SetDtorIdKey( destructorIdKey );
    return newPlugin;
    }           
    
// ===========================================================================
// PRIVATE MEMBER FUNCTIONS
// ===========================================================================

// ---------------------------------------------------------------------------
// AddNewPluginToSuiteL
// ---------------------------------------------------------------------------
//
void CDiagPluginPoolImpl::AddNewPluginToSuiteL(MDiagPlugin* aPlugin)
    {
    // Search the plug-in list for a matching suite plug-in and add this newly
    // loaded plug-in to that corresponding suite.  If a suite is not found,
    // this plug-in will have no parent unless the parent suite is loaded
    // at some later point.
    for ( TInt pluginIndex = 0; pluginIndex < iPlugins.Count(); pluginIndex++ )
        {
        
        // Find matching parent suite
        if ( aPlugin->ParentUid() == iPlugins[pluginIndex]->Uid() )
            {               
            if ( iPlugins[pluginIndex]->Type() == MDiagPlugin::ETypeSuitePlugin )
                {
                // If parent is a suite, add child         
                MDiagSuitePlugin* parent = static_cast<MDiagSuitePlugin*>( iPlugins[pluginIndex] );
                
                parent->AddChildL( aPlugin );
                return;
                }
            else
                {
                // if parent is not a suite, leave
                User::Leave( KErrCorrupt );
                }
            }            
        }   
    }
    
// ---------------------------------------------------------------------------
// AddPluginsToNewSuiteL
// ---------------------------------------------------------------------------
//    
void CDiagPluginPoolImpl::AddPluginsToNewSuiteL(MDiagSuitePlugin* aPluginSuite)
    {        
    // Search the plug-in list for plug-ins with matching parent suite.
    for ( TInt pluginIndex = 0; pluginIndex < iPlugins.Count(); pluginIndex++ )
        {        
        // Add any "orphan" plug-ins
        if ( aPluginSuite->Uid() == iPlugins[pluginIndex]->ParentUid() )
            {                                      
            aPluginSuite->AddChildL( iPlugins[pluginIndex] );                      
            }            
        }   
    }    

// ---------------------------------------------------------------------------
// SetNextIteration
// ---------------------------------------------------------------------------
//
void CDiagPluginPoolImpl::SetNextIteration()
    {
    TRequestStatus* status = &iStatus;
    
    // Complete own request
    User::RequestComplete( status, KErrNone );    
    SetActive();
    }
    
// ---------------------------------------------------------------------------
// LoadNextPlugin
// ---------------------------------------------------------------------------
//    
void CDiagPluginPoolImpl::LoadNextPluginL()
    {
    // Get parameters to generate plug-in
    CDiagPluginConstructionParam* constructionParams = ( *iConstructionParamArray )[ 0 ];
    iCurrentPluginIndex++;
    MDiagPlugin* newPlugin = NULL;
    
    // Uid to report to application, defaults to NULL.
    TUid reportedUid = TUid::Null();

    // Create plug-in from ECOM structure
    // Any parsing or ECOM error will be caught here
    TRAPD( error, newPlugin = CreatePluginFromConstructionParamsL( constructionParams ) )
        
    // Construction params are now owned by plug-in so remove from array
    iConstructionParamArray->Remove( 0 );        

    LOGSTRING4( "CDiagPluginPoolImpl::LoadNextPluginL Plugin( %d/%d ): Error=%d\n",
                iCurrentPluginIndex, iTotalPluginsFound, error )
                                                          
    // Check for error in plug-in creation                
    if ( error != KErrNone )
        {        
        if ( iErrorCode == KErrNone )
            {
            // Sets error code to whatever was the first error encountered        
            iErrorCode = error;
            }                     
        }
    else
        {
        
        CleanupDeletePushL( newPlugin );
        
        // Add to plug-in pool
        reportedUid = AddPluginToPoolLD( newPlugin );
        newPlugin = NULL;
        }

    // Load is not complete
    if ( iConstructionParamArray->Count() )
        {        
        
        // Set next plug-in to load in next active scheduler cycle
        SetNextIteration();
        
        // Report progress to client
        iObserver.LoadProgressL( iCurrentPluginIndex, iTotalPluginsFound, reportedUid );
        }
        
    // Load is complete        
    else
        {
        // Report load completed to client
        iPluginsLoading = EFalse;
        iPluginsLoaded = ETrue;

        iObserver.LoadCompletedL( iErrorCode );

        // Don't need implementation implementationInfo list anymore
        DestroyConstructionParams();
        }
    }

// ---------------------------------------------------------------------------
// DestroyConstructionParams
// ---------------------------------------------------------------------------
//    
TUid CDiagPluginPoolImpl::AddPluginToPoolLD( MDiagPlugin* aPlugin )
    {
    
    TUid reportedUid = TUid::Null();
    
    // Add plug-in to list
    if ( aPlugin->IsSupported() )
        {
        iPlugins.AppendL( aPlugin );
        
        // Indicate to client that a plug-in was loaded with this UID
        reportedUid = aPlugin->Uid();
        
        // Update suite tree.  If this is a plug-in, try to find parent
        // suite and add.  If this is a suite, try to find child plug-ins
        // and add them.            
        AddNewPluginToSuiteL(aPlugin);
        
        if ( aPlugin->Type() == MDiagPlugin::ETypeSuitePlugin )
            {
            MDiagSuitePlugin* pluginSuite = 
                    static_cast<MDiagSuitePlugin*>(aPlugin);
            AddPluginsToNewSuiteL( pluginSuite );
            }            
        
        CleanupStack::Pop();    // aPlugin        
        }
    else
        {
        LOGSTRING2( " Plugin UID 0x%x NOT SUPPORTED. Skipped", aPlugin->Uid().iUid )
        CleanupStack::PopAndDestroy(); // aPlugin        
        }
        
    return reportedUid;        
    }
    
// ---------------------------------------------------------------------------
// DestroyConstructionParams
// ---------------------------------------------------------------------------
// 
void CDiagPluginPoolImpl::DestroyConstructionParams( )
    {
    if ( iConstructionParamArray )
        {
        iConstructionParamArray->ResetAndDestroy();
        delete iConstructionParamArray;
        iConstructionParamArray = NULL;
        }
    }
    
// ---------------------------------------------------------------------------
// ResetAndNotify
// ---------------------------------------------------------------------------
//
TInt CDiagPluginPoolImpl::ResetAndNotify(TInt aErrorCode)
    {
    // Reset load data
    iCurrentPluginIndex = 0;
    iPlugins.ResetAndDestroy();

    iPluginsLoading = EFalse;

    // Reset and delete implementation implementationInfo array
    DestroyConstructionParams();

    TRAPD( error, iObserver.LoadCompletedL( aErrorCode ) )
    
    return error;
    }

    

// ===========================================================================
// From CActive
// ===========================================================================

// ---------------------------------------------------------------------------
// RunL
// ---------------------------------------------------------------------------
//
void CDiagPluginPoolImpl::RunL()
    {
    // Load a plug-in in active scheduler cycle
    LoadNextPluginL();
    }

// ---------------------------------------------------------------------------
// DoCancel
// ---------------------------------------------------------------------------
//
void CDiagPluginPoolImpl::DoCancel()
    {   
    LOGSTRING( "CDiagPluginPoolImpl::DoCancel" )
    
    TInt error = ResetAndNotify(KErrCancel);

    // Reset load data
    if ( error )
        {
        User::Panic( KPanicCode(), error );
        }
    }

// ---------------------------------------------------------------------------
// RunError
// ---------------------------------------------------------------------------
//
TInt CDiagPluginPoolImpl::RunError( TInt aError )
    {
    // An unhandled exception occurs while running.  Destroy all currently
    // loaded plug-ins.

    LOGSTRING2( "CDiagPluginPoolImpl::RunError %d", aError )

    return ResetAndNotify( aError );
    }
    
// End of File

