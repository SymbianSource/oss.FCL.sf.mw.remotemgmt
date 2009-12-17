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
* Description:  SyncML Obex server internal server module
*
*/



#ifndef __NSMLOBEXCOMMSERVER_H__
#define __NSMLOBEXCOMMSERVER_H__

// ---------------------------------------------------------------------------------------
// Includes
// ---------------------------------------------------------------------------------------
#include <e32base.h>

// ---------------------------------------------------------------------------------------
// Constants
// ---------------------------------------------------------------------------------------
static const TInt KTimeOutInSeconds = 1800;

// ---------------------------------------------------------------------------------------
// Class forwards
// ---------------------------------------------------------------------------------------
class CNSmlObexCommSession;
class CNSmlTimeOut;

// ---------------------------------------------------------------------------------------
// Server
// ---------------------------------------------------------------------------------------
class CNSmlObexCommServer : public CServer2
	{
public:
	IMPORT_C static CNSmlObexCommServer* NewL( const TDesC& aServerName );
	IMPORT_C virtual ~CNSmlObexCommServer();

protected:
	virtual CSession2* NewSessionL( const TVersion& aVersion, const RMessage2& aMessage ) const;

private:
	CNSmlObexCommServer();
	void ConstructL();

	void IncrementSessions();
	void DecrementSessions();
	void Shutdown();

private:
	friend class CNSmlObexCommSession;

	RMessagePtr2 iClientReceiveMsg;
	TBool iClientWaitingReceivePacket;	
	CNSmlTimeOut* iClientReceiveTimeOut;

	RMessagePtr2 iServerGetSendMsg;
	TBool iServerWaitingSendPacket;
	CNSmlTimeOut* iServerGetSendTimeOut;

	RMessagePtr2 iServerListenDisconnectMsg;
	TBool iServerListeningDisconnect;

	TBool iClientDisconnected;
	HBufC8* iSendBuffer;
	HBufC8* iRecvBuffer;
	TBool iSendReady;
	TBool iRecvReady;

	TInt iSessionCount;
	};

// ---------------------------------------------------------------------------------------
// CNSmlObexCommServer session
// ---------------------------------------------------------------------------------------
class CNSmlObexCommSession : public CSession2
	{
public:
	static CNSmlObexCommSession* NewL( CNSmlObexCommServer& aServer );
	void ServiceL( const RMessage2& aMessage );
	void DispatchMessageL( const RMessage2& aMessage );

protected:
	// ObexClientSession
	void Disconnect( const RMessagePtr2& aMessage );
	void SendL( const RMessagePtr2& aMessage );
	void ReceiveL( const RMessagePtr2& aMessage );
	void CancelReceive( const RMessagePtr2& aMessage );
	void ReceiveTimeout();

	// ObexServerSession
	void SetReceivedPacketL( const RMessagePtr2& aMessage );
	void GetSendPacketL( const RMessagePtr2& aMessage );
	void CancelGetSendPacket( const RMessagePtr2& aMessage );
	void GetSendTimeout();
	void ListenDisconnect( const RMessagePtr2& aMessage );
	void CancelListenDisconnect( const RMessagePtr2& aMessage );

private:
	void ConstructL();
	CNSmlObexCommSession( CNSmlObexCommServer& aServer );
	~CNSmlObexCommSession();
	void PanicClient( TInt aReason ) const;

	void ClientReceivePacketL();
	void ServerGetSendPacketL();
	void ServerHandleDisconnect( TInt aResult );
	void DoCancelGetSendPacket();
private:
	CNSmlObexCommServer& iServer;
	};

// ---------------------------------------------------------------------------------------
// CNSmlTimeOut
// ---------------------------------------------------------------------------------------
typedef void (CNSmlObexCommSession::*TNSmlTimeoutCallback)();

class CNSmlTimeOut : public CActive
	{
public:
	static CNSmlTimeOut* NewL();
	~CNSmlTimeOut();
	void RunL();
	void DoCancel();
	
	void StartTimeoutHandling( CNSmlObexCommSession* aThis, TNSmlTimeoutCallback aCallback );
private:
	CNSmlTimeOut();
private:
	RTimer iTimer;
	CNSmlObexCommSession* iThis;
	TNSmlTimeoutCallback iCallback;
	};

#endif // __NSMLOBEXCOMMSERVER_H__