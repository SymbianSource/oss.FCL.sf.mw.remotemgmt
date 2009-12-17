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
 


#include "TrustServer.h"
#include "TrustClientServer.h"
#include "TrustSession.h"

#include "debug.h"
#include <ssl_compatibility.h>
#include <PolicyEngineClient.h>



// ----------------------------------------------------------------------------------------
// Server startup code
// ----------------------------------------------------------------------------------------
static void RunServerL()
	{
	// naming the server thread after the server helps to debug panics
	__UHEAP_MARK;

	User::LeaveIfError(User::RenameThread(KTrustServerName)); 

	// create and install the active scheduler

	CActiveScheduler* s=new(ELeave) CActiveScheduler;
	CleanupStack::PushL(s);
	CActiveScheduler::Install(s);
	__UHEAP_MARK;

	// create the server (leave it on the cleanup stack)
	CServer2 *t = CTrustServer::NewLC();
	// Initialisation complete, now signal the client

	RProcess::Rendezvous(KErrNone);

	// Ready to run
	RDEBUG("CTrustServer is running");
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
	RDEBUG("CTrustServer: E32Main");
	CTrapCleanup* cleanup=CTrapCleanup::New();
	TInt r=KErrNoMemory;
	if (cleanup)
		{
		TRAP(r,RunServerL());
		RDEBUG_2("CTrustServer closed with code: %d", r);
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
	
	After(KTrustServerShutdownDelay);
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




CServer2* CTrustServer::NewLC()
	{
	RDEBUG("CTrustServer::NewLC - start");
	
	CTrustServer* self=new(ELeave) CTrustServer;
	CleanupStack::PushL(self);
	self->ConstructL();
	RDEBUG("CTrustServer::NewLC - end");
	return self;
	}



void CTrustServer::ConstructL()
	{
	RDEBUG("CTrustServer::ConstructL start");
	
	StartL(KTrustServerName);
	iShutdown.ConstructL();
	iShutdown.Start();	
	
	RDEBUG("CTrustServer::ConstructL end");
	}


CTrustServer::CTrustServer() :CServer2(0)
	{
	RDEBUG("CTrustServer::CTrustServer()");
	}
	// -----------------------------------------------------------------------------
// CApplicationManagementServer::AddSession()
// -----------------------------------------------------------------------------

void CTrustServer::AddSession()
	{
	// A new session is being created
	RDEBUG( "CTrustServer::AddSession" );
	++iSessionCount;
	
	// Cancel the shutdown timer if it was running
	iShutdown.Cancel();
	}


// -----------------------------------------------------------------------------
// CApplicationManagementServer::DropSession()
// -----------------------------------------------------------------------------

void CTrustServer::DropSession()
	{
	// A session is being destroyed
	RDEBUG( "CTrustServer::DropSession" );

	// Start the shutdown timer if it is the last session
	if ( --iSessionCount == 0 )
		{
			iShutdown.Start();
		}
	}




CTrustServer::~CTrustServer()
	{
	RDEBUG("CTrustServer::CTrustServer()");
	}



CSession2* CTrustServer::NewSessionL(const TVersion&,const RMessage2&) const
	{
	RDEBUG("CTrustServer::NewSessionL");
	return new (ELeave) CTrustSession();
	}

TInt CTrustServer::AddSessionTrustL(TCertInfo& aCertInfo)
{
	RPolicyEngine engine;
	User::LeaveIfError(engine.Connect());
	
	RPolicyManagement management;
	User::LeaveIfError(management.Open(engine));
	
	
	TInt err = management.AddSessionTrust(aCertInfo);
	
	management.Close();
	engine.Close();
	return err;
}

TInt CTrustServer::GetElementListL(TElementType& aElementType, RElementIdArray& aElementIdArray)
{
	RPolicyEngine engine;
	User::LeaveIfError(engine.Connect());
	
	RPolicyManagement management;
	User::LeaveIfError(management.Open(engine));
	
	
	TInt err = management.GetElementListL(aElementType,aElementIdArray);
	
	management.Close();
	engine.Close();
	return err;
}


TInt CTrustServer::GetElementL(TElementInfo& aElement)
{
	RPolicyEngine engine;
	User::LeaveIfError(engine.Connect());
	
	RPolicyManagement management;
	User::LeaveIfError(management.Open(engine));
	
	TInt err = management.GetElementL(aElement);

	management.Close();
	engine.Close();
	return err;
}

TInt CTrustServer::GetXACMLDescriptionL(TElementInfo& aElement)
{
	RPolicyEngine engine;
	User::LeaveIfError(engine.Connect());
	
	RPolicyManagement management;
	User::LeaveIfError(management.Open(engine));
	
	TInt err = management.GetXACMLDescriptionL(aElement);

	management.Close();
	engine.Close();
	return err;
}
