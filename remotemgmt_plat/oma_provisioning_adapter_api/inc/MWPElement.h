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
* Description:  Declares an abstract element in WAP Provisioning.
*
*/


#ifndef CWPELEMENT_H
#define CWPELEMENT_H

// INCLUDES
#include <e32base.h>

// CONSTANTS
enum TWPElementTypes 
    {
    KWPElement,
    KWPParameter,
    KWPPxLogical = 0x51,    // codepage 0
    KWPPxPhysical,
    KWPPort,
    KWPValidity,
    KWPNapDef,
    KWPBootstrap,
    KWPVendorConfig,
    KWPClientIdentity,
    KWPPxAuthInfo,
    KWPNapAuthInfo,
    KWPAccess,
    KWPBearerInfo,
    KWPDNSAddrInfo,
    KWPWLAN,
    KWPWepKey,
	KWPWebKey = 0x5f,
    KWPPort1 =0x53+0x100,   // codepage 1
                                    // 0x54 empty
    KWPApplication = 0x55+0x100,    
    KWPAppAddr,
    KWPAppAuth,
    KWPClientIdentity1,
    KWPResource,
    KWPLoginScript,

    KWPNamedCharacteristic = 0xffff
    };

// DATA TYPES

// FORWARD DECLARATIONS
class RWriteStream;
class RReadStream;
class MWPVisitor;

// CLASS DECLARATION

/**
*  Abstract base class for the contents of a WAP Provisioning document.
*
*  @lib ProvisioningEngine
*  @since 2.0
*/
class MWPElement 
    {
    public: // New functions
        /**
        * Destructor.
        */
        virtual ~MWPElement() {};

        /**
        * Calls the visitor with this as parameter.
        * @param aVisitor Visitor is called.
        */
        virtual void CallVisitorL( MWPVisitor& aVisitor ) = 0;

        /**
        * Returns the type of the element.
        * @return Type
        */
        virtual TInt Type() const = 0;
        
        /**
        * Externalizes this element.
        * @param aStream Stream to externalize to.
        */
        virtual void ExternalizeL(RWriteStream& aStream) const = 0;
        
        /**
        * Internalizes this element.
        * @param aStream Stream to internalize from.
        */
        virtual void InternalizeL(RReadStream& aStream) = 0;
    };

#endif /* CWPELEMENT_H */
