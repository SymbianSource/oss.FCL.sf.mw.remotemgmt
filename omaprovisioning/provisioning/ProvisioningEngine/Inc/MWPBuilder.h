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
* Description:  MWPBuilder can be used by the parser to pass data to the engine.
*
*/


#ifndef MWPBUILDER_H
#define MWPBUILDER_H

// INCLUDES
#include <e32base.h>

// CLASS DECLARATION

/**
 * MWPBuilder can be used by the parser to pass data to the engine.
*
*  @lib ProvisioningEngine
*  @since 2.0
 */ 
class MWPBuilder 
    {
    public:
        /**
        * Starts a new characteristic and sets it as current.
        * @param aType The characteristic type
        */
        virtual void StartCharacteristicL( TInt aType ) = 0;
        
        /**
        * Starts a new characteristic and sets it as current.
        * @param aName The name of the characteristic
        */
        virtual void StartCharacteristicL( const TDesC& aName ) = 0;
        
        /**
        * Ends the current characteristic.
        */
        virtual void EndCharacteristicL() = 0;

        /**
        * Inserts a parameter in the current characteristic.
        * @param aID The ID of the parameter
        * @param aValue The value of the parameter
        */
        virtual void ParameterL( TInt aID, const TDesC& aValue ) = 0;

        /**
        * Inserts a parameter in the current characteristic.
        * @param aName The name of the parameter
        * @param aValue The value of the parameter
        */
        virtual void ParameterL( const TDesC& aName, const TDesC& aValue ) = 0;
    };

#endif /* MWPBUILDER_H*/
