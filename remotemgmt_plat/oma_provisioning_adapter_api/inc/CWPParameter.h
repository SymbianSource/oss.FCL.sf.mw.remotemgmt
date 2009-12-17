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
* Description:  One parameter in a Provisioning document.
*
*/


#ifndef CWPPARAMETER_H
#define CWPPARAMETER_H

// INCLUDES
#include <e32base.h>
#include "MWPElement.h"

// DATA TYPES
/// Parameter IDs follow the WBXML specification.
enum TWPParameterID
    {
    // This value means that the name of the parameter 
    // has been stored instead of an id
    EWPNamedParameter = 0,

    // Actual parameter ids follow
    EWPParameterName = 7,
    EWPParameterNapAddress,
    EWPParameterNapAddrType,
    EWPParameterCallType,
    EWPParameterValidUntil,
    EWPParameterAuthType,
    EWPParameterAuthName,
    EWPParameterAuthSecret,
    EWPParameterLinger,
    EWPParameterBearer,
    EWPParameterNapID,
    EWPParameterCountry,
    EWPParameterNetwork,
    EWPParameterInternet,
    EWPParameterProxyID,
    EWPParameterProxyProviderID,
    EWPParameterDomain,
    EWPParameterProvURL,
    EWPParameterPxAuthType,
    EWPParameterPxAuthID,
    EWPParameterPxAuthPW,
    EWPParameterStartPage,
    EWPParameterBasAuthID,
    EWPParameterBasAuthPW,
    EWPParameterPushEnabled,
    EWPParameterPxAddr,
    EWPParameterPxAddrType,
    EWPParameterToNapID,
    EWPParameterPortNbr,
    EWPParameterService,
    EWPParameterLinkspeed,
    EWPParameterDnLinkSpeed,
    EWPParameterLocalAddr,
    EWPParameterLocalAddrType,
    EWPParameterContextAllow,
    EWPParameterTrust,
    EWPParameterMaster,
    EWPParameterSID,
    EWPParameterSOC,
    EWPParameterWSPVersion,
    EWPParameterPhysicalProxyID,
    EWPParameterClientID,
    EWPParameterDeliveryErrSDU,
    EWPParameterDeliveryOrder,
    EWPParameterTrafficClass,
    EWPParameterMaxSDUSide,
    EWPParameterMaxBitrateUplink,
    EWPParameterMaxBitrateDnlink,
    EWPParameterResidualBER,
    EWPParameterSDUErrorRatio,
    EWPParameterTrafficHandlPrio,
    EWPParameterTransferDelay,
    EWPParameterGuaranteedBitrateUplink,
    EWPParameterGuaranteedBitrateDnlink,
    EWPParameterPxAddrFQDN,
    EWPParameterProxyPW,
    EWPParameterPPGAuthType,
    EWPParameterPullEnabled = 0x47,
    EWPParameterDNSAddr,
    EWPParameterMaxNumRetry,
    EWPParameterFirstRetryTimeout,
    EWPParameterReregThreshold,
    EWPParameterTBit,
    EWPParameterAuthEntity = 0x4e,
    EWPParameterSPI,

    EWPParameterDirection = 0x60,
    EWPParameterDNSAddrType,
    EWPParameterDNSPriority,
    EWPParameterSourceStatisticsDescriptor,
    EWPParameterSignallingIndication,
    EWPParameterDefGW,
    EWPParameterNetworkMask,
    EWPParameterUseCB,
    EWPParameterCBNBR,
    EWPParameterPPPComp,
    EWPParameterToLoginScriptId,
    EWPParameterUsePTxtLog,
    EWPParameterGPRSPDP,
    EWPParameterModemInit,
    EWPParameterIPAddrFromServer,
    EWPParameterDNSAddrFromServer,
    EWPParameterIPv6AddrFromServer,
    EWPParameterIfNetworks,
    EWPParameterIAPService,
    EWPParameterSSID,
    EWPParameterNetworkMode,
    EWPParameterSecurityMode,
    EWPParameterWPAPreSharedKey,
    EWPParameterLength,
    EWPParameterData,

    // Code page 1
    EWPParameterName1 = 0x07+0x100, 
    EWPParameterInternet1 = 0x14+0x100, 
    EWPParameterStartPage1 = 0x1C+0x100,
    EWPParameterToNapID1 = 0x22+0x100,
    EWPParameterPortNbr1 = 0x23+0x100,
    EWPParameterService1  = 0x24+0x100,

    // Skip in code page
    EWPParameterAAccept = 0x2E + 0x100,
    EWPParameterAAuthData,
    EWPParameterAAuthLevel,
    EWPParameterAAuthName,
    EWPParameterAAuthSecret,
    EWPParameterAAuthType,
    EWPParameterAddr,
    EWPParameterAddrType,
    EWPParameterAppID,
    EWPParameterAProtocol,
    EWPParameterProviderID,
    EWPParameterToProxy,
    EWPParameterURI,
    EWPParameterRule,
    EWPParameterAppRef,
    EWPParameterToAppRef,
    EWPParameterLoginTW,
    EWPParameterLoginScriptType,
    EWPParameterLoginScriptData,
    EWPParameterLoginScriptId,

    EWPParameterFrom = 0x0201,
    EWPParameterMaxBandwidth,
    EWPParameterMinUDPPort,
    EWPParameterMaxUDPPort,

    // This enumeration will always be the last element in enumeration 
    EWPLastParameter
    };

/**
*  CWPParameter contains one parameter.
*
*  @lib ProvisioningEngine
*  @since 2.0
*/ 
class CWPParameter : public CBase, public MWPElement  
    {
    public:  // Constructors and destructor
        
        /**
        * Two-phased constructor.
        */
        IMPORT_C static CWPParameter* NewL();

        /**
        * Two-phased constructor.
        */
        //   Static constructor
        IMPORT_C static CWPParameter* NewLC();

        /**
        * Destructor.
        */
        ~CWPParameter();

    public: // New methods

        /**
        * Sets the parameter's ID. Must be one of enumeration
        * TWPParameterID.
        * @param aID Parameter's ID
        */
        IMPORT_C void SetID( TInt aId );

        /**
        * Returns the parameter's ID.
        * @return Parameter's ID
        */
        IMPORT_C TInt ID() const;
        
        /**
        * Sets the parameter's name.
        * @param aName Parameter's new name
        */
        IMPORT_C void SetNameL( const TDesC& aValue );

        /**
        * Returns the parameter's name.
        * @return Parameter's name
        */
        IMPORT_C const TDesC& Name() const;
        
        /**
        * Sets the parameter's value.
        * @param aValue Parameter's new value
        */
        IMPORT_C void SetValueL( const TDesC& aValue );

        /**
        * Returns the parameter's value.
        * @return Parameter's value
        */
        IMPORT_C const TDesC& Value() const;
        
    public: // From CWPElement

        void CallVisitorL( MWPVisitor& aVisitor );
        IMPORT_C void InternalizeL( RReadStream& aStream );
        IMPORT_C void ExternalizeL( RWriteStream& aStream ) const;
        IMPORT_C TInt Type() const;

    private:

        /**
        * C++ default constructor.
        */
        CWPParameter();

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();


    private:
        /// ID of the parameter. See TWPParameterIDs.
        TInt iParameterID;
        
        /// Name of the parameter. Owns.
        HBufC* iName;

        /// Value of the parameter. Owns.
        HBufC* iValue;
        
    };

#endif /* CWPPARAMETER_H */
