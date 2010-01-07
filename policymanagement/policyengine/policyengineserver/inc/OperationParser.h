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


#ifndef OPERATION_PARSER_HEADER_
#define OPERATION_PARSER_HEADER_

// INCLUDES

// INCLUDES

#include "ElementBase.h"
#include "PolicyEngineClientServer.h"

#include <e32base.h>
#include <xml/contenthandler.h>
#include <xml/documentparameters.h>
#include <cstack.h>
#include <xml/parser.h>

// CONSTANTS
// MACROS
// DATA TYPES
// FUNCTION PROTOTYPES
// FORWARD DECLARATIONS

class CElementBase;
class CPolicyStorage;
class CPolicyManager;

// CLASS DECLARATION

class COperationInfo	: 	public CBase
{
	public:
		COperationInfo();
		~COperationInfo();
		static COperationInfo* NewL();
		
		void SetActionIdL( const TDesC8& aActionId);
		void SetTargetElementIdL( const TDesC8& aTargetElementId);
		void SetXACMLContentL( const TPtrC8& aXACMLContent);
		void SetLineOffset( const TInt& aLineOffset);
		void UseBearerCertificate( const TBool aUseBearerCertificate);
		
		void ExecuteL( CPolicyManager *aManager);
		
	private:
		HBufC8 *iActionId;
		HBufC8 *iTargetElement;	
		TBool iUseBearerCertificate;
		TPtrC8 iXACMLContent;
		TInt iLineOffset;
};

class COperationParser : public CBase, public Xml::MContentHandler
{
	public:
		COperationParser();
		~COperationParser();
		static COperationParser * NewL( const TDesC8& aOperationDescription);
		void ConstructL( const TDesC8& aOperationDescription);
	
		//operation handling
		/*
		* NextOperationL();
		*/
		void ExecuteL( TInt aIndex, CPolicyManager *aManager);
		TInt OperationCount();
		void ParseOperationsL( TDes8& aReturnTxt);
		
	private:
		void HandleErrorL( const TDesC8& aErrorTxt, const TDesC8& aElementName);
		void ResetOperations();
	
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
		Xml::CParser *iParser;
		COperationInfo * iActiveOperation;
		RArray<COperationInfo *> iOperations;

		TInt iXMLStackPointer;
		TInt iActiveLine;
		TDes8* iReturnText;
		TPtrC8 iXMLContent;
};


#endif