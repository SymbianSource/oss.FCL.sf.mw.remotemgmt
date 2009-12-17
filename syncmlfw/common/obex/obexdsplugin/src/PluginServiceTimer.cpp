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
* Description:  SyncML Obex plugin for data syncronization
*
*/



// INCLUDE FILES
#include "NSmlObexDSplugin.h"


// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CPluginServiceTimer::CPluginServiceTimer( CNSmlObexDSplugin* aEngine )
//
// -----------------------------------------------------------------------------
//
CPluginServiceTimer::CPluginServiceTimer( CNSmlObexDSplugin* aEngine, TInt aValue )
	: CActive( EPriorityStandard ), iEngine( aEngine ), iValue( aValue ) {}

// -----------------------------------------------------------------------------
// CPluginServiceTimer::ConstructL()
//
// -----------------------------------------------------------------------------
//
void CPluginServiceTimer::ConstructL()
	{
	DBG_FILE( _S8( "CPluginServiceTimer::ConstructL()" ) );
	// create a thread-relative timer
	User::LeaveIfError( iTimer.CreateLocal() );
	DBG_FILE( _S8( "CPluginServiceTimer::ConstructL() : end" ) );
	}

// -----------------------------------------------------------------------------
// CPluginServiceTimer::~CPluginServiceTimer()
//
// -----------------------------------------------------------------------------
//
CPluginServiceTimer::~CPluginServiceTimer()
	{
	DBG_FILE( _S8( "CPluginServiceTimer::~CPluginServiceTimer()" ) );
	iTimer.Close();
	DBG_FILE( _S8( "CPluginServiceTimer::~CPluginServiceTimer() : end" ) );
	}

// -----------------------------------------------------------------------------
// CPluginServiceTimer::StartTimer()
// -----------------------------------------------------------------------------
//
void CPluginServiceTimer::StartTimer()
	{
	DBG_FILE( _S8( "CPluginServiceTimer::StartTimer()" ) );
	if( !IsActive() )
		{
		DBG_FILE( _S8( "!IsActive()" ) );
		iTimer.After( iStatus, TTimeIntervalMicroSeconds32( iValue ) );
		iEngine->iSessionActive = ETrue;
		SetActive();
		DBG_FILE( _S8( "SetActive()" ) );
		}

	DBG_FILE( _S8( "CPluginServiceTimer::StartTimer() : end" ) );
	}

// -----------------------------------------------------------------------------
// CPluginServiceTimer::StopTimer()
// -----------------------------------------------------------------------------
//
void CPluginServiceTimer::StopTimer()
	{
	DBG_FILE( _S8( "CPluginServiceTimer::StopTimer()" ) );
	iEngine->iSessionActive = ETrue;	
	DBG_FILE( _S8( "iSessiActive = ETrue" ) );
	DBG_FILE( _S8( "CPluginServiceTimer::StopTimer() : end" ) );
	}

// -----------------------------------------------------------------------------
// CPluginServiceTimer::DoCancel()
// -----------------------------------------------------------------------------
//
void CPluginServiceTimer::DoCancel()
	{
	DBG_FILE( _S8( "CPluginServiceTimer::DoCancel()" ) );
	iTimer.Cancel();
	DBG_FILE( _S8( "CPluginServiceTimer::DoCancel() : end" ) );
	}

// -----------------------------------------------------------------------------
// CPluginServiceTimer::RunL()
// -----------------------------------------------------------------------------
//
void CPluginServiceTimer::RunL()
	{
	DBG_FILE( _S8( "CPluginServiceTimer::RunL()" ) );
	iEngine->iSessionActive = EFalse;
	DBG_FILE( _S8( "iSessionActive = EFalse" ) );
	iEngine->iStartTimer = EFalse;
	DBG_FILE( _S8( "iStartTimer = EFalse" ) );
	DBG_FILE( _S8( "CPluginServiceTimer::RunL() : end" ) );
	}
