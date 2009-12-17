/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description: Implementation of policymanagement components
*
*/
/* DMUtilClient.cpp
*/

#include <e32svr.h>
#include <centralrepository.h>
#include "PMUtilInternalCRKeys.h"
#include "DMUtilClientServer.h"
#include "DMUtilClient.h"
#include "DMCert.h"
#include "debug.h"

#ifdef __TARM_SYMBIAN_CONVERGENCY
#include <e32property.h>
const TUid KUidSmlSyncAgentCategory = { 0x10009F46 }; // temporary
const TUint KHttpsServerCertificateKey = 1234; // temporary
#else
// nothing
#endif

// MACROS


// Standard server startup code
// 
static TInt StartServer()
	{
	RDEBUG("DMUtilServer: Starting server...");
	
	const TUidType serverUid(KNullUid,KNullUid,KDMUtilServerUid);

	// EPOC and EKA 2 is easy, we just create a new server process. Simultaneous
	// launching of two such processes should be detected when the second one
	// attempts to create the server object, failing with KErrAlreadyExists.
	RProcess server;
	//TInt r=server.Create(KHelloWorldServerImg,KNullDesC,serverUid);
	TInt r=server.Create(KDMUtilServerImg,KNullDesC);

	if (r!=KErrNone)
		{
		RDEBUG_2("DMUtilClient: server start failed %d",r);
		return r;
		}
	TRequestStatus stat;
	server.Rendezvous(stat);
	if (stat!=KRequestPending)
		server.Kill(0);		// abort startup
	else
		server.Resume();	// logon OK - start the server
	RDEBUG("DMUtilClient: Started");
	User::WaitForRequest(stat);		// wait for start or death
	// we can't use the 'exit reason' if the server panicked as this
	// is the panic 'reason' and may be '0' which cannot be distinguished
	// from KErrNone
	r=(server.ExitType()==EExitPanic) ? KErrGeneral : stat.Int();
	server.Close();
	return r;
	}

// This is the standard retry pattern for server connection
EXPORT_C TInt RDMUtil::Connect()
	{
	RDEBUG("RDMUtil::Connect - start");
	TInt retry=2;
	for (;;)
		{
		TInt r=CreateSession(KDMUtilServerName,TVersion(0,0,0),50);
		if (r!=KErrNotFound && r!=KErrServerTerminated)
			return r;
		if (--retry==0)
			return r;
		r=StartServer();
		if (r!=KErrNone && r!=KErrAlreadyExists)
			return r;
		}
	}

EXPORT_C void RDMUtil::Close()
	{
	RSessionBase::Close();  //basecall
	}
	
	
EXPORT_C TInt RDMUtil::RemoveACL( const TDesC8& aURI, TBool aRestoreDefaults)
{
	TPckg<TBool> par2Pack( aRestoreDefaults);	
	
	return SendReceive( ERemoveACL, TIpcArgs( &aURI, &par2Pack));			
}

EXPORT_C TInt RDMUtil::AddACLForNode( const TDesC8& aURI, const TACLDestination& aDestination, const TAclCommands& aCommandType)
{
	TPckg<TACLDestination> par2Pack( aDestination);	
	TPckg<TAclCommands> par3Pack( aCommandType);	
	
	return SendReceive( EAddACL, TIpcArgs( &aURI, &par2Pack, &par3Pack));			
	
}

EXPORT_C TInt RDMUtil::SetACLForNode( const TDesC8& aURI, const TACLDestination& aDestination, const TAclCommands& aCommandType)
{
	TPckg<TACLDestination> par2Pack( aDestination);	
	TPckg<TAclCommands> par3Pack( aCommandType);	
	
	return SendReceive( ESetACL, TIpcArgs( &aURI, &par2Pack, &par3Pack));			
}
	
EXPORT_C TInt RDMUtil::SetMngSessionCertificate( const TCertInfo& aCertInfo)
{
	TPckg<TCertInfo> certInfoPckg( aCertInfo);		
	
	return SendReceive( EMngSessionCertificate, TIpcArgs( &certInfoPckg));				
}
	


EXPORT_C TInt RDMUtil::NewDMSession( const TCertInfo& aCertInfo, const TDesC8& aServerID)
{
	TPckg<TCertInfo> certInfoPckg( aCertInfo);		
	
	return SendReceive( ENewDMSession, TIpcArgs( &certInfoPckg, &aServerID));			
}


EXPORT_C TInt RDMUtil::Flush()
{
	return SendReceive( EFlush, TIpcArgs());
}

EXPORT_C TInt RDMUtil::UpdatePolicyMngStatusFlags( KPolicyMngStatusFlags aPolicyMngStatusFlags)
{
	TPckg<KPolicyMngStatusFlags> policyMngPckg( aPolicyMngStatusFlags);		
	
	return SendReceive( EUpdatePolicyMngStatusFlags, TIpcArgs( &policyMngPckg));	
}

EXPORT_C TInt RDMUtil::PerformRFS()
{
	return SendReceive( EPerformDMUtilRFS);	
}



EXPORT_C TInt RDMUtil::GetDMSessionCertInfo( TCertInfo &aCertInfo )
{
    TPckg<TCertInfo> certp( aCertInfo );
    return SendReceive( EGetDMSessionCertInfo, TIpcArgs( &certp ) );
}

EXPORT_C TInt RDMUtil::GetDMSessionServerId( TDes8& aServerId )
{
    return SendReceive( EGetDMSessionServerId, TIpcArgs( &aServerId ) );
}

EXPORT_C TInt RDMUtil::MarkMMCWipe()
	{
	return SendReceive( EMarkMMCWipe );
	}

EXPORT_C TInt RDMCert::Get( TCertInfo &aCertInfo )
	{
	TInt result = KErrNone;
	TPckg<TCertInfo> certp( aCertInfo );
	
#ifdef __TARM_SYMBIAN_CONVERGENCY
	result = RProperty::Get( KUidSmlSyncAgentCategory, KHttpsServerCertificateKey, certp );
#else
	TRAPD( err, 
		{
		CRepository* rep = CRepository::NewL( KCRUidPolicyManagementUtilInternalKeys );
		CleanupStack::PushL( rep );
		result = rep->Get( KSyncMLSessionCertificate, certp );
		CleanupStack::PopAndDestroy( rep );
		} );
	if( err != KErrNone )
		{
		result = err;
		}	
#endif
	
	return result;
	};

