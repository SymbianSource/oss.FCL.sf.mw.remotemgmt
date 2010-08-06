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

#include "fotafullscreendialog.h"

#include <hbaction.h>
#include <hbprogressbar.h>
#include <hbicon.h>
#include <hbextendedlocale.h>
#include <hbtranslator.h>
#include <e32std.h>
#include <e32math.h>


// ---------------------------------------------------------------------------
// CFotaDownloadNotifHandler::FotaFullscreenDialog
// Constructor of the full screen dialogs, it takes all the widgets from the docml
// initializes it and sets the progress bar to zero.
// ---------------------------------------------------------------------------
//


FotaFullscreenDialog::FotaFullscreenDialog(CFotaServer* aObserver)
    {
    FLOG(_L("FotaFullscreenDialog::FotaFullscreenDialog >>"));
    bool ok = false;
    iServer = aObserver;
    iloader.load(":/xml/data/fullscreendialog.docml", &ok);
    if (!ok)
        {
        return;
        }
    iClicked = EFalse;
    idialog = qobject_cast<HbDialog *> (iloader.findWidget("dialog"));

	idialog->setTimeout(HbPopup::NoTimeout);
   
    iPrimaryAction = qobject_cast<HbPushButton *> (iloader.findWidget(
            "btnHide"));
			
     iSecondaryAction = qobject_cast<HbPushButton *> (iloader.findWidget(
            "btnResumeLater"));
			
			
    
    iprogressBar = qobject_cast<HbProgressBar *> (iloader.findWidget(
            "horizontalProgressBar"));
    iprogressBar->setProgressValue(0);
	
	    /*iInstallNote = qobject_cast<HbLabel *> (iloader.findWidget(
            "lblinstallnote"));*/
    iDownloadState = qobject_cast<HbLabel *> (iloader.findWidget(
            "lblDownloadState"));
    //iEmergencyIcon = qobject_cast<HbLabel *> (iloader.findWidget(
    //        "icnEmergency"));
    iChargerIcon = qobject_cast<HbLabel *> (iloader.findWidget("icnCharger"));
    HbIcon iconCharger;
    //iconCharger.setIconName(:/icons/qgn_prop_sml_http.svg);
    //iChargerIcon->setIcon(iconCharger);
    HbIcon iconEmergency;
    //iconEmergency.setIconName(:/icons/qgn_prop_sml_http.svg);
    //iEmergencyIcon->setIcon(iconEmergency);
    //iEmergencyNote = qobject_cast<HbLabel *> (iloader.findWidget(
    //        "lblEmergency"));
    //iRestartIcon = qobject_cast<HbLabel *> (iloader.findWidget("icnInstall"));
    //HbIcon iconRestart;
    //iconRestart.setIconName(:/icons/qgn_prop_sml_http.svg);
    //iRestartIcon->setIcon(iconRestart);
    //iRestartNote = qobject_cast<HbLabel *> (iloader.findWidget(
    //        "lblRestartNote"));
    //installEventFilter(this);
    iPrimaryAction->setText(hbTrId("txt_common_button_hide"));
    iSecondaryAction->setText(hbTrId("txt_device_update_button_resume_later"));
    iTitle = qobject_cast<HbLabel *> (iloader.findWidget("lblTitle"));
    iTitle->setPlainText(hbTrId("txt_device_update_title_updating_phone"));
    iDownloadState->setPlainText(hbTrId("txt_device_update_info_downloading"));
    //iInstallNote->setPlainText(hbTrId(
    //        "txt_device_update_info_installation_will_proceed_n"));
    iChargerNote = qobject_cast<HbLabel *> (iloader.findWidget("lblcharger"));
    iChargerNote->setPlainText(hbTrId(
            "txt_device_update_info_FS_its_recommended_to_connec"));
    //iRestartNote->setPlainText(hbTrId(
    //        "txt_device_update_info_FS_after_the_installation_the"));
    //iEmergencyNote->setPlainText(hbTrId(
    //        "txt_device_update_info_FS_during_the_installation_the"));

    QObject::connect(iPrimaryAction, SIGNAL(clicked()), this,
            SLOT(LSKSelected()));
    QObject::connect(iSecondaryAction, SIGNAL(clicked()), this,
            SLOT(RSKSelected()));
    QObject::connect(idialog, SIGNAL(aboutToClose()), this,
            SLOT(aboutToClose()));

    RefreshDialog();
    FLOG(_L("FotaFullscreenDialog::FotaFullscreenDialog <<"));
    }



// ---------------------------------------------------------------------------
// CFotaDownloadNotifHandler::refreshDialog
// This function is used to refresh the contents of the full screen dialog
// once it is changed.
// ---------------------------------------------------------------------------
//


void FotaFullscreenDialog::RefreshDialog()
    {
    FLOG(_L("FotaFullscreenDialog::RefreshDialog >>"));
    if (idialog)
        {
        FLOG(_L("FotaFullscreenDialog::idialog >>"));
        iClicked = EFalse;
        idialog->show();
        }
    FLOG(_L("FotaFullscreenDialog::RefreshDialog <<"));
    }

// ---------------------------------------------------------------------------
// CFotaDownloadNotifHandler::setsoftwaredetails
// This function is called to set the details of the software,which is downloaded.
// ---------------------------------------------------------------------------
//

void FotaFullscreenDialog::SetSoftwareDetails(int size,
        const QString version, const QString aName)
    {
    //set Sw Details
    FLOG(_L("FotaFullscreenDialog::RefreshDialog >>"));
    iSwDetails = qobject_cast<HbLabel *> (iloader.findWidget("lblSwVersion"));
    QString content;
    
    TReal sizeKB = size / 1024;
    TReal sizeRounded = 0;
    QString pkgsize;

    if (sizeKB > 1024)
        {
        TReal sizeMB = sizeKB / 1024;
        Math::Round(sizeRounded,sizeMB,2);
        content = hbTrId("txt_device_update_info_new_device_software_availab_Mb") .arg(aName) .arg(version).arg(sizeRounded);
        }
    else
        {
        Math::Round(sizeRounded,sizeKB,2);
        content = hbTrId("txt_device_update_info_new_device_software_availab_Kb") .arg(aName) .arg(version).arg(sizeRounded);
        }

    iSwDetails->setPlainText(content);
    FLOG(_L("FotaFullscreenDialog::RefreshDialog <<"));
    }

// ---------------------------------------------------------------------------
// CFotaDownloadNotifHandler::close
// This slot is called to manually close the dialog.
// ---------------------------------------------------------------------------
//

void FotaFullscreenDialog::Close()
    {
    FLOG(_L("FotaFullscreenDialog::Close <<"));
    iClicked = ETrue;
    if (idialog)
        idialog->close();
    FLOG(_L("FotaFullscreenDialog::Close >>"));
    }

// ---------------------------------------------------------------------------
// CFotaDownloadNotifHandler::setwarningdetails
// This function is to set the warnings details of the full screen dialog
// according to the state of the firmware update.
// ---------------------------------------------------------------------------
//

void FotaFullscreenDialog::SetWarningDetails(TFotaWarningType aType)
    {
    FLOG(_L("FotaFullscreenDialog::SetWarningDetails <<"));
    //set heading content
    if (aType == EHbFotaDownload)
        {
        FLOG(_L("FotaFullscreenDialog::EHbFotaDownload"));
        idialog->setTimeout(HbPopup::NoTimeout);
        iInstallNote = qobject_cast<HbLabel *> (iloader.findWidget(
                    "lblinstallnote"));
        iInstallNote->setPlainText(hbTrId(
                    "txt_device_update_info_installation_will_proceed_n"));
        //User::After(1000);
        iInstallNote->setVisible(false);
        
        iRestartNote = qobject_cast<HbLabel *> (iloader.findWidget(
                    "lblRestartNote"));
        iRestartNote->setPlainText(hbTrId(
                    "txt_device_update_info_FS_after_the_installation_the"));
        //User::After(1000);
        iRestartNote->setVisible(false);
        
        iRestartIcon = qobject_cast<HbLabel *> (iloader.findWidget("icnInstall"));
        HbIcon iconRestart;
        //iconRestart.setIconName(:/icons/qgn_prop_sml_http.svg);
        //iRestartIcon->setIcon(iconRestart);
        //User::After(1000);
        iRestartIcon->setVisible(false);
        
        iEmergencyNote = qobject_cast<HbLabel *> (iloader.findWidget(
                    "lblEmergency"));
        iEmergencyNote->setPlainText(hbTrId(
                   "txt_device_update_info_FS_during_the_installation_the"));
        //User::After(1000);
        iEmergencyNote->setVisible(false);
        
        iEmergencyIcon = qobject_cast<HbLabel *> (iloader.findWidget(
                    "icnEmergency"));
        HbIcon iconEmergency;
        //iconEmergency.setIconName(:/icons/qgn_prop_sml_http.svg);
        //iEmergencyIcon->setIcon(iconEmergency);
        //User::After(1000);
        iEmergencyIcon->setVisible(false);

        }
    else if (aType == EHbFotaUpdate)
        {
        FLOG(_L("FotaFullscreenDialog::EHbFotaUpdate"));
        idialog->setTimeout(HbPopup::ContextMenuTimeout);
        HbIcon iconCharger;
        //iconCharger.setIconName(:/icons/qgn_prop_sml_http.svg);
        //iChargerIcon->setIcon(iconCharger);
        HbIcon iconRestart;
        //iconRestart.setIconName(:/icons/qgn_prop_sml_http.svg);
        //iRestartIcon->setIcon(iconRestart);
        iInstallNote->setPlainText(hbTrId(
                "txt_device_update_info_installation_will_proceed_n"));
        iChargerNote->setPlainText(hbTrId(
                "txt_device_update_info_FS_its_recommended_to_connec"));
        iRestartNote->setPlainText(hbTrId(
                "txt_device_update_info_FS_after_the_installation_the"));
        iInstallNote->setVisible(true);
        iRestartNote->setVisible(true);
        iRestartIcon->setVisible(true);
        iEmergencyNote->setVisible(true);
        iEmergencyIcon->setVisible(true);
        iDownloadState->setPlainText(hbTrId(
                "txt_device_update_info_download_complete"));
        iDownloadState->setVisible(true);
        iPrimaryAction->setEnabled(ETrue);
        }
    else if (aType == EHbFotaLowBattery)
        {
        FLOG(_L("FotaFullscreenDialog::EHbFotaLowBattery"));
        idialog->setTimeout(HbPopup::NoTimeout);
        HbIcon iconCharger;
        //iconCharger.setIconName(:/icons/qgn_prop_sml_http.svg);
        //iChargerIcon->setIcon(iconCharger);
        HbIcon iconRestart;
        //iconRestart.setIconName(:/icons/qgn_prop_sml_http.svg);
        //iRestartIcon->setIcon(iconRestart);
        iInstallNote->setPlainText(hbTrId(
                "txt_device_update_info_to_proceed_with_installatio"));
        iChargerNote->setPlainText(hbTrId(
                "txt_device_update_info_FS_after_the_installation_the"));
        iRestartNote->setPlainText(hbTrId(
                "txt_device_update_info_FS_during_the_installation_the"));
        //iEmergencyNote->setPlainText(hbTrId("txt_device_update_info_FS_during_the_installation_the"));
        iInstallNote->setVisible(true);
        iRestartNote->setVisible(true);
        iRestartIcon->setVisible(true);
        iEmergencyNote->setVisible(false);
        iEmergencyIcon->setVisible(false);
        iDownloadState->setPlainText(hbTrId(
                "txt_device_update_info_download_complete"));
        iDownloadState->setVisible(true);
        iPrimaryAction->setText(hbTrId("txt_device_update_button_continue"));
        iSecondaryAction->setText(hbTrId(
                "txt_device_update_button_install_later"));
        iPrimaryAction->setEnabled(EFalse);
        }
    RefreshDialog();
    FLOG(_L("FotaFullscreenDialog::SetWarningDetails >>"));
    }

// ---------------------------------------------------------------------------
// CFotaDownloadNotifHandler::updateprogressbar
// This function is called to update the progress bar with the download progress details/.
// ---------------------------------------------------------------------------
//

void FotaFullscreenDialog::UpdateProgressBar(TInt aProgress)
    {
    FLOG(_L("FotaFullscreenDialog::UpdateProgressBar <<"));
    iprogressBar->setProgressValue(aProgress);
    FLOG(_L("FotaFullscreenDialog::UpdateProgressBar >>"));
    }

// ---------------------------------------------------------------------------
// CFotaDownloadNotifHandler::DisableRSK
// This function is called disable the RSK of the dialog when the postpone limeit expires
// ---------------------------------------------------------------------------
//

void FotaFullscreenDialog::DisableRSK(TBool aVal)
    {
    FLOG(_L("FotaFullscreenDialog::DisableRSK <<"));
    iSecondaryAction->setEnabled(!aVal);
    FLOG(_L("FotaFullscreenDialog::DisableRSK >>"));
    }

bool FotaFullscreenDialog::IsLSKEnabled()
    {
    FLOG(_L("FotaFullscreenDialog::IsLSKEnabled <<"));
    return (iPrimaryAction->isEnabled());
    }

// ---------------------------------------------------------------------------
// CFotaDownloadNotifHandler::cancelSelected
// This slot is called when user presses the left softkey of fullscreen dialog.
// ---------------------------------------------------------------------------
//

void FotaFullscreenDialog::LSKSelected()
    {
    FLOG(_L("FotaFullscreenDialog::LSKSelected <<"));
    iClicked = ETrue;
    idialog->hide();
    iServer->HandleFullScreenDialogResponse(EHbLSK);
    FLOG(_L("FotaFullscreenDialog::LSKSelected >>"));
    }
	
	
	// ---------------------------------------------------------------------------
// CFotaDownloadNotifHandler::cancelSelected
// This slot is called when user presses the right softkey of fullscreen dialog.
// ---------------------------------------------------------------------------
//

void FotaFullscreenDialog::RSKSelected()
    {
    FLOG(_L("FotaFullscreenDialog::RSKSelected >>"));
    iClicked = ETrue;
    idialog->hide();
    iServer->HandleFullScreenDialogResponse(EHbRSK);
    FLOG(_L("FotaFullscreenDialog::RSKSelected <<"));
    }

// ---------------------------------------------------------------------------
// CFotaDownloadNotifHandler::~FotaFullscreenDialog
// Destructor which destroys all the widgets
// ---------------------------------------------------------------------------
//

FotaFullscreenDialog::~FotaFullscreenDialog()
    {
    FLOG(_L("~ FotaFullscreenDialog <<"));

    QObject::disconnect(iPrimaryAction, SIGNAL(clicked()), this,
            SLOT(LSKSelected()));
    QObject::disconnect(iSecondaryAction, SIGNAL(clicked()), this,
            SLOT(RSKSelected()));
    QObject::disconnect(idialog, SIGNAL(aboutToClose()), this,
            SLOT(aboutToClose()));

    if (iprogressBar)
        iprogressBar->deleteLater();

    if (iPrimaryAction)
        delete iPrimaryAction;

    if (iSecondaryAction)
        delete iSecondaryAction;

    if (iTitle)
        delete iTitle;

    if (iSwDetails)
        delete iSwDetails;

    if (iInstallNote)
        delete iInstallNote;

    if (iRestartNote)
        delete iRestartNote;

    if (iRestartIcon)
        delete iRestartIcon;

    if (iEmergencyNote)
        delete iEmergencyNote;

    if (iEmergencyIcon)
        delete iEmergencyIcon;

    if (iDownloadState)
        delete iDownloadState;

    if (idialog)
        delete idialog;

    FLOG(_L("~ FotaFullscreenDialog >>"));
    }

// ---------------------------------------------------------------------------
// CFotaDownloadNotifHandler::showUpdateDialog
// This function is called when download is complete and update dialog has to be shown.
// ---------------------------------------------------------------------------
//

void FotaFullscreenDialog::ShowUpdateDialog()
    {
    FLOG(_L("FotaFullscreenDialog::ShowUpdateDialog <<"));
    iPrimaryAction->setText(hbTrId("txt_common_button_continue_dialog"));
    iSecondaryAction->setText(
            hbTrId("txt_device_update_button_install_later"));
    SetWarningDetails(EHbFotaUpdate);
    FLOG(_L("FotaFullscreenDialog::ShowUpdateDialog >>"));
    }


void FotaFullscreenDialog::SetVisible(TBool aVisible)
{
    FLOG(_L("RefreshDialog::SetVisible >>"));
    RefreshDialog();
    FLOG(_L("RefreshDialog::SetVisible <<"));
}
// ---------------------------------------------------------------------------
// CFotaDownloadNotifHandler::aboutToClose
// This slot is called when full screen dialogs is cloased during a timeout.
// ---------------------------------------------------------------------------
//

void FotaFullscreenDialog::aboutToClose()
    {
    FLOG(_L("FotaFullscreenDialog::aboutToClose <<"));
    if (!iClicked)
        {
        FLOG(_L("Calling fotaserver as timedout..."));
        iServer->HandleFullScreenDialogResponse(EHbLSK);
        }
    iClicked = EFalse;
    FLOG(_L("FotaFullscreenDialog::aboutToClose >>"));
    }


