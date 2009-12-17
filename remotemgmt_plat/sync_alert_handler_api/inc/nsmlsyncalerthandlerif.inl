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
* Description:  Inline interface methods for plugin alert handlers
 *
*/


#ifndef NSMLSYNCALERTHANDLER_INL_
#define NSMLSYNCALERTHANDLER_INL_

#include <ecom.h>

// ------------------------------------------------------------------------------------------------
//
// ------------------------------------------------------------------------------------------------
inline CNSmlSyncAlertHandler* CNSmlSyncAlertHandler::NewL(
    const TUid& aImplementationUid)
	{
	TAny* any = REComSession::CreateImplementationL(
        aImplementationUid,
        _FOFF(CNSmlSyncAlertHandler, iDtor_ID_Key));
	CNSmlSyncAlertHandler* object = REINTERPRET_CAST(CNSmlSyncAlertHandler*, any);
	return object;
	}

// ------------------------------------------------------------------------------------------------
//
// ------------------------------------------------------------------------------------------------
inline CNSmlSyncAlertHandler::~CNSmlSyncAlertHandler()
	{
	REComSession::DestroyedImplementation(iDtor_ID_Key);
	}

#endif
