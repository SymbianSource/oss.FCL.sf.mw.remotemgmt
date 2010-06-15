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
#include <fotaserver.rsg>
#include <centralrepository.h>
#include <AknUtils.h>
#include <AknBidiTextUtils.h> 
#include <biditext.h>
#include <gdi.h>
#include <fotaengine.h>
#include <apgtask.h>
#include <SyncMLNotifierParams.h>
#include <aknradiobuttonsettingpage.h> 
#include <akntitle.h>
#include <schtime.h>
#include <csch_cli.h>

#include "fotaupdate.h"
#include "fmsclient.h"
#include "FotaReminderDlg.h"
#include "FotasrvSession.h"
#include "fotaserverPrivateCRKeys.h"
#include "fotaserverPrivatePSKeys.h"





// ============== LOCAL FUNCTIONS ============================================



TInt WriteUpdateBitmapL( const TDesC& aText, const TDesC& aFile)
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


// ---------------------------------------------------------------------------
// PackageFileName  Creates pgk filename ,like 5.swupd
// ---------------------------------------------------------------------------
void PackageFileName( TInt aPkgid, TDes8& aFileName )
    {
    aFileName.AppendNum(aPkgid);
    aFileName.Append( KSwupdFileExt8 );
    }


// ---------------------------------------------------------------------------
// PackageFilePath  Creates pk file path, like c:\\private\\102072C4\\5.swupd
// ---------------------------------------------------------------------------
void PackageFilePath( TInt aPkgid, TDes8& aPath )
    {
    TBuf8<20> fn;
    PackageFileName ( aPkgid,fn);
    aPath.Append(KSwupdPath8);
    aPath.Append(fn);
    }

// ============================= MEMBER FUNCTIONS ============================


// ---------------------------------------------------------------------------
// CFotaUpdate::CFotaUpdate()
// ---------------------------------------------------------------------------
//
CFotaUpdate::CFotaUpdate() : CActive(EPriorityNormal) 
,iScheduledUpdate(NULL),iHandleUpdateAcceptLater(EFalse)
                {
                CActiveScheduler::Add( this ); 
                iNotifParams.iNoteType  = ESyncMLFwUpdUnknown;  
                iNotifParams.iIntParam  = 0;
                iChargeToMonitor = 0;
                iFinalizeLater = EFalse;
                }


// ---------------------------------------------------------------------------
// CFotaUpdate::~CFotaUpdate()
// ---------------------------------------------------------------------------
//
CFotaUpdate::~CFotaUpdate()
    {
    if(iScheduledUpdate)
        {
        delete iScheduledUpdate;
        iScheduledUpdate = NULL;	
        }
    iIntervalType.Close();  
    iInterval.Close();  
    }


// ---------------------------------------------------------------------------
// CFotaUpdate::NewL 
// ---------------------------------------------------------------------------
//
CFotaUpdate* CFotaUpdate::NewL (CFotaServer* aServer)
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
TBool CFotaUpdate::CheckUpdateResults( RFs& aRfs )
    {
    RFile   f;
    TInt    err;
    err = f.Open ( aRfs, KUpdateResultFile, EFileShareAny );
    f.Close();
    if ( err!=KErrNone )
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
void CFotaUpdate::ExecuteUpdateResultFileL( RFs& aRfs )
    {
    FLOG(_L("CFotaUpdate::ExecuteUpdateResultFileL >>"));
    iFs = &aRfs;
    TInt                err;
    RFileReadStream     rstr;
    TInt                result;
    TInt                msglen;
    HBufC8*             message=NULL;
    HBufC16*            message16=NULL;
    TBool               deleteData ( EFalse );

    // Open update result file
    FLOG(_L("opening rstr 1/2  "));
    err = rstr.Open(*iFs,KUpdateResultFile ,EFileRead|EFileStream);
    FLOG(_L("opened  rstr 2/2  "));
    if(err) FLOG(_L("   update result file open err %d"), err);
    User::LeaveIfError (err );
    FLOG(_L("       0.1"));
    CleanupClosePushL( rstr );
    FLOG(_L("       0.2"));

    // Read resultcode 
    result = rstr.ReadUint32L();        FLOG(_L("       0.3"));
    msglen = rstr.ReadUint32L();        FLOG(_L("       0.4  result: %d  \
    msglen:%d "),  result, msglen);
    if(msglen != 0)
        {
        FLOG(_L("       0.5  reading msg"));
        message         = HBufC8::NewLC(msglen+1);
        TPtr8  ptrdesc  = message->Des();
        TRAPD ( err2, rstr.ReadL(ptrdesc) );
        if ( err2 != KErrNone && err2 != KErrEof)
            {
            FLOG(_L("  file read err %d"),err2); User::Leave( err2 ); 
            }

        message16 = HBufC16::NewLC (message->Des().Length());
        message16->Des().Copy( *message );
        FLOG(_L("   1 update result: %d"),  result) ;
        FLOG(_L("   2 dbg msg: %S"),message16);
        CleanupStack::PopAndDestroy( message16 );
        CleanupStack::PopAndDestroy( message );
        }

    FLOG(_L("       0.6 "));
    CleanupStack::PopAndDestroy( &rstr ); 

    // Map resultcode to FUMO result code
    RFotaEngineSession::TState  fstate  
    = RFotaEngineSession::EUpdateFailed;
    RFotaEngineSession::TResult fresult 
    = RFotaEngineSession::EResUpdateFailed;

    TDriveNumber drive;
    TBool toencrypt =   iFotaServer->NeedToEncryptL(drive);

    FLOG(_L("   3"));
    switch ( result )
        {
        case UPD_OK:
            {
            fstate = RFotaEngineSession::EUpdateSuccessfulNoData;
            fresult = RFotaEngineSession::EResSuccessful;
            deleteData = ETrue;
            LaunchNotifierL ( ESyncMLFwUpdResultNote, KErrNone, toencrypt );
            }
            break;
        case UPD_INSTALL_REQUEST_IS_INVALID:
            {
            fstate = RFotaEngineSession::EUpdateFailedNoData;
            fresult = RFotaEngineSession::EResUpdateFailed;
            deleteData = ETrue;
            LaunchNotifierL ( ESyncMLFwUpdResultNote, KErrNotFound, toencrypt );
            }
            break;
        case UPD_UPDATE_PACKAGE_IS_ABSENT:
            {
            fstate = RFotaEngineSession::EUpdateFailedNoData;
            fresult = RFotaEngineSession::EResUpdateFailed;
            }
            break;
        case UPD_UPDATE_PACKAGE_IS_CORRUPTED:
            {
            fstate = RFotaEngineSession::EUpdateFailedNoData;
            fresult = RFotaEngineSession::EResCorruptedFWUPD;
            deleteData = ETrue;
            LaunchNotifierL ( ESyncMLFwUpdResultNote, KErrNotSupported, toencrypt );
            }
            break;
        case UPD_UPDATE_PACKAGE_CONTENTS_IS_INVALID:
            {
            fstate = RFotaEngineSession::EUpdateFailedNoData;
            fresult = RFotaEngineSession::EResCorruptedFWUPD;
            deleteData = ETrue;
            LaunchNotifierL ( ESyncMLFwUpdResultNote, KErrNotSupported, toencrypt );
            }
            break;
        case UPD_UPDATE_PACKAGE_IS_NOT_COMPATIBLE_WITH_CURRENT_MOBILE_DEVICE:
            {
            fstate = RFotaEngineSession::EUpdateFailedNoData;
            fresult = RFotaEngineSession::EResPackageMismatch;
            deleteData = ETrue;
            LaunchNotifierL ( ESyncMLFwUpdResultNote, KErrNotSupported, toencrypt );
            }
            break;
        case UPD_FATAL_ERROR:
            {
            fstate = RFotaEngineSession::EUpdateFailedNoData;
            fresult = RFotaEngineSession::EResUpdateFailed;
            deleteData = ETrue;
            LaunchNotifierL ( ESyncMLFwUpdResultNote, KErrNotFound, toencrypt );
            }
            break;

        default:
            {
            FLOG(_L("   3.1 invalid result: %d"), result);
            LaunchNotifierL ( ESyncMLFwUpdResultNote, KErrNotFound, toencrypt );
            }
            break;      
        }

    // Find the state 60 (update  progressing) -> 100 (etc)
    RArray<TInt>    states;
    TPackageState   state;
    CleanupClosePushL (states);
    iFotaServer->iDatabase->OpenDBL();
    iFotaServer->iDatabase->GetAllL(states);

    FLOG(_L("   4.1 found %d states "),states.Count() );
    for(TInt i=0;i<states.Count(); ++i ) 
        {
        TPackageState   tmp;
        tmp = iFotaServer->iDatabase->GetStateL(  states[i]  );
        FLOG(_L("   5 got state "));
        if ( tmp.iState == RFotaEngineSession::EUpdateProgressing )
            {
            state = tmp;
            state.iState    = fstate;
            state.iResult   = fresult;
            FLOG(_L("   6 Updating state id %d  to %d ,result %d  ")
                    ,state.iPkgId, state.iState,state.iResult );
            iFotaServer->iDatabase->SetStateL(state,KNullDesC8
                    ,EFDBState|EFDBResult ) ;
            }
        }
    iFotaServer->iDatabase->CloseAndCommitDB();
    CleanupStack::PopAndDestroy(&states);


    // Delete request file
    err = BaflUtils::DeleteFile ( *iFs, KUpdateRequestFile );
    if ( err != KErrNone && err != KErrNotFound ) 
        {
        FLOG(_L("   6.1  req file deleted, err %d"), err);
        User::Leave(err); 
        } 

    // Write timestamp (shown to user in device management ui)
    if( fresult == RFotaEngineSession::EResSuccessful )
        {
        RFileWriteStream    wstr;
        TTime               time;
        User::LeaveIfError( wstr.Replace( *iFs, KUpdateTimeStampFileName
                ,EFileWrite ) );
        CleanupClosePushL ( wstr );
        time.HomeTime();
        TInt                year   = time.DateTime().Year();
        TInt                month  = time.DateTime().Month();    
        TInt                day    = time.DateTime().Day();    
        TInt                hour   = time.DateTime().Hour();    
        TInt                minute = time.DateTime().Minute();    
        wstr.WriteInt32L(year) ;
        wstr.WriteInt32L(month) ;
        wstr.WriteInt32L(day) ;
        wstr.WriteInt32L(hour) ;
        wstr.WriteInt32L(minute) ;   
        CleanupStack::PopAndDestroy (1); // wstr
        }
    // Delete package content
    if ( deleteData  && state.iPkgId > KErrNotFound )
        {
        iFotaServer->StoragePluginL()->DeleteUpdatePackageL( state.iPkgId ) ;
        }


    // Report state back to syncml server
    if ( state.iPkgId >= 0 )
        {
        FLOG(_L("   6.1 creating device mgmt session for profile %d")
                ,state.iProfileId);
        iFotaServer->CreateDeviceManagementSessionL( state );
        }
    else 
        {
        FLOG(_L(" No state found in 'update progress' mode! cannot report\
        status to DM server "));
        }

    FLOG(_L("CFotaUpdate::ExecuteUpdateResultFileL <<"));
    }

// ---------------------------------------------------------------------------
// CFotaUpdate::DeleteUpdateResultsL
// ---------------------------------------------------------------------------
//
void CFotaUpdate::DeleteUpdateResultFileL( RFs& aRfs )
    {
    FLOG(_L("CFotaUpdate::DeleteUpdateResultsL "));
    // Delete result file
    BaflUtils::DeleteFile( aRfs, KUpdateResultFile );
    BaflUtils::DeleteFile( aRfs, KUpdateRequestFile );
    BaflUtils::DeleteFile( aRfs, KUpdateBitmap );
    BaflUtils::DeleteFile( aRfs, KRestartingBitmap );
    }


// ---------------------------------------------------------------------------
// CFotaUpdate::RunL
//  Handles user selection (button press: accept/ cancel update)
// ---------------------------------------------------------------------------
//
void CFotaUpdate::RunL()
    {
    FLOG(_L( "[FotaServer] CFotaUpdate::RunL() >> %d"  ),iStatus.Int() );
    TBool       handled( EFalse );
    User::LeaveIfError(iNotifier.CancelNotifier(KSyncMLFwUpdNotifierUid));
    iNotifier.Close();

    if (iStatus.Int() == KErrNotFound )
        {
        FLOG(_L("Exiting...."));
        TApaTaskList taskList(iFotaServer->GetEikEnv()->WsSession());
        TApaTask task=taskList.FindApp(TUid::Uid(KOmaDMAppUid));
        if(task.Exists())
            {
            task.EndTask();
            }
        return;

        }
    // Handle update start query
    if ( iNotifParams.iNoteType == ESyncMLFwUpdStartQuery )
        {
        FLOG(_L("   update start query"));
        // User pressed accept
        if ( iStatus.Int() == KErrNone )
            {
            handled = ETrue;
            HandleUpdateAcceptStartL();
            }
        // User pressed later
        else if ( iStatus.Int() == KErrCancel )
            {

            FLOG(_L("USER PRESSED LATER"));
            handled = ETrue;
            HandleUpdateAcceptLaterL();
            }
        }
    if ( iNotifParams.iNoteType == ESyncMLFwUpdStartQueryEnc )
        {
        FLOG(_L("   update start query"));

        if ( iStatus.Int() == KErrNone )
            {
            FLOG(_L(" user accepted query to decrypt memory"));
            handled = ETrue;
            TRAPD(err, iFotaServer->DoStartDecryptionL());
            
            if (err == KErrNotReady)
                {
                HandleUpdateErrorL(err);
                }
            }
        else if (iStatus.Int() == KErrCancel )
            {
            FLOG(_L("User pressed cancel. Don't decrypt. Stop."));
            handled = ETrue;
            LaunchNotifierL( ESyncMLFwUpdNoReminder , KErrNone );
            }

        }
    // Handle battery error
    if ( iNotifParams.iNoteType == ESyncMLFwUpdErrorNote )
        {

        if (iNotifParams.iIntParam == KErrBadPower || iNotifParams.iIntParam == KErrBadUsbPower)
            {
            FLOG(_L("       battery note"));
            if ( iStatus.Int() == KErrNone )
                {
                handled = ETrue;

                // Still check batt. (user might have connected charger)

                HandleErrorBasedonChargingTypeL();

                }
            else if (iNotifParams.iIntParam == KErrDeviceMemoryBusy)
                {
                FLOG(_L("Do nothing. Stop the update."));
                iFotaServer->FinalizeUpdateL();
                }
            else if (iNotifParams.iIntParam == KErrBadPowerForEnc)
                {
                if (iFinalizeLater)
                    {
                    FLOG(_L("Finalizing update now..."));
                    iFinalizeLater = EFalse;
                    iFotaServer->FinalizeUpdateL();
                    }
                }
            }
        }

    //  Handle update complete notification
    if ( iNotifParams.iNoteType == ESyncMLFwUpdResultNote )
        {
        handled = ETrue;
        DeleteUpdateResultFileL( iFotaServer->iFs );
        if (!iNotifParams.iEncryptReq)
            {
            FLOG(_L("Going to finalize update..."));
            iFotaServer->FinalizeUpdateL();
            }

        }
    if ( iNotifParams.iNoteType == ESyncMLFwUpdNoReminder )
        {
        FLOG(_L(" No Reminder selected"));
        if ( iStatus.Int() == KErrNone )
            {
            handled = ETrue;

            FindScheduleL( ETrue );
            iUpdateState.iState = RFotaEngineSession::EStartingUpdate;
            iFotaServer->iDatabase->OpenDBL();
            iFotaServer->iDatabase->SetStateL(iUpdateState ,KNullDesC8,EFDBState);
            iFotaServer->iDatabase->CloseAndCommitDB();

            // Report state back to syncml server
            if ( iUpdateState.iPkgId >= 0 && iUpdateState.iSendAlert )
                {
                FLOG(_L("   creating device mgmt session for profile %d")
                        ,iUpdateState.iProfileId);
                iFotaServer->CreateDeviceManagementSessionL( iUpdateState );
                }
            iFotaServer->FinalizeUpdateL();
            }
        }
    if ( iNotifParams.iNoteType == ESyncMLFwUpdResultNote && iNotifParams.iEncryptReq)
        {
        if (iStatus.Int() == KErrNone)
            {
            FLOG(_L("User wanted to start encryption..."));
            iFotaServer->DoStartEncryptionL();
            }
        else 
            {
            FLOG(_L("User do not want to start encryption."));
            }
        if (!iFinalizeLater)
            iFotaServer->FinalizeUpdateL();
        }
    if ( !handled ) 
        {
        FLOG(_L("CANNOT INTERPRET NOTIFIER RESULT CODE: %d"), iStatus.Int() );
        }

    FLOG(_L( "[FotaServer] CFotaUpdate::RunL() <<" ) );
    }





// ---------------------------------------------------------------------------
//  CFotaUpdate::HandleUpdateAcceptStartL()
//  Handles user selection of NOW
// ---------------------------------------------------------------------------
//
void CFotaUpdate::HandleUpdateAcceptStartL(TBool aSkipBatteryChk)
    {
    FLOG(_L("USER PRESSED ACCEPT"));

    TBool	enoughPower (EFalse);
    TInt usbchargingsupport =0;
    if (aSkipBatteryChk)
        {
        enoughPower = ETrue;
        }
    else
        {
        CRepository* centrep = NULL;
        //centrep = CRepository::NewLC( TUid::Uid(KFotaServerUid));
        TRAPD(err, centrep = CRepository::NewL( TUid::Uid(KFotaServerUid)));
        if(err)
            {
            FLOG(_L("reading from cenrep failed"));
            }

        if(centrep)
            {centrep->Get( KFotaUSBChargerEnabled , usbchargingsupport );
            if(usbchargingsupport != 0 && usbchargingsupport != 1)
                usbchargingsupport =0;
            delete centrep;
            } 
        centrep = NULL;
        enoughPower = CheckBatteryL(usbchargingsupport);
#if defined(__WINS__)
        enoughPower = ETrue;
#endif
        }
    // Failure due to low battery
    if ( !enoughPower )
        {
        FLOG(_L("Launching low battery notifier"));
        if(!usbchargingsupport)
            LaunchNotifierL( ESyncMLFwUpdErrorNote, KErrBadPower );
        else
            LaunchNotifierL( ESyncMLFwUpdErrorNote , KErrBadUsbPower );
        }            
    else
        {
        UpdateL();
        }   
    }


void CFotaUpdate::HandleUpdateErrorL(TInt aError, TInt aValue)
    {
    FLOG(_L("CFotaUpdate::HandleUpdateErrorL, error = %d, value = %d >>"), aError, aValue);

    switch (aError)
        {
        case KErrNotReady: //Called from within StartUpdate
            {
            LaunchNotifierL(ESyncMLFwUpdErrorNote,KErrDeviceMemoryBusy);
            }
            break;
        case KErrBadPower: //Called from fotaserver when battery power is not enough
            {
            iChargeToMonitor = aValue;
            CRepository* centrep = NULL;
            TRAPD(err, centrep = CRepository::NewL( TUid::Uid(KFotaServerUid)));
            if(err)
                {
                FLOG(_L("reading from cenrep failed"));
                }
            TInt usbchargingsupport =0;
            if(centrep)
                {centrep->Get( KFotaUSBChargerEnabled , usbchargingsupport );
                if(usbchargingsupport != 0 && usbchargingsupport != 1)
                    usbchargingsupport =0;
                delete centrep;
                } 
            centrep = NULL;
            FLOG(_L("Launching low battery notifier, usbsupport = %d, state = %d"), usbchargingsupport, iUpdateState.iState);
            if(!usbchargingsupport )
                {
                FLOG(_L("Here 1"));
                if (iUpdateState.iState == RFotaEngineSession::EStartingUpdate)
                    {
                    FLOG(_L("Here 2"));
                    LaunchNotifierL( ESyncMLFwUpdErrorNote, KErrBadPower );
                    }
                }
            else
                {
                LaunchNotifierL( ESyncMLFwUpdErrorNote , KErrBadUsbPower );
                }

            }
            break;
        default:
            {
            //Do nothing
            }
        }


    FLOG(_L("CFotaUpdate::HandleUpdateErrorL <<"));
    }

void CFotaUpdate::HandleEncryptionErrorL(TInt aError)
    {
    FLOG(_L("CFotaUpdate::HandleEncryptionErrorL, error = %d >>"), aError);

    if (aError == KErrBadPower)
        {
        iFinalizeLater = ETrue;
        LaunchNotifierL(ESyncMLFwUpdErrorNote, KErrBadPowerForEnc);
        }
    else 
        {
        FLOG(_L("Unknown error. Ignored"));
        }

    FLOG(_L("CFotaUpdate::HandleEncryptionErrorL <<"));
    }


// ---------------------------------------------------------------------------
//  CFotaUpdate::HandleUpdateAcceptLaterL
//  Handles user selection of Later
// ---------------------------------------------------------------------------
void CFotaUpdate::HandleUpdateAcceptLaterL()
    {
    TInt fotaUpdateAppName (EFotaUpdateDM);
    TInt err = RProperty::Get( KPSUidNSmlDMSyncApp, KNSmlCurrentFotaUpdateAppName, fotaUpdateAppName );
    FLOG(_L("Setting iHandleUpdateAcceptLater to True"));

    if (!err && ((fotaUpdateAppName == EFotaUpdateNSC) || (fotaUpdateAppName == EFotaUpdateNSCBg )))
        {
        iFotaServer->FinalizeUpdateL();		//make sure it is not called 2 times				
        }
    else
        {
        iHandleUpdateAcceptLater = ETrue;
        ShowReminderDialogL();				
        }
    }


// ---------------------------------------------------------------------------
//  CFotaUpdate::HandleErrorBasedonChargingTypeL
//  Handles the error based on charger type supported by the device
// ---------------------------------------------------------------------------

void CFotaUpdate::HandleErrorBasedonChargingTypeL()
    {
    FLOG(_L("CFotaUpdate::HandleErrorBasedonChargingTypeL >>"));

    if (iUpdateState.iState == RFotaEngineSession::EStartingUpdate)
        {
        if (iNotifParams.iIntParam == KErrBadUsbPower)
            {  
            FLOG(_L("handling usb charger error "));
            SetUpdateFailedStatesL();
            FLOG(_L("calling for monitoring  the battery"));
            iFotaServer->MonitorBattery(iChargeToMonitor);
            iFotaServer->FinalizeUpdateL();
            }
        else
            {
            TInt usbcharger = KErrNone; 
            TBool enoughPower = CheckBatteryL(usbcharger);

#if defined(__WINS__)
            //It will not come here, just used for testing.
            enoughPower = ETrue;
#endif

            if (enoughPower)
                {
                //User has connected the charger
                FLOG(_L("User has connected the charger"));
                UpdateL();
                }
            else
                {
                FLOG(_L("User hasn't connected the charger"));
                //User hasn't connected the charger, hence notify user to update later.
                SetUpdateFailedStatesL();
                //Call reminder here
                if(!iHandleUpdateAcceptLater)
                    {
                    FLOG(_L("calling HandleUpdateAcceptLaterL"));
                    HandleUpdateAcceptLaterL();
                    }
                iHandleUpdateAcceptLater = EFalse;
                }
            }
        }
    else //must be during encryption after update
        {
        if (iNotifParams.iIntParam == KErrBadUsbPower)
            {
            iFotaServer->MonitorBattery(iChargeToMonitor);
            }
        else
            {
            //nothing to do for dynamo charging
            }
        iFotaServer->FinalizeUpdateL();
        }
    FLOG(_L("CFotaUpdate::HandleErrorBasedonChargingTypeL <<"));
    }

// ---------------------------------------------------------------------------
//  CFotaUpdate::SetUpdateFailedStatesL
//  Set the state to update failed
// ---------------------------------------------------------------------------

void CFotaUpdate::SetUpdateFailedStatesL()
    {
    FLOG(_L("CFotaUpdate::SetUpdateFailedStatesL >>"));

    iUpdateState.iState = RFotaEngineSession::EStartingUpdate;
    iUpdateState.iResult= RFotaEngineSession::EResUpdateFailed;
    iFotaServer->iDatabase->OpenDBL();
    iFotaServer->iDatabase->SetStateL( iUpdateState ,KNullDesC8
            ,EFDBState|EFDBResult );
    iFotaServer->iDatabase->CloseAndCommitDB();

    FLOG(_L("CFotaUpdate::SetUpdateFailedStatesL <<"));

    }


// ---------------------------------------------------------------------------
// CFotaUpdate::DoCancel()
// ---------------------------------------------------------------------------
//
void CFotaUpdate::DoCancel()
    {
    FLOG(_L("CFotaUpdate::DoCancel() >>"));
    iNotifier.CancelNotifier(KSyncMLFwUpdNotifierUid);
    iNotifier.Close();
    FLOG(_L("CFotaUpdate::DoCancel() <<"));
    }

// ---------------------------------------------------------------------------
// CFotaUpdate::RunError(TInt aError)
// ---------------------------------------------------------------------------
//
TInt CFotaUpdate::RunError(TInt aError)
    {
    FLOG(_L("CFotaUpdate::RunError >> err %d"),aError);
    FLOG(_L("CFotaUpdate::RunError << err %d"),aError);
    return KErrNone;
    }

// ---------------------------------------------------------------------------
// CSyncMLNTestAppAO::LaunchNotifierL
// Shows one of the following queries: "update now?", "update success",
//  "update fail", "too low battery"
// ---------------------------------------------------------------------------
void CFotaUpdate::LaunchNotifierL( const TSyncMLFwUpdNoteTypes aNotetype
        ,const TInt aIntParam, const TBool aEnc )
    {
    FLOG(_L("CFotaUpdate::LaunchNotifierL() >>  prof.Id: %d")
            ,iUpdateState.iProfileId);
    if (!IsActive())
        {
        TSyncMLFwUpdNotifParams         params;
        params.iNoteType        = aNotetype;
        params.iIntParam        = aIntParam;
        params.iEncryptReq = aEnc;
        TSyncMLFwUpdNotifParamsPckg     pckg(params);
        iNotifParams.iNoteType  = params.iNoteType;
        iNotifParams.iIntParam  = params.iIntParam;
        iNotifParams.iEncryptReq = params.iEncryptReq;
        iDummyResponsePckg = TSyncMLFwUpdNotifRetValPckg();
        User::LeaveIfError( iNotifier.Connect() );
        iNotifier.StartNotifierAndGetResponse( iStatus,KSyncMLFwUpdNotifierUid
                , pckg, iDummyResponsePckg );
        SetActive();
        }
    FLOG(_L("CFotaUpdate::LaunchNotifierL() <<"));
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
    iUpdateState.iState  = RFotaEngineSession::EUpdateProgressing;
    iFotaServer->iDatabase->OpenDBL();
    iFotaServer->iDatabase->SetStateL( iUpdateState ,KNullDesC8, EFDBState );
    iFotaServer->iDatabase->CloseAndCommitDB();

    // Write update request for update agent..............
    FLOG(_L("CFotaUpdate::UpdateL  1 writing update.req "));
    TBuf8<150>           dp2filepath;
    HBufC16*             dp2;
    RFileWriteStream    wstr;
    CleanupClosePushL ( wstr );
    FLOG(_L("CFotaUpdate::UpdateL  2 getting pkg location"));
    iFotaServer->StoragePluginL()->GetUpdatePackageLocationL( 
            iUpdateState.iPkgId,  dp2filepath  );
    FLOG(_L("CFotaUpdate::UpdateL  3 craeting update.req") );
    User::LeaveIfError( wstr.Replace( iFotaServer->iFs
            ,KUpdateRequestFile, EFileWrite ) );
    wstr.WriteInt16L( 1 ); // version number is  1
    wstr.WriteInt32L( 1 ); // count   of cmds is 1
    wstr.WriteInt16L( 0 ); // requestid is 0

    dp2 = HBufC16::NewLC ( dp2filepath.Length( ) );
    dp2->Des().Copy( dp2filepath );
    wstr.WriteInt32L( dp2->Des().Length() + 1 ); // length of filename + null
    wstr.WriteL( dp2->Des() );
    wstr.WriteInt16L( 0 ); // null character
    CleanupStack::PopAndDestroy( dp2 );
    CleanupStack::PopAndDestroy( &wstr ); // wstr

    FLOG(_L("CFotaUpdate::UpdateL  4 craeting update.bmp") );

    // Write update graphic for update agent ...............
    HBufC*              updatetxt;
    HBufC*              restarttxt;
    updatetxt  = StringLoader::LoadLC( R_APPS_FOTA_UPDATING );
    restarttxt = StringLoader::LoadLC( R_APPS_FOTA_RESTARTING );
    WriteUpdateBitmapL( updatetxt->Des(), KUpdateBitmap );
    WriteUpdateBitmapL( restarttxt->Des(), KRestartingBitmap ); 
    CleanupStack::PopAndDestroy( restarttxt );
    CleanupStack::PopAndDestroy( updatetxt );
    TRAPD(error,FindScheduleL( ETrue)); //deleting the schedule (fix for failed update) 
    if(error)
        {
        FLOG(_L("CFotaUpdate::UpdateL  error in deleting previous schedule") );	

        }  
    // Simulate update agent by writing result file.
    CRepository* centrep( NULL);
    TInt err = KErrNone;
    TRAP(err, centrep = CRepository::NewL( KCRUidFotaServer ) );
    TInt simulate(KErrNotFound);
    if ( centrep ) 
        {
        err = centrep->Get( KSimulateUpdateAgent, simulate );
        }
    delete centrep;
    if ( simulate>0 )
        {   
        FLOG(_L("CFotaUpdate::UpdateL  5  - writing update.resp"));
        RFileWriteStream    respstr;
        CleanupClosePushL ( respstr );
        User::LeaveIfError( respstr.Replace( iFotaServer->iFs, KUpdateResultFile
                , EFileWrite ) );

        respstr.WriteUint32L(UPD_OK);
        respstr.WriteUint32L(15);
        respstr.WriteL(_L("UPDATE DONE!"));
        CleanupStack::PopAndDestroy( &respstr );
        }

    CRepository* centrep2 = NULL;
    TRAP( err, centrep2 = CRepository::NewL( KCRUidFotaServer ) )
    if (err==KErrNone ) 
        {
        centrep2->Set(  KFotaUpdateState, EFotaPendingGenAlert );
        }
    delete centrep2;

    // Boot to update mode 
    LaunchNotifierL( ESyncMLFwUpdRebootNote, KErrNone);

    FLOG(_L("   waiting 2 sec to keep 'restarting' visible"));
    User::After(TTimeIntervalMicroSeconds32(2000000));

    RStarterSession     starter;
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
TBool CFotaUpdate::CheckBatteryL(TBool aUSBChargingsupported)
    {
    TInt        chargingstatus( EChargingStatusError );
    TInt        batterylevel( 1 );
    TBool       enoughPower( EFalse );
    // Read battery
    FLOG(_L("   0.9 checking batt level"));
    RProperty   pw;
    User::LeaveIfError(pw.Attach(KPSUidHWRMPowerState,KHWRMBatteryLevel));
    User::LeaveIfError(pw.Get( batterylevel ));
    pw.Close();
    if(aUSBChargingsupported)
        {
        if ( batterylevel >= EBatteryLevelLevel2 ) 
            enoughPower = ETrue;

        //return enoughPower; 
        }
    else
        {
        User::LeaveIfError(pw.Attach(KPSUidHWRMPowerState,KHWRMChargingStatus));
        User::LeaveIfError(pw.Get( chargingstatus ));
        pw.Close();

        // Too low battery, power insufficient
        if ( batterylevel >= EBatteryLevelLevel2 ) 
            {
            enoughPower = ETrue;
            }
        // But charger is connected, power sufficient
        if ( chargingstatus != EChargingStatusError
                && chargingstatus != EChargingStatusNotConnected )
            {
            enoughPower = ETrue;
            }
        }

    if (!enoughPower)
        iChargeToMonitor = EBatteryLevelLevel2;
    FLOG(_L("   1.0 batt level: %d  (0..7), chargingstatus %d")
            ,batterylevel,chargingstatus);
    FLOG(_L("CFotaUpdate::checkBattery %d"), enoughPower?1:0 );
    return enoughPower;
    }


// ---------------------------------------------------------------------------
// CFotaUpdate::StartUpdateL
// Starts fw updating (shows a notifier to user). 
// ---------------------------------------------------------------------------
//
void CFotaUpdate::StartUpdateL( const TDownloadIPCParams &aParams )
    {
    FLOG(_L("CFotaUpdate::StartUpdateL(TDownloadIPCParams aParams) >> \
    pkig:%d"), aParams.iPkgId );
    TSmlProfileId profile(KErrNotFound);

    // Get update state from db
    iFotaServer->iDatabase->OpenDBL( );
    iUpdateState = iFotaServer->iDatabase->GetStateL( aParams.iPkgId );
    iFotaServer->iDatabase->CloseAndCommitDB( );

    FLOG(_L("   0 "));
    if ( iUpdateState.iPkgId == KErrNotFound )
        {
        FLOG(_L("  STATE NOT FOUND pkgid %d"), aParams.iPkgId);
        User::Leave(KErrNotFound);
        }

    // Resolve profile id
    RSyncMLSession sml;
    TInt                    tmp1;
    TSmlServerAlertedAction tmp2;
    TBool	                tmp3;
    TInt					tmp4;
    CleanupClosePushL( sml );
    sml.OpenL();
    // Use stored profile
    if ( aParams.iProfileId == KErrNotFound )
        {
        iFotaServer->GetProfileDataL( &sml,iUpdateState.iProfileId
                ,tmp1,tmp2,tmp3,tmp4);
        profile = iUpdateState.iProfileId;
        }
    // Use passed profile. If error, then try to use stored profile
    else
        {
        TRAPD(err, iFotaServer->GetProfileDataL( &sml,aParams.iProfileId
                ,tmp1,tmp2, tmp3, tmp4 ) );
        if ( err != KErrNone )
            {
            iFotaServer->GetProfileDataL( &sml,iUpdateState.iProfileId
                    ,tmp1,tmp2,tmp3,tmp4);
            profile = iUpdateState.iProfileId;
            }
        else
            {
            profile = aParams.iProfileId;
            }
        }
    CleanupStack::PopAndDestroy( &sml );

    // Must update package state with fresh params (it could be empty)
    TUint fields = EFDBState | EFDBPkgName|EFDBVersion;
    if ( profile != iUpdateState.iProfileId )
        {  
        fields |= EFDBProfileId;
        }

    iUpdateState.iProfileId     = profile;
    iUpdateState.iPkgName       = aParams.iPkgName;
    iUpdateState.iPkgVersion    = aParams.iPkgVersion;
    iUpdateState.iState         = RFotaEngineSession::EStartingUpdate;
    iUpdateState.iResult		= KErrNotFound;
    iUpdateState.iSendAlert     = EFalse; // HJHA-6MJCFE . This operation will
    // not reach any final state.
    iFotaServer->iDatabase->OpenDBL( );
    iFotaServer->iDatabase->SetStateL( iUpdateState, KNullDesC8, fields );
    iFotaServer->iDatabase->CloseAndCommitDB( );

    CRepository* centrep2 = NULL;
    TRAPD( err, centrep2 = CRepository::NewL( KCRUidFotaServer ) )
    if (err==KErrNone ) 
        {  TInt val;
        //
        centrep2->Get(  KFotaUpdateState, val );

        if(val == EFotaUpdateInterrupted)
            centrep2->Set(  KFotaUpdateState, EFotaDefault );
        }
    delete centrep2;

    FLOG(_L("CFotaUpdate::StartUpdateL  2"));
    //new code
    	 TInt client = iFotaServer->GetInstallUpdateClientL();
       if( CheckUpdateVariations() && 
   			client!= CFotaSrvSession::EOMADMAppUi &&  
         client != CFotaSrvSession::EFMSServer ) 
       {
       TInt callactive(EFalse);
       RFMSClient fmsclient;    
       TRAPD(err,fmsclient.OpenL());
       if(err == KErrNone)
           {
           CleanupClosePushL(fmsclient);
           FLOG(_L("CFotaUpdate::startupdate FMS for active call >>"));                
           TInt err1 = fmsclient.IsPhoneCallActive(callactive);
           FLOG(_L("IsPhoneCallActive returns %d  >>"),err1);       
           }
       if(callactive)
           {
           FLOG(_L(" active phone call found"));           
           
           FLOG(_L("putting call end Ao start pkgid=%d, profileid=%d"),iUpdateState.iPkgId,iUpdateState.iProfileId);
           TInt err1 = fmsclient.MonitorActivePhoneCallEnd(iUpdateState.iPkgId,iUpdateState.iProfileId);
           FLOG(_L("MonitorActivePhoneCallEnd returns %d  >>"),err1);    
           }
       else
           {
           FLOG(_L("CFotaUpdate::StartUpdateL  2"));
           
           TDriveNumber drive;

    TBool ret (EFalse);
    TRAPD(err2, ret = iFotaServer->NeedToDecryptL(iUpdateState.iPkgId, drive));
    if ( err2 == KErrNone )
        {
        if (!ret)
            {
            //Drive is not encrypted
            LaunchNotifierL( ESyncMLFwUpdStartQuery, iUpdateState.iProfileId );
            }
        else
            {
            //Drive is encrypted.
            LaunchNotifierL( ESyncMLFwUpdStartQueryEnc, iUpdateState.iProfileId );
            }
        }
    else
        {
        HandleUpdateErrorL(err2);
        }
       
           }
       if(err == KErrNone) //fms opening error
           {
           CleanupStack::PopAndDestroy(&fmsclient);
           }
       }
   else
       {
       FLOG(_L("CFotaUpdate::StartUpdateL  2 DM UI Install update"));
       //LaunchNotifierL( ESyncMLFwUpdStartQuery, iUpdateState.iProfileId );
       
           TDriveNumber drive;

    TBool ret (EFalse);
    TRAP(err, ret = iFotaServer->NeedToDecryptL(iUpdateState.iPkgId, drive));
    if ( err == KErrNone )
        {
        if (!ret)
            {
            //Drive is not encrypted
            LaunchNotifierL( ESyncMLFwUpdStartQuery, iUpdateState.iProfileId );
            }
        else
            {
            //Drive is encrypted.
            LaunchNotifierL( ESyncMLFwUpdStartQueryEnc, iUpdateState.iProfileId );
            }
        }
    else
        {
        HandleUpdateErrorL(err);
        }
       
       }                   

    //new code
    

    FLOG(_L("CFotaUpdate::StartUpdateL(TDownloadIPCParams aParams) << \
    pkig:%d"),aParams.iPkgId);
    }



// ---------------------------------------------------------------------------
// CFotaUpdate::CreateRadionButtonArrayLC
// Creates the array of radio buttons to display 
// on radio button setting page
// ---------------------------------------------------------------------------
//

CDesCArray* CFotaUpdate::CreateRadionButtonArrayLC( const TScheduleEntryInfo2 aScEntry,
        TInt& aSelectionIndex)
    {
    // selection defaults to 0
    FLOG(_L("CFotaUpdate::CreateRadionButtonArrayLC >>"));  
    aSelectionIndex = 0;
    CDesCArray* array = new (ELeave) CDesC16ArrayFlat(4);
    CleanupStack::PushL(array);
    for (TInt i=0;i<iInterval.Count();i++)   

        {    
        switch (iIntervalType[i])
            {
            case EHourly :
                {

                HBufC* string1 ;
                if(iInterval[i] == 1)
                    string1 = StringLoader::LoadLC(R_FOTA_RADIO_DIALOG_1HOUR_REMAINDER);
                else
                    string1 = StringLoader::LoadLC(R_QTN_FOTA_REMINDER_HOURS , iInterval[i] );

                array->AppendL( string1->Des() );
                CleanupStack::PopAndDestroy( string1 );
                if(aScEntry.Interval() == iInterval[i] && aScEntry.IntervalType()== EHourly ) 
                    aSelectionIndex = i; 
                }
                break; 

            case EDaily:
                {

                HBufC* string1; //= StringLoader::LoadLC(R_QTN_FOTA_REMINDER_DAYS , iInterval[i] );
                if(iInterval[i] == 1)
                    string1 = StringLoader::LoadLC(R_FOTA_RADIO_DIALOG_1DAY_REMAINDER);
                else
                    string1 = StringLoader::LoadLC(R_QTN_FOTA_REMINDER_DAYS , iInterval[i] );

                array->AppendL( string1->Des() );
                CleanupStack::PopAndDestroy( string1 );
                if(aScEntry.Interval() == iInterval[i] && aScEntry.IntervalType()== EDaily ) 
                    aSelectionIndex = i;
                }
                break;
            case EMonthly:
                {

                HBufC* string1 ;//== StringLoader::LoadLC(R_QTN_FOTA_REMINDER_MINUTES , iInterval[i] );
                if(iInterval[i] == 1)
                    string1 = StringLoader::LoadLC(R_QTN_FOTA_REMINDER_1_MINUTE);
                else
                    string1 = StringLoader::LoadLC(R_QTN_FOTA_REMINDER_MINUTES , iInterval[i] );

                array->AppendL( string1->Des() );
                CleanupStack::PopAndDestroy( string1 );
                if(aScEntry.Interval() == iInterval[i] && aScEntry.IntervalType()== EMonthly) 
                    aSelectionIndex = i;
                }
                break; 

                /*case EYearly:
    		     {
    		     tempChar.Num(iInterval[i]);
    		     HBufC* string1 = StringLoader::LoadLC(R_FOTA_RADIO_DIALOG_YEAR_REMAINDER , iInterval[i] );
               	 array->AppendL( string1->Des() );
               	 CleanupStack::PopAndDestroy( string1 );
               	 if(aScEntry.Interval() == iInterval[i] ) 
               	          aSelectionIndex = i;
    		     }
    		     break;*/
            }
        }

    if(iIntervalType[iIntervalType.Count()-1] == ENoReminderOn )
        {
        HBufC* string1 = StringLoader::LoadLC( R_FOTA_RADIO_DIALOG_NO_REMAINDER);
        array->AppendL( string1->Des() );
        CleanupStack::PopAndDestroy( string1 );
        }
    FLOG(_L("CFotaUpdate::CreateRadionButtonArrayLC <<"));  
    return array;
    }
// ---------------------------------------------------------------------------
// CFotaUpdate::ParseScheduleString()
// Parse the schedule string from cenrep
// ---------------------------------------------------------------------------
//
//
void CFotaUpdate::ParseScheduleStringL()
    {    
    FLOG(_L("CFotaUpdate::ParseScheduleString() >>")); 

    GetSchedulesInfoL();



    iInterval.Reset();
    iIntervalType.Reset();
    TBuf<5> aTime;
    TBuf<9> aSched,tempChar;
    aTime.Zero();
    aSched.Zero();


    //TText*sep =   (TText *)ksep.Ptr();
    TBufC<1>ksep(Ksep);
    TText *t = (TText *)ksep.Ptr();      //just for Initialization
    for(TInt i = 0; i<iscInfo.Length();i++)
        {

        TChar ch = iscInfo[i];
        if ( ch.IsDigit())            //separates digits from the string
            {  aTime.Append(ch); 

            }
        else
            {  
            aSched.Append(ch);             
            *t = ch;
            if (*t ==     'h' || *t =='H')      //hour
                { iIntervalType.Append(EHourly) ;
                TInt val =1 ; 
                TLex  lex(aTime);
                TInt err = lex.Val(val);
                if (err)
                    {
                    iInterval.Append(1);
                    }
                else
                    {
                    iInterval.Append(val); 	
                    }

                }
            else if (*t ==     'd'|| *t =='D')      //daily
                {iIntervalType.Append(EDaily);
                TInt val =1 ; 
                TLex  lex(aTime);
                TInt err = lex.Val(val);
                if (err)
                    {
                    iInterval.Append(1);
                    }
                else
                    {
                    iInterval.Append(val);  	
                    }

                }
            else if (*t ==     'm'||  *t =='M')   //minute
                {iIntervalType.Append( EMonthly);
                TInt val =1 ; 
                TLex  lex(aTime);
                TInt err = lex.Val(val);
                if (err)
                    {

                    iInterval.Append(1);
                    }
                else
                    {
                    iInterval.Append(val);  	
                    }

                }
            /* else if (*t ==     'y')   //year
                  {iIntervalType.Append( EYearly);
                    TInt val =1 ; 
               	    TLex  lex(aTime);
               	    TInt err = lex.Val(val);
               	    if (err)
               	    {

                        iInterval.Append(1);    
               	    }
               	    else
               	    {
               	     iInterval.Append(val); 	
               	    }

                  }*/
            else if (*t == ':')   //colon
                {
                aTime.Zero();
                aSched.Zero();
                }
            }
        }
    TBool  checkSchInfo =EFalse;

    if(iInterval.Count() == iIntervalType.Count() )
        {  
        for (TInt j =0 ;j< iInterval.Count();j++ )
            {                                                 //setting to default 1h:4h:1d:3d
            if(iInterval[j] < 0 || iIntervalType[j] > 3 ) 
                {   
                iInterval.Reset();    iIntervalType.Reset();
                iInterval.Append(1);  iIntervalType.Append(EHourly); 
                iInterval.Append(4);  iIntervalType.Append(EHourly); 
                iInterval.Append(1);  iIntervalType.Append(EDaily); 
                iInterval.Append(3);  iIntervalType.Append(EDaily);     
                checkSchInfo =ETrue;
                break;
                }
            }
        }
    else
        {        
        iInterval.Reset();    iIntervalType.Reset();
        iInterval.Append(1);  iIntervalType.Append(EHourly); 
        iInterval.Append(4);  iIntervalType.Append(EHourly);  //setting to default 1h:4h:1d:3d
        iInterval.Append(1);  iIntervalType.Append(EDaily); 
        iInterval.Append(3);  iIntervalType.Append(EDaily);     
        checkSchInfo =ETrue;
        }

    TPtrC16 lastFive =  iscInfo.Right(5);
    _LIT( KNoRem , "NROFF");
    TPtrC noRem( KNoRem );   //check for No Remainder
    if (lastFive != noRem || checkSchInfo)
        {
        iIntervalType.Append(ENoReminderOn);
        }
    else
        { 
        iIntervalType.Append(ENoReminderOff);

        }
    FLOG(_L("CFotaUpdate::ParseScheduleString() <<"));

    } 


// ---------------------------------------------------------------------------
// CFotaUpdate::GetSchedulesInfoL
// Get the schedule info from the cenrep
// ---------------------------------------------------------------------------
//
//
void CFotaUpdate::GetSchedulesInfoL()
    {

    CRepository* centrep = NULL;
    //TBuf<255> scInfo; 
    centrep = CRepository::NewLC( TUid::Uid(KFotaServerUid));
    if ( centrep )
        {TBuf<255> temp;
        if (centrep->Get( KFotaOptionsForReminderDuration , temp )==KErrNone && temp.Length() )
            {   FLOG(_L("CFotaUpdate::ParseScheduleString taking from cenrep <<"));
            temp.Trim();    //read from cenrep
            iscInfo.Copy(temp);
            }
        else
            {                               //else set to default
            iscInfo.Copy(KDefaultSched);  

            }
        CleanupStack::PopAndDestroy(centrep);
        }
    else
        {
        iscInfo.Copy(KDefaultSched);    //else set to default
        }	

    }

// ---------------------------------------------------------------------------
// CFotaUpdate::ShowReminderDialogL
// Prepare and display Reminder Dialog using RFotaReminderDlg
// on radio button setting page
// ---------------------------------------------------------------------------
//
void CFotaUpdate::ShowReminderDialogL()
    {
    // Push default content to navigation pane and change title
    CEikStatusPane* statusPane = iAvkonAppUi->StatusPane();
    CAknNavigationControlContainer* naviPane = NULL;
    HBufC* originalTitle;
    CAknTitlePane* titlePane = NULL;
    originalTitle = HBufC::NewLC( 50 ); //KNsmlMaxTitleSize

    if ( statusPane && statusPane->PaneCapabilities( 
            TUid::Uid(EEikStatusPaneUidTitle) ).IsPresent() )
        {
        naviPane = ( CAknNavigationControlContainer* )
        statusPane->ControlL( TUid::Uid(EEikStatusPaneUidNavi) );
        naviPane->PushDefaultL();
        titlePane = static_cast<CAknTitlePane*>
        ( statusPane->ControlL( TUid::Uid(
                EEikStatusPaneUidTitle) ) );
        if ( titlePane->Text() ) 
            {
            originalTitle->Des().Copy( *titlePane->Text() );
            HBufC* titleText = StringLoader::LoadLC(
                    R_QTN_DM_TITLE_SETTINGS_DIALOG );
            titlePane->SetTextL( *titleText );
            CleanupStack::PopAndDestroy( titleText );
            titleText = NULL;
            }
        }

    TApaTaskList taskList(iFotaServer->GetEikEnv()->WsSession());
    TApaTask task=taskList.FindApp(TUid::Uid(KFotaServerUid));

    if(task.Exists())
        task.BringToForeground();

    // Construct radio btn list and launch radio button dlg
    TScheduleEntryInfo2 en=FindScheduleL( EFalse );
    TInt radioSelectionIndex( KErrNotFound );
    TRAPD(error,ParseScheduleStringL());  
    if (error)
        {
        iInterval.Reset();    iIntervalType.Reset();
        iInterval.Append(1);  iIntervalType.Append(EHourly); 
        iInterval.Append(4);  iIntervalType.Append(EHourly); 
        iInterval.Append(1);  iIntervalType.Append(EDaily); 
        iInterval.Append(3);  iIntervalType.Append(EDaily); 
        iIntervalType.Append(ENoReminderOn);
        }
    if(iInterval.Count() > 4)
        {
        for(TInt i= 4;i <iInterval.Count();i++)
            iInterval.Remove(i);
        }  
    CDesCArray* itemArray = CreateRadionButtonArrayLC( en,radioSelectionIndex );

    TInt aEvent(KErrNone);
    CFotaReminderDlg* aFotaReminderDlg = CFotaReminderDlg::NewL(radioSelectionIndex,itemArray,aEvent );
    FLOG(_L("after creating fota reminder dialog"));   
    CleanupStack::PushL(aFotaReminderDlg);
    TInt presentPriotiry = CEikonEnv::Static()->RootWin().OrdinalPriority();
    TInt presentPosition = CEikonEnv::Static()->RootWin().OrdinalPosition();
    CEikonEnv::Static()->RootWin().SetOrdinalPosition(presentPosition, ECoeWinPriorityMedium);  //setting the priority to medium for activation on top of global notes

    TInt ret = aFotaReminderDlg->ExecuteLD(CAknSettingPage::EUpdateWhenChanged);
    CleanupStack::Pop(aFotaReminderDlg);	     							
    CleanupStack::PopAndDestroy( itemArray );

    CEikonEnv::Static()->RootWin().SetOrdinalPosition(presentPosition, presentPriotiry); //resetting to previous priority
    FLOG(_L("****Event key = %d"),aEvent);

    if (aEvent == EEventKey)
        {
        FLOG(_L("Exiting...."));
        TApaTaskList taskList(iFotaServer->GetEikEnv()->WsSession());
        TApaTask task=taskList.FindApp(TUid::Uid(KOmaDMAppUid));
        if(task.Exists())
            {
            task.EndTask();
            task.KillTask();
            }

        return;
        }
		if(naviPane)
    	naviPane->Pop();
    if(	titlePane )
    {	
    	if ( titlePane->Text() && originalTitle ) 
        {
        TRAP_IGNORE( titlePane->SetTextL( *originalTitle ) );
        }
    }
    CleanupStack::PopAndDestroy( originalTitle );
    task.SendToBackground();	

    CreateNewScheduleL(ret,radioSelectionIndex);	


    }

// ---------------------------------------------------------------------------
// CFotaUpdate::CreateNewScheduleL
// Creates the new schedule
// ---------------------------------------------------------------------------
//
void CFotaUpdate::CreateNewScheduleL(TInt aRet, TInt aRadioSelectionIndex)
    {

    TScheduleEntryInfo2 en=FindScheduleL( EFalse );
    TIntervalType   tIntervalType;    
    TInt tInterval; 
    if ( aRet && aRadioSelectionIndex== 0 ) 
        {
        FLOG(_L("Creating reminder: for 1st option"));
        tIntervalType = (TIntervalType)iIntervalType[aRadioSelectionIndex];				
        tInterval     = iInterval[aRadioSelectionIndex];
        en=FindScheduleL( ETrue );
        CreateScheduleL ( iUpdateState.iPkgId, tIntervalType ,tInterval );
        iFotaServer->FinalizeUpdateL();
        }
    else if(aRet && aRadioSelectionIndex == 1 )
        {
        FLOG(_L("Creating reminder: 4 hours"));
        tIntervalType = (TIntervalType)iIntervalType[aRadioSelectionIndex];				
        tInterval     = iInterval[aRadioSelectionIndex];
        en=FindScheduleL( ETrue );
        CreateScheduleL ( iUpdateState.iPkgId, tIntervalType ,tInterval );
        iFotaServer->FinalizeUpdateL();
        }
    else if(aRet && aRadioSelectionIndex == 2 )
        {
        FLOG(_L("Creating reminder: for 2nd option"));
        tIntervalType = (TIntervalType)iIntervalType[aRadioSelectionIndex];				
        tInterval     = iInterval[aRadioSelectionIndex];
        en=FindScheduleL( ETrue );
        CreateScheduleL ( iUpdateState.iPkgId, tIntervalType ,tInterval );
        iFotaServer->FinalizeUpdateL();
        }
    else if(aRet && aRadioSelectionIndex == 3 )
        {
        FLOG(_L("Creating reminder: for 3rd option"));
        tIntervalType = (TIntervalType)iIntervalType[aRadioSelectionIndex];				
        tInterval     = iInterval[aRadioSelectionIndex];  
        en=FindScheduleL( ETrue );
        CreateScheduleL ( iUpdateState.iPkgId, tIntervalType ,tInterval );  
        iFotaServer->FinalizeUpdateL();
        }
    else if (!aRet || aRadioSelectionIndex == 4)
        {
        FLOG(_L("Deleting reminder, btn press %d"),aRet);
        TScheduleEntryInfo2 en=FindScheduleL( ETrue );

        if ( !aRet ) // cancel pressed
            {
            LaunchNotifierL( ESyncMLFwUpdStartQuery, iUpdateState.iProfileId );
            }
        else			  // ok pressed
            {
            LaunchNotifierL( ESyncMLFwUpdNoReminder , KErrNone );
            //iFotaServer->FinalizeUpdateL();
            }
        }

    }

// ---------------------------------------------------------------------------
// CFotaUpdate::FindScheduleL
// Finds fotaupdate schedule (named [102072c4])
// ---------------------------------------------------------------------------
//
TScheduleEntryInfo2 CFotaUpdate::FindScheduleL( const TBool aDelete )
    {
    TScheduleEntryInfo2                     ret;
    TInt                                    err;    
    RScheduler                              sc;
    
    TTsTime                                 time;
    TSchedulerItemRef                       scitem; 
    CArrayFixFlat<TSchedulerItemRef>*     	aSchRefArray = new CArrayFixFlat <TSchedulerItemRef>(5);
    TScheduleFilter                      	aFilter(EAllSchedules);
    User::LeaveIfError( sc.Connect() );                             // xx
    CleanupClosePushL( sc );
    CleanupStack::PushL(aSchRefArray);

    User::LeaveIfError( sc.GetScheduleRefsL( *aSchRefArray,aFilter) );  // xx
    FLOG(_L("Schedule items: "));
    for ( TInt i=0; i<aSchRefArray->Count(); ++i  )
        {
        TSchedulerItemRef it = (*aSchRefArray)[i];
        if ( it.iName == TUid::Uid(KFotaServerUid).Name()  )
            {
            TScheduleState2 sc_state;
            CArrayFixFlat<TScheduleEntryInfo2>*  sc_entries = new CArrayFixFlat <TScheduleEntryInfo2>(5);
            CArrayFixFlat<TTaskInfo>*            sc_tasks  = new CArrayFixFlat <TTaskInfo>(5);
            TTsTime                              sc_duetime;
            CleanupStack::PushL( sc_entries );
            CleanupStack::PushL( sc_tasks );
            FLOG (_L("%d. schedule handle: %d name:'%S'"),i,it.iHandle, &(it.iName) );

            err = sc.GetScheduleL ( it.iHandle , sc_state, *sc_entries,*sc_tasks,sc_duetime ); // xx

            TDateTime  dtm = sc_duetime.GetLocalTime().DateTime();
            FLOG(_L("   schedule duetime:%d:%d"), dtm.Hour(), dtm.Minute());

            if ( err ) 
                {
                FLOG(_L("     schedule  sc get err %d"),err);
                }
            else 
                {
                for ( TInt k=0; k<sc_entries->Count();++k)
                    {
                    TScheduleEntryInfo2 sc_entry = (*sc_entries)[k];
                    ret = sc_entry;
                    TTime sctime = sc_entry.StartTime().GetLocalTime();
                    FLOG(_L("         schedule entry %d int-type:%d int:%d start: %d:%d"),k,sc_entry.IntervalType(),sc_entry.Interval(),sctime.DateTime().Hour(),sctime.DateTime().Minute());
                    }

                for ( TInt j=0; j<sc_tasks->Count();++j)
                    {
                    TTaskInfo sc_task = (*sc_tasks)[j];
                    FLOG(_L("         schedule task  %d  '%S'"),sc_task.iTaskId,&(sc_task.iName) );
                    if ( aDelete && sc_task.iName==TUid::Uid(KFotaServerUid).Name() )
                        {
                        FLOG(_L("          schedule DeleteTask %d"),sc_task.iTaskId);
                        User::LeaveIfError( sc.DeleteTask(sc_task.iTaskId) );       // xx
                        }
                    }
                }
            if ( aDelete )
                {
                FLOG(_L("     DeleteSchedule %d"),it.iHandle);
                User::LeaveIfError(sc.DeleteSchedule(it.iHandle ));                 // xx
                }
            CleanupStack::PopAndDestroy( sc_tasks );
            CleanupStack::PopAndDestroy( sc_entries );
            }
        }
    CleanupStack::PopAndDestroy( aSchRefArray );
    CleanupStack::PopAndDestroy(&sc);
    return ret;
    }



// ---------------------------------------------------------------------------
// CFotaUpdate::CreateScheduleL
// Creates fotaupdate schedule (named [102072c4])
// ---------------------------------------------------------------------------
//
TInt CFotaUpdate::CreateScheduleL ( const TInt aPackageId 
        ,const TIntervalType  aIntervalType
        ,const TInt aInterval)
    {
    FLOG(_L("CFotaUpdate::CreateScheduleL ()") );
    const TInt KRepeatForever = 0;
    const TInt KTaskPriority = 1;
    _LIT(KFotaScheduleExe, "Z:\\sys\\bin\\fotaschedulehandler.exe");
    RScheduler                              sc;
    TTime                                   t; 
    TTsTime                                 time;
    TSchedulerItemRef                       scitem; 
    CArrayFixFlat<TScheduleEntryInfo2>*     scentries = new CArrayFixFlat <TScheduleEntryInfo2>(5);
    User::LeaveIfError( sc.Connect() );                                       // xx
    CleanupClosePushL( sc );
    CleanupStack::PushL(scentries );
    t.HomeTime(); 
    switch (aIntervalType)  //create schedule based on the interval type 
        {
        case EHourly :
            t = t + ((TTimeIntervalHours)aInterval);
            break;
        case EDaily  :
            t = t + ((TTimeIntervalDays) aInterval);
            break;
        case EMonthly :
            t = t + ((TTimeIntervalMinutes ) aInterval);
            break; 
        case EYearly  :
            t = t + ((TTimeIntervalYears ) aInterval);
            break;
        default :
            User::Panic(KFotaPanic, KErrArgument);

        }  

    time.SetLocalTime( t );
    TScheduleEntryInfo2 scentry1(time, aIntervalType, aInterval, (TTimeIntervalMinutes)1440 ) ; // xx
    scentries->AppendL( scentry1 );

    scitem.iName = TUid::Uid(KFotaServerUid).Name();
    User::LeaveIfError( sc.Register(TFileName( KFotaScheduleExe ), 0 ) );    // xxx
    User::LeaveIfError( sc.CreatePersistentSchedule(scitem, *scentries) );   // xxx
    FLOG(_L("created schedule %d  %d:%d"),scitem.iHandle, t.DateTime().Hour(),t.DateTime().Minute() );
    TTaskInfo taskInfo;
    taskInfo.iName      = TUid::Uid(KFotaServerUid).Name();
    taskInfo.iRepeat    = KRepeatForever;
    taskInfo.iPriority  = KTaskPriority;

    TFotaScheduledUpdate fotareminder( aPackageId, scitem.iHandle );
    TPckg<TFotaScheduledUpdate>   fotareminderpkg(fotareminder);

    HBufC* data = HBufC::NewLC(fotareminderpkg.Length());
    data->Des().Copy(fotareminderpkg);

    User::LeaveIfError( sc.ScheduleTask(taskInfo, *data, scitem.iHandle) );  // xxx

    CleanupStack::PopAndDestroy( data );
    CleanupStack::PopAndDestroy( scentries );
    CleanupStack::PopAndDestroy(&sc);                                         // xx
    return scitem.iHandle;
	}
// --------------------------------------------------------------------------
// CFotaUpdate::CheckUpdateVariations
// Checks the Install update note display,when a phone call is active
// --------------------------------------------------------------------------
//
TBool CFotaUpdate::CheckUpdateVariations()
    {
    FLOG(_L("CFotaUpdate::CheckUpdateVariations >>"));
    CRepository* centrep( NULL);
    TRAPD(err, centrep = CRepository::NewL( KCRUidFotaServer ) );
    if (err) FLOG(_L("Error reading FotaServer cenrep... %d"),err);
    TInt supported(KErrNone);
    if (centrep ) 
        {
        err = centrep->Get( KFotaUpdateNoteDisplayAlways, supported );
        if (err) FLOG(_L("Error reading cenrep key... %d"),err);                
        delete centrep;
        centrep = NULL;
        }
    if(supported) //means note to be displayed when a call is active
        {
        return EFalse;
        }
    FLOG(_L("CFotaUpdate::CheckUpdateVariations <<"));
    return ETrue; //means note should not be displayed when a call is there
    }

