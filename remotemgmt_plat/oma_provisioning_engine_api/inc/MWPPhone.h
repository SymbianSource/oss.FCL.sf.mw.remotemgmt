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


#ifndef MWPPHONE_H
#define MWPPHONE_H

// INCLUDES
#include <e32base.h>
#include <etelmm.h>

// FORWARD DECLARATIONS

// CLASS DECLARATION

/**
 * MWPPhone is an interface to telephony functionality.
 *
 * @since 2.0
 * @lib ProvisioningEngine
 */ 
class MWPPhone
    {
    public:

        /**
        * Returns the IMSI.
        * @return The IMSI
        */
        virtual const RMobilePhone::TMobilePhoneSubscriberId& SubscriberId() const = 0;

        /**
        * Returns the country code.
        * @return Country code
        */
        virtual const RMobilePhone::TMobilePhoneNetworkCountryCode& Country() const = 0;

        /**
        * Returns the network id.
        * @return Network id
        */ 
        virtual const RMobilePhone::TMobilePhoneNetworkIdentity& Network() const = 0;

        /**
        * Empty destructor.
        */
        virtual ~MWPPhone() {};

    };

#endif  // MWPPHONE_H
            
// End of File
