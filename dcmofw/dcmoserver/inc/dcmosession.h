/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  DCMO Session
*
*/

#ifndef __DCMO_SESSION_H__
#define __DCMO_SESSION_H__

// INCLUDES
#include <e32base.h>
#include "dcmoserver.h"
#include "dcmoclientserver.h"

class CDCMOServer;

/**
*  CDCMOSession 
*  Description.
*/

class CDCMOSession : public CSession2
	{

public:
	/**
	 * Createss CDCMOSession
	 * Default Constructor
	 */
	CDCMOSession();

private:
	/**
	 * Destructor
	 */
	~CDCMOSession();
	
	/**
	 * Provides DCMO Server reference
	 * @param None
	 * @return CDCMOServer reference
	 */
	CDCMOServer& Server();
	
	/**
	 * Called by the framework to handle the request 	 
	 * @param aMessage
	 * @return None
	 */
	void ServiceL           ( const RMessage2& aMessage );
	
	/**
	 * Called from ServiceL to handle the request 
	 * @param aMessage
	 * @return None
	 */
	void DispatchMessageL   ( const RMessage2& aMessage );
	
	/**
	 * Called when ServiceL leaves
	 * @param aMessage
	 * @param aError
	 * @return None
	 */
	void ServiceError       ( const RMessage2& aMessage, TInt aError );
	
	
private:	

};

#endif //__DCMO_SESSION_H__
