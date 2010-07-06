/*
 * Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies). 
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
 * Description: 
 *
 */
#ifndef FOTANOTIFIER_H
#define FOTANOTIFIER_H

// INCLUDES
#include <e32std.h>
#include <e32base.h>
#include <centralrepository.h>
#include <devicedialogconsts.h>
#include <e32property.h>

#include "FotaDlMgrClient.h"
#include "fotaserverPrivateCRKeys.h"

// CLASS DECLARATION

/**
 *  CClassName
 * 
 */

//#include <hb/hbcore/hbsymbiandevicedialog.h>    // MHbDeviceDialogObserver

NONSHARABLE_CLASS ( CFotaDownloadNotifHandler ) : public MHbDeviceDialogObserver
    {
public:
    // new functions
    /**
     * Construction and setting of observer happens here
     *
     * @since   S60   SF4
     * @param   aObserver is the parent class which owns the notifier and information is
     * passed to the parent class from the device dialog.
     */

    static CFotaDownloadNotifHandler* NewL(
            MfotadevicedialogObserver* aObserver);

    /**
     * constructor for CFotaDownloadNotifHandler
     *
     * @since   S60   SF4
     * @param   None
     */

    CFotaDownloadNotifHandler();

    /**
     * Destructor for CFotaDownloadNotifHandler
     *
     * @since   S60   SF4
     * @param   None
     */

    ~CFotaDownloadNotifHandler();

    /**
     * Shows notifier and/or ends download
     *
     * @since   S60   SF4
     * @param   aNotifParams   Contains necessary information to lauch a notifier
     */

    void CFotaDownloadNotifHandler::LaunchNotifierL(
            CHbSymbianVariantMap *aNotifParams, TInt aDialogId);

    void Cancel();

public:
    // from MHbDeviceDialogObserver

    /**
     * Slot to be called from the device dialog
     *
     * @aData   - It contains the information from the device dialog
     */
    void DataReceived(CHbSymbianVariantMap& aData);

    /**
     * Slot to be called from the device dialog is closed
     *
     * @aCompletionCode   - It contains the error code due to which dialog is closed.
     */
    void DeviceDialogClosed(TInt aCompletionCode);

private:

    //DownloadClient* iDownload;
    MfotadevicedialogObserver * iObserver;

    // Device dialog instance
    CHbDeviceDialogSymbian* iDevDialog;

    TInt iDialogID;

    };

#endif // FOTANOTIFIER_H
