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


#ifndef _DATA_SYNC_ML_ENFORCEMENT_HEADER__
#define _DATA_SYNC_ML_ENFORCEMENT_HEADER__

// INCLUDES

#include "SettingEnforcementManager.h"

#include "CentRepToolClient.h"
#include <SettingEnforcementInfo.h>
#include "DMUtilClient.h"
#include <e32base.h>


class CDataSyncMLEnforcement : public CEnforcementBase
{
	public:
		//construction
		CDataSyncMLEnforcement();
		~CDataSyncMLEnforcement();
		static CDataSyncMLEnforcement* NewL( const TDesC8& aEnforcementId);
	
		//from MEnforcementBase
		void InitEnforcementL( TRequestStatus& aRequestStatus);
		void DoEnforcementL( TRequestStatus& aRequestStatus);
		TBool InitReady();
		TBool EnforcementReady();
		void FinishEnforcementL( TBool aFlushSettings);
		void ResetEnforcementL();
				
		void AccessRightList( RAccessControlList& aAccessControlList);		
		static TBool ValidEnforcementElement( const TDesC8& aEnforcementId);
	private:
		//active state 
		TInt iInitState;
		TInt iEnforcementState;
		
		//editor list
		RAccessControlList* iAccessControlList;
		
		//ACL editing
		RDMUtil iDMUtil;		
		
		KSettingEnforcements iSettingType;

};


#endif