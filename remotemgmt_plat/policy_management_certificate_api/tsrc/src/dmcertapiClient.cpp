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
 


#include "dmcertapiClientServer.h"
#include "dmcertapiClient.h"
#include "debug.h"
#include <f32file.h>
#include "dmcert.h"

//const TUid KCRUidPolicyManagementUtilInternalKeys = {0x10207843};
//const TUint32 KSyncMLSessionCertificate = 0x00000001;

TInt StartServer()
	{
	RDEBUG("RdmcertapiClient: StartServer()...");	
	const TUidType serverUid(KNullUid,KNullUid,KDMSSServerUid);

	RProcess server;
	TInt r=server.Create(KDMSSServerImg,KNullDesC);

	if (r!=KErrNone)
		{
		RDEBUG_2("RdmcertapiClient: server start failed %d",r);
		return r;
		}
	TRequestStatus stat;
	server.Rendezvous(stat);
	if (stat!=KRequestPending)
		server.Kill(0);		// abort startup
	else
		server.Resume();	// logon OK - start the server
	RDEBUG("RdmcertapiClient: Started");
	User::WaitForRequest(stat);		// wait for start or death
	r=(server.ExitType()==EExitPanic) ? KErrGeneral : stat.Int();
	server.Close();
	return r;
	}

TInt RdmcertapiClient::Connect()
	{
	RDEBUG("RdmcertapiClient::Connect - start");
	TInt retry=2;
	for (;;)
		{
		TInt r=CreateSession(KDMSSServerName,TVersion(0,0,0),50);
		if (r!=KErrNotFound && r!=KErrServerTerminated)
			return r;
		if (--retry==0)
			return r;
		r=StartServer();
		if (r!=KErrNone && r!=KErrAlreadyExists)
			return r;
		}
	}


void RdmcertapiClient::Close()
	{
	RSessionBase::Close();  //basecall
	}


TInt RdmcertapiClient::GetCertificateL()
{
	TInt ret(0);
	TPckgBuf<TInt> retPackage(ret);
	
	User::LeaveIfError(SendReceive(EGetCertificate,TIpcArgs(&retPackage)));
	return retPackage();
}


	
