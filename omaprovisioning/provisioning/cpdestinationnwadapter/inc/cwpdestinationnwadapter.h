/*
* Copyright (c) 2007-2008 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:   Handles destination network provisioning.
*
*/



#ifndef CWPDESTINATIONNWADAPTER_H
#define CWPDESTINATIONNWADAPTER_H

#include <CWPAdapter.h>
#include <MWPContextExtension.h>

#include "cwpdestinationnwitem.h"

class CWPCharacteristic;

/**
 *  Realizes destination network provisioning adapter.
 *
 *  Destination network adapter handles destination network provisioning and
 *  stores them via Connection Method Manager API.
 *
 *  @lib cwpdestinationnwadapter.lib
 *  @since S60 v3.2.
 */
class CWPDestinationNwAdapter : public CWPAdapter, public MWPContextExtension
    {
    /**
     * Friend class for unit testing - uncomment when running tests.
     */
    //friend class UT_wpdestinationnwadapter;

public:

    static CWPDestinationNwAdapter* NewL();

    virtual ~CWPDestinationNwAdapter();

    /**
     * Converts given 8-bit descriptor to TUint32.
     *
     * @since S60 v3.2.
     * @param aDes Descriptor to be converted.
     * @return Unsigned 32-bit integer.
     */
    TUint32 CWPDestinationNwAdapter::Des8ToUint32L( const TDesC8& aDes );

// from base class CWPAdapter

    /**
     * From CWPAdapter.
     * Checks the number of settings items.
     *
     * @since S60 v3.2.
     * @return Quantity of items (always one).
     */
    TInt ItemCount() const;

    /**
     * From CWPAdapter.
     * Getter for the summary title of Destination network settings. The text
     * is shown to the user in an opened configuration message.
     *
     * @since S60 v3.2.
     * @param aIndex Location of the settings item. Not used.
     * @return Summary title.
     */
    const TDesC16& SummaryTitle( TInt aIndex ) const;

    /**
     * From CWPAdapter.
     * Getter for the summary text of Destination network settings. The text
     * is shown to the user in opened configuration message.
     *
     * @since S60 v3.2.
     * @param aIndex Location of the settings item.
     * @return Summary text.
     */
    const TDesC16& SummaryText( TInt aIndex ) const;

    /**
     * From CWPAdapter.
     * Saves a destination network.
     *
     * @since S60 v3.2.
     * @param aIndex Location of the settings item.
     */
    void SaveL( TInt aIndex );

    /**
     * From CWPAdapter.
     * Returns EFalse since a destination cannot be set as default.
     *
     * @since S60 v3.2.
     * @param aIndex Location of the settings item to be queried. Not used.
     * @return Information whether these settings can be set as default.
     */
    TBool CanSetAsDefault( TInt /*aIndex*/ ) const;

    /**
     * From CWPAdapter.
     * Adapter sets the settings as default. Not supported.
     *
     * @since S60 v3.2.
     * @param aIndex Location of the settings item to be set as default.
     */
    inline void SetAsDefaultL( TInt /*aIndex*/ );

    /**
     * From CWPAdapter.
     * Query for detailed information about the destination network.
     * This is not supported feature as in other adapters in the framework.
     * Always returns KErrNotSupported.
     *
     * @since S60 v3.2.
     * @param aItem Not used but here because of inheritance.
     * @param aVisitor Object for retrieveing details of a setting entry.
     *                 Not used.
     * @return KErrNotSupported if not supported.
     */
    TInt DetailsL( TInt aItem, MWPPairVisitor& aVisitor );

    /**
     * From CWPAdapter.
     * Visit method. Visits the adapter. Adapter checks from 
     * aCharacteristic if the data is targeted to it and acts according to
     * that.
     *
     * @since S60 v3.2.
     * @param aCharacteristic The characteristic found.
     */
    void VisitL( CWPCharacteristic& aCharacteristic );        

    /**
     * From CWPAdapter.
     * Visit method. Visits the adapter. Method sets parameter values to
     * destination network settings item.
     *
     * @since S60 v3.2.
     * @param aParameter The parameter found.
     */
    void VisitL( CWPParameter& aParameter );        

    /**
     * From CWPAdapter.
     * Visit method. Used for linking settings to NAPDEF definition.
     *
     * @since S60 v3.2.
     * @param aLink Link to the characteristic.
     */
    void VisitLinkL( CWPCharacteristic& aLink );

    /**
     * From CWPAdapter.
     * Gets the saving information from the adapter that has saved settings.
     *
     * @since S60 v3.2.
     * @param aIndex Index of the previously saved item.
     * @param aSavingInfo Saving info.
     */
    void GetSavingInfoL( TInt aIndex, RPointerArray<HBufC8>& aSavingInfo );

    /**
     * From CWPAdapter.
     * Informs the adapters of the saved settings.
     * Not used.
     *
     * @since S60 v3.2.
     * @param aAppIdOfSavingItem UID of the adapter component. 
     * @param aAppRef APPREF of the saved settings.
     * @param aStorageIdValue Value that identifies the settings in its
     *        storage.
     */
    inline void SettingsSavedL( const TDesC8& /*aAppIdOfSavingItem*/,
        const TDesC8& /*aAppRef*/, const TDesC8& /*aStorageIdValue*/ );

    /**
     * From CWPAdapter.
     * Lets the adapters know when the saving has been finalized in 
     * viewpoint of the provisioning framework. The adapters can make 
     * finalization of saving the settings after receiving this call.
     * Not used.
     *
     * @since S60 v3.2.
     */
    inline void SavingFinalizedL();

// from base class MWPContextExtension

    /**
     * From MWPContextExtension.
     * Returns a pointer to a context extension.
     *
     * @since S60 v3.2.
     * @param aExtension Contains a pointer to MWPContextExtension if
     *                   supported.
     * @return KErrNotSupported if not supported, otherwise KErrNone.
     */
    TInt ContextExtension( MWPContextExtension*& aExtension );

    /**
     * From MWPContextExtension.
     * Returns the data used for saving.
     *
     * @since S60 v3.2.
     * @param aIndex The index of the data.
     * @return The data; ownership is transferred.
     */
    const TDesC8& SaveDataL( TInt aIndex ) const;

    /**
     * From MWPContextExtension.
     * Deletes a saved destination network.
     *
     * @since S60 v3.2.
     * @param aSaveData The data used for saving.
     */
    void DeleteL( const TDesC8& aSaveData );

    /**
     * From MWPContextExtension.
     * Returns the UID of the adapter.
     *
     * @since S60 v3.2.
     * @return UID of the adapter.
     */
    TUint32 Uid() const;

private:

    CWPDestinationNwAdapter();

    void ConstructL();

private: // data

    /**
     * Application ID of the current characteristic.
     */
    TPtrC iAppId;

    /**
     * Title for destination network. Value read from the resource file 
     * in initiation phase.
     * Own.
     */
    HBufC16* iDefaultTitle;

    /**
     * Name for destination network. Value read from the resource file 
     * in initiation phase.
     * Own.
     */
    HBufC16* iDefaultName;

    /**
     * Destination network item.
     * Own.
     */
    CWPDestinationNwItem* iCurrentItem;

    /**
     * Destination Network items.
     */
    RPointerArray<CWPDestinationNwItem> iItems;
    };

#include "cwpdestinationnwadapter.inl"

#endif // CWPDESTINATIONNWADAPTER_H
