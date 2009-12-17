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
* Description:  DS Host Server
*
*/


#ifndef __NSMLDSHOSTSERVER_H__
#define __NSMLDSHOSTSERVER_H__

// INCLUDES
#include <e32base.h>

// DATA TYPES
enum TNSmlHostServerErrors
    {
    ESvrCreateServer,
    EMainSchedulerError
    };

// CLASS DECLARATION
    
// ------------------------------------------------------------------------------------------------
// class CNSmlDSHostServer
//
// @lib nsmldshostserver.lib
// ------------------------------------------------------------------------------------------------
class CNSmlDSHostServer : public CServer2
	{
	public:
		/**
		* Destructor.
		*/
		IMPORT_C ~CNSmlDSHostServer();
		
		/**
		* Increments session count.
		*/
		IMPORT_C void IncSessionCount();
		
		/**
		* Decrements session count. When count reaches zero, stops active scheduler
		*/	
		IMPORT_C void DecSessionCount();

	protected:
		/**
		* Constructor. Protected to prevent direct instantiation.
		*/
		IMPORT_C CNSmlDSHostServer();
    
   		/**
		* initializes server, calls aFunction
		* @param aServer. Server to initialize.
		* @param aFunction. Function to call after initialization.
		* @return TInt. Status code. KErrNone if method suceeded, system wide error otherwise.
		*/
		IMPORT_C static TInt InitHostServer( TNSmlHostServers aServer, TThreadFunction aFunction );

   		/**
		* calls NewSessionL.
		* @param aVersion. Version for session.
		* @param aMessage. Message associated with the request.
		* @return CSession2*. The requested session object.
		*/
		IMPORT_C CSession2* CallNewSessionL( const TVersion& aVersion, const RMessage2& aMessage ) const;
    
	private:

   		/**
		* returns the server name.
		* @param aServer. The server.
		* @return const TDesC&. The server name. 
		*/    
		static const TDesC& ServerName( TNSmlHostServers aServer );
		
   		/**
		* returns the server module name.
		* @param aServer. The server.
		* @return const TDesC&. The server module name. 
		*/
		static const TDesC& ServerModuleName( TNSmlHostServers aServer );
		
		/**
		* returns the server that runs in aThread.
		* @param aThread. The thread
		* @return TNSmlHostServers. The server
		*/
		static TNSmlHostServers ServerFromThread( const RThread &aThread );
		
		/**
		* returns session object.
		* @param aVersion. Version for session.
		* @param aMessage. Message associated with the request.
		* @return CSession2*. The requested session object.
		*/
		CSession2* NewSessionL( const TVersion& aVersion, const RMessage2& aMessage ) const;

		/**
		* starting point of a server thread.
		* @param aStarted. parameter for a thread, contains function pointer to specialized server initialization.
		* @param aServer. Server to start.
		* @return TInt. exit code from thread.
		*/
		static TInt ThreadFunction( TAny* aStarted, TNSmlHostServers aServer );

		/**
		* starts the server.
		* @param aFunction. function to call after start up
		* @param aServer. The server
		*/    
		static void HostServerStartUpL( TThreadFunction aFunction, TNSmlHostServers aServer );
    
		/**
		* Handles leave situations on startup.
		* @param aP. Pointer added to cleanupstack.
		*/    
		static void HandleLeaveOnStartup( TAny* aP );

		/**
		* Panics the server.
		* @param aPanic. Panic code.
		*/    
		static void PanicServer( TNSmlHostServerErrors aPanic );

	private:
		TInt iSessionCount;
	};

#endif // __NSMLDSHOSTSERVER_H__

// End of File
