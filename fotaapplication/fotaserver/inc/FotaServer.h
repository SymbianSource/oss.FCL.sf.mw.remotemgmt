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
 * Description:   Header file for CFotaServer
 *
 */

#ifndef __FOTASERVER_H__
#define __FOTASERVER_H__

// INCLUDES

#include <coemain.h>
#include <s32file.h>
#include <e32property.h>
#include <SyncMLClient.h>
#include <SyncMLClientDM.h>
#include <hwrmpowerstatesdkpskeys.h>
#include <SyncMLObservers.h>
#include <devicedialogconsts.h>
#include <hbsymbianvariant.h>
#include <hbmainwindow.h>
#include "fotaserverPrivateCRKeys.h"
#include "FotaIPCTypes.h"
#include "FotaDB.h"
#include "fotaConst.h"
#include "fotaupdate.h" 
#include "fmsclient.h"	//The FMS Client header file
#include "FotaSrvDebug.h"
#include "fotadevicedialogobserver.h"

/** If download is active this P&S key is set to ETrue, otherwise EFalse. 
 * The key is owned by omadmappui**/
const TUint32 KFotaServerActive = 0x0000008;
#define __LEAVE_IF_ERROR(x) if(KErrNone!=x) {FLOG(_L("LEAVE in %s: %d"), __FILE__, __LINE__); User::Leave(x); }

//Forward declarations
class FotaFullscreenDialog;
/** File that stores the firmware version at the start of download. This is in fota's private directory*/
_LIT (KSWversionFile, "swv.txt");

/** Time and the interval download finalizing retries*/
const TInt KDownloadFinalizerWaitTime(1000000);

/** Time interval between syncml session retry attempts*/
const TInt KSyncmlSessionRetryInterval(60000000);

/** Time and the interval between the fota shutdownload retries*/
const TTimeIntervalMicroSeconds32 KFotaTimeShutDown(10000000);

/** How many times generic alert sending should be tried in row. */
const TInt KSyncmlAttemptCount = 3;

/** How many times a download can be restarted when a non-resumable interrupt occurs. */
const TInt KMaxDownloadRestartCount = 3;

_LIT_SECURITY_POLICY_C1( KReadPolicy, ECapabilityReadDeviceData );
_LIT_SECURITY_POLICY_C1( KWritePolicy, ECapabilityWriteDeviceData );
_LIT_SECURITY_POLICY_PASS(KAllowAllPolicy);

// FORWARD CLASS DECLARATION

class CFotaUpdate;
class CFotaSrvApp;
class CFotaNetworkRegStatus;
class DownloadManagerClient;
class CFotaDownloadNotifHandler;
/**
 * Fota server. Handles client requests. Starts download and/or update.
 *
 *  This class is 
 *
 *  @lib    fotaserver
 *  @since  S60 v3.1
 */
NONSHARABLE_CLASS( CFotaServer ) : public CServer2,
        MSyncMLEventObserver,
        MfotadevicedialogObserver
    {
    friend class CFotaSrvSession;
    friend class CFotaUpdate;

public:

    // Member functions

    static CFotaServer* NewInstance(HbMainWindow& mainwindow);

    /**
     * Destructor.
     */
    virtual ~CFotaServer();

public:

    // All functions called from the session
    /**
     * Start download
     *
     * @since   S60   v3.1
     * @param   aParams     Package state params to be save in db
     * @param   aPkgURL     URL
     * @return  none
     */
    void DownloadL(TDownloadIPCParams aParams, const TDesC8& aPkgURL,
            TFotaClient aRequester, TBool aSilent, TBool aUpdateLtr);

    /**
     * Pauses ongoing download
     *
     * @since   S60   v3.1
     * @param   none
     * @return  none, can leave with system wide errors
     */
    void PauseDownloadL();

    /**
     * Start update
     *
     * @since   S60   v3.1
     * @param   aClient - the client which invoked the update
     * @return  none, can leave with system wide errors
     */
    void TryUpdateL(TFotaClient aClient);

    /**
     * Update, triggered by scheduler
     *
     * @since   S60   v3.1
     * @param   aUpdate - Update details
     * @param   aClient - the client which invoked the update
     * @return  none, can leave with system wide errors
     */
    void ScheduledUpdateL(TFotaScheduledUpdate aUpdate, TFotaClient aClient);

    /**
     * Delete swupd package
     *
     * @since   S60   v3.1
     * @param   aPkgId - Package id
     * @return  none, can leave with system wide errors
     */
    void DeletePackageL(const TInt aPkgId);

    /**
     * Get state of a swupd package
     *
     * @since   S60   v3.1
     * @param   aPkgId - Package id
     * @return  Package state, can leave with system wide errors
     */
    TPackageState GetStateL(const TInt aPkgId);

    /**
     * Tries to resume the download.
     *
     * @since   S60   v5.2
     * @param   aClient the client which triggers resume
     * @param   aSilentDl	Whether to query user for resume?
     * @return  None, can leave with system wide errors
     */
    void TryResumeDownloadL(TFotaClient aClient, TBool aSilentDl);

    /**
     * Get ids of present swupd packages
     *
     * @since   S60   v3.1
     * @param   aPackageIdList  On return, ids will be here
     * @return  None, can leave with system wide errors
     */
    void GetUpdatePackageIdsL(TDes16& aPackageIdList);

    /**
     * Get time of last update
     *
     * @since   S60   v3.1
     * @param   aTime       On return, time of last update
     * @return  None, can leave with system wide errors
     */
    void GetUpdateTimeStampL(TDes16& aTime);

    void GetCurrentFwDetailsL(TDes8& aName, TDes8& aVersion, TInt& aSize);

    /**
     * Do cleanup for package
     *
     * @since   S60   v3.1
     * @param   aPackageID Packageid
     * @return  None
     */
    void ResetFotaStateL(const TInt aPackageID);

    /**
     * Set download to be finalized. 
     *
     * @since   S60   v3.1
     * @param   aDLState    Final state of the download
     * @return  None, can leave with system wide errors
     */
    void FinalizeDownloadL();

    /**
     * Callback function to notify the network status
     *
     * @since S60	v3.2
     * @param Status of network connection
     * @return None
     */
    void ReportNetworkStatus(TBool status);

    //All user interface functions

    /**
     * Function to start the full screen dialog
     *
     * @since SF4
     * @param aSize - the full size of the download
     * @param aVersion - the version of the firmware upate
     * @param aName - the name of the firmware update
     * @return None
     */
    void StartDownloadDialog(const QString &aName, const QString &aVersion,
            const TInt &Size);

    /**
     * Function to update the full screen dialog with download progress
     *
     * @since SF4
     * @param aProgress - the download progress in percentage
     * @return None
     */
    void UpdateDownloadDialog(TInt aProgress);

    /**
     * Function to show any dialog within fota server 
     *
     * @since SF4
     * @param dialogid -the dialog identifier
     * @return None
     */
    void ShowDialogL(TFwUpdNoteTypes aDialogid);

    /**
     * Function that handles all the user responses on full screen dialog
     *
     * @since SF4
     * @param aResponse - key response
     * @return None
     */
    void HandleFullScreenDialogResponse(TInt aResponse);

    /**
     * Function called from the fota update when battery condition changes 
     *
     * @since SF4
     * @param aStatus - status of the battery, true if low, false if acceptable
     * @return None
     */
    void UpdateBatteryLowInfo(TBool aStatus);

    /**
     * Returns the full screen dialog pointer
     *
     * @since SF4
     * @param none
     * @return pointer to full screen dialog instance
     */
    FotaFullscreenDialog* FullScreenDialog();

    /**
     * Sets the phone's startup reason which will used by Fota Startup Pluggin to any decision.
     *
     * @since   S60   v3.2
     * @param   aReason     Startup reason, either download interrupted
     * @return  None
     */
    void SetStartupReason(TInt aReason);

    void ConstructApplicationUI(TBool aVal);
    void SetServerActive(TBool aValue);
    
    void FinalizeUpdate();

public:
    // member variables

    /**
     * The package state of the firmware update
     */

    TPackageState iPackageState;

    /**
     * Used for package state saving.
     */
    CFotaDB* iDatabase;
    
    
    TBuf8<KMaxFileName> iLastFwUrl;

public:
    // from  base classes

    /**
     * Handle syncml event
     * @param    aEvent
     * @param    aIdentifier
     * @param    aError
     * @param    aAdditionalData
     * @return   none
     */
    void OnSyncMLSessionEvent(TEvent aEvent, TInt aIdentifier, TInt aError,
            TInt aAdditionalData);

    /**
     * Function that is called when any user action happens on the dialogs or full screen dialog
     *
     * @since SF4
     * @param response - key press
     * @return None
     */
    void HandleDialogResponse(int response, TInt aDialogid);

    //All finalizing functions
    /**
     * Finalize download
     *
     * @since   S60   v3.1
     * @param   None
     * @return  None, can leave with system wide errors
     */
    void DoFinalizeDownloadL();

    /**
     * Read update result file and report result back to DM server
     *
     * @since   S60   v3.1
     * @param   None
     * @return  None, can leave with system wide errors
     */
    void DoExecuteResultFileL();

    /**
     * Close syncml session
     *
     * @since   S60 v3.1
     * @param none
     * @return  None, can leave with system wide errors
     */
    void DoCloseSMLSessionL();

    /**
     * When called, will try to shut the server when possible.
     * This will use the timer to shut down.
     *
     * @since   SF4
     * @param none
     * @return  None
     */
    void StopServerWhenPossible();

    void ServerCanShut(TBool aParam);
    
    TBool DecrementDownloadRestartCount();
	
    void SetVisible(TBool aVisible);

protected:
    // from base classes

    /**
     * Calls when a New session is getting created
     * @param aVersion
     * @param aMessage
     * @return CSession2*
     */
    CSession2* NewSessionL(const TVersion& aVersion,
            const RMessage2& aMessage) const;

    /**
     * DoConnect. 
     *
     * @since   S60   v3.1
     * @param   aMessage    IPC message
     * @return  none
     */
    void DoConnect(const RMessage2& aMessage);

    void DropSession();

private:

    /**
     * C++ default constructor.
     */
    CFotaServer(HbMainWindow& mainwindow);

    /**
     * ConstructL
     *
     * @since   S60   v3.1
     * @param   none
     * @return  none
     */
    virtual void ConstructL();

    /**
     * Latter part of constructing. It's ensured that parent fotaserver is 
     * constructed only once, and child fotaserver is not constructed at all.
     * Client fotaserver has a task of saving swupd as its only task. It should
     * not update swupd states.
     *
     * @since   S60   v3.1
     * @param   aMessage    Client process details
     * @return  none
     */
    void ClientAwareConstructL(const RMessage2 &aMessage);

    /**
     * Resets the fota state
     * @since S60	v3.2
     * @param IPC params
     * @return None
     */

    void ResetFotaStateToFailL();

    /**
     * Starts network monitoring operaiton for defined interval and retries
     *
     * @since S60	v3.2
     * @param None
     * @return None
     */
    void StartNetworkMonitorL();

    /**
     * Creates the disk reservation as set in the configuration
     *
     * @since SF4
     * @param 
     * @return None
     */
    void CreateDiskReservation();

    /**
     * Deletes the disk reservation in the given absolute path
     *
     * @since SF4
     * @param 
     * @return None
     */
    void DeleteDiskReservation(TDesC& path);

    /**
     * Sets the appropriate access point id for the download
     *
     * @since SF4
     * @param none
     * @return None, can leave with system wide errors.
     */
    void SetIapToUseL();

    /**
     * Invoke Fota Monitory Service when the download gets suspended
     *
     * @since   S60   v3.2
     * @return  None
     */
    void InvokeFmsL();

    /**
     * Cancels any outstanding request for resume notification
     *
     * @since   S60   v3.2
     * @return  None
     */
    void CancelFmsL();

    /**
     * Check s/w version with the current version
     *
     * @since   S60   v3.2
     * @param   none
     * @return  Bool value(matches or not)
     */
    TBool CheckSWVersionL();

    /**
     * Create DM session to DM server (identified by profileid)
     *
     * @since   S60   v3.1
     * @param   aProfileId  DM profile id
     * @param   aIapid      IAP id. If not kerrnone, will be used instead
     *                      of profiles' IAP id.
     * @return  none
     */
    void CreateDeviceManagementSessionL(TPackageState& aState);

    /**
     * Get the software version of the device
     * @since S60   v3.2
     * @param s/w version 
     * @return error
     */
    TInt GetSoftwareVersion(TDes& aVersion);

    /**
     * Checks for IAP Id exists or not in commsdb
     * IAPId used for resuming the paused download
     * @since S60   v5.0
     * @param IapId
     * @return ETrue/EFalse
     */
    TBool CheckIapExistsL(TUint32 aIapId);

    /**
     * Checks if the drive is busy due to encryption
     *
     * @since SF4
     * @param None
     * @return true, if busy, else false
     */
    TBool IsDeviceDriveBusyL();

    /**
     * The downloader manager object
     *
     * @since SF4
     * @param 
     * @return None
     */
    DownloadManagerClient* DownloaderL();

    /**
     * Sets the update requester
     *
     * @since SF4
     * @param aRequester - the requester of firmware update
     * @return None
     */
    void SetUpdateRequester(TFotaClient aRequester);

    /**
     * Gets the update requester
     *
     * @since SF4
     * @param 
     * @return None
     */
    TFotaClient GetUpdateRequester();

    /**
     * Reports firmware update status to the requester
     *
     * @since SF4
     * @param astate - the package state that has the status
     * @return None
     */
    void ReportFwUpdateStatusL(TPackageState& aState);

    /**
     * Loops for the battery status whilst full screen dialog is open
     * 
     * @since SF4
     * @param aBatteryLevel - the battery level
     * @return None
     */
    void LoopBatteryCheckL(TBool aBatteryLevel);

    /**
     * Shows the full screen dialog
     *
     * @since SF4
     * @param aType - type of the dialog
     * @return None
     */
    void ShowFullScreenDialog(TInt aType);

    /**
     * Swaps the fota process from background to foreground
     *
     * @since SF4
     * @param aState - true will bring to foreground, false to background
     * @return None
     */
    //void swapProcess(TBool aState);

    void ResetCounters();

    TBool IsUserPostponeAllowed();

    void DecrementUserPostponeCount();
    
    void WakeupServer();

private:
    // Data

private:
    // member variables    
    /**
     * Is server initialized?
     */
    TBool iInitialized;

    /**
     * The download manager client
     */
    DownloadManagerClient* iDownloader;

    /**
     * Used for starting update of fw.
     */
    CFotaUpdate* iUpdater;

    /**
     * Used for deleting download.
     */
    CPeriodic* iDownloadFinalizer;

    /**
     * Used for deleting update.
     */
    CPeriodic* iUpdateFinalizer;

    /**
     * Used for reading result file.
     */
    CPeriodic* iTimedExecuteResultFile;

    /**
     * Used for closing syncml session
     */
    CPeriodic* iTimedSMLSessionClose;

    /**
     * File server session
     */
    RFs iFs;

    /**
     * Tries to periodically shut down fotaserver
     */
    CPeriodic* iAppShutter;

    /**
     * Syncmlsession for creating dmsession
     */
    RSyncMLSession iSyncMLSession;

    /**
     * Class that checks for network availability 
     * 
     */
    CFotaNetworkRegStatus* iMonitor;

    /**
     * How many times to try dm session
     */
    TInt iSyncMLAttempts;

    /**
     * sync job id
     */
    TSmlJobId iSyncJobId;

    /**
     * profile
     */
    TSmlProfileId iSyncProfile;

    /**
     * To tell whether Generic Alert sending is being retried or not. This is checked before shutting down fota server
     * 
     */
    TBool iRetryingGASend;

    /**
     * To tell whether network is available to send Generic Alert.
     * 
     */
    TBool iNetworkAvailable;

    /**
     * The Fota Monitory Client object
     */
    RFMSClient iFMSClient;

    /**
     * Holds the storage drive of the package.
     */
    TDriveNumber iStorageDrive;

    /**
     * The session count to fota server.
     */
    static TInt iSessionCount;

    /**
     * The full screen dialog 
     */
    FotaFullscreenDialog *iFullScreenDialog;

    /**
     * The notifier params to the dialogs
     */
    CHbSymbianVariantMap * iNotifParams;

    CFotaDownloadNotifHandler * iNotifier;
    /**
     * Can the server shut?
     */
    TBool iServerCanShut;

    /**
     * Is async operation requested?
     */
    TBool iAsyncOperation;

    TInt iDialogId;

    TBool iConstructed;
    
    HbMainWindow& iMainwindow;
    };

#endif 

// End of File
