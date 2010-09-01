/*
* Copyright (c) 2007-2007 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Symbian OS server results database.
*  libraries   : 
*
*/


#ifndef DIAG_RESULTS_DB_SERVER_H
#define DIAG_RESULTS_DB_SERVER_H

#include "diagresultsdatabasecommon.h"

//System includes
#include <e32base.h>
#include <f32file.h>

/**
* Results database symbian OS server class. Creates server sessions and keeps
* track of the session counts.
*
* @since S60 v5.0
**/
class CDiagResultsDbServer : public CPolicyServer
	{
public:

    /**
    * Destructor
    **/ 
	~CDiagResultsDbServer();
	
	/**
	* NewLC. 
	* @param aPolicy Security policy that is defined at the end of this file.
	**/
	static void NewLC(const TPolicy &aPolicy);
	
	/**
	* Create a new server session.
	* @param aVersion version of the server.
	* @param aMessage
	* @return a new session.
	**/
	virtual CSession2 *NewSessionL(const TVersion &aVersion, const RMessage2 &/*aMessage*/) const;
	
	/**
	* Run server i.e. first create cleanup stack and active scheduler. This server is CActive based.
	* @return Symbian error code or KErrNone.
	**/
	static TInt RunServer();
	static void RunServerL();
	
public: // New methods

    /**
    * Create object container that is needed when handling subsession.
    * @return object container.
    **/
	CObjectCon* NewContainerL();
	
	/**
	* Remove object container from container index.
	* @param aCon container to be removed.
	**/
	void RemoveContainer(CObjectCon* aCon);
	
    /**
	* Decrease session count by one. ServerSession uses this function when client dies.
	**/
	void DecreaseSessionCount();
	
	/**
	* Increase session count by one. ServerSession uses this function when client dies.
	**/
	void IncreaseSessionCount();
	
	/**
	* Returns the number of active sessions.
	* @return Session count.
	**/
	TInt SessionCount();
	
	/**
	* Custom security check. From CPolicyServer.
	**/
	TCustomResult CustomSecurityCheckL(const RMessage2 &aMsg, TInt &aAction, TSecurityInfo &aMissing);
	
protected:
    /**
    * Constructor.
    * @param aPriority Priority of this server. Should be pretty high.
    * @param aPolicy Security policy of this server.
    **/
	CDiagResultsDbServer(TInt aPriority, const TPolicy &aPolicy);

private:

    /**
    * ConstructL.
    **/
	void ConstructL();
	
private: // Data

    // For creating object container instances
	CObjectConIx* iContainerIndex;  
	
	// The number of active sessions 
	TInt iSessionCount;
	};

// Security policy definition.
// ReadDeviceData + WriteDeviceData is needed.
const CPolicyServer::TPolicyElement elements[] = 
	{
		{_INIT_SECURITY_POLICY_C2(ECapabilityReadDeviceData, ECapabilityWriteDeviceData),
			 CPolicyServer::EFailClient},
	};

const TInt ranges[2] = 
		{
		0,
		DiagResultsDbCommon::ENotSupported,
		
		};

const TUint8 elementsIndex[2] = 
		{
		CPolicyServer::ECustomCheck, //ReadDeviceData + WriteDeviceData for all clients.
        CPolicyServer::ENotSupported,
		};

const CPolicyServer::TPolicy policy = 
	{
	CPolicyServer::EAlwaysPass,	// on connect
	2,		// range count
	ranges, 
  	elementsIndex,
	elements,
  	};    
    
#endif //DIAG_RESULTS_DB_SERVER_H