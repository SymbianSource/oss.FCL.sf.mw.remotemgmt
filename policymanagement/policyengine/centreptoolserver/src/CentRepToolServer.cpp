/*
* Copyright (c) 2002-2004 Nokia Corporation and/or its subsidiary(-ies). 
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


#include "CentRepToolServer.h"
#include "RepositorySession.h"
#include "debug.h"

#include "CentRepToolClientServer.h"
#include <e32svr.h>

   
// ----------------------------------------------------------------------------------------
// Server startup code
// ----------------------------------------------------------------------------------------

static void RunServerL()
	{
	// naming the server thread after the server helps to debug panics
	User::LeaveIfError(User::RenameThread(KCentRepToolServerName));

	// create and install the active scheduler
	CActiveScheduler* s=new(ELeave) CActiveScheduler;
	CleanupStack::PushL(s);
	CActiveScheduler::Install(s);
	
	// create the server (leave it on the cleanup stack)
	CCentRepToolServer::NewLC();
	// Initialisation complete, now signal the client

	RProcess::Rendezvous(KErrNone);

	// Ready to run
	RDEBUG("Centrep tool server is running");
	CActiveScheduler::Start();
	
	// Cleanup the server and scheduler
	CleanupStack::PopAndDestroy(2);
	}

// Server process entry-point
TInt E32Main()
	{
	__UHEAP_MARK;
	RDEBUG( "CentRepToolServer: E32Main");
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
    
// RMessagePtr2::Panic() also completes the message. This is:
// (a) important for efficient cleanup within the kernel
// (b) a problem if the message is completed a second time
void PanicClient(const RMessagePtr2& aMessage, TCentRepToolServerPanic aPanic)
	{
	RDEBUG("CentRepTool PanicClient");
	aMessage.Panic( IniConstants::KCentRepToolPanic,aPanic);
	}

    
// ----------------------------------------------------------------------------------------
// CShutDown
// ----------------------------------------------------------------------------------------
inline CShutdown::CShutdown()
	:CTimer(-1)
	{
	CActiveScheduler::Add(this);
	}
	
inline void CShutdown::ConstructL()
	{
	CTimer::ConstructL();
	}

inline void CShutdown::Start()
	{
	RDEBUG("CentRepToolServer: starting shutdown timeout");
	After(KPolicyEngineShutdownDelay);
	}

void CShutdown::RunL()
	{
	RDEBUG("CentRepToolServer timeout ... closing");
	CActiveScheduler::Stop();
	}

// ----------------------------------------------------------------------------------------
// CPolicyEngineServer
// ----------------------------------------------------------------------------------------
inline CCentRepToolServer::CCentRepToolServer()
	:CPolicyServer(0, CentRepToolSecurityPolicy,ESharableSessions)
	{
	}
	
CServer2* CCentRepToolServer::NewLC()
	{
	CentRepToolSecurityPolicy.iOnConnect = KCentRepToolSecurityElementsIndex[2];	//Specifis that only policy engine may connect to server
	CentRepToolSecurityPolicy.iRangeCount = KCentRepToolRangeCount;					//number of ranges                                   
	CentRepToolSecurityPolicy.iRanges = KCentRepToolRanges;							
	CentRepToolSecurityPolicy.iElementsIndex = KCentRepToolSecurityElementsIndex;
	CentRepToolSecurityPolicy.iElements = KCentRepToolSecurityElements;

	RDEBUG("CentRepToolServer: CentRepToolServer::NewLC");
	
	CCentRepToolServer* self=new(ELeave) CCentRepToolServer;
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}
	
CCentRepToolServer::~CCentRepToolServer()
{
	delete iContainerIndex;
}

// 2nd phase construction - ensure the timer and server objects are running
void CCentRepToolServer::ConstructL()
	{
	StartL(KCentRepToolServerName);
	
	iContainerIndex = CObjectConIx::NewL();
	
	iShutdown.ConstructL();
	// ensure that the server still exits even if the 1st client fails to connect
	iShutdown.Start();
	}


// Create a new client session. This should really check the version number.
CSession2* CCentRepToolServer::NewSessionL(const TVersion&,const RMessage2&) const
	{
	RDEBUG("CentRepToolServer: CCentRepToolServer::NewSessionL");
	return new (ELeave) CCentRepToolSession();
	}

// A new session is being created
// Cancel the shutdown timer if it was running
void CCentRepToolServer::AddSession()
	{
	RDEBUG("CentRepToolServer: CPolicyEngineServer::AddSession");
	++iSessionCount;
	iShutdown.Cancel();
	}

// A session is being destroyed
// Start the shutdown timer if it is the last session.
void CCentRepToolServer::DropSession()
	{
	RDEBUG("CentRepToolServer: CPolicyEngineServer::DropSession");
	if (--iSessionCount==0)
		iShutdown.Start();
	}

CObjectCon* CCentRepToolServer::NewContainerL()
	{
	return iContainerIndex->CreateL();
	}

inline CCentRepToolSession::CCentRepToolSession()
	{
	RDEBUG("CentRepToolServer: CCentRepToolSession::CCentRepToolSession");
	}
	
inline CCentRepToolServer& CCentRepToolSession::Server()
	{
	return *static_cast<CCentRepToolServer*>(const_cast<CServer2*>(CSession2::Server()));
	}

// 2nd phase construct for sessions - called by the CServer framework
void CCentRepToolSession::CreateL()
	{
	RDEBUG("CentRepTool Server: CCentRepToolSession::CreateL");
	
	iRepositorySessions = CObjectIx::NewL();
	iContainer = Server().NewContainerL();
	
	Server().AddSession();
	}

CCentRepToolSession::~CCentRepToolSession()
	{
	RDEBUG("CentRepTool Server: CCentRepToolSession::~CCentRepToolSession");
	
	delete iRepositorySessions;
	
	Server().DropSession();
	}






void CCentRepToolSession::ServiceL(const RMessage2& aMessage)
{
	iCurrentSession = 0;
	TRAPD(err,DispatchMessageL(aMessage));
	
	aMessage.Complete(err);
}

void CCentRepToolSession::DispatchMessageL(const RMessage2& aMessage)
	{
	//Subsession management
	switch (aMessage.Function())
	{
		case ECreateRepositorySubSession:
		case ECreateCheckAccessSession:
		{
			RDEBUG("CentRepTool: New repository session");
			NewRepositorySessionL( aMessage);
			return;
		}
		case ECloseRepositorySubSession:
		case ECloseCheckAcceessSession:
		{
			RDEBUG("CentRepTool: Close repository session");
			DeleteRepositorySession( aMessage);
			return;
		}
		case ECheckCommitState :
		{
			RDEBUG("CentRepTool: Check last commit state");
			CRepositorySession::CheckCommitStateL();
			return;
		}
		case EPerformCentRepToolRFS :
		{
			RDEBUG("CentRepTool: Perform RFS");
			PerformRFSL();
			return;
		}
	}

	//Trusted session operations
	CRepositorySession * repositorySession = RepositorySessionFromHandle( aMessage);
	iCurrentSession = repositorySession;
			
	switch (aMessage.Function())
	{
		case EInitRepositorySession:
		{
			RDEBUG("CentRepTool: Init repository session");
			repositorySession->InitRepositorySessionL();
		}
		break;
		case ESetSIDWRForSetting :
		{
			RDEBUG("CentRepTool: Add SID for individual setting");
			repositorySession->SetSecurityIdForSettingL( aMessage);
		}
		break;
		case ESetSIDWRForRange:
		{
			RDEBUG("CentRepTool: Add SID for range");
			repositorySession->SetSecurityIdForRangeL( aMessage);
		}
		break;		
		case ERestoreSetting :
		{
			RDEBUG("CentRepTool: Restore individual setting");
			repositorySession->RestoreSettingL( aMessage);
		}
		break;		
		case ERestoreRange :
		{
			RDEBUG("CentRepTool: Restore range");
			repositorySession->RestoreRangeL( aMessage);
		}
		break;
		case EAddSIDWRForDefaults :
		{
			RDEBUG("CentRepTool: Add SID for all settings (default, range, mask)");
			repositorySession->AddSidForDefaultsL( aMessage);
		}
		break;
		case ERestoreDefaults :
		{
			RDEBUG("CentRepTool: ");
			RDEBUG("CentRepTool: Remove SID from all settings (default, range, mask)");
			repositorySession->RestoreDefaultsL( aMessage);
		}
		break;
		case EFlushRepository :
		{
			RDEBUG("CentRepTool: Commit security changes in repository");
			repositorySession->CommitRepositoryL();
		}
		break;
		case ESetSIDWRForMask :
		{
			RDEBUG("CentRepTool: Set SID for mask setting");
			repositorySession->SetSecurityIdForMaskL( aMessage);
		}
		break;
		case ERestoreMask :
		{
			RDEBUG("CentRepTool: Restore mask setting");
			repositorySession->RestoreMaskL( aMessage);
		}
		break;
		case ERemoveBackupFlagForMask:
		{
			RDEBUG("CentRepTool: Set backup flag for mask");
			repositorySession->RemoveBackupForMaskL( aMessage);
		}
		break;
		case ERestoreBackupFlagForMask:
		{
			RDEBUG("CentRepTool: Restore backup flag for mask");
			repositorySession->RestoreMaskBackupL( aMessage);
		}
		break;
		case ERemoveBackupFlagForDefaults:
		{
			RDEBUG("CentRepTool: Set backup flag for defaults");
			repositorySession->RemoveDefaultBackup();
		}
		break;
		case ERestoreBackupFlagForDefaults:
		{
			RDEBUG("CentRepTool: Restore backup flag for defaults");
			repositorySession->RestoreDefaultBackupL();
		}
		break;		
		case ERemoveBackupFlagForRange:
		{
			RDEBUG("CentRepTool: Set backup flag for range");
			repositorySession->RemoveBackupForRangeL( aMessage);
		}
		break;
		case ERestoreBackupFlagForRange:
		{
			RDEBUG("CentRepTool: Restore backup flag for range");
			repositorySession->RestoreRangeBackupL( aMessage);
		}
		break;		
		case ECheckAccess:
		{
			RDEBUG("CentRepTool: Check access");
			repositorySession->CheckAccessL( aMessage);
		}
		break;		
		default:
		break;	
	}		
		
}

void CCentRepToolSession::PerformRFSL()
{
	RDEBUG("CentRepToolSession: Restore factory setting operation started");
	//RFS tasks in centrep tool
	// 1. restory setting enforcements
	// 2. remove backup and temp files from private directory
	
	
	//clean private directory
	RFs rfs;
	TInt err = rfs.Connect();
	if( err != KErrNone )
		{
		RDEBUG_2("**** CCentRepToolSession::PerformRFSL() - failed to connect to RFs: %d", err );
		return;
		}
		
	CleanupClosePushL( rfs);
	
	TBuf<100> privatePath;
	err = rfs.PrivatePath( privatePath);
	
	if ( err == KErrNone)
	{
		//remove files from private directory, also backups
		CFileMan* file = CFileMan::NewL( rfs);
		
		privatePath.Append(_L("*.*"));
		err = file->Delete( privatePath, CFileMan::ERecurse);
		delete file;
	}
	
	CleanupStack::PopAndDestroy( &rfs);	
	
	RDEBUG("CentRepToolSession: Restore factory setting operation finished");
}

	
CRepositorySession* CCentRepToolSession::RepositorySessionFromHandle( const RMessage2& aMessage)
    {
	CRepositorySession* repositorySession = (CRepositorySession*)iRepositorySessions->At(aMessage.Int3());
	
	if (repositorySession == NULL)
	{
		PanicClient( aMessage, EBadSubsessionHandle); 
	}
	
	return repositorySession;
    }


// Create a new counter; pass back its handle via the message
void CCentRepToolSession::NewRepositorySessionL( const RMessage2& aMessage)
	{

	TUid repositoryUid;
	TPckg<TUid> repositoryUidPack( repositoryUid);
	aMessage.ReadL(0, repositoryUidPack);
	
	//add new CTrustedSession object into container and object index
	CRepositorySession * repositorySession = CRepositorySession::NewL( repositoryUid);
	
	iContainer->AddL( repositorySession);
	TInt handle = iRepositorySessions->AddL( repositorySession);

	//transmit handle to client 
	TPckg<TInt> handlePckg(handle);
	TRAPD( r, aMessage.WriteL(3, handlePckg))

	if ( r != KErrNone)
		{
		iRepositorySessions->Remove(handle);
		PanicClient( aMessage, EBadDescriptor);
		return;
		}
		
	iSubsessionCount++;	
	}
	
void CCentRepToolSession::DeleteRepositorySession( const RMessage2& aMessage)
{
	// panic if bad handle
	CRepositorySession* repositorySession = (CRepositorySession*)iRepositorySessions->At(aMessage.Int3());
	if (repositorySession == NULL)
		PanicClient( aMessage, EBadSubsessionHandle); 
	iRepositorySessions->Remove(aMessage.Int3());	
	
	iSubsessionCount--;
}	
	

// Handle an error from CentRepTool::ServiceL()
void CCentRepToolSession::ServiceError(const RMessage2& aMessage,TInt aError)
	{
	RDEBUG_2("CentRepTool: CentRepTool::ServiceError %d",aError);
	CSession2::ServiceError(aMessage,aError);
	}
