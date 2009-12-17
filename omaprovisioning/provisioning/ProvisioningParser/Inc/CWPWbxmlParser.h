/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  CWPWbxmlParser parses WBXML data with builder pattern
*
*/


#ifndef CWPWBXMLPARSER_H
#define CWPWBXMLPARSER_H

// INCLUDES
#include <e32base.h>
// DATA TYPES
typedef struct NW_TinyTree_Node_s NW_TinyTree_Node_t;
typedef NW_TinyTree_Node_t NW_DOM_DocumentNode_t;
typedef NW_TinyTree_Node_t NW_DOM_Node_t;
typedef NW_DOM_Node_t NW_DOM_ElementNode_t;
typedef struct NW_DOM_AttributeHandle_s NW_DOM_AttributeHandle_t;

typedef struct NW_TinyDom_Handle_s NW_TinyDom_Handle_t;

typedef struct NW_String_String_s NW_String_String_t;
typedef struct NW_String_String_s NW_String_t;

typedef struct NW_String_UCS2Buff_s NW_String_UCS2Buff_t;

typedef unsigned long	NW_Uint32;
typedef signed   long   NW_Int32;
typedef unsigned short	NW_Uint16;
typedef unsigned char	NW_Byte; 
typedef unsigned short	NW_Ucs2; 

typedef struct NW_WBXML_Dictionary_s NW_WBXML_Dictionary_t;

typedef NW_TinyTree_Node_t NW_DOM_DocumentNode_t;

// FORWARD DECLARATIONS
class MWPBuilder;

// CLASS DECLARATION

/**
*  Class CWPNameValuePair for passing attribute name and value
*  @lib ProvisioningParser
*  @since 2.0
*/
class CWPNameValuePair : public CBase
	{
	public:
        /**
        * C++ default constructor.
        */
		CWPNameValuePair();

        /**
        * Destructor.
        */
		~CWPNameValuePair();

	public:
		/**
		* Sets the values for a string pair.
		* @param aValue The value of the string
		*/
		void SetValue( HBufC* aValue );
		/**
		* Gets the value of a string pair.
		* @param aValue The value of the string
		*/
		const TDesC& Value() const;


	private:
		// The value of the pair. Owns.
		HBufC* iValue;
	};

/**
*  Class CWPWbxmlParser parses wbxml data received from ProvisioningEngine
*  Parsed data is built back into ProvisioningEngine.
*
*  @lib ProvisioningParser
*  @since 2.0
*/
class CWPWbxmlParser : public CBase
	{
	public:  // Constructors and destructor

        /**
        * Two-phased constructor.
        */
		IMPORT_C static CWPWbxmlParser* NewL();

		/**
        * Destructor.
        */
		virtual ~CWPWbxmlParser();

	public:	  // New functions

		/**
        * Parses WBXML document and builds data model
        * @since 2.0
        * @param aDocument Reference a wbxml document
		* @param aRoot Pointing to data model builder 
        */
		IMPORT_C void ParseL(const TDesC8& aDocument, MWPBuilder& aRoot );


	private:  

        /**
        * C++ default constructor.
        */
		CWPWbxmlParser();

        /**
        * By default Symbian 2nd phase constructor is private.
        */
		void ConstructL();

		/* 
		*	Cleanup parser handle 
		*/		
		static void Cleanup( TAny *aPtr );

		/**
        * Parses WBXML document
        * @since 2.0
        * @param aDocument Pointer to a document node
        * @param aRoot Reference to a document builder
        * @return parser status
        */
		NW_Uint16 ParseDocumentL( NW_DOM_DocumentNode_t* aDocument,
							MWPBuilder& aRoot );
		/**
        * Parses node in DOM tree
        * @since 2.0
        * @param aNode Pointing to DOM tree node
		* @param aEncoding Wbxml document encoding ( UTF8 )
		* @param aRoot Pointing to data model builder 
        * @return parser status
        */
		NW_Uint16 ParseNodeL( NW_DOM_Node_t* aNode, NW_Uint32 aEncoding,
						MWPBuilder& aRoot );
		/**
        * Parses XML Element
        * @since 2.0
        * @param aElement Pointing to DOM tree element
		* @param aEncoding Wbxml document encoding ( UTF8 )
		* @param aRoot Pointing to data model builder 
        * @return parser status
        */
		void ParseElementL( NW_DOM_ElementNode_t* aElement,
							NW_Uint32 aEncoding, MWPBuilder& aRoot );
		/**
        * Parses XML Element attributes
        * @since 2.0
        * @param aAttrHandle Pointing to a handle to the attribute
		* @param aEncoding Wbxml document encoding ( UTF8 )
		* @param aPair for passing attribute name and value
        * @return attribute token or error code
        */
		NW_Uint16 ParseAttributeL( NW_DOM_AttributeHandle_t* aAttrHandle,
									NW_Uint32 aEncoding,
									 CWPNameValuePair& aPair );

		/**
        * Processes attribute string and returns the value in HBuFC
        * @since 2.0
        * @param aString Source string
		* @param aEncoding Wbxml document encoding ( UTF8 )
		* @param aStatus Current processing status
        * @return HBufC containing the source string, ownership is transferred
        */
		HBufC* ProcessAttributePartL( NW_String_t* aString,											
														NW_Uint32 aEncoding,
														NW_Uint16 aStatus );

	private: // member variables
		NW_WBXML_Dictionary_t* iDictionary[1]; // single dictionary, owned
		NW_DOM_DocumentNode_t* iDocNode;	// used
		TInt iRecursionDepth;
		TInt iErrCode;
		HBufC* iParameterName;
	

	};

#endif	// CWPWBXMLPARSER_H
            
// End of File
