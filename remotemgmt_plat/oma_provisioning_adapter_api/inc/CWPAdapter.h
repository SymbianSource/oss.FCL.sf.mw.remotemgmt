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
* Description:  Declares adapter interface.
*
*/


#ifndef CWPADAPTER_H
#define CWPADAPTER_H

// INCLUDES
#include <e32base.h>
#include <ecom/ecom.h>
#include "MWPVisitor.h"

// FORWARD DECLARATIONS
class MWPPairVisitor;
class MWPContextExtension;

// DATA TYPES
enum TWPSaveLeaveCode
    {
    EWPAccessPointsProtected = -49152,
    EWPCommsDBLocked = -49153,
    EWPBookmarksLocked = -49154
    };

// CLASS DECLARATION

/**
 * CWPAdapter has to be implemented by each adapter.
 * CWPAdapter contains only pure virtual methods, which must be 
 * implemented by adapters, and inline methods, which can be
 * used by adapters or engine. Inlining prevents circular binary
 * dependency.
*
*  @lib ProvisioningEngine
*  @since 2.0
 */ 
class CWPAdapter : public CBase, public MWPVisitor
    {
    public:
        /**
        * Two-phased constructor.
        * @param aImplementationUid The implementation UID of the adapter to be created.
        * @return New adapter
        */
        IMPORT_C static CWPAdapter* NewL( const TUid& aImplementationUid );

        /**
        * Destructor.
        */
        IMPORT_C ~CWPAdapter();

    public:
        /**
        * Lists all CWPAdapter implementations.
        * @param aImplInfoArray Array to be filled with implementations
        */
        static void ListL(RImplInfoPtrArray& aImplInfoArray);

        /**
        * Check the number of settings items.
        * @return Number of items
        */
        virtual TInt ItemCount() const = 0;

        /**
        * Returns the title of the nth summary line. 
        * @param aIndex Number of summary line
        * @return Summary line title
        */
        virtual const TDesC16& SummaryTitle(TInt aIndex) const = 0;
        
        /**
        * Returns the text of the nth summary line. 
        * @param aIndex Number of summary line
        * @return Summary line text
        */
        virtual const TDesC16& SummaryText(TInt aIndex) const = 0;
        
        /**
        * Saves one setting handled by the adapter.
        * @param Setting number
        */
        virtual void SaveL( TInt aItem ) = 0;

        /**
        * Returns ETrue if the adapter can set the settings as default.
        * @param Setting number
        * @return ETrue if the setting can be made default
        */
        virtual TBool CanSetAsDefault( TInt aItem ) const = 0;

        /**
        * Adapter sets the settings as default.
        * @param Setting number
        */
        virtual void SetAsDefaultL( TInt aItem ) = 0;

        /**
        * Calls the pair visitor for each detail line of an item.
        * @param aItem Item whose details are needed
        * @param aVisitor Pair visitor
        * @return KErrNotSupported if not supported
        */
        virtual TInt DetailsL( TInt aItem, MWPPairVisitor& aVisitor ) = 0;

    public: // From MWPVisitor

        void VisitL(CWPCharacteristic& aCharacteristic) = 0;
        void VisitL(CWPParameter& aParameter) = 0;
        void VisitLinkL(CWPCharacteristic& aCharacteristic ) = 0;

    protected:
        /**
        * C++ constructor.
        */
        IMPORT_C CWPAdapter();

    private:
        /// Reserved. Must return KErrNotSupported.
        virtual TInt Reserved_1() { return KErrNotSupported; };
        
    public:
        /**
        * Returns a pointer to a context extension.
        * @param aExtension Contains a pointer to MWPContextExtension if supported
        * @return KErrNotSupported if not supported, KErrNone otherwise
        */
        virtual TInt ContextExtension( MWPContextExtension*& /*aExtension*/ ) 
            { 
            return KErrNotSupported; 
            };
            
// Code block based on the APPREF & TO-APPREF parameter handling starts here.
        /**
        * Gets the saving information from the adapter that has saved settings.
        * @param TInt aIndex. Index of the previously saved item.
        * @param RPointerArray< HBufC8 > aSavingInfo.
        * @param TBool& aIsLastOfItsType. Adapter can tell to the framework if 
        *        the saved item was the last one of its type.
        */
		virtual void GetSavingInfoL( TInt  /*aIndex*/,
		                             RPointerArray< HBufC8 >& /*aSavingInfo*/ )
		    {
		    };

        /**
        * Informs the adapters of the saved settings.
        * @param TDesC8& aAppIdOfSavingItem. UID of the adapter component. 
        * @param HBufC8& aAppRef. APPREF of the saved settings.
        * @param HBufC8& aStorageIdValue. Value that identifies the settings in
        *        its storage.
        * @param TBool& aIsLastOfItsType. ETrue is gotten if the saved set 
        *        was last of its type.
        */
        virtual void SettingsSavedL ( const TDesC8& /*aAppIdOfSavingItem*/,
                                      const TDesC8& /*aAppRef*/,
                                      const TDesC8& /*aStorageIdValue*/ )
            {
            };

        /**
        * Informs the adapters of the finalization of saving.
        */
        virtual void SavingFinalizedL()
            {
            };
            
// Code block based on the APPREF & TO-APPREF parameter handling ends here.

    protected:
        /// Implementation UID of the adapter
        TUid iDtor_ID_Key;

    };

#endif /* CWPADAPTER_H */
