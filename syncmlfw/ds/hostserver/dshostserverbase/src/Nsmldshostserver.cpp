/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  DS Host Server
*
*/


// INCLUDE FILES
#include <s32std.h>
#include <data_caging_path_literals.hrh>
#include <f32file.h>
#include <e32math.h>

#include <nsmldebug.h>
#include "nsmldshostconstants.h"
#include "Nsmldshostserver.h"
#include "Nsmldshostsession.h"

// CONSTANTS
const TInt KNSmlMaxSessionCount = 200;

// ======================================= MEMBER FUNCTIONS =======================================

// ------------------------------------------------------------------------------------------------
// CNSmlDSHostServer::~CNSmlDSHostServer
// ------------------------------------------------------------------------------------------------
EXPORT_C CNSmlDSHostServer::~CNSmlDSHostServer()
	{
	_DBG_FILE("CNSmlDSHostServer::~CNSmlDSHostServer(): begin");
	_DBG_FILE("CNSmlDSHostServer::~CNSmlDSHostServer(): end");
	}
	
// ------------------------------------------------------------------------------------------------
// CNSmlDSHostServer::InitHostServer
// ------------------------------------------------------------------------------------------------
EXPORT_C TInt CNSmlDSHostServer::InitHostServer( TNSmlHostServers aServer, TThreadFunction aFunction )
    {
	const TDesC& serverName = ServerName( aServer );
	if ( serverName == KNullDesC )
		{
		return KErrArgument;
		}
    	
    // check server not already started
    TFindServer findHostServer( serverName );
    TFullName name;
    if ( findHostServer.Next( name ) == KErrNone )
	    { // found server already 
	    return KErrGeneral;
	    }
        
	TInt error( KErrNone );
    error = ThreadFunction( (TAny*) aFunction, aServer );
    
	return error;
    }
    
// ------------------------------------------------------------------------------------------------
// CNSmlDSHostServer::ServerName
// ------------------------------------------------------------------------------------------------
const TDesC& CNSmlDSHostServer::ServerName( TNSmlHostServers aServer )
	{
	if ( aServer > ENSmlHostServerEnd )
    	{
    	aServer = ENSmlHostServerEnd;
    	}
    	
    const TDesC* KNSmlHostServerNames[ENSmlHostServerEnd + 1] = 
		{
		&KNSmlDSHostServer1Name,
		&KNSmlDSHostServer2Name,
		&KNullDesC
		};
		
    return *KNSmlHostServerNames[aServer];
	}

// ------------------------------------------------------------------------------------------------
// CNSmlDSHostServer::ServerModuleName
// ------------------------------------------------------------------------------------------------	
const TDesC& CNSmlDSHostServer::ServerModuleName( TNSmlHostServers aServer )
	{
	if ( aServer > ENSmlHostServerEnd )
    	{
    	aServer = ENSmlHostServerEnd;
    	}
    	
    const TDesC* KNSmlHostServerModuleNames[ENSmlHostServerEnd + 1] =
		{
		&KNSmlDSHostServer1Exe,
		&KNSmlDSHostServer2Exe,
		&KNullDesC
		};
	
	return *KNSmlHostServerModuleNames[aServer];
	}
    
// ------------------------------------------------------------------------------------------------
// CNSmlDSHostServer::ThreadFunction
// ------------------------------------------------------------------------------------------------ 
TInt CNSmlDSHostServer::ThreadFunction( TAny* aStarted, TNSmlHostServers aServer )
	{
	__UHEAP_MARK;
	
	TThreadFunction function = reinterpret_cast<TThreadFunction>( aStarted );

    CTrapCleanup* cleanup = CTrapCleanup::New();
    __ASSERT_ALWAYS( cleanup, HandleLeaveOnStartup( &aServer ) );
    TRAPD(err, HostServerStartUpL( function, aServer ) );
    delete cleanup;
    __UHEAP_MARKEND;
    
	return err;
	}
	
// ------------------------------------------------------------------------------------------------
// CNSmlDSHostServer::HostServerStartUpL
// ------------------------------------------------------------------------------------------------    
void CNSmlDSHostServer::HostServerStartUpL( TThreadFunction aFunction, TNSmlHostServers aServer )
	{
	CleanupStack::PushL( TCleanupItem ( HandleLeaveOnStartup, &aServer ) );
	
    CActiveScheduler* scheduler = new ( ELeave ) CActiveScheduler();
	CActiveScheduler::Install( scheduler );
	
	__UHEAP_MARK;
	aFunction( NULL );
	__UHEAP_MARKEND;
	
	CActiveScheduler::Install( NULL );
    delete scheduler;
	
	CleanupStack::Pop();
	}

// ------------------------------------------------------------------------------------------------
// CNSmlDSHostServer::ServerFromThread
// ------------------------------------------------------------------------------------------------
TNSmlHostServers CNSmlDSHostServer::ServerFromThread( const RThread &aThread )
	{
	TInt tmp;
	TName thname = aThread.Name();
	TPtrC serverid = thname.Left( 4 );
	TLex lex( serverid );
	lex.Val(tmp);
	return static_cast<TNSmlHostServers>( tmp );
	}
// ------------------------------------------------------------------------------------------------
// CNSmlDSHostServer::NewSessionL
// ------------------------------------------------------------------------------------------------
CSession2* CNSmlDSHostServer::NewSessionL( const TVersion& aVersion, const RMessage2& aMessage ) const
	{
	_DBG_FILE("CNSmlDSHostServer::NewSessionL(): begin");
	if( iSessionCount >= KNSmlMaxSessionCount )
		{
		User::Leave( KErrCouldNotConnect );
		}

	// check we're the right version
	TVersion DSHostServerVersion( KNSmlDSHostServerVersionMajor, 
		KNSmlDSHostServerVersionMinor, KNSmlDSHostServerVersionBuild );
		
	if( !User::QueryVersionSupported( DSHostServerVersion, aVersion ) )
		{
		_DBG_FILE("CNSmlDSHostServer::NewSessionL(): wrong version offered -> BAIL OUT!");
		User::Leave(KErrNotSupported);
		}
	TSecureId id = aMessage.SecureId();
	_LIT_SECURITY_POLICY_S0( mySidPolicy, KNSmlSOSServerSecureID );

	if ( !mySidPolicy().CheckPolicy( aMessage ) )
		{
		User::Leave( KErrPermissionDenied );
		}

	_DBG_FILE("CNSmlDSHostServer::NewSessionL(): end");
	return CNSmlDSHostSession::NewL( *const_cast<CNSmlDSHostServer*>( this ) );	
	}

// ------------------------------------------------------------------------------------------------
// CNSmlDSHostServer::CNSmlDSHostServer
// ------------------------------------------------------------------------------------------------
EXPORT_C CNSmlDSHostServer::CNSmlDSHostServer() : CServer2(EPriorityStandard)
	{
	_DBG_FILE("CNSmlDSHostServer::CNSmlDSHostServer(): begin");
	_DBG_FILE("CNSmlDSHostServer::CNSmlDSHostServer(): end");
	}

// ------------------------------------------------------------------------------------------------
// CNSmlDSHostServer::IncSessionCount
// ------------------------------------------------------------------------------------------------
EXPORT_C void CNSmlDSHostServer::IncSessionCount()
	{
	_DBG_FILE("CNSmlDSHostServer::IncSessionCount(): begin");
	++iSessionCount;
	_DBG_FILE("CNSmlDSHostServer::IncSessionCount(): end");
	}

// ------------------------------------------------------------------------------------------------
// CNSmlDSHostServer::DecSessionCount
// ------------------------------------------------------------------------------------------------
EXPORT_C void CNSmlDSHostServer::DecSessionCount()
	{
	_DBG_FILE("CNSmlDSHostServer::DecSessionCount(): begin");
	--iSessionCount;
    if ( !iSessionCount )
        {
        CActiveScheduler::Stop();   
        }
	_DBG_FILE("CNSmlDSHostServer::DecSessionCount(): end");
	}
	
// ------------------------------------------------------------------------------------------------
// CNSmlDSHostServer::CallNewSessionL
// ------------------------------------------------------------------------------------------------
EXPORT_C CSession2* CNSmlDSHostServer::CallNewSessionL( const TVersion& aVersion, const RMessage2& aMessage ) const
    {
    return CNSmlDSHostServer::NewSessionL( aVersion, aMessage );
    }

// ------------------------------------------------------------------------------------------------
// CNSmlDSHostServer::HandleLeaveOnStartup
// ------------------------------------------------------------------------------------------------
void CNSmlDSHostServer::HandleLeaveOnStartup( TAny* /*aP*/ )
	{
	RProcess::Rendezvous( KErrGeneral );
	PanicServer( EMainSchedulerError );
	}

// ------------------------------------------------------------------------------------------------
// CNSmlDSHostServer::PanicServer
// ------------------------------------------------------------------------------------------------    
void CNSmlDSHostServer::PanicServer( TNSmlHostServerErrors aPanic )
	{
	_LIT( KTxtDSHostServerBase, "NSmlDSHostServerBase" );
	User::Panic( KTxtDSHostServerBase, aPanic );
	}

// End of File
