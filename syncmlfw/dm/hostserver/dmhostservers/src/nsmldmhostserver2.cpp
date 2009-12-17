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
* Description:  Specialized DM Host Server. In practice differs from 
*				 the other servers only on the capability and the name.
*
*/


// ------------------------------------------------------------------------------------------------
// Includes
// ------------------------------------------------------------------------------------------------
#include <nsmldebug.h>
#include "nsmldmclientandserver.h"
#include "nsmldmhostserver2.h"

// ------------------------------------------------------------------------------------------------
// CNSmlDMHostServer2::LaunchServer()
//	Launches the server.
//	Calls the common code part (InitHostServer) of all specialized DM Host Servers.
//	@param (TAny*). Not used in this function. 
//	  			    Kept here for possible future needs.
//	@return TInt. Return value from InitHostServer function.
// ------------------------------------------------------------------------------------------------
TInt CNSmlDMHostServer2::LaunchServer ( TAny* /*aStarted*/ )
    {
    return InitHostServer ( KNSmlDmHostServer2Name(), (TThreadFunction) ThreadFunction );
    }

// ------------------------------------------------------------------------------------------------
// CNSmlDMHostServer2::ThreadFunction()
//	Calls the NewL for creating the DM Host Server 2 instance.
//	This ThreadFunction is called when the 'common initialization actions' 
//	are successfully run.
//	Starts the active scheduler.
//	@param aStarted. Not used.
//	@return TInt. Returns always KErrNone.
// ------------------------------------------------------------------------------------------------
TInt CNSmlDMHostServer2::ThreadFunction ( TAny* /*aStarted*/ )
    {
	CNSmlDMHostServer2* hs = NULL;
    TRAPD ( err, ( hs = CNSmlDMHostServer2::NewL ( KNSmlDmHostServer2Name() ) ) );
    RProcess::Rendezvous( err );
	__ASSERT_ALWAYS ( !err, PanicServer ( ESvrCreateServer ) );
    CActiveScheduler::Start();
    delete hs;
    return KErrNone;
    }

// ------------------------------------------------------------------------------------------------
// CNSmlDMHostServer2::NewL()
//	Creates a new instance of DM Host Server 2.
//	Adds the server with the specified name to the active scheduler, 
//	and issues the first request for messages. 
//	Signals the global semaphore, which releases the client (DM Tree Module)
//	from the waiting state.
//	@param aServerName. The name of the server: "nsmldmhostserver2".
//	@return CNSmlDMHostServer2*. The created server instance.
// ------------------------------------------------------------------------------------------------
CNSmlDMHostServer2* CNSmlDMHostServer2::NewL ( const TDesC& aServerName )
	{
	CNSmlDMHostServer2* pS = new ( ELeave ) CNSmlDMHostServer2();
	CleanupStack::PushL ( pS );
	pS->StartL ( aServerName );
	CleanupStack::Pop(); // pS
	return pS;
	}

// ------------------------------------------------------------------------------------------------
// CNSmlDMHostServer2::~CNSmlDMHostServer2()
//	Destructor.
// ------------------------------------------------------------------------------------------------
CNSmlDMHostServer2::~CNSmlDMHostServer2()
	{
	_DBG_FILE("CNSmlDMHostServer2::~CNSmlDMHostServer2(): begin");
	_DBG_FILE("CNSmlDMHostServer2::~CNSmlDMHostServer2(): end");
	}

// ------------------------------------------------------------------------------------------------
// CNSmlDMHostServer2::CNSmlDMHostServer2()
//	C++ constructor.
// ------------------------------------------------------------------------------------------------
CNSmlDMHostServer2::CNSmlDMHostServer2()
	{
	}

// ------------------------------------------------------------------------------------------------
// CNSmlDMHostServer2::PanicServer()
//	Panics the server.
//	This is called if the instance creation of DM Host Server 2 leaves,
//	e.g. due to call 'StartL' CServer2 -function.
//	@param aPanic. The panic reason code.
// ------------------------------------------------------------------------------------------------
void CNSmlDMHostServer2::PanicServer ( TNSmlDmHostServerPanic aPanic )
    {
	_LIT ( KTxtDmHostServer2,"NSmlDmHostServer2");
	User::Panic ( KTxtDmHostServer2, aPanic );
    }

// ------------------------------------------------------------------------------------------------
// CNSmlDMHostServer2::NewSessionL()
//	Calls CallNewSessionL.
//	@param aVersion. The version of the server.
//	@param aMessage. An object which encapsulates a client request.
//	@return CSession2*. The created new session instance.
// ------------------------------------------------------------------------------------------------
CSession2* CNSmlDMHostServer2::NewSessionL ( const TVersion& aVersion, const RMessage2& aMessage ) const
	{ 
    return CallNewSessionL ( aVersion, aMessage );
   	}

// ------------------------------------------------------------------------------------------------
// E32Main
// Entry point when the server is launched.
// ------------------------------------------------------------------------------------------------
GLDEF_C TInt E32Main()
    {
    return CNSmlDMHostServer2::LaunchServer(NULL);
    }

// End of file
