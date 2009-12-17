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

#include "ElementBase.h"
#include "PolicyStorage.h"
#include "PolicyParser.h"
#include "XACMLconstants.h"
#include "elements.h"
#include "ErrorCodes.h"
#include "debug.h"
#include "PolicyEngineServer.h"

#include <e32std.h>

// CONSTANTS
// MACROS
// DATA TYPES
// FUNCTION PROTOTYPES
// FORWARD DECLARATIONS
// CLASS DECLARATION

// ----------------------------------------------------------------------------------------
// ElementHelper namespace contains utility functions for element handling
// ----------------------------------------------------------------------------------------

namespace ElementHelper
{
	// ----------------------------------------------------------------------------------------
	// ElementHelper::DecodeEmptyElement 
	// ----------------------------------------------------------------------------------------

	HBufC8 * DecodeEmptyElement( const TDesC8& aElementName)
	{
		//needed extra space in description
		const TInt KExtraSpace = 4;
		
		//create cbuf (return value)
		HBufC8 * hbuf = HBufC8::NewL( aElementName.Length() + KExtraSpace);
		TPtr8 ptr = hbuf->Des();
		
		//append empty element format
		ptr.Append('<');
		ptr.Append( aElementName);
		ptr.Append(_L(" />"));

		return hbuf;		
	}

	// ----------------------------------------------------------------------------------------
	// ElementHelper::CreateNewElementL Create new element, element type is identified by aElementId
	// ----------------------------------------------------------------------------------------

	CElementBase * CreateNewElementL( const TUint32& aElementId)
	{
		//decode element type
		TInt type = aElementId & 0xff;
		CElementBase * element = 0;
	
		//create empty element
		switch ( type)
		{
			case EPolicySet:
				element = CPolicySet::NewL();
				break;
			case EPolicy:
				element = CPolicy::NewL();
				break;
			case ETarget:
				element = CTarget::NewL();
				break;
			case ERule:	
				element = CRule::NewL();
				break;
			case ESubjects:
				element = CSubjects::NewL();
				break;
			case ESubject:
				element = CSubject::NewL();
				break;
			case ESubjectAttributeDesignator:
				element = CSubjectAttributeDesignator::NewL();
				break;
			case ESubjectMatch:
				element = CMatchObject::NewL( ESubjectMatch);
			break;
			case EActions:
				element = CActions::NewL();
				break;
			case EAction:
				element = CAction::NewL();
				break;
			case EActionAttributeDesignator:
				element = CActionAttributeDesignator::NewL();
				break;
			case EActionMatch:
				element = CMatchObject::NewL( EActionMatch);
			break;
			case EResources:
				element = CResources::NewL();
				break;
			case EResource:
				element = CResource::NewL();
				break;
			case EResourceAttributeDesignator:
				element = CResourceAttributeDesignator::NewL();
				break;
			case EResourceMatch:
				element = CMatchObject::NewL( EResourceMatch);
			break;
			case EEnvironments:
				element = CEnvironment::NewL();
				break;
			case EEnvironment:
				element = CEnvironment::NewL();
				break;
			case EEnvironmentAttributeDesignator:
				element = CEnvironmentAttributeDesignator::NewL();
				break;
			case EEnvironmentMatch:
				element = CMatchObject::NewL( EEnvironmentMatch);
			break;
			case EDescription:
				element = CDescription::NewL();
			break;
			case ECondition:
				element = CCondition::NewL();
				break;
			case EApply:
				element = CApply::NewL();
				break;
			default:
				User::Panic(PolicyExecutionPanic, KErrCorrupt);	
			break;
		}
	
		//set id and storage reference
		element->SetId( aElementId);
		
		return element;
	}
	
	// ----------------------------------------------------------------------------------------
	// ElementHelper::CompareElements Compare function for RArray 
	// ----------------------------------------------------------------------------------------

	TInt CompareElements( CElementBase* const& aElement1, CElementBase* const& aElement2)
	{
		if ( aElement1->GetId() == aElement2->GetId()) return 0;
		return ( aElement1->GetId() < aElement2->GetId() ? -1 : 1 );
	}
}


// -----------------------------------------------------------------------------
// CElementBase::CElementBase()
// -----------------------------------------------------------------------------
//

CElementBase::CElementBase()
	: iElementState( ENewElement), iSaveType( EOwnElement), 
	  iCreateExternalId( EFalse), iExternalId( NULL), iParentId( 0), iElementId( 0), 
	  iReferenceCount( KNotInElementCache)
{
}


// -----------------------------------------------------------------------------
// CElementBase::CElementBase()
// -----------------------------------------------------------------------------
//

CElementBase::CElementBase( const TUint32& aElementId )
	: iElementState( ENotLoaded), iSaveType( EOwnElement), iCreateExternalId( EFalse), iExternalId( NULL), iParentId( 0), 
	  iElementId( aElementId), iReferenceCount( KNotInElementCache)
{
}



// -----------------------------------------------------------------------------
// CElementBase::~CElementBase()
// -----------------------------------------------------------------------------
//

CElementBase::~CElementBase()
{
	//delete all containers in the element list
	for (TInt i(0); i < iElements.Count(); i++)
	{
		TElementContainer * container = iElements[i];
		
		//delete element only when it isn't in policy storage
		if ( container->iDeleteWithElement)
		{
			delete container->iElement;
		}
		
		//allways delete container
		delete container;
	}

	iElements.Close();
}

// -----------------------------------------------------------------------------
// CElementBase::ElementType()
// -----------------------------------------------------------------------------
//

TNativeElementTypes CElementBase::ElementType()
{
	return iElementType;
}

// -----------------------------------------------------------------------------
// CElementBase::ElementType()
// -----------------------------------------------------------------------------
//
TNativeElementTypes CElementBase::SubElementType( const TInt& aIndex, const TInt& aExpectedCount)
{
	//check that aIndex is valid
	if ( aIndex >= iElements.Count())
	{
		return (TNativeElementTypes)KErrNotFound;
	}
	
	//Check also expected count if parameter is given (positive)
	if ( aExpectedCount >= 0 && aExpectedCount != iElements.Count())
	{
		return (TNativeElementTypes)KErrNotFound;
	}
	
	//return type of element
	return iElements[ aIndex]->iElement->ElementType();
}


// -----------------------------------------------------------------------------
// CElementBase::SetId()
// -----------------------------------------------------------------------------
//

void CElementBase::SetId( const TUint32& aElementId)
{
	iElementId = aElementId;	
}

// -----------------------------------------------------------------------------
// CElementBase::CreateId()
// -----------------------------------------------------------------------------
//

void CElementBase::CreateIdL()
{
	//if element doesn't have id, get new id from policy storage
	if ( iElementId == 0)
	{
		iElementId = CPolicyStorage::PolicyStorage()->CreateIdL( iElementType);
	}
	
	//create ids also for child elements
/*	for ( TInt i(0); i < iElements.Count(); i++)
	{
		TElementContainer * container = iElements[i];
		container->iElement->CreateIds( aPolicyStorage);
	}*/
}

// -----------------------------------------------------------------------------
// CElementBase::GetId()
// -----------------------------------------------------------------------------
//
		
TUint32 CElementBase::GetId() const 
{
	return iElementId;
}

// -----------------------------------------------------------------------------
// CElementBase::AddParentIdL()
// -----------------------------------------------------------------------------
//

void CElementBase::AddParentIdL( TUint32 iMotherId)
{
	//parent id is saved only for rules, policysets and policies
	if ( iElementType == ERule || iElementType == EPolicySet || iElementType == EPolicy )
	{
/*		//create new parant element list if list doesn't exist
		if ( !iParentElements )
		{
			iParentElements	= new (ELeave) RIdList;
		}
	
		//add parent to list
		iParentElements->AppendL( iMotherId);
*/
		iParentId = iMotherId;
	}
}

// -----------------------------------------------------------------------------
// CElementBase::CheckNewParentId()
// -----------------------------------------------------------------------------
//
/*
TBool CElementBase::CheckNewParentId( TUint32 iMotherId)
{
	if (!iParentElements)
	{
		return ETrue;
	}
	
	for ( TInt i = 0; i < iParentElements->Count(); i++)
	{
		if ( (*iParentElements)[i] == iMotherId)
		{
			return EFalse;
		}
	}
	
	return ETrue;	
}*/

// -----------------------------------------------------------------------------
// CElementBase::DeleteElementL()
// -----------------------------------------------------------------------------
//

void CElementBase::DeleteElementL()
{
	__ASSERT_ALWAYS ( iElementId || iSaveType == ESubElement, User::Panic(PolicyStoragePanic, KErrGeneral));
	
	//if element is subelement which is saved part of parent element
	if ( iSaveType == ESubElement) return;
	
	//remove element
	CPolicyStorage::PolicyStorage()->DeleteElementL( iElementId);	
}

// -----------------------------------------------------------------------------
// CElementBase::DeleteMarkRecursiveL()
// -----------------------------------------------------------------------------
//
void CElementBase::DeleteMarkRecursiveL()
{
	iElementState = EDeletedEditableElement;

	for ( TInt i(0); i < iElements.Count(); i++)
	{
		//get element from list, add parent and save it
		CElementBase * element = iElements[i]->iElement;
		
		//check and load element (to editable cache)
		TElementReserver reserver( element);
		CPolicyStorage::PolicyStorage()->CheckElementL( element);	
		
		element->DeleteMarkRecursiveL();
		
		reserver.Release();
	}
}




// -----------------------------------------------------------------------------
// CElementBase::RemoveChild()
// -----------------------------------------------------------------------------
//

TInt CElementBase::RemoveChild( const TUint32& aChildId)
{
	TInt err = KErrNotFound;

	//delete also child elements
	for ( TInt i(0); i < iElements.Count(); i++)
	{
		//get element from list, add parent and save it
		TElementContainer * container = iElements[i];
		__ASSERT_ALWAYS ( container, User::Panic(PolicyStoragePanic, KErrGeneral));
		
		if ( container->iElement->iElementId == aChildId)
		{
			//delete container and remove it from child list
			delete iElements[i];
			iElements.Remove( i);
			err = KErrNone;
			break;
		}
	}		
	
	return err;
}


// -----------------------------------------------------------------------------
// CElementBase::CreateExternalId()
// -----------------------------------------------------------------------------
//
void CElementBase::CreateExternalId()
{
	iCreateExternalId = ETrue;
}


// -----------------------------------------------------------------------------
// CElementBase::SaveElementL()
// -----------------------------------------------------------------------------
//

TInt CElementBase::SaveElementL( TBool aRecursive)
{
	

	//check id
	if ( (iElementId == 0 && iSaveType == EOwnElement )|| iCreateExternalId)
	{
		if ( iElementId == 0)
		{
			iElementId = CPolicyStorage::PolicyStorage()->CreateIdL( iElementType);
		}
	}
	
	//if aResursive, make recursive save also for childs
	if ( aRecursive )
	{
		for ( TInt i(0); i < iElements.Count(); i++)
		{
			//get element from list, add parent and save it
			TElementContainer * container = iElements[i];
			if ( iSaveType == EOwnElement)
			{
				container->iElement->AddParentIdL( GetId());
			}
			container->iElement->SaveElementL( ETrue);
		}
	}

	TInt err(KErrNone);

	//Save element, if element is subelement it is already saved by parent element
	if ( iSaveType == EOwnElement)
	{
		//decode element, use id mode where only child id is saved to description
		HBufC8 * description = DecodeElementL( ENative, EIdMode);
		CleanupStack::PushL( description);
	
		//id parent element is defined
		if ( iParentId )
		{
			//create buffer for parent mapping
			HBufC8 * temp = HBufC8::NewL( description->Length() + (KIdLength + 1));		
			TPtr8 ptr = temp->Des();
		
			//add original description
			ptr.Append( *description);
			
			//and parent mapping
			ptr.Append( KParentListStartMark);
			ptr.AppendNum( iParentId);
		
			//delete old description and push old in cleanup stack
			CleanupStack::PopAndDestroy( description);			//old description
			CleanupStack::PushL( temp);
			description = temp;
		}

		//write policy to database
		CPolicyStorage::PolicyStorage()->SaveElementL( iElementId, description);

		CleanupStack::PopAndDestroy( description);
	}

	return err;
}
	
// -----------------------------------------------------------------------------
// CElementBase::SelectCorrectValue()
// -----------------------------------------------------------------------------
//

const TDesC8& CElementBase::SelectCorrectValue( const TLanguageSelector &aLanguage, const TDesC8& aOriginalValue ) const
{
	if ( aLanguage == ENative )
	{
		return aOriginalValue;
	}

	return CPolicyParser::ConvertValues( ENative, aOriginalValue);
}
	
	
// -----------------------------------------------------------------------------
// CElementBase::DecodeElementOrIdL()
// -----------------------------------------------------------------------------
//
		
HBufC8 * CElementBase::DecodeElementOrIdL( const TLanguageSelector &aLanguage, CElementBase * aElement, const TDecodeMode &aMode)
{		
	HBufC8 * buffer = NULL;
	
	//if full mode, decode full element. In id mode only id list is decoded
	if ( aMode == EFullMode)
	{
		buffer = aElement->DecodeElementL( aLanguage, EFullMode);
	}
	else 
	{
		if ( aElement->iSaveType == ESubElement )
		{
			buffer = aElement->DecodeElementL( aLanguage, aMode);
		}
		else
		{
			buffer = aElement->DecodeIdL();
		}
	}
		
	
	return buffer;
}

// -----------------------------------------------------------------------------
// CElementBase::DecodeIdL()
// -----------------------------------------------------------------------------
//

HBufC8 * CElementBase::DecodeIdL()
{
	//decoding format
	_LIT8( DecodeFormat,"|%d");
	
	//create buffer for id 
	const TInt KIdPtrLength = 10;
	HBufC8 * hbuf = HBufC8::NewLC(KIdPtrLength);
	
	//format it
	hbuf->Des().Format( DecodeFormat, iElementId);
	
	CleanupStack::Pop();
	return hbuf;
}

// -----------------------------------------------------------------------------
// CElementBase::AddToElementListL()
// -----------------------------------------------------------------------------
//

void CElementBase::AddToElementListL( CElementBase * aElement, TBool aDeleteWithElement)
{	
	//create new container for element
	TElementContainer * container = new (ELeave) TElementContainer;

		
	//fill container and apppend it to list
	container->iElement = aElement;
	container->iDeleteWithElement = aDeleteWithElement;
	
	iElements.AppendL( container);
}

// -----------------------------------------------------------------------------
// CElementBase::DescriptionL()
// -----------------------------------------------------------------------------
//

HBufC8 * CElementBase::DescriptionL()
{
	//try find description element from element list
	for (TInt i(0); i < iElements.Count(); i++)
	{
		TElementContainer * container = iElements[i];
		//reserve element from storage
		TElementReserver elementReserver( container->iElement);
		
		if ( container->iElement->iElementType == EDescription )
		{		
			//check element and read description
			CPolicyStorage::PolicyStorage()->CheckElementL( container->iElement);
			CDescription * description = (CDescription*) container->iElement;
			elementReserver.Release();
			return  description->DescriptionText();
		}
		
		elementReserver.Release();
	}
	
	return NULL;
}

// -----------------------------------------------------------------------------
// CElementBase::GetChildListLengthL()
// -----------------------------------------------------------------------------
//

TInt CElementBase::GetChildListLengthL()
{
	TInt size = 0;

	//policies, rules, policysets are elements which are shown external child list
	for (TInt i(0); i < iElements.Count(); i++)
	{
		CElementBase * element = iElements[i]->iElement;
		TElementReserver elementReserver( element);
		
		if ( element->iElementType == ERule || element->iElementType == EPolicy ||
			 element->iElementType == EPolicySet )
		{
			//check element and read external id length
			CPolicyStorage::PolicyStorage()->CheckElementL( element);
			size += element->ExternalId()->Length() + 1;
		}
		
		elementReserver.Release();
	}
	
	return size;
}

// -----------------------------------------------------------------------------
// CElementBase::GetChildListL()
// -----------------------------------------------------------------------------
//

void CElementBase::GetChildListL( TDes8& aChilds)
{
	//policies, rules, policysets are elements which are shown external child list
	for (TInt i(0); i < iElements.Count(); i++)
	{
		CElementBase * element = iElements[i]->iElement;
		TElementReserver elementReserver( element);
		
		//add child ids to one list one after one
		if ( element->iElementType == ERule || element->iElementType == EPolicy ||
			 element->iElementType == EPolicySet )
		{
			CPolicyStorage::PolicyStorage()->CheckElementL( element);
			aChilds.Append( *(element->ExternalId()));
			aChilds.Append( KMessageDelimiterChar);
		}
		
		elementReserver.Release();
	}
}

// -----------------------------------------------------------------------------
// CElementBase::GetChildElementCountL()
// -----------------------------------------------------------------------------
//

TInt CElementBase::GetChildElementCountL()
{
	TInt count(0);
	
	//policies, rules, policysets are elements which are shown external child list
	for (TInt i(0); i < iElements.Count(); i++)
	{
		CElementBase * element = iElements[i]->iElement;
		TElementReserver elementReserver( element);
		
		if ( element->iElementType == ERule || element->iElementType == EPolicy ||
			 element->iElementType == EPolicySet )
		{
			count++;
		}
	}
	
	return count;
}


// -----------------------------------------------------------------------------
// CElementBase::CheckAllElementsL()
// -----------------------------------------------------------------------------
//
	
void CElementBase::CheckAllElementsL()
{
	//Make element check for all elements in contaneiner list
	for (TInt i(0); i < iElements.Count(); i++)
	{
		CPolicyStorage::PolicyStorage()->CheckElementL( iElements[i]->iElement);
	}
}

// -----------------------------------------------------------------------------
// CElementBase::CheckAllElementsL()
// -----------------------------------------------------------------------------
//

TInt CElementBase::ElementCount()
{
	return iElements.Count();	
}

// -----------------------------------------------------------------------------
// CElementBase::CheckAllElementsL()
// -----------------------------------------------------------------------------
//

CElementBase * CElementBase::Element( TInt aIndex)
{
	return iElements[ aIndex]->iElement;
}

// -----------------------------------------------------------------------------
// CElementBase::ReleaseElement()
// -----------------------------------------------------------------------------
//

void CElementBase::ReleaseElement()
{
	//this operation is only for cache elements
	if ( iReferenceCount == KNotInElementCache) return;
	
	iReferenceCount--;
	
//	RDEBUG_3(_L("Policy Engine: Decrease element %d reference count (%d)"), iElementId, iReferenceCount);

	//reference count must be equal or greater than zero
	__ASSERT_ALWAYS ( iReferenceCount >= 0, User::Panic(PolicyStoragePanic, KErrGeneral));
	
}

// -----------------------------------------------------------------------------
// CElementBase::ReserveElement()
// -----------------------------------------------------------------------------
//


void CElementBase::ReserveElement()
{
	//this operation is only for cache elements
	if ( iReferenceCount == KNotInElementCache) return;

	iReferenceCount++;
	
//	RDEBUG_3(_L("Policy Engine: Increase element %d reference count (%d)"), iElementId, iReferenceCount);	
}

// -----------------------------------------------------------------------------
// CElementBase::Container()
// -----------------------------------------------------------------------------
//


RElementContainer* CElementBase::Container()
{
	return &iElements;
}

// -----------------------------------------------------------------------------
// CElementBase::IdentificateElement()
// -----------------------------------------------------------------------------
//

		
TBool CElementBase::IdentificateElement( const TDesC8& aElementName )
{
	return (iElementName == aElementName);
}

// -----------------------------------------------------------------------------
// CElementBase::AbsoluteTarget()
// -----------------------------------------------------------------------------
//
/*

void CElementBase::AbsoluteTargetL( TUnionTarget * target)
{
	//assert allways when element is not rule, policy or policyset
	__ASSERT_ALWAYS ( iElementType == ERule || iElementType == EPolicySet || iElementType == EPolicy, User::Panic(PolicyStoragePanic, KErrGeneral));

	//add target definitions to one target union from parent list, resolve absolute target for them
	if ( iParentElements)
	{
		for ( TInt i(0); i < iParentElements->Count(); i++ )
		{
			//get element from storage and check it
			CElementBase * element = iPolicyStorage->GetElementL( (*iParentElements)[i]);
			TElementReserver elementReserver( element);
		
			//no need for type check, bacause all parent elements are correct type
			iPolicyStorage->CheckElementL( element);
			element->AbsoluteTargetL( target);
			
			elementReserver.Release();
		}
	}
	
	//add element target to targer union
	for (TInt i(0); i < iElements.Count(); i++)
	{
		CElementBase * element = iElements[i]->iElement;
		TElementReserver elementReserver( element);
			
		if ( element->iElementType == ETarget )
		{
			//add target definitions to one target union
			iPolicyStorage->CheckElementL( element);
			target->AddTargetElementsL( element);	
		}

		elementReserver.Release();
	}
	
}
*/
// -----------------------------------------------------------------------------
// CElementBase::AddAttributeL()
// -----------------------------------------------------------------------------
//

			
void CElementBase::AddAttributeL( CPolicyParser *aParser, const TDesC8& aName, const TDesC8& /*aValue*/)
{
	//unexpected attribute
	aParser->HandleErrorL( ParserErrors::UnexpectedAttribute, aName);
}

// -----------------------------------------------------------------------------
// CElementBase::AddContentL()
// -----------------------------------------------------------------------------
//


void CElementBase::AddContentL( CPolicyParser *aParser, const TDesC8& aName)
{
	//unexpected content
	aParser->HandleErrorL( ParserErrors::UnexpectedContent, aName);
}

// -----------------------------------------------------------------------------
// CElementBase::AddElementL()
// -----------------------------------------------------------------------------
//

void CElementBase::AddElementL( CPolicyParser *aParser, CElementBase * /*aElement*/)
{
	//unexpected element
	aParser->HandleErrorL( ParserErrors::UnexpectedElement, aParser->ActiveElementName());
}

// -----------------------------------------------------------------------------
// CElementBase::Match()
// -----------------------------------------------------------------------------
//


TMatchResponse CElementBase::MatchL( CPolicyProcessor* /*aPolicyProcessor*/)
{
 	User::Panic(PolicyExecutionPanic, KErrCorrupt);
 	TMatchResponse response = EIndeterminate;
 	return response;
}


// -----------------------------------------------------------------------------
// CElementBase::ApplyValueL()
// -----------------------------------------------------------------------------
//

CAttributeValue* CElementBase::ApplyValueL( CPolicyProcessor* /*aPolicyProcessor*/)
{
 	User::Panic(PolicyExecutionPanic, KErrCorrupt);
 	return NULL;
}

// -----------------------------------------------------------------------------
// CElementBase::DecodeElementL()
// -----------------------------------------------------------------------------
//

HBufC8 * CElementBase::DecodeElementL( const TLanguageSelector &/*aLanguage*/, const TDecodeMode &/*aMode*/  )
{
	User::Panic(PolicyExecutionPanic, KErrCorrupt);
	return NULL;
}

// -----------------------------------------------------------------------------
// CElementBase::AddIdElementL()
// -----------------------------------------------------------------------------
//

void CElementBase::AddIdElementL( CElementBase * /*aElement*/)
{
	User::Panic(PolicyExecutionPanic, KErrCorrupt);
}

// -----------------------------------------------------------------------------
// CElementBase::ValidElement()
// -----------------------------------------------------------------------------
//

TBool CElementBase::ValidElement()
{
	User::Panic(PolicyExecutionPanic, KErrCorrupt);
	return ETrue;
}

// -----------------------------------------------------------------------------
// CElementBase::CheckAllElementsL()
// -----------------------------------------------------------------------------
//

HBufC8 * CElementBase::ExternalId() const 
{
	return iExternalId;
}


// -----------------------------------------------------------------------------
// CElementBase::GetElementsListLengthL()
// -----------------------------------------------------------------------------
//

TInt CElementBase::GetElementsListLengthL( TElementType aType )
{
	TInt size = 0;

	if ( iExternalId &&
	   ((iElementType == ERule && aType == ERules ) ||
	    (iElementType == EPolicy && aType == EPolicies ) ||
	    (iElementType == EPolicySet && aType == EPolicySets )))
	{
		size = iExternalId->Length() + 1;
	}

	//policies, rules, policysets are elements which are shown external child list
	for (TInt i(0); i < iElements.Count(); i++)
	{
		CElementBase * element = iElements[i]->iElement;
		TElementReserver elementReserver( element);
		
		if ( element->iElementType == ERule || 
			 element->iElementType == EPolicy ||
			 element->iElementType == EPolicySet )
		{
			//check element and read external id length
			CPolicyStorage::PolicyStorage()->CheckElementL( element);
			size += element->GetElementsListLengthL( aType);
		}
		
		elementReserver.Release();
	}
	
	return size;
}

// -----------------------------------------------------------------------------
// CElementBase::GetElementsListL()
// -----------------------------------------------------------------------------
//

void CElementBase::GetElementsListL( TElementType aType, TDes8& aChilds)
{
	if ( iExternalId &&
	   ((iElementType == ERule && aType == ERules ) ||
	    (iElementType == EPolicy && aType == EPolicies ) ||
	    (iElementType == EPolicySet && aType == EPolicySets )))
	{
		aChilds.Append( *iExternalId);
		aChilds.Append( KMessageDelimiterChar);
	}

	//policies, rules, policysets are elements which are shown external child list
	for (TInt i(0); i < iElements.Count(); i++)
	{
		CElementBase * element = iElements[i]->iElement;
		TElementReserver elementReserver( element);
		
		//add child ids to one list one after one
		if ( element->iElementType == ERule || 
			 element->iElementType == EPolicy ||
			 element->iElementType == EPolicySet )
		{
			CPolicyStorage::PolicyStorage()->CheckElementL( element);
			element->GetElementsListL( aType, aChilds);
		}
		
		elementReserver.Release();
	}
}

// -----------------------------------------------------------------------------
// CElementBase::FindAttributesL()
// -----------------------------------------------------------------------------
//
void CElementBase::FindAttributesL( TNativeElementTypes aAttributeType, RElementArray& aValuesList)
{
	if ( iElementType == aAttributeType)
	{
		aValuesList.AppendL( this);
	}
	else
	{
		for ( TInt i = 0; i < iElements.Count(); i++)
		{
			CElementBase * element = iElements[i]->iElement;
			TNativeElementTypes type = element->ElementType();
	
			if ( type == ERule ||
				 type == ETarget ||
				 type == EPolicy ||
				 type == EPolicySet ||
				 type == ESubject ||
				 type == ESubjects ||
				 type == EAction ||
				 type == EActions ||
				 type == EResource ||
				 type == EResources ||
				 type == EEnvironment ||
				 type == EEnvironment ||
				 type == ESubjectMatch ||
				 type == EActionMatch ||
				 type == EResourceMatch ||
				 type == EEnvironmentMatch )
			{
				TElementReserver reserve( element);
				CPolicyStorage::PolicyStorage()->CheckElementL( element);		
				
				element->FindAttributesL( aAttributeType, aValuesList);
				
				reserve.Release();
			}
		}	
	}
}
