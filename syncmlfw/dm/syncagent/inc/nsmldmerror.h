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
* Description:  error code conversion
*
*/




#ifndef __NSMLDMERROR_H
#define __NSMLDMERROR_H

#include <e32base.h>
#include "nsmlerror.h"


class TNSmlDMError:public TNSmlError
	{
	public:  //Data types 
	//
    //Error coding
	enum TNSmlDMAgentErrorCode
		{
		ESmlServerSessionAbort = 3000
		};
	
	// Errors and Info messages, which are delivered to sync log.
	enum TNSmlDMAgentMessage    
		{		
		ESystemError = 1,                  // "System Error"
		EServerSystemError,				   // "Error in remote conf server"
		ESyncMLError,					   // "Unable to configure"
		ESyncMLCommandNotSupported,         
		ESyncMLVersionNotSupported,        // "Unsupported SyncML Software version in server"
		EUnsupportedContentTypeOrFormat,   // "Unsupported Content type"
		ESyncMLClientAuthenticationError,  // "Invalid User name or password"
		ESyncMLServerAuthenticationError,  // "Authentication failed, check server ID and password"
		EServerBusy,                       // "Remote conf server was busy" 
		EServerNotResponding,			   // "Server is not responding"	
		EInvalidURI,					   // "Invalid URI"	
		ECommunicationError,               // "Error in communication"  
		ESessionAborted,                   // "Session was aborted by remote conf server"  
        ENotPossibleInOfflineMode          // CHANGES: Offline mode
		};
	
	public: // Constructor
	TNSmlDMError();
	private: // functions from base classes
	// Sync Log Message code 
	TInt SyncLogMessageCode(); //from TNSmlError
	void ErrorCodeConversion(); //from TNSmlError
	// Private variables
	};

#endif // __NSMLDMERROR_H

