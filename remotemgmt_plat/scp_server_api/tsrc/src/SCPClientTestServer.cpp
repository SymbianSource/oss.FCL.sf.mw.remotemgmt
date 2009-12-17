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
* Description: definition of dm constants/exported methods
* 	This is part of remotemgmt_plat.
*
*/
 

#include "SCPClientTestServer.h"
#include "SCPClientTestClientServer.h"
#include "SCPClientTestSession.h"

#include "debug.h"


// ----------------------------------------------------------------------------------------
// Server startup code
// ----------------------------------------------------------------------------------------
static void RunServerL()
	{
	// naming the server thread after the server helps to debug panics
	__UHEAP_MARK;

	User::LeaveIfError(User::RenameThread(KDMSSServerName)); 

	// create and install the active scheduler

	CActiveScheduler* s=new(ELeave) CActiveScheduler;
	CleanupStack::PushL(s);
	CActiveScheduler::Install(s);
	__UHEAP_MARK;

	// create the server (leave it on the cleanup stack)
	CServer2 *t = CSCPClientTestServer::NewLC();
	// Initialisation complete, now signal the client

	RProcess::Rendezvous(KErrNone);

	// Ready to run
	RDEBUG("CSCPClientTestServer is running");
	CActiveScheduler::Start();


	// Cleanup the server and scheduler
	CleanupStack::PopAndDestroy(t);
	__UHEAP_MARKEND;

	CleanupStack::PopAndDestroy(s);
	__UHEAP_MARKEND;
	}

// Server process entry-point
TInt E32Main()
	{
	__UHEAP_MARK;
	RDEBUG("CSCPClientTestServer: E32Main");
	CTrapCleanup* cleanup=CTrapCleanup::New();
	TInt r=KErrNoMemory;
	if (cleanup)
		{
		TRAP(r,RunServerL());
		RDEBUG_2("CSCPClientTestServer closed with code: %d", r);
		delete cleanup;
		}
	__UHEAP_MARKEND;
	return r;
	}




// -----------------------------------------------------------------------------
// CShutDown::CShutdown()
// -----------------------------------------------------------------------------

inline CShutdown::CShutdown()
	:CTimer(-1)
	{
	CActiveScheduler::Add(this);
	}
	
	
// -----------------------------------------------------------------------------
// CShutDown::ConstructL()
// -----------------------------------------------------------------------------

inline void CShutdown::ConstructL()
	{
	CTimer::ConstructL();
	}

// -----------------------------------------------------------------------------
// CShutDown::Start()
// -----------------------------------------------------------------------------

inline void CShutdown::Start()
	{
	
	RDEBUG( "creenSaverServer: starting shutdown timeout" );
	
	After(KSCPClientTestServerShutdownDelay);
	//SetActive();
	}

// -----------------------------------------------------------------------------
// CShutDown::RunL()
// -----------------------------------------------------------------------------

void CShutdown::RunL()
	{
	RDEBUG( "creenSaverServer timeout ... closing" );
	CActiveScheduler::Stop();
	}


// -----------------------------------------------------------------------------
// CSCPClientTestServer::NewLC()
// -----------------------------------------------------------------------------


CServer2* CSCPClientTestServer::NewLC()
	{
	RDEBUG("CSCPClientTestServer::NewLC - start");
	
	CSCPClientTestServer* self=new(ELeave) CSCPClientTestServer;
	CleanupStack::PushL(self);
	self->ConstructL();
	RDEBUG("CSCPClientTestServer::NewLC - end");
	return self;
	}

// -----------------------------------------------------------------------------
// CSCPClientTestServer::ConstructL()
// -----------------------------------------------------------------------------


void CSCPClientTestServer::ConstructL()
	{
	RDEBUG("CSCPClientTestServer::ConstructL start");
	
	StartL(KDMSSServerName);
	iShutdown.ConstructL();
	iShutdown.Start();	
	
	RDEBUG("CSCPClientTestServer::ConstructL end");
	}

// -----------------------------------------------------------------------------
// CSCPClientTestServer::CSCPClientTestServer()
// -----------------------------------------------------------------------------


CSCPClientTestServer::CSCPClientTestServer() :CServer2(0)
	{
	RDEBUG("CSCPClientTestServer::CSCPClientTestServer()");
	}
	

	
// -----------------------------------------------------------------------------
// CSCPClientTestServer::AddSession()
// -----------------------------------------------------------------------------

void CSCPClientTestServer::AddSession()
	{
	// A new session is being created
	RDEBUG( "CSCPClientTestServer::AddSession" );
	++iSessionCount;
	
	// Cancel the shutdown timer if it was running
	iShutdown.Cancel();
	}



// -----------------------------------------------------------------------------
// CSCPClientTestServer::DropSession()
// -----------------------------------------------------------------------------

void CSCPClientTestServer::DropSession()
	{
	// A session is being destroyed
	RDEBUG( "CSCPClientTestServer::DropSession" );

	// Start the shutdown timer if it is the last session
	if ( --iSessionCount == 0 )
		{
			iShutdown.Start();
		}
	}
	


// -----------------------------------------------------------------------------
// CSCPClientTestServer::~CSCPClientTestServer()
// -----------------------------------------------------------------------------

CSCPClientTestServer::~CSCPClientTestServer()
	{
	RDEBUG("CSCPClientTestServer::CSCPClientTestServer()");
	}



// -----------------------------------------------------------------------------
// CSCPClientTestServer::NewSessionL()
// -----------------------------------------------------------------------------

CSession2* CSCPClientTestServer::NewSessionL(const TVersion&,const RMessage2&) const
	{
	RDEBUG("CSCPClientTestServer::NewSessionL");
	return new (ELeave) CSCPClientTestSession();
	}

// end of file
