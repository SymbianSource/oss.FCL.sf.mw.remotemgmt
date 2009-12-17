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
#include "nsmldmhostserver4.h"

// ------------------------------------------------------------------------------------------------
// CNSmlDMHostServer4::LaunchServer()
//	Launches the server.
//	Calls the common code part (InitHostServer) of all specialized DM Host Servers.
//	@param (TAny*). Not used in this function. 
//	  			    Kept here for possible future needs.
//	@return TInt. Return value from InitHostServer function.
// ------------------------------------------------------------------------------------------------
TInt CNSmlDMHostServer4::LaunchServer ( TAny* /*aStarted*/ )
    {
    return InitHostServer ( KNSmlDmHostServer4Name(), (TThreadFunction) ThreadFunction );
    }

// ------------------------------------------------------------------------------------------------
// CNSmlDMHostServer4::ThreadFunction()
//	Calls the NewL for creating the DM Host Server 4 instance.
//	This ThreadFunction is called when the 'common initialization actions' 
//	are successfully run.
//	Starts the active scheduler.
//	@param aStarted. Not used.
//	@return TInt. Returns always KErrNone.
// ------------------------------------------------------------------------------------------------
TInt CNSmlDMHostServer4::ThreadFunction ( TAny* /*aStarted*/ )
    {
	CNSmlDMHostServer4* hs = NULL;
    TRAPD ( err, ( hs = CNSmlDMHostServer4::NewL ( KNSmlDmHostServer4Name() ) ) );
    RProcess::Rendezvous( err );
	__ASSERT_ALWAYS ( !err, PanicServer ( ESvrCreateServer ) );    
    CActiveScheduler::Start();
    delete hs;
    return KErrNone;
    }

// ------------------------------------------------------------------------------------------------
// CNSmlDMHostServer4::NewL()
//	Creates a new instance of DM Host Server 4.
//	Adds the server with the specified name to the active scheduler, 
//	and issues the first request for messages. 
//	Signals the global semaphore, which releases the client (DM Tree Module)
//	from the waiting state.
//	@param aServerName. The name of the server: "nsmldmhostserver4".
//	@return CNSmlDMHostServer4*. The created server instance.
// ------------------------------------------------------------------------------------------------
CNSmlDMHostServer4* CNSmlDMHostServer4::NewL ( const TDesC& aServerName )
	{
	CNSmlDMHostServer4* pS = new (ELeave) CNSmlDMHostServer4();
	CleanupStack::PushL ( pS );
	pS->StartL ( aServerName );
	CleanupStack::Pop(); // pS
	return pS;
	}

// ------------------------------------------------------------------------------------------------
// CNSmlDMHostServer4::~CNSmlDMHostServer4()
//	Destructor.
// ------------------------------------------------------------------------------------------------
CNSmlDMHostServer4::~CNSmlDMHostServer4()
	{
	_DBG_FILE("CNSmlDMHostServer4::~CNSmlDMHostServer4(): begin");
	_DBG_FILE("CNSmlDMHostServer4::~CNSmlDMHostServer4(): end");
	}

// ------------------------------------------------------------------------------------------------
// CNSmlDMHostServer4::CNSmlDMHostServer4()
//	C++ constructor.
// ------------------------------------------------------------------------------------------------
CNSmlDMHostServer4::CNSmlDMHostServer4()
	{
	}

// ------------------------------------------------------------------------------------------------
// CNSmlDMHostServer4::PanicServer()
//	Panics the server.
//	This is called if the instance creation of DM Host Server 4 leaves,
//	e.g. due to call 'StartL' CServer2 -function.
//	@param aPanic. The panic reason code.
// ------------------------------------------------------------------------------------------------
void CNSmlDMHostServer4::PanicServer ( TNSmlDmHostServerPanic aPanic )
    {
	_LIT ( KTxtDmHostServer4,"NSmlDmHostServer4");
	User::Panic ( KTxtDmHostServer4, aPanic );
    }

// ------------------------------------------------------------------------------------------------
// CNSmlDMHostServer4::NewSessionL()
//	Calls CallNewSessionL.
//	@param aVersion. The version of the server.
//	@param aMessage. An object which encapsulates a client request.
//	@return CSession2*. The created new session instance.
// ------------------------------------------------------------------------------------------------
CSession2* CNSmlDMHostServer4::NewSessionL ( const TVersion& aVersion, const RMessage2& aMessage ) const
	{ 
    return CallNewSessionL ( aVersion, aMessage );
   	}

// ------------------------------------------------------------------------------------------------
// E32Main
// Entry point when the server is launched.
// ------------------------------------------------------------------------------------------------
GLDEF_C TInt E32Main()
    {
    return CNSmlDMHostServer4::LaunchServer(NULL);
    }

// End of file
