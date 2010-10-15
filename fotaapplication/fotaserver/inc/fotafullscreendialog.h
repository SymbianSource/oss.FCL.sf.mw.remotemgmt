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
 *  HbFotaFullscreenDialog
 * 
 */
class HbFotaFullscreenDialog : public QObject
    {
Q_OBJECT

public:

		/**
     * Constructor to construct the fullscreen dialog.
     * @param   aObserver - Observer to notify the user inputs from the fullscreen dialog
     * @return   void
     */
    HbFotaFullscreenDialog(CFotaServer* aObserver);


		/**
     * Destructor for the full screen dialog
     * @return   void
     */
    ~HbFotaFullscreenDialog();

    /**
     * This function updates the details of the update to the fullscreen dialog.
     * @param   size - The total size of the update package.
     * @param   version - The version of the soeftware that is being updated.
     * @param   aName - the package name of the current update.
     * @return   void
     */
    void setSoftwareDetails(int size, const QString version,
            const QString aName);

    /**
     * This function updates the download progress bar to the progress value passed.
     * @param   aType - the state in which the firmware update is in(downloading or donwload complete)
     * @return   void
     */
    void setWarningDetails(TFotaWarningType aType);

    /**
     * This function updates the download progress bar to the progress value passed.
     * @param   aProgress - the progress value to be update to the progress bar.
     * @return   void
     */
    void updateProgressBar(TInt aProgress);

    /**
     * Used to change the warnings and softkeys required for the update dialog
     * @param   void
     * @return   void
     */
    void showUpdateDialog();

    /**
     * Used to refresh the dialog when the content of the dialog content changes.
     * @param   void
     * @return   void
     */
    void close();

    /**
     * Used to disable the RSK of the dialog when resume dialogs
     * @param   aVal - to enable or disable the key
     * @return   void
     */
    void disableRSK(TBool aVal);

    /**
     * Used to determine if the LSK of the dialog is enabled or not.
     * @return   bool: returns if LSK id enabled or disabled.
     */
    bool isLSKEnabled();

    /**
     * Used to set the fullscreen dialog visible
     * @param   aVisible - To set the visibility to true/false.
     * @return   void
     */
    void setVisible(TBool aVisible);

private:

    /**
     * Used to refresh the dialog when the content of the dialog changes.
     * @param   void
     * @return   void
     */
    void refreshDialog();

public slots:

    /**
     * Slot to be called when user selects the Left soft key.
     * @param   void
     * @return   void
     */
    void lSKSelected();

    /**
     * Slot to be called when user selects the Right soft key.
     * @param   void
     * @return   void
     */
    void rSKSelected();

    /**
     * Slot to be called when the dialog is about to get closed.
     * @param   void
     * @return   void
     */
    void aboutToClose();

private:

    // dialog instance
    HbDialog * m_dialog;

    //document loader to load the widgets
    HbDocumentLoader m_loader;

    //progress bar for updating download progress
    HbProgressBar * m_progressBar;

    // Fota server instance to send the keypress event
    CFotaServer* m_Server;

    // LSK of the dialog
    HbPushButton * m_PrimaryAction;

    // RSK of the dialog
    HbPushButton * m_SecondaryAction;

    // Flag to differentiate from keypress and dialog timeout case.
    TBool m_Clicked;

    // Warning note displayed in the full screen dialog 
    HbLabel *m_InstallNote;

    // Warning note displayed in the full screen dialog
    HbLabel *m_RestartNote;

    // Warning note displayed in the full screen dialog
    HbLabel *m_RestartIcon;

    // Warning note displayed in the full screen dialog
    HbLabel *m_EmergencyNote;

    // Warning note displayed in the full screen dialog
    HbLabel *m_EmergencyIcon;

    // Warning note displayed in the full screen dialog
    HbLabel *m_ChargerNote;

		// Variable which holds the charger icon from the docml.
    HbLabel *m_ChargerIcon;

    // To display the state of the download above progress bar
    HbLabel *m_DownloadState;

    // Fullscreen dialog title.
    HbLabel *m_Title;
    // Firmware details that is to be shwon in full screen dialog.
    HbLabel *m_SwDetails;
    };

#endif // CLASSNAME_H
