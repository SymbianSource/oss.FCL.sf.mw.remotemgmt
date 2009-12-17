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
* Description:  Emulator for CWPPhone
*
*/


#ifndef CWPPHONEEMUL_H
#define CWPPHONEEMUL_H

// INCLUDES
#include <e32base.h>
#include <etelmm.h>
#include "MWPPhone.h"

// FORWARD DECLARATIONS

// CLASS DECLARATION

/**
 * Emulator for CWPPhone.
*
*  @lib ProvisioningEngine
*  @since 2.0
 */ 
class CWPPhoneEmul : public CBase, public MWPPhone
    {
    public:  // Constructors and destructor

        /**
        * Two-phased constructor.
        */
        static CWPPhoneEmul* NewL();

        /**
        * Two-phased constructor.
        */
        static CWPPhoneEmul* NewLC();

        /**
        * Destructor.
        */
        virtual ~CWPPhoneEmul();

    public: // From MWPPhone

        const RMobilePhone::TMobilePhoneSubscriberId& SubscriberId() const;
        const RMobilePhone::TMobilePhoneNetworkCountryCode& Country() const;
        const RMobilePhone::TMobilePhoneNetworkIdentity& Network() const;

    private:
        /**
        * C++ default constructor.
        */
        CWPPhoneEmul();

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();

    private:
        /// IMSI.
        RMobilePhone::TMobilePhoneSubscriberId iIMSI;
        /// Country code
        RMobilePhone::TMobilePhoneNetworkCountryCode iCountry;
        /// Network identity
        RMobilePhone::TMobilePhoneNetworkIdentity iNetwork;
    };

#endif  // CWPPHONEEMUL_H
            
// End of File
