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
* Description:   fotaengines server side session
*
*/



// INCLUDE FILES
#include "FotasrvSession.h"
#include "FotaSrvDebug.h"
#include <eikenv.h>
#include <eikappui.h>
#include <s32mem.h>
#include <e32base.h>
#include <centralrepository.h>
#include "FotaServer.h"
#include "FotaSrvApp.h"
// ============================= MEMBER FUNCTIONS ============================

class CFotaServer;

// ---------------------------------------------------------------------------
// CFotaSrvSession::CheckClientSecureIdL 
// Returns True if caller is syncml framework. False for fotasrv.
// Leaves if client is unknown
// ---------------------------------------------------------------------------
//
CFotaSrvSession::TClient CFotaSrvSession::CheckClientSecureIdL (
                                                  const RMessage2& aMessage)
    {
    //TClient     client(EUnknown);
    TUid    dmhostserver1   = TUid::Uid( KDMHostServer1Uid );
    TUid    fota            = TUid::Uid( KFotaServerUid );
    TUid    omadmappui      = TUid::Uid( KOmaDMAppUid );
    TUid    starter         = TUid::Uid( KStarterUid );
	TUid	fscheduler		= TUid::Uid( KFotaScheduler );
	TUid	fms				= TUid::Uid( KFMSServerUid );
	TUid    softwarechecker = TUid::Uid( 0x2000BB96 );    
	TUid    softwarecheckerbackground = TUid::Uid( 0x2000BB97 );    
   TUid    testapp =        TUid::Uid(  0x102073E4); 

  TUid    iadclient = TUid::Uid( 0x2000F85A ); 

	// Syncml
	if ( aMessage.SecureId() == dmhostserver1.iUid )
		{
		FLOG(_L("[CFotaSrvSession] CheckClientSecureIdL client is \
		DmHostserver!"));
		return EDMHostServer;
		}

	// OmaDMAPpui
	if ( aMessage.SecureId() == omadmappui.iUid )
		{
		FLOG(_L("[CFotaSrvSession] CheckClientSecureIdL client is \
		omadmappui!"));
		return EOMADMAppUi;
		}

	// Fotaserver
	if ( aMessage.SecureId() == fota.iUid ) 
		{
		FLOG(_L("[CFotaSrvSession] CheckClientSecureIdL client is \
		fotaserver!"));
		return EFotaServer;
		}

	// Starter
	if ( aMessage.SecureId() == starter.iUid )
		{
		return EStarter;
		}



	// Software Checker app
	if ( aMessage.SecureId() == softwarechecker.iUid || aMessage.SecureId() == iadclient.iUid)
        {
        return ESoftwareChecker;
        }

	// Software Checker Background app
	if ( aMessage.SecureId() == softwarecheckerbackground.iUid )
        {
        return ESoftwareCheckerBackground;
        }


// *3* <-        


	// Tester app for testing purp. 
	if ( aMessage.SecureId() == 0x0323231 || aMessage.SecureId() == testapp.iUid)
		{
		return EFotaTestApp;
		}
	if ( aMessage.SecureId() == fms.iUid )
		{
		return EFMSServer;
		}
	// schedulehandler
	if ( aMessage.SecureId() == fscheduler.iUid)
		{
		return EFotaTestApp;
		}

	FLOG(_L("[CFotaSrvSession] CheckClientSecureIdL client 0x%X is unknown \
	(msg %d), bailing out"),aMessage.SecureId().iId, aMessage.Function());
	User::Leave( KErrAccessDenied );    
	return EFotaTestApp; // compiler warning otherwise
	}


// ---------------------------------------------------------------------------
// CFotaSrvSession::CFotaSrvSession
// ---------------------------------------------------------------------------
//
CFotaSrvSession::CFotaSrvSession( )
	{
	FLOG( _L( "CFotaSrvSession::CFotaSrvSession( )" ) );
	iError =  KErrNone;
	//iDoc = CEikonEnv::Static()->EikAppUi()->Document();
	}

// ---------------------------------------------------------------------------
// CFotaSrvSession::~CFotaSrvSession
// ---------------------------------------------------------------------------
//
CFotaSrvSession::~CFotaSrvSession()
{
}

// ---------------------------------------------------------------------------
// CFotaSrvSession::CreateL
// ---------------------------------------------------------------------------
//
void CFotaSrvSession::CreateL()
	{
	FLOG( _L( "CFotaSrvSession::CreateL() >>" ) );
	CAknAppServiceBase::CreateL();		
	FLOG( _L( "CFotaSrvSession::CreateL() <<" ) );
	}

// ---------------------------------------------------------------------------
// CFotaSrvSession::ServiceL
// Handle client request
// ---------------------------------------------------------------------------
//
void CFotaSrvSession::ServiceL(const RMessage2& aMessage)
	{
	TInt        err(KErrNone);
	TInt        pkgid = 0;
	TPackageState       state;
	RThread clt; aMessage.ClientL(clt); 
	TFullName cltnm = clt.FullName();
	FLOG(_L( "CFotaSrvSession::ServiceL      %d   serving for %S?" )
			,aMessage.Function(), &cltnm );
	clt.Close();

	TInt cmd = aMessage.Function();
	if(( cmd!= EGetState ) && (cmd!= EGetResult ))
	{
		TInt fotaValue(1);    
  	CRepository* centrep( NULL);
  	TUid uidValue = {0x101F9A08}; // KCRFotaAdapterEnabled
  	centrep = CRepository::NewL( uidValue); 
  	if(centrep )
  	{     
  		FLOG(_L("centralrepository found "));
    	centrep->Get( 1 , fotaValue ); // KCRFotaAdapterEnabled     
    	delete centrep;
  	}
  	if ( ! fotaValue )
  	{
  		User::Leave( KErrNotSupported );   
  	}
  }
  
	TClient client = CheckClientSecureIdL ( aMessage );

	switch( aMessage.Function() )
	{
	case EFotaFirstCommand:
		{
		}
		aMessage.Complete(KErrNone);
		break;
	case EFotaDownload:
		{
		FLOG(_L( "CFotaSrvSession::ServiceL DOWNLOAD") );
		TDownloadIPCParams          ipc;
		TPckg<TDownloadIPCParams>   pkg(ipc);
		aMessage.Read (0, pkg);
		TInt        deslen  = aMessage.GetDesLengthL (1);
		HBufC8*     urlbuf  = HBufC8::NewLC(deslen);
		TPtr8       urlptr  = urlbuf->Des();
		aMessage.Read (1,urlptr);
		FotaServer()->DownloadL(ipc,urlptr);
		CleanupStack::PopAndDestroy(); // urlbuf
		aMessage.Complete( KErrNone );
		break;
		}
	case EFotaUpdate:
		{
		FLOG(_L( "CFotaSrvSession::ServiceL UPDATE" ));
		TDownloadIPCParams          ipc;
		TPckg<TDownloadIPCParams>   pkg(ipc);
		aMessage.Read( 0 ,pkg );
		FotaServer()->SetInstallUpdateClientL(client);
		// If update started from omadmappui, no alert should be sent if 
		// update is cancelled
		if ( client == EOMADMAppUi )
			{
			ipc.iSendAlert = EFalse;
			}
		FotaServer()->UpdateL(ipc);
		aMessage.Complete( KErrNone );	
		break;
		}
	case EFotaDownloadAndUpdate:
		{
		FLOG(_L( "CFotaSrvSession::ServiceL DOWNLOADANDUPDATE" ));
		TDownloadIPCParams          ipc;
		TPckg<TDownloadIPCParams>   pkg(ipc);
		aMessage.Read (0, pkg);
		TInt        deslen  = aMessage.GetDesLengthL (1);
		HBufC8*     urlbuf  = HBufC8::NewLC(deslen);
		TPtr8       urlptr  = urlbuf->Des();
		aMessage.Read (1,urlptr);
		FotaServer()->DownloadAndUpdateL(ipc,urlptr);
		CleanupStack::PopAndDestroy( urlbuf );
		aMessage.Complete( KErrNone );	
		}
		break;
	case EFotaOpenUpdatePackageStore:
		{
		FLOG(_L("CFotaSrvSession::ServiceL OPENUPDATEPACKAGESTORE"));
		FotaServer()->OpenUpdatePackageStoreL( aMessage );
		aMessage.Complete( KErrNone );
		}
		break;

	case EFotaGetDownloadUpdatePackageSize:
		{
		TInt dlsize = 0;
		TInt totalsize = 0;
		pkgid = aMessage.Int0();
		//err is already KErrNone.
		FotaServer()->GetDownloadUpdatePackageSizeL( pkgid, dlsize, totalsize );
		TPckg<TInt> pkg2(dlsize);
		TPckg<TInt> pkg3 (totalsize);
		aMessage.Write (1,pkg2);
		aMessage.Write (2,pkg3);
		aMessage.Complete( KErrNone );
		}
		break;

	case EFotaSendChunk:
		{
		FotaServer()->ReadChunkL( aMessage.Int0(),iError );
		iError =  KErrNone;
		aMessage.Complete( KErrNone );
		}
		break;
	case EFotaReleaseChunkHandle:
		{
		FLOG(_L("CFotaSrvSession::ServiceL  EFotaReleaseChunkHandle"));
		FotaServer()->UpdateDBdataL();
		aMessage.Complete( KErrNone );
		}
		break;

	case EFotaTryResumeDownload:
		{
		if ( (client == EOMADMAppUi) || (client == ESoftwareChecker) )
			{
			FotaServer()->TryResumeDownloadL(ETrue); // user initiated
			aMessage.Complete( KErrNone );				
			}
		else if (client == EFMSServer)
			{
			FotaServer()->TryResumeDownloadL(EFalse); // fms initiated
			aMessage.Complete( KErrNone );
			}
		else if(client == 	EFotaTestApp)
		{
			FotaServer()->TryResumeDownloadL(EFalse); // fms initiated
			aMessage.Complete( KErrNone );
		}
		else
			{
			aMessage.Complete ( KErrAccessDenied );
			}
		}
		break;	

	case EUpdatePackageDownloadComplete:
		{
		FLOG(_L("CFotaSrvSession::ServiceL \
		UPDATEPACKAGEDOWNLOADCOMPLETE"));
		// If caller is syncmlDM framework, download is largeobject 
		// download. If caller is other, download is Oma DL/http
   
		TBool largeobj = (client == EDMHostServer)? ETrue : EFalse;
		pkgid = aMessage.Int0();
		FotaServer()->UpdatePackageDownloadCompleteL(largeobj
				, pkgid);
		aMessage.Complete( KErrNone );
		}
		break;
	case EDeletePackage:
		{
		FLOG(_L( "CFotaSrvSession::ServiceL DELETEPACKAGE") );
		pkgid = aMessage.Int0();
		FotaServer()->DeletePackageL(pkgid);
		aMessage.Complete( KErrNone );
		}
		break;
	case EGetState:
		{
		FLOG(_L( "CFotaSrvSession::ServiceL GETSTATE >>" ));
		pkgid = aMessage.Int0();
		
		// NSC_DM: commented out 2 strings below, otherwise it crashes from NSC with Kern-Exec 44 -> 
		if (client != EOMADMAppUi && client != ESoftwareChecker && client != ESoftwareCheckerBackground && client != EFotaTestApp && pkgid < 0)
			aMessage.Complete (KErrArgument);
		//	<-		
		state = FotaServer()->GetStateL(pkgid);
		//Protecting state 25. If any client other than DM UI queries for state, 25 should not be sent and 
		//20 will be sent instead.
		if ( (client != EOMADMAppUi && client != ESoftwareChecker) && state.iState == RFotaEngineSession::EDownloadProgressingWithResume)
			state.iState = RFotaEngineSession::EDownloadProgressing;
		FLOG(_L( "CFotaSrvSession::ServiceL GETSTATE << %d" )
				,state.iState);
		TPckg<RFotaEngineSession::TState>   pkg2(state.iState);
		aMessage.Write (1, pkg2);
		aMessage.Complete (KErrNone);
		}
		break;
	case EGetResult:
		{
		FLOG(_L( "CFotaSrvSession::ServiceL GETRESULT >>" ));
		pkgid = aMessage.Int0();
		state = FotaServer()->GetStateL(pkgid);
		TPckg<TInt>     pkg2(state.iResult);
		FLOG(_L( "CFotaSrvSession::ServiceL GETRESULT << %d" )
				,state.iResult);
		aMessage.Write (1, pkg2);
		aMessage.Complete (err);
		break;
		}
	case EIsPackageStoreSizeAvailable:
		{
		FLOG(_L( "CFotaSrvSession::ServiceL \
		ISPACKAGESTORESIZEAVAILABLE" ));

		TInt size   = aMessage.Int0();
		TBool avail;
		// In UI evolution, size of file will be checked by fotaserver itself. Keep following
		// until codhandler skips size check itself 
		//#ifdef RD_OMADMAPPUI_UI_EVOLUTION
		if ( client == EFotaServer )	avail = ETrue;
		else	avail = FotaServer()->IsPackageStoreSizeAvailableL( size );
		/*#else
						avail = FotaServer()->IsPackageStoreSizeAvailableL( size );
			#endif*/
		TPckg<TBool>     pavail(avail);
		aMessage.Write (1, pavail);
		aMessage.Complete (KErrNone);
		break;
		}

	case EGetUpdatePackageIds:
		{
		FLOG(_L( "CFotaSrvSession::ServiceL EGETUPDATEPACKAGEIDS" ));
		TPkgIdList                      pkgids;
		FotaServer()->GetUpdatePackageIdsL(pkgids);
		TPckg<TPkgIdList>               pkgids_pkg(pkgids);
		aMessage.Write(0, pkgids_pkg);
		aMessage.Complete(KErrNone);
		}
		break;

	case EGetUpdateTimestamp:
		{
		FLOG(_L( "CFotaSrvSession::ServiceL EGETUPDATETIMESTAMP" ));
		TBuf16<15>        timestamp;
		FotaServer()->GetUpdateTimeStampL(timestamp);
		aMessage.Write(0, timestamp );
		aMessage.Complete(KErrNone);
		}
		break;

	case EGenericAlertSentForPackage:
		{
		FLOG(_L( "CFotaSrvSession::ServiceL EGENERICALERTSENT FOR PKGID" ));
		TInt pkgid = aMessage.Int0();
		FotaServer()->GenericAlertSentL( pkgid );
		aMessage.Complete( err );
		}
		break;

	case EScheduledUpdate:
		{
		FLOG(_L( "CFotaSrvSession::ServiceL ESCHEDULEDUPDATE" ));
		TFotaScheduledUpdate           sched(-1,-1);
		TPckg<TFotaScheduledUpdate>    p(sched);
		aMessage.Read (0, p);
		FotaServer()->SetInstallUpdateClientL(client);
		FLOG(_L(" pkgid: %d   scheduleid:%d"), sched.iPkgId,sched.iScheduleId);
		FotaServer()->ScheduledUpdateL( sched );
		aMessage.Complete( KErrNone );
		}
		break;

	default:
		{
		CAknAppServiceBase::ServiceL( aMessage );
		}
		break;
	}		
	}

// ---------------------------------------------------------------------------
// CFotaSrvSession::ServiceError
// ---------------------------------------------------------------------------
//
void CFotaSrvSession::ServiceError( const RMessage2& aMessage,TInt aError )
	{
	FLOG(_L( "CFotaSrvSession::ServiceError %d" ),aError );
	iError = aError;
	CFotaServer* f = FotaServer();
	TRAPD(err2, f->iParentApp->SetUIVisibleL ( EFalse,ETrue ) );
	if(err2); // remove compiler warning
	CAknAppServiceBase::ServiceError( aMessage, aError );
	}

// ---------------------------------------------------------------------------
// CFotaSrvSession::FotaServer
// Helper function
// ---------------------------------------------------------------------------
//

CFotaServer*  CFotaSrvSession::FotaServer() const
{
return (CFotaServer* ) Server();
}
