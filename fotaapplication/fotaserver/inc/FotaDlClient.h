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
 * Description:   Fota download client
 *
 */
#ifndef FOTADLCLIENT_H
#define FOTADLCLIENT_H

//All UI related headers
#include <QWidget>
#include <QtGui/QWidget>
#include <QtGui>

//All download manager related headers
#include <dmcommon.h>
#include <downloadmanager.h>
#include <download.h>

//All user includes
#include "FotaSrvDebug.h"

using namespace WRT;

//Forward declarations
class DownloadManagerClient;

//Enums for mapping download manager operation status
enum DLReturns
    {
    EOk = 0, ENotOk
    };

//Enums for type of download mechanism
enum TDownloadType
    {
    THttpDownload = 0, //HTTP download mechanism
    TOmaDL10Download, //OMA DL1.0 download mechanism
    TUnknownType
    //Unknown download mechanism
    };

//Enums for client (fota) side errors. This is used to know why client decided to pause/cancel the download.
enum TClientErrorType
    {
    ErrorNone = 0, //No error

    //Device side errors
    UserCancelled = 100,//User cancelled download    
    NeedMoreMemory, //Memory insufficient to start download

    //Server side errors        
    InvalidContentType = 200
    //Content type invalid. ie. either HTTP nor OMA DL1.0
    };

/**
 *  This is the download client class. 
 *
 *  @lib    fotaserver
 *  @since  SF^4
 */
class DownloadClient : public QObject
    {
Q_OBJECT

public:
    /**
     * Constructor.
     */
    DownloadClient(DownloadManagerClient* aObserver);

    /**
     * Destructor.
     */
    ~DownloadClient();

    /**
     * Creates a single download with the download manager
     *
     * @since   SF^4
     * @param   url - the source which is to be downloaded
     * @param   type - determines the type of download. Either parallel or sequential.
     * @return  One of DLReturns
     */
    TInt CreateDownload(const QString& url, DownloadType type);

    /**
     * Sets the required attributes for the single download.
     *
     * @since   SF^4
     * @return  One of DLReturns
     */
    TInt SetDownloadAttributes();

    /**
     * Gets the attribute of the single download
     *
     * @since   SF^4
     * @param   attr - the attribute of the download for which value is needed.
     * @return  The value of the attribute. Caller should read the value in right format.
     */
    QVariant GetDownloadAttribute(DownloadAttribute attr);

    /**
     * Starts the single download. Download should be created and attributes set before this.
     *
     * @since   SF^4
     * @param   None
     * @return  One of DLReturns
     */
    TInt Start();

    /**
     * Pauses the single download.
     *
     * @since   SF^4
     * @param   Client reason for pausing the download.
     * @return  One of DLReturns
     */
    TInt Pause(TClientErrorType aReason);

    /**
     * Resumes the single download.
     *
     * @since   SF^4
     * @param   None
     * @return  One of DLReturns
     */
    TInt Resume();

    /**
     * Cancels the single download.
     *
     * @since   SF^4
     * @param   Client reason for cancelling the download.
     * @return  One of DLReturns
     */
    TInt Cancel(TClientErrorType aReason);

    /**
     * Restarts the single download. This is equivalent to cancel and start on Download.
     *
     * @since   SF^4
     * @param   None
     * @return  None
     */
    TInt Restart();

private slots:

    /**
     * The slot which receives all the single download events.
     *
     * @since   SF^4
     * @param   event - the download event
     * @return  true, if handled here. Otherwise false.
     */
    bool DownloadEventRecieved(DownloadEvent *event);

private:
    /**
     * Called to update the progress of download to fota server. This fetches the percentage
     * of download from download manager.
     *  
     * @since   SF^4
     * @param   None
     * @return  None
     */
    void UpdateDownloadProgress();

    /**
     * Called when download is complete, either successfully or unsuccessfully.
     * The arguments to this function is read to know the actual status.
     *
     * @since   SF^4
     * @param   dlevent - the event of the single download
     * @param   err0 - the last error occured
     * @return  None
     */
    void HandleDownloadComplete(Download::State dlstate, int err0 = 0);

    /**
     * Called to handle the post download interrupt operation when client cancels/pauses download.
     * 
     * @since   SF^4
     * @param   dlstate - the state of the single download
     * @param   err0 - the last error occured
     * @return  None
     */
    void HandleClientInterrupt(Download::State dlstate, int err0);

    /**
     * Called to read the OMA DL1.0 download descriptor. This will update the fota server with size and version.
     *
     * @since   SF^4
     * @param   None
     * @return  None
     */
    void ReadDescriptorData();

    /**
     * Called to validate the content type of the download as received in header.
     *
     * @since   SF^4
     * @param   aContent - the content type as received in the header
     * @return  One of TDownloadType
     */
    TDownloadType CheckContentType(const QString aContent);

    void SetSubDownloadAttributes();

private:

    /**
     * The Fota download manger client. This is not owned
     */
    DownloadManagerClient* iFotaDlMgrClient;

    /**
     * The Download object of the single download
     */
    Download *iDownload;

    /**
     * Progress of the download. true when download progressing, otherwise false.
     */
    TBool iProgress;

    /**
     * The total size of the download (ie. update package).
     */
    int iTotalSize;

    /**
     * To tell if space check has happened or not, before the start of the download.
     */
    bool iSpaceChecked;

    /**
     * To tell if client has interrupted download or not. The reason will be in iClientError.
     */
    bool iClientinterrupted;

    /**
     * Holds the client error that caused the download to cancel or pause.
     */
    TClientErrorType iClientError;

    /**
     * The state of the single download. This corresponds to the state received in the download event slot.
     */
    Download::State iDlState;

    TDownloadType iContentType;

    };

#endif // FOTADLCLIENT_H
