/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description: Implementation of DM Device Dialogs
* 	This is part of syncmlfw.
*/

#ifndef __DMDEVDIALOG_CONSTS_H__
#define __DMDEVDIALOG_CONSTS_H__

#include <e32std.h>
#include <e32base.h>

// server name
_LIT(KDMDEVDIALOGSERVER,"dmdevicedialog");

const TInt KDmDevDialogServerVersionMinor = 0;
const TInt KDmDevDialogServerVersionMajor = 1;
const TUint KDefaultMessageSlots=4;
const TUint KDmDevDialogUid = 0x2002FF68;

enum TNotifiertype
{
	EServerAlert,
	EPkgConnectNoteIsActive,
	EPkgZeroConnectNoteDismiss,
	EPkgZeroConnectNoteShowAgain,
	EShowDisplayAlert,
	EShowConfirmationalert,
	ECancelServerAlert
	
};
#endif
