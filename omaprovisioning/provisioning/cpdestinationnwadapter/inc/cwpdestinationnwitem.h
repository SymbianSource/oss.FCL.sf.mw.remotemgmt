/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:   Settings item for one destination network. Also handles
*                saving destination networks to permanent storage.
*
*/



#ifndef CWPDESTINATIONNWITEM_H
#define CWPDESTINATIONNWITEM_H

#include <e32base.h>
#include <cmmanagerext.h>

class CWPCharacteristic;

/**
 *  Destination network settings item.
 *
 *  Destination network item handles saving destination networks via
 *  Connection Method Manager API.
 *
 *  @lib cwpdestinationnwadapter.lib
 *  @since S60 v3.2.
 */
class CWPDestinationNwItem : public CBase
    {
    /**
     * Friend classes for unit testing - uncomment when running tests.
     */
    //friend class UT_wpdestinationnwadapter;

public:

    static CWPDestinationNwItem* NewL();

    virtual ~CWPDestinationNwItem();

    /**
     * Setter for APPREF.
     * @since S60 v3.2.
     * @param aAPPREF.
     */
    void SetAppRefL( const TDesC8& aAppRef );

    /**
     * Setter for user viewable name of the setting.
     * @since S60 v3.2.
     * @param aNAME. Value is TCP or UDP.
     */
    void SetSettingsNameL( const TDesC& aSettingsName );

    /**
     * Setter for NAPDEF (IAP settings).
     * @since S60 v3.2.
     * @param aNapDef.
     */
    void AddNapDefL( CWPCharacteristic* aNapDef );

    /**
     * Getter for APPREF value.
     * @since S60 v3.2.
     * @return APPREF.
     */
    const TDesC8& AppRef() const;

    /**
     * Getter for user viewable name of Destination network.
     * @since S60 v3.2.
     * @return Settings name.
     */
    const TDesC& SettingsName() const;

    /**
     * Saves the destination network settings.
     * @since S60 v3.2.
     */
    void SaveL();

    /**
     * Return destination network ID.
     * @since S60 v3.2.
     * @return ID of the destination network.
     */
    const TDesC8& SaveData() const;

private:

    /**
     * Creates a network destination.
     * @since S60 v3.2.
     * @param aCmManager Connection Method Manager handle.
     * @return Network destination.
     */
    RCmDestinationExt CreateDestinationL( RCmManagerExt& aCmManager );

		/**
     * Returnes the Iap id that matches the given Wap id.
     * @since S60 v3.2.3
     * @param aWapId IAP's Wap Id.
     * @return Iap Id.
     */
		TUint32 IapIdFromWapIdL( TUint32 aWapId );
		
private:

    void ConstructL();
    CWPDestinationNwItem();

private: // data

    /**
     * Destination network reference identity in provisioning document.
     * Own.
     */
    HBufC8* iAppRef;

    /**
     * Name of destination network visible to user when viewing the
     * provisioning message.
     * Own.
     */
    HBufC* iSettingsName;

    /**
     * Array of NAPDEFs, IAP settings.
     * Not own. (Contained pointers are deleted by ProvisioningEngine.)
     */
    RPointerArray<CWPCharacteristic> iNapDefs;

    /**
     * ID value of the referred IAP.
     */
    TUint32 iIAPId;

    /** 
     * Destination network ID as a descriptor to be returned by the
     * SaveData method to the framework via adapters SaveDataL.
     * Own.
     */
    HBufC8* iSavedProfileId;

    };

#endif // CWPDESTINATIONNWITEM_H
