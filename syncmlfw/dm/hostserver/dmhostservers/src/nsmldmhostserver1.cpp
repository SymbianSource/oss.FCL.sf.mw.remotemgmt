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
#include "nsmldmhostserver1.h"

// ------------------------------------------------------------------------------------------------
// CNSmlDMHostServer1::LaunchServer()
//	Launches the server.
//	Calls the common code part (InitHostServer) of all specialized DM Host Servers.
//	@param (TAny*). Not used in this function. 
//	  			    Kept here for possible future needs.
//	@return TInt. Return value from InitHostServer function.
// ------------------------------------------------------------------------------------------------
TInt CNSmlDMHostServer1::LaunchServer ( TAny* /*aStarted*/ )
    {
    return InitHostServer ( KNSmlDmHostServer1Name(), (TThreadFunction) ThreadFunction );
    }

// ------------------------------------------------------------------------------------------------
// CNSmlDMHostServer1::ThreadFunction()
//	Calls the NewL for creating the DM Host Server 1 instance.
//	This ThreadFunction is called when the 'common initialization actions' 
//	are successfully run.
//	Starts the active scheduler.
//	@param aStarted. Not used.
//	@return TInt. Returns always KErrNone.
// ------------------------------------------------------------------------------------------------
TInt CNSmlDMHostServer1::ThreadFunction ( TAny* /*aStarted*/ )
    {
	CNSmlDMHostServer1* hs = NULL;
    TRAPD ( err, ( hs = CNSmlDMHostServer1::NewL ( KNSmlDmHostServer1Name() ) ) );
    RProcess::Rendezvous( err );
	__ASSERT_ALWAYS ( !err, PanicServer ( ESvrCreateServer ) );    
    CActiveScheduler::Start();
    delete hs;
    return KErrNone;
    }

// ------------------------------------------------------------------------------------------------
// CNSmlDMHostServer1::NewL()
//	Creates a new instance of DM Host Server 1.
//	Adds the server with the specified name to the active scheduler, 
//	and issues the first request for messages. 
//	Signals the global semaphore, which releases the client (DM Tree Module)
//	from the waiting state.
//	@param aServerName. The name of the server: "nsmldmhostserver1".
//	@return CNSmlDMHostServer1*. The created server instance.
// ------------------------------------------------------------------------------------------------
CNSmlDMHostServer1* CNSmlDMHostServer1::NewL ( const TDesC& aServerName )
	{
	CNSmlDMHostServer1* pS = new ( ELeave ) CNSmlDMHostServer1();
	CleanupStack::PushL ( pS );
	pS->StartL ( aServerName );
	CleanupStack::Pop(); // pS
	return pS;
	}

// ------------------------------------------------------------------------------------------------
// CNSmlDMHostServer1::~CNSmlDMHostServer1()
//	Destructor.
// ------------------------------------------------------------------------------------------------
CNSmlDMHostServer1::~CNSmlDMHostServer1()
	{
	_DBG_FILE("CNSmlDMHostServer1::~CNSmlDMHostServer1(): begin");
	_DBG_FILE("CNSmlDMHostServer1::~CNSmlDMHostServer1(): end");
	}

// ------------------------------------------------------------------------------------------------
// CNSmlDMHostServer1::CNSmlDMHostServer1()
//	C++ constructor.
// ------------------------------------------------------------------------------------------------
CNSmlDMHostServer1::CNSmlDMHostServer1()
	{
	}

// ------------------------------------------------------------------------------------------------
// CNSmlDMHostServer1::PanicServer()
//	Panics the server.
//	This is called if the instance creation of DM Host Server 1 leaves,
//	e.g. due to call 'StartL' CServer2 -function.
//	@param aPanic. The panic reason code.
// ------------------------------------------------------------------------------------------------

void CNSmlDMHostServer1::PanicServer ( TNSmlDmHostServerPanic aPanic )
    {
	_LIT ( KTxtDmHostServer1,"NSmlDmHostServer1");
	User::Panic ( KTxtDmHostServer1, aPanic );
    }

// ------------------------------------------------------------------------------------------------
// CNSmlDMHostServer1::NewSessionL()
//	Calls CallNewSessionL.
//	@param aVersion. The version of the server.
//	@param aMessage. An object which encapsulates a client request.
//	@return CSession2*. The created new session instance.
// ------------------------------------------------------------------------------------------------
CSession2* CNSmlDMHostServer1::NewSessionL ( const TVersion& aVersion, const RMessage2& aMessage ) const
	{ 
    return CallNewSessionL ( aVersion, aMessage );
   	}

// ------------------------------------------------------------------------------------------------
// E32Main
// Entry point when the server is launched.
// ------------------------------------------------------------------------------------------------
GLDEF_C TInt E32Main()
    {
    return CNSmlDMHostServer1::LaunchServer(NULL);
    }

// End of file
