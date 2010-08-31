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
 * Description:   Fota server update and download functionality 
 *
 */
// INCLUDE FILES
#include <apgtask.h>
#include <apgwgnam.h>
#include <schtime.h>
#include <csch_cli.h>
#include <e32property.h>
#include <cmmanager.h>
#include <cmconnectionmethod.h> 
#include <DevManInternalCRKeys.h>
#include <nsmlconstants.h>
#include <centralrepository.h>
#include <sysversioninfo.h>
#include <hbstackedlayout.h>
#include <hbmainwindow.h>
#include <hbview.h>
#include <featmgr.h>
#include <DevEncProtectedPSKey.h>
#include <hbapplication.h>
#include <xqconversions.h>
#include <hbtransparentwindow.h>
#include <es_sock.h>
#include <es_enum_internal.h>
#include <es_sock_partner.h>
#include <e32property.h>
#include <xqserviceutil.h>

#include "FotaServer.h"
#include "FotaSrvDebug.h"
#include "FotasrvSession.h"
#include "fotaConst.h"
#include "nsmldmtreedbclient.h"
#include "fotaserverPrivatePSKeys.h"
#include "FotaNetworkRegStatus.h"
#include "FotaDlMgrClient.h"
#include "fotadevicedialogobserver.h"
#include "fotanotifiers.h"
#include "fotaserverPrivatePSKeys.h"

#define __LEAVE_IF_ERROR(x) if(KErrNone!=x) {FLOG(_L("LEAVE in %s: %d"), __FILE__, __LINE__); User::Leave(x); }

TInt CFotaServer::iSessionCount = 0;


// ============================= LOCAL FUNCTIONS =============================

// ---------------------------------------------------------------------------
// GetPredefinedNodeL
// function to get preconfigured node for FUMO
// ---------------------------------------------------------------------------
void GetPredefinedNodeL(TDes8& aNode)
    {
    FLOG(_L("GetPredefinedNodeL() Begin"));
    CRepository* centrep(NULL);
    aNode.Zero();
    centrep = CRepository::NewLC(TUid::Uid(0x101F9A0A));

    if (centrep)
        {
        FLOG(_L("centralrepository found End"));
        TFullName temp;

        if (centrep->Get(KDevManFUMOPredefinedNodes, temp) == KErrNone
                && temp.Length())
            {
            temp.Trim();
            aNode.Copy(temp);
            }
        CleanupStack::PopAndDestroy(centrep);
        }
    FLOG(_L("GetPredefinedNodeL() End"));
    }
// ---------------------------------------------------------------------------
// DeleteFUMOTreeL
// function to get preconfigured node for FUMO
// ---------------------------------------------------------------------------
void DeleteFUMOTreeL()
    {
    FLOG(_L("DeleteFUMOTreeL() Begin"));
    const TUint32 KNSmlFotaAdapterUid = 0x101F9A09;
    _LIT8( KNSmlFumoPath, "FUMO" );
    _LIT8( KNSmlFumoSeparator, "/" );
    const TInt KGranularity = 10;
    TBuf8<KMaxFullName> temp;
    GetPredefinedNodeL(temp);
    RNSmlDMCallbackSession session;
    __LEAVE_IF_ERROR(session.Connect());
    CleanupClosePushL(session);

    CBufBase *emptyList = CBufFlat::NewL(0);
    CleanupStack::PushL(emptyList);
    CArrayFixFlat<TSmlDmMappingInfo>* UriSegList;
    UriSegList = new (ELeave) CArrayFixFlat<TSmlDmMappingInfo> (KGranularity);

    CleanupStack::PushL(UriSegList);

    session.GetUriSegListL(KNSmlFotaAdapterUid, KNSmlFumoPath, *UriSegList);

    for (TUint16 i = 0; i < UriSegList->Count(); i++)
        {
        if (temp.Length() && UriSegList->At(i).iURISeg.Find(temp)
                != KErrNotFound)
            {
            FLOG(_L("DeleteFUMOTreeL predefined match =%S"), &temp);
            continue;
            }

        TBuf8<KMaxFullName> tempFumo;
        tempFumo.Append(KNSmlFumoPath);
        tempFumo.Append(KNSmlFumoSeparator);
        tempFumo.Append(UriSegList->At(i).iURISeg);
        FLOG(_L("DeleteFUMOTreeL() call update mapping info for node %S"), &tempFumo);
        session.UpdateMappingInfoL(KNSmlFotaAdapterUid, tempFumo, *emptyList);

        }
    // session.UpdateMappingInfoL(KNSmlFotaAdapterUid,KNSmlFumoPath,*emptyList);
    CleanupStack::PopAndDestroy(UriSegList);
    CleanupStack::PopAndDestroy(emptyList);
    CleanupStack::PopAndDestroy(&session); //session
    FLOG(_L("DeleteFUMOTreeL() End"));
    }

// ----------------------------------------------------------------------------------------
// CFotaServer::NewInstance
// ----------------------------------------------------------------------------------------
CFotaServer* CFotaServer::NewInstance(HbMainWindow& mainwindow)
    {
    FLOG(_L("CFotaServer::NewInstance >>"));
    
    CFotaServer* self = new CFotaServer(mainwindow);
    
    if (self)
        {
        TRAPD( err, self->ConstructL());
        
        if (err != KErrNone)
            {
            delete self; self = NULL;
            }
        }
    FLOG(_L("CFotaServer::NewInstance >>"));
    return self;
    }

// ----------------------------------------------------------------------------------------
// CFotaSrvSession::NewSessionL() 
// ----------------------------------------------------------------------------------------
CSession2* CFotaServer::NewSessionL(const TVersion&, const RMessage2&) const
    {
    iSessionCount++;
    FLOG(_L("Number of active sessions = %d"), iSessionCount);
    return new (ELeave) CFotaSrvSession();
    }

void CFotaServer::ReportFwUpdateStatusL(TPackageState& aState)
    {
    FLOG(_L("CFotaServer::ReportFwUpdateStatusL >>"));

    TFotaClient requester = GetUpdateRequester();

    if (requester == EDMHostServer)
        {
        FLOG(_L("Reporting status back to hostserver..."));
        ServerCanShut(EFalse);
        CreateDeviceManagementSessionL(aState);
        }
    else
        {
        //iServerCanShut = EFalse; Don't do here. Should be set in downloadmgrcli based on error type.
        //ResetFotaStateL(aState.iPkgId);
        SetStartupReason(EFotaDefault);
        StopServerWhenPossible();
        FLOG(_L("Not reporting status as requester is unknown!"));
        }

    FLOG(_L("CFotaServer::ReportFwUpdateStatusL >>"));
    }

void CFotaServer::StartDownloadDialog(const QString &aName,
        const QString &aVersion, const TInt &aSize)
    {
    FLOG(_L("CFotaServer::StartDownloadDialog >>"));
    //The dialog should not timeout here.
    if (!iFullScreenDialog)
        {
        //ConstructApplicationUI(ETrue);
        iFullScreenDialog = new FotaFullscreenDialog(this);
        }

    iFullScreenDialog->SetSoftwareDetails(aSize, aVersion, aName);

    iFullScreenDialog->SetWarningDetails(EHbFotaDownload);

    TBool postpone = IsUserPostponeAllowed();
    if (!postpone)
        {
        FLOG(_L("Disabling option to resume later!"));
        iFullScreenDialog->DisableRSK(ETrue);
        }
    
    FLOG(_L("CFotaServer::StartDownloadDialog <<"));
    }

void CFotaServer::UpdateDownloadDialog(TInt aProgress)
    {
    FLOG(_L("CFotaServer::UpdateDownloadDialog >>"));
    if (!iFullScreenDialog)
        {
        QString name = XQConversions::s60Desc8ToQString(
                iPackageState.iPkgName);
        QString version = XQConversions::s60Desc8ToQString(
                iPackageState.iPkgVersion);
        StartDownloadDialog(name, version, iPackageState.iPkgSize);
        ConstructApplicationUI(ETrue);
        }

    iFullScreenDialog->UpdateProgressBar(aProgress);
    FLOG(_L("CFotaServer::UpdateDownloadDialog <<"));
    }

void CFotaServer::ShowDialogL(TFwUpdNoteTypes adialogid)
    {
    FLOG(_L("CFotaServer::ShowDialogL  adialogid = %d<<"), adialogid);

    if (iNotifier)
        {
        FLOG(_L("Deleting the open device dialog!!"));
        iNotifier->Cancel();
        }

    ServerCanShut(EFalse);
    iDialogId = (TInt) adialogid;

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

    HBufC* keyParam4 = HBufC::NewL(10);
    CleanupStack::PushL(keyParam4);
    *keyParam4 = KKeyParam4;

    CHbSymbianVariant* dialogId = CHbSymbianVariant::NewL(&adialogid,
            CHbSymbianVariant::EInt);
    CleanupStack::PushL(dialogId);
    iNotifParams->Add(*keyDialog, dialogId);

    if (!iNotifier)
        iNotifier = CFotaDownloadNotifHandler::NewL(this);

    switch (adialogid)
        {
        case EFwUpdNotEnoughBattery:
        case EFwUpdDeviceBusy:
            {
            FLOG(_L("CFotaServer::EFwUpdNotEnoughBattery/EFwUpdDeviceBusy"));
            iNotifier->LaunchNotifierL(iNotifParams, adialogid);
            }
            break;
        case EFwUpdResumeUpdate:
        case EFwUpdResumeDownload:
            {
            FLOG(_L("CFotaServer::EFwUpdResumeUpdate / EFwUpdResumeDownload"));
            CHbSymbianVariant* param1Val = CHbSymbianVariant::NewL(
                    &iPackageState.iPkgSize, CHbSymbianVariant::EInt);
            CleanupStack::PushL(param1Val);
            iNotifParams->Add(*keyParam1, param1Val);
            TBuf16<KFotaMaxPkgNameLength>    temp1;
            temp1.Copy(iPackageState.iPkgVersion);
            CHbSymbianVariant* param2Val = CHbSymbianVariant::NewL(
                    //&iPackageState.iPkgVersion, CHbSymbianVariant::EDes);
                    &temp1, CHbSymbianVariant::EDes);
            CleanupStack::PushL(param2Val);
            iNotifParams->Add(*keyParam2, param2Val);
            TBuf16<KFotaMaxPkgNameLength>    temp2;
            temp2.Copy(iPackageState.iPkgName);
            CHbSymbianVariant* param3Val = CHbSymbianVariant::NewL(
                    &temp2, CHbSymbianVariant::EDes);
            CleanupStack::PushL(param3Val);
            iNotifParams->Add(*keyParam3, param3Val);
            TBool postpone = IsUserPostponeAllowed();
            CHbSymbianVariant* param4Val = CHbSymbianVariant::NewL(&postpone,
                    CHbSymbianVariant::EInt);
            CleanupStack::PushL(param4Val);
            iNotifParams->Add(*keyParam4, param4Val);
            iNotifier->LaunchNotifierL(iNotifParams, adialogid);
            CleanupStack::PopAndDestroy(4);

            }
            break;

        default:
            {
            FLOG(_L("CFotaServer::default"));
            //Do nothing
            }
            break;
        }
    CleanupStack::PopAndDestroy(6);
    FLOG(_L("CFotaServer::ShowDialogL >>"));

    }

void CFotaServer::HandleDialogResponse(int response, TInt aDialogId)
    {
    FLOG( _L("CFotaServer::HandleDialogResponse, response = %d dialog = %d >>"), response, aDialogId);

    switch (aDialogId)
        {
        case EFwUpdNotEnoughBattery:
        case EFwUpdDeviceBusy:
            {
            SetServerActive(EFalse);

            StopServerWhenPossible();
            }
            break;
        case EFwUpdResumeDownload:
            {
            if (response == EHbLSK) //Continue
                {
                FLOG(_L("User accepted to resume the download"));

                TRAP_IGNORE(CancelFmsL());
                TRAP_IGNORE(DownloaderL()->TryResumeDownloadL());
                }
            else //Resume Later
                {
                FLOG(_L("User denied resuming the download"));
                DecrementUserPostponeCount();
                SetServerActive(EFalse);
                SetStartupReason(EFotaDownloadInterrupted);
                
                TRAP_IGNORE(InvokeFmsL());
                
                StopServerWhenPossible();
                }
            }
            break;
        case EFwUpdResumeUpdate:
            {
            if (response == EHbLSK) //Continue
                {
                FLOG(_L("User accepted to resume the update"));
                iPackageState.iResult = KErrNotFound;
                TRAPD(err,
                        iDatabase->OpenDBL();
                        iDatabase->SetStateL( iPackageState ,KNullDesC8,EFDBResult );
                );
                FLOG(_L("Updating the fota database... err = %d"), err);
                iDatabase->CloseAndCommitDB();

                TRAP(err, iUpdater->StartUpdateL( iPackageState ));
                FLOG(_L("Starting update, err = %d"), err);
                }
            else //Update Later
                {
                FLOG(_L("User denied resuming the update"));
                iPackageState.iState = RFotaEngineSession::EStartingUpdate;
                iPackageState.iResult = RFotaEngineSession::EResUserCancelled;
                TRAPD(err,
                        iDatabase->OpenDBL();
                        iDatabase->SetStateL( iPackageState ,KNullDesC8,EFDBState|EFDBResult );
                        iDatabase->CloseAndCommitDB();
                );
                FLOG(_L("Updating the fota database... err = %d"), err);

                DecrementUserPostponeCount();
                SetServerActive(EFalse);
                SetStartupReason(EFotaUpdateInterrupted);

                TRAP(err, InvokeFmsL());
                FLOG(_L("Invoking fms, err = %d"), err);

                StopServerWhenPossible();
                }
            }
            break;
        default:
            {
            //Do nothing
            }
            break;
        }
    /*
     if(iNotifParams)
     {
     delete iNotifParams; iNotifParams = NULL;
     }
     
     if(iNotifier)
     {
     delete iNotifier; iNotifier = NULL;
     }*/
    iDialogId = 0;
    FLOG(_L("CFotaServer::HandleDialogResponse<<"));
    }

void CFotaServer::SetServerActive(TBool aValue)
    {
    FLOG(_L("CFotaServer::SetServerActive, aValue = %d"), aValue);

    TInt err = RProperty::Set(TUid::Uid(KOmaDMAppUid), KFotaServerActive,
            aValue);
    FLOG(_L("RProperty SetServerActive Set %d, err = %d"), aValue, err);

    if (err == KErrNotFound)
        {
        err = RProperty::Define(TUid::Uid(KOmaDMAppUid), KFotaServerActive,
                RProperty::EInt, KReadPolicy, KWritePolicy);
        err = RProperty::Set(TUid::Uid(KOmaDMAppUid), KFotaServerActive,
                aValue);
        FLOG(_L("RProperty SetServerActive Set %d, err = %d"), aValue,
                err);
        }

    FLOG(_L("CFotaServer::SetServerActive <<"));
    }

void CFotaServer::ShowFullScreenDialog(TInt aType)
    {
    FLOG(_L("CFotaServer::ShowFullScreenDialog, type = %d >>"), aType);

    if (!iFullScreenDialog)
        {
        const QString  ver =   QString::fromUtf8( reinterpret_cast<const char*> (iPackageState.iPkgVersion.Ptr()), iPackageState.iPkgVersion.Length());
        const QString name =   QString::fromUtf8( reinterpret_cast<const char*> (iPackageState.iPkgName.Ptr()), iPackageState.iPkgName.Length());
        iFullScreenDialog = new FotaFullscreenDialog(this);

        iFullScreenDialog->SetSoftwareDetails(iPackageState.iPkgSize, ver, name);
        iFullScreenDialog->SetWarningDetails(EHbFotaDownload);
        ConstructApplicationUI(ETrue);
        }

    if (aType == EHbFotaUpdate)
        {
        iFullScreenDialog->UpdateProgressBar(100);
        TBool postpone = IsUserPostponeAllowed();
        if (!postpone)
            {
            FLOG(_L("Disabling option to resume later!"));
            iFullScreenDialog->DisableRSK(ETrue);
            }

        iFullScreenDialog->ShowUpdateDialog();
        }
    else if (aType == EHbFotaLowBattery)
        {
        iFullScreenDialog->UpdateProgressBar(100);
        iFullScreenDialog->DisableRSK(EFalse);
        iFullScreenDialog->SetWarningDetails(EHbFotaLowBattery);
        }

    FLOG(_L("CFotaServer::ShowFullScreenDialog <<"));
    }

// --------------------------------------------------------------------------
// CreateDeviceManagementSessionL       
// Creates DM session 
// --------------------------------------------------------------------------
void CFotaServer::CreateDeviceManagementSessionL(TPackageState& aState)
    {
    FLOG(
            _L("[cfotasever]   CreateDeviceManagementSessionL dms >> profid %d\
    %d counts left ,sml handle %d, iNetworkAvailable=%d"),
            aState.iProfileId, aState.iSmlTryCount, iSyncMLSession.Handle(),
            iNetworkAvailable);

    SetStartupReason(EFotaPendingGenAlert);

    if (!iNetworkAvailable)
        {
        iRetryingGASend = ETrue;
        iPackageState = aState;
        StartNetworkMonitorL();
        }
    else
        {

        TBool dbAlreadyOpen = iDatabase->IsOpen();
        TBool triesLeft(EFalse);

        iNetworkAvailable = EFalse; // to check network again when GA is sent next time

        if (iSyncMLSession.Handle())
            {
            __LEAVE_IF_ERROR( KErrAlreadyExists );
            }

        if (!dbAlreadyOpen)
            iDatabase->OpenDBL();
        // There is still tries left
        if (aState.iSmlTryCount > 0)
            {
            triesLeft = ETrue;
            FLOG(_L(" decrementing the retry count"));
            aState.iSmlTryCount = aState.iSmlTryCount - 1;
            iDatabase->SetStateL(aState, KNullDesC8, EFDBSmlTryCount);
            }
        // out of tries, set state to idle
        else
            {
            triesLeft = EFalse;
            FLOG(_L("   out of tries, resetting pkg state"));

            SetStartupReason(EFotaDefault);
            aState.iState = RFotaEngineSession::EIdle;
            aState.iResult = KErrNotFound;
            iDatabase->SetStateL(aState, KNullDesC8, EFDBState | EFDBResult);
            }
        if (!dbAlreadyOpen)
            iDatabase->CloseAndCommitDB();

        if (triesLeft)
            {

            CRepository *cenrep = CRepository::NewL(
                    KCRUidDeviceManagementInternalKeys);
            TInt x = cenrep->Set(KDevManClientInitiatedFwUpdateId,
                    iPackageState.iProfileId);
            delete cenrep;
            FLOG(_L("Status writing the cenrep for GA: %d"), x);

            iSyncMLSession.OpenL();

            if (aState.iIapId <0)
                aState.iIapId = 0;
            
            FLOG(_L("IAP set in the Fota profile %d is :%d"),
                    aState.iProfileId, aState.iIapId);

            FLOG(_L("From Db Using IAP: %d to send GA"), aState.iIapId);

            RSyncMLDevManJob dmJob;
            TBuf<10> genalertap, temp;
            genalertap.Zero();
            temp.Zero();
            genalertap.Append(KNSmlDMJobIapPrefix);
            temp.Num(aState.iIapId);//Decimal Iap
            if (temp.Length() <= KNSmlHalfTransportIdLength && aState.iIapId
                    > KErrNotFound && CheckIapExistsL(aState.iIapId))
                {
                genalertap.AppendFill('0', KNSmlHalfTransportIdLength
                        - temp.Length());
                genalertap.Append(temp);
                TLex gavalue(genalertap);
                TInt temp2(0);
                gavalue.Val(temp2);
                dmJob.CreateL(iSyncMLSession, aState.iProfileId, temp2);
                iSyncMLAttempts = KSyncmlAttemptCount;
                }
            else
                {
                iSyncMLAttempts = 0;
                dmJob.CreateL(iSyncMLSession, aState.iProfileId);
                }

            // If there's no iapid defined, sml will pop up connection dialog.
            // In that case, only one connection attempt is allowed.
            iSyncJobId = dmJob.Identifier();
            iSyncProfile = aState.iProfileId;
            dmJob.Close();
            iSyncMLSession.RequestEventL(*this);
            }
        }

    FLOG(_L("[cfotasever]   CreateDeviceManagementSessionL dms << profid %d"), aState.iProfileId);
    }

// ============================= MEMBER FUNCTIONS ============================

// --------------------------------------------------------------------------
// CFotaServer::CFotaServer()   
// --------------------------------------------------------------------------
//
CFotaServer::CFotaServer(HbMainWindow& mainwindow) :
    CServer2(EPriorityStandard, EUnsharableSessions) /*CServer2(0)*/,
            iDatabase(0), iInitialized(EFalse), iDownloader(0), iUpdater(0),
            iDownloadFinalizer(0), iUpdateFinalizer(0), iTimedExecuteResultFile(0), iTimedSMLSessionClose(0),
            iAppShutter(0), iMonitor(NULL), iSyncMLAttempts(0), iSyncJobId(-1),iRetryingGASend(EFalse),
            iNetworkAvailable(EFalse),iFullScreenDialog(NULL), iNotifParams(NULL), iNotifier(NULL),
            iServerCanShut(EFalse), iAsyncOperation(EFalse),iDialogId (0), iConstructed(EFalse), iMainwindow(mainwindow)
    {
    RProcess pr;
    TFullName fn = pr.FullName();
    TUint prid = pr.Id();
    FLOG(_L( "CFotaServer::CFotaServer process(id %d)%S. this 0x%x"), prid,
            &fn, this);

    }

// --------------------------------------------------------------------------
// CFotaServer::DoExecuteResultFileL
// Interprets result of update (file update.resp)
// --------------------------------------------------------------------------
//
void CFotaServer::DoExecuteResultFileL()
    {
    FLOG(_L("CFotaServer::DoExecuteResultFileL() >>"));

    if (iTimedExecuteResultFile)
        {
        iTimedExecuteResultFile->Cancel();
        delete iTimedExecuteResultFile;
        iTimedExecuteResultFile = NULL;
        }
    iUpdater = CFotaUpdate::NewL(this);
    iUpdater->ExecuteUpdateResultFileL();
    FLOG(_L("CFotaServer::DoExecuteResultFileL() <<"));
    }

// --------------------------------------------------------------------------
// StaticDoExecuteResultFile
// --------------------------------------------------------------------------
//
static TInt StaticDoExecuteResultFile(TAny *aPtr)
    {
    FLOG(_L("[cfotaserver] StaticDoExecuteResultFile() >>"));
    __ASSERT_ALWAYS( aPtr, User::Panic(KFotaPanic, KErrBadHandle) );
    CFotaServer* srv = (CFotaServer*) aPtr;
    TRAPD( err, srv->DoExecuteResultFileL() );
    if (err)
        {
        FLOG(_L("[cfotaserver] StaticDoExecuteResultFile ERROR %d"), err);
        }

    FLOG(_L("[cfotaserver] StaticDoExecuteResultFile() <<"));
    return err;
    }

// --------------------------------------------------------------------------
// CFotaServer::CFotaServer()   
// Constructor. Can't do all constructing since fotaserver might call itself
// recursively (via downloadmgr). 
// --------------------------------------------------------------------------
//
void CFotaServer::ConstructL()
    {
    FLOG(_L("CFotaServer::ConstructL() >>"));
    TBool updated(EFalse);
    TInt err;
    iLastFwUrl.Zero();
    StartL(KFotaServerName);

    __LEAVE_IF_ERROR(iFs.Connect());

    err = iFs.CreatePrivatePath(EDriveC);
    if (err != KErrNone && err != KErrAlreadyExists)
        {
        __LEAVE_IF_ERROR(err);
        }
    __LEAVE_IF_ERROR(iFs.SetSessionToPrivate(EDriveC));

    if (!iDatabase)
        {
        TRAPD( err,iDatabase = CFotaDB::NewL() );
        if (err)
            {
            FLOG(_L("CFotaServer::  DB creationg error %d"), err);
            __LEAVE_IF_ERROR(err);
            }
        }

    updated = CFotaUpdate::CheckUpdateResults(iFs);

    // Update has happened, and result file is in place
    if (updated)
        {
        FLOG(_L("scheduling update result file execution"));
        ServerCanShut(EFalse);

        if (iTimedExecuteResultFile)
            {
            iTimedExecuteResultFile->Cancel();
            delete iTimedExecuteResultFile;
            iTimedExecuteResultFile = NULL;
            }
        iTimedExecuteResultFile = CPeriodic::NewL(EPriorityNormal);
        iTimedExecuteResultFile->Start(TTimeIntervalMicroSeconds32(
                KDownloadFinalizerWaitTime), TTimeIntervalMicroSeconds32(
                KDownloadFinalizerWaitTime), TCallBack(
                StaticDoExecuteResultFile, this));
        }

    FLOG(_L("CFotaServer::ConstructL() <<"));
    }

// --------------------------------------------------------------------------
// CFotaServer::ClientAwareConstructL
// Does rest of constructing, if not done. If parent of this fotaserver 
// instance is another fotaserver, skip maintenance operations (since parent 
// takes care of them). DmHOstserver triggers cleanup for states: 20,70,80
// ,90,100 , must handle state 60 here
// --------------------------------------------------------------------------
void CFotaServer::ClientAwareConstructL(const RMessage2 &aMessage)
    {
    FLOG(_L("CFotaServer::ClientAwareConstructL >>"));
    if (iInitialized)
        return;

    ConstructApplicationUI(EFalse);
    CreateDiskReservation();

    TFotaClient client = CFotaSrvSession::CheckClientSecureIdL(aMessage);

    RArray<TInt> states;
    TPackageState state;
    CleanupClosePushL(states);
    iDatabase->OpenDBL();
    iDatabase->GetAllL(states);
    TBool dlactive(EFalse);

    dlactive = DownloaderL()->IsDownloadActive();

    FLOG(_L("Download active value is:%d "), (TInt) dlactive);
    // Loop states. 
    for (TInt i = 0; i < states.Count(); ++i)
        {
        TPackageState tmp;

        tmp = iDatabase->GetStateL(states[i],iLastFwUrl);
        FLOG(_L("   1 got state id:%d state:%d result:%d"), tmp.iPkgId,
                tmp.iState, tmp.iResult);

        //Download was started earlier and was interrupted.
        if (tmp.iState == RFotaEngineSession::EStartingUpdate || tmp.iState
                == RFotaEngineSession::EDownloadProgressing || tmp.iState
                == RFotaEngineSession::EDownloadProgressing)

            {
            TBool ispkgvalid = ETrue;
            //Software version check from the time download started.
            TRAPD(err1,ispkgvalid= CheckSWVersionL() )
            if (err1 == KErrNone && !ispkgvalid)
                {
                FLOG(_L("Mismatch in software version  since the download started! Hence resetting!"));

                DownloaderL()->DeleteUpdatePackageL();

                tmp.iState = RFotaEngineSession::EUpdateFailed;
                tmp.iResult = RFotaEngineSession::EResPackageMismatch;
                iDatabase->SetStateL(tmp, KNullDesC8, EFDBState | EFDBResult);

                }

            }
        // Update has been started (60)
        // If there is no result file, means that update agent failed
        // to run. Must get back to 50 to allow user to try again. 
        if (tmp.iState == RFotaEngineSession::EUpdateProgressing
                && iTimedExecuteResultFile == NULL)
            {
            FLOG(_L("     State EUpdateProgressing found, UA didnt run! id %d "), tmp.iPkgId);
            
            tmp.iState = RFotaEngineSession::EStartingUpdate;
            iDatabase->SetStateL(tmp, KNullDesC8, EFDBState);
            }

        // These states need must trigger generic alert! (70+ )
        if (tmp.iState >= RFotaEngineSession::EUpdateFailed && client != EDMHostServer)
            {
            if (iSyncMLSession.Handle() == NULL)
                {
                iDatabase->CloseAndCommitDB();
                ReportFwUpdateStatusL(tmp);
                iDatabase->OpenDBL();
                }
            }

        if (tmp.iState == RFotaEngineSession::EDownloadComplete
                && tmp.iResult == RFotaEngineSession::EResSuccessful && client != EDMHostServer)
            {
            ReportFwUpdateStatusL(tmp);
            }

        if (dlactive == EFalse) //if download is not active, EStartingDownload should be reset to EDownloadFailed
            {
            if (tmp.iState == RFotaEngineSession::EStartingDownload)
                {
                FLOG(_L("Resetting state %d to 20..."), tmp.iState);
                tmp.iState = RFotaEngineSession::EDownloadFailed;
                iDatabase->SetStateL(tmp, KNullDesC8, EFDBState);
                iDatabase->CloseAndCommitDB();
                iDatabase->OpenDBL();
                }
            else if (tmp.iState == RFotaEngineSession::EStartingDownload)
                {
                FLOG(_L("Resetting state %d to 30..."), tmp.iState);
                tmp.iState = RFotaEngineSession::EDownloadProgressing;
                iDatabase->SetStateL(tmp, KNullDesC8, EFDBState);
                iDatabase->CloseAndCommitDB();
                iDatabase->OpenDBL();
                }
            }
        if (tmp.iState == RFotaEngineSession::EDownloadProgressing)
            {
            FLOG(_L("Firmware update state is EDownloadProgressing"));
            //FMS will start fota server when it is appropriate to resume download.
            }
        else if (tmp.iState == RFotaEngineSession::EStartingUpdate)
            {
            FLOG(_L("Firmware update state is EStartingUpdate"));
            //FMS will start fota server when it is appropriate to start install.
            }
        if (tmp.iState == RFotaEngineSession::EDownloadFailed && client != EDMHostServer)
            {
            FLOG(_L("Resetting state %d to 20..."), tmp.iState);
            DownloaderL()->DeleteUpdatePackageL();
            tmp.iResult = RFotaEngineSession::EResDLFailDueToNWIssues;
            iDatabase->SetStateL(tmp, KNullDesC8, EFDBResult);
            iDatabase->CloseAndCommitDB();
            ReportFwUpdateStatusL(tmp);
            iDatabase->OpenDBL();
            }

        }
    iDatabase->CloseAndCommitDB();
    CleanupStack::PopAndDestroy(&states);

    iInitialized = ETrue;
    FLOG(_L("CFotaServer::ClientAwareConstructL <<"));
    }

// --------------------------------------------------------------------------
// StaticDoFinalizeDownload
// Intermediate function
// --------------------------------------------------------------------------
static TInt StaticDoFinalizeDownload(TAny *aPtr)
    {
    FLOG(_L("[cfotaserver] StaticDoFinalizeDownload() >>"));
    CFotaServer* srv = (CFotaServer*) aPtr;
    TRAPD( err, srv->DoFinalizeDownloadL() );
    if (err)
        {
        FLOG(_L("[cfotaserver] StaticDoFinalizeDownload ERROR %d"), err);
        }

    FLOG(_L("[cfotaserver] StaticDoFinalizeDownload() <<"));
    return err;
    }

// --------------------------------------------------------------------------
// CFotaServer::FinalizeDownload
// Initialize finalization of download
// --------------------------------------------------------------------------
//
void CFotaServer::FinalizeDownloadL()
    {
    FLOG(_L("CFotaServer::FinalizeDownload() >> state:%d result:%d"),
            iPackageState.iState, iPackageState.iResult);
    __ASSERT_ALWAYS( iDownloader, User::Panic(KFotaPanic, KErrBadHandle ));

    if (iDownloadFinalizer)
        {
        iDownloadFinalizer->Cancel();
        delete iDownloadFinalizer;
        iDownloadFinalizer = NULL;
        }
    iDownloadFinalizer = CPeriodic::NewL(EPriorityMuchMore);

    // Not restarting,quick finalize
    iDownloadFinalizer->Start(TTimeIntervalMicroSeconds32(
            KDownloadFinalizerWaitTime), TTimeIntervalMicroSeconds32(
            KDownloadFinalizerWaitTime), TCallBack(StaticDoFinalizeDownload,
            this));
    FLOG(_L("CFotaServer::FinalizeDownload() <<"));
    }

// --------------------------------------------------------------------------
// CFotaServer::DoFinalizeDownloadL
// Finalize download. Free resources
// --------------------------------------------------------------------------
//
void CFotaServer::DoFinalizeDownloadL()
    {

    FLOG(_L("CFotaServer::DoFinalizeDownloadL() >> state:%d result:%d"),
            iPackageState.iState, iPackageState.iResult);
    __ASSERT_ALWAYS( iDownloader, User::Panic(KFotaPanic, KErrBadHandle ));


    if (iDownloadFinalizer)
        {
        iDownloadFinalizer->Cancel();
        delete iDownloadFinalizer;
        iDownloadFinalizer = NULL;
        }

    // Set downloader's ending state to DB
    iDatabase->OpenDBL();
    iDatabase->SetStateL(iPackageState, KNullDesC8, EFDBState | EFDBResult);
    iDatabase->CloseAndCommitDB();

    if (iPackageState.iResult != RFotaEngineSession::EResDLFailDueToDeviceOOM)
        {
        FLOG(_L("Adjusting the reserved memory..."));
        DownloaderL()->CreateDiskReservation();
        }

    // Initiate update 
    if (iPackageState.iState == RFotaEngineSession::EDownloadComplete
            && iPackageState.iUpdateLtr)
        {
        TRAP_IGNORE(TryUpdateL(EOMADMAppUi));

        }
    else if (iPackageState.iState == RFotaEngineSession::EDownloadProgressing)
        {
        FLOG(_L("Download has paused due to an error. Invoking FMS..."));
        if (iFullScreenDialog)
            {
            iFullScreenDialog->Close();
            iFullScreenDialog->deleteLater();
            iFullScreenDialog = NULL;
            }
        SetStartupReason(EFotaDownloadInterrupted);
        InvokeFmsL();
        StopServerWhenPossible();
        SetServerActive(EFalse);
        iAsyncOperation = EFalse;
        }
    else
        {
        if (iFullScreenDialog)
            {
            iFullScreenDialog->Close();
            iFullScreenDialog->deleteLater();
            iFullScreenDialog = NULL;
            }
        ConstructApplicationUI(EFalse);
        ReportFwUpdateStatusL(iPackageState);
        SetServerActive(EFalse);
        iAsyncOperation = EFalse;
        }

    FLOG(_L("CFotaServer::DoFinalizeDownloadL() <<"));
    }

// --------------------------------------------------------------------------
// StaticDoCloseSMLSession 
// Intermediate function
// --------------------------------------------------------------------------
static TInt StaticDoCloseSMLSession(TAny *aPtr)
    {
    FLOG(_L("[cfotaserver] StaticDoCloseSMLSession() >>"));
    CFotaServer* srv = (CFotaServer*) aPtr;
    TRAPD( err, srv->DoCloseSMLSessionL() );
    if (err)
        {
        FLOG(_L("[cfotaserver] StaticDoCloseSMLSession ERROR %d"), err);
        }

    FLOG(_L("[cfotaserver] StaticDoCloseSMLSession() <<"));
    return err;
    }

// --------------------------------------------------------------------------
// CFotaServer::DoCloseSMLSessionL
// Close syncml session, or resync
// --------------------------------------------------------------------------
//
void CFotaServer::DoCloseSMLSessionL()
    {
    FLOG(_L("CFotaServer::DoCloseSMLSessionL() >>"));

    // Must still try to sync
    if (iSyncMLAttempts > 0)
        {
        FLOG(_L("   trycount %d => creating new job"), iSyncMLAttempts);
        RSyncMLDevManJob dmJob;
        if (iPackageState.iIapId > KErrNotFound)
            {
            FLOG(_L("DoCloseSMLSessionL new job uses iap from fotadb %d"),
                    iPackageState.iIapId);
            dmJob.CreateL(iSyncMLSession, iSyncProfile, iPackageState.iIapId);
            }
        else
            {
            FLOG(_L("DoCloseSMLSessionL new job uses iap from profile"));
            dmJob.CreateL(iSyncMLSession, iSyncProfile);
            }
        iSyncMLAttempts--;
        iSyncJobId = dmJob.Identifier();
        dmJob.Close();
        }
    else
    // We ran out of attempts, close sml
        {
        if (iSyncMLSession.Handle())
            {
            FLOG(_L("   Closing syncml session"));
            iSyncMLSession.CancelEvent();
            iSyncMLSession.Close();
            }
        }

    if (iTimedSMLSessionClose)
        {
        FLOG(_L("   closing smlsession timer"));
        iTimedSMLSessionClose->Cancel();
        delete iTimedSMLSessionClose;
        iTimedSMLSessionClose = NULL;
        }

    FLOG(_L("CFotaServer::DoCloseSMLSessionL() <<"));
    }

// --------------------------------------------------------------------------
// CFotaServer::TryResumeDownloadL 
// Tries to resume the download operation
// --------------------------------------------------------------------------
//
void CFotaServer::TryResumeDownloadL(TFotaClient aClient, TBool aSilentDl)
    {
    FLOG(_L("CFotaServer::TryResumeDownloadL, client = %d aSilent = %d  >>"),
            (TInt) aClient, aSilentDl);
    //Check whether there is a paused resume actually.
    iAsyncOperation = ETrue;
    SetServerActive(ETrue);
    WakeupServer();

    if (DownloaderL()->IsDownloadActive())
        {
        FLOG(_L("Another download is already active, hence returning..."));
        __LEAVE_IF_ERROR (KErrAlreadyExists);
        }

    if (iPackageState.iPkgId == KErrNotFound)
        iPackageState = GetStateL(-1); //Gets the state of the current/last fota download

    FLOG(_L("Session type is =%d  "), iPackageState.iSessionType);

    if (iPackageState.iState != RFotaEngineSession::EDownloadProgressing)
        {
        FLOG(_L("There are no paused downloads currently; hence leaving with KErrNotFound..."));
        SetServerActive(EFalse);
        __LEAVE_IF_ERROR (KErrNotFound);
        }

    //Resume download now

    iPackageState.iSessionType = aSilentDl;

    if (aSilentDl && iPackageState.iResult
            == RFotaEngineSession::EResUserCancelled)
        {
        //If user has paused download earlier, then resume should be non-silent.
        FLOG(_L("Converting to non-silent download as user had paused it earlier!"));
        iPackageState.iSessionType = EFalse;
        }
    
    TRAP_IGNORE(SetIapToUseL());
    FLOG(_L("Using IAP = %d for the download"), iPackageState.iIapId);
    
    FLOG(_L("Session type = %d"), iPackageState.iSessionType);
    iDatabase->OpenDBL();
    iDatabase->SetStateL(iPackageState, KNullDesC8, EFDBIapId||EFDBSessionType);
    iDatabase->CloseAndCommitDB();

    if (iPackageState.iSessionType || aClient == EOMADMAppUi)
        {
        ConstructApplicationUI(EFalse);
        DownloaderL()->TryResumeDownloadL();
        }
    else
        {
        ShowDialogL(EFwUpdResumeDownload);
        }

    FLOG(_L("CFotaServer::TryResumeDownloadL  <<"));
    }

void CFotaServer::PauseDownloadL()
    {

    if (DownloaderL()->IsDownloadActive())
        {
        DownloaderL()->PauseDownloadL();
        }
    else
        {
        FLOG(_L("No download is active. Hence leaving with KErrNotFound"));
        __LEAVE_IF_ERROR(KErrNotFound);
        }
    }
// --------------------------------------------------------------------------
// CFotaServer::InvokeFmsL 
// Starts Fota Monitory Service with relevant parameters for monitoring.
// --------------------------------------------------------------------------
//
void CFotaServer::InvokeFmsL()
    {
    FLOG(_L("CFotaServer::InvokeFmsL  >>"));
    //Collect all information needed to invoke FMS.
    CancelFmsL();

    FLOG(_L("State as recorded in fota db:"));
    FLOG(_L("iPkgId = %d"), iPackageState.iPkgId);
    FLOG(_L("iProfileId = %d"), iPackageState.iProfileId);
    FLOG(_L("iPkgName = %S"), &iPackageState.iPkgName);
    FLOG(_L("iPkgVersion = %S"), &iPackageState.iPkgVersion);
    FLOG(_L("iSendAlert = %d"), iPackageState.iSendAlert);
    FLOG(_L("iIapId = %d"), iPackageState.iIapId);
    FLOG(_L("iPkgSize = %d"), iPackageState.iPkgSize);
    FLOG(_L("iSessionType = %d"), iPackageState.iSessionType);
    FLOG(_L("iState = %d"), iPackageState.iState);
    FLOG(_L("iResult = %d"), iPackageState.iResult);

    if (iPackageState.iState == RFotaEngineSession::EDownloadProgressing)
        {
        //Finding the reason for download interrupt
        TFmsIpcCommands reason(EDLGeneralInterrupt); // 13

        switch (iPackageState.iResult)
            {
            case RFotaEngineSession::EResUserCancelled:
                {
                reason = EDLUserInterrupt; //10
                break;
                }
            case RFotaEngineSession::EResDLFailDueToNWIssues:
                {
                reason = EDLNetworkInterrupt; //11
                break;
                }
            case RFotaEngineSession::EResDLFailDueToDeviceOOM:
                {
                reason = EDLMemoryInterrupt; //12
                break;
                }
            default:
                {
                //reason is already EGeneralInterrupt
                break;
                }
            }

        //Finding the drive number
        TBuf<KMaxPath> path;
        path.Zero();
        DownloaderL()->GetUpdatePackageLocation(path);

        TInt drive(EDriveC); //Default drive is Phone Memory
        TParse p;
        if (path.Length() && !p.Set(path, NULL, NULL))
            {
            TDriveName drivename(p.Drive());
            TDriveUnit driveunit(drivename);
            if (iFs.IsValidDrive((TInt) driveunit)) //some crash here
                drive = driveunit;
            }
        else
            {
            FLOG(_L("Error while parsing for drive number! defaulting to Phone Memory (C)"));
            }

        TInt dlsize = DownloaderL()->GetDownloadPackageSize();

        TInt neededsize = iPackageState.iPkgSize - dlsize;

        if (neededsize < 0)
            neededsize = 0;

        FLOG(_L("Launching FMS with params... reason = %d, iapid = %d, drive = %d, neededsize = %d"),
                reason, iPackageState.iIapId, drive, neededsize);
        iFMSClient.OpenL();
        iFMSClient.NotifyForResumeL(reason, iPackageState.iIapId,
                (TDriveNumber) drive, neededsize);
        iFMSClient.Close();
        }
    else if (iPackageState.iState == RFotaEngineSession::EStartingUpdate)
        {
        //Finding the reason for update interrupt
        TFmsIpcCommands reason(ENoInterrupt);

        switch (iPackageState.iResult)
            {
            case RFotaEngineSession::EResUserCancelled:
                {
                reason = EDLUserInterrupt;
                }
                break;
            case RFotaEngineSession::EResLowBattery:
                {
                reason = EUpdMonitorbattery;
                }
            default:
                {
                break;
                }
            }
        FLOG(_L("Launching FMS with params... reason = %d, iapid = %d"), reason, iPackageState.iIapId);
        iFMSClient.OpenL();
        iFMSClient.NotifyForUpdateL(reason);
        iFMSClient.Close();

        }
    FLOG(_L("CFotaServer::InvokeFmsL  <<"));
    }

// --------------------------------------------------------------------------
// CFotaServer::CancelFmsL 
// Cancels any outstanding monitoring requests in Fota Monitory Service
// --------------------------------------------------------------------------
//
void CFotaServer::CancelFmsL()
    {
    FLOG(_L("CFotaServer::CancelFmsL  >>"));

    iFMSClient.OpenL();
    iFMSClient.Cancel();
    iFMSClient.Close();

    FLOG(_L("CFotaServer::CancelFmsL  <<"));
    }

// --------------------------------------------------------------------------
// CFotaServer::SetStartupReason 
// Sets the startup reason for Fota. This is used in Fota Startup pluggin.
// --------------------------------------------------------------------------
//
void CFotaServer::SetStartupReason(TInt aReason)
    {
    FLOG(_L("CFotaServer::SetStartupReason, aReason = %d  >>"), aReason);

    CRepository* centrep = NULL;
    TRAPD( err, centrep = CRepository::NewL( KCRUidFotaServer ) );
    if (err == KErrNone)
        {
        err = centrep->Set(KFotaUpdateState, aReason);
        }
    delete centrep;
    centrep = NULL;

    FLOG(_L("CFotaServer::SetStartupReason, err = %d  <<"), err);
    }

void CFotaServer::SetUpdateRequester(TFotaClient aRequester)
    {
    FLOG(_L("CFotaServer::SetUpdateRequester, requester = %d  >>"),
            aRequester);

    CRepository* centrep = NULL;
    TRAPD( err, centrep = CRepository::NewL( KCRUidFotaServer ) )
    if (err == KErrNone)
        {
        err = centrep->Set(KUpdateRequesterUid, (TInt) aRequester);
        }
    delete centrep;
    centrep = NULL;

    FLOG(_L("CFotaServer::SetUpdateRequester, err = %d  <<"), err);
    }

TFotaClient CFotaServer::GetUpdateRequester()
    {
    FLOG(_L("CFotaServer::GetUpdateRequester >>"));

    TInt ret(EUnknown);

    CRepository* centrep = NULL;
    TRAPD( err, centrep = CRepository::NewL( KCRUidFotaServer ) );
    if (err == KErrNone)
        {
        err = centrep->Get(KUpdateRequesterUid, ret);
        }
    delete centrep;
    centrep = NULL;

    FLOG(_L("CFotaServer::GetUpdateRequester, requester = %d, err = %d  <<"),
            ret, err);

    return (TFotaClient) ret;
    }

// --------------------------------------------------------------------------
// CFotaServer::~CFotaServer()
// Frees database, download, chunk, filewriter, etc resources
// --------------------------------------------------------------------------
//
CFotaServer::~CFotaServer()
    {
    FLOG(_L("CFotaServer::~CFotaServer  >>"));

    if (iDatabase)
        {
        iDatabase->CloseAndCommitDB();
        delete iDatabase;
        iDatabase = NULL;
        }

    if (iUpdater)
        {
        delete iUpdater;
        iUpdater = NULL;
        }

    if (iDownloadFinalizer)
        {
        iDownloadFinalizer->Cancel();
        delete iDownloadFinalizer;
        iDownloadFinalizer = NULL;
        }

    if (iUpdateFinalizer)
        {
        iUpdateFinalizer->Cancel();
        delete iUpdateFinalizer;
        iUpdateFinalizer = NULL;
        }

    if (iTimedExecuteResultFile)
        {
        iTimedExecuteResultFile->Cancel();
        delete iTimedExecuteResultFile;
        iTimedExecuteResultFile = NULL;
        }

    if (iTimedSMLSessionClose)
        {
        iTimedSMLSessionClose->Cancel();
        delete iTimedSMLSessionClose;
        iTimedSMLSessionClose = NULL;
        }

    if (iFs.Handle())
        iFs.Close();

    if (iSyncMLSession.Handle())
        iSyncMLSession.Close();

    if (iMonitor)
        {
        delete iMonitor;
        iMonitor = NULL;
        }

    if (iFMSClient.Handle())
        iFMSClient.Close();

    if (iDownloader)
        {
        delete iDownloader;
        iDownloader = NULL;
        }

    if (iFullScreenDialog)
        {
        iFullScreenDialog->deleteLater();
        }

		/*
    if (iNotifParams)
        {
        delete iNotifParams;
        iNotifParams = NULL;
        }

    if (iNotifier)
        {
        delete iNotifier;
        iNotifier = NULL;
        }*/

    FLOG(_L("CFotaServer::~CFotaServer  <<"));
    }

// ---------------------------------------------------------------------------
// CFotaServer::DeletePackageL 
// Deletes update package from db
// ---------------------------------------------------------------------------
void CFotaServer::DeletePackageL(const TInt aPkgId)
    {
    FLOG(_L("CFotaServer::DeletePackageL  >> id %d"), aPkgId);

    DownloaderL()->DeleteUpdatePackageL();

    FLOG(_L("CFotaServer::DeletePackageL  <<"));
    }

// ---------------------------------------------------------------------------
//  CFotaServer::DownloadL
//  Create package downloader and download update package.
// ---------------------------------------------------------------------------
void CFotaServer::DownloadL(TDownloadIPCParams aParams,
        const TDesC8& aPkgURL, TFotaClient aRequester, TBool aSilent,
        TBool aUpdateLtr)
    {
    FLOG(_L("[FotaServer]  Download >>"));
    iAsyncOperation = ETrue;
    SetServerActive(ETrue);
    WakeupServer();


    if (DownloaderL()->IsDownloadActive())
        {
        FLOG(_L("One download is already active, hence leaving!"));
        __LEAVE_IF_ERROR(KErrAlreadyExists);
        }

    TBuf<KSysVersionInfoTextLength> temp;
    if (GetSoftwareVersion(temp) == KErrNone)
        {

        RFileWriteStream wstr;
        CleanupClosePushL(wstr);
        TInt err1 = wstr.Replace(iFs, KSWversionFile, EFileWrite);
        if (err1 == KErrNone)
            {
            HBufC16* swv;
            swv = HBufC16::NewLC(temp.Length());
            swv->Des().Copy(temp);
            wstr.WriteInt16L(swv->Des().Length()); // length  
            wstr.WriteL(swv->Des());
            wstr.WriteInt16L(0);
            CleanupStack::PopAndDestroy(swv);

            }

        CleanupStack::PopAndDestroy(&wstr); // wstr      
        }

    iPackageState = aParams;
    iPackageState.iUpdateLtr = aUpdateLtr;
    iPackageState.iSessionType = aSilent;
    iPackageState.iIapId = -2; //Signifies default connection to use.
    TRAP_IGNORE(SetIapToUseL());
    FLOG(_L("Using IAP = %d for the download"), iPackageState.iIapId);

    //First entry to fota database
    FLOG(_L("DownloadManagerClient::DownloadL, State 1 - writing to database"));

    iDatabase->OpenDBL();
    iDatabase->SetStateL(iPackageState, aPkgURL, EFDBState | EFDBResult
            | EFDBProfileId | EFDBPkgUrl | EFDBPkgName | EFDBVersion
            | EFDBUpdateLtr | EFDBSessionType | EFDBIapId);
    iDatabase->CloseAndCommitDB();

    //Cancel any outstanding requests to monitor.
    CancelFmsL();
    SetUpdateRequester(aRequester);
    ResetCounters();
    
    DownloaderL()->DownloadL(aPkgURL);


    FLOG(_L("[FotaServer]  Download <<"));
    }

// --------------------------------------------------------------------------
// CFotaServer::UpdateL
// Start update 
// --------------------------------------------------------------------------
//
void CFotaServer::TryUpdateL(TFotaClient aClient)
    {
    FLOG(_L("CFotaServer::TryUpdateL, client = %d  >>"), (TInt) aClient);
    iAsyncOperation = ETrue;
    SetServerActive(ETrue);
    WakeupServer();

    TBool isPkgvalid(ETrue);
    isPkgvalid = CheckSWVersionL();

    if (!isPkgvalid)
        {
        FLOG( _L("Fota Update:: Firmware version mismatch! Resetting fota state"));

        ResetFotaStateToFailL();
        SetServerActive(EFalse);
        __LEAVE_IF_ERROR(KErrNotFound);
        }
    
    if (iPackageState.iPkgId == KErrNotFound)
        iPackageState = GetStateL(-1);
    
    iPackageState.iState = RFotaEngineSession::EStartingUpdate;
    iPackageState.iResult = KErrNotFound;
    iPackageState.iSendAlert = EFalse;

    iDatabase->OpenDBL();
    iDatabase->SetStateL(iPackageState, KNullDesC8, EFDBState | EFDBResult);
    iDatabase->CloseAndCommitDB();

    SetStartupReason(EFotaUpdateInterrupted);

    if (IsDeviceDriveBusyL())
        {
        FLOG(
                _L("Fota Update:: Device encryption is onging, hence aborting update!"));
        ShowDialogL(EFwUpdDeviceBusy);
        
        __LEAVE_IF_ERROR(KErrNotReady);
        }

    FLOG(_L("Fota Update:: Firmware version check okie"));

    if (!iUpdater)
        {
        iUpdater = CFotaUpdate::NewL(this);
        }

    if (aClient != EFMSServer)
        {
        //Check any active phone call
        TInt callactive(EFalse);
        iFMSClient.OpenL();
        TInt err1 = iFMSClient.IsPhoneCallActive(callactive);

        if (callactive)
            {
            FLOG(_L("Fota Update:: Active call found; differing showing the install dialog!"));

            iPackageState.iResult = RFotaEngineSession::EResUpdateFailed;
            iDatabase->OpenDBL();
            iDatabase->SetStateL(iPackageState, KNullDesC8, EFDBResult);
            iDatabase->CloseAndCommitDB();

            iFMSClient.NotifyForUpdateL(EUpdMonitorPhoneCallEnd);
            iFMSClient.Close();
            SetServerActive(EFalse);
            __LEAVE_IF_ERROR(KErrNotReady);
            }
        iFMSClient.Close();

        FLOG(_L("Fota Update:: Active phone call check okie "));

        TBool batt = iUpdater->CheckBatteryL();

#if defined (__WINS__)
        batt = ETrue;
#endif

        if (aClient == EOMADMAppUi || aClient == EDMHostServer)
            {
            FLOG(_L("Device Updates/Adapter. Show Full screen dialog."));
            LoopBatteryCheckL(batt);
            }
        else
            {
            if (batt)
                {
                FLOG(_L("Not Device Updates. Show device dialog."));

                ShowDialogL(EFwUpdResumeUpdate);
                }
            else
                {
                FLOG(_L("Not Device Updates. Leave and monitor for battery."));

                iPackageState.iState = RFotaEngineSession::EStartingUpdate;
                iPackageState.iResult = RFotaEngineSession::EResLowBattery;

                iDatabase->OpenDBL();
                iDatabase->SetStateL(iPackageState, KNullDesC8, EFDBState
                        | EFDBResult);
                iDatabase->CloseAndCommitDB();

                InvokeFmsL();

                __LEAVE_IF_ERROR(KErrBadPower);
                }
            }
        }
    else
        {
        ShowDialogL(EFwUpdResumeUpdate);
        }
    FLOG(_L("CFotaServer::TryUpdateL  <<"));
    }

void CFotaServer::LoopBatteryCheckL(TBool aBatteryLevel)
    {
    FLOG(_L("CFotaServer::LoopBatteryCheckL, level = %d"), aBatteryLevel);
    if (aBatteryLevel)
        {
        FLOG(_L("Fota Update:: Battery check success;  monitoring battery until update"));
        ShowFullScreenDialog(EHbFotaUpdate);
        ConstructApplicationUI(ETrue);
        iUpdater->MonitorBatteryChargeLevel();
        }
    else
        {
        FLOG(_L("Fota Update:: Battery check failed;  monitoring for charger connection"));

        iPackageState.iState = RFotaEngineSession::EStartingUpdate;
        iPackageState.iResult = RFotaEngineSession::EResLowBattery;
        iDatabase->OpenDBL();
        iDatabase->SetStateL(iPackageState, KNullDesC8, EFDBResult);
        iDatabase->CloseAndCommitDB();

        ShowFullScreenDialog(EHbFotaLowBattery);
        ConstructApplicationUI(ETrue);
        iUpdater->MonitorBatteryChargingStatus();
        }
    FLOG(_L("CFotaServer::LoopBatteryCheckL <<"));
    }


void CFotaServer::FinalizeUpdate()
    {
    FLOG(_L("CFotaServer::FinalizeUpdate >>"));
    
    if (iPackageState.iResult == RFotaEngineSession::EResLowBattery)
        {
        TRAP_IGNORE(InvokeFmsL());
        }
    SetServerActive(EFalse);
    ServerCanShut(ETrue);
    StopServerWhenPossible();
    FLOG(_L("CFotaServer::FinalizeUpdate <<"));
    }


FotaFullscreenDialog* CFotaServer::FullScreenDialog()
    {
    return iFullScreenDialog;
    }

void CFotaServer::HandleFullScreenDialogResponse(TInt aResponse)
    {
    FLOG(_L("CFotaServer::HandleFullScreenDialogResponse, response = %d >>"),
            aResponse);

    TBool active (EFalse);
    TRAP_IGNORE(active = DownloaderL()->IsDownloadActive());
    CEikonEnv* env = CEikonEnv::Static();

    if (iPackageState.iState == RFotaEngineSession::EDownloadProgressing)
        {

        //swapProcess(EFalse);
        ConstructApplicationUI(EFalse);
        if (aResponse == EHbLSK)
            {
            //HIDE is pressed
            FLOG(_L("HIDE is pressed"));
            if (env)
                {
                CApaWindowGroupName* wgName (NULL);
                TRAP_IGNORE( wgName = CApaWindowGroupName::NewL(env->WsSession()));
                if (wgName)
                    {
                    wgName->SetHidden(EFalse); // hides us from FSW and protects us from OOM FW etc.
                    delete wgName;
                    }
                }
            }
        else
            {
            DecrementUserPostponeCount();
            if (active)
                {
                FLOG(_L("CONTINUE LATER is pressed on update dialog"));
                /*
                if (env)
                    {
                    CApaWindowGroupName* wgName;
                    TRAP_IGNORE(wgName = CApaWindowGroupName::NewL(env->WsSession()));
                    if (wgName)
                        {
                        wgName->SetHidden(ETrue); // hides us from FSW and protects us from OOM FW etc.
                        delete wgName;
                        }
                    }*/
                TRAP_IGNORE(PauseDownloadL());
                }
            }
        }
    else if (iPackageState.iState == RFotaEngineSession::EDownloadComplete
            || iPackageState.iState == RFotaEngineSession::EStartingUpdate)
        {
        if (aResponse == EHbLSK)
            {
            FLOG(_L("CONTINUE is pressed/Timeout on update dialog"));
            iPackageState.iResult = KErrNotFound;
            TRAPD(err,
                    iDatabase->OpenDBL();
                    iDatabase->SetStateL( iPackageState ,KNullDesC8,EFDBResult );
                    iDatabase->CloseAndCommitDB();
            );
            FLOG(_L("Updating the fota database... err = %d"), err);

            TInt callactive(EFalse);
            
            TRAP(err, 
            iFMSClient.OpenL();
            TInt err1 = iFMSClient.IsPhoneCallActive(callactive);
            iFMSClient.Close();
            );
            
            if (callactive)
                {
                FLOG(_L("Fota Update:: Active call found; differing showing the install dialog!"));

                iPackageState.iResult = RFotaEngineSession::EResUpdateFailed;
                TRAP(err, 
                    iDatabase->OpenDBL();
                    iDatabase->SetStateL(iPackageState, KNullDesC8, EFDBResult);
                    iDatabase->CloseAndCommitDB();
                    );
                
                TRAP(err,
                    iFMSClient.OpenL();
                    iFMSClient.NotifyForUpdateL(EUpdMonitorPhoneCallEnd);
                    iFMSClient.Close();
                    );
                
                FLOG(_L("Deleting the fullscreen dialog..."));
                iFullScreenDialog->deleteLater();
                iFullScreenDialog = NULL;
                SetServerActive(EFalse);
                return;
                }

            TRAP(err, iUpdater->StartUpdateL( iPackageState ));
            FLOG(_L("Starting update, err = %d"), err);
            }
        else
            {
            FLOG(_L("CONTINUE LATER is pressed on update dialog"));
            ConstructApplicationUI(EFalse);

            iUpdater->CancelMonitor();

            if (iFullScreenDialog->IsLSKEnabled())
                {
                DecrementUserPostponeCount();
                }

            iPackageState.iState = RFotaEngineSession::EStartingUpdate;
            iPackageState.iResult = RFotaEngineSession::EResUserCancelled;
            TRAPD(err,
                    iDatabase->OpenDBL();
                    iDatabase->SetStateL( iPackageState ,KNullDesC8,EFDBState|EFDBResult );
                    iDatabase->CloseAndCommitDB();
            );
            FLOG(_L("Updating the fota database... err = %d"), err);

            SetStartupReason(EFotaUpdateInterrupted);
            SetServerActive(EFalse);
            TRAP(err, InvokeFmsL());
            StopServerWhenPossible();

            iAsyncOperation = EFalse;
            FLOG(_L("Invoking fms, err = %d"), err);
            }
        }
    FLOG(_L("CFotaServer::HandleFullScreenDialogResponse <<"));
    }

void CFotaServer::UpdateBatteryLowInfo(TBool aValue)
    {
    FLOG(_L("CFotaServer::UpdateBatteryLowInfo >>"));

    if (aValue)
        {
        FLOG(
                _L("Fota Update:: Battery has become low; disabling installation"));
        iPackageState.iResult = RFotaEngineSession::EResUpdateFailed;
        TRAP_IGNORE(
                iDatabase->OpenDBL();
                iDatabase->SetStateL( iPackageState ,KNullDesC8,EFDBResult );
                iDatabase->CloseAndCommitDB();
            );

        ShowFullScreenDialog(EHbFotaLowBattery);
        }
    else
        {
        ShowFullScreenDialog(EHbFotaUpdate);
        FLOG(
                _L("Fota Update:: Battery is still sufficient; enabling installation"));
        }
    FLOG(_L("CFotaServer::UpdateBatteryLowInfo <<"));
    }

// --------------------------------------------------------------------------
// CFotaServer::CheckSWVersionL
// Check the s/w version
// --------------------------------------------------------------------------
//

TBool CFotaServer::CheckSWVersionL()

    {
    FLOG(_L("CFotaServer::CheckSWVersionL  >>"));
    TBuf<KSysVersionInfoTextLength> temp;
    HBufC16* message16 = NULL;
    TBool isPkgvalid(ETrue);
    if (GetSoftwareVersion(temp) == KErrNone)
        {

        //TBuf<KSysVersionInfoTextLength>swvfromfile;
        //Fetch the software version ...
        RFileReadStream rstr;
        TInt err1 = rstr.Open(iFs, KSWversionFile, EFileRead);
        if (err1 == KErrNone)
            {
            CleanupClosePushL(rstr);
            TInt msglen = rstr.ReadInt16L();
            if (msglen > 0)
                {
                message16 = HBufC16::NewLC(msglen + 1);
                TPtr16 tempswv = message16->Des();
                TRAPD(err, rstr.ReadL(tempswv,msglen ));

                if (err != KErrNone && err != KErrEof)
                    {
                    FLOG(_L("  file read err %d"), err); //User::Leave( err ); 
                    msglen = 0;
                    }
                else
                    {
                    FLOG(_L("  msglen  %d"), msglen);
                    TPtr swvfromfile = message16->Des();

                    FLOG(_L("  swvfromfile=%S"), message16);

                    //Compare the software versions to decide whether the download is still valid or not.
                    if (msglen != temp.Length() || temp.Compare(tempswv)
                            != KErrNone)
                        {
                        isPkgvalid = EFalse;
                        FLOG(_L("CFotaServer::software not matching  >>"));

                        }
                    }

                CleanupStack::PopAndDestroy(message16);
                }
            CleanupStack::PopAndDestroy(&rstr);

            }

        }

    FLOG(_L("CFotaServer::CheckSWVersionL  <<"));
    return isPkgvalid;

    }

// --------------------------------------------------------------------------
// CFotaServer::ScheduledUpdateL
// Update, triggered by scheduler
// --------------------------------------------------------------------------
void CFotaServer::ScheduledUpdateL(TFotaScheduledUpdate aUpdate,
        TFotaClient aClient)
    {
    FLOG(_L("CFotaServer::ScheduledUpdateL >>"));

    iAsyncOperation = ETrue;
    WakeupServer();

    TPackageState s = GetStateL(aUpdate.iPkgId);

    if (s.iState == RFotaEngineSession::EDownloadProgressing)
        {
        FLOG(_L("Trying to resume the download in non-silent mode"));
        iPackageState = s;
        TryResumeDownloadL(aClient, EFalse);
        }
    else if (s.iState == RFotaEngineSession::EStartingUpdate)
        {
        // If update is in progress, do not start new one (multiple popups)
        if (iUpdater)
            {
            FLOG(_L("\t\tupdate in progress"));
            return;
            }
        else
            {
            FLOG(_L("Trying to resume the installation in non-silent mode"));
            iPackageState = s;
            TryUpdateL(aClient);
            }
        }
    FLOG(_L("CFotaServer::ScheduledUpdateL <<"));
    }

// --------------------------------------------------------------------------
// CFotaServer::DoConnect
// From CServer2. Initializes class members. 
// --------------------------------------------------------------------------
void CFotaServer::DoConnect(const RMessage2 &aMessage)
    {
    FLOG(_L("CFotaServer::DoConnect(const RMessage2 &aMessage) >>"));

    // In case shutdown is in progress, cancel it.
    if (iInitialized == EFalse)
        {
        TRAPD( err, ClientAwareConstructL( aMessage ) );
        if (err)
            FLOG(_L("   ClientAwareConstructL err %d"), err);
        }
    CServer2::DoConnect(aMessage);
    FLOG(_L("CFotaServer::DoConnect(const RMessage2 &aMessage) <<"));
    }

// --------------------------------------------------------------------------
// CFotaServer::GetStateL
// Get state of a download package
// --------------------------------------------------------------------------
TPackageState CFotaServer::GetStateL(const TInt aPkgId)
    {
    FLOG(_L("CFotaServer::GetStateL >>"));
    TPackageState s = RFotaEngineSession::EIdle;

    if (aPkgId >= 0) // Used by all clients
        {
        iDatabase->OpenDBL();
        s = iDatabase->GetStateL(aPkgId,iLastFwUrl);
        iDatabase->CloseAndCommitDB();
        }
    else if (aPkgId == -1) //Used by DM UI to get the state of last fota operation
        {
        //Read status from fotastate last entry

        iDatabase->OpenDBL();

        RArray<TInt> states;
        CleanupClosePushL(states);
        iDatabase->GetAllL(states);
        // Loop states. 
        for (TInt i = 0; i < states.Count(); ++i)
            {
            TPackageState tmp;
            TBuf8<KMaxFileName> tmpurl;
            tmp = iDatabase->GetStateL(states[i], tmpurl);
            FLOG(_L("***Package: %d, State = %d"), states[i],
                    (TInt) tmp.iState);
            if (tmp.iState != RFotaEngineSession::EIdle)
                {
                s = tmp;
                iLastFwUrl.Copy(tmpurl);
                }
            }
        FLOG(_L("Status of current operation is %d"), (TInt) s.iState);

        CleanupStack::PopAndDestroy(&states);
        iDatabase->CloseAndCommitDB();
        }
    FLOG(_L("CFotaServer::GetStateL <<"));
    return s;
    }

// --------------------------------------------------------------------------
// CFotaServer::OnSyncMLSessionEvent
// --------------------------------------------------------------------------
//
void CFotaServer::OnSyncMLSessionEvent(TEvent aEvent, TInt aIdentifier,
        TInt aError, TInt /*aAdditionalData*/)
    {
    FLOG(_L("CFotaServer::OnSyncMLSessionEvent >>"));

    if (iSyncJobId != aIdentifier)
        return;
    FLOG(_L("CFotaServer::OnSyncMLSessionEvent %d err:%d (id %d==%d?)"),
            aEvent, aError, aIdentifier, iSyncJobId);
    TBool end(EFalse);

    if (iSyncJobId == aIdentifier)
        {

        switch (aEvent)
            {
            //EJobStart = 0
            case EJobStartFailed: // 1 E
                {
                end = ETrue;
                }
                break;
            case EJobStop: // 2 E
                {
                end = ETrue;
                // Sync ok => do not try anymore
                if (aError == KErrNone)
                    {
                    iSyncMLAttempts = 0;
                    }
                }
                break;
            case EJobRejected: // 3 E
                {
                end = ETrue;
                }
                break;
                // ETransportTimeout , // 7
            default:
                {
                }
                break;
            }
        }

    if (end && iSyncMLAttempts == 0)
        {
        if (iTimedSMLSessionClose)
            {
            FLOG(_L("   closing smlsession timer"));
            iTimedSMLSessionClose->Cancel();
            delete iTimedSMLSessionClose;
            iTimedSMLSessionClose = NULL;
            }
        StopServerWhenPossible();
        
        }
    else if (end)
        {

        if (iTimedSMLSessionClose)
            {
            FLOG(_L("   closing smlsession timer"));
            iTimedSMLSessionClose->Cancel();
            delete iTimedSMLSessionClose;
            iTimedSMLSessionClose = NULL;
            }
        
        FLOG(_L("   starting smlsession timer"));
        TRAPD( err2, iTimedSMLSessionClose = CPeriodic::NewL (EPriorityNormal) );
        if (!err2)
            {
            iTimedSMLSessionClose->Start(TTimeIntervalMicroSeconds32(
                    KSyncmlSessionRetryInterval),
                    TTimeIntervalMicroSeconds32(
                            KSyncmlSessionRetryInterval), TCallBack(
                            StaticDoCloseSMLSession, this));
            }
        else
            FLOG(_L(" iTimedSMLSessionClose err %d"), err2);
        }
    FLOG(_L("CFotaServer::OnSyncMLSessionEvent <<"));
    }

// --------------------------------------------------------------------------
// CFotaServer::GetUpdateTimeStampL 
// Gets time of last update. It is stored in a file.
// --------------------------------------------------------------------------
void CFotaServer::GetUpdateTimeStampL(TDes16& aTime)
    {
    FLOG(_L("CFotaServer::GetUpdateTimeStampL  >>"));
    TInt err;

    RFileReadStream rstr;
    err = rstr.Open(iFs, _L("updatetimestamp"), EFileRead);

    if (err == KErrNone)
        {
        FLOG(_L(" update time stamp file found,reading"));
        CleanupClosePushL(rstr);
        TInt year = rstr.ReadInt32L();
        TInt month = rstr.ReadInt32L();
        TInt day = rstr.ReadInt32L();
        TInt hour = rstr.ReadInt32L();
        TInt minute = rstr.ReadInt32L();
        TInt year16 = year;
        TInt month16 = month;
        TInt day16 = day;
        TInt hour16 = hour;
        TInt minute16 = minute;
        CleanupStack::PopAndDestroy(&rstr);
        aTime.Append(year16);
        aTime.Append(month16);
        aTime.Append(day16);
        aTime.Append(hour16);
        aTime.Append(minute16);
        }
    else if (err != KErrNotFound)
        {
        __LEAVE_IF_ERROR ( err );
        }

    if (err == KErrNotFound)
        {
        FLOG(_L(" update time stamp not found "));
        }

    FLOG(_L("CFotaServer::GetUpdateTimeStampL  <<"));
    }

void CFotaServer::GetCurrentFwDetailsL(TDes8& aName, TDes8& aVersion,
        TInt& aSize)
    {
    FLOG(_L("CFotaServer::GetCurrentFwDetailsL >>"));

    TPackageState package = GetStateL(-1);

    aName.Copy(package.iPkgName);
    aVersion.Copy(package.iPkgVersion);
    aSize = package.iPkgSize;

    FLOG(_L("CFotaServer::GetCurrentFwDetailsL <<"));
    }
// --------------------------------------------------------------------------
// CFotaServer::GetUpdatePackageIdsL
// --------------------------------------------------------------------------
//
void CFotaServer::GetUpdatePackageIdsL(TDes16& aPackageIdList)
    {
    FLOG(_L("CFotaServer::GetUpdatePackageIdsL()"));
    __LEAVE_IF_ERROR(KErrNotSupported);
    }

// --------------------------------------------------------------------------
// CFotaServer::GenericAlertSentL
// Generic alert sent, do cleanup. FUMO spec specifies cleanup need to have 
// for states 20,70,80,90,100. Called by syncml framework when it has sent
// generic alert
// --------------------------------------------------------------------------
//
void CFotaServer::ResetFotaStateL(const TInt aPackageID)
    {
    FLOG(_L("CFotaServer::ResetFotaStateL %d"), aPackageID);
    TPackageState state;
    TBool toidle(EFalse);
    TBool deletepkg(EFalse);

    iDatabase->OpenDBL();
    state = iDatabase->GetStateL(aPackageID, iLastFwUrl);

    switch (state.iState)
        {
        case RFotaEngineSession::EDownloadFailed:
            {
            toidle = ETrue;
            deletepkg = ETrue;
            }
            break;
        case RFotaEngineSession::EUpdateFailed:
            {
            toidle = ETrue;
            deletepkg = ETrue;
            }
            break;
        case RFotaEngineSession::EUpdateFailedNoData:
            {
            toidle = ETrue;
            }
            break;
        case RFotaEngineSession::EUpdateSuccessful:
            {
            toidle = ETrue;
            }
            break;
        case RFotaEngineSession::EUpdateSuccessfulNoData:
            {
            toidle = ETrue;
            }
            break;
        case RFotaEngineSession::EDownloadComplete:
            {
            state.iState = RFotaEngineSession::EStartingUpdate;
            state.iResult = KErrNotFound;
            iDatabase->SetStateL(state, KNullDesC8, EFDBState);
            toidle = EFalse;
            }
            break;
        default:
            {
            FLOG(_L(" pkg %d (state:%d) doesnt need cleanup"), aPackageID,
                    state.iState);
            }
            break;
            
        }

    if (toidle)
        {
        state.iState = RFotaEngineSession::EIdle;
        state.iResult = KErrNotFound;
        iDatabase->SetStateL(state, KNullDesC8, EFDBState | EFDBResult);
        DeleteFUMOTreeL();
        }

    iDatabase->CloseAndCommitDB();

    SetStartupReason(EFotaDefault);

    if (deletepkg)
        {
        DownloaderL()->DeleteUpdatePackageL();
        }
    }

void CFotaServer::ResetCounters()
    {
    FLOG(_L("CFotaServer::ResetCounters >>"));

    CRepository* centrep = NULL;
    TInt maxcount(0);
    TRAPD( err, centrep = CRepository::NewL( KCRUidFotaServer ) );
    if (err == KErrNone)
        {
        err = centrep->Get(KFOTAMaxPostponeCount, maxcount);
        err = centrep->Set(KFOTAUserPostponeCount, maxcount);
        err = centrep->Set(KFOTADownloadRestartCount, KMaxDownloadRestartCount);
        }
    delete centrep;
    centrep = NULL;

    FLOG(_L("CFotaServer::ResetCounters, postpone count set to %d, err = %d <<"), maxcount, err);
    }

TBool CFotaServer::IsUserPostponeAllowed()
    {
    FLOG(_L("CFotaServer::IsUserPostponeAllowed >>"));

    TBool ret(ETrue);
    CRepository* centrep = NULL;
    TInt count(1);
    TRAPD( err, centrep = CRepository::NewL( KCRUidFotaServer ) );
    if (err == KErrNone)
        {
        err = centrep->Get(KFOTAUserPostponeCount, count);
        }
    delete centrep;
    centrep = NULL;

    if (count == 0)
        ret = EFalse;
    else if (count == -1) //-1 signifies infinite postpone
        ret = ETrue;
    
    FLOG(_L("CFotaServer::IsUserPostponeAllowed, count = %d, ret = %d, err = %d >>"), count, ret, err);
    
    return ret;
    }

void CFotaServer::DecrementUserPostponeCount()
    {
    FLOG(_L("CFotaServer::DecrementUserPostponeCount >>"));

    CRepository* centrep = NULL;
    TInt count;
    TRAPD( err, centrep = CRepository::NewL( KCRUidFotaServer ) );
    if (err == KErrNone)
        {
        err = centrep->Get(KFOTAUserPostponeCount, count);
        if (--count < 0)
            count = 0;
        err = centrep->Set(KFOTAUserPostponeCount, count);
        }
    delete centrep;
    centrep = NULL;

    FLOG(
            _L("CFotaServer::DecrementUserPostponeCount, tries left: %d, err = %d >>"),
            count, err);
    }

TBool CFotaServer::DecrementDownloadRestartCount()
    {
    FLOG(_L("CFotaServer::DecrementDownloadRestartCount >>"));
    
    TBool ret (ETrue);
    CRepository* centrep = NULL;
    TInt count;
    
    TRAPD( err, centrep = CRepository::NewL( KCRUidFotaServer ) );
    if (err == KErrNone)
        {
        err = centrep->Get(KFOTADownloadRestartCount, count);
        if (--count < 0)
            count = 0;
        err = centrep->Set(KFOTADownloadRestartCount, count);
        }
    delete centrep;
    centrep = NULL;
    
    if (count == 0)
        ret = EFalse;
    
    FLOG(_L("CFotaServer::DecrementDownloadRestartCount, ret = %d, err = %d <<"), ret, err);
    return ret;
    }


// --------------------------------------------------------------------------
// CFotaServer::StartNetworkMonitorL       
// Starts Network Monitoring operation for defined interval and retries (FotaNetworkRegMonitor.h)
// --------------------------------------------------------------------------
void CFotaServer::StartNetworkMonitorL()
    {
    FLOG(_L("CFotaServer::StartNetworkMonitorL >>"));
    if (!iMonitor)
        iMonitor = CFotaNetworkRegStatus::NewL(this);
    iMonitor->StartMonitoringL();

    FLOG(_L("CFotaServer::StartNetworkMonitorL <<"));
    }

// --------------------------------------------------------------------------
// CFotaServer::ReportNetworkStatus       
// called by CFotaNetworkRegStatus for reporting status
// --------------------------------------------------------------------------
void CFotaServer::ReportNetworkStatus(TBool status)
    {
    FLOG(_L("CFotaServer::ReportNetworkStatus, status = %d >>"), status);
    iRetryingGASend = EFalse;
    iNetworkAvailable = status;

    if (iNetworkAvailable)
        {
        TRAPD (err, CreateDeviceManagementSessionL (iPackageState));
        if (err != KErrNone)
            {
            FLOG(_L("Error %d occured while sending GA after retries"), err);
            }
        }
    //No need of iMonitor anymore	
    if (iMonitor)
        {
        delete iMonitor;
        iMonitor = NULL;
        }

    FLOG(_L("CFotaServer::ReportNetworkStatus >>"));
    }

// --------------------------------------------------------------------------
// CFotaServer::CheckIapExistsL 
// Checks for IAP Id exists or not in commsdb
// IAP Id used for resuming the download or for sending Generic alert
// --------------------------------------------------------------------------
//
TBool CFotaServer::CheckIapExistsL(TUint32 aIapId)
    {
    FLOG(_L("CFotaServer::CheckIapExistsL >>"));
    TBool exists = EFalse;  
    RCmManager cmManager;    
    cmManager.OpenLC();
    RCmConnectionMethod conn;
    TRAPD(err, conn = cmManager.ConnectionMethodL( aIapId ));
    if(err == KErrNone)//connection method exists
       exists = ETrue;
    conn.Close();
    CleanupStack::PopAndDestroy();//cmManager                    
    FLOG(_L("CFotaServer::CheckIapExistsL <<"));
    return exists;
    }

// --------------------------------------------------------------------------
// CFotaServer::IsDeviceDriveBusyL 
// Finds if device encryption or decryption is in progress.
// It is harmful to restart device whilst encryption/decryption is in progress
// --------------------------------------------------------------------------
//
TBool CFotaServer::IsDeviceDriveBusyL()
    {
    FLOG(_L("CFotaServer::IsDeviceDriveBusyL >>"));

    TBool ret(EFalse);
    FeatureManager::InitializeLibL();
    TBool defeature = FeatureManager::FeatureSupported(
            KFeatureIdFfDeviceEncryptionFeature);
    FeatureManager::UnInitializeLib();

    if (defeature)
        {
        TInt value(EOpIdle); // Encryption idle
        RProperty::Get(KDevEncProtectedUid, KDevEncOperationKey, value);

        if (value != EOpIdle)
            ret = ETrue;
        }

    FLOG(_L("CFotaServer::IsDeviceDriveBusyL, ret = %d <<"), ret);
    return ret;
    }
// --------------------------------------------------------------------------
// CFotaDownload::SetIapToUseL
// Sets the IAP ID to use. This menthod is used in fresh and resume download.
// --------------------------------------------------------------------------
//
void CFotaServer::SetIapToUseL()
    {
    FLOG(_L("CFotaServer::SetIapToUseL >>"));

    TInt aIapId(KErrNotFound);

    // GET IAP FROM PROFILE ----------------------------------------------

    FLOG(_L("[FotaServer]   1"));
    RSyncMLSession syncsession;
    syncsession.OpenL();
    FLOG(_L("[FotaServer]   2"));
    RSyncMLDevManProfile smlprof;
    RArray<TSmlTransportId> connections;
    TSmlTransportId transport;
    RSyncMLConnection connection;

    CleanupClosePushL(syncsession);
    CleanupClosePushL(smlprof);
    CleanupClosePushL(connections);
    CleanupClosePushL(connection);

    FLOG(_L("[FotaServer]   1.1 opening syncml profileid %d "),
            iPackageState.iProfileId);
    smlprof.OpenL(syncsession, iPackageState.iProfileId, ESmlOpenRead);
    FLOG(_L("[FotaServer]  1.1"));
    smlprof.ListConnectionsL(connections);
    FLOG(_L("[FotaServer]  1.3"));
    transport = connections[0];
    connection.OpenL(smlprof, transport);
    TBuf8<20> iapid2 = connection.GetPropertyL(KNSmlIAPId);
    TLex8 iapid3(iapid2);
    iapid3.Val(aIapId);

    CleanupStack::PopAndDestroy(&connection);
    CleanupStack::PopAndDestroy(&connections);
    CleanupStack::PopAndDestroy(&smlprof);
    CleanupStack::PopAndDestroy(&syncsession);

    if (aIapId > KErrNotFound)
        {
        iPackageState.iIapId = aIapId;
        }
    else if (iPackageState.iState != RFotaEngineSession::EDownloadProgressing)
        {
        // GET IAP FROM CURRENT CONNECTION ----------------------------------------------

        FLOG(_L("IAP in DM profile is default. Hence reading from the connection manager..."));
        TInt sockIapid(-1);
        RSocketServ serv;
        CleanupClosePushL(serv);
        User::LeaveIfError(serv.Connect());

        RConnection conn;
        CleanupClosePushL(conn);
        User::LeaveIfError(conn.Open(serv));

        TUint count(0);
        User::LeaveIfError(conn.EnumerateConnections(count));
        // enumerate connections
        for (TUint idx = 1; idx <= count; ++idx)
            {
            TConnectionInfo connectionInfo;
            TConnectionInfoBuf connInfo(connectionInfo);

            TInt err = conn.GetConnectionInfo(idx, connInfo); // iapid
            if (err != KErrNone)
                {
                CleanupStack::PopAndDestroy(2); // conn, serv
                User::Leave(err);
                }
            // enumerate connectionclients
            TConnectionEnumArg conArg;
            conArg.iIndex = idx;
            TConnEnumArgBuf conArgBuf(conArg);
            err = conn.Control(KCOLConnection, KCoEnumerateConnectionClients,
                    conArgBuf);
            if (err != KErrNone)
                {
                CleanupStack::PopAndDestroy(2); // conn, serv
                User::Leave(err);
                }
            TInt cliCount = conArgBuf().iCount;
            for (TUint j = 1; j <= cliCount; ++j)
                {
                TConnectionGetClientInfoArg conCliInfo;
                conCliInfo.iIndex = j;
                TConnGetClientInfoArgBuf conCliInfoBuf(conCliInfo);
                err = conn.Control(KCOLConnection,
                        KCoGetConnectionClientInfo, conCliInfoBuf);

                if (err != KErrNone)
                    {
                    CleanupStack::PopAndDestroy(2); // conn, serv
                    User::Leave(err);
                    }
                TConnectionClientInfo conCliInf = conCliInfoBuf().iClientInfo;
                TUid uid = conCliInf.iUid;
                if (uid == TUid::Uid(KSosServerUid))
                    {
                    sockIapid = connInfo().iIapId;
                    FLOG(_L("[FotaServer]  IAP found from ESOCK %d"), sockIapid);
                    iPackageState.iIapId = sockIapid;
                    }

                FLOG(_L("[FotaServer]  CFotaDownload::DownloadL uid %x"),
                        uid.iUid);
                }
            }
        CleanupStack::PopAndDestroy(2); // conn, serv     
        }

    FLOG(_L("CFotaDownload::SetIapToUseL, iap = %d <<"), iPackageState.iIapId);
    }
// --------------------------------------------------------------------------
// CFotaServer::GetSoftwareVersion
// Gets the software version
// 
// --------------------------------------------------------------------------
//
TInt CFotaServer::GetSoftwareVersion(TDes& aVersion)
    {
    FLOG(_L("CFotaServer::GetSoftwareVersion >>"));
    aVersion.Zero();

    SysVersionInfo::TVersionInfoType what = SysVersionInfo::EFWVersion;
    TInt error(KErrNone);
    error = SysVersionInfo::GetVersionInfo(what, aVersion);
    FLOG(_L("CFotaServer::GetSoftwareVersion,SwV=%S <<"), &aVersion);
    return error;
    }

// --------------------------------------------------------------------------
// CFotaServer::ResetFotaStateL
// Resets the Fotastate
// 
// --------------------------------------------------------------------------
//    
void CFotaServer::ResetFotaStateToFailL()
    {
    FLOG(_L("CFotaServer::ResetFotaStateToFailL >>"));

    TPackageState state;
    if (!iDatabase->IsOpen())
        iDatabase->OpenDBL();
    //Fetch the software version that was before download from db.
    state = iDatabase->GetStateL(iPackageState.iPkgId, iLastFwUrl);
    state.iState = RFotaEngineSession::EUpdateFailed;
    state.iResult = RFotaEngineSession::EResPackageMismatch;
    iDatabase->SetStateL(state, KNullDesC8, EFDBState | EFDBResult);
    iDatabase->CloseAndCommitDB();

    DownloaderL()->DeleteUpdatePackageL();

    ReportFwUpdateStatusL(state);

    FLOG(_L("CFotaServer::ResetFotaStateToFailL <<"));
    }

void CFotaServer::CreateDiskReservation()
    {
    FLOG(_L("CFotaServer::CreateDiskReservation >>"));

    TRAP_IGNORE(DownloaderL()->CreateDiskReservation());

    FLOG(_L("CFotaServer::CreateDiskReservation <<"));
    }

void CFotaServer::DeleteDiskReservation(TDesC& path)
    {
    FLOG(_L("CFotaServer::DeleteDiskReservation >>"));

    QString temp = QString::fromUtf8(reinterpret_cast<const char*> (path.Ptr()), path.Length());

    TRAP_IGNORE(DownloaderL()->DeleteDiskReservation(temp));

    FLOG(_L("CFotaServer::DeleteDiskReservation <<"));
    }

inline DownloadManagerClient* CFotaServer::DownloaderL()
    {
    if (!iDownloader)
        {
        FLOG(_L("Creating new download client..."));
        iDownloader = DownloadManagerClient::NewL(this);
        }

    return iDownloader;
    }

void CFotaServer::DropSession()
    {
    FLOG(_L("CFotaServer::DropSession >>"));

    iSessionCount--;

    FLOG(_L("Number of active sessions = %d"), iSessionCount);

    if (iSessionCount == 0 && !iAsyncOperation)
        {
        StopServerWhenPossible();
        ServerCanShut(ETrue);
        }

    FLOG(_L("CFotaServer::DropSession <<"));
    }

static TInt StaticApplicationShutter(TAny *aPtr)
    {
    __ASSERT_ALWAYS( aPtr, User::Panic(KFotaPanic, KErrArgument) );
    CFotaServer* srv = (CFotaServer*) aPtr;
    srv->StopServerWhenPossible();
    return KErrNone;
    }

void CFotaServer::WakeupServer()
    {
    FLOG(_L("CFotaServer::WakeupServer >>"));
    ServerCanShut(EFalse);
    if (iAppShutter)
        {
        iAppShutter->Cancel();
        delete iAppShutter;
        iAppShutter = NULL;
        }

    FLOG(_L("CFotaServer::WakeupServer >>"));
    }

void CFotaServer::StopServerWhenPossible()
    {
    FLOG(_L("CFotaServer::StopServerWhenPossible, sessioncount = %d, servercanshut = %d >>"), iSessionCount, iServerCanShut);
    //Check if it's the right time to do so..

    if (iSessionCount == 0 && iServerCanShut)
        {
        FLOG(_L("Shutting down the Fota server..."));
        //Do some cleanup

        if (iAppShutter)
            {
            iAppShutter->Cancel();
            delete iAppShutter;
            iAppShutter = NULL;
            }

        //Exit. This will stop the active scheduler too.
        QApplication::exit();
        }
    else if (iSessionCount == 0)
        {
        FLOG(_L("Diferring shutdown now. Started shutdown timer..."));

        if (!iAppShutter)
            {
            TRAP_IGNORE(
                    iAppShutter = CPeriodic::NewL (EPriorityNormal);
                    iAppShutter->Start(KFotaTimeShutDown, KFotaTimeShutDown,
                    TCallBack(StaticApplicationShutter, this));
                    );
            }

        }
    else
        {
        //one or more client is still open
        FLOG(_L("Diferring shutdown now."));
        WakeupServer();
        }
    FLOG(_L("CFotaServer::StopServerWhenPossible <<"));
    }

void CFotaServer::ServerCanShut(TBool aParam)
    {
    FLOG(_L("CFotaServer::ServerCanShut, param = %d >>"), aParam);

    iServerCanShut = aParam;

    FLOG(_L("CFotaServer::ServerCanShut <<"));

    }

void CFotaServer::ConstructApplicationUI(TBool aVal)
    {
    FLOG(_L("CFotaServer::ConstructApplicationUI, value = %d >>"), aVal);

    if (!aVal)
        iMainwindow.lower();
    else
        iMainwindow.raise();
    
    FLOG(_L("CFotaServer::ConstructApplicationUI <<"));
    }


void CFotaServer::SetVisible(TBool aVisible)
{
    FLOG(_L("CFotaServer::SetVisible >>"));

    if(iFullScreenDialog)
	iFullScreenDialog->SetVisible(aVisible);

    FLOG(_L("CFotaServer::SetVisible <<"));
}

//End of file
