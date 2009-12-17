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
* Description:  Phone interface for handler
*
*/


#ifndef CWPPHONE_H
#define CWPPHONE_H

// INCLUDES
#include <e32base.h>
#include "MWPPhone.h"

// FORWARD DECLARATIONS

// CLASS DECLARATION

/**
 * CWPPhone reads IMSI from SIM card.
*
*  @lib ProvisioningEngine
*  @since 2.0
 */ 
class CWPPhone : public CBase, public MWPPhone
    {
    public:  // Constructors and destructor

        /**
        * Two-phased constructor.
        */
        static CWPPhone* NewL();

        /**
        * Two-phased constructor.
        */
        static CWPPhone* NewLC();

        /**
        * Destructor.
        */
        virtual ~CWPPhone();

    public: // From MWPPhone

        const RMobilePhone::TMobilePhoneSubscriberId& SubscriberId() const;
        const RMobilePhone::TMobilePhoneNetworkCountryCode& Country() const;
        const RMobilePhone::TMobilePhoneNetworkIdentity& Network() const;

    private:
        /**
        * C++ default constructor.
        */
        CWPPhone();

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();

    private:  // Data
        /// IMSI.
        RMobilePhone::TMobilePhoneSubscriberId iIMSI;
        /// Country code
        RMobilePhone::TMobilePhoneNetworkCountryCode iCountry;
        /// Network identity
        RMobilePhone::TMobilePhoneNetworkIdentity iNetwork;
    };

#endif  // CWPPHONE_H
            
// End of File
