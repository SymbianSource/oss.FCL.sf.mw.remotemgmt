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

#include "IMEnforcement.h"
#include "XACMLconstants.h"
#include "debug.h"
#include "PolicyEngineServer.h"
#include "PolicyStorage.h"

// EXTERNAL DATA STRUCTURES
// EXTERNAL FUNCTION PROTOTYPES
// CONSTANTS

_LIT( KImEnformentPanic, "IM enforcement panic");


//Repository UIDs

const TUid TIMRepository = { 0x101F884C };
const TUint32 TMIMBaseMask = 0xFF000000;
const TUint32 KSAPSettingsIMBaseId = 0x31; // IM SAP settings base id
const TUint32 KBaseOffset = 26; //Base offset

_LIT8( KIMPSURI, "IMPS");

// MACROS
// LOCAL CONSTANTS AND MACROS
// MODULE DATA STRUCTURES
// LOCAL FUNCTION PROTOTYPES


// ==================== LOCAL FUNCTIONS ====================

// ================= MEMBER FUNCTIONS =======================

// -----------------------------------------------------------------------------
// CImEnforcement::CEmailEnforcement()
// -----------------------------------------------------------------------------
//

CImEnforcement::CImEnforcement()
{
}


// -----------------------------------------------------------------------------
// CImEnforcement::~CEmailEnforcement()
// -----------------------------------------------------------------------------
//

CImEnforcement::~CImEnforcement()
{
	RDEBUG("CImEnforcement::~CImEnforcement()");
	iIMEnforcement.Close();

	//Close centrep server...
	iCentRepServer.Close();
	
	iDMUtil.Close();
}

// -----------------------------------------------------------------------------
// CImEnforcement::NewL()
// -----------------------------------------------------------------------------
//
		
CImEnforcement* CImEnforcement::NewL( const TDesC8& aEnforcementId )
{
	CImEnforcement* self = 0;

	if ( aEnforcementId == PolicyLanguage::Constants::EIMEnforcementPolicy )
	{
		self = new (ELeave) CImEnforcement();	
	}
		
	return self;
}

// -----------------------------------------------------------------------------
// CImEnforcement::ValidEnforcementElement()
// -----------------------------------------------------------------------------
//
TBool CImEnforcement::ValidEnforcementElement( const TDesC8& aEnforcementId)
{
	if ( aEnforcementId == PolicyLanguage::Constants::EIMEnforcementPolicy )
	{
		return ETrue;
	} 
	
	return EFalse;
}

		

	
// -----------------------------------------------------------------------------
// CImEnforcement::InitEnforcement()
// -----------------------------------------------------------------------------
//

void CImEnforcement::InitEnforcementL( TRequestStatus& aRequestStatus)
{
	RDEBUG("CImEnforcement::InitEnforcementL");
	//in first phase open connections to centreptool
	if ( iInitState == 0)
	{
		User::LeaveIfError( iCentRepServer.Connect());
		User::LeaveIfError( iIMEnforcement.Open( TIMRepository , iCentRepServer));
		User::LeaveIfError( iDMUtil.Connect());
	}
	
	//init each session in own cycle....
	switch ( iInitState )
	{
		case 0:
			iIMEnforcement.InitSession( aRequestStatus);
		break;
		default:
			User::Panic( KImEnformentPanic, KErrAbort);
		break;
	}
	
	iInitState++;
}

// -----------------------------------------------------------------------------
// CImEnforcement::InitReady()
// -----------------------------------------------------------------------------
//

TBool CImEnforcement::InitReady()
{
	return (iInitState > 0);
}


// -----------------------------------------------------------------------------
// CImEnforcement::InitReady()
// -----------------------------------------------------------------------------
//

void CImEnforcement::DoEnforcementL( TRequestStatus& aRequestStatus)
{
	RDEBUG("CImEnforcement::DoEnforcementL()");
    TUint32 maskBase( 0);
   	maskBase += KSAPSettingsIMBaseId << KBaseOffset;

	if ( iAccessControlList->Count())
	{
		//if there any number subject which have exclusively right for setting, give access only for DM client

		User::LeaveIfError( iIMEnforcement.SetSIDWRForMask( maskBase, TMIMBaseMask, KDMClientUiD));
		User::LeaveIfError( iIMEnforcement.RemoveBackupFlagForMask( maskBase, TMIMBaseMask));
		CPolicyStorage::PolicyStorage()->ActivateEnforcementFlagL( EIMEnforcement);	
	
		//ACL...
		User::LeaveIfError( iDMUtil.SetMngSessionCertificate( SessionCertificate()));
		User::LeaveIfError( iDMUtil.AddACLForNode( KIMPSURI, EForChildrens, EACLDelete));
		User::LeaveIfError( iDMUtil.AddACLForNode( KIMPSURI, EForNode, EACLGet ) );
		User::LeaveIfError( iDMUtil.SetACLForNode( KIMPSURI, EForNode, EACLAdd));
	
	}
	else
	{
		//Clear default settings
		User::LeaveIfError( iIMEnforcement.RestoreMask( maskBase, TMIMBaseMask));
		User::LeaveIfError( iIMEnforcement.RestoreBackupFlagForMask( maskBase, TMIMBaseMask));
		CPolicyStorage::PolicyStorage()->DeactivateEnforcementFlagL( EIMEnforcement);


		//ACL...
		User::LeaveIfError( iDMUtil.RemoveACL( KIMPSURI, ETrue));
	}
	
	iEnforcementState++;
	TRequestStatus * status = &aRequestStatus;
	User::RequestComplete( status, KErrNone);
		
}

// -----------------------------------------------------------------------------
// CImEnforcement::EnforcementReady()
// -----------------------------------------------------------------------------
//
TBool CImEnforcement::EnforcementReady()
{
	return iEnforcementState > 0;
}
	
// -----------------------------------------------------------------------------
// CImEnforcement::FinishEnforcementL()
// -----------------------------------------------------------------------------
//
void CImEnforcement::FinishEnforcementL( TBool aFlushSettings)
{
	RDEBUG("CImEnforcement::FinishEnforcementL");
	//Close sessions
	if (aFlushSettings)
	{
		iIMEnforcement.Flush();
		iDMUtil.Flush();		
	}
	
	iDMUtil.Close();
	iIMEnforcement.Close();

	//Close centrep server...
	iCentRepServer.Close();
}
				
// -----------------------------------------------------------------------------
// CImEnforcement::AccessRightList()
// -----------------------------------------------------------------------------
//
void CImEnforcement::AccessRightList( RAccessControlList& aAccessControlList)
{
	iAccessControlList = &aAccessControlList;
}

// -----------------------------------------------------------------------------
// CImEnforcement::AccessRightList()
// -----------------------------------------------------------------------------
//
void CImEnforcement::ResetEnforcementL()
{
	RDEBUG("CImEnforcement::ResetEnforcementL()");
	User::LeaveIfError( iCentRepServer.Connect());
	User::LeaveIfError( iIMEnforcement.Open( TIMRepository , iCentRepServer));
	User::LeaveIfError( iDMUtil.Connect());

    TUint32 maskBase( 0);
   	maskBase += KSAPSettingsIMBaseId << KBaseOffset;

	//Clear default settings
	User::LeaveIfError( iIMEnforcement.RestoreMask( maskBase, TMIMBaseMask));
	User::LeaveIfError( iIMEnforcement.RestoreBackupFlagForMask( maskBase, TMIMBaseMask));
	CPolicyStorage::PolicyStorage()->DeactivateEnforcementFlagL( EIMEnforcement);

	//ACL...
	User::LeaveIfError( iDMUtil.RemoveACL( KIMPSURI, ETrue));

	
	iIMEnforcement.Close();

	//Close centrep server...
	iCentRepServer.Close();
	iDMUtil.Close();
}
