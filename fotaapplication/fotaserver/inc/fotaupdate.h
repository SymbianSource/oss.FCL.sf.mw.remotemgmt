/*
 * Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies). 
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
 * Description:   Header for updater active object
 *
 */
#ifndef __UPDATER_H___
#define __UPDATER_H___
//System includes
#include <bautils.h>
#include <schinfo.h>
#include <starterclient.h>
#include "FotaServer.h"
#include "fotaUpdateAgentComms.h"
#include "fotaengine.h"
#include <devicedialogconsts.h>
#include <hbsymbianvariant.h>
#include "fotadevicedialogobserver.h"
// CONSTANTS
_LIT (KUpdateResultFile, "update.resp");
_LIT (KUpdateRequestFile, "update.req");
_LIT (KUpdateBitmap ,"installing.bmp");
_LIT (KRestartingBitmap ,"restarting.bmp");
_LIT (KUpdateTimeStampFileName, "updatetimestamp");
_LIT8 (KSwupdPath8, "c:\\private\\102072C4\\");
_LIT8 (KSwupdFileExt8, ".swupd");

const TInt BatteryLevelAccepted = EBatteryLevelLevel3;

enum MonitorType
    {
    None, BatteryLevel, ChargingStatus
    };

// FORWARD  DECLARATIONS
class CFotaServer;
class CFotaUpdate;
class CFotaDownloadNotifHandler;

// CLASS DECLARATION

/**
 *  Update related activities
 *
 *  @lib    fotaserver
 *  @since  S60 v3.1
 */
NONSHARABLE_CLASS( CFotaUpdate ) : public CActive, MfotadevicedialogObserver
    {
public:

    static CFotaUpdate* NewL(CFotaServer* aServer);

    virtual ~CFotaUpdate();

    /*****Functions called before update*****/

    TBool CheckBatteryL();

    void MonitorBatteryChargeLevel();

    void MonitorBatteryChargingStatus();

    void CancelMonitor();

    /**
     * Start update
     *
     * @since   S60   v3.1
     * @param   aParams         details
     * @return  none
     */
    void StartUpdateL(const TDownloadIPCParams &aParams);

    /**
     * Update. boot.
     *
     * @since   S60   v3.1
     * @param   none
     * @return  none
     */
    void UpdateL();

    /*****Functions called after update *****/

    /**
     * Check update result written update agent. Is it there?
     *
     * @since   S60   v3.1
     * @param   aRfs
     * @return  is it there
     */
    static TBool CheckUpdateResults(RFs& aRfs);

    /**
     * Delete update result file
     *
     * @since   S60   v3.1
     * @param   none
     * @return  none
     */
    static void DeleteUpdateResultFileL();

    /**
     * Execute update result file. That is, read result code. Send it to srv.
     *
     * @since   S60   v3.1
     * @param   aRfs
     * @return  none
     */
    void ExecuteUpdateResultFileL();

protected:
    // from base classes


    /**
     * Hanlde notifier clicking
     *
     * @since   S60   v3.1
     * @param   none
     * @return  none
     */
    virtual void RunL();

    /**
     * Cancel outstanding reqeust
     *
     * @since   S60   v3.1
     * @param   none
     * @return  none
     */
    void DoCancel();

    /**
     * Handle leaving runl
     *
     * @since   S60   v3.1
     * @param   aError  error
     * @return  none
     */
    virtual TInt RunError(TInt aError);

private:

    CFotaUpdate();

    void ShowDialogL(TFwUpdNoteTypes dialogid);

    TBool IsUSBConnected();
    
    TInt WriteUpdateBitmapL( const TDesC& aText, const TDesC& aFile);

public:
    // from MfotadevicedialogObserver

    void HandleDialogResponse(int response, TInt aDialogid);

private:

    /**
     * reference to fotaserver.
     * Not own
     */
    CFotaServer* iFotaServer;

    RProperty iProperty;

    MonitorType iMonitorType;

    CHbSymbianVariantMap * iNotifParams;

    CFotaDownloadNotifHandler * iNotifier;
    };

#endif // __UPDATER_H___
