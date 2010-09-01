/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Private Central Repository keys.
*
*/


#ifndef DIAGNOSTICSFWPRIVATECRKEYS_H
#define DIAGNOSTICSFWPRIVATECRKEYS_H

//  INCLUDES

#include <e32std.h>

// =============================================================================
// Diagnostics Framework
// =============================================================================

const TUid KCRUidDiagnosticsFw = { 0x2000B16B };

/**
* Configure number of microseconds Diagnostics Framework will delay before starting
* to execute each test.  This gives user a chance to cancel before a test 
* begins.
*/
const TUint32 KDiagFwTestInitDelay = 0x00000001;

/**
* Configure number of microseconds of test inactivity Diagnostics Framework will 
* tolerate before cancelling an interactive test. This prevents single tests
* from blocking entire testing session.
* The value will be used for interactive test steps.
*/
const TUint32 KDiagFwWatchdogTimeoutInteractive = 0x00000002;

/**
* Configure number of microseconds of test inactivity Diagnostics Framework will 
* tolerate before cancelling an automatic test. This prevents single tests
* from blocking entire testing session.
* The value will be used for non-interactive (automatic) test steps.
*/
const TUint32 KDiagFwWatchdogTimeoutAutomatic = 0x00000003;

#endif      // DIAGNOSTICSFWPRIVATECRKEYS_H

// End of File

