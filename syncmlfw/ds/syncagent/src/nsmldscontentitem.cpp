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
* Description:  Database (content) specific fata encapsulation
*
*/


// INCLUDE FILES
#include "NsmlDSContentItem.h"
#include "NSmlURI.h"
#include "nsmlagentlog.h"
#include "nsmldsluidbuffer.h"
#include "nsmldsmapcontainer.h"

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CNSmlDSContentItem::CNSmlDSContentItem()
// C++ default constructor.
// -----------------------------------------------------------------------------
//
CNSmlDSContentItem::CNSmlDSContentItem()
    {
    iAtomicModeOn = EFalse;
    iBatchModeOn = EFalse;
    iStoreOpened = EFalse;
    }
	
// -----------------------------------------------------------------------------
// CNSmlDSContentItem::~CNSmlDSContentItem()
// Destructor.
// -----------------------------------------------------------------------------
//
CNSmlDSContentItem::~CNSmlDSContentItem()
    {
    iFilters.ResetAndDestroy();
    delete iStoreName;
    delete iLocalDatabase;
    delete iRemoteDatabase;
    delete iAgentLog;
    delete iSyncType; //RD_SUSPEND_RESUME
    delete iClientSyncType;//RD_SUSPEND_RESUME
    delete iLUIDBuffer;
    delete iMapContainer;
    delete iServerId;
    }
	
// End of File
