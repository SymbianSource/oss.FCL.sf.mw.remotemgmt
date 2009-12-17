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


#ifndef ELEMENT_BASE_HEADER_
#define ELEMENT_BASE_HEADER_

// INCLUDES

#include <e32base.h>

#include <PolicyEngineClientServer.h>

// CONSTANTS

const int KMaxLengthPolicyElementID = 30;
const int KNotInElementCache = -1;
const int KIdLength = 8;

enum TNativeElementTypes
{		
	EPolicy = 0,
	EPolicySet,
	ESubjectAttributeDesignator,
	EActionAttributeDesignator,
	EEnvironmentAttributeDesignator,
	EResourceAttributeDesignator,
	EAttributeValue,
	ESubjectMatch,
	EActionMatch,
	EResourceMatch,
	EEnvironmentMatch,	
	ESubject,
	EAction,
	EResource,
	EEnvironment,	
	ESubjects,
	EActions,
	EResources,
	EEnvironments,
	ETarget,
	ERule,
	ECondition,
	EAttribute,
	EDescription,
	EApply
};


enum TElementState
{
	//Brand-new element. Storage automatically saves also all subelements
	ENewElement,
	//Element is created (and added into the cache), but its subelements are not loaded
	ENotLoaded,
	//Element is loaded and added into the cache, subelements are loaded
	ECacheElement,
	//Element is created (and added into the editable cache), but its subelements are not loaded
	ENotLoadedEditableElement,
	//Element is loaded and added into the editable cache, subelements are loaded
	EEditableCacheElement,
	//element is in the editable cache (element is loaded) and contains changes, 
	//saved to disk during commit. Recursive save is not possible for EEditedElement!
	EEditedElement,
	//element is in the editable cache and is deleted from disk (during commit)
	EDeletedEditableElement,
	//element is in the cache, but disk contains newer version
	EDepricated
};

enum TDecodeMode
{
	EFullMode,
	EIdMode	
};

enum TSaveType
{
	EOwnElement,
	ESubElement
};

enum TLanguageSelector
{
	EXACML = 0,
	ENative = 1
};

enum TMatchResponse
{
	EDeny = 0,
	EPermit,
	ENotApplicable,
	EIndeterminate,
	EMatch,
	EUnMatch
};

_LIT( PolicyParserPanic, "Policy Parser Panic!");
_LIT( PolicyExecutionPanic, "Policy execution Panic!");
_LIT( PolicyStoragePanic, "Policy storage Panic!");

// MACROS
// DATA TYPES
// FUNCTION PROTOTYPES
// FORWARD DECLARATIONS

class CElementBase;
class TUnionTarget;
class CAttributeValue;
class CPolicyStorage;
class CPolicyParser;
class CPolicyProcessor;
class CMatchObject;

// CLASS DECLARATION




namespace ElementHelper
{
	HBufC8 * DecodeEmptyElement( const TDesC8& aElementName);
	CElementBase * CreateNewElementL( const TUint32& aElementId);
	TInt CompareElements( CElementBase* const& aElement1, CElementBase* const& aElement2);
	TInt XACMLCompare( const TDesC8& aTag, const TPtrC8 aTags[2]);
}

class TElementContainer
{	
	public:
	CElementBase * iElement;
	TBool iDeleteWithElement;
};

typedef RArray<TElementContainer*> RElementContainer;
typedef RArray<TUint32> RIdList;
typedef RArray<CMatchObject*> RMatchObjectArray;
typedef RArray<CElementBase*> RElementArray;

class MElementBase
{
	public:
		virtual void SetId( const TUint32& aElementId) = 0;
		
		virtual TUint32 GetId() const = 0;
		virtual HBufC8 * ExternalId() const = 0;
		
		virtual TNativeElementTypes ElementType() = 0;
		
		virtual HBufC8 * DecodeElementL( const TLanguageSelector &aLanguage, const TDecodeMode &aMode) = 0;
		virtual HBufC8 * DecodeIdL() = 0;
				
		//Storage functions
		virtual TInt SaveElementL( TBool aRecursive) = 0;
		
		//Execution functions
		virtual TMatchResponse MatchL( CPolicyProcessor* aPolicyProcessor) = 0;
		virtual CAttributeValue *  ApplyValueL( CPolicyProcessor* aPolicyProcessor) = 0;
		
	protected:
		virtual void AddIdElementL( CElementBase * aElement) = 0;
		
	private:	
		//Parsing functions
		virtual void AddAttributeL( CPolicyParser *aParser, const TDesC8& aName, const TDesC8& aValue) = 0;
		virtual void AddContentL( CPolicyParser *aParser, const TDesC8& aName) = 0;
		virtual void AddElementL( CPolicyParser *aParser, CElementBase * aElement) = 0;			
		virtual TBool ValidElement() = 0;
};

class CElementBase : public CBase, public MElementBase
{
	public:
		CElementBase();
		CElementBase( const TUint32& aElementId);
		virtual ~CElementBase();
	
		virtual void SetId( const TUint32& aElementId);
		void SetElementType( const TNativeElementTypes& aElementType);
		void CreateIdL();
		TUint32 GetId() const;
		void AddParentIdL( TUint32 iMotherId);
		virtual HBufC8 * ExternalId() const;
		
		
		TNativeElementTypes ElementType();
		TBool IdentificateElement( const TDesC8& aElementName );
		
		HBufC8 * DecodeElementL( const  TLanguageSelector &aLanguage, const TDecodeMode &aMode);
		HBufC8 * DecodeIdL();
		const TDesC8 & SelectCorrectValue( const TLanguageSelector &aLanguage, const TDesC8& aOriginalValue ) const;

		TInt ElementCount();
		CElementBase * Element( TInt aIndex);
		TNativeElementTypes SubElementType( const TInt& aIndex, const TInt& aExpectedCount = -1);
		
		//Management functions
		HBufC8 * DescriptionL();
		TInt GetChildListLengthL();
		void GetChildListL( TDes8& aChilds);
		TInt GetElementsListLengthL( TElementType aType );
		void GetElementsListL( TElementType aType, TDes8& aChilds);
		TInt RemoveChild( const TUint32& aChildId);
		void CheckAllElementsL();
		RElementContainer* Container();
		void FindAttributesL( TNativeElementTypes aAttributeType, RElementArray& aValuesList);
		void CreateExternalId();
		TInt GetChildElementCountL();
	
		//Storage functions
		TInt SaveElementL( TBool aRecursive);
		void DeleteElementL();
		void DeleteMarkRecursiveL();
		void ReleaseElement();
		void ReserveElement();

		//Execution functions
		virtual TMatchResponse MatchL( CPolicyProcessor* aPolicyProcessor);
		virtual CAttributeValue * ApplyValueL( CPolicyProcessor* aPolicyProcessor);
//		void AbsoluteTargetL( TUnionTarget *aTarget);


		void AddToElementListL( CElementBase * aElement, TBool aDeleteWithElement);
	protected:
		HBufC8 * DecodeElementOrIdL( const TLanguageSelector &aLanguage, CElementBase * aElement, const TDecodeMode &aMode);		
		void CheckContentL( const TDesC8& aName);
		void AddIdElementL( CElementBase * aElement);
		
	
	private:	
		//Parsing functions
		void AddAttributeL( CPolicyParser *aParser, const TDesC8& aName, const TDesC8& aValue);
		void AddContentL( CPolicyParser *aParser, const TDesC8& aName);
		void AddElementL( CPolicyParser *aParser, CElementBase * aElement);
		virtual TBool ValidElement();
		
		
	protected:
		TElementState iElementState;
		TSaveType iSaveType; 
		TNativeElementTypes iElementType;
		TBool iCreateExternalId;
		TPtrC8 iElementName;
		HBufC8 * iExternalId;
	private: 	//Variables
		RElementContainer iElements;
		TUint32 iParentId;
		TUint32 iElementId;
		TInt16 iReferenceCount;
		
		friend class CPolicyParser;
		friend class CPolicyStorage;
		friend class CElementBase;
		friend class CPolicyManager;
		friend class CCertificateMaps;
		friend class TUnionTarget;
		
};


#endif


