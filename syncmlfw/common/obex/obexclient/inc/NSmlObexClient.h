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
* Description:  SyncML Obex client
*
*/


#ifndef __NSMLOBEXCLIENT_H__
#define __NSMLOBEXCLIENT_H__

#include <e32base.h>
#include <e32std.h>
#include <obex.h>

#include "nsmltransport.h"
#include "Obexsearcherobserver.h"
#include "Btconninfo.h"

const TUint16 KNSmlObexClientGranularity = 1024;
const TUint16 KNameLen = 256;

_LIT8( ObexName, "SyncML Obex" );


#ifndef __NSMLOBEXCLIENT_PAN__
#define __NSMLOBEXCLIENT_PAN__


/** CNsmlObexClient panic codes */
enum TNSmlObexClientPanics 
    {
    EBTObjectExchangeBasicUi = 1,
    EBTObjectExchangeReceiverInvalidState,
    EBTObjectExchangeSenderExists,
    EBTObjectExchangeReceiverExists,
    EBTObjectExchangeSenderInvalidState,
    EBTObjectExchangeNoSender,
    EBTObjectExchangeAddMessage,
    EBTObjectExchangeNextRecordRequestComplete,
    EBTObjectExchangeAttributeRequestResult,
    EBTObjectExchangeAttributeRequestComplete,
    EBTObjectExchangeInvalidControlIndex,
    EBTObjectExchangeProtocolRead,
    EBTObjectExchangeAttributeRequest,
    EBTObjectExchangeSdpRecordDelete,
    EBTObjectExchangeUnexpectedState,
    EBTObjectExchangeUnexpectedLogicState
    };

inline void Panic(TNSmlObexClientPanics aReason)
    { 
    User::Panic(_L("NSMLOBEXCLIENT"), aReason);
    }

#endif // __NSMLOBEXCLIENT_PAN__



class CNSmlObexServiceSearcher;
class CObexSearcherBase;
class MExtBTSearcherObserver;
//class T_CNsmlObexClient;	//For testing purposes

class CNsmlObexClient : public CActive, public MObexSearcherObserver
    {
public:
// 	friend T_CNsmlObexClient;	//For testing purposes

	CNsmlObexClient();
    static CNsmlObexClient* NewL();        
	virtual void ConstructL ( const TDesC8& aName );
    virtual ~CNsmlObexClient();

	virtual void ConnectL( TNSmlObexTransport aTransport,TBool aServerAlerted, TDesC8& aMimeType, TRequestStatus &aStatus );
	virtual void CloseCommunicationL( TRequestStatus &aStatus );
	virtual void ReceiveDataL( TPtr8& aStartPtr, TRequestStatus &aStatus );
	virtual void SendDataL( TDesC8& aStartPtr, TBool /*aFinalPacket*/, TRequestStatus &aStatus );
	virtual void SetBTConnInfo( const TBTDevAddr aBTDevAddr, const TUUID aUid );
	virtual void SetExtObserver( MExtBTSearcherObserver* aExtObserver );

	virtual TBool IsBusy();
public:
	TRequestStatus* iAgentStatus;

protected:    // from CActive
    void DoCancel();
    void RunL();
    TInt RunError(TInt aError);

protected:		//from MObexSearcherObserver
    void HandleDeviceFoundL();
    void HandleDeviceErrorL( TInt aErr );
    void HandleServiceFoundL();
    void HandleServiceErrorL( TInt aErr );


private:
    void ConnectToServerL();
	void StopL();
	void DisconnectL();
    TBool IsConnected();
    void ErrorConversion( const TInt aError, TInt& aErrCode );
    
private:

    enum TState 
        {
		EDisconnected,
        EGettingConnection,
        EWaitingToSend,
		EWaitingToReceive,
        EDisconnecting,
		EConnectionFailed
        };

    /*! @var iState the state of the active object, determines behaviour within the RunL method. */
    TState iState;

	CObexSearcherBase*				iObexSearcher;
	MExtBTSearcherObserver*			iExtObserver;
	TBTConnInfo						iBTConnInfo;

    CObexClient*					iClient;
    CObexBufObject*					iCurrObject;
	HBufC8*							iMimeType;
	TInt							iDocumentLength;
	CBufFlat*						iDataBuf;
	TPtr8							iDataPtr;
    };

	IMPORT_C CNsmlObexClient* CreateCNsmlObexClientL();
	typedef CNsmlObexClient* (*TNSmlCreateObexClientFunc) ();   

#endif 

