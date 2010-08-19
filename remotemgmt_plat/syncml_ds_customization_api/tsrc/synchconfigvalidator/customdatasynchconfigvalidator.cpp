/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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

// CLASS HEADER
#include "customdatasynchconfigvalidator.h"

// INTERNAL INCLUDES
#include "customdatasynchconfigvalidatorenginewrapper.h"

// EXTERNAL INCLUDES
#include <qmessagebox.h>
#include <qgraphicseffect.h>
#include <qlabel.h>

const int TESTNAMECOLUMN = 0;
const int RESULTCOLUMN = 1;
const int MOREINFOCOLUMN = 2;
const QString MOREINFO_SYMBOL = "!";
const QString MOREINFO_VERIFY_BY_USER_SYMBOL = "?";
const QString NOK_SYMBOL = "NOK";
const QString OK_SYMBOL = "OK";

// ----------------------------------------------------------------------------
void CustomDataSynchConfigValidator::Initialize()
{
    //TODO: cleanup QTableWidgetItem settings and harmonize layout for columns
    QFont font;
    font.setPointSize(4);
    ui.buttonTestActivator->setFont(font);
    for (int r = 0; r < ui.tableWidget->rowCount(); r++) {
        for (int c = 0; c < ui.tableWidget->columnCount(); c++) {
            ui.tableWidget->item(r, c)->setFlags(ui.tableWidget->item(r, c)->flags()
                & ~Qt::ItemIsEditable);
            ui.tableWidget->item(r, c)->setFont(font);
        }
    }
    //Split columns widths for first time
    const int descriptionColumnInitialWidth = 260;
    ui.tableWidget->setColumnWidth(TESTNAMECOLUMN, descriptionColumnInitialWidth);
    ui.tableWidget->resizeColumnToContents(RESULTCOLUMN);
    ui.tableWidget->resizeColumnToContents(MOREINFOCOLUMN);
}

// ----------------------------------------------------------------------------
CustomDataSynchConfigValidator::CustomDataSynchConfigValidator(QWidget *parent) :
    QMainWindow(parent)
{
    ui.setupUi(this);
    Initialize();

    mEngineWrapper = new CustomDataSynchConfigValidatorEngineWrapper(this);

    bool connectSucceeded;
    connectSucceeded = QObject::connect(ui.buttonTestActivator, SIGNAL(clicked()), this,
        SLOT(activateValidation()));
    connectSucceeded = QObject::connect(ui.tableWidget, SIGNAL(itemClicked(QTableWidgetItem *)),
        this, SLOT(handleMoreInfoItemsClicks(QTableWidgetItem *)));
}

// ----------------------------------------------------------------------------
void CustomDataSynchConfigValidator::resizeEvent(QResizeEvent* event)
{
    //TODO: in basic QT how to detect landscape vs portrait
    if (ui.tableWidget && ui.tableWidget->isVisible()) {
        ui.tableWidget->resizeColumnToContents(RESULTCOLUMN);//result column
        ui.tableWidget->resizeColumnToContents(MOREINFOCOLUMN);//info column
    }
    QMainWindow::resizeEvent(event);
}

// ----------------------------------------------------------------------------
CustomDataSynchConfigValidator::~CustomDataSynchConfigValidator()
{
}

// ----------------------------------------------------------------------------
void CustomDataSynchConfigValidator::activateValidation()
{
    bool cenrepCheck;
    for (int r = 0; r < ui.tableWidget->rowCount(); r++) {
        if (matchCheckNameWithTableRowIdAndString(r, "OperatorDataSynchErrorCenrepExistsTest")) {
            cenrepCheck = mEngineWrapper->operatordataSynchErrorCenrepExists();
            ui.tableWidget->item(r, RESULTCOLUMN)->setText(cenrepCheck ? OK_SYMBOL : NOK_SYMBOL);
            ui.tableWidget->item(r, MOREINFOCOLUMN)->setText(cenrepCheck ? "" : MOREINFO_SYMBOL);
        }
        else if (matchCheckNameWithTableRowIdAndString(r, "OperatorDataSynchCenrepExistsTest")) {
            cenrepCheck = mEngineWrapper->operatordataSynchCenrepExists();
            ui.tableWidget->item(r, RESULTCOLUMN)->setText(cenrepCheck ? OK_SYMBOL : NOK_SYMBOL);
            ui.tableWidget->item(r, MOREINFOCOLUMN)->setText(cenrepCheck ? "" : MOREINFO_SYMBOL);
        }
        else if (matchCheckNameWithTableRowIdAndString(r, "DataSynchCenrepExistsTest")) {
            cenrepCheck = mEngineWrapper->dataSynchCenrepExists();
            ui.tableWidget->item(r, RESULTCOLUMN)->setText(cenrepCheck ? OK_SYMBOL : NOK_SYMBOL);
            ui.tableWidget->item(r, MOREINFOCOLUMN)->setText(cenrepCheck ? "" : MOREINFO_SYMBOL);
        }
        else if (matchCheckNameWithTableRowIdAndString(r, "CustomProfilesTest")) {
            cenrepCheck = mEngineWrapper->customSynchProfilesAllowed();
            ui.tableWidget->item(r, RESULTCOLUMN)->setText(cenrepCheck ? OK_SYMBOL : NOK_SYMBOL);
            ui.tableWidget->item(r, MOREINFOCOLUMN)->setText(cenrepCheck ? "" : MOREINFO_SYMBOL);
        }
        else if (matchCheckNameWithTableRowIdAndString(r, "ContactsAdapterAvailabilityTest")) {
            bool adapterCheck = mEngineWrapper->configuredContactsAdapterExists(
                mContactsAdapterAvailabilityTestErrorNote);
            ui.tableWidget->item(r, RESULTCOLUMN)->setText(adapterCheck ? OK_SYMBOL : NOK_SYMBOL);
            ui.tableWidget->item(r, MOREINFOCOLUMN)->setText(adapterCheck ? "" : MOREINFO_SYMBOL);
        }
        else if (matchCheckNameWithTableRowIdAndString(r, "CustomSynchProfileServerIdTest")) {
            cenrepCheck = mEngineWrapper->configuredCustomSynchProfileExists(mCustomSynchProfileServerIdTestErrorNote);
            ui.tableWidget->item(r, RESULTCOLUMN)->setText(cenrepCheck ? OK_SYMBOL : NOK_SYMBOL);
            ui.tableWidget->item(r, MOREINFOCOLUMN)->setText(cenrepCheck ? "" : MOREINFO_SYMBOL);
        }
        else if (matchCheckNameWithTableRowIdAndString(r, "ContactsAdapterSynchProfileTest")) {
            cenrepCheck = mEngineWrapper->contactAdapterCenrepValueToProfileMatches(mContactsAdapterSynchProfileTestErrorNote);
            ui.tableWidget->item(r, RESULTCOLUMN)->setText(cenrepCheck ? OK_SYMBOL : NOK_SYMBOL);
            ui.tableWidget->item(r, MOREINFOCOLUMN)->setText(cenrepCheck ? "" : MOREINFO_SYMBOL);
        }
        else if (matchCheckNameWithTableRowIdAndString(r, "MyprofileAdapterSynchProfileTest")) {
            cenrepCheck = mEngineWrapper->myprofileAdapterCenrepValueToProfileMatches(mMyprofileAdapterSynchProfileTestErrorNote);
            ui.tableWidget->item(r, RESULTCOLUMN)->setText(cenrepCheck ? OK_SYMBOL : "VERIFY");
            ui.tableWidget->item(r, MOREINFOCOLUMN)->setText(cenrepCheck ? "" : MOREINFO_VERIFY_BY_USER_SYMBOL);
        }
        // Check if Device info extension plugin exists
        else if (matchCheckNameWithTableRowIdAndString(r, "DeviceInfoExtensionPluginTest"))
        {
            bool devInfoExtCheck = mEngineWrapper->devInfoExtensionPluginAvailable();
            ui.tableWidget->item(r, RESULTCOLUMN)->setText(
                devInfoExtCheck ? OK_SYMBOL : "VERIFY");
            ui.tableWidget->item(r, MOREINFOCOLUMN)->setText(
                devInfoExtCheck ? "" : MOREINFO_VERIFY_BY_USER_SYMBOL);
        }
        else {
            continue;
        }
    }
}

// ----------------------------------------------------------------------------
void CustomDataSynchConfigValidator::handleMoreInfoItemsClicks(
    QTableWidgetItem *itemClicked)
{
    if ((0 == itemClicked->text().compare(MOREINFO_SYMBOL))||(0 == itemClicked->text().compare(MOREINFO_VERIFY_BY_USER_SYMBOL))) {
        //user wants to have some info,
        //and possible hint regarding how to solve NOK case
        int r = itemClicked->row();

        QScopedPointer<QMessageBox> msgBox(new QMessageBox());
        msgBox->setWindowTitle(ui.tableWidget->verticalHeaderItem(r)->text());

        if (matchCheckNameWithTableRowIdAndString(r, "OperatorDataSynchErrorCenrepExistsTest")) {
            QString
                line1 =
                    "OperatorDataSynchErrorCenrep ini file z:\\private\\10202be9\\2001FDF1.txt was not found from image";
            QString line2 = "\n\nCheck image creation \\output subfolder for the cenrep ini file.";
            QString line3 =
                "\n\nCheck also that .\\s60\\root.confml that operatordatasyncerror.confml is included.";
            msgBox->setText(line1 + line2 + line3);
        }
        else if (matchCheckNameWithTableRowIdAndString(r, "OperatorDataSynchCenrepExistsTest")) {
            QString line1 =
                "OperatorDataSynchCenrep ini file z:\\private\\10202be9\\2001E2E1.txt was not found from image";
            QString line2 = "\n\nCheck image creation \\output subfolder for the cenrep ini file.";
            QString line3 =
                "\n\nCheck also that .\\s60\\root.confml that operatordatasyncerror.confml is included.";
            msgBox->setText(line1 + line2 + line3);
        }
        else if (matchCheckNameWithTableRowIdAndString(r, "DataSynchCenrepExistsTest")) {
            QString line1 =
                "DataSynchCenrep ini file z:\\private\\10202be9\\2000CF7E.txt was not found from image";
            QString line2 = "\n\nCheck image creation \\output subfolder for the cenrep ini file.";
            QString line3 =
                "\n\nCheck also that .\\s60\\root.confml that operatordatasyncerror.confml is included.";
            msgBox->setText(line1 + line2 + line3);
        }
        else if (matchCheckNameWithTableRowIdAndString(r, "CustomProfilesTest")) {
            QString line1 =
                "Support for custom synch profiles defined using z:\\private\\101F99FB\\variantdata.xml is (0) now disabled in cenrep.";
            QString
                line2 =
                    "\n\nCheck that variant.confml has KCRUidDataSyncInternalKeys cenrep key KNsmlDsCustomProfiles set to 1.";
            QString
                line3 =
                    "\n\nCheck also that DataSynchCenrep ini file z:\\private\\10202be9\\2000CF7E.txt exists in image, see check above.";
            msgBox->setText(line1 + line2 + line3);
        }
        else if (matchCheckNameWithTableRowIdAndString(r, "ContactsAdapterAvailabilityTest")) {
            QString line1 = "Configured contact adapter does not exist.";
            QString line2 = "\n\n" + mContactsAdapterAvailabilityTestErrorNote;
            QString line3 = "\n\n";
            msgBox->setText(line1 + line2 + line3);
        }
        else if (matchCheckNameWithTableRowIdAndString(r, "CustomSynchProfileServerIdTest")) {
            QString line1 = "ServerId match failed."+mCustomSynchProfileServerIdTestErrorNote;
            QString line2 = 
                "\n\noperator datasync cenrep z:\\private\\10202be9\\2001E2E1.txt and synch profile was not found.";
            QString line3 = "\n\nCheck variantdata.xml also.";
            msgBox->setText(line1 + line2 + line3);
        }
        else if (matchCheckNameWithTableRowIdAndString(r, "ContactsAdapterSynchProfileTest")) {
            QString errorMsg = mContactsAdapterSynchProfileTestErrorNote;
            msgBox->setText(errorMsg);
        }
        else if (matchCheckNameWithTableRowIdAndString(r, "MyprofileAdapterSynchProfileTest")) {
            QString errorMsg = mMyprofileAdapterSynchProfileTestErrorNote;
            msgBox->setText(errorMsg);
        }
        // DevInfo extension error note
        else if (matchCheckNameWithTableRowIdAndString(r, "DeviceInfoExtensionPluginTest")) {
            msgBox->setText("DevInfo extension plugin not found.");
        }
        QGraphicsOpacityEffect* effect = new QGraphicsOpacityEffect();
        effect->setOpacity(0.85);
        msgBox->setGraphicsEffect(effect);
        msgBox->exec();
    }
}

// ----------------------------------------------------------------------------
bool CustomDataSynchConfigValidator::matchCheckNameWithTableRowIdAndString(
    int row, QString matchString)
{
    bool match = ui.tableWidget->verticalHeaderItem(row)->text().compare(matchString) == 0 ? true
        : false;
    return match;
}

// End of file
