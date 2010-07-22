/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  fotaserver constants
*
*/


#ifndef __FOTACONST_H___
#define __FOTACONST_H___


// Definition of app server uid and service uid
#define KFotaServerUid	0x102072C4
#define KFotaServiceUid	0x102072C5
#define KDMHostServer1Uid	0x101F9A02
#define KSosServerUid   0x101f99fb
#define KOmaDMAppUid    0x101F6DE5
#define KStarterUid     0x2000D75B
#define KFotaScheduler	0x101F99FA
#define KFMSServerUid	0x200100C8
#define KFMSServiceUid	0x200100C7
#define KDLMgrServerUid 0x10008D60

// How many times generic alert sending should be tried. Checked on launch 
// of fotaserver
const TInt KDefaultSmlTryCount = 4;
const TInt KMaximumSmlTryCount = 1000;


const TInt KFotaMaxPkgURLLength     ( 2048 );
const TInt KFotaMaxPkgNameLength    ( 80 );
const TInt KFotaMaxPkgVersionLength ( 80 );


const TInt KBmpMargin(21);

_LIT8( KNSmlIAPId,			"NSmlIapId" ); 
_LIT( KFotaPanic, "FotaServer");
_LIT(KFotaServerName,"FotaServer");
_LIT( KFotaServerScem, "FotaServer_10247628");

enum TFotaClient
    {
    EUnknown = 0,
    EDMHostServer = 1,
    EOMADMAppUi = 2,
    EFotaScheduler  = 3,
    EStarter  = 4,
    EFMSServer = 5,
    EFotaTestApp = 6,
    ESoftwareChecker = 7,
    ESoftwareCheckerBackground = 8
    };

#endif