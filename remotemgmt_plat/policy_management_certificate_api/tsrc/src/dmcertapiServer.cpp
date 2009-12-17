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
 

#include "dmcertapiServer.h"
#include "dmcertapiClientServer.h"
#include "dmcertapiSession.h"

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
	CServer2 *t = CdmcertapiServer::NewLC();
	// Initialisation complete, now signal the client

	RProcess::Rendezvous(KErrNone);

	// Ready to run
	RDEBUG("CdmcertapiServer is running");
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
	RDEBUG("CdmcertapiServer: E32Main");
	CTrapCleanup* cleanup=CTrapCleanup::New();
	TInt r=KErrNoMemory;
	if (cleanup)
		{
		TRAP(r,RunServerL());
		RDEBUG_2("CdmcertapiServer closed with code: %d", r);
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
	
	After(KdmcertapiServerShutdownDelay);
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
// CdmcertapiServer::NewLC()
// -----------------------------------------------------------------------------


CServer2* CdmcertapiServer::NewLC()
	{
	RDEBUG("CdmcertapiServer::NewLC - start");
	
	CdmcertapiServer* self=new(ELeave) CdmcertapiServer;
	CleanupStack::PushL(self);
	self->ConstructL();
	RDEBUG("CdmcertapiServer::NewLC - end");
	return self;
	}

// -----------------------------------------------------------------------------
// CdmcertapiServer::ConstructL()
// -----------------------------------------------------------------------------


void CdmcertapiServer::ConstructL()
	{
	RDEBUG("CdmcertapiServer::ConstructL start");
	
	StartL(KDMSSServerName);
	iShutdown.ConstructL();
	iShutdown.Start();	
	
	RDEBUG("CdmcertapiServer::ConstructL end");
	}

// -----------------------------------------------------------------------------
// CdmcertapiServer::CdmcertapiServer()
// -----------------------------------------------------------------------------


CdmcertapiServer::CdmcertapiServer() :CServer2(0)
	{
	RDEBUG("CdmcertapiServer::CdmcertapiServer()");
	}
	

	
// -----------------------------------------------------------------------------
// CdmcertapiServer::AddSession()
// -----------------------------------------------------------------------------

void CdmcertapiServer::AddSession()
	{
	// A new session is being created
	RDEBUG( "CdmcertapiServer::AddSession" );
	++iSessionCount;
	
	// Cancel the shutdown timer if it was running
	iShutdown.Cancel();
	}



// -----------------------------------------------------------------------------
// CdmcertapiServer::DropSession()
// -----------------------------------------------------------------------------

void CdmcertapiServer::DropSession()
	{
	// A session is being destroyed
	RDEBUG( "CdmcertapiServer::DropSession" );

	// Start the shutdown timer if it is the last session
	if ( --iSessionCount == 0 )
		{
			iShutdown.Start();
		}
	}
	


// -----------------------------------------------------------------------------
// CdmcertapiServer::~CdmcertapiServer()
// -----------------------------------------------------------------------------

CdmcertapiServer::~CdmcertapiServer()
	{
	RDEBUG("CdmcertapiServer::CdmcertapiServer()");
	}



// -----------------------------------------------------------------------------
// CdmcertapiServer::NewSessionL()
// -----------------------------------------------------------------------------

CSession2* CdmcertapiServer::NewSessionL(const TVersion&,const RMessage2&) const
	{
	RDEBUG("CdmcertapiServer::NewSessionL");
	return new (ELeave) CdmcertapiSession();
	}

// end of file
