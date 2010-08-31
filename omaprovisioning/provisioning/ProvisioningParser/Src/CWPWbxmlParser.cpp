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
* Description:  CWPWbxmlParser parses WBXML data using builder design pattern
*
*/


// INCLUDE FILES
#include <stdlib.h>  // included for free()
#include "MWPBuilder.h"
#include "CWPWbxmlParser.h"
#include "OMAProvisioningDictionary.h"
#include "ProvisioningDebug.h"
#include <xml/cxml/nw_cXML_Parser.h>

// EXTERNAL DATA STRUCTURES
extern "C" NW_WBXML_Dictionary_t NW_omawapprovisioning_WBXMLDictionary;

// CONSTANTS
const NW_Uint16 KParseError = 0xffff;
const NW_Uint16 KParseStatusOk = 0;
const TUint KWapProvisioningDocToken = 0x05;
const TUint KCharacteristicToken = 0x06;
const NW_Uint16 KTokenValueMask = 0xff;
//const TUint16 KTokenPageMask = 0xff00; // Commented to remove warning in armv5
const TUint KAttribValMin = 128;
const TUint KParmNameToken = 0x05;
const TUint KCharTypeToken = 0x50;
const TUint KParmStartValueToken = 0x06;
const TUint KParmToken = 0x07;
const TUint KFromTag = 0x0201;
//const TUint KMaxBandwidthTag = 0x0202; // Commented to remove warning in armv5
//const TUint KMaxUdpPortTag = 0x0203; // Commented to remove warning in armv5
//const TUint KMinUdpPortTag = 0x0204; // Commented to remove warning in armv5
//const NW_Int32 publicID = 0x0b; // Commented to remove warning in armv5
const TUint KDictionaryCount = 1;
const TInt KHeaderLength = 4;
// binary values of the clashing names in different code pages
//const TInt KDuplicateTagTable[]={ 0x06,0x07 };
const TInt KDuplicateCharacteristicTable[]={ 0x53,0x58 };
const TInt KDuplicateParmAttributeStartTable[]={ 0x07, 0x14, 0x1C,
												 0x22, 0x23, 0x24 };
//const TInt KDuplicateAddrtypeValueTable[]={ 0x86, 0x87, 0x88 }; // Commented to remove warning in armv5

_LIT(KFromStr,"FROM");
//_LIT(KMaxBandwidthStr,"MAX-BANDWIDTH"); // Commented to remove warning in armv5
//_LIT(KMaxUdpPortStr,"MAX-UDP-PORT"); // Commented to remove warning in armv5
//_LIT(KMinUdpPortStr,"MIN-UDP-PORT"); // Commented to remove warning in armv5

// ============================ MEMBER FUNCTIONS ===============================


// -----------------------------------------------------------------------------
// CWPNameValuePair::CWPNameValuePair
// C++ default constructor.
// -----------------------------------------------------------------------------
//
CWPNameValuePair::CWPNameValuePair()
	{
	}

// -----------------------------------------------------------------------------
// CWPNameValuePair::~CWPNameValuePair
// Destructor
// -----------------------------------------------------------------------------
//
CWPNameValuePair::~CWPNameValuePair()
	{
	delete iValue;
	}

// ---------------------------------------------------------
// CWPStringPair::SetValue
// ---------------------------------------------------------
//
void CWPNameValuePair::SetValue( HBufC* aValue )
	{
	delete iValue;
	iValue = aValue;
	}

const TDesC& CWPNameValuePair::Value() const
	{
	if ( iValue )
		{
		return *iValue;
		}
	else
		{
		return KNullDesC;
		}
	}

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CWPWbxmlParser::CWPWbxmlParser
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CWPWbxmlParser::CWPWbxmlParser()
	{
	}

// -----------------------------------------------------------------------------
// CWPWbxmlParser::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CWPWbxmlParser::ConstructL()
	{

	iDocNode = NULL;	
	// A dictionary containing OMA provisioning WBXML tokens:
	iDictionary[0] = &NW_omawapprovisioning_WBXMLDictionary;
	TInt err = NW_WBXML_Dictionary_initialize ( KDictionaryCount, iDictionary );
	switch( err )
		{
		case NW_STAT_OUT_OF_MEMORY:
			{
			User::Leave( KErrNoMemory ); 
			break;
			}		
		case NW_STAT_FAILURE:
			{
			User::Leave( KErrGeneral ); 
			break;
			}
		default:
		break;
		}

	}

// -----------------------------------------------------------------------------
// CWPWbxmlParser::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CWPWbxmlParser* CWPWbxmlParser::NewL()
	{
	CWPWbxmlParser* self = new( ELeave ) CWPWbxmlParser; 
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

// Destructor
CWPWbxmlParser::~CWPWbxmlParser()
	{ 
	delete iParameterName;
	NW_WBXML_Dictionary_destroy(); // deletes iDictionary
	}

void CWPWbxmlParser::Cleanup( TAny *aPtr )
	{
	NW_TinyDom_ParserDelete( STATIC_CAST( Parser_t* , aPtr ) );
	}


// -----------------------------------------------------------------------------
// CWPWbxmlParser::ParseL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C void CWPWbxmlParser::ParseL( const TDesC8& aDocument,
									 MWPBuilder& aRoot )
	{	
	NW_TinyDom_Handle_t handle; 
	FLOG( _L( "[ProvisioningParser] CWPWbxmlParser::ParseL begin" ) );

	NW_Int32 bufferLength( aDocument.Size() );
	if( bufferLength <= KHeaderLength )
		{
		FLOG( _L( "[ProvisioningParser] CWPWbxmlParser::ParseL leaving" ) );
		User::Leave( KErrCorrupt );
		}
	NW_Byte* buffer = CONST_CAST( NW_Byte*, aDocument.Ptr() ); 	
	// create a tree from a wbxml buffer
    iDocNode = NW_DOM_DocumentNode_BuildWBXMLTree(&handle, 
                                           buffer,
                                           bufferLength,
                                           NW_FALSE,
                                           NW_FALSE);

	if( !iDocNode )
		{
		User::Leave( KErrCorrupt );
		FLOG( _L( "[ProvisioningParser] CWPWbxmlParser::ParseL error branch" ) );
		}
	else
		{ 
		FLOG( _L( "[ProvisioningParser] CWPWbxmlParser::ParseL okey branch" ) ); 

       	CleanupStack::PushL( TCleanupItem( Cleanup, &handle ) ) ;
		// traverse tree and build data into engine	
		ParseDocumentL( iDocNode, aRoot ) ; 
    	CleanupStack::PopAndDestroy(); // via handle
		}
	FLOG( _L( "[ProvisioningParser] CWPWbxmlParser::ParseL end" ) );
	}


// -----------------------------------------------------------------------------
// CWPWbxmlParser::ParseDocumentL
// Parses DOM document
// -----------------------------------------------------------------------------
//
NW_Uint16 CWPWbxmlParser::ParseDocumentL( NW_DOM_DocumentNode_t *aDocument,
									MWPBuilder& aRoot )
	{
	FLOG( _L( "[ProvisioningParser] CWPWbxmlParser::ParseDocumentL begin" ) );
	NW_Uint32 encoding( NW_DOM_DocumentNode_getCharacterEncoding( aDocument ) );
	NW_DOM_ElementNode_t* elem = 
							NW_DOM_DocumentNode_getDocumentElement( aDocument );
	FLOG( _L( "[ProvisioningParser] CWPWbxmlParser::ParseDocumentL end" ) );
	return ParseNodeL( elem, encoding, aRoot );
	}

// -----------------------------------------------------------------------------
// CWPWbxmlParser::ParseNode
// Parses DOM tree Node recursively
// -----------------------------------------------------------------------------
//
NW_Uint16 CWPWbxmlParser::ParseNodeL( NW_DOM_Node_t* aNode, NW_Uint32 aEncoding,
										MWPBuilder& aRoot)
	{
	FLOG( _L( "[ProvisioningParser] CWPWbxmlParser::ParesNodeL begin" ) );
	NW_DOM_Node_t* node = aNode;
	if( !node )
		{
		User::Leave( KErrGeneral );
		}
	else
		{
		NW_Uint32 type = NW_DOM_Node_getNodeType( aNode );
		if ( type != NW_DOM_ELEMENT_NODE )
			{
			User::Leave( KErrGeneral );
			}
		}
	NW_Uint16 errCode( KParseStatusOk );
	// traverse tree by simulating a stack
	while( node )
		{
		ParseElementL( node, aEncoding, aRoot );
		if( NW_DOM_Node_hasChildNodes( node ) )
			{
			node = NW_DOM_Node_getFirstChild( node );
			}
		else // leaf reached, find parent level
			{
			while( NW_DOM_Node_getNextSibling( node ) == NULL && node != aNode )
				{
				NW_Uint32 tagToken = NW_DOM_ElementNode_getTagToken( node ); 
				if( tagToken == KCharacteristicToken )
					{
					aRoot.EndCharacteristicL(); 
					}
			
				if(tagToken==0)
				User::Leave(KErrCorrupt);
				node =  NW_DOM_Node_getParentNode( node );
				}
			// always end leaf characteristic
			NW_Uint32 tagToken = NW_DOM_ElementNode_getTagToken( node ); 
			if( tagToken == KCharacteristicToken )
				{
				aRoot.EndCharacteristicL(); 
				}
			node = NW_DOM_Node_getNextSibling( node );
			} // end leaf
		} // end outer while
	FLOG( _L( "[ProvisioningParser] CWPWbxmlParser::ParseNodeL end" ) );
	return errCode; 
	}

// -----------------------------------------------------------------------------
// CWPWbxmlParser::ParseElement
// Parses DOM tree element into Engine data model using builder pattern.
// -----------------------------------------------------------------------------
//
void CWPWbxmlParser::ParseElementL( NW_DOM_ElementNode_t* aElement,
								  NW_Uint32 aEncoding, MWPBuilder& aRoot )
	{
	FLOG( _L( "[ProvisioningParser] CWPWbxmlParser::ParseElementL begin" ) );
	NW_Uint32 token = NW_DOM_ElementNode_getTagToken( aElement );			
	NW_Uint16 parmNameToken( 0 );
	NW_TinyTree_t* tiny_tree =  NW_TinyTree_Node_findTree( aElement ) ;
	NW_TinyDom_getParser( tiny_tree );
	if ( NW_DOM_ElementNode_hasAttributes( aElement ) )
		{
		NW_DOM_AttributeListIterator_t listIter;
		NW_Status_t status = 
			NW_DOM_ElementNode_getAttributeListIterator( aElement, &listIter );
		if( status != NW_STAT_SUCCESS )
			{
			User::Leave( KErrCorrupt );
			}
		TBool isFirstPart( ETrue ); // only one iteration
		TBool isLastPartProcessed( EFalse );
		TBool unknownNameFound( EFalse );
		NW_DOM_AttributeHandle_t attrHandle;
		while ( NW_DOM_AttributeListIterator_getNextAttribute( &listIter,
															   &attrHandle )
												== NW_STAT_WBXML_ITERATE_MORE )
			{
			CWPNameValuePair* pair = new( ELeave ) CWPNameValuePair();
			CleanupStack::PushL( pair );
			NW_Uint16 attribToken( KParseError );	
			attribToken = ParseAttributeL( &attrHandle,
													aEncoding,
													 *pair );

			if(  attribToken == KParseError  )
				{
				// syntactically invalid attribute
				User::Leave( KErrCorrupt );
				}

			// extract code page from token
			NW_Uint16 codePage(
				STATIC_CAST( NW_Uint16, ( ( attribToken >> 8 ) & KTokenValueMask 
									   ) ) );
			NW_Uint16 extractedValue (
				STATIC_CAST( NW_Uint16, ( attribToken & KTokenValueMask )
										) );
			// process attributes based on current element token
			switch( token )
				{
				case KWapProvisioningDocToken:
					{
					FLOG( _L( "[ProvisioningParser] ParseElementL: prov doc found" ) );
					break; // start of document
					}
				case KCharacteristicToken: 
					{
					FLOG( _L( "[ProvisioningParser] characteristic token found" ) );
					// check if the name clashes with a name in code page zero
					if ( attribToken == KCharTypeToken && pair )
						{
						aRoot.StartCharacteristicL( pair->Value() );
						}
					else {
						if(  codePage != 0  )
							{
							TInt numElements(	STATIC_CAST( TInt, (
								sizeof ( KDuplicateCharacteristicTable ) /
								sizeof ( KDuplicateCharacteristicTable[0] )	) ) );
							for( TInt i( 0 ); i < numElements; i++ ) 
								{
								if ( extractedValue ==
										KDuplicateCharacteristicTable[i] )
									{
									attribToken = extractedValue;
									FTRACE(RDebug::Print(_L("[ProvisioningParser] characteristic: id %d"), extractedValue));        
									break; // end for
									}
								}
							} // end codepage not 0
						aRoot.StartCharacteristicL( attribToken ); 
					}
					break;
					}
				// Note: parameters are constructed in two phases:
				// 1. the attribute value prefix token of the parm is stored
				// 2. the value of the attribute is read and stored
				//		with the prefix.
				case KParmToken:
					{
					FLOG( _L( "[ProvisioningParser] parameter token found" ) );
					// process the first part of the parameter attribute
					if( isFirstPart && attribToken > 0 &&
						 extractedValue  < KAttribValMin ) 
						{						
						if( extractedValue == KParmNameToken && pair )
							{
							if( pair->Value().Compare( KFromStr ) ==0 )
								{							
								parmNameToken = KFromTag;
								}

							// unknown parameter name.
							else
								{
								unknownNameFound = ETrue;
								delete iParameterName;
								iParameterName = NULL;
								iParameterName = (pair->Value()).AllocL();
								}
						}
						// set parameter name token for storing it in step 2.
						else
							{
							parmNameToken = attribToken; 
							}
						FTRACE(RDebug::Print(_L("[ProvisioningParser] parameter id %d"), parmNameToken));        

						// check if the name clashes with a name in code page 0
						if(  codePage != 0  )
							{
							TInt numElements( STATIC_CAST( TInt, (
								sizeof ( KDuplicateParmAttributeStartTable ) /
								sizeof ( KDuplicateParmAttributeStartTable[0] ))
								));
							for( TInt i(0); i < numElements ; i++ ) 
								{
								if ( extractedValue ==
										KDuplicateParmAttributeStartTable[i] )
									{
									parmNameToken = extractedValue;
									break; // end for
									}
								}
							} // end codepage not 0	
						isFirstPart = EFalse;
						break;
						}
					// process the second part of the attribute (value)
					else if(!isFirstPart && (extractedValue == KParmStartValueToken
								|| extractedValue >= KAttribValMin ) )
						{
						if( !unknownNameFound )							
							{
							aRoot.ParameterL( parmNameToken, pair->Value() );
							}
						else if ( unknownNameFound ) 							 
							{
							aRoot.ParameterL( *iParameterName , pair->Value() );
							delete iParameterName;
							iParameterName = NULL;
							}
						isLastPartProcessed=ETrue;
						}
					break;
					}
				default: // error no corresponding element exists
					{
					break;
					}
				} // end switch
			CleanupStack::PopAndDestroy(); // pair
			} // end while 	
			// if this parameter consisted of only one attribute, then
			// store it
			if( parmNameToken && !isLastPartProcessed )
				{
				aRoot.ParameterL( parmNameToken, KNullDesC ); 
				}
			else if( iParameterName && !isLastPartProcessed )
				{
				aRoot.ParameterL( *iParameterName, KNullDesC );
				delete iParameterName;
				iParameterName = NULL;
				}

		} 
		
		FLOG( _L( "[ProvisioningParser] CWPWbxmlParser::ParseElementL end" ) );
		// end if has attributes.
	}

// -----------------------------------------------------------------------------
// CWPWbxmlParser::ParseAttribute
// Constructs HBufCs containing attribute name and value
// -----------------------------------------------------------------------------
//
NW_Uint16 CWPWbxmlParser::ParseAttributeL( NW_DOM_AttributeHandle_t* aAttrHandle,
											NW_Uint32 aEncoding,
											 CWPNameValuePair& aPair )
	{	
	FLOG( _L( "[ProvisioningParser] CWPWbxmlParser::ParseAttributeL begin" ) );	
	NW_Uint16 getStatus( KParseError );
	// All tokenised attributes must begin with a single attribute start token 
	// and may be followed by zero or more attribute value, string, entity, 
	// opaque, or extension tokens.
	// 1. process attribute name
	TUint16 token( NW_DOM_AttributeHandle_getToken( aAttrHandle ) ); 
	// 2. process attribute value (only one allowed)
	NW_String_t strValue;
	NW_Status_t status = NW_DOM_AttributeHandle_getValue( aAttrHandle, &strValue );
	if( status == NW_STAT_SUCCESS )
		{
		getStatus = KParseStatusOk;
		}
	aPair.SetValue( ProcessAttributePartL( &strValue, aEncoding, getStatus ) );
	
	FLOG( _L( "[ProvisioningParser] CWPWbxmlParser::ParseAttributeL end" ) ); 
	return token;
	}
// -----------------------------------------------------------------------------
// CWPWbxmlParser::ProcessAttributePartL
// Constructs HBufCs containing attribute name and value
// -----------------------------------------------------------------------------
//
HBufC* CWPWbxmlParser::ProcessAttributePartL( NW_String_t* aString,
											  NW_Uint32 aEncoding,
											  NW_Uint16 aStatus )
	{
	FLOG( _L( "[ProvisioningParser] CWPWbxmlParser::ProcessAttributePartL begin" ) );
	
	NW_Ucs2 *ucs2Value = NULL;
	HBufC* attribute = NULL;
	if ( aStatus == KParseStatusOk ) 
		{
		NW_String_stringToUCS2Char( aString, aEncoding, &ucs2Value ); 
		if ( !ucs2Value )
			{
			NW_String_deleteStorage( aString );
			//User::Leave( KErrCorrupt );
			attribute = HBufC::NewL(1);
			TPtr aptr(attribute->Des());
			aptr.Append(_L(""));
			return attribute;
			}
		TPtrC attribValuePtr( ucs2Value );
		attribute = attribValuePtr.Alloc();		
		if( !attribute ) // mem alloc failed
			{
			free( ucs2Value ); 
			NW_String_deleteStorage( aString );
			User::Leave( KErrNoMemory );
			}
		}
	else
		{
		NW_String_deleteStorage( aString );
		User::Leave( KErrCorrupt );
		}
	// clean attribute value
	free( ucs2Value ); 
	NW_String_deleteStorage( aString );

	FLOG( _L( "[ProvisioningParser] CWPWbxmlParser::ProcessAttributePartL end" ) );
	return attribute;
	}

//  End of File  
