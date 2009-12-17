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
* Description:  SyncML Obex plugin for Device Management
*
*/


#ifndef __NSMLOBEXDMPLUGIN__
#define __NSMLOBEXDMPLUGIN__

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

#include "nsmltransport.h"
#include "NSmlObexDMpluginConstants.h"
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
class CNSmlObexPacketWaiter;

enum TNSmlObexContent
    {
    ENSmlDataSync,
    ENSmlDeviceManagement
    };



class CNSmlObexDMplugin : public CSrcsInterface, public MObexServerNotify
    {
friend class CDisListener;

public:
	IMPORT_C static CNSmlObexDMplugin* NewL();
    IMPORT_C static CNSmlObexDMplugin* NewLC();
    ~CNSmlObexDMplugin();

	// Get MObexServerNotify object to bind to CObexServer
	MObexServerNotify* GetServerCallback();
	TBool IsOBEXActive();

public:
	TPtr8 iPtr;
	TPtr8 iSendPtr;
	HBufC8* iData;
	RNSmlDMObexServerSession iSs;
	TBool iStopped;

private:
    void ConstructL();
    CNSmlObexDMplugin();

	static TInt CreateServer( TAny* aPtr );
	static void StartServerL( TAny* aPtr );
	void TrimRightSpaceAndNull( TDes8& aDes ) const;
	TInt ForwardZeroPkgL( TNSmlObexContent aSyncSelection );
	TInt CheckMimeType();
	void Disconnect();
	TInt ConvertError( TInt aError );

private: // from CSrcsInterface
    void ErrorIndication( TInt aError );
    void TransportUpIndication();
    void TransportDownIndication();
    TInt ObexConnectIndication( const TObexConnectInfo& aRemoteInfo, const TDesC8& aInfo );
    void ObexDisconnectIndication( const TDesC8& aInfo );
    CObexBufObject* PutRequestIndication();
    TInt PutPacketIndication();
    TInt PutCompleteIndication();
    CObexBufObject* GetRequestIndication( CObexBaseObject* aRequestedObject );
    TInt GetPacketIndication();
    TInt GetCompleteIndication();
    TInt SetPathIndication( const CObex::TSetPathInfo& aPathInfo, const TDesC8& aInfo );
    void AbortIndication();
	void SetMediaType( TSrcsMediaType aMediaType );
	TInt SetObexServer( CObexServer* aObexServer );
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
	CPluginServiceTimer* iServiceTimer;
	TBool iStartTimer;
	TSrcsMediaType iMediaType;

private:
	friend class CPluginServiceTimer;
	CNSmlObexPacketWaiter* iGetPacketWaiter;

	};


class CDisListener : public CActive
	{
public:
	CDisListener( RNSmlDMObexServerSession& aSs, CNSmlObexDMplugin* aPlugin ) : CActive(0), iSessCancelled(EFalse), iSs(aSs), iPlugin( aPlugin ) { CActiveScheduler::Add(this); };
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
	RNSmlDMObexServerSession& iSs;
	CNSmlObexDMplugin* iPlugin;
	};


//============================================================
// Class CPluginServiceTimer declaration
//============================================================

class CPluginServiceTimer : public CActive
	{
	public:
		CPluginServiceTimer( CNSmlObexDMplugin* aEngine, TInt aValue );
		void ConstructL();
		~CPluginServiceTimer();
		void StartTimer();
		void StopTimer();

	private:
		void DoCancel();
		void RunL();

	private:
		RTimer iTimer;
		CNSmlObexDMplugin* iEngine;
		TInt iValue;
	};

//============================================================
// CNSmlObexPacketWaiter
//
//============================================================
class CNSmlObexPacketWaiter : public CBase
	{
public:
	CNSmlObexPacketWaiter(RNSmlDMObexServerSession& aSession);
	~CNSmlObexPacketWaiter();
	void ConstructL();

public:
	TInt WaitForGetData();
	inline const TDesC8& Data() const { return *iData; }

private:
	HBufC8* iData;
	RNSmlDMObexServerSession& iSession;
	TPtr8 iPtr;
	};

#endif // __NSMLOBEXDMPLUGIN__

