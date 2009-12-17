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
* Description:  DS Host Server 1
*
*/


#ifndef __NSMLHOSTSERVER1_H__
#define __NSMLHOSTSERVER1_H__

// INCLUDES
#include <e32base.h>
#include "Nsmldshostserver.h"

// CLASS DECLARATION

/**
* class CNSmlDSHostServer1
*
* @lib nsmldshostserver1.lib
*/
class CNSmlDSHostServer1 : public CNSmlDSHostServer
	{
	public: // constructors and destructor

		/**
		* Two-phased constructor.
		* @param aServerName. The server's name.
		* @return created server instance.
		*/
		static CNSmlDSHostServer1* NewL( const TDesC& aServerName);

		/**
		* Destructor
		*/
		~CNSmlDSHostServer1();

	public: // new functions

   		/**
		* Launches the server.
		*/
		static TInt LaunchServer( TAny* );
		
		/**
		* Thread function where the active scheduler runs.
		* @param aStarted. Not used.
		* @return. KErrNone.
		*/
		static TInt ThreadFunctionL(TAny* aStarted);

   		/**
		* Panics the server
		* @param aPanic. Panic reason.
		*/
		static void PanicServer( TNSmlHostServerErrors aPanic );

	protected:

		/**
		* Constructor
		*/
		CNSmlDSHostServer1();

	private: // functions from base classes

		/**
		* Returns session for the server.
		* @param aVersion. Version for session
		* @param aMessage. Message to identify the client.
		*/
		CSession2* NewSessionL( const TVersion& aVersion, const RMessage2& aMessage ) const;
	};

#endif // __NSMLHOSTSERVER1_H__

// End of File

