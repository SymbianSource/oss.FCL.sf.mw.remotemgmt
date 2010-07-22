/*
 * Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies). 
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
 * Description: 
 *
 */
#include "fotanotifiers.h"
#include "FotaDlMgrClient.h"
_LIT(KHbNotifier,"com.nokia.hb.devicemanagementdialog/1.0");

// --------------------------------------------------------------------------
// CFotaDownloadNotifHandler::NewL 
// --------------------------------------------------------------------------
//
CFotaDownloadNotifHandler* CFotaDownloadNotifHandler::NewL(
        MfotadevicedialogObserver* aObserver)
    {
    FLOG(_L("CFotaDownloadNotifHandler::NewL >>"));
    __ASSERT_ALWAYS( aObserver, User::Panic(KFotaPanic, KErrArgument) );
    CFotaDownloadNotifHandler* h = new (ELeave) CFotaDownloadNotifHandler;
    h->iObserver = aObserver;
    
    FLOG(_L("CFotaDownloadNotifHandler::NewL <<"));

    return h;
    }

// --------------------------------------------------------------------------
CFotaDownloadNotifHandler::CFotaDownloadNotifHandler() :
    iDevDialog(NULL), iDialogID(0)
    {
    FLOG(_L("CFotaDownloadNotifHandler::CFotaDownloadNotifHandler()"));
    }

// --------------------------------------------------------------------------
CFotaDownloadNotifHandler::~CFotaDownloadNotifHandler()
    {
    FLOG(_L("CFotaDownloadNotifHandler::~CFotaDownloadNotifHandler >>"));
    if (iDevDialog)
        delete iDevDialog;
    FLOG(_L("CFotaDownloadNotifHandler::~CFotaDownloadNotifHandler <<"));
    }

// ---------------------------------------------------------------------------
// CFotaDownloadNotifHandler::LaunchNotifierL
// This function is used the observer which uses this notifer to lauch the notifier
// It puts the necessary parameters to lauch the notifier in CHbSymbianVariantMap
// ---------------------------------------------------------------------------
//
void CFotaDownloadNotifHandler::LaunchNotifierL(
        CHbSymbianVariantMap *aNotifParams, TInt aDialogId)
    {
    FLOG(_L("CFotaDownloadNotifHandler::LaunchNotifierL() >>"));
    if (!iDevDialog)
        iDevDialog = CHbDeviceDialogSymbian::NewL();
    FLOG(_L("CFotaDownloadNotifHandler::CHbDeviceDialogSymbian::NewL()"));
    if (iDevDialog == NULL)
        FLOG(_L("Error in CHbDeviceDialogSymbian::NewL()"));

    iDialogID = aDialogId;
    //connect(mDeviceDialog, SIGNAL(dataReceived(QVariantMap)), this, SLOT(dataReceived(QVariantMap)));

    TInt Err = iDevDialog->Show(KHbNotifier, *aNotifParams, this);
    FLOG(
            _L("CFotaDownloadNotifHandler::CHbDeviceDialogSymbian::Show() - %d"),
            Err);

    FLOG(_L("CFotaDownloadNotifHandler::LaunchNotifierL() <<"));
    }

void CFotaDownloadNotifHandler::Cancel()
    {
    FLOG(_L("CFotaDownloadNotifHandler::Cancel >>"));
    if (iDialogID)
        iDevDialog->Cancel();
    FLOG(_L("CFotaDownloadNotifHandler::Cancel <<"));
    }

// ---------------------------------------------------------------------------
// CFotaDownloadNotifHandler::DataReceived
// This slot is called when user gives a keypress event to the device dialog(through deviceDialogData signal).
// CHbSymbianVariantMap is passed from the device dialog which contains the information
// of the user input.
// ---------------------------------------------------------------------------
//
void CFotaDownloadNotifHandler::DataReceived(CHbSymbianVariantMap& aData)
    {
    FLOG(_L("CFotaDownloadNotifHandler::DataReceived() >>"));
    iDevDialog->Cancel();
    TFwUpdNoteTypes ret;
    const CHbSymbianVariant* dialogId = aData.Get(KResult);
    ret = *(TFwUpdNoteTypes *) dialogId->Value<TInt> ();
    TInt temp = iDialogID;
    iDialogID = 0;
    iObserver->HandleDialogResponse(ret, temp);

    FLOG(_L("CFotaDownloadNotifHandler::DataReceived() <<"));
    }

// ---------------------------------------------------------------------------
// CFotaDownloadNotifHandler::DeviceDialogClosed
// This slot is called when the device dialog is closed due to some error.
// aCompletionCode gives the error information on which the dialog is closed.
// ---------------------------------------------------------------------------
//
void CFotaDownloadNotifHandler::DeviceDialogClosed(TInt aCompletionCode)
    {
    FLOG(_L("CFotaDownloadNotifHandler::DeviceDialogClosed() >>"));

    FLOG(_L("CFotaDownloadNotifHandler::DeviceDialogClosed() <<"));
    }

