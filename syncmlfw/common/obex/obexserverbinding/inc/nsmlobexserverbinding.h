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




#include <e32std.h>
#include <f32file.h>
#include <utf.h>


#include <nsmldebug.h>
#include <nsmlobexclientsession.h>
#include "nsmlobexcommserver.h"
#include "nsmltransport.h"


#ifndef __NSMLOBEXSERVERBINDING_H__
#define __NSMLOBEXSERVERBINDING_H__

_LIT8( KDataSyncMIME, "application/vnd.syncml+wbxml" );
_LIT8( KDevManMIME, "application/vnd.syncml.dm+wbxml" );


// ---------------------------------------------------
// class CNSmlObexServerBinding
// ---------------------------------------------------
class CNSmlObexServerBinding : public CActive
	{
friend class CDisListener;

public:
    static CNSmlObexServerBinding* NewL();        
	virtual void ConstructL();
	CNSmlObexServerBinding();
	virtual ~CNSmlObexServerBinding();

	virtual  void Connect( TNSmlObexTransport aTransport,TBool aServerAlerted, TDesC8& aMimeType, TRequestStatus &aStatus );
	virtual  void Disconnect();
	virtual  void Send( const TDesC8& aStartPtr, TBool /*aFinalPacket*/, TRequestStatus &aStatus );
	virtual  void Receive( TDes8& aStartPtr, TRequestStatus &aStatus );


protected:
	void DoCancel();
	void RunL();


private:
	TInt ConnectToServer();

private:
	enum TState
		{
		EIdle,
		EConnecting,
		ESending,
		EReceiving
		};
	enum TNSmlObexContent 
		{
		ENSmlDataSync,
		ENSmlDeviceManagement,
		};

	TRequestStatus* iAgentStatus;

	RNSmlDSObexClientSession iCsDS;
	RNSmlDMObexClientSession iCsDM;
	TState iState;
	TBool iCancelledByServer;
	TNSmlObexContent iContent;
	TBool iSessionAlive;
	};

	IMPORT_C CNSmlObexServerBinding* CreateCNsmlObexServerBindingL();
	typedef CNSmlObexServerBinding* (*TNSmlCreateObexServerBindingFunc) ();   


#endif // __NSMLOBEXSERVERBINDING_H_