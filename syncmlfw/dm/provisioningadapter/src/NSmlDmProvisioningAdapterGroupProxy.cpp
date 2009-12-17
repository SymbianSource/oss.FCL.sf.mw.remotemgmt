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
* Description:  DM Settings Provisioning Adapter
*
*/



// INCLUDE FILES
#include <e32std.h>
#include <implementationproxy.h>
#include "NSmlDmProvisioningAdapterUIDs.h"
#include "NSmlDmProvisioningAdapter.h"


#ifndef IMPLEMENTATION_PROXY_ENTRY
#define IMPLEMENTATION_PROXY_ENTRY(aUid, aFuncPtr)	{{aUid},(aFuncPtr)}
#endif

// CONSTANTS
const TImplementationProxy KImplementationTable[] = 
	{
//	{{KNSmlDmProvisioningAdapterDllUid3}, CNSmlDmProvisioningAdapter::NewL}
	IMPLEMENTATION_PROXY_ENTRY(KNSmlDmProvisioningAdapterImplUid, CNSmlDmProvisioningAdapter::NewL)
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
