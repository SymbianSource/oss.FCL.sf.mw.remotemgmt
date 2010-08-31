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
 * Description:   starts update sequence
 *
 */

// INCLUDE FILES
#include <StringLoader.h>
#include <centralrepository.h>
#include <AknUtils.h>
#include <AknBidiTextUtils.h> 
#include <biditext.h>
#include <gdi.h>
#include <fotaengine.h>
#include <apgtask.h>
#include <aknradiobuttonsettingpage.h> 
#include <akntitle.h>
#include <schtime.h>
#include <csch_cli.h>
#include <fotaengine.h>
#include <XQConversions>
#include "fotaupdate.h"
#include "fmsclient.h"
#include "FotasrvSession.h"
#include "fotaserverPrivateCRKeys.h"
#include "fotaserverPrivatePSKeys.h"
#include "fotanotifiers.h"

#include <usbman.h>
#include <usbstates.h>
// ============================= MEMBER FUNCTIONS ============================


// ---------------------------------------------------------------------------
// CFotaUpdate::CFotaUpdate()
// ---------------------------------------------------------------------------
//
CFotaUpdate::CFotaUpdate() :
    CActive(EPriorityNormal)
    {
    CActiveScheduler::Add(this);
    }

// ---------------------------------------------------------------------------
// CFotaUpdate::~CFotaUpdate()
// ---------------------------------------------------------------------------
//
CFotaUpdate::~CFotaUpdate()
    {
    FLOG(_L("CFotaUpdate::~CFotaUpdate >>"));
    iFotaServer = NULL;

    CancelMonitor();

    if (iProperty.Handle())
        iProperty.Close();

    FLOG(_L("CFotaUpdate::~CFotaUpdate <<"));
    }

// ---------------------------------------------------------------------------
// CFotaUpdate::NewL 
// ---------------------------------------------------------------------------
//
CFotaUpdate* CFotaUpdate::NewL(CFotaServer* aServer)
    {
    CFotaUpdate* ao = new (ELeave) CFotaUpdate();
    ao->iFotaServer = aServer;
    return ao;
    }

// ---------------------------------------------------------------------------
// CFotaUpdate::CheckUpdateResults
// Checks if there is update result file available (meaning that update just
// took place)
// ---------------------------------------------------------------------------
//
TBool CFotaUpdate::CheckUpdateResults(RFs& aRfs)
    {
    RFile f;
    TInt err;
    err = f.Open(aRfs, KUpdateResultFile, EFileShareAny);
    f.Close();
    if (err != KErrNone)
        {
        return EFalse;
        }
    FLOG(_L(" CFotaUpdate::CheckUpdateResults  update result file Found! "));
    return ETrue;
    }

// ---------------------------------------------------------------------------
// CFotaUpdate::ExecuteUpdateResultFileL
// Read result code from update result file and update state accordingly. 
// Show notifier "Update succesful". Do some cleanup.
// ---------------------------------------------------------------------------
//
void CFotaUpdate::ExecuteUpdateResultFileL()
    {
    FLOG(_L("CFotaUpdate::ExecuteUpdateResultFileL >>"));

    TInt err;
    RFileReadStream rstr;
    TInt result;
    TInt msglen;
    HBufC8* message = NULL;
    HBufC16* message16 = NULL;
    TBool deleteData(EFalse);

    // Open update result file
    FLOG(_L("opening rstr 1/2  "));
    err = rstr.Open(iFotaServer->iFs, KUpdateResultFile, EFileRead
            | EFileStream);
    FLOG(_L("opened  rstr 2/2  "));
    if (err)
    	{
        FLOG(_L("   update result file open err %d"), err);
      }
    User::LeaveIfError(err);
    FLOG(_L("       0.1"));
    CleanupClosePushL(rstr);
    FLOG(_L("       0.2"));

    // Read resultcode 
    result = rstr.ReadUint32L();
    msglen = rstr.ReadUint32L();
    if (msglen != 0)
        {
        message = HBufC8::NewLC(msglen + 1);
        TPtr8 ptrdesc = message->Des();
        TRAPD ( err2, rstr.ReadL(ptrdesc) );
        if (err2 != KErrNone && err2 != KErrEof)
            {
            FLOG(_L("  file read err %d"), err2);
            User::Leave(err2);
            }

        message16 = HBufC16::NewLC(message->Des().Length());
        message16->Des().Copy(*message);
        FLOG(_L("   1 update result: %d"), result);
        FLOG(_L("   2 dbg msg: %S"), message16);
        CleanupStack::PopAndDestroy(message16);
        CleanupStack::PopAndDestroy(message);
        }

    FLOG(_L("       0.6 "));
    CleanupStack::PopAndDestroy(&rstr);

    // Map resultcode to FUMO result code
    RFotaEngineSession::TState fstate = RFotaEngineSession::EUpdateFailed;
    RFotaEngineSession::TResult fresult =
            RFotaEngineSession::EResUpdateFailed;

    FLOG(_L("   3"));
    switch (result)
        {
        case UPD_OK:
            {
            fstate = RFotaEngineSession::EUpdateSuccessfulNoData;
            fresult = RFotaEngineSession::EResSuccessful;
            deleteData = ETrue;
            ShowDialogL(EFwUpdSuccess);
            }
            break;
        case UPD_INSTALL_REQUEST_IS_INVALID:
            {
            fstate = RFotaEngineSession::EUpdateFailedNoData;
            fresult = RFotaEngineSession::EResUpdateFailed;
            deleteData = ETrue;
            ShowDialogL(EFwUpdNotCompatible);
            }
            break;
        case UPD_UPDATE_PACKAGE_IS_ABSENT:
            {
            fstate = RFotaEngineSession::EUpdateFailedNoData;
            fresult = RFotaEngineSession::EResUpdateFailed;
            ShowDialogL(EFwUpdNotCompatible);
            }
            break;
        case UPD_UPDATE_PACKAGE_IS_CORRUPTED:
            {
            fstate = RFotaEngineSession::EUpdateFailedNoData;
            fresult = RFotaEngineSession::EResCorruptedFWUPD;
            deleteData = ETrue;
            ShowDialogL(EFwUpdNotCompatible);
            }
            break;
        case UPD_UPDATE_PACKAGE_CONTENTS_IS_INVALID:
            {
            fstate = RFotaEngineSession::EUpdateFailedNoData;
            fresult = RFotaEngineSession::EResCorruptedFWUPD;
            deleteData = ETrue;
            ShowDialogL(EFwUpdNotCompatible);
            }
            break;
        case UPD_UPDATE_PACKAGE_IS_NOT_COMPATIBLE_WITH_CURRENT_MOBILE_DEVICE:
            {
            fstate = RFotaEngineSession::EUpdateFailedNoData;
            fresult = RFotaEngineSession::EResPackageMismatch;
            deleteData = ETrue;
            ShowDialogL(EFwUpdNotCompatible);
            }
            break;
        case UPD_FATAL_ERROR:
            {
            fstate = RFotaEngineSession::EUpdateFailedNoData;
            fresult = RFotaEngineSession::EResUpdateFailed;
            deleteData = ETrue;
            ShowDialogL(EFwUpdNotCompatible);
            }
            break;

        default:
            {
            FLOG(_L("   3.1 invalid result: %d"), result);
            fstate = RFotaEngineSession::EUpdateFailedNoData;
            fresult = RFotaEngineSession::EResUpdateFailed;
            deleteData = ETrue;
            ShowDialogL(EFwUpdNotCompatible);
            }
            break;
        }

    // Find the state 60 (update  progressing) -> 100 (etc)
    RArray<TInt> states;
    TPackageState state;
    CleanupClosePushL(states);
    iFotaServer->iDatabase->OpenDBL();
    iFotaServer->iDatabase->GetAllL(states);

    FLOG(_L("   4.1 found %d states "), states.Count());
    for (TInt i = 0; i < states.Count(); ++i)
        {
        TPackageState tmp;
        TBuf8<KMaxFileName> tmpurl;
        tmp = iFotaServer->iDatabase->GetStateL(states[i], tmpurl);
        FLOG(_L("   5 got state "));
        if (tmp.iState == RFotaEngineSession::EUpdateProgressing)
            {
            state = tmp;
            state.iState = fstate;
            state.iResult = fresult;
            FLOG(_L("   6 Updating state id %d  to %d ,result %d  "),
                    state.iPkgId, state.iState, state.iResult);
            iFotaServer->iDatabase->SetStateL(state, KNullDesC8, EFDBState
                    | EFDBResult);
            }
        }
    iFotaServer->iDatabase->CloseAndCommitDB();
    CleanupStack::PopAndDestroy(&states);

    // Delete request file
    err = BaflUtils::DeleteFile(iFotaServer->iFs, KUpdateRequestFile);
    if (err != KErrNone && err != KErrNotFound)
        {
        FLOG(_L("   6.1  req file deleted, err %d"), err);
        User::Leave(err);
        }

    // Write timestamp (shown to user in device management ui)
    if (fresult == RFotaEngineSession::EResSuccessful)
        {
        RFileWriteStream wstr;
        TTime time;
        User::LeaveIfError(wstr.Replace(iFotaServer->iFs,
                KUpdateTimeStampFileName, EFileWrite));
        CleanupClosePushL(wstr);
        time.HomeTime();
        TInt year = time.DateTime().Year();
        TInt month = time.DateTime().Month();
        TInt day = time.DateTime().Day();
        TInt hour = time.DateTime().Hour();
        TInt minute = time.DateTime().Minute();
        wstr.WriteInt32L(year);
        wstr.WriteInt32L(month);
        wstr.WriteInt32L(day);
        wstr.WriteInt32L(hour);
        wstr.WriteInt32L(minute);
        CleanupStack::PopAndDestroy(1); // wstr
        }
    // Delete package content
    if (deleteData && state.iPkgId > KErrNotFound)
        {
        iFotaServer->DeletePackageL(state.iPkgId);
        DeleteUpdateResultFileL();
        }

    // Report state back to syncml server
    if (state.iPkgId >= 0)
        {
        FLOG(_L("   6.1 creating device mgmt session for profile %d"),
                state.iProfileId);
        iFotaServer->ReportFwUpdateStatusL(state);
        }
    else
        {
        FLOG(_L(" No state found in 'update progress' mode! cannot report status to DM server "));
        }

    FLOG(_L("CFotaUpdate::ExecuteUpdateResultFileL <<"));
    }

// ---------------------------------------------------------------------------
// CFotaUpdate::DeleteUpdateResultsL
// ---------------------------------------------------------------------------
//
void CFotaUpdate::DeleteUpdateResultFileL()
    {
    FLOG(_L("CFotaUpdate::DeleteUpdateResultsL >>"));
    // Delete result file
    RFs fs;
    __LEAVE_IF_ERROR(fs.Connect());
    BaflUtils::DeleteFile(fs, KUpdateResultFile);
    BaflUtils::DeleteFile(fs, KUpdateRequestFile);
    BaflUtils::DeleteFile(fs, KUpdateBitmap);
    BaflUtils::DeleteFile(fs, KRestartingBitmap);
    fs.Close();
    FLOG(_L("CFotaUpdate::DeleteUpdateResultsL <<"));
    }

// ---------------------------------------------------------------------------
// CFotaUpdate::UpdateL
// Updates the fw: Creates input files for update agent and boots device to 
// update mode.
// ---------------------------------------------------------------------------
void CFotaUpdate::UpdateL()
    {
    FLOG(_L("CFotaUpdate::UpdateL() >>"));
    // Set state ........................................
    iFotaServer->iPackageState.iState
            = RFotaEngineSession::EUpdateProgressing;
    iFotaServer->iDatabase->OpenDBL();
    iFotaServer->iDatabase->SetStateL(iFotaServer->iPackageState, KNullDesC8,
            EFDBState);
    iFotaServer->iDatabase->CloseAndCommitDB();

    // Write update request for update agent..............
    FLOG(_L("CFotaUpdate::UpdateL  1 writing update.req "));
    TBuf16<KMaxFileName> dp2filepath;
    HBufC16* dp2;
    RFileWriteStream wstr;
    CleanupClosePushL(wstr);
    FLOG(_L("CFotaUpdate::UpdateL  2 getting pkg location"));
    iFotaServer->DownloaderL()->GetUpdatePackageLocation(dp2filepath);

    FLOG(_L("CFotaUpdate::UpdateL  3 craeting update.req"));
    User::LeaveIfError(wstr.Replace(iFotaServer->iFs, KUpdateRequestFile,
            EFileWrite));
    wstr.WriteInt16L(1); // version number is  1
    wstr.WriteInt32L(1); // count   of cmds is 1
    wstr.WriteInt16L(0); // requestid is 0

    dp2 = HBufC16::NewLC(dp2filepath.Length());
    dp2->Des().Copy(dp2filepath);
    wstr.WriteInt32L(dp2->Des().Length() + 1); // length of filename + null
    wstr.WriteL(dp2->Des());
    wstr.WriteInt16L(0); // null character
    CleanupStack::PopAndDestroy(dp2);
    CleanupStack::PopAndDestroy(&wstr); // wstr

    FLOG(_L("CFotaUpdate::UpdateL  4 craeting update.bmp"));


    // Write update graphic for update agent ...............
    HBufC* updatetxt;
    HBufC* restarttxt;
    QString installstr = hbTrId("txt_device_update_info_installing");
    QString rebootstr = hbTrId("txt_device_update_info_rebooting");
    updatetxt = XQConversions::qStringToS60Desc(installstr);
    restarttxt = XQConversions::qStringToS60Desc(rebootstr);
    
    WriteUpdateBitmapL( updatetxt->Des(), KUpdateBitmap );
    WriteUpdateBitmapL( restarttxt->Des(), KRestartingBitmap );
    delete updatetxt;
    delete restarttxt;
    
    // Simulate update agent by writing result file.
    CRepository* centrep(NULL);
    TInt err = KErrNone;
    TRAP(err, centrep = CRepository::NewL( KCRUidFotaServer ) );
    TInt simulate(KErrNotFound);
    if (centrep)
        {
        err = centrep->Get(KSimulateUpdateAgent, simulate);
        }
    delete centrep;
    if (simulate > 0)
        {
        FLOG(_L("CFotaUpdate::UpdateL  5  - writing update.resp"));
        RFileWriteStream respstr;
        CleanupClosePushL(respstr);
        User::LeaveIfError(respstr.Replace(iFotaServer->iFs,
                KUpdateResultFile, EFileWrite));

        respstr.WriteUint32L(UPD_OK);
        respstr.WriteUint32L(15);
        respstr.WriteL(_L("UPDATE DONE!"));
        CleanupStack::PopAndDestroy(&respstr);
        }

    iFotaServer->SetStartupReason(EFotaDefault);
    
    RStarterSession starter;
    FLOG(_L("           starter->Connect"));
    User::LeaveIfError(starter.Connect());
    starter.Reset(RStarterSession::EFirmwareUpdate);
    starter.Close();
    FLOG(_L("CFotaUpdate::UpdateL() <<"));
    }

// ---------------------------------------------------------------------------
// CFotaUpdate::CheckBatteryL()
// Cheks if there's enough battery power to update
// ---------------------------------------------------------------------------
//
TBool CFotaUpdate::CheckBatteryL()
    {
    FLOG(_L("CFotaUpdate::CheckBatteryL >>"));

    TInt chargingstatus(EChargingStatusError);
    TInt batterylevel(EBatteryLevelUnknown);
    TBool enoughPower(EFalse);

    // Read battery level

    RProperty pw;
    User::LeaveIfError(pw.Attach(KPSUidHWRMPowerState, KHWRMBatteryLevel));
    User::LeaveIfError(pw.Get(batterylevel));

    if (batterylevel >= BatteryLevelAccepted)
        {
        FLOG(_L("Battery charge is above acceptable level!"));
        enoughPower = ETrue;
        }
    else
        {

        //Read charger status
        User::LeaveIfError(pw.Attach(KPSUidHWRMPowerState,
                KHWRMChargingStatus));
        User::LeaveIfError(pw.Get(chargingstatus));
        pw.Close();

        // But charger is connected, power sufficient
        if (chargingstatus == EChargingStatusCharging || chargingstatus
                == EChargingStatusAlmostComplete || chargingstatus
                == EChargingStatusChargingComplete || chargingstatus
                == EChargingStatusChargingContinued)
            {
            FLOG(
                    _L("Battery charge is below acceptable level, but charger is connected!"));
            enoughPower = ETrue;
            }
        }

    FLOG(_L("CFotaUpdate::CheckBattery, current charge = %d, ret = %d <<"),
            batterylevel, enoughPower);
    return enoughPower;
    }

void CFotaUpdate::MonitorBatteryChargeLevel()
    {
    FLOG(_L("CFotaUpdate::MonitorBatteryChargeLevel >>"));

    CancelMonitor();

    iProperty.Attach(KPSUidHWRMPowerState, KHWRMBatteryLevel);
    TInt value = 0;

    iProperty.Get(KPSUidHWRMPowerState, KHWRMBatteryLevel, value);

    FLOG(_L("Battery level at this time is %d, err = %d"), value);
    iStatus = KRequestPending;
    iProperty.Subscribe(iStatus);

    iMonitorType = BatteryLevel;
    SetActive();

    FLOG(_L("CFotaUpdate::MonitorBatteryChargeLevel <<"));
    }

void CFotaUpdate::MonitorBatteryChargingStatus()
    {
    FLOG(_L("CFotaUpdate::MonitorBatteryChargingStatus >>"));

    CancelMonitor();

    iProperty.Attach(KPSUidHWRMPowerState, KHWRMChargingStatus);
    TInt value = 0;

    iProperty.Get(KPSUidHWRMPowerState, KHWRMChargingStatus, value);

    FLOG(_L("Charging status at this time is %d, err = %d"), value);
    iStatus = KRequestPending;
    iProperty.Subscribe(iStatus);

    iMonitorType = ChargingStatus;
    SetActive();

    FLOG(_L("CFotaUpdate::MonitorBatteryChargingStatus <<"));
    }

void CFotaUpdate::CancelMonitor()
    {
    FLOG(_L("CFotaUpdate::CancelMonitor >>"));
    if (IsActive())
        {
        FLOG(_L("Cancelling...."));
        Cancel();
        }
    FLOG(_L("CFotaUpdate::CancelMonitor <<"));
    }

TBool CFotaUpdate::IsUSBConnected()
    {
    FLOG(_L("CFotaUpdate::IsUSBConnected >>"));
    
    TBool ret (EFalse);
    TUsbDeviceState state(EUsbDeviceStateUndefined);
    RUsb usbman;
    TInt err = usbman.Connect();
    if (err == KErrNone)
        {
        usbman.GetDeviceState(state);
        usbman.Close();
        }
    if (state != EUsbDeviceStateUndefined)
    	{
        ret = ETrue;
      }
    
    FLOG(_L("CFotaUpdate::IsUSBConnected, value = %d, ret = %d <<"), state, ret);
    return ret;
    }

void CFotaUpdate::RunL()
    {
    FLOG(_L("CFotaUpdate::RunL >>"));

    if (iStatus.Int() == KErrNone)
        {
        TInt value = 0;

        if (iMonitorType == BatteryLevel)
            {
            iProperty.Get(KPSUidHWRMPowerState, KHWRMBatteryLevel,
                    value);

            FLOG(_L("Battery level has changed to %d"), value);

            iFotaServer->UpdateBatteryLowInfo(
                    (value >= BatteryLevelAccepted) ? EFalse : ETrue);

            MonitorBatteryChargeLevel();
            }
        else if (iMonitorType == ChargingStatus)
            {
            iProperty.Get(KPSUidHWRMPowerState, KHWRMChargingStatus,
                    value);

            FLOG(_L("Charging status has changed to %d"), value);

            if (value == EChargingStatusCharging || value
                    == EChargingStatusAlmostComplete || value
                    == EChargingStatusChargingComplete || value
                    == EChargingStatusChargingContinued)
                {
                if (IsUSBConnected())
                    {
                    ShowDialogL(EFwUpdNotEnoughBattery);
                    }
                else
                    {
                    iFotaServer->UpdateBatteryLowInfo(EFalse);
                    }
                }
            else
                {
                iFotaServer->UpdateBatteryLowInfo(ETrue);
                }
            }

        }
    FLOG(_L("CFotaUpdate::RunL, iStatus = %d <<"), iStatus.Int());
    }

// ---------------------------------------------------------------------------
// CFotaUpdate::StartUpdateL
// Starts fw updating (shows a notifier to user). 
// ---------------------------------------------------------------------------
//
void CFotaUpdate::StartUpdateL(const TDownloadIPCParams &aParams)
    {
    FLOG(_L("CFotaUpdate::StartUpdateL,  pkig:%d >>"), aParams.iPkgId);

    //First cancel any ongoing battery monitoring.
    CancelMonitor();

    UpdateL();

    FLOG(_L("CFotaUpdate::StartUpdateL <<"));
    }

TInt CFotaUpdate::RunError(TInt aError)
    {
    FLOG(_L("CFotaUpdate::RunError, error = %d >>"), aError);
    return aError;
    }

void CFotaUpdate::DoCancel()
    {
    FLOG(_L("CFotaUpdate::DoCancel >>"));

    iProperty.Cancel();

    FLOG(_L("CFotaUpdate::DoCancel <<"));
    }

void CFotaUpdate::ShowDialogL(TFwUpdNoteTypes aDialogid)
    {
    FLOG(_L("CFotaUpdate::ShowDialogL, dialogid = %d >>"), aDialogid);
    iFotaServer->ServerCanShut(EFalse);

    if (iFotaServer->FullScreenDialog())
        iFotaServer->FullScreenDialog()->Close();

    iNotifParams = CHbSymbianVariantMap::NewL();

    HBufC* keyDialog = HBufC::NewL(10);
    CleanupStack::PushL(keyDialog);
    *keyDialog = KKeyDialog;

    CHbSymbianVariant* dialogId = CHbSymbianVariant::NewL(&aDialogid,
            CHbSymbianVariant::EInt);
    iNotifParams->Add(*keyDialog, dialogId);
    iNotifier = CFotaDownloadNotifHandler::NewL(this);

    iNotifier->LaunchNotifierL(iNotifParams, aDialogid);

    CleanupStack::PopAndDestroy();
    FLOG(_L("CFotaUpdate::ShowDialogL <<"));
    }

void CFotaUpdate::HandleDialogResponse(int response, TInt aDialogId)
    {
    FLOG(_L("CFotaUpdate::HandleDialogResponse, dialogid = %d response = %d >>"), aDialogId, response);

    if (aDialogId == EFwUpdNotEnoughBattery)
        {
        iFotaServer->FinalizeUpdate();
        }

    FLOG(_L("CFotaUpdate::HandleDialogResponse <<"));
    }

TInt CFotaUpdate::WriteUpdateBitmapL( const TDesC& aText, const TDesC& aFile)
    {
    FLOG(_L("WriteUpdateBitmapL writing %S to %S w/ txtdir"),&aText,&aFile);

    TSize   screensize = CCoeEnv::Static()->ScreenDevice()->SizeInPixels();
    TInt                width  = screensize.iWidth - KBmpMargin*2;
    TInt                height =  screensize.iHeight;

    CArrayFixSeg<TPtrC>*   lines = new CArrayFixSeg<TPtrC>(5);
    CleanupStack::PushL(lines);
    CFbsBitmap*         bitmap = new ( ELeave ) CFbsBitmap;
    CleanupStack::PushL( bitmap );
    bitmap->Create(  TSize(width,height), EColor64K );
    CFbsBitmapDevice*   device = CFbsBitmapDevice::NewL( bitmap );
    CleanupStack::PushL( device );
    const CFont* font = AknLayoutUtils::FontFromId(EAknLogicalFontPrimaryFont);
    CFbsBitGc*          context;
    User::LeaveIfError( device->CreateContext( context ) );
    CleanupStack::PushL( context );
    TInt                ascent = font->AscentInPixels();
    TInt                descent = font->DescentInPixels();
    context->UseFont ( font );
    context->Clear(); // bg color

    // Visually ordered text
    HBufC* wrappedstring = AknBidiTextUtils::ConvertToVisualAndWrapToArrayL(
            aText, width,*font, *lines);
    CleanupStack::PushL ( wrappedstring );
    TBool dirfound (ETrue);
    // direction of text, affects alignemnt
    TBidiText::TDirectionality direction = TBidiText::TextDirectionality(
            *wrappedstring, &dirfound );

    // Print visual text to bitmap
    for ( TInt i=0; i<lines->Count(); ++i ) 
        {
        TPtrC l = (*lines)[i];
        TInt top = (ascent+descent)*(i);
        TInt bot = (ascent+descent)*(i+1);
        TRect rect (0, top ,width, bot );
        CGraphicsContext::TTextAlign alignment = 
        direction==TBidiText::ELeftToRight ? CGraphicsContext::ELeft 
            : CGraphicsContext::ERight;
        context->DrawText(l, rect, ascent, alignment);
        }
    height = (ascent+descent)*lines->Count() + descent;
    bitmap->Resize( TSize(width,height));
    bitmap->Save( aFile );
    context->DiscardFont();
    CleanupStack::PopAndDestroy( wrappedstring );
    CleanupStack::PopAndDestroy( context );
    CleanupStack::PopAndDestroy( device );
    CleanupStack::PopAndDestroy( bitmap );
    CleanupStack::PopAndDestroy( lines );
    return 1;
    }
