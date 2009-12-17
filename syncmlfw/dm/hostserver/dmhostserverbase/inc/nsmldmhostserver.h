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
* Description:  Contains common part of server code. Dedicated DM HostServers 
*				 (1-4) are inherited from this class.
*
*/


#ifndef __NSMLDMHOSTSERVER_H__
#define __NSMLDMHOSTSERVER_H__

// ------------------------------------------------------------------------------------------------
// Includes
// ------------------------------------------------------------------------------------------------
#include <e32base.h>


/**
* Class CNSmlDMHostServer. Contains common part of server code. Dedicated DM HostServers 
* (1-4) are inherited from this class
*
* @lib nsmldmhostserver.lib
*/

class CNSmlDMHostServer : public CServer2
	{
public:
	enum TNSmlDmHostServerPanic
    	{
    	ESvrCreateServer,
    	EMainSchedulerError
    	};
	/**
	* Destructor.
	*/
	IMPORT_C ~CNSmlDMHostServer();

	/**
	* Increments session count.
	*/
	void IncSessionCount();
	
	/**
	* Decrements session count. When count reaches zero, stops active scheduler
	*/	
	void DecSessionCount();
protected:
	/**
	* Constructor. Protected to prevent direct instantiation.
	*/
    IMPORT_C CNSmlDMHostServer();

   	/**
	* initializes server, calls aFunction
	* @param aServerName. Server to initialize.
	* @param aFunction. Function to call after initialization.
	* @return TInt. Status code. KErrNone if method suceeded, system wide error otherwise.
	*/    
    IMPORT_C static TInt InitHostServer ( const TDesC& aServerName, TThreadFunction aFunction );

   	/**
	* calls NewSessionL.
	* @param aVersion. Version for session.
	* @param aMessage. Message associated with the request.
	* @return CSession2*. The requested session object.
	*/
    IMPORT_C CSession2* CallNewSessionL ( const TVersion& aVersion, const RMessage2& aMessage ) const;
private:
	/**
	* returns session object.
	* @param aVersion. Version for session.
	* @param aMessage. Message associated with the request.
	* @return CSession2*. The requested session object.
	*/
    CSession2* NewSessionL ( const TVersion& aVersion, const RMessage2& aMessage ) const;
	
	/**
	* starting point of a server thread, when in emulator.
	* @param aStarted. parameter for a thread, contains function pointer to specialized server initialization.
	* @return TInt. exit code from thread.
	*/
    static TInt ThreadFunction ( TAny* aStarted );

	/**
	* Panics the server.
	* @param aPanic. Panic code.
	*/    
    static void PanicServer ( TNSmlDmHostServerPanic aPanic );
    
	/**
	* Server exe name.
	* @return TPtrC. Server exe name concluded from the current thread name.
	*/        
    static TPtrC ServerName();

private:
	TInt iSessionCount;
	};

#endif // __NSMLDMHOSTSERVER_H__
