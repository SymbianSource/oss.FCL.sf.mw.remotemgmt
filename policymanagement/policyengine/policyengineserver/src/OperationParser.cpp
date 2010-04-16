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

#include "OperationParser.h"
#include "PolicyParser.h"
#include "OperationParserConstants.h"
#include "PolicyManager.h"
#include "PolicyStorage.h"
#include "debug.h"
#include "ErrorCodes.h"


// CONSTANTS
_LIT8( KCDataElement, "KCData");
const TChar KXMLElementEndMark = '>';

// -----------------------------------------------------------------------------
// COperationParser::COperationParser()
// -----------------------------------------------------------------------------
//

COperationParser::COperationParser()
{
}

// -----------------------------------------------------------------------------
// COperationParser::~COperationParser()
// -----------------------------------------------------------------------------
//

COperationParser::~COperationParser()
{
	ResetOperations();
	delete iParser;
}

// -----------------------------------------------------------------------------
// COperationParser::NewL()
// -----------------------------------------------------------------------------
//

COperationParser * COperationParser::NewL( const TDesC8& aOperationDescription)
{
	//2nd phase constructor
	COperationParser * self = new (ELeave) COperationParser();
	
	CleanupStack::PushL( self);
	self->ConstructL( aOperationDescription);
	CleanupStack::Pop( self);
	
	return self;
}

// -----------------------------------------------------------------------------
// COperationParser::ConstructL()
// -----------------------------------------------------------------------------
//

void COperationParser::ConstructL( const TDesC8& aOperationDescription)
{
	//2nd phase constructor
	iParser = Xml::CParser::NewL( _L8( "text/xml"), *this );	
	iXMLContent.Set( aOperationDescription);
}	


// -----------------------------------------------------------------------------
// COperationParser::ResetOperations()
// -----------------------------------------------------------------------------
//
void COperationParser::ResetOperations()
{
	for ( TInt i(0); i < iOperations.Count(); i++)
	{
		delete iOperations[i];
		iOperations[i] = NULL;
	}

	iOperations.Close();
}

// -----------------------------------------------------------------------------
// COperationParser::ConstructL()
// -----------------------------------------------------------------------------
//

void COperationParser::HandleErrorL( const TDesC8& aErrorTxt, const TDesC8& aElementName)
{
	//parse error text
	iReturnText->Copy( _L8("(Line "));
	iReturnText->AppendNum( iActiveLine);
	iReturnText->Append( _L8(") "));
	iReturnText->Append( aErrorTxt);
	iReturnText->Append( _L8(": "));
	iReturnText->Append( aElementName);
		
	RDEBUG_2("Operation parser error: %S", &iReturnText);

	User::Leave( KErrOpParser);	
}




// -----------------------------------------------------------------------------
// COperationParser::ParseOperationsL()
// -----------------------------------------------------------------------------
//

void COperationParser::ParseOperationsL( TDes8& aReturnTxt)
{
	//reset line counter and initialize XML-parser
	iActiveLine = 0;
	iReturnText = &aReturnTxt;
	iXMLStackPointer = 0;
	ResetOperations();


	//initliaze symbian xml parser
	iParser->ParseBeginL();
	
	//Max size of chunk (max text length, which go once to parser)
	const TInt KMaxChunkLength = 1000;
	
	TPtrC8 xml = iXMLContent;
	TPtrC8 chunk;
	TInt length = KMaxChunkLength < xml.Length() ? KMaxChunkLength : xml.Length();

	do
	{
		//Find line feed mark (asciicode 10)
		TInt indexLineFeed = xml.Locate(10);
		TInt elementEndIndex = xml.Locate( KXMLElementEndMark );

		if ( elementEndIndex != KErrNotFound && (elementEndIndex < indexLineFeed || indexLineFeed == KErrNotFound))
		{
			length = elementEndIndex + 1;
		}
		else
		if ( indexLineFeed != KErrNotFound)
		{ 
			//max text, which go to parser is one line or KMaxChunkLength. Counter tracks line, where parsing is going
			iActiveLine++;
		
			length = indexLineFeed + 1;
		}

		//set chunk pointer
		chunk.Set( xml.Left(length));
		

		if ( ParserUtility::ContainsCDataField( chunk))
		{
			//cdata mark indicated in chunk, remove cdata area from chunk, 
			TPtrC8 cdata;
			ParserUtility::ReadCDataAreaL( cdata, chunk, xml);
			
			//pointer to cdata content
			ParserUtility::RemoveCDataMarksL( cdata);
	
			if ( !iActiveOperation)
			{
				HandleErrorL( ParserErrors::UnexpectedElement, KCDataElement); 
			}
			
			iActiveOperation->SetXACMLContentL( cdata);
			iActiveOperation->SetLineOffset( iActiveLine - 1);
			
			iActiveLine += ParserUtility::LineCounter( cdata);
		}
		else
		{
			//set remaining XACML description to xacmlDescription
			xml.Set( xml.Mid(length));			
		}

		//'|' and '~' are not allowed in policy system...
		if ( chunk.Locate('|') != KErrNotFound || chunk.Locate('~') != KErrNotFound)
		{
			HandleErrorL( ParserErrors::InvalidMark, ParserErrors::InvalidMarks); 
		}
		
		//drive description to parser	
		iParser->ParseL( chunk);
	
		length = KMaxChunkLength < xml.Length() ? KMaxChunkLength : xml.Length();
	} while ( length);

	//close parser
	iParser->ParseEndL();	
}


// -----------------------------------------------------------------------------
// COperationParser::OnProcessingInstructionL()
// -----------------------------------------------------------------------------
//
void COperationParser::ExecuteL( TInt aIndex, CPolicyManager *aManager)
{
	iOperations[ aIndex]->ExecuteL( aManager);
}

// -----------------------------------------------------------------------------
// COperationParser::OperationCount()
// -----------------------------------------------------------------------------
//
TInt COperationParser::OperationCount()
{
	return iOperations.Count();
}

// -----------------------------------------------------------------------------
// COperationParser::OnStartDocumentL()
// -----------------------------------------------------------------------------
//
	

void COperationParser::OnStartDocumentL(const Xml::RDocumentParameters& /*aDocParam*/, TInt /*aErrorCode*/)
{
}

// -----------------------------------------------------------------------------
// COperationParser::OnEndDocumentL()
// -----------------------------------------------------------------------------
//
void COperationParser::OnEndDocumentL(TInt /*aErrorCode*/)
{
	
}

// -----------------------------------------------------------------------------
// COperationParser::OnStartElementL()
// -----------------------------------------------------------------------------
//
void COperationParser::OnStartElementL(const Xml::RTagInfo& aElement, const Xml::RAttributeArray& aAttributes, TInt /*aErrorCode*/)
{
	using namespace OperationParserConstants;

  	TPtrC8 elementName = aElement.LocalName().DesC();
  	CPolicyStorage* policyStorage = CPolicyStorage::PolicyStorage();

	//package starts operation package
  	if ( elementName != XMLStack[ iXMLStackPointer] )
  	{
		HandleErrorL( ParserErrors::UnexpectedElement, elementName); 
  	}
  	
  	if ( elementName == Operation )
  	{
  		if ( !iActiveOperation )
  		{
  			iActiveOperation = COperationInfo::NewL();
  		}
  		else
  		{
  			HandleErrorL( ParserErrors::UnexpectedElement, elementName); 
  		}
  	} 
  	
   	//search element attributes
	for ( TInt i = 0; i < aAttributes.Count(); i++)
	{
		Xml::RAttribute attribute( aAttributes[i]);
		
		TPtrC8 attributeName = attribute.Attribute().LocalName().DesC();
		TPtrC8 attributeValue = attribute.Value().DesC();

	  	
		if ( iActiveOperation && elementName == Operation )
		{
			if ( attributeName == ActionId )
			{
				iActiveOperation->SetActionIdL( attributeValue );
			}
			else if ( attributeName == TargetId)
			{
				iActiveOperation->SetTargetElementIdL( attributeValue );
			}
			else if ( attributeName == UseBearerCert)
			{
				if ( attributeValue == True)
				{
					iActiveOperation->UseBearerCertificate( ETrue);
				}
				else
				{
					iActiveOperation->UseBearerCertificate( EFalse);
				}
			}
			else if ( attributeName == AddServerId)
			{
				if ( KErrNone != policyStorage->AddNewServerId( attributeValue))
				{
	  				HandleErrorL( ManagementErrors::ServerIdManagementError, attributeName); 
				}
			}
			else if ( attributeName == RemoveServerId)
			{
				if ( attributeValue == ResetAllTag)
				{
					if ( KErrNone != policyStorage->ResetServerIdList())
					{
	  					HandleErrorL( ManagementErrors::ServerIdManagementError, attributeName); 
					}
				}
				else
				{
					if ( KErrNone != policyStorage->RemoveServerId( attributeValue))
					{
		  				HandleErrorL( ManagementErrors::ServerIdManagementError, attributeName); 
					}
				}
			}
			else
			{
  				HandleErrorL( ParserErrors::UnexpectedAttribute, attributeName); 
			}
		}
		else
		{
  			HandleErrorL( ParserErrors::UnexpectedAttribute, attributeName); 
		}

	}
	
	iXMLStackPointer++;
}

// -----------------------------------------------------------------------------
// COperationParser::OnEndElementL()
// -----------------------------------------------------------------------------
//
void COperationParser::OnEndElementL(const Xml::RTagInfo& aElement, TInt /*aErrorCode*/)
{
	using namespace OperationParserConstants;

  	TPtrC8 elementName = aElement.LocalName().DesC();

	//Add new operation operation list
	if ( elementName == Operation )
  	{
  		iOperations.AppendL( iActiveOperation);
  		iActiveOperation = NULL;
  	}
  	
  	iXMLStackPointer--;
}

// -----------------------------------------------------------------------------
// COperationParser::OnContentL()
// -----------------------------------------------------------------------------
//
void COperationParser::OnContentL(const TDesC8& /*aBytes*/, TInt /*aErrorCode*/)
{
}

// -----------------------------------------------------------------------------
// COperationParser::OnStartPrefixMappingL()
// -----------------------------------------------------------------------------
//
void COperationParser::OnStartPrefixMappingL(const RString& /*aPrefix*/, const RString& /*aUri*/, TInt /*aErrorCode*/)
{
	
}

// -----------------------------------------------------------------------------
// COperationParser::OnEndPrefixMappingL()
// -----------------------------------------------------------------------------
//

void COperationParser::OnEndPrefixMappingL(const RString& /*aPrefix*/, TInt /*aErrorCode*/)
{
	
}

// -----------------------------------------------------------------------------
// COperationParser::OnIgnorableWhiteSpaceL()
// -----------------------------------------------------------------------------
//
void COperationParser::OnIgnorableWhiteSpaceL(const TDesC8& /*aBytes*/, TInt /*aErrorCode*/)
{
	
}

// -----------------------------------------------------------------------------
// COperationParser::OnSkippedEntityL()
// -----------------------------------------------------------------------------
//
void COperationParser::OnSkippedEntityL(const RString& /*aName*/, TInt /*aErrorCode*/)
{
	
}

// -----------------------------------------------------------------------------
// COperationParser::OnProcessingInstructionL()
// -----------------------------------------------------------------------------
//
void COperationParser::OnProcessingInstructionL(const TDesC8& /*aTarget*/, const TDesC8& /*aData*/, TInt /*aErrorCode*/)
{
	
}

// -----------------------------------------------------------------------------
// COperationParser::OnError()
// -----------------------------------------------------------------------------
//
void COperationParser::OnError(TInt /*aErrorCode*/)
{
	//parse error text
	iReturnText->Copy( _L8("(Line "));
	iReturnText->AppendNum( iActiveLine);
	iReturnText->Append( _L8(") "));
	iReturnText->Append( ParserErrors::SyntaxError);

	RDEBUG_2("Operation parser error: %S", &iReturnText);

	// PolicyEngine expects this leave in error conditions -> prgram flow
	// goes to correct TRAP after this. Can not remove this Leave although
	// the function name does not have "L"
	
	// Commented due to RVCT4.0
	//User::Leave( KErrOpParser ); 	
}

// -----------------------------------------------------------------------------
// COperationParser::GetExtendedInterface()
// -----------------------------------------------------------------------------
//
TAny* COperationParser::GetExtendedInterface(const TInt32 /*aUid*/)
{
	return NULL;
}


// -----------------------------------------------------------------------------
// COperationParser::GetExtendedInterface()
// -----------------------------------------------------------------------------
//
COperationInfo::COperationInfo()
{
}


// -----------------------------------------------------------------------------
// COperationParser::~COperationInfo()
// -----------------------------------------------------------------------------
//
COperationInfo::~COperationInfo()
{
	delete iActionId;
	delete iTargetElement;
}

// -----------------------------------------------------------------------------
// COperationParser::NewL()
// -----------------------------------------------------------------------------
//

COperationInfo* COperationInfo::NewL()
{
	return new (ELeave) COperationInfo();
}

// -----------------------------------------------------------------------------
// COperationInfo::UseBearerCertificate()
// -----------------------------------------------------------------------------
//
void COperationInfo::UseBearerCertificate( const TBool aUseBearerCertificate)
{
	iUseBearerCertificate = aUseBearerCertificate;
}


// -----------------------------------------------------------------------------
// COperationParser::SetActionIdL()
// -----------------------------------------------------------------------------
//	
	
void COperationInfo::SetActionIdL( const TDesC8& aActionId)
{
	if ( iActionId )
	{
		User::Leave( KErrOpParser);
	}
	
	iActionId = aActionId.AllocL();
}

// -----------------------------------------------------------------------------
// COperationParser::SetTargetElementIdL()
// -----------------------------------------------------------------------------
//

void COperationInfo::SetTargetElementIdL( const TDesC8& aTargetElementId)
{
	if ( iTargetElement )
	{
		User::Leave( KErrOpParser);
	}
	
	iTargetElement = aTargetElementId.AllocL();
}

// -----------------------------------------------------------------------------
// COperationParser::SetXACMLContentL()
// -----------------------------------------------------------------------------
//

void COperationInfo::SetXACMLContentL( const TPtrC8& aXACMLContent)
{
	if ( iXACMLContent.Length())
	{
		User::Leave( KErrOpParser);
	}
	
	iXACMLContent.Set( aXACMLContent);
}


// -----------------------------------------------------------------------------
// COperationParser::ExecuteL()
// -----------------------------------------------------------------------------
//
		
void COperationInfo::ExecuteL( CPolicyManager *iManager)
{
	if ( *iActionId == OperationParserConstants::Add)
	{
		iManager->NewElementL( iLineOffset, *iTargetElement, iXACMLContent, iUseBearerCertificate);
	} 
	else if ( *iActionId == OperationParserConstants::Remove)
	{
		iManager->RemoveElementL( *iTargetElement);
	}
	else if ( *iActionId == OperationParserConstants::Replace)
	{
		iManager->ReplaceElementL( iLineOffset, *iTargetElement, iXACMLContent);
	}
	else
	{
		User::Panic( Panics::OperationParserPanic, KErrGeneral);
	}
}

// -----------------------------------------------------------------------------
// COperationParser::SetLineOffset()
// -----------------------------------------------------------------------------
//

void COperationInfo::SetLineOffset( const TInt& aLineOffset)
{
	iLineOffset = aLineOffset;
}


