/*
* Copyright (c) 2000 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description: Implementation of policymanagement components
*
*/

#include "PolicyEngineUI.h"
#include <hbpopup.h>
#include <hbinputdialog.h>
#include <hbdocumentloader.h>
#include <hbdialog.h>
#include <hblabel.h>
#include <hbaction.h>
#include <hbnotificationdialog.h>
#include <hbmessagebox.h>
#include <hbdevicenotificationdialog.h>
#include "Logger.h"

const int KMaxTries = 4;
const int KMaxCodeLength = 4;


// -----------------------------------------------------------------------------
// PolicyEngineUI::PolicyEngineUI()
// -----------------------------------------------------------------------------
PolicyEngineUI::PolicyEngineUI(const QVariantMap& parameters)
    {
    estbTrustCount = KMaxTries;
    DisplayNotificationDialog(parameters);
    }


// -----------------------------------------------------------------------------
// PolicyEngineUI::~PolicyEngineUI()
// -----------------------------------------------------------------------------
PolicyEngineUI::~PolicyEngineUI()
    {

    }


// -----------------------------------------------------------------------------
// PolicyEngineUI::DisplayNotificationDialog()
// Read the parameters sent from client
// -----------------------------------------------------------------------------
void PolicyEngineUI::DisplayNotificationDialog(const QVariantMap& parameters)
    {
    LOGSTRING( "+ DisplayNotificationDialog +" );
    QVariantMap::const_iterator i = parameters.constBegin();

    while (i != parameters.constEnd())
        {
        if (i.key().toAscii() == "serverdisplayname")
            {
            iServerName = i.value().toString();
            }
        else if (i.key().toAscii() == "fingerprint")
            {
            iFingerprint = i.value().toString();
            }
        ++i;
        }
    
    TBuf<50> server(iServerName.utf16());
    TBuf<10> buffer(iFingerprint.utf16());

    LOGSTRING2( "serverdisplayname %S", &server );
    LOGSTRING2( "fingerprint %S", &buffer );

    ShowInputDialog();
    LOGSTRING( "- DisplayNotificationDialog -" );
    }


// -----------------------------------------------------------------------------
// PolicyEngineUI::ShowInputDialog()
// Show the accept control dialog
// -----------------------------------------------------------------------------
void PolicyEngineUI::ShowInputDialog()
    {
    LOGSTRING( "+ ShowInputDialog +" );
    HbDocumentLoader loader;
    bool ok = false;
    loader.load(":/xml/dialog.docml", &ok);
    if (!ok)
        {
        return;
        }

    HbDialog *dialog1 =
            qobject_cast<HbDialog *> (loader.findWidget("dialog"));

    //set heading content
    HbLabel *contentheading = qobject_cast<HbLabel *> (loader.findWidget(
            "qtl_dialog_pri_heading"));
    QString heading(hbTrId("txt_device_update_title_security_information"));
    contentheading->setPlainText(heading);

    //set body content
    HbLabel *contentbody = qobject_cast<HbLabel *> (loader.findWidget(
            "qtl_dialog_pri5"));

    QString body(
            (hbTrId("txt_device_update_info_1_server_wants_to_contro").arg(
                    iServerName)));
    contentbody->setTextWrapping(Hb::TextWordWrap);
    contentbody->setPlainText(body);

    HbAction *primaryAction = qobject_cast<HbAction *> (
            dialog1->actions().at(0));
    HbAction *secondaryAction = qobject_cast<HbAction *> (
            dialog1->actions().at(1));

    //set dialog properties
    dialog1->setTimeout(HbPopup::NoTimeout);
    dialog1->setDismissPolicy(HbPopup::NoDismiss);

    QObject::connect(primaryAction, SIGNAL(triggered()), this,
            SLOT(onOKSelected()));
    QObject::connect(secondaryAction, SIGNAL(triggered()), this,
            SLOT(onCancelSelected()));

	emit applicationReady();
    if (dialog1)
        dialog1->show();

    LOGSTRING( "- ShowInputDialog -" );
    }


// -----------------------------------------------------------------------------
// PolicyEngineUI::onOKSelected()
// Show the user input dialog once control is accepted
// -----------------------------------------------------------------------------
void PolicyEngineUI::onOKSelected()
    {

    LOGSTRING( "+ onOKSelected +" );

    HbDocumentLoader loader;
    bool ok = false;
    loader.load(":/xml/InputDialog.docml", &ok);
    if (!ok)
        {
        return;
        }

    mdialog = qobject_cast<HbDialog *> (loader.findWidget("dialog"));

    //set heading content
    HbLabel *contentheading = qobject_cast<HbLabel *> (loader.findWidget(
            "qtl_dialog_pri_heading"));
    QString heading(hbTrId("txt_device_update_title_security_information"));
    contentheading->setPlainText(heading);

    //set label
    HbLabel *contentlabel = qobject_cast<HbLabel *> (loader.findWidget(
            "HeadingLabel"));
    contentlabel->setTextWrapping(Hb::TextWordWrap);
    QString label(
                (hbTrId("txt_device_update_info_enter_first_4_characters_of").arg(
                        iServerName)));
    contentlabel ->setPlainText(label);

    //set length for editline
    mContentEdit = qobject_cast<HbLineEdit*> (loader.findWidget("InputLine"));
    mContentEdit->setMaxLength(KMaxCodeLength);

    //set dialog properties
    mdialog->setTimeout(HbPopup::NoTimeout);
    mdialog->setDismissPolicy(HbPopup::NoDismiss);
    
    HbAction* primaryAction = (HbAction*) (mdialog->actions().at(0));
    HbAction *secondaryAction = (HbAction*) (mdialog->actions().at(1));
    primaryAction->setEnabled(false);

    QObject::connect(primaryAction, SIGNAL(triggered()), this,
            SLOT(establishTrust()));
    QObject::connect(secondaryAction, SIGNAL(triggered()), this,
            SLOT(cancelTrust()));
    QObject::connect(mContentEdit, SIGNAL(contentsChanged()), this,
            SLOT(codeTextChanged()));

    if (mdialog)
        mdialog->show();

    LOGSTRING( "- onOKSelected -" );
    }


// -----------------------------------------------------------------------------
// PolicyEngineUI::onCancelSelected()
// -----------------------------------------------------------------------------
void PolicyEngineUI::onCancelSelected()
    {
    LOGSTRING( "+ onCancelSelected +" );

    QVariantMap result;

    result.insert("keyResponse", -1);
    emit
    deviceDialogData(result);

    emit
    deviceDialogClosed();

    LOGSTRING( "- onCancelSelected -" );
    }


// -----------------------------------------------------------------------------
// PolicyEngineUI::onTrustCreation()
// -----------------------------------------------------------------------------
void PolicyEngineUI::onTrustCreation()
    {
    LOGSTRING( "+ onTrustCreation +" );
    QVariantMap result;

    result.insert("keyResponse", 0);
    emit
    deviceDialogData(result);

    emit
    deviceDialogClosed();

    LOGSTRING( "- onTrustCreation -" );
    }


// -----------------------------------------------------------------------------
// PolicyEngineUI::establishTrust()
// -----------------------------------------------------------------------------
void PolicyEngineUI::establishTrust()
    {
    LOGSTRING( "+ establishTrust +" );
    bool retVal; // return value from code verification API.
    estbTrustCount--;
    retVal = verifyFingerPrint();

    if (retVal)
        {
        LOGSTRING( "Trust Created" );
        //Reset the counter.
        estbTrustCount = 0;
        onTrustCreation();

        }
    else
        {
        //check the number of tries.
        if (estbTrustCount == 0)
            {
            //Access denied.
            LOGSTRING( "Access Denied" );
            HbMessageBox *msgBox = new HbMessageBox();
            msgBox->setHeadingWidget(new HbLabel(hbTrId(
                    "txt_device_update_title_security_information")));

            msgBox->setText(hbTrId(
                    "txt_device_update_info_security_info_access_denied"));
            msgBox->setTimeout(HbDialog::NoTimeout);
            msgBox->setDismissPolicy(HbDialog::NoDismiss);

            HbAction *action = (HbAction*) (msgBox->actions().at(0));
            QObject::connect(action, SIGNAL(triggered()), this,
                    SLOT(onCancelSelected()));

            msgBox->show();
            }
        else
            {
            LOGSTRING2( "tries left %i", estbTrustCount );
            //Code didnot match.
            HbDocumentLoader loader;
            bool ok = false;
            loader.load(":/xml/InputDialog.docml", &ok);
            if (!ok)
                {
                return;
                }

            mdialog = qobject_cast<HbDialog *> (loader.findWidget("dialog"));

            //set heading content
            HbLabel *contentheading = qobject_cast<HbLabel *> (
                    loader.findWidget("qtl_dialog_pri_heading"));
            contentheading->setTextWrapping(Hb::TextWordWrap);
            QString heading(hbTrId(
                    "txt_device_update_title_security_information"));
            contentheading->setPlainText(heading);

            //set label
            HbLabel *contentlabel = qobject_cast<HbLabel *> (
                    loader.findWidget("HeadingLabel"));
            QString
                    label(
                            (hbTrId(
                                    "txt_device_update_info_security_information_did_no").arg(
                                    estbTrustCount)));

            contentlabel->setTextWrapping(Hb::TextWordWrap);
            contentlabel->setPlainText(label);

            mContentEdit = qobject_cast<HbLineEdit*> (loader.findWidget(
                    "InputLine"));
            mContentEdit->setMaxLength(KMaxCodeLength);

            mdialog->setTimeout(HbPopup::NoTimeout);
            mdialog->setDismissPolicy(HbPopup::NoDismiss);

            HbAction* primaryAction = qobject_cast<HbAction*> (
                    mdialog->actions().at(0));
            HbAction *secondaryAction = qobject_cast<HbAction*> (
                    mdialog->actions().at(1));
            primaryAction->setEnabled(false);

            QObject::connect(primaryAction, SIGNAL(triggered()), this,
                    SLOT(establishTrust()));
            QObject::connect(secondaryAction, SIGNAL(triggered()), this,
                    SLOT(cancelTrust()));
            QObject::connect(mContentEdit, SIGNAL(contentsChanged()), this,
                    SLOT(codeTextChanged()));

            if (mdialog)
                mdialog->show();
            }
        }
    LOGSTRING( "- establishTrust -" );
    }


// -----------------------------------------------------------------------------
// PolicyEngineUI::cancelTrust()
// -----------------------------------------------------------------------------
void PolicyEngineUI::cancelTrust()
    {
    LOGSTRING( "+ cancelTrust +" );
    //Reset the counter.
    estbTrustCount = 0;

    QVariantMap result;
    result.insert("keyResponse", -1);
    emit
    deviceDialogData(result);

    emit
    deviceDialogClosed();
    LOGSTRING( "- cancelTrust -" );
    }


// -----------------------------------------------------------------------------
// PolicyEngineUI::setDeviceDialogParameters()
// -----------------------------------------------------------------------------
bool PolicyEngineUI::setDeviceDialogParameters(const QVariantMap &parameters)
    {
	Q_UNUSED(parameters);
    LOGSTRING( "+ setDeviceDialogParameters +" );
    return true;
    }


// -----------------------------------------------------------------------------
// PolicyEngineUI::deviceDialogError()
// Get error
// -----------------------------------------------------------------------------
int PolicyEngineUI::deviceDialogError() const
    {
    LOGSTRING( "+ deviceDialogError +" );
    return 0;
    }


// -----------------------------------------------------------------------------
// PolicyEngineUI::PolicyEngineUI()
// Close device dialog
// -----------------------------------------------------------------------------
void PolicyEngineUI::closeDeviceDialog(bool byClient)
    {
	Q_UNUSED(byClient);
    LOGSTRING( "+ closeDeviceDialog +" );
    close();
    }


// -----------------------------------------------------------------------------
// PolicyEngineUI::PolicyEngineUI()
// Return display widget
// -----------------------------------------------------------------------------
HbDialog *PolicyEngineUI::deviceDialogWidget() const
    {
    LOGSTRING( "+ deviceDialogWidget +" );
    return const_cast<PolicyEngineUI*> (this);
    }


// -----------------------------------------------------------------------------
// PolicyEngineUI::PolicyEngineUI()
// Verify the user entered code
// -----------------------------------------------------------------------------
bool PolicyEngineUI::verifyFingerPrint()
    {
    LOGSTRING( "+ verifyFingerPrint +" );

    QString enteredCode = mContentEdit->text();
    TBuf<10> buffer(enteredCode.utf16());

    LOGSTRING2( "User entered code %i", &buffer);
    LOGSTRING2( "User entered code %S", &buffer);

    if (enteredCode.compare(iFingerprint, Qt::CaseInsensitive) == 0)
    {
        LOGSTRING( "+ fingerprint MATCH !!!! +" );
        return true;
    }
    else
    {
        LOGSTRING( "+  fingerprint DOESNOT MATCH  +" );
        return false;
    }
    }


// -----------------------------------------------------------------------------
// PolicyEngineUI::codeTextChanged()
// checks the entered text length and enables the OK option accordingly.
// -----------------------------------------------------------------------------
void PolicyEngineUI::codeTextChanged()
    {
    QString str = mContentEdit->text();

    if (str.length() >= KMaxCodeLength)
        mdialog->actions().at(0)->setEnabled(true);
    else
        mdialog->actions().at(0)->setEnabled(false);
    }
