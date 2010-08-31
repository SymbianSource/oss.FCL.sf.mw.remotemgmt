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
* Description:  DS Settings Provisioning Adapter
*
*/


#ifndef NSMLDSPROVISIONINGADAPTER_H
#define NSMLDSPROVISIONINGADAPTER_H

// INCLUDES
#include <CWPAdapter.h>
#include <MWPContextExtension.h>
#include <MWPVisitor.h>
#include <SyncMLClient.h>
#include <SyncMLClientDS.h>
#include <nsmlconstants.h>
#include "NSmlDsProvisioningAdapterUIDs.h"

// CONSTANTS

// DATA TYPES

// FORWARD DECLARATIONS
class CDesC16Array;

_LIT8( KNSmlTrueVal, "1" );
_LIT8( KNSmlFalseVal, "0" );

_LIT( KNSmlDsProvisioningDefName, "Profile" );

_LIT( KNSmlDsProvisioningCharacteristic, "CHARACTERISTIC" );
_LIT( KNSmlDsProvisioningNapdef, "NAPDEF" );
_LIT( KNSmlDsProvisioningApplication, "APPLICATION" );
_LIT( KNSmlDsProvisioningDMAppIdVal, "w5" );

_LIT( KNSmlDsProvisioningHTTP, "http://" );
_LIT( KNSmlDsProvisioningHTTPS, "https://" );

_LIT( KNSmlDSProvisioningHTTPBasic, "HTTP-BASIC" );
_LIT( KNSmlDSProvisioningHTTPDigest, "HTTP-DIGEST" );

_LIT( KNSmlDsPovisioningDirAndResource,	"z:NSmlDsProvisioningAdapter.rsc" );

_LIT( KNSmlDsDefaultPort, "80");
_LIT(KNSmlVersion12, "1.2");
_LIT(KNSmlVersion112, "1.1.2");
_LIT(KNSmlVersion11, "1.1");

const TUint8 KNSmlDMColon = ':';
const TUint8 KNSmlDMUriSeparator = '/';

// CLASS DECLARATION
class CNSmlDataProviderElement : public CBase
	{
	public:
		static CNSmlDataProviderElement* NewL();
		~CNSmlDataProviderElement();
	
		TUint32 iUid;
		HBufC *iRemoteDBUri;
		HBufC *iLocalDBUri;
	};

class CNSmlDsProfileElement : public CBase
	{
	public:
		static CNSmlDsProfileElement* NewL();
		~CNSmlDsProfileElement();
	
		RPointerArray<CNSmlDataProviderElement> iDataProvElement;
		CWPCharacteristic *iVisitParameter;
		TPckgBuf<TInt> iProfileId;
		HBufC *iHostAddress;
		HBufC *iPort;
		HBufC8 *iHTTPUserName;
		HBufC8 *iHTTPPassword;
		
		HBufC* iDisplayName;
		TSmlServerAlertedAction iServerAlertedAction;
		TSmlProtocolVersion iProtocolVersion;
		HBufC8* iServerId;
		HBufC8* iUsername;
		HBufC8* iPassword;
	};

class CNSmlDsProvisioningAdapter : public CWPAdapter, public MWPContextExtension
	{
	public:  // Constructors and destructor

		static CNSmlDsProvisioningAdapter* NewL();
		~CNSmlDsProvisioningAdapter();
		
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

		CNSmlDsProvisioningAdapter();
		void ConstructL();
		TInt GetDefaultIAPL();
		TInt CombineURILC( const TDesC& aAddr, const TDesC& aPort, HBufC*& aRealURI );
		void StoreAttributesL( const TDesC& aType );
		void GetTitleL();
		TDesC8& ConvertTo8LC( const TDesC& aSource );
		TDesC8& ConvertTo8L( const TDesC& aSource );
		TBool IsOperatorProfile( const CNSmlDsProfileElement& aProfile );
		TInt OperatorAdapterUid();
		void StoreOperatorUrlL( const TDesC& aUrl );

	private:
		
		TInt Reserved_1();
		TInt Reserved_2();
		
	private:  // Data

		enum TDsProvisioningState 
			{
			EStateNull = 0,
			EStateDsSettings,
			EStateApplication,
			EStateResource,
			EStateAppAuth,
			EStateNotInterested
			};

		enum TDsProvisioningAuthLevel 
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

		CNSmlDsProvisioningAdapter::TDsProvisioningState iState;
		CNSmlDsProvisioningAdapter::TDsProvisioningAuthLevel iAuthLevel;
		
		RPointerArray<CNSmlDsProfileElement> iProfiles;
		HBufC *iTitle;
		RSyncMLSession iSession;
        //To NAPID Internet indicator
        TBool iToNapIDInternetIndicator;
	};

#endif	// NSMLDSPROVISIONINGADAPTER_H
            
// End of File
