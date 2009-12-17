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
 


#include "TempClientServer.h"
#include "TempClient.h"
#include "debug.h"
#include <f32file.h>


TInt StartServer()
	{
	RDEBUG("RTempClient: StartServer()...");	
	const TUidType serverUid(KNullUid,KNullUid,KPMRFSServerUid);

	RProcess server;
	TInt r=server.Create(KTempServerImg,KNullDesC);

	if (r!=KErrNone)
		{
		RDEBUG_2("RTempClient: server start failed %d",r);
		return r;
		}
	TRequestStatus stat;
	server.Rendezvous(stat);
	if (stat!=KRequestPending)
		server.Kill(0);		// abort startup
	else
		server.Resume();	// logon OK - start the server
	RDEBUG("RTempClient: Started");
	User::WaitForRequest(stat);		// wait for start or death
	r=(server.ExitType()==EExitPanic) ? KErrGeneral : stat.Int();
	server.Close();
	return r;
	}


EXPORT_C TInt RTempClient::Connect()
	{
	RDEBUG("RTempClient::Connect - start");
	TInt retry=2;
	for (;;)
		{
		TInt r=CreateSession(KTempServerName,TVersion(0,0,0),50);
		if (r!=KErrNotFound && r!=KErrServerTerminated)
			return r;
		if (--retry==0)
			return r;
		r=StartServer();
		if (r!=KErrNone && r!=KErrAlreadyExists)
			return r;
		}
	}


EXPORT_C void RTempClient::Close()
	{
	RSessionBase::Close();  //basecall
	}
	
	
EXPORT_C void RTempClient::PerformRFSL()
{
	TInt result(0);
	TPckg<TInt> resultpkg(result);
	User::LeaveIfError(SendReceive(EPerformRFS,TIpcArgs(&resultpkg)));
	
}
