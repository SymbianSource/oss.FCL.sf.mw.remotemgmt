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
* Description:  Ecom plug-in for x-wap-application:syncml.dm application ID
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
#include <SyncMLDef.h>
#include <nsmldebug.h>
#include <nsmldmconst.h>
#include <NSmlPrivateAPI.h>
#include "nsmldmwappushalert.h"

// ============================ MEMBER FUNCTIONS ===============================

// ------------------------------------------------------------------------------------------------
// CNSmlDMWapPushAlert* CNSmlDMWapPushAlert::NewL()
// ------------------------------------------------------------------------------------------------
CNSmlDMWapPushAlert* CNSmlDMWapPushAlert::NewL()
	{
	CNSmlDMWapPushAlert* self = new (ELeave) CNSmlDMWapPushAlert;
	return self;
	}

// ------------------------------------------------------------------------------------------------
// CNSmlDMWapPushAlert::~CNSmlDMWapPushAlert()
// ------------------------------------------------------------------------------------------------
CNSmlDMWapPushAlert::~CNSmlDMWapPushAlert()
	{
	}

// ------------------------------------------------------------------------------------------------
// CNSmlDMWapPushAlert::CNSmlDMWapPushAlert()
// ------------------------------------------------------------------------------------------------
CNSmlDMWapPushAlert::CNSmlDMWapPushAlert()
: CPushHandlerBase()
	{
	CActiveScheduler::Add(this);
	}
// ------------------------------------------------------------------------------------------------
// CNSmlDMWapPushAlert::ProcessMessageL()
// ------------------------------------------------------------------------------------------------
void CNSmlDMWapPushAlert::ProcessMessageL(const CPushMessage& aPushMsg) 
	{
	TPtrC contentType;
	aPushMsg.GetContentType(contentType);
	if (contentType == KNSmlDMNotificationMIMEType )
		{
		TPtrC8 messageBody;
		if ( aPushMsg.GetMessageBody( messageBody ) )
			{
			RNSmlPrivateAPI privateApi;
					
			TRAPD(openErr, privateApi.OpenL());
			
			if ( openErr != KErrNone )
				{
				return;
				}
				
			TRAP_IGNORE(privateApi.SendL( messageBody, ESmlDevMan, ESmlVersion1_1_2 ));
		
			privateApi.Close();
			}
		}
	}
// ------------------------------------------------------------------------------------------------
// CNSmlDMWapPushAlert::HandleMessageL()
// ------------------------------------------------------------------------------------------------
void CNSmlDMWapPushAlert::HandleMessageL(CPushMessage* aPushMsg, TRequestStatus& aStatus)
	{
	 _DBG_FILE("CNSmlDMWapPushAlert::HandleMessageL() Async Func begins"); 	

	SetConfirmationStatus(aStatus);
	
	ProcessMessageL( *aPushMsg );
	
	SetActive();
	}

// ------------------------------------------------------------------------------------------------
// CNSmlDMWapPushAlert::CancelHandleMessage()
// ------------------------------------------------------------------------------------------------
void CNSmlDMWapPushAlert::CancelHandleMessage()
	{
	}

// ------------------------------------------------------------------------------------------------
// CNSmlDMWapPushAlert::HandleMessageL()
// ------------------------------------------------------------------------------------------------
void CNSmlDMWapPushAlert::HandleMessageL(CPushMessage* aPushMsg)
	{
	 _DBG_FILE("CNSmlDMWapPushAlert::HandleMessageL() Sync Func begins"); 	
	
	ProcessMessageL( *aPushMsg );
	
	iPluginKiller->KillPushPlugin();
	}

// ------------------------------------------------------------------------------------------------
// CNSmlDMWapPushAlert::CPushHandlerBase_Reserved1()
// ------------------------------------------------------------------------------------------------
void CNSmlDMWapPushAlert::CPushHandlerBase_Reserved1()
	{
	}

// ------------------------------------------------------------------------------------------------
// CNSmlDMWapPushAlert::CPushHandlerBase_Reserved2()
// ------------------------------------------------------------------------------------------------
void CNSmlDMWapPushAlert::CPushHandlerBase_Reserved2()
	{
	}

// ------------------------------------------------------------------------------------------------
// CNSmlDMWapPushAlert::DoCancel()
// ------------------------------------------------------------------------------------------------
void CNSmlDMWapPushAlert::DoCancel()
	{
	}

// ------------------------------------------------------------------------------------------------
// CNSmlDMWapPushAlert::RunL()
// ------------------------------------------------------------------------------------------------
void CNSmlDMWapPushAlert::RunL()
	{
	_DBG_FILE("CNSmlDMWapPushAlert:: RunL Called");
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
		IMPLEMENTATION_PROXY_ENTRY(KNSmlDMWapPushAlertImplUid, CNSmlDMWapPushAlert::NewL)
    };

// ------------------------------------------------------------------------------------------------
//
// ------------------------------------------------------------------------------------------------
EXPORT_C const TImplementationProxy* ImplementationGroupProxy( TInt& aTableCount )
    {
	_DBG_FILE("ImplementationGroupProxy() for CNSmlDMWapPushAlert: begin");
    aTableCount = sizeof(ImplementationTable) / sizeof(TImplementationProxy);
	_DBG_FILE("ImplementationGroupProxy() for CNSmlDMWapPushAlert: end");
    return ImplementationTable;
	}

// End of file

