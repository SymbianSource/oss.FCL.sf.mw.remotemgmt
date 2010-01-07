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


#ifndef POLICY_PARSER_HEADER_
#define POLICY_PARSER_HEADER_


// INCLUDES

#include "ElementBase.h"
#include "PolicyEngineClientServer.h"

#include <e32base.h>
#include <xml/contenthandler.h>
#include <xml/documentparameters.h>
#include <cstack.h>
#include <xml/parser.h>

// CONSTANTS

enum TAllowedExternalIdConflicts
{
	KNoExternalIdCheck = 0,
	KSameExternalIdAllowedForRoot,
	KNoExternalIdConflictsAllowed
};


// MACROS
// DATA TYPES
// FUNCTION PROTOTYPES
// FORWARD DECLARATIONS

class CElementBase;
class CPolicyStorage;

// CLASS DECLARATION

namespace ParserUtility
{
	TBool ContainsCDataField( TPtrC8& aChunk);
	void ReadCDataAreaL( TPtrC8& aCDataArea, TPtrC8& aChunk, TPtrC8& aFinalPart);
	void RemoveCDataMarksL( TPtrC8& aData);
	TInt LineCounter( TPtrC8& aData);
}

class CPolicyParser : public CBase, public Xml::MContentHandler
{
	public:
		static CPolicyParser * NewL( CPolicyStorage * aStorage);
		CPolicyParser( CPolicyStorage * aStorage);
		virtual ~CPolicyParser();
	
		void ConstructL();
	
		CElementBase * ParseXACMLObjects( TInt& aError, const TDesC8 &aXACML, TDes8 &aReturnText);
		TInt ParseNativeObjects( CElementBase * aSeedElement, const TDesC8 &aNative, TBool aAddToEditableCache);
		void HandleErrorL( const TDesC8 &aText);
		void HandleErrorL( const TDesC8 &aText, const TDesC8 &aErrorElement);
		void HandleErrorL( const TAny * aPointer, const TDesC8 &aError, const TDesC8 &aErrorElement);
		TDesC8& ActiveElementName();
		void HandleNativeErrorL( TInt aError);
		
		TLanguageSelector Language();
		
		void SetExternalIdChecked( TAllowedExternalIdConflicts aCheckerState);
		void CheckExternalIdL( const HBufC8 * iExternalId);
		void SetLineOffset( TInt aLine);
		
		//Validity Check functions
		void CheckCompiningAlgorithmsL( TNativeElementTypes aType, const TDesC8& aAlgorithmId );
		void CheckFunctionIdL( const TNativeElementTypes& aType, const TDesC8& aFunctionId );
		void CheckEffectL( const TDesC8& aEffect );
		void CheckDataTypeL( const TDesC8& aDataType );
	
		//convert XACML to Native 
		const TDesC8& ConvertElementNamesL( const TDesC8& aElementName );
		const TDesC8& ConvertAttributesL( const TDesC8& aType );
		static const TDesC8& ConvertValues( TLanguageSelector aOriginalLanguage, const TDesC8& aValue);
			
		//from content handler
		void OnStartDocumentL(const Xml::RDocumentParameters& aDocParam, TInt aErrorCode);
    	void OnEndDocumentL(TInt aErrorCode);
    	void OnStartElementL(const Xml::RTagInfo& aElement, const Xml::RAttributeArray& aAttributes, TInt aErrorCode);
    	void OnEndElementL(const Xml::RTagInfo& aElement, TInt aErrorCode);
    	void OnContentL(const TDesC8& aBytes, TInt aErrorCode);
    	void OnStartPrefixMappingL(const RString& aPrefix, const RString& aUri, TInt aErrorCode);
    	void OnEndPrefixMappingL(const RString& aPrefix, TInt aErrorCode);
    	void OnIgnorableWhiteSpaceL(const TDesC8& aBytes, TInt aErrorCode);
    	void OnSkippedEntityL(const RString& aName, TInt aErrorCode);
    	void OnProcessingInstructionL(const TDesC8& aTarget, const TDesC8& aData, TInt aErrorCode);
    	void OnError(TInt aErrorCode);
		TAny* GetExtendedInterface(const TInt32 aUid);
	
	private:
		void CreateIdElementsL( const TDesC8& aIdString);
		void ParseL( const TDesC8 &aXACML);
	
	private: 
		TPtrC8 iActiveElementName;
		TInt iActiveLine;
		TInt iLineOffset;
		Xml::CParser * iParser;
		CStack<CElementBase, EFalse> * iElementStack;
		CElementBase * iActiveElement;
		CElementBase * iRootElement;
		CPolicyStorage * iStorage;
		TLanguageSelector iLanguage;
		TAllowedExternalIdConflicts iCheckerState;
		TBool iErrorHandling;
		TBuf8<KMaxReturnMessageLength> iReturnText;
		TBool iAddToEditableCache;
		TBool iValidCharacterSet;
};


#endif