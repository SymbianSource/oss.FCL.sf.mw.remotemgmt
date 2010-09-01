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
* Description:  Parse dependency information from XML in ECOM plug-in
                 resource files
*
*/


// CLASS DECLARATION
#include "diagpluginloaderdependencyparser.h"

// SYSTEM INCLUDE FILES
#include <xml/parser.h>         // CParser
#include <badesca.h>
#include <e32svr.h>
#include <DiagFrameworkDebug.h> // LOGSTRING

// USER INCLUDE FILES

using namespace Xml;

namespace DiagPluginPool
    {
// ============================================================================
// LOCAL DATA
// ============================================================================

_LIT8( KParserMIMEType, "text/xml" );
_LIT8( KRootXmlTag, "diagplugininfo" );
_LIT8( KOrderNumberTag, "order" );
_LIT8( KServiceProvidedTag, "serviceprovided" );
_LIT8( KServicesRequiredTag, "servicesrequired" );
_LIT8( KServiceTag, "service" );
_LIT8( KNameAttrTag, "name" );
_LIT8( KNumberAttrTag, "number" );

// ============================================================================
// MEMBER FUNCTIONS( CDepdendencyParser )
// ============================================================================


// ---------------------------------------------------------------------------
// NewL
// ---------------------------------------------------------------------------
//
CDependencyParser* CDependencyParser::NewL()
    {
    CDependencyParser* self = CDependencyParser::NewLC();
    CleanupStack::Pop( self );
    return self;
    }


// ---------------------------------------------------------------------------
// NewLC
// ---------------------------------------------------------------------------
//
CDependencyParser* CDependencyParser::NewLC()
    {
    CDependencyParser* self = new( ELeave )CDependencyParser;
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    } 

// ---------------------------------------------------------------------------
// ~CDependencyParser
// ---------------------------------------------------------------------------
//
CDependencyParser::~CDependencyParser()
    {
    delete iParser;
    delete iServiceProvided;
    if ( iServicesRequired )
        {
        iServicesRequired->Reset();
        }
    delete iServicesRequired;
    }

// ---------------------------------------------------------------------------
// ParseL
// ---------------------------------------------------------------------------
//
void CDependencyParser::ParseL( const TDesC8& aOpaqueData, const TDesC8& aDefaultData )
    {
    
    ASSERT( aOpaqueData.Length() );    
    ASSERT( aDefaultData.Length() );        
    
    // Start out by deleting and zeroing old elements
    if ( iHasBeenRun )
        {        
        ResetL();
        }        
           
    iHasBeenRun = ETrue;
    
    // Get Parent UID    
    iParseStatus = ParseToUid( aDefaultData, iParentUid );       
    
    // Do not parse opaque data if UID is invalid because plug-in will not be loaded
    // when data error is detected.        
    if ( iParseStatus == KErrNone )    
        {
        // Parse buffer in one fell swoop            
        iParser->ParseBeginL();    
        iParser->ParseL( aOpaqueData );    
        iParser->ParseEndL();            
        }            
    
    // Check for error from parser
    User::LeaveIfError( iParseStatus );
    
    // Check for error in format
    if ( iBadXML )
        {
        User::Leave( KErrCorrupt );
        }
    }
   
// ---------------------------------------------------------------------------
// GetOrderNumber
// ---------------------------------------------------------------------------
//    
TInt CDependencyParser::GetOrderNumber() const
    {
    return iOrderNumber;
    }
    
// ---------------------------------------------------------------------------
// GetServiceProvided
// ---------------------------------------------------------------------------
//    
HBufC* CDependencyParser::GetServiceProvided()
    {
    HBufC* ret = iServiceProvided;
    iServiceProvided = NULL;
    return ret;
    }
 
// ---------------------------------------------------------------------------
// GetServicesRequired
// ---------------------------------------------------------------------------
//   
CDesC16ArrayFlat* CDependencyParser::GetServicesRequired()
    {
    // Ownership is passed to caller
    CDesC16ArrayFlat* ret = iServicesRequired;    
    iServicesRequired = NULL;
    return ret;
    }
    
// ---------------------------------------------------------------------------
// GetServicesRequired
// ---------------------------------------------------------------------------
//       
const TUid& CDependencyParser::GetParentUid() const
    {
    return iParentUid;       
    }

// ---------------------------------------------------------------------------
// From Xml::MContentHandler
// OnStartElementL
// ---------------------------------------------------------------------------
//
void CDependencyParser::OnStartElementL(
    const RTagInfo& aElement,
    const RAttributeArray& aAttributes,
    TInt aErrorCode
    )
    {
    const TDesC8& elementName8 = aElement.LocalName().DesC();
    
#ifdef _DEBUG   
    HBufC* elementName16 = Convert8BitTo16BitBufferLC( elementName8 );
    LOGSTRING3( "OnStartElementL %S %d\n", elementName16, aErrorCode )
    CleanupStack::PopAndDestroy( elementName16 );    
#endif
    
    /*
    Parse XML tags in the following sample format:
    
    <diagplugininfo>
       <order number="5"/>
       <serviceprovided name="Browser"/>
       <servicesrequired>"
           <service name="SIM"/>
           <service name="Graphics"/>
       </servicesrequired>
   </diagplugininfo>   
    */        
    
    if ( !iInsideRootNode )
        {
        // Look for root node
        if ( ! elementName8.Compare( KRootXmlTag ) )
            {
            iInsideRootNode = ETrue;
            }
        else
            {
            LOGSTRING( "Error: unexpected element looking for root node.\n" )
            iBadXML = ETrue;
            }
        }
    else // Look for "body" nodes
        {
        // Check order number
        if ( ! elementName8.Compare( KOrderNumberTag ) )
            {
            LOGSTRING("Order number\n")
            if ( ValidateXmlTag( aAttributes, iFoundOrderNumber ))
                {
                if ( ! ParseNumericAttribute( aAttributes[0], iOrderNumber ) )
                    {
                    LOGSTRING2( "Order Number: \'%d\'\n", iOrderNumber )
                    }
                else
                    {                                       
                    iBadXML = ETrue;
                    }
                }           
            }
        // Check service provided
        else if ( ! elementName8.Compare( KServiceProvidedTag ) )
            {
            LOGSTRING("Service Provided\n")
            
            if ( ValidateXmlTag( aAttributes, iFoundServiceProvided ))
                {
                if ( ! ParseStringAttributeL( aAttributes[0], iServiceProvided ) )
                                            
                    {                    
                    LOGSTRING2( "Service Provided \"%S\"\n", iServiceProvided )
                    }
                else
                    {                    
                    iBadXML = ETrue;
                    }
                }              
            }

        // Check services required block
        else if ( ! elementName8.Compare( KServicesRequiredTag ) )
            {
            if ( ! iFoundServicesRequired )
                {                
                iParsingServicesRequired = ETrue;
                iFoundServicesRequired = ETrue;
                }
            else
                {                
                LOGSTRING( "Error: unexpected services required tag found.\n" )
                iBadXML = ETrue;
                }
            }
        
        // Check service tag
        else if ( ! elementName8.Compare( KServiceTag ) )
            {
            LOGSTRING("Service\n")
            
            // OK to have multiple service tags so use dummy as "found" flag
            TBool Dummy = EFalse;
            
            if ( ValidateXmlTag( aAttributes, Dummy ))
                {
                HBufC* newService = NULL;
                
                if ( ! ParseStringAttributeL( aAttributes[0],
                                                newService ) )
                    {
                    LOGSTRING2( "Got service required \"%S\"\n", newService )
                    CleanupStack::PushL( newService );
                    iServicesRequired->AppendL( *newService );
                    CleanupStack::PopAndDestroy( newService );                    
                    }
                else
                    {
                    iBadXML = ETrue;
                    }
                }            
            }
        // Unexpected tag            
        else
            {
            LOGSTRING( "Error: unexpected XML tag found.\n" )
            iBadXML = ETrue;
            }
        }        
    }       

// ---------------------------------------------------------------------------
// From Xml::MContentHandler
// OnEndElementL
// ---------------------------------------------------------------------------
//    
void CDependencyParser::OnEndElementL( const Xml::RTagInfo& aElement, TInt aErrorCode )
    {
    const TDesC8& elementName8 = aElement.LocalName().DesC();
    
 #ifdef _DEBUG
    HBufC* elementName16 = Convert8BitTo16BitBufferLC( elementName8 );
    LOGSTRING3( "OnEndElementL %S %d\n", elementName16, aErrorCode )
    CleanupStack::PopAndDestroy( elementName16 );
 #endif
    
    // Check for end services required block
    if ( ! elementName8.Compare( KServicesRequiredTag ) )
        {
        iParsingServicesRequired = EFalse;
        }    
        
    // Check for end of root block
    if ( ! elementName8.Compare( KRootXmlTag ) )        
        {            
        iInsideRootNode = EFalse;
        }
    }

// ---------------------------------------------------------------------------
// From Xml::MContentHandler
// OnContentL
// ---------------------------------------------------------------------------
//     
void CDependencyParser::OnContentL( const TDesC8& /*aBytes*/, TInt aErrorCode )
    {    
    LOGSTRING2( "OnContentL %d\n", aErrorCode )
    }    
    
// ---------------------------------------------------------------------------
// From Xml::MContentHandler
// OnError
// ---------------------------------------------------------------------------
//    
void CDependencyParser::OnError( TInt aErrorCode )
    {
    LOGSTRING2( "OnError %d\n", aErrorCode )
        
    iParseStatus = aErrorCode;
    }  
    
// ============================================================================
// PRIVATE MEMBER FUNCTIONS( CDepdendencyParser )
// ============================================================================    
    
// ---------------------------------------------------------------------------
// Convert8BitTo16BitBufferLC
// ---------------------------------------------------------------------------
//  
HBufC* CDependencyParser::Convert8BitTo16BitBufferLC( const TDesC8& Des ) const
    {
    HBufC* buf = HBufC::NewLC( Des.Length() );
    buf->Des().Copy( Des );
    return buf;
    }

// ---------------------------------------------------------------------------
// ParseNumericAttribute
// ---------------------------------------------------------------------------
//  
TBool CDependencyParser::ParseNumericAttribute( const RAttribute& aAttribute, 
                                                TInt& aNumericAttribute )
    {
    TBool corrupt = EFalse;
    const TDesC8& attrName = aAttribute.Attribute().LocalName().DesC();
    const TDesC8& attrVal = aAttribute.Value().DesC();

    // Attribute name should be "number"
    if ( attrName.Compare( KNumberAttrTag ) )
        {
        LOGSTRING( "Error: expected number attribute.\n" )
        corrupt = ETrue;
        }
    else
        {
        // Parse out order number
        TLex8 lex( attrVal );
        TInt rc=lex.Val( aNumericAttribute );
        if ( rc != KErrNone )
            {
            LOGSTRING2( "Error: Could not parse attribute( rc=%d )", rc )
            corrupt = ETrue;
            }
        }

    return corrupt;
    }

// ---------------------------------------------------------------------------
// ParseStringAttributeL
// ---------------------------------------------------------------------------
//
TBool CDependencyParser::ParseStringAttributeL( const RAttribute& aAttribute, 
                                                HBufC*& aStringAttribute )
    {
    TBool corrupt = EFalse;
    const TDesC8& attrName = aAttribute.Attribute().LocalName().DesC();
    const TDesC8& attrVal = aAttribute.Value().DesC();

    // Attribute name should be "name"
    if ( attrName.Compare( KNameAttrTag ) )
        {
        LOGSTRING( "Error: expected name attribute.\n" )
        corrupt = ETrue;
        }
    else
        {
        aStringAttribute = Convert8BitTo16BitBufferLC( attrVal );
        CleanupStack::Pop( aStringAttribute );
        }

    return corrupt;
    }

// ----------------------------------------------------------------------------
// ParseToUid
// ----------------------------------------------------------------------------
//
TInt CDependencyParser::ParseToUid( const TDesC8& aSource, TUid& aTarget )
    {
    // Remove required "0x" from the descriptor
    _LIT8( KHexPrefix, "0x" );

    TPtrC8 pSource( aSource );
    const TInt prefixPosition = pSource.Find( KHexPrefix );
    if ( prefixPosition == KErrNotFound )
        {
        return KErrCorrupt;            
        }
    else
        {
        pSource.Set( aSource.Mid( prefixPosition + KHexPrefix().Length() ) );
        }            

    // Parse to integer
    TLex8 lex( pSource );
    TUint integer = 0;

    // Parse using TRadix::EHex as radix:
    const TInt err = lex.Val( integer, EHex );
    aTarget.iUid = integer;
   
    return err;
    }
    

// ---------------------------------------------------------------------------
// ValidateXmlTag
// Validate Xml tag following a few standard rules that apply to
// all XML tags in this format:
// 1) Only one attribute is in each XML tag
// 2) Only one of any given tag is allowed
// ---------------------------------------------------------------------------
//
TBool CDependencyParser::ValidateXmlTag( const Xml::RAttributeArray& aAttributes,
                                         TBool& aFoundTag )
    {         
    if ( aFoundTag )
        {
        LOGSTRING( "Error: multiple tags found.  Ignoring subsequent values.\n" )        
        }
    else 
        {
        aFoundTag = ETrue;
        
        if ( aAttributes.Count() == 1 )
            {
            return ETrue;
            }
        else if ( aAttributes.Count() > 1 )
            {
            LOGSTRING( "Error: Extra data after attribute tag.\n" )
            iBadXML = ETrue;
            }
        else
            {
            LOGSTRING( "Error: No attributes found.\n" )
            iBadXML = ETrue;
            }
        }                
        
    return EFalse;        
    }    

// ---------------------------------------------------------------------------
// ResetL
// ---------------------------------------------------------------------------
//
void CDependencyParser::ResetL()
    {
    delete iServiceProvided;
    iServiceProvided = NULL;
            
    if ( iServicesRequired )
        {
        iServicesRequired->Reset();
        }
    delete iServicesRequired;
    iServicesRequired = new( ELeave )CDesC16ArrayFlat( 1 );
            
    iOrderNumber = 0;
    iParentUid = TUid::Uid(0);
    
    // Reset all flags
    iParsingServicesRequired = EFalse;
    iBadXML = EFalse;
    iInsideRootNode = EFalse;
    iFoundOrderNumber = EFalse;
    iFoundServiceProvided = EFalse;
    iFoundServicesRequired = EFalse;
    iParseStatus = 0;
    }
        
// ============================================================================
// PRIVATE CONSTRUCTORS ( CDepdendencyParser )
// ============================================================================           
        
// ---------------------------------------------------------------------------
// CDependencyParser::CDependencyParser
// ---------------------------------------------------------------------------
//
CDependencyParser::CDependencyParser()
    {    //lint !e1927 variables not showing up in initializer list
         // initialized in CBase constructor
    }    //lint !e1744 member variables possibly not initialized
    
// ---------------------------------------------------------------------------
// ConstructL
// ---------------------------------------------------------------------------
//
void CDependencyParser::ConstructL()
    {        
    iParser = CParser::NewL( KParserMIMEType, *this );            
    iServicesRequired = new( ELeave )CDesC16ArrayFlat( 1 );
    }       
    

// ============================================================================
// Unused pure virtual functions from Xml::MContentHandler
// ============================================================================

void CDependencyParser::OnStartDocumentL( const RDocumentParameters& /* aDocParam */, TInt /* aErrorCode */ )
    {
    }

void CDependencyParser::OnEndDocumentL( TInt /* aErrorCode */ )
    {
    }

void CDependencyParser::OnStartPrefixMappingL( const RString& /* aPrefix */, const RString& /* aUri */, TInt /* aErrorCode */ )
    {
    }

void CDependencyParser::OnEndPrefixMappingL( const RString& /* aPrefix */, TInt /* aErrorCode */ )
    {
    }

void CDependencyParser::OnIgnorableWhiteSpaceL( const TDesC8& /* aBytes */, TInt /* aErrorCode */ )
    {
    }

void CDependencyParser::OnSkippedEntityL( const RString& /* aName */, TInt /* aErrorCode */ )
    {
    }

void CDependencyParser::OnProcessingInstructionL( const TDesC8& /* aTarget */, const TDesC8& /* aData */, TInt /* aErrorCode */ )
    {
    }

void CDependencyParser::OnExtensionL( const RString& /* aData */, TInt /* aToken */, TInt /* aErrorCode */ )
    {
    }

TAny* CDependencyParser::GetExtendedInterface( const TInt32 /* aUid */ )
    {
    return 0;
    }
    
    } // namespace DiagPluginPool

// End of File

