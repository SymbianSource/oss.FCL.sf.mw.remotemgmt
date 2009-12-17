/*
* Copyright (c) 2000 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description: Implementation of policymanagement components
*
*/


#ifndef __DMUTIL_SERVER_H__
#define __DMUTIL_SERVER_H__

// INCLUDES

#ifdef __TARM_SYMBIAN_CONVERGENCY 
#include <e32property.h>
#else
// nothing
#endif

#include <e32base.h>
#include "DMUtilClient.h"

// CONSTANTS
#include "PMUtilInternalCRKeys.h"

// ----------------------------------------------------------------------------------------
// DMUtilServer server panic codes
// ----------------------------------------------------------------------------------------
enum TDMUtilPanic
	{
	EPanicGeneral,
	EPanicIllegalFunction,
	EBadDescriptor
	};

// MACROS
// DATA TYPES
// FUNCTION PROTOTYPES

void PanicClient(const RMessagePtr2& aMessage,TDMUtilPanic aPanic);

// FORWARD DECLARATIONS
class CRepository;
class CACLStorage;

// CLASS DECLARATION

/**
*  MDMCertNotifierCallback
*  Description.
*/

class MDMCertNotifierCallback
    {
    public: virtual void CertificateChangedL() = 0;
    public: virtual void NotifierErrorL( TInt aCode ) = 0;
    };

/**
*  CDMCertNotifier
*  Description.
*/

class CDMCertNotifier : public CActive
    {
private:

	enum TSessionCertStatus
		{
		ESessionCertContentZero,
		ESessionCertHasChanged,
		ESessionCertNoChange
		};
		
public:
    CDMCertNotifier();
    ~CDMCertNotifier();
    void StartL( MDMCertNotifierCallback* aCallback );
    void RunL();
    void DoCancel();
	void RunLToBeTrappedL();
	
	void SaveCertToCache();	
	CDMCertNotifier::TSessionCertStatus GetCertStatus();
	void ResetCertCache();
	TBool IsEmpty( const TCertInfo& aCertInfo ) const;
	TBool IsEqual( const TCertInfo& aCertInfo1, 
				   const TCertInfo& aCertInfo2 ) const;
	
	TInt GetCert( TCertInfo& aCertInfo );
	
private:
    MDMCertNotifierCallback* iCallback;

#ifdef __TARM_SYMBIAN_CONVERGENCY    
    RProperty iCertificateProperty;
#else    
    CRepository* iRepository;
#endif    
    // used to filter out unnecessary "certificate changed events"
    TCertInfo iCachedSessionCertificate;
    };

/**
*  CDMUtilServer
*  Description.
*/

class CDMUtilServer : public CPolicyServer, public MDMCertNotifierCallback
	{
	friend class CDMUtilSession;

public:
    enum TPropertyDMCertKeys {EDMCert};

	~CDMUtilServer();

	static CServer2*    NewLC                   ();
	void                Panic                   ( TInt aPanicCode );
	void                CertificateChangedL     ();
    void                NotifierErrorL          ( TInt aCode );

	static CACLStorage * ACLStorage();

protected:
	CSession2* NewSessionL(const TVersion&,const RMessage2&) const;
    CPolicyServer::TCustomResult CustomSecurityCheckL(const RMessage2& aMsg, TInt& aAction, TSecurityInfo& aMissing);

private:
	CDMUtilServer();
	void                ConstructL              ();
	void                UpdateSessionInfoL      ();
	HBufC8*             GetServerIDL            ();
	TInt                GetCertFromCentRep      ();
	TInt                UpdateACLsL              ();
	TBool 				IsSilentModeAllowed		();
	HBufC8* 			GetServerIDFromSyncMLSessionL();
	HBufC8* 			GetServerIDFromLocalCacheL();



private:
	// Server policy
	#define DMUTILSERVER_NUMBER_OF_POLICIES 3
	static CPolicyServer::TPolicy               iTcConnectionPolicy;
    static const TInt                           tcRanges[DMUTILSERVER_NUMBER_OF_POLICIES];
    static const TUint8                         tcElementsIndex[DMUTILSERVER_NUMBER_OF_POLICIES];
    static const CPolicyServer::TPolicyElement  tcElements[DMUTILSERVER_NUMBER_OF_POLICIES];
	static TBool iManagementActive;

    TCertInfo iCertInfo;
    HBufC8*   iServerID;
    CDMCertNotifier iNotifier;
    static CACLStorage* iACLs;
	};

#endif //__DMUTIL_SERVER_H__
