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
* Description:  WPAdapterFactory can be used to create provisioning adapters.
*
*/


#ifndef WPADAPTERFACTORY_H
#define WPADAPTERFACTORY_H

// INCLUDES
#include <e32base.h>

// FORWARD DECLARATIONS
class CWPAdapter;
class CImplementationInformation;

//  CLASS DEFINITION

/**
*  Factory class for adapters.
*
*  @lib ProvisioningEngine
*  @since 2.0
*/
class WPAdapterFactory 
    {
    public:
        /**
        * Creates all adapters and stores them in an array.
        * @return Array of adapters. Ownership is transferred.
        */
        static CArrayPtr<CWPAdapter>* CreateAdaptersL();

    private:

        /**
        * ResetAndDestroy() cleanup for an RImplInfoPtrArray.
        * @param aAny Array
        */
        static void CleanupImplArray( TAny* aAny );

        /**
        * ResetAndDestroy() cleanup for a CArrayPtr<CWPAdapter>.
        * @param aAny Array
        */
        static void CleanupAdapterArray( TAny* aAny );

        /** 
        * Comparator for two implementation information entries.
        * Compares the opaque data as a string.
        * @param aImpl1 First implementation to compare
        * @param aImpl2 Second implementation to compare
        */
        static TInt Compare( const CImplementationInformation& aImpl1, const CImplementationInformation& aImpl2 );
    };

#endif /* WPADAPTERFACTORY_H */