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

#ifndef POLICYENGUINOTIFIERPLUGIN_H_
#define POLICYENGUINOTIFIERPLUGIN_H_

#include <hbdevicedialogplugin.h>


class PolicyEngUINotifierplugin : public HbDeviceDialogPlugin
    {

public:
    PolicyEngUINotifierplugin();
    ~PolicyEngUINotifierplugin();

    //interface between client and plugin
    HbDeviceDialogInterface *createDeviceDialog(
            const QString &deviceDialogType, const QVariantMap &parameters);

    bool accessAllowed(const QString &deviceDialogType,
            const QVariantMap &parameters,
            const QVariantMap &securityInfo) const;

    bool deviceDialogInfo(const QString &deviceDialogType,
            const QVariantMap &parameters, DeviceDialogInfo *info) const;
    QStringList deviceDialogTypes() const;
    PluginFlags pluginFlags() const;
    int error() const;
    };
#endif /* POLICYENGUINOTIFIERPLUGIN_H_ */
