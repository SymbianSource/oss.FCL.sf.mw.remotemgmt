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
 


#include "TempServer.h"
#include "TempClientServer.h"
#include "TempSession.h"

#include "debug.h"
#include <PolicyEngineClient.h>




// ----------------------------------------------------------------------------------------
// Server startup code
// ----------------------------------------------------------------------------------------
static void RunServerL()
	{
	// naming the server thread after the server helps to debug panics
	__UHEAP_MARK;

	User::LeaveIfError(User::RenameThread(KTempServerName)); 

	// create and install the active scheduler

	CActiveScheduler* s=new(ELeave) CActiveScheduler;
	CleanupStack::PushL(s);
	CActiveScheduler::Install(s);
	__UHEAP_MARK;

	// create the server (leave it on the cleanup stack)
	CServer2 *t = CTempServer::NewLC();
	// Initialisation complete, now signal the client

	RProcess::Rendezvous(KErrNone);

	// Ready to run
	RDEBUG("CTempServer is running");
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
	RDEBUG("CTempServer: E32Main");
	CTrapCleanup* cleanup=CTrapCleanup::New();
	TInt r=KErrNoMemory;
	if (cleanup)
		{
		TRAP(r,RunServerL());
		RDEBUG_2("CTempServer closed with code: %d", r);
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
	
	After(KTempServerShutdownDelay);
	
	}

// -----------------------------------------------------------------------------
// CShutDown::RunL()
// -----------------------------------------------------------------------------

void CShutdown::RunL()
	{
	RDEBUG( "creenSaverServer timeout ... closing" );
	CActiveScheduler::Stop();
	}




CServer2* CTempServer::NewLC()
	{
	RDEBUG("CTempServer::NewLC - start");
	
	CTempServer* self=new(ELeave) CTempServer;
	CleanupStack::PushL(self);
	self->ConstructL();
	RDEBUG("CTempServer::NewLC - end");
	return self;
	}



void CTempServer::ConstructL()
	{
	RDEBUG("CTempServer::ConstructL start");
	
	StartL(KTempServerName);
	iShutdown.ConstructL();
	iShutdown.Start();	
	
	RDEBUG("CTempServer::ConstructL end");
	}


CTempServer::CTempServer() :CServer2(0)
	{
	RDEBUG("CTempServer::CTempServer()");
	}
	// -----------------------------------------------------------------------------
// CApplicationManagementServer::AddSession()
// -----------------------------------------------------------------------------

void CTempServer::AddSession()
	{
	// A new session is being created
	RDEBUG( "CTempServer::AddSession" );
	++iSessionCount;
	
	// Cancel the shutdown timer if it was running
	iShutdown.Cancel();
	}


// -----------------------------------------------------------------------------
// CApplicationManagementServer::DropSession()
// -----------------------------------------------------------------------------

void CTempServer::DropSession()
	{
	// A session is being destroyed
	RDEBUG( "CTempServer::DropSession" );

	// Start the shutdown timer if it is the last session
	if ( --iSessionCount == 0 )
		{
			iShutdown.Start();
		}
	}




CTempServer::~CTempServer()
	{
	RDEBUG("CTempServer::CTempServer()");
	}



CSession2* CTempServer::NewSessionL(const TVersion&,const RMessage2&) const
	{
	RDEBUG("CTempServer::NewSessionL");
	return new (ELeave) CTempSession();
	}

TInt CTempServer::PerformRFS()
{
	RPolicyEngine engine;
	TInt err = engine.Connect();
	if (!err)
	{
		err = engine.PerformRFS();
	}
	
	engine.Close();
	return err;
}
