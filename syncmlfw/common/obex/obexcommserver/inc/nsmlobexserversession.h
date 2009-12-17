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



#ifndef __NSMLOBEXSERVERSESSION_H__
#define __NSMLOBEXSERVERSESSION_H__

// ---------------------------------------------------------------------------------------
// Includes
// ---------------------------------------------------------------------------------------
#include <e32std.h>

// ---------------------------------------------------------------------------------------
// Class forwards
// ---------------------------------------------------------------------------------------
class CNSmlObexCommServer;

// ---------------------------------------------------------------------------------------
// Base implementation of obex server session
// ---------------------------------------------------------------------------------------
class RNSmlObexServerSession : public RSessionBase
	{
public:
	virtual TInt Connect() = 0;
	IMPORT_C void Close();
	IMPORT_C void Disconnect();
	IMPORT_C void SetReceivedPacket( const TDesC8& aData );
	IMPORT_C TInt GetSendPacket( TDes8& aData );
	IMPORT_C void GetSendPacket( TDes8& aData, TRequestStatus& aStatus ) ;
	IMPORT_C void CancelGetSendPacket();
	IMPORT_C void ListenDisconnect( TRequestStatus& aStatus );
	IMPORT_C void CancelListenDisconnect();

protected:
	TInt DoConnect( const TDesC& aServerName );
	};

// ---------------------------------------------------------------------------------------
// DS implementation of obex server session
// ---------------------------------------------------------------------------------------
class RNSmlDSObexServerSession : public RNSmlObexServerSession
	{
public:
	IMPORT_C virtual TInt Connect();
	};

// ---------------------------------------------------------------------------------------
// DM implementation of obex server session
// ---------------------------------------------------------------------------------------
class RNSmlDMObexServerSession : public RNSmlObexServerSession
	{
public:
	IMPORT_C virtual TInt Connect();
	};

#endif // __NSMLOBEXSERVERSESSION_H__