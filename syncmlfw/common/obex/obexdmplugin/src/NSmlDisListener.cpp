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
//	CDisListener::~CDisListener()
//
// -----------------------------------------------------------------------------
//
CDisListener::~CDisListener()
	{
	Cancel();
	}

// -----------------------------------------------------------------------------
//	CDisListener::RunL()
//
// -----------------------------------------------------------------------------
//
void CDisListener::RunL()
	{
	_DBG_FILE("CDisListener::RunL() begin");	
	this->iPlugin->iStopped = ETrue;
	this->iPlugin->Disconnect();
	_DBG_FILE("CDisListener::RunL() end");	
	}

// -----------------------------------------------------------------------------
//	CDisListener::DoCancel()
//
// -----------------------------------------------------------------------------
//
void CDisListener::DoCancel()
	{
	_DBG_FILE("CDisListener::DoCancel()");	
	if( iState == EListening && !iSessCancelled )
		{
		_DBG_FILE("before CancelListenDisconnect");	
		iSs.CancelListenDisconnect();
		iState = EIdle;
		}
	}

// -----------------------------------------------------------------------------
//	CDisListener::ListenDisconnect()
//
// -----------------------------------------------------------------------------
//
void CDisListener::ListenDisconnect()
	{
	_DBG_FILE("CDisListener::ListenDisconnect()");	
	iState = EListening;
	iSs.ListenDisconnect(iStatus);
	SetActive();
	}

