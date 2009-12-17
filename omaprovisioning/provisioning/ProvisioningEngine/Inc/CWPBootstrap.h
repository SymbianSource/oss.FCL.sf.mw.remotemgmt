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
* Description:  CWPBootstrap loads/saves settings.
*
*/


#ifndef CWPBOOTSTRAP_H
#define CWPBOOTSTRAP_H

// INCLUDES
#include "MWPVisitor.h"
#include <e32base.h>
#include <etelmm.h>

// FORWARD DECLARATIONS
class CDesC16Array;
class CWPPushMessage;
class CWPEngine;

//  CLASS DEFINITION

/**
*  Utility class for reading/writing data store.
*
*  @lib ProvisioningEngine
*  @since 2.0
*/
class CWPBootstrap : public CBase, private MWPVisitor
    {
    public:
        enum TBootstrapResult 
            { 
            // The message does not contain a bootstrap.
            ENoBootstrap,

            // The message contains a bootstrap, but no authentication
            ENotAuthenticated,

            // The message contains a bootstrap, and a PIN is required.
            EPinRequired,

            // The PIN is wrong.
            EAuthenticationFailed, 

            // Bootstrap already exists
            EBootstrapExists,

            // Authentication succeeded.
            ESucceeded
            };

    public:  // Constructors and destructor
        
        /**
        * Two-phased constructor.
        * @param aIMSI The SIM card number
        */
        IMPORT_C static CWPBootstrap* NewL( 
            const TDesC& aIMSI );

        /**
        * Two-phased constructor.
        * @param aIMSI The SIM card number
        */
        static CWPBootstrap* NewLC( 
            const TDesC& aIMSI );

        /**
        * Destructor.
        */
        ~CWPBootstrap();

    public:
        /**
        * Authenticates a message and tries to perform a
        * bootstrap if the message authenticated. If the 
        * message has previously been authenticated, only
        * bootstrap is performed.
        * @param aMessage The message to authenticate
        * @param The Provisioning Engine
        * @param aPIN The PIN. If empty, no PIN is used.
        * @return Result of bootstrap. If EPinRequired,
        *         a PIN must be provided.
        */
        IMPORT_C TBootstrapResult BootstrapL( 
            CWPPushMessage& aMessage,
            CWPEngine& aEngine,
            const TDesC& aPIN );

        /**
        * The contents of the ProvURL field.
        * @return ProvURL
        */
        IMPORT_C const TDesC& TPS() const;

    private:    // From MWPVisitor
        void VisitL(CWPCharacteristic& aCharacteristic);
        void VisitL(CWPParameter& aParameter);
        void VisitLinkL(CWPCharacteristic& aCharacteristic );

    private:
        /**
        * C++ default constructor.
        * @param aIMSI The SIM card number
        */
        CWPBootstrap( const TDesC& aIMSI );

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();

        /**
        * Performs a bootstrap.
        * @param aEngine The Provisioning Engine
        * @return Result of bootstrap.
        */
        TBootstrapResult DoBootstrapL( CWPEngine& aEngine );

        /**
        * Processes bootstrap when the message has to be 
        * authenticated first.
        * @param aMessage The message to authenticate
        * @param aPIN The PIN. If empty, no PIN is used.
        * @param aEngine The Provisioning Engine
        * @return Result of bootstrap. If EPinRequired,
        *         a PIN must be provided.
        */
        TBootstrapResult DoAuthenticateL( CWPPushMessage& aMessage,
            const TDesC& aPIN, 
            CWPEngine& aEngine );

        /**
        * Saves the settings.
        * @param aEngine The Provisioning Engine
        */
        void SaveL( CWPEngine& aEngine );

        /**
        * Loads the settings.
        * @param aEngine The Provisioning Engine
        */
        void LoadL( CWPEngine& aEngine );

        /**
        * Checks if settings have been loaded.
        * @return ETrue if settings have been loaded
        */
        TBool Loaded() const;

    private:

        /// ETrue if settings have been loaded
        TBool iLoaded;

        /// The trusted provisioning server. Owns.
        HBufC* iTPS;

        /// The name of the bootstrap. Owns.
        HBufC* iName;

        /// The IMSI
        RMobilePhone::TMobilePhoneSubscriberId iIMSI;

        /// The proxies. Owns.
        CDesC16Array* iProxies;

        /// The ID of the current characteristic
        TInt iCurrentChar;

    };

#endif /* CWPBOOTSTRAP_H */
