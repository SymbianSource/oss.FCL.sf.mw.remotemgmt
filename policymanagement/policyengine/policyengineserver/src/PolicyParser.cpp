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

#include "PolicyParser.h"
#include "ElementBase.h"
#include "elements.h"
#include "DataTypes.h"
#include "debug.h"
#include "PolicyStorage.h"
#include "XACMLconstants.h"
#include "ErrorCodes.h"

#include <f32file.h>
#include <e32base.h>
#include <s32file.h>

// MACROS
// CONSTANTS

_LIT8( KCDataStart, "<![CDATA[");
const TInt KCDataStartLength = 9;
const TInt KCDataEndLength = 3;
_LIT8( KCDataEnd, "]]>");

// DATA TYPES
// FUNCTION PROTOTYPES


namespace ParserUtility
{
	// -----------------------------------------------------------------------------
	// ParserUtility::ContainsCDataField
	// -----------------------------------------------------------------------------
	//
	TBool ContainsCDataField( TPtrC8& aChunk)
	{
		//Find CData indicator
		return 0 <= aChunk.Find( KCDataStart);
	}
	
	// -----------------------------------------------------------------------------
	// ParserUtility::ReadCDataArea
	// -----------------------------------------------------------------------------
	//
	void ReadCDataAreaL( TPtrC8& aCDataArea, TPtrC8& aChunk, TPtrC8& aFinalPart)
	{
		//find cdata start index and calculate nested cdata fields
		TPtrC8 temp = aFinalPart;
		TInt tempIndex;
		TInt cdatastart = KErrNotFound;

		TInt cdataCount = 0;
		while (	KErrNotFound != (tempIndex = temp.Find( KCDataStart)))
		{
			if ( cdatastart == KErrNotFound)
			{
				//save cdata field start
				cdatastart = tempIndex;
			}
			temp.Set( temp.Mid(tempIndex + KCDataStartLength));
			cdataCount++;	
		}
		
		User::LeaveIfError( cdatastart);
		
		//find end location for cdata field (dismiss nested CDATA end marks)
		TInt cdataend = 0;
		temp.Set( aFinalPart);
			
		while ( cdataCount-- )
		{
			TInt current = temp.Find( KCDataEnd);
			temp.Set( temp.Mid(current + KCDataEndLength ));
			cdataend += current + KCDataEndLength;
		}
		
		if ( cdataCount != -1 )
		{
			User::Leave( KErrCorrupt);
		}

		//set cdata area
		aCDataArea.Set( aFinalPart.Mid( cdatastart));
		aCDataArea.Set( aCDataArea.Left( cdataend - cdatastart));

		
		//shrink cdata part from chunk and set aFinalPart to begin after cdata part 
		aChunk.Set( aChunk.Left( cdatastart));
		aFinalPart.Set( aFinalPart.Mid( cdataend));
	}
	
	// -----------------------------------------------------------------------------
	// ParserUtility::RemoveCDataMarks
	// -----------------------------------------------------------------------------
	//
	void RemoveCDataMarksL( TPtrC8& aData)
	{
		TInt index = aData.Find( KCDataStart);
	
		User::LeaveIfError( index );
		
		aData.Set( aData.Mid( index +  KCDataStartLength));
		
		TInt validIndex = 0;
		TInt tempIndex;
		
		TPtrC8 temp = aData;

		while ( KErrNotFound != ( tempIndex = temp.Find( KCDataEnd)))
		{
			validIndex += tempIndex + KCDataEndLength;
			temp.Set( temp.Mid( tempIndex + KCDataEndLength ));
		}
		
		User::LeaveIfError( index );
		
		aData.Set( aData.Left( validIndex - KCDataEndLength));
	}
	
	TInt LineCounter( TPtrC8& aData)
	{
		TPtrC8 temp = aData;
		TInt rowCount = 0;
		TInt index;
		
		//ascii code 10 is line feed mark
		while ( KErrNotFound != ( index = temp.Locate( 10)))
		{
			rowCount++;
			temp.Set( temp.Mid( index + 1 ));
		}
		
		return rowCount;
	}

}


// FORWARD DECLARATIONS
// CLASS DECLARATION
	

// -----------------------------------------------------------------------------
// CPolicyParser::CPolicyParser
// -----------------------------------------------------------------------------
//
CPolicyParser::CPolicyParser( CPolicyStorage * aStorage)
	: iStorage( aStorage)
{
	
}

// -----------------------------------------------------------------------------
// CPolicyParser::~CPolicyParser
// -----------------------------------------------------------------------------
//

CPolicyParser::~CPolicyParser()
{
	delete iElementStack;
	delete iParser;
}

// -----------------------------------------------------------------------------
// CPolicyParser::NewL
// -----------------------------------------------------------------------------
//

CPolicyParser* CPolicyParser::NewL( CPolicyStorage * aStorage)
{
	//2nd phase constructor
	CPolicyParser * self = new (ELeave) CPolicyParser( aStorage);
	
	CleanupStack::PushL( self);
	self->ConstructL();
	CleanupStack::Pop( self);
	
	return self;
}

// -----------------------------------------------------------------------------
// CPolicyParser::ConstructL()
// -----------------------------------------------------------------------------
//

void CPolicyParser::ConstructL()
{
	//2nd phase constructor
	iElementStack = new (ELeave)CStack<CElementBase, EFalse>;
	iParser = Xml::CParser::NewL( _L8( "text/xml"), *this );	
}

// -----------------------------------------------------------------------------
// CPolicyParser::ParseXACMLObjects()
// -----------------------------------------------------------------------------
//

CElementBase * CPolicyParser::ParseXACMLObjects( TInt& aError, const TDesC8 &aXACML, TDes8 &aReturnText)
{
	//parsing for XACML objects
	//Set parsing properties
	iValidCharacterSet = EFalse;		
	iErrorHandling = ETrue;
	iLanguage = EXACML;
	
	iElementStack->Reset();
	iStorage->ResetRealIdValidTest();
		
	iActiveElement = NULL;		
	iReturnText = ParserErrors::ParsingOk;	

	//actual parsing....
	TRAP( aError, ParseL( aXACML));
	
	//In error case delete root element...
	if ( aError != KErrNone )
	{
		delete iRootElement;
		iRootElement = NULL;
	}
	
	iStorage->ResetRealIdValidTest();

	aReturnText  = iReturnText;
	
	
	return iRootElement;
}	

// -----------------------------------------------------------------------------
// CPolicyParser::ParseL()
// -----------------------------------------------------------------------------
//

void CPolicyParser::ParseL( const TDesC8 &aXACML)
{
	//reset line counter and initialize XML-parser
	iActiveLine = 0;
	iParser->ParseBeginL();

	//Max size of chunk (max text length, which go once to parser)
	const TInt KMaxChunkLength = 1000;
	
	TPtrC8 xacmlDescription = aXACML;
	TPtrC8 chunk;
	TInt length = KMaxChunkLength < xacmlDescription.Length() ? KMaxChunkLength : xacmlDescription.Length();

	do
	{
		//max text, which go to parser is one line or KMaxChunkLength. Counter tracks line, where parsing is going
		iActiveLine++;
		//Find line feed mark (asciicode 10)
		TInt index = xacmlDescription.Locate(10);
		
		if ( index != KErrNotFound)
		{
			length = index + 1;
		}
		
		//set chunk pointer
		chunk.Set( xacmlDescription.Left(length));
		TPtrC8 cdata;

		if ( ParserUtility::ContainsCDataField( chunk))
		{
			//cdata mark indicated in chunk, remove cdata area from chunk, 
			ParserUtility::ReadCDataAreaL( cdata, chunk, xacmlDescription);
			
			if ( iLanguage == EXACML)
			{
				iActiveLine += ParserUtility::LineCounter( cdata);
			}
		}
		else
		{
			//set remaining XACML description to xacmlDescription
			xacmlDescription.Set( xacmlDescription.Mid(length));			
		}		
	
		
		//'|' and '~' are not allowed in policy system...
		if ( iLanguage == EXACML && (chunk.Locate('|') != KErrNotFound || chunk.Locate('~') != KErrNotFound))
		{
			HandleErrorL( ParserErrors::InvalidMark, ParserErrors::InvalidMarks); 
		}
	
		//drive description to parser	
		iParser->ParseL( chunk);
		
		//in cdata case call directly OnContent
		if ( cdata.Length())
		{
			TInt err = KErrNotFound; 
			OnContentL( cdata, err);
		}
	
		length = KMaxChunkLength < xacmlDescription.Length() ? KMaxChunkLength : xacmlDescription.Length();
	} while ( length);

	//close parser
	iParser->ParseEndL();
}

// -----------------------------------------------------------------------------
// CPolicyParser::ParseL()
// -----------------------------------------------------------------------------
//

TInt CPolicyParser::ParseNativeObjects( CElementBase * aSeedElement, const TDesC8 &aNative, TBool aAddToEditableCache)
{
	//parsing for native objects
	//set parsing properties
	iValidCharacterSet = ETrue;
	iErrorHandling = EFalse;
	iLanguage = ENative;		
	iElementStack->Reset();
	iAddToEditableCache = aAddToEditableCache;
	
	//seed element is given by function caller
	iActiveElement = aSeedElement;		

	//parsing
	TRAPD( err, ParseL( aNative));
	
	return err;	
}

// -----------------------------------------------------------------------------
// CPolicyParser::SetLineOffset()
// -----------------------------------------------------------------------------
//

void CPolicyParser::SetLineOffset( TInt aLine)
{
	iLineOffset = aLine;
}


// -----------------------------------------------------------------------------
// CPolicyParser::HandleErrorL()
// -----------------------------------------------------------------------------
//
void CPolicyParser::HandleErrorL( const TDesC8 &aText)
	{
	//if error handling is on....
	if ( iErrorHandling )
		{
		//parse error text
		iReturnText.Copy( _L8("(Line "));
		iReturnText.AppendNum( iActiveLine + iLineOffset);
		iReturnText.Append( _L8(") "));
		iReturnText.Append( aText);
		RDEBUG8_2("XACML parser error: %S", &iReturnText);
		}

	User::Leave( KErrParser );
	}

// -----------------------------------------------------------------------------
// CPolicyParser::HandleErrorL()
// -----------------------------------------------------------------------------
//

void CPolicyParser::HandleErrorL( const TDesC8 &aText, const TDesC8 &aErrorElement)
{
	//parse error text

	TBuf8<KMaxReturnMessageLength> returnText;
	
	const TInt KErrorMsgLength = 20;
	
	if ( (aText.Length() + aErrorElement.Length() + KErrorMsgLength) < KMaxReturnMessageLength) 
	{
		returnText = aText;
		returnText.Append(_L(": "));
		returnText.Append( aErrorElement);
	}
	
		
	HandleErrorL( returnText); 
}

// -----------------------------------------------------------------------------
// CPolicyParser::HandleErrorL()
// -----------------------------------------------------------------------------
//

void CPolicyParser::HandleErrorL( const TAny * aPointer, const TDesC8 &aError, const TDesC8 &aErrorElement)
{
	if ( aPointer != NULL)
	{
		HandleErrorL( aError, aErrorElement);
	}
}

// -----------------------------------------------------------------------------
// CPolicyParser::HandleNativeErrorL()
// -----------------------------------------------------------------------------
//

void CPolicyParser::HandleNativeErrorL( TInt /*aError*/)
{
	//When parsing native objects, error messages are not needed
	User::Leave( KErrNativeParser);
}

// -----------------------------------------------------------------------------
// CPolicyParser::Language()
// -----------------------------------------------------------------------------
//

TLanguageSelector CPolicyParser::Language()
{
	return iLanguage;
}

// -----------------------------------------------------------------------------
// CPolicyParser::CreateIdElementsL()
// -----------------------------------------------------------------------------
//

void CPolicyParser::CreateIdElementsL( const TDesC8& aIdString )
{
#ifndef __POLICY_BASE_CREATOR_TOOL

	TPtrC8 ptr( aIdString);
	TUint32 policySystemid(0);
	TBool atLeastOneId = EFalse;
		
	//parse from id string ids, which belongs to current component. 	
	while ( ptr[0] == '|')
	{
		//remove delimiter
		ptr.Set( ptr.Mid(1));
		
		//find second delimiter
		TInt index = ptr.Locate('|');
		
		TPtrC8 elementId( ptr);
		
		//if second delimiter found set correct length for elementId, otherwise elementId length is allready valid
		if ( index != KErrNotFound)
		{
			elementId.Set( ptr.Left( index));
			ptr.Set( ptr.Mid( index));
		}
			
		//read element id to TUint32 variable (policySystemid)
		TLex8 lex( elementId);
		lex.Val( policySystemid, EDecimal);		

		CElementBase * element = 0;
	
		if ( iAddToEditableCache )
		{
			element = iStorage->GetEditableElementL( policySystemid);
		}
		else
		{
			element = iStorage->GetElementL( policySystemid);
		}
	
		//add id element (element with id) to active element
		if ( element )
		{
			iActiveElement->AddIdElementL( element);
		}
		else
		{
			HandleNativeErrorL( KErrParser);
		}	
		
		//at least one element must be found
		atLeastOneId = ETrue;
	} 
	
	if ( !atLeastOneId)
	{
		HandleNativeErrorL( KErrParser);
	}
	
#endif //__POLICY_BASE_CREATOR_TOOL
}

// -----------------------------------------------------------------------------
// CPolicyParser::OnStartDocumentL()
// -----------------------------------------------------------------------------
//
void CPolicyParser::OnStartDocumentL(const Xml::RDocumentParameters& aDocParam, TInt /*aErrorCode*/)
{
	if ( aDocParam.CharacterSetName().DesC() == PolicyLanguage::XACML::CharacterSetUTF8)
	{
		iValidCharacterSet = ETrue;	
	}
}

// -----------------------------------------------------------------------------
// CPolicyParser::OnEndDocumentL()
// -----------------------------------------------------------------------------
//	
void CPolicyParser::OnEndDocumentL(TInt /*aErrorCode*/)
{
}

// -----------------------------------------------------------------------------
// CPolicyParser::OnStartElementL()
// -----------------------------------------------------------------------------
//		
void CPolicyParser::OnStartElementL(const Xml::RTagInfo& aElement, const Xml::RAttributeArray& aAttributes, TInt /*aErrorCode*/)
{
	//Debug variables
	const TDesC8& prefix( aElement.Prefix().DesC());
	const TDesC8& uri(aElement.Uri().DesC());
	//Debug variables
	
	//Set active element name (used in error handling)
  	iActiveElementName.Set( aElement.LocalName().DesC());
  	TPtrC8 elementName = iActiveElementName;
  	
  	//if content is XACML, convert it to native language
  	if ( iLanguage == EXACML )
  	{
		elementName.Set( ConvertElementNamesL( iActiveElementName));		
  	}
  	
  	TBool createNewElement = ETrue;
  	if ( iActiveElement )
  	{
 		//When activeelement is created (ENotLoaded state) and stack size is 0, parser
 		//assumes that activeelement is seed element and there is no need to create new elelent
  		if ((iActiveElement->iElementState == ENotLoadedEditableElement || 
  			 iActiveElement->iElementState == ENotLoaded ) && iElementStack->Count() == 0)
  		{
  			//element must have valid type
  			if ( !iActiveElement->IdentificateElement( elementName)) 
  			{
  				HandleNativeErrorL( KErrParser);
  			}
  			createNewElement = EFalse;
  		}
  	}
  	
  	//Identifie type and create new element
  	CElementBase * element( NULL);
   	if ( createNewElement )
   	{
		if ( CAttributeValue::IdentificateType( elementName) )
		{
	 		element = CAttributeValue::NewL();
 		} else   
		if ( CSubjectAttributeDesignator::IdentificateType( elementName) )
		{
 			element = CSubjectAttributeDesignator::NewL();
		} else
		if ( CActionAttributeDesignator::IdentificateType( elementName) )
		{
 			element = CActionAttributeDesignator::NewL();
		} else
		if ( CResourceAttributeDesignator::IdentificateType( elementName) )
		{
 			element = CResourceAttributeDesignator::NewL();
		} else
		if ( CEnvironmentAttributeDesignator::IdentificateType( elementName) )
		{
 			element = CEnvironmentAttributeDesignator::NewL();
		} else
		if ( CMatchObject::IdentificateType( elementName) )
		{
 			element = CMatchObject::NewL(elementName);
 		} else
		if ( CSubject::IdentificateType( elementName) )
		{
 			element = CSubject::NewL();
 		} else
		if ( CAction::IdentificateType( elementName) )
		{
 			element = CAction::NewL();
 		} else
		if ( CResource::IdentificateType( elementName) )
		{
 			element = CResource::NewL();
 		} else
		if ( CEnvironment::IdentificateType( elementName) )
		{
 			element = CEnvironment::NewL();
 		} else
		if ( CSubjects::IdentificateType( elementName) )
		{
 			element = CSubjects::NewL();
 		} else
		if ( CActions::IdentificateType( elementName) )
		{
 			element = CActions::NewL();
 		} else
		if ( CResources::IdentificateType( elementName) )
		{
 			element = CResources::NewL();
 		} else
		if ( CEnvironments::IdentificateType( elementName) )
		{
 			element = CEnvironments::NewL();
 		} else
		if ( CRule::IdentificateType( elementName) )
		{
 			element = CRule::NewL();
 		} else
		if ( CTarget::IdentificateType( elementName) )
		{
 			element = CTarget::NewL();
 		} else
		if ( CPolicy::IdentificateType( elementName) )
		{
 			element = CPolicy::NewL();
 		} else
		if ( CPolicySet::IdentificateType( elementName) )
		{
 			element = CPolicySet::NewL();
 		} else
		if ( CDescription::IdentificateType( elementName) )
		{
 			element = CDescription::NewL();
 		} else
		if ( CApply::IdentificateType( elementName) )
		{
 			element = CApply::NewL();
 		} else
		if ( CCondition::IdentificateType( elementName) )
		{
 			element = CCondition::NewL();
		} 		
 		else
 		{
 			HandleErrorL( ParserErrors::InvalidElement, iActiveElementName);
 		}
 		
 		//set element to correct state in Native mode
 		if ( iLanguage == ENative)
 		{
 			element->iElementState = iAddToEditableCache ? EEditableCacheElement : ECacheElement;
 		}
   	}
   	else
   	{
   		//and if active element is already created use it....
   		element = iActiveElement;
   	}
 	 	
 	 //cleanup...
 	CleanupStack::PushL( element);
 	 	
 	 //search eleement attributes
	for ( TInt i = 0; i < aAttributes.Count(); i++)
	{
		Xml::RAttribute attribute( aAttributes[i]);
		
		//DEBUG VARIABLES
		const TDesC8& prefixx( attribute.Attribute().Prefix().DesC());
		const TDesC8& urii(attribute.Attribute().Uri().DesC());
  		const TDesC8& elementNamee( attribute.Attribute().LocalName().DesC());
		//DEBUG VARIABLES

		TPtrC8 convertedName = attribute.Attribute().LocalName().DesC();
		TPtrC8 convertedValue = attribute.Value().DesC();
	  	
		//if XACML used, convert names to native language
	  	if ( iLanguage == EXACML )
	  	{
			convertedName.Set( ConvertAttributesL( attribute.Attribute().LocalName().DesC()));
			convertedValue.Set( ConvertValues( EXACML, attribute.Value().DesC()));
  		}
		
		//and add attributes to element
		element->AddAttributeL( this, convertedName, convertedValue );			
	}

	//add element to element stack
	iElementStack->PushL( iActiveElement);
	
	//and if new element created
	if ( createNewElement )
	{
		if ( iActiveElement != NULL)	
		{	
			//if active element exist add element to active element...
			iActiveElement->AddElementL( this, element);
		}
		else
		{
			//...otherwise add element to root
			iRootElement = element;
		}
		
		//current element is active element for next element
		iActiveElement = element;
	}
	
	//cleanup: element
	CleanupStack::Pop( element);
}

// -----------------------------------------------------------------------------
// CPolicyParser::OnEndElementL()
// -----------------------------------------------------------------------------
//		
	
void CPolicyParser::OnEndElementL(const Xml::RTagInfo& aElement, TInt /*aErrorCode*/)
{
	//set active element name
	iActiveElementName.Set( aElement.LocalName().DesC());
	TPtrC8 elementName( iActiveElementName);	
	
	
	//if XACML parsing, convert name to native language
	if ( iLanguage == EXACML)
	{
		elementName.Set( ConvertElementNamesL( iActiveElementName));
	}
	
	//Check is end stack correct
   	if ( !iActiveElement->IdentificateElement( elementName))
   	{
   		HandleErrorL( ParserErrors::InvalidElement, aElement.LocalName().DesC());	
   	}
   
   	//when parsing XACML make validy check for element... 
   	if ( iLanguage == EXACML)
   	{
	  	if ( !iActiveElement->ValidElement() )
	   	{
   			HandleErrorL( ParserErrors::InvalidElement, iActiveElementName);
	   	}
   	}
   
    //previous active element from stack
	iActiveElement = iElementStack->Pop();
}
	
// -----------------------------------------------------------------------------
// CPolicyParser::OnContentL()
// -----------------------------------------------------------------------------
//		
	
void CPolicyParser::OnContentL(const TDesC8& aBytes, TInt /*aErrorCode*/)
{
	//activename....
	iActiveElementName.Set( aBytes);

	TPtrC8 ptr = aBytes;

	//remove tabs, spaces, line feeds and so on
	ptr.Set( aBytes);
	if ( ptr.Length() == 1 && (ptr[0] == 9 || ptr[0] == 13 || ptr[0] == 10 || ptr[0] == 32))
	{
		//do nothing
	}
	else
	{
		//remove extra marks before id check
		//from start
		while ( ptr[0] == 9 || ptr[0] == 13 || ptr[0] == 10 || ptr[0] == 32)
		{
		
			ptr.Set( ptr.Mid(1));	
			if ( ptr.Length() == 0) 
			{
				break;
			}
		}

		//from end
		TInt index = ptr.Length() - 1;
		if ( index >= 0 )
		{
			while ( ptr[ index] == 9 || ptr[ index] == 13 || ptr[ index] == 10 || ptr[ index] == 32)
			{
				ptr.Set( ptr.Ptr(), index);	
				if ( ptr.Length() == 0) 
				{
					break;
				}
			
				index--;
			}
		}

		if ( ptr.Length() > 0 )
		{
			//if delimiter found, element content contains element ids, which belong to element
			if ( ptr[0] == KMessageDelimiterChar)
			{
				//create ID-elements 
				CreateIdElementsL( ptr);
			}
			else
			{
				//or add element content
				iActiveElement->AddContentL( this, ptr);
			}
		}
	}

}

// -----------------------------------------------------------------------------
// CPolicyParser::SetExternalIdChecked()
// -----------------------------------------------------------------------------
//

void CPolicyParser::SetExternalIdChecked( TAllowedExternalIdConflicts aCheckerState)
{
	iCheckerState = aCheckerState;
}

// -----------------------------------------------------------------------------
// CPolicyParser::SetExternalIdChecked()
// -----------------------------------------------------------------------------
//

void CPolicyParser::CheckExternalIdL( const HBufC8 * iExternalId)
{
	//make external id check only when parsing XACML
	if ( iLanguage == EXACML && iCheckerState != KNoExternalIdCheck)
	{
		if ( !iStorage->IsRealIdValidL( *iExternalId))
		{
			if ( !( iCheckerState == KSameExternalIdAllowedForRoot && iElementStack->Count() == 0 ))
			{
				HandleErrorL( ManagementErrors::IdAlreadyExist, *iExternalId);
			}
		}
	}
}

// -----------------------------------------------------------------------------
// CPolicyParser::ActiveElementName()
// -----------------------------------------------------------------------------
//

TDesC8& CPolicyParser::ActiveElementName()
{
	return iActiveElementName;
}

// -----------------------------------------------------------------------------
// CPolicyParser::CheckCompiningAlgorithmsL()
// -----------------------------------------------------------------------------
//
void CPolicyParser::CheckCompiningAlgorithmsL( TNativeElementTypes aType, const TDesC8& aAlgorithmId )
{
	//check only when parsinf XACML
	if ( iLanguage == EXACML)
	{
		if ( aAlgorithmId == PolicyLanguage::NativeLanguage::CombiningAlgorithms::RuleDenyOverrides ||
			 aAlgorithmId == PolicyLanguage::NativeLanguage::CombiningAlgorithms::RulePermitOverrides)
		{
			if ( aType != EPolicy)
			{
				HandleErrorL( ParserErrors::InvalidValue, aAlgorithmId);
			}
		}
		else
		if ( aAlgorithmId == PolicyLanguage::NativeLanguage::CombiningAlgorithms::PolicyDenyOverrides ||
			 aAlgorithmId == PolicyLanguage::NativeLanguage::CombiningAlgorithms::PolicyPermitOverrides)
		{
			if ( aType != EPolicySet)
			{
				HandleErrorL( ParserErrors::InvalidValue, aAlgorithmId);
			}
		}
		else
		{
			HandleErrorL( ParserErrors::InvalidValue, aAlgorithmId);
		}
	}
}

// -----------------------------------------------------------------------------
// CPolicyParser::CheckFunctionIdL()
// -----------------------------------------------------------------------------
//

void CPolicyParser::CheckFunctionIdL( const TNativeElementTypes& aType, const TDesC8& aFunctionId )
{
	if ( iLanguage == EXACML)
	{
		//these functions are valid for apply-component
		if ( aType == EApply)
		{
			if (!( aFunctionId == PolicyLanguage::NativeLanguage::Functions::FunctionAnd ||
   				   aFunctionId == PolicyLanguage::NativeLanguage::Functions::FunctionNot ||
				   aFunctionId == PolicyLanguage::NativeLanguage::Functions::FunctionOr ||
				   aFunctionId == PolicyLanguage::NativeLanguage::Functions::FunctionCertificatedSession ||
				   aFunctionId == PolicyLanguage::NativeLanguage::Functions::FunctionRuleTargetStructure ||
				   aFunctionId == PolicyLanguage::NativeLanguage::Functions::FunctionUserAcceptCorpPolicy ||
				   aFunctionId == PolicyLanguage::NativeLanguage::Functions::FunctionBooleanEqualId ||	
				   aFunctionId == PolicyLanguage::NativeLanguage::Functions::FunctionStringEqualId ))
			{
				HandleErrorL( ParserErrors::InvalidValue, aFunctionId );
			}
		}	//and these are valid also for other elements (match elements in practice)
		else if	(!(aFunctionId == PolicyLanguage::NativeLanguage::Functions::FunctionStringEqualId || 
			  	aFunctionId == PolicyLanguage::NativeLanguage::Functions::FunctionBooleanEqualId ||	
			  	aFunctionId == PolicyLanguage::NativeLanguage::Functions::FunctionUserAcceptCorpPolicy ||
			  	aFunctionId == PolicyLanguage::NativeLanguage::Functions::TrustedRoleMatch ||
			  	aFunctionId == PolicyLanguage::NativeLanguage::Functions::TrustedSubjectMatch))
		{
			HandleErrorL( ParserErrors::InvalidValue, aFunctionId );
		}
	}
}

// -----------------------------------------------------------------------------
// CPolicyParser::CheckEffectL()
// -----------------------------------------------------------------------------
//

void CPolicyParser::CheckEffectL( const TDesC8& aEffect )
{
	if ( iLanguage == EXACML)
	{
		if (!(aEffect == PolicyLanguage::NativeLanguage::Rule::Deny ||
			 aEffect == PolicyLanguage::NativeLanguage::Rule::Permit ))
		{
			HandleErrorL( ParserErrors::InvalidValue, aEffect );
		}
	}
}

// -----------------------------------------------------------------------------
// CPolicyParser::CheckDataTypeL()
// -----------------------------------------------------------------------------
//

void CPolicyParser::CheckDataTypeL( const TDesC8& aDataType )
{
	if ( iLanguage == EXACML)
	{
		if (!(aDataType == PolicyLanguage::NativeLanguage::AttributeValues::StringDataType || 
			  aDataType == PolicyLanguage::NativeLanguage::AttributeValues::BooleanDataType	))
		{
			HandleErrorL( ParserErrors::InvalidValue, aDataType );
		}
	}
}

// -----------------------------------------------------------------------------
// CPolicyParser::OnStartPrefixMappingL()
// -----------------------------------------------------------------------------
//
void CPolicyParser::OnStartPrefixMappingL(const RString& /*aPrefix*/, const RString& /*aUri*/, TInt /*aErrorCode*/)
{

}
	
// -----------------------------------------------------------------------------
// CPolicyParser::OnEndPrefixMappingL()
// -----------------------------------------------------------------------------
//    
void CPolicyParser::OnEndPrefixMappingL(const RString& /*aPrefix*/, TInt /*aErrorCode*/)
{
   
}

// -----------------------------------------------------------------------------
// CPolicyParser::OnIgnorableWhiteSpaceL()
// -----------------------------------------------------------------------------
//	
void CPolicyParser::OnIgnorableWhiteSpaceL(const TDesC8& /*aBytes*/, TInt /*aErrorCode*/)
   {
 
}

// -----------------------------------------------------------------------------
// CPolicyParser::OnSkippedEntityL()
// -----------------------------------------------------------------------------
//	
void CPolicyParser::OnSkippedEntityL(const RString& /*aName*/, TInt /*aErrorCode*/)
{
   	
}


// -----------------------------------------------------------------------------
// CPolicyParser::OnProcessingInstructionL()
// -----------------------------------------------------------------------------
//	
void CPolicyParser::OnProcessingInstructionL(const TDesC8& /*aTarget*/, const TDesC8& /*aData*/, TInt /*aErrorCode*/)
{
   	
}

// -----------------------------------------------------------------------------
// CPolicyParser::OnError()
// -----------------------------------------------------------------------------
//	
void CPolicyParser::OnError(TInt /*aErrorCode*/)
	{
   	// we need to leave here because Policy Engine traps parser errors, even though
   	// this function does not have L in it's name.
   	//HandleErrorL( ParserErrors::SyntaxError );
   	
   	// Ignoreing the leave code due to RVCT4.0
   	TRAP_IGNORE(HandleErrorL( ParserErrors::SyntaxError ));
	}

// -----------------------------------------------------------------------------
// CPolicyParser::GetExtendedInterface()
// -----------------------------------------------------------------------------
//	
TAny* CPolicyParser::GetExtendedInterface(const TInt32 /*aUid*/)
	{
	
	return NULL;		
	}

// -----------------------------------------------------------------------------
// CPolicyParser::ConvertElementNamesL()
// -----------------------------------------------------------------------------
//	
const TDesC8& CPolicyParser::ConvertElementNamesL( const TDesC8& aElementName )
{
	using namespace PolicyLanguage;
	
	//Conver name from XACML to native language (maybe array structure should be used to avoid editing when values is added)
	if ( AttributeDesignators::SubjectAttributeDesignator[ EXACML ] == aElementName ) return AttributeDesignators::SubjectAttributeDesignator[ ENative];
	if ( AttributeDesignators::ActionAttributeDesignator[ EXACML ] == aElementName ) return AttributeDesignators::ActionAttributeDesignator[ ENative];
	if ( AttributeDesignators::ResourceAttributeDesignator[ EXACML ] == aElementName ) return AttributeDesignators::ResourceAttributeDesignator[ ENative];
	if ( AttributeDesignators::EnvironmentAttributeDesignator[ EXACML ] == aElementName ) return AttributeDesignators::EnvironmentAttributeDesignator[ ENative];
	
	if ( AttributeValues::AttributeValue[ EXACML ] == aElementName ) return AttributeValues::AttributeValue[ ENative];
	
	if ( MatchObject::SubjectMatch[ EXACML ] == aElementName ) return MatchObject::SubjectMatch[ ENative];
	if ( MatchObject::ActionMatch[ EXACML ] == aElementName ) return MatchObject::ActionMatch[ ENative];
	if ( MatchObject::EnvironmentMatch[ EXACML ] == aElementName ) return MatchObject::EnvironmentMatch[ ENative];
	if ( MatchObject::ResourceMatch[ EXACML ] == aElementName ) return MatchObject::ResourceMatch[ ENative];
	
	if ( MatchContainers::Subject[ EXACML ] == aElementName ) return MatchContainers::Subject[ ENative];
	if ( MatchContainers::Subjects[ EXACML ] == aElementName ) return MatchContainers::Subjects[ ENative];
	if ( MatchContainers::Action[ EXACML ] == aElementName ) return MatchContainers::Action[ ENative];
	if ( MatchContainers::Actions[ EXACML ] == aElementName ) return MatchContainers::Actions[ ENative];
	if ( MatchContainers::Resource[ EXACML ] == aElementName ) return MatchContainers::Resource[ ENative];
	if ( MatchContainers::Resources[ EXACML ] == aElementName ) return MatchContainers::Resources[ ENative];
	if ( MatchContainers::Environment[ EXACML ] == aElementName ) return MatchContainers::Environment[ ENative];
	if ( MatchContainers::Environments[ EXACML ] == aElementName ) return MatchContainers::Environments[ ENative];

	if ( Rule::Rule[ EXACML ] == aElementName ) return Rule::Rule[ ENative];
	if ( Rule::Target[ EXACML ] == aElementName ) return Rule::Target[ ENative];
	if ( PolicySet::PolicySet[ EXACML ] == aElementName ) return PolicySet::PolicySet[ ENative];
	if ( Policy::Policy[ EXACML ] == aElementName ) return Policy::Policy[ ENative];
	
	if ( Expressions::Apply[ EXACML ] == aElementName ) return Expressions::Apply[ ENative];
	if ( Expressions::Condition[ EXACML ] == aElementName ) return Expressions::Condition[ ENative];

	if ( Description::Description[ EXACML ] == aElementName ) return Description::Description[ ENative];

	HandleErrorL( ParserErrors::InvalidElement, aElementName);

	return aElementName;
}

// -----------------------------------------------------------------------------
// CPolicyParser::ConvertAttributesL()
// -----------------------------------------------------------------------------
//
const TDesC8& CPolicyParser::ConvertAttributesL( const TDesC8& aType )
{
	using namespace PolicyLanguage;
	
	//Conver name from XACML to native language (maybe array structure should be used to avoid editing when values is added)
	if ( AttributeDesignators::AttributeId[ EXACML ] == aType ) return AttributeDesignators::AttributeId[ ENative];
	if ( AttributeDesignators::DataType[ EXACML ] == aType ) return AttributeDesignators::DataType[ ENative];
	
	if ( AttributeValues::DataType[ EXACML ] == aType ) return AttributeValues::DataType[ ENative];

	if ( MatchObject::MatchId[ EXACML ] == aType ) return MatchObject::MatchId[ ENative];
	
	if ( Rule::RuleId[ EXACML ] == aType ) return Rule::RuleId[ ENative];
	if ( Rule::Effect[ EXACML ] == aType ) return Rule::Effect[ ENative];

	if ( Expressions::FunctionId[ EXACML ] == aType ) return Expressions::FunctionId[ ENative];

	if ( Policy::PolicyId[ EXACML ] == aType ) return Policy::PolicyId[ ENative];
	if ( Policy::RuleCombiningAlgId[ EXACML ] == aType ) return Policy::RuleCombiningAlgId[ ENative];

	if ( PolicySet::PolicySetId[ EXACML ] == aType ) return PolicySet::PolicySetId[ ENative];
	if ( PolicySet::PolicyCombiningAlgId[ EXACML ] == aType ) return PolicySet::PolicyCombiningAlgId[ ENative];

	HandleErrorL( ParserErrors::InvalidElement, aType);

	return aType;
}

// -----------------------------------------------------------------------------
// CPolicyParser::ConvertValues()
// -----------------------------------------------------------------------------
//
const TDesC8& CPolicyParser::ConvertValues( TLanguageSelector aLanguage, const TDesC8& aValue)
{
	using namespace PolicyLanguage;

	//Conver name from XACML (or native depends on aLanguage variable) to native (or XACML) language 
	//(maybe array structure should be used to avoid editing when values is added)
	
	//destination langauge, aLanguage is source language
	TLanguageSelector destLang = ENative;
	
	if ( aLanguage == ENative)
	{
		destLang = EXACML;
	}
	
	if ( AttributeValues::StringDataType[ aLanguage ] == aValue ) return AttributeValues::StringDataType[ destLang];
	if ( AttributeValues::BooleanDataType[ aLanguage ] == aValue ) return AttributeValues::BooleanDataType[ destLang];
	if ( AttributeValues::BooleanTrue[ aLanguage ] == aValue ) return AttributeValues::BooleanTrue[ destLang];
	if ( AttributeValues::BooleanFalse[ aLanguage ] == aValue ) return AttributeValues::BooleanFalse[ destLang];

	if ( Rule::Permit[ aLanguage ] == aValue ) return Rule::Permit[ destLang];
	if ( Rule::Deny[ aLanguage ] == aValue ) return Rule::Deny[ destLang];

	if ( Functions::TrustedRoleMatch[ aLanguage ] == aValue ) return Functions::TrustedRoleMatch[ destLang];
	if ( Functions::TrustedSubjectMatch[ aLanguage ] == aValue ) return Functions::TrustedSubjectMatch[ destLang];
	if ( Functions::FunctionStringEqualId[ aLanguage ] == aValue ) return Functions::FunctionStringEqualId[ destLang];
	if ( Functions::FunctionBooleanEqualId[ aLanguage ] == aValue ) return Functions::FunctionBooleanEqualId[ destLang];

	if ( Functions::FunctionOr[ aLanguage ] == aValue ) return Functions::FunctionOr[ destLang];
	if ( Functions::FunctionAnd[ aLanguage ] == aValue ) return Functions::FunctionAnd[ destLang];
	if ( Functions::FunctionAnd[ aLanguage ] == aValue ) return Functions::FunctionAnd[ destLang];

	if ( Functions::FunctionCertificatedSession[ aLanguage ] == aValue ) return Functions::FunctionCertificatedSession[ destLang];
	if ( Functions::FunctionUserAcceptCorpPolicy[ aLanguage ] == aValue ) return Functions::FunctionUserAcceptCorpPolicy[ destLang];
	if ( Functions::FunctionRuleTargetStructure[ aLanguage ] == aValue ) return Functions::FunctionRuleTargetStructure[ destLang];

	if ( CombiningAlgorithms::RuleDenyOverrides[ aLanguage ] == aValue ) return CombiningAlgorithms::RuleDenyOverrides[ destLang];
	if ( CombiningAlgorithms::PolicyDenyOverrides[ aLanguage ] == aValue ) return CombiningAlgorithms::PolicyDenyOverrides[ destLang];
	if ( CombiningAlgorithms::RulePermitOverrides[ aLanguage ] == aValue ) return CombiningAlgorithms::RulePermitOverrides[ destLang];
	if ( CombiningAlgorithms::PolicyPermitOverrides[ aLanguage ] == aValue ) return CombiningAlgorithms::PolicyPermitOverrides[ destLang];
	

	return aValue;
}
	
