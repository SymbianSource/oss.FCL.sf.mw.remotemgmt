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
 


#include "SCPClientTestClientServer_AL.h"
#include "SCPClientTestClient_AL.h"
#include "debug.h"
#include <f32file.h>


TInt AL_StartServer()
	{
	RDEBUG("RSCPClientTestClient_AL: StartServer()...");	
	const TUidType serverUid(KNullUid,KNullUid,KDMSSServerUid);

	RProcess server;
	TInt r=server.Create(KDMSSServerImg,KNullDesC);

	if (r!=KErrNone)
		{
		RDEBUG_2("RSCPClientTestClient_AL: server start failed %d",r);
		return r;
		}
	TRequestStatus stat;
	server.Rendezvous(stat);
	if (stat!=KRequestPending)
		server.Kill(0);		// abort startup
	else
		server.Resume();	// logon OK - start the server
	RDEBUG("RSCPClientTestClient_AL: Started");
	User::WaitForRequest(stat);		// wait for start or death
	r=(server.ExitType()==EExitPanic) ? KErrGeneral : stat.Int();
	server.Close();
	return r;
	}

TInt RSCPClientTestClient_AL::Connect()
	{
	RDEBUG("RSCPClientTestClient_AL::Connect - start");
	TInt retry=2;
	for (;;)
		{
		TInt r=CreateSession(KDMSSServerName,TVersion(0,0,0),50);
		if (r!=KErrNotFound && r!=KErrServerTerminated)
			return r;
		if (--retry==0)
			return r;
		r=AL_StartServer();
		if (r!=KErrNone && r!=KErrAlreadyExists)
			return r;
		}
	}

void RSCPClientTestClient_AL::Close()
	{
	RSessionBase::Close();  //basecall
	}

TInt RSCPClientTestClient_AL::GetParamCallL()
{
	TInt ret(0);
	TPckgBuf<TInt> retPackage(ret);
	
	User::LeaveIfError(SendReceive(ESCPClientTestGetParam,TIpcArgs(&retPackage)));
	return retPackage();
}

TInt RSCPClientTestClient_AL::CheckConfigCallL()
{
	TInt ret(0);
	TPckgBuf<TInt> retPackage(ret);
	
	User::LeaveIfError(SendReceive(ESCPClientTestCheckConfig,TIpcArgs(&retPackage)));
	return retPackage();
}

TInt RSCPClientTestClient_AL::QueryAdminCmdCallL()
{
	TInt ret(0);
	TPckgBuf<TInt> retPackage(ret);
	
	User::LeaveIfError(SendReceive(ESCPClientTestQueryAdminCmd,TIpcArgs(&retPackage)));
	return retPackage();
}


	
