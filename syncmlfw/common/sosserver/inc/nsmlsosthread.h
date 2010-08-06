/*
* Copyright (c) 2002-2005 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description: Thread that is used by alert module to launch engine server. 
*
*/


#ifndef __NSMLTHREADSTART_H__
#define __NSMLTHREADSTART_H__

#include <devicedialogsymbian.h>
#include <dmdevdialogclient.h>
#include <hbdevicedialogsymbian.h>
#include <hbsymbianvariant.h>
#include "nsmlsosserver.h"


class CNSmlJob;
class CNSmlNotifierObserver;
class CNSmlNotifierTimeOut;
class CNSmlSOSHandler;
class CNSmlThreadEngine;
class CNSmlCancelTimeOut;

// CONSTANTS
//name of DM thread
_LIT( KNSmlJobThread, "NSmlJobThread" );
const TBool KNSmlWaitNotifierForEver = ETrue;



/**
* TNSmlThreadParams class
* 
*/
class CNSmlThreadParams : public CBase
	{
	public:
		CNSmlThreadParams( CNSmlJob& aJob, CNSmlSOSHandler* aSOSHandler)
		: iCurrentJob(aJob), iSOSHandler(aSOSHandler) { }
		
		~CNSmlThreadParams();
		
		CNSmlSOSHandler* iSOSHandler;
		CArrayFix<TNSmlContentSpecificSyncType>* iCSArray;
		CNSmlJob& iCurrentJob;
		TInt iSyncInit;
		HBufC8* iNotifierParams;
		CNSmlThreadEngine* iThreadEngine;
	};



/**
* CNSmlThreadObserver class
* 
*/
class CNSmlThreadObserver : public CActive
	{
	public:
		CNSmlThreadObserver(TRequestStatus& aStatus);
		~CNSmlThreadObserver();
		void LogonToThread( RThread& aThread );
	protected:
		void DoCancel();
		void RunL();
	private:
		TRequestStatus& iSosStatus;
	};

/**
* CNSmlThreadEngine class
* 
*/
class CNSmlThreadEngine : public CActive
	{
	public:
		static CNSmlThreadEngine* NewL( CNSmlThreadParams& aParams );
		
		~CNSmlThreadEngine();
		
		void StartJobSessionL();
		void CancelJob();
		TInt VerifyJobFromNotifierL(TBool aServerInitiated);
		
		TInt FinishedStatus() 
			{
			return iStatus.Int();
			};
		
		CNSmlDMAgent* DMAgentL() const;
		CNSmlDSAgent* DSAgentL() const;
		
		TBool SilentModeL( const CNSmlThreadParams* aParams );
		void SaveSessionInfoL();
		void ForcedCertificateCheckL( TBool aForcedState );
	protected:
		void DoCancel();
		void RunL();
		
	private:
		CNSmlThreadEngine( CNSmlThreadParams& aParams );
		void ConstructL();
		void StartDMSessionL();
		void StartDSSessionL();
		void WriteHistoryLogL( TInt aErrorCode );
		TBool DoRunL();
	private:
		CNSmlThreadParams& iThreadParams;
		TSmlUsageType iUsageType;
		
		//Data Sync contents
		CArrayFix<TNSmlContentSpecificSyncType>* iContentArray;
		
		//used for server alert initiated job
		RTimer iTimeout;
//    	RNotifier iNotifier;
    	CNSmlNotifierObserver* iNotifierObserver;
    	RLibrary iSessionLib;
    	CNSmlDMAgent* iDMAgent;		
		CNSmlDSAgent* iDSAgent;	
		CNSmlCancelTimeOut* iCancelTimeout;
	};


/**
* CNSmlNotifierTimeOut class
* 
*/
class CNSmlNotifierTimeOut : public CActive
	{
	public:
		CNSmlNotifierTimeOut();
		~CNSmlNotifierTimeOut();
		void LaunchNotifierTimer( CNSmlNotifierObserver*  aObserver );
	protected:
		void DoCancel();
		void RunL();
	private:
		CNSmlNotifierObserver* iObserver;
		RTimer iTimeOutTimer;
	};

/**
* CNSmlCancelTimeOut class
* 
*/
class CNSmlCancelTimeOut : public CActive
	{
	public:
		CNSmlCancelTimeOut( CNSmlDMAgent& iDMAgent, CNSmlDSAgent& iDSAgent );
		~CNSmlCancelTimeOut();
		void SetJobCancelled( TSmlUsageType aUsageType );
	protected:
		void DoCancel();
		void RunL();
	private:
		TSmlUsageType iUsageType; 
		CNSmlDMAgent& iDMAgent;		
		CNSmlDSAgent& iDSAgent;	
		RTimer iTimeOutTimer;
		TBool iDoCancel;
	};

	
/**
* CNSmlNotifierObserver class
* 
*/
class CNSmlNotifierObserver : public CActive
	{
	public:
		CNSmlNotifierObserver( TRequestStatus& aStatus, CNSmlThreadParams& aParams );
		~CNSmlNotifierObserver();
		void ConnectToNotifierL( const TSyncMLAppLaunchNotifParams& aParam );
		void NotifierTimeOut();
		TBool IsHbSyncmlNotifierEnabledL();
		void HbNotifierObserverL(const TSyncMLAppLaunchNotifParams& aParam);
	protected:
		void DoCancel();
		void RunL();
	private:
		TRequestStatus& iCallerStatus;
		
//		RNotifier iNotifier;
		TPckgBuf<TInt> iResBuf;
	
		CNSmlThreadParams& iThreadParams;
		CNSmlNotifierTimeOut iNotifierTimeOut;
		TBool iTimeOut;

		CHbDeviceDialogSymbian* iDevDialog;
            RProperty iProperty;
            TBool iHbSyncmlNotifierEnabled;
           RDmDevDialog iDmDevdialog;

	};
	

/**
* TNSmlThreadLauncher class
* 
*/
class TNSmlThreadLauncher
	{
	public:
		static TInt RunJobSession( CNSmlThreadParams& aParams, CNSmlThreadObserver& aObserver );
	};
	
	
#endif