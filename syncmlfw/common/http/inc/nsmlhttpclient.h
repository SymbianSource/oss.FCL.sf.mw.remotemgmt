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
* Description:  SyncML HTTP client
*
*/



#include "nsmlhttp.h"

// Standard headers used by default
_LIT8(KSmlCacheControl, "no-store");
_LIT8(KSmlAcceptCharSet, "utf-8");
_LIT8(KSmlAcceptLanguage, "en");
_LIT8(KSmlContentType, "Content-Type");
_LIT8(KSmlAcceptEncodingType, "Accept-Encoding");
_LIT8(KSmlContentEncodingType, "Content-Encoding");
_LIT8(KSmlContentTypeDS, "application/vnd.syncml+wbxml");
_LIT8(KSmlContentTypeDM, "application/vnd.syncml.dm+wbxml");
_LIT8(KSmlContentDeflate, "deflate");

_LIT(KSmlHttpClientPanic, "HTTP Client Panic");
#define KServerUntrusted -7548 //symbian error code

enum TSmlHttpPanics
	{
		EReqBodySubmitBufferNotAllocated,
		KBodyWithInvalidSize,
		KCouldNotNotifyBodyDataPart
	};

// FORWARD DECLARATIONS
class CRepository;

//============================================================
// Class CHttpEventHandler declaration
// Handles all events for the active transaction. 
//
//============================================================
class CHttpEventHandler : public CBase, public MHTTPTransactionCallback
	{
public:
	virtual ~CHttpEventHandler();
	static CHttpEventHandler* NewLC();
	static CHttpEventHandler* NewL();
	//
	// methods from MHTTPTransactionCallback
	//
	virtual void MHFRunL( RHTTPTransaction aTransaction, const THTTPEvent& aEvent );
	virtual TInt MHFRunError( TInt aError, RHTTPTransaction aTransaction, const THTTPEvent& aEvent );

protected:
	CHttpEventHandler();
	void ConstructL( CNSmlHTTP* aAgent );
private:
	CNSmlHTTP* iAgent;
	MHTTPDataSupplier* iRespBody;
	//For storing Server Status Code
	CRepository* iRepositorySSC;
	TBool iErrorReportingEnabled;

private:
	friend class CNSmlHTTP;

	};

