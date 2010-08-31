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
* Description:  WPPhoneFactory can be used to create phone interface.
*
*/


#ifndef WPPHONEFACTORY_H
#define WPPHONEFACTORY_H

// FORWARD DECLARATIONS
class MWPPhone;

//  CLASS DEFINITION

/**
*  Factory class for phone interface.
*
*  @lib ProvisioningEngine
*  @since 2.0
*/
class WPPhoneFactory 
    {
    public:
        /**
        * Creates all adapters and stores them in an array.
        * @return Array of adapters. Ownership is transferred.
        */
        IMPORT_C static MWPPhone* CreateL();

        /**
        * Creates all adapters and stores them in an array.
        * @return Array of adapters. Ownership is transferred.
        */
        IMPORT_C static MWPPhone* CreateLC();
    };

#endif /* WPPHONEFACTORY_H*/