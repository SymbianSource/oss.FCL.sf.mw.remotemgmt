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
 * Description:   	PS keys for fotaserver
 *
 */

#ifndef FOTASERVER_PRIVATE_PSKEYS_H
#define FOTASERVER_PRIVATE_PSKEYS_H

// CONSTANTS

// Reading the current or ongoing NSML Job Id from DM UI
const TUint32 KNsmlCurrentJobId = 0x00000001;

// Reading the Server Alert UI Mode Key
const TUint32 KNsmlSanUIMode = 0x00000004;

// Reading the OMA DM large object download status
const TUint32 KFotaLrgObjDl = 0x00000005;

// Reading the Server profile id used for OMA DM large object download
const TUint32 KFotaLrgObjProfileId = 0x00000006;

//PubSub key used to mark which application is handling the process of download
//Sets by DM UI, NSC or NSCBackground
const TUint32 KNSmlCurrentFotaUpdateAppName = 0x0000000A;

//PubSub key used to define should "Install Now?" query be shown or not
//Sets by DM UI or NSC
const TUint32 KDevManShowInstallNowQuery = 0x0000000B;

//Reading the DM Job status/error used for OMA DM large object download
const TUint32 KDmJobCancel = 0x00000012;

#endif // FOTASERVER_PRIVATE_PSKEYS_H
// End of File
