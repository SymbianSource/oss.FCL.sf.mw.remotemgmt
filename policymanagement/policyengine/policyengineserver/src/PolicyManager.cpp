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

#include "PolicyManager.h"
#include "elements.h"
#include "DataTypes.h"
#include "PolicyParser.h"
#include "ElementBase.h"
#include "PolicyProcessor.h"
#include "TrustedSession.h"
#include "ErrorCodes.h"
#include "OperationParser.h"
#include "OperationParserConstants.h"
#include "SettingEnforcementManager.h"
#include "PolicyEngineServer.h"
#include "PolicyEngineXACML.h"
#include "debug.h"


#include "DMUtilClient.h"
#include <PolicyEngineClientServer.h>
#include <ManagementContext.h>


// CONSTANTS
const TInt KMaxServerIdLength = 250;

// -----------------------------------------------------------------------------
// CPolicyManager::CPolicyManager()
// -----------------------------------------------------------------------------
//

CPolicyManager::CPolicyManager( CPolicyProcessor * aPolicyProcessor)
	: CActive( EPriorityLow),  iPolicyProcessor( aPolicyProcessor)
{
	iPolicyStorage = CPolicyStorage::PolicyStorage();
}

// -----------------------------------------------------------------------------
// CPolicyManager::~CPolicyManager()
// -----------------------------------------------------------------------------
//

CPolicyManager::~CPolicyManager()
{
	//CSettingEnforcementManager
	delete iSEManager;

	//descriptors used in operations 
	delete iActiveChildList;
	delete iActiveXACMLContent;
	delete iActiveElementList;
	delete iOperationParser;
	delete iCurrentManagementCommand;
	
	CPolicyEngineServer::RemoveActiveObject( this);
}

// -----------------------------------------------------------------------------
// CPolicyManager::ConstructL()
// -----------------------------------------------------------------------------
//
void CPolicyManager::ConstructL()
{
	CPolicyEngineServer::AddActiveObjectL( this);
	iSEManager = CSettingEnforcementManager::NewL();
}

// -----------------------------------------------------------------------------
// CPolicyStorage::NewL()
// -----------------------------------------------------------------------------
//

CPolicyManager * CPolicyManager::NewL( CPolicyProcessor * aPolicyProcessor)
{
	CPolicyManager * self = new(ELeave) CPolicyManager( aPolicyProcessor);
	
	CleanupStack::PushL( self);
	self->ConstructL();
	CleanupStack::Pop( self);
	
	return self;
	
}

// -----------------------------------------------------------------------------
// CPolicyManager::ExecuteOperationL()
// -----------------------------------------------------------------------------
//

void CPolicyManager::ExecuteOperation( CTrustedSession * aSession, const RMessage2& aMessage)
{
	iMessage = &aMessage;

	//Only one management session is accepted
	if ( CPolicyEngineServer::StatusFlags()->iManagementActive )
	{
		CompleteOperation( KErrInUse);
		return;
	}
	
	CPolicyEngineServer::StatusFlags()->iManagementActive = ETrue;


	//set pointer to trusted session and save reference to RMessage
	iTrustedSession = aSession;
	
	//initialize states
	iOperationStatus = 0;
	iUseBearerCertificate = EFalse;
	iCertificateUpdates = EFalse;

	
	//Add this active object active scheduler	
	if ( !IsAdded())
	{
		CActiveScheduler::Add( this);
	}
	
	//Complete own request
	SetActive();
	TRequestStatus * status = &iStatus;
	User::RequestComplete( status, KErrNone);
}



// -----------------------------------------------------------------------------
// CPolicyManager::RunL()
// -----------------------------------------------------------------------------
//

void CPolicyManager::RunL()
{
	CPolicyEngineServer::SetActiveSubSession( this);	

	//management operations are allowed only when server is free
 	if ( CPolicyEngineServer::StatusFlags()->iProcessorActive )
	{
		SetActive();
		TRequestStatus * status = &iStatus;
		User::RequestComplete( status, KErrNone);
		return;		
	}

	if ( iOperationStatus == 0)
	{
		//Start management session for next operations
		iPolicyStorage->StartCommitSessionL();
	
		iCurrentManagementCommand = HBufC8::NewL( iMessage->GetDesLength(0));
		//Read policy description	
		TPtr8 ptr = iCurrentManagementCommand->Des(); 
		iMessage->ReadL(0, ptr, 0);
		iOperationParser = COperationParser::NewL( ptr);
	
		//parse operations, "iMsg" stores error message
		iOperationParser->ParseOperationsL( iMsg);
	}
	else if ( (iOperationStatus - 1) < iOperationParser->OperationCount())
	{
		//Execute operations, "this" is pointer to callback function 
		iCertificateUpdates = EFalse;
		iOperationParser->ExecuteL( iOperationStatus - 1, this);
		
		if ( iCertificateUpdates)
		{
			CPolicyEngineServer::CertificateMaps()->NewMappingsAvailable();
		}			
	}
	
	if ( iOperationStatus == iOperationParser->OperationCount() + 2)
	{
		//Finish management session and commit changes in repository
		iMsg = ManagementErrors::OperationOk;
		
		if ( iStatus.Int() == KErrNone)
		{
			CommitChangesL();	
			CompleteOperation( iStatus.Int());
			CPolicyStorage::PolicyStorage()->ReleaseElements();
		}
		else
		{
			RunError( iStatus.Int());			
		}
		
	}
	else
	if ( iOperationStatus == iOperationParser->OperationCount() + 1)
	{
		//Make setting enforcements...
		MakeEnforcementL( KErrNone);
	}
	else
	{
		SetActive();
		TRequestStatus * status = &iStatus;
		User::RequestComplete( status, KErrNone);
	}

	iOperationStatus++;
}

// -----------------------------------------------------------------------------
// CPolicyManager::DoCancel()
// -----------------------------------------------------------------------------
//

void CPolicyManager::DoCancel()
{
	RunError( KErrAbort);
}

// -----------------------------------------------------------------------------
// CPolicyManager::DoCancel()
// -----------------------------------------------------------------------------
//

TInt CPolicyManager::RunError( TInt aError)
{
	RDEBUG("CPolicyManager::RunError");
	RDEBUG8_2("Policy engine operation error message: %S", &iMsg);

	//restores possible changes in policy storage
	TRAPD( err, iPolicyStorage->CommitChangesL( EFalse))
	
	if ( err != KErrNone )
	{
		aError = err;	
	}

	//complete operation
	CompleteOperation( aError);	
	
	return KErrNone;
}

// -----------------------------------------------------------------------------
// CPolicyManager::MakeEnforcementL()
// -----------------------------------------------------------------------------
//

void CPolicyManager::MakeEnforcementL( TInt aError)
{
	//initialize enforcement session
	iSEManager->StartEnforcementSessionL( iStatus, iTrustedSession->SessionCertificate());

	//get modified elements 
	iPolicyStorage->GetEditedElementsL( iSEManager);
	
	//start enforcement session enforcement policies
	if ( aError == KErrNone)
	{
		iSEManager->ExecuteEnforcementSessionL();
	}

	
	//set active (waiting for enforcement completing)
	SetActive();	
}

// -----------------------------------------------------------------------------
// CPolicyManager::CompleteOperation()
// -----------------------------------------------------------------------------
//

void CPolicyManager::CompleteOperation( TInt aError )
	{
	RDEBUG_2("CPolicyManager::CompleteOperation( %d )", aError );

	if( aError != KErrNone && aError != KErrParser && KErrOpParser != aError && KErrPolicyManager != aError)
		{
		iMsg = KNullDesC8;
		aError = KErrGeneral;
		}
	else
		{
		aError = KErrNone;
		}

	//finish storage session and enforcement session
	iPolicyStorage->ResetEditableMemory();
	iSEManager->EndEnforcementSession( KErrNone == aError );

	if ( iCertificateUpdates )
		{
		CPolicyEngineServer::CertificateMaps()->NewMappingsAvailable();
		}	

	CPolicyEngineServer::StatusFlags()->iManagementActive = EFalse;
	
	//release resources
	delete iOperationParser;
	iOperationParser = NULL;
	delete iCurrentManagementCommand;
	iCurrentManagementCommand = NULL;

	//write return message to client process
	TParserResponse response( iMsg);
	TPckgC<TParserResponse> retPack(response);
	TRAPD( err, iMessage->WriteL( 1, retPack ) );
	if( err != KErrNone )
		{
		iMessage->Complete( err );
		return;
		}

	iMessage->Complete( aError);
	}

// -----------------------------------------------------------------------------
// CPolicyManager::UpdateStateFlags()
// -----------------------------------------------------------------------------
//
void CPolicyManager::UpdateStateFlagsL()
{
	RDEBUG("CPolicyManager::UpdateStateFlagsL - start");

	RDMUtil dmutil;
	User::LeaveIfError( dmutil.Connect());
	CleanupClosePushL( dmutil);
	TBool securityActive = EFalse;
	User::LeaveIfError( dmutil.UpdatePolicyMngStatusFlags( EPolicyChanged));
	
	//Check is terminal security activated
	CElementBase * iElement = iPolicyStorage->GetElementL( PolicyLanguage::Constants::RolesMappingPolicy);	
	if (iElement)
	{
		TElementReserver reserver(iElement);

		iPolicyStorage->CheckElementL(iElement);

		
		//check child elements, any rule child means that terminal security is acticated
		for (TInt i(0); i < iElement->ElementCount(); i++) {
			CElementBase * element = iElement->Element(i);

			if (element->ElementType() == ERule) {
				RDEBUG("CPolicyManager::UpdateStateFlagsL -> securityActive = ETrue");
				securityActive = ETrue;
				break;
			}
		}
		reserver.Release();
	}
	
	//update policy management flags	
	if ( securityActive)
	{
		User::LeaveIfError( dmutil.UpdatePolicyMngStatusFlags( ETerminalSecurityActive));
	}
	else
	{
		User::LeaveIfError( dmutil.UpdatePolicyMngStatusFlags( ETerminalSecurityDeactive));
	}

	
	CleanupStack::PopAndDestroy( &dmutil);

	RDEBUG("CPolicyManager::UpdateStateFlagsL - end");
}





// -----------------------------------------------------------------------------
// CPolicyManager::CommitChangesL()
// -----------------------------------------------------------------------------
//
void CPolicyManager::CommitChangesL()
{
	//Accept or decline changes to storage...
	iPolicyStorage->SaveEditableMemoryL();

	//This ensures that enforcement settings are committed
	iSEManager->CommitChanges();	
		
	//This ensures that all changes are commited (also enforcement)
	iPolicyStorage->CommitChangesL( ETrue);	
	
	//update state flags
	UpdateStateFlagsL();
	
	RDEBUG8_2("Policy engine operation message: %S", &iMsg);
}


// -----------------------------------------------------------------------------
// CPolicyManager::NewPolicy()
// -----------------------------------------------------------------------------
//

void CPolicyManager::NewElementL( const TInt& aLineOffset, const TDesC8& aTargetElement, const TDesC8& aElementDescription, const TBool& aUseBearerCertificate )
{
	RDEBUG("PolicyEngineServer: CPolicyManager::NewElementL");

	//initialize policy parser...
	CPolicyParser * parser = CPolicyParser::NewL( iPolicyStorage);
	CleanupStack::PushL( parser);
	parser->SetExternalIdChecked( KNoExternalIdConflictsAllowed);
	parser->SetLineOffset( aLineOffset);
	iUseBearerCertificate = aUseBearerCertificate;

	//Parse native model from XACML description
	TInt err(KErrNone);	
	TBuf8<KMaxReturnMessageLength> msg;
	CElementBase * element = parser->ParseXACMLObjects( err, aElementDescription, msg);	
	CleanupStack::PopAndDestroy( parser); //Parser

	if ( err != KErrNone )
	{
		iMsg = msg;
		User::Leave( KErrParser);	
	}
	

	//Add element to policy storage (storage take responsibility to delete object)
	RDEBUG_2("PolicyEngineServer: Element parsed and id created(id %d)", element->GetId());

	element->CreateIdL();
	element->CreateExternalId();
	iPolicyStorage->AddNewElementL( element);
	TElementReserver newElement( element);
		
	//Check is the policy valid and if so, add element to temporary storage
	AssignElementL( aTargetElement, element);
	
	newElement.Release();
}

// -----------------------------------------------------------------------------
// CPolicyManager::ReplaceElementL()
// -----------------------------------------------------------------------------
//
void CPolicyManager::ReplaceElementL( const TInt& aLineOffset, const TDesC8& aTargetElement, const TDesC8& aElementDescription )
{
	RDEBUG("PolicyEngineServer: CPolicyManager::ReplaceElementL");

	//initialize policy parser...
	CPolicyParser * parser = CPolicyParser::NewL( iPolicyStorage);
	CleanupStack::PushL( parser);
	parser->SetExternalIdChecked( KSameExternalIdAllowedForRoot);
	parser->SetLineOffset( aLineOffset);

	//Parse native model from XACML description
	TInt err(KErrNone);	
	TBuf8<KMaxReturnMessageLength> msg;
	CElementBase * element = parser->ParseXACMLObjects( err, aElementDescription, msg);	
	CleanupStack::PopAndDestroy( parser); //Parser

	if ( err != KErrNone )
	{
		iMsg = msg;
		User::Leave( KErrParser);	
	}
	

	//Add element to policy storage (storage take responsibility to delete object)
	RDEBUG_2("PolicyEngineServer: Element parsed and id created(id %d)", element->GetId());

	element->CreateIdL();
	element->CreateExternalId();
	iPolicyStorage->AddNewElementL( element);
	TElementReserver newElement( element);
		
	//Check is the policy valid and if so, add element to temporary storage
	ReAssignElementL( aTargetElement, element);
	
	newElement.Release();
	
}


// -----------------------------------------------------------------------------
// CPolicyManager::AssignPolicyL()
// -----------------------------------------------------------------------------
//

void CPolicyManager::AssignElementL( const TDesC8& aTargetElement, CElementBase * aElement)
{
	RDEBUG("PolicyEngineServer: CPolicyManager::AssignElementL");

	//get parent element and load it
	CElementBase * parentElement = iPolicyStorage->GetEditableElementL( aTargetElement);

	//if parent eleemnt is valid
	if ( !parentElement)
	{
		RDEBUG("PolicyEngineServer: Target element not found");
		iMsg = ManagementErrors::ElementNotFound;
		User::Leave( KErrPolicyManager);
	}
	
	TElementReserver assignReserver( parentElement);
	iPolicyStorage->CheckElementL( parentElement);

	//add parant id to components
	aElement->AddParentIdL( parentElement->GetId());

	//Check is assignment acceptable
	IsAssigmentAcceptableL( parentElement, aElement, ETrue);
	assignReserver.Release();
	

	//Add element to parent element, check is assigment valid
	parentElement->AddIdElementL( aElement);
	parentElement->iElementState = EEditedElement;
}

// -----------------------------------------------------------------------------
// CPolicyManager::AssignPolicyL()
// -----------------------------------------------------------------------------
//

void CPolicyManager::ReAssignElementL( const TDesC8& aTargetElement, CElementBase * aElement)
{
	RDEBUG("PolicyEngineServer: CPolicyManager::ReAssignElementL");

	//get replaced element and load it
	CElementBase * replacedElement = iPolicyStorage->GetEditableElementL( aTargetElement);

	//check that element is valid
	if ( !replacedElement)
	{
		RDEBUG("PolicyEngineServer: Target element not found");
		iMsg = ManagementErrors::ElementNotFound;
		User::Leave( KErrPolicyManager);
	}
	
	//Check element
	TElementReserver assignReserver( replacedElement);
	iPolicyStorage->CheckElementL( replacedElement);

	//copy parents to new element
	aElement->AddParentIdL( replacedElement->iParentId);
		
	//Get parent elements
	CElementBase * parentElement = iPolicyStorage->GetEditableElementL( replacedElement->iParentId);
	
	//check that element is valid
	if ( !parentElement)
	{
		iMsg = ManagementErrors::ElementNotFound;
		User::Leave( KErrPolicyManager);
	}
		
	//Check element
	TElementReserver parentReserver( parentElement);
	iPolicyStorage->CheckElementL( parentElement);

	//Check is reassignment acceptable
	IsAssigmentAcceptableL( parentElement, aElement, EFalse);

	//remove child from parent child list
	parentElement->RemoveChild( replacedElement->GetId());
	
	//Add element to parent element and mark to modified
	parentElement->AddIdElementL( aElement);
	parentElement->iElementState = EEditedElement;
	
	//When edited element list is committed all "EDeletedEditableElement"s will be deleted...
	replacedElement->DeleteMarkRecursiveL();

	
	
	
	parentReserver.Release();
	assignReserver.Release();
}
// -----------------------------------------------------------------------------
// CPolicyManager::RemoveElementL()
// -----------------------------------------------------------------------------
//

void CPolicyManager::RemoveElementL( const TDesC8& aTargetElement)
{
	RDEBUG("CPolicyManager::RemoveElementL");


	iMsg = ManagementErrors::OperationOk;

	//get parent element and load it
	CElementBase * element = iPolicyStorage->GetEditableElementL( aTargetElement);
	
	//error if element not found
	if ( !element )
	{
		RDEBUG("PolicyEngineServer: Target element not found");
		iMsg = ManagementErrors::ElementNotFound;
		User::Leave( KErrPolicyManager);	
	}


	TElementReserver elementReserver( element);
	iPolicyStorage->CheckElementL( element);


	if ( element->iParentId)
	{
		//Remove element reference from parent element

		CElementBase * parentElement = iPolicyStorage->GetEditableElementL( element->iParentId);
		__ASSERT_ALWAYS ( parentElement, User::Panic( Panics::PolicyManagerPanic, KErrGeneral));
			
		//check element, use editable cache
		TElementReserver elementReserver( parentElement);
		iPolicyStorage->CheckElementL( parentElement);
		
		IsRemoveAcceptableL( parentElement, element );

		TInt err = parentElement->RemoveChild( element->GetId());
	
		if ( err != KErrNone )
		{
			User::Leave( KErrPolicyManager);	
		}
		
		parentElement->iElementState = EEditedElement;
		
		elementReserver.Release();
	}
	
	//When edited element list is committed all "EDeletedEditableElement" will be deleted...
	element->DeleteMarkRecursiveL();
	
	elementReserver.Release();
}


// -----------------------------------------------------------------------------
// CPolicyManager::IsAssigmentAcceptable()
// -----------------------------------------------------------------------------
//


void CPolicyManager::IsAssigmentAcceptableL( CElementBase * aParentElement, CElementBase * aElement, TBool aNewElement)
{
	RDEBUG("CPolicyManager::IsAssigmentAcceptableL");

	using namespace PolicyLanguage::Constants;
	using namespace AttributeContainerHelper;
	using namespace PolicyLanguage::NativeLanguage::AttributeValues;
	
	//return if policy check is not needed
	NO_POLICY_CHECK_RETURN;
	RDEBUG_3("CPolicyManager: Parent element (%d) element (%d)", aParentElement->GetId(), aElement->GetId());

	HBufC8* extId = aParentElement->ExternalId();
	TInt match = extId->CompareF( PolicyLanguage::Constants::RolesMappingPolicy );
	if( (match == 0) && aNewElement )
	{
		RDEBUG("CPolicyManager: Parent element is RolesMappingPolicy. Adding trust for server");
		TInt rolesElements = aParentElement->GetChildElementCountL();
				
		RDEBUG_2("CPolicyManager: rolesElements count (%d)", rolesElements);
		if( rolesElements >= 1 )
		{ 
			RDEBUG("CPolicyManager: Trust already exists");
			iMsg = ManagementErrors::AccessDenied;
			User::Leave( KErrPolicyManager);
		}				
		RDEBUG("CPolicyManager: No trust created yet");
	}
	//setup request context
	iPolicyProcessor->ResetRequestContext();
	iPolicyProcessor->SetTargetElement( aElement);
	iPolicyProcessor->SetSessionTrust( iTrustedSession);
	
	//attribute list for request attributes
	RAttributeContainer attributes;
	CleanupClosePushL( attributes);
	
	//action attributes for meta_policy_Set	
	if ( aNewElement)
	{
		attributes.AppendL( EActionAttributes, CAttribute::NewL( PolicyEngineXACML::KActionId, AddPolicy, StringDataType));
	}
	else
	{
		attributes.AppendL( EActionAttributes, CAttribute::NewL( PolicyEngineXACML::KActionId, ReplacePolicy, StringDataType));
	}

	//Trusted subject is either trustedsubject of session or trustedsubject used in element to be added.
	//In a cases where new element contains trustedsubject, it is used. Other cases sessions trustedsubject
	
	//Get new element subjectmatch-elements
	RElementArray subjectMatches;
	CleanupClosePushL( subjectMatches);
	aElement->FindAttributesL( ESubjectMatch, subjectMatches);
	
	//util variables
	TPtrC8 trustedSubject( KNullDesC8);
	TBool oneTrustedSubject = EFalse;
		
	//add trusted subject from target element
	for ( TInt i(0); i < subjectMatches.Count(); i++)
	{
		CMatchObject* subjectMatch = (CMatchObject*) subjectMatches[i];
		
		//check attribute id (Certificate id is also trusted subject!)
		TPtrC8 attributeId = subjectMatch->AttributeDesignator()->GetAttributeid();
		if ( attributeId == PolicyEngineXACML::KTrustedSubject || attributeId == PolicyEngineXACML::KCertificate  )
		{
			//Only one target policy is valid
			if ( oneTrustedSubject && trustedSubject.Compare( subjectMatch->AttributeValue()->Data()->Value()) != 0)
			{
				iMsg = ParserErrors::InvalidElement;
				User::Leave( KErrPolicyManager);				
			}
		
			oneTrustedSubject = ETrue;

			//add trusted subject to attribute list
			trustedSubject.Set( subjectMatch->AttributeValue()->Data()->Value());
			
			//certificates have to be transformed to CASN form
			if ( attributeId == PolicyEngineXACML::KCertificate)
			{
				//create CSubject info
				CCertificateMaps::CSubjectInfo* info = new (ELeave) CCertificateMaps::CSubjectInfo();
				CleanupStack::PushL( info);
				
				//parse CASN info from certificate and add TargetTrustedSubject attribute with CASN value				
				CCertificateMaps::CreateSubjectInfoL( info, trustedSubject);
				attributes.AppendL( ESubjectAttributes, CAttribute::NewL( TargetTrustedSubject, *info->iCASN, StringDataType));
				CleanupStack::PopAndDestroy( info);
			}
			else
			{
				attributes.AppendL( ESubjectAttributes, CAttribute::NewL( TargetTrustedSubject, trustedSubject, StringDataType));
			}
			
		}
	}
	CleanupStack::PopAndDestroy( &subjectMatches);	//subjectMatches
	
	
	
	if ( iUseBearerCertificate )
	{
		if ( !iTrustedSession->CertificatedSession())
		{
			RDEBUG("PolicyEngineServer: Certificated session is required");
			//when bearer certificate is used, session must be certificated
			iMsg = ManagementErrors::SessionMustBeCertificated;
			User::Leave( KErrPolicyManager);				
		}
	
		//add certificate mapping for alias, certificate (TCertInfo) is available inside of trusted session
		CElementBase * rule = iTrustedSession->CreateCertificateMappingL( trustedSubject);
		CleanupStack::PushL( rule);
		
		if ( !rule)
		{
			RDEBUG("PolicyEngineServer: Invalid certificate!");
			iMsg = ManagementErrors::InvalidCertificateMapping;
			User::Leave( KErrPolicyManager);
		}
		else
		{
			rule->CreateIdL();
			rule->CreateExternalId();
			iPolicyStorage->AddNewElementL( rule);
			
			RDEBUG_2("PolicyEngineServer: Auto certifate created (id %d)", rule->GetId());
			
			//add parant id to cert policy
			CElementBase* certMappingPolicy = iPolicyStorage->GetEditableElementL( CertMappingPolicy);
			//if element is not valid
			if (!certMappingPolicy)
			{
				return;
			}
			TElementReserver certReserver( certMappingPolicy);
			iPolicyStorage->CheckElementL( certMappingPolicy);

			rule->AddParentIdL( certMappingPolicy->GetId());

			//Add element to parent element, check is assigment valid
			certMappingPolicy->AddIdElementL( rule);
			certMappingPolicy->iElementState = EEditedElement;
			
						
			certReserver.Release();		
						
			CPolicyEngineServer::CertificateMaps()->NewMappingsAvailable();
			iCertificateUpdates = ETrue;			
		}
		CleanupStack::Pop( rule);	
	}
	

	//add session trusted subject
	if ( iTrustedSession->CertificatedSession())
	{
		attributes.AppendL( ESubjectAttributes, 
				CAttribute::NewL( PolicyEngineXACML::KTrustedSubject, iTrustedSession->CASNForSessionL(), StringDataType));
		attributes.AppendL( ESubjectAttributes, 
				CAttribute::NewL( PolicyEngineXACML::KRoleId, iTrustedSession->CASNForSessionL(), StringDataType));
	}
	else
	{
		attributes.AppendL( ESubjectAttributes, 
				CAttribute::NewL( PolicyEngineXACML::KSubjectId, iTrustedSession->SIDForSession(), StringDataType));
	}

	//certificate mapping must be checked first!! 
	if ( *aParentElement->ExternalId() == CertMappingPolicy)
	{
		TBool validCertificate = EFalse;
		TRAPD( err, validCertificate = iTrustedSession->IsCertificateMappingValidL( aElement))
	
		if ( !validCertificate || err != KErrNone)
		{
			RDEBUG("PolicyEngineServer: Invalid certificate mapping!");
			iMsg = ManagementErrors::InvalidCertificateMapping;
			User::Leave( KErrPolicyManager);
		}
		
		iCertificateUpdates = ETrue;
	}
	
	
	//parent element for new policy
	attributes.AppendL( EResourceAttributes, CAttribute::NewL( PolicyTargetAttr, *aParentElement->ExternalId(), StringDataType));

	//make request
	TMatchResponse response;

	RDEBUG("CPolicyManager - Start request execution");
	iPolicyProcessor->ExecuteRequestL( attributes, response);
	RDEBUG("CPolicyManager - End request execution");
		
	CleanupStack::PopAndDestroy( &attributes);
	RDEBUG("CPolicyManager - CleanupStack::PopAndDestroy( &attributes);");

	if ( response != EPermit)
	{
		RDEBUG("CPolicyManager - Access denied");
		iMsg = ManagementErrors::AccessDenied;
		User::Leave( KErrPolicyManager);
	}
		
	//add current DM session server id to list which defines server ids which DM operations are silent.
	if ( iUseBearerCertificate)
	{
		AddSilentDMOperationServerIdL();
	}
		
}
	
// -----------------------------------------------------------------------------
// CPolicyManager::IdRemoveAcceptableL()
// -----------------------------------------------------------------------------
//

void CPolicyManager::IsRemoveAcceptableL( CElementBase* aParentElement, CElementBase* aElement )
	{
	RDEBUG("CPolicyManager::IsRemoveAcceptableL");

	using namespace PolicyLanguage::Constants;
	using namespace AttributeContainerHelper;
	using namespace PolicyLanguage::NativeLanguage::AttributeValues;

	//return if policy check is not needed
	NO_POLICY_CHECK_RETURN;
	
	//setup request context
	iPolicyProcessor->ResetRequestContext();
	iPolicyProcessor->SetTargetElement( aElement );
	iPolicyProcessor->SetSessionTrust( iTrustedSession );
	
	//attribute list for request attributes
	RAttributeContainer attributes;
	CleanupClosePushL( attributes );
	
	//action attributes for meta_policy_Set	
	attributes.AppendL( EActionAttributes, CAttribute::NewL( PolicyEngineXACML::KActionId, RemovePolicy, StringDataType));

	//subject attributes for trusted sessions
	if( iTrustedSession->CertificatedSession() )
		{
		attributes.AppendL( ESubjectAttributes, 
				CAttribute::NewL( PolicyEngineXACML::KRoleId, iTrustedSession->CASNForSessionL(), StringDataType));
		attributes.AppendL( ESubjectAttributes, 
				CAttribute::NewL( PolicyEngineXACML::KTrustedSubject, iTrustedSession->CASNForSessionL(), StringDataType));
		}
	else
		{
		attributes.AppendL( ESubjectAttributes, 
				CAttribute::NewL( PolicyEngineXACML::KSubjectId, iTrustedSession->SIDForSession(), StringDataType));
		
		}
	
	// ======= original certificate mapping ======================
	//search trustedsubject from target element (element to be deleted)
	HBufC8* targetTrustedSubject = iTrustedSession->GetTargetTrustedSubjectL( aElement, ETrue );	

	//and target element trusted subject to request context....
	if( targetTrustedSubject )
		{
		CleanupStack::PushL( targetTrustedSubject); 
		attributes.AppendL( ESubjectAttributes, CAttribute::NewL( TargetTrustedSubject, *targetTrustedSubject, StringDataType));
		CleanupStack::PopAndDestroy( targetTrustedSubject);
		}
	// ===========================================================
	
	
	// ======= mapped certificates ===============================
	// all certificate mappings, including mapped certificates, are under "cert_mapping_policy"
	CElementBase* certMappings = iPolicyStorage->GetEditableElementL( _L8("cert_mapping_policy") );
	if( certMappings )
		{
		TElementReserver mappedElementReserver( certMappings );
		iPolicyStorage->CheckElementL( certMappings );
	
		RPointerArray<HBufC8> mappedTargetTrustedSubjects;
		iTrustedSession->GetMappedTargetTrustedSubjectL( mappedTargetTrustedSubjects, certMappings, ETrue );
		TInt count = mappedTargetTrustedSubjects.Count();
		if( count )
			{
			for( TInt i = 0; i < count; i++ )
				{
				HBufC8* mappedTargetTrustedSubject = mappedTargetTrustedSubjects[ i ];
				CleanupStack::PushL( mappedTargetTrustedSubject);
				attributes.AppendL( ESubjectAttributes, CAttribute::NewL( PolicyEngineXACML::KCertificateMapped, *mappedTargetTrustedSubject, StringDataType));
				CleanupStack::PopAndDestroy( mappedTargetTrustedSubject ); 
				}
			}
	
		mappedTargetTrustedSubjects.Close();
		mappedElementReserver.Release();
		}
	// ===========================================================
	
	//parent element for new policy
	attributes.AppendL( EResourceAttributes, CAttribute::NewL( PolicyTargetAttr, *aParentElement->ExternalId(), StringDataType));

	//make request
	TMatchResponse response;
	iPolicyProcessor->ExecuteRequestL( attributes, response );
	CleanupStack::PopAndDestroy( &attributes );

	if( response != EPermit )
		{
		iMsg = ManagementErrors::AccessDenied;
		User::Leave( KErrPolicyManager );
		}	
	}
	

// -----------------------------------------------------------------------------
// CPolicyManager::GetElementListL()
// -----------------------------------------------------------------------------
//		
void CPolicyManager::GetElementListL( const RMessage2& aMessage)
{
	
	//element info is two phase operation...
	if ( aMessage.Function() == EGetElementListLength)
	{
		//In first phase descriptor lengths are fetched

		//Read element types
		TElementType type;
		TPckg<TElementType> typePack( type);
		aMessage.ReadL(0, typePack, 0);
		
		//fetch element
		CElementBase * element = iPolicyStorage->GetElementL( PolicyLanguage::Constants::RootElement);
		
		if ( element)
		{
			//reserve element and check it
			TElementReserver elementReserver( element);
			iPolicyStorage->CheckElementL( element);
			
			//get list length
			TInt length = element->GetElementsListLengthL( type);
	
			delete iActiveElementList;
			iActiveElementList = NULL;
			
			iActiveElementList = HBufC8::NewL( length);
			TPtr8 ptr = iActiveElementList->Des();
			element->GetElementsListL( type, ptr);
	
	
			//write values to client side
			TPckgC<TInt> intPack( length); 
			aMessage.WriteL(1, intPack);	
			
			elementReserver.Release();
		}
		else
		{
			User::Leave( KErrNotFound);
		}
	}
	else
	{
		// and in second phase descriptor are written to client side
		if ( iActiveElementList )
		{
			aMessage.WriteL(0, *iActiveElementList);	
		}
		
		//delete child list
		delete iActiveElementList;
		iActiveElementList = 0;
	}
	
}
	
	
	
// -----------------------------------------------------------------------------
// CPolicyManager::GetElementInfoL()
// -----------------------------------------------------------------------------
//


void CPolicyManager::GetElementInfoL( const RMessage2& aMessage)
{


	//element info is two phase operation...
	if ( aMessage.Function() == EGetElementDescriptionAndChildListLength)
	{
		//In first phase descriptor lengths are fetched

		//Read element id
		TPtr8 id = HBufC8::NewLC( aMessage.GetDesLength(0))->Des(); 
		aMessage.ReadL(0, id, 0);
		
		//fetch element
		CElementBase * element = iPolicyStorage->GetElementL( id);
		
		if ( element)
		{
			//reserve element and check it
			TElementReserver elementReserver( element);
			iPolicyStorage->CheckElementL( element);
			
			//Create helper pack, which is delivered to client side			
			TElementInfoHelpPack helper;
			helper.iDescriptionLength = 0;
				
			//get description and child list		
			iActiveDescription = element->DescriptionL();
			if ( iActiveDescription)
			{
				helper.iDescriptionLength = iActiveDescription->Length();
			}
	
			helper.iChildListLength = element->GetChildListLengthL();
	
			//write values to client side
			TPckgC<TElementInfoHelpPack> helperPack( helper); 
			
			delete iActiveChildList;
			iActiveChildList = NULL;
			iActiveChildList = HBufC8::NewL( helper.iChildListLength); 
			TPtr8 ptr = iActiveChildList->Des();
			element->GetChildListL( ptr);
	
			aMessage.WriteL(1, helperPack);	
			
			elementReserver.Release();
		}
		else
		{
			User::Leave( KErrNotFound);
		}
		
		CleanupStack::PopAndDestroy();	//HBufC8
	}
	else
	{
		// and in second phase descriptor are written to client side
		if ( iActiveDescription)
		{
			aMessage.WriteL(0, *iActiveDescription);	
		}

		if ( iActiveChildList)
		{
			aMessage.WriteL(1, *iActiveChildList);	
		}

		
		//description is owned by descriptor element (do not delete)
		iActiveDescription = 0;
		//delete child list
		delete iActiveChildList;
		iActiveChildList = 0;
	}
}

// -----------------------------------------------------------------------------
// CPolicyManager::GetElementXACMLDescriptionL()
// -----------------------------------------------------------------------------
//

void CPolicyManager::GetElementXACMLDescriptionL( const RMessage2& aMessage)
{
	

	//get element XACML description is two phase operation...
	if ( aMessage.Function() == EGetElementXACMLLength)
	{
		//Read element id
		TPtr8 id = HBufC8::NewLC( aMessage.GetDesLength(0))->Des(); 
		aMessage.ReadL(0, id, 0);
	
		//In first phase descriptor length is fetched.
		CElementBase * element = iPolicyStorage->GetElementL( id);

		if ( element)
		{
			//reserve element and check it
			TElementReserver elementReserver( element);
			iPolicyStorage->CheckElementL( element);
	
			//Member pointer is used to save XACML description during two phase operation
			delete iActiveXACMLContent;
			iActiveXACMLContent = NULL;
			iActiveXACMLContent = element->DecodeElementL( EXACML, EFullMode);
		
			TInt length = 0;
			if ( iActiveXACMLContent)
			{
				length = iActiveXACMLContent->Length();
			}
	
			//write values to client side
			TPckg<TInt> descriptionLength( length); 
			aMessage.WriteL(1, descriptionLength);	
			
			elementReserver.Release();
		}
		else
		{
			User::Leave( KErrNotFound);
		}
	
		CleanupStack::PopAndDestroy();	//HBufC8
	}
	else
	{
		// and in second phase descriptor are written to client side
		if ( iActiveXACMLContent )
		{
			aMessage.WriteL(0, *iActiveXACMLContent);	
		}
		
		//delete XACML content
		delete iActiveXACMLContent;
		iActiveXACMLContent = NULL;
		
	}
}

// -----------------------------------------------------------------------------
// CPolicyManager::GetElementXACMLDescriptionL()
// -----------------------------------------------------------------------------
//
void CPolicyManager::IsServerIdValidL( const RMessage2& aMessage)
{
	RDEBUG("PolicyEngineServer: CPolicyManager: Is allowed server id");
	
	//Read server id
	TPtr8 id = HBufC8::NewLC( aMessage.GetDesLength(0))->Des(); 
	aMessage.ReadL(0, id, 0);

	//Create package for data
	TBool response;
	TPckg<TBool> respPck( response);

	//check id
	response = CPolicyStorage::PolicyStorage()->IsServerIdValid( id);

	if ( response )
		{
		RDEBUG("CPolicyManager: Is allowed server id -> TRUE");
		}
	else
		{
		RDEBUG("CPolicyManager: Is allowed server id -> FALSE");
		}
	
	//write response
	aMessage.WriteL(1, respPck);	
	
	CleanupStack::PopAndDestroy();	
}

// -----------------------------------------------------------------------------
// CPolicyManager::GetCertificateRoleL()
// -----------------------------------------------------------------------------
//
void CPolicyManager::GetCertificateRoleL( const RMessage2& aMessage)
	{
	RDEBUG("PolicyEngineServer: CPolicyManager::GetCertificateRoleL");

	//Create package for data
	TCertInfo certInfo;
	TPckg<TCertInfo> certInfoPck( certInfo);
	aMessage.ReadL(0, certInfoPck, 0);

	//TRole
	TRole role;
	TPckg<TRole> rolePck( role);
	
	//get certificate role...
	role = CPolicyEngineServer::CertificateMaps()->CertificateRoleL( certInfo, EFalse );

	//write response
	aMessage.WriteL( 1, rolePck );	
	}


// -----------------------------------------------------------------------------
// CPolicyManager::AddSilentDMOperationServerIdL()
// -----------------------------------------------------------------------------
//
void CPolicyManager::AddSilentDMOperationServerIdL()
{
	//connect to DM util client...
	RDMUtil dmutil;
	User::LeaveIfError( dmutil.Connect());
	CleanupClosePushL( dmutil);
	
	//..and get server id
	TBuf8<KMaxServerIdLength> serverid;
	User::LeaveIfError( dmutil.GetDMSessionServerId( serverid));
	
	User::LeaveIfError( CPolicyStorage::PolicyStorage()->AddNewServerId( serverid));


	CleanupStack::PopAndDestroy( &dmutil); 
}


// -----------------------------------------------------------------------------
// CPolicyManager::IsCurrentServerIdTrustedL()
// -----------------------------------------------------------------------------
//
TBool CPolicyManager::IsCurrentServerIdTrustedL()
{
	TBool trustedServer = EFalse;
	
	//connect to DM util client...
	RDMUtil dmutil;
	User::LeaveIfError( dmutil.Connect());
	CleanupClosePushL( dmutil);
	
	//..and get server id
	TBuf8<KMaxServerIdLength> serverid;
	User::LeaveIfError( dmutil.GetDMSessionServerId( serverid));
	TRAPD( err, trustedServer = CPolicyStorage::PolicyStorage()->IsServerIdValid( serverid) );
	User::LeaveIfError( err );
	CleanupStack::PopAndDestroy( &dmutil); 
	
	return trustedServer;
}
