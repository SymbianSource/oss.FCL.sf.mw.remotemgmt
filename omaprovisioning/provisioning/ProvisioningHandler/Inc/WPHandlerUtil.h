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
* Description: 
*     WPHandlerUtil contains utility functions for ProvisioningHandler.
*
*/


#ifndef WPHANDLERUTIL_H 
#define WPHANDLERUTIL_H

// INCLUDES
#include <e32base.h>
#include "WPHandlerDebug.h"

// FORWARD DECLARATIONS

//  CLASS DEFINITION

/**
*  Utility class for ProvisioningHandler.
*
*  @lib ProvisioningHandler
*  @since 2.0
*/
class WPHandlerUtil 
    {
    public:
        /**
        * Prints a debug entry to FLogger.
        */
        static void Debug( TRefByValue<const TDesC> aText, ... );

        /**
        * Converts to hexadecimal and prints a debug entry to FLogger.
        */
        static void BinDebug( const TDesC8& aText );

    };

#endif /* WPHANDLERUTIL_H*/
