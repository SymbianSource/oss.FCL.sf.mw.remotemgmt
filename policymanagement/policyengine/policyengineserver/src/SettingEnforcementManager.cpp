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

#include "SettingEnforcementManager.h"
#include "PolicyEngineServer.h"
#include "EMailEnforcement.h"
#include "CommsDatEnforcement.h"
#include "IMEnforcement.h"
#include "dcmoenforcement.h"
#include "datasyncmlenforcement.h"
#include "ElementBase.h"
#include "PolicyStorage.h"
#include "elements.h"
#include "DataTypes.h"

#include "XACMLconstants.h"
#include "ErrorCodes.h"
#include "debug.h"

// EXTERNAL DATA STRUCTURES
// EXTERNAL FUNCTION PROTOTYPES
// CONSTANTS
// MACROS
// LOCAL CONSTANTS AND MACROS
// MODULE DATA STRUCTURES
// LOCAL FUNCTION PROTOTYPES


// ==================== LOCAL FUNCTIONS ====================

// ================= MEMBER FUNCTIONS =======================

// -----------------------------------------------------------------------------
// CSettingEnforcementManager::CSettingEnforcementManager()
// -----------------------------------------------------------------------------
//

CSettingEnforcementManager::CSettingEnforcementManager()
	: CActive( EPriorityLow)
{
}

// -----------------------------------------------------------------------------
// CSettingEnforcementManager::~CSettingEnforcementManager()
// -----------------------------------------------------------------------------
//
CSettingEnforcementManager::~CSettingEnforcementManager()
{
	iEnforcementElements.ResetAndDestroy();
	iEnforcementElements.Close();

	iValidEditorsForSetting.ResetAndDestroy();
	iValidEditorsForSetting.Close();
	
	delete iActiveEnforcementBase;
}

// -----------------------------------------------------------------------------
// CSettingEnforcementManager::NewL()
// -----------------------------------------------------------------------------
//
CSettingEnforcementManager* CSettingEnforcementManager::NewL()
{
	return new (ELeave) CSettingEnforcementManager();
}

// -----------------------------------------------------------------------------
// CSettingEnforcementManager::StartEnforcementSessionL()
// -----------------------------------------------------------------------------
//		
void CSettingEnforcementManager::StartEnforcementSessionL( TRequestStatus& aRequestStatus, const TCertInfo& aCertInfo)
{
	//reset context
	iEnforcementElements.ResetAndDestroy();
	iValidEditorsForSetting.ResetAndDestroy();

	iExternalRequestStatus = &aRequestStatus;
	aRequestStatus = KRequestPending;
	
	iCertInfo = aCertInfo;
	
	//Add object to active scheduler
	if ( !IsAdded())
	{
		CActiveScheduler::Add( this);
	}
}

// -----------------------------------------------------------------------------
// CSettingEnforcementManager::AddModifiedElementL()
// -----------------------------------------------------------------------------
//
void CSettingEnforcementManager::AddModifiedElementL( const CElementBase* aElement)
{
	using namespace PolicyLanguage::Constants;

	__ASSERT_ALWAYS( aElement, User::Panic( Panics::SettingEnforcementManagerPanic, KErrAbort));

	HBufC8 * elementName = aElement->ExternalId();
	
	//If element has external id
	if ( elementName)
	{
		//and if element is enforcement element
		if ( CEmailEnforcement::ValidEnforcementElement( *elementName) ||
			 CCommsDatEnforcement::ValidEnforcementElement( *elementName) ||
			 CDataSyncMLEnforcement::ValidEnforcementElement( *elementName) ||
			 CImEnforcement::ValidEnforcementElement(*elementName)	||
			 CDcmoEnforcement::ValidEnforcementElement(*elementName))	
		{
			iEnforcementElements.AppendL( elementName->Alloc());
		}
	}
}

// -----------------------------------------------------------------------------
// CSettingEnforcementManager::ExecuteEnforcementSessionL()
// -----------------------------------------------------------------------------
//
void CSettingEnforcementManager::ExecuteEnforcementSessionL()
{
	//Reset operation state
	iOperationCounter = 0;
	iOperationState = 0;

	//Start scheduling
	SetActive();
	TRequestStatus * status = &iStatus;
	User::RequestComplete( status, KErrNone);
}

// -----------------------------------------------------------------------------
// CSettingEnforcementManager::CommitChanges()
// -----------------------------------------------------------------------------
//
void CSettingEnforcementManager::CommitChanges()
{
	//Commit changes....
}

// -----------------------------------------------------------------------------
// CSettingEnforcementManager::EndEnforcementSessionL()
// -----------------------------------------------------------------------------
//
void CSettingEnforcementManager::EndEnforcementSession( TBool aFlushSettings)
{
	//finish active enforcement base
	if ( iActiveEnforcementBase)
	{
		TRAPD(err, iActiveEnforcementBase->FinishEnforcementL( aFlushSettings));
		
		if (err != KErrNone)
			{
			RDEBUG_2("	**** ERROR, iActiveEnforcementBase->FinishEnforcementL left with %d", err );
			}
			
		delete iActiveEnforcementBase;
		iActiveEnforcementBase = 0;
	}

	//destroy editor list (this must be valid until FinishEnforcementL is called for enforcementbase)
	iValidEditorsForSetting.ResetAndDestroy();
}
	
// -----------------------------------------------------------------------------
// CSettingEnforcementManager::RunL()
// -----------------------------------------------------------------------------
//
void CSettingEnforcementManager::RunL()
{
	using namespace PolicyLanguage::Constants;

	if ( iStatus.Int() != KErrNone)
	{
		User::RequestComplete( iExternalRequestStatus, iStatus.Int());
		return;
	}


	if ( iOperationCounter < iEnforcementElements.Count())
	{
		TBool requested = ETrue;
		TInt requestStatus = KErrNone;

		switch ( iOperationState )
		{
			case 0:
			if ( iOperationState == 0)
			{
				//reset arrays and get new editors for setting
				iValidEditorsForSetting.ResetAndDestroy();

				iActivelementName = iEnforcementElements[ iOperationCounter];
				TUint32 elementId = CPolicyStorage::PolicyStorage()->MapRealIdL( *iActivelementName, EFalse );
			
				if ( elementId != 0)
				{
					ResolveValidEditorsForSettingL( elementId);
				}
			
				requested = EFalse;
				iOperationState++;
	
				//select active enforment base
				__ASSERT_ALWAYS( iActivelementName, User::Panic( Panics::SettingEnforcementManagerPanic, KErrAbort));
				SetActiveEnforcementBaseL( *iActivelementName);
			}
			break;
			case 1:
			if ( iOperationState == 1)
			{
				if ( iActiveEnforcementBase->InitReady())
				{
					requested = EFalse;
					iOperationState++;
				}
				else
				{
					iActiveEnforcementBase->InitEnforcementL( iStatus);
				}
			}
			break;
			case 2:
			if ( iOperationState == 2)
			{
				if ( iActiveEnforcementBase->EnforcementReady())
				{
					//when enforcement is ready...
					requested = EFalse;
					iOperationState = 0;
					iOperationCounter++;
				}
				else
				{
					//and the next rounds are dedicated for doing enforcement until enforcement is ready
					iActiveEnforcementBase->DoEnforcementL( iStatus);
				}
			}
			break;
		}

		//Continue scheduling...
		SetActive();
		if ( !requested )
		{
			TRequestStatus * status = &iStatus;
			User::RequestComplete( status, requestStatus);
		}
	}
	else
	{
		User::RequestComplete( iExternalRequestStatus, KErrNone);
	}
}

// -----------------------------------------------------------------------------
// CSettingEnforcementManager::RunError()
// -----------------------------------------------------------------------------
//
TInt CSettingEnforcementManager::RunError( TInt aError)
{
	RDEBUG_2("	**** ERROR, CSettingEnforcementManager::RunError( %d )", aError );
	//Complete external request with error value
	User::RequestComplete( iExternalRequestStatus, aError);
	
	return KErrNone;
}

// -----------------------------------------------------------------------------
// CSettingEnforcementManager::DoCancel()
// -----------------------------------------------------------------------------
//
void CSettingEnforcementManager::DoCancel()
{
	RunError( KErrAbort);
}


// -----------------------------------------------------------------------------
// CSettingEnforcementManager::SetActiveEnforcementBaseL()
// -----------------------------------------------------------------------------
//

void CSettingEnforcementManager::SetActiveEnforcementBaseL(  const TDesC8& aEnforcementId)
{	
	//delete old enforcement base
	delete iActiveEnforcementBase;
	iActiveEnforcementBase = 0;

	//and if element is enforcement element
	iActiveEnforcementBase = CEmailEnforcement::NewL( aEnforcementId);

		
	if ( !iActiveEnforcementBase)
	{
		iActiveEnforcementBase = CCommsDatEnforcement::NewL( aEnforcementId);
	}

	if ( !iActiveEnforcementBase)
	{
		iActiveEnforcementBase = CDataSyncMLEnforcement::NewL( aEnforcementId);
	}

	if ( !iActiveEnforcementBase)
	{
		iActiveEnforcementBase = CImEnforcement::NewL( aEnforcementId);
	}
	
	if ( !iActiveEnforcementBase)
	{
		iActiveEnforcementBase = CDcmoEnforcement::NewL( aEnforcementId);
	}
	
	if ( iActiveEnforcementBase )
	{
		iActiveEnforcementBase->AccessRightList( iValidEditorsForSetting);
		iActiveEnforcementBase->SetSessionCertificate( iCertInfo );
	}
}




// -----------------------------------------------------------------------------
// CSettingEnforcementManager::ResolveValidEditorsForSettingL()
// -----------------------------------------------------------------------------
//
void CSettingEnforcementManager::ResolveValidEditorsForSettingL( TUint32 iElementId)
{
	using namespace PolicyLanguage::NativeLanguage::Functions;
	using namespace PolicyLanguage::Constants;

	//get and reserve element
	CElementBase * policyElement = CPolicyStorage::PolicyStorage()->GetEditableElementL( iElementId);
	
	if ( policyElement == NULL)
	{
		return;	
	}
	
	if ( policyElement->ElementType() != EPolicy)
	{
		return;
	}
	
	TElementReserver policyReserver( policyElement);
	CPolicyStorage::PolicyStorage()->CheckElementL( policyElement);


	//Find Subjectmatch element, they determines who has right to modifie settings	
	RElementArray rules;	
	policyElement->FindAttributesL( ERule, rules);
	CleanupClosePushL( rules);
	
	for ( TInt j(0); j < rules.Count(); j++)
	{
		CRule* rule = (CRule*)rules[ j];
		TElementReserver ruleReserver( rule);
		CPolicyStorage::PolicyStorage()->CheckElementL( rule);
		
		if ( rule->Effect() == EPermit)
		{
			//Find Subjectmatch element, they determines who has right to modifie settings	
			RElementArray attributes;	
			rule->FindAttributesL( ESubjectMatch, attributes);
			CleanupClosePushL( attributes);
	

			for ( TInt i(0); i < attributes.Count(); i++)
			{
				CElementBase * element = attributes[i];
	
				if ( element->ElementType() == ESubjectMatch )
				{
					TElementReserver elementReserver( attributes[i]);
					CPolicyStorage::PolicyStorage()->CheckElementL( element);
		
					CMatchObject* subjectMatch = (CMatchObject*)element;
																
					//Search TrustedSubjectMatch attributes													
					if ( *subjectMatch->MatchId() == TrustedSubjectMatch )
					{
						//and add valid editors (Trusted subjects) to valid editors list...
						if ( subjectMatch->AttributeDesignator()->GetAttributeid() == PolicyEngineXACML::KTrustedSubject )
						{
							iValidEditorsForSetting.AppendL( subjectMatch->AttributeValue()->Data()->Value().AllocL());
						}
					}		
				
					elementReserver.Release();
				}
			}
			
			CleanupStack::PopAndDestroy( &attributes);
		}
		
		ruleReserver.Release();			
	}

	policyReserver.Release();
	CleanupStack::PopAndDestroy( &rules);
	
}




// -----------------------------------------------------------------------------
// CSettingEnforcementManager::SessionCertificate()
// -----------------------------------------------------------------------------
//
TCertInfo& CSettingEnforcementManager::SessionCertificate()
{
	return iCertInfo;
}

// -----------------------------------------------------------------------------
// CEnforcementBase::SetSessionCertificate()
// -----------------------------------------------------------------------------
//
void CEnforcementBase::SetSessionCertificate( TCertInfo& aCertInfo)
{
	iCertInfo = &aCertInfo;
}


// -----------------------------------------------------------------------------
// CEnforcementBase::SetSessionCertificate()
// -----------------------------------------------------------------------------
//
TCertInfo& CEnforcementBase::SessionCertificate()
{
	return *iCertInfo;
}







