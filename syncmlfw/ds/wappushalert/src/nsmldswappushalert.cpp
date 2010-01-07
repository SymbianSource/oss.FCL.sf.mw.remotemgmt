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
* Description:  Ecom plug-in for x-wap-application:push.syncml application ID
*                          
*
*/


// INCLUDES
//
#include <e32base.h>
#include <pushlog.h>
#include <pushmessage.h>
#include <pluginkiller.h>
//#include <pushdispatcher.h>
#include <ccontenthandlerbase.h>
#include <implementationproxy.h> // For TImplementationProxy definition

#include <nsmlconstants.h>
#include <nsmldebug.h>
#include <nsmlsyncalerthandlerif.h>
#include "nsmldswappushalert.h"

// -----------------------------------------------------------
// Constants
// -----------------------------------------------------------
const TUint KNSmlDSAlertImplUid = 0x101F99EC;


// ------------------------------------------------------------------------------------------------
// CNSmlDSWapPushAlert* CNSmlDSWapPushAlert::NewL()
// ------------------------------------------------------------------------------------------------
CNSmlDSWapPushAlert* CNSmlDSWapPushAlert::NewL()
	{
	CNSmlDSWapPushAlert* self = new (ELeave) CNSmlDSWapPushAlert;
	return self;
	}

// ------------------------------------------------------------------------------------------------
// CNSmlDSWapPushAlert::~CNSmlDSWapPushAlert()
// ------------------------------------------------------------------------------------------------
CNSmlDSWapPushAlert::~CNSmlDSWapPushAlert()
	{
	}

// ------------------------------------------------------------------------------------------------
// CNSmlDSWapPushAlert::CNSmlDSWapPushAlert()
// ------------------------------------------------------------------------------------------------
CNSmlDSWapPushAlert::CNSmlDSWapPushAlert()
: CPushHandlerBase()
	{
	CActiveScheduler::Add(this);
	}
// ------------------------------------------------------------------------------------------------
// CNSmlDSWapPushAlert::ProcessMessageL()
// ------------------------------------------------------------------------------------------------
void CNSmlDSWapPushAlert::ProcessMessageL(const CPushMessage& aPushMsg) 
	{
	TPtrC8 messageBody;
	if ( aPushMsg.GetMessageBody( messageBody ) )
		{
		CNSmlSyncAlertHandler* alert;
		TUid uid;
		uid.iUid = KNSmlDSAlertImplUid;
		alert = CNSmlSyncAlertHandler::NewL( uid );
		if ( alert )
			{
			CleanupStack::PushL( alert );
			// Only first and second parameter means in StartSyncL call
			alert->StartSyncL( messageBody, KUidNSmlMediumTypeInternet.iUid, TBTDevAddr(), KNullDesC16, 0 );
			CleanupStack::PopAndDestroy( alert );
			}
		}
	}
// ------------------------------------------------------------------------------------------------
// CNSmlDSWapPushAlert::HandleMessageL()
// ------------------------------------------------------------------------------------------------
void CNSmlDSWapPushAlert::HandleMessageL(CPushMessage* aPushMsg, TRequestStatus& aStatus)
	{
	 _DBG_FILE("CNSmlDSWapPushAlert::HandleMessageL() Async Func begins"); 	

	SetConfirmationStatus(aStatus);
	
	ProcessMessageL( *aPushMsg );
	
	SetActive();
	}

// ------------------------------------------------------------------------------------------------
// CNSmlDSWapPushAlert::CancelHandleMessage()
// ------------------------------------------------------------------------------------------------
void CNSmlDSWapPushAlert::CancelHandleMessage()
	{
	}

// ------------------------------------------------------------------------------------------------
// CNSmlDSWapPushAlert::HandleMessageL()
// ------------------------------------------------------------------------------------------------
void CNSmlDSWapPushAlert::HandleMessageL(CPushMessage* aPushMsg)
	{
	 _DBG_FILE("CNSmlDSWapPushAlert::HandleMessageL() Sync Func begins"); 	
	
	ProcessMessageL( *aPushMsg );
	
	iPluginKiller->KillPushPlugin();
	}

// ------------------------------------------------------------------------------------------------
// CNSmlDSWapPushAlert::CPushHandlerBase_Reserved1()
// ------------------------------------------------------------------------------------------------
void CNSmlDSWapPushAlert::CPushHandlerBase_Reserved1()
	{
	}

// ------------------------------------------------------------------------------------------------
// CNSmlDSWapPushAlert::CPushHandlerBase_Reserved2()
// ------------------------------------------------------------------------------------------------
void CNSmlDSWapPushAlert::CPushHandlerBase_Reserved2()
	{
	}

// ------------------------------------------------------------------------------------------------
// CNSmlDSWapPushAlert::DoCancel()
// ------------------------------------------------------------------------------------------------
void CNSmlDSWapPushAlert::DoCancel()
	{
	}

// ------------------------------------------------------------------------------------------------
// CNSmlDSWapPushAlert::RunL()
// ------------------------------------------------------------------------------------------------
void CNSmlDSWapPushAlert::RunL()
	{
	_DBG_FILE("CNSmlDSWapPushAlert:: RunL Called");
	SignalConfirmationStatus(KErrNone);
	iPluginKiller->KillPushPlugin();
	}

// ------------------------------------------------------------------------------------------------
//
// ------------------------------------------------------------------------------------------------

#ifndef IMPLEMENTATION_PROXY_ENTRY
#define IMPLEMENTATION_PROXY_ENTRY(aUid, aFuncPtr)	{{aUid},(aFuncPtr)}
#endif

const TImplementationProxy ImplementationTable[] = 
    {
		IMPLEMENTATION_PROXY_ENTRY(KNSmlDSWapPushAlertImplUid, CNSmlDSWapPushAlert::NewL)
    };

// ------------------------------------------------------------------------------------------------
//
// ------------------------------------------------------------------------------------------------
EXPORT_C const TImplementationProxy* ImplementationGroupProxy( TInt& aTableCount )
    {
	_DBG_FILE("ImplementationGroupProxy() for CNSmlDSWapPushAlert: begin");
    aTableCount = sizeof(ImplementationTable) / sizeof(TImplementationProxy);
	_DBG_FILE("ImplementationGroupProxy() for CNSmlDSWapPushAlert: end");
    return ImplementationTable;
	}

//End of File

