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


// INCLUDE FILES

#include "dcmoenforcement.h"
#include "XACMLconstants.h"
#include "debug.h"
#include "PolicyEngineServer.h"
#include "PolicyStorage.h"

// EXTERNAL DATA STRUCTURES
// EXTERNAL FUNCTION PROTOTYPES
// CONSTANTS

_LIT8( KDCMOURI, "DCMO");

// MACROS
// LOCAL CONSTANTS AND MACROS
// MODULE DATA STRUCTURES
// LOCAL FUNCTION PROTOTYPES


// ==================== LOCAL FUNCTIONS ====================

// ================= MEMBER FUNCTIONS =======================

// -----------------------------------------------------------------------------
// CDcmoEnforcement::CDcmoEnforcement()
// -----------------------------------------------------------------------------
//

CDcmoEnforcement::CDcmoEnforcement()
{
RDEBUG("CDcmoEnforcement::CDcmoEnforcement()");
}


// -----------------------------------------------------------------------------
// CDcmoEnforcement::~CDcmoEnforcement()
// -----------------------------------------------------------------------------
//

CDcmoEnforcement::~CDcmoEnforcement()
{
	RDEBUG("CDcmoEnforcement::~CDcmoEnforcement()");
	iDMUtil.Close();
}

// -----------------------------------------------------------------------------
// CDcmoEnforcement::NewL()
// -----------------------------------------------------------------------------
//
		
CDcmoEnforcement* CDcmoEnforcement::NewL( const TDesC8& aEnforcementId )
{
	CDcmoEnforcement* self = 0;

	if ( aEnforcementId == PolicyLanguage::Constants::EDCMOEnforcementPolicy )
	{
		RDEBUG("		-> New DCMOEnforcementPolicy");
		self = new (ELeave) CDcmoEnforcement();
		self->iSettingType = EDCMOEnforcement;	
	}
		
	return self;
}

// -----------------------------------------------------------------------------
// CDcmoEnforcement::ValidEnforcementElement()
// -----------------------------------------------------------------------------
//
TBool CDcmoEnforcement::ValidEnforcementElement( const TDesC8& aEnforcementId)
{
	RDEBUG("CDcmoEnforcement::ValidEnforcementElement");
	if ( aEnforcementId == PolicyLanguage::Constants::EDCMOEnforcementPolicy )
	{
		return ETrue;
	} 
	
	return EFalse;
}

		

	
// -----------------------------------------------------------------------------
// CDcmoEnforcement::InitEnforcement()
// -----------------------------------------------------------------------------
//

void CDcmoEnforcement::InitEnforcementL( TRequestStatus& aRequestStatus)
{
	RDEBUG("CDcmoEnforcement::InitEnforcementL");
	//connect to server
	User::LeaveIfError( iDMUtil.Connect());
	
	iInitState++;

	//complete request	
	TRequestStatus * status = &aRequestStatus;
	User::RequestComplete( status, KErrNone);	
}

// -----------------------------------------------------------------------------
// CDcmoEnforcement::InitReady()
// -----------------------------------------------------------------------------
//

TBool CDcmoEnforcement::InitReady()
{
	return iInitState;
}


// -----------------------------------------------------------------------------
// CDcmoEnforcement::DoEnforcementL()
// -----------------------------------------------------------------------------
//

void CDcmoEnforcement::DoEnforcementL( TRequestStatus& aRequestStatus)
{
	RDEBUG("CDcmoEnforcement::DoEnforcementL()");

	if ( iAccessControlList->Count())
	{
		//ACL...
		User::LeaveIfError( iDMUtil.SetMngSessionCertificate( SessionCertificate()));
		User::LeaveIfError( iDMUtil.AddACLForNode( KDCMOURI, EForChildrens, EACLDelete));
		User::LeaveIfError( iDMUtil.AddACLForNode( KDCMOURI, EForNode, EACLGet ) );
		User::LeaveIfError( iDMUtil.SetACLForNode( KDCMOURI, EForNode, EACLAdd));
		User::LeaveIfError( iDMUtil.SetACLForNode( KDCMOURI, EForNode, EACLExec));

		CPolicyStorage::PolicyStorage()->ActivateEnforcementFlagL( iSettingType);	
	}
	else
	{
		CPolicyStorage::PolicyStorage()->DeactivateEnforcementFlagL( iSettingType);
		
		//ACL...
		User::LeaveIfError( iDMUtil.RemoveACL( KDCMOURI, ETrue));
		
	}
	
	iEnforcementState++;
	TRequestStatus * status = &aRequestStatus;
	User::RequestComplete( status, KErrNone);
}

// -----------------------------------------------------------------------------
// CDcmoEnforcement::EnforcementReady()
// -----------------------------------------------------------------------------
//
TBool CDcmoEnforcement::EnforcementReady()
{
	return iEnforcementState > 0;
}
	
// -----------------------------------------------------------------------------
// CDcmoEnforcement::FinishEnforcementL()
// -----------------------------------------------------------------------------
//
void CDcmoEnforcement::FinishEnforcementL( TBool aFlushSettings)
{
	RDEBUG("CDcmoEnforcement::FinishEnforcementL");
	//Close sessions
	if (aFlushSettings)
	{
		iDMUtil.Flush();		
	}
	
	iDMUtil.Close();
}
				
// -----------------------------------------------------------------------------
// CDcmoEnforcement::AccessRightList()
// -----------------------------------------------------------------------------
//
void CDcmoEnforcement::AccessRightList( RAccessControlList& aAccessControlList)
{
	iAccessControlList = &aAccessControlList;
}

// -----------------------------------------------------------------------------
// CDcmoEnforcement::ResetEnforcementL()
// -----------------------------------------------------------------------------
//
void CDcmoEnforcement::ResetEnforcementL()
{
	RDEBUG("CDcmoEnforcement::ResetEnforcementL()");
	CPolicyStorage::PolicyStorage()->DeactivateEnforcementFlagL( EDCMOEnforcement);

	User::LeaveIfError( iDMUtil.Connect());
	CleanupClosePushL( iDMUtil);

	//ACL...
	User::LeaveIfError( iDMUtil.RemoveACL( KDCMOURI, ETrue ) );
	
	CleanupStack::PopAndDestroy( &iDMUtil);	
}
