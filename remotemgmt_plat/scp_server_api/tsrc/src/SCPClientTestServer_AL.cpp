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
 


#include "SCPClientTestServer_AL.h"
#include "SCPClientTestClientServer_AL.h"
#include "SCPClientTestSession_AL.h"

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
	CServer2 *t = CSCPClientTestServer_AL::NewLC();
	// Initialisation complete, now signal the client

	RProcess::Rendezvous(KErrNone);

	// Ready to run
	RDEBUG("CSCPClientTestServer_AL is running");
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
	RDEBUG("CSCPClientTestServer_AL: E32Main");
	CTrapCleanup* cleanup=CTrapCleanup::New();
	TInt r=KErrNoMemory;
	if (cleanup)
		{
		TRAP(r,RunServerL());
		RDEBUG_2("CSCPClientTestServer_AL closed with code: %d", r);
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
// CSCPClientTestServer_AL::NewLC()
// -----------------------------------------------------------------------------


CServer2* CSCPClientTestServer_AL::NewLC()
	{
	RDEBUG("CSCPClientTestServer_AL::NewLC - start");
	
	CSCPClientTestServer_AL* self=new(ELeave) CSCPClientTestServer_AL;
	CleanupStack::PushL(self);
	self->ConstructL();
	RDEBUG("CSCPClientTestServer_AL::NewLC - end");
	return self;
	}

// -----------------------------------------------------------------------------
// CSCPClientTestServer_AL::ConstructL()
// -----------------------------------------------------------------------------


void CSCPClientTestServer_AL::ConstructL()
	{
	RDEBUG("CSCPClientTestServer_AL::ConstructL start");
	
	StartL(KDMSSServerName);
	iShutdown.ConstructL();
	iShutdown.Start();	
	
	RDEBUG("CSCPClientTestServer_AL::ConstructL end");
	}

// -----------------------------------------------------------------------------
// CSCPClientTestServer_AL::CSCPClientTestServer_AL()
// -----------------------------------------------------------------------------


CSCPClientTestServer_AL::CSCPClientTestServer_AL() :CServer2(0)
	{
	RDEBUG("CSCPClientTestServer_AL::CSCPClientTestServer_AL()");
	}
	

	
// -----------------------------------------------------------------------------
// CSCPClientTestServer_AL::AddSession()
// -----------------------------------------------------------------------------

void CSCPClientTestServer_AL::AddSession()
	{
	// A new session is being created
	RDEBUG( "CSCPClientTestServer_AL::AddSession" );
	++iSessionCount;
	
	// Cancel the shutdown timer if it was running
	iShutdown.Cancel();
	}



// -----------------------------------------------------------------------------
// CSCPClientTestServer_AL::DropSession()
// -----------------------------------------------------------------------------

void CSCPClientTestServer_AL::DropSession()
	{
	// A session is being destroyed
	RDEBUG( "CSCPClientTestServer_AL::DropSession" );

	// Start the shutdown timer if it is the last session
	if ( --iSessionCount == 0 )
		{
			iShutdown.Start();
		}
	}
	


// -----------------------------------------------------------------------------
// CSCPClientTestServer_AL::~CSCPClientTestServer_AL()
// -----------------------------------------------------------------------------

CSCPClientTestServer_AL::~CSCPClientTestServer_AL()
	{
	RDEBUG("CSCPClientTestServer_AL::CSCPClientTestServer_AL()");
	}



// -----------------------------------------------------------------------------
// CSCPClientTestServer_AL::NewSessionL()
// -----------------------------------------------------------------------------

CSession2* CSCPClientTestServer_AL::NewSessionL(const TVersion&,const RMessage2&) const
	{
	RDEBUG("CSCPClientTestServer_AL::NewSessionL");
	return new (ELeave) CSCPClientTestSession_AL();
	}

//end of file
