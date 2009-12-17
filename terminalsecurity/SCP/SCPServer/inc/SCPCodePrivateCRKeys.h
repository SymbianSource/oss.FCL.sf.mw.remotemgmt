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



#ifndef __SCP_Code_PrivateKeys__
#define __SCP_Code_PrivateKeys__

//  INCLUDES

#include <e32std.h>

// =============================================================================
// SCP Lock code Parameters 
// =============================================================================

const TUid KCRUidSCPParameters = { 0x2001FCBD }; 
	

/**
 *
 * The value assigned to this key is used to set the Default input mode of the lock code query dialog
 * Possible values: 0 or 1
 * Default value: 0
*/
const TUint32 KSCPLockCodeDefaultInputMode = 0x00000001;


#endif// End of File



