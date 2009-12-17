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
 


#include "TrustClientServer.h"
#include "TrustClient.h"
#include "debug.h"
#include <f32file.h>
#include <ssl_compatibility.h>

TInt StartServer()
	{
	RDEBUG("RTrustClient: StartServer()...");	
	const TUidType serverUid(KNullUid,KNullUid,KTrustServerUid);

	RProcess server;
	TInt r=server.Create(KTrustServerImg,KNullDesC);

	if (r!=KErrNone)
		{
		RDEBUG_2("RTrustClient: server start failed %d",r);
		return r;
		}
	TRequestStatus stat;
	server.Rendezvous(stat);
	if (stat!=KRequestPending)
		server.Kill(0);		// abort startup
	else
		server.Resume();	// logon OK - start the server
	RDEBUG("RTrustClient: Started");
	User::WaitForRequest(stat);		// wait for start or death
	r=(server.ExitType()==EExitPanic) ? KErrGeneral : stat.Int();
	server.Close();
	return r;
	}

EXPORT_C TInt RTrustClient::Connect()
	{
	RDEBUG("RTrustClient::Connect - start");
	TInt retry=2;
	for (;;)
		{
		TInt r=CreateSession(KTrustServerName,TVersion(0,0,0),50);
		if (r!=KErrNotFound && r!=KErrServerTerminated)
			return r;
		if (--retry==0)
			return r;
		r=StartServer();
		if (r!=KErrNone && r!=KErrAlreadyExists)
			return r;
		}
	}


EXPORT_C void RTrustClient::Close()
	{
	RSessionBase::Close();  //basecall
	}
	
	
EXPORT_C TInt RTrustClient::AddSessionTrustL(TCertInfo& aCertInfo)
{
	TInt result(0);
	TPckgBuf<TInt> resultpkg(result);
	TPckgBuf<TCertInfo> certInfoPck( aCertInfo);
	User::LeaveIfError(SendReceive(EAddTrust,TIpcArgs(&resultpkg,&certInfoPck)));
	result = resultpkg();
	return result;
}

EXPORT_C TInt RTrustClient::GetElementListL(TElementType& aElementType)
{
	TInt result(0);
	TPckgBuf<TInt> resultpkg(result);
	TPckgBuf<TInt> EleTypePkg(aElementType);
	User::LeaveIfError(SendReceive(EGetElementList,TIpcArgs(&resultpkg,&EleTypePkg)));
	result = resultpkg();
	return result;
}


EXPORT_C TInt RTrustClient::GetElement(TDesC8& aElementInfo)
{
	TInt err = SendReceive(EGetElement,TIpcArgs(&aElementInfo));
	return err;
}

EXPORT_C TInt RTrustClient::GetXACMLDescription(TDesC8& aElementInfo)
{
	TInt err = SendReceive(EGetXACMLDescription,TIpcArgs(&aElementInfo));
	return err;
}

