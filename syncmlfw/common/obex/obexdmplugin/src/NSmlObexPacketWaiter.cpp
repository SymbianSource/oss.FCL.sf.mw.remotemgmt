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
* Description:  SyncML Obex plugin for Device Management
*
*/



// INCLUDE FILES
#include "NSmlObexDMplugin.h"


// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CNSmlObexPacketWaiter::CNSmlObexPacketWaiter
// 
// -----------------------------------------------------------------------------
// 
CNSmlObexPacketWaiter::CNSmlObexPacketWaiter(RNSmlDMObexServerSession& aSession)
:	iSession(aSession), iPtr(NULL, 0)
	{
	}
	
// -----------------------------------------------------------------------------
// CNSmlObexPacketWaiter::~CNSmlObexPacketWaiter()
// 
// -----------------------------------------------------------------------------
// 
CNSmlObexPacketWaiter::~CNSmlObexPacketWaiter()
	{
	delete iData;
	}

// -----------------------------------------------------------------------------
// CNSmlObexPacketWaiter::ConstructL()
// 
// -----------------------------------------------------------------------------
// 
void CNSmlObexPacketWaiter::ConstructL()
	{
	iData = HBufC8::NewL(KNSmlDefaultWorkspaceSize);
	iPtr.Set( iData->Des() );
	}

// -----------------------------------------------------------------------------
// CNSmlObexPacketWaiter::WaitForGetData()
// 
// -----------------------------------------------------------------------------
// 
TInt CNSmlObexPacketWaiter::WaitForGetData()
	{
	TInt err( KErrNone );
	iPtr.SetLength(0);
	err = iSession.GetSendPacket( iPtr );
	return err;
	}
