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


#ifndef ELEMENTS_HEADER_
#define ELEMENTS_HEADER_

// INCLUDES

#include "ElementBase.h"
#include "XACMLconstants.h"
#include "RequestContext.h"
#include "PolicyStorage.h"

#include <e32base.h>

// CONSTANTS
// MACROS
// FORWARD DECLARATIONS

class CAttributeDesignator;
class CAttributeValue;
class CPolicyProcessor;
class CMatchObject;
class CItemContainer;
class CTarget;

// DATA TYPES

typedef RArray<HBufC*> RAttributeValuesList;

// FUNCTION PROTOTYPES
// CLASS DECLARATION

/*
class TUnionTarget
{
	public:
		TUnionTarget( CPolicyStorage * aPolicyStorage);
		~TUnionTarget();
		void AddTargetElementsL( CElementBase * aElement);
		
		RMatchObjectArray * GetSubjectAttributes();
		RMatchObjectArray * GetResourceAttributes();
		RMatchObjectArray * GetActionAttributes();
		RMatchObjectArray * GetEnviromentAttributes();
		
		CMatchObject * FindSubjectAttribute( const TDesC8 &aAttributeId);
		CMatchObject * FindActionAttribute( const TDesC8 &aAttributeId);
		CMatchObject * FindResourceAttribute( const TDesC8 &aAttributeId);
		CMatchObject * FindEnvironmentAttribute( const TDesC8 &aAttributeId);
	
	private:
		CMatchObject * FindAttribute( const TDesC8 &aAttributeId, RMatchObjectArray *aMatchObjectArray);
		
	private:		
		CPolicyStorage * iPolicyStorage;
		TElementReserver iReserver;
		
		RMatchObjectArray iSubjectAttributes;
		RMatchObjectArray iResourceAttributes;
		RMatchObjectArray iActionAttributes;
		RMatchObjectArray iEnvironmentAttributes;
};*/

class RContentBuilder : public RPointerArray<HBufC8>
{
	public:
		RContentBuilder();
	
		void AddContentL( HBufC8 * aBuffer);
		HBufC8 * ContentL();
		HBufC8 * ContentLC();
		
		void Close();		
	private:
		TInt iSize;
};

class CApply : public CElementBase
{
	public:
			CApply();
			~CApply();
		
			static CApply * NewL();
			static TBool IdentificateType( const TDesC8& aElementName);

			HBufC8 * DecodeElementL( const  TLanguageSelector &aLanguage, const TDecodeMode &aMode);

			void AddAttributeL( CPolicyParser *aParser, const TDesC8& aName, const TDesC8& aValue);
			void AddElementL( CPolicyParser *aParser, CElementBase * aElement);
	
			void AddIdElementL( CElementBase * aElement);	
			virtual TBool ValidElement();
		
			CAttributeValue * ApplyValueL( CPolicyProcessor* aPolicyProcessor);
			
			const TPtrC8 ReturnDataType();
		
		private:
			void ConstructL();
			HBufC8 * iFunctionId;
			CAttributeValue * iResponseValue;			
};

class CCondition : public CElementBase
{
	public:
		CCondition();
		~CCondition();
		
		static CCondition * NewL();
		static TBool IdentificateType( const TDesC8& aElementName);

		HBufC8 * DecodeElementL( const  TLanguageSelector &aLanguage, const TDecodeMode &aMode);

		void AddElementL( CPolicyParser *aParser, CElementBase * aElement);
	
		void AddIdElementL( CElementBase * aElement);	
		virtual TBool ValidElement();
		
		TMatchResponse ConditionValueL( CPolicyProcessor* aPolicyProcessor);
	private:
		CElementBase * iExpression;
};


class CPolicySet : public CElementBase
{
	public:
		CPolicySet();
		virtual ~CPolicySet();

		static CPolicySet * NewL();
		static TBool IdentificateType( const TDesC8& aElementName);

		HBufC8 * DecodeElementL( const  TLanguageSelector &aLanguage, const TDecodeMode &aMode);

		void AddAttributeL( CPolicyParser *aParser, const TDesC8& aName, const TDesC8& aValue);
		void AddElementL( CPolicyParser *aParser, CElementBase * aElement);
	
		TMatchResponse MatchL( CPolicyProcessor* aPolicyProcessor);
		void AddIdElementL( CElementBase * aElement);	
		
		virtual TBool ValidElement();
	private:
		HBufC8 * iPolicySetId;
		HBufC8 * iPolicyCompiningAlg;
		CElementBase * iTarget;
		CElementBase * iDescription;
};

class CPolicy : public CElementBase
{
	public:
		CPolicy();
		virtual ~CPolicy();

		static CPolicy * NewL();
		static TBool IdentificateType( const TDesC8& aElementName);

		HBufC8 * DecodeElementL( const  TLanguageSelector &aLanguage, const TDecodeMode &aMode);

		void AddAttributeL( CPolicyParser *aParser, const TDesC8& aName, const TDesC8& aValue);
		void AddElementL( CPolicyParser *aParser, CElementBase * aElement);
	
		TMatchResponse MatchL( CPolicyProcessor* aPolicyProcessor);
		void AddIdElementL( CElementBase * aElement);	

		virtual TBool ValidElement();
		
	private:
		HBufC8 * iPolicyId;
		HBufC8 * iRuleCompiningAlg;
		CElementBase * iDescription;
		CElementBase * iTarget;
};

class CRule : public CElementBase
{
	public:
		CRule();
		virtual ~CRule();
		
		static CRule * NewL();
		static TBool IdentificateType( const TDesC8& aElementName);
			
		HBufC8 * DecodeElementL( const  TLanguageSelector &aLanguage, const TDecodeMode &aMode);

		TMatchResponse MatchL( CPolicyProcessor* aPolicyProcessor);
		TMatchResponse Effect();
		
		const TDesC8& EffectValue();
		CTarget* GetTarget();

		void AddIdElementL( CElementBase * aElement);
		void SetRuleIdL( const TDesC8& aName);
		void SetEffectL( const TDesC8& aEffect);

		virtual TBool ValidElement();
		void AddAttributeL( CPolicyParser *aParser, const TDesC8& aName, const TDesC8& aValue);
		void AddElementL( CPolicyParser *aParser, CElementBase * aElement);
	private: 
		
	private:
		CElementBase * iDescription;
		HBufC8 * iEffect;
		HBufC8 * iRuleId;
		
		CElementBase * iTarget;
		CElementBase * iCondition;
};

class CTarget : public CElementBase
{
	public:
		CTarget();
		virtual ~CTarget();

		static CTarget * NewL();
		static TBool IdentificateType( const TDesC8& aElementName);
			
		HBufC8 * DecodeElementL( const  TLanguageSelector &aLanguage, const TDecodeMode &aMode);
		TMatchResponse MatchL( CPolicyProcessor* aPolicyProcessor);
		void AddIdElementL( CElementBase * aElement);

		virtual TBool ValidElement();
		
		void GetMatchContainersL( RMatchObjectArray& aMatchContainer );
		
		//Parsing functions
		void AddElementL( CPolicyParser *aParser, CElementBase * aElement);
	private:	
		
	private:
		CElementBase * iSubjects;
		CElementBase * iActions;
		CElementBase * iResources;
		CElementBase * iEnvironments;			
};

class CItemContainer : public CElementBase
{
	public:
		virtual ~CItemContainer();
		
		HBufC8 * DecodeElementL( const  TLanguageSelector &aLanguage, const TDecodeMode &aMode  );
		TMatchResponse MatchL( CPolicyProcessor* aPolicyProcessor);
		void AddIdElementL( CElementBase * aElement);
		
		virtual TBool ValidElement();
		
		void GetMatchContainersL( RMatchObjectArray& aMatchContainer );
	protected:
		CItemContainer();

	private:
		void AddElementL( CPolicyParser *aParser, CElementBase * aElement);

	protected:
		enum TBooleanType
		{
			EOrBoolean,
			EAndBoolean
		};
	
		TNativeElementTypes iAllowedType;
		TBooleanType iBooleanType;
};

class CSubjects : public CItemContainer
{
	public:
		CSubjects();
		virtual ~CSubjects();
		
		static CSubjects * NewL();
		static TBool IdentificateType( const TDesC8& aElementName);
	private:
};


class CSubject : public CItemContainer
{
	public:
		CSubject();
		virtual ~CSubject();
		
		static CSubject * NewL();
		static TBool IdentificateType( const TDesC8& aElementName);
	private:
};

class CActions : public CItemContainer
{
	public:
		CActions();
		virtual ~CActions();
		
		static CElementBase * NewL();
		static TBool IdentificateType( const TDesC8& aElementName);
	private:
};


class CAction : public CItemContainer
{
	public:
		CAction();
		virtual ~CAction();
		
		static CElementBase * NewL();
		static TBool IdentificateType( const TDesC8& aElementName);
	private:
};

class CResources : public CItemContainer
{
	public:
		CResources();
		virtual ~CResources();
		
		static CElementBase * NewL();
		static TBool IdentificateType( const TDesC8& aElementName);
	private:
};


class CResource : public CItemContainer
{
	public:
		CResource();
		virtual ~CResource();
		
		static CElementBase * NewL();
		static TBool IdentificateType( const TDesC8& aElementName);
	private:
};

class CEnvironments : public CItemContainer
{
	public:
		CEnvironments();
		virtual ~CEnvironments();
		
		static CElementBase * NewL();
		static TBool IdentificateType( const TDesC8& aElementName);
	private:
};


class CEnvironment : public CItemContainer
{
	public:
		CEnvironment();
		virtual ~CEnvironment();
		
		static CElementBase * NewL();
		static TBool IdentificateType( const TDesC8& aElementName);
	private:
};




class CAttribute : public CElementBase
{
	public:
		CAttribute();
		virtual ~CAttribute();
		
		static CAttribute * NewL( const TDesC8& aAttributeId, const TDesC8& aAttributeValue, const TDesC8& aDataType);
		static CAttribute * NewL( const TDesC8& aAttributeId, const TBool& aAttributeValue);
		void ConstructL( const TDesC8& aAttributeId, const TDesC8& aAttributeValue, const TDesC8& aDataType);
	
	
		CAttributeValue * AttributeValue();
		TDesC8& AttributeId();
		TDesC8& AttributeType();	
	
	private:
		CAttributeValue * iAttributeValue;
		HBufC8 * iAttributeId;
		HBufC8 * iDataType;
};

class CAttributeDesignator : public CElementBase
{
	public:
		CAttributeDesignator();
		virtual ~CAttributeDesignator();
		static CElementBase * NewL( TNativeElementTypes aType);
		
		HBufC8 * DecodeElementL( const  TLanguageSelector &aLanguage, const TDecodeMode &aMode  );
		
		TDesC8& GetAttributeid();
		TDesC8& GetDataType();
		
		CAttribute * GetAttributeFromRequestContext( CPolicyProcessor * iRequestContext);
		
		virtual TBool ValidElement();
		void AddAttributeL( CPolicyParser *aParser, const TDesC8& aName, const TDesC8& aValue);
	private:	
	protected:		
		HBufC8 * iAttributeId;
		HBufC8 * iDataType;
};

class CSubjectAttributeDesignator : public CAttributeDesignator
{
	public:
		CSubjectAttributeDesignator();
		virtual ~CSubjectAttributeDesignator();

		static CSubjectAttributeDesignator * NewL();
		static TBool IdentificateType( const TDesC8& aElementName);
		
	private:
};

class CActionAttributeDesignator : public CAttributeDesignator
{
	public:
		CActionAttributeDesignator();
		virtual ~CActionAttributeDesignator();

		static CElementBase * NewL();
		static TBool IdentificateType( const TDesC8& aElementName);
		
	private:
};

class CResourceAttributeDesignator : public CAttributeDesignator
{
	public:
		CResourceAttributeDesignator();
		virtual ~CResourceAttributeDesignator();

		static CElementBase * NewL();
		static TBool IdentificateType( const TDesC8& aElementName);
		
	private:
};

class CEnvironmentAttributeDesignator : public CAttributeDesignator
{
	public:
		CEnvironmentAttributeDesignator();
		virtual ~CEnvironmentAttributeDesignator();

		static CElementBase * NewL();
		static TBool IdentificateType( const TDesC8& aElementName);
		
	private:
};


class CMatchObject : public CElementBase
{
	public:
		CMatchObject( TNativeElementTypes aElementType);
		CMatchObject( const TDesC8& aElementName);
		virtual ~CMatchObject();
	
		void AddAttributeL( CPolicyParser *aParser, const TDesC8& aName, const TDesC8& aValue);
		void AddElementL( CPolicyParser *aParser, CElementBase * aElement);
	
		static CElementBase * NewL( const TDesC8& aElementName);
		static CElementBase * NewL( TNativeElementTypes aElementType);
		
		
		static TBool IdentificateType( const TDesC8& aElementName);
		HBufC8 * DecodeElementL( const  TLanguageSelector &aLanguage, const TDecodeMode &aMode  );
		
		CAttributeDesignator* AttributeDesignator();
		CAttributeValue* AttributeValue();
		HBufC8* MatchId();
		
		TMatchResponse MatchL( CPolicyProcessor* aPolicyProcessor);

		TBool CheckMatchObject( const TDesC8& aType, const TDesC8& aAttribute, const TDesC8& aValue);
		

		void AddIdElementL( CElementBase * aElement);		
		virtual TBool ValidElement();
	private: 
		HBufC8 * iMatchId;
		
		CAttributeDesignator* iDesignator;
		CAttributeValue* iMatchValue;
};


class CDescription : public CElementBase
{
	public:
		CDescription();
		virtual ~CDescription();
	
		static CElementBase * NewL();
		
		static TBool IdentificateType( const TDesC8& aElementName);
		HBufC8 * DecodeElementL( const  TLanguageSelector &aLanguage, const TDecodeMode &aMode  );
		HBufC8 * DescriptionText();
		
		void AddContentL( CPolicyParser *aParser, const TDesC8& aName);

		virtual TBool ValidElement();
	private: 
		HBufC8 * iDescription;
		
};


#endif


