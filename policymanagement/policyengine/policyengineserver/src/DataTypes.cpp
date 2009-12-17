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

#include "DataTypes.h"
#include "XACMLconstants.h"
#include "PolicyParser.h"
#include "ErrorCodes.h"
#include "elements.h"



namespace FunctionHelper
{
	// ----------------------------------------------------------------------------------------
	// FunctionHelper::AcceptableParamCount 
	// ----------------------------------------------------------------------------------------

	TBool AcceptableParamCount( const TDesC8& aFunctionId, TInt aParams )
	{
		using namespace PolicyLanguage::NativeLanguage::Functions;

		if ( aFunctionId == FunctionStringEqualId && aParams == 2 )
		{
			return ETrue;
		}
		else if (aFunctionId == FunctionBooleanEqualId  && aParams == 2)
		{
			return ETrue;
		}
		else if (aFunctionId == TrustedRoleMatch  && aParams == 2)
		{
			return ETrue;
		}
		else if (aFunctionId == TrustedSubjectMatch  && aParams == 2)
		{
			return ETrue;
		}
		else if (aFunctionId == FunctionOr)
		{
			return ETrue;
		}
		else if (aFunctionId == FunctionAnd)
		{
			return ETrue;
		}
		else if (aFunctionId == FunctionNot && aParams == 1)
		{
			return ETrue;
		}
		else if (aFunctionId == FunctionUserAcceptCorpPolicy && aParams == 1)
		{
			return ETrue;
		}
		else if (aFunctionId == FunctionCertificatedSession && aParams == 0)
		{
			return ETrue;
		}
		else if (aFunctionId == FunctionRuleTargetStructure && (aParams % 3) == 0)
		{
			return ETrue;
		}
		
		return EFalse;
	}
	
	// ----------------------------------------------------------------------------------------
	// FunctionHelper::AcceptableParamType 
	// ----------------------------------------------------------------------------------------

	TBool AcceptableParamType( const TDesC8& aFunctionId, const TDesC8& aParamType, TInt& /*aParamIndex*/ )
	{
		using namespace PolicyLanguage::NativeLanguage::Functions;

		if ( aParamType == PolicyLanguage::NativeLanguage::AttributeValues::StringDataType &&
			( aFunctionId == FunctionStringEqualId ||
			  aFunctionId == FunctionUserAcceptCorpPolicy ||
			  aFunctionId == FunctionRuleTargetStructure ||
			  aFunctionId == TrustedRoleMatch ||
			  aFunctionId == TrustedSubjectMatch ))
			{
				return ETrue;
			}

		if ( aParamType == PolicyLanguage::NativeLanguage::AttributeValues::BooleanDataType &&
			( aFunctionId == FunctionBooleanEqualId ||
			  aFunctionId == FunctionOr ||
			  aFunctionId == FunctionAnd ||
			  aFunctionId == FunctionNot ))
			{
				return ETrue;
			}
		
			return EFalse;
	}

	// ----------------------------------------------------------------------------------------
	// FunctionHelper::IsAcceptableParamsForFunction 
	// ----------------------------------------------------------------------------------------

	TBool IsAcceptableParamsForFunction( const TDesC8& aFunctionId, RElementContainer* aParams )
	{
		using namespace PolicyLanguage::Functions;
		
		TBool returnValue = EFalse;

		if ( AcceptableParamCount( aFunctionId, aParams->Count()))
		{
			returnValue = ETrue;
			
			for ( TInt i = 0; i < aParams->Count() && returnValue; i++ )
			{
				CElementBase * element = (*aParams)[i]->iElement;
				__ASSERT_ALWAYS ( element, User::Panic(PolicyParserPanic, KErrCorrupt));
							
				switch ( element->ElementType())
				{
					case EApply:
					{
						if ( !AcceptableParamType( aFunctionId, ((CApply*)element)->ReturnDataType(), i))
						{
							returnValue = EFalse;
						}
					}
					break;
					case ESubjectAttributeDesignator:
					case EEnvironmentAttributeDesignator:
					case EResourceAttributeDesignator:
					case EActionAttributeDesignator:
					{
						if ( !AcceptableParamType( aFunctionId, ((CAttributeDesignator*) element)->GetDataType(), i))
						{
							returnValue = EFalse;
						}
					}
					break;
					case EAttributeValue:
					{
						if ( !AcceptableParamType( aFunctionId, ((CAttributeValue*) element)->Data()->DataType(ENative), i))
						{
							returnValue = EFalse;
						}
					}
					break;
					default:
					{
						User::Panic( Panics::PolicyParserPanic, KErrAbort);
					}
					break;
				}
			}
		}
		
		return returnValue;
	}
}


// ----------------------------------------------------------------------------------------
// CAttributeValue::CAttributeValue 
// ----------------------------------------------------------------------------------------


CAttributeValue::CAttributeValue()
	: CElementBase()
{
	iElementType = EAttributeValue;	
	iSaveType = ESubElement;
	iElementName.Set( PolicyLanguage::NativeLanguage::AttributeValues::AttributeValue);

}

// ----------------------------------------------------------------------------------------
// CAttributeValue::~CAttributeValue 
// ----------------------------------------------------------------------------------------

CAttributeValue::~CAttributeValue()
{
	delete iDataType;
}
		
// ----------------------------------------------------------------------------------------
// CAttributeValue::NewL 
// ----------------------------------------------------------------------------------------

CAttributeValue* CAttributeValue::NewL()
{
	CAttributeValue * self;
	self = new (ELeave) CAttributeValue();
	return self;
}
// ----------------------------------------------------------------------------------------
// CAttributeValue::NewL 
// ----------------------------------------------------------------------------------------

CAttributeValue * CAttributeValue::NewL( const TDesC8& aAttributeValue, const TDesC8& aDataType)
{
	CAttributeValue * self = new (ELeave) CAttributeValue();
	
	CleanupStack::PushL( self);
	self->ConstructL( aAttributeValue, aDataType);
	CleanupStack::Pop( self);
	
	return self;
}
// ----------------------------------------------------------------------------------------
// CAttributeValue::SetDataL 
// ----------------------------------------------------------------------------------------

void CAttributeValue::SetDataL( const TDesC8& aAttributeValue, const TDesC8& aDataType)
{
	delete iDataType;
	iDataType = NULL;
	
	ConstructL( aAttributeValue, aDataType);
}

// ----------------------------------------------------------------------------------------
// CAttributeValue::ConstructL 
// ----------------------------------------------------------------------------------------

void CAttributeValue::ConstructL( const TDesC8& aAttributeValue, const TDesC8& aDataType)
{
	TInt err = CreateDataTypeL( aDataType);
	
	if ( err == KErrNone )
	{
		iDataType->SetValueL( aAttributeValue);
	}
	else
	{
		User::Leave( KErrNotFound);
	}
}

// ----------------------------------------------------------------------------------------
// CAttributeValue::IdentificateType 
// ----------------------------------------------------------------------------------------

TBool CAttributeValue::IdentificateType( const TDesC8& aElementName)
{
	return ( aElementName == PolicyLanguage::NativeLanguage::AttributeValues::AttributeValue);
}

// ----------------------------------------------------------------------------------------
// CAttributeValue::AddAttributeL 
// ----------------------------------------------------------------------------------------

	
void CAttributeValue::AddAttributeL( CPolicyParser *aParser, const TDesC8& aName, const TDesC8& aValue)
{
	TInt err( KErrNotFound);
	
	if ( aName == PolicyLanguage::NativeLanguage::AttributeValues::DataType )
	{
		 err = CreateDataTypeL( aValue);
	}
	
	if ( err != KErrNone )
	{
		if ( aParser)
		{
			aParser->HandleErrorL( ParserErrors::UnexpectedAttribute, aName);
		}
	}
}
// ----------------------------------------------------------------------------------------
// CAttributeValue::CreateDataTypeL 
// ----------------------------------------------------------------------------------------

TInt CAttributeValue::CreateDataTypeL( const TDesC8& aDataType)
{
	if ( aDataType == PolicyLanguage::NativeLanguage::AttributeValues::StringDataType )
 	{
		iDataType = new (ELeave) CStringType();
		return KErrNone;
	}
	else 
	if ( aDataType == PolicyLanguage::NativeLanguage::AttributeValues::BooleanDataType)
 	{
		iDataType = new (ELeave) CBooleanType();
		return KErrNone;
	}
	
	return KErrNotFound;
}

// ----------------------------------------------------------------------------------------
// CAttributeValue::AddContentL 
// ----------------------------------------------------------------------------------------


void CAttributeValue::AddContentL( CPolicyParser *aParser, const TDesC8& aName)
{
	if ( KErrNone != iDataType->SetValueL( aName))
	{
		aParser->HandleErrorL( ParserErrors::InvalidValue);
	}
}

// ----------------------------------------------------------------------------------------
// CAttributeValue::DecodeElementL 
// ----------------------------------------------------------------------------------------


HBufC8 * CAttributeValue::DecodeElementL( const  TLanguageSelector &aLanguage , const TDecodeMode &/*aMode*/)
{
	User::LeaveIfNull( iDataType);
	
	_LIT8( DecodeFormat, "<%S %S= \"%S\">%S</%S>");	
	
	TInt size(35);
	size += PolicyLanguage::AttributeValues::AttributeValue[ aLanguage].Length();
	size += PolicyLanguage::AttributeValues::DataType[ aLanguage].Length();
	size += iDataType->DataType(aLanguage).Length();
	size += iDataType->Value().Length();
	
	HBufC8 * hbuf = HBufC8::NewLC( size);
	
	hbuf->Des().Format( DecodeFormat, &PolicyLanguage::AttributeValues::AttributeValue[ aLanguage], 
									  &PolicyLanguage::AttributeValues::DataType[ aLanguage],
									  &iDataType->DataType(aLanguage),
									  &iDataType->Value(),
									  &PolicyLanguage::AttributeValues::AttributeValue[ aLanguage]);
	
	CleanupStack::Pop( hbuf);
	
	return hbuf;
}

// ----------------------------------------------------------------------------------------
// CAttributeValue::ValidElement 
// ----------------------------------------------------------------------------------------


TBool CAttributeValue::ValidElement()
{
	return iDataType != NULL;
}

// ----------------------------------------------------------------------------------------
// CAttributeValue::Data 
// ----------------------------------------------------------------------------------------



CDataType * CAttributeValue::Data()
{
	return iDataType;
}
	
// ----------------------------------------------------------------------------------------
// CDataType::CDataType 
// ----------------------------------------------------------------------------------------



CDataType::CDataType()
	: CBase()
{
	
}

// ----------------------------------------------------------------------------------------
// CDataType::~CDataType 
// ----------------------------------------------------------------------------------------


CDataType::~CDataType()
{
}

// ----------------------------------------------------------------------------------------
// CDataType::Compare 
// ----------------------------------------------------------------------------------------

TInt CDataType::Compare( const CDataType */*aData*/) const
{
	return 0;
}

// ----------------------------------------------------------------------------------------
// CDataType::SetValueL 
// ----------------------------------------------------------------------------------------

TInt CDataType::SetValueL( const TDesC8 & /*aValue*/)
{	
	return KErrGeneral;	
}


// ----------------------------------------------------------------------------------------
// CStringType::CStringType 
// ----------------------------------------------------------------------------------------

CStringType::CStringType()
	: CDataType()
{
}

// ----------------------------------------------------------------------------------------
// CStringType::~CStringType 
// ----------------------------------------------------------------------------------------

CStringType::~CStringType()
{
	delete iValue;
}

// ----------------------------------------------------------------------------------------
// CStringType::SetValueL 
// ----------------------------------------------------------------------------------------

TInt CStringType::SetValueL( const TDesC8& aValue)
{
	if ( iValue )
	{
		iValue = iValue->ReAllocL( iValue->Length() + aValue.Length());
		TPtr8 ptr = iValue->Des();
		ptr.Append( aValue);
	
	}
	else
	{
		delete iValue;
		iValue = NULL;
		iValue = aValue.AllocL();
	}
	
	
	
	return KErrNone;
}

// ----------------------------------------------------------------------------------------
// CStringType::Value 
// ----------------------------------------------------------------------------------------

TDesC8& CStringType::Value() const
{
	return *iValue;
}

// ----------------------------------------------------------------------------------------
// CStringType::DataType 
// ----------------------------------------------------------------------------------------

const TDesC8& CStringType::DataType( const TLanguageSelector &aLanguage)
{
	return PolicyLanguage::AttributeValues::StringDataType[aLanguage];
}

// ----------------------------------------------------------------------------------------
// CStringType::Compare 
// ----------------------------------------------------------------------------------------

TInt CStringType::Compare( const CDataType *aData) const
{
	return iValue->Compare( aData->Value()); 
}


// ----------------------------------------------------------------------------------------
// CBooleanType::CBooleanType 
// ----------------------------------------------------------------------------------------

CBooleanType::CBooleanType()
	: CDataType()
{
	
}

// ----------------------------------------------------------------------------------------
// CBooleanType::~CBooleanType 
// ----------------------------------------------------------------------------------------

CBooleanType::~CBooleanType()
{
	delete iValue;
}
		
// ----------------------------------------------------------------------------------------
// CBooleanType::SetValueL 
// ----------------------------------------------------------------------------------------

TInt CBooleanType::SetValueL( const TDesC8 & aValue)
{
	if ( iValue )
	{
		return KErrAlreadyExists;	
	}
	

	
	iValue = aValue.Alloc();
	
	return KErrNone;
}

// ----------------------------------------------------------------------------------------
// CBooleanType::DataType 
// ----------------------------------------------------------------------------------------

const TDesC8& CBooleanType::DataType( const TLanguageSelector &aLanguage)
{
	return PolicyLanguage::AttributeValues::BooleanDataType[aLanguage];
}

// ----------------------------------------------------------------------------------------
// CBooleanType::Value 
// ----------------------------------------------------------------------------------------

TDesC8& CBooleanType::Value() const
{
	return *iValue;
}

// ----------------------------------------------------------------------------------------
// CBooleanType::Compare 
// ----------------------------------------------------------------------------------------

		
TInt CBooleanType::Compare( const CDataType *aData) const
{
	return iValue->Compare( aData->Value()); 
}



