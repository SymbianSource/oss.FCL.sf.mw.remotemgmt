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

#include "PolicyEngineServer.h"
#include "PolicyParser.h"
#include "TrustedSession.h"
#include "PolicyStorage.h"
#include "PolicyManager.h"
#include "PolicyProcessor.h"
#include "PolicyEngineClientServerDefs.h"
#include "debug.h"

#include <e32svr.h>
#include <f32file.h>
#include "DMUtilClient.h"


CPolicyEngineServer* CPolicyEngineServer::iServer = 0;

#ifdef _DEBUG
namespace DebugUtility
{
	_LIT( PolicyCheckFile, "c:\\policy_check_off_file.txt");
	
	TBool PolicyCheckActive()
	{
		TBool response = ETrue;
	
		RFs rfs;
		TInt err = rfs.Connect();
		if( err != KErrNone )
			{
			return EFalse;
			}
			
		RFile file;
		
		//Check is file exist
		if ( KErrNone == file.Open( rfs, PolicyCheckFile, EFileRead))
		{
			response = EFalse;
		}
		
		file.Close();
		rfs.Close();
		
		return response;
	}
}
#endif


// ----------------------------------------------------------------------------------------
// Server startup code
// ----------------------------------------------------------------------------------------

static void RunServerL()
	{
	// naming the server thread after the server helps to debug panics
	User::LeaveIfError(User::RenameThread(KPolicyEngineServerName)); 

	// create and install the active scheduler
	CActiveScheduler* s=new(ELeave) CActiveScheduler;
	CleanupStack::PushL(s);
	CActiveScheduler::Install(s);
	
	// create the server (leave it on the cleanup stack)
	CPolicyEngineServer::NewLC();
	// Initialisation complete, now signal the client

	RProcess::Rendezvous(KErrNone);

	// Ready to run
	RDEBUG("Policy engine server is running");
	
	CActiveScheduler::Start();
	
	// Cleanup the server and scheduler
	CleanupStack::PopAndDestroy(2);
	}

// Server process entry-point
TInt E32Main()
	{
	__UHEAP_MARK;
	RDEBUG( "PolicyEngineServer: E32Main");
	CTrapCleanup* cleanup=CTrapCleanup::New();
	TInt r=KErrNoMemory;
	if (cleanup)
		{
		TRAP(r,RunServerL());
		delete cleanup;
		}
	__UHEAP_MARKEND;
	return r;
	}
    
// ----------------------------------------------------------------------------------------
// PanicClient
// ----------------------------------------------------------------------------------------
    
void PanicClient(const RMessagePtr2& aMessage,TPolicyEnginePanic aPanic)
	{
	_LIT(KPanic,"PolicyEngineServer");
	aMessage.Panic(KPanic,aPanic);
	}

// ----------------------------------------------------------------------------------------
// CShutDown::CShutdown()
// ----------------------------------------------------------------------------------------

inline CShutdown::CShutdown()
	:CTimer(-1)
	{
	CActiveScheduler::Add(this);
	}
	
// ----------------------------------------------------------------------------------------
// CShutDown::ConstructL()
// ----------------------------------------------------------------------------------------

inline void CShutdown::ConstructL()
	{
	CTimer::ConstructL();
	}

// ----------------------------------------------------------------------------------------
// CShutDown::Start()
// ----------------------------------------------------------------------------------------

inline void CShutdown::Start()
	{
	RDEBUG("PolicyEngineServer: starting selfcheck timeout");
	iSelfCheckCounter = 0;
	After( KPolicyEngineSelfCheckDelay);
	}

// ----------------------------------------------------------------------------------------
// CShutDown::RunL()
// ----------------------------------------------------------------------------------------

void CShutdown::RunL()
{
	RDEBUG("Policy engine server selfcheck timeout...");
	iSelfCheckCounter++;
	
	
	if ( iSelfCheckCounter == KPolicyEngineReleaseElementsDelay)
	{
		RDEBUG("PolicyEngineServer: Release cache elements...");
		CPolicyStorage::PolicyStorage()->ReleaseElements();
	} 
	
	if ( iSelfCheckCounter >= KPolicyEngineShutdownDelay)
	{
		RDEBUG("PolicyEngineServer: Shutdown...");
		CActiveScheduler::Stop();
	}
	else
	{
		After( KPolicyEngineSelfCheckDelay);
	}
	
}

// ----------------------------------------------------------------------------------------
// CPolicyEngineServer::CPolicyEngineServer
// ----------------------------------------------------------------------------------------

inline CPolicyEngineServer::CPolicyEngineServer()
	: CPolicyServer(0, KPolicyEngineSecurityPolicy, ESharableSessions)
	{
		CPolicyEngineServer::iServer = this;
	}

// ----------------------------------------------------------------------------------------
// CPolicyEngineServer::~CPolicyEngineServer
// ----------------------------------------------------------------------------------------
	
CPolicyEngineServer::~CPolicyEngineServer()
{
	RDEBUG("CPolicyEngineServer::~CPolicyEngineServer()");
	delete iPolicyStorage;
	delete iCertificateMaps;
	delete iFlags;
	delete iContainerIndex;
	
	iActiveObjectCancelList.Close();
}

// ----------------------------------------------------------------------------------------
// CPolicyEngineServer::NewLC()
// ----------------------------------------------------------------------------------------
	
CServer2* CPolicyEngineServer::NewLC()
{
	RDEBUG("PolicyEngineServer: CPolicyEngineServer::NewLC");

	CPolicyEngineServer* self=new(ELeave) CPolicyEngineServer;
	CleanupStack::PushL(self);

	self->ConstructL();
	return self;
}

// ----------------------------------------------------------------------------------------
// CPolicyEngineServer::ConstructL()
// ----------------------------------------------------------------------------------------

void CPolicyEngineServer::ConstructL()
	{
	RDEBUG("CPolicyEngineServer::ConstructL() begin");
	// 2nd phase construction - ensure the timer and server objects are running

	StartL (KPolicyEngineServerName);
	iContainerIndex = CObjectConIx::NewL();
	//create storage and set static reference
	iPolicyStorage = CPolicyStorage::NewL();
	CPolicyStorage::iPolicyStorage = iPolicyStorage;
	
	iCertificateMaps = CCertificateMaps::NewL();
	iFlags = new (ELeave) CServerStatusFlags();
	iFlags->iUiActive =EFalse;
	iFlags->iManagementActive =EFalse;
	iFlags->iProcessorActive =EFalse;

	iShutdown.ConstructL();
	// ensure that the server still exits even if the 1st client fails to connect
	iShutdown.Start();
	RDEBUG("CPolicyEngineServer::ConstructL() end");
	}

// ----------------------------------------------------------------------------------------
// CPolicyEngineServer::NewSessionL()
// ----------------------------------------------------------------------------------------

CSession2* CPolicyEngineServer::NewSessionL( const TVersion&, const RMessage2&) const
	{
		// Create a new client session. This should really check the version number	
		
		RDEBUG("PolicyEngineServer: CPolicyEngineServer::NewSessionL");
		
		return new (ELeave) CPolicyEngineSession();
	}

// ----------------------------------------------------------------------------------------
// CPolicyEngineServer::AddSession()
// ----------------------------------------------------------------------------------------

void CPolicyEngineServer::AddSession()
{
	// A new session is being created
	RDEBUG("PolicyEngineServer: CPolicyEngineServer::AddSession");
	++iSessionCount;
	
	// Cancel the shutdown timer if it was running
	iShutdown.Cancel();
}


// ----------------------------------------------------------------------------------------
// CPolicyEngineServer::DropSession()
// ----------------------------------------------------------------------------------------

void CPolicyEngineServer::DropSession()
{
	// A session is being destroyed
	RDEBUG("PolicyEngineServer: CPolicyEngineServer::DropSession");

	// Start the shutdown timer if it is the last session
	if (--iSessionCount==0)
	{
		iShutdown.Start();
	}
}

// ----------------------------------------------------------------------------------------
// CPolicyEngineServer::DropSession()
// ----------------------------------------------------------------------------------------

CServerStatusFlags* CPolicyEngineServer::StatusFlags()
{
	return CPolicyEngineServer::iServer->iFlags;
}

// ----------------------------------------------------------------------------------------
// CPolicyEngineServer::NewContainerL()
// ----------------------------------------------------------------------------------------

CObjectCon* CPolicyEngineServer::NewContainerL()
{
	//create new container and return pointer
	return iContainerIndex->CreateL();
}


// ----------------------------------------------------------------------------------------
// CPolicyEngineServer::CertificateMaps()
// ----------------------------------------------------------------------------------------
CCertificateMaps * CPolicyEngineServer::CertificateMaps()
{
	//return pointer to global certificate maps instance
	return CPolicyEngineServer::iServer->iCertificateMaps;
}


// ----------------------------------------------------------------------------------------
// CPolicyEngineServer::SetActiveSubSession()
// ----------------------------------------------------------------------------------------
void CPolicyEngineServer::SetActiveSubSession( const CBase* iBase)
{
	RDEBUG("CPolicyEngineServer::SetActiveSubSession()");
	CPolicyEngineServer::iServer->iSubSessionObjects = 0;
	
	CPolicyEngineServer::iServer->iSessionIter.SetToFirst();
	for ( ; CPolicyEngineServer::iServer->iSessionIter != 0 ; CPolicyEngineServer::iServer->iSessionIter++)
	{
		CSession2 * session = CPolicyEngineServer::iServer->iSessionIter;
		CPolicyEngineSession* policyenginesession = (CPolicyEngineSession*) session;
		CSubSessionObjects* objects = policyenginesession->GetSubSessionObjects( iBase);
		
		if ( objects )
		{
			CPolicyEngineServer::iServer->iSubSessionObjects = objects;
			break;
		}
	}
}





// ----------------------------------------------------------------------------------------
// CPolicyEngineServer::AddActiveObjectL()
// ----------------------------------------------------------------------------------------
void CPolicyEngineServer::AddActiveObjectL( CActive* aActiveObject)
{
	CPolicyEngineServer::iServer->iActiveObjectCancelList.AppendL( aActiveObject);
}

// ----------------------------------------------------------------------------------------
// CPolicyEngineServer::RemoveActiveObject()
// ----------------------------------------------------------------------------------------
void CPolicyEngineServer::RemoveActiveObject( CActive* aActiveObject)
{
	for ( TInt i(0); i < CPolicyEngineServer::iServer->iActiveObjectCancelList.Count(); i++)
	{
		if ( aActiveObject == CPolicyEngineServer::iServer->iActiveObjectCancelList[i])
		{
			CPolicyEngineServer::iServer->iActiveObjectCancelList.Remove( i);
			break;
		}
	}
}


// ----------------------------------------------------------------------------------------
// CPolicyEngineServer::RemoveActiveObject()
// ----------------------------------------------------------------------------------------
void CPolicyEngineServer::StopAllPendingRequests()
{
	RDEBUG("CPolicyEngineServer::StopAllPendingRequests()");
	for ( TInt i(0); i < CPolicyEngineServer::iServer->iActiveObjectCancelList.Count(); i++)
	{
		TInt count = CPolicyEngineServer::iServer->iActiveObjectCancelList.Count();
		CPolicyEngineServer::iServer->iActiveObjectCancelList[i]->Cancel();
		
		//if request cancelling has caused some changes in active object list, then start list from beginning
		if ( CPolicyEngineServer::iServer->iActiveObjectCancelList.Count() != count)
		{
			i = 0;
		}
	}
}



// ----------------------------------------------------------------------------------------
// CPolicyEngineServer::CustomSecurityCheckL()
// ----------------------------------------------------------------------------------------
CPolicyServer::TCustomResult CPolicyEngineServer::CustomSecurityCheckL(const RMessage2& aMsg, TInt& /*aAction*/, TSecurityInfo& /*aMissing*/)
{
	RDEBUG("PolicyEngineServer: Custom security check");

	//Three custom check function...
	__ASSERT_ALWAYS( aMsg.Function() == ECreateManagementSubSession ||
					 aMsg.Function() == ECloseManagementSubSession ||
					 aMsg.Function() == EExecuteOperation ||
					 aMsg.Function() == RMessage2::EConnect, User::Panic( KPolicyEngineServerName,EPanicIllegalFunction));

	CPolicyServer::TCustomResult customResult = EFail;

	if ( aMsg.HasCapability( ECapabilityWriteDeviceData)  && aMsg.HasCapability( ECapabilityReadDeviceData))
	{
		RDEBUG("PolicyEngineServer: Client has CapabilityWriteDeviceData & ECapabilityReadDeviceData - ok");
		RDEBUG_3("PolicyEngineServer: Client SID: %d & VID %d",aMsg.SecureId().iId,aMsg.VendorId().iId);
		
		customResult = EPass;	//Every process can try to make policy management, valid SIDs are 
								//defined in policies. Certificate usega is allowed only for DM process (check from platformsecuritypolicies.h)
	}
	else
	{
		RDEBUG("PolicyEngineServer: PlatSec failed! Missing cababilities!");
		customResult = EFail;
	}

	return customResult;
}





// ----------------------------------------------------------------------------------------
// CPolicyEngineSession::CPolicyEngineSession()
// ----------------------------------------------------------------------------------------

inline CPolicyEngineSession::CPolicyEngineSession()
	{
	RDEBUG("CPolicyEngineSession::CPolicyEngineSession");
	}


// ----------------------------------------------------------------------------------------
// CPolicyEngineSession::Server()
// ----------------------------------------------------------------------------------------
	
inline CPolicyEngineServer& CPolicyEngineSession::Server()
{
	//return pointer to Server instance
	return *static_cast<CPolicyEngineServer*>(const_cast<CServer2*>(CSession2::Server()));
}

// ----------------------------------------------------------------------------------------
// CPolicyEngineSession::CreateL()
// ----------------------------------------------------------------------------------------

void CPolicyEngineSession::CreateL()
{
	// 2nd phase construct for sessions
	RDEBUG("PolicyEngineSession: CPolicyEngineSession::CreateL");
	
	//create session specific objects
	iSessionsObjects = CObjectIx::NewL();
	iContainer = Server().NewContainerL();
	
	//add session to server
	Server().AddSession();
}

// ----------------------------------------------------------------------------------------
// CPolicyEngineSession::~CPolicyEngineSession()
// ----------------------------------------------------------------------------------------

CPolicyEngineSession::~CPolicyEngineSession()
{
	RDEBUG("CPolicyEngineSession::~CPolicyEngineSession");
	
	delete iSessionsObjects;
	
	Server().DropSession();
}

// ----------------------------------------------------------------------------------------
// CPolicyEngineSession::ServiceL()
// ----------------------------------------------------------------------------------------

void CPolicyEngineSession::ServiceL( const RMessage2& aMessage)
{
	RDEBUG("CPolicyEngineSession::ServiceL");
	TRAPD(err,DispatchMessageL(aMessage));

	RDEBUG_2("CPolicyEngineSession::ServiceL, Error status: %d", err);
	
	//only KErrGeneral errors 
	if ( err != KErrNone && err != KErrNotFound )
	{
		err = KErrGeneral;
	}
	
	//no completion for asynch methods
	switch (aMessage.Function())
	{
		case EExecuteOperation:
		case EPolicyRequest:
			//No completion
		break;
		default:
			aMessage.Complete(err);
		break;		
	}

}

// ----------------------------------------------------------------------------------------
// CPolicyEngineSession::DispatchMessageL()
// ----------------------------------------------------------------------------------------

void CPolicyEngineSession::DispatchMessageL(const RMessage2& aMessage)
	{
	RDEBUG_2("CPolicyEngineSession::DispatchMessageL: %d",aMessage.Function());
	
	//Subsession management
	switch (aMessage.Function())
	{
		case ECreateRequestSubSession:
		case ECreateManagementSubSession:
		{
			NewSubSessionL( aMessage);
			return;
		}
		case ECloseManagementSubSession:
		case ECloseRequestSubSessio:
		{
			DeleteSubSession( aMessage);
			return;
		}
		case EPerformPMRFS :
		{
			PerformRFSL();	
			return;
		}
	}

	//Trusted session operations
	CSubSessionObjects * subSession = SubSessionFromHandle( aMessage);
		
	switch (aMessage.Function())
	{
		case EExecuteOperation :
		{
			subSession->iPolicyManager->ExecuteOperation( subSession->iTrustedSession, aMessage);
		}
		break;
		case EGetElementListLength:
		case EReadElementList:
		{
			subSession->iPolicyManager->GetElementListL( aMessage);
		}
		break;		
		case EGetElementDescriptionAndChildListLength :
		case EReadElementAndChildList :
		{
			subSession->iPolicyManager->GetElementInfoL( aMessage);
		}
		break;		
		case EGetElementXACMLLength :
		case EReadElementXACML :
		{
			subSession->iPolicyManager->GetElementXACMLDescriptionL( aMessage);
		}
		break;		
		case EPolicyRequest :
		{
			subSession->iPolicyProcessor->ExecuteRequestL( aMessage);	
		}
		break;
		case EAddSessionTrust :
		{
			subSession->iTrustedSession->MakeSessionTrustL( aMessage);	
		}
		break;
		case EIsServerIdValid :
		{
			subSession->iPolicyManager->IsServerIdValidL( aMessage);	
			return;
		}
		
        case EServerCertAddRequest:
                {
                subSession->iPolicyProcessor->AddCertificateToStoreL(aMessage);
                return;
                }
        case EServerCertRemoveRequest:
                {
                subSession->iPolicyProcessor->RemoveCertificateFromStoreL(aMessage);
                return;
                }
            
		case ECertificateRole :
		{
			subSession->iPolicyManager->GetCertificateRoleL( aMessage);	
		}
		break;
		default:
		break;	
	}
	
}
	
// ----------------------------------------------------------------------------------------
// CPolicyEngineSession::PerformRFS()
// ----------------------------------------------------------------------------------------

void CPolicyEngineSession::PerformRFSL()
{
	RDEBUG("PolicyEngineServer: Restory factory setting operation started");
	//RFS steps in Policy Management subsystem
	// 1. Remove policy files (RAM) from PolicyEngineServers private directory
	// 2. Remove backup files (RAM) from PolicyEngineServers private directory
	// 3. Remove Central Repository (RAM), handled by CentralRepository
	
	//stop all requests...
	CPolicyEngineServer::StopAllPendingRequests();
	
	//clean private directory
	RFs rfs;
	User::LeaveIfError( rfs.Connect() );
	CleanupClosePushL( rfs);
	
	TBuf<100> privatePath;
	TInt err = rfs.PrivatePath( privatePath);
	
	if ( err == KErrNone)
	{
		//remove files from private directory, also backups
		CFileMan* file = CFileMan::NewL( rfs);
		
		privatePath.Append(_L("*.*"));
		err = file->Delete( privatePath, CFileMan::ERecurse);
		delete file;
	}
	
	
	CleanupStack::PopAndDestroy( &rfs);
	
	RDEBUG("PolicyEngineServer: Restory factory setting operation finished");
}
		
	
// ----------------------------------------------------------------------------------------
// CPolicyEngineSession::GetSubSessionObjects()
// ----------------------------------------------------------------------------------------

CSubSessionObjects* CPolicyEngineSession::GetSubSessionObjects( const CBase* iBase)
{
	CSubSessionObjects* retVal = 0;

	
	for ( TInt i(0); i < iSessionsObjects->ActiveCount() ; i++)
	{
		CSubSessionObjects* objects = (CSubSessionObjects*)((*iSessionsObjects)[i]);
		
		if ( objects->iPolicyProcessor == iBase ||
 		 	 objects->iPolicyManager == iBase ||
			 objects->iTrustedSession == iBase)
		{
			retVal = objects;
			break;
		}
			
	}
	
	return retVal;
}


	
// ----------------------------------------------------------------------------------------
// CPolicyEngineSession::CSubSessionObjects()
// ----------------------------------------------------------------------------------------

CSubSessionObjects* CPolicyEngineSession::SubSessionFromHandle( const RMessage2& aMessage)
{
    //return pointer to trusted session from handle
   	CSubSessionObjects* subSession = (CSubSessionObjects*)iSessionsObjects->At(aMessage.Int3());
	
	if ( subSession == NULL)
	{
		PanicClient( aMessage, EBadSubsessionHandle); 
	}
	
	CPolicyEngineServer::iServer->iSubSessionObjects = subSession;
	
	return subSession;
}

// ----------------------------------------------------------------------------------------
// CPolicyEngineSession::NewSubSessionL()
// ----------------------------------------------------------------------------------------

void CPolicyEngineSession::NewSubSessionL( const RMessage2& aMessage)
	{
	//add new session objects object into container and object index
	CPolicyProcessor * policyProcessor = CPolicyProcessor::NewL(); 
	CTrustedSession * trustedSession = CTrustedSession::NewL();
	trustedSession->AddSessionSIDL( aMessage.SecureId());
	policyProcessor->SetSessionTrust( trustedSession);
	CPolicyManager * policyManager = NULL;
	
 	if ( aMessage.Function() == ECreateManagementSubSession)
 	{
 		policyManager = CPolicyManager::NewL( policyProcessor);
 	}	
	
	//Create new subsession object for subsession
	CSubSessionObjects * subSessionObjects = CSubSessionObjects::NewL( policyProcessor, policyManager, trustedSession); 

	iContainer->AddL( subSessionObjects);
	TInt handle = iSessionsObjects->AddL( subSessionObjects);

	//transmit handle to client 
	TPckg<TInt> handlePckg(handle);
	TRAPD( r, aMessage.WriteL(3, handlePckg))

	if ( r != KErrNone)
	{
		iSessionsObjects->Remove(handle);
		PanicClient( aMessage, EBadDescriptor);
		return;
	}
		
	iSubsessionCount++;	
	}
	
// ----------------------------------------------------------------------------------------
// CPolicyEngineSession::DeleteSubSession()
// ----------------------------------------------------------------------------------------
	
void CPolicyEngineSession::DeleteSubSession( const RMessage2& aMessage)
{
	// panic if bad handle
	CSubSessionObjects* subSessionObjects = (CSubSessionObjects*)iSessionsObjects->At(aMessage.Int3());
	
	if ( subSessionObjects == NULL)
	{
		PanicClient( aMessage, EBadSubsessionHandle); 
	}
	
	iSessionsObjects->Remove(aMessage.Int3());	
	iSubsessionCount--;
}

// ----------------------------------------------------------------------------------------
// CPolicyEngineSession::ServiceError()
// ----------------------------------------------------------------------------------------

void CPolicyEngineSession::ServiceError(const RMessage2& aMessage,TInt aError)
{
	// Handle an error from CPolicyEngineSession::ServiceL()
	RDEBUG_2("CPolicyEngineSession::ServiceError %d",aError);

	CSession2::ServiceError(aMessage,aError);
}


// ----------------------------------------------------------------------------------------
// CSubSessionObjects::CSubSessionObjects()
// ----------------------------------------------------------------------------------------

CSubSessionObjects::CSubSessionObjects( CPolicyProcessor * aPolicyProcessor, 
															  CPolicyManager * aPolicyManager,
										  					  CTrustedSession * aTrustedSession)
	: iPolicyProcessor( aPolicyProcessor), iPolicyManager( aPolicyManager), iTrustedSession( aTrustedSession)
{
	
}


// ----------------------------------------------------------------------------------------
// CSubSessionObjects::NewL()
// ----------------------------------------------------------------------------------------

CSubSessionObjects * CSubSessionObjects::NewL( CPolicyProcessor * aPolicyProcessor, 
			      			  						        CPolicyManager * aPolicyManager,
					  						   				CTrustedSession * aTrustedSession)
{
	return new (ELeave) CSubSessionObjects( aPolicyProcessor, aPolicyManager, aTrustedSession);
}

// ----------------------------------------------------------------------------------------
// CSubSessionObjects::~CSubSessionObjects()
// ----------------------------------------------------------------------------------------

CSubSessionObjects::~CSubSessionObjects()
{
	delete iPolicyProcessor;
	delete iPolicyManager;
	delete iTrustedSession;
}


// ----------------------------------------------------------------------------------------
// CSubSessionObjects::~CSubSessionObjects()
// ----------------------------------------------------------------------------------------

CASyncHandler::CASyncHandler()
	: CActive( EPriorityStandard)
{
	RDEBUG("PolicyEngineServer: CASyncHandler constructor( EPriorityStandard) ");
}

CASyncHandler::~CASyncHandler()
{
	CPolicyEngineServer::RemoveActiveObject( this);	
}


CASyncHandler* CASyncHandler::NewLC()
{
	CASyncHandler* self = new (ELeave) CASyncHandler();	
	CleanupStack::PushL( self);

	RDEBUG("PolicyEngineServer: CASyncHandler created");
	CPolicyEngineServer::AddActiveObjectL( self);
	
	return self;
}


TRequestStatus& CASyncHandler::GetRequestStatus()
{
	RDEBUG("PolicyEngineServer: CASyncHandler::GetRequestStatus()");
	return iStatus;
}

void CASyncHandler::WaitForRequest()
{
	RDEBUG("PolicyEngineServer: CASyncHandler::WaitForRequest()");

	CActiveScheduler::Add( this);
	SetActive();	
	
	RDEBUG("PolicyEngineServer: CASyncHandler - WaitScheduler start");
	iWaitScheduler.Start();
	RDEBUG("PolicyEngineServer: CASyncHandler - WaitScheduler stopped");
}


void CASyncHandler::RunL()
{
	RDEBUG("PolicyEngineServer: CASyncHandler::RunL - Stop WaitScheduler!");
	iWaitScheduler.AsyncStop();
}

void CASyncHandler::DoCancel()
{
	RDEBUG("PolicyEngineServer: CASyncHandler::DoCancel()");
	iWaitScheduler.AsyncStop();
}


