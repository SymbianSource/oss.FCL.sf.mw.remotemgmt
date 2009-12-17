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
 

#ifndef __DMCERTAPI_SESSION_H__
#define __DMCERTAPI_SESSION_H__

#include "dmcertapiServer.h"
#include <s32mem.h>
#include <ssl.h>


class CdmcertapiSession: public CSession2
{
	friend class CdmcertapiServer;
	public:
		~CdmcertapiSession();
	void CreateL();
	
	private:
		CdmcertapiSession();
		CdmcertapiServer& Server();
		
		void ServiceL(const RMessage2& aMessage);
		void DispatchMessageL(const RMessage2& aMessage);
		void ServiceError(const RMessage2 &aMessage, TInt aError);
		void CreateCertificate( TCertInfo& aCertInfo );
};


#endif

// end of file