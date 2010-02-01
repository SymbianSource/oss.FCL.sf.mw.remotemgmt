/*
* Copyright (c) 2009-2010 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description: Implementation of policymanagement component for DCMO enforcement
*
*/


#ifndef __DCMO_ENFORCEMENT_HEADER__
#define __DCMO_ENFORCEMENT_HEADER__

// INCLUDES

#include <e32base.h>
#include <SettingEnforcementInfo.h>
#include "SettingEnforcementManager.h"
#include "CentRepToolClient.h"
#include "DMUtilClient.h"

// -----------------------------------------------------------------------------
// Class CDcmoEnforcement
// -----------------------------------------------------------------------------
//
class CDcmoEnforcement : public CEnforcementBase
{
	public:
		//construction
		CDcmoEnforcement();
		virtual ~CDcmoEnforcement();
		static CDcmoEnforcement* NewL( const TDesC8& aEnforcementId );
	
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