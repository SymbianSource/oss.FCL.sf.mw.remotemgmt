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


#ifndef DIAGRESULTSDBPRIVATECRKEYS_H
#define DIAGRESULTSDBPRIVATECRKEYS_H

//  INCLUDES

#include <e32std.h>

// =============================================================================
// Diagnostics Results DB
// =============================================================================

const TUid KCRUidDiagnosticsResults = { 0x2000B45D };

/**
* The maximum number of test records inside each DB file. 
**/
const TUint32 KDiagDatabaseMaxRecordCount = 0x00000001;

/**
* The maximum file size for DB file in bytes. 
**/
const TUint32 KDiagDatabaseMaxFileSize = 0x00000002;

/**
* The value indicates which deletion algorithm the DB uses. The First alternative 
* deletes test records when max record count is exceeded (0). The other uses a buffer 
* to store last results (1) and keeps the max record count.
**/
const TUint32 KDiagDatabaseDeletionAlgorithm = 0x00000003;


#endif      // DIAGRESULTSDBPRIVATECRKEYS_H

// End of File

