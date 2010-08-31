/*
* Copyright (c) 2005-2006 Nokia Corporation and/or its subsidiary(-ies). 
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
 * Description:  Fotaengine api
*
*/

#ifndef __FOTAENGINE_H__
#define __FOTAENGINE_H__

// INCLUDES
#include <e32std.h>
#include <e32def.h>
#include <SyncMLDef.h>

#include "fotaConst.h"
#include <s32strm.h>

// CONSTANTS
/**  FOTA Server commands */
enum TFotaIPCCmds
    {
    EFotaDownload,              // The download command
    EFotaDownloadAndUpdate,     // The download and update command
    EFotaUpdate,                // The update command
    EPauseDownload,             // The download pause command
    EFotaTryResumeDownload,     // The download resume command
    EGetState,                  // The get state command
    EGetResult,                 // The get result command
    EDeletePackage,             // The package delete command
    EGetUpdateTimestamp,        // The get command for updated timestamp    
    EGetUpdatePackageIds,       // The get command for all package ids
    EGenericAlertSentForPackage,    // The generic alert sent notification command
    EScheduledUpdate,               // The schedule update command
    EGetCurrFwDetails               // The get command for reading current firmware package details
    };

/** Fota start up action states */
enum TFotaUpdateStates
    {
    EFotaDefault,               // No action        
    EFotaPendingGenAlert,       // Send pending generic alert
    EFotaDownloadInterrupted,   // Try to resume download
    EFotaUpdateInterrupted      // Try to resume update
    };

class TFotaScheduledUpdate;

// CLASS DECLARATION

/**
 *	A client handle to a FOTA engine session.
 *   @lib    fotaengine.lib
 *   @since   SF4
 */
//class RFotaEngineSession : public RAknAppServiceBase // 10.1 changes
class RFotaEngineSession : public RSessionBase
    {

public:
    // enums

    /** 
     * An enumeration of the firmware update progress state codes as specified
     * in FUMO spec.
     */
    enum TState
        {
        /** No firmware update has been started */
        EIdle = 10,
        /** Client has sent a client initiated request */
        EClientRequest = 5,
        /** There is no data available and download is about to start */
        EStartingDownload = 15,
        /** Download failed and there is no data received */
        EDownloadFailed = 20,
        /** Download is progressing without resume support. */
        EDownloadProgressing = 30,
        /** Have data and download has been completed successfully */
        EDownloadComplete = 40,
        /** Have data and about to start update */
        EStartingUpdate = 50,
        /** Denotes that the update is currently running, but has not yet 
         completed */
        EUpdateProgressing = 60,
        /** Have data but update failed */
        EUpdateFailed = 70,
        /** Update failed and data deleted or removed */
        EUpdateFailedNoData = 80,
        /** Update complete and data still available */
        EUpdateSuccessful = 90,
        /** Data deleted or removed after a successful update */
        EUpdateSuccessfulNoData = 100,
        };

    /** 
     * An enumeration of the firmware update result codes as specified
     * in the OMA Firmware Update Management Oobject specification. 
     */
    enum TResult
        {
        /** Requested operation is successful */
        EResSuccessful = 200,
        /** Requested operation is cancelled by user */
        EResUserCancelled = 401,
        /** Firmware update is corrupt */
        EResCorruptedFWUPD = 402,
        /** Firmware update mismatches with the device */
        EResPackageMismatch = 403,
        /** Not used */
        EResFailedSignatureAuthentication = 404,
        /** Undefined error occured during the operation */
        EResUndefinedError = 409,
        /** Update has failed */
        EResUpdateFailed = 410,
        /** Download failed due to malformed or bad url */
        EResMalformedOrBadURL = 411,
        /** The OMA DL server is unavailable */
        EResAlternateDLServerUnavailable = 412,
        /** Download paused due to out of memory */
        EResDLFailDueToDeviceOOM = 501,
        /** Download paused due to network error */
        EResDLFailDueToNWIssues = 503,

        //Vendor specific errors
        /** Download failed due to content type mismatch */
        EResContentMisMatch = 600,
        /** Download failed due to invalid OMA DL1.0 descriptor */
        EResInvalidDownloadDescriptor = 601,
        /** Update suspended due to low battery */
        EResLowBattery = 602
        };

public:
    // Constructors 

    IMPORT_C RFotaEngineSession();

public:
    // new functions
    /**
     * Opens session to Fota Engine
     *         
     * @since  SF4
     * @param      None
     * @return     None
     */
    IMPORT_C void OpenL();

    /**
     * Closes session to Fota Engine
     * 
     * @since   SF4
     * @param      None
     * @return     None
     */
    IMPORT_C void Close();

    /**
     * Called when DM server calls execute command to
     * Firmware update object URI ./FUMO/<x>/Download
     * Initiates a firmware download for the specified update package.
     * 
     * @since   SF4
     * @param aPkgId		Unique identifier of the update package.
     * 					Mapped to Mgmt URI ./FUMO/<x> in DM Framework
     * @param aPkgURL		Contains the URL where the firmware update package
     *                   or download 
     *					descriptor is located. This URL is used for 
     *                    alternative download
     *					mechanism such as Descriptor Based Download.
     *					Mgmt URI ./FUMO/<x>/Download/PkgURL
     * @param aProfileId	ProfileId of the DM server that send the execute 
     *                   command
     * @param aPkgName	Name associated with the firmware update package,
     *                   may be empty
     * 			        Mgmt URI ./FUMO/<x>/PkgName
     * @param aPkgVersion	Version information for the firmware update 
     *                       package,
     *                       may be empty.
     *					    Mgmt URI./FUMO/<x>/PkgVersion
     * @param aSilent   Decides whether the download has to be silent or not					    
     * @return			Immediate result of the command
     *                   KErrNotFound: url doesn't exist
     */
    IMPORT_C TInt Download(const TInt aPkgId, const TDesC8& aPkgURL,
            const TSmlProfileId aProfileId, const TDesC8& aPkgName,
            const TDesC8& aPkgVersion, TBool aSilentDownload = EFalse);

    /**
     * Called when DM server calls execute command to
     * Firmware update object URI ./FUMO/<x>/DownloadAndUpdate
     * Initiates a firmware download and an immediate update for the specified
     *                   update package.
     *
     * @since   SF4
     * @param aPkgId		Unique identifier of the update package.
     *				    Mapped to Mgmt URI ./FUMO/<x> in DM Framework
     * @param aPkgURL	Contains the URL where the firmware update package or
     *                   download descriptor is located. This URL is used for 
     *                   alternative download mechanism such as Descriptor 
     *                   Based Download.
     *                   Mgmt URI ./FUMO/<x>/DownloadAndUpdate/PkgURL
     * @param aProfileId	ProfileId of the DM server that send the execute 
     *                   command
     * @param aPkgName	Name associated with the firmware update package,
     *                   may be empty.
     *				    Mgmt URI ./FUMO/<x>/PkgName
     * @param aPkgVersion	Version information for the firmware update 
     *                       package, may be empty.
     *				        Mgmt URI./FUMO/<x>/PkgVersion
     * @param aSilent   Decides whether the download has to be silent or not
     * @return			Immediate result of the command
     */
    IMPORT_C TInt DownloadAndUpdate(const TInt aPkgId, const TDesC8& aPkgURL,
            const TSmlProfileId aProfileId, const TDesC8& aPkgName,
            const TDesC8& aPkgVersion, TBool aSilentDownload = EFalse);

    /**
     * Called when DM server calls execute command to
     * Firmware update object URI ./FUMO/<x>/Update
     * Initiates a firmware update for the specified update package.
     * Firmware Update Package should be already downloaded to the device 
     * either using DM Large Object or OMA OTA download mechanisms.
     *
     * @since   SF4
     * @param aPkgId		Unique identifier of the update package.
     * @param aProfileId	ProfileId of the DM server that send the execute 
     *                   command
     * @param aPkgName	Name associated with the firmware update package, may
     *                   be empty.
     *				    Mgmt URI ./FUMO/<x>/PkgName
     * @param aPkgVersion	Version information for the firmware update
     *                       package, may be empty.
     *				        Mgmt URI./FUMO/<x>/PkgVersion
     * @return			Immediate result of the command
     */
    IMPORT_C TInt Update(const TInt aPkgId, const TSmlProfileId aProfileId,
            const TDesC8& aPkgName, const TDesC8& aPkgVersion);

    /**
     * Requests to pause the ongoing download. 
     *
     * @since  Symbian 4
     * @param  None
     * @return   KErrNone if pause is successful, or any system-wide error.
     */
    IMPORT_C TInt PauseDownload();

    /**
     * Requests to resume the suspended download of the update package.   
     * Called by Fota Monitory Service. for ex, when network is available again.
     * @since   3.2.2
     * @param aSilent   Decides whether the download has to be silent or not
     *
     * @return			KErrNone when successful, else System wide errors
     *
     */

    IMPORT_C TInt TryResumeDownload(TBool aSilentDownload = EFalse);

    /**
     * Called when caller wants to enquire State of specified
     * firmware update. If the State is unknown to FOTA Engine, then it should
     * return EIdle.
     *
     * @since   SF4
     * @param aPkgId		Unique identifier of the update package.
     * @return			State reached by specified firmware update package 
     */
    IMPORT_C TState GetState(const TInt aPkgId);

    /**
     * Called when caller wants to enquire Final Result Code of specified
     * firmware update operation. If the update operation is not yet reached 
     * final stage, then -1 should be returned to the caller. Possible Final 
     * Result Codes are specified in OMA FUMO Specification. Note that Download
     * operation also ends to final result code e.g. 202 - Successful Download.
     *
     * @since   SF4
     * @param aPkgId		Unique identifier of the update package.
     * @return			Result code as specified in FUMO specification, -1 if 
     *                   not yet reached final result.
     */
    IMPORT_C TInt GetResult(const TInt aPkgId);

    /**
     * Called when DM server deletes firmware update management object
     * from device's management tree. I.e. Delete to mgmt URI ./FUMO/<x>.
     * If FOTA Engine has not yet deleted specified update package, then it 
     * should do so.	
     *
     * @since   SF4
     * @param aPkgId		Unique identifier of the update package.
     * @return			Result code
     */
    IMPORT_C TInt DeleteUpdatePackage(const TInt aPkgId);

    /**
     * Retrieves the timestamp of last succesful update.
     *
     * @since   SF4
     * @param aUpdate    On return, contains time of last succesfull update
     * @return			Error code. KErrUnknown if device has never been 
     *                   updated.
     */
    IMPORT_C TInt LastUpdate(TTime& aUpdate);

    /**
     * Tells device FW version
     *
     * @since   SF4
     * @param aSWVersion On return, contains FW version of device.
     * @return			Error code
     */
    IMPORT_C TInt CurrentVersion(TDes& aSWVersion);

    /**
     * Gets IDs of the present update packages.
     *
     * @since   SF4
     * @param aPackageIdList   On return, contains array of pkg ids
     * @return				  Error code
     */
    IMPORT_C TInt GetUpdatePackageIds(TDes16& aPackageIdList);

    /**
     * Version of the fota server
     *
     * @since   SF4
     * @return  version
     */
    IMPORT_C TVersion Version() const;

    /**
     * Tells fotaserver that generic alert for package is sent. 
     * When fotaengine session is closed, cleanup for package
     * is done.
     *
     * @since   SF4
     * @param    aPackageId    
     * @return   Error code
     */
    IMPORT_C void GenericAlertSentL(const TInt aPackageID);

    /**
     * Like Update, but called by scheduler mechanism. 
     * Needed package details (profile id etc are already known)
     *
     * @since   3.2
     * @param  aSchedule		Schedule data
     * @return	Error code
     */
    IMPORT_C TInt ScheduledUpdateL(TFotaScheduledUpdate aSchedule);

    /**
     * Gets the details for current/last firmware update. 
     *
     * @since   SF4
     * @param   aName - name of the firmware
     * @param   aVersion - version of the firmware
     * @param   aSize - size of the firmware in bytes
     * @return, on return all the params would be filled. Can leave with system wide error.
     */
    IMPORT_C    void GetCurrentFirmwareDetailsL(TDes8& aName, TDes8& aVersion,
            TInt& aSize);
			
	IMPORT_C TBool IsPackageStoreSizeAvailable(const TInt aSize);
    
    
    IMPORT_C TInt OpenUpdatePackageStore(const TInt aPkgId
                                                 ,RWriteStream*& aPkgStore);
    
    IMPORT_C TInt GetDownloadUpdatePackageSize(const TInt aPkgId, TInt& aDownloadedSize, TInt& aTotalSize);
    	
    IMPORT_C void UpdatePackageDownloadComplete(
                                                          const TInt aPkgId);

private:

    /** Returns the UID of the service that this session provides an 
     * interface for. Client side service implementations must implement this
     * function to return the UID for the service that they implement.
     *
     * @since  SF4
     * @param  None    
     * @return The UID of the service implemented by the derived class.
     */
    TUid ServiceUid() const;


private:

    /**
     * Starts fotaserver server application.
     *
     * @since SF4
     * @param aNameUid         Differentiator.
     * @param aAppServerUid    FotaServer app uid
     * @return	None
     **/
    void StartApplicationL(const TUid& aNameUid, const TUid& aAppServerUid);


    /**
     * Connects to the server
     *
     * @since SF4
     * @return  KErrNone if successful, otherwise system wide errors
     **/
    
    TBool connectToHbServer();

private: // Data


    /**
     * If not -1 , indicates that generic alert has been sent for this package
     */
    TInt iGenericAlertSentPkgID;

    };

#endif // __FOTAENGINE_H__
