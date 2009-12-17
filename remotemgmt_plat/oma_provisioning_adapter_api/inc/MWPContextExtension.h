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
* Description:  Declares extension interface for adapters.
*
*/


#ifndef MWPCONTEXTEXTENSION_H
#define MWPCONTEXTEXTENSION_H

// INCLUDES
#include <e32def.h>

// FORWARD DECLARATIONS
class HBufC8;
class TDesC8;

// CLASS DECLARATION

/**
 * MWPContextExtension declares extension interface for adapters.
 *
 * @lib ProvisioningEngine
 * @since 2.0
 */ 
class MWPContextExtension 
    {
    public:

        /**
        * Returns the data used for saving.
        * @param aIndex The index of the data
        * @return The data. Ownership is transferred.
        */
        virtual const TDesC8& SaveDataL( TInt aIndex ) const = 0;

        /**
        * Deletes a saved item.
        * @param aSaveData The data used for saving
        */
        virtual void DeleteL( const TDesC8& aSaveData ) = 0;

        /**
        * Returns the UID of the adapter.
        * @return UID
        */
        virtual TUint32 Uid() const = 0;
    };

#endif /* MWPCONTEXTEXTENSION_H */
