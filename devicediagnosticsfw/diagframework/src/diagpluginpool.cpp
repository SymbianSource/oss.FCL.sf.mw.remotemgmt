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
* Description:  Class definition of CDiagPluginPool
*
*/


// CLASS DECLARATION
#include "diagpluginpoolimpl.h" // CDiagPluginPoolImpl
#include <DiagFrameworkDebug.h>
// SYSTEM INCLUDE FILES
#include <DiagPluginPool.h>
#include <DiagPlugin.h>             // MDiagPlugin

// USER INCLUDE FILES


// ===========================================================================
// MEMBER FUNCTIONS
// ===========================================================================


// ---------------------------------------------------------------------------
// CDiagPluginPool::NewL()
// ---------------------------------------------------------------------------
//
EXPORT_C CDiagPluginPool* CDiagPluginPool::NewL( 
            MDiagPluginPoolObserver& aObserver )
    {
    LOGSTRING("CDiagPluginPool* CDiagPluginPool::NewL");
    CDiagPluginPool* self = CDiagPluginPool::NewLC( aObserver );
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// CDiagPluginPool::NewLC()
// ---------------------------------------------------------------------------
//
EXPORT_C CDiagPluginPool* CDiagPluginPool::NewLC( 
            MDiagPluginPoolObserver& aObserver )
    {
    LOGSTRING("CDiagPluginPool* CDiagPluginPool::NewLC");
    CDiagPluginPool* self = new( ELeave )CDiagPluginPool();
    CleanupStack::PushL( self );
    self->ConstructL( aObserver );
    return self;
    }

// ---------------------------------------------------------------------------
// CDiagPluginPool::CDiagPluginPool
// ---------------------------------------------------------------------------
//
CDiagPluginPool::CDiagPluginPool()
    {
    }    

// ---------------------------------------------------------------------------
// CDiagPluginPool::LoadAsyncL
// ---------------------------------------------------------------------------
//
EXPORT_C void CDiagPluginPool::LoadAsyncL( TUid /*aUid*/ )
    {    
    iPluginPoolImpl->LoadAsyncL();
    }


// ---------------------------------------------------------------------------
// CDiagPluginPool::CreatePluginL
// ---------------------------------------------------------------------------
//
EXPORT_C MDiagPlugin* CDiagPluginPool::CreatePluginL( TUid aUid )
    {
    LOGSTRING("CDiagPluginPool::CreatePluginL");
    return CDiagPluginPoolImpl::CreatePluginL( aUid );
    }

// ---------------------------------------------------------------------------
// CDiagPluginPool::CancelLoadPluginsL
// ---------------------------------------------------------------------------
//
EXPORT_C void CDiagPluginPool::CancelLoadPluginsL()
    {
    iPluginPoolImpl->Cancel();
    }


// ---------------------------------------------------------------------------
// CDiagPluginPool::FindPlugin
// ---------------------------------------------------------------------------
//
EXPORT_C TInt CDiagPluginPool::FindPlugin( TUid aUid, 
            MDiagPlugin*& aPlugin ) const
    {    
    LOGSTRING("CDiagPluginPool::FindPlugin");
    return iPluginPoolImpl->FindPlugin( aUid, aPlugin );
    }

// ---------------------------------------------------------------------------
// CDiagPluginPool::FindPluginL
// ---------------------------------------------------------------------------
//
EXPORT_C MDiagPlugin& CDiagPluginPool::FindPluginL( TUid aUid ) const
    {    
    MDiagPlugin* plugin = NULL;
    User::LeaveIfError( FindPlugin( aUid, plugin ) );
    return *plugin;
    }

// ---------------------------------------------------------------------------
// CDiagPluginPool::FindPlugin
// ---------------------------------------------------------------------------
//
EXPORT_C TInt CDiagPluginPool::FindPlugin( const TDesC& aServiceName, 
            MDiagPlugin*& aPlugin ) const
    {
    return iPluginPoolImpl->FindPlugin( aServiceName, aPlugin );
    }

// ---------------------------------------------------------------------------
// CDiagPluginPool::FindPluginL
// ---------------------------------------------------------------------------
//
EXPORT_C MDiagPlugin& CDiagPluginPool::FindPluginL( const TDesC& aServiceName ) const
    {    
    MDiagPlugin* plugin = NULL;
    User::LeaveIfError( FindPlugin( aServiceName, plugin ) );
    return *plugin;
    }
    
// ---------------------------------------------------------------------------
// CDiagPluginPool::ConstructL()
// ---------------------------------------------------------------------------
//
void CDiagPluginPool::ConstructL( MDiagPluginPoolObserver& aObserver )
    {
    // Construct implementation class
    iPluginPoolImpl = CDiagPluginPoolImpl::NewL( aObserver );
    }    

// ---------------------------------------------------------------------------
// CDiagPluginPool::~CDiagPluginPool
// ---------------------------------------------------------------------------
//
EXPORT_C CDiagPluginPool::~CDiagPluginPool()
    {
    LOGSTRING("CDiagPluginPool::~CDiagPluginPool");
    delete iPluginPoolImpl;
    }    

// End of File

