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


#ifndef __POLICY_ENGINE_SERVER_H__
#define __POLICY_ENGINE_SERVER_H__

// INCLUDES

#include <e32base.h>
#include "PolicyEngineClient.h"
#include "PlatformSecurityPolicies.h"

// CONSTANTS

// ----------------------------------------------------------------------------------------
// PolicyEngine server panic codes
// ----------------------------------------------------------------------------------------

enum TPolicyEnginePanic
	{
	EPanicGeneral,
	EBadSubsessionHandle,
	EPanicIllegalFunction,
	EBadDescriptor
	};
	
const TInt KPolicyEngineSelfCheckDelay 			= 8000000;	// approx 8s
const TInt KPolicyEngineShutdownDelay  		 	= 5;		// approx 8s * 2 = 40s
const TInt KPolicyEngineReleaseElementsDelay  	= 1;		// approx 8s * 1 = 8s


// DATA TYPES
// FUNCTION PROTOTYPES

void PanicClient(const RMessagePtr2& aMessage,TPolicyEnginePanic aPanic);

// FORWARD DECLARATIONS

class CPolicyStorage;
class CPolicyProcessor;
class CPolicyManager;
class CTrustedSession;
class CCertificateMaps;

#ifdef _DEBUG
namespace DebugUtility
{
	TBool PolicyCheckActive();
}
#endif


// MACROS

#ifdef _DEBUG
	#define NO_POLICY_CHECK_RETURN if(!DebugUtility::PolicyCheckActive())return;
	#define NO_POLICY_CHECK_FALSE DebugUtility::PolicyCheckActive()
#else
	#define NO_POLICY_CHECK_RETURN ;
	#define NO_POLICY_CHECK_FALSE ETrue
#endif

// CLASS DECLARATION     

class CShutdown : public CTimer
{
	//shutdown timer for policy server
	public:
	public:
		inline CShutdown();
		inline void ConstructL();
		inline void Start();
	private:
		void RunL();
		TInt iSelfCheckCounter;
};

class CServerStatusFlags : public CBase
{
	public:
		CServerStatusFlags() {}
	public:
		TBool iManagementUiActive;
		TBool iUiActive;
		TBool iProcessorActive;
		TBool iManagementActive;
	private:
};

class CSubSessionObjects : public CObject
{
	public:
		static CSubSessionObjects * NewL( CPolicyProcessor * aPolicyProcessor, 
										  CPolicyManager * aPolicyManager,
			  							  CTrustedSession * aTrustedSession);
		~CSubSessionObjects();
	private:
		CSubSessionObjects( CPolicyProcessor * aPolicyProcessor, 
							CPolicyManager * aPolicyManager,
							CTrustedSession * aTrustedSession);
	private:
		CPolicyProcessor * iPolicyProcessor;
 		CPolicyManager * iPolicyManager; 
		CTrustedSession * iTrustedSession;

	
		friend class CPolicyEngineSession;
		friend class CPolicyEngineServer;
};

/**
*  CPolicyEngineServer 
*  Description.
*/

class CPolicyEngineServer : public CPolicyServer
{
	public:
		static CServer2* NewLC();
		~CPolicyEngineServer();
		void AddSession();
		void DropSession();
	
	/**
    * Description
	* @param a??? 
    * @return CObjectCon
    */
	
		CObjectCon* NewContainerL();
		
		static CCertificateMaps * CertificateMaps();
		static CServerStatusFlags* StatusFlags();
		static void AddActiveObjectL( CActive* aActiveObject);
		static void RemoveActiveObject( CActive* aActiveObject);
		static void StopAllPendingRequests();
		
		//get active subsessions object to PolicyEngineServer
		static void SetActiveSubSession( const CBase* iBase);
		void Panic( TInt aPanicCode);
private:
		CPolicyEngineServer();
		void ConstructL();
		CSession2* NewSessionL(const TVersion& aVersion, const RMessage2& aMessage) const;	
		
		//From CPolicyServer
		CPolicyServer::TCustomResult CustomSecurityCheckL(const RMessage2& aMsg, TInt& aAction, TSecurityInfo& aMissing);

		
private:
		TInt iSessionCount;
		CShutdown iShutdown;
		RArray<CActive*> iActiveObjectCancelList;
		CPolicyStorage* iPolicyStorage;
		CCertificateMaps* iCertificateMaps;
		CSubSessionObjects* iSubSessionObjects;
		CServerStatusFlags* iFlags;
		CObjectConIx *iContainerIndex;
		
		static CPolicyEngineServer* iServer;
		CPolicyServer::TPolicy iPolicyEngineSecurityPolicy;
		
		friend class CPolicyEngineSession;
};

class CPolicyEngineSession : public CSession2
{
	
	public:
		CPolicyEngineSession();
		void CreateL();
		void SetStatusFlags( CServerStatusFlags* aFlags);
		
		CSubSessionObjects* GetSubSessionObjects( const CBase* iBase);
 	private:

	private:
		~CPolicyEngineSession();
		inline CPolicyEngineServer& Server();

		void ServiceL(const RMessage2& aMessage);
		
		void DispatchMessageL(const RMessage2& aMessage);
		void ServiceError(const RMessage2& aMessage,TInt aError);
	
		void NewSubSessionL( const RMessage2& aMessage);
		CSubSessionObjects* SubSessionFromHandle( const RMessage2& aMessage);
		void DeleteSubSession( const RMessage2& aMessage);

		void ExecuteRequestL( const RMessage2& aMessage);
		void AddCertificateForSessionL(  const RMessage2& aMessage);
		
		void PerformRFSL();
	private:	
		TInt iSubsessionCount;

		CObjectCon *iContainer; 		// object container for this session
		CObjectIx* iSessionsObjects; 	// object index which stores objects for this session
};

class CASyncHandler : public CActive
{
	public:
		~CASyncHandler();
		static CASyncHandler* NewLC();
	
		TRequestStatus& GetRequestStatus();
		void WaitForRequest();
	protected:	
		CASyncHandler();
		void RunL();
		void DoCancel();		
	private:
		CActiveSchedulerWait iWaitScheduler;	
};

#endif //__POLICYENGINESERVER_H__





