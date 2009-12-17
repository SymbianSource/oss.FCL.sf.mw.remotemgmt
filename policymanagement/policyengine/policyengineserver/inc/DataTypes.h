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


#ifndef DATA_TYPES_HEADER_
#define DATA_TYPES_HEADER_

// INCLUDES

#include "ElementBase.h"
#include <e32base.h>

// CONSTANTS
// MACROS
// DATA TYPES
// FUNCTION PROTOTYPES
// FORWARD DECLARATIONS

class CDataType;

// CLASS DECLARATION

namespace FunctionHelper
{
	TBool IsAcceptableParamsForFunction( const TDesC8& aFunctionId, RElementContainer* aParams );
}

class CAttributeValue : public CElementBase
{
	public:
		CAttributeValue();
		
		virtual ~CAttributeValue();
		static CAttributeValue * NewL();
		static CAttributeValue * NewL( const TDesC8& aAttributeValue, const TDesC8& aDataType);
		
		void SetDataL( const TDesC8& aAttributeValue, const TDesC8& aDataType);
		
		static TBool IdentificateType( const TDesC8& aElementName);
		
		HBufC8 * DecodeElementL( const  TLanguageSelector &aLanguage, const TDecodeMode &aMode);		
		
		CDataType * Data();

		TBool ValidElement();
	private:	
		void AddAttributeL( CPolicyParser *aParser, const TDesC8& aName, const TDesC8& aValue);
		void AddContentL( CPolicyParser *aParser, const TDesC8& aName);
		
		TInt CreateDataTypeL( const TDesC8& aDataType);
		void ConstructL( const TDesC8& aAttributeValue, const TDesC8& aDataType);
	protected:
		CDataType * iDataType;
};

class CDataType : public CBase
{
	public:
		CDataType();
		virtual ~CDataType();
		virtual TInt Compare( const CDataType *aData) const;
		
		virtual TInt SetValueL( const TDesC8 & aValue);
		virtual TDesC8& Value() const = 0;
		virtual const TDesC8& DataType( const TLanguageSelector &aLanguage) = 0;
		
	private:
		TInt iTypeIdentifier;
};

		
class CStringType : public CDataType
{
	public:
		CStringType();
		~CStringType();
		
		virtual TInt SetValueL( const TDesC8 & aValue);
		virtual TDesC8& Value() const;
		virtual const TDesC8& DataType( const TLanguageSelector &aLanguage);
		
		virtual TInt Compare( const CDataType *aData) const;
	private:
		HBufC8 * iValue;
		
};

class CBooleanType : public CDataType
{
	public:
		CBooleanType();
		~CBooleanType();
		
		virtual TInt SetValueL( const TDesC8 & aValue);
		virtual TDesC8& Value() const;
		virtual const TDesC8& DataType( const TLanguageSelector &aLanguage);
		
		virtual TInt Compare( const CDataType *aData) const;
	private:
		HBufC8 * iValue;
		
};


#endif


