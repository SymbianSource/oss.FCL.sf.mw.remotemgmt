/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  SyncMLNotifier Domain CenRep key definitions
*
*/


#ifndef SYNCMLNOTIFIER_DOMAIN_CR_KEYS_H
#define SYNCMLNOTIFIER_DOMAIN_CR_KEYS_H

// CONSTANTS

// SyncMLNotifier key UID
const TUid KCRUidNSmlNotifierDomainKeys = {0x101F8769};

// CenRep key used in variation of the aspects of
// the roaming feature. The flag values are
// commented below.
const TUint32 KNSmlDMBlockedInRoaming = 0x00000001;


// CenRep key for variable DM UI UID
const TUint32 KNSmlDMAppName = 0x00000002;

// CenRep key for variable DM UI UID
const TUint32 KNSmlDMChargingNote = 0x00000003;

#endif // NSMLDMSYNCAPP_PRIVATE_CR_KEYS_H
            
// End of File
