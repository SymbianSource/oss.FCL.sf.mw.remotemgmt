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
* Description:  Factory for adapters.
*
*/


//  INCLUDE FILES
#include "WPElementFactory.h"
#include "CWPCharacteristic.h"
#include "CWPParameter.h"
#include "CWPEngine.pan"

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// WPElementFactory::CreateCharacteristicL
// -----------------------------------------------------------------------------
//
EXPORT_C CWPCharacteristic* WPElementFactory::CreateCharacteristicL(
    TInt aType )
    {
    __ASSERT_DEBUG( aType > KWPParameter && aType <= KWPResource, 
        Panic( EWPIllegalCharacteristic ) );
    return CWPCharacteristic::NewL( aType );
    }

// -----------------------------------------------------------------------------
// WPElementFactory::CreateCharacteristicLC
// -----------------------------------------------------------------------------
//
EXPORT_C CWPCharacteristic* WPElementFactory::CreateCharacteristicLC( 
    TInt aType )
    {
    __ASSERT_DEBUG( aType > 0 && aType <= KWPResource,
        Panic( EWPIllegalCharacteristic ) );
    return CWPCharacteristic::NewLC( aType );
    }

// -----------------------------------------------------------------------------
// WPElementFactory::CreateCharacteristicL
// -----------------------------------------------------------------------------
//
EXPORT_C CWPCharacteristic* WPElementFactory::CreateCharacteristicL(
    const TDesC& aName )
    {
    CWPCharacteristic* result = 
        WPElementFactory::CreateCharacteristicLC( aName );
    CleanupStack::Pop(); // result
    return result;
    }

// -----------------------------------------------------------------------------
// WPElementFactory::CreateCharacteristicLC
// -----------------------------------------------------------------------------
//
EXPORT_C CWPCharacteristic* WPElementFactory::CreateCharacteristicLC( 
    const TDesC& aName )
    {
    CWPCharacteristic* result = 
        CWPCharacteristic::NewLC( KWPNamedCharacteristic );
    result->SetNameL( aName );
    return result;
    }

// -----------------------------------------------------------------------------
// WPElementFactory::CreateParameterL
// -----------------------------------------------------------------------------
//
EXPORT_C CWPParameter* WPElementFactory::CreateParameterL( TInt aID, 
                                                          const TDesC& aValue )
    {
    CWPParameter* param = CreateParameterLC( aID, aValue );
    CleanupStack::Pop( param );
    return param;
    }

// -----------------------------------------------------------------------------
// WPElementFactory::CreateParameterLC
// -----------------------------------------------------------------------------
//
EXPORT_C CWPParameter* WPElementFactory::CreateParameterLC( TInt aID, 
                                                           const TDesC& aValue )
    {
    CWPParameter* param = CWPParameter::NewLC();
    param->SetID( aID );
    param->SetValueL( aValue );

    return param;
    }

// -----------------------------------------------------------------------------
// WPElementFactory::CreateParameterL
// -----------------------------------------------------------------------------
//
EXPORT_C CWPParameter* WPElementFactory::CreateParameterL( const TDesC& aName, 
                                                          const TDesC& aValue )
    {
    CWPParameter* param = CreateParameterLC( aName, aValue );
    CleanupStack::Pop( param );
    return param;
    }

// -----------------------------------------------------------------------------
// WPElementFactory::CreateParameterLC
// -----------------------------------------------------------------------------
//
EXPORT_C CWPParameter* WPElementFactory::CreateParameterLC( const TDesC& aName, 
                                                          const TDesC& aValue )
    {
    CWPParameter* param = CWPParameter::NewLC();
    param->SetNameL( aName );
    param->SetValueL( aValue );

    return param;
    }

// -----------------------------------------------------------------------------
// WPElementFactory::CreateL
// -----------------------------------------------------------------------------
//
EXPORT_C MWPElement* WPElementFactory::CreateL( TInt aType )
    {
    MWPElement* element = CreateLC( aType );
    CleanupStack::Pop( element );
    return element;
    }

// -----------------------------------------------------------------------------
// WPElementFactory::CreateLC
// -----------------------------------------------------------------------------
//
EXPORT_C MWPElement* WPElementFactory::CreateLC( TInt aType )
    {
    __ASSERT_DEBUG( aType >= KWPParameter 
        && aType <= KWPResource, Panic( EWPIllegalElement ) );

    MWPElement* element = NULL;

    if( aType == KWPParameter )
        {
        element = CWPParameter::NewLC();
        }
    else
        {
        element = CreateCharacteristicLC( aType );
        }

    return element;
    }

//  End of File  
