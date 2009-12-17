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
#include <AknDoc.h>
#include <aknapp.h>
#include <AknServerApp.h>
#include <coemain.h>
#include <s32file.h>
#include <DocumentHandler.h>
#include <apmstd.h>
#include <e32property.h>
#include <SyncMLClient.h>
#include <SyncMLClientDM.h>
#include <hwrmpowerstatesdkpskeys.h>
#include <SyncMLNotifier.h>
#include <SyncMLNotifierParams.h>
#include <SyncMLObservers.h>
#include <fotastorage.h>
#include <starterclient.h>

#include "fotaserverPrivateCRKeys.h"

#include "FotaIPCTypes.h"
#include "FotaDB.h"
#include "fotaConst.h"
#include "fotadownload.h"
#include "fotaupdate.h" 
#include "fmsclient.h"	//The FMS Client header file


//Forward declarations
class CDevEncController;

enum TOmaDmLrgObjDl
{
    EOmaDmLrgObjDlFail = 1,
    EOmaDmLrgObjDlExec
};
_LIT (KSWversionFile, "swv.txt");
/** Timeout to let cfotadownloader callstack to empty*/
const TInt KDownloadFinalizerWaitTime(1000000);

/** Time interval between syncml session retry attempts*/
const TInt KSyncmlSessionRetryInterval(60000000);

/** shutdown timer */
const TTimeIntervalMicroSeconds32 KFotaTimeShutDown(10000000);

/** How many times generic alert sending should be tried in row. */
const TInt KSyncmlAttemptCount = 3;

/** This P&S Key is used to notify DM UI on any download event. key=0 for idle and key=1 for download */ 
const TUint32 KFotaDownloadState = 0x00000001;

/** If download is active this P&S key is set to ETrue, otherwise EFalse. 
 * The key is owned by omadmappui**/
const TUint32 KFotaDownloadActive = 0x0000008;

/** The P&S key to say DM UI to perform refresh of UI elements */
const TUint32 KFotaDMRefresh = 0x00000009;

enum {
	ENo=0,
	EYes
};

//Constants used when update happen NSC/DM 
enum {
		EFotaUpdateNSC=1,
		EFotaUpdateNSCBg,
		EFotaUpdateDM
};

_LIT_SECURITY_POLICY_C1( KReadPolicy, ECapabilityReadDeviceData );
_LIT_SECURITY_POLICY_C1( KWritePolicy, ECapabilityWriteDeviceData );

//Constants for Device Encryption operation
enum TDEOperations
    {
    EIdle = 0,
    EEncryption,
    EDecryption,
    EDecryptionPending
    };
// FORWARD CLASS DECLARATION

class CFotaServer;
class CFotaDownload;
class CFotaUpdate;
class CFotaSrvApp;
class CFotaNetworkRegStatus;
/**
* Fota server. Handles client requests. Starts download and/or update.
 *
 *  This class is 
 *
 *  @lib    fotaserver
 *  @since  S60 v3.1
 */
NONSHARABLE_CLASS( CFotaServer ) : public CAknAppServer, MSyncMLEventObserver
	{
    friend class  CFotaSrvSession;
    friend class  CFotaDownload;
    friend class  CFotaUpdate;

public:

    /**
    * C++ default constructor.
    */
	CFotaServer();
    
    /**
    * Destructor.
    */		
	virtual ~CFotaServer();

public: // New functions


    /**
     * Create database
     *
     * @since   S60   v3.1
     * @param   aLargObj    Is it largeobject download (or dlmgr download)
     * @param   aPkgId      Packageid of the download
     * @return  none
     */
    void UpdatePackageDownloadCompleteL( TBool aLargObj,const TInt aPkgId);
 
 
    /**
     * Check s/w version with the current version
     *
     * @since   S60   v3.2
     * @param   none
     * @return  Bool value(matches or not)
     */
     TBool CheckSWVersionL();
     
    /**
     * Start download
     *
     * @since   S60   v3.1
     * @param   aParams     Package state params to be save in db
     * @param   aPkgURL     URL
     * @return  none
     */
    void DownloadL(TDownloadIPCParams aParams,const TDesC8& aPkgURL);


    /**
     * Start download which finishes with update
     *
     * @since   S60   v3.1
     * @param   aParams     Package state params to be save in db
     * @param   aPkgURL     URL
     * @return  none
     */
    void DownloadAndUpdateL(TDownloadIPCParams aParams
            ,const TDesC8& aPkgURL);



    /**
     * Start update
     *
     * @since   S60   v3.1
     * @param   aParams     Package state params to be save in db
     * @return  none
     */    
    void UpdateL( const TDownloadIPCParams& aParams );



    /**
     * Update, triggered by scheduler
     *
     * @since   S60   v3.1
     * @param   aUpdate     Update details
     * @return  none
     */ 
    void ScheduledUpdateL( TFotaScheduledUpdate aUpdate );

    /**
     * Delete swupd package
     *
     * @since   S60   v3.1
     * @param   aPkgId  Package id
     * @return  none
     */    
    void DeletePackageL ( const TInt aPkgId);



    /**
     * Get state of a swupd package
     *
     * @since   S60   v3.1
     * @param   aPkgId  Package id
     * @return  Package state
     */       
    TPackageState GetStateL( const TInt aPkgId);


    /**
     * Open a store to save swupd into
     *
     * @since   S60   v3.1
     * @param   aMessage    containing client side chunk handle  
     * @return  None
     */       
    void OpenUpdatePackageStoreL ( const RMessage2& aMessage );

    /**
     * Gets the downloaded and complete size of the update package
     *
     * @since   S60   v3.2
     * @param   aPkgId    Package id
     * @param	aDownloadedSize		size of download package in bytes
     * @param	aTotalSize	size of the complete package in bytes
     * @return  None
     */       
    void GetDownloadUpdatePackageSizeL (const TInt aPkgId, TInt& aDownloadedSize, TInt& aTotalSize);

    /**
     * Tries to resume the download.
     *
     * @since   S60   v3.2
     * @param   aQueryUser	Whether to query user for resume?
     * @return  None
     */       
	void TryResumeDownloadL(TBool aQueryUser);

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
     * Sets the phone's startup reason. This is used by Fota Startup Pluggin to any decision.
     *
     * @since   S60   v3.2
     * @param   aReason		Startup reason, either download interrupted
     * @return  None
     */       
	void SetStartupReason(TInt aReason);
	
	/**
	 * Shutdown DM UI application
	 * @since	S60	 v3.2
	 * @return 	None
	 */
	void ShutApp();
	
	/**
	 * Stops ongoing download, if any.
	 * @since	S60	 v3.2
	 * @param	aReason		Reason for stopping the download.
	 * @return 	None
	 */
	void StopDownload(TInt aReason);
	
    /**
     * Read data from chunk
     *
     * @since   S60   v3.1
     * @param   aSize   Size of data
     * @param   aError error caught while writing data into FS
     * @return  none
     */
    void ReadChunkL ( const TInt aSize ,TInt aError );


    /**
     * Check if swupd package fits into store
     *
     * @since   S60   v3.1
     * @param   aSize   Size of swupd
     * @return  Does it fit?
     */
    TBool IsPackageStoreSizeAvailableL ( const TInt aSize );


    /**
     * Get ids of present swupd packages
     *
     * @since   S60   v3.1
     * @param   aPackageIdList  On return, ids will be here
     * @return  None
     */
    void  GetUpdatePackageIdsL(TDes16& aPackageIdList);


    /**
     * Get time of last update
     *
     * @since   S60   v3.1
     * @param   aTime       On return, time of last update
     * @return  None
     */
    void GetUpdateTimeStampL (TDes16& aTime) ; 


    /**
     * Set download to be finalized. 
     *
     * @since   S60   v3.1
     * @param   aDLState    Final state of the download
     * @return  None
     */
    void  FinalizeDownloadL( const TPackageState& aDLState );


    /**
     * Set update to be finalized. 
     *
     * @since   S60   v3.1
     * @param   None
     * @return  None
     */
    void  FinalizeUpdateL();

    /**
     * Monitors for the battery in case of USB charger
     *
     * @since   S60   v3.2.3
     * @param   aLevel - level of the battery charge to monitor
     * @return  None
     */
    void MonitorBattery(TInt aLevel);

    /**
     * Finalize download
     *
     * @since   S60   v3.1
     * @param   None
     * @return  None
     */
    void  DoFinalizeDownloadL();


    /**
     * Finalize Update
     *
     * @since   S60   v3.1
     * @param   None
     * @return  None
     */
    void  DoFinalizeUpdateL();


    /**
     * Read update result file and report result back to DM server
     *
     * @since   S60   v3.1
     * @param   None
     * @return  None
     */
    void  DoExecuteResultFileL();
    
    /**
     * Deletes the update result file
     *
     * @since   S60   v3.2
     * @param   None
     * @return  None
     */
    void  DoDeleteUpdateResultFileL();

    /**
    * Close syncml session
    *
    * @since   S60 v3.1
    * @param none
    * @return none
    */
    void DoCloseSMLSessionL();

    /**
     * Shutdown fotaserver, if there's no operations in progress (
     * download,update)
     *
     * @since   S60   v3.1
     * @param   None
     * @return  Error
     */
    TInt TryToShutDownFotaServer();


    /**
     * Get reference to storageplugin. Load it, if not existing
     *
     * @since   S60   v3.1
     * @param   None
     * @return  Reference to storage
     */
    CFotaStorage*   StoragePluginL();

    
    /**
     * Do cleanup for package
     *
     * @since   S60   v3.1
     * @param   aPackageID Packageid
     * @return  None
     */
    void GenericAlertSentL( const TInt aPackageID );


    /**
     * Create DM session to DM server (identified by profileid)
     *
     * @since   S60   v3.1
     * @param   aProfileId  DM profile id
     * @param   aIapid		IAP id. If not kerrnone, will be used instead
     *						of profiles' IAP id.
     * @return  none
     */
    void CreateDeviceManagementSessionL( TPackageState& aState );
    
    
    /**
     * Get profiles' data.
     *
     * @since   S60   v3.1
     * @param   aSyncml     syncml session. must be open.
     * @param   aProf 	    profile id.
     * @param   aIapid      iapid 
     * @param   aSrvAA      server alerted action
     * @param   aUIJob      is current dm job started from ui
     * @param	aSanVal		the SAN bit set by server for the DM Session
     * @return  none
     */    
    void GetProfileDataL( RSyncMLSession* aSyncml,const TSmlProfileId aProf
        , TInt& aIapId, TSmlServerAlertedAction& aSrvAA,TBool& aUIJob, TInt& aSanVal);

	/**
	 * Gets the EikonEnv object
	 *
	 * @since S60	v3.1
	 * @param None
	 * @return CEikonEnv
	 */ 
	CEikonEnv* GetEikEnv();


	/**
	 * Callback function to notify the network status
	 *
	 * @since S60	v3.2
	 * @param Status of network connection
	 * @return None
	 */ 
	void ReportNetworkStatus(TBool status);



  /**
	 * Get the software version of the device
	 * @since S60	v3.2
	 * @param s/w version 
	 * @return error
	 */
	TInt GetSoftwareVersion(TDes& aVersion);
	
	/**
	 * Incase of large object download failure
	 * Updates the fota database state & reason information
	 * @since S60	v3.2
	 * @param None
	 * @return None
	 */
	void UpdateDBdataL();

    /**
     * stores the client id who triggered the update
     * @since S60   v3.2
     * @param aClient, client Id
     * @return None
     */ 	
	void SetInstallUpdateClientL(TInt aClient);
	
	/**
	 * returns the client id,who triggered the update
	 * @since S60   v3.2
	 * @param None
	 * @return TInt
	 */ 
	TInt GetInstallUpdateClientL();	
	
	
	/**
	 * Checks for IAP Id exists or not in commsdb
	 * IAPId used for resuming the paused download
	 * @since S60   v5.0
	 * @param IapId
	 * @return ETrue/EFalse
	 */
	TBool CheckIapExistsL(TUint32 aIapId);
	
    /**
     * This method is called to check if dencryption needs to be performed.
     */
	TBool NeedToDecryptL(const TInt &aPkgId, TDriveNumber &aDrive);
	
    /**
     * This method is called to start the decryption operation.
     */
	void DoStartDecryptionL();
	
    /**
     * This method is called once the decryption operation is complete.
     */
    void HandleDecryptionCompleteL(TInt aResult, TInt aValue = 0);
    
    /**
     * This method is called to check if encryption needs to be performed.
     */
    TBool NeedToEncryptL(TDriveNumber &aDrive);
    
    /**
     * This method is called to start the encryption operation.
     */
    void DoStartEncryptionL();
	
    /**
     * This method is called once the encryption operation is complete.
     */
	void HandleEncryptionCompleteL(TInt aResult, TInt aValue = 0);
	
    /**
     * Gets the device encryption operation
     */
	TInt GetDEOperation();
	
    
    /**
     * Is server initialized? Must be public for session to use.
     */
    TBool iInitialized;

    /**
    * Whether Fota server should close or not
    */
    TBool iNeedToClose;
    
    /**
     * handles notifier completion
     */
    CFotaDownloadNotifHandler*  iNotifHandler;

public: // from  base classes

    /**
     * Creator
     * Not own
     */
    CFotaSrvApp* iParentApp;

    /**
    * CreateServiceL. Creates session.
    * @param    aServiceType
    * @return   CApaAppServiceBase*
    */
    CApaAppServiceBase* CreateServiceL( TUid aServiceType ) const;
    /**
    * Handle syncml event
    * @param    aEvent
    * @param    aIdentifier
    * @param    aError
    * @param    aAdditionalData
    * @return   none
    */
    void OnSyncMLSessionEvent(TEvent aEvent, TInt aIdentifier, TInt aError, TInt aAdditionalData) ;

    /**
     * Reference to client side chunk.  Swupd comes this way.
     */
    RChunk iChunk;


    /**
     * Used for package state saving.
     */
    CFotaDB* iDatabase;


    /**
     * Used for downloading.
     */
    CFotaDownload* iDownloader;


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
    CPeriodic*          iUpdateFinalizer;


    /**
     * Used for reading result file.
     */
    CPeriodic*          iTimedExecuteResultFile;


    /**
     * Used for closing syncml session
     */
    CPeriodic* iTimedSMLSessionClose;

    /**
     * ConstructL
     *
     * @since   S60   v3.1
     * @param   aFixedServerName    Name of this app server 
     * @return  none
     */    
    virtual void ConstructL(const TDesC &aFixedServerName);

protected: // from base classes


    /**
     * DoConnect. 
     *
     * @since   S60   v3.1
     * @param   aMessage    IPC message
     * @return  none
     */
    void DoConnect(const RMessage2& aMessage);


    /**
     * When last client disconnects, this is called.
     *
     * @since   S60   v3.1
     * @param   none
     * @return  none
     */
    void HandleAllClientsClosed();

    
private: // new functions


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
    void ClientAwareConstructL( const RMessage2 &aMessage );


    /**
     * Loads storage plugin if not loaded.
     *
     * @since   S60   v3.1
     * @param   none
     * @return  none
     */
    void            LoadStoragePluginL ();


    /**
     * Unloads storage plugin.
     *
     * @since   S60   v3.1
     * @param   none
     * @return  none
     */
    void            UnLoadStoragePluginL ();



/**
	 * Resets the fota state
	 * @since S60	v3.2
	 * @param IPC params
	 * @return None
	 */
	 
	void ResetFotaStateL(const TDownloadIPCParams& aParams);
	
	/**
	 * Starts network monitoring operaiton for defined interval and retries
	 *
	 * @since S60	v3.2
	 * @param None
	 * @return None
	 */ 
	void StartNetworkMonitorL();


private: // Data


    /**
     * The state that iDownloader was in, when it called to be finalized.
     */
    TPackageState       iDLFinalizeState;

    /**
     * The state before sending generic alert.
     */
    TPackageState		iStoredState;


    /**
     * Storage class (plugin).
     */
    CFotaStorage*       iStorage;


    /**
     * Packageid of the swupd being stored. Used only by child fotaserver.
     */
    TInt                iStorageDownloadPackageId;


    /**
     * Destructor key of the storage plugin.
     */
    TUid                iStorageDtorKey;


    /**
     * Destructor key of the storage plugin.
     */
    RWriteStream*       iStorageStream;


    /**
     * File server session
     */
    RFs                 iFs;


    /**
     * Tries to periodically shut down fotaserver
     */
    CPeriodic*          iAppShutter;

    /**
     * Syncmlsession for creating dmsession
     */
    RSyncMLSession      iSyncMLSession;

    /**
     * Class that checks for network availability 
     * 
     */
    CFotaNetworkRegStatus* iMonitor;

    /**
     * How many times to try dm session
     */
    TInt    iSyncMLAttempts;

    /**
     * sync job id
     */
    TSmlJobId iSyncJobId;

    /**
     * profile
     */
    TSmlProfileId iSyncProfile;

    /**
     * Reference to eikonenv. Used by other classes
     * 
     */
    CEikonEnv*	iEikEnv;

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
    RFMSClient	iFMSClient;
    
    /**
     * For sending GA, this will set to fotastate's iapid
     */
    TInt iIapId;
    /**
     * Tells the session mode.
     */
    TInt iSessMode;
    
    /**
     * Tells about who started resume.
     */
    TInt iUserResume;
    
    /**
     * Tells which client triggered install update.
     */
    TInt iInstallupdClient;
    
    /**
     * The pointer to encryption controller
     */
    CDevEncController *iDEController;
    
    /**
     * Holds the storage drive of the package.
     */
    TDriveNumber iStorageDrive;
    
	};

#endif 

// End of File
