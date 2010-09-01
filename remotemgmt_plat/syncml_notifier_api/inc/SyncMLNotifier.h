/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Partial declaration of the notifier API.
*
*/



#ifndef SYNCMLNOTIFIER_H
#define SYNCMLNOTIFIER_H

//  INCLUDES
#include <eiknotapi.h>

// CONSTANTS
// Notifier id for app starter notifier
const TUid KSyncMLAppLaunchNotifierUid = {0x101F8769};

// Notifier id for server dialog notifier
const TUid KSyncMLDlgNotifierUid =       {0x101F876A};
const TUid KSyncMLFwUpdNotifierUid =     {0x102072BF}; // Notifier id for firmware update notifier

// FUNCTION PROTOTYPES
// Notifier array (entry point)
IMPORT_C CArrayPtr<MEikSrvNotifierBase2>* NotifierArray();

#endif      // SYNCMLNOTIFIER_H   
            
// End of File
