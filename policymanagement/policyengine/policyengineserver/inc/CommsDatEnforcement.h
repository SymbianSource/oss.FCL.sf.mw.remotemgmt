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


#ifndef _COMMSDAT_ENFORCEMENT_HEADER__
#define _COMMSDAT_ENFORCEMENT_HEADER__

// INCLUDES

#include "SettingEnforcementManager.h"

#include "CentRepToolClient.h"
#include "DMUtilClient.h"
#include <e32base.h>
#include <SettingEnforcementInfo.h>
#include <commdb.h>



class CCommsDatEnforcement : public CEnforcementBase
{
	public:
		//construction
		CCommsDatEnforcement();
		virtual ~CCommsDatEnforcement();
		static CCommsDatEnforcement* NewL( const TDesC8& aEnforcementId );
		void ConstructL();
		
		//from MEnforcementBase
		void InitEnforcementL( TRequestStatus& aRequestStatus);
		void DoEnforcementL( TRequestStatus& aRequestStatus);
		TBool InitReady();
		TBool EnforcementReady();
		void FinishEnforcementL( TBool aFlushSettings);
		void ResetEnforcementL();

				
		void AccessRightList( RAccessControlList& aAccessControlList);		
		
		static TBool ValidEnforcementElement( const TDesC8& aEnforcementId);
		
		void LockWLANAccessPointsL( TBool aLockValue );
		
		TInt CheckTableL( const TDesC& aTableName );
		
		
		TUint32 CCommsDatEnforcement::GetRecordIdL( const TDesC& aTableName );
		
	private:
		//active state 
		TInt iInitState;
		TInt iEnforcementState;
		TBool iRestore;
		
		//mask value for setting
		RArray<TUint32> iMaskList;
		RArray<TPtrC> iTableList;
		
		//editor list
		RAccessControlList* iAccessControlList;
		
		//CentRep tool 
		RCentRepTool iCentRepServer;
		
		RSettingManagement iCommsDatEnforcement;
		//ACL editing
		RDMUtil iDMUtil;
				
		KSettingEnforcements iSettingType;
		
		//@var session - Owned
        CMDBSession* iSession;
};


#endif