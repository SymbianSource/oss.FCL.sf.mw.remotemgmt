/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  DM callback server source file
*
*/



// ---------------------------------------------------------------------------
// Includes
// ---------------------------------------------------------------------------

#include <f32file.h>

#include <nsmlconstants.h>
#include <nsmldmdbhandler.h>

#include "nsmldmcallbackserver.h"

// ---------------------------------------------------------------------------
// constants
// ---------------------------------------------------------------------------

const TUint32 KNSmlSOSServer = 0x101F99FB;
const TUint32 KTarmDMUtilServer = 0x10207843; //tarm
const TUint32 KFotaServer       = 0x102072C4; //fota
const TUint32 KNSmlDMHostServer1ID = 270506498;  // 0x101F9A02
const TUint32 KNSmlDMHostServer2ID = 270506499;  // 0x101F9A03
const TUint32 KNSmlDMHostServer3ID = 270506500;  // 0x101F9A04
const TUint32 KNSmlDMHostServer4ID = 270506501;  // 0x101F9A05


// ---------------------------------------------------------------------------
// E32Main
// ---------------------------------------------------------------------------
GLDEF_C TInt E32Main() // main function called by E32
{
    LaunchServer();
    return KErrNone;
}

// ---------------------------------------------------------------------------
// LaunchServer()
// The only exported function of exedll for launching the server
// ---------------------------------------------------------------------------
TInt LaunchServer()
    {
    // check server not already started
	TFindServer findServer(KNSmlDmCallbackServerName);
	TFullName name;
	if ( findServer.Next(name) == KErrNone )
		{ // found server already 
		return KErrGeneral;
		}

	TInt error = CNSmlDmCallbackServer::ThreadFunction( NULL );
    return error;
    }

// ===========================================================================
// CNSmlDmCallbackServer
// ===========================================================================


// ---------------------------------------------------------------------------
// CNSmlDmCallbackServer::PanicServer
// ---------------------------------------------------------------------------
void CNSmlDmCallbackServer::PanicServer( TNSmlDmCallbackServerPanic aPanic )
    {
	_LIT(KTxtServer,"NSmlDmCallbackServer");
	User::Panic(KTxtServer,aPanic);
    }

// ---------------------------------------------------------------------------
// CNSmlDmCallbackServer::ThreadFunction
// ---------------------------------------------------------------------------
TInt CNSmlDmCallbackServer::ThreadFunction(TAny* /*aStarted*/)
	{
    __UHEAP_MARK;
	// create cleanup stack
    CTrapCleanup* cleanup = CTrapCleanup::New();
	if ( !cleanup )
	    {
	    PanicServer(ECleanupCreateError);
	    }    
	// construct and install active scheduler
	CActiveScheduler* scheduler = new CActiveScheduler;
	__ASSERT_ALWAYS( scheduler,PanicServer(EMainSchedulerError) );
	CActiveScheduler::Install( scheduler );

	// construct server
    CNSmlDmCallbackServer* server=0;
    TRAPD( err, server = CNSmlDmCallbackServer::NewL());
    RProcess::Rendezvous( err );
	// start handling requests
	CActiveScheduler::Start();
	
    delete server;
    delete scheduler;    
    delete cleanup;
    __UHEAP_MARKEND;
    return KErrNone;
	}
	
// ---------------------------------------------------------------------------
// CNSmlDmCallbackServer::NewL()
// ---------------------------------------------------------------------------
CNSmlDmCallbackServer* CNSmlDmCallbackServer::NewL()
	{
	CNSmlDmCallbackServer* self = new (ELeave) CNSmlDmCallbackServer();
	CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(); // self
	self->StartL(KNSmlDmCallbackServerName);
	return self;
	}

// ---------------------------------------------------------------------------
// CNSmlDmCallbackServer::CNSmlDmCallbackServer() 
// ---------------------------------------------------------------------------
CNSmlDmCallbackServer::CNSmlDmCallbackServer() :
	CServer2(EPriorityStandard, ESharableSessions)
	{
	}

// ---------------------------------------------------------------------------
// CNSmlDmCallbackServer::ConstructL()
// ---------------------------------------------------------------------------
void CNSmlDmCallbackServer::ConstructL()
    {
   	iDbHandler = CNSmlDmDbHandler::NewL();
    }

// ---------------------------------------------------------------------------
// CNSmlDmCallbackServer::~CNSmlDmCallbackServer()
// ---------------------------------------------------------------------------
CNSmlDmCallbackServer::~CNSmlDmCallbackServer()
	{
	delete iDbHandler;
    }

// ---------------------------------------------------------------------------
// CNSmlDmCallbackServer::NewSessionL()
// ---------------------------------------------------------------------------
CSession2* CNSmlDmCallbackServer::NewSessionL( const TVersion& aVersion,
	const RMessage2& aMessage ) const
	{
	TVersion v(KNSmlDmCallbackServerVersionMajor,
		KNSmlDmCallbackServerVersionMinor, 0);
		
	if( !User::QueryVersionSupported(v,aVersion) )
		{
		User::Leave(KErrNotSupported);
		}

	TUint32 idOfCaller = aMessage.SecureId().iId;
	if ( idOfCaller != KNSmlSOSServer &&
			idOfCaller != KNSmlDMHostServer1ID &&
			idOfCaller != KNSmlDMHostServer2ID &&
			idOfCaller != KNSmlDMHostServer3ID &&
			idOfCaller != KNSmlDMHostServer4ID &&
			idOfCaller != KTarmDMUtilServer  && 
            idOfCaller != KFotaServer  // fota
            ) 
		{
		_LIT_SECURITY_POLICY_S0 ( mySidPolicy, KNSmlSOSServerSecureID );
    	if(!mySidPolicy().CheckPolicy ( aMessage ))
    	{
				User::Leave(KErrAccessDenied);
			}
		}

	return CNSmlDmCallbackSession::NewL( (CNSmlDmCallbackServer&)*this ); 
	}


// ---------------------------------------------------------------------------
// CNSmlDmCallbackServer::Increase()
// ---------------------------------------------------------------------------
void CNSmlDmCallbackServer::Increase()
	{
	iCount++;
	}

// ---------------------------------------------------------------------------
// CNSmlDmCallbackServer::Decrease()
// ---------------------------------------------------------------------------
void CNSmlDmCallbackServer::Decrease()
	{
	iCount--;
    if ( iCount < 1 )
        {
        CActiveScheduler::Stop();   
        }
	}

// ---------------------------------------------------------------------------
// CNSmlDmCallbackServer::Count()
// ---------------------------------------------------------------------------
TInt CNSmlDmCallbackServer::Count()
	{
	return iCount;
	}


// ---------------------------------------------------------------------------
// CNSmlDmCallbackServer::DbHandler()
// ---------------------------------------------------------------------------
CNSmlDmDbHandler* CNSmlDmCallbackServer::DbHandler()
	{
	return iDbHandler;
	}

// End of file
