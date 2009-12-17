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
#include <nsmlobexclientsession.h>
#include "nsmlobexdefs.h"

// ---------------------------------------------------------------------------------------
// RNSmlObexClientSession::DoConnect( const TDesC& aServerName )
// ---------------------------------------------------------------------------------------
TInt RNSmlObexClientSession::DoConnect( const TDesC& aServerName )
	{
	return CreateSession( aServerName, TVersion(KNSmlObexCommServerVersionMajor, KNSmlObexCommServerVersionMinor, 0) );
	}

// ---------------------------------------------------------------------------------------
// RNSmlObexClientSession::Close()
// ---------------------------------------------------------------------------------------
EXPORT_C void RNSmlObexClientSession::Close()
	{
	RSessionBase::Close();
	}

// ---------------------------------------------------------------------------------------
// RNSmlObexClientSession::Disconnect()
// ---------------------------------------------------------------------------------------
EXPORT_C void RNSmlObexClientSession::Disconnect()
	{
	_DBG_FILE("RNSmlObexClientSession::Disconnect(): begin");
    SendReceive(ENSmlObexCommCommandDisconnect, TIpcArgs ( TIpcArgs::ENothing ));
	_DBG_FILE("RNSmlObexClientSession::Disconnect(): end");
	}

// ---------------------------------------------------------------------------------------
// RNSmlObexClientSession::Send( const TDesC8& aStartPtr )
// ---------------------------------------------------------------------------------------
EXPORT_C void RNSmlObexClientSession::Send( const TDesC8& aStartPtr )
	{
	_DBG_FILE("RNSmlObexClientSession::Send(): begin");
	SendReceive(ENSmlObexCommCommandSend, TIpcArgs ( &aStartPtr ));
	_DBG_FILE("RNSmlObexClientSession::Send(): end");
	}

// ---------------------------------------------------------------------------------------
// RNSmlObexClientSession::Receive( TDes8& aStartPtr, TRequestStatus& aStatus )
// ---------------------------------------------------------------------------------------
EXPORT_C void RNSmlObexClientSession::Receive( TDes8& aStartPtr, TRequestStatus& aStatus )
	{
	_DBG_FILE("RNSmlObexClientSession::Receive(): begin");
	SendReceive(ENSmlObexCommCommandReceive, TIpcArgs ( &aStartPtr ), aStatus);
	_DBG_FILE("RNSmlObexClientSession::Receive(): end");
	}

// ---------------------------------------------------------------------------------------
// RNSmlObexClientSession::CancelReceive()
// ---------------------------------------------------------------------------------------
EXPORT_C void RNSmlObexClientSession::CancelReceive()
	{
	_DBG_FILE("RNSmlObexClientSession::CancelReceive(): begin");
    SendReceive(ENSmlObexCommCommandCancelReceive, TIpcArgs ( TIpcArgs::ENothing ));
	_DBG_FILE("RNSmlObexClientSession::CancelReceive(): end");
	}
// ---------------------------------------------------------------------------------------
// RNSmlDSObexClientSession
// ---------------------------------------------------------------------------------------

// ---------------------------------------------------------------------------------------
// TInt RNSmlDSObexClientSession::Connect()
// ---------------------------------------------------------------------------------------
EXPORT_C TInt RNSmlDSObexClientSession::Connect()
	{
	_DBG_FILE("RNSmlDSObexClientSession::Connect()");
	return DoConnect( KNSmlDSObexCommServerName() );
	}

// ---------------------------------------------------------------------------------------
// RNSmlDMObexClientSession
// ---------------------------------------------------------------------------------------

// ---------------------------------------------------------------------------------------
// TInt RNSmlDMObexClientSession::Connect()
// ---------------------------------------------------------------------------------------
EXPORT_C TInt RNSmlDMObexClientSession::Connect()
	{
	_DBG_FILE("RNSmlDMObexClientSession::Connect()");
	return DoConnect( KNSmlDMObexCommServerName() );
	}
