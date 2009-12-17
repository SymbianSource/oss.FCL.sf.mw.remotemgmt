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
* Description:  Multiple context manager.
*
*/


#ifndef CWPBINDINGCONTEXTMANAGER_H
#define CWPBINDINGCONTEXTMANAGER_H

// INCLUDES
#include <e32base.h>
#include "CWPMultiContextManager.h"

// FORWARD DECLARATIONS
class CDesC8Array;

// CLASS DECLARATION

/**
*  CWPBindingContextManager handles data in contexts.
*
*  @lib ProvisioningEngine
*  @since 2.1
 */ 
class CWPBindingContextManager : public CWPMultiContextManager
    {
    public:  // Constructors and destructor
        
        /**
        * Two-phased constructor.
        * @return Context manager
        */
        static CWPBindingContextManager* NewL();

        /**
        * Two-phased constructor.
        * @return Context manager
        */
        static CWPBindingContextManager* NewLC();

        /**
        * Destructor.
        */
        ~CWPBindingContextManager();

    public: // From MWPContextManager

        void DeleteContextL( MWPContextExtensionArray& aArray, TUint32 aUid );
        TInt ContextDataCountL( TUint32 aUid );
        TBool DeleteContextDataL( MWPContextExtensionArray& aArray, TUint32 aUid );
        void SaveL( MWPContextExtension& aExtension, TInt aItem );
        CDesC8Array* ContextDataL( TUint32 aUid );

    private: // New methods
        /**
        * C++ default constructor.
        */
        CWPBindingContextManager();

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();
    };

#endif /* CWPBINDINGCONTEXTMANAGER_H */
