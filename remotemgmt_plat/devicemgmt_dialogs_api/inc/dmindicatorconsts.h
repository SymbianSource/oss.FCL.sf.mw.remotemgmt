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
* Description:  constants for the device indicator.
*
*/

#ifndef DMINDICATORCONSTS_H_
#define DMINDICATORCONSTS_H_

#include <qstring.h>

//Indicators strings
const QString KDMProgressIndicatorType =
        "com.nokia.devicemanagement.progressindicatorplugin/1.0";
const QString KDMNotificationIndicatorType =
        "com.nokia.devicemanagement.notificationindicatorplugin/1.0";
const QString KDMSettingsIndicatorType =
        "com.nokia.devicemanagement.settingsindicatorplugin/1.0";
const QString KScomoProgressIndicatorType =
        "com.nokia.scomo.progressindicatorplugin/1.0";
const QString KScomoNotificationIndicatorType =
        "com.nokia.scomo.notificationindicatorplugin/1.0";
        
       
const QString KTarmTrustManagementActive = "ManagementActive";
const QString KTarmTrustTerminalSecurity = "TerminalSecurity";


#endif /* DMINDICATORCONSTS_H_ */
