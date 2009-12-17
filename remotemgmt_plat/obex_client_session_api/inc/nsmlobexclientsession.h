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



#ifndef __NSMLOBEXCLIENTSESSION_H__
#define __NSMLOBEXCLIENTSESSION_H__

// ---------------------------------------------------------------------------------------
// Includes
// ---------------------------------------------------------------------------------------
#include <e32std.h>

// ---------------------------------------------------------------------------------------
// Base implementation of obex client session
// ---------------------------------------------------------------------------------------
class RNSmlObexClientSession : public RSessionBase
	{
public:
	virtual TInt Connect() = 0;
	IMPORT_C void Close();
	
	// These methods are mapped in Obex binding to transport methods
	IMPORT_C void Disconnect();
	IMPORT_C void Send( const TDesC8& aStartPtr );
	IMPORT_C void Receive( TDes8& aStartPtr, TRequestStatus& aStatus );
	IMPORT_C void CancelReceive();

protected:
	TInt DoConnect( const TDesC& aServerName ); 
	};

// ---------------------------------------------------------------------------------------
// DS obex client session
// ---------------------------------------------------------------------------------------
class RNSmlDSObexClientSession : public RNSmlObexClientSession
	{
public:
	IMPORT_C virtual TInt Connect();
	};

// ---------------------------------------------------------------------------------------
// DM obex client session
// ---------------------------------------------------------------------------------------
class RNSmlDMObexClientSession : public RNSmlObexClientSession
	{
public:
	IMPORT_C virtual TInt Connect();
	};

#endif // __NSMLOBEXCLIENTSESSION_H__
