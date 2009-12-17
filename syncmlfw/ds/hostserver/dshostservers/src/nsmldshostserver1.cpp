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
* Description:  DS Host Server 1
*
*/


// INCLUDE FILES
#include <s32mem.h>
#include <nsmldebug.h>

#include "nsmldshostconstants.h"
#include "nsmlhostserver1.h"

// ======================================= MEMBER FUNCTIONS =======================================

// ------------------------------------------------------------------------------------------------
// CNSmlDSHostServer1::NewL
// ------------------------------------------------------------------------------------------------
//
CNSmlDSHostServer1* CNSmlDSHostServer1::NewL( const TDesC& aServerName )
	{
	_DBG_FILE( "CNSmlDSHostServer1::NewL(): begin" );
	CNSmlDSHostServer1 *pS = new ( ELeave ) CNSmlDSHostServer1();
	CleanupStack::PushL( pS );
	pS->StartL( aServerName );
	CleanupStack::Pop(); // pS
	_DBG_FILE( "CNSmlDSHostServer1::NewL(): end" );
	return pS;
	}

// ------------------------------------------------------------------------------------------------
// CNSmlDSHostServer1::~CNSmlDSHostServer1
// ------------------------------------------------------------------------------------------------
//
CNSmlDSHostServer1::~CNSmlDSHostServer1()
	{
	_DBG_FILE( "CNSmlDSHostServer1::~CNSmlDSHostServer1(): begin" );
	_DBG_FILE( "CNSmlDSHostServer1::~CNSmlDSHostServer1(): end" );
	}

// ------------------------------------------------------------------------------------------------
// CNSmlDSHostServer1::LaunchServer
// ------------------------------------------------------------------------------------------------
//
TInt CNSmlDSHostServer1::LaunchServer( TAny* )
    {
    return InitHostServer( ENSmlHostServer1, ( TThreadFunction ) ThreadFunctionL );
    }
    
// ------------------------------------------------------------------------------------------------
// CNSmlDSHostServer1::ThreadFunctionL
// ------------------------------------------------------------------------------------------------
//
TInt CNSmlDSHostServer1::ThreadFunctionL( TAny* /*aStarted*/ )
    {
    CNSmlDSHostServer1* hs = NULL;
	TRAPD( err, hs = CNSmlDSHostServer1::NewL( KNSmlDSHostServer1Name() ) );
	RProcess::Rendezvous( err );
    CActiveScheduler::Start();
    delete hs;
    
    return KErrNone;
    }

// ------------------------------------------------------------------------------------------------
// CNSmlDSHostServer1::PanicServer
// ------------------------------------------------------------------------------------------------
//
void CNSmlDSHostServer1::PanicServer( TNSmlHostServerErrors aPanic )
    {
	_LIT( KTxtDSHostServer1,"NSmlDSHostServer1" );
	User::Panic( KTxtDSHostServer1, aPanic );
    }

// ------------------------------------------------------------------------------------------------
// CNSmlDSHostServer1::CNSmlDSHostServer1
// ------------------------------------------------------------------------------------------------
//
CNSmlDSHostServer1::CNSmlDSHostServer1()
	{
	_DBG_FILE( "CNSmlDSHostServer1::CNSmlDSHostServer1(): begin" );
	_DBG_FILE( "CNSmlDSHostServer1::CNSmlDSHostServer1(): end" );
	}

// ------------------------------------------------------------------------------------------------
// CNSmlDSHostServer1::NewSessionL
// ------------------------------------------------------------------------------------------------
//
CSession2* CNSmlDSHostServer1::NewSessionL( const TVersion& aVersion, const RMessage2& aMessage ) const
    { 
    return CallNewSessionL( aVersion, aMessage );
    }

// ------------------------------------------------------------------------------------------------
// 
// ------------------------------------------------------------------------------------------------
//
GLDEF_C TInt E32Main()
    {
    CNSmlDSHostServer1::LaunchServer( NULL );
    return KErrNone;
    }

// End of File


