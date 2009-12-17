/*
* Copyright (c) 2006-2006 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Handles Always-On settings in OMA provisioning.
*
*/


#ifndef C_ALWAYSONADAPTER_H
#define C_ALWAYSONADAPTER_H

// INCLUDES
#include    <CWPAdapter.h>

// FORWARD DECLARATIONS
class CRepository;

// MODULE DATA STRUCTURES

// ENUMERATIONS

// CLASS DECLARATION

/**
 *  CAlwaysOnAdapter handles AWON-PDPC and T-Retry OTA conifurations.
 *  The processes configurations are stored inside VENDORCONFIG element.
 *
 *  @lib
 *  @since S60 v3.1
 */
class CAlwaysOnAdapter : public CWPAdapter
    {
    private:    // MODULE DATA STRUCTURES

        /**
        * Struct for alwayson vendor config data.
        */
        struct TAlwaysOnVConfigAdapterData
            {
            TPtrC iName;
            TBool iAwonPdpcHPLMN;
            TBool iAwonPdpcVPLMN;
            TInt  iTRetry;
            };

    public:  // Constructors and destructor

        /**
        * Two-phased constructor.
        * @return an instance of class.
        */
        static CAlwaysOnAdapter* NewL();

        /**
        * Destructor
        */
        virtual ~CAlwaysOnAdapter();

    public: // Functions from base classes

        /**
        * From CWPAdapter
        */
        TInt ItemCount() const;

        /**
        * From CWPAdapter
        */
        const TDesC16& SummaryTitle( TInt aIndex ) const;

        /**
        * From CWPAdapter
        */
        const TDesC16& SummaryText( TInt aIndex ) const;

        /**
        * From CWPAdapter
        */
        TInt DetailsL( TInt aItem, MWPPairVisitor& aVisitor );

        /**
        * From CWPAdapter
        */
        void SaveL( TInt aItem );

        /**
        * From CWPAdapter
        */
        TBool CanSetAsDefault( TInt aItem ) const;

        /**
        * From CWPAdapter
        */
        void SetAsDefaultL( TInt aItem );

        /**
        * From CWPAdapter
        */
        void VisitL( CWPCharacteristic& aElement );

        /**
        * From CWPAdapter
        */
        void VisitL( CWPParameter& aElement );

        /**
        * From CWPAdapter
        */
        void VisitLinkL( CWPCharacteristic& aCharacteristic );

    private:  // Default constructors

        /**
        * C++ default constructor.
        */
        CAlwaysOnAdapter();

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();

    private: // New functions

        /**
        * Validates the data in iTempVCongigData.
        */
        TBool IsVConfigValid();

        /**
        * Initializes the data in iTempVCongigData.
        */
        void InitTempVConfigL();

        /**
        * Parses an integer from descriptor.
        */
        void ParseIntegerL( const TDesC& aPtr, TInt& aInt );

        /**
        * Parses the AwonPdpc values from integer.
        */
        void ParseAwonPdpcValuesL( TInt aInt );

    private: // Data

        /**
         * Pointer to validated vendor config data.
         * Own.
         */
        TAlwaysOnVConfigAdapterData* iVConfigData;

        /**
         * Pointer to vendor config data what is not yet validated.
         * Own.
         */
        TAlwaysOnVConfigAdapterData* iTempVConfigData;

        /**
         * Pointer PDPContextManager central repository.
         * Own.
         */
        CRepository* iPdpCenrep;

    private:
        friend class T_CAlwaysOnAdapter;


    };

#endif  // C_ALWAYSONADAPTER_H

// End of File
