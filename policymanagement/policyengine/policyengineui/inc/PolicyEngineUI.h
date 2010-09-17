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

#ifndef POLICYENGINEUI_H
#define POLICYENGINEUI_H

#include <hbdialog.h>
#include <hbdevicedialoginterface.h>

class HbDialog;
class HbLineEdit;

class PolicyEngineUI : public HbDialog, public HbDeviceDialogInterface
    {
Q_OBJECT

public:
    PolicyEngineUI(const QVariantMap& parameters);
    ~PolicyEngineUI();
    void DisplayNotificationDialog(const QVariantMap &parameters);
    void ShowInputDialog();
    bool verifyFingerPrint();

public slots:
    void onOKSelected();
    void onCancelSelected();
    void establishTrust();
    void cancelTrust();
    void onTrustCreation();
    void codeTextChanged();
public:
    //derived from HbDeviceDialogInterface
    bool setDeviceDialogParameters(const QVariantMap &parameters);
    int deviceDialogError() const;
    void closeDeviceDialog(bool byClient);
    HbDialog *deviceDialogWidget() const;

signals:
    //signal to be sent to client
    void deviceDialogClosed();
    void deviceDialogData(QVariantMap data);
  /**
	 * applicationReady() - To emit the application ready signal for matti tool.
   */
    void applicationReady();
private:
    // number of tries user does
    int estbTrustCount;
    QString iServerName;
    QString iFingerprint;
    QString iCode;
    HbLineEdit *mContentEdit;
    HbDialog* mdialog;
    };

#endif // POLICYENGINEUI_H
