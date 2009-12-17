/*
* Copyright (c) 2002-2004 Nokia Corporation and/or its subsidiary(-ies). 
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


// INCLUDE FILES

#include "datasyncmlenforcement.h"
#include "XACMLconstants.h"
#include "debug.h"
#include "PolicyEngineServer.h"
#include "PolicyStorage.h"
#include <featmgr.h>

// EXTERNAL DATA STRUCTURES
// EXTERNAL FUNCTION PROTOTYPES
// CONSTANTS

//_LIT( KCommsDatEnformentPanic, "CommsDat enforcement panic");

_LIT8( KSyncMLURI, "SyncML/DMAcc");
_LIT8( KSyncML12URI, "DMAcc");
_LIT8( KDataSyncURI, "SyncML/DSAcc");

// MACROS
// LOCAL CONSTANTS AND MACROS
// MODULE DATA STRUCTURES
// LOCAL FUNCTION PROTOTYPES


// ==================== LOCAL FUNCTIONS ====================

// ================= MEMBER FUNCTIONS =======================

// -----------------------------------------------------------------------------
// CCommsDatEnforcement::CEmailEnforcement()
// -----------------------------------------------------------------------------
//

CDataSyncMLEnforcement::CDataSyncMLEnforcement()
{
RDEBUG("CDataSyncMLEnforcement::CDataSyncMLEnforcement()");
}


// -----------------------------------------------------------------------------
// CDataSyncMLEnforcement::~CDataSyncMLEnforcement()
// -----------------------------------------------------------------------------
//

CDataSyncMLEnforcement::~CDataSyncMLEnforcement()
{
	RDEBUG("CDataSyncMLEnforcement::~CDataSyncMLEnforcement()");
	iDMUtil.Close();
}

// -----------------------------------------------------------------------------
// CDataSyncMLEnforcement::NewL()
// -----------------------------------------------------------------------------
//
		
CDataSyncMLEnforcement* CDataSyncMLEnforcement::NewL( const TDesC8& aEnforcementId )
{
	CDataSyncMLEnforcement* self = 0;

	if ( aEnforcementId == PolicyLanguage::Constants::EDataSyncEnforcementPolicy )
	{
		RDEBUG("		-> New DataSyncEnforcementPolicy");
		self = new (ELeave) CDataSyncMLEnforcement();	
		self->iSettingType = EDataSyncEnforcement;
	}
	else if ( aEnforcementId == PolicyLanguage::Constants::ESyncMLEnforcementPolicy )
	{
		RDEBUG("		-> New SyncMLEnforcementPolicy");
		self = new (ELeave) CDataSyncMLEnforcement();	
		self->iSettingType = ESyncMLEnforcement;
	}
		
	return self;
}

// -----------------------------------------------------------------------------
// CDataSyncMLEnforcement::ValidEnforcementElement()
// -----------------------------------------------------------------------------
//
TBool CDataSyncMLEnforcement::ValidEnforcementElement( const TDesC8& aEnforcementId)
{
	if ( aEnforcementId == PolicyLanguage::Constants::EDataSyncEnforcementPolicy )
	{
		return ETrue;
	} else if ( aEnforcementId == PolicyLanguage::Constants::ESyncMLEnforcementPolicy )
	{
		return ETrue;	
	}
	
	return EFalse;
}

		

	
// -----------------------------------------------------------------------------
// CDataSyncMLEnforcement::InitEnforcement()
// -----------------------------------------------------------------------------
//

void CDataSyncMLEnforcement::InitEnforcementL( TRequestStatus& aRequestStatus)
{
	RDEBUG("CDataSyncMLEnforcement::InitEnforcementL");
	//connect to server
	User::LeaveIfError( iDMUtil.Connect());
	
	iInitState++;

	//complete request	
	TRequestStatus * status = &aRequestStatus;
	User::RequestComplete( status, KErrNone);	
}

// -----------------------------------------------------------------------------
// CDataSyncMLEnforcement::InitReady()
// -----------------------------------------------------------------------------
//

TBool CDataSyncMLEnforcement::InitReady()
{
	return iInitState;
}


// -----------------------------------------------------------------------------
// CDataSyncMLEnforcement::InitReady()
// -----------------------------------------------------------------------------
//

void CDataSyncMLEnforcement::DoEnforcementL( TRequestStatus& aRequestStatus)
{
	RDEBUG("CDataSyncMLEnforcement::DoEnforcementL");
	//Select correct URI
	TPtrC8 uriPtr;
	FeatureManager::InitializeLibL();
	if ( iSettingType == EDataSyncEnforcement)
	{
		uriPtr.Set( KDataSyncURI);
	}
	else
	{
		if(!FeatureManager::FeatureSupported( KFeatureIdSyncMlDm112  ))
		{
		   uriPtr.Set( KSyncML12URI);
		}
		else
		{
			uriPtr.Set( KSyncMLURI);
		}
	
	}
	FeatureManager::UnInitializeLib();
	if ( iAccessControlList->Count())
	{
		//ACL...
		User::LeaveIfError( iDMUtil.SetMngSessionCertificate( SessionCertificate()));
		User::LeaveIfError( iDMUtil.AddACLForNode( uriPtr, EForChildrens, EACLDelete));
		User::LeaveIfError( iDMUtil.AddACLForNode( uriPtr, EForNode, EACLGet ) );
		User::LeaveIfError( iDMUtil.SetACLForNode( uriPtr, EForNode, EACLAdd));

		CPolicyStorage::PolicyStorage()->ActivateEnforcementFlagL( iSettingType);	
	}
	else
	{
		CPolicyStorage::PolicyStorage()->DeactivateEnforcementFlagL( iSettingType);
		
		//ACL...
		User::LeaveIfError( iDMUtil.RemoveACL( uriPtr, ETrue));
		
	}
	
	iEnforcementState++;
	TRequestStatus * status = &aRequestStatus;
	User::RequestComplete( status, KErrNone);
		

}

// -----------------------------------------------------------------------------
// CDataSyncMLEnforcement::EnforcementReady()
// -----------------------------------------------------------------------------
//
TBool CDataSyncMLEnforcement::EnforcementReady()
{
	return iEnforcementState > 0;
}
	
// -----------------------------------------------------------------------------
// CDataSyncMLEnforcement::FinishEnforcementL()
// -----------------------------------------------------------------------------
//
void CDataSyncMLEnforcement::FinishEnforcementL( TBool aFlushSettings )
{
	RDEBUG("CDataSyncMLEnforcement::FinishEnforcementL");
    if( aFlushSettings )
        {
        iDMUtil.Flush();
        }
	iDMUtil.Close();
}
				
// -----------------------------------------------------------------------------
// CDataSyncMLEnforcement::AccessRightList()
// -----------------------------------------------------------------------------
//
void CDataSyncMLEnforcement::AccessRightList( RAccessControlList& aAccessControlList)
{
	iAccessControlList = &aAccessControlList;
}

// -----------------------------------------------------------------------------
// CDataSyncMLEnforcement::AccessRightList()
// -----------------------------------------------------------------------------
//
void CDataSyncMLEnforcement::ResetEnforcementL()
{
	RDEBUG("CDataSyncMLEnforcement::ResetEnforcementL()");
	CPolicyStorage::PolicyStorage()->DeactivateEnforcementFlagL( ESyncMLEnforcement);
	CPolicyStorage::PolicyStorage()->DeactivateEnforcementFlagL( EDataSyncEnforcement);

	User::LeaveIfError( iDMUtil.Connect());
	CleanupClosePushL( iDMUtil);

	//ACL...
	User::LeaveIfError( iDMUtil.RemoveACL( KSyncMLURI, ETrue ) );
	User::LeaveIfError( iDMUtil.RemoveACL( KDataSyncURI, ETrue ) );
	
	CleanupStack::PopAndDestroy( &iDMUtil);	
}

