/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  XML Parser for opaque data field of Plug-ins.  This field
*  contains information about depdendencies and ordering.
*
*/


#ifndef DIAGPLUGINLOADERDEPENDENCYPARSER_H
#define DIAGPLUGINLOADERDEPENDENCYPARSER_H

// INCLUDES
#include <e32base.h>
#include <xml/contenthandler.h>

// FORWARD DECLARATION
class CDesC16ArrayFlat;

namespace Xml
    {
    class CParser;
    class RAttribute;    
    }

namespace DiagPluginPool
    {
/**
*  Plugin Loader Depdenency Parser
*  Parse dependency information and other data out of plug-ins via the
*  opaque_data field.
* 
* @since S60 v5.0
*/
NONSHARABLE_CLASS( CDependencyParser ) : public CBase, 
                                         public Xml::MContentHandler
    {
public:

    /**
    * Two-phased constructor.
    *
    * @return New instance of CDependencyParser
    */
    static CDependencyParser* NewL();   
    
    /**
    * Two-phased constructor.
    *
    * @return New instance of CDependencyParser and pop onto cleanup stack
    */
    static CDependencyParser* NewLC();   

    /**
    * Destructor.
    */
    virtual ~CDependencyParser();

    /**
    * Parse out Plug-in Information from XML.
    *
    * @param aOpaqueData    Contains text with opaque data for Dependencies/Order Number
    * @param aDefaultData   Contains text with default data for Parent UID
    * Leave Codes:    KErrNone      Valid data was parsed
    *                 KErrNotFound  No valid fields found
    *                 KErrArgument  Text is NULL or empty
    *                 KErrCorrupt   XML was determined to be corrupt
    * 
    */
    void ParseL( const TDesC8& aOpaqueData, const TDesC8& aDefaultData );
    
    /**
    * Return parsed order number
    *
    * @return orrder number
    */ 
    TInt GetOrderNumber() const;

    /**
    * Return parsed parent UID
    *
    * @return parent UID
    */ 
    const TUid& GetParentUid() const;

    /**
    * Return parsed service provided.  Ownership is transferred to the caller    
    *
    * @return service provided
    */
    HBufC* GetServiceProvided();

    /**
    * Return parsed services required.  Caller assumes ownership.
    *
    * @return services required
    */
    CDesC16ArrayFlat* GetServicesRequired();
    
private:    // from  Base Class Xml::MContentHandler 
    /**
    * Callback from XML Parser that indicates a Plugin element is starting
    *
    * @param aElement     handle to element's details
    * @param aAttributes  contains attributes for element
    * @param aErrorCode   indicates error, if any
    */     
    virtual void OnStartElementL( const Xml::RTagInfo& aElement,
                                  const Xml::RAttributeArray& aAttributes,
                                  TInt aErrorCode );
    
    /**
    * Callback from XML Parser that indicates the end of a plug-in element
    *
    * @param aElement     handle to element's details
    * @param aErrorCode   indicates error, if any
    */         
    virtual void OnEndElementL( const Xml::RTagInfo& aElement, TInt aErrorCode );
    
    /**
    * Callback from XML Parser that indicates content in an XML element
    *
    * @param aBytes       simple stream of data indicating the content
    * @param aErrorCode   indicates error, if any
    */     
    virtual void OnContentL( const TDesC8& aBytes, TInt aErrorCode );

    /**
    * Callback from XML Parser that indicates a parsing error
    *
    * @param aErrorCode   indicates error, if any
    * Note: The XmlFramework has been allocated error codes in the range: -17550 to -17599
    */         
    virtual void OnError( TInt aErrorCode );
    
    // Unused virtual member functions
    
    /**
    * @see Xml::MContentHandler::OnStartDocumentL
    */
    virtual void OnStartDocumentL( const Xml::RDocumentParameters& aDocParam, TInt aErrorCode );
    
    /**
    * @see Xml::MContentHandler::OnEndDocumentL
    */
    virtual void OnEndDocumentL( TInt aErrorCode );

    /**
    * @see Xml::MContentHandler::OnEndDocumentL
    */    
    virtual void OnStartPrefixMappingL( const RString& aPrefix, 
                                        const RString& aUri, 
                                        TInt aErrorCode );    
    /**
    * @see Xml::MContentHandler::OnEndDocumentL
    */    
    virtual void OnEndPrefixMappingL( const RString& aPrefix, TInt aErrorCode );
    
    /**
    * @see Xml::MContentHandler::OnEndDocumentL
    */    
    virtual void OnIgnorableWhiteSpaceL( const TDesC8& aBytes, TInt aErrorCode );
    
    /**
    * @see Xml::MContentHandler::OnEndDocumentL
    */    
    virtual void OnSkippedEntityL( const RString& aName, TInt aErrorCode );
    
    /**
    * @see Xml::MContentHandler::OnEndDocumentL
    */    
    virtual void OnProcessingInstructionL( const TDesC8& aTarget, 
                                           const TDesC8& aData, 
                                           TInt aErrorCode );    
    /**
    * @see Xml::MContentHandler::OnEndDocumentL
    */    
    virtual void OnExtensionL( const RString& aData, TInt aToken, TInt aErrorCode );
    
    /**
    * @see Xml::MContentHandler::OnEndDocumentL
    */    
    virtual TAny* GetExtendedInterface( const TInt32 aUid );
    
private:    // private utility functions

    /**     
    * Simple utility function to allocate and pop onto the stack a 16 bit buffer descriptor
    * from an 8 bit descriptor.
    *
    * @param aDes   8 bit descriptor to be converted to 16 bits
    * @return       Newly allocated 16-bit buffer containing converted contents of aDes
    */      
    HBufC* Convert8BitTo16BitBufferLC( const TDesC8& aDes ) const;
    
    /**     
    * Parse a numeric attribute from XML text with specific notation for diagnostics framework
    * plug-ins.
    *
    * @param aAttribute         XML attribute
    * @param aNumericAttribute  Parsed out integer
    * @return                   Indicates whether or not attribute was parsed correctly
    */      
    TBool ParseNumericAttribute( const Xml::RAttribute& aAttribute, TInt& aNumericAttribute );
    
    /**     
    * Parse a string attribute from XML text with specific notation for diagnostics framework
    * plug-ins.
    *
    * @param aAttribute         XML attribute
    * @param aStringAttribute   Parsed out string
    * @return                   Indicates whether or not attribute was parsed correctly
    */          
    TBool ParseStringAttributeL( const Xml::RAttribute& aAttribute, HBufC*& aStringAttribute );
    
    /**     
    * Parse a hex UID from given descriptor
    *
    * @param aSource             String containing UID
    * @param aTarget             Parsed out UID
    * @return                    Error Code
    */          
    TInt ParseToUid( const TDesC8& aSource, TUid& aTarget );
    
    /**     
    * Parse a hex UID from given descriptor
    *
    * @param aAttributes         List of attributes to parse
    * @param aFoundTag           Reference to some class variable that indicates if a given tag has been found.
    * @return                    ETrue==XML OK/EFalse==XML not validated
    */     
    TBool ValidateXmlTag( const Xml::RAttributeArray& aAttributes, TBool& aFoundTag );
       
    /**     
    * Reset all data
    *
    */  
    void ResetL();
    
private:  // private constructors

    /**
    * Construct a CDependencyParser object
    *    
    */
    CDependencyParser();
    
    /**
    * 2nd phase constructor
    *
    */     
    void ConstructL();

private: // data
    
    /**
     * Pointer to XML Parser from SDK
     * owns
     */
    Xml::CParser* iParser;

    /**
    * Error code indicates error in parsing
    */
    TInt iParseStatus;

    /**
    * Parsed order number
    */
    TInt iOrderNumber;

    /**
    * Parsed provided service
    * owns
    */
    HBufC* iServiceProvided;

    /**
    * Parsed required service
    * owns
    */
    CDesC16ArrayFlat* iServicesRequired;

    /**
    * Parsed parent uid
    */
    TUid iParentUid;

    /**
    * Indicates that parsing is happening within services required blocks
    */
    TBool iParsingServicesRequired;

    /**
    * Indicates that there was some problem with the parsing
    */
    TBool iBadXML;

    /**
    * Indicates that root node has been found and we are inside it
    */
    TBool iInsideRootNode;
    
    /**
    * Indicates that order number has been found
    */    
    TBool iFoundOrderNumber;
    
    /**
    * Indicates that services provided has been found
    */    
    TBool iFoundServiceProvided;
    
    /**
    * Indicates that services required has been found
    */    
    TBool iFoundServicesRequired;
    
    /**
    * Indicates that parser has been run at least once
    */    
    TBool iHasBeenRun;
        
    };
        
    } // namespace DiagPluginPool

#endif // DIAGPLUGINLOADERDEPENDENCYPARSER_H

// End of File

