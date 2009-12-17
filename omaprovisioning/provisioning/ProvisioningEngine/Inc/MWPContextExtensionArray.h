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


#ifndef MWPCONTEXTEXTENSIONARRAY_H
#define MWPCONTEXTEXTENSIONARRAY_H

// FORWARD DECLARATIONS
class MWPContextExtension;

// CLASS DECLARATION

/**
 * MWPContextExtension has to be implemented by visitors.
*
*  @lib ProvisioningEngine
*  @since 2.0
 */ 
class MWPContextExtensionArray
    {
    public:
        /**
        * Returns the number of context extensions in this array.
        * @return Number of context extensions
        */
        virtual TInt MwpceCount() const = 0;

        /**
        * Returns a context extension.
        * @param aIndex Index of the extension
        * @return The reference to extension
        */
        virtual MWPContextExtension&  MwpcePoint(TInt aIndex) = 0;
    };

#endif /* MWPCONTEXTEXTENSIONARRAY_H*/
