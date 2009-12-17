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
* Description:  WPContextManagerFactory can be used to create a context manager.
*
*/


#ifndef WPCONTEXTMANAGERFACTORY_H
#define WPCONTEXTMANAGERFACTORY_H

// FORWARD DECLARATIONS
class MWPContextManager;

//  CLASS DEFINITION

/**
*  Factory class for context manager.
*
*  @lib ProvisioningHandler
*  @since 2.0
*/
class WPContextManagerFactory 
    {
    public:
        /**
        * Creates a context manager.
        * @return Context manager
        */
        static MWPContextManager* CreateL();

        /**
        * Creates a context manager.
        * @return Context manager
        */
        static MWPContextManager* CreateLC();
    };

#endif /* WPCONTEXTMANAGERFACTORY_H */
