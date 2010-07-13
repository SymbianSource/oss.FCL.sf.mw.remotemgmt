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

//System includes
#include <sysutil.h> 
#include <bautils.h>
#include <cmmanager.h>
#include <cmconnectionmethod.h>
#include <devicedialogconsts.h>
#include <centralrepository.h>
#include <XQConversions>
#include <uri8.h> //For parsing uri
//User includes
#include "FotaDlMgrClient.h"
#include "FotaDlClient.h"
#include "FotaServer.h"
#include "fotanotifiers.h"

// --------------------------------------------------------------------------
//  Two phase construction
// --------------------------------------------------------------------------
DownloadManagerClient* DownloadManagerClient::NewL(CFotaServer* aServer)
    {
    DownloadManagerClient* self = new (ELeave) DownloadManagerClient(aServer);
    self->ConstructL();
    return self;
    }

// --------------------------------------------------------------------------
//  The constructor
// --------------------------------------------------------------------------
DownloadManagerClient::DownloadManagerClient(CFotaServer* aServer) :
    iDownloadManager(NULL), iDownloadClient(NULL), iFotaServer(aServer),
            iInitialized(EFalse), iDownloadSubmitted(EFalse), iReservedSize(
                    5242880), iNotifParams(NULL), iNotifier (NULL)
    {

    }

// --------------------------------------------------------------------------
//  The destructor
// --------------------------------------------------------------------------
DownloadManagerClient::~DownloadManagerClient()
    {
    FLOG(_L("DownloadManagerClient::~DownloadManagerClient >>"));

    if (iDownloadClient)
        {
        delete iDownloadClient;
        iDownloadClient = NULL;
        }

    UninitDownloadMgr(); //Closes the RFs session and iDownloadManager.

    iFs.Close();
    
    	/*
    if (iNotifParams)
        {
        FLOG(_L("DownloadManagerClient::~DownloadManagerClient, iNotifParams >>"));
        delete iNotifParams;
        iNotifParams = NULL;
        FLOG(_L("DownloadManagerClient::~DownloadManagerClient, iNotifParams <<"));
        }
    if (iNotifier)
        {
        FLOG(_L("DownloadManagerClient::~DownloadManagerClient, iNotifier >>"));
        delete iNotifier;
        iNotifier = NULL;
        FLOG(_L("DownloadManagerClient::~DownloadManagerClient, iNotifier <<"));
        }*/
    FLOG(_L("DownloadManagerClient::~DownloadManagerClient <<"));
    }

// --------------------------------------------------------------------------
//  Two phase construction
// --------------------------------------------------------------------------
void DownloadManagerClient::ConstructL()
    {
    FLOG(_L("DownloadManagerClient::ConstructL >>"));

    TInt err;
    CRepository* centrep(NULL);

    TRAP(err, centrep = CRepository::NewL( KCRUidFotaDiskStorage ) );
    if (centrep)
        {
        err = centrep->Get(KFotaDiskSpaceReservationKey, iReservedSize);
        }

    delete centrep;
    centrep = NULL;

    __LEAVE_IF_ERROR(iFs.Connect());

    iProgress = EFalse;

    FLOG(_L("DownloadManagerClient::ConstructL, reservation needed is %d <<"),iReservedSize);
    }

// --------------------------------------------------------------------------
//  Initializes the download manager for fota requirements
// --------------------------------------------------------------------------
int DownloadManagerClient::InitDownloadMgr()
    {
    FLOG(_L("DownloadManagerClient::InitDownloadMgr >>"));

    int ret = EOk;

    if (!iInitialized)
        {
        iDownloadManager = new DownloadManager("FotaServer"); //Step 0
        connect(iDownloadManager,
                SIGNAL(downloadManagerEvent(DownloadManagerEvent *)), this,
                SLOT(DownloadMgrEventRecieved(DownloadManagerEvent *))); //step 1
        //Scope as false signifies that the download should happen on fota process
        bool scope = false;
        ret = iDownloadManager->initialize(scope); //Step 2

#if defined(__WINS__)
        iDownloadManager->setProxy("bswebproxy01.americas.nokia.com", 8080); //Step 3/
#endif
        //Set the progress mode of download to be non quite
        ret = iDownloadManager->setAttribute(ProgressMode, NonQuiet);
        //Set the persistent mode of download to be active.
        ret = iDownloadManager->setAttribute(PersistantMode, Active);
        //Set the default destination path for all the downloads
        ret = iDownloadManager->setAttribute(DefaultDestinationPath, DefaultPath);

        if (iFotaServer->iPackageState.iIapId > 0)
            {
            QString name;
            TRAPD(err, name = GetIapNameWithIdL(iFotaServer->iPackageState.iIapId));
            HBufC* temp = XQConversions::qStringToS60Desc(name);
            FLOG(_L("IAP name for ID %d is %S, err = %d"), iFotaServer->iPackageState.iIapId, temp, err);
            delete temp;
#if defined(__WINS__)

#else
            if (err == KErrNone)
                {
                FLOG(_L("Setting IAP =int for the single download"), iFotaServer->iPackageState.iIapId);
                ret = iDownloadManager->setAttribute(AccessPoint, name);
                }
#endif  
            }

        iInitialized = ETrue;
        }

    FLOG(_L("DownloadManagerClient::InitDownloadMgr, ret = %d <<"), ret);
    return ret;
    }

// --------------------------------------------------------------------------
//  Uninitializes the download manager
// --------------------------------------------------------------------------
void DownloadManagerClient::UninitDownloadMgr()
    {
    FLOG(_L("DownloadManagerClient::UninitDownloadMgr >>"));

    if (iDownloadManager)
        {
        disconnect(iDownloadManager,
                SIGNAL(downloadManagerEvent(DownloadManagerEvent *)), this,
                SLOT(DownloadMgrEventRecieved(DownloadManagerEvent *)));

        iDownloadManager->deleteLater();
        iDownloadManager = NULL;
        }
    iInitialized = EFalse;

    FLOG(_L("DownloadManagerClient::UninitDownloadMgr <<"));
    }

// --------------------------------------------------------------------------
//  Gets the access point name for a given access point id.
// --------------------------------------------------------------------------
QString DownloadManagerClient::GetIapNameWithIdL(TInt aIapId)
    {
    FLOG(_L("DownloadManagerClient::GetIapNameWithIdL, iapid = %d"), aIapId);
    
    QString name(NULL);

    RCmManager cmManager;
    cmManager.OpenL();

    RCmConnectionMethod conn;
    conn = cmManager.ConnectionMethodL(aIapId);

    HBufC* temp = conn.GetStringAttributeL(CMManager::ECmName);
    name = QString::fromUtf16(temp->Ptr(), temp->Length());
    delete temp;
    temp = NULL;

    conn.Close();

    cmManager.Close();
    FLOG(_L("DownloadManagerClient::GetIapNameWithIdL <<"));
    return name;
    }

// --------------------------------------------------------------------------
//  Starts the download using a given url
// --------------------------------------------------------------------------
void DownloadManagerClient::DownloadL(const TDesC8& url)
    {
    FLOG(_L("DownloadManagerClient::Download >>"));

    iDownloadSubmitted = EFalse;
    iProgress = EFalse;
    //Validate url...
    
    TUriParser8 parser;

    if (parser.Parse(url) && !parser.IsSchemeValid())
        {
        FLOG(_L("URL is malformed.. finalizing download"));
        iFotaServer->iPackageState.iState = RFotaEngineSession::EDownloadFailed;
        iFotaServer->iPackageState.iResult= RFotaEngineSession::EResMalformedOrBadURL;
        iFotaServer->iDatabase->OpenDBL();
        iFotaServer->iDatabase->SetStateL(iFotaServer->iPackageState, KNullDesC8, EFDBState | EFDBResult);
        iFotaServer->iDatabase->CloseAndCommitDB();

        iFotaServer->FinalizeDownloadL();
        TRAP_IGNORE(ShowDialogL(EFwDLNonResumableFailure));
        return;
        }
    
    FLOG(_L("DownloadManagerClient::DownloadL, State 2 - init download manager"));
    InitDownloadMgr();

    //Remove any existing download for this client
    iDownloadManager->removeAll();

    DeleteUpdatePackageL();

    DownloadType type = Parallel;

    if (!iDownloadClient)
        {
        iDownloadClient = new DownloadClient(this);
        }

    FLOG(_L("DownloadManagerClient::DownloadL, State 3 - creating download"));

    QString temp = QString::fromUtf8(
            reinterpret_cast<const char*> (url.Ptr()), url.Length());
    int err (0);
    if ((err = iDownloadClient->CreateDownload(temp, type)) != 0)
        {
        FLOG(_L("Error in creating download"), err);
        }
    else if ((err = iDownloadClient->SetDownloadAttributes()) != 0)
        {
        FLOG(_L("Error in setting attributes for download: %d"), err);
        }
    else 
        {
        iFotaServer->iPackageState.iState  = RFotaEngineSession::EStartingDownload;
        iFotaServer->iPackageState.iResult = KErrNotFound;
        iFotaServer->iDatabase->OpenDBL();
        iFotaServer->iDatabase->SetStateL(iFotaServer->iPackageState, KNullDesC8, EFDBState | EFDBResult);
        iFotaServer->iDatabase->CloseAndCommitDB();
        iDownloadSubmitted = ETrue;
        iFotaServer->SetStartupReason(EFotaDownloadInterrupted);
        
        iDownloadClient->Start();
        FLOG(_L("Download is submitted successfully"));
        }

    if (err == ENotOk)
        {
        iFotaServer->iDatabase->OpenDBL();
        iFotaServer->iPackageState.iState = RFotaEngineSession::EDownloadFailed;
        iFotaServer->iPackageState.iResult = RFotaEngineSession::EResUndefinedError;

        iFotaServer->iDatabase->SetStateL(iFotaServer->iPackageState,  KNullDesC8, EFDBState | EFDBResult);
        iFotaServer->iDatabase->CloseAndCommitDB();

        iFotaServer->FinalizeDownloadL();
        TRAP_IGNORE(ShowDialogL(EFwDLNonResumableFailure));
        }

    FLOG(_L("DownloadManagerClient::Download <<"));
    }

// --------------------------------------------------------------------------
//  Pauses an ongoing download.
// --------------------------------------------------------------------------
void DownloadManagerClient::PauseDownloadL()
    {
    FLOG(_L("DownloadManagerClient::PauseDownloadL >>"));

    TInt ret(ENotOk);

    if (iDownloadClient)
        {
        ret = iDownloadClient->Pause(UserCancelled);
        
        __LEAVE_IF_ERROR(ret);
        }

    FLOG(_L("DownloadManagerClient::PauseDownloadL <<"));
    }

// --------------------------------------------------------------------------
//  Tries to resume a suspended download
// --------------------------------------------------------------------------
void DownloadManagerClient::TryResumeDownloadL()
    {
    FLOG(_L("DownloadManagerClient::TryResumeDownloadL >>"));

    TInt ret(ENotOk);

    FLOG(_L("DownloadManagerClient::TryResumeDownloadL, State 1 - init download manager"));

    __LEAVE_IF_ERROR(InitDownloadMgr());

    iDownloadSubmitted = ETrue;
    iProgress = EFalse;

    if (!iDownloadClient)
        {
        iDownloadClient = new DownloadClient(this);
        }

    iFotaServer->SetStartupReason(EFotaDownloadInterrupted);
    ret = iDownloadClient->Resume();
    
    if (ret == ENotOk )
        {
        if (iFotaServer->DecrementDownloadRestartCount())
            {
            FLOG(_L("There is no paused download! Hence restarting download..."));
            DownloadL(iFotaServer->iLastFwUrl);
            }
        else
            {
            FLOG(_L("There is no paused download! Restart exhausted and hence finalizing download..."));
            iFotaServer->iPackageState.iState = RFotaEngineSession::EDownloadFailed;
            
            if (iFotaServer->iPackageState.iResult == -1 )
                iFotaServer->iPackageState.iResult = RFotaEngineSession::EResUndefinedError;
            
            iDownloadSubmitted = EFalse;
            iFotaServer->FinalizeDownloadL();
            TRAP_IGNORE(ShowDialogL(EFwDLNonResumableFailure));
            }
        }
    FLOG(_L("DownloadManagerClient::TryResumeDownloadL, ret = %d <<"), ret);
    }

// --------------------------------------------------------------------------
//  The slot which gets all the download manager events.
// --------------------------------------------------------------------------
bool DownloadManagerClient::DownloadMgrEventRecieved(
        DownloadManagerEvent *event)
    {
    FLOG(_L("DownloadManagerClient::event >>"));

    DownloadManagerEvent::Event type = (DownloadManagerEvent::Event) event->type();
    TBool ret(EFalse);

    FLOG(_L("Download Manager Event Type: %d"), type);
    switch (type)
        {
        case DownloadManagerEvent::Created:
            {
            FLOG(_L("Download is created..."));
            ret = ETrue;
            break;
            }
        case DownloadManagerEvent::Removed:
            {
            FLOG(_L("Download is cleared..."));
            ret = ETrue;
            break;
            }
        }

    FLOG(_L("DownloadManagerClient::event, ret = %d <<"), ret);
    return ret;
    }

// --------------------------------------------------------------------------
//  Called by the download client when the single download begins.
// --------------------------------------------------------------------------
void DownloadManagerClient::StartDownloadProgress(QString name,
        QString version, int size)
    {

    HBufC8* temp1 = XQConversions::qStringToS60Desc8(name);
    HBufC8* temp2 = XQConversions::qStringToS60Desc8(version);
    iFotaServer->iPackageState.iState = RFotaEngineSession::EDownloadProgressing;
    iFotaServer->iPackageState.iPkgSize = size;

    if (temp1->Length() <= iFotaServer->iPackageState.iPkgName.MaxLength())
        iFotaServer->iPackageState.iPkgName.Copy(temp1->Des());

    if (temp2->Length() <= iFotaServer->iPackageState.iPkgVersion.MaxLength())
        iFotaServer->iPackageState.iPkgVersion.Copy(temp2->Des());
    delete temp1;
    delete temp2;
    TRAP_IGNORE(
            iFotaServer->iDatabase->OpenDBL();
            iFotaServer->iDatabase->SetStateL( iFotaServer->iPackageState, KNullDesC8, EFDBPkgName|EFDBVersion|EFDBPkgSize|EFDBState);
            iFotaServer->iDatabase->CloseAndCommitDB();
            );

    if (!iFotaServer->iPackageState.iSessionType)
        {
        iFotaServer->StartDownloadDialog(name, version, size);
        iFotaServer->ConstructApplicationUI(ETrue);
        }
    }

// --------------------------------------------------------------------------
//  Called by the download client to update the progress of download.
// --------------------------------------------------------------------------
void DownloadManagerClient::UpdateDownloadProgress(int progress)
    {
    FLOG(_L("DownloadManagerClient::UpdateDownloadProgress, progress = %d >>"),
            progress );

    iProgress = ETrue;
    
    if (!iFotaServer->iPackageState.iSessionType)
        {
        iFotaServer->UpdateDownloadDialog(progress);
        }

    FLOG(_L("DownloadManagerClient::UpdateDownloadProgress <<"));

    }

// --------------------------------------------------------------------------
//  Called by the download client when the download is complete, either successfully or unsuccessfully.
// --------------------------------------------------------------------------
void DownloadManagerClient::HandleDownloadEvent(Download::State dlstate,
        int err0)
    {
    FLOG(_L("DownloadManagerClient::HandleDownloadEvent, status = %d >>"),  dlstate);

    iDownloadSubmitted = EFalse;
    iProgress = EFalse;
    TInt notetype(-1);

    if (iDownloadClient)
        {
        delete iDownloadClient;
        iDownloadClient = NULL;
        }

    if (dlstate == Download::Completed)
        {
        iFotaServer->iPackageState.iState = RFotaEngineSession::EDownloadComplete;
        iFotaServer->iPackageState.iResult = -1;
        }
    else if (dlstate == Download::Paused)
        {
        iFotaServer->iPackageState.iState = RFotaEngineSession::EDownloadProgressing;

        MapDownloadErrors(err0);
        }
    else if (dlstate == Download::Failed)
        {
        iFotaServer->iPackageState.iState = RFotaEngineSession::EDownloadFailed;

        MapDownloadErrors(err0);

        if(iFotaServer->iPackageState.iResult == RFotaEngineSession::EResDLFailDueToDeviceOOM
           || iFotaServer->iPackageState.iResult == RFotaEngineSession::EResDLFailDueToNWIssues)
            {
            iFotaServer->iPackageState.iState = RFotaEngineSession::EDownloadProgressing;
            }
        }
    else if (dlstate == Download::Cancelled)
        {
        iFotaServer->iPackageState.iState = RFotaEngineSession::EDownloadProgressing;

        MapDownloadErrors(err0);
        }
    else if (dlstate == Download::Paused)
        {
        iFotaServer->iPackageState.iState = RFotaEngineSession::EDownloadProgressing;

        MapDownloadErrors(err0);
        }
    else
        {
        FLOG(_L(" I DO NOT KNOW THIS DLSTATE!!"));
        }
    TRAPD(err,
            iFotaServer->iDatabase->OpenDBL();
            iFotaServer->iDatabase->SetStateL( iFotaServer->iPackageState, KNullDesC8, EFDBState|EFDBResult);
            iFotaServer->iDatabase->CloseAndCommitDB();
    );
    FLOG(_L("Updating fota database, error = %d"), err);

    UninitDownloadMgr();

    if (iFotaServer->iPackageState.iState == RFotaEngineSession::EDownloadComplete
            && iFotaServer->iPackageState.iUpdateLtr)
        {
        iFotaServer->SetStartupReason(EFotaUpdateInterrupted);
        }

    if (iFotaServer->iPackageState.iResult
            == RFotaEngineSession::EResDLFailDueToNWIssues)
        {
        notetype = EFwDLConnectionFailure;
        }
    else if (iFotaServer->iPackageState.iState == RFotaEngineSession::EDownloadProgressing &&
            iFotaServer->iPackageState.iResult == RFotaEngineSession::EResUndefinedError)
        {
        notetype = EFwDLGeneralFailure;
        }
    else if (iFotaServer->iPackageState.iResult == RFotaEngineSession::EResFailedSignatureAuthentication
            || iFotaServer->iPackageState.iResult == RFotaEngineSession::EResMalformedOrBadURL
            || iFotaServer->iPackageState.iResult == RFotaEngineSession::EResAlternateDLServerUnavailable
            || iFotaServer->iPackageState.iResult == RFotaEngineSession::EResInvalidDownloadDescriptor
            || iFotaServer->iPackageState.iResult == RFotaEngineSession::EResUndefinedError)
        {
        notetype = EFwDLNonResumableFailure;
        }

    TRAP(err, iFotaServer->FinalizeDownloadL());

    FLOG(_L("Finalized download, error = %d"), err);

    if (notetype != -1)
        {
        TRAP_IGNORE(ShowDialogL((TFwUpdNoteTypes) notetype));
        }

    FLOG(_L("DownloadManagerClient::HandleDownloadEvent <<"));
    }

// --------------------------------------------------------------------------
//  Called by the download client when download is interrupted by fota.
// --------------------------------------------------------------------------
void DownloadManagerClient::HandleClientInterrupt(Download::State dlstate,
        int err0)
    {
    FLOG(_L("DownloadManagerClient::HandleClientInterrupt, status = %d >>"),
            dlstate);

    iDownloadSubmitted = EFalse;
    iProgress = EFalse;
    TInt notetype(-1);

    if (iDownloadClient)
        {
        delete iDownloadClient;
        iDownloadClient = NULL;
        }

    if (dlstate == Download::Cancelled || dlstate == Download::Failed)
        {
        if (err0 == UserCancelled)
            {
            iFotaServer->iPackageState.iState  = RFotaEngineSession::EDownloadFailed;
            iFotaServer->iPackageState.iResult = RFotaEngineSession::EResUserCancelled;
            notetype = EFwDLNonResumableFailure;
            }
        else if (err0 == NeedMoreMemory)
            {
            iFotaServer->iPackageState.iState = RFotaEngineSession::EDownloadProgressing;
            iFotaServer->iPackageState.iResult= RFotaEngineSession::EResUserCancelled;
            }
        else if (err0 == InvalidContentType)
            {
            iFotaServer->iPackageState.iState = RFotaEngineSession::EDownloadFailed;
            iFotaServer->iPackageState.iResult= RFotaEngineSession::EResContentMisMatch;
            notetype = EFwDLNonResumableFailure;
            }
        }
    else if (dlstate == Download::Paused)
        {
        if (err0 == UserCancelled)
            {
            iFotaServer->iPackageState.iState = RFotaEngineSession::EDownloadProgressing;
            iFotaServer->iPackageState.iResult= RFotaEngineSession::EResUserCancelled;
            
            iFotaServer->ServerCanShut(ETrue);
            }
        }
    else
        {
        FLOG(_L(" I DO NOT KNOW THIS!!"));
        }
    TRAPD(err,
            iFotaServer->iDatabase->OpenDBL();
            iFotaServer->iDatabase->SetStateL( iFotaServer->iPackageState, KNullDesC8, EFDBState|EFDBResult);
            iFotaServer->iDatabase->CloseAndCommitDB();
    );
    FLOG(_L("Updating fota database, error = %d"), err);

    UninitDownloadMgr();

    TRAP(err, iFotaServer->FinalizeDownloadL());

    FLOG(_L("Finalizing download, error = %d"), err);

    if (notetype != -1)
        {
        TRAP_IGNORE(ShowDialogL((TFwUpdNoteTypes) notetype));
        }

    FLOG(_L("DownloadManagerClient::HandleClientInterrupt <<"));
    }

// --------------------------------------------------------------------------
//  Called to map the download error codes to fota result codes (FUMO result codes)
// --------------------------------------------------------------------------
void DownloadManagerClient::MapDownloadErrors(int err0)
    {
    FLOG(_L("DownloadManagerClient::MapDownloadErrors, err =  %d >>"), err0);

    if (err0 >= ConnectionRefusedError && err0 <= UnknownNetworkError)
        {
        //Connection related error
        iFotaServer->iPackageState.iResult
                = RFotaEngineSession::EResAlternateDLServerUnavailable;
        }
    else if (err0 == ContentNotFoundError)
        {
        iFotaServer->iPackageState.iResult
                    = RFotaEngineSession::EResAlternateDLServerUnavailable;
        }
    else if (err0 >= ContentAccessDenied && err0 <= UnknownContentError)
        {
        //Content related error
        iFotaServer->iPackageState.iResult
                = RFotaEngineSession::EResUndefinedError;
        }
    else if (err0 >= ProtocolUnknownError && err0 <= ProtocolFailure)
        {
        //Protocol related error
        iFotaServer->iPackageState.iResult
                = RFotaEngineSession::EResUndefinedError;
        }
    else if (err0 >= ConnectionFailed && err0 <= TransactionFailed)
        {
        //No network coverage
        iFotaServer->iPackageState.iResult
                = RFotaEngineSession::EResDLFailDueToNWIssues;
        }
    else if (err0 >= HttpRestartFailed && err0 <= ContentExpired)
        {
        iFotaServer->iPackageState.iResult
                = RFotaEngineSession::EResUndefinedError;
        }
    else if (err0 == ObjectNotFound)
        {
        iFotaServer->iPackageState.iResult
                = RFotaEngineSession::EResAlternateDLServerUnavailable;
        }
    else if (err0 == BadUrl)
        {
        iFotaServer->iPackageState.iResult
                = RFotaEngineSession::EResMalformedOrBadURL;
        }
    else if (err0 == MediaRemoved)
        {
        iFotaServer->iPackageState.iResult
                = RFotaEngineSession::EResDLFailDueToDeviceOOM;
        }
    else if (err0 == DiskFull)
        {
        iFotaServer->iPackageState.iResult
                = RFotaEngineSession::EResDLFailDueToDeviceOOM;
        }
    else if (err0 == InvalidDownloadDescriptor)
        {
        iFotaServer->iPackageState.iResult
                = RFotaEngineSession::EResInvalidDownloadDescriptor;
        }
    else
        {
        //Unknown failure reason
        iFotaServer->iPackageState.iResult
                = RFotaEngineSession::EResUndefinedError;
        }
    FLOG(_L("DownloadManagerClient::MapDownloadErrors, mapped fota failure reason = %d"), iFotaServer->iPackageState.iResult);
    }

// --------------------------------------------------------------------------
//  Tells whether any download is active or not.
// --------------------------------------------------------------------------
TBool DownloadManagerClient::IsDownloadActive()
    {
    return (iDownloadSubmitted || iProgress);
    }

// --------------------------------------------------------------------------
//  Returns the download manager instance pointer
// --------------------------------------------------------------------------
DownloadManager* DownloadManagerClient::Manager()
    {
    return iDownloadManager;
    }

// --------------------------------------------------------------------------
//  Finds the suitable drive and folder for a download, given its size.
// --------------------------------------------------------------------------
TFreeSpace DownloadManagerClient::GetSuitablePath(TInt aSize, QString& aPath)
    {
    TFreeSpace isavailable;
    TInt swupdSize(0);
    TInt dummySize(0);

    SpaceAllocatedBySWUPDFiles(swupdSize, dummySize);

    if (aSize <= dummySize)
        {
        // fits to reservation
        isavailable = EFitsToReservation;
        }
    else
        {
        // doesnt fit to reservation, does it fit to filesystem?
        TInt sizeNeededFromFS = aSize - dummySize;

        if (sizeNeededFromFS < 0)
            sizeNeededFromFS = 0;
        TBool critical(ETrue);
        TRAP_IGNORE(critical = SysUtil::FFSSpaceBelowCriticalLevelL( &iFs, sizeNeededFromFS ));

        if (critical)
            {
            // how much space would be needed
            TVolumeInfo vi;
            iFs.Volume(vi, EDriveC);

            TInt neededspace = sizeNeededFromFS - vi.iFree
                    + KSystemCriticalWorkingspace;
            FLOG(_L("neededspace = %d vi.iFree = %d "), neededspace, vi.iFree);
            FLOG(_L(" neededspace = sizeNeededFromFS - vi.iFree + KSystemCriticalWorkingspace;"));

            aSize = neededspace;
            isavailable = EDoesntFitToFileSystem;
            }
        else
            {
            isavailable = EFitsToFileSystem;
            }

        }
    aPath = DefaultPath;

    FLOG(_L("DownloadManagerClient::IsPackageStoreSizeAvailableL, isavailable = %d <<"), isavailable);

    return isavailable;
    }

// ---------------------------------------------------------------------------
// DownloadManagerClient::GetUpdatePackageLocation
// Gets update package location, that is , path.
// ---------------------------------------------------------------------------
void DownloadManagerClient::GetUpdatePackageLocation(TDes& aPath)
    {
    FLOG(_L("DownloadManagerClient::GetUpdatePackageLocation >>"));

    TBuf<KMaxFileName> temp;
    temp.Zero();
    temp.Copy(KDefaultPath);

    aPath.Zero();

    if (iFotaServer->iPackageState.iState == RFotaEngineSession::EDownloadProgressing
            || iFotaServer->iPackageState.iState == RFotaEngineSession::EDownloadFailed)
        temp.Append(KDownloadTempDir);

    TPtrC name(reinterpret_cast<const unsigned short*> (PackageName.constData()));

    temp.Append(name);

    if (BaflUtils::FileExists(iFs, temp))
        aPath.Copy(temp);

    FLOG(_L("DownloadManagerClient::GetUpdatePackageLocation <<"));
    }

// --------------------------------------------------------------------------
//  Gets the download package size
// --------------------------------------------------------------------------
TInt DownloadManagerClient::GetDownloadPackageSize()
    {
    FLOG(_L("DownloadManagerClient::GetDownloadPackageSize >>"));

    TInt size(0);
    TBuf<KMaxFileName> temp;
    temp.Zero();
    temp.Copy(KDefaultPath);

    if (iFotaServer->iPackageState.iState
            == RFotaEngineSession::EDownloadProgressing)
        temp.Append(KDownloadTempDir);

    TPtrC  name(reinterpret_cast<const unsigned short*> (PackageName.constData()));

    temp.Append(name);

    TEntry entry;
    if (iFs.Entry(temp, entry) == KErrNone)
        size = entry.iSize;

    FLOG(_L("DownloadManagerClient::GetDownloadPackageSize, size = %d >>"),
            size);
    return size;
    }

// --------------------------------------------------------------------------
//  Gets the space allocated for the package and reserved memory 
// --------------------------------------------------------------------------
void DownloadManagerClient::SpaceAllocatedBySWUPDFiles(TInt& aSwupdSize,
        TInt& aReservedSize)
    {
    // get sizes of swupd files
    aSwupdSize = GetDownloadPackageSize();

    // get size of reserved file
    aReservedSize = 0;
    TBuf<KMaxFileName> temp;
    temp.Zero();
    temp.Copy(KDefaultPath);
    temp.Append(KReservedFileName);

    TEntry entry;
    if (iFs.Entry(temp, entry) == KErrNone)
        aReservedSize = entry.iSize;

    FLOG(_L("CFotaDiskStorage::SpaceAllocatedBySWUPDFilesL reserved:%d swupd:%d"), aReservedSize, aSwupdSize);
    }

// --------------------------------------------------------------------------
//  Creates the disk reservation
// --------------------------------------------------------------------------
void DownloadManagerClient::CreateDiskReservation()
    {
    FLOG(_L("DownloadManagerClient::CreateDiskReservationL >>"));

    // Count size reserved by .swupd files
    // CDir*       list;
    TInt err;
    TInt swupdSize(0);
    TInt dummySize(0);
    RFile dummy;

    SpaceAllocatedBySWUPDFiles(swupdSize, dummySize);

    // Calculate space for dummy file
    TInt targetsize = iReservedSize - swupdSize;
    if (targetsize < 0)
        {
        targetsize = 0;
        }

    // Reduce dummy file size 
    if (dummySize != targetsize || dummySize == 0)
        {
        FLOG(_L("   dummy new size %d (old %d)"), targetsize, dummySize);

        err = dummy.Open(iFs, KReservedFileName, EFileWrite
                | EFileShareExclusive);

        if (err == KErrNotFound)
            {
            dummy.Replace(iFs, KReservedFileName, EFileWrite
                    | EFileShareExclusive);
            }

        TInt err = KErrNone;
        //Reservation logic - if 'x' bytes is not available, try x/2 to reserve. 
        //Trial would end if unable to save atleast 1MB.

        do {
            FLOG(_L("Trying to reserve size: %d bytes..."), targetsize);
            err = dummy.SetSize(targetsize);
            if (err != KErrNoMemory)
                break;
            targetsize = targetsize / 2;

            if (targetsize < 1024 * 1024) //Don't reserver anything lesser than 1MB
                break;
            }
        while (err == KErrNoMemory);
        dummy.Close();

        if (err == KErrNone)
            {
            FLOG(_L("Successfully created reservation of size: %d bytes"),
                    targetsize);
            }
        else
            {
            FLOG(_L("Error %d while creating reserved of size:  %d bytes"),
                    err, targetsize);
            }
        }

    FLOG(_L("DownloadManagerClient::CreateDiskReservationL <<"));
    }

// --------------------------------------------------------------------------
//  Deletes the disk reservation in the specified path
// --------------------------------------------------------------------------
void DownloadManagerClient::DeleteDiskReservation(QString& path)
    {
    FLOG(_L("DownloadManagerClient::DeleteDiskReservation >>"));

    RFile file;
    TBuf<KMaxFileName> temp;
    temp.Zero();

    TPtrC spath(reinterpret_cast<const unsigned short*> (path.constData()));
    temp.Copy(spath);
    temp.Append(KReservedFileName);

    TInt err = file.Open(iFs, temp, EFileWrite | EFileShareExclusive);

    if (!err)
        {
        FLOG(_L("Removing the reserved memory as download has started"),
                iReservedSize);
        file.SetSize(KErrNone);
        }

    file.Close();

    FLOG(_L("DownloadManagerClient::DeleteDiskReservation <<"));
    }

// ---------------------------------------------------------------------------
// DownloadManagerClient::DeleteUpdatePackageL 
// ---------------------------------------------------------------------------
void DownloadManagerClient::DeleteUpdatePackageL()
    {
    FLOG(_L("DownloadManagerClient::DeleteUpdatePackageL >>"));

    CDir*       list;
   
    // get sizes of swupd files
    TInt err = iFs.GetDir (KPackageExtensionAll, KEntryAttNormal ,ESortByName, list );
    if (err == KErrNone)
        {
        CleanupStack::PushL ( list );

        for(int i=0; i<list->Count() ;++i )
            {
            TEntry t = (*list)[i];
            iFs.Delete(t.iName);
            FLOG(_L("Deleted: %S"), &t.iName);
            }

        CleanupStack::PopAndDestroy( list );        
        }
    
    TBuf<KMaxFileName> temp;
    temp.Copy(KDefaultPath);
    temp.Append(KDownloadTempDir);
    temp.Append(KPackageExtensionAll);
    
    err = iFs.GetDir (temp, KEntryAttNormal ,ESortByName, list );
    
    if (err == KErrNone)
        {
        CleanupStack::PushL ( list );
        TBuf<KMaxFileName> temp;

        for(int i=0; i<list->Count() ;++i )
            {
            TEntry t = (*list)[i];
            temp.Copy(KDefaultPath);
            temp.Append(KDownloadTempDir);
            temp.Append(t.iName);
            iFs.Delete(temp);
            FLOG(_L("Deleted: %S"), &temp);
            }
        
        CleanupStack::PopAndDestroy( list );
        }
    
    CreateDiskReservation();

    FLOG(_L("DownloadManagerClient::DeleteUpdatePackageL<<"));
    }

// --------------------------------------------------------------------------
//  Called to show any update specific dialogs
// --------------------------------------------------------------------------
void DownloadManagerClient::ShowDialogL(TFwUpdNoteTypes adialogid,
        TInt aValue)
    {
    FLOG(_L("DownloadManagerClient::ShowDialog, dialogid = %d >>"), adialogid);
    iFotaServer->ServerCanShut(EFalse);
    if (iFotaServer->FullScreenDialog())
        iFotaServer->FullScreenDialog()->Close();

    if (iFotaServer->iPackageState.iSessionType && adialogid
            != EFwDLNeedMoreMemory)
        {
        FLOG(_L("Differing showing dialog as session is silent."));
        return;
        }

    iNotifParams = CHbSymbianVariantMap::NewL();

    HBufC* keyDialog = HBufC::NewL(10);
    CleanupStack::PushL(keyDialog);
    *keyDialog = KKeyDialog;

    HBufC* keyParam1 = HBufC::NewL(10);
    CleanupStack::PushL(keyParam1);
    *keyParam1 = KKeyParam1;

    HBufC* keyParam2 = HBufC::NewL(10);
    CleanupStack::PushL(keyParam2);
    *keyParam2 = KKeyParam2;

    HBufC* keyParam3 = HBufC::NewL(10);
    CleanupStack::PushL(keyParam3);
    *keyParam3 = KKeyParam3;

    //adialogid = EFwUpdResumeUpdate;
    CHbSymbianVariant* dialogId = CHbSymbianVariant::NewL(&adialogid,
            CHbSymbianVariant::EInt);
    CleanupStack::PushL(dialogId);
    iNotifParams->Add(*keyDialog, dialogId);
    iNotifier = CFotaDownloadNotifHandler::NewL(this);

    switch (adialogid)
        {
        case EFwDLNeedMoreMemory: //For Flexible memory
            {
            //TFwUpdNoteTypes aType = EFwDLNeedMoreMemory;
            CHbSymbianVariant* param1Val = CHbSymbianVariant::NewL(&aValue,
                    CHbSymbianVariant::EInt);

            iNotifParams->Add(*keyParam1, param1Val);
            iNotifier->LaunchNotifierL(iNotifParams, adialogid);
            }
            break;
        case EFwDLConnectionFailure:
            {
            iNotifier->LaunchNotifierL(iNotifParams, adialogid);
            }
            break;
        case EFwDLGeneralFailure:
            {
            iNotifier->LaunchNotifierL(iNotifParams, adialogid);
            }
            break;
        case EFwDLNonResumableFailure:
             {
             iNotifier->LaunchNotifierL(iNotifParams, adialogid);
             }
            break;
        default:
            {

            }
            break;
        }
    CleanupStack::PopAndDestroy(5);

    FLOG(_L("DownloadManagerClient::ShowDialog <<"));
    }

// --------------------------------------------------------------------------
//  The call back when update specific dialog receives an user response
// --------------------------------------------------------------------------
void DownloadManagerClient::HandleDialogResponse(int response, TInt aDialogId)
    {

    FLOG(_L("DownloadManagerClient::HandleDialogResponse, response = %d, aDialogid = %d >>"), response, aDialogId);
    //TInt dialogID;
    //const CHbSymbianVariant* dialogId = iNotifParams->Get(KKeyDialog);
    //dialogID = *(TFwUpdNoteTypes *)dialogId->Value<TInt>();

    switch (aDialogId)
        {
        case EFwDLNeedMoreMemory: //For Flexible memory
            {
            if (response == EHbLSK) //LSK
                {
                iDownloadClient->Restart();
                }
            else //End key
                {
                iDownloadClient->Cancel(NeedMoreMemory);
                }
            }
            break;
        case EFwDLConnectionFailure:
        case EFwDLGeneralFailure:
        case EFwDLNonResumableFailure:
            {
            iFotaServer->ServerCanShut(ETrue);
            }
            break;
        default:
            {
            iFotaServer->ServerCanShut(ETrue);
            }
            break;
        }

    FLOG(_L("DownloadManagerClient::HandleDialogResponse <<\n"));
    }

//End of file
