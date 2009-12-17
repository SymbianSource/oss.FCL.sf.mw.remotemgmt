/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description: definition of dm constants/exported methods
* 	This is part of remotemgmt_plat.
*
*/
 

#ifndef __SCPCLIENTTEST_SESSION_AL_H__
#define __SCPCLIENTTEST_SESSION_AL_H__

#include "SCPClientTestServer_AL.h"
#include <s32mem.h>

class CSCPClientTestSession_AL: public CSession2
{
	friend class CSCPClientTestServer_AL;
	public:
		~CSCPClientTestSession_AL();
	void CreateL();
	
	private:
		CSCPClientTestSession_AL();
		CSCPClientTestServer_AL& Server();
		
		void ServiceL(const RMessage2& aMessage);
		void DispatchMessageL(const RMessage2& aMessage);
		void ServiceError(const RMessage2 &aMessage, TInt aError);
};

#endif

//end of file