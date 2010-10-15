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
 * Description: FotaFullscreenDialog, which is responsible for showing the fullscreen dialog of FOTA from the docml.
 *
 */

#include "fotafullscreendialog.h"

#include <HbAction.h>
#include <hbprogressbar.h>
#include <hbicon.h>
#include <hbextendedlocale.h>
#include <hbtranslator.h>
#include <e32std.h>
#include <e32math.h>


// ---------------------------------------------------------------------------
// HbFotaFullscreenDialog::HbFotaFullscreenDialog
// Constructor of the full screen dialogs, it takes all the widgets from the docml
// initializes it and sets the progress bar to zero.
// ---------------------------------------------------------------------------
//


HbFotaFullscreenDialog::HbFotaFullscreenDialog(CFotaServer* aObserver)
    {
    FLOG(_L("HbFotaFullscreenDialog::HbFotaFullscreenDialog >>"));
    bool ok = false;
    m_Server = aObserver;
    m_loader.load(":/xml/data/fullscreendialog.docml", &ok);
    if (!ok)
        {
        return;
        }
    m_Clicked = EFalse;
    m_dialog = qobject_cast<HbDialog *> (m_loader.findWidget("dialog"));

		m_dialog->setTimeout(HbPopup::NoTimeout);
   
    m_PrimaryAction = qobject_cast<HbPushButton *> (m_loader.findWidget(
            "btnHide"));
			
     m_SecondaryAction = qobject_cast<HbPushButton *> (m_loader.findWidget(
            "btnResumeLater"));
			
			
    
    m_progressBar = qobject_cast<HbProgressBar *> (m_loader.findWidget(
            "horizontalProgressBar"));
    m_progressBar->setProgressValue(0);
	
	  m_InstallNote = qobject_cast<HbLabel *> (m_loader.findWidget(
            "lblinstallnote"));
    m_InstallNote->setPlainText(hbTrId(
            "txt_device_update_info_installation_will_proceed_n"));
            
    m_DownloadState = qobject_cast<HbLabel *> (m_loader.findWidget(
            "lblDownloadState"));
    m_DownloadState->setPlainText(hbTrId("txt_device_update_info_downloading"));
            
    m_EmergencyIcon = qobject_cast<HbLabel *> (m_loader.findWidget(
            "icnEmergency"));
    HbIcon iconEmergency;
    iconEmergency.setIconName(":/icons/qgn_prop_sml_http.svg");
    m_EmergencyIcon->setIcon(iconEmergency);
    
    m_ChargerIcon = qobject_cast<HbLabel *> (m_loader.findWidget("icnCharger"));
    HbIcon iconCharger;
    iconCharger.setIconName(":/icons/qgn_prop_sml_http.svg");
    m_ChargerIcon->setIcon(iconCharger);
    
    m_RestartIcon = qobject_cast<HbLabel *> (m_loader.findWidget("icnInstall"));
    HbIcon iconRestart;
    iconRestart.setIconName(":/icons/qgn_prop_sml_http.svg");
    m_RestartIcon->setIcon(iconRestart);
    
    m_PrimaryAction->setText(hbTrId("txt_common_button_hide"));
    m_SecondaryAction->setText(hbTrId("txt_device_update_button_resume_later"));
    m_Title = qobject_cast<HbLabel *> (m_loader.findWidget("lblTitle"));
    m_Title->setPlainText(hbTrId("txt_device_update_title_updating_phone"));
    
    
    m_ChargerNote = qobject_cast<HbLabel *> (m_loader.findWidget("lblcharger"));
    m_ChargerNote->setPlainText(hbTrId(
            "txt_device_update_info_FS_its_recommended_to_connec"));
            
    m_RestartNote = qobject_cast<HbLabel *> (m_loader.findWidget(
            "lblRestartNote"));
    m_RestartNote->setPlainText(hbTrId(
            "txt_device_update_info_FS_after_the_installation_the"));
            
    m_EmergencyNote = qobject_cast<HbLabel *> (m_loader.findWidget(
            "lblEmergency"));
    m_EmergencyNote->setPlainText(hbTrId(
            "txt_device_update_info_FS_during_the_installation_the"));

    QObject::connect(m_PrimaryAction, SIGNAL(clicked()), this,
            SLOT(lSKSelected()));
    QObject::connect(m_SecondaryAction, SIGNAL(clicked()), this,
            SLOT(rSKSelected()));
    QObject::connect(m_dialog, SIGNAL(aboutToClose()), this,
            SLOT(aboutToClose()));

    refreshDialog();
    FLOG(_L("HbFotaFullscreenDialog::HbFotaFullscreenDialog <<"));
    }



// ---------------------------------------------------------------------------
// HbFotaFullscreenDialog::refreshDialog
// This function is used to refresh the contents of the full screen dialog
// once it is changed.
// ---------------------------------------------------------------------------
//


void HbFotaFullscreenDialog::refreshDialog()
    {
    FLOG(_L("HbFotaFullscreenDialog::RefreshDialog >>"));
    if (m_dialog)
        {
        FLOG(_L("HbFotaFullscreenDialog::idialog >>"));
        m_Clicked = EFalse;
        m_dialog->show();
        }
    FLOG(_L("HbFotaFullscreenDialog::RefreshDialog <<"));
    }

// ---------------------------------------------------------------------------
// HbFotaFullscreenDialog::setSoftwareDetails
// This function is called to set the details of the software,which is downloaded.
// ---------------------------------------------------------------------------
//

void HbFotaFullscreenDialog::setSoftwareDetails(int size,
        const QString version, const QString aName)
    {
    //set Sw Details
    FLOG(_L("HbFotaFullscreenDialog::RefreshDialog >>"));
    m_SwDetails = qobject_cast<HbLabel *> (m_loader.findWidget("lblSwVersion"));
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

    m_SwDetails->setPlainText(content);
    FLOG(_L("HbFotaFullscreenDialog::RefreshDialog <<"));
    }

// ---------------------------------------------------------------------------
// HbFotaFullscreenDialog::close
// This slot is called to manually close the dialog.
// ---------------------------------------------------------------------------
//

void HbFotaFullscreenDialog::close()
    {
    FLOG(_L("HbFotaFullscreenDialog::Close <<"));
    m_Clicked = ETrue;
    if (m_dialog)
        m_dialog->close();
    FLOG(_L("HbFotaFullscreenDialog::Close >>"));
    }

// ---------------------------------------------------------------------------
// HbFotaFullscreenDialog::setWarningDetails
// This function is to set the warnings details of the full screen dialog
// according to the state of the firmware update.
// ---------------------------------------------------------------------------
//

void HbFotaFullscreenDialog::setWarningDetails(TFotaWarningType aType)
    {
    FLOG(_L("HbFotaFullscreenDialog::SetWarningDetails <<"));
    //set heading content
    if (aType == EHbFotaDownload)
        {
        FLOG(_L("HbFotaFullscreenDialog::EHbFotaDownload"));
        m_dialog->setTimeout(HbPopup::NoTimeout);
        m_InstallNote->setVisible(false);
        m_RestartNote->setVisible(false);
        m_RestartIcon->setVisible(false);
        m_EmergencyNote->setVisible(false);
        m_EmergencyIcon->setVisible(false);
        }
    else if (aType == EHbFotaUpdate)
        {
        FLOG(_L("HbFotaFullscreenDialog::EHbFotaUpdate"));
        m_dialog->setTimeout(HbPopup::ContextMenuTimeout);
        HbIcon iconCharger;
        iconCharger.setIconName(":/icons/qtg_small_charger.svg");
        m_ChargerIcon->setIcon(iconCharger);
        HbIcon iconRestart;
        iconRestart.setIconName(":/icons/qtg_small_reboot.svg");
        m_RestartIcon->setIcon(iconRestart);
        m_InstallNote->setPlainText(hbTrId(
                "txt_device_update_info_installation_will_proceed_n"));
        m_ChargerNote->setPlainText(hbTrId(
                "txt_device_update_info_FS_its_recommended_to_connec"));
        m_RestartNote->setPlainText(hbTrId(
                "txt_device_update_info_FS_after_the_installation_the"));
        m_InstallNote->setVisible(true);
        m_RestartNote->setVisible(true);
        m_RestartIcon->setVisible(true);
        m_EmergencyNote->setVisible(true);
        m_EmergencyIcon->setVisible(true);
        m_DownloadState->setPlainText(hbTrId(
                "txt_device_update_info_download_complete"));
        m_DownloadState->setVisible(true);
        m_PrimaryAction->setEnabled(ETrue);
        }
    else if (aType == EHbFotaLowBattery)
        {
        FLOG(_L("HbFotaFullscreenDialog::EHbFotaLowBattery"));
        m_dialog->setTimeout(HbPopup::NoTimeout);
        HbIcon iconCharger;
        iconCharger.setIconName(":/icons/qtg_small_reboot.svg");
        m_ChargerIcon->setIcon(iconCharger);
        HbIcon iconRestart;
        iconRestart.setIconName(":/icons/qtg_small_phone_disabled.svg");
        m_RestartIcon->setIcon(iconRestart);
        m_InstallNote->setPlainText(hbTrId(
                "txt_device_update_info_to_proceed_with_installatio"));
        m_ChargerNote->setPlainText(hbTrId(
                "txt_device_update_info_FS_after_the_installation_the"));
        m_RestartNote->setPlainText(hbTrId(
                "txt_device_update_info_FS_during_the_installation_the"));
        m_InstallNote->setVisible(true);
        m_RestartNote->setVisible(true);
        m_RestartIcon->setVisible(true);
        m_EmergencyNote->setVisible(false);
        m_EmergencyIcon->setVisible(false);
        m_DownloadState->setPlainText(hbTrId(
                "txt_device_update_info_download_complete"));
        m_DownloadState->setVisible(true);
        m_PrimaryAction->setText(hbTrId("txt_device_update_button_continue"));
        m_SecondaryAction->setText(hbTrId(
                "txt_device_update_button_install_later"));
        m_PrimaryAction->setEnabled(EFalse);
        }
    refreshDialog();
    FLOG(_L("HbFotaFullscreenDialog::SetWarningDetails >>"));
    }

// ---------------------------------------------------------------------------
// HbFotaFullscreenDialog::updateprogressbar
// This function is called to update the progress bar with the download progress details/.
// ---------------------------------------------------------------------------
//

void HbFotaFullscreenDialog::updateProgressBar(TInt aProgress)
    {
    FLOG(_L("HbFotaFullscreenDialog::UpdateProgressBar <<"));
    m_progressBar->setProgressValue(aProgress);
    FLOG(_L("HbFotaFullscreenDialog::UpdateProgressBar >>"));
    }

// ---------------------------------------------------------------------------
// HbFotaFullscreenDialog::disableRSK
// This function is called disable the RSK of the dialog when the postpone limeit expires
// ---------------------------------------------------------------------------
//

void HbFotaFullscreenDialog::disableRSK(TBool aVal)
    {
    FLOG(_L("HbFotaFullscreenDialog::DisableRSK <<"));
    m_SecondaryAction->setEnabled(!aVal);
    FLOG(_L("HbFotaFullscreenDialog::DisableRSK >>"));
    }

// ---------------------------------------------------------------------------
// HbFotaFullscreenDialog::isLSKEnabled
// This function is called to check if left softkey of the fullscreen dialog is disabled or not.
// ---------------------------------------------------------------------------
//

bool HbFotaFullscreenDialog::isLSKEnabled()
    {
    FLOG(_L("HbFotaFullscreenDialog::IsLSKEnabled <<"));
    return (m_PrimaryAction->isEnabled());
    }

// ---------------------------------------------------------------------------
// HbFotaFullscreenDialog::cancelSelected
// This slot is called when user presses the left softkey of fullscreen dialog.
// ---------------------------------------------------------------------------
//

void HbFotaFullscreenDialog::lSKSelected()
    {
    FLOG(_L("FotaFullscreenDialog::LSKSelected <<"));
    m_Clicked = ETrue;
    m_dialog->hide();
    m_Server->HandleFullScreenDialogResponse(EHbLSK);
    FLOG(_L("HbFotaFullscreenDialog::LSKSelected >>"));
    }
	
	
	// ---------------------------------------------------------------------------
// HbFotaFullscreenDialog::cancelSelected
// This slot is called when user presses the right softkey of fullscreen dialog.
// ---------------------------------------------------------------------------
//

void HbFotaFullscreenDialog::rSKSelected()
    {
    FLOG(_L("HbFotaFullscreenDialog::RSKSelected >>"));
    m_Clicked = ETrue;
    m_dialog->hide();
    m_Server->HandleFullScreenDialogResponse(EHbRSK);
    FLOG(_L("HbFotaFullscreenDialog::RSKSelected <<"));
    }

// ---------------------------------------------------------------------------
// HbFotaFullscreenDialog::~HbFotaFullscreenDialog
// Destructor which destroys all the widgets
// ---------------------------------------------------------------------------
//

HbFotaFullscreenDialog::~HbFotaFullscreenDialog()
    {
    FLOG(_L("~ HbFotaFullscreenDialog <<"));

    QObject::disconnect(m_PrimaryAction, SIGNAL(clicked()), this,
            SLOT(LSKSelected()));
    QObject::disconnect(m_SecondaryAction, SIGNAL(clicked()), this,
            SLOT(RSKSelected()));
    QObject::disconnect(m_dialog, SIGNAL(aboutToClose()), this,
            SLOT(aboutToClose()));

    if (m_progressBar)
        m_progressBar->deleteLater();

    if (m_PrimaryAction)
        delete m_PrimaryAction;

    if (m_SecondaryAction)
        delete m_SecondaryAction;

    if (m_Title)
        delete m_Title;

    if (m_SwDetails)
        delete m_SwDetails;

    if (m_InstallNote)
        delete m_InstallNote;

    if (m_RestartNote)
        delete m_RestartNote;

    if (m_RestartIcon)
        delete m_RestartIcon;

    if (m_EmergencyNote)
        delete m_EmergencyNote;

    if (m_EmergencyIcon)
        delete m_EmergencyIcon;

    if (m_DownloadState)
        delete m_DownloadState;

    if (m_dialog)
        delete m_dialog;

    FLOG(_L("~ HbFotaFullscreenDialog >>"));
    }

// ---------------------------------------------------------------------------
// HbFotaFullscreenDialog::showUpdateDialog
// This function is called when download is complete and update dialog has to be shown.
// ---------------------------------------------------------------------------
//

void HbFotaFullscreenDialog::showUpdateDialog()
    {
    FLOG(_L("HbFotaFullscreenDialog::ShowUpdateDialog <<"));
    m_PrimaryAction->setText(hbTrId("txt_common_button_continue_dialog"));
    m_SecondaryAction->setText(
            hbTrId("txt_device_update_button_install_later"));
    setWarningDetails(EHbFotaUpdate);
    FLOG(_L("HbFotaFullscreenDialog::ShowUpdateDialog >>"));
    }


// ---------------------------------------------------------------------------
// HbFotaFullscreenDialog::setVisible
// This function is called to make the fullscreen dialog visible.
// ---------------------------------------------------------------------------
//

void HbFotaFullscreenDialog::setVisible(TBool aVisible)
{
    FLOG(_L("RefreshDialog::SetVisible >>"));
    refreshDialog();
    FLOG(_L("RefreshDialog::SetVisible <<"));
}
// ---------------------------------------------------------------------------
// HbFotaFullscreenDialog::aboutToClose
// This slot is called when full screen dialogs is cloased during a timeout.
// ---------------------------------------------------------------------------
//

void HbFotaFullscreenDialog::aboutToClose()
    {
    FLOG(_L("HbFotaFullscreenDialog::aboutToClose <<"));
    if (!m_Clicked)
        {
        FLOG(_L("Calling fotaserver as timedout..."));
        m_Server->HandleFullScreenDialogResponse(EHbLSK);
        }
    m_Clicked = EFalse;
    FLOG(_L("HbFotaFullscreenDialog::aboutToClose >>"));
    }


