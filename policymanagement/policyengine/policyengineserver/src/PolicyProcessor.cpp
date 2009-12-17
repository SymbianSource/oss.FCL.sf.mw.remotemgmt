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

#include "PolicyProcessor.h"
#include "PolicyStorage.h"
#include "PolicyParser.h"
#include "elements.h"
#include "ElementBase.h"
#include "DataTypes.h"
#include "ErrorCodes.h"
#include "TrustedSession.h"
#include "XACMLconstants.h"
#include "PolicyEngineServer.h"
#include "PolicyEngineClientServer.h"
#include "debug.h"

#include "PolicyEngineUi.h"

// CONSTANTS
const TUid KUidPolicyEngineUi = { 0x10207817 };


// -----------------------------------------------------------------------------
// RAttributeContainer::AppendL()
// -----------------------------------------------------------------------------
//
void RAttributeContainer::AppendL( AttributeContainerHelper::TAttributeTypes aAttributeTypes, CAttribute* aAttribute)
{
	//temporary to cleanup stack
	CleanupStack::PushL( aAttribute);

	//create container for attribute
	AttributeContainerHelper::TContainer * container = new (ELeave) AttributeContainerHelper::TContainer;
	CleanupStack::PushL( container);
	
	//set container properties
	container->iAttribute = aAttribute;
	container->iAttributeType = aAttributeTypes; 

	//append to list
	RArray::AppendL( container);
		
	//remove from cleanup
	CleanupStack::Pop( 2, aAttribute);
}

// -----------------------------------------------------------------------------
// RAttributeContainer::Close()
// -----------------------------------------------------------------------------
//
void RAttributeContainer::Close()
{
	//delete attributes and container...
	for ( TInt i(0); i < Count(); i++)
	{
		AttributeContainerHelper::TContainer * container = operator[]( i);
		delete container->iAttribute;
		delete container;
	}
	
	RArray::Close();
}



// -----------------------------------------------------------------------------
// CPolicyProcessor::CPolicyProcessor()
// -----------------------------------------------------------------------------
//

CPolicyProcessor::CPolicyProcessor()
	: CActive( EPriorityStandard)
{
}


// -----------------------------------------------------------------------------
// CPolicyProcessor::NewL()
// -----------------------------------------------------------------------------
//

CPolicyProcessor * CPolicyProcessor::NewL()
{
	CPolicyProcessor * self = new(ELeave) CPolicyProcessor();
	
	CleanupStack::PushL( self);
	self->ConstructL();
	CleanupStack::Pop( self);
	
	return self;
	
}
	
// -----------------------------------------------------------------------------
// CPolicyProcessor::~CPolicyProcessor()
// -----------------------------------------------------------------------------
//

CPolicyProcessor::~CPolicyProcessor()
{
	ResetRequestContext();
	delete iRequestBuffer;

	CPolicyEngineServer::RemoveActiveObject( this);
}

// -----------------------------------------------------------------------------
// CPolicyProcessor::ConstructL()
// -----------------------------------------------------------------------------
//

void CPolicyProcessor::ConstructL()
{
	CPolicyEngineServer::AddActiveObjectL( this);
}

// -----------------------------------------------------------------------------
// CPolicyProcessor::HandleErrorL()
// -----------------------------------------------------------------------------
//

void CPolicyProcessor::HandleErrorL( const TDesC8& aText	)
{
	RDEBUG("CPolicyProcessor::HandleErrorL");
	//error handling for processor
	RDEBUG_2("XACML request indeterminate: %S", &aText);
	User::Leave( KErrAbort);	
}

// -----------------------------------------------------------------------------
// CPolicyProcessor::ResetRequestContext()
// -----------------------------------------------------------------------------
//
void CPolicyProcessor::ResetRequestContext()
{
	if ( iDeleteAttributes )
	{
		//delete subject attributes and closes RArray
		for ( TInt  i(0); i < iSubjects.Count(); i++)
		{
			CAttribute * attribute = iSubjects[i];
			delete attribute;
		}

		//delete actions attributes and closes RArray
		for ( TInt  i(0); i < iActions.Count(); i++)
		{
			CAttribute * attribute = iActions[i];
			delete attribute;
		}

		//delete resources attributes and closes RArray
		for ( TInt  i(0); i < iResources.Count(); i++)
		{
			CAttribute * attribute = iResources[i];
			delete attribute;
		}

		//delete environments attributes and closes RArray
		for ( TInt  i(0); i < iEnvironments.Count(); i++)
		{
			CAttribute * attribute = iEnvironments[i];
			delete attribute;
		}
	}
	
	iSubjects.Close();
	iActions.Close();
	iResources.Close();
	iEnvironments.Close();
}



// -----------------------------------------------------------------------------
// CPolicyProcessor::ResetRequestContext()
// -----------------------------------------------------------------------------
//
TPtrC8 CPolicyProcessor::NextToken( TPtrC8& aText)
{
	if ( !aText.Length() ) return aText;

	//remove first delimiter
	if ( aText[0] == KMessageDelimiterChar)
	{
		aText.Set( aText.Mid(1));
	}
		
	//find next delimiter
	TPtrC8 retVal = aText;
	TInt index = aText.Locate( KMessageDelimiterChar);		    	
		
	if ( index != KErrNotFound )
	{
		//set retVal to point token and aText to remaining part
		retVal.Set( aText.Left( index));
		aText.Set( aText.Mid( index ));
	}
	
	return retVal;
}

// -----------------------------------------------------------------------------
// CPolicyProcessor::ExecuteRequestL()
// -----------------------------------------------------------------------------
//
void CPolicyProcessor::ExecuteRequestL( RAttributeContainer &aAttributes, TMatchResponse &aResponse)
{
	iDeleteAttributes = EFalse;
	iUseEditedElements = ETrue;

	//reset request context
	ResetRequestContext();

	for ( TInt i(0); i < aAttributes.Count(); i++)
	{
		RAttributeList * list = 0;
	
		using namespace AttributeContainerHelper;
		TContainer * container = aAttributes[i];
	
		switch ( container->iAttributeType )
		{
			case ESubjectAttributes :
			{
				list = &iSubjects;
				break;		
			}
			case EActionAttributes :
			{
				list = &iActions;
				break;		
			}
			case EResourceAttributes :
			{
				list = &iResources;
				break;		
			}
			case EEnvironmentAttributes :
			{
				list = &iEnvironments;
				break;		
			}
			default:
				User::Panic( PolicyParserPanic, KErrCorrupt);
			break;
		}
		
		//append attribute to attribute list
		list->AppendL( container->iAttribute);
	}
	
	//make request
	DoRequestExecutionL( aResponse, ETrue);
}

// -----------------------------------------------------------------------------
// CPolicyProcessor::ExecuteRequestL()
// -----------------------------------------------------------------------------
//
void CPolicyProcessor::ExecuteRequestL( const RMessage2& aMessage)
{
	iUseEditedElements = EFalse;
	iDeleteAttributes = ETrue;


	RDEBUG("PolicyEngineServer: CPolicyProcessor::ExecuteRequestL");
	
	
	//reset request context
	ResetRequestContext();
	
	iMessage = &aMessage;
	iProcessorState = 0;
	
	//Add object to active scheduler
	if ( !IsAdded())
	{
		CActiveScheduler::Add( this);
	}
	
	//complete own request
	SetActive();
	TRequestStatus * status = &iStatus;
	User::RequestComplete( status, KErrNone);
}


// -----------------------------------------------------------------------------
// CPolicyProcessor::RunL()
// -----------------------------------------------------------------------------
//

void CPolicyProcessor::RunL()
{
	RDEBUG("PolicyEngineServer: CPolicyProcessor::RunL");
	
	CPolicyEngineServer::SetActiveSubSession( this);	

	if ( iProcessorState == 0)
	{
		//Read request context from client
		delete iRequestBuffer;
		iRequestBuffer = NULL;
		iRequestBuffer = HBufC8::NewL( iMessage->GetDesLength(0));
		TPtr8 msg = iRequestBuffer->Des();
		iMessage->ReadL(0, msg, 0);	
		TPtrC8 ptr = msg;

		TBool continueLoop( ETrue);
		
		//read msg from client, msg contain request attributes
		while ( continueLoop )
		{
			TPtrC8 type = NextToken( ptr);	
			RAttributeList * list;
	
			//identifie attribute type (subject, action, resource or env)
			if ( type == _L8("S"))
			{
				list = &iSubjects;
			}
			else
			if ( type == _L8("A"))
			{
				list = &iActions;
			}
			else
			if ( type == _L8("R"))
			{
				list = &iResources;
			}
			else
			if ( type == _L8("E"))
			{
				list = &iEnvironments;
			}
			else
			{
				//no more attributes
				continueLoop = EFalse;
			}
		
			if ( continueLoop )
			{
				//read attribute id, value and datatype
				TPtrC8 aAttributeId( NextToken( ptr));
				TPtrC8 aAttributeValue( NextToken( ptr));
				TPtrC8 aDataType( CPolicyParser::ConvertValues( EXACML, NextToken( ptr)));
		
				//types, which contains default values must be converted to native language
				if ( aDataType == PolicyLanguage::NativeLanguage::AttributeValues::BooleanDataType )
				{
					aAttributeValue.Set( CPolicyParser::ConvertValues( EXACML, aAttributeValue));
				}

				CAttribute * attribute = CAttribute::NewL( aAttributeId, 
													   aAttributeValue, 
													   aDataType);
			
				//append attribute to attribute list
				list->AppendL( attribute);
			}
		}

		iProcessorState++;		
		SetActive();
		TRequestStatus * status = &iStatus;
		User::RequestComplete( status, KErrNone);
		return;	
	}

	if ( CPolicyEngineServer::StatusFlags()->iProcessorActive && !CPolicyEngineServer::StatusFlags()->iUiActive )
	{
		//Start new execution only if no other executions active or
		//if execution is ongoing in state
		SetActive();
		TRequestStatus * status = &iStatus;
		User::RequestComplete( status, KErrNone);
		return;		
	}
	
	//tell other processors that processor is reserved
	CPolicyEngineServer::StatusFlags()->iProcessorActive = ETrue;
	
	//make request
	TMatchResponse matchResponse;
	RDEBUG("PolicyEngineServer: RunL - start request execution");
	TRAPD( err, DoRequestExecutionL( matchResponse, EFalse));
	RDEBUG("PolicyEngineServer: RunL - stop request execution");
	
	if ( err != KErrNone)
	{
		matchResponse = EIndeterminate;
	}
	
	//set default response
	TResponse response;
	response.SetResponseValue( EResponseIndeterminate);
	
	
	//convert internal response to external response
	switch ( matchResponse )
	{
		case EDeny :
		{
			RDEBUG("PolicyEngineServer: Request executed - response deny");
			response.SetResponseValue( EResponseDeny);
			break;
		}
		case EPermit :
		{
			RDEBUG("PolicyEngineServer: Request executed - response permit");
			response.SetResponseValue( EResponsePermit);
			break;
		}
		case ENotApplicable :
		{
			RDEBUG("PolicyEngineServer: Request executed - response not applicable");
			response.SetResponseValue( EResponseNotApplicable);
			break;
		}
		case EIndeterminate :
		{
			RDEBUG("PolicyEngineServer: Request executed - response indeterminate");
			response.SetResponseValue( EResponseIndeterminate);
			break;
		}
		default:
		{
			RDEBUG("PolicyEngineServer: Request executed - response indeterminate");
			response.SetResponseValue( EResponseIndeterminate);
			break;
		}
	}	
	
	//tell other processors that processor is free
	CPolicyEngineServer::StatusFlags()->iProcessorActive = EFalse;
		
	CompleteMessage( KErrNone, response);
}

// -----------------------------------------------------------------------------
// CPolicyProcessor::DoCancel()
// -----------------------------------------------------------------------------
//
void CPolicyProcessor::DoCancel()
{
	RunError( KErrAbort);
}

// -----------------------------------------------------------------------------
// CPolicyProcessor::RunError()
// -----------------------------------------------------------------------------
//
TInt CPolicyProcessor::RunError( TInt /*aError*/)
{
	TResponse resp;
	resp.SetResponseValue( EResponseIndeterminate);
	CompleteMessage( KErrAbort, resp );
	
	return KErrNone;	
}

// -----------------------------------------------------------------------------
// CPolicyProcessor::CompleteMessage()
// -----------------------------------------------------------------------------
//
void CPolicyProcessor::CompleteMessage( TInt aError, TResponse aResponse)
	{
	RDEBUG_2("PolicyEngineServer: CPolicyProcessor::CompleteMessage( %d )", aError );

	delete iRequestBuffer; 	
	iRequestBuffer = NULL;

	//write response to msg (to client side)
	TPckg< TResponse> pack( aResponse);
	TRAPD( err, iMessage->WriteL(1, pack) );	
	if( err != KErrNone )
		{
		iMessage->Complete( err );
		}
	else
		{
		iMessage->Complete( aError );
		}

	RDEBUG("PolicyEngineServer: CPolicyProcessor::CompleteMessage - end");
	}


// -----------------------------------------------------------------------------
// CPolicyProcessor::DoRequestExecutionL()
// -----------------------------------------------------------------------------
//
void CPolicyProcessor::DoRequestExecutionL( TMatchResponse &aResponse, TBool iUseEditableElements)
{
	RDEBUG("PolicyEngineServer: CPolicyProcessor::DoRequestExecutionL");



	//get root element, execution starts from root  
	CElementBase * rootElement = 0;
	
	if ( iUseEditableElements )
	{
		rootElement = CPolicyStorage::PolicyStorage()->GetEditableElementL( PolicyLanguage::Constants::RootElement);
	}
	else
	{
		rootElement = CPolicyStorage::PolicyStorage()->GetElementL( PolicyLanguage::Constants::RootElement);
	}
	
	aResponse = EIndeterminate;
	
	//if root found
	if ( rootElement )
	{
		//reserve element from storage and check match to root policy
		TElementReserver rootReserver( rootElement);
		aResponse = rootElement->MatchL( this);
		rootReserver.Release();
		//execution runs recursive inside MatchL-function to correct branches and elements in policy system
		//response is result of many element combination
		RDEBUG("PolicyEngineServer: DoRequestExecutionL - Match evaluated");
	}
	
	RDEBUG("PolicyEngineServer: CPolicyProcessor::DoRequestExecutionL - end");
}

// -----------------------------------------------------------------------------
// CPolicyProcessor::FunctionL()
// -----------------------------------------------------------------------------
//
void CPolicyProcessor::FunctionL( const TDesC8 &aFunctionId, RParameterList& aParams, CAttributeValue* aResponseElement)
{
	RDEBUG("PolicyEngineServer: CPolicyProcessor::FunctionL");

	//executes correct function call
	if ( aFunctionId == PolicyLanguage::NativeLanguage::Functions::FunctionAnd)
	{
		RDEBUG("PolicyEngineServer: AND-function start");
		AndFunctionL( aParams, aResponseElement);
		RDEBUG("PolicyEngineServer: AND-function end");
	}
	else if ( aFunctionId == PolicyLanguage::NativeLanguage::Functions::FunctionOr)
	{
		RDEBUG("PolicyEngineServer: OR-function start");
		OrFunctionL( aParams, aResponseElement);
		RDEBUG("PolicyEngineServer: OR-function end");
	}
	else if ( aFunctionId == PolicyLanguage::NativeLanguage::Functions::FunctionNot)
	{
		RDEBUG("PolicyEngineServer: NOT-function start");
		NotFunctionL( aParams, aResponseElement);		
		RDEBUG("PolicyEngineServer: NOT-function end");
	}
	else if ( aFunctionId == PolicyLanguage::NativeLanguage::Functions::FunctionRuleTargetStructure)
	{
		RDEBUG("PolicyEngineServer: Rule target structure-function start");
		RuleTargetStructureFunctionL( aParams, aResponseElement);
		RDEBUG("PolicyEngineServer: Rule target structure-function end");
	}
	else if ( aFunctionId == PolicyLanguage::NativeLanguage::Functions::FunctionUserAcceptCorpPolicy)
	{		
		RDEBUG("PolicyEngineServer: User accept corporate-function start");
		CorporateUserAcceptFunctionL( aParams, aResponseElement);
		RDEBUG("PolicyEngineServer: User accept corporate-function end");
	} 
	else if ( aFunctionId == PolicyLanguage::NativeLanguage::Functions::FunctionCertificatedSession)
	{
		RDEBUG("PolicyEngineServer: Certificate for session-function start");
		CertificateForSessionL( aResponseElement);
		RDEBUG("PolicyEngineServer: Certificate for session-function end");
	}
	else if ( aFunctionId == PolicyLanguage::NativeLanguage::Functions::FunctionStringEqualId || 
			  aFunctionId == PolicyLanguage::NativeLanguage::Functions::FunctionBooleanEqualId)
	{
		RDEBUG("PolicyEngineServer: Equal-function start");
		EqualFunctionsL( aParams, aResponseElement);
		RDEBUG("PolicyEngineServer: Equal-function end");
	}
	else
	{
		RDEBUG("PolicyEngineServer: Function is not allowed!");
		HandleErrorL( RequestErrors::FunctionIsNotAllowed);
	}
}

// -----------------------------------------------------------------------------
// CPolicyProcessor::BooleanFunction()
// -----------------------------------------------------------------------------
//

TBool CPolicyProcessor::MatchFunctionL( const TDesC8 &aFunctionId, CDataType * data1, CDataType * data2)
{
	RDEBUG("PolicyEngineServer: CPolicyProcessor::MatchFunctionL");

	//data1 is policy context value
	//data2 is request context value

	TBool retVal(EFalse);

	//execute function
	if ( aFunctionId == PolicyLanguage::NativeLanguage::Functions::FunctionStringEqualId ||
		 aFunctionId == PolicyLanguage::NativeLanguage::Functions::FunctionBooleanEqualId )
	{
		TInt value = data1->Compare( data2);
		retVal = ( 0 == value);
	}
	else if ( aFunctionId == PolicyLanguage::NativeLanguage::Functions::TrustedSubjectMatch)
	{
		//compare trusted subjects...
		retVal = iTrustedSession->CertMatchL( data1->Value(), data2->Value(), iUseEditedElements);
	} 
	else if ( aFunctionId == PolicyLanguage::NativeLanguage::Functions::TrustedRoleMatch)
	{
		//compare roles and subjects...
		retVal = iTrustedSession->RoleMatchL( data2->Value(), data1->Value(), iUseEditedElements);
	}
	else
	{
		User::Panic(PolicyParserPanic, KErrCorrupt);
	}
	
	
	
	return retVal;
}
	
// -----------------------------------------------------------------------------
// CPolicyProcessor::Subjects()
// -----------------------------------------------------------------------------
//	
CPolicyProcessor::RAttributeList * CPolicyProcessor::Subjects()
{
	return &iSubjects;
}

// -----------------------------------------------------------------------------
// CPolicyProcessor::Actions()
// -----------------------------------------------------------------------------
//
CPolicyProcessor::RAttributeList * CPolicyProcessor::Actions()
{
	return &iActions;
}

// -----------------------------------------------------------------------------
// CPolicyProcessor::Resources()
// -----------------------------------------------------------------------------
//
CPolicyProcessor::RAttributeList * CPolicyProcessor::Resources()
{
	return &iResources;
}

// -----------------------------------------------------------------------------
// CPolicyProcessor::Environments()
// -----------------------------------------------------------------------------
//
CPolicyProcessor::RAttributeList * CPolicyProcessor::Environments()		
{
	return &iEnvironments;
}

// -----------------------------------------------------------------------------
// CPolicyProcessor::SetTargetElement()
// -----------------------------------------------------------------------------
//

void CPolicyProcessor::SetTargetElement( CElementBase * aElement)
{
	iManagementTargetPolicy = aElement;
}

// -----------------------------------------------------------------------------
// CPolicyProcessor::SetSessionTrust()
// -----------------------------------------------------------------------------
//

void CPolicyProcessor::SetSessionTrust( CTrustedSession * aTrustedSession)
{
	iTrustedSession = aTrustedSession;
}


// -----------------------------------------------------------------------------
// CPolicyProcessor::MakeBooleanResponseL()
// -----------------------------------------------------------------------------
//


void CPolicyProcessor::MakeBooleanResponseL( const TBool aValue, CAttributeValue* aResponseElement )
{
	if ( aValue )
	{
		RDEBUG("PolicyEngineServer: Boolean response TRUE");
		aResponseElement->SetDataL( PolicyLanguage::NativeLanguage::AttributeValues::BooleanTrue, 
								   PolicyLanguage::NativeLanguage::AttributeValues::BooleanDataType);
	}
	else
	{
		RDEBUG("PolicyEngineServer: Boolean response FALSE");
		aResponseElement->SetDataL( PolicyLanguage::NativeLanguage::AttributeValues::BooleanFalse, 
								   PolicyLanguage::NativeLanguage::AttributeValues::BooleanDataType);
	}	
}


// -----------------------------------------------------------------------------
// CPolicyProcessor::CorporateUserAcceptFunctionL()
// -----------------------------------------------------------------------------
//

void CPolicyProcessor::CorporateUserAcceptFunctionL( const RParameterList& aParams, CAttributeValue* aResponseElement )
{
	RDEBUG("PolicyEngineServer: CPolicyProcessor::CorporateUserAcceptFunctionL");
	
	if ( !aParams.Count())
	{
		HandleErrorL( RequestErrors::FunctionIsNotAllowed);
	}

	//Get trusted subject
	CAttributeValue * attributeValue = aParams[0];
	TDesC8& trustedSubject = attributeValue->Data()->Value();

	//resolve name and fingerprint
	const TDesC8& name = iTrustedSession->CommonNameForSubjectL( trustedSubject, iUseEditedElements);
	const TDesC8& fingerPrint = iTrustedSession->FingerPrintForSubjectL( trustedSubject, iUseEditedElements);

	TUserResponse response = EUserDeny;

	if ( name.Length() && fingerPrint.Length())
	{
		//create notifier
		RNotifier notifier;
		CleanupClosePushL( notifier);
		User::LeaveIfError( notifier.Connect() );
		
		//create parameter descriptor
		TBuf8<100> responseBuf;
		HBufC8 * data = HBufC8::NewLC( name.Length() + fingerPrint.Length() + 1);
		TPtr8 ptr = data->Des();
		ptr.Append(name);
		ptr.Append(KDelimeterChar);
		ptr.Append(fingerPrint.Left(4));

		//create CAsyncHandler to Auto start/stop CActiveScheduler
		CASyncHandler * async = CASyncHandler::NewLC();
		notifier.StartNotifierAndGetResponse( async->GetRequestStatus(), KUidPolicyEngineUi, ptr, responseBuf);
		
		//Start CActiveScheduler and execute stop when request is completed
		async->WaitForRequest();
		CPolicyEngineServer::SetActiveSubSession( this);	

		
		if ( async->GetRequestStatus() > 0) //request pending...
		{
			notifier.CancelNotifier( KUidPolicyEngineUi);	
		}
		else
		{
			//Check response
			if ( responseBuf == KUserAcceptMark)
			{
				RDEBUG("PolicyEngineServer: CPolicyProcessor user accept corporate policy!");
				response = EUserAccept;
			}
		}
		
		CleanupStack::PopAndDestroy( 3, &notifier);	//notifier, data, CASyncHandler
		
		
	}

	MakeBooleanResponseL( response == EUserAccept, aResponseElement);
}

// -----------------------------------------------------------------------------
// CPolicyProcessor::OrFunctionL()
// -----------------------------------------------------------------------------
//
void CPolicyProcessor::OrFunctionL( const RParameterList& aParams, CAttributeValue* aResponseElement )
{
	TBool valueTrue = EFalse;
	for ( TInt i(0); i < aParams.Count(); i++)
	{
		CAttributeValue * attribute = aParams[i];
		__ASSERT_ALWAYS ( attribute, User::Panic(PolicyParserPanic, KErrCorrupt));

	
		if ( attribute->Data()->DataType(ENative) != PolicyLanguage::NativeLanguage::AttributeValues::BooleanDataType)
		{
			HandleErrorL( ParserErrors::IncombatibleDataType);	
		}
		
		if ( attribute->Data()->Value() == PolicyLanguage::NativeLanguage::AttributeValues::BooleanTrue )
		{
			valueTrue = ETrue;
			break;
		} 	
	}
	
	MakeBooleanResponseL( valueTrue, aResponseElement);
}

// -----------------------------------------------------------------------------
// CPolicyProcessor::EqualFunctionsL()
// -----------------------------------------------------------------------------
//
void CPolicyProcessor::EqualFunctionsL( const RParameterList& aParams, CAttributeValue* aResponseElement )
	{
	TInt count = aParams.Count();
	if( count <= 1 )
		{
		MakeBooleanResponseL( EFalse, aResponseElement );
		}
	else
		{
		__ASSERT_ALWAYS ( aParams.Count() == 2, User::Panic(PolicyParserPanic, KErrCorrupt));

		CAttributeValue * value1 = aParams[0];
		CAttributeValue * value2 = aParams[1];
		__ASSERT_ALWAYS ( value1, User::Panic(PolicyParserPanic, KErrCorrupt));
		__ASSERT_ALWAYS ( value2, User::Panic(PolicyParserPanic, KErrCorrupt));

		TInt value = value1->Data()->Compare( value2->Data());

		MakeBooleanResponseL( 0 == value, aResponseElement );
		}
	}



// -----------------------------------------------------------------------------
// CPolicyProcessor::AndFunctionL()
// -----------------------------------------------------------------------------
//
void CPolicyProcessor::AndFunctionL( const RParameterList& aParams, CAttributeValue* aResponseElement )
{
	TBool valueTrue = ETrue;
	for ( TInt i(0); i < aParams.Count(); i++)
	{
		CAttributeValue * attribute = aParams[i];
		__ASSERT_ALWAYS ( attribute, User::Panic(PolicyParserPanic, KErrCorrupt));
	
		if ( attribute->Data()->DataType(ENative) != PolicyLanguage::NativeLanguage::AttributeValues::BooleanDataType)
		{
			HandleErrorL( ParserErrors::IncombatibleDataType);	
		}
		
		if ( attribute->Data()->Value() != PolicyLanguage::NativeLanguage::AttributeValues::BooleanTrue )
		{
			valueTrue = EFalse;
			break;
		} 	
	}
	
	MakeBooleanResponseL( valueTrue, aResponseElement);

}

// -----------------------------------------------------------------------------
// CPolicyProcessor::NotFunctionL()
// -----------------------------------------------------------------------------
//
void CPolicyProcessor::NotFunctionL( const RParameterList& aParams, CAttributeValue* aResponseElement )
{
	if ( aParams.Count() != 1)
	{
		HandleErrorL( ParserErrors::IncombatibleParameters);	
	}
	
	CAttributeValue * attribute = aParams[0];
	__ASSERT_ALWAYS ( attribute, User::Panic(PolicyParserPanic, KErrCorrupt));


	if ( attribute->Data()->DataType(ENative) != PolicyLanguage::NativeLanguage::AttributeValues::BooleanDataType)
	{
		HandleErrorL( ParserErrors::IncombatibleDataType);	
	}
		
	MakeBooleanResponseL( aParams[0]->Data()->Value() != PolicyLanguage::NativeLanguage::AttributeValues::BooleanTrue, aResponseElement);

}

// -----------------------------------------------------------------------------
// CPolicyProcessor::RuleTargetStructureFunctionL()
// -----------------------------------------------------------------------------
//
void CPolicyProcessor::RuleTargetStructureFunctionL( const RParameterList& aParams, CAttributeValue* aResponseElement )
{
	//We expect that param count is 3n...
	if ( (aParams.Count() % 3) != 0)
	{
		HandleErrorL( ParserErrors::IncombatibleParameters);
	}
	
	if ( !iManagementTargetPolicy )
	{
		HandleErrorL( RequestErrors::FunctionIsNotAllowed);
	}

	TBool validRule = EFalse;	
	
	//this operation is only valid for rules
	if ( iManagementTargetPolicy->ElementType() == ERule )
	{
		CRule * rule = (CRule*)iManagementTargetPolicy;
		CTarget * target = rule->GetTarget();
		__ASSERT_ALWAYS ( target, User::Panic(PolicyParserPanic, KErrCorrupt));
	
		for ( TInt i = 0; i < aParams.Count(); i++)
		{
			if ( aParams[i]->Data()->DataType(ENative) != PolicyLanguage::NativeLanguage::AttributeValues::StringDataType)
			{
				HandleErrorL( ParserErrors::IncombatibleDataType);	
			}
		}
	
		//get subject/actions/resource/environment and compare their match content to parameters
		RMatchObjectArray container;
		CleanupClosePushL( container);
		target->GetMatchContainersL( container);

		validRule = ETrue;

		//read one subject/action/resource/environment group at time
		for ( TInt i = 0; i < aParams.Count() / 3 && validRule; i++)
		{
			validRule = EFalse;
			for ( TInt j = 0; j < container.Count() && !validRule; j++)
			{
				validRule = container[j]->CheckMatchObject( aParams[ i*3]->Data()->Value(),
								  							aParams[ (i*3)+1]->Data()->Value(),
														 	aParams[ (i*3)+2]->Data()->Value());
			}
		}		
	
		CleanupStack::PopAndDestroy( &container);
	}
	
	MakeBooleanResponseL( validRule, aResponseElement);
}

// -----------------------------------------------------------------------------
// CPolicyProcessor::CertificateForSessionL()
// -----------------------------------------------------------------------------
//

void CPolicyProcessor::CertificateForSessionL( CAttributeValue* aResponseElement )
{

	if ( !iTrustedSession)
	{
		HandleErrorL( RequestErrors::FunctionIsNotAllowed);
	}
	
	MakeBooleanResponseL( iTrustedSession->CertificatedSession(), aResponseElement);
}



// -----------------------------------------------------------------------------
// TCombiningAlgorith::TCombiningAlgorith()
// -----------------------------------------------------------------------------
//
TCombiningAlgorith::TCombiningAlgorith( const TDesC8 &aAlgorithId)
{
	//set properties 
	iResultReady = iAtLeastOneError = iPotentialDeny = EFalse;
	iPotentialPermit = iAtLeastOneDeny = iAtLeastOnePermit = EFalse;

	//set algorithm specific properties....
	if ( aAlgorithId == PolicyLanguage::NativeLanguage::CombiningAlgorithms::RuleDenyOverrides)	
	{
		iAlgorithm = ERuleDenyOverrides;
		iCurrentResponse = ENotApplicable;
	}
	else 
	if ( aAlgorithId == PolicyLanguage::NativeLanguage::CombiningAlgorithms::PolicyDenyOverrides)	
	{
		iAlgorithm = EPolicyDenyOverrides;
		iCurrentResponse = ENotApplicable;
	}
	else
	if ( aAlgorithId == PolicyLanguage::NativeLanguage::CombiningAlgorithms::RulePermitOverrides)	
	{
		iAlgorithm = ERulePermitOverrides;
		iCurrentResponse = ENotApplicable;
	}
	else 
	if ( aAlgorithId == PolicyLanguage::NativeLanguage::CombiningAlgorithms::PolicyPermitOverrides)	
	{
		iAlgorithm = EPolicyPermitOverrides;
		iCurrentResponse = ENotApplicable;
	}
	
	
	iResultReady = EFalse;
}

// -----------------------------------------------------------------------------
// TCombiningAlgorith::AddInput()
// -----------------------------------------------------------------------------
//
		
TBool TCombiningAlgorith::AddInput( const TMatchResponse& aInput, const TMatchResponse& aEffect)
{
	//for rule algorithms, also "aEffect" has effect to result

	//if result is ready return it...	
	if ( iResultReady )
	{
		return iResultReady;	
	}

	//select algorithm...
	switch ( iAlgorithm)
	{
		case ERuleDenyOverrides:
		{
			//algorithm specific handling.... 
			if ( aInput	== EDeny )
			{
				iCurrentResponse = EDeny;
				iResultReady = ETrue;		
			}
			else if ( aInput == EIndeterminate)
			{
				iAtLeastOneError = ETrue;
				if ( aEffect == EDeny )
				{
					iPotentialDeny = ETrue;
				}
			}
			else if ( aInput == EPermit)
			{
				iAtLeastOnePermit = ETrue;
			}
		}
		break;
		case ERulePermitOverrides:
		{
			//algorithm specific handling.... 
			if ( aInput	== EPermit )
			{
				iCurrentResponse = EPermit;
				iResultReady = ETrue;		
			}
			else if ( aInput == EIndeterminate)
			{
				iAtLeastOneError = ETrue;
				if ( aEffect == EPermit )
				{
					iPotentialPermit = ETrue;
				}
			}
			else if ( aInput == EDeny)
			{
				iAtLeastOneDeny = ETrue;
			}
		}
	}
	
	return iResultReady;
}

// -----------------------------------------------------------------------------
// TCombiningAlgorith::AddInput()
// -----------------------------------------------------------------------------
//

TBool TCombiningAlgorith::AddInput( const TMatchResponse& aInput)
{
	//for policy algorithms

	//if result is ready return it...	
	if ( iResultReady )
	{
		return iResultReady;	
	}

	switch ( iAlgorithm)
	{
		case EPolicyDenyOverrides:
		{
			//algorithm specific handling.... 
			if ( aInput	== EDeny || aInput == EIndeterminate)
			{
				iCurrentResponse = EDeny;
				iResultReady = ETrue;		
			}
			else if ( aInput == EPermit)
			{
				iAtLeastOnePermit = ETrue;
			}
		}
		break;
		case EPolicyPermitOverrides:
		{
			//algorithm specific handling.... 
			if ( aInput	== EPermit ) 
			{
				iCurrentResponse = EPermit;
				iResultReady = ETrue;		
			}
			else if ( aInput == EDeny)
			{
				iAtLeastOneDeny = ETrue;
			}
			else if ( aInput == EIndeterminate)
			{
				iAtLeastOneError = ETrue;
			}
		}
		break;
	}
	
	return iResultReady;
}

// -----------------------------------------------------------------------------
// TCombiningAlgorith::ResultReady()
// -----------------------------------------------------------------------------
//

TBool TCombiningAlgorith::ResultReady()
{
	return iResultReady;
}

// -----------------------------------------------------------------------------
// TCombiningAlgorith::Result()
// -----------------------------------------------------------------------------
//

TMatchResponse TCombiningAlgorith::Result()
{
	//default...
	TMatchResponse response( ENotApplicable);
	
	if ( !iResultReady)
	{
		//if result is not ready
		switch ( iAlgorithm)
		{
			case ERuleDenyOverrides:
			{	
				//algorithm specific handling.... 
				if ( iPotentialDeny ) response = EIndeterminate; else
				if ( iAtLeastOnePermit ) response = EPermit; else
				if ( iAtLeastOneError ) response = EIndeterminate;
			}
			break;
			case ERulePermitOverrides:
			{
				//algorithm specific handling.... 
				if ( iPotentialPermit ) response = EIndeterminate; else
				if ( iAtLeastOneDeny ) response = EDeny; else
				if ( iAtLeastOneError ) response = EIndeterminate;
			}
			break;
			case EPolicyDenyOverrides:
			{	
				//algorithm specific handling.... 
				if ( iAtLeastOnePermit ) response = EPermit;
			}
			break;
			case EPolicyPermitOverrides:
			{
				//algorithm specific handling.... 
				if ( iAtLeastOneDeny ) response = EDeny; else
				if ( iAtLeastOneError ) response = EIndeterminate;
			}
			break;
		}
		
		iCurrentResponse = response;
		iResultReady = ETrue;
		
	}
	else
	{
		//and if result is ready return it
		response = iCurrentResponse;
	}
	
#ifdef _DEBUG
	switch ( response)
	{
		case EDeny:
			RDEBUG("PolicyEngineServer: DENY");
		break;
		case EPermit:
			RDEBUG("PolicyEngineServer: PERMIT");
		break;
		case EIndeterminate:
			RDEBUG("PolicyEngineServer: INDETERMINATE");
		break;
		default:
			RDEBUG("PolicyEngineServer: NOT APPLICABLE");
		break;
	};
#endif	
	
	return response;
}
	
