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
 


#include "SCPClientTestClientServer.h"
#include "SCPClientTestClient.h"
#include "debug.h"
#include <f32file.h>


TInt StartServer()
	{
	RDEBUG("RSCPClientTestClient: StartServer()...");	
	const TUidType serverUid(KNullUid,KNullUid,KDMSSServerUid);

	RProcess server;
	TInt r=server.Create(KDMSSServerImg,KNullDesC);

	if (r!=KErrNone)
		{
		RDEBUG_2("RSCPClientTestClient: server start failed %d",r);
		return r;
		}
	TRequestStatus stat;
	server.Rendezvous(stat);
	if (stat!=KRequestPending)
		server.Kill(0);		// abort startup
	else
		server.Resume();	// logon OK - start the server
	RDEBUG("RSCPClientTestClient: Started");
	User::WaitForRequest(stat);		// wait for start or death
	r=(server.ExitType()==EExitPanic) ? KErrGeneral : stat.Int();
	server.Close();
	return r;
	}

TInt RSCPClientTestClient::Connect()
	{
	RDEBUG("RSCPClientTestClient::Connect - start");
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


void RSCPClientTestClient::Close()
	{
	RSessionBase::Close();  //basecall
	}


TInt RSCPClientTestClient::ConstructorCallL()
{
	TInt ret(0);
	TPckgBuf<TInt> retPackage(ret);
	
	User::LeaveIfError(SendReceive(ESCPClientTestConstructor,TIpcArgs(&retPackage)));
	return retPackage();
}

TInt RSCPClientTestClient::ConnectCallL()
{
	TInt ret(0);
	TPckgBuf<TInt> retPackage(ret);
	
	User::LeaveIfError(SendReceive(ESCPClientTestConnect,TIpcArgs(&retPackage)));
	return retPackage();
}

TInt RSCPClientTestClient::VersionCallL()
{
	TInt ret(0);
	TPckgBuf<TInt> retPackage(ret);
	
	User::LeaveIfError(SendReceive(ESCPClientTestVersion,TIpcArgs(&retPackage)));
	return retPackage();
}

TInt RSCPClientTestClient::GetCodeCallL()
{
	TInt ret(0);
	TPckgBuf<TInt> retPackage(ret);
	
	User::LeaveIfError(SendReceive(ESCPClientTestGetCode,TIpcArgs(&retPackage)));
	return retPackage();
}

TInt RSCPClientTestClient::SetCodeCallL()
{
	TInt ret(0);
	TPckgBuf<TInt> retPackage(ret);
	
	User::LeaveIfError(SendReceive(ESCPClientTestSetCode,TIpcArgs(&retPackage)));
	return retPackage();
}
	
TInt RSCPClientTestClient::ChangeCodeCallL()
{
	TInt ret(0);
	TPckgBuf<TInt> retPackage(ret);
	
	User::LeaveIfError(SendReceive(ESCPClientTestChangeCode,TIpcArgs(&retPackage)));
	return retPackage();
}

TInt RSCPClientTestClient::SetPhoneLockCallL()
{
	TInt ret(0);
	TPckgBuf<TInt> retPackage(ret);
	
	User::LeaveIfError(SendReceive(ESCPClientTestSetPhoneLock,TIpcArgs(&retPackage)));
	return retPackage();
}

TInt RSCPClientTestClient::GetLockStateCallL()
{
	TInt ret(0);
	TPckgBuf<TInt> retPackage(ret);
	
	User::LeaveIfError(SendReceive(ESCPClientTestGetLockState,TIpcArgs(&retPackage)));
	return retPackage();
}

TInt RSCPClientTestClient::QueryAdminCmdCallL()
{
	TInt ret(0);
	TPckgBuf<TInt> retPackage(ret);
	
	User::LeaveIfError(SendReceive(ESCPClientTestQueryAdminCmd,TIpcArgs(&retPackage)));
	return retPackage();
}

TInt RSCPClientTestClient::SetParamCallL()
{
	TInt ret(0);
	TPckgBuf<TInt> retPackage(ret);
	
	User::LeaveIfError(SendReceive(ESCPClientTestSetParam,TIpcArgs(&retPackage)));
	return retPackage();
}

TInt RSCPClientTestClient::GetParamCallL()
{
	TInt ret(0);
	TPckgBuf<TInt> retPackage(ret);
	
	User::LeaveIfError(SendReceive(ESCPClientTestGetParam,TIpcArgs(&retPackage)));
	return retPackage();
}

TInt RSCPClientTestClient::AuthenticateS60CallL()
{
	TInt ret(0);
	TPckgBuf<TInt> retPackage(ret);
	
	User::LeaveIfError(SendReceive(ESCPClientTestAuthenticateS60,TIpcArgs(&retPackage)));
	return retPackage();
}

TInt RSCPClientTestClient::CodeChangeQueryCallL()
{
	TInt ret(0);
	TPckgBuf<TInt> retPackage(ret);
	
	User::LeaveIfError(SendReceive(ESCPClientTestCodeChangeQuery,TIpcArgs(&retPackage)));
	return retPackage();
}

/*TInt RSCPClientTestClient::CheckConfigCall()
{
	TInt ret(0);
	TPckgBuf<TInt> retPackage(ret);
	
	User::LeaveIfError(SendReceive(ESCPClientTestCheckConfig,TIpcArgs(&retPackage)));
	return retPackage();
}
	
TInt RSCPClientTestClient::testcall()
{
	TInt ssCount(0);
	TPckgBuf<TInt>count(ssCount);
	
	User::LeaveIfError(SendReceive(ESCPClientTestConstructor,TIpcArgs(&count)));
	return count();
	
}*/

TInt RSCPClientTestClient::PerformCleanUpL(RArray<TUid>& aUids)
{
    TInt ret(0);
    TPckgBuf<TInt> retPackage(ret);
    
    // Fill the aUids to TPckgBuf or TIpcArgs & send
    /*
     *   TPtr8 p( reinterpret_cast< TUint8* >( &aArray[0] ), 0, count * sizeof( TUint32 ) );
     *     err = SendReceive( ESCPClientTestPerformCleanUp, TIpcArgs( &p, &pk, &retPackage ) ) ;
     */
//    User::LeaveIfError(SendReceive(ESCPClientTestPerformCleanUp,TIpcArgs(&retPackage)));
    /*
    TInt count = aUids.Count();
    TPtr8 p( reinterpret_cast< TUint8* >( &aUids[0] ), 0, count * sizeof( TUint32 ) );
    User::LeaveIfError(SendReceive( ESCPClientTestPerformCleanUp, TIpcArgs( &p, &retPackage ) ) );
    */

    TInt lCount = aUids.Count();
    if(lCount < 1) {
        return KErrNone;  
    }
    HBufC8* lBuff = HBufC8 :: NewLC(lCount * sizeof(TInt32));
    TPtr8 lBufPtr = lBuff->Des();
    RDesWriteStream lWriteStream(lBufPtr);
    CleanupClosePushL(lWriteStream);
    for(TInt i=0; i < lCount; i++) {
        lWriteStream.WriteInt32L(aUids[i].iUid);
    }
    lWriteStream.CommitL();
    TInt lStatus = SendReceive(ESCPClientTestPerformCleanUp, TIpcArgs(ESCPClientTestPerformCleanUp, &lBuff->Des(), &retPackage));
    CleanupStack :: PopAndDestroy(2); // lBuff, lWriteStream
    return retPackage();
}


	
