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
* Description:   Constants declaration for SCPLockCode.
*
*/

#ifndef __SCP_LockCode__
#define __SCP_LockCode__

//  INCLUDES

#include <e32std.h>

// =============================================================================
// SCP Lock code Parameters
// =============================================================================

const TUid KCRUidSCPLockCode = { 0x2002677B };


/**
 *
 * The value assigned to this key is used to get and reset the default lock code
 * Possible values: 0 or 12345 (configurable)
 * Default value: 12345
*/
const TUint32 KSCPLockCodeDefaultLockCode = 0x00000001;


#endif// End of File



