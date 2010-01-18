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
* Description:   Swupd downloader.
*
*/



#ifndef ___DOWNLOADER_H_
#define ___DOWNLOADER_H_

#include <downloadmgrclient.h>
#include <etelpckt.h> //To read GSM Error codes from symbian
#include <exterror.h> //To read GSM Error codes from symbian
#include <wlanerrorcodes.h> //To read WLAN Error codes from s60
#include <AknWaitDialog.h>
#include "FotaServer.h"
#include "FotaSrvDebug.h"
#include "FotaDLProgressDlg.h"

// FORWARD DECLARATIONS
class CFotaServer;
class CAknWaitDialog;
class CFotaDownloadNotifHandler;
class CFotaNetworkRegStatus; 	//To know the network connectivity status while resuming suspended download
// CONSTANTS
 // from coderror.h
#define KErrCodInvalidDescriptor -1000
#define KErrCodInsufficientSpace -1001
#define KErrCodWapConnectionDropped -20041

// If download times out, it must be tried again
#define KFotaDownloadTimeoutRestartCount 5

// For reading connecting resource from DM UI
_LIT( KSCPResourceFilename, "\\Resource\\Apps\\NsmlDMSync.rsc" );
_LIT( KDriveZ,"z:" );
_LIT( KDDDirPath, "c:\\system\\fota\\" );
_LIT( KDDFilename, "fotadownload.dd" );

//Constants when SAN Bits are set
enum {
		ESanNotSpecified=0,
		ESanSilent,
		ESanInformative,
		ESanInteractive
};

//Contants when SAN is Off. It refers to Accepted state in DM Profile Settings
enum {
	EAcceptedNo=0,
	EAcceptedYes
};

// Constants for KSilentOmaDlUIBehavior cenrep key
 enum {
 		EOmacdOffdlprgOff=0, //OMA CD View = Off, Download Progress Bar = Off
 		EOmacdOffdlprgOn,    //OMA CD View = Off, Download Progress Bar = On
 		EOmacdOndlprgOff,    //OMA CD View = On,  Download Progress Bar = Off
 		EOmacdOndlprgOn,     //OMA CD View = On,  Download Progress Bar = On 
 		EChooseFromSan		 //Choose behavior from SAN bits set during DM Session
 };
/**
 *  Downloader 
 *
 *  Starts download and observes it.
 *
 *  @lib    fotaserver
 *  @since  S60 v3.1
 */
NONSHARABLE_CLASS (CFotaDownload ) 
        : public CActive 
        , MHttpDownloadMgrObserver
        , MDLProgressDlgObserver
    {
    friend class CFotaDownloadNotifHandler;


	enum TFotaDLProgress
		{
		EStarted,
		EDescriptorDownloaded,
		ESizeChecked
		};
		
			
public: // new functions

    CFotaDownload ();

    virtual ~CFotaDownload ();

    static CFotaDownload* NewL (CFotaServer* aServer);

public: // Functions from base classes:


    /**
     * Schedule update to be carried out after download
     *
     * @since   S60   v3.1
     * @param   aButtonId Button pressed
     * @return  None
     */
    TBool HandleDLProgressDialogExitL( TInt aButtonId );

protected:  // Functions from base classes:
    
    /**
     * Handle user action
     *
     * @since   S60   v3.1
     * @param   None
     * @return  None
     */
    virtual void RunL();


     /**
     * Handle leaving Runl
     *
     * @since   S60   v3.1
     * @param   aError  error code
     * @return  Error code
     */
    virtual TInt RunError(TInt aError);


     /**
     * Handle cancel of active object
     *
     * @since   S60   v3.1
     * @param   none
     * @return  none
     */
    void DoCancel();

public:


    /**
     * Handle Download manager event
     *
     * @since   S60   v3.1
     * @param   aDownload       download 
     * @param   aEvent          event
     * @return  none
     */
    virtual void HandleDMgrEventL( RHttpDownload& aDownload
                        , THttpDownloadEvent aEvent );


    /**
     * Handle Download manager event
     *
     * @since   S60   v3.1
     * @param   aDownload       download 
     * @param   aEvent          event
     * @return  none
     */
    void DoHandleDMgrEventL( RHttpDownload* aDownload
                        ,THttpDownloadEvent aEvent );


    /**
     * Start download of swupd
     *
     * @since   S60   v3.1
     * @param   aParams     download parameters
     * @param   aPkgURL     Url of swupd
     * @param   aUpdateAfterDownload     Is update needed
     * @param   aIapid      Iap
     * @param   aRestartDownload Is this a retry download (counter)? 
     *                            If yes, don't show descriptor.
     * @return  none
     */
    void DownloadL(TDownloadIPCParams aParams,const TDesC8& aPkgURL
        ,TBool aUpdateAfterDownload,const TInt aIapid=KErrNotFound
        ,const TInt aRestartDownload=0 );


    /**
     * Cancel
     *
     * @since   S60   v3.1
     * @param   aCancelProgressBar  Cancel progress bar also?
     * @return  none
     */
    void CancelDownload( const TBool aCancelProgressBar );

    /**
     * Tries to resume any suspended fota download by quering the user. When the DM session is
     * Silent, this method directly starts resuming of download by calling ResumeDownloadL.
     *
     * @since   S60   v3.2
     * @param   aUserInitiated  Is user initiated resume?
     * @return  none
     */
    void TryResumeDownloadL(TBool aUserInitiated);
    
    /**
     * Resumes any suspended fota download. Firstly, it retrieves the RHttpDownload download object from Download Manager
     * and then performs the following - 
     * - sets the IAP ID to use
     * - checks whether the connection is possible with that IAP ID
     * - sets download ui behavior
     *
     * @since   S60   v3.2
     * @return  none
     */
    void ResumeDownloadL();
    /**
     * Finds whether the download is resumable or not.
     *
     * @since   S60   v3.2
     * @return  ETrue if download is resumable, else EFalse.
     */
    TBool IsDownloadResumable();
    
    /**
     * Finds whether Fota Monitory Service is enabled or not.
     *
     * @since   S60   v3.2
     * @return  ETrue if FMS is enabled, else EFalse.
     */
	TBool IsFMSEnabled();
	
    /**
     * Sets the download activity state to the P&S Key used to sync with DM UI.
     *
     * @since   S60   v3.2
     * @param	aValue	ETrue when download is active, else EFalse.
     * @return  None
     */
	void SetDownloadActive(TBool aValue);
	
    /**
     * Returns the download activity state
     *
     * @since   S60   v3.2
     * @return	aValue	ETrue when download is active, else EFalse.
     */

	TBool IsDownloadActive();

    /**
     * Starts to suspend the ongoing download
     *
     * @since   S60   v3.2
     * @param	aReason		Reason to suspend the download. Value is one of RFotaEngineSession::TResult.
     * @param	aSilent		Is suspend operation to be silent?
     * @return  None
     */
	void RunDownloadSuspendL(const TInt aReason, TBool aSilent=EFalse);
		
    /**
     * Starts to cancel the ongoing download
     *
     * @since   S60   v3.2
     * @param	aReason		Reason to cancel the download. Value is one of RFotaEngineSession::TResult.
     * @param	aSilent		Is cancel operation to be silent?
     * @return  None
     */
	void RunDownloadCancelL(const TInt aReason, TBool aSilent=EFalse);
	
    /**
     * Should update be started after download completes
     */
    TBool iUpdateAfterDownload;

    /**
     * Download is to be restarted. (counter)
     */
    TInt iRestartCounter;
    
    /**
     * Download state
     */
    TPackageState iDLState;


    /**
    * Download URL 
    */
    HBufC8* iUrl;


  

private:


    /**
     * Launch notifier. Also used to end download silently.
     *
     * @since   S60   v3.1
     * @param   aNotetype    Notetype
     * @param   aIntParam    int param
     * @param   aMemoryneeded How much disk space needed for download
     * @param   aFinishOnly  finish only, do not launch notifier
     * @return  none
     */
    
    void LaunchNotifierL( const TSyncMLFwUpdNoteTypes aNotetype
                            ,const TInt aIntParam
                            ,const TInt aMemoryNeeded
                            ,TBool aFinishOnly=EFalse );

    /**
     * Reads the download variations from the Cenrep keys into instance variables.
     * They include - Fota Suspend & Resume feature state (on/off) &
     * Fota Monitory Service feature state (on/off)
     * 
     * @since   S60   v3.2
     * @return  none
     */
	void CheckDownloadVariations();

    /**
     * Retrieves the RHttpDownload object from Download Manager for the suspended download.
     * Since concurrent downloads are not possible, there will be always zero or one suspended download.
     * 
     * @since   S60   v3.2
     * @return  RHttpDownload object. If NULL, it means that there is no suspended download or the retrieval has fails.
     */
	RHttpDownload* RetrieveDownloadL();
	
    /**
     * Sets the IAP Id to use during download. The same API is used for fresh or resuming download.
     * During resuming of download, the selection of IAP logic varies as below:
     * 1. If IAP is always ask in DM profile, uses the earlier set during FMS trigger
	 * 2. If IAP is always ask in DM profile, query user for IAP during user trigger
	 * 3. If IAP has changed in DM profile after suspend, use the newer one during resume
     * 
     * @since   S60   v3.2
     * @param	aParams		contains the download operation details
     * @param	aIapid		Out param, the IAP Id is set here.
     * @return  None
     */
	void SetIapToUseL(TPackageState aParams,const TInt aIapid);
	
    /**
     * Sets the Download UI behavior for fresh or resuming download.
     * 
     * @since   S60   v3.2
     * @param	aFreshDL	ETrue if the download is fresh, else EFalse.
     * @return  None
     */
	void SetDownloadUiBehavior(TBool aFreshDL);
	
    /**
     * Creates a fresh download
     *
     * @since   S60   v3.1
     * @param   
     * @return  none
     */
    void CreateDownloadL();
	  /**
     * SetDLResultdlErrorId Set the appropriate error codes.
     *
     * @since   S60   v5.0
     * @param   adlErrorId  error id from DlMgr
     * @param   asysErrorId error id from system
     * @return  none
     */   
    void SetDLResultdlErrorId( THttpDownloadMgrError adlErrorId, TInt32 asysErrorId);
    
    /**
     * ReSetDownloadL Reset the Fota state to Download failed
     *
     * @since   S60   v5.0
     * @param   atoresetdl Bool value if ETrue the set the state to Download Failed
     * @return  none
     */   
    void ReSetDownloadL(TBool atoresetdl);
    
    /**
     * DetermineUISANOFF Determine UI behavior when SAN is OFF
     *
     * @since   S60   v5.0
     * @param   aBehavior behavior depending upon whether SAN is supported or not
     * @return  none
     */    
    void DetermineUISANOFF(TInt aBehavior);
    
     /**
     * DetermineUISANOFF Determine UI behavior when SAN is OFF
     *
     * @since   S60   v5.0
     * @param   aBehavior behavior depending upon whether SAN is supported or not
     * @return  none
     */
    void DetermineUISANON(TInt aBehavior);
    /**
     * Reference to fotaserver.
     * Not own
     */
    CFotaServer*            iFotaServer;

    /**
     * Reference to download mgr.
     */
    RHttpDownloadMgr        iDownloadMgr;

    /**
     * The download object for the current download.
     */
    RHttpDownload* iDownload;
    
    /**
     * The downloaded content size in bytes.
     */
    TInt	iDownloadedContentSize;

    /**
     * Download progress dialog.
     */
    CFotaDLProgressDlg*     iDLProgressDlg;

    /**
     * Content size received in previous event
     */
    TInt                    iPreviousContentSize;

    /**
     * handles notifier completion
     */
	CFotaDownloadNotifHandler*  iNotifHandler;

    
    /**
	* Indicates that this download should be reset. (could be inclded in progress)
    */
    TBool iDLNeedsReset;
    
    /**
	* Progress state.
    */
    TFotaDLProgress iDLProgress;
    
    /**
     * Showing OMA Content Descriptor
     */
    TBool iShowOMACDUI;

    /**
     * Showing Progress bar
     */
    TBool iShowDLPrgUI;
    
    /**
     * Is download resumable?
     */
	TBool iDownloadResumable;
	
	/**
	 * Is Fota Monitory service enabled?
	 */
	TBool iFMSEnabled;
	
	/**
	 * Is Fota download active?
	 */
	TBool iDownloadActive;
    /**
     * Iap id to be used in download
     */
	TInt	iIapid;
	
	/**
	 * Is download fresh?
	 */
	TBool iFreshDL;
	
	/**
	 * Is user initiated resume?
	 */
	TBool iUserInitiatedResume;
	
	/**
	 * Is fota operation started by user?
	 */
	TBool iIsjobStartedFromUI;
	
	/**
	 * Value of SAN bit set by the remote Server
	 */
	TInt iSanValue;
	
	/**
	 * Is suspend/cancel of download operation to be silent?
	 */
	TBool 	iSilentOpn;
	
	/**
	 * to check whether downlaod is finalized or not 
	 */
	 TBool iDownloadFinalized;
	 /**
     * Class that checks for network availability 
     */
    CFotaNetworkRegStatus* iMonitor;

    };





NONSHARABLE_CLASS ( CFotaDownloadNotifHandler ) 
        : public CActive 
    {
public: // new functions
    static CFotaDownloadNotifHandler* NewL (CFotaDownload* aDownload);
    
    CFotaDownloadNotifHandler();
    
    ~CFotaDownloadNotifHandler();
    /**
     * Handle user action
     *
     * @since   S60   v3.1
     * @param   None
     * @return  None
     */
    virtual void RunL();


     /**
     * Handle leaving Runl
     *
     * @since   S60   v3.1
     * @param   aError  error code
     * @return  Error code
     */
    virtual TInt RunError(TInt aError);


     /**
     * Handle cancel of active object
     *
     * @since   S60   v3.1
     * @param   none
     * @return  none
     */
    void DoCancel();

     /**
     * Shows notifier and/or ends download
     *
     * @since   S60   v3.1
     * @param   aNotetype	type of the note as defined in syncmlnotifier
     * @param   aIntParam  Generic parameter, defined in syncmlnotifier
     * @param   aMemoryNeeded	Amount of disk space needed. Only one notifier
	 *							type consumes this.
     * @param   aFinishOnly		Do not show notifier, finish download only
     * @return  none
     */
    void LaunchNotifierL( const TSyncMLFwUpdNoteTypes aNotetype
                            ,const TInt aIntParam
                            ,const TInt aMemoryNeeded
                            ,TBool aFinishOnly=EFalse );

     /**
     * Is any notifier displayed?
     *
     * @since   S60   v3.2
     * @param   none
     * @return  ETrue if yes, otherwise EFalse
     */
    TBool IsOpen();
private:

	CFotaDownload* iDownload;

    /**
     * Notifier for showing notes
     */
    RNotifier               iNotifier;

    /**
     * Notifier input parameters
     */
    TSyncMLFwUpdNotifParams     iNotifParams;

    /**
     * Notifier response 
     */
    TSyncMLFwUpdNotifRetValPckg  iDummyResponsePckg;
    

    };



#endif