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

#include "EMailEnforcement.h"
#include "XACMLconstants.h"
#include "PolicyEngineServer.h"
#include "PolicyStorage.h"
#include "debug.h"

// EXTERNAL DATA STRUCTURES
// EXTERNAL FUNCTION PROTOTYPES
// CONSTANTS

_LIT( KEMailEnformentPanic, "EMail enforcement panic");

//Repository UIDs

const TUid TPOPEmailRepository = { 0x10001029 };
const TUid TSMTPEmailRepository = { 0x10001028 };
const TUid TIMAPEmailRepository = { 0x1000102A };

_LIT8( KEMailURI, "Email");

// MACROS
// LOCAL CONSTANTS AND MACROS
// MODULE DATA STRUCTURES
// LOCAL FUNCTION PROTOTYPES


// ==================== LOCAL FUNCTIONS ====================

// ================= MEMBER FUNCTIONS =======================

// -----------------------------------------------------------------------------
// CEmailEnforcement::CEmailEnforcement()
// -----------------------------------------------------------------------------
//

CEmailEnforcement::CEmailEnforcement()
{
}


// -----------------------------------------------------------------------------
// CEmailEnforcement::~CEmailEnforcement()
// -----------------------------------------------------------------------------
//

CEmailEnforcement::~CEmailEnforcement()
{
	RDEBUG("CEmailEnforcement::~CEmailEnforcement()");
	iEMailPOPEnforcement.Close();
	iEMailSMTPEnforcement.Close();
	iEMailIMAPEnforcement.Close();

	iCentRepServer.Close();
	iDMUtil.Close();
	
}


// -----------------------------------------------------------------------------
// CEmailEnforcement::NewL()
// -----------------------------------------------------------------------------
//
		
CEmailEnforcement* CEmailEnforcement::NewL( const TDesC8& aEnforcementId)
{
	if ( ValidEnforcementElement( aEnforcementId))
	{
		return new (ELeave) CEmailEnforcement();	
	}
	
	return 0;
}

// -----------------------------------------------------------------------------
// CEmailEnforcement::InitEnforcement()
// -----------------------------------------------------------------------------
//

TBool CEmailEnforcement::ValidEnforcementElement( const TDesC8& aEnforcementId)
{
	if ( aEnforcementId == PolicyLanguage::Constants::EMailEnforcementPolicy )
	{
		return ETrue;
	}
	
	return EFalse;
}

	
// -----------------------------------------------------------------------------
// CEmailEnforcement::InitEnforcement()
// -----------------------------------------------------------------------------
//

void CEmailEnforcement::InitEnforcementL( TRequestStatus& aRequestStatus)
{
	RDEBUG("CEmailEnforcement::InitEnforcementL");
	//in first phase open connections to centreptool
	if ( iInitState == 0)
	{
		User::LeaveIfError( iCentRepServer.Connect());
		User::LeaveIfError( iDMUtil.Connect());
	
		User::LeaveIfError( iEMailPOPEnforcement.Open( TPOPEmailRepository , iCentRepServer));
		User::LeaveIfError( iEMailSMTPEnforcement.Open( TSMTPEmailRepository , iCentRepServer));
		User::LeaveIfError( iEMailIMAPEnforcement.Open( TIMAPEmailRepository , iCentRepServer));
	}
	
	//init each session in own cycle....
	switch ( iInitState )
	{
		case 0:
			iEMailPOPEnforcement.InitSession( aRequestStatus);
		break;
		case 1:
			iEMailSMTPEnforcement.InitSession( aRequestStatus);	
		break;
		case 2:
			iEMailIMAPEnforcement.InitSession( aRequestStatus);
		break;
		default:
			User::Panic( KEMailEnformentPanic, KErrAbort);
		break;
	}
	
	iInitState++;
}

// -----------------------------------------------------------------------------
// CEmailEnforcement::InitReady()
// -----------------------------------------------------------------------------
//

TBool CEmailEnforcement::InitReady()
{
	return (iInitState > 2);
}


// -----------------------------------------------------------------------------
// CEmailEnforcement::DoEnforcementL()
// -----------------------------------------------------------------------------
//

void CEmailEnforcement::DoEnforcementL( TRequestStatus& aRequestStatus)
{
	RDEBUG("CEmailEnforcement::DoEnforcementL");
	if ( iAccessControlList->Count())
	{
		//if there any number subject which have exclusively right for setting, give access only for DM client
		User::LeaveIfError( iEMailPOPEnforcement.AddSIDWRForDefaults( KDMClientUiD));
		User::LeaveIfError( iEMailSMTPEnforcement.AddSIDWRForDefaults( KDMClientUiD));
		User::LeaveIfError( iEMailIMAPEnforcement.AddSIDWRForDefaults( KDMClientUiD));
		User::LeaveIfError( iEMailPOPEnforcement.RemoveBackupFlagForDefaults());
		User::LeaveIfError( iEMailSMTPEnforcement.RemoveBackupFlagForDefaults());
		User::LeaveIfError( iEMailIMAPEnforcement.RemoveBackupFlagForDefaults());
		
		//ACL...
		User::LeaveIfError( iDMUtil.SetMngSessionCertificate( SessionCertificate()));
		User::LeaveIfError( iDMUtil.AddACLForNode( KEMailURI, EForChildrens, EACLDelete));
		User::LeaveIfError( iDMUtil.AddACLForNode( KEMailURI, EForNode, EACLGet ) );
		User::LeaveIfError( iDMUtil.SetACLForNode( KEMailURI, EForNode, EACLAdd));
		        
		//centrep flag
		CPolicyStorage::PolicyStorage()->ActivateEnforcementFlagL( EEMailEnforcement);	
	}
	else
	{
		//Clear default settings
		User::LeaveIfError( iEMailPOPEnforcement.RestoreDefaults());
		User::LeaveIfError( iEMailSMTPEnforcement.RestoreDefaults());
		User::LeaveIfError( iEMailIMAPEnforcement.RestoreDefaults());
		User::LeaveIfError( iEMailPOPEnforcement.RestoreBackupFlagForDefaults());
		User::LeaveIfError( iEMailSMTPEnforcement.RestoreBackupFlagForDefaults());
		User::LeaveIfError( iEMailIMAPEnforcement.RestoreBackupFlagForDefaults());

		//ACL...
		User::LeaveIfError( iDMUtil.RemoveACL( KEMailURI, ETrue));
		
		//centrep flag
		CPolicyStorage::PolicyStorage()->DeactivateEnforcementFlagL( EEMailEnforcement);	
	}
	
	iEnforcementState++;
	TRequestStatus * status = &aRequestStatus;
	User::RequestComplete( status, KErrNone);
		

}

// -----------------------------------------------------------------------------
// CEmailEnforcement::EnforcementReady()
// -----------------------------------------------------------------------------
//
TBool CEmailEnforcement::EnforcementReady()
{
	return iEnforcementState > 0;
}
	
// -----------------------------------------------------------------------------
// CEmailEnforcement::FinishEnforcementL()
// -----------------------------------------------------------------------------
//
void CEmailEnforcement::FinishEnforcementL( TBool aFlushSettings)
{
	RDEBUG("CEmailEnforcement::FinishEnforcementL");
	//Close sessions
	if ( aFlushSettings)
	{
		iEMailPOPEnforcement.Flush();
	 	iEMailSMTPEnforcement.Flush();
		iEMailIMAPEnforcement.Flush();
		
		iDMUtil.Flush();
	}
	
	iEMailPOPEnforcement.Close();
	iEMailSMTPEnforcement.Close();
	iEMailIMAPEnforcement.Close();
	
	iDMUtil.Close();

	//Close centrep server...
	iCentRepServer.Close();
}
				
// -----------------------------------------------------------------------------
// CEmailEnforcement::AccessRightList()
// -----------------------------------------------------------------------------
//
void CEmailEnforcement::AccessRightList( RAccessControlList& aAccessControlList)
{
	iAccessControlList = &aAccessControlList;
}

// -----------------------------------------------------------------------------
// CEmailEnforcement::ResetEnforcementL()
// -----------------------------------------------------------------------------
//
void CEmailEnforcement::ResetEnforcementL()
{
	RDEBUG("CEmailEnforcement::ResetEnforcementL()");
	User::LeaveIfError( iCentRepServer.Connect());
	User::LeaveIfError( iDMUtil.Connect());
	
	User::LeaveIfError( iEMailPOPEnforcement.Open( TPOPEmailRepository , iCentRepServer));
	User::LeaveIfError( iEMailSMTPEnforcement.Open( TSMTPEmailRepository , iCentRepServer));
	User::LeaveIfError( iEMailIMAPEnforcement.Open( TIMAPEmailRepository , iCentRepServer));


	User::LeaveIfError( iEMailPOPEnforcement.RestoreDefaults());
	User::LeaveIfError( iEMailSMTPEnforcement.RestoreDefaults());
	User::LeaveIfError( iEMailIMAPEnforcement.RestoreDefaults());
	User::LeaveIfError( iEMailPOPEnforcement.RestoreBackupFlagForDefaults());
	User::LeaveIfError( iEMailSMTPEnforcement.RestoreBackupFlagForDefaults());
	User::LeaveIfError( iEMailIMAPEnforcement.RestoreBackupFlagForDefaults());

	//ACL...
	User::LeaveIfError( iDMUtil.RemoveACL( KEMailURI, ETrue));
		
	//centrep flag
	CPolicyStorage::PolicyStorage()->DeactivateEnforcementFlagL( EEMailEnforcement);
	
	iEMailPOPEnforcement.Close();
	iEMailSMTPEnforcement.Close();
	iEMailIMAPEnforcement.Close();
	
	iDMUtil.Close();
}

