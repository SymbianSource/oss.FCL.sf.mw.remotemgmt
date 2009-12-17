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
* Description:  Document structure validator.
*
*/


//  INCLUDE FILES
#include "CWPValidator.h"
#include "MWPElement.h"
#include "CWPParameter.h"
#include "CWPCharacteristic.h"
#include "WPEngineDebug.h"

// DATA TYPES
// Element in the validating table.
struct TMapping 
    {
    // The contained element type 
    TInt iTo;

    // Pointer to the validating table for the contained element
    const TMapping* iMapping;
    };

// CONSTANTS

// Validating table for unknown characteristics
const TMapping KMappingArrayNamedCharacteristic[] =
    {
        { KWPParameter, NULL },
        { KWPNamedCharacteristic, KMappingArrayNamedCharacteristic },
        { KWPElement, NULL }
    };

// Validating table for parameters only characteristics
const TMapping KMappingArrayParametersOnly[] =
    {
        { KWPParameter, NULL },
        { KWPNamedCharacteristic, KMappingArrayNamedCharacteristic },
        { KWPElement, NULL }
    };

// Validating table for PXPHYSICAL
const TMapping KMappingArrayPxPhysical[] =
    {
        { KWPParameter, NULL },
        { KWPPort, KMappingArrayParametersOnly },
        { KWPNamedCharacteristic, KMappingArrayNamedCharacteristic },
        { KWPElement, NULL }
    };

// Validating table for PXLOGICAL
const TMapping KMappingArrayPxLogical[] =
    {
        { KWPParameter, NULL },
        { KWPPxPhysical, KMappingArrayPxPhysical },
        { KWPPort, KMappingArrayParametersOnly },
        { KWPPxAuthInfo, KMappingArrayParametersOnly },
        { KWPNamedCharacteristic, KMappingArrayNamedCharacteristic },
        { KWPElement, NULL }
    };

// Validating table for WLAN
const TMapping KMappingArrayWLAN[] =
    {
        { KWPParameter, NULL },
        { KWPWepKey, KMappingArrayParametersOnly },
        { KWPElement, NULL }
    };

// Validating table for NAPDEF
const TMapping KMappingArrayNapDef[] =
    {
        { KWPParameter, NULL },
        { KWPValidity, KMappingArrayParametersOnly },
        { KWPNapAuthInfo, KMappingArrayParametersOnly },
        { KWPBearerInfo, KMappingArrayParametersOnly },
        { KWPDNSAddrInfo, KMappingArrayParametersOnly },
        { KWPWLAN, KMappingArrayWLAN },
        { KWPNamedCharacteristic, KMappingArrayNamedCharacteristic },
        { KWPElement, NULL }
    };

// Validating table for APPADDR
const TMapping KMappingArrayAppAddr[] =
    {
        { KWPParameter, NULL },
        { KWPPort, KMappingArrayParametersOnly },
        { KWPNamedCharacteristic, KMappingArrayNamedCharacteristic },
        { KWPElement, NULL }
    };

// Validating table for APPLICATION
const TMapping KMappingArrayApplication[] =
    {
        { KWPParameter, NULL },
        { KWPAppAddr, KMappingArrayAppAddr },
        { KWPAppAuth, KMappingArrayParametersOnly },
        { KWPResource, KMappingArrayParametersOnly },
        { KWPNamedCharacteristic, KMappingArrayNamedCharacteristic },
        { KWPElement, NULL }
    };

// Validating table for the document root
const TMapping KMappingArrayRoot[] =
    {
        { KWPPxLogical, KMappingArrayPxLogical },
        { KWPNapDef, KMappingArrayNapDef },
        { KWPBootstrap, KMappingArrayParametersOnly },
        { KWPVendorConfig, KMappingArrayParametersOnly },
        { KWPClientIdentity, KMappingArrayParametersOnly },
        { KWPAccess, KMappingArrayParametersOnly },
        { KWPApplication, KMappingArrayApplication },
        { KWPBearerInfo, KMappingArrayParametersOnly },
        { KWPWLAN, KMappingArrayParametersOnly },
        { KWPDNSAddrInfo, KMappingArrayParametersOnly },
        { KWPLoginScript, KMappingArrayParametersOnly },
        { KWPNamedCharacteristic, KMappingArrayNamedCharacteristic },
        { KWPElement, NULL }
    };

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CWPValidator::CWPValidator
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CWPValidator::CWPValidator()
    {
    iCurrentMapping = KMappingArrayRoot;
    iFlag = ETrue;
    }

// -----------------------------------------------------------------------------
// CWPValidator::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CWPValidator::ConstructL()
    {
    }

// -----------------------------------------------------------------------------
// CWPValidator::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CWPValidator* CWPValidator::NewL()
    {
    CWPValidator* self = NewLC();
    CleanupStack::Pop();
    
    return self;
    }

// -----------------------------------------------------------------------------
// CWPValidator::NewLC
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CWPValidator* CWPValidator::NewLC()
    {
    CWPValidator* self = new( ELeave ) CWPValidator;
    
    CleanupStack::PushL( self );
    self->ConstructL();
    
    return self;
    }

// Destructor
CWPValidator::~CWPValidator()
    {
    iCurrentMapping = NULL;
    iCurrentElement = NULL;
    }

// -----------------------------------------------------------------------------
// CWPValidator::Visit
// -----------------------------------------------------------------------------
//
void CWPValidator::VisitL( CWPParameter& aParameter )
    {
    // Store the current mapping and element
    const TMapping* currentMapping = iCurrentMapping;
    const MWPElement* currentElement = iCurrentElement;

    DVA( "Validator: Mapping to parameter %d", aParameter.ID() );
    // Check if the mapping is valid
    MapToL( aParameter );
    DEBUG( "Validator: Exit" );

    // Restore the current mapping and element
    iCurrentMapping = currentMapping;
    iCurrentElement = currentElement;
    }

// -----------------------------------------------------------------------------
// CWPValidator::Visit
// -----------------------------------------------------------------------------
//
void CWPValidator::VisitL( CWPCharacteristic& aCharacteristic )
    {
    // Store the current mapping and element
    const TMapping* currentMapping = iCurrentMapping;
    const MWPElement* currentElement = iCurrentElement;

    DVA( "Validator: Mapping to characteristic %d", aCharacteristic.Type() );
    // Check if the mapping is valid
    MapToL( aCharacteristic );

    // Mapping is valid, so enter the characteristic
    if(iFlag)
    {
    aCharacteristic.AcceptL( *this );
    }
    DEBUG( "Validator: Exit" );

    // Restore the current mapping and element
    iCurrentMapping = currentMapping;
    iCurrentElement = currentElement;
    iFlag = ETrue;
    }

// -----------------------------------------------------------------------------
// CWPValidator::VisitLink
// -----------------------------------------------------------------------------
//
void CWPValidator::VisitLinkL( CWPCharacteristic& /*aLink*/ )
    {
    }

// -----------------------------------------------------------------------------
// CWPValidator::MapToL
// -----------------------------------------------------------------------------
//

void CWPValidator::MapToL( MWPElement& aTo )
    {
    TInt toType( aTo.Type() );

    for( TInt i( 0 ); iCurrentMapping[i].iTo != KWPElement; i++ )
        {
        if( iCurrentMapping[i].iTo == toType )
            {
            iCurrentMapping = iCurrentMapping[i].iMapping;
            iCurrentElement = &aTo;
            return;
            }
        }
    if ( Rangecheck( toType ) )
    {
    	iFlag = EFalse;
    	return;
    }

    DEBUG( "Validator: Error" );
    User::Leave( KErrCorrupt );
    }

TBool CWPValidator::Rangecheck(const TInt toType )
{
	if((toType != KWPElement) || (toType != KWPParameter) ||
	   (toType != KWPPxLogical ) || (toType != KWPPxPhysical) ||
	   (toType != KWPPort) || (toType != KWPValidity) ||
	   (toType != KWPNapDef) || (toType != KWPBootstrap) ||
	   (toType != KWPVendorConfig) || (toType != KWPClientIdentity) ||
	   (toType != KWPPxAuthInfo) || (toType != KWPNapAuthInfo) ||
	   (toType != KWPAccess) || (toType != KWPBearerInfo) ||
	   (toType != KWPDNSAddrInfo) || (toType != KWPWLAN) ||
	   (toType != KWPWepKey) || (toType != KWPPort1) ||
	   (toType != KWPApplication) || (toType != KWPAppAddr) ||
	   (toType != KWPAppAuth) || (toType != KWPClientIdentity1) ||
	   (toType != KWPResource) || (toType != KWPLoginScript) ||
	   (toType != KWPNamedCharacteristic))
	   return ETrue;
	else
	   return EFalse;
}

//  End of File  
