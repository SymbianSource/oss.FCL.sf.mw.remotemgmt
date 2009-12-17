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
* Description:  Table of plugins for ECom
*
*/



// INCLUDE FILES
#include <e32std.h>
#include <implementationproxy.h>
#include "ProvisioningUIDs.h"
#include "CWPWAPMMSAdapter.h"
#include "CWPWAPBrowserAdapter.h"

// CONSTANTS
const TImplementationProxy KImplementationTable[] = 
    {
	IMPLEMENTATION_PROXY_ENTRY( KProvisioningMMSAdapterUid, CWPWAPMMSAdapter::NewL )
//    {{KProvisioningMMSAdapterUid}, CWPWAPMMSAdapter::NewL},
	,
	IMPLEMENTATION_PROXY_ENTRY( KProvisioningBrowserAdapterUid, CWPWAPBrowserAdapter::NewL )
//    {{KProvisioningBrowserAdapterUid}, CWPWAPBrowserAdapter::NewL}
    };

// ========================== OTHER EXPORTED FUNCTIONS =========================

// -----------------------------------------------------------------------------
// ImplementationGroupProxy
// -----------------------------------------------------------------------------
//
EXPORT_C const TImplementationProxy* ImplementationGroupProxy(TInt& aTableCount)
    {
    aTableCount = sizeof(KImplementationTable) / sizeof(TImplementationProxy);

    return KImplementationTable;
    }

//  End of File  
