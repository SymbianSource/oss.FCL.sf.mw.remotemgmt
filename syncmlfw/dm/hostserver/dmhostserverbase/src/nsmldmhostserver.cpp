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
* Description:  Contains a common code for all dedicated dm host servers.
*
*/


// ------------------------------------------------------------------------------------------------
// Includes
// ------------------------------------------------------------------------------------------------

#include <s32mem.h>
#include <data_caging_path_literals.hrh>
#include <f32file.h>
#include <e32math.h>
#include <nsmlconstants.h>
#include <nsmldebug.h>
#include "nsmldmclientandserver.h"
#include "nsmldmhostserver.h"
#include "nsmldmhostsession.h"


// ------------------------------------------------------------------------------------------------
// Constants
// ------------------------------------------------------------------------------------------------
//
const TInt KNSmlMaxSessionCount = 2;  // Allowed clients: Treemodule and another dm host server
_LIT ( KNSmlDmThreadAux, "_thread" );
_LIT ( KTxtDmHostServerBase,"NSmlDmHostServerBase");
const TUint32 KNSmlDMHostServer1ID = 270506498;  // 0x101F9A02
const TUint32 KNSmlDMHostServer2ID = 270506499;  // 0x101F9A03
const TUint32 KNSmlDMHostServer3ID = 270506500;  // 0x101F9A04
const TUint32 KNSmlDMHostServer4ID = 270506501;  // 0x101F9A05


// ------------------------------------------------------------------------------------------------
// CNSmlDMHostServer::~CNSmlDMHostServer
// Destructor.
// ------------------------------------------------------------------------------------------------
//
EXPORT_C CNSmlDMHostServer::~CNSmlDMHostServer()
	{
	_DBG_FILE("CNSmlDMHostServer::~CNSmlDMHostServer(): begin");
	_DBG_FILE("CNSmlDMHostServer::~CNSmlDMHostServer(): end");
	}
// ------------------------------------------------------------------------------------------------
// CNSmlDMHostServer::InitHostServer
// Initializes the server, calls aFunction.
// ------------------------------------------------------------------------------------------------
//
EXPORT_C TInt CNSmlDMHostServer::InitHostServer ( const TDesC& aServerName, TThreadFunction aFunction )
    {
    // check server not already started
	TFindServer findHostServer ( aServerName );
	TFullName name;
	if ( findHostServer.Next ( name ) == KErrNone )
		{
		return KErrAlreadyExists;
		}
	TInt error( KErrNone );
    error = ThreadFunction( (TAny*)aFunction );
//	_DBG_FILE("CNSmlDMHostServer::InitHostServer(): end");        
	return error;
    }

// ------------------------------------------------------------------------------------------------
// CNSmlDMHostServer::ThreadFunction
// Starting point of a server thread.
// ------------------------------------------------------------------------------------------------    
//
TInt CNSmlDMHostServer::ThreadFunction ( TAny* aStarted )
    {
    __UHEAP_MARK;
    CTrapCleanup* cleanup = CTrapCleanup::New();
    __ASSERT_ALWAYS ( cleanup, PanicServer ( EMainSchedulerError ) );    
    CActiveScheduler* scheduler = new CActiveScheduler();
    __ASSERT_ALWAYS ( scheduler, PanicServer ( EMainSchedulerError ) );
	CActiveScheduler::Install ( scheduler );
	TThreadFunction function = reinterpret_cast<TThreadFunction>( aStarted );

	__UHEAP_MARK;
	TInt ret( function( NULL ) );
	__UHEAP_MARKEND;

    CActiveScheduler::Install ( NULL );
    delete scheduler;
    delete cleanup;
    __UHEAP_MARKEND;
    
    return ret;
    }

// ------------------------------------------------------------------------------------------------
// CNSmlDMHostServer::ServerName
// Server exe name is concluded from the current thread name.
// ------------------------------------------------------------------------------------------------    
//
TPtrC CNSmlDMHostServer::ServerName()
    {
	RThread th;    
	TName thname = th.Name();
	TInt offset = thname.Find ( KNSmlDmThreadAux );
	TPtrC serverExeName;
	serverExeName.Set ( KNullDesC );		
	if ( offset != KErrNotFound )
		{
		TPtrC serverName = thname.Left ( offset );
		if ( !serverName.Compare ( KNSmlDmHostServer1Name ) )
			{
			serverExeName.Set ( KNSmlDmHostServer1Exe );
			}
		else if ( !serverName.Compare ( KNSmlDmHostServer2Name ) )
			{
			serverExeName.Set ( KNSmlDmHostServer2Exe );
			} 
		else if ( !serverName.Compare ( KNSmlDmHostServer3Name ) )
			{
			serverExeName.Set ( KNSmlDmHostServer3Exe );
			} 
		else if ( !serverName.Compare ( KNSmlDmHostServer4Name ) )
			{
			serverExeName.Set ( KNSmlDmHostServer4Exe );
			}
		}
	return serverExeName;
    }
// ------------------------------------------------------------------------------------------------
// CNSmlDMHostServer::NewSessionL
// Returns a new session object, when security and version checkings are successfully passed.
// ------------------------------------------------------------------------------------------------
//
CSession2* CNSmlDMHostServer::NewSessionL( const TVersion& aVersion, const RMessage2& aMessage ) const
	{
	_DBG_FILE("CNSmlDMHostServer::NewSessionL(): begin");
	if( iSessionCount >= KNSmlMaxSessionCount )
		{
		User::Leave(KErrCouldNotConnect);
		}

	// check we're the right version
	TVersion ver ( KNSmlDmHostServerVerMajor, KNSmlDmHostServerVerMinor, KNSmlDmHostServerVerBuild );
	if( !User::QueryVersionSupported ( ver, aVersion) )
		{
		_DBG_FILE("CNSmlDMHostServer::NewSessionL(): wrong version offered -> BAIL OUT!");
		User::Leave ( KErrNotSupported );
		}

	TUint32 idOfCaller = aMessage.SecureId().iId;
	if ( idOfCaller != KNSmlDMHostServer1ID && idOfCaller != KNSmlDMHostServer2ID
			&& idOfCaller != KNSmlDMHostServer3ID && idOfCaller != KNSmlDMHostServer4ID )
		{
		_LIT_SECURITY_POLICY_S0 ( mySidPolicy, KNSmlSOSServerSecureID );
    	TBool status = mySidPolicy().CheckPolicy ( aMessage );
		}

	_DBG_FILE("CNSmlDMHostServer::NewSessionL(): end");
	return CNSmlDMHostSession::NewL ( *CONST_CAST ( CNSmlDMHostServer*, this ) );	
	}

// ------------------------------------------------------------------------------------------------
// CNSmlDMHostServer::CNSmlDMHostServer()
// Constructor. Protected to prevent direct instantiation.
// ------------------------------------------------------------------------------------------------
//
EXPORT_C CNSmlDMHostServer::CNSmlDMHostServer() : CServer2 ( EPriorityStandard )
	{
	_DBG_FILE("CNSmlDMHostServer::CNSmlDMHostServer(): begin");
	_DBG_FILE("CNSmlDMHostServer::CNSmlDMHostServer(): end");
	}

// ------------------------------------------------------------------------------------------------
// CNSmlDMHostServer::IncSessionCount
// Increments session count.
// ------------------------------------------------------------------------------------------------
//
void CNSmlDMHostServer::IncSessionCount()
	{
	_DBG_FILE("CNSmlDMHostServer::IncSessionCount(): begin");
	iSessionCount++;
	_DBG_FILE("CNSmlDMHostServer::IncSessionCount(): end");
	}

// ------------------------------------------------------------------------------------------------
// CNSmlDMHostServer::DecSessionCount
// Decrements session count. When count reaches zero, stops active scheduler.
// ------------------------------------------------------------------------------------------------
//
void CNSmlDMHostServer::DecSessionCount()
	{
	_DBG_FILE("CNSmlDMHostServer::DecSessionCount(): begin");
	iSessionCount--;
    if ( iSessionCount < 1 )
        {
        Cancel();
        CActiveScheduler::Stop();   
        }
	_DBG_FILE("CNSmlDMHostServer::DecSessionCount(): end");
	}

// ------------------------------------------------------------------------------------------------
// CNSmlDMHostServer::CallNewSessionL
// Calls NewSessionL.
// ------------------------------------------------------------------------------------------------
//
EXPORT_C CSession2* CNSmlDMHostServer::CallNewSessionL ( const TVersion& aVersion, const RMessage2& aMessage ) const
    {
	_DBG_FILE("CNSmlDMHostServer::CallNewSessionL()");
    return CNSmlDMHostServer::NewSessionL( aVersion, aMessage );
    }

// ------------------------------------------------------------------------------------------------
// CNSmlDMHostServer::PanicServer()
// Panics the server. Signals the semaphore in order to prevent dm client halting.
// ------------------------------------------------------------------------------------------------
//
void CNSmlDMHostServer::PanicServer ( TNSmlDmHostServerPanic aPanic )
    {
	_DBG_FILE("CNSmlDMHostServer::PanicServer()");            
	RSemaphore s;
	TBuf<64> semaphoreName ( ServerName() );
	if( s.OpenGlobal ( semaphoreName ) == KErrNone )
		{
		s.Signal();
		s.Close();
		}
	
	User::Panic ( KTxtDmHostServerBase, aPanic );
    }	

// End of file
