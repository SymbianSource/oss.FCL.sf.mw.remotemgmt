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

#ifndef FOTAFULLSCREENDIALOG_H
#define FOTAFULLSCREENDIALOG_H

// INCLUDES

#include <QWidget>
#include <QtGui/QWidget>
#include <QtGui>
#include <hbdialog.h>
#include <hbdocumentloader.h>
#include <hbprogressbar.h>
#include <hbdataform.h>
#include <hbdataformmodel.h>
#include <hbpushbutton.h>
#include <hblabel.h>

#include "FotaDlClient.h"
#include "FotaServer.h"

enum TFotaWarningType
    {
    EHbFotaDownload = 1, EHbFotaUpdate, EHbFotaLowBattery
    };

// CLASS DECLARATION

/**
 *  FotaFullscreenDialog
 * 
 */
class FotaFullscreenDialog : public QObject
    {
Q_OBJECT

public:

    FotaFullscreenDialog(CFotaServer* aObserver);

    ~FotaFullscreenDialog();

    /**
     * This function updates the details of the update to the fullscreen dialog.
     * @param   size - The total size of the update package.
     * @param   version - The version of the soeftware that is being updated.
     * @param   aName - the package name of the current update.
     * @return   void
     */
    void SetSoftwareDetails(int size, const QString version,
            const QString aName);

    /**
     * This function updates the download progress bar to the progress value passed.
     * @param   aType - the state in which the firmware update is in(downloading or donwload complete)
     * @return   void
     */
    void SetWarningDetails(TFotaWarningType aType);

    /**
     * This function updates the download progress bar to the progress value passed.
     * @param   aProgress - the progress value to be update to the progress bar.
     * @return   void
     */
    void UpdateProgressBar(TInt aProgress);

    /**
     * Used to change the warnings and softkeys required for the update dialog
     * @param   void
     * @return   void
     */
    void ShowUpdateDialog();

    /**
     * Used to refresh the dialog when the content of the dialog content changes.
     * @param   void
     * @return   void
     */
    void Close();

    /**
     * Used to disable the RSK of the dialog when resume dialogs
     * @param   aVal - to enable or disable the key
     * @return   void
     */
    void DisableRSK(TBool aVal);

    bool IsLSKEnabled();

    void SetVisible(TBool aVisible);

private:

    /**
     * Used to refresh the dialog when the content of the dialog changes.
     * @param   void
     * @return   void
     */
    void RefreshDialog();

public slots:

    /**
     * Slot to be called when user selects the Left soft key.
     * @param   void
     * @return   void
     */
    void LSKSelected();

    /**
     * Slot to be called when user selects the Right soft key.
     * @param   void
     * @return   void
     */
    void RSKSelected();

    /**
     * Slot to be called when the dialog is about to get closed.
     * @param   void
     * @return   void
     */
    void aboutToClose();

private:

    // dialog instance
    HbDialog * idialog;

    //document loader to load the widgets
    HbDocumentLoader iloader;

    //progress bar for updating download progress
    HbProgressBar * iprogressBar;

    // Fota server instance to send the keypress event
    CFotaServer* iServer;

    // LSK of the dialog
    HbPushButton * iPrimaryAction;

    // RSK of the dialog
    HbPushButton * iSecondaryAction;

    // Flag to differentiate from keypress and dialog timeout case.
    TBool iClicked;

    // Warning note displayed in the full screen dialog 
    HbLabel *iInstallNote;

    // Warning note displayed in the full screen dialog
    HbLabel *iRestartNote;

    // Warning note displayed in the full screen dialog
    HbLabel *iRestartIcon;

    // Warning note displayed in the full screen dialog
    HbLabel *iEmergencyNote;

    // Warning note displayed in the full screen dialog
    HbLabel *iEmergencyIcon;

    // Warning note displayed in the full screen dialog
    HbLabel *iChargerNote;

    HbLabel *iChargerIcon;

    // To display the state of the download above progress bar
    HbLabel *iDownloadState;

    // Fullscreen dialog title.
    HbLabel *iTitle;
    // Firmware details that is to be shwon in full screen dialog.
    HbLabel *iSwDetails;
    };

#endif // CLASSNAME_H
