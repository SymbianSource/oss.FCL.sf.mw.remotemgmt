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


// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CNSmlTimeOut* CNSmlTimeOut::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CNSmlTimeOut* CNSmlTimeOut::NewL()
	{
	_DBG_FILE("CNSmlTimeOut::NewL(): begin");
	CNSmlTimeOut* self = new (ELeave) CNSmlTimeOut();
	CleanupStack::PushL(self);
	User::LeaveIfError(self->iTimer.CreateLocal());
	CleanupStack::Pop(); // self
	_DBG_FILE("CNSmlTimeOut::NewL(): end");
	return self;
	}

// -----------------------------------------------------------------------------
// CNSmlTimeOut::CNSmlTimeOut
// C++ default constructor.
// -----------------------------------------------------------------------------
//
CNSmlTimeOut::CNSmlTimeOut() : CActive(EPriorityStandard)
	{
	_DBG_FILE("CNSmlTimeOut::CNSmlTimeOut(): begin");
	CActiveScheduler::Add(this);
	_DBG_FILE("CNSmlTimeOut::CNSmlTimeOut(): end");
	}

// -----------------------------------------------------------------------------
// CNSmlTimeOut::~CNSmlTimeOut
// Destructor.
// -----------------------------------------------------------------------------
//
CNSmlTimeOut::~CNSmlTimeOut()
	{
	_DBG_FILE("CNSmlTimeOut::~CNSmlTimeOut(): begin");
	Cancel();
	_DBG_FILE("CNSmlTimeOut::~CNSmlTimeOut(): end");
	}

// -----------------------------------------------------------------------------
// CNSmlTimeOut::RunL
// -----------------------------------------------------------------------------
//
void CNSmlTimeOut::RunL()
	{
	_DBG_FILE("CNSmlTimeOut::RunL(): begin");
	if( iStatus.Int() == KErrNone )
		{
		(iThis->*iCallback)();
		}
	_DBG_FILE("CNSmlTimeOut::RunL(): end");
	}

// -----------------------------------------------------------------------------
// CNSmlTimeOut::DoCancel
// -----------------------------------------------------------------------------
//
void CNSmlTimeOut::DoCancel()
	{
	_DBG_FILE("CNSmlTimeOut::DoCancel(): begin");
	iTimer.Cancel();
	_DBG_FILE("CNSmlTimeOut::DoCancel(): end");
	}

// -----------------------------------------------------------------------------
// CNSmlTimeOut::StartTimeoutHandling
// -----------------------------------------------------------------------------
//
void CNSmlTimeOut::StartTimeoutHandling( CNSmlObexCommSession* aThis, TNSmlTimeoutCallback aCallback )
	{
	_DBG_FILE("CNSmlTimeOut::StartTimeoutHandling(): begin");
	iThis = aThis;
	iCallback = aCallback;
	if ( !IsActive() )
	    {
    	iTimer.After(iStatus, KTimeOutInSeconds*1000000);
	    SetActive();
	    }
	_DBG_FILE("CNSmlTimeOut::StartTimeoutHandling(): end");
	}
