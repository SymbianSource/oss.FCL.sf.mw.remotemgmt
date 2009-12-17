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
 

#include <mmf/common/mmfcontrollerpluginresolver.h>
#include "TempSession.h"
#include "TempClientServer.h"
#include <badesca.h>
#include "debug.h"
#include <e32cmn.h>
#include <f32file.h>

CTempSession::CTempSession()
	{
	RDEBUG("CTempSession::CTempSession");
	}


CTempSession::~CTempSession()
	{
	RDEBUG("CTempSession::~CTempSession");
	Server().DropSession();
	}

CTempServer& CTempSession::Server()
	{
	return *static_cast<CTempServer*>(const_cast<CServer2*>(CSession2::Server()));
	}

// ----------------------------------------------------------------------------------------
// CTempSession::CreateL
// 2nd phase construct for sessions - called by the CServer framework
// ----------------------------------------------------------------------------------------
void CTempSession::CreateL()
	{
	RDEBUG("CTempSession::CreateL");
	Server().AddSession();
	
	}


void CTempSession::ServiceL(const RMessage2& aMessage)
	{
	TRAPD(err,DispatchMessageL(aMessage));
	aMessage.Complete(err);
	}

void CTempSession::ServiceError(const RMessage2& aMessage,TInt aError)
	{
	RDEBUG_2("CTempSession::ServiceError %d",aError);
	CSession2::ServiceError(aMessage,aError);
	}
	

void CTempSession::DispatchMessageL(const RMessage2& aMessage)
	{
	
	switch(aMessage.Function())
		{
		case EPerformRFS:
		{
			TInt err = Server().PerformRFS();
			TPckg<TInt> result(err);
			aMessage.WriteL(0,result);
		}
		break;
   
		default:
			break;
		}
	}
