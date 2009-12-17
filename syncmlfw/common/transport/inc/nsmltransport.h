/*
* Copyright (c) 2003 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  SyncML transport interface
*
*/


#ifndef __NSML_TRANSPORT_H__
#define __NSML_TRANSPORT_H__

#include <e32base.h>
///#include "Extbtsearcherobserver.h"
#include "bttypes.h"

class MExtBTSearcherObserver;

_LIT(KSmlHTTPClientLibName, "nsmlhttp.dll");

_LIT(KSmlObexClientLibName, "nsmlobexclient.dll");
_LIT(KSmlObexServerBindingLibName, "nsmlobexserverbinding.dll");
_LIT(KSmlObexUsbClientLibName, "nsmlobexusbclient.dll");


const TInt KSmlTransBindingUidValue = 0x101F6DE4;
const TUid KSmlTransBindingUid = {KSmlTransBindingUidValue};


enum TNSmlObexTransport
{
	EObexBt,
	EObexUsb,
	EObexIr
};


class CNSmlHTTP;
class CNsmlObexClient;
class CNSmlObexServerBinding;

// Reserved to maintain binary compability 
class CNSmlFutureReservation;


//============================================================
// CNSmlTransport declaration
//============================================================
class CNSmlTransport : public CBase 
    {
    public:
		IMPORT_C static CNSmlTransport* NewL();
		IMPORT_C static CNSmlTransport* NewLC();
		IMPORT_C CNSmlTransport();
		IMPORT_C ~CNSmlTransport();
		IMPORT_C void ConstructL();
		IMPORT_C void ConnectL( TUid aMediumType, TBool aServerAlerted, CArrayFixFlat<TUint32>* aIAPIdArray, TDesC8& aURI, TDesC8& aMimeType, TRequestStatus &aStatus, TDesC8& aHTTPusername, TDesC8& aHTTPpassword, TInt aHTTPauthused ); 
		IMPORT_C void Disconnect();
		IMPORT_C void Send( TDesC8& aStartPtr, TBool /*aFinalPacket*/, TRequestStatus &aStatus );
		IMPORT_C void Receive( TPtr8& aStartPtr, TRequestStatus &aStatus );
		IMPORT_C void ChangeTargetURIL( TDesC8& aURI );
		IMPORT_C void SetBTConnInfo( const TBTDevAddr aBTDevAddr, const TUUID aUid );
		IMPORT_C void SetExtObserver( MExtBTSearcherObserver* aExtObserver );

	private:
		void Connect( CArrayFixFlat<TUint32>* aIAPIdArray, TDesC8& aURI, TDesC8& aMimeType, TRequestStatus &aStatus, TDesC8& aHTTPusername, TDesC8& aHTTPpassword, TInt aHTTPauthused ); 
		void Connect( TNSmlObexTransport aTransport,TBool aServerAlerted, TDesC8& aMimeType, TRequestStatus &aStatus );
        static TBool IsInOfflineModeL(); // 1.2 CHANGES: Offline mode
		void LoadObexDllL();
		void LoadHTTPDllL();
		void LoadObexServerBindingDllL();

	public:
		CNSmlObexServerBinding* iObexServerBinding;
		CNsmlObexClient* iObexBinding;
		CNSmlHTTP* iHTTPBinding;
		TRequestStatus* iAgentStatus;

	private:
		TBool iObex;
		TBool iObexServer;
		RLibrary iObexServerBindingLib;
		RLibrary iObexLibrary;
		RLibrary iHTTPLibrary;
		TUid iCurrMedium;
		// Bluetooth specific members
		TBTDevAddr iBTDevAddr;
		TUUID iServiceUid;
		MExtBTSearcherObserver* iBTObserver;
		// Reserved to maintain binary compability
		CNSmlFutureReservation* iReserved;
		TBool iWLANBearer;
		
    };


#endif // __NSML_TRANSPORT_H__
