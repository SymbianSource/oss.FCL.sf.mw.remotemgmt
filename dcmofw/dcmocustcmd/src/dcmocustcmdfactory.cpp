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
* Implementation of DCMOCmdsFactory class.
*
*/

#include "dcmocustcmdsfactory.h"
#include "dcmocustcmdallreasons.h"
#include "dcmostartupDebug.h"


// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// DCMOCustCmdsFactory::DCMOCustCmdAllReasonsNewL
// ---------------------------------------------------------------------------
//
EXPORT_C MSsmCustomCommand* DCMOCustCmdsFactory::DCMOCustCmdAllReasonsNewL()
    {
    	FLOG( _L( "[DCMOCustCmdsFactory::DCMOCustCmdAllReasonsNewL() " ) );
    	return CDCMOCustCmdAllReasons::NewL();
    }
