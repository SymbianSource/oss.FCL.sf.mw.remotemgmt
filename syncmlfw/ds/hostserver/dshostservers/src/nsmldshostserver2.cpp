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
* Description:  DS Host Server 2
*
*/


// INCLUDE FILES
#include <s32mem.h>
#include <nsmldebug.h>

#include "nsmldshostconstants.h"
#include "nsmlhostserver2.h"

// ======================================= MEMBER FUNCTIONS =======================================

// ------------------------------------------------------------------------------------------------
// CNSmlDSHostServer2::NewL
// ------------------------------------------------------------------------------------------------
//
CNSmlDSHostServer2* CNSmlDSHostServer2::NewL( const TDesC& aServerName )
	{
	_DBG_FILE( "CNSmlDSHostServer2::NewL(): begin" );
	CNSmlDSHostServer2 *pS = new ( ELeave ) CNSmlDSHostServer2();
	CleanupStack::PushL( pS );
	pS->StartL( aServerName );
	CleanupStack::Pop(); // pS
	_DBG_FILE( "CNSmlDSHostServer2::NewL(): end" );
	return pS;
	}

// ------------------------------------------------------------------------------------------------
// CNSmlDSHostServer2::~CNSmlDSHostServer2
// ------------------------------------------------------------------------------------------------
//
CNSmlDSHostServer2::~CNSmlDSHostServer2()
	{
	_DBG_FILE( "CNSmlDSHostServer2::~CNSmlDSHostServer2(): begin" );
	_DBG_FILE( "CNSmlDSHostServer2::~CNSmlDSHostServer2(): end" );
	}

// ------------------------------------------------------------------------------------------------
// CNSmlDSHostServer2::LaunchServer
// ------------------------------------------------------------------------------------------------
//
TInt CNSmlDSHostServer2::LaunchServer( TAny* )
    {
    return InitHostServer( ENSmlHostServer2, ( TThreadFunction ) ThreadFunctionL );
    }

// ------------------------------------------------------------------------------------------------
// CNSmlDSHostServer2::ThreadFunctionL
// ------------------------------------------------------------------------------------------------
//
TInt CNSmlDSHostServer2::ThreadFunctionL( TAny* /*aStarted*/ )
    {
    CNSmlDSHostServer2* hs = NULL;
	TRAPD( err, hs = CNSmlDSHostServer2::NewL( KNSmlDSHostServer2Name() ) );
	RProcess::Rendezvous( err );
    CActiveScheduler::Start();
    delete hs;
    
    return KErrNone;
    }

// ------------------------------------------------------------------------------------------------
// CNSmlDSHostServer2::PanicServer
// ------------------------------------------------------------------------------------------------
//
void CNSmlDSHostServer2::PanicServer( TNSmlHostServerErrors aPanic )
    {
	_LIT( KTxtDSHostServer2,"NSmlDSHostServer2" );
	User::Panic( KTxtDSHostServer2, aPanic );
    }

// ------------------------------------------------------------------------------------------------
// CNSmlDSHostServer2::CNSmlDSHostServer2
// ------------------------------------------------------------------------------------------------
//
CNSmlDSHostServer2::CNSmlDSHostServer2()
	{
	_DBG_FILE( "CNSmlDSHostServer2::CNSmlDSHostServer2(): begin" );
	_DBG_FILE( "CNSmlDSHostServer2::CNSmlDSHostServer2(): end" );
	}

// ------------------------------------------------------------------------------------------------
// CNSmlDSHostServer2::NewSessionL
// ------------------------------------------------------------------------------------------------
//
CSession2* CNSmlDSHostServer2::NewSessionL( const TVersion& aVersion, const RMessage2& aMessage ) const
    { 
    return CallNewSessionL( aVersion, aMessage );
    }

// ------------------------------------------------------------------------------------------------
// 
// ------------------------------------------------------------------------------------------------
//
GLDEF_C TInt E32Main()
    {
    // Temporary Workaround for Kernel Issue (ou1cimx1#131074)
    // Load libc here because otherwise plugins which link to Qt will result in libc constructors being called twice
    RLibrary lib;
    lib.Load(_L("libc"));

    return CNSmlDSHostServer2::LaunchServer( NULL );
    }

// End of File
