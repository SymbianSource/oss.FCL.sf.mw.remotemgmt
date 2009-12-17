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


#ifndef MWPCONTEXTOBSERVER_H
#define MWPCONTEXTOBSERVER_H

// INCLUDES
#include <e32def.h>
#include <d32dbms.h>

// FORWARD DECLARATIONS

// CLASS DECLARATION

/**
 * MWPContextObserver declares extension interface for adapters.
 *
 * @lib ProvisioningEngine
 * @since 2.5
 */ 
class MWPContextObserver
    {
    public:

        /**
        * Called when the context database is changed.
        */
        virtual void ContextChangeL( RDbNotifier::TEvent aEvent ) = 0;

    };

#endif /* MWPCONTEXTOBSERVER_H */
