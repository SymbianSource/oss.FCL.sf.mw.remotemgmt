/*
* Copyright (c) 2000 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description: Implementation of terminalsecurity components
*
*/


// INCLUDE FILES
#include <e32base.h>
#include <e32std.h>
#include <ecom/implementationproxy.h>

#include "SCPHistoryPlugin.h"
#include "SCPSpecificStringsPlugin.h"



// ----------------------------------------------------------------------------
// ImplementationTable
// Ecom plug-in implementation table
// Status : Approved
// ----------------------------------------------------------------------------
//
const TImplementationProxy ImplementationTable[] =
     {
#ifdef __EABI__ 
	IMPLEMENTATION_PROXY_ENTRY( 0x10274265, CSCPHistoryPlugin::NewL ), // SCPHistoryPlugin
	IMPLEMENTATION_PROXY_ENTRY( 0x10274267, CSCPSpecificStringsPlugin::NewL ) // SCPSpecificStringsPlugin
	
#else // !__EABI__ 
     { { 0x10274265  }, CSCPHistoryPlugin::NewL },// SCPHistoryPlugin
     { { 0x10274267  }, CSCPSpecificStringsPlugin::NewL }// SCPSpecificStringsPlugin
#endif // __EABI__ 
     };

// ----------------------------------------------------------------------------
// ImplementationGroupProxy
// Instance of implementation proxy
// (exported)
// Status : Approved
// ----------------------------------------------------------------------------
//
EXPORT_C const TImplementationProxy* ImplementationGroupProxy( TInt& aTableCount )
    {
	aTableCount = sizeof( ImplementationTable ) / sizeof( TImplementationProxy );

	return ImplementationTable;
    }

// End of File
