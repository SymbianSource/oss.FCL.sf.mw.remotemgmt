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



#ifndef TRUSTED_SESSION_HEADER_
#define TRUSTED_SESSION_HEADER_

// INCLUDES

#include "ElementBase.h"


#include <PolicyEngineClientServer.h>
#include <e32base.h>
#include <ssl.h>
#include <e32cmn.h>

class CX509Certificate;
class CX500DistinguishedName;

// CONSTANTS

const TInt KSerialNoLength = 20;
const TInt KCertNameLength = 100;

// MACROS
// DATA TYPES
// FUNCTION PROTOTYPES
// FORWARD DECLARATIONS

class CPolicyStorage;
class CCertificateMaps;
class CElementBase;
class CSubjectInfo;

// CLASS DECLARATION


class CTrustedSession : public CBase
{
	public:
		/**
		* CTrustedSession Constructor for CTrustedSession class
        */
		CTrustedSession();

		/**
		* ~CTrustedSession Destructor for CTrustedSession class
        */
		~CTrustedSession();
	
		/**
		* NewL() Symbian specific two phase constructor
		* @return Pointer to created CTrustedSession object
        */
		static CTrustedSession * NewL();
	
		void ConstructL();
	
		void MakeSessionTrustL( const RMessage2& aMessage);
		TCertInfo& SessionCertificate();
		void AddSessionSIDL( TUid aSecirityUID);
		void RemoveSessionTrust();
	
		TBool CertificatedSession() const;
		const TDesC8& CASNForSessionL(); 
		const TDesC8& SIDForSession(); 
		
		TBool CertMatchL( const TDesC8& aTrustedSubject1, const TDesC8& aTrustedSubject2, TBool aUseEditedItems );
		TBool RoleMatchL( const TDesC8& aTrustedSubject, const TDesC8& aRole, TBool aUseEditedItems );
		
		const TDesC8& CommonNameForSubjectL( const TDesC8& aTrustedSubject, TBool aUseEditedItems ); 
		const TDesC8& FingerPrintForSubjectL( const TDesC8& aTrustedSubject, TBool aUseEditedItems ); 
 		
  		TBool IsCertificateMappingValidL( CElementBase * aElement);
  		void GetMappedTargetTrustedSubjectL( RPointerArray<HBufC8>& aTargetTrustedMappedSubjectArray, CElementBase * aElement, TBool aUseEditedItems );
  		HBufC8* GetTargetTrustedSubjectL( CElementBase * aElement, TBool aUseEditedItems );
  		CElementBase* CreateCertificateMappingL( const TDesC8& aAlias );
		 
		void ReplaceIllegalCharacters( TPtr8& aData );
 		
	private:
		TBool iSessionTrust;
		TCertInfo iSessionCertInfo;	
		TBuf8<40> iFingerPrint;
		TBuf8<40> iSessionSID;
		
		HBufC8* iCASN;
};

class CCertificateMaps : public CBase
{
	public:
		class CRolesInfo : public CBase
		{
			public:
				CRolesInfo();
				~CRolesInfo();
				static CRolesInfo* NewL( const TDesC8& aRole, TElementState aState);
			public:
				HBufC8 * iRole;
				TElementState iState;
		};
	
		typedef RPointerArray<CRolesInfo> RRoles;
		typedef RPointerArray<CSubjectInfo> RInfos;
		class CSubjectInfo : public CBase
		{
			public:
				CSubjectInfo();
				~CSubjectInfo();
				
				TBool Match( const TDesC8& aTrustedSubject, TBool aCheckMappedCertificates);
				TBool RoleMatchL( const TDesC8& aRole);
				TRole CertificateRoleL( TBool aUseEditedItems );

			public:
				HBufC8* iCASN;					//CA+SerialNumber+FP
				HBufC8* iMappedToCert;			//CA+SerialNumber+FP
				CSubjectInfo* iMappedToInfo;	//pointer to subject info
				HBufC8* iAlias;					//Alias for certificate
				HBufC8* iSubject;				//Issued to information
				HBufC8* iFingerPrint;			//fingerprint
				RRoles iRoles;					//list of roles which are valid for subject 
				TElementState iState;			//indicates corresponding rule element state
		};

	public:
		CCertificateMaps();
		~CCertificateMaps();
		static CCertificateMaps* NewL();

		void NewMappingsAvailable();
		TRole CertificateRoleL( const TCertInfo& aCertInfo, TBool aUseEditedItems);
 		static void CreateSubjectInfoL( CSubjectInfo * aInfo, const TDesC8& aCertificate);
 		static void CreateSubjectInfoL( CSubjectInfo * aInfo, const TCertInfo& aCertInfo);
 		static void CreateFingerPrint( TDes8& iFingerPrint, const TCertInfo& aCertInfo);
 		static HBufC8* CasnForCertInfoL( const TCertInfo& aInfo);
 		
 		static CX509Certificate* ParseCertificateL( const TDesC8& aCertificate);
		static TBool EvaluateElementValidity( const TBool aUseEditedElements, const TElementState aState);
		
	
	private:
		void BringUpToDateL();
		void LoadCertificateMapsL();
		void AddRolesToCertL( const TDesC8& aRole, const TDesC8& aSubject, TElementState aElementState);
		static void ParseSubjectInfoL( CSubjectInfo* aInfo, CX509Certificate* aCertificate);
		static HBufC8* ParseDNL( const CX500DistinguishedName * aDN, TBool aOnlyCommonName);
		
		CSubjectInfo* FindSubjectL( const TDesC8& aTrustedSubject, TBool aUseEditedItems);
		
		TBool IsAliasExistL( const TDesC8& aAlias);
	private:
		RPointerArray<CSubjectInfo> iCerts;
		CPolicyStorage* iPolicyStorage;
		TBool iMapsUpToDate;
		
		friend class CTrustedSession;
};

#endif 