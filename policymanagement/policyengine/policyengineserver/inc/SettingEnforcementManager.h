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


#ifndef _SETTING_ENFORCEMENT_MANAGER_HEADER__
#define _SETTING_ENFORCEMENT_MANAGER_HEADER__

// INCLUDES

#include "CentRepToolClient.h"

#include <e32base.h>
#include <ssl.h>

// CONSTANTS
//DM client UID
const TUid KDMClientUiD = { 0x101f9a02 };


// DATA TYPES
typedef RPointerArray<HBufC8> RAccessControlList;


// FORWARD DECLARATIONS
class CElementBase;
class CPolicyStorage;
class CEnforcementBase;

class CSettingEnforcementManager : public CActive
{
	public:
		//constructors
		CSettingEnforcementManager();
		~CSettingEnforcementManager();
		static CSettingEnforcementManager* NewL();
		
		//enforcement management

	    /**
		* Start setting enforcement session 
        */		
		void StartEnforcementSessionL( TRequestStatus& aRequestStatus, const TCertInfo& aCertInfo);
		
	    /**
		* Check if changed element is a element, which affect to setting enforcements 
		* aElement Pointer to modified element. Function doesn't reserve element, only get element name.
        */		

		void AddModifiedElementL( const CElementBase* aElement);

	    /**
		* Execute setting enforcement for all elements added to session
        */		
		void ExecuteEnforcementSessionL();

	    /**
		* Commit changes
        */		
		void CommitChanges();

	    /**
		* Finish execution session
        */		
		void EndEnforcementSession( TBool aFlushSettings);
		
	protected:
		//Derived from CActive
		void RunL();
		TInt RunError( TInt aError);
		void DoCancel();
		
		//certificate
		TCertInfo& SessionCertificate();
	private:
		void ResolveValidEditorsForSettingL( TUint32 iElementId);
		void SetActiveEnforcementBaseL( const TDesC8& aEnforcementId);
	private:
		RPointerArray<HBufC8> iEnforcementElements;
		RAccessControlList iValidEditorsForSetting;
		CEnforcementBase* iActiveEnforcementBase;
		
		//certificate info
		TCertInfo iCertInfo;
		
		//active state variables
		HBufC8 * iActivelementName;
		TInt iOperationCounter;
		TInt iOperationState;
		TRequestStatus* iExternalRequestStatus;
};

class CEnforcementBase : public CBase
{
	public:
		virtual ~CEnforcementBase() {}
	
		virtual void InitEnforcementL( TRequestStatus& aRequestStatus) = 0;
		virtual void DoEnforcementL( TRequestStatus& aRequestStatus) = 0;
		virtual TBool InitReady() = 0;
		virtual TBool EnforcementReady() = 0;
		virtual void FinishEnforcementL( TBool aFlushSettings) = 0;
		virtual void ResetEnforcementL() = 0;
				
		virtual void AccessRightList( RAccessControlList& aAccessControlList) = 0;		
		
		void SetSessionCertificate( TCertInfo& aCertInfo);
	protected:
		TCertInfo& SessionCertificate();	
	
	private:
		TCertInfo* iCertInfo;
};


#endif