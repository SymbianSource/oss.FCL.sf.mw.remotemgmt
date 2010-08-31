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
#include <XQConversions>
#include "FotaDlClient.h"
#include "FotaDlMgrClient.h"

// -----------------------------------------------------------------------------
// The constructor of this class
// -----------------------------------------------------------------------------
DownloadClient::DownloadClient(DownloadManagerClient* Observer)
    {
    FLOG(_L("DownloadClient::DownloadClient >>"));
    
    iFotaDlMgrClient = Observer;
    iTotalSize = -1;
    iSpaceChecked = 0;
    iDownload = NULL;
    iClientinterrupted = false;
    iClientError = ErrorNone;
    iContentType = TUnknownType;
    
    FLOG(_L("DownloadClient::DownloadClient <<"));
    }

// -----------------------------------------------------------------------------
// The destructor of this class 
// -----------------------------------------------------------------------------
DownloadClient::~DownloadClient()
    {
    FLOG(_L("DownloadClient::~DownloadClient >>"));

    if (iDownload)
        {
        disconnect(iDownload, SIGNAL(downloadEvent(DownloadEvent *)), this,
                SLOT(DownloadEventRecieved(DownloadEvent *)));

        //Do not delete iDownload as it is owned by download manager.
        //delete iDownload;
        iDownload = NULL;
        }
    //Don't delete as it is not owned
    iFotaDlMgrClient = NULL;

    FLOG(_L("DownloadClient::~DownloadClient <<"));
    }

// -----------------------------------------------------------------------------
// Creates a single download with the download manager
// -----------------------------------------------------------------------------
TInt DownloadClient::CreateDownload(const QString &url, DownloadType type)
    {
    FLOG(_L("DownloadClient::CreateDownload >>"));

    TInt ret = ENotOk;

    iDownload = iFotaDlMgrClient->Manager()->createDownload(url, type); //Step 6

    if (iDownload)
        {
        connect(iDownload, SIGNAL(downloadEvent(DownloadEvent *)), this,
                SLOT(DownloadEventRecieved(DownloadEvent *)));
        ret = EOk;
        }
    else
        {
        FLOG(_L("Download creation is unsuccessful!"));
        }

    FLOG(_L("DownloadClient::CreateDownload, ret = %d <<"), ret);
    return ret;
    }

// -----------------------------------------------------------------------------
// Sets the required attributes for the single download.
// -----------------------------------------------------------------------------
TInt DownloadClient::SetDownloadAttributes()
    {
    FLOG(_L("DownloadClient::SetDownloadAttributes >>"));

    int ret(EOk);

    iDownload->setAttribute(ProgressInterval, 100); //Step 7
    iDownload->setAttribute(Priority, High); //Step 8

    //Setting the default path
    iDownload->setAttribute(DestinationPath, DefaultPath);

    iDownload->setAttribute(FileName, PackageName);

    FLOG(_L("DownloadClient::SetDownloadAttributes, ret = %d <<"), ret);

    return ret;
    }

// -----------------------------------------------------------------------------
// Starts the single download. Download should be created and attributes set before this.
// -----------------------------------------------------------------------------
TInt DownloadClient::Start()
    {
    FLOG(_L("DownloadClient::Start >>"));

    iClientinterrupted = false;
    iDownload->start(); //Step 9

    FLOG(_L("DownloadClient::Start <<"));
    return EOk;

    }

// -----------------------------------------------------------------------------
// Gets the attribute of the single download
// -----------------------------------------------------------------------------
inline QVariant DownloadClient::GetDownloadAttribute(DownloadAttribute attr)
    {
    FLOG(_L("DownloadClient::GetDownloadAttribute >>"));

    QVariant val;

    val = iDownload->attribute(attr);

    FLOG(_L("DownloadClient::GetDownloadAttribute<<"));
    return val;
    }

// -----------------------------------------------------------------------------
// Pauses the single download.
// -----------------------------------------------------------------------------
TInt DownloadClient::Pause(TClientErrorType aReason)
    {
    FLOG(_L("DownloadClient::Pause >>"));

    iClientinterrupted = true;
    iClientError = aReason;

    iDownload->pause();

    FLOG(_L("DownloadClient::Pause <<"));
    return EOk;
    }

// -----------------------------------------------------------------------------
// Resumes the single download.
// -----------------------------------------------------------------------------
TInt DownloadClient::Resume()
    {
    FLOG(_L("DownloadClient::Resume >>"));

    int ret(ENotOk);
    iSpaceChecked = true;

    if (!iDownload)
        {
        QList<Download*> dls =
                iFotaDlMgrClient->Manager()->currentDownloads();
        int count = dls.count();
        FLOG(_L("Number of current downloads = %d"), count);

        if (count)
            {
            iDownload = dls[0];
            FLOG(_L("Connecting to download event"));
            connect(iDownload, SIGNAL(downloadEvent(DownloadEvent *)), this,
                    SLOT(DownloadEventRecieved(DownloadEvent *)));
            FLOG(_L("Connecting to download events done"));
            }

        }
    iClientinterrupted = false;
    iClientError = ErrorNone;

    if (iDownload)
        {
        FLOG(_L("B4 Download Start"));
        iDownload->start();
        ret = EOk;
        FLOG(_L("After Download Start"));
        }

    FLOG(_L("DownloadClient::Resume, ret = %d <<"), ret);
    return ret;
    }

// -----------------------------------------------------------------------------
// Cancels the single download.
// -----------------------------------------------------------------------------
TInt DownloadClient::Cancel(TClientErrorType aReason)
    {
    FLOG(_L("DownloadClient::Cancel >>"));

    iClientinterrupted = true;
    iClientError = aReason;

    iDownload->cancel();

    FLOG(_L("DownloadClient::Cancel <<"));
    return EOk;
    }

// -----------------------------------------------------------------------------
// Restarts the single download. This is equivalent to cancel and start on Download.
// -----------------------------------------------------------------------------
TInt DownloadClient::Restart()
    {
    FLOG(_L("DownloadClient::Restart >>"));

    QString path(NULL);
    int size = iTotalSize;
    TFreeSpace avail = iFotaDlMgrClient->GetSuitablePath(size, path);

    if (avail == EFitsToReservation || avail == EFitsToFileSystem)
        {
        //FLOG(_L("Space is available for download at %S"), TPtrC (reinterpret_cast<const TText*> (path.constData()),path.length()));
        
        if (iContentType == TOmaDL10Download)
            {
            ReadDescriptorData();
            }
        Resume();
        }
    else
        {
        FLOG(_L("Space not available. Download is stopped!"));
        TRAP_IGNORE(iFotaDlMgrClient->ShowDialogL(EFwDLNeedMoreMemory, size));
        }

    FLOG(_L("DownloadClient::Restart >>"));

    return EOk;
    }

// -----------------------------------------------------------------------------
// The slot which receives all the single download events.
// -----------------------------------------------------------------------------
bool DownloadClient::DownloadEventRecieved(DownloadEvent *event)
    {
    FLOG(_L("DownloadClient::event >>"));

    DownloadEvent::Event type = (DownloadEvent::Event) event->type();
    bool eventHandled = false;
    int err0(NoError);

    FLOG(_L("Download Event Type: %d"), type);

    switch (type)
        {
        case DownloadEvent::Started:
            {
            FLOG(_L("DownloadEventRecieved - DownloadEvent::Started"));
            iProgress = true;
            iDlState = Download::Created;

            eventHandled = true;
            break;
            }

        case DownloadEvent::HeadersReceived:
            {
            FLOG(_L("DownloadEventRecieved - DownloadEvent::HeadersReceived"));
            iProgress = true;
            iDlState = Download::Created;

            QString contenttype =
                    GetDownloadAttribute(ContentType).toString();
            HBufC* s_contenttype = XQConversions::qStringToS60Desc(contenttype);
            FLOG(_L("Content type received is %S"), s_contenttype);
            delete s_contenttype;

            iContentType = CheckContentType(contenttype);

            if (iContentType == THttpDownload)
                {
                FLOG(_L("Content type: Http; checking the size of download"));
                if (!iSpaceChecked)
                    {

                    iTotalSize = GetDownloadAttribute(TotalSize).toInt();

                    FLOG(_L("Size of the firmware update as received is %d"),
                            iTotalSize);

                    if (iTotalSize > 0)
                        {
                        QString path(NULL);

                        TFreeSpace avail = iFotaDlMgrClient->GetSuitablePath(
                                iTotalSize, path);

                        if (avail == EFitsToReservation || avail
                                == EFitsToFileSystem)
                            {
                            //FLOG(_L("Space is available for download at %S"),TPtrC (reinterpret_cast<const TText*> (path.constData()),path.length()));
                            iFotaDlMgrClient->DeleteDiskReservation(path);
                            iDownload->setAttribute(DestinationPath, path);
                            }
                        else
                            {
                            FLOG(
                                    _L("Space not available. Download is stopped!"));
                            Pause(NeedMoreMemory);
                            TRAP_IGNORE(iFotaDlMgrClient->ShowDialogL(EFwDLNeedMoreMemory));
                            break;
                            }

                        iSpaceChecked = true;
                        iFotaDlMgrClient->StartDownloadProgress(
                                QString::null, QString::null, iTotalSize);
                        }
                    }
                }
            else if (iContentType == TUnknownType)
                {
                FLOG(_L("Content type unknown; hence cancelling download !"));
                Cancel(InvalidContentType);
                }

            eventHandled = true;
            break;
            }

        case DownloadEvent::InProgress:
            {
            FLOG(_L("DownloadEventRecieved - DownloadEvent::InProgress"));

            iProgress = true;
            iDlState = Download::InProgress;
            UpdateDownloadProgress();

            eventHandled = true;
            break;
            }

        case DownloadEvent::Completed:
            {
            FLOG(_L("DownloadEventRecieved - DownloadEvent::Completed"));
            iProgress = false;
            iDlState = Download::Completed;
            iTotalSize = 0;
            UpdateDownloadProgress();

            eventHandled = true;
            break;
            }

        case DownloadEvent::Paused:
            {
            FLOG(_L("DownloadEventRecieved - DownloadEvent::Paused"));
            iProgress = false;
            iDlState = Download::Paused;

            eventHandled = true;
            break;
            }
        case DownloadEvent::Failed:
            {
            FLOG(_L("DownloadEventRecieved - DownloadEvent::Failed"));
            iProgress = false;
            iDlState = Download::Failed;

            eventHandled = true;
            break;
            }
        case DownloadEvent::Cancelled:
            {
            FLOG(_L("DownloadEventRecieved - DownloadEvent::Cancelled"));
            iProgress = false;
            iDlState = Download::Cancelled;

            eventHandled = true;
            break;
            }

        case DownloadEvent::NetworkLoss:
            {
            FLOG(_L("DownloadEventRecieved - DownloadEvent::NetworkLoss"));
            iProgress = false;
            iDlState = Download::Paused;

            eventHandled = true;
            break;
            }

        case DownloadEvent::Error:
            {
            FLOG(_L("DownloadEventRecieved - DownloadEvent::Error"));
            iProgress = false;
           // iDlState = (Download::State) GetDownloadAttribute(State).toInt();
            iDlState = Download::Failed;
            eventHandled = true;
            break;
            }

        case DownloadEvent::DescriptorReady:
            {
            FLOG(_L("DownloadEventRecieved - DownloadEvent::DescriptorReady"));
            iProgress = true;
            iDlState = Download::InProgress;
            iContentType = TOmaDL10Download;

            iTotalSize = GetDownloadAttribute(DescriptorSize).toInt();
            QString path(NULL);
            if (iTotalSize > 0)
                {

                TFreeSpace avail = iFotaDlMgrClient->GetSuitablePath(
                        iTotalSize, path);

                if (avail == EFitsToReservation || avail == EFitsToFileSystem)
                    {
                    //FLOG(_L("Space is available for download at %S"), TPtrC (reinterpret_cast<const TText*> (path.constData()),path.length()));
                    iFotaDlMgrClient->DeleteDiskReservation(path);
                    }
                else
                    {
                    FLOG(_L("Space not available. Download is stopped!"));
                    TRAP_IGNORE(iFotaDlMgrClient->ShowDialogL(EFwDLNeedMoreMemory));
                    break;
                    }
                }
            ReadDescriptorData();
            Resume();
            SetSubDownloadAttributes();

            eventHandled = true;
            break;
            }
        case DownloadEvent::CreatingConnection:
            {
            FLOG(
                    _L("DownloadEventRecieved - DownloadEvent::CreatingConnection"));
            iProgress = true;
            iDlState = Download::InProgress;

            eventHandled = true;
            break;
            }
        case DownloadEvent::ConnectionNeeded:
        case DownloadEvent::ConnectionDisconnected:
            {
            FLOG(
                    _L("DownloadEventRecieved - DownloadEvent::ConnectionNeeded/ConnectionDisconnected"));
            iProgress = false;
            iDlState = Download::Paused;
            err0 = ConnectionFailed;
            eventHandled = true;
            break;
            }

        default:
            {
            FLOG(_L("DownloadEventRecieved - Skipped this event: %d"), type);
            break;
            }
        }

    if (iProgress == false)
        {

        if (iClientinterrupted)
            {
            //Client has requested for cancellation. Hence provide the same error code
            err0 = iClientError;
            HandleClientInterrupt(iDlState, err0);
            }
        else
            {
            //Download Manager has cancelled download. Hence provide the last error code.
            if (err0 == NoError)
                err0 = GetDownloadAttribute(LastError).toInt();

            HandleDownloadComplete(iDlState, err0);
            }

        }

    FLOG(_L("DownloadClient::event <<"));

    return eventHandled;
    }

// -----------------------------------------------------------------------------
// Called to update the progress of download to fota server. This fetches the percentage
// of download from download manager.
// -----------------------------------------------------------------------------
inline void DownloadClient::UpdateDownloadProgress()
    {
    FLOG(_L("DownloadClient::UpdateDownloadProgress >>"));

    // Remaining size calculation
    int prog = GetDownloadAttribute(Percentage).toInt();
    iFotaDlMgrClient->UpdateDownloadProgress(prog);

    FLOG(_L("DownloadClient::UpdateDownloadProgress, progress = %d <<"), prog);
    }

// -----------------------------------------------------------------------------
// Called to read the OMA DL1.0 download descriptor. This will update the fota server with size and version.
// -----------------------------------------------------------------------------
void DownloadClient::ReadDescriptorData()
    {
    FLOG(_L("DownloadClient::ReadDescriptorData >>"));

    QString name = GetDownloadAttribute(DescriptorName).toString(); // "name" in OMA dd
    QString version = GetDownloadAttribute(DescriptorVersion).toString(); // "version" in OMA dd
    QString type = GetDownloadAttribute(DescriptorType).toString(); // "type" in OMA dd
    int size = GetDownloadAttribute(DescriptorSize).toInt(); // "size" in OMA dd
    QString vendor = GetDownloadAttribute(DescriptorVendor).toString(); // "vendor" in OMA dd
    QString description =
            GetDownloadAttribute(DescriptorDescription).toString(); // "description" in OMA dd
    QString nxturl = GetDownloadAttribute(DescriptorNextURL).toString(); // "nextURL" in OMA dd

    TDownloadType gottype = CheckContentType(type);

    if (gottype == THttpDownload)
        {
        iFotaDlMgrClient->StartDownloadProgress(name, version, size);
        }
    else
        {
        FLOG(
                _L("The content type in descriptor is not appropriate! Hence cancelling download"));
        Cancel(InvalidContentType);
        }

    FLOG(_L("DownloadClient::ReadDescriptorData <<"));
    }

// -----------------------------------------------------------------------------
// Called when download is complete, either successfully or unsuccessfully.
// The arguments to this function is read to know the actual status.
// -----------------------------------------------------------------------------
void DownloadClient::HandleDownloadComplete(Download::State dlstate, int err0)
    {
    FLOG(_L("DownloadClient::HandleDownloadComplete, idlstate = %d, err0 = %d>>"), dlstate, err0);

    iFotaDlMgrClient->HandleDownloadEvent(dlstate, err0);

    FLOG(_L("DownloadClient::HandleDownloadComplete <<"));
    }

// -----------------------------------------------------------------------------
// Called to handle the post download interrupt operation when client cancels/pauses download.
// -----------------------------------------------------------------------------
void DownloadClient::HandleClientInterrupt(Download::State dlstate, int err0)
    {
    FLOG(_L("DownloadClient::HandleClientInterrupt, idlstate = %d, err0 = %d >>"), dlstate, err0);

    iFotaDlMgrClient->HandleClientInterrupt(dlstate, err0);

    FLOG(_L("DownloadClient::HandleClientInterrupt <<"));
    }

// -----------------------------------------------------------------------------
// Called to validate the content type of the download as received in header.
// -----------------------------------------------------------------------------
TDownloadType DownloadClient::CheckContentType(const QString aContent)
    {
    FLOG(_L("DownloadClient::CheckContentType >>"));

    TDownloadType type(TUnknownType);

    QString semicollon(";");
    QString contenttype = aContent;

    int index = aContent.indexOf(semicollon, 0);

    if (index > 0)
        {
        contenttype.chop(aContent.length() - index);
        }

    if (contenttype.compare(ContentTypeHttp) == 0)
        type = THttpDownload;
    else if (contenttype.compare(ContentTypeOmaDl) == 0)
        type = TOmaDL10Download;

    FLOG(_L("DownloadClient::CheckContentType, type = %d<<"), type);

    return type;
    }

void DownloadClient::SetSubDownloadAttributes()
    {
    FLOG(_L("DownloadClient::SetSubDownloadAttributes >>"));

    QList<Download*> dls = iDownload->subDownloads();

    if (dls.count() > 0)
        {
        Download* subdl = dls[0];

        subdl->setAttribute(FileName, PackageName);
        }
    else
        {
        FLOG(_L("Error: There are no sub downloads!"));
        }

    FLOG(_L("DownloadClient::SetSubDownloadAttributes <<"));
    }

//End of file
