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


#ifndef __NSMLDMHOSTSERVER2_H__
#define __NSMLDMHOSTSERVER2_H__

// ------------------------------------------------------------------------------------------------
// Includes
// ------------------------------------------------------------------------------------------------
#include <e32base.h>
#include "nsmldmhostserver.h"

/**
* Class CNSmlDMHostServer2.
*
* @lib In order to use this class, launch nsmldmhostserver2 (.exe)
*/
class CNSmlDMHostServer2 : public CNSmlDMHostServer
	{
public:

   	/**
	* Launches the server.
	*  Calls the common code part (InitHostServer) of all specialized DM Host Servers.
	* @param (TAny*). Not used in this function. 
	*				  Kept here for possible future needs.
	* @return TInt. Return value from InitHostServer function.
	*/
    static TInt LaunchServer ( TAny* );

   	/**
	* Calls the NewL for creating the DM Host Server 2 instance.
	*  This ThreadFunction is called when the 'common initialization actions' 
	*  are successfully run.
	*  Launches the active scheduler.
	* @param aStarted. Not used.
	* @return TInt. Returns always KErrNone.
	*/
    static TInt ThreadFunction ( TAny* aStarted );

   	/**
	* Creates a new instance of DM Host Server 2.
	*  Adds the server with the specified name to the active scheduler, 
	*  and issues the first request for messages. 
	*  Signals the global semaphore, which releases the client (DM Tree Module)
	*  from the waiting state.
	* @param aServerName. The name of the server: "nsmldmhostserver2".
	* @return CNSmlDMHostServer2*. The created server instance.
	*/
	static CNSmlDMHostServer2* NewL ( const TDesC& aServerName);

	/**
	* Destructor.
	*/
	~CNSmlDMHostServer2();
private:

	/**
	* C++ constructor.
	*/
    CNSmlDMHostServer2();

   	/**
	* Panics the server.
	*  This is called if the instance creation of DM Host Server 2 leaves,
	*  e.g. due to call 'StartL' CServer2 -function.
	* @param aPanic. The panic reason code.
	*/
    static void PanicServer ( TNSmlDmHostServerPanic aPanic );

   	/**
	* Calls CallNewSessionL.
	* @param aVersion. The version of the server.
	* @param aMessage. An object which encapsulates a client request.
	* @return CSession2*. The created new session instance.
	*/
    CSession2* NewSessionL ( const TVersion& aVersion, const RMessage2& aMessage ) const;
	};

#endif // __NSMLDMHOSTSERVER2_H__
