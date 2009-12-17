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
* Description:  SyncML Obex server binding
*
*/


#include "nsmlobexserverbinding.h"
#include "nsmlerror.h"

// CONSTANTS
_LIT( KInvalidState, "Invalid state" );

//============================================================
// CNSmlObexServerBinding definition
//============================================================

//------------------------------------------------------------
// CNSmlObexServerBinding::NewL()
//------------------------------------------------------------
CNSmlObexServerBinding* CNSmlObexServerBinding::NewL()
    {
	CNSmlObexServerBinding* self = new (ELeave) CNSmlObexServerBinding();
	CleanupStack::PushL( self );
    self->ConstructL();
	CleanupStack::Pop(); //self
    return self;
    }
//------------------------------------------------------------
// CNSmlObexServerBinding::CNSmlObexServerBinding()
//------------------------------------------------------------
CNSmlObexServerBinding::CNSmlObexServerBinding() : CActive(0)
	{
	CActiveScheduler::Add( this );
	}
//------------------------------------------------------------
// CNSmlObexServerBinding::~CNSmlObexServerBinding()
//------------------------------------------------------------
CNSmlObexServerBinding::~CNSmlObexServerBinding()
	{
	_DBG_FILE("CNSmlObexServerBinding::~CNSmlObexServerBinding() begin");
	if( iContent == ENSmlDataSync && iSessionAlive )
		{
		_DBG_FILE("Closing iCsDS");
		iCsDS.Disconnect();
		iCsDS.Close();
		}
	if( iContent == ENSmlDeviceManagement && iSessionAlive )
		{
		_DBG_FILE("Closing iCsDM");
		iCsDM.Disconnect();
		iCsDM.Close();
		}
	Cancel();
	_DBG_FILE("CNSmlObexServerBinding::~CNSmlObexServerBinding() end");
	}
//------------------------------------------------------------
// CNSmlObexServerBinding::ConstructL()
// 2-phase
//------------------------------------------------------------
void CNSmlObexServerBinding::ConstructL()
    {
	iSessionAlive = EFalse;
    }
//------------------------------------------------------------
// CNSmlObexServerBinding* CreateCNsmlObexServerBindingL()
// 
//------------------------------------------------------------
EXPORT_C CNSmlObexServerBinding* CreateCNsmlObexServerBindingL()
	{
	return CNSmlObexServerBinding::NewL();
	}
//------------------------------------------------------------
// CNSmlObexServerBinding::Connect( TNSmlObexTransport /*aTransport*/,TBool /*aServerAlerted*/, TDesC8& /*aMimeType*/, TRequestStatus &aStatus )
// 
//------------------------------------------------------------
void CNSmlObexServerBinding::Connect( TNSmlObexTransport /*aTransport*/,TBool /*aServerAlerted*/, TDesC8& aMimeType, TRequestStatus &aStatus )
	{
	_DBG_FILE("CNSmlObexServerBinding::Connect");
	TInt err ( KErrNone );
	iCancelledByServer = EFalse;
	iAgentStatus = &aStatus;
	*iAgentStatus = KRequestPending;

	if( aMimeType.Compare( KDataSyncMIME ) == 0 )
		{
		_DBG_FILE("Data sync MIME type");
		iContent = ENSmlDataSync;
		}
	else
		{
		_DBG_FILE("Dev Man MIME type");
		iContent = ENSmlDeviceManagement;
		}
	err = ConnectToServer();
	DBG_FILE_CODE( err, _S8("ConnectToServer returned") );
	if ( err )
	    {
	    _DBG_FILE("ConnectToServer error converted to Server not responding");
	    err = TNSmlError::ESmlServerNotResponding;
	    }
	User::RequestComplete( iAgentStatus, err );
	}
//------------------------------------------------------------
// CNSmlObexServerBinding::ConnectToServer()
// 
//------------------------------------------------------------
TInt CNSmlObexServerBinding::ConnectToServer()
	{
	_DBG_FILE("CNSmlObexServerBinding::ConnectToServer");
	TInt err( KErrNone );
	if( iContent == ENSmlDataSync )
		{
		_DBG_FILE("DS client session");
		err = iCsDS.Connect();
		if( err == KErrNone )
			{
			iSessionAlive = ETrue;
			}
		}
	else
		{
		_DBG_FILE("DM client session");
		err = iCsDM.Connect();
		if( err == KErrNone )
			{
			iSessionAlive = ETrue;
			}
		}
	return err;
	}
//------------------------------------------------------------
// CNSmlObexServerBinding::DoCancel()
// From CActive
//------------------------------------------------------------
void CNSmlObexServerBinding::DoCancel()
	{
	_DBG_FILE("CNSmlObexServerBinding::DoCancel");
	if( iState == EReceiving )
		{
		_DBG_FILE("CNSmlObexServerBinding - Canceling receive");
		if( iContent == ENSmlDataSync )
			{
			_DBG_FILE("Canceling iCsDS");
			iCsDS.CancelReceive();
			}
		else
			{
			_DBG_FILE("Canceling iCsDM");
			iCsDM.CancelReceive();
			}
		if( iCancelledByServer )
			{
			_DBG_FILE("iCancelledByServer");
			User::RequestComplete( iAgentStatus, KErrCancel );
			}
		else
			{
			_DBG_FILE("Cancelled by sync app");
			User::RequestComplete( iAgentStatus, KErrNone );
			}
		}
	}
//------------------------------------------------------------
// CNSmlObexServerBinding::RunL()
// From CActive
//------------------------------------------------------------
void CNSmlObexServerBinding::RunL()
	{
	TState s = iState;
	iState = EIdle;
	if( iStatus.Int() != KErrNone )
		{
		DBG_FILE_CODE( iStatus.Int(), _S8("error...") );
		if( iContent == ENSmlDataSync )
			{
			_DBG_FILE("Disconnecting iCsDS");
			iCsDS.Disconnect();
			iCsDS.Close();
			iSessionAlive = EFalse;
			}
		else
			{
			_DBG_FILE("Disconnecting iCsDM");
			iCsDM.Disconnect();
			iCsDM.Close();
			iSessionAlive = EFalse;
			}
		User::RequestComplete( iAgentStatus, iStatus.Int() );
		}
	else
		{
		switch( s )
			{
			case EIdle:
				if( iContent == ENSmlDataSync )
					{
					_DBG_FILE("Disconnecting iCsDS");
					iCsDS.Disconnect();
					iCsDS.Close();
					iSessionAlive = EFalse;
					}
				else
					{
					_DBG_FILE("Disconnecting iCsDM");
					iCsDM.Disconnect();
					iCsDM.Close();
					iSessionAlive = EFalse;
					}
				break;
			case EConnecting:
			case ESending:
			case EReceiving:
				User::RequestComplete( iAgentStatus, iStatus.Int() );
				iState = EIdle;
				break;
			default:
				User::Panic( KInvalidState, s );
			}
		}
	}
//------------------------------------------------------------
// CNSmlObexServerBinding::Send( TDesC8& aStartPtr, TBool /*aFinalPacket*/, TRequestStatus &aStatus )
// 
//------------------------------------------------------------
void CNSmlObexServerBinding::Send( const TDesC8& aStartPtr, TBool /*aFinalPacket*/, TRequestStatus &aStatus )
	{
	_DBG_FILE("CNSmlObexServerBinding::Send");
	iCancelledByServer = EFalse;
	iState = ESending;
	iAgentStatus = &aStatus;
	*iAgentStatus = KRequestPending;

	if( iContent == ENSmlDataSync )
		{
		_DBG_FILE("iCsDS Send");
		iCsDS.Send( aStartPtr );
		}
	else
		{
		_DBG_FILE("iCsDM Send");
		iCsDM.Send( aStartPtr );
		}
	User::RequestComplete( iAgentStatus, KErrNone );
	iState = EIdle;
	}
//------------------------------------------------------------
// CNSmlObexServerBinding::Receive( TDes8& aStartPtr, TRequestStatus &aStatus )
// 
//------------------------------------------------------------
void CNSmlObexServerBinding::Receive( TDes8& aStartPtr, TRequestStatus &aStatus )
	{
	_DBG_FILE("CNSmlObexServerBinding::Receive");
	iCancelledByServer = EFalse;
	iState = EReceiving;
	iAgentStatus = &aStatus;
	*iAgentStatus = KRequestPending;

	if( iContent == ENSmlDataSync )
		{
		_DBG_FILE("iCsDS Receive");
		iCsDS.Receive( aStartPtr, iStatus );
		}
	else
		{
		_DBG_FILE("iCsDM Receive");
		iCsDM.Receive( aStartPtr, iStatus );
		}
	SetActive();
	}
//------------------------------------------------------------
// CNSmlObexServerBinding::Disconnect()
// 
//------------------------------------------------------------
void CNSmlObexServerBinding::Disconnect()
	{
	_DBG_FILE("CNSmlObexServerBinding::Disconnect");
	if( iSessionAlive )
		{
		if( iContent == ENSmlDataSync )
			{
			_DBG_FILE("iCsDS Disconnect");
			iCsDS.Disconnect();
			iCsDS.Close();
			iSessionAlive = EFalse;
			}
		else
			{
			_DBG_FILE("iCsDM Disconnect");
			iCsDM.Disconnect();
			iCsDM.Close();
			iSessionAlive = EFalse;
			}
		}
	}

//End of File

