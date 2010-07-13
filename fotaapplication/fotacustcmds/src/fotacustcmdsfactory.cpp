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
* Description:
* Implementation of FotaCustCmdsFactory class.
*
*/

#include "fotacustcmdsfactory.h"
#include "fotacustcmdallreasons.h"
#include "fotacustcmdfirmwareupdate.h"
#include "fotacustcmdfirstboot.h"
#include "fotastartupDebug.h"
//#include "trace.h"

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// FotaCustCmdsFactory::FotaCustCmdAllReasonsNewL
// ---------------------------------------------------------------------------
//
EXPORT_C MSsmCustomCommand* FotaCustCmdsFactory::FotaCustCmdAllReasonsNewL()
    {
   FLOG( _L( "[FotaCustCmdsFactory::FotaCustCmdAllReasonsNewL() " ) );

    return CFotaCustCmdAllReasons::NewL();
    }


// ---------------------------------------------------------------------------
// FotaCustCmdsFactory::FotaCustCmdFirmwareUpdateNewL
// ---------------------------------------------------------------------------
//
EXPORT_C MSsmCustomCommand* FotaCustCmdsFactory::FotaCustCmdFirmwareUpdateNewL()
    {
    FLOG( _L( "[FotaCustCmdsFactory::FotaCustCmdFirmwareUpdateNewL() " ) );
    return CFotaCustCmdFirmwareUpdate::NewL();
    }


// ---------------------------------------------------------------------------
// FotaCustCmdsFactory::FotaCustCmdFirstBootNewL
// ---------------------------------------------------------------------------
//
EXPORT_C MSsmCustomCommand* FotaCustCmdsFactory::FotaCustCmdFirstBootNewL()
    {
   
    FLOG( _L( "[FotaCustCmdsFactory::FotaCustCmdFirstBootNewL() " ) );
    return CFotaCustCmdFirstBoot::NewL();
    }
