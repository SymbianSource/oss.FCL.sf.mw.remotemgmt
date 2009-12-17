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
* Description:  WPElementFactory can be used to create any CWP* element.
*
*/


#ifndef WPELEMENTFACTORY_H
#define WPELEMENTFACTORY_H

// INCLUDES
#include <e32base.h>

// FORWARD DECLARATIONS
class CWPCharacteristic;
class CWPParameter;
class MWPElement;

//  CLASS DEFINITION

/**
*  Factory class for CWP* elements.
*
*  @lib ProvisioningEngine
*  @since 2.0
*/
class WPElementFactory 
    {
    public:
        /** 
        * Create a CWPCharacteristic.
        * @param aType The type of characteristic
        * @return Characteristic. Ownership is transferred.
        */
        IMPORT_C static CWPCharacteristic* CreateCharacteristicL( TInt aType );

        /** 
        * Create a CWPCharacteristic.
        * @param aType The type of characteristic
        * @return Characteristic. Ownership is transferred.
        */
        IMPORT_C static CWPCharacteristic* CreateCharacteristicLC( TInt aType );

        /** 
        * Create a CWPCharacteristic.
        * @param aName The characteristic's name
        * @return Characteristic. Ownership is transferred.
        */
        IMPORT_C static CWPCharacteristic* CreateCharacteristicL( const TDesC& aName );

        /** 
        * Create a CWPCharacteristic.
        * @param aName The characteristic's name
        * @return Characteristic. Ownership is transferred.
        */
        IMPORT_C static CWPCharacteristic* CreateCharacteristicLC( const TDesC& aName );

        /** 
        * Create a CWPParameter.
        * @param aType The parameter id
        * @param aValue The parameter value
        * @return Parameter. Ownership is transferred.
        */
        IMPORT_C static CWPParameter* CreateParameterL( TInt aID, const TDesC& aValue );

        /** 
        * Create a CWPParameter.
        * @param aType The parameter id 
        * @param aValue The parameter value
        * @return Parameter. Ownership is transferred.
        */
        IMPORT_C static CWPParameter* CreateParameterLC( TInt aID, const TDesC& aValue );

        /** 
        * Create a CWPParameter.
        * @param aName The parameter name
        * @param aValue The parameter value
        * @return Parameter. Ownership is transferred.
        */
        IMPORT_C static CWPParameter* CreateParameterL( const TDesC& aName, 
            const TDesC& aValue );

        /** 
        * Create a CWPParameter.
        * @param aName The parameter name
        * @param aValue The parameter value
        * @return Parameter. Ownership is transferred.
        */
        IMPORT_C static CWPParameter* CreateParameterLC( const TDesC& aName,
            const TDesC& aValue );

        /** 
        * Create an element.
        * @param aType The element type
        * @return Element. Ownership is transferred.
        */
        IMPORT_C static MWPElement* CreateL( TInt aType );

        /** 
        * Create an element.
        * @param aType The element type
        * @return Element. Ownership is transferred.
        */
        IMPORT_C static MWPElement* CreateLC( TInt aType );
        
    };

#endif /* WPELEMENTFACTORY_H */