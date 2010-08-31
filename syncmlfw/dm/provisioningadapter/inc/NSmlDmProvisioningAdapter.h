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
* Description:  DM Settings Provisioning Adapter
*
*/


#ifndef NSMLDMPROVISIONINGADAPTER_H
#define NSMLDMPROVISIONINGADAPTER_H

// INCLUDES
#include <CWPAdapter.h>
#include <MWPContextExtension.h>
#include <MWPVisitor.h>
#include <SyncMLClient.h>
#include <SyncMLClientDM.h>
#include <nsmldmauthinfo.h>
#include <nsmlconstants.h>
#include <nsmldmconstants.h>

#include "NSmlDmProvisioningAdapterUIDs.h"

// CONSTANTS

// DATA TYPES

// FORWARD DECLARATIONS
class CDesC16Array;

_LIT8( KNSmlTrueVal, "1" );
_LIT8( KNSmlFalseVal, "0" );

_LIT( KNSmlDMProvisioningDefName, "Profile" );

_LIT( KNSmlDMProvisioningCharacteristic, "CHARACTERISTIC" );
_LIT( KNSmlDMProvisioningNapdef, "NAPDEF" );
_LIT( KNSmlDMProvisioningApplication, "APPLICATION" );
_LIT( KNSmlDMProvisioningDMAppIdVal, "w7" );
_LIT( KNSmlDMProvisioningINIT, "INIT" );

_LIT( KNSmlDMProvisioningHTTP, "http://" );
_LIT( KNSmlDMProvisioningHTTPS, "https://" );

_LIT( KNSmlDMProvisioningServerAuth, "APPSRV" );
_LIT( KNSmlDMProvisioningClientAuth, "CLIENT" );

_LIT( KNSmlDMProvisioningHTTPBasic, "HTTP-BASIC" );
_LIT( KNSmlDMProvisioningHTTPDigest, "HTTP-DIGEST" );

_LIT( KNSmlDmPovisioningDirAndResource,	"z:NSmlDmProvisioningAdapter.rsc" );

_LIT8( KNSmlDMProvisioningNoVal, "" );
_LIT( KNSmlDMProvisioningNoVal16, "" );
_LIT( KNSmlDmDefaultPort, "80");
_LIT8( KNSmlDmStaticAlertMessagePart,	"000000000000000000000" ); //21 characters
const TUint8 KNSmlDMColon = ':';
const TUint8 KNSmlDMUriSeparator = '/';

// CLASS DECLARATION
class CNSmlDmProfileElement : public CBase
	{
	public:
		static CNSmlDmProfileElement* NewL();
		~CNSmlDmProfileElement();
		
		HBufC *iServerNonce;
		CWPCharacteristic *iVisitParameter;
		TPckgBuf<TInt> iProfileId;
		HBufC *iHostAddress;
		HBufC *iPort;
		CNSmlDMAuthInfo iAuthInfo;
		HBufC8 *iHTTPUserName;
		HBufC8 *iHTTPPassword;
		
		HBufC* iDisplayName;
		TSmlServerAlertedAction iServerAlertedAction;
		TSmlProtocolVersion iProtocolVersion;
		HBufC8* iServerId;
		HBufC8* iUsername;
		HBufC8* iPassword;
		HBufC8* iServerPassword;
	};

class CNSmlDmProvisioningAdapter : public CWPAdapter , public MWPContextExtension
	{
	public:

		static CNSmlDmProvisioningAdapter* NewL();
		~CNSmlDmProvisioningAdapter();
		
		//from CWPAdapter
		TInt ItemCount() const;
		const TDesC16& SummaryTitle( TInt aIndex ) const;
		const TDesC16& SummaryText( TInt aIndex ) const;
		void SaveL( TInt aItem );
		TBool CanSetAsDefault( TInt aItem ) const;
		void SetAsDefaultL( TInt aItem );
		TInt ContextExtension( MWPContextExtension*& );
		
		//from MWPContextExtension
		const TDesC8& SaveDataL( TInt aIndex ) const;
		void DeleteL( const TDesC8& aSaveData );
        TUint32 Uid() const;

	private:
		void VisitL( CWPCharacteristic& aElement );
		void VisitL( CWPParameter& aElement );
		void VisitLinkL( CWPCharacteristic& aCharacteristic );
		TInt DetailsL( TInt aItem, MWPPairVisitor& aVisitor );

		CNSmlDmProvisioningAdapter();
		void ConstructL();
		TInt GetDefaultIAPL();
		void GetTitleL();
		TInt CombineURILC( const TDesC& aAddr, const TDesC& aPort, HBufC*& aRealURI );
		
		TDesC8& ConvertTo8LC( const TDesC& aSource );
		TDesC8& ConvertTo8L( const TDesC& aSource );

	private:
		TInt Reserved_1();
		TInt Reserved_2();
		
	private:  // Data
		enum TDmProvisioningState 
			{
			EStateNull = 0,
			EStateDmSettings,
			EStateApplication,
			EStateNotInterested
			};

		enum TDmProvisioningAuthLevel 
			{
			ENone = 0,
			EServer,
			EClient,
			EHttp
			};

		enum TDmAuthenticationType
			{
			ENoAuth,
			EBasic,
			EMD5
			};

		CNSmlDmProvisioningAdapter::TDmProvisioningState iState;
		CNSmlDmProvisioningAdapter::TDmProvisioningAuthLevel iAuthLevel;

		RPointerArray<CNSmlDmProfileElement> iProfiles;
		HBufC *iTitle;
		RSyncMLSession iSession;
		TUint iInitSession;
		TUint iAuthSecretLimitIndicator;
	};

#endif	// NSMLDMPROVISIONINGADAPTER_H
            
// End of File
