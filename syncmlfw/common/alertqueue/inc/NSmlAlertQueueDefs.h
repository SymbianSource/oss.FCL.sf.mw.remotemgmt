/*
* Copyright (c) 2004 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description: SyncML alert error constants definition.  
*
*/


#ifndef __NSMLALERTQUEUEDEFS_H__
#define __NSMLALERTQUEUEDEFS_H__

const TInt KNSmlAlertServerIdLenPos = 24;
const TInt KNSmlAlertVersionPos = 16;
const TInt KNSmlAlertVersion = 12;

const TInt KNSmlAlertSession = 21;
const TInt KNSmlAlertServerIdLength = 23;

const TInt KNSmlDSAgentSyncInfoPos = 1;
const TInt KNSmlDSAgentContentTypePos = 4;
const TInt KNSmlDSAgentServerURILenPos = 5;
const TInt KNSmlDSAgentAlertCodeBase = 200;
const TInt KNSmlDSDbChangeQueryMaxTime = 30;

const TInt KNSmlServerAlertCode = 200;
const TInt KNSmlServerAlertCodeTwoWay = 206;
const TInt KNSmlServerAlertCodeOneWayFromClient = 207;
const TInt KNSmlServerAlertCodeRefreshFromClient = 208;
const TInt KNSmlServerAlertCodeOneWayFromServer = 209;
const TInt KNSmlServerAlertCodeRefreshFromServer = 210;

const TInt KNSmlDatabasePathStartPos = 5;


// masks
const TUint8 KUiModeMask = 0x30;
const TUint8 KInitiatorMask = 0x8;
const TUint8 KMaskUpperFuture = 0x7;
const TUint8 KFutureMask = 0xF;

const TInt KNSmlBufLength = 16;
	
_LIT( KNSmlAlertDirAndResource, "z:NSMLDSTYPESRES.RSC" );

//Container for task id and sync type pairs
class TNSmlContentTypeInfo
	{
	public:
		TInt iTaskId;
		TSmlSyncType iSyncType;
	};
	
//Container for server alerted info
class CNSmlAlertJobInfo : public CBase
	{
	public:
		virtual ~CNSmlAlertJobInfo();
		TSmlUsageType iType;
		TInt iProfileId;
		TInt iTransportId;
		TInt iSessionId;
		TPtrC8 iPackage;
		CArrayFix<TNSmlContentTypeInfo>* iContentType;
		TInt iUimode;
	};

//Notifies server to create a new (server alerted) job
class MNSmlAlertObserver
	{
	public:
	
		/**
        * void CreateJobL( CNSmlAlertJobInfo& aJobInfo )
        * @param aJobInfo - reference to CNSmlAlertJobInfo
		* @return - 
        */
		virtual void CreateJobL( CNSmlAlertJobInfo& aJobInfo ) = 0;	
		
		/**
        * void DoDisconnectL()
        * Disconnects local connection.
        * @param -
		* @return - 
        */
		virtual void DoDisconnectL() = 0;					
	};



#endif