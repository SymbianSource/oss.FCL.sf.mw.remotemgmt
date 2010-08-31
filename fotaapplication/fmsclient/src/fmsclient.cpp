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
* Description: Implementation of fotaserver component
* 	This is part of fotaapplication.
*
*/

#include <f32file.h>
#include <fotaConst.h>
#include <fotaengine.h>
#include "fmsclientserver.h"
#include "fmsclient.h"
#include "fmsclientdebug.h"

#include "fotaserverPrivateCRKeys.h"
#include <centralrepository.h>
// ---------------------------------------------------------------------------
// IsSecureClient()  
// Checks who is trying to launch FMS
// ---------------------------------------------------------------------------
TBool IsSecureClient(TBool& aFotaStartUp)
	{
	FLOG(_L("IsSecureClient Started"));
	RThread thread;
	TUid fotaserver,fotastartup;
	fotaserver.iUid = KFotaServerUid;
	fotastartup.iUid = KStarterUid;	
	if ( thread.SecureId() == fotaserver.iUid ) 
		{
		FLOG(_L("fmsclient fotaserver is using the service"));
		return ETrue;
		}
	else if ( thread.SecureId() ==  fotastartup.iUid )
		{
		FLOG(_L("fmsclient fotastartupplugin is using the service"));
		aFotaStartUp = ETrue;
		return ETrue;    
		}
		else if (thread.SecureId() == 0x01D278B9 ) //test app
		{
		FLOG(_L("fmsclient test app is using the service"));		
		return ETrue;
		}
	FLOG(_L("fmsclient not launched"));
	return EFalse;
	}

// ---------------------------------------------------------------------------
// StartServerL()  
//	Starts Fota Downlaod Interrupt Monitory server
// ---------------------------------------------------------------------------
TInt StartServerL()
	{
	FLOG(_L("StartServerLt Started"));
	TInt res = KErrNone;
	RProcess server;
	res=server.Create(KFMSServerName,KNullDesC);
	FLOG(_L("StartServerL-- create server error as %d"),res);
	if (res!=KErrNone)
		{		
		return res;
		}
	TRequestStatus stat;
	server.Rendezvous(stat);
	if (stat!=KRequestPending)
		server.Kill(0);		// abort startup
	else
		server.Resume();	// logon OK - start the server
	User::WaitForRequest(stat);		// wait for start or death
	res=(server.ExitType()==EExitPanic) ? KErrGeneral : stat.Int();
	FLOG(_L("StartServerL-- server.ExitType() returns %d"),res);
	server.Close();
	return res;
	}

// ---------------------------------------------------------------------------
// RFMSClient::OpenL()
// ---------------------------------------------------------------------------
EXPORT_C TInt RFMSClient::OpenL()
	{	
	FLOG(_L("RFMSClient::OpenL()- Begin"));
	TInt res = KErrNone;
	TBool FotaStartUp = EFalse;
	if( IsSecureClient(FotaStartUp))
		{
		FLOG(_L("RFMSClient::OpenL()- Secured client"));
		res = CreateSession( KFMSServerName,TVersion(1,0,0),2 );
		if ( res != KErrNone )
			{
			FLOG(_L("RFMSClient::OpenL()- session not created"));
			res = StartServerL();
			User::LeaveIfError( res );
			res = CreateSession( KFMSServerName,TVersion(1,0,0),2 );
			}
		User::LeaveIfError( res );
		FLOG(_L("RFMSClient::OpenL()- session created"));
		CRepository* centrep2 = NULL;
    TRAPD( err, centrep2 = CRepository::NewL( KCRUidFotaServer ) )
    TInt val = KErrNone;
    if (err==KErrNone ) 
      {
      	      centrep2->Get(  KFotaUpdateState, val );
      	      delete centrep2;
      }
            
            centrep2 = NULL;
    
		if( FotaStartUp && val == EFotaDownloadInterrupted )
			{
			FLOG(_L("RFMSClient::OpenL()- fotastartup using FMS session"));
			SendInterruptParamsL();
			}
		}
	else //not a secured client
		{
		FLOG(_L("fmsclient not launched--insecured client"));
		res = KErrNotFound;
		}
	FLOG(_L("RFMSClient::OpenL()- End %d"),res);	
	return res;
	}

// ---------------------------------------------------------------------------
// RFMSClient::Close()
// ---------------------------------------------------------------------------
EXPORT_C void RFMSClient::Close()
	{
    FLOG(_L(" RFMSClient::Close"));
	RSessionBase::Close();  //basecall
	}
	
// ----------------------------------------------------------------------------
// RFMSClient::NotifyForResumeL()
// ----------------------------------------------------------------------------	
EXPORT_C void RFMSClient::NotifyForResumeL(const TFmsIpcCommands& aReason,
		const TInt& aBearer, const TDriveNumber& aDrive, const TInt& aSize)
	{		
    FLOG(_L(" RFMSClient::NotifyForResumeL--Begin"));
    TInt Size(aSize),Drive((TInt)aDrive),Bearer(aBearer);
    TInt err = SendReceive( (TInt)aReason, TIpcArgs(Bearer,Drive,Size));
    FLOG(_L(" RFMSClient::NotifyForResumeL--End error as %d"),err);
	}

// ---------------------------------------------------------------------------
// RFMSClient::SendInterruptParamsL() called only when fotastartup launched
// ---------------------------------------------------------------------------	
void RFMSClient::SendInterruptParamsL()
	{
	FLOG(_L(" RFMSClient::SendInterruptParamsL--Begin"));
	TInt err = Send( EFotaStartUpPlugin );
	FLOG(_L(" RFMSClient::SendInterruptParamsL--End as %d"),err);
	}
	
EXPORT_C void RFMSClient::NotifyForUpdateL(const TFmsIpcCommands& aReason, TUint aLevel)
    {
    FLOG(_L(" RFMSClient::NotifyForUpdateL--Begin, level = %d"), aLevel);
    TInt err = SendReceive( (TInt) aReason, TIpcArgs (aLevel));
    FLOG(_L(" RFMSClient::NotifyForUpdateL--End as %d"),err);
    User::LeaveIfError(err);
    }
// ---------------------------------------------------------------------------
// RFMSClient::Cancel() 
// ---------------------------------------------------------------------------	
EXPORT_C void RFMSClient::Cancel()
	{
	FLOG(_L(" RFMSClient::Cancel--Begin"));
	TInt err = SendReceive( ECancelOutstandingRequest );
	FLOG(_L(" RFMSClient::Cancel--End as %d"),err);
	}
	
// ---------------------------------------------------------------------------
// RFMSClient::ActivePhoneCallSync() 
// ---------------------------------------------------------------------------  
EXPORT_C TInt RFMSClient::IsPhoneCallActive(TInt& aCallActive)
    {
    FLOG(_L(" RFMSClient::IsPhoneCallActive--Begin"));
    TPckgBuf<TInt> result(aCallActive);
    TInt err = SendReceive( EUpdPhoneCallActive,TIpcArgs(&result));    
    aCallActive=result();
    FLOG(_L(" RFMSClient::IsPhoneCallActive--End as %d,%d"),err,aCallActive);
    return err;
    }

//End of file
