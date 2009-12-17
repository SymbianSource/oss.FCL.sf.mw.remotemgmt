/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  SyncML Obex server internal server module
*
*/


#include <nsmldebug.h>

#include "nsmlobexcommserver.h"
#include "nsmlobexdefs.h"


// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CNSmlObexCommServer::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CNSmlObexCommServer* CNSmlObexCommServer::NewL( const TDesC& aServerName )
	{
	_DBG_FILE("CNSmlObexCommServer::NewL(): begin");
	CNSmlObexCommServer *pS = new (ELeave) CNSmlObexCommServer();
	CleanupStack::PushL(pS);
	pS->ConstructL();
	pS->StartL(aServerName);
	CleanupStack::Pop(); // pS
	_DBG_FILE("CNSmlObexCommServer::NewL(): end");
	return pS;
	}

// -----------------------------------------------------------------------------
// CNSmlObexCommServer::~CNSmlObexCommServer
// Destructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CNSmlObexCommServer::~CNSmlObexCommServer()
	{
	_DBG_FILE("CNSmlObexCommServer::~CNSmlObexCommServer(): begin");
	delete iSendBuffer;
	delete iRecvBuffer;
	delete iClientReceiveTimeOut;
	delete iServerGetSendTimeOut;
	_DBG_FILE("CNSmlObexCommServer::~CNSmlObexCommServer(): end");
	}

// -----------------------------------------------------------------------------
// CNSmlObexCommServer::NewSessionL
// -----------------------------------------------------------------------------
//
CSession2* CNSmlObexCommServer::NewSessionL( const TVersion& aVersion,
    const RMessage2& /*aMessage*/ ) const
	{
	_DBG_FILE("CNSmlObexCommServer::NewSessionL(): begin");
	if( iSessionCount > 1 )
		{
		_DBG_FILE("CNSmlObexCommServer - too many sessions, leaving");
		User::Leave(KErrCouldNotConnect);
		}
	TVersion v(KNSmlObexCommServerVersionMajor, KNSmlObexCommServerVersionMinor, 0);
	if( !User::QueryVersionSupported(v,aVersion) )
		{
		_DBG_FILE("CNSmlObexCommServer::NewSessionL(): wrong version offered -> BAIL OUT!");
		User::Leave(KErrNotSupported);
		}
	_DBG_FILE("CNSmlObexCommServer::NewSessionL(): end");
	return CNSmlObexCommSession::NewL( *CONST_CAST(CNSmlObexCommServer*, this));	
	}

// -----------------------------------------------------------------------------
// CNSmlObexCommServer::CNSmlObexCommServer
// -----------------------------------------------------------------------------
//
CNSmlObexCommServer::CNSmlObexCommServer() : CServer2(EPriorityStandard)
	{
	}

// -----------------------------------------------------------------------------
// CNSmlObexCommServer::ConstructL
// Symbian 2nd phase constructor.
// -----------------------------------------------------------------------------
//
void CNSmlObexCommServer::ConstructL()
	{
	_DBG_FILE("CNSmlObexCommServer::ConstructL(): begin");
	iClientReceiveTimeOut = CNSmlTimeOut::NewL();
	iServerGetSendTimeOut = CNSmlTimeOut::NewL();
	_DBG_FILE("CNSmlObexCommServer::ConstructL(): end");
	}

// -----------------------------------------------------------------------------
// CNSmlObexCommServer::IncrementSessions
// -----------------------------------------------------------------------------
//
void CNSmlObexCommServer::IncrementSessions()
	{
	_DBG_FILE("CNSmlObexCommServer::IncrementSessions(): begin");
	iSessionCount++;
	_DBG_FILE("CNSmlObexCommServer::IncrementSessions(): end");
	}

// -----------------------------------------------------------------------------
// CNSmlObexCommServer::DecrementSessions
// -----------------------------------------------------------------------------
//
void CNSmlObexCommServer::DecrementSessions()
	{
	_DBG_FILE("CNSmlObexCommServer::DecrementSessions(): begin");
	iSessionCount--;
	if( !iSessionCount )
		{
		Shutdown();
		}
	_DBG_FILE("CNSmlObexCommServer::DecrementSessions(): end");
	}

// -----------------------------------------------------------------------------
// CNSmlObexCommServer::Shutdown
// -----------------------------------------------------------------------------
//
void CNSmlObexCommServer::Shutdown()
	{
	_DBG_FILE("CNSmlObexCommServer::Shutdown(): begin");
	CActiveScheduler::Stop();
	_DBG_FILE("CNSmlObexCommServer::Shutdown(): end");
	}

// ========================== OTHER EXPORTED FUNCTIONS =========================

//End of File
