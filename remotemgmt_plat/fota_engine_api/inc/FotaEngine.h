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
* Description:  Fotaengine hdr
*
*/



#ifndef __FOTAENGINE_H__
#define __FOTAENGINE_H__

// INCLUDES
#include <e32std.h>
#include <e32def.h>
#include <badesca.h>
#include <AknServerApp.h>
#include <SyncMLDef.h>
#include <apacmdln.h>
#include <apgtask.h>
#include <coemain.h>
#include <s32file.h>
#ifndef SYMBIAN_ENABLE_SPLIT_HEADERS
#include <uikon.hrh>
#else
#include <uikon.hrh>
#include <uiklaf/private/pluginuid.hrh>
#endif


#include "fotaengstream.h"
#include "fotaConst.h"

// CONSTANTS
/**  FOTA Server commands */
enum TFotaIPCCmds
	{
	EFotaFirstCommand = RApaAppServiceBase::KServiceCmdBase,
    EDeletePackage,
    EFotaDownload,
    EFotaDownloadAndUpdate,
    EGetResult,
    EGetState,
    EIsPackageStoreSizeAvailable,
    EFotaOpenUpdatePackageStore,
    // Command for getting the downloaded & full size of the update package 
    EFotaGetDownloadUpdatePackageSize,
    // Command to attempt resuming of the download session for downloading the remaining parts of the update package.
    // This service is restriced to DM UI and FMS Server.
    EFotaTryResumeDownload, 
    EFotaUpdate,
    EUpdatePackageDownloadComplete,
    EFotaSendChunk,
    EFotaReleaseChunkHandle,
    EGetUpdatePackageIds,
    EGetUpdateTimestamp,
    EGenericAlertSentForPackage,
    EScheduledUpdate
	};


enum TFotaUpdateStates
    {
    EFotaDefault,
    EFotaPendingGenAlert,
    EFotaDownloadInterrupted,
    EFotaUpdateInterrupted
    };

class TFotaScheduledUpdate;

// CLASS DECLARATION

/**
*	A client handle to a FOTA engine session.
*   @lib    fotaengine.lib
*   @since  Series 60 3.1
*/
class RFotaEngineSession : public RAknAppServiceBase
	{

    friend class    TDP2StreamBuf;  // TDP2StreamBuf will use iChunk

public: // enums

    /** 
    * An enumeration of the firmware update progress state codes as specified
    * in FUMO spec.
    */
    enum TState
	    {
	    /** No firmware update has been started */
	    EIdle                       = 10,
	    /** Client has sent a client initiated request */
	    EClientRequest              = 5,
	    /** There is no data available and download is about to start */
	    EStartingDownload           = 15,
	    /** Download failed and there is no data received */
	    EDownloadFailed             = 20,
  		/** Download is progressing with resume support. This is an internal state and is not a valid FUMO state.
	  	 * Only DM UI and Fota Server can get this state; others get state 30 instead.*/
  		EDownloadProgressingWithResume	= 25,
	    /** Download is progressing without resume support. */ 
	    EDownloadProgressing        = 30,
	    /** Have data and download has been completed successfully */
	    EDownloadComplete           = 40,
	    /** Have data and about to start update */
	    EStartingUpdate             = 50,
	    /** Denotes that the update is currently running, but has not yet 
        completed */
	    EUpdateProgressing          = 60,
	    /** Have data but update failed */
	    EUpdateFailed               = 70,
	    /** Update failed and data deleted or removed */
	    EUpdateFailedNoData         = 80,
	    /** Update complete and data still available */
	    EUpdateSuccessful           = 90,
	    /** Data deleted or removed after a successful update */
	    EUpdateSuccessfulNoData     = 100,	
	    };

	/** 
    * An enumeration of the firmware update result codes as specified
    * in the OMA Firmware Update Management Oobject specification. 
    */

    enum TResult
        {
        EResSuccessful                     = 200,
        EResUserCancelled                  = 401,
        EResCorruptedFWUPD                 = 402,
        EResPackageMismatch                = 403,
        EResFailedSignatureAuthentication  = 404,
        EResUndefinedError                  = 409,
        EResUpdateFailed                   = 410,
        EResMalformedOrBadURL              = 411,
        EResAlternateDLServerUnavailable   = 412,
        EResDLFailDueToDeviceOOM           = 501,
        EResDLFailDueToNWIssues            = 503
        };
		    
public: // Constructors 

	IMPORT_C RFotaEngineSession();
	
public: // new functions
	/**
	* Opens session to Fota Engine
    *         
    * @since  Series 60 3.1
    * @param      None
    * @return     None
	*/
	IMPORT_C void OpenL();
	
	/**
	* Closes session to Fota Engine
    * 
    * @since  Series 60 3.1
    * @param      None
    * @return     None
	*/
	IMPORT_C void Close();
	
		
	/**
	* Called when DM server calls execute command to
	* Firmware update object URI ./FUMO/<x>/Download
	* Initiates a firmware download for the specified update package.
    * 
    * @since  Series 60 3.1
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
	* @return			Immediate result of the command
    *                   KErrNotFound: url doesn't exist
	*/
	IMPORT_C TInt Download(
                 const TInt aPkgId
                ,const TDesC8& aPkgURL
                ,const TSmlProfileId aProfileId
                ,const TDesC8& aPkgName
                ,const TDesC8& aPkgVersion);
	
	
	/**
	* Called when DM server calls execute command to
	* Firmware update object URI ./FUMO/<x>/DownloadAndUpdate
	* Initiates a firmware download and an immediate update for the specified
    *                   update package.
    *
    * @since  Series 60 3.1
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
	* @return			Immediate result of the command
	*/
	IMPORT_C TInt DownloadAndUpdate(
                    const TInt aPkgId
                    ,const TDesC8& aPkgURL
                    ,const TSmlProfileId aProfileId
                    ,const TDesC8& aPkgName
                    ,const TDesC8& aPkgVersion);	
	

	/**
	* Called when DM server calls execute command to
	* Firmware update object URI ./FUMO/<x>/Update
	* Initiates a firmware update for the specified update package.
	* Firmware Update Package should be already downloaded to the device 
	* either using DM Large Object or OMA OTA download mechanisms.
    *
    * @since  Series 60 3.1
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
	IMPORT_C TInt Update(
                    const TInt aPkgId
                    ,const TSmlProfileId aProfileId
                    ,const TDesC8& aPkgName
                    ,const TDesC8& aPkgVersion);
	
	
	/**
	* Called when DM server is about to start sending
	* new firmware update package using DM Large Object download.
	* This function is used to enquire if there is enough space available
	* for the firmware update package. 
	* e.g when DM server is about to start sending new firmware update package using 
	* DM Large Object download.

    *
    * @since  Series 60 3.1
	* @param aSize		Size of the firmware update package. Since 
    *                   continuation of interrupted downloads is not supported
    *                   , this means new update package.
	* @return			ETrue if there is enough space available, EFalse 
    *                   otherwise
	*/
	IMPORT_C TBool IsPackageStoreSizeAvailable( const TInt aSize );

	
	/**
    * Opens stream to update package storage.
    * Call UpdatePackageDownloadComplete when package is completely written.
    *
    * @since  Series 60 3.1
	* @param aPkgId		Unique identifier of the update package. Download mgr
    *                   may use value -1.
	* @param aPkgStore	On return, open stream in which file may be written 
    *                   to. 
	* @return			Immediate result of the command. KErrInUse   store is 
    *                   already opened
	*/
	IMPORT_C TInt OpenUpdatePackageStore( const TInt aPkgId
                        , RWriteStream*& aPkgStore );

	/**
    * Gets the downloaded and full size of the update package.
    * Called by Download Manager during resume operation to know the size of partially downloaded package.
    * @since  Series 60 3.2.2
	* @param aPkgId		Unique identifier of the update package. 
	* @param aDownloadedSize	On return, size of the downloaded package in bytes 
	* @param aTotalSize		On return, full size of the download package in bytes
	*
	* @return KErrNone when successful, else System wide errors
    *
	*/

	IMPORT_C TInt GetDownloadUpdatePackageSize( const TInt aPkgId, TInt& aDownloadedSize, TInt& aTotalSize );

	/**
	* Requests to resume the suspended download of the update package.   
    * Called by Fota Monitory Service. for ex, when network is available again.
    * @since  Series 60 3.2.2
	* @param			None
    *
	* @return			KErrNone when successful, else System wide errors
    *
	*/
	
	IMPORT_C TInt TryResumeDownload();

	
	/**
	* Call this when download of update package is completed. In case of 
    * software error or network interruption, this is not called and package 
    * store is left empty. When this is called, fotaengine closes the stream 
    * and associated storage.
    *
    * @since  Series 60 3.1
	* @param      aPkgId	Unique identifier of the update package. Download
    *                       mgr may use value -1.
    * @return     None
	*/
	IMPORT_C void UpdatePackageDownloadComplete( const TInt aPkgId );
	
	
	/**
	* Called when caller wants to enquire State of specified
	* firmware update. If the State is unknown to FOTA Engine, then it should
	* return EIdle.
    *
    * @since  Series 60 3.1
	* @param aPkgId		Unique identifier of the update package.
	* @return			State reached by specified firmware update package 
	*/	
	IMPORT_C TState GetState( const TInt aPkgId );
	
		
	/**
	* Called when caller wants to enquire Final Result Code of specified
	* firmware update operation. If the update operation is not yet reached 
    * final stage, then -1 should be returned to the caller. Possible Final 
    * Result Codes are specified in OMA FUMO Specification. Note that Download
    * operation also ends to final result code e.g. 202 - Successful Download.
    *
    * @since  Series 60 3.1
	* @param aPkgId		Unique identifier of the update package.
	* @return			Result code as specified in FUMO specification, -1 if 
    *                   not yet reached final result.
	*/
	IMPORT_C TInt GetResult( const TInt aPkgId );
	
	
	/**
	* Called when DM server deletes firmware update management object
	* from device's management tree. I.e. Delete to mgmt URI ./FUMO/<x>.
	* If FOTA Engine has not yet deleted specified update package, then it 
    * should do so.	
    *
    * @since  Series 60 3.1
	* @param aPkgId		Unique identifier of the update package.
	* @return			Result code
	*/
	IMPORT_C TInt DeleteUpdatePackage( const TInt aPkgId );
	

    /**
    * Retrieves the timestamp of last succesful update.
    *
    * @since  Series 60 3.1
    * @param aUpdate    On return, contains time of last succesfull update
	* @return			Error code. KErrUnknown if device has never been 
    *                   updated.
	*/
    IMPORT_C TInt LastUpdate( TTime& aUpdate );

	/**
    * Tells device FW version
    *
    * @since  Series 60 3.1
    * @param aSWVersion On return, contains FW version of device.
	* @return			Error code
	*/
    IMPORT_C TInt CurrentVersion( TDes& aSWVersion );

	/**
    * Gets IDs of the present update packages.
    *
    * @since  Series 60 3.1
    * @param aPackageIdList   On return, contains array of pkg ids
	* @return				  Error code
	*/
    IMPORT_C TInt GetUpdatePackageIds( TDes16& aPackageIdList );

    IMPORT_C TVersion Version() const;


	/**
    * Tells fotaserver that generic alert for package is sent. 
    * When fotaengine session is closed, cleanup for package
    * is done.
    *
    * @since  Series 60 3.1
	* @param    aPackageId    
	* @return   Error code
	*/
    IMPORT_C void GenericAlertSentL( const TInt aPackageID ) ;


	/**
	* Like Update, but called by scheduler mechanism. 
    * Needed package details (profile id etc are already known)
    *
    * @since  Series 60 3.2
	* @param  aSchedule		Schedule data
	* @return	Error code
	*/
	IMPORT_C TInt ScheduledUpdateL( TFotaScheduledUpdate aSchedule );


private: // From RApaAppServiceBase

	/** Returns the UID of the service that this session provides an 
    * interface for. Client side service implementations must implement this
    * function to return the UID for the service that they implement.
    *
    * @since  Series 60 3.1
    * @param  None    
	* @return The UID of the service implemented by the derived class.
    */
    TUid ServiceUid() const;        

    IMPORT_C virtual void ExtensionInterface( 
                        TUid aInterfaceId
                        ,TAny*& aImplementation );

private: // New methods

    /**
    * Signals the server to read chunk contents
    *
    * @since  Series 60 3.1
    * @param aP1      Pointer to beginning of content
	* @param aP2      Pointer to end of content
	* @return		
    **/
    void SendChunkL(const TUint8* aP1, const TUint8* aP2);


    /**
    * Signals the server to release chunk handle
    *
    * @since  Series 60 3.1
    * @param    None
	* @return		
    **/
    TInt ReleaseChunkHandle();

    /**
    * Starts fotaserver server application.
    *
    * @since  Series 60 3.1
	* @param aNameUid         Differentiator.
    * @param aAppServerUid    FotaServer app uid
	* @return	None
    **/
    void StartApplicationL( const TUid& aNameUid,const TUid& aAppServerUid );


    /**
    * Connects to running fotaserver instance
    *
    * @since  Series 60 3.1
	* @param aNameUid         Differentiator.
    * @param aAppServerUid    FotaServer app uid
	* @return		None
    **/    
    void ConnectToServerL( const TUid& aNameUid,const TUid& aAppServerUid );

private: // Data

    /**
     * Stream for writing swupd file to chunk
     */
    RFotaWriteStream*  iStream;

    /**
     * Chunk for sending swupd contents to fotaserver
     */
    RChunk          iChunk;

    
    /**
     * If not -1 , indicates that generic alert has been sent for this package
     */
    TInt iGenericAlertSentPkgID;

	};


#endif // __FOTAENGINE_H__
