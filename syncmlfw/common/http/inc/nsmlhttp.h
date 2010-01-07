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
* Description:  SyncML HTTP client
*
*/




#ifndef _SMLTRANSPORT_H_
#define _SMLTRANSPORT_H_

#include <e32base.h>
#include <e32cons.h>
#include <reent.h>

#include <http.h>
#include <chttpformencoder.h>
#include <http/mhttpauthenticationcallback.h>
#include <e32test.h>
#include <f32file.h>
#include <uri8.h>
#include <nifman.h>

#include <nsmldebug.h>
#include <nsmlconstants.h>
#include <nsmldefines.h>

#include "nsmltransport.h"
#include "nsmldialiap.h"

#include <rconnmon.h>

// FORWARD DECLARATION
class CNSmlXptShutdownTimer;
class CNSmlHTTP;
class CHttpEventHandler;
//Class to observe the network status events 
class CNsmlNetworkStatusEngine;
// Reserved to maintain binary compability 
class CNSmlFutureReservation;


// Maximum size of buffer to hold content-type data
//
const TInt KMaxContentTypeSize = 256;
// DM ONLY : Maximum duration in seconds for an incoming call  
//during the session which will be used to decide if packet 
//is to be resent or not . This is applicable only for DM 
const TInt KDMMaxHttpAutoResumeDurationSec = 160;

//============================================================
// Class CNSmlHTTPErr
//============================================================
class TNSmlHTTPErrCode
	{
	public:	
	enum 
		{
		/* 'Client Error' range of codes 4xx */
		ENSmlHTTPErrCodeBase						= 1400,
		ENSmlHTTPErr_BadRequest						= ENSmlHTTPErrCodeBase + HTTPStatus::EBadRequest,
		ENSmlHTTPErr_Unauthorized					= ENSmlHTTPErrCodeBase + HTTPStatus::EUnauthorized,
		ENSmlHTTPErr_PaymentRequired				= ENSmlHTTPErrCodeBase + HTTPStatus::EPaymentRequired,
		ENSmlHTTPErr_Forbidden						= ENSmlHTTPErrCodeBase + HTTPStatus::EForbidden,
		ENSmlHTTPErr_NotFound						= ENSmlHTTPErrCodeBase + HTTPStatus::ENotFound,
		ENSmlHTTPErr_MethodNotAllowed				= ENSmlHTTPErrCodeBase + HTTPStatus::EMethodNotAllowed,
		ENSmlHTTPErr_NotAcceptable					= ENSmlHTTPErrCodeBase + HTTPStatus::ENotAcceptable,
		ENSmlHTTPErr_ProxyAuthenticationRequired	= ENSmlHTTPErrCodeBase + HTTPStatus::EProxyAuthenticationRequired,
		ENSmlHTTPErr_RequestTimeout					= ENSmlHTTPErrCodeBase + HTTPStatus::ERequestTimeout,
		ENSmlHTTPErr_Conflict						= ENSmlHTTPErrCodeBase + HTTPStatus::EConflict,
		ENSmlHTTPErr_Gone							= ENSmlHTTPErrCodeBase + HTTPStatus::EGone,
		ENSmlHTTPErr_LengthRequired					= ENSmlHTTPErrCodeBase + HTTPStatus::ELengthRequired,
		ENSmlHTTPErr_PreconditionFailed				= ENSmlHTTPErrCodeBase + HTTPStatus::EPreconditionFailed,
		ENSmlHTTPErr_RequestEntityTooLarge			= ENSmlHTTPErrCodeBase + HTTPStatus::ERequestEntityTooLarge,
		ENSmlHTTPErr_RequestURITooLong				= ENSmlHTTPErrCodeBase + HTTPStatus::ERequestURITooLong,
		ENSmlHTTPErr_UnsupportedMediaType			= ENSmlHTTPErrCodeBase + HTTPStatus::EUnsupportedMediaType,
		ENSmlHTTPErr_RequestedRangeNotSatisfiable	= ENSmlHTTPErrCodeBase + HTTPStatus::ERequestedRangeNotSatisfiable,
		ENSmlHTTPErr_ExpectationFailed				= ENSmlHTTPErrCodeBase + HTTPStatus::EExpectationFailed,
		/* 'Server Error' range of codes 5xx */
		ENSmlHTTPErr_InternalServerError			= ENSmlHTTPErrCodeBase + HTTPStatus::EInternalServerError,
		ENSmlHTTPErr_NotImplemented					= ENSmlHTTPErrCodeBase + HTTPStatus::ENotImplemented,
		ENSmlHTTPErr_BadGateway						= ENSmlHTTPErrCodeBase + HTTPStatus::EBadGateway,
		ENSmlHTTPErr_ServiceUnavailable				= ENSmlHTTPErrCodeBase + HTTPStatus::EServiceUnavailable,
		ENSmlHTTPErr_GatewayTimeout					= ENSmlHTTPErrCodeBase + HTTPStatus::EGatewayTimeout,
		ENSmlHTTPErr_HTTPVersionNotSupported		= ENSmlHTTPErrCodeBase + HTTPStatus::EHTTPVersionNotSupported
		};
	};

//============================================================
// Class CNSmlXptShutdownTimer
//============================================================
class CNSmlXptShutdownTimer : public CActive
	{
	public:
		// construct/destruct
		CNSmlXptShutdownTimer( CNSmlHTTP* aEngine );
		void ConstructL();
		~CNSmlXptShutdownTimer();
		// start timer
		void Start();
		void StartAborted();
	private:
		// from CActive
		void DoCancel();
		void RunL();
		TTime iTimerSnapTime;
	private:
		RTimer iTimer;
		CNSmlHTTP* iEngine; 
	};

//============================================================
// Class CNSmlHTTP declaration
//============================================================
class CNSmlHTTP : public CActive, public MHTTPDataSupplier, public MHTTPAuthenticationCallback
	{

	public:
		enum TEngineState {	ExptIdle, 
							ExptOpenCommunication, 
							ExptCloseCommunication,
							ExptReceiveData, 
							ExptSendData
							};

		enum TServerContentEncoding { ExptNone,
		                              ExptDeflate
	                            };

	public:
		// construct / destruct
		static CNSmlHTTP* NewL();        
		CNSmlHTTP();
		virtual void ConstructL();
		virtual ~CNSmlHTTP();

		// syncml xpt api
		virtual void OpenCommunicationL( CArrayFixFlat<TUint32>* aIAPidArray, TDesC8& aURI, TDesC8& aMimeType, TRequestStatus &aStatus, TDesC8& aHTTPusername, TDesC8& aHTTPpassword, TInt aHTTPauthused );
		virtual void CloseCommunicationL( TRequestStatus &aStatus );
		virtual void ReceiveDataL( TDes8& aStartPtr, TRequestStatus &aStatus );
		virtual void SendDataL( TDesC8& aStartPtr, TBool aFinalPacket, TRequestStatus &aStatus );
		virtual void ChangeTargetURIL( TDesC8& aURI );

	    void CompleteRequest();
		
	public:
		// methods inherited from MHTTPAuthenticationCallback
		virtual TBool GetCredentialsL (const TUriC8 &aURI,RString aRealm,RStringF aAuthenticationType,RString &aUsername,RString &aPassword); 

	public:
		TRequestStatus* iAgentStatus;
		//iPreemptRequest: Specifies how many requests were completed
		//before the httpevent handler sends the status back.
		//This can occur due to long wait time, http layer in some scenario sends
		//status to client after 5-9 minutes
		TInt iPreemptRequest;
        TBool iTimeOut;
		TServerContentEncoding iServerContentEncoding;
		TServerContentEncoding iServerAcceptEncoding;
		TInt iMaxMsgSize;

	private:
		// methods inherited from MHTTPDataSupplier
		TBool GetNextDataPart(TPtrC8& aDataPart);
		void ReleaseData();
		TInt OverallDataSize();
		TInt Reset();
		// from CActive
		void DoCancel();
		void RunL();
		// set http error status
		static TInt SetErrorStatus( TInt aError );
		void ProcessRequestL();
		void InvokeHttpMethodL( const TDesC8& aUri, RStringF aMethod );
		void SetHeaderL( RHTTPHeaders aHeaders, TInt aHdrField, const TDesC8& aHdrValue );
		void GetResponseBodyL( TDes8& aStartPtr );
		void SetHttpConnectionInfoL( TBool aUseOwnConnection );

		void SaveCertificateL( RHTTPTransaction &aTransaction );
		void DeleteCertificate();

	
		void CompressL(TDesC8& aStartPtr);
		void DecompressL(TDes8& aStartPtr);
		TInt ReadRepositoryL(TInt aKey, TInt& aValue);
	private:
		HBufC8* iData;
		HBufC8* iReqBodySubmitBuffer;
		TPtr8 iReqBodySubmitBufferPtr;

		TInt iDocumentLength;
		TInt iIAPid;
        CArrayFixFlat<TUint32>* iIAPidArray;
		HBufC8* iURI;
		HBufC8* iMimeType;
		CNSmlDialUpAgent* iDialUpAgent;

		HBufC8* iHTTPusername;
		HBufC8* iHTTPpassword;
		TInt iAuthUsed;		
		TInt iAuthRetryCount;

		// timer for closing transport activity
		CNSmlXptShutdownTimer* iShutdown;
		CNsmlNetworkStatusEngine* iNetworkStatusEngine;
		TBool iLastPart;
		TEngineState iEngineState;
		RHTTPSession iSess;
		RHTTPTransaction iTrans;
		CHttpEventHandler* iTransObs;
		// Reserved to maintain binary compability
		CNSmlFutureReservation* iReserved;
		//Stores the current ongoing session
		TInt iSession;

	private:
		friend class CNSmlXptShutdownTimer;
		friend class CHttpEventHandler;
		friend class CNsmlNetworkStatusEngine;
	};

	IMPORT_C CNSmlHTTP* CreateCNSmlHTTPL();
	typedef CNSmlHTTP* (*TNSmlCreateHTTPFunc) ();   




//============================================================
// Class CNsmlNetworkStatusEngine
// This is used for listening for gprs suspend/resume events
//============================================================


class CNsmlNetworkStatusEngine :public CBase, private MConnectionMonitorObserver

{
		public:
        CNsmlNetworkStatusEngine( CNSmlHTTP* aAgent );
		~CNsmlNetworkStatusEngine();
		void ConstructL();
	
    public: // New methods
        
      	/**
        * Starts notifications.
        */
		void NotifyL() ;
		
        /**
        * Stops notifications.
        */
       	void StopNotify() ;
       	
       	/**
        * Stops DM session.
        */
        void StopDMSession();
        			
	private : // From MConnectionMonitorObserver
		void EventL( const CConnMonEventBase& aConnMonEvent ) ;

	private: // Data 
	
		RConnectionMonitor            iConnectionMonitor;
		TRequestStatus                iStatus;
		TInt                          iNetwStatus;
		CNSmlHTTP* 	iAgent;
		TBool iTimerOn;
		CPeriodic*          iTimedExecuteStopSession;
		TInt iDMSmlSessionTimeout;
		TTime iSuspendedTime;
		TTime iSuspendDuration;
		
};

#endif