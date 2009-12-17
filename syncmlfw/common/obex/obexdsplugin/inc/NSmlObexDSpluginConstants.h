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
* Description:  SyncML Obex plugin for data syncronization
*
*/


#ifndef __NSMLOBEXDSPLUGINCONSTANTS__
#define __NSMLOBEXDSPLUGINCONSTANTS__

#include <e32def.h>
#include <bt_sock.h>

static const TUint KMIMELen = 128;
static const TInt KBufferSize = 1024;

_LIT8( KDataSyncMIME, "application/vnd.syncml+wbxml" );
_LIT8( KDataSyncDSAlertNotification, "application/vnd.syncml.ds.notification" );
_LIT8( KDataSyncDM, "application/vnd.syncml.dm+wbxml" );
_LIT8( KTargetSyncDS, "SYNCML-SYNC" );
_LIT8( KTargetSyncDM, "SYNCML-DM" );


#endif //__NSMLOBEXDSPLUGINCONSTANTS__