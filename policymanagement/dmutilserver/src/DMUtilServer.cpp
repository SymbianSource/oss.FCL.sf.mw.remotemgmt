/*
* Copyright (c) 2000 Nokia Corporation and/or its subsidiary(-ies). 
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



// INCLUDES

#include <e32svr.h>
#include <centralrepository.h>

#ifdef __TARM_SYMBIAN_CONVERGENCY
const TUid KUidSmlSyncAgentCategory = { 0x10009F46 }; // temporary
const TUint KHttpsServerCertificateKey = 1234; // temporary
#else
// nothing
#endif

#include <SyncMLClient.h>
#include <SyncMLClientDM.h>
 
#include "ACLStorage.h"
#include "DMUtilServer.h"
#include "DMUtilSession.h"
#include "debug.h"


// CONSTANTS

/**
    Ranges of policies
*/
const TInt   CDMUtilServer::tcRanges[DMUTILSERVER_NUMBER_OF_POLICIES] = {0, EPerformDMUtilRFS, ERemoveACL};

/**
    Number of elements
*/
const TUint8 CDMUtilServer::tcElementsIndex[DMUTILSERVER_NUMBER_OF_POLICIES] = {0, 1, 2};

/**
    Elements for each range
*/    
const CPolicyServer::TPolicyElement CDMUtilServer::tcElements[DMUTILSERVER_NUMBER_OF_POLICIES] =
	    {
        { _INIT_SECURITY_POLICY_PASS },
        { _INIT_SECURITY_POLICY_S0(0x102073EA) },	// RFs SID
        { _INIT_SECURITY_POLICY_S0(0x10207814) }
	    };

/**
    The policy 
*/
CPolicyServer::TPolicy CDMUtilServer::iTcConnectionPolicy =
    {
        CPolicyServer::ECustomCheck, // On connection, check for policies
        1,
        tcRanges,
        tcElementsIndex,
        tcElements
    };


// MACROS

#define __INCLUDE_CAPABILITY_NAMES__

// DATA TYPES
// FUNCTION PROTOTYPES
// FORWARD DECLARATIONS

CACLStorage* CDMUtilServer::iACLs = 0;
TBool CDMUtilServer::iManagementActive = EFalse;




// ----------------------------------------------------------------------------------------
// Server startup code
// ----------------------------------------------------------------------------------------
static void RunServerL()
	{
	// naming the server thread after the server helps to debug panics
	//User::LeaveIfError(RThread().Rename(KDMUtilServerName));   //Deprecated! Replace this with the line below as soon as possible
	User::LeaveIfError(User::RenameThread(KDMUtilServerName)); //Correct PlatSec function, not available until week 2004_32

	// create and install the active scheduler

	CActiveScheduler* s=new(ELeave) CActiveScheduler;
	CleanupStack::PushL(s);
	CActiveScheduler::Install(s);

	// create the server (leave it on the cleanup stack)
	CDMUtilServer::NewLC();
	// Initialisation complete, now signal the client

	RProcess::Rendezvous(KErrNone);

	// Ready to run
	RDEBUG("DMUtilServer is running");
	CActiveScheduler::Start();
	RDEBUG("DMUtilServer has stopped");

	// Cleanup the server and scheduler
	CleanupStack::PopAndDestroy(2);
	}

// Server process entry-point
TInt E32Main()
	{
	__UHEAP_MARK;
	RDEBUG("DMUtilServer: E32Main");
	CTrapCleanup* cleanup=CTrapCleanup::New();
	TInt r=KErrNoMemory;
	if (cleanup)
		{
		TRAP(r,RunServerL());
		RDEBUG_2("DMUtilServer closed with code: %d", r);
		delete cleanup;
		}
	__UHEAP_MARKEND;
	return r;
	}
    
// RMessagePtr2::Panic() also completes the message. This is:
// (a) important for efficient cleanup within the kernel
// (b) a problem if the message is completed a second time
void PanicClient(const RMessagePtr2& aMessage,TDMUtilPanic aPanic)
	{
	_LIT(KPanic,"DMUtilServer");
	aMessage.Panic(KPanic,aPanic);
	}

// ----------------------------------------------------------------------------------------
// CDMUtilServer
// ----------------------------------------------------------------------------------------
CDMUtilServer::CDMUtilServer()
	:CPolicyServer(0, 
    iTcConnectionPolicy,
	ESharableSessions),
	iServerID(0)
	{
	}

// ----------------------------------------------------------------------------------------
// CDMUtilServer::CertificateChangedL
// ----------------------------------------------------------------------------------------
void CDMUtilServer::CertificateChangedL()
    {
    RDEBUG("DMUtilServer: CDMUtilServer::CertificateChangedL");
    TRAPD( reason, UpdateSessionInfoL() );
    if( KErrNone != reason )
        {
        RDEBUG("DMUtilServer: ERROR Failed to update certificate!");
        iACLs->CloseSession();
		iManagementActive = EFalse;
        CDMUtilSession::SetIndicatorStateL( 0 );
        }
    else
        {
        RDEBUG("DMUtilServer: Starting new session, updating ACLs");
        UpdateACLsL();
        }
    }

// ----------------------------------------------------------------------------------------
// CDMUtilServer::NotifierErrorL
// ----------------------------------------------------------------------------------------
void CDMUtilServer::NotifierErrorL( TInt aCode )
    {
    RDEBUG_2("CDMUtilServer::NotifierErrorL Certificate notifier error %d", aCode);

    if( KErrAbort == aCode )
        {
        RDEBUG("**** DMUtilServer::NotifierErrorL KErrAbort == aCode");
        iNotifier.StartL( this );
        }
    }

// ----------------------------------------------------------------------------------------
// CDMUtilServer::NewLC
// ----------------------------------------------------------------------------------------
CServer2* CDMUtilServer::NewLC()
	{    
	RDEBUG("CDMUtilServer::NewLC - start");
	
	CDMUtilServer* self=new(ELeave) CDMUtilServer;
	CleanupStack::PushL(self);
	self->ConstructL();
	RDEBUG("CDMUtilServer::NewLC - end");
	return self;
	}

// ----------------------------------------------------------------------------------------
// CDMUtilServer::~CDMUtilServer
// ----------------------------------------------------------------------------------------
CDMUtilServer::~CDMUtilServer()
	{
	RDEBUG("CDMUtilServer::~CDMUtilServer()");
    delete iServerID; iServerID=0;
    delete iACLs;
	}

// ----------------------------------------------------------------------------------------
// CDMUtilServer::ConstructL
// 2nd phase construction - ensure the timer and server objects are running
// ----------------------------------------------------------------------------------------
void CDMUtilServer::ConstructL()
	{
	RDEBUG("CDMUtilServer::ConstructL");
	StartL(KDMUtilServerName);
	
	CDMUtilSession::DefinePropertiesL();

    iACLs = CACLStorage::NewL();

    TRAPD( reason, UpdateSessionInfoL() );
    if( KErrNone != reason )
        {
        RDEBUG("DMUtilServer: Failed to update certificate");
        }
    else
        {
        RDEBUG("	CDMUtilServer::ConstructL	UPDATING ACLs ....");
        UpdateACLsL();
        RDEBUG("	CDMUtilServer::ConstructL	UPDATING ACLs ....DONE!");
        }
            
    iNotifier.StartL( this );
    }

// ----------------------------------------------------------------------------------------
// CDMUtilServer::UpdateSessionInfoL
// ----------------------------------------------------------------------------------------
void CDMUtilServer::UpdateSessionInfoL()
    {
	RDEBUG("CDMUtilServer::UpdateSessionInfoL");
    TPckg<TCertInfo> certp( iCertInfo );
    certp.Fill( TChar(0) );
    delete iServerID; iServerID=0;

    iServerID = GetServerIDL();
    if( iServerID != 0 )
        {
        if( KErrNone != GetCertFromCentRep() )
            {
            RDEBUG("**** CDMUtilServer::UpdateSessionInfoL - unable to get certificate from cenrep");
            delete iServerID; iServerID=0;
            }
        else
        	{
        	if ( IsSilentModeAllowed() )
            	{
            	iManagementActive = ETrue;
            	}
			CDMUtilSession::SetIndicatorStateL(0);
            }
            
        }
    }



CACLStorage * CDMUtilServer::ACLStorage()
	{
	return iACLs;
	}


// ----------------------------------------------------------------------------------------
// CDMUtilServer::GetServerIDL
// ----------------------------------------------------------------------------------------
HBufC8* CDMUtilServer::GetServerIDL()
    {
    HBufC8* serverId = NULL;
    
	TRAPD( err, serverId = GetServerIDFromSyncMLSessionL() );
	if( err == KErrNone )
		{
		return serverId;
		}
		
	serverId = GetServerIDFromLocalCacheL();
	if( serverId == NULL )
		{
		User::Leave( KErrNotFound );
		}
	
	return serverId;
    }

HBufC8* CDMUtilServer::GetServerIDFromSyncMLSessionL()
	{
#ifdef __TARM_SYMBIAN_CONVERGENCY
	RDEBUG("CDMUtilServer::GetServerIDFromSyncMLSessionL() ......");
    HBufC8* serverIdStr = 0;

	RSyncMLSession ses;
	ses.OpenL();
	CleanupClosePushL( ses );

	TSmlJobId jid;
	TSmlUsageType jtype;
    ses.CurrentJobL( jid, jtype );
    RSyncMLDevManJob job;
    job.OpenL( ses, jid );
    CleanupClosePushL( job );

	TSmlProfileId pid( job.Profile() );

	RSyncMLDevManProfile profile;
	profile.OpenL( ses, pid );
	CleanupClosePushL( profile );

	serverIdStr = profile.ServerId().AllocL();

	CleanupStack::PopAndDestroy( &profile );
	CleanupStack::PopAndDestroy( &job );
	CleanupStack::PopAndDestroy( &ses );

	RDEBUG("CDMUtilServer::GetServerIDFromSyncMLSessionL ......DONE!");
	return serverIdStr;
#else
RDEBUG("CDMUtilServer::GetServerIDFromSyncMLSessionL ......");
    HBufC8* serverIdStr = 0;

	RSyncMLSession ses;
	ses.OpenL();
	CleanupClosePushL( ses );

	TSmlJobId jid;
	TSmlUsageType jtype;
    ses.CurrentJobL(jid, jtype );
    RSyncMLDevManJob job;
    job.OpenL( ses, jid );
    CleanupClosePushL( job );

    TSmlProfileId pid( job.Profile() );
    RSyncMLDevManProfile prof;
    prof.OpenL( ses, pid, ESmlOpenRead );
    CleanupClosePushL( prof );

	serverIdStr = prof.ServerId().AllocL();

	CleanupStack::PopAndDestroy( &prof );
	CleanupStack::PopAndDestroy( &job );
	CleanupStack::PopAndDestroy( &ses );

	RDEBUG("CDMUtilServer::GetServerIDFromSyncMLSessionL ......DONE!");
	return serverIdStr;
#endif
	}

HBufC8* CDMUtilServer::GetServerIDFromLocalCacheL()
	{
	RDEBUG("CDMUtilServer::GetServerIDFromLocalCacheL ......");
	if( iServerID )
		{
		RDEBUG("CDMUtilServer::GetServerIDFromLocalCacheL ......DONE!");
		return iServerID->AllocL();
		}
	
	RDEBUG("CDMUtilServer::GetServerIDFromLocalCacheL ...... **** NOT FOUND!");
	return NULL;
	}

// ----------------------------------------------------------------------------------------
// CDMUtilServer::GetCertFromCentRep
// ----------------------------------------------------------------------------------------
TInt CDMUtilServer::GetCertFromCentRep()
    {
	RDEBUG("CDMUtilServer::GetCertFromCentRep");
	TInt result = KErrNone;
	
#ifdef __TARM_SYMBIAN_CONVERGENCY
	result = iNotifier.GetCert( iCertInfo );
	if ( result != KErrNone ) 
		{
		RDEBUG_2("**** CDMUtilServer::GetCertFromCentRep ... NOT FOUND: %d", result );
		result = KErrNotFound;
		}
#else	
    TCertInfo info;
	CRepository *rep = NULL;
	TRAP( result, rep = CRepository::NewL ( KCRUidPolicyManagementUtilInternalKeys ) );
	if (result == KErrNone )
		{
		TPckg<TCertInfo> pcert( iCertInfo );
		TInt len( 0 );
		result = rep->Get( KSyncMLSessionCertificate, pcert, len );
		delete rep;
		if ( len == 0 ) 
			{
			RDEBUG("**** CDMUtilServer::GetCertFromCentRep ... NOT FOUND");
			result = KErrNotFound;
			}
		}
#endif	

	return result;
    }


// ----------------------------------------------------------------------------------------
// CDMUtilServer::IsSilentModeAllowed
// ----------------------------------------------------------------------------------------
TBool CDMUtilServer::IsSilentModeAllowed()
    {
	RDEBUG("CDMUtilServer::IsSilentModeAllowed()");
	CRepository *rep = NULL;
    TInt result = KErrNone;
	TRAP( result, rep = CRepository::NewL ( KCRUidPolicyManagementUtilInternalKeys ) );
	if (result == KErrNone )
		{
		TInt state( 0 );
		result = rep->Get( KSyncMLForcedCertificateCheckKey, state );
		delete rep;
		if ( state == 1 ) 
			{
			RDEBUG("CDMUtilServer::IsSilentModeAllowed ... allowed");
			return ETrue;
			}
		else
			{
			RDEBUG("CDMUtilServer::IsSilentModeAllowed ... NOT allowed");
			}
		}
	return EFalse;
    }


// ----------------------------------------------------------------------------------------
// CDMUtilServer::UpdateACLs
// ----------------------------------------------------------------------------------------
TInt CDMUtilServer::UpdateACLsL()
    {
    RDEBUG("CDMUtilServer::UpdateACLsL()");
    
    if( NULL != iServerID )
        {
        TRAPD( reason, iACLs->NewSessionL( iCertInfo, *iServerID ) );
        RDEBUG8_3("CDMUtilServer::UpdateACLs() ... iACLs->NewSessionL = %S with error: %d", &iServerID, reason);
        return reason;
        }
    else
        {
       	iACLs->CloseSession();
	   	iManagementActive = EFalse;
       	CDMUtilSession::SetIndicatorStateL( 0 );
        }
        
    return KErrNone;
    }

// ----------------------------------------------------------------------------------------
// CDMUtilServer::NewSessionL
// Create a new client session. This should really check the version number.
// ----------------------------------------------------------------------------------------
CSession2* CDMUtilServer::NewSessionL(const TVersion&,const RMessage2&) const
	{
	RDEBUG("CDMUtilServer::NewSessionL");
	return new (ELeave) CDMUtilSession();
	}

// ----------------------------------------------------------------------------------------
// CDMUtilServer::CustomSecurityCheckL
// ----------------------------------------------------------------------------------------
CPolicyServer::TCustomResult CDMUtilServer::CustomSecurityCheckL(const RMessage2& /*aMsg*/, TInt& /*aAction*/, TSecurityInfo& /*aMissing*/)
	{
	RDEBUG("CDMUtilServer::CustomSecurityCheckL");
	return EPass;
	}
	



// ----------------------------------------------------------------------------------------
// ----------------------------------------------------------------------------------------
// ----------------------------------------------------------------------------------------


// ----------------------------------------------------------------------------------------
// CDMCertNotifier::CDMCertNotifier
// ----------------------------------------------------------------------------------------
CDMCertNotifier::CDMCertNotifier()
    : CActive( EPriorityStandard )
    , iCallback( 0 )
#ifdef __TARM_SYMBIAN_CONVERGENCY  
// nothing
#else    
    , iRepository(0)
#endif    
    {
	RDEBUG("CDMCertNotifier::CDMCertNotifier - start");
    CActiveScheduler::Add( this );
    
#ifdef __TARM_SYMBIAN_CONVERGENCY //  
    TInt err = iCertificateProperty.Attach( KUidSmlSyncAgentCategory, KHttpsServerCertificateKey );
    RDEBUG_2("CDMCertNotifier::CDMCertNotifier() - Attached to P&S key with code: %d", err );
    __ASSERT_ALWAYS( err == KErrNone, User::Panic( _L("DMUtilServer P&S attach"), err ) );
#else
// nothing
#endif    
	RDEBUG("CDMCertNotifier::CDMCertNotifier - end");
    }

// ----------------------------------------------------------------------------------------
// CDMCertNotifier::~CDMCertNotifier
// ----------------------------------------------------------------------------------------
CDMCertNotifier::~CDMCertNotifier()
    {
	RDEBUG("CDMCertNotifier::~CDMCertNotifier");
#ifdef __TARM_SYMBIAN_CONVERGENCY  
	iCertificateProperty.Close();
#else
	delete iRepository; 
    iRepository = 0;
#endif	
    }

// ----------------------------------------------------------------------------------------
// CDMCertNotifier::Start
// ----------------------------------------------------------------------------------------

#ifdef __TARM_SYMBIAN_CONVERGENCY  
void CDMCertNotifier::StartL( MDMCertNotifierCallback* aCallback )
	{
	RDEBUG("CDMCertNotifier::StartL.....");
	
	if( IsActive() )
        {
        Cancel();
        }

	iCallback = aCallback;
	
	// initial cache reset
	ResetCertCache();
	
	TCertInfo ci;
    TPckg<TCertInfo> certp( ci );
    certp.Fill( TChar( 0 ) );
    
    TInt err = iCertificateProperty.Get( certp );
    if( err == KErrNone )
    	{
    	RDEBUG("	initial certificate found, saving to cache .. ");
    	// content found, save to cache
    	SaveCertToCache();
    	RDEBUG("	initial certificate found, saving to cache .. done");
    	}
    
    // ask for notifications in certificate changes
    iCertificateProperty.Subscribe( iStatus );
    SetActive();
    RDEBUG("CDMCertNotifier::StartL.....end");
	}
#else // Nokia SyncML framework used
void CDMCertNotifier::StartL( MDMCertNotifierCallback* aCallback )
    {
	RDEBUG("CDMCertNotifier::StartL.....");
    if( IsActive() )
        {
        Cancel();
        }

    if( !IsActive() )
        {
        if( aCallback != 0)
            {
            iCallback = aCallback;
            }

        delete iRepository; iRepository = 0;
        iRepository = CRepository::NewL ( KCRUidPolicyManagementUtilInternalKeys );
		// initial cache reset
		ResetCertCache();
        
        TCertInfo ci;
        TPckg<TCertInfo> certp( ci );
        certp.Fill( TChar(0) );

        TInt result = iRepository->Get( KSyncMLSessionCertificate, certp );
        if( KErrNone == result )
            {
            // get initial cache content
            SaveCertToCache();
            if( KErrNone == iRepository->NotifyRequest( KSyncMLSessionCertificate, iStatus ) )
                {
                SetActive();
                }
            }
        else
            {
            RDEBUG_2("**** CDMCertNotifier::StartL ERROR Failed to Get certificate repository key: %d", result);
            User::Leave( result );
            }
        }
    else
        {
    	RDEBUG("**** CDMCertNotifier::StartL: ERROR failed to start notifier");
        }
    RDEBUG("CDMCertNotifier::StartL....DONE!");
    }
#endif



void CDMCertNotifier::RunLToBeTrappedL()
	{
	RDEBUG_2("CDMCertNotifier::RunLToBeTrappedL( %d )", iStatus.Int() );

	TInt status = iStatus.Int();

#ifdef __TARM_SYMBIAN_CONVERGENCY  

	// resubscribe before processing new value to prevent missing updates
	iCertificateProperty.Subscribe( iStatus );
	SetActive();
#else
// nothing, see the end of this function
#endif

    // Report status to callback
    if( iCallback != 0 )
        {
#ifdef __TARM_SYMBIAN_CONVERGENCY  
		if( status == KErrNone )
#else        
        if( status == KSyncMLSessionCertificate )
#endif        
            {
            TSessionCertStatus certStatus = GetCertStatus();
            
            if( certStatus == ESessionCertContentZero )
            	{
            	// this case indicates that the DM session has been terminated
            	// -> clear the certificate cache
            	RDEBUG("	<-> session certificate notification - cert data reseted - reset cert cache");
            	ResetCertCache();
            	// this will eventually cause the indicator flag update
            	iCallback->CertificateChangedL();
            	}
            else
            if( certStatus == ESessionCertHasChanged )
            	{
            	RDEBUG("CDMCertNotifier::RunLToBeTrappedL: DM Certificate has changed");
            	// cache the new session certificate data
            	RDEBUG("	<-> session certificate notification - cert data changed - save cert to cache");
            	SaveCertToCache();
            	// notify (to update ACLs)
            	RDEBUG("	<-> session certificate notification - cert data changed - notify call-back");
            	iCallback->CertificateChangedL();
            	}
            else
            if( certStatus == ESessionCertNoChange )
            	{
            	// nothing to do <-> if the session certificate has not changed -> we have
            	// no need to update the ACLs
            	RDEBUG("	<-> session certificate notification - no change in the cert data - nothing to do");
            	}
            }
        else if( 0 > status )
            {
            RDEBUG("CDMCertNotifier::RunLToBeTrappedL: iCallback->NotifierErrorL( KErrGeneral );");
            iCallback->NotifierErrorL( status );
            }
        }

#ifdef __TARM_SYMBIAN_CONVERGENCY  
// nothing, see the start of this function
#else
    // Start again anyway ...
    if( iRepository != 0 )
        {
        RDEBUG("CDMCertNotifier::RunLToBeTrappedL: iRepository->NotifyRequest( KSyncMLSessionCertificate, iStatus ) );");
        TInt err( iRepository->NotifyRequest( KSyncMLSessionCertificate, iStatus ) );
        if( KErrNone == err )
            {
            RDEBUG("CDMCertNotifier::RunLToBeTrappedL: Setting active...");
            SetActive();
            }
        else
            {
            if( iCallback != 0)
                {
                RDEBUG("CDMCertNotifier::RunLToBeTrappedL: iCallback->NotifierErrorL( KErrAbort );");
                iCallback->NotifierErrorL( KErrAbort );
                }
            }
        }
#endif
	}


// ----------------------------------------------------------------------------------------
// CDMCertNotifier::RunL
// ----------------------------------------------------------------------------------------
void CDMCertNotifier::RunL()
    {
	RDEBUG("CDMCertNotifier::RunL");
    
    TRAPD( err, RunLToBeTrappedL());
   	
   	if( err != KErrNone )
   		{
   		ResetCertCache();
   		RDEBUG_2("**** CDMCertNotifier::RunL - LEAVE: %d", err);
   		}
    }

// ----------------------------------------------------------------------------------------
// CDMCertNotifier::DoCancel
// ----------------------------------------------------------------------------------------
void CDMCertNotifier::DoCancel()
    {
	RDEBUG("CDMCertNotifier::DoCancel");
#ifdef __TARM_SYMBIAN_CONVERGENCY
	iCertificateProperty.Cancel();	
	ResetCertCache();
#else	
    if( iRepository != 0 )
        {
        ResetCertCache();
        iRepository->NotifyCancel( KSyncMLSessionCertificate );
        }
#endif        
    }

// ----------------------------------------------------------------------------------------



void CDMCertNotifier::SaveCertToCache()
	{
	TPckg<TCertInfo> certp( iCachedSessionCertificate );
	
#ifdef __TARM_SYMBIAN_CONVERGENCY
    TInt err = iCertificateProperty.Get( certp );
#else	
	TInt err = iRepository->Get( KSyncMLSessionCertificate, certp );
#endif
	
	if( err != KErrNone )
		{
		RDEBUG_2("**** Failed to save certificate data to cache: %d", err );
		}		
	}
	
CDMCertNotifier::TSessionCertStatus CDMCertNotifier::GetCertStatus()
	{
	TCertInfo ci;
    TPckg<TCertInfo> certp( ci );
    certp.Fill( TChar( 0 ) );

#ifdef __TARM_SYMBIAN_CONVERGENCY
	TInt result = iCertificateProperty.Get( certp );
#else    
    TInt result = iRepository->Get( KSyncMLSessionCertificate, certp );
#endif
    
    if( result != KErrNone )
    	{
    	RDEBUG_2("**** Failed to fetch certificate data from centrep: %d", result );
    	return ESessionCertNoChange;
    	}
    	
    if( IsEmpty( ci ) )
    	{
    	RDEBUG("	Session certificate content is empty - DM session closed");
    	return ESessionCertContentZero;
    	}
    	
    if( !IsEqual( ci, iCachedSessionCertificate ) )
    	{
    	RDEBUG("	Session certificate data has changed");
    	return ESessionCertHasChanged;
    	}
    	
    return ESessionCertNoChange;
	}
	
void CDMCertNotifier::ResetCertCache()
	{
    TPckg<TCertInfo> certp( iCachedSessionCertificate );
    certp.Fill( TChar( 0 ) );
	}


TBool CDMCertNotifier::IsEmpty( const TCertInfo& aCertInfo ) const
	{
	/*
	TPckg<TCertInfo> certp1( aCertInfo );
	if( certp1.Length() == 0 )
		{
		return ETrue;
		}
	*/
	if( aCertInfo.iFingerprint.Length() == 0 )
		{
		return ETrue;
		}
		
	return EFalse;
	}
	
TBool CDMCertNotifier::IsEqual( const TCertInfo& aCertInfo1, 
								const TCertInfo& aCertInfo2 ) const
	{
	TInt result = aCertInfo1.iFingerprint.Compare( aCertInfo2.iFingerprint );
	if( result != 0 )
		{
		return EFalse;
		}
	
	result = aCertInfo1.iSerialNo.Compare( aCertInfo2.iSerialNo );
	if( result != 0 )
		{
		return EFalse;
		}
	
	if( aCertInfo1.iVersionNo != aCertInfo2.iVersionNo )
		{
		return EFalse;
		}

	return ETrue;
	}	


TInt CDMCertNotifier::GetCert( TCertInfo& aCertInfo )
    {
	RDEBUG("CDMUtilServer::GetCertFromCentRep");
	
#ifdef __TARM_SYMBIAN_CONVERGENCY
	TPckg<TCertInfo> pcert( aCertInfo );
	pcert.Fill( TChar( 0 ) );
	return iCertificateProperty.Get( pcert );
#else	
    TPckg<TCertInfo> certp( aCertInfo );
    certp.Fill( TChar( 0 ) );
	return iRepository->Get( KSyncMLSessionCertificate, certp );
#endif
    }

// End of file





