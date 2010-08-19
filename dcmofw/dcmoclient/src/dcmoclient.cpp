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
* Description:  DCMO Client implementation
*
*/

#include <e32svr.h>
#include <featmgr.h>
#include "dcmoclientserver.h"
#include "dcmoconst.h"
#include "dcmoclient.h"
#include "dcmodebug.h"


// Standard server startup code
// 
static TInt StartServerL()
	{
	RDEBUG("DCMOServer: Starting server...");
	
	// EPOC and EKA 2 is easy, we just create a new server process. Simultaneous
	// launching of two such processes should be detected when the second one
	// attempts to create the server object, failing with KErrAlreadyExists.
	RProcess server;
	TInt r=server.Create(KDCMOServerName,KNullDesC);

	if (r!=KErrNone)
		{
		RDEBUG_2("DCMOClient: server start failed %d",r);
		return r;
		}
	TRequestStatus stat;
	server.Rendezvous(stat);
	if (stat!=KRequestPending)
		server.Kill(0);		// abort startup
	else
		server.Resume();	// logon OK - start the server
	RDEBUG("DCMOClient: Started");
	User::WaitForRequest(stat);		// wait for start or death
	// we can't use the 'exit reason' if the server panicked as this
	// is the panic 'reason' and may be '0' which cannot be distinguished
	// from KErrNone
	r=(server.ExitType()==EExitPanic) ? KErrGeneral : stat.Int();
	server.Close();
	return r;
	}

// ---------------------------------------------------------------------------
// RDCMOClient::NewL() 
// ---------------------------------------------------------------------------	
EXPORT_C RDCMOClient* RDCMOClient::NewL()
	{
	RDEBUG(" RDCMOClient::NewL--Begin");
	FeatureManager::InitializeLibL();
	TBool featureSupport = FeatureManager::FeatureSupported( KFeatureIdFfRuntimeDeviceCapabilityConfiguration );
	FeatureManager::UnInitializeLib();
	
	if ( ! featureSupport )  
  {
		// feature is not supported
		RDEBUG(" RDCMOClient::NewL-- DCMO Feature is Not Supported" );	
		User::Leave(KErrNotSupported);
	}
		
	RDCMOClient* self = new( ELeave ) RDCMOClient( );
	CleanupStack::PushL( self );
	TInt err = self->OpenL();
  CleanupStack::Pop();
  RDEBUG_2(" RDCMOClient::NewL--End error as %d",err);  	
  return self;
  
	}

// -----------------------------------------------------------------------------
// RDCMOClient::~RDCMOClient()
// Destructor.
// -----------------------------------------------------------------------------
//
EXPORT_C RDCMOClient::~RDCMOClient()
{
    Close();
}	

// ---------------------------------------------------------------------------
// RDCMOClient::OpenL()
// ---------------------------------------------------------------------------
EXPORT_C TInt RDCMOClient::OpenL()
	{	
	RDEBUG("RDCMOClient::OpenL()- Begin");
	TInt res = KErrNone;	
	res = CreateSession( KDCMOServerName,TVersion(KDCMOServerMajorVersionNumber, KDCMOServerMinorVersionNumber, KDCMOServerBuildVersionNumber), KDCMOServerMessageSlotNumber );
	if ( res == KErrNotFound || res == KErrServerTerminated )
		{
		RDEBUG("RDCMOClient::OpenL()- session not created");
		res = StartServerL();
		User::LeaveIfError( res );
		res = CreateSession( KDCMOServerName,TVersion(KDCMOServerMajorVersionNumber, KDCMOServerMinorVersionNumber, KDCMOServerBuildVersionNumber), KDCMOServerMessageSlotNumber );
		}
	User::LeaveIfError( res );
	RDEBUG_2("RDCMOClient::OpenL()- End with Errorcode as %d",res);	
	return res;
	}

// ---------------------------------------------------------------------------
// RDCMOClient::Close()
// ---------------------------------------------------------------------------
EXPORT_C void RDCMOClient::Close()
	{
	RDEBUG(" RDCMOClient::Close");
	RSessionBase::Close();  //basecall
	}


// ----------------------------------------------------------------------------
// RDCMOClient::GetDCMOIntAttributeValue()
// ----------------------------------------------------------------------------	
EXPORT_C TDCMOStatus RDCMOClient::GetDCMOIntAttributeValue(TDes& aCategory, TDCMONode aId, TInt& aValue)
	{		
    RDEBUG(" RDCMOClient::GetDCMOIntAttributeValue--Begin");
	TPckg<TDCMONode> p(aId);
	TPckg<TInt> intValue(aValue);
	TDCMOStatus status(EDcmoSuccess);
	TPckg<TDCMOStatus> statusValue(status);
	TInt err = SendReceive( EDcmoGetAttrInt, TIpcArgs(&aCategory, &p, &intValue, &statusValue));
	if(err != KErrNone)
	    status = EDcmoFail;  	
	RDEBUG_2(" RDCMOClient::GetDCMOIntAttributeValue--End error as %d",err);
	return status;
	}

// ----------------------------------------------------------------------------
// RDCMOClient::GetDCMOStrAttributeValue()
// ----------------------------------------------------------------------------	
EXPORT_C TDCMOStatus RDCMOClient::GetDCMOStrAttributeValue(TDes& aCategory, TDCMONode aId, TDes& aStrValue)
	{		
	RDEBUG(" RDCMOClient::GetDCMOStrAttributeValue--Begin");
	TPckg<TDCMONode> p(aId);
	TDCMOStatus status(EDcmoSuccess);
	TPckg<TDCMOStatus> statusValue(status);
	TInt err = SendReceive( EDcmoGetAttrStr, TIpcArgs(&aCategory, &p, &aStrValue, &statusValue));
	if(err != KErrNone)
	    status = EDcmoFail;
	RDEBUG_2(" RDCMOClient::GetDCMOStrAttributeValue--End error as %d",err);
	return status;
	}

// ----------------------------------------------------------------------------
// RDCMOClient::SetDCMOIntAttributeValue()
// ----------------------------------------------------------------------------	
EXPORT_C TDCMOStatus RDCMOClient::SetDCMOIntAttributeValue(TDes& aCategory, TDCMONode aId, TInt aValue)
	{		
	RDEBUG(" RDCMOClient::SetDCMOIntAttributeValue--Begin");
	TPckg<TDCMONode> p(aId);
	TPckg<TInt> intValue(aValue);
	TDCMOStatus status(EDcmoSuccess);
	TPckg<TDCMOStatus> statusValue(status);
	TInt err = SendReceive( EDcmoSetAttrInt, TIpcArgs(&aCategory, &p, &intValue, &statusValue));
	if(err != KErrNone)
	    status = EDcmoFail;
	RDEBUG_2(" RDCMOClient::SetDCMOIntAttributeValue--End error as %d",err);
	return status;
	}
	
// ----------------------------------------------------------------------------
// RDCMOClient::SetDCMOStrAttributeValue()
// ----------------------------------------------------------------------------	
EXPORT_C TDCMOStatus RDCMOClient::SetDCMOStrAttributeValue(TDes& aCategory, TDCMONode aId, TDes& aStrValue)
	{		
	RDEBUG(" RDCMOClient::SetDCMOStrAttributeValue--Begin");
	TPckg<TDCMONode> p(aId);
	TDCMOStatus status(EDcmoSuccess);
	TPckg<TDCMOStatus> statusValue(status);
	TInt err = SendReceive( EDcmoSetAttrStr, TIpcArgs(&aCategory, &p, &aStrValue, &statusValue));
	if(err != KErrNone)
	    status = EDcmoFail;
	RDEBUG_2(" RDCMOClient::SetDCMOStrAttributeValue--End error as %d",err);
	return status;
	}	

// ----------------------------------------------------------------------------
// RDCMOClient::SearchByGroupValue()
// ----------------------------------------------------------------------------	
EXPORT_C void RDCMOClient::SearchByGroupValue(TDes& aGroup, TDes& aAdapterList)
	{		
	RDEBUG(" RDCMOClient::SearchByGroupValue--Begin");
	TInt err = SendReceive( EDcmoSearchAdapter, TIpcArgs(&aGroup, &aAdapterList));
	RDEBUG_2(" RDCMOClient::SearchByGroupValue--End error as %d",err);
	}		
