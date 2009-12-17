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
* Description:  Manager for all adapters.
*
*/


#ifndef CWPADAPTERMANAGER_H
#define CWPADAPTERMANAGER_H

// INCLUDES
#include <e32base.h>
#include "MWPContextExtensionArray.h"

// FORWARD DECLARATIONS
class CWPAdapter;
class MWPPairVisitor;
class CWPRoot;
class MWPContextManager;

// CLASS DECLARATION

/**
*  CWPAdapterManager owns and controls all adapters.
*
*  @lib ProvisioningEngine
*  @since 2.0
*/ 
class CWPAdapterManager : public CBase, public MWPContextExtensionArray
    {
    public:
        /**
        * Two-phased constructor.
        * @return Adapter manager
        */
        static CWPAdapterManager* NewL();

        /**
        * Two-phased constructor.
        * @return Adapter manager
        */
        static CWPAdapterManager* NewLC();

        /**
        * Destructor.
        */
        ~CWPAdapterManager();

    public: // From MWPContextExtensionArray
        TInt MwpceCount() const;
        MWPContextExtension&  MwpcePoint(TInt aIndex);

    public: // New methods

        /**
        * Populates adapters with a document.
        * @param aRoot The document root
        */
        void PopulateL( CWPRoot& aRoot );

        /**
        * Returns the number of settings items.
        * @return Number of saveable settings items
        */
        TInt ItemCount() const;

        /**
        * Returns a summary title.
        * @param aIndex Number of the setting item to read
        * @return The summary title for the item
        */
        const TDesC16& SummaryTitle(TInt aIndex) const;
        
        /**
        * Returns a summary text.
        * @param aIndex Number of the setting item to read
        * @return The summary text for the item
        */
        const TDesC16& SummaryText(TInt aIndex) const;
        
        /**
        * Calls a visitor for each details line of a particular item.
        * @param aIndex The number of the item
        * @param aVisitor The visitor to call
        * @return KErrNotSupported if not supported
        */
        TInt DetailsL(TInt aIndex, MWPPairVisitor& aVisitor);
        
        /**
        * Saves one setting.
        * @param aItem Setting number
        * @param aManager The context manager
        */
        void SaveL( MWPContextManager& aManager, TInt aItem );

        /**
        * Checks if the setting can be set as default.
        * @param aItem Setting number
        * @return ETrue if the setting can be set as default
        */
        TBool CanSetAsDefault( TInt aItem ) const;

        /**
        * Sets the setting as default.
        * @param aItem Setting number
        */
        void SetAsDefaultL( TInt aItem );

    private:
        /**
        * Converts an item number into a pair of adapter and adapter local item number.
        * @param aItem  Setting number
        * @param aAdapter Contains adapter number after the call
        * @param aAdapterIndex Contains adapter local item number after the call
        */
        void ItemIndex( TInt aItem , TInt& aAdapter, TInt& aAdapterIndex ) const;

    protected:
        /**
        * C++ default constructor.
        */
        CWPAdapterManager();

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();

    protected:
        
        /// All adapters. Owns.
        CArrayPtr<CWPAdapter>* iAdapters;

        /// Setting item line indexes. Owns.
        RArray<TInt> iItemIndexes;

        /// Number of setting items.
        TInt iItemCount;
    };

#endif /* CWPADAPTERMANAGER_H */
