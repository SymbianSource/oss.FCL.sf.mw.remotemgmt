/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  SyncML Obex plugin for data syncronization
*
*/


#ifndef __NSMLOBEXDSPLUGIN__
#define __NSMLOBEXDSPLUGIN__

#include <e32std.h>
#include <e32base.h>
#include <es_sock.h>
#include <obex.h>
#include <btsdp.h>
#include <btmanclient.h>
#include <SrcsInterface.h>
#include <nsmldebug.h>
#include <nsmlconstants.h>
#include <nsmldefines.h>
#include <nsmlsyncalerthandlerif.h>

#include "nsmltransport.h"
#include "NSmlObexDSpluginConstants.h"
#include "nsmlobexserversession.h"
#include "nsmlobexcommserver.h"
#include "nsmlobexdefs.h"


#ifdef __NSML_DEBUG__
#include "wbxml2xmlconverter.h"
#endif // __NSML_DEBUG__

const TInt KPluginTimeOutValue = 180000000;// Time (in milliseconds) for the timer

class CDisListener;
class CNSmlObexServerSession;
class CPluginServiceTimer;
class CThreadObserver;
class CNSmlObexPacketWaiterAsync;

enum TNSmlObexContent
    {
    ENSmlDataSync,
    ENSmlDeviceManagement
    };


class CNSmlObexDSplugin : public CSrcsInterface,
			public MObexServerNotifyAsync, public MNSmlAlertObexPluginContext
    {
friend class CDisListener;

public:
	IMPORT_C static CNSmlObexDSplugin* NewL();
    IMPORT_C static CNSmlObexDSplugin* NewLC();
    ~CNSmlObexDSplugin();

	// virtuals from MNSmlAlertObexPluginContext
	RLibrary& AlertLibrary();
	TThreadId* AlertThreadId();

	TBool IsOBEXActive();

public:
	TPtr8 iPtr;
	TPtr8 iSendPtr;
	HBufC8* iData;
	RNSmlDSObexServerSession iSs;
	TBool iStopped;

private:
    void ConstructL();
    CNSmlObexDSplugin();

	static TInt CreateServerL( TAny* aPtr );
	static void StartServerL( TAny* aPtr );
	TInt ForwardZeroPkgL( TNSmlObexContent aSyncSelection );
	TInt CheckMimeType();
	void TrimRightSpaceAndNull( TDes8& aDes ) const;
	void Disconnect();
	TInt ConvertError( TInt aError );
    TInt SetObexServer( CObexServer* aObexServer );
    void BluetoothNameByAddressL( TSockAddr aAddr );

private: // from CSrcsInterface
    void ErrorIndication( TInt aError );
    void TransportUpIndication();
    void TransportDownIndication();
	void ObexDisconnectIndication( const TDesC8& aInfo );
	TInt PutPacketIndication();
	TInt GetPacketIndication();
	void AbortIndication();
	void SetMediaType( TSrcsMediaType aMediaType );
	void GetCompleteIndication ();
	void GetRequestIndication (CObexBaseObject *aRequestedObject);
	void PutCompleteIndication ();
	void PutRequestIndication ();
	void SetPathIndication (const CObex::TSetPathInfo& aPathInfo, const TDesC8& aInfo);
	void ObexConnectIndication  (const TObexConnectInfo& aRemoteInfo, const TDesC8& aInfo);
	void CancelIndicationCallback();

private:
	enum TState
		{
		EIdle,
		EGetSendPacket
		};

	TBool iZeroPkg;
	TNSmlObexContent iObexContent;
    CBufFlat* iObexBufData;
    CObexBufObject* iObexBufObject;
	CDisListener* iListener;
	TState iState;
	TBool iSessCancelled;
	TBool iSessionActive;
    CObexServer* iObexServer;

	RLibrary iAlertLib;
	TThreadId iThreadId;

	TBTDevAddr iBtAddr;
	HBufC* iBtName;

	CPluginServiceTimer* iServiceTimer;
	TBool iStartTimer;
	TSrcsMediaType iMediaType;
	RThread* iServerThread;

	TBool iConnectionAlive;

private:
	friend class CPluginServiceTimer;

	CNSmlObexPacketWaiterAsync* iGetPacketWaiterAo;

	TInt iResponseError;
	};


class CDisListener : public CActive
	{
public:

	CDisListener( RNSmlDSObexServerSession& aSs, CNSmlObexDSplugin* aPlugin )
			: CActive(0), iSessCancelled(EFalse), iSs(aSs), iPlugin( aPlugin )
	{
		CActiveScheduler::Add(this);
	};

	~CDisListener();
	void RunL();
	void DoCancel();
	void ListenDisconnect();
	TBool iSessCancelled;

private:
	enum TState
		{
		EIdle,
		EListening
		};
	TState iState;
	RNSmlDSObexServerSession& iSs;
	CNSmlObexDSplugin* iPlugin;
	};


//============================================================
// Class CPluginServiceTimer declaration
//============================================================

class CPluginServiceTimer : public CActive
	{
	public:
		CPluginServiceTimer( CNSmlObexDSplugin* aEngine, TInt aValue );
		void ConstructL();
		~CPluginServiceTimer();
		void StartTimer();
		void StopTimer();

	private:
		void DoCancel();
		void RunL();

	private:
		RTimer iTimer;
		CNSmlObexDSplugin* iEngine;
		TInt iValue;
	};

//============================================================
// CNSmlObexPacketWaiterAsync
//
//============================================================
class CNSmlObexPacketWaiterAsync : public CActive
	{
public:
	static CNSmlObexPacketWaiterAsync* NewL (RNSmlDSObexServerSession& aSession,
									  CBufFlat* aObexBufData,
									  CObexBufObject* aObexBufObject);

	void WaitForGetData() ;
	inline const TDesC8& Data() const { return *iData; }

	void SetObexServer( CObexServer* aObexServer );

	~CNSmlObexPacketWaiterAsync();

private:
	CNSmlObexPacketWaiterAsync (RNSmlDSObexServerSession& aSession,
							    CBufFlat* aObexBufData,
							    CObexBufObject* aObexBufObject);

	void ConstructL();
	void RunL();
	void DoCancel();

private:

	enum TState
		{
		EIdle,
		EGetting
		};

	RNSmlDSObexServerSession& iSession;
	CObexServer* iObexServer ;
	CBufFlat* iObexBufData;
    CObexBufObject* iObexBufObject;
    HBufC8* iData;
	TPtr8 iPtr;
	TState iState;
	};

#endif // __NSMLOBEXDSPLUGIN__

