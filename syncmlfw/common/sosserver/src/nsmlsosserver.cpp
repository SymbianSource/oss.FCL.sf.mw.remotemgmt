/*
* Copyright (c) 2004 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Symbian OS Server source.
*
*/


#include <f32file.h>
#include <e32math.h>

#include <nsmldebug.h>
#include "nsmlsosserver.h"

GLDEF_C TInt E32Main() // main function called by E32
	{
    return LaunchServer();
	}

// --------------------------------------------------------------------------
// EXPORT_C TInt LaunchServer()
// --------------------------------------------------------------------------
//
TInt LaunchServer()
    {
    // check server not already started
	TFindServer findSOSServer( KSOSServerName );
	TFullName name;
	if ( findSOSServer.Next(name) == KErrNone )
		{ // found server already 
		return KErrGeneral;
		}

	// create server thread
	RSemaphore signal;
    signal.CreateLocal(0);

	// randomize server thread's name
    TName threadName( KSOSServerThread );
    threadName.AppendNum( Math::Random(), EHex );
    
    RThread thread;
	TInt res = thread.Create( threadName, // name of thread
		                      CNSmlSOSServer::ThreadFunction, // thread function
		                      KDefaultStackSize, NULL,
		                      &signal ); // parameter to thread function
		
	if ( res != KErrNone )
		{
		thread.Close();
		return res;
		}
	// start thread
	thread.SetPriority(EPriorityMuchMore); // set priority
	thread.Resume(); 
    thread.Close();
    signal.Wait(); 
    signal.Close();

    return res;
    }

// --------------------------------------------------------------------------
// void CNSmlSOSServer::PanicServer( TNSmlSOSServerPanic aPanic )
// --------------------------------------------------------------------------
//
void CNSmlSOSServer::PanicServer( TNSmlSOSServerPanic aPanic )
    {
	_LIT(KTxtSOSServer,"SOSServer");
	User::Panic(KTxtSOSServer,aPanic);
    }

// --------------------------------------------------------------------------
// TInt CNSmlSOSServer::ThreadFunction(TAny* aStarted)
// --------------------------------------------------------------------------
//
TInt CNSmlSOSServer::ThreadFunction( TAny* aStarted )
	{
#ifdef __CLIENT_API_MT_
    __UHEAP_MARK;
#endif // __CLIENT_API_MT_

	// create cleanup stack
    CTrapCleanup* cleanup = CTrapCleanup::New();
	if ( !cleanup )
	    {
	    PanicServer(ECleanupCreateError);
	    }    
	    
	DBG_BEGIN();
	_DBG_FILE("CNSmlSOSServer::ThreadFunction(): CleanupStack created.");
	    
	// convert argument to semaphore
	RSemaphore& started= *(RSemaphore*) aStarted;
	
	// construct and install active scheduler
	CActiveScheduler* scheduler = new CActiveScheduler;
	__ASSERT_ALWAYS( scheduler,PanicServer(EMainSchedulerError) );
	CActiveScheduler::Install( scheduler );

	// construct server
    CNSmlSOSServer* server = NULL;
    TRAPD( err, server = CNSmlSOSServer::NewL());

	RProcess::Rendezvous( err );

	// start handling requests
	CActiveScheduler::Start();
	
    started.Signal();
	
	_DBG_FILE("CNSmlSOSServer::ThreadFunction(): CActiveScheduler stopped");

    delete server;
    delete scheduler;    
    delete cleanup;

#ifdef __CLIENT_API_MT_
    __UHEAP_MARKEND;
#endif // __CLIENT_API_MT_
	
    return err;
	}

// --------------------------------------------------------------------------
// CNSmlSOSServer* CNSmlSOSServer::NewL()
// --------------------------------------------------------------------------
//
CNSmlSOSServer* CNSmlSOSServer::NewL()
	{
	_DBG_FILE("CNSmlSOSServer::NewL(): begin");
	
	CNSmlSOSServer* self = new (ELeave) CNSmlSOSServer();
	CleanupStack::PushL(self);
    self->ConstructL();
	self->StartL(KSOSServerName);
    CleanupStack::Pop(); // self

	_DBG_FILE("CNSmlSOSServer::NewL(): end");
	
	return self;
	}

// --------------------------------------------------------------------------
// CNSmlSOSServer::CNSmlSOSServer()
// --------------------------------------------------------------------------
//
CNSmlSOSServer::CNSmlSOSServer() : CServer2(EPriorityStandard, ESharableSessions)
	{
	}

// --------------------------------------------------------------------------
// void CNSmlSOSServer::ConstructL()
// --------------------------------------------------------------------------
//
void CNSmlSOSServer::ConstructL()
    {
    iHandler = CNSmlSOSHandler::NewL();
    iBackup = CNSmlSOSBackup::NewL(iHandler);
    iObjConIndex = CObjectConIx::NewL();
    }

// --------------------------------------------------------------------------
// CNSmlSOSServer::~CNSmlSOSServer()
// --------------------------------------------------------------------------
//
CNSmlSOSServer::~CNSmlSOSServer()
	{
    delete iBackup;
    delete iHandler;
    delete iObjConIndex;
    }

// --------------------------------------------------------------------------
// CSession2* CNSmlSOSServer::NewSessionL( const TVersion& aVersion, const RMessage2& aMessage ) const
// --------------------------------------------------------------------------
//
CSession2* CNSmlSOSServer::NewSessionL( const TVersion& aVersion, const RMessage2& /*aMessage*/ ) const
	{
	_DBG_FILE("CNSmlSOSServer::NewSessionL(): begin");
	TVersion v(KNSmlSOSServerVersionMajor, KNSmlSOSServerVersionMinor, 0);
	if( !User::QueryVersionSupported(v,aVersion) )
		{
		User::Leave(KErrNotSupported);
		}

    iBackup->Subscribe();
    
    _DBG_FILE("CNSmlSOSServer::NewSessionL(): end");
	return CNSmlSOSSession::NewL( (CNSmlSOSServer*)this, iHandler ); 
	}

// --------------------------------------------------------------------------
// CObjectCon* CNSmlSOSServer::NewContainerL()
// --------------------------------------------------------------------------
//
CObjectCon* CNSmlSOSServer::NewContainerL()
    {
    return iObjConIndex->CreateL();
    }

//End of File

