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

#include <QtPlugin>
#include "PolicyEngUINotifierplugin.h"
#include "PolicyEngineUI.h"

Q_EXPORT_PLUGIN2(PolicyEngUINotifiersplugin, PolicyEngUINotifierplugin)

static const struct
    {
    const char *mTypeString;
    } dialogInfos[] =
    {
        {
        "com.nokia.hb.policymanagementdialog/1.0"
        }
    };

    
// -----------------------------------------------------------------------------
// PolicyEngUINotifierplugin::PolicyEngUINotifierplugin()
// -----------------------------------------------------------------------------    
PolicyEngUINotifierplugin::PolicyEngUINotifierplugin()
    {
    //policyEngUI = new QtPolicyEngineUI();
    }


// -----------------------------------------------------------------------------
// PolicyEngUINotifierplugin::~PolicyEngUINotifierplugin()
// -----------------------------------------------------------------------------    
PolicyEngUINotifierplugin::~PolicyEngUINotifierplugin()
    {
    //delete policyEngUI;
    }


// -----------------------------------------------------------------------------
// PolicyEngUINotifierplugin::createDeviceDialog()
// -----------------------------------------------------------------------------    
HbDeviceDialogInterface* PolicyEngUINotifierplugin::createDeviceDialog(
        const QString &deviceDialogType, const QVariantMap &parameters)
    {
	Q_UNUSED(deviceDialogType);
    // policyEngUI->DisplayNotificationDialog(parameters);
    return new PolicyEngineUI(parameters);
    }


// -----------------------------------------------------------------------------
// PolicyEngUINotifierplugin::accessAllowed()
// Check if client is allowed to use device dialog widget
// -----------------------------------------------------------------------------    
bool PolicyEngUINotifierplugin::accessAllowed(
        const QString &deviceDialogType, const QVariantMap &parameters,
        const QVariantMap &securityInfo) const
    {
	Q_UNUSED(deviceDialogType);
	Q_UNUSED(parameters);
	Q_UNUSED(securityInfo);
    // This plugin doesn't perform operations that may compromise security.
    // All clients are allowed to use.
    return true;
    }


// -----------------------------------------------------------------------------
// PolicyEngUINotifierplugin::deviceDialogInfo()
// -----------------------------------------------------------------------------    
bool PolicyEngUINotifierplugin::deviceDialogInfo(
        const QString &deviceDialogType, const QVariantMap &parameters,
        DeviceDialogInfo *info) const
    {
	Q_UNUSED(deviceDialogType);
	Q_UNUSED(parameters);
    info->group = GenericDeviceDialogGroup;
    info->flags = NoDeviceDialogFlags;
    info->priority = DefaultPriority;
    return true;
    }


// -----------------------------------------------------------------------------
// PolicyEngUINotifierplugin::deviceDialogTypes()
// Return device dialog types this plugin implements
// -----------------------------------------------------------------------------    
QStringList PolicyEngUINotifierplugin::deviceDialogTypes() const
    {
    QStringList types;
    const int numTypes = sizeof(dialogInfos) / sizeof(dialogInfos[0]);
    for (int i = 0; i < numTypes; i++)
        {
        types.append(dialogInfos[i].mTypeString);
        }

    return types;
    }


// -----------------------------------------------------------------------------
// PolicyEngUINotifierplugin::pluginFlags()
// Return plugin flags
// -----------------------------------------------------------------------------    
HbDeviceDialogPlugin::PluginFlags PolicyEngUINotifierplugin::pluginFlags() const
    {
    return NoPluginFlags;
    }


// -----------------------------------------------------------------------------
// PolicyEngUINotifierplugin::error()
// Return last error
// -----------------------------------------------------------------------------    
int PolicyEngUINotifierplugin::error() const
    {
    return 0;
    }
