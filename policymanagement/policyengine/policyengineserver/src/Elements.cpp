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


// INCLUDES

#include "elements.h"
#include "PolicyParser.h"
#include "XACMLconstants.h"
#include "DataTypes.h"
#include "PolicyProcessor.h"
#include "PolicyStorage.h"
#include "ErrorCodes.h"
#include "debug.h"



// -----------------------------------------------------------------------------
// RContentBuilder::RContentBuilder()
// -----------------------------------------------------------------------------
//

RContentBuilder::RContentBuilder()
	: RPointerArray<HBufC8>(), iSize(0)
	{	
	}

// -----------------------------------------------------------------------------
// RContentBuilder::AddContentL()
// -----------------------------------------------------------------------------
//

void RContentBuilder::AddContentL( HBufC8 * aBuffer)
	{
	//Append buffer to pointer array
	CleanupStack::PushL( aBuffer);
	User::LeaveIfError( Append( aBuffer));
	CleanupStack::Pop( aBuffer);
	
	//calculate new total size
	iSize += aBuffer->Length();
	}

// -----------------------------------------------------------------------------
// RContentBuilder::ContentL()
// -----------------------------------------------------------------------------
//

HBufC8 * RContentBuilder::ContentL()
	{
	//create buffer for new content
	HBufC8 * buffer = HBufC8::NewL( iSize);		
	TPtr8 ptr = buffer->Des();
	
	//and append content of pointer array to buffer (step by step)
	for ( TInt i(0); i < Count(); i++)
		{
		ptr.Append( *operator[]( i));
		}
	
	//return buffer
	return buffer;
	}

// -----------------------------------------------------------------------------
// RContentBuilder::ContentLC()
// -----------------------------------------------------------------------------
//

HBufC8 * RContentBuilder::ContentLC()
	{
	//create buffer and push it to cleanupstack
	HBufC8 * buffer = ContentL();
	CleanupStack::PushL( buffer);
	
	//return buffer
	return buffer;
	}

// -----------------------------------------------------------------------------
// RContentBuilder::Close()
// -----------------------------------------------------------------------------
//
		
void RContentBuilder::Close()
	{
	//destroy pointer array content
	ResetAndDestroy();
	
	//and close array (parent class)
	RPointerArray<HBufC8>::Close();
	}



// -----------------------------------------------------------------------------
// CApply::CApply()
// -----------------------------------------------------------------------------
//

CApply::CApply()
	{	
	iElementType = EApply;
	iElementName.Set( PolicyLanguage::NativeLanguage::Expressions::Apply);

	iSaveType = ESubElement;	
	}

// -----------------------------------------------------------------------------
// CApply::~CApply()
// -----------------------------------------------------------------------------
//

CApply::~CApply()
{
	delete iResponseValue;
	delete iFunctionId;
}

// -----------------------------------------------------------------------------
// CApply::NewL()
// -----------------------------------------------------------------------------
//
		
CApply * CApply::NewL()
{
	CApply * self = new (ELeave) CApply();
	
	CleanupStack::PushL( self);
	self->ConstructL();
	CleanupStack::Pop ( self);
	
	return self;
}

// -----------------------------------------------------------------------------
// CApply::ConstructL()
// -----------------------------------------------------------------------------
//

void CApply::ConstructL()
{
	iResponseValue = CAttributeValue::NewL();
}

// -----------------------------------------------------------------------------
// CApply::IdentificateType()
// -----------------------------------------------------------------------------
//

TBool CApply::IdentificateType( const TDesC8& aElementName)
{
	return ( aElementName == PolicyLanguage::NativeLanguage::Expressions::Apply);			
}

// -----------------------------------------------------------------------------
// CApply::DecodeElementL
// -----------------------------------------------------------------------------
//

HBufC8 * CApply::DecodeElementL( const  TLanguageSelector &aLanguage, const TDecodeMode &aMode)
{
	RDEBUG("PolicyEngineServer: Decode Apply element");

	__ASSERT_ALWAYS ( iFunctionId, User::Panic(PolicyParserPanic, KErrCorrupt));
	
	//reserve all...
	CheckAllElementsL();
	TElementReserver reserver( Container());
	
	TInt size = 9;

	size += 2 * PolicyLanguage::Expressions::Apply[ aLanguage].Length();

	size += PolicyLanguage::Expressions::FunctionId[ aLanguage].Length();
	
	const TDesC8& functionId = SelectCorrectValue( aLanguage, *iFunctionId);
	size += functionId.Length();

	RContentBuilder builder;
	CleanupClosePushL( builder);
	
	for ( TInt i(0); i < ElementCount(); i++)
	{
		CElementBase * element = Element(i);

		if ( element->ElementType() == EApply )
		{
			builder.AddContentL( DecodeElementOrIdL( aLanguage, element, aMode));
		}
		else
		{
			builder.AddContentL( DecodeElementOrIdL( aLanguage, element, EFullMode));
		}
		
	}	

	HBufC8* applies	= builder.ContentL();
	CleanupStack::PopAndDestroy( &builder);
	CleanupStack::PushL( applies);

	if (applies) size += applies->Length();
	
	HBufC8 * hbuf = HBufC8::NewLC( size);
	TPtr8 ptr = hbuf->Des();
	
	ptr.Append('<');
	ptr.Append(PolicyLanguage::Expressions::Apply[ aLanguage]);
	ptr.Append(' ');
	ptr.Append(PolicyLanguage::Expressions::FunctionId[ aLanguage]);
	ptr.Append( _L8("=\""));
	ptr.Append( functionId);
	ptr.Append( _L8("\">"));
	if (applies) ptr.Append(*applies);	
	ptr.Append(_L8("</"));
	ptr.Append(PolicyLanguage::Expressions::Apply[ aLanguage]);
	ptr.Append('>');											   

	CleanupStack::Pop( hbuf);
	CleanupStack::PopAndDestroy( applies);
	
	reserver.Release();
	

	return hbuf;
}

// -----------------------------------------------------------------------------
// CApply::AddAttributeL
// -----------------------------------------------------------------------------
//

void CApply::AddAttributeL( CPolicyParser *aParser, const TDesC8& aName, const TDesC8& aValue)
{
	if ( aName == PolicyLanguage::NativeLanguage::Expressions::FunctionId)
	{
		aParser->HandleErrorL( iFunctionId , ParserErrors::UnexpectedAttribute, aName);	
		iFunctionId = aValue.AllocL();
		aParser->CheckFunctionIdL( EApply, *iFunctionId);
	}
	else
	{
		aParser->HandleErrorL( ParserErrors::UnexpectedAttribute, aName );	
	}	
}

// -----------------------------------------------------------------------------
// CApply::AddElementL
// -----------------------------------------------------------------------------
//

void CApply::AddElementL( CPolicyParser *aParser, CElementBase * aElement)
{
	TNativeElementTypes type = aElement->ElementType();
	
	if ( type == EApply ||
		 type == EAttributeValue ||
		 type == ESubjectAttributeDesignator ||  
		 type == EEnvironmentAttributeDesignator ||
		 type == EResourceAttributeDesignator ||
		 type == EActionAttributeDesignator ) 
	{
		AddToElementListL( aElement, ETrue);
	}
	else
	{	
		aParser->HandleErrorL( ParserErrors::UnexpectedElement, aParser->ActiveElementName());
	}	
}
	
// -----------------------------------------------------------------------------
// CApply::AddIdElementL
// -----------------------------------------------------------------------------
//

void CApply::AddIdElementL( CElementBase * aElement)
{
	__ASSERT_ALWAYS ( aElement, User::Panic(PolicyParserPanic, KErrCorrupt));
	
	AddToElementListL( aElement, EFalse);	
}

// -----------------------------------------------------------------------------
// CApply::ValidElement
// -----------------------------------------------------------------------------
//

TBool CApply::ValidElement()
{
	TBool returnValue = EFalse;

	//function id is needed...
	if ( iFunctionId)
	{
		returnValue = FunctionHelper::IsAcceptableParamsForFunction( *iFunctionId, Container());
	}
	
	return returnValue;
}

// -----------------------------------------------------------------------------
// CApply::ApplyValueL
// -----------------------------------------------------------------------------
//

CAttributeValue * CApply::ApplyValueL( CPolicyProcessor* aPolicyProcessor)
{
#ifndef __POLICY_BASE_CREATOR_TOOL

	//reserve elements
	CheckAllElementsL();
	TElementReserver reserver( Container());

	//create parameter list add it cleanup stack
	RParameterList params;
	CleanupClosePushL( params);
	
	//search attributedesignators and apply elements
	for (TInt i(0); i < ElementCount(); i++)
	{
		CElementBase * element = Element( i);
		
		switch ( element->ElementType())
		{
			case EApply:
			{
				//get parameter value from apply-element and append it parameter list
				params.AppendL( ((CApply*)element)->ApplyValueL( aPolicyProcessor));
			}
			break;
			case ESubjectAttributeDesignator:
			case EEnvironmentAttributeDesignator:
			case EResourceAttributeDesignator:
			case EActionAttributeDesignator:
			{
				//get parameter value from request context (via attribute designators)			
				CAttribute* attribute = ((CAttributeDesignator*) element)->GetAttributeFromRequestContext( aPolicyProcessor);
				
				if( attribute )
					{
					params.AppendL( attribute->AttributeValue());
					}
							
				if ( !attribute) 
					{
					RDEBUG("PolicyEngineServer: PolicyProcessor: attribute not found");	

				//	aPolicyProcessor->HandleErrorL( RequestErrors::MissingAttribute );
					}
			
				//append value to paramer list
				//params.AppendL( attribute->AttributeValue());
			}
			break;
			case EAttributeValue:
			{
				//attribute value is valid parameter type
				params.AppendL( (CAttributeValue*)element);
			}
			break;
			default:
				User::Panic( Panics::PolicyExecutionPanic, KErrAbort);
			break;
		}
	}
	
	//execute function
	aPolicyProcessor->FunctionL( *iFunctionId, params, iResponseValue );
	
	CleanupStack::PopAndDestroy( &params);	//params.Close()
	reserver.Release();
	
	return iResponseValue;
	
#endif //#ifndef __POLICY_BASE_CREATOR_TOOL
}



// -----------------------------------------------------------------------------
// CApply::ReturnDataType
// -----------------------------------------------------------------------------
//
const TPtrC8 CApply::ReturnDataType()
{
	__ASSERT_ALWAYS ( iFunctionId, User::Panic(PolicyParserPanic, KErrCorrupt));

	using namespace PolicyLanguage::NativeLanguage::Functions;

	TPtrC8 ptr;

	//set return data tpye
	if ( *iFunctionId == FunctionStringEqualId ||
		 *iFunctionId == FunctionBooleanEqualId ||
		 
		 *iFunctionId == FunctionOr ||
		 *iFunctionId == FunctionAnd ||
		 *iFunctionId == FunctionNot ||

		 *iFunctionId == FunctionCertificatedSession ||
		 *iFunctionId == FunctionUserAcceptCorpPolicy ||
		 *iFunctionId == FunctionRuleTargetStructure )
	{
		ptr.Set( PolicyLanguage::NativeLanguage::AttributeValues::BooleanDataType);
	}
	
	return ptr;
}

// -----------------------------------------------------------------------------
// CCondition::CCondition
// -----------------------------------------------------------------------------
//

CCondition::CCondition()
{
	iElementType = ECondition;
	iElementName.Set( PolicyLanguage::NativeLanguage::Expressions::Condition);

	iSaveType = ESubElement;
}

// -----------------------------------------------------------------------------
// CCondition::~CCondition
// -----------------------------------------------------------------------------
//
CCondition::~CCondition()
{
}
		
// -----------------------------------------------------------------------------
// CCondition::NewL
// -----------------------------------------------------------------------------
//
CCondition * CCondition::NewL()
{
	return new (ELeave) CCondition();
}

// -----------------------------------------------------------------------------
// CCondition::IdentificateType
// -----------------------------------------------------------------------------
//
TBool CCondition::IdentificateType( const TDesC8& aElementName)
{
	return ( aElementName == PolicyLanguage::NativeLanguage::Expressions::Condition);			
}

// -----------------------------------------------------------------------------
// CCondition::DecodeElementL
// -----------------------------------------------------------------------------
//

HBufC8 * CCondition::DecodeElementL( const  TLanguageSelector &aLanguage, const TDecodeMode &aMode)
{
	RDEBUG("PolicyEngineServer: Decode Condition element");


	__ASSERT_ALWAYS ( iExpression, User::Panic(PolicyParserPanic, KErrCorrupt));

	CheckAllElementsL();
	TElementReserver reserver( Container());

	TInt size(5);
	TPtrC8 conditionPtr = PolicyLanguage::Expressions::Condition[ aLanguage];
	size += 2 * conditionPtr.Length();
	
	HBufC8 * expression = DecodeElementOrIdL( aLanguage, iExpression, aMode);
	CleanupStack::PushL( expression);
	
	size += expression->Length();
	
	HBufC8 * hbuf = HBufC8::NewLC( size);
	TPtr8 ptr = hbuf->Des();
	
	ptr.Append( '<');
	ptr.Append( conditionPtr);
	ptr.Append( '>');
	ptr.Append( *expression);
	ptr.Append(_L8("</"));
	ptr.Append( conditionPtr);
	ptr.Append( '>');
	
	CleanupStack::Pop( hbuf);
	CleanupStack::PopAndDestroy( expression);
	
	reserver.Release();
	
	return hbuf;
}

// -----------------------------------------------------------------------------
// CCondition::AddElementL
// -----------------------------------------------------------------------------
//
void CCondition::AddElementL( CPolicyParser *aParser, CElementBase * aElement)
{
	if ( aElement->ElementType() == EApply ) 
	{
		aParser->HandleErrorL( iExpression, ParserErrors::UnexpectedElement, aParser->ActiveElementName());	
		AddToElementListL( aElement, ETrue);
		iExpression = aElement;
	}
	else
	{	
		aParser->HandleErrorL( ParserErrors::UnexpectedElement, aParser->ActiveElementName());
	}	
}

// -----------------------------------------------------------------------------
// CCondition::AddIdElementL
// -----------------------------------------------------------------------------
//	
void CCondition::AddIdElementL( CElementBase * aElement)
{
	__ASSERT_ALWAYS ( aElement, User::Panic(PolicyParserPanic, KErrCorrupt));
	
	iExpression = aElement;
	AddToElementListL( aElement, EFalse);	
}

// -----------------------------------------------------------------------------
// CCondition::ValidElement
// -----------------------------------------------------------------------------
//	

TBool CCondition::ValidElement()
{
	TBool returnValue = EFalse;

	if ( iExpression)
	{
		TPtrC8 dataType = ((CApply*) iExpression)->ReturnDataType();
		
		if ( dataType == PolicyLanguage::NativeLanguage::AttributeValues::BooleanDataType)
		{
			returnValue = ETrue;
		}		
	}

	return returnValue;
}


// -----------------------------------------------------------------------------
// CCondition::ConditionValuement
// -----------------------------------------------------------------------------
//¨

#ifndef __POLICY_BASE_CREATOR_TOOL

TMatchResponse CCondition::ConditionValueL( CPolicyProcessor* aPolicyProcessor)
{
	RDEBUG("PolicyEngineServer: Evaluate condition value - start");

	__ASSERT_ALWAYS ( iExpression, User::Panic(PolicyParserPanic, KErrCorrupt));

	//default response
	TMatchResponse response = EIndeterminate;
	
	//check element
	TElementReserver reserver( iExpression);
	CPolicyStorage::PolicyStorage()->CheckElementL( iExpression);
	
	//apply-value from apply element
	CAttributeValue * element = iExpression->ApplyValueL( aPolicyProcessor);
	
	//check that respose element is attribute...
	if ( element->ElementType() == EAttributeValue )
	{
		CDataType * data = element->Data();
		
		//..attribute type should boolean
		if ( data->DataType( ENative) == PolicyLanguage::NativeLanguage::AttributeValues::BooleanDataType)
		{
			if ( data->Value() == PolicyLanguage::NativeLanguage::AttributeValues::BooleanTrue)
			{
				RDEBUG("PolicyEngineServer: Condition value is TRUE");
				response = EMatch;
			}
			else
			{
				RDEBUG("PolicyEngineServer: Condition value is FALSE");
				response = EUnMatch;
			}
		}
	}
	
	reserver.Release();
	
	//response...
	RDEBUG("PolicyEngineServer: Evaluate condition value - end");
	return response;
}

#endif // __POLICY_BASE_CREATOR_TOOL

// -----------------------------------------------------------------------------
// CPolicySet::CPolicySet
// -----------------------------------------------------------------------------
//
CPolicySet::CPolicySet()
{
	iElementType = EPolicySet;
	iElementName.Set( PolicyLanguage::NativeLanguage::PolicySet::PolicySet);
}

// -----------------------------------------------------------------------------
// CPolicySet::~CPolicySet
// -----------------------------------------------------------------------------
//
CPolicySet::~CPolicySet()
{
	delete iPolicySetId;
	delete iPolicyCompiningAlg;
}

// -----------------------------------------------------------------------------
// CPolicySet::NewL
// -----------------------------------------------------------------------------
//
CPolicySet * CPolicySet::NewL()
{
	return new (ELeave) CPolicySet();
}

// -----------------------------------------------------------------------------
// CPolicySet::IdentificateType
// -----------------------------------------------------------------------------
//
TBool CPolicySet::IdentificateType( const TDesC8& aElementName)
{
	return ( aElementName == PolicyLanguage::NativeLanguage::PolicySet::PolicySet);			
}

// -----------------------------------------------------------------------------
// CPolicySet::AddAttributeL
// -----------------------------------------------------------------------------
//
void CPolicySet::AddAttributeL( CPolicyParser *aParser, const TDesC8& aName, const TDesC8& aValue)
{
	if ( aName == PolicyLanguage::NativeLanguage::PolicySet::PolicySetId)
	{
		aParser->HandleErrorL( iPolicySetId , ParserErrors::UnexpectedAttribute, aName);	
		iPolicySetId = aValue.AllocL();
		iExternalId = iPolicySetId;
		aParser->CheckExternalIdL( iExternalId);
	}
	else	
	if ( aName == PolicyLanguage::NativeLanguage::PolicySet::PolicyCombiningAlgId)
	{
		aParser->HandleErrorL( iPolicyCompiningAlg, ParserErrors::UnexpectedAttribute, aName);	
		aParser->CheckCompiningAlgorithmsL( EPolicySet, aValue);
		iPolicyCompiningAlg = aValue.AllocL();
	}
	else
	{
		aParser->HandleErrorL( ParserErrors::UnexpectedAttribute, aName );	
	}
}

// -----------------------------------------------------------------------------
// CPolicySet::AddElementL
// -----------------------------------------------------------------------------
//
void CPolicySet::AddElementL( CPolicyParser *aParser, CElementBase * aElement)
{
	if ( aElement->ElementType() == ETarget ) 
	{
		aParser->HandleErrorL( iTarget, ParserErrors::UnexpectedElement, aParser->ActiveElementName());	
		AddToElementListL( aElement, ETrue);
		iTarget = aElement;
	}
	else if ( aElement->ElementType() == EPolicy ||  aElement->ElementType() == EPolicySet) 
	{
		AddToElementListL( aElement, ETrue);
	}
	else
	if ( aElement->ElementType() == EDescription )
	{
		aParser->HandleErrorL( iDescription, ParserErrors::UnexpectedElement, aParser->ActiveElementName());	
		AddToElementListL( aElement, ETrue);
		iDescription = aElement;
	}
	else
	{	
		aParser->HandleErrorL( ParserErrors::UnexpectedElement, aParser->ActiveElementName());
	}
};

// -----------------------------------------------------------------------------
// CPolicySet::AddIdElementL
// -----------------------------------------------------------------------------
//

void CPolicySet::AddIdElementL( CElementBase * aElement)
{
	__ASSERT_ALWAYS ( aElement, User::Panic(PolicyParserPanic, KErrCorrupt));
	
	if ( aElement->ElementType() == ETarget )
	{
		iTarget = aElement;
	}
	else
	if ( aElement->ElementType() == EDescription )
	{
		iDescription = aElement;
	}
	
	AddToElementListL( aElement, EFalse);
}

// -----------------------------------------------------------------------------
// CPolicySet::DecodeElementL
// -----------------------------------------------------------------------------
//

HBufC8 * CPolicySet::DecodeElementL( const  TLanguageSelector &aLanguage, const TDecodeMode &aMode)
{
	RDEBUG("PolicyEngineServer: Decode PolicySet element");

	__ASSERT_ALWAYS ( iPolicySetId, User::Panic(PolicyParserPanic, KErrCorrupt));
	__ASSERT_ALWAYS ( iPolicyCompiningAlg, User::Panic(PolicyParserPanic, KErrCorrupt));
	
	CheckAllElementsL();
	TElementReserver reserver( Container());
	
	TInt size = 16;

	size += 2 * PolicyLanguage::PolicySet::PolicySet[ aLanguage].Length();

	size += PolicyLanguage::PolicySet::PolicySetId[ aLanguage].Length();
	size += iPolicySetId->Length();

	size += PolicyLanguage::PolicySet::PolicyCombiningAlgId[ aLanguage].Length();
	
	
	const TDesC8& policyCompiningAlg = SelectCorrectValue( aLanguage, *iPolicyCompiningAlg);
	size += policyCompiningAlg.Length();
	
	RContentBuilder builder;
	CleanupClosePushL( builder);
	
	for ( TInt i(0); i < ElementCount(); i++)
	{
		CElementBase * element = Element(i);
		builder.AddContentL( DecodeElementOrIdL( aLanguage, element, aMode));
	}	

	HBufC8* rules = builder.ContentL();
	CleanupStack::PopAndDestroy( &builder);
	CleanupStack::PushL(rules);
	
	if (rules) size += rules->Length();
	
	HBufC8 * hbuf = HBufC8::NewLC( size);
	TPtr8 ptr = hbuf->Des();
	
	ptr.Append( '<');
	ptr.Append( PolicyLanguage::PolicySet::PolicySet[ aLanguage]);
	ptr.Append( ' ');
	ptr.Append( PolicyLanguage::PolicySet::PolicySetId[ aLanguage]);
	ptr.Append( _L8("=\""));
	ptr.Append( *iPolicySetId);
	ptr.Append( _L8("\" "));
	ptr.Append( PolicyLanguage::PolicySet::PolicyCombiningAlgId[ aLanguage]);
	ptr.Append( _L8("=\""));
	ptr.Append( policyCompiningAlg);
	ptr.Append( _L8("\">"));
	
	if (rules) 
	{
		ptr.Append(*rules);			
	}
	
	ptr.Append( _L8("</"));
	ptr.Append( PolicyLanguage::PolicySet::PolicySet[ aLanguage]);
	ptr.Append( '>');											   

	CleanupStack::Pop( hbuf);
	CleanupStack::PopAndDestroy( rules);
	
	reserver.Release();

	return hbuf;	
}

// -----------------------------------------------------------------------------
// CPolicySet::ValidElement
// -----------------------------------------------------------------------------
//

TBool CPolicySet::ValidElement()
{
	return iPolicySetId && iPolicyCompiningAlg;
}

// -----------------------------------------------------------------------------
// CPolicySet::MatchL
// -----------------------------------------------------------------------------
//


TMatchResponse CPolicySet::MatchL( CPolicyProcessor* aPolicyProcessor)
{
#ifndef __POLICY_BASE_CREATOR_TOOL

	CPolicyStorage::PolicyStorage()->CheckElementL( this);
	RDEBUG8_2("PolicyEngineServer: %S match evaluation started", ExternalId());

	//check policyset target
	if ( iTarget)
	{
		//reserve eleemnt
		TElementReserver targetReserver( iTarget);
		CPolicyStorage::PolicyStorage()->CheckElementL( iTarget);
		
		//and check target match
		TMatchResponse response = iTarget->MatchL( aPolicyProcessor);
		targetReserver.Release();

		if ( response == EUnMatch )
		{
			return ENotApplicable;
		}
	}

	__ASSERT_ALWAYS ( iPolicyCompiningAlg, User::Panic(PolicyParserPanic, KErrCorrupt));

	//set up combining algorithm
	TCombiningAlgorith combiner( *iPolicyCompiningAlg);

	//loop all policies and policysets and combine their results 
	for ( TInt i(0); i < ElementCount(); i++)
	{
		CElementBase * element = Element(i);		
		TElementReserver elementReserver( element);

		//select only policies and sets
		if ( element->ElementType() == EPolicy || element->ElementType() == EPolicySet)
		{
			CPolicyStorage::PolicyStorage()->CheckElementL( element );
			combiner.AddInput( element->MatchL( aPolicyProcessor));
		}
		
		elementReserver.Release();
	}	

	RDEBUG8_2("PolicyEngineServer: %S match evaluation finished", ExternalId());

	return combiner.Result();

#endif // __POLICY_BASE_CREATOR_TOOL
}


// -----------------------------------------------------------------------------
// CPolicy::CPolicy
// -----------------------------------------------------------------------------
//

CPolicy::CPolicy()
{
	iElementType = EPolicy;
	iElementName.Set( PolicyLanguage::NativeLanguage::Policy::Policy);
}

// -----------------------------------------------------------------------------
// CPolicy::~CPolicy
// -----------------------------------------------------------------------------
//

CPolicy::~CPolicy()
{
	delete iPolicyId;
	delete iRuleCompiningAlg;
}

// -----------------------------------------------------------------------------
// CPolicy::NewL
// -----------------------------------------------------------------------------
//

CPolicy * CPolicy::NewL()
{
	return new (ELeave) CPolicy();
}

// -----------------------------------------------------------------------------
// CPolicy::IdentificateType
// -----------------------------------------------------------------------------
//

TBool CPolicy::IdentificateType( const TDesC8& aElementName)
{
	return ( aElementName == PolicyLanguage::NativeLanguage::Policy::Policy);			
}

// -----------------------------------------------------------------------------
// CPolicy::AddAttributeL
// -----------------------------------------------------------------------------
//

void CPolicy::AddAttributeL( CPolicyParser *aParser, const TDesC8& aName, const TDesC8& aValue)
{
	if ( aName == PolicyLanguage::NativeLanguage::Policy::PolicyId)
	{
		aParser->HandleErrorL( iPolicyId , ParserErrors::UnexpectedAttribute, aName);	
		iPolicyId = aValue.AllocL();
		iExternalId = iPolicyId;
		aParser->CheckExternalIdL( iExternalId);
	}
	else	
	if ( aName == PolicyLanguage::NativeLanguage::Policy::RuleCombiningAlgId)
	{
		aParser->HandleErrorL( iRuleCompiningAlg, ParserErrors::UnexpectedAttribute, aName);	
		aParser->CheckCompiningAlgorithmsL( EPolicy, aValue);
		iRuleCompiningAlg = aValue.AllocL();
	}
	else
	{
		aParser->HandleErrorL( ParserErrors::UnexpectedAttribute, aName);	
	}
}

// -----------------------------------------------------------------------------
// CPolicy::AddElementL
// -----------------------------------------------------------------------------
//
void CPolicy::AddElementL( CPolicyParser *aParser, CElementBase * aElement)
{
	if ( aElement->ElementType() == ETarget ) 
	{
		aParser->HandleErrorL( iTarget, ParserErrors::UnexpectedElement, aParser->ActiveElementName());	
		AddToElementListL( aElement, ETrue);
		iTarget = aElement;
	}
	else
	if ( aElement->ElementType() == EDescription )
	{
		aParser->HandleErrorL( iDescription, ParserErrors::UnexpectedElement, aParser->ActiveElementName());	
		AddToElementListL( aElement, ETrue);
		iDescription = aElement;
	}
	else if ( aElement->ElementType() == ERule ) 
	{
		AddToElementListL( aElement, ETrue);
	}
	else
	{	
		aParser->HandleErrorL( ParserErrors::UnexpectedElement, aParser->ActiveElementName());
	}
};

// -----------------------------------------------------------------------------
// CPolicy::AddIdElementL
// -----------------------------------------------------------------------------
//

void CPolicy::AddIdElementL( CElementBase * aElement)
{
	__ASSERT_ALWAYS ( aElement, User::Panic(PolicyParserPanic, KErrCorrupt));
	
	if ( aElement->ElementType() == ETarget )
	{
		iTarget = aElement;
	}
	else
	if ( aElement->ElementType() == EDescription )
	{
		iDescription = aElement;
	}
	
	AddToElementListL( aElement, EFalse);
}

// -----------------------------------------------------------------------------
// CPolicy::ValidElement
// -----------------------------------------------------------------------------
//

TBool CPolicy::ValidElement()
{
	return iPolicyId && iRuleCompiningAlg;
}

// -----------------------------------------------------------------------------
// CPolicy::DecodeElementL
// -----------------------------------------------------------------------------
//

HBufC8 * CPolicy::DecodeElementL( const  TLanguageSelector &aLanguage, const TDecodeMode &aMode)
{
	RDEBUG("PolicyEngineServer: Decode Policy element");


	__ASSERT_ALWAYS ( iPolicyId, User::Panic(PolicyParserPanic, KErrCorrupt));
	__ASSERT_ALWAYS ( iRuleCompiningAlg, User::Panic(PolicyParserPanic, KErrCorrupt));
	
	CheckAllElementsL();
	TElementReserver reserver( Container());
	
	
	TInt size = 16;

	size += 2 * PolicyLanguage::Policy::Policy[ aLanguage].Length();

	size += PolicyLanguage::Policy::PolicyId[ aLanguage].Length();
	const TDesC8& policyId = SelectCorrectValue( aLanguage, *iPolicyId);
	size += policyId.Length();

	size += PolicyLanguage::Policy::RuleCombiningAlgId[ aLanguage].Length();
	const TDesC8& ruleCompiningAlg = SelectCorrectValue( aLanguage, *iRuleCompiningAlg);
	size += ruleCompiningAlg.Length();
	
		
	HBufC8 * rules = NULL;
	CleanupStack::PushL(rules);
	
	for ( TInt i(0); i < ElementCount(); i++)
	{
		CElementBase * element = Element(i);

		HBufC8 * decode = DecodeElementOrIdL( aLanguage, element, aMode);
		CleanupStack::PushL( decode);		
	
		if ( rules )
		{
			rules = rules->ReAllocL( rules->Length() + decode->Length() + 10);
		}
		else
		{
			rules = HBufC8::NewL( decode->Length() + 10);
		}
		
		rules->Des().Append( *decode);
		CleanupStack::PopAndDestroy();		//decode
		CleanupStack::Pop();				//old rules or null...
		CleanupStack::PushL(rules);			
	}	
						//rules or null
		
	if (rules) size += rules->Length();
	
	HBufC8 * hbuf = HBufC8::NewLC( size);
	TPtr8 ptr = hbuf->Des();
	
	ptr.Append('<');
	ptr.Append(PolicyLanguage::Policy::Policy[ aLanguage]);
	ptr.Append(' ');
	ptr.Append(PolicyLanguage::Policy::PolicyId[ aLanguage]);
	ptr.Append( _L8("=\""));
	ptr.Append( policyId);
	ptr.Append( _L8("\" "));
	ptr.Append(PolicyLanguage::Policy::RuleCombiningAlgId[ aLanguage]);
	ptr.Append( _L8("=\""));
	ptr.Append( ruleCompiningAlg);
	ptr.Append( _L8("\">"));
	if (rules) ptr.Append(*rules);	
	ptr.Append(_L8("</"));
	ptr.Append(PolicyLanguage::Policy::Policy[ aLanguage]);
	ptr.Append('>');											   

	CleanupStack::Pop();
	CleanupStack::PopAndDestroy();

	reserver.Release();

	return hbuf;	
}

// -----------------------------------------------------------------------------
// CPolicy::
// -----------------------------------------------------------------------------
//


TMatchResponse CPolicy::MatchL( CPolicyProcessor* aPolicyProcessor)
{
#ifndef __POLICY_BASE_CREATOR_TOOL

	CPolicyStorage::PolicyStorage()->CheckElementL( this);
	RDEBUG8_2("PolicyEngineServer: %S match evaluation started", ExternalId());

	if ( iTarget)
	{
		TElementReserver targetReserver( iTarget);
		CPolicyStorage::PolicyStorage()->CheckElementL( iTarget);
		TMatchResponse response = iTarget->MatchL( aPolicyProcessor);
		targetReserver.Release();
		
		if ( response == EUnMatch )
		{
			return ENotApplicable;
		}
	}

	__ASSERT_ALWAYS ( iRuleCompiningAlg, User::Panic(PolicyParserPanic, KErrCorrupt));

	TCombiningAlgorith combiner( *iRuleCompiningAlg);

	for ( TInt i(0); i < ElementCount(); i++)
	{
		CElementBase * element = Element(i);	
		TElementReserver elementReserver( element);	

		if ( element->ElementType() == ERule)
		{
			CPolicyStorage::PolicyStorage()->CheckElementL( element);
			CRule * rule = (CRule*) element;
			combiner.AddInput( rule->MatchL( aPolicyProcessor), rule->Effect());
		}
		
		elementReserver.Release();
	}	

	RDEBUG8_2("PolicyEngineServer: %S match evaluation finished", ExternalId());
	return combiner.Result();

#endif// __POLICY_BASE_CREATOR_TOOL
}


// -----------------------------------------------------------------------------
// CAttribute::
// -----------------------------------------------------------------------------
//

CAttribute::CAttribute()
{
	iSaveType = ESubElement;
}

// -----------------------------------------------------------------------------
// CAttribute::
// -----------------------------------------------------------------------------
//

CAttribute * CAttribute::NewL( const TDesC8& aAttributeId, const TBool& aAttributeValue)
{
	CAttribute * self = new (ELeave) CAttribute();
	CleanupStack::PushL( self);
	
	using namespace PolicyLanguage::AttributeValues;

	//create attribute with true or false value	
	if ( aAttributeValue  )
	{
		self->ConstructL( aAttributeId, BooleanTrue[ENative], BooleanDataType[ENative]);
	}
	else
	{
		self->ConstructL( aAttributeId, BooleanFalse[ENative], BooleanDataType[ENative]);
	}
	
	CleanupStack::Pop( self);
	
	return self;
}

// -----------------------------------------------------------------------------
// CAttribute::
// -----------------------------------------------------------------------------
//


CAttribute * CAttribute::NewL( const TDesC8& aAttributeId, const TDesC8& aAttributeValue, const TDesC8& aDataType)
{
	CAttribute * self = new (ELeave) CAttribute();
	
	CleanupStack::PushL( self);
	self->ConstructL( aAttributeId, aAttributeValue, aDataType );
	CleanupStack::Pop( self);
	
	return self;
}

// -----------------------------------------------------------------------------
// CAttribute::
// -----------------------------------------------------------------------------
//


void CAttribute::ConstructL( const TDesC8& aAttributeId, const TDesC8& aAttributeValue, const TDesC8& aDataType)
{
	iAttributeValue = CAttributeValue::NewL( aAttributeValue, aDataType);
	iElementType = EAttribute;
	
	AddToElementListL( iAttributeValue, ETrue);
	
	iAttributeId = aAttributeId.AllocL();
	iDataType = aDataType.AllocL();
}

// -----------------------------------------------------------------------------
// CAttribute::
// -----------------------------------------------------------------------------
//

CAttribute::~CAttribute()
{
	delete iDataType;
	delete iAttributeId;
}

// -----------------------------------------------------------------------------
// CAttribute::
// -----------------------------------------------------------------------------
//
	
CAttributeValue * CAttribute::AttributeValue()
{
	return iAttributeValue;	
}

// -----------------------------------------------------------------------------
// CAttribute::
// -----------------------------------------------------------------------------
//

TDesC8& CAttribute::AttributeId()
{
	return *iAttributeId;	
}

// -----------------------------------------------------------------------------
// CAttribute::
// -----------------------------------------------------------------------------
//

TDesC8& CAttribute::AttributeType()
{
	return *iDataType;
}

// -----------------------------------------------------------------------------
// CRule::
// -----------------------------------------------------------------------------
//

CRule::CRule()
{
	iElementType = ERule;
	iElementName.Set( PolicyLanguage::NativeLanguage::Rule::Rule);
}

// -----------------------------------------------------------------------------
// CRule::
// -----------------------------------------------------------------------------
//


CRule::~CRule()
{
	delete iEffect;
	delete iRuleId;
}

// -----------------------------------------------------------------------------
// CRule::
// -----------------------------------------------------------------------------
//

		
CRule * CRule::NewL()
{
	return new (ELeave) CRule();
}

// -----------------------------------------------------------------------------
// CRule::
// -----------------------------------------------------------------------------
//


TBool CRule::IdentificateType( const TDesC8& aElementName)
{
	return ( aElementName == PolicyLanguage::NativeLanguage::Rule::Rule);		
}

// -----------------------------------------------------------------------------
// CRule::
// -----------------------------------------------------------------------------
//
			

HBufC8 * CRule::DecodeElementL( const  TLanguageSelector &aLanguage, const TDecodeMode &aMode)
{
	RDEBUG("PolicyEngineServer: Decode Rule element");

	__ASSERT_ALWAYS ( iEffect, User::Panic(PolicyParserPanic, KErrCorrupt));
	__ASSERT_ALWAYS ( iRuleId, User::Panic(PolicyParserPanic, KErrCorrupt));
	__ASSERT_ALWAYS ( iTarget, User::Panic(PolicyParserPanic, KErrCorrupt));
	
	CheckAllElementsL();
	TElementReserver reserver( Container());

	TInt size = 16;

	size += 2 * PolicyLanguage::Rule::Rule[ aLanguage].Length();

	size += PolicyLanguage::Rule::RuleId[ aLanguage].Length();
	const TDesC8& ruleId = SelectCorrectValue( aLanguage, *iRuleId);
	size += ruleId.Length();

	size += PolicyLanguage::Rule::Effect[ aLanguage].Length();
	const TDesC8& effect = SelectCorrectValue( aLanguage, *iEffect);
	size += effect.Length();
	
	HBufC8 * elements = NULL;
	CleanupStack::PushL( elements);
	
	for ( TInt i(0); i < ElementCount(); i++)
	{
		CElementBase * element = Element(i);
		
		HBufC8 * decode = DecodeElementOrIdL( aLanguage, element, aMode);
		
		CleanupStack::PushL( decode);		
		
		if ( elements)
		{
			elements = elements->ReAlloc( elements->Length() + decode->Length());
		}
		else
		{
			elements = HBufC8::NewL( decode->Length() + 10);
		}
		
		elements->Des().Append( *decode);
		CleanupStack::PopAndDestroy();
		CleanupStack::Pop();
		CleanupStack::PushL( elements);
	}	
	
	if ( elements) size += elements->Length();

	HBufC8 * hbuf = HBufC8::NewLC( size);

	TPtr8 ptr = hbuf->Des();
	
	ptr.Append('<');
	ptr.Append(PolicyLanguage::Rule::Rule[ aLanguage]);
	ptr.Append(' ');
	ptr.Append(PolicyLanguage::Rule::RuleId[ aLanguage]);
	ptr.Append(_L8("=\""));
	ptr.Append( ruleId);
	ptr.Append(_L8("\" "));
	ptr.Append(PolicyLanguage::Rule::Effect[ aLanguage]);
	ptr.Append(_L8("=\""));
	ptr.Append( effect);
	ptr.Append(_L8("\">"));
	if ( elements) ptr.Append(*elements);	
	ptr.Append(_L8("</"));
	ptr.Append(PolicyLanguage::Rule::Rule[ aLanguage]);
	ptr.Append('>');											   

	CleanupStack::Pop();
	CleanupStack::PopAndDestroy();
	
	reserver.Release();
	
	return hbuf;	
}


// -----------------------------------------------------------------------------
// CRule::AddIdElementL
// -----------------------------------------------------------------------------
//
const TDesC8& CRule::EffectValue()
{
	if ( iEffect)
	{
		return *iEffect;
	}
	
	return KNullDesC8;
}

// -----------------------------------------------------------------------------
// CRule::SetRuleIdL
// -----------------------------------------------------------------------------
//
void CRule::SetRuleIdL( const TDesC8& aName)
{
	delete iExternalId;
	iExternalId = NULL;
	iRuleId = iExternalId = aName.AllocL();
}


// -----------------------------------------------------------------------------
// CRule::SetEffectL
// -----------------------------------------------------------------------------
//
void CRule::SetEffectL( const TDesC8& aEffect)
{
	delete iEffect;
	iEffect = NULL;
	iEffect = aEffect.AllocL();
	
}



// -----------------------------------------------------------------------------
// CRule::GetTarget
// -----------------------------------------------------------------------------
//

CTarget* CRule::GetTarget()
{
	return (CTarget*) iTarget;
}


// -----------------------------------------------------------------------------
// CRule::AddIdElementL
// -----------------------------------------------------------------------------
//


void CRule::AddIdElementL( CElementBase * aElement)
{
	__ASSERT_ALWAYS ( aElement, User::Panic(PolicyParserPanic, KErrCorrupt));
	
	if ( aElement->ElementType() == ETarget )
	{
		iTarget = aElement;
	}
	else
	if ( aElement->ElementType() == ECondition )
	{
		iCondition = aElement;
	}
	else
	if ( aElement->ElementType() == EDescription )
	{
		iDescription = aElement;
	}
	
	AddToElementListL( aElement, EFalse);
}

// -----------------------------------------------------------------------------
// CRule::AddAttributeL
// -----------------------------------------------------------------------------
//


void CRule::AddAttributeL( CPolicyParser *aParser, const TDesC8& aName, const TDesC8& aValue)
{
	if ( aName == PolicyLanguage::NativeLanguage::Rule::RuleId)
	{
		aParser->HandleErrorL( iRuleId , ParserErrors::UnexpectedAttribute, aName);	
		iRuleId = aValue.AllocL();
		iExternalId = iRuleId;
		aParser->CheckExternalIdL( iExternalId);
	}
	else	
	if ( aName == PolicyLanguage::NativeLanguage::Rule::Effect)
	{
		aParser->HandleErrorL( iEffect, ParserErrors::UnexpectedAttribute, aName);	
		aParser->CheckEffectL( aValue);
		iEffect = aValue.AllocL();
	}
	else
	{
		aParser->HandleErrorL( ParserErrors::UnexpectedAttribute, aName );	
	}
}

// -----------------------------------------------------------------------------
// CRule::AddElementL
// -----------------------------------------------------------------------------
//


void CRule::AddElementL( CPolicyParser *aParser, CElementBase * aElement)
{
	if ( aElement->ElementType() == ETarget ) 
	{
		if ( aParser )
		{
			aParser->HandleErrorL( iTarget, ParserErrors::UnexpectedElement, aParser->ActiveElementName());	
		}
		
		AddToElementListL( aElement, ETrue);
		iTarget = aElement;
	} else
	if ( aElement->ElementType() == EDescription )
	{
		aParser->HandleErrorL( iDescription, ParserErrors::UnexpectedElement, aParser->ActiveElementName());	
		AddToElementListL( aElement, ETrue);
		iDescription = aElement;
	} else
	if ( aElement->ElementType() == ECondition )
	{
		aParser->HandleErrorL( iCondition, ParserErrors::UnexpectedElement, aParser->ActiveElementName());	
		AddToElementListL( aElement, ETrue);
		iCondition = aElement;
	}	else
	{	
		aParser->HandleErrorL( ParserErrors::UnexpectedElement, aParser->ActiveElementName());
	}
};

// -----------------------------------------------------------------------------
// CRule::ValidElement
// -----------------------------------------------------------------------------
//


TBool CRule::ValidElement()
{
	return iEffect && iRuleId && iTarget;
}

// -----------------------------------------------------------------------------
// CRule::MatchL
// -----------------------------------------------------------------------------
//


TMatchResponse CRule::MatchL( CPolicyProcessor* aPolicyProcessor)
{
#ifndef __POLICY_BASE_CREATOR_TOOL

	__ASSERT_ALWAYS ( iTarget, User::Panic(PolicyParserPanic, KErrCorrupt));

	//check rule target match
	TElementReserver targetReserver( iTarget);
	CPolicyStorage::PolicyStorage()->CheckElementL( iTarget);
	RDEBUG8_2("PolicyEngineServer: %S match evaluation started", ExternalId());
	TMatchResponse response = iTarget->MatchL( aPolicyProcessor);

	//if match evaluated effect of rule is checked otherwise result is NOT APPLICABLE
	if ( response == EMatch)
	{
		RDEBUG8_2("PolicyEngineServer: %S evaluated: MATCH", ExternalId());

		//effect if deny or permit, in effect case also condition is evaluated 
		if ( *iEffect == PolicyLanguage::Rule::Permit[ ENative])
		{
			RDEBUG8_2("PolicyEngineServer: %S, Effect == PERMIT", ExternalId());
			response = EPermit;
		
			if ( iCondition)
			{
				//condition exists
				RDEBUG8_2("PolicyEngineServer: %S contains condition", ExternalId());

				//avaluated condition value (true or false)
				TElementReserver targetReserver( iCondition);
				CPolicyStorage::PolicyStorage()->CheckElementL( iCondition);
				TMatchResponse condResp = ((CCondition*)iCondition)->ConditionValueL( aPolicyProcessor);
				targetReserver.Release();

				//condition == unmatch -> conditition is not valid -> result will be NOT APPLICABLE
				if ( condResp == EUnMatch)
				{
					RDEBUG8_2("PolicyEngineServer: %S, condition evaluated -> NOT APPLICABLE", ExternalId());
					response = ENotApplicable;
				}
				else if ( condResp == EIndeterminate)
				{
					//error in condition evaluation -> result will be INDETERMINATE
					RDEBUG8_2("PolicyEngineServer: %S, condition evaluated -> INDETERMINATE", ExternalId());
					response = EIndeterminate;
				}
				else
				{
					//other result is permit....
					response = EPermit;
					RDEBUG8_2("PolicyEngineServer: %S, condition evaluated -> PERMIT", ExternalId());
				}
			}
		}
		else
		{
			//in DENY is clear -> result will be DENY
			RDEBUG8_2("PolicyEngineServer: %S, Effect == DENY", ExternalId());
			response = EDeny;
		}
	}
	else
	{
		RDEBUG8_2("PolicyEngineServer: %S -> NOT APPLICABLE", ExternalId());
		response = ENotApplicable;
	}
	
	RDEBUG8_2("PolicyEngineServer: %S match evaluation finished", ExternalId());
	targetReserver.Release();
	return response;

#endif// __POLICY_BASE_CREATOR_TOOL
}


// -----------------------------------------------------------------------------
// CRule::Effect
// -----------------------------------------------------------------------------
//


TMatchResponse CRule::Effect()
{
	TMatchResponse response;
	if ( *iEffect == PolicyLanguage::Rule::Permit[ ENative])
	{
		response = EPermit;
	}
	else
	{
		response = EDeny;
	}
	
	return response;
}

// -----------------------------------------------------------------------------
// CTarget::CTarget
// -----------------------------------------------------------------------------
//


CTarget::CTarget()
{
	iElementType = ETarget;		
	iElementName.Set( PolicyLanguage::NativeLanguage::Rule::Target);
	
	iSaveType = ESubElement;
}

// -----------------------------------------------------------------------------
// CTarget::~CTarget
// -----------------------------------------------------------------------------
//

CTarget::~CTarget()
{
}

// -----------------------------------------------------------------------------
// CTarget::NewL
// -----------------------------------------------------------------------------
//

CTarget * CTarget::NewL()
{
	return new (ELeave) CTarget();
}


// -----------------------------------------------------------------------------
// CTarget::GetMatchContainers
// -----------------------------------------------------------------------------
//

void CTarget::GetMatchContainersL( RMatchObjectArray& aMatchContainer )
{
	if ( iSubjects)
	{
		((CItemContainer*)iSubjects)->GetMatchContainersL( aMatchContainer);
	}
	if ( iResources)
	{
		((CItemContainer*)iResources)->GetMatchContainersL( aMatchContainer);
	}
	if ( iActions)
	{
		((CItemContainer*)iActions)->GetMatchContainersL( aMatchContainer);
	}
	if ( iEnvironments)
	{
		((CItemContainer*)iEnvironments)->GetMatchContainersL( aMatchContainer);
	}
}



// -----------------------------------------------------------------------------
// CTarget::IdentificateType
// -----------------------------------------------------------------------------
//

TBool CTarget::IdentificateType( const TDesC8& aElementName)
{
	return ( aElementName == PolicyLanguage::NativeLanguage::Rule::Target);	
}

// -----------------------------------------------------------------------------
// CTarget::DecodeElementL
// -----------------------------------------------------------------------------
//
			
HBufC8 * CTarget::DecodeElementL( const  TLanguageSelector &aLanguage, const TDecodeMode &aMode)
{
	RDEBUG("PolicyEngineServer: Decode Target element");

	TPtrC8 ptr;
	ptr.Set( PolicyLanguage::Rule::Target[ aLanguage]);	

	if ( !ElementCount()) 
	{
		return ElementHelper::DecodeEmptyElement( ptr);
	}

	CheckAllElementsL();
	TElementReserver reserver( Container());

	_LIT8( DecodeFormat, "<%S>%S</%S>");	
	
	TInt size(15);
	size += 2 * ptr.Length();
	
	HBufC8 * elements = NULL;
	CleanupStack::PushL( elements);
	
	for ( TInt i(0); i < ElementCount(); i++)
	{
		CElementBase * element = Element(i);
		
		HBufC8 * decode = DecodeElementOrIdL( aLanguage, element, aMode);
		
		CleanupStack::PushL( decode);		
		
		if ( elements)
		{
			elements = elements->ReAlloc( elements->Length() + decode->Length() + 10);;
		}
		else
		{
			elements = HBufC8::NewL( decode->Length() + 10);
		}
		
		elements->Des().Append( *decode);
		CleanupStack::PopAndDestroy();
		CleanupStack::Pop();
		CleanupStack::PushL( elements);
	}	
	
	if (elements)
	{
	size += elements->Length();
	}
	
	HBufC8 * hbuf = HBufC8::NewLC( size);
	
	hbuf->Des().Format( DecodeFormat, &ptr, elements, &ptr);

	CleanupStack::Pop();
	CleanupStack::PopAndDestroy();
	
	reserver.Release();
	
	return hbuf;	
}

// -----------------------------------------------------------------------------
// CTarget::AddIdElementL
// -----------------------------------------------------------------------------
//

void CTarget::AddIdElementL( CElementBase * aElement)
{
	if ( aElement->ElementType() == ESubjects )
	{
		iSubjects = aElement;	
	}
	else
	if ( aElement->ElementType() == EActions )
	{
		iActions = aElement;
	}
	else
	if ( aElement->ElementType() == EResources )
	{
		iResources = aElement; 	
	}
	else
	if ( aElement->ElementType() == EEnvironments )
	{
		iEnvironments = aElement;	
	}
	
	AddToElementListL( aElement, EFalse);
}

// -----------------------------------------------------------------------------
// CTarget::ValidElement()
// -----------------------------------------------------------------------------
//

TBool CTarget::ValidElement()
{
	//Target is allways valid
	return ETrue;//(iSubjects || iActions || iResources || iEnvironments);
}


// -----------------------------------------------------------------------------
// CTarget::MatchL()
// -----------------------------------------------------------------------------
//


TMatchResponse CTarget::MatchL( CPolicyProcessor* aPolicyProcessor)
{
#ifndef __POLICY_BASE_CREATOR_TOOL

	TMatchResponse response( ENotApplicable);
	TBool atLeastOneMatchExecuted = EFalse;
	CPolicyStorage * storage = CPolicyStorage::PolicyStorage();

	//check all matches...
	//if any subject exist, check match
	if ( iSubjects )
	{
		TElementReserver element( iSubjects);
		storage->CheckElementL( iSubjects);
		response = iSubjects->MatchL( aPolicyProcessor);
		atLeastOneMatchExecuted = ETrue;
		element.Release();
	} 
	
	//if not unmatch from previous elements, and any actions exist, check match
	if ( response != EUnMatch && iActions)
	{
		TElementReserver element( iActions);
		storage->CheckElementL( iActions);
		response = iActions->MatchL( aPolicyProcessor);
		atLeastOneMatchExecuted = ETrue;
		element.Release();
	}

	//if not unmatch from previous elements, and any resources exist, check match
	if ( response != EUnMatch  && iResources)
	{
		TElementReserver element( iResources);
		storage->CheckElementL( iResources);
		response = iResources->MatchL( aPolicyProcessor);
		atLeastOneMatchExecuted = ETrue;
		element.Release();
	}

	//if not unmatch from previous elements, and any environments exist, check match
	if ( response != EUnMatch && iEnvironments)
	{
		TElementReserver element( iEnvironments);
		storage->CheckElementL( iEnvironments);
		response = iEnvironments->MatchL( aPolicyProcessor);
		atLeastOneMatchExecuted = ETrue;
		element.Release();
	}
	
	//if target doesn't have any objects, it is applicable to any object...
	if ( !atLeastOneMatchExecuted )
	{
		response = EMatch;
	}
	
	if ( response == ENotApplicable)
	{
		response = EUnMatch;
	}

	return response;

#endif //__POLICY_BASE_CREATOR_TOOL
}


// -----------------------------------------------------------------------------
// CTarget::AddElementL()
// -----------------------------------------------------------------------------
//

void CTarget::AddElementL( CPolicyParser *aParser, CElementBase * aElement)
{
	//check is element already exist and add it correct place
	
	if ( aElement->ElementType() == ESubjects )
	{
		if ( aParser )
		{
			aParser->HandleErrorL( iSubjects, ParserErrors::UnexpectedElement, aParser->ActiveElementName());
		}
		iSubjects = aElement;	
	}
	else
	if ( aElement->ElementType() == EActions )
	{
		if ( aParser )
		{
			aParser->HandleErrorL( iActions, ParserErrors::UnexpectedElement, aParser->ActiveElementName());
		}
		iActions = aElement;
	}
	else
	if ( aElement->ElementType() == EResources )
	{
		if ( aParser )
		{
			aParser->HandleErrorL( iResources, ParserErrors::UnexpectedElement, aParser->ActiveElementName());
		}
		iResources = aElement; 	
	}
	else
	if ( aElement->ElementType() == EEnvironments )
	{
		if ( aParser )
		{
			aParser->HandleErrorL( iEnvironments, ParserErrors::UnexpectedElement, aParser->ActiveElementName());
		}
		iEnvironments = aElement;	
	}
	else
		aParser->HandleErrorL( ParserErrors::UnexpectedElement, aParser->ActiveElementName());
	
	//add element also to element list
	AddToElementListL( aElement, ETrue);
}

// -----------------------------------------------------------------------------
// CItemContainer::CItemContainer()
// -----------------------------------------------------------------------------
//

CItemContainer::CItemContainer()
{
	
}

// -----------------------------------------------------------------------------
// CItemContainer::~CItemContainer()
// -----------------------------------------------------------------------------
//


CItemContainer::~CItemContainer()
{
	
}

// -----------------------------------------------------------------------------
// CItemContainer::DecodeElementL
// -----------------------------------------------------------------------------
//

HBufC8 * CItemContainer::DecodeElementL( const  TLanguageSelector &aLanguage, const TDecodeMode &aMode  )
{	
	RDEBUG("PolicyEngineServer: Decode ItemContainer element");


	_LIT8( DecodeFormat, "<%S>%S</%S>");	
	
	TInt size(15);
	
	TPtrC8 ptr;
	if ( iElementType == ESubject )
		ptr.Set( PolicyLanguage::MatchContainers::Subject[ aLanguage]);
	else if ( iElementType == EAction )
		ptr.Set( PolicyLanguage::MatchContainers::Action[ aLanguage]);
	else if ( iElementType == EResource )
		ptr.Set( PolicyLanguage::MatchContainers::Resource[ aLanguage]);
	else if ( iElementType == EEnvironment )
		ptr.Set( PolicyLanguage::MatchContainers::Environment[ aLanguage]);
	else if ( iElementType == ESubjects )
		ptr.Set( PolicyLanguage::MatchContainers::Subjects[ aLanguage]);
	else if ( iElementType == EActions )
		ptr.Set( PolicyLanguage::MatchContainers::Actions[ aLanguage]);
	else if ( iElementType == EResources )
		ptr.Set( PolicyLanguage::MatchContainers::Resources[ aLanguage]);
	else 
		ptr.Set( PolicyLanguage::MatchContainers::Environments[ aLanguage]);
	
	if ( !ElementCount()) 
	{
		return ElementHelper::DecodeEmptyElement( ptr);
	}
	
	CheckAllElementsL();
	TElementReserver reserver( Container());
	
	size += 2 * ptr.Length();
	
	HBufC8 * elements = NULL;
	CleanupStack::PushL( elements);
	
	for ( TInt i(0); i < ElementCount(); i++)
	{
		CElementBase * element = Element(i);
		
		HBufC8 * decode = DecodeElementOrIdL( aLanguage, element, aMode);
		
		CleanupStack::PushL( decode);		
		
		if ( elements)
		{
			elements = elements->ReAlloc( elements->Length() + decode->Length() + 10);;
		}
		else
		{
			elements = HBufC8::NewL( decode->Length() + 10);
		}
		
		elements->Des().Append( *decode);
		
		CleanupStack::PopAndDestroy();
		CleanupStack::Pop();
		CleanupStack::PushL( elements);		
	}	
	
	if (elements)
	{
	size += elements->Length();
	}
	
	HBufC8 * hbuf = HBufC8::NewLC( size);
	
	hbuf->Des().Format( DecodeFormat, &ptr, elements, &ptr);

	CleanupStack::Pop( hbuf);
	CleanupStack::PopAndDestroy();
	
	reserver.Release();	
	
	return hbuf;
}

// -----------------------------------------------------------------------------
// CItemContainer::GetMatchContainersL
// -----------------------------------------------------------------------------
//

void CItemContainer::GetMatchContainersL( RMatchObjectArray& aMatchContainer )
{
	for ( TInt i(0); i < ElementCount(); i++)
	{
		CElementBase * element = Element(i);
		
		__ASSERT_ALWAYS ( element, User::Panic( Panics::PolicyExecutionPanic, KErrCorrupt));
		 
		 TNativeElementTypes elementType = element->ElementType();

		if ( elementType == ESubjectMatch ||
			 elementType == EResourceMatch ||
			 elementType == EActionMatch ||
			 elementType == EEnvironmentMatch )
		{
			aMatchContainer.AppendL((CMatchObject*)element);
		}
		else if ( elementType == ESubject ||
			 	  elementType == EResource ||
			      elementType == EAction ||
			 	  elementType == EEnvironment )
		{
			((CItemContainer*)element)->GetMatchContainersL(aMatchContainer);
		}
	}
}


// -----------------------------------------------------------------------------
// CItemContainer::ValidElement
// -----------------------------------------------------------------------------
//

TBool CItemContainer::ValidElement()
{
	//empty elements are possible
	return ETrue; //( ElementCount() > 0); 
}

// -----------------------------------------------------------------------------
// CItemContainer::MatchL
// -----------------------------------------------------------------------------
//


TMatchResponse CItemContainer::MatchL( CPolicyProcessor* aPolicyProcessor)
{
#ifndef __POLICY_BASE_CREATOR_TOOL

	//evaluate subject (or resouce...) components or subjectmatch (or resourcematch) components
	for ( TInt i(0); i < ElementCount(); i++)
	{
		//reserve and check element
		CElementBase * element = Element(i);
		TElementReserver elementReserver( element); 
		CPolicyStorage::PolicyStorage()->CheckElementL( element);

		//avaluate match
		TMatchResponse response = element->MatchL( aPolicyProcessor);
		
		elementReserver.Release();
		
		//if indeterminate -> result will be indeterminate
		if ( response == EIndeterminate ) return EIndeterminate;
		
		
		if ( response == EMatch )
		{
			//In OR-(subject) case one match is enough to evaluate result -> EMatch		
			if ( iBooleanType == EOrBoolean ) 
			{
				return EMatch;					
			}
		}
		else
		{
			//In AND-(subjectmatch) case one unmatch is enough to evaluate result -> EUnMatch		
			if ( iBooleanType == EAndBoolean ) 
			{
				return EUnMatch;	
			}
		}
	}
	
	//if this point is reached, match evaluation bases on component type (OR or AND)
	//in OR cases it means that none of matched found
	//in AND cases it means that none of unmatches founb
	TMatchResponse response( EMatch);
	if ( iBooleanType == EOrBoolean) 
	{
		response = EUnMatch;	
	}
	
	return response;

#endif // __POLICY_BASE_CREATOR_TOOL
}	


// -----------------------------------------------------------------------------
// CItemContainer::
// -----------------------------------------------------------------------------
//	

void CItemContainer::AddIdElementL( CElementBase * aElement)
{
	AddToElementListL( aElement, EFalse);
}

// -----------------------------------------------------------------------------
// CItemContainer::
// -----------------------------------------------------------------------------
//

void CItemContainer::AddElementL( CPolicyParser *aParser, CElementBase * aElement)
{
	if ( aElement->ElementType() == iAllowedType)
		AddToElementListL( aElement, ETrue);
	else
		aParser->HandleErrorL( ParserErrors::UnexpectedElement, aParser->ActiveElementName());
}

// -----------------------------------------------------------------------------
// CSubjects::
// -----------------------------------------------------------------------------
//

CSubjects::CSubjects()
{
	iElementType = ESubjects;	
	iAllowedType = ESubject;
	iBooleanType = EOrBoolean;
	iElementName.Set( PolicyLanguage::NativeLanguage::MatchContainers::Subjects);

	iSaveType = ESubElement;
}

// -----------------------------------------------------------------------------
// CSubjects::
// -----------------------------------------------------------------------------
//

CSubjects::~CSubjects()
{
}

// -----------------------------------------------------------------------------
// CSubjects::
// -----------------------------------------------------------------------------
//
		
CSubjects * CSubjects::NewL()
{
	return new (ELeave) CSubjects();
}

// -----------------------------------------------------------------------------
// CSubjects::
// -----------------------------------------------------------------------------
//

TBool CSubjects::IdentificateType( const TDesC8& aElementName)
{
	return ( aElementName == PolicyLanguage::NativeLanguage::MatchContainers::Subjects);	
}

// -----------------------------------------------------------------------------
// CSubject::
// -----------------------------------------------------------------------------
//

CSubject::CSubject() 
{
	iElementType = ESubject;	
	iAllowedType = ESubjectMatch;
	iBooleanType = EAndBoolean;
	iElementName.Set( PolicyLanguage::NativeLanguage::MatchContainers::Subject);

	iSaveType = ESubElement;
}

// -----------------------------------------------------------------------------
// CSubject::
// -----------------------------------------------------------------------------
//


CSubject::~CSubject()
{
}
	
// -----------------------------------------------------------------------------
// CSubject::
// -----------------------------------------------------------------------------
//
	
		
CSubject * CSubject::NewL()
{
	return new (ELeave) CSubject();
}

// -----------------------------------------------------------------------------
// CSubject::
// -----------------------------------------------------------------------------
//


TBool CSubject::IdentificateType( const TDesC8& aElementName)
{
	return ( aElementName == PolicyLanguage::NativeLanguage::MatchContainers::Subject);	
}

// -----------------------------------------------------------------------------
// CAction::
// -----------------------------------------------------------------------------
//


CAction::CAction() 
{
	iElementType = EAction;	
	iAllowedType = EActionMatch;
	iBooleanType = EAndBoolean;
	iElementName.Set( PolicyLanguage::NativeLanguage::MatchContainers::Action);

	iSaveType = ESubElement;
}

// -----------------------------------------------------------------------------
// CAction::
// -----------------------------------------------------------------------------
//


CAction::~CAction()
{
}

// -----------------------------------------------------------------------------
// CAction::
// -----------------------------------------------------------------------------
//
		
CElementBase * CAction::NewL()
{
	return new (ELeave) CAction();
}

// -----------------------------------------------------------------------------
// CAction::
// -----------------------------------------------------------------------------
//

TBool CAction::IdentificateType( const TDesC8& aElementName)
{
	return ( aElementName == PolicyLanguage::NativeLanguage::MatchContainers::Action);	
}

// -----------------------------------------------------------------------------
// CActions::
// -----------------------------------------------------------------------------
//


CActions::CActions()
{
	iElementType = EActions;	
	iAllowedType = EAction;
	iBooleanType = EOrBoolean;
	iElementName.Set( PolicyLanguage::NativeLanguage::MatchContainers::Actions);

	iSaveType = ESubElement;
}

// -----------------------------------------------------------------------------
// CActions::
// -----------------------------------------------------------------------------
//


CActions::~CActions()
{
}
	
// -----------------------------------------------------------------------------
// CActions::
// -----------------------------------------------------------------------------
//
	
		
CElementBase * CActions::NewL()
{
	return new (ELeave) CActions();
}

// -----------------------------------------------------------------------------
// CActions::
// -----------------------------------------------------------------------------
//


TBool CActions::IdentificateType( const TDesC8& aElementName)
{
	return ( aElementName == PolicyLanguage::NativeLanguage::MatchContainers::Actions);	
}

// -----------------------------------------------------------------------------
// CResource::
// -----------------------------------------------------------------------------
//



CResource::CResource() 
{
	iElementType = EResource;	
	iAllowedType = EResourceMatch;
	iBooleanType = EAndBoolean;
	iElementName.Set( PolicyLanguage::NativeLanguage::MatchContainers::Resource);

	iSaveType = ESubElement;
}

// -----------------------------------------------------------------------------
// CResource::
// -----------------------------------------------------------------------------
//


CResource::~CResource()
{
}

// -----------------------------------------------------------------------------
// CResource::
// -----------------------------------------------------------------------------
//

		
CElementBase * CResource::NewL()
{
	return new (ELeave) CResource();
}

// -----------------------------------------------------------------------------
// CResource::IdentificateType
// -----------------------------------------------------------------------------
//


TBool CResource::IdentificateType( const TDesC8& aElementName)
{
	return ( aElementName == PolicyLanguage::NativeLanguage::MatchContainers::Resource);	
}

// -----------------------------------------------------------------------------
// CResources::CResources
// -----------------------------------------------------------------------------
//


CResources::CResources()
{
	iElementType = EResources;	
	iAllowedType = EResource;
	iBooleanType = EOrBoolean;
	iElementName.Set( PolicyLanguage::NativeLanguage::MatchContainers::Resources);
	
	iSaveType = ESubElement;
}


// -----------------------------------------------------------------------------
// CResources::
// -----------------------------------------------------------------------------
//


CResources::~CResources()
{
}
// -----------------------------------------------------------------------------
// CResources::
// -----------------------------------------------------------------------------
//

		
CElementBase * CResources::NewL()
{
	return new (ELeave) CResources();
}

// -----------------------------------------------------------------------------
// CResources::
// -----------------------------------------------------------------------------
//


TBool CResources::IdentificateType( const TDesC8& aElementName)
{
	return ( aElementName == PolicyLanguage::NativeLanguage::MatchContainers::Resources);	
}

// -----------------------------------------------------------------------------
// CEnviroment::
// -----------------------------------------------------------------------------
//


CEnvironment::CEnvironment() 
{
	iElementType = EEnvironment;	
	iAllowedType = EEnvironmentMatch;
	iBooleanType = EAndBoolean;
	iElementName.Set( PolicyLanguage::NativeLanguage::MatchContainers::Environment);

	iSaveType = ESubElement;
}

// -----------------------------------------------------------------------------
// CEnviroment::
// -----------------------------------------------------------------------------
//


CEnvironment::~CEnvironment()
{
}

// -----------------------------------------------------------------------------
// CEnviroment::
// -----------------------------------------------------------------------------
//

		
CElementBase * CEnvironment::NewL()
{
	return new (ELeave) CEnvironment();
}

// -----------------------------------------------------------------------------
// CEnviroment::
// -----------------------------------------------------------------------------
//

TBool CEnvironment::IdentificateType( const TDesC8& aElementName)
{
	return ( aElementName == PolicyLanguage::NativeLanguage::MatchContainers::Environment);	
}

// -----------------------------------------------------------------------------
// CEnviroments::
// -----------------------------------------------------------------------------
//


CEnvironments::CEnvironments()
{
	iElementType = EEnvironments;	
	iAllowedType = EEnvironment;
	iBooleanType = EOrBoolean;
	iElementName.Set( PolicyLanguage::NativeLanguage::MatchContainers::Environments);

}

// -----------------------------------------------------------------------------
// CEnviroments::
// -----------------------------------------------------------------------------
//


CEnvironments::~CEnvironments()
{
}

// -----------------------------------------------------------------------------
// CEnviroments::
// -----------------------------------------------------------------------------
//

		
CElementBase * CEnvironments::NewL()
{
	return new (ELeave) CEnvironments();
}

// -----------------------------------------------------------------------------
// CEnviroments::
// -----------------------------------------------------------------------------
//


TBool CEnvironments::IdentificateType( const TDesC8& aElementName)
{
	return ( aElementName == PolicyLanguage::NativeLanguage::MatchContainers::Environments);	
}

// -----------------------------------------------------------------------------
// CAttributeDesignator::
// -----------------------------------------------------------------------------
//


CAttributeDesignator::CAttributeDesignator()
	: CElementBase() 
{
	iSaveType = ESubElement;
}

// -----------------------------------------------------------------------------
// CAttributeDesignator::
// -----------------------------------------------------------------------------
//


CAttributeDesignator::~CAttributeDesignator()
{
	delete iAttributeId;
	delete iDataType;
}

// -----------------------------------------------------------------------------
// CAttributeDesignator::
// -----------------------------------------------------------------------------
//

	
CElementBase * CAttributeDesignator::NewL( TNativeElementTypes aType)
{
	CElementBase * designator = NULL; 

	if ( aType == ESubjectAttributeDesignator) designator = CSubjectAttributeDesignator::NewL();
	else 
	if ( aType == EActionAttributeDesignator) designator = CActionAttributeDesignator::NewL();
	else 
	if ( aType == EResourceAttributeDesignator) designator = CResourceAttributeDesignator::NewL();
	else 
	if ( aType == EEnvironmentAttributeDesignator) designator = CEnvironmentAttributeDesignator::NewL();
	
	return designator;
}

// -----------------------------------------------------------------------------
// CAttributeDesignator::
// -----------------------------------------------------------------------------
//


TDesC8& CAttributeDesignator::GetAttributeid()
{
	return *iAttributeId;	
}

// -----------------------------------------------------------------------------
// CAttributeDesignator::
// -----------------------------------------------------------------------------
//


TDesC8& CAttributeDesignator::GetDataType()
{
	return *iDataType;
}

// -----------------------------------------------------------------------------
// CAttributeDesignator::
// -----------------------------------------------------------------------------
//


TBool CAttributeDesignator::ValidElement()
{
	return iAttributeId && iDataType;
}

// -----------------------------------------------------------------------------
// CAttributeDesignator::
// -----------------------------------------------------------------------------
//



void CAttributeDesignator::AddAttributeL( CPolicyParser *aParser, const TDesC8& aName, const TDesC8& aValue)
{	
	if ( aName == PolicyLanguage::NativeLanguage::AttributeDesignators::AttributeId )
	{
		iAttributeId = aValue.AllocL();
	}
	else
	if ( aName == PolicyLanguage::NativeLanguage::AttributeDesignators::DataType)
	{
		if ( aParser)
		{
			aParser->CheckDataTypeL( aValue);
		}
		iDataType = aValue.AllocL();
	}
	else
	{
		if ( aParser)
		{
			aParser->HandleErrorL( ParserErrors::UnexpectedAttribute, aName);
		}
	}
}

// -----------------------------------------------------------------------------
// CAttributeDesignator::
// -----------------------------------------------------------------------------
//


HBufC8 * CAttributeDesignator::DecodeElementL( const  TLanguageSelector &aLanguage, const TDecodeMode &/*aMode*/  )
{
	RDEBUG("PolicyEngineServer: Decode Attribute Designator element");


	__ASSERT_ALWAYS ( iAttributeId, User::Panic(PolicyParserPanic, KErrCorrupt));
	__ASSERT_ALWAYS ( iDataType, User::Panic(PolicyParserPanic, KErrCorrupt));

	CheckAllElementsL();
	TElementReserver reserver( Container());
	
	_LIT8( DecodeFormat, "<%S %S=\"%S\" %S=\"%S\"/>");	
	
	TInt size(25);
	size += PolicyLanguage::AttributeDesignators::AttributeId[ aLanguage].Length();
	const TDesC8& attributeId = SelectCorrectValue( aLanguage, *iAttributeId);
	size += attributeId.Length();

	size += PolicyLanguage::AttributeDesignators::DataType[ aLanguage].Length();
	const TDesC8& dataType = SelectCorrectValue( aLanguage, *iDataType);
	size += dataType.Length();

	TPtrC8 ptr;
	if ( iElementType == ESubjectAttributeDesignator )
		ptr.Set( PolicyLanguage::AttributeDesignators::SubjectAttributeDesignator[ aLanguage]);
	else if ( iElementType == EActionAttributeDesignator )
		ptr.Set( PolicyLanguage::AttributeDesignators::ActionAttributeDesignator[ aLanguage]);
	else if ( iElementType == EResourceAttributeDesignator )
		ptr.Set( PolicyLanguage::AttributeDesignators::ResourceAttributeDesignator[ aLanguage]);
	else 
		ptr.Set( PolicyLanguage::AttributeDesignators::EnvironmentAttributeDesignator[ aLanguage]);
	
	size += 1 * ptr.Length();
	
	HBufC8 * hbuf = HBufC8::NewLC( size);
	
	hbuf->Des().Format( DecodeFormat, &ptr, 
									  &PolicyLanguage::AttributeDesignators::AttributeId[ aLanguage], 
									  &attributeId, 
									  &PolicyLanguage::AttributeDesignators::DataType[ aLanguage], 
									  &dataType);

	CleanupStack::Pop( hbuf);
	
	reserver.Release();	
	
	return hbuf;	
}

// -----------------------------------------------------------------------------
// CAttributeDesignator::
// -----------------------------------------------------------------------------
//


CAttribute * CAttributeDesignator::GetAttributeFromRequestContext( CPolicyProcessor * aRequestContext)
{
	__ASSERT_ALWAYS ( iAttributeId, User::Panic(PolicyExecutionPanic, KErrCorrupt));

	CPolicyProcessor::RAttributeList * container = NULL;

	switch ( iElementType)
	{
		case ESubjectAttributeDesignator :
			container = aRequestContext->Subjects();
		break;
		case EActionAttributeDesignator :
			container = aRequestContext->Actions();
		break;
		case EResourceAttributeDesignator :
			container = aRequestContext->Resources();
		break;
		case EEnvironmentAttributeDesignator :
			container = aRequestContext->Environments();
		break;
		default:
		break;
	}


	__ASSERT_ALWAYS ( container, User::Panic(PolicyExecutionPanic, KErrCorrupt));

	for ( TInt i(0); i < container->Count(); i++)
	{
		CAttribute * matchAttribute = (*container)[i];
		__ASSERT_ALWAYS ( matchAttribute, User::Panic(PolicyExecutionPanic, KErrCorrupt));
		
		RDEBUG8_2("PolicyEngineServer: Attribute in request context: %S", &matchAttribute->AttributeId());
		if ( matchAttribute->AttributeId() == *iAttributeId )
		{
			return matchAttribute;
		}
	}
	
	return NULL;
}

// -----------------------------------------------------------------------------
// CSubjectAttributeDesignator::
// -----------------------------------------------------------------------------
//



CSubjectAttributeDesignator::CSubjectAttributeDesignator()
	: CAttributeDesignator()
{
	iElementType = ESubjectAttributeDesignator;
	iElementName.Set( PolicyLanguage::NativeLanguage::AttributeDesignators::SubjectAttributeDesignator);

}

// -----------------------------------------------------------------------------
// CSubjectAttributeDesignator::
// -----------------------------------------------------------------------------
//

CSubjectAttributeDesignator::~CSubjectAttributeDesignator()
{
	
}

// -----------------------------------------------------------------------------
// CSubjectAttributeDesignator::
// -----------------------------------------------------------------------------
//
	
CSubjectAttributeDesignator * CSubjectAttributeDesignator::NewL()
{
	CSubjectAttributeDesignator * self = new (ELeave) CSubjectAttributeDesignator();
	return self;
}

// -----------------------------------------------------------------------------
// CSubjectAttributeDesignator::
// -----------------------------------------------------------------------------
//


TBool CSubjectAttributeDesignator::IdentificateType( const TDesC8& aElementName)
{
	return ( aElementName == PolicyLanguage::NativeLanguage::AttributeDesignators::SubjectAttributeDesignator);
}		

// -----------------------------------------------------------------------------
// CActionAttributeDesignator::
// -----------------------------------------------------------------------------
//


CActionAttributeDesignator::CActionAttributeDesignator()
	: CAttributeDesignator()
{
	iElementType = EActionAttributeDesignator;
	iElementName.Set( PolicyLanguage::NativeLanguage::AttributeDesignators::ActionAttributeDesignator);

}

// -----------------------------------------------------------------------------
// CActionAttributeDesignator::
// -----------------------------------------------------------------------------
//


CActionAttributeDesignator::~CActionAttributeDesignator()
{
	
}

// -----------------------------------------------------------------------------
// CActionAttributeDesignator::
// -----------------------------------------------------------------------------
//

	
CElementBase * CActionAttributeDesignator::NewL()
{
	CActionAttributeDesignator * self = new (ELeave) CActionAttributeDesignator();
	return self;
}

// -----------------------------------------------------------------------------
// CActionAttributeDesignator::
// -----------------------------------------------------------------------------
//


TBool CActionAttributeDesignator::IdentificateType( const TDesC8& aElementName)
{
	return ( aElementName == PolicyLanguage::NativeLanguage::AttributeDesignators::ActionAttributeDesignator);
}		

// -----------------------------------------------------------------------------
// CResourceAttributeDesignator::
// -----------------------------------------------------------------------------
//


CResourceAttributeDesignator::CResourceAttributeDesignator()
	: CAttributeDesignator()
{
	iElementType = EResourceAttributeDesignator;
	iElementName.Set( PolicyLanguage::NativeLanguage::AttributeDesignators::ResourceAttributeDesignator);

}

// -----------------------------------------------------------------------------
// CResourceAttributeDesignator::
// -----------------------------------------------------------------------------
//

CResourceAttributeDesignator::~CResourceAttributeDesignator()
{
	
}

// -----------------------------------------------------------------------------
// CResourceAttributeDesignator::NewL
// -----------------------------------------------------------------------------
//
	
CElementBase * CResourceAttributeDesignator::NewL()
{
	CResourceAttributeDesignator * self = new (ELeave) CResourceAttributeDesignator();
	return self;
}

// -----------------------------------------------------------------------------
// CResourceAttributeDesignator::IdentificateType
// -----------------------------------------------------------------------------
//

TBool CResourceAttributeDesignator::IdentificateType( const TDesC8& aElementName)
{
	return ( aElementName == PolicyLanguage::NativeLanguage::AttributeDesignators::ResourceAttributeDesignator);
}		

// -----------------------------------------------------------------------------
// CEnvironmentAttributeDesignator::CEnvironmentAttributeDesignator
// -----------------------------------------------------------------------------
//

CEnvironmentAttributeDesignator::CEnvironmentAttributeDesignator()
	: CAttributeDesignator()
{
	iElementType = EEnvironmentAttributeDesignator;
	iElementName.Set( PolicyLanguage::NativeLanguage::AttributeDesignators::EnvironmentAttributeDesignator);

}

// -----------------------------------------------------------------------------
// CEnvironmentAttributeDesignator::~CEnvironmentAttributeDesignator
// -----------------------------------------------------------------------------
//


CEnvironmentAttributeDesignator::~CEnvironmentAttributeDesignator()
{
	
}

// -----------------------------------------------------------------------------
// CEnvironmentAttributeDesignator::NewL
// -----------------------------------------------------------------------------
//

	
CElementBase * CEnvironmentAttributeDesignator::NewL()
{
	CEnvironmentAttributeDesignator * self = new (ELeave) CEnvironmentAttributeDesignator();
	return self;
}

// -----------------------------------------------------------------------------
// CEnvironmentAttributeDesignator::IdentificateType
// -----------------------------------------------------------------------------
//


TBool CEnvironmentAttributeDesignator::IdentificateType( const TDesC8& aElementName)
{
	return ( aElementName == PolicyLanguage::NativeLanguage::AttributeDesignators::EnvironmentAttributeDesignator);
}		

// -----------------------------------------------------------------------------
// CMatchObject::CMatchObject
// -----------------------------------------------------------------------------
//

CMatchObject::CMatchObject( const TDesC8& aElementName)
{
	if ( aElementName == PolicyLanguage::NativeLanguage::MatchObject::SubjectMatch ) 
	{
		iElementType = ESubjectMatch;
		iElementName.Set( PolicyLanguage::NativeLanguage::MatchObject::SubjectMatch);
	}
	else
	if ( aElementName == PolicyLanguage::NativeLanguage::MatchObject::ResourceMatch) 
	{
		iElementType = EResourceMatch;	
		iElementName.Set( PolicyLanguage::NativeLanguage::MatchObject::ResourceMatch);
	}
	else
	if ( aElementName == PolicyLanguage::NativeLanguage::MatchObject::ActionMatch ) 
	{
		iElementType = EActionMatch;
		iElementName.Set( PolicyLanguage::NativeLanguage::MatchObject::ActionMatch);
	}
	else
	{
		iElementType = EEnvironmentMatch;		
		iElementName.Set( PolicyLanguage::NativeLanguage::MatchObject::EnvironmentMatch);
	}
	
	iSaveType = ESubElement;
}



// -----------------------------------------------------------------------------
// CMatchObject::CMatchObject
// -----------------------------------------------------------------------------
//

CMatchObject::CMatchObject( TNativeElementTypes aElementType)
{
	iElementType = aElementType;
	
	switch ( aElementType)
	{
		case ESubjectMatch:
		iElementName.Set( PolicyLanguage::NativeLanguage::MatchObject::SubjectMatch);
		break;
		case EResourceMatch:
		iElementName.Set( PolicyLanguage::NativeLanguage::MatchObject::ResourceMatch);
		break;
		case EActionMatch:
		iElementName.Set( PolicyLanguage::NativeLanguage::MatchObject::ActionMatch);
		break;
		case EEnvironmentMatch:
		iElementName.Set( PolicyLanguage::NativeLanguage::MatchObject::EnvironmentMatch);
		break;
	}
}

// -----------------------------------------------------------------------------
// CMatchObject::NewL
// -----------------------------------------------------------------------------
//

CElementBase * CMatchObject::NewL( TNativeElementTypes aElementType)
{
	CMatchObject * self = new (ELeave) CMatchObject(aElementType);
	return self;
}

// -----------------------------------------------------------------------------
// CMatchObject::~CMatchObject
// -----------------------------------------------------------------------------
//

CMatchObject::~CMatchObject()
{
	delete iMatchId;
}

// -----------------------------------------------------------------------------
// CMatchObject::NewL
// -----------------------------------------------------------------------------
//

CElementBase * CMatchObject::NewL( const TDesC8& aElementName)
{
	CMatchObject * self = new (ELeave) CMatchObject(aElementName);
	return self;	
}

// -----------------------------------------------------------------------------
// CMatchObject::ValidElement
// -----------------------------------------------------------------------------
//
	
TBool CMatchObject::ValidElement()
{
	return iMatchId && iDesignator && iMatchValue;
}

// -----------------------------------------------------------------------------
// CMatchObject::AttributeDesignator
// -----------------------------------------------------------------------------
//

CAttributeDesignator* CMatchObject::AttributeDesignator()
{
	return iDesignator;
}

// -----------------------------------------------------------------------------
// CMatchObject::AttributeValue
// -----------------------------------------------------------------------------
//

CAttributeValue* CMatchObject::AttributeValue()
{
	return iMatchValue;
}

// -----------------------------------------------------------------------------
// CMatchObject::MatchId
// -----------------------------------------------------------------------------
//
HBufC8* CMatchObject::MatchId()
{
	return iMatchId;
}


// -----------------------------------------------------------------------------
// CMatchObject::IdentificateType
// -----------------------------------------------------------------------------
//
		
TBool CMatchObject::IdentificateType( const TDesC8& aElementName)
{
	return ( aElementName == PolicyLanguage::NativeLanguage::MatchObject::SubjectMatch ||
			 aElementName == PolicyLanguage::NativeLanguage::MatchObject::ActionMatch ||
			 aElementName == PolicyLanguage::NativeLanguage::MatchObject::EnvironmentMatch ||
			 aElementName == PolicyLanguage::NativeLanguage::MatchObject::ResourceMatch );	
}

// -----------------------------------------------------------------------------
// CMatchObject::CheckContainer
// -----------------------------------------------------------------------------
//

TBool CMatchObject::CheckMatchObject( const TDesC8& aType, const TDesC8& aAttribute, const TDesC8& aValue)
{
	using namespace PolicyLanguage::Constants;

	TBool response = EFalse;
	
	if ( (aType == Subject && iElementType == ESubjectMatch) || 
		 (aType == Action && iElementType == EActionMatch) ||
		 (aType == Resource && iElementType == EResourceMatch) ||
		 (aType == Environment && iElementType == EEnvironmentMatch) )
	{

		
		__ASSERT_ALWAYS ( iMatchValue, User::Panic(PolicyExecutionPanic, KErrCorrupt));
		__ASSERT_ALWAYS ( iDesignator, User::Panic(PolicyExecutionPanic, KErrCorrupt));
	
		response = ETrue;
	
		if ( aValue != Any)
		{
			if ( iMatchValue->Data()->Value() != aValue )
			{
				response = EFalse;
			}
		}
		
		if ( response && iDesignator->GetAttributeid() != aAttribute)
		{
			response = EFalse;
		}
	}
	
	return response;
}

// -----------------------------------------------------------------------------
// CMatchObject::AddAttributeL
// -----------------------------------------------------------------------------
//




void CMatchObject::AddAttributeL( CPolicyParser *aParser, const TDesC8& aName, const TDesC8& aValue)
{	
	if ( aName == PolicyLanguage::NativeLanguage::MatchObject::MatchId )
	{
		if ( aParser)
		{
			aParser->CheckFunctionIdL( iElementType, aValue);
		}
		iMatchId = aValue.AllocL();
	}
	else
	{
		if ( aParser)
		{
			aParser->HandleErrorL( ParserErrors::UnexpectedAttribute, aName );
		}
	}
}

// -----------------------------------------------------------------------------
// CMatchObject::AddElementL
// -----------------------------------------------------------------------------
//

void CMatchObject::AddElementL( CPolicyParser *aParser, CElementBase * aElement)
{
	if ( aElement->ElementType() == ESubjectAttributeDesignator ||
		 aElement->ElementType() == EActionAttributeDesignator ||
		 aElement->ElementType() == EResourceAttributeDesignator ||
		 aElement->ElementType() == EEnvironmentAttributeDesignator)
	{
		if ( aParser )
		{
			aParser->HandleErrorL( iDesignator, ParserErrors::UnexpectedElement, aParser->ActiveElementName());	
		}
		iDesignator = (CAttributeDesignator*) aElement;
		AddToElementListL( aElement, ETrue);
	}
	else if ( aElement->ElementType() == EAttributeValue)
	{
		if ( aParser )
		{
			aParser->HandleErrorL( iMatchValue, ParserErrors::UnexpectedElement, aParser->ActiveElementName());	
		}
		iMatchValue = (CAttributeValue*) aElement;
		AddToElementListL( aElement, ETrue);
	}
	else
	{
		aParser->HandleErrorL( ParserErrors::UnexpectedElement, aParser->ActiveElementName());
	}
};

// -----------------------------------------------------------------------------
// CMatchObject::
// -----------------------------------------------------------------------------
//

HBufC8 * CMatchObject::DecodeElementL( const TLanguageSelector &aLanguage, const TDecodeMode &aMode )
{
	RDEBUG("Decode MatchObject element");

	__ASSERT_ALWAYS ( iDesignator, User::Panic(PolicyParserPanic, KErrCorrupt));
	__ASSERT_ALWAYS ( iMatchValue, User::Panic(PolicyParserPanic, KErrCorrupt));
	__ASSERT_ALWAYS ( iMatchId, User::Panic(PolicyParserPanic, KErrCorrupt));

	CheckAllElementsL();
	TElementReserver reserver( Container());

	
	_LIT8( DecodeFormat, "<%S %S=\"%S\"> %S %S </%S>");	
	
	TInt size(25);
	size += PolicyLanguage::MatchObject::MatchId[ aLanguage].Length();
	const TDesC8& matchId = SelectCorrectValue( aLanguage, *iMatchId);
	size += matchId.Length();

	TPtrC8 ptr;
	if ( iElementType == ESubjectMatch )
		ptr.Set( PolicyLanguage::MatchObject::SubjectMatch[ aLanguage]);
	else if ( iElementType == EActionMatch )
		ptr.Set( PolicyLanguage::MatchObject::ActionMatch[ aLanguage]);
	else if ( iElementType == EResourceMatch )
		ptr.Set( PolicyLanguage::MatchObject::ResourceMatch[ aLanguage]);
	else 
		ptr.Set( PolicyLanguage::MatchObject::EnvironmentMatch[ aLanguage]);
	
	size += 2 * ptr.Length();
	
	HBufC8 * matchValue = DecodeElementOrIdL( aLanguage, iMatchValue, aMode);//EFullMode);
	
	CleanupStack::PushL( matchValue);
	
	size += matchValue->Length();
	

	HBufC8 * designator = DecodeElementOrIdL( aLanguage, iDesignator, aMode); //EFullMode);

	CleanupStack::PushL( designator);
		
	size += designator->Length();
	
	HBufC8 * hbuf = HBufC8::NewLC( size);
	
	hbuf->Des().Format( DecodeFormat, &ptr, 
									  &PolicyLanguage::MatchObject::MatchId[ aLanguage],
									  &matchId, 
									  matchValue, 
									  designator, 
									  &ptr);

	CleanupStack::Pop( hbuf);
	CleanupStack::PopAndDestroy( 2, matchValue);
	
	
	reserver.Release();	
	
	return hbuf;
}

// -----------------------------------------------------------------------------
// CMatchObject::MatchL
// -----------------------------------------------------------------------------
//
	
	
TMatchResponse CMatchObject::MatchL( CPolicyProcessor* aPolicyProcessor)
{
#ifndef __POLICY_BASE_CREATOR_TOOL

	RDEBUG("PolicyEngineServer: CMatchObject::MatchL");

	__ASSERT_ALWAYS ( iDesignator, User::Panic(PolicyExecutionPanic, KErrCorrupt));
	__ASSERT_ALWAYS ( iMatchValue, User::Panic(PolicyExecutionPanic, KErrCorrupt));

	
	TElementReserver designatorReserver( iDesignator);
	TElementReserver matchReserver( iMatchValue);

	#ifdef _DEBUG
		switch ( iElementType)
		{
			case ESubjectMatch:
				RDEBUG8_2("PolicyEngineServer: SubjectMatch - AttrId: %S", &iDesignator->GetAttributeid());
			break;
			case EActionMatch:
				RDEBUG8_2("PolicyEngineServer: ActionMatch - AttrId: %S", &iDesignator->GetAttributeid());
			break;
			case EResourceMatch:
				RDEBUG8_2("PolicyEngineServer: ResourceMatch - AttrId: %S", &iDesignator->GetAttributeid());
			break;
			default:
				RDEBUG8_2("PolicyEngineServer: EnvinromentMatch - AttrId: %S", &iDesignator->GetAttributeid());
			break;
		}
	#endif 

	CAttribute * requestAttribute = iDesignator->GetAttributeFromRequestContext( aPolicyProcessor); 	

	TMatchResponse response = EUnMatch;
		
	if ( requestAttribute)
	{
		if ( iDesignator->GetDataType() != requestAttribute->AttributeType() )
		{
			RDEBUG("PolicyEngineServer: Datatypes doesn't match between request and policy attributes -> INDETERMINATE!");
			response =  EIndeterminate;
		}
		else
		{
			if ( aPolicyProcessor->MatchFunctionL( *iMatchId, iMatchValue->Data(), requestAttribute->AttributeValue()->Data()))
			{
				RDEBUG8_3("PolicyEngineServer: MATCH: %S == %S", &iMatchValue->Data()->Value(), &requestAttribute->AttributeValue()->Data()->Value());
				response =  EMatch;
			}
			else
			{
				RDEBUG8_3("PolicyEngineServer: UNMATCH: %S != %S", &iMatchValue->Data()->Value(), &requestAttribute->AttributeValue()->Data()->Value());
			}
		}
	}
	else
	{
		
	}	
	
	matchReserver.Release();
	designatorReserver.Release();
	return response;

#endif// __POLICY_BASE_CREATOR_TOOL
}


// -----------------------------------------------------------------------------
// CMatchObject::AddIdElementL
// -----------------------------------------------------------------------------
//

void CMatchObject::AddIdElementL( CElementBase * aElement)
{
	__ASSERT_ALWAYS ( aElement, User::Panic(PolicyParserPanic, KErrCorrupt));
	
	switch ( aElement->ElementType())
	{
		case ESubjectAttributeDesignator:
		case EActionAttributeDesignator:
		case EResourceAttributeDesignator:
		case EEnvironmentAttributeDesignator:
			iDesignator = (CAttributeDesignator*) aElement;
		break;
		
		case EAttributeValue :
			iMatchValue = (CAttributeValue*) aElement;
		break;
	}

	AddToElementListL( aElement, EFalse);
}

// -----------------------------------------------------------------------------
// CDescription::
// -----------------------------------------------------------------------------
//

CDescription::CDescription()
{
	iElementType = EDescription;
	iElementName.Set( PolicyLanguage::NativeLanguage::Description::Description);
}

// -----------------------------------------------------------------------------
// CDescription::
// -----------------------------------------------------------------------------
//

CDescription::~CDescription()
{
	delete iDescription;	
}

// -----------------------------------------------------------------------------
// CDescription::
// -----------------------------------------------------------------------------
//
	
CElementBase * CDescription::NewL()
{
	return new (ELeave) CDescription();
}

// -----------------------------------------------------------------------------
// CDescription::
// -----------------------------------------------------------------------------
//
		
TBool CDescription::IdentificateType( const TDesC8& aElementName)
{
	return aElementName == PolicyLanguage::NativeLanguage::Description::Description;
}

// -----------------------------------------------------------------------------
// CDescription::
// -----------------------------------------------------------------------------
//
	
HBufC8 * CDescription::DecodeElementL( const  TLanguageSelector &aLanguage, const TDecodeMode &/*aMode*/  )
{
	RDEBUG("PolicyEngineServer: Decode Description element");


	__ASSERT_ALWAYS ( iDescription, User::Panic(PolicyParserPanic, KErrCorrupt));
	
	_LIT8( DecodeFormat, "<%S>%S</%S>");	
	
	TInt size(5);
	TPtrC8 ptr = PolicyLanguage::Description::Description[ aLanguage];
	size += ptr.Length() * 2;

	size += iDescription->Length();
	
	HBufC8 * hbuf = HBufC8::NewLC( size);
	hbuf->Des().Format( DecodeFormat, &ptr, iDescription, &ptr);
	CleanupStack::Pop( hbuf);
	
	return hbuf;
}

// -----------------------------------------------------------------------------
// CDescription::
// -----------------------------------------------------------------------------
//
		
void CDescription::AddContentL( CPolicyParser */*aParser*/, const TDesC8& aName)
{
	if ( iDescription )
	{
		iDescription = iDescription->ReAllocL( iDescription->Length() + aName.Length());
		iDescription->Des().Append( aName);
	}
	else
	{
		iDescription = aName.AllocL();
	}
};

// -----------------------------------------------------------------------------
// CDescription::
// -----------------------------------------------------------------------------
//

HBufC8 * CDescription::DescriptionText()
{
	return iDescription;
}

// -----------------------------------------------------------------------------
// CDescription::
// -----------------------------------------------------------------------------
//

TBool CDescription::ValidElement()
{
	return ETrue;
}

