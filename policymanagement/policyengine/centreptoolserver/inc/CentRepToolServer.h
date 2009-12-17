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


#ifndef __CENTREPTOOL_SERVER_H__
#define __CENTREPTOOL_SERVER_H__

// INCLUDES

#include <e32base.h>
#include "CentRepToolClient.h"
#include "PlatformSecurityPolicies.h"

// CONSTANTS

// ----------------------------------------------------------------------------------------
// PolicyEngine server panic codes
// ----------------------------------------------------------------------------------------
enum TCentRepToolServerPanic
	{
	EPanicGeneral,
	EBadSubsessionHandle,
	EPanicIllegalFunction,
	EBadDescriptor
	};

// MACROS
// DATA TYPES
// FUNCTION PROTOTYPES

void PanicClient(const RMessagePtr2& aMessage, TCentRepToolServerPanic aPanic);

// FORWARD DECLARATIONS

class CRepositorySession;

// CLASS DECLARATION     

class CShutdown : public CTimer
	{
	enum {KPolicyEngineShutdownDelay=0x200000};	// approx 2s
public:
	inline CShutdown();
	inline void ConstructL();
	inline void Start();
private:
	void RunL();
	};



/**
*  CPolicyEngineServer 
*  Description.
*/

class CCentRepToolServer : public CPolicyServer
	{
public:
	static CServer2* NewLC();
	~CCentRepToolServer();
	void AddSession();
	void DropSession();
	
	/**
    * Description
	* @param a??? 
    * @return CObjectCon
    */

	CObjectCon* NewContainerL();
	
	void Panic( TInt aPanicCode);
private:
	CCentRepToolServer();
	void ConstructL();
	CSession2* NewSessionL(const TVersion& aVersion, const RMessage2& aMessage) const;	
private:
	TInt iSessionCount;
	
	CShutdown iShutdown;
	
	CObjectConIx *iContainerIndex;
	};


class CCentRepToolSession : public CSession2
	{
public:
	CCentRepToolSession();
	void CreateL();
 private:
	~CCentRepToolSession();
	inline CCentRepToolServer& Server();

	void ServiceL(const RMessage2& aMessage);
	void DispatchMessageL(const RMessage2& aMessage);
	void ServiceError(const RMessage2& aMessage,TInt aError);
	
	void NewRepositorySessionL( const RMessage2& aMessage);
	CRepositorySession* RepositorySessionFromHandle( const RMessage2& aMessage);
	void DeleteRepositorySession( const RMessage2& aMessage);
	
	void ExecuteRequestL( const RMessage2& aMessage);
	
	void PerformRFSL();

private:	
	TInt iSubsessionCount;
	CRepositorySession* iCurrentSession;

	CObjectCon *iContainer;		 		// object container for this session
	CObjectIx* iRepositorySessions; 	// object index which stores objects for this session
};

#endif //__CENTREPTOOL_SERVER_H__
