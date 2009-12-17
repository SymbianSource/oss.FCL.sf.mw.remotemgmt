/*
* Copyright (c) 2003 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  SyncML HTTP client
*
*/


// INCLUDE FILES
#include "nsmlhttp.h"

//RD_Auto_Restart
#include "nsmlprivatepskeys.h"
//RD_Auto_Restart


// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CNSmlXptShutdownTimer::CNSmlXptShutdownTimer
// constructor
// -----------------------------------------------------------------------------
//
CNSmlXptShutdownTimer::CNSmlXptShutdownTimer( CNSmlHTTP* aEngine ) 
	: CActive( EPriorityStandard ), iEngine( aEngine ) {}
	
// -----------------------------------------------------------------------------
// CNSmlXptShutdownTimer::ConstructL
// 2-phase constructor
// -----------------------------------------------------------------------------
//
void CNSmlXptShutdownTimer::ConstructL()
	{
	CActiveScheduler::Add( this );
	// create a thread-relative timer
	User::LeaveIfError( iTimer.CreateLocal() );
	}

// -----------------------------------------------------------------------------
// CNSmlXptShutdownTimer::~CNSmlXptShutdownTimer
// desctructor
// -----------------------------------------------------------------------------
//
CNSmlXptShutdownTimer::~CNSmlXptShutdownTimer()
	{
	DBG_FILE(_S8("CNSmlXptShutdownTimer destructor called"));
	Cancel();
	iTimer.Close();
	}

// -----------------------------------------------------------------------------
// CNSmlXptShutdownTimer::Start()
// Start timer. CNSmlXptShutdownTimer is started with all the xpt calls
// that have possibility to get jammed due to slow response from server etc.
// -----------------------------------------------------------------------------
//
void CNSmlXptShutdownTimer::Start()
	{
	// do this only if session is DS 
	TInt session=0;    
    TInt r=RProperty::Get( KPSUidNSmlSOSServerKey, KNSmlSyncJobOngoing, session);                       
    DBG_FILE_CODE(session, _S8("CNSmlXptShutdownTimer::Start() Current Session is (DM = 2, DS = 1) "));
    
    TTimeIntervalMicroSeconds32 delay;
    if( session == ESyncMLDSSession )//for DS session
       delay = KNSmlDSXptShutdownInterval;
    else
       delay = KNSmlXptShutdownInterval;
    
	iTimerSnapTime.HomeTime();
	iTimerSnapTime += delay;	
	iTimer.At( iStatus, iTimerSnapTime );

	SetActive();
	iEngine->iTimeOut = EFalse;
	}

// -----------------------------------------------------------------------------
// CNSmlXptShutdownTimer::StartAborted()
// Start aborted timer. CNSmlXptShutdownTimer is started with all the xpt calls
// that have possibility to get jammed due to slow response from server etc.
// -----------------------------------------------------------------------------
//
void CNSmlXptShutdownTimer::StartAborted()
	{
	DBG_FILE(_S8("CNSmlXptShutdownTimer Start aborted timer begins"));
	iTimer.At( iStatus, iTimerSnapTime );
	SetActive();
	iEngine->iTimeOut = EFalse;
	}

// -----------------------------------------------------------------------------
// CNSmlXptShutdownTimer::DoCancel()
// DoCancel() from CActive
// -----------------------------------------------------------------------------
//
void CNSmlXptShutdownTimer::DoCancel()
	{
	DBG_FILE(_S8("CNSmlXptShutdownTimer DoCancel called"));
	iTimer.Cancel();
	}

// -----------------------------------------------------------------------------
// CNSmlXptShutdownTimer::RunL()
// From CActive. Timer executes following operations if time is up.
// -----------------------------------------------------------------------------
//
void CNSmlXptShutdownTimer::RunL()
	{
	DBG_FILE(_S8("HTTP client Time Out !!"));
	DBG_FILE(_S8("CNSmlXptShutdownTimer RunL called"));
	
	switch( iStatus.Int() )
		{
		case KErrNone:
			{
			DBG_FILE(_S8("No Error in ShutDownTimer, Normal completion"));
			iEngine->iTimeOut = ETrue;
			iEngine->Cancel();
			} break;

		case KErrCancel:
			{
			DBG_FILE(_S8("ShutDownTimer was cancelled!"));
			iEngine->iTimeOut = ETrue;
			iEngine->Cancel();
			} break;

		case KErrAbort:
			{
			DBG_FILE(_S8("ERROR, ShutDownTimer aborted"));
			DBG_FILE(_S8("Restarting the Timer"));
		//	this->StartAborted();
			this->Start();
			} break;

		default:
            {
			DBG_FILE(_S8("ShutDownTimer default operation"));
			iEngine->iTimeOut = ETrue;
			iEngine->Cancel();
			} break;
		}
	}
