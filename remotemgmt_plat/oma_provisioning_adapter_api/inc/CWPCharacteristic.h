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
* Description:  CWPCharacteristic contains one characteristic.
*
*/


#ifndef CWPCHARACTERISTIC_H
#define CWPCHARACTERISTIC_H

// INCLUDES
#include <e32base.h>
#include "MWPElement.h"
#include "MWPVisitor.h"

// FORWARD DECLARATIONS
class CWPParameter;

// CLASS DECLARATION

/**
*  CWPCharacteristic contains one characteristic.
*
*  @lib ProvisioningEngine
*  @since 2.0
*/ 
class CWPCharacteristic : public CBase, public MWPElement
    {
    public:  // Constructors and destructor
        
        /**
        * Two-phased constructor.
        */
        static CWPCharacteristic* NewL( TInt aType );

        /**
        * Two-phased constructor.
        */
        static CWPCharacteristic* NewLC( TInt aType );

        /**
        * Destructor.
        */
        ~CWPCharacteristic();

    public:// New functions

        /**
        * Inserts a parameter to the characteristic. Ownership is tranferred.
        * @param aParameter Parameter to insert.
        */
        IMPORT_C void InsertL(MWPElement* aElement);

        /**
        * Inserts a link to a characteristic.
        * @param aParameter Parameter to insert.
        */
        IMPORT_C void InsertLinkL(CWPCharacteristic& aParameter);

        /**
        * Sets the characteristic's name.
        * @param aName Characteristic's new name
        */
        IMPORT_C void SetNameL( const TDesC& aValue );

        /**
        * Returns the characteristic's name.
        * @return Characteristic's name
        */
        IMPORT_C const TDesC& Name() const;
        
        /**
        * Sets adapter specific data. Takes a copy.
        * @param aData Data to be copied.
        */
        IMPORT_C void SetDataL( const TDesC8& aData );

        /**
        * Sets adapter specific data in specific slot. Takes a copy.
        * @param aData Data to be copied.
        * @param aIndex index of slot where to store the data. 
        */
        IMPORT_C void SetDataL( const TDesC8& aData, TInt aIndex );

        /**
        * Deletes all adapter specific data. 
        */
        IMPORT_C void DeleteAllData();

        /**
        * Returns adapter specific data.
        * @return Data as a descriptor.
        */
        IMPORT_C const TDesC8& Data() const;

        /**
        * Returns adapter specific data from the specific slot.
        * @param aIndex index of the slot from where to get data. Return KNullDesC
        * if the slot doesn't contain anything.
        * @return Data as a descriptor.
        */
        IMPORT_C const TDesC8& Data( TInt aIndex ) const;

        /**
        * Returns all parameters with a certain type inside this characteristic.
        * @param aID The ID of the parameter to look for
        * @param aParameters Table to store all values for the parameter
        * @return All parameter values with a certain type
        */
        IMPORT_C void ParameterL( TInt aID, CArrayFix<TPtrC>* aParameters ); 

        /**
        * Accepts a visitor for the parameters in characteristic.
        * @param aVisitor The visitor to be called for each element.
        */
        IMPORT_C void AcceptL(MWPVisitor& aVisitor);

    public: // From MWPElement

        void CallVisitorL( MWPVisitor& aVisitor );
        void InternalizeL( RReadStream& aStream );
        void ExternalizeL( RWriteStream& aStream ) const;
        TInt Type() const;

    private:
        /**
        * C++ default constructor.
        */
        CWPCharacteristic( TInt aType );

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();

    private:
        /// The type of the CHARACTERISTIC
        TInt iType;

        /// The name of the CHARACTERISTIC
        HBufC* iName;

        /// Parameters in this characteristic. Owns.
        RPointerArray<MWPElement> iElements;

        /// Parameters in this characteristic. Owns.
        RPointerArray<CWPCharacteristic> iLinks;

        /// Adapters can set data here. Owns.
        CArrayPtr<HBufC8>* iData;
    };

#endif /* CWPCHARACTERISTIC_H */
