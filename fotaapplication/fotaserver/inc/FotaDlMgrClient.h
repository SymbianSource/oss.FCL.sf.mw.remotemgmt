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
 * Description:   Fota download manager client 
 *
 */
#ifndef FOTADLMGRCLIENT_H
#define FOTADLMGRCLIENT_H

#include <QtGui/QWidget>
#include <QtGui>
#include <QString>
#include <QWidget>
//All download manager related headers
#include <dmcommon.h>
#include <downloadmanager.h>
#include <downloadevent.h>
#include <download.h>
#include <devicedialogconsts.h>
#include <e32property.h>
#include <e32def.h>

//All orbit related headers
#include <hbdialog.h>
#include <hbdevicedialog.h>
#include <hbdevicedialogsymbian.h>
#include <hbsymbianvariant.h>

//All user includes
#include "FotaDlMgrClient.h"
#include "FotaSrvDebug.h"
#include <fotaengine.h>
#include "FotaIPCTypes.h"
#include "fotadiskstoragePrivateCRKeys.h"
#include "fotafullscreendialog.h"
#include "fotadevicedialogobserver.h"

//Forward declarations
class DownloadClient;
class CFotaServer;
class CFotaDownloadNotifHandler;

using namespace WRT;

enum TFreeSpace
    {
    EFitsToReservation, // fits to space reservation
    EFitsToFileSystem, // doesn't fit to reservation,fits to FS
    EDoesntFitToFileSystem
    // doesn't fit to fs
    };

// System Critical Level (128KB) plus 5KB for fota operations.
const TInt KSystemCriticalWorkingspace = 136192;
_LIT( KReservedFileName, "reserved");
_LIT( KDefaultPath, "C:\\private\\102072C4\\");
_LIT( KPackageExtension, ".swupd");
_LIT( KPackageExtensionAll, "*.swupd");
_LIT( KDownloadTempDir, "FotaServer\\downloads\\");

const QString DefaultPath = "C:\\private\\102072C4\\";
const QString PackageName = "fwupdate.swupd";

//Supported content types for firmware download
const QString ContentTypeHttp = "application/vnd.nokia.swupd.dp2";
const QString ContentTypeOmaDl = "application/vnd.oma.dd+xml";

/**
 *  This is the download manager client class. 
 *
 *  @lib    fotaserver
 *  @since  SF^4
 */
class DownloadManagerClient : public QObject, MfotadevicedialogObserver
    {
Q_OBJECT

public:
    /**
     * Two phase construction.
     */
    static DownloadManagerClient* NewL(CFotaServer* aServer);

    /**
     * The destructor
     */
    ~DownloadManagerClient();

    /**
     * This method returns the created DownloadManager pointer 
     * 
     * @since   SF^4
     * @param   
     * @return  the download manager pointer
     */
    DownloadManager* Manager();

    /**
     * Creates a single download from the given url.
     * 
     * @since   SF^4
     * @param   the source url of the download
     * @return  None. Can leave with system wide errors
     */
    void DownloadL(const TDesC8& url);

    /**
     * This method is called by the download client when the download begins.
     * @since   SF^4
     * @param   size - the size of the actual download
     * @param   version - the version as specified in the OMA DL1.0 descriptor
     * @return  None
     */
    void StartDownloadProgress(QString name, QString version, int size);

    /**
     * This method is called by the download client when the download is in progress.
     * @since   SF^4
     * @param   progress - progress of the download in percentage.
     * @return  None
     */
    void UpdateDownloadProgress(int progress);

    /**
     * This method is called by the download client when the download is complete, either 
     * successfully or unsuccessfully.
     * 
     * @since   SF^4
     * @param   dlstate - the state of the single download
     * @param   err0 - the last download error
     * @return  None
     */
    void HandleDownloadEvent(Download::State dlstate, int err0);

    /**
     * This method is called by the download client when the download is interrupted due to client specific reasons.
     * @since   SF^4
     * @param   dlstate - the state of the single download
     * @param   err0 - the last download error
     * @return  None
     */
    void HandleClientInterrupt(Download::State dlstate, int err0);

    /**
     * This method tells if the download is active or not.
     * @since   SF^4
     * @param   None
     * @return true, if active, otherwise false.
     */
    TBool IsDownloadActive();

    /**
     * The method finds the suitable path for a given download. It checks the availability of memory using the size provided.
     * @since   SF^4
     * @param   aSize - the size of the download
     * @param   aPath - the suitable path for the download to happen
     * @return  one of TFreeSpace - space available, space fits to reservation, space unavailable
     */
    TFreeSpace GetSuitablePath(TInt aSize, QString& aPath);

    /**
     * This method creates the required space reservation on the disk.
     * @since   SF^4
     * @param   None
     * @return None
     */
    void CreateDiskReservation();

    /**
     * This method deletes the reservation on the path provided.
     * @since   SF^4
     * @param   path - the path where reservation to be deleted.
     * @return None
     */
    void DeleteDiskReservation(QString& path);

    /**
     * This method deletes the update package on the disk.
     * @since   SF^4
     * @param   None
     * @return None, can leave with system wide errors.
     */
    void DeleteUpdatePackageL();

    /**
     * This method will try to resume any suspended single download
     * @since   SF^4
     * @param   None
     * @return  None, can leave with system wide errors.
     */
    void TryResumeDownloadL();

    /**
     * This method will try to pause any ongoing download
     * @since   SF^4
     * @param   None
     * @return 
     */
    void PauseDownloadL();

    /**
     * This method will get the location of the update package
     * @since   SF^4
     * @param   aPathj - location of the update package
     * @return None, can leave with system wide errors.
     */
    void GetUpdatePackageLocation(TDes& aPath);

    /**
     * This method will get size of the downloaded content
     * @since   SF^4
     * @param   None
     * @return size of the package in bytes
     */
    TInt GetDownloadPackageSize();

    /**
     * This function shows the device dialog which is requested
     * @since   SF^4
     * @param   dialogid - the dialog id to be shown
     * @param   aValue - the value to be passed, if any.
     * @return 
     */
    void ShowDialogL(TFwUpdNoteTypes dialogid, TInt aValue = 0);

public:
    // from MfotadevicedialogObserver

    void HandleDialogResponse(int response, TInt aDialogid);

public slots:

    /**
     * This is the slot which receives the download manager events.
     * @since   SF^4
     * @param   event - the download manager event
     * @return true if handled, otherwise false.
     */
    bool DownloadMgrEventRecieved(DownloadManagerEvent *event);

private:

    /**
     * Constructor.
     */

    DownloadManagerClient(CFotaServer *aServer);

    /**
     * Two phase construction
     */
    void ConstructL();

    /**
     * This functions initializes the download manager
     * @since   SF^4
     * @param   None
     * @return one of DLReturns
     */
    int InitDownloadMgr();

    /**
     * This function uninitializes the download manager.
     * @since   SF^4
     * @param   None
     * @return None
     */
    void UninitDownloadMgr();

    /**
     * This function returns the access point name for a given access point id.
     * @since   SF^4
     * @param   aIapId - access point id
     * @return access point name, can leave with system wide errors.
     */
    QString GetIapNameWithIdL(TInt aIapId);

    /**
     * This function calculates the disk space allocated for firmware update and reserved file.
     * @since   SF^4
     * @param   aSwupdSize, the size of the swupd files in bytes
     * @param   aReservedSize, the size of the reserved file in bytes.
     * @return None
     */
    void SpaceAllocatedBySWUPDFiles(TInt& aSwupdSize, TInt& aReservedSize);

    /**
     * This function maps the download errors to fota failure reasons
     * @since   SF^4
     * @param   err0 - the error in the single download
     * @return None
     */
    void MapDownloadErrors(int err0);

private:
    /**
     * The download manager instance
     */
    DownloadManager* iDownloadManager;

    /**
     * The fota download client instance
     */
    DownloadClient* iDownloadClient;

    /**
     * The fota server instance
     */
    CFotaServer* iFotaServer; // not owned

    /**
     * Is download manager initialized?
     */
    TBool iInitialized;

    /**
     * Is download submitted to download manager?
     */
    TBool iDownloadSubmitted;

    /**
     * Is download in progress?
     */
    TBool iProgress;

    /**
     * The file server instance
     */
    RFs iFs;

    /**
     * the reserved memory size in bytes
     */
    TInt iReservedSize;

    /**
     * The dialog variant maps which is exchanged with device dialog server
     */

    CHbSymbianVariantMap * iNotifParams;
    CFotaDownloadNotifHandler* iNotifier;
    };

#endif // FOTADLMGRCLIENT_H
