/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Class definition of 
*
*/


#include "diagresultsdatabasecommon.h"
#include "diagresultsdbserver.h"
#include "diagresultsdbsession.h"

#include <e32base.h>
#include <e32uid.h>

//Enter any new accepted UIDs here.
const TInt KAcceptedUids[] = 
    { 
    0x2000B0E8, //Phone Doctor
    0x2000E542, //Diagnostics application
    0x102073E4,    // STIF
#if defined( _DEBUG ) || defined( __WINSCW__ )
    0x10282CDA,   //Results DB test app
    0x20000FB1,   //EUnitAppEnv
    0x20000FB2,   //EUnitEikApp
    0x20000FB3,   //EUnitExeEnv
    0xED80F513,   // DiagFramework Test app
#endif // _DEBUG || __WINSCW__
    0x2000F8E8    // donno
    };

const TInt KAcceptedUidCount = sizeof( KAcceptedUids ) / sizeof( TInt );

// ---------------------------------------------------------------------------
// Constructor.
// ---------------------------------------------------------------------------
//  
CDiagResultsDbServer::CDiagResultsDbServer(TInt aPriority, 
                                            const TPolicy &aPolicy)
: CPolicyServer(aPriority, aPolicy), iContainerIndex( NULL ), iSessionCount(0)
	{
	}

// ---------------------------------------------------------------------------
// Destructor.
// ---------------------------------------------------------------------------
//  
CDiagResultsDbServer::~CDiagResultsDbServer()
	{
	delete iContainerIndex;
	}

// ---------------------------------------------------------------------------
// NewLC.
// ---------------------------------------------------------------------------
//  
void CDiagResultsDbServer::NewLC(const TPolicy &aPolicy)
	{
	CDiagResultsDbServer *pS = new CDiagResultsDbServer( 
	                                        CActive::EPriorityHigh, aPolicy );
	CleanupStack::PushL( pS ); 
	pS->StartL( KDiagResultsDatabaseServerName );
	pS->ConstructL();
	}

// ---------------------------------------------------------------------------
// ConstructL.
// ---------------------------------------------------------------------------
// 
void CDiagResultsDbServer::ConstructL()
	{
	iContainerIndex = CObjectConIx::NewL();
	}


// ---------------------------------------------------------------------------
// Create a new server session.
// ---------------------------------------------------------------------------
// 
CSession2 *CDiagResultsDbServer::NewSessionL(const TVersion &aVersion, 
                                        const RMessage2 &/*aMessage*/) const
	{
	// check we're the right version
	TVersion v(KDiagResultsDatabaseServerMajor, KDiagResultsDatabaseServerMinor, 
	                                            KDiagResultsDatabaseServerBuild);
	if( !User::QueryVersionSupported( v, aVersion ) )
		User::Leave( KErrNotSupported );
	// make new session
	return CDiagResultsDbSession::NewL((CDiagResultsDbServer*)this);
	}

// ---------------------------------------------------------------------------
// Create new object container.
// ---------------------------------------------------------------------------
//
CPolicyServer::TCustomResult CDiagResultsDbServer::CustomSecurityCheckL(
                        const RMessage2 &aMsg, 
                        TInt &/*aAction*/, 
                        TSecurityInfo &/*aMissing*/)
	{
	CPolicyServer::TCustomResult result = EFail;
	TInt sid = aMsg.SecureId();

    for ( TInt i = 0; i < KAcceptedUidCount; ++i )
        {
        if ( sid == KAcceptedUids[i] )
            {
            if ( aMsg.HasCapability( ECapabilityReadDeviceData ) &&
	             aMsg.HasCapability( ECapabilityWriteDeviceData ) )
	            {
	            result = EPass;   
	            break; 
	            }    
            }
        }
	    
	return result;
	}

// ---------------------------------------------------------------------------
// Create new object container.
// ---------------------------------------------------------------------------
//
CObjectCon* CDiagResultsDbServer::NewContainerL()
	{
	return iContainerIndex->CreateL();
	}
	

// ---------------------------------------------------------------------------
// Remove object from a container index,
// ---------------------------------------------------------------------------
// 
void CDiagResultsDbServer::RemoveContainer(CObjectCon* aCon)
	{
	iContainerIndex->Remove( aCon );
	}	

// ---------------------------------------------------------------------------
// Return the total amount of client sessions.
// ---------------------------------------------------------------------------
// 	
TInt CDiagResultsDbServer::SessionCount()
	{
	return iSessionCount;
	}

// ---------------------------------------------------------------------------
// Decrease session count when one client leaves and stop server if there are
// no more clients.
// ---------------------------------------------------------------------------
// 
void CDiagResultsDbServer::DecreaseSessionCount()
	{
	iSessionCount--;
	if (iSessionCount == 0)
		{
		CActiveScheduler::Stop();
		}
	}

// ---------------------------------------------------------------------------
// One new session created.
// ---------------------------------------------------------------------------
// 	
void CDiagResultsDbServer::IncreaseSessionCount()
	{
	iSessionCount++;
	}		
    
// ---------------------------------------------------------------------------
// Create cleanupStack and call RunserverL.
// ---------------------------------------------------------------------------
// 
TInt CDiagResultsDbServer::RunServer()
	{
	__UHEAP_MARK;
	//
	CTrapCleanup* cleanup = CTrapCleanup::New();
	TInt ret = KErrNoMemory;
	if( cleanup )
		{
		TRAP( ret, CDiagResultsDbServer::RunServerL() );
		delete cleanup;
		}
	//
	__UHEAP_MARKEND;
	if( ret != KErrNone )
		{
		// Signal the client that server creation failed
		RProcess::Rendezvous( ret );
		}
	return ret;
	}

// ---------------------------------------------------------------------------
// Create active scheduler and run server.
// ---------------------------------------------------------------------------
// 
void CDiagResultsDbServer::RunServerL()
	{
	User::RenameThread(KDiagResultsDatabaseServerName);
	
	// Create and install the active scheduler we need
	CActiveScheduler *as=new (ELeave)CActiveScheduler;
	CleanupStack::PushL( as );
	CActiveScheduler::Install(as);

	// Create server
	CDiagResultsDbServer::NewLC(policy);

	// Initialisation complete, now signal the client

	RProcess::Rendezvous(KErrNone);

	// Ready to run
	CActiveScheduler::Start();

	// Cleanup the server and scheduler
	CleanupStack::PopAndDestroy(2, as);
	}


// ---------------------------------------------------------------------------
// Main function that is called when server(.EXE) is launched.
// ---------------------------------------------------------------------------
// 
TInt E32Main()
	{
	TInt error(KErrNone);
	error = CDiagResultsDbServer::RunServer();
	return error;
	}


