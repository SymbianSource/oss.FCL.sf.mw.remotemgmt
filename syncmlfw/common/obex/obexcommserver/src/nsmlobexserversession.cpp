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



// ---------------------------------------------------------------------------------------
// Include
// ---------------------------------------------------------------------------------------
#include <nsmldebug.h>
#include "nsmlobexserversession.h"
#include "nsmlobexcommserver.h"
#include "nsmlobexdefs.h"

// ---------------------------------------------------------------------------------------
// RNSmlObexServerSession::Connect()
// ---------------------------------------------------------------------------------------
TInt RNSmlObexServerSession::DoConnect( const TDesC& aServerName )
	{
	return CreateSession(aServerName,
						 TVersion(KNSmlObexCommServerVersionMajor,
						 KNSmlObexCommServerVersionMinor, 0));		
	}

// ---------------------------------------------------------------------------------------
// RNSmlObexServerSession::Close()
// ---------------------------------------------------------------------------------------
EXPORT_C void RNSmlObexServerSession::Close()
	{
	RSessionBase::Close();
	}

// ---------------------------------------------------------------------------------------
// RNSmlObexServerSession::Disconnect()
// ---------------------------------------------------------------------------------------
EXPORT_C void RNSmlObexServerSession::Disconnect()
	{
	_DBG_FILE("RNSmlObexServerSession::Disconnect(): begin");
    SendReceive(ENSmlObexCommCommandDisconnect, TIpcArgs( TIpcArgs::ENothing ) );
	_DBG_FILE("RNSmlObexServerSession::Disconnect(): end");
	}

// ---------------------------------------------------------------------------------------
// RNSmlObexServerSession::SetReceivedPacket( const TDesC8& aData )
// ---------------------------------------------------------------------------------------
EXPORT_C void RNSmlObexServerSession::SetReceivedPacket( const TDesC8& aData )
	{
	_DBG_FILE("RNSmlObexServerSession::SetReceivedPacket(): begin");
	SendReceive(ENSmlObexCommCommandSetReceivePacket, TIpcArgs( &aData ));
	_DBG_FILE("RNSmlObexServerSession::SetReceivedPacket(): end");
	}

// ---------------------------------------------------------------------------------------
// RNSmlObexServerSession::GetSendPacket( TDes8& aData )
// ---------------------------------------------------------------------------------------
EXPORT_C TInt RNSmlObexServerSession::GetSendPacket( TDes8& aData )
	{
	_DBG_FILE("RNSmlObexServerSession::GetSendPacket(): begin");
	TInt err( KErrNone );
	err = SendReceive( ENSmlObexCommCommandGetSendPacket, TIpcArgs( &aData ) );
	DBG_FILE_CODE( err, _S8("RNSmlObexServerSession::GetSendPacket(): end"));
	return err;
	}
	
// ---------------------------------------------------------------------------------------
// RNSmlObexServerSession::CancelGetSendPacket()
// ---------------------------------------------------------------------------------------
EXPORT_C void RNSmlObexServerSession::CancelGetSendPacket()
	{
	_DBG_FILE("RNSmlObexServerSession::CancelGetSendPacket(): begin");
    SendReceive(ENSmlObexCommCommandCancelGetSendPacket, TIpcArgs( TIpcArgs::ENothing ));
	_DBG_FILE("RNSmlObexServerSession::CancelGetSendPacket(): end");
	}

// ---------------------------------------------------------------------------------------
// RNSmlObexServerSession::ListenDisconnect( TRequestStatus& aStatus )
// ---------------------------------------------------------------------------------------
EXPORT_C void RNSmlObexServerSession::ListenDisconnect( TRequestStatus& aStatus )
	{
	_DBG_FILE("RNSmlObexServerSession::ListenDisconnect(): begin");
	SendReceive(ENSmlObexCommCommandListenDisconnect, TIpcArgs( TIpcArgs::ENothing ), aStatus);
	_DBG_FILE("RNSmlObexServerSession::ListenDisconnect(): end");
	}

// ---------------------------------------------------------------------------------------
// RNSmlObexServerSession::CancelListenDisconnect()
// ---------------------------------------------------------------------------------------
EXPORT_C void RNSmlObexServerSession::CancelListenDisconnect()
	{
	_DBG_FILE("RNSmlObexServerSession::CancelListenDisconnect(): begin");
	SendReceive(ENSmlObexCommCommandCancelListenDisconnect, TIpcArgs( TIpcArgs::ENothing ));
	_DBG_FILE("RNSmlObexServerSession::CancelListenDisconnect(): end");
	}


// ---------------------------------------------------------------------------------------
// RNSmlDSObexServerSession
// ---------------------------------------------------------------------------------------

// ---------------------------------------------------------------------------------------
// TInt RNSmlDSObexServerSession::Connect()
// ---------------------------------------------------------------------------------------
EXPORT_C TInt RNSmlDSObexServerSession::Connect()
	{
	_DBG_FILE("RNSmlDSObexServerSession::Connect()");
	return DoConnect( KNSmlDSObexCommServerName() );
	}

// ---------------------------------------------------------------------------------------
// RNSmlDMObexServerSession
// ---------------------------------------------------------------------------------------

// ---------------------------------------------------------------------------------------
// TInt RNSmlDMObexServerSession::Connect()
// ---------------------------------------------------------------------------------------
EXPORT_C TInt RNSmlDMObexServerSession::Connect()
	{
	_DBG_FILE("RNSmlDMObexServerSession::Connect()");
	return DoConnect( KNSmlDMObexCommServerName() );
	}

// ---------------------------------------------------------------------------------------
// RNSmlObexServerSession::GetSendPacket( TDes8& aData, TRequestStatus& aStatus  )
// ---------------------------------------------------------------------------------------
EXPORT_C void RNSmlObexServerSession::GetSendPacket( TDes8& aData, TRequestStatus& aStatus )
	{
	_DBG_FILE("RNSmlObexServerSession::GetSendPacket(): begin overloaded version");

	SendReceive( ENSmlObexCommCommandGetSendPacket, TIpcArgs( &aData ), aStatus );
	
	_DBG_FILE("RNSmlObexServerSession::GetSendPacket(): end overloaded version");
	return;
	}	
