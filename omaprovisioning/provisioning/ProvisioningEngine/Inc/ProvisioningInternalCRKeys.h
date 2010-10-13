/*
* Copyright (c) 2004 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Provisioning internal Central Repository keys
*
*/


#ifndef PROVISIONINGINTERNALCRKEYS_H
#define PROVISIONINGINTERNALCRKEYS_H

#include <e32def.h>

/******************************************************************************
* Provisioning Variation API
* Keys under this category are used in defining Provisioning variation.
*/
const TUid KCRUidOMAProvisioningLV = {0x101F87AA};
const TUid KOMAProvAuthenticationLV	= {0x101F87AA};

/**
* Provisioning Local Variation Flags. Values are defined in ProvisioningVariant.hrh.
*/
const TUint32 KOMAProvisioningLVFlag = 0x00000000;
const TUint32 KOMAProvAuthenticationLVFlag 	= 0x00000001;
const TUint32 KOMAProvAuthFailMsgHandling = 0x00000002;   
const TUint32 KOMAProvOriginatorContent =  0x00000003;  

#endif      // PROVISIONINGINTERNALCRKEYS_H

// End of file
