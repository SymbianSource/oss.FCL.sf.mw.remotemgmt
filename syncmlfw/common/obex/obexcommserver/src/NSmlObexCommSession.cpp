/*
* Copyright (c) 2002-2004 Nokia Corporation and/or its subsidiary(-ies). 
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


// INCLUDE FILES
#include <nsmldebug.h>
#include "nsmlobexcommserver.h"
#include "nsmlobexdefs.h"


// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CNSmlObexCommSession::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CNSmlObexCommSession* CNSmlObexCommSession::NewL( CNSmlObexCommServer& aServer )
	{
	CNSmlObexCommSession* self = new (ELeave) CNSmlObexCommSession( aServer);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(); // self
	return self;
	}

// -----------------------------------------------------------------------------
// CNSmlObexCommSession::ServiceL
// -----------------------------------------------------------------------------
//
void CNSmlObexCommSession::ServiceL( const RMessage2& aMessage )
	{
	_DBG_FILE("CNSmlObexCommSession::ServiceL(): begin");
	TRAP_IGNORE(DispatchMessageL(aMessage));
	_DBG_FILE("CNSmlObexCommSession::ServiceL(): end");
	}

// -----------------------------------------------------------------------------
// CNSmlObexCommSession::DispatchMessageL
// -----------------------------------------------------------------------------
//
void CNSmlObexCommSession::DispatchMessageL( const RMessage2& aMessage )
	{
	_DBG_FILE("CNSmlObexCommSession::DispatchMessageL(): begin");
	switch( aMessage.Function() )
        {
	case ENSmlObexCommCommandDisconnect:
		Disconnect(aMessage);
		break;
	case ENSmlObexCommCommandSend:
		SendL(aMessage);
		break;
	case ENSmlObexCommCommandReceive:
		ReceiveL(aMessage);
		break;
	case ENSmlObexCommCommandCancelReceive:
		CancelReceive(aMessage);
		break;
	case ENSmlObexCommCommandSetReceivePacket:
		SetReceivedPacketL(aMessage);
		break;
	case ENSmlObexCommCommandGetSendPacket:
		GetSendPacketL(aMessage);
		break;
	case ENSmlObexCommCommandCancelGetSendPacket:
		CancelGetSendPacket(aMessage);
		break;
	case ENSmlObexCommCommandListenDisconnect:
		ListenDisconnect(aMessage);
		break;
	case ENSmlObexCommCommandCancelListenDisconnect:
		CancelListenDisconnect(aMessage);
		break;
	default:
		PanicClient(KErrNotSupported);
        }
	_DBG_FILE("CNSmlObexCommSession::DispatchMessageL(): end");
	}


// -----------------------------------------------------------------------------
// CNSmlObexCommSession::Disconnect
// -----------------------------------------------------------------------------
//
void CNSmlObexCommSession::Disconnect( const RMessagePtr2& aMessage )
	{
	_DBG_FILE("CNSmlObexCommSession::Disconnect(): begin");
	iServer.iClientReceiveTimeOut->Cancel();
	iServer.iServerGetSendTimeOut->Cancel();
	iServer.iClientDisconnected = ETrue;
	if( iServer.iServerListeningDisconnect )
		{
		ServerHandleDisconnect(KErrNone);
		}
	aMessage.Complete(KErrNone);
	_DBG_FILE("CNSmlObexCommSession::Disconnect(): end");
	}


// -----------------------------------------------------------------------------
// CNSmlObexCommSession::SendL
// -----------------------------------------------------------------------------
//
void CNSmlObexCommSession::SendL( const RMessagePtr2& aMessage )
	{
	_DBG_FILE("CNSmlObexCommSession::SendL(): begin");

	HBufC8* tempPtr = NULL;
	TRAP_IGNORE( tempPtr = HBufC8::NewL(aMessage.GetDesLength(0)) );
	iServer.iSendBuffer = tempPtr;
	
	TPtr8 sendBufferPtr = iServer.iSendBuffer->Des();
	aMessage.ReadL(0, sendBufferPtr, 0);
	if( iServer.iServerWaitingSendPacket )
		{
		ServerGetSendPacketL();
		}
	else
		{
		iServer.iSendReady = ETrue;
		}
	aMessage.Complete(KErrNone);
	_DBG_FILE("CNSmlObexCommSession::SendL(): end");
	}

// -----------------------------------------------------------------------------
// CNSmlObexCommSession::ReceiveL
// -----------------------------------------------------------------------------
//
void CNSmlObexCommSession::ReceiveL( const RMessagePtr2& aMessage )
	{
	_DBG_FILE("CNSmlObexCommSession::ReceiveL(): begin");
	iServer.iClientReceiveTimeOut->StartTimeoutHandling(this, &CNSmlObexCommSession::ReceiveTimeout);
	iServer.iClientReceiveMsg = aMessage;

	if( iServer.iClientDisconnected )
		{
		DBG_FILE_CODE(iServer.iSessionCount, _S8("CNSmlObexCommSession::Receive(): canceling"));
		iServer.iClientReceiveMsg.Complete( KErrCancel );
		}
	else if( iServer.iRecvReady )
		{
		_DBG_FILE("CNSmlObexCommSession before ClientReceivePacketL()");
		ClientReceivePacketL();
		}
	else
		{
		_DBG_FILE("CNSmlObexCommSession:: updating iClientWaitingReceivePacket");
		iServer.iClientWaitingReceivePacket = ETrue;
		}
	_DBG_FILE("CNSmlObexCommSession::ReceiveL(): end");
	}

// -----------------------------------------------------------------------------
// CNSmlObexCommSession::CancelReceive
// -----------------------------------------------------------------------------
//
void CNSmlObexCommSession::CancelReceive( const RMessagePtr2& aMessage )
	{
	_DBG_FILE("CNSmlObexCommSession::CancelReceive(): begin");
	if( iServer.iClientWaitingReceivePacket )
		{
		iServer.iClientReceiveMsg.Complete(KErrCancel);
		iServer.iClientWaitingReceivePacket = EFalse;
		}
	aMessage.Complete(KErrNone);
	_DBG_FILE("CNSmlObexCommSession::CancelReceive(): end");
	}

// -----------------------------------------------------------------------------
// CNSmlObexCommSession::ReceiveTimeout
// -----------------------------------------------------------------------------
//
void CNSmlObexCommSession::ReceiveTimeout()
	{
	_DBG_FILE("CNSmlObexCommSession::ReceiveTimeout(): begin");
	iServer.iClientWaitingReceivePacket = EFalse;
	iServer.iClientReceiveMsg.Complete(KErrTimedOut);
	_DBG_FILE("CNSmlObexCommSession::ReceiveTimeout(): end");
	}

// -----------------------------------------------------------------------------
// CNSmlObexCommSession::SetReceivedPacketL
// -----------------------------------------------------------------------------
//
void CNSmlObexCommSession::SetReceivedPacketL( const RMessagePtr2& aMessage )
	{
	_DBG_FILE("CNSmlObexCommSession::SetReceivedPacketL(): begin");
	TInt err(0);
	HBufC8* tempPtr = NULL;
	TRAP( err,  tempPtr = HBufC8::NewL(aMessage.GetDesLength(0)) );
	iServer.iRecvBuffer = tempPtr;
	TPtr8 recvBufferPtr = iServer.iRecvBuffer->Des();
	aMessage.ReadL(0, recvBufferPtr, 0);
	
	if( iServer.iClientWaitingReceivePacket )
		{
		ClientReceivePacketL();
		}
	else
		{
		iServer.iRecvReady = ETrue;
		}
	aMessage.Complete(KErrNone);
	_DBG_FILE("CNSmlObexCommSession::SetReceivedPacketL(): end");
	}
	
// -----------------------------------------------------------------------------
// CNSmlObexCommSession::GetSendPacketL
// -----------------------------------------------------------------------------
//
void CNSmlObexCommSession::GetSendPacketL( const RMessagePtr2& aMessage )
	{
	_DBG_FILE("CNSmlObexCommSession::GetSendPacketL(): begin");
	iServer.iServerGetSendTimeOut->StartTimeoutHandling(this, &CNSmlObexCommSession::GetSendTimeout);
	iServer.iServerGetSendMsg = aMessage;
	if( iServer.iSendReady )
		{
		ServerGetSendPacketL();
		}
	else
		{
		iServer.iServerWaitingSendPacket = ETrue;
		}
	_DBG_FILE("CNSmlObexCommSession::GetSendPacketL(): end");
	}

// -----------------------------------------------------------------------------
// CNSmlObexCommSession::CancelGetSendPacket
// -----------------------------------------------------------------------------
//
void CNSmlObexCommSession::CancelGetSendPacket( const RMessagePtr2& aMessage )
	{
	_DBG_FILE("CNSmlObexCommSession::CancelGetSendPacket(): begin");
	DoCancelGetSendPacket();
	aMessage.Complete(KErrNone);
	_DBG_FILE("CNSmlObexCommSession::CancelGetSendPacket(): end");
	}

// -----------------------------------------------------------------------------
// CNSmlObexCommSession::DoCancelGetSendPacket
// -----------------------------------------------------------------------------
//
void CNSmlObexCommSession::DoCancelGetSendPacket()
	{
	_DBG_FILE("CNSmlObexCommSession::DoCancelGetSendPacket(): begin");
	if( iServer.iServerWaitingSendPacket )
		{
		iServer.iServerGetSendMsg.Complete(KErrCancel);
		iServer.iServerWaitingSendPacket = EFalse;
		}
	_DBG_FILE("CNSmlObexCommSession::DoCancelGetSendPacket(): end");
	}

// -----------------------------------------------------------------------------
// CNSmlObexCommSession::GetSendTimeout
// -----------------------------------------------------------------------------
//
void CNSmlObexCommSession::GetSendTimeout()
	{
	_DBG_FILE("CNSmlObexCommSession::GetSendTimeout(): begin");
	iServer.iServerWaitingSendPacket = EFalse;
	iServer.iServerGetSendMsg.Complete(KErrTimedOut);
	_DBG_FILE("CNSmlObexCommSession::GetSendTimeout(): end");
	}

// -----------------------------------------------------------------------------
// CNSmlObexCommSession::ListenDisconnect
// -----------------------------------------------------------------------------
//
void CNSmlObexCommSession::ListenDisconnect( const RMessagePtr2& aMessage )
	{
	_DBG_FILE("CNSmlObexCommSession::ListenDisconnect(): begin");
	iServer.iServerListenDisconnectMsg = aMessage;
	if( iServer.iClientDisconnected )
		{
		ServerHandleDisconnect(KErrNone);
		}
	else
		{
		iServer.iServerListeningDisconnect = ETrue;
		}
	_DBG_FILE("CNSmlObexCommSession::ListenDisconnect(): end");
	}

// -----------------------------------------------------------------------------
// CNSmlObexCommSession::CancelListenDisconnect
// -----------------------------------------------------------------------------
//
void CNSmlObexCommSession::CancelListenDisconnect( const RMessagePtr2& aMessage )
	{
	_DBG_FILE("CNSmlObexCommSession::CancelListenDisconnect(): begin");
	if( iServer.iServerListeningDisconnect )
		{
		ServerHandleDisconnect(KErrCancel);
		}
	aMessage.Complete(KErrNone);
	_DBG_FILE("CNSmlObexCommSession::CancelListenDisconnect(): end");
	}

// -----------------------------------------------------------------------------
// CNSmlObexCommSession::ConstructL
// Symbian 2nd phase constructor.
// -----------------------------------------------------------------------------
//
void CNSmlObexCommSession::ConstructL()
	{
	_DBG_FILE("CNSmlObexCommSession::ConstructL(): begin");
	iServer.IncrementSessions();
	_DBG_FILE("CNSmlObexCommSession::ConstructL(): end");
	}

// -----------------------------------------------------------------------------
// CNSmlObexCommSession::CNSmlObexCommSession
// -----------------------------------------------------------------------------
//
CNSmlObexCommSession::CNSmlObexCommSession( CNSmlObexCommServer& aServer ) : iServer(aServer)
	{
	_DBG_FILE("CNSmlObexCommSession::CNSmlObexCommSession(): begin");
	_DBG_FILE("CNSmlObexCommSession::CNSmlObexCommSession(): end");
	}

// -----------------------------------------------------------------------------
// CNSmlObexCommSession::~CNSmlObexCommSession
// Destructor.
// -----------------------------------------------------------------------------
//
CNSmlObexCommSession::~CNSmlObexCommSession()
	{
	_DBG_FILE("CNSmlObexCommSession::~CNSmlObexCommSession(): begin");
	iServer.DecrementSessions();
	_DBG_FILE("CNSmlObexCommSession::~CNSmlObexCommSession(): end");
	}

// -----------------------------------------------------------------------------
// CNSmlObexCommSession::PanicClient
// -----------------------------------------------------------------------------
//
void CNSmlObexCommSession::PanicClient( TInt /*aReason*/ ) const
	{
	_DBG_FILE("CNSmlObexCommSession::PanicClient(): begin");
	_DBG_FILE("CNSmlObexCommSession::PanicClient(): end");
	}

// -----------------------------------------------------------------------------
// CNSmlObexCommSession::ClientReceivePacketL
// -----------------------------------------------------------------------------
//
void CNSmlObexCommSession::ClientReceivePacketL()
	{
	_DBG_FILE("CNSmlObexCommSession::ClientReceivePacketL(): begin");
	iServer.iClientReceiveTimeOut->Cancel();
	iServer.iClientWaitingReceivePacket = EFalse;
	iServer.iRecvReady = EFalse;
	iServer.iClientReceiveMsg.WriteL(0, *iServer.iRecvBuffer, 0);
	delete iServer.iRecvBuffer;
	iServer.iRecvBuffer = 0;
	iServer.iClientReceiveMsg.Complete(KErrNone);
	_DBG_FILE("CNSmlObexCommSession::ClientReceivePacketL(): end");
	}

// -----------------------------------------------------------------------------
// CNSmlObexCommSession::ServerGetSendPacketL
// -----------------------------------------------------------------------------
//
void CNSmlObexCommSession::ServerGetSendPacketL()
	{
	_DBG_FILE("CNSmlObexCommSession::ServerGetSendPacketL(): begin");
	iServer.iServerGetSendTimeOut->Cancel();
	iServer.iServerWaitingSendPacket = EFalse;
	iServer.iSendReady = EFalse;
	iServer.iServerGetSendMsg.WriteL(0, *iServer.iSendBuffer, 0);
	delete iServer.iSendBuffer;
	iServer.iSendBuffer = 0;
	iServer.iServerGetSendMsg.Complete(KErrNone);
	_DBG_FILE("CNSmlObexCommSession::ServerGetSendPacketL(): end");
	}

// -----------------------------------------------------------------------------
// CNSmlObexCommSession::ServerHandleDisconnect
// -----------------------------------------------------------------------------
//
void CNSmlObexCommSession::ServerHandleDisconnect( TInt aResult )
	{
	_DBG_FILE("CNSmlObexCommSession::ServerHandleDisconnect(): begin");
	DoCancelGetSendPacket();
	iServer.iServerListenDisconnectMsg.Complete(aResult);
	iServer.iServerListeningDisconnect = EFalse;
	
	if( iServer.iClientWaitingReceivePacket )
		{
		DBG_FILE_CODE(aResult, _S8("CNSmlObexCommSession::ServerHandleDisconnect() - signal client"));
		iServer.iClientReceiveMsg.Complete( KErrCancel );
		iServer.iClientWaitingReceivePacket = EFalse;
		}
	_DBG_FILE("CNSmlObexCommSession::ServerHandleDisconnect(): end");
	}
