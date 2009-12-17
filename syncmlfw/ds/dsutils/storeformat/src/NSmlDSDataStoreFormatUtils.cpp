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
* Description:  DS Utils Data Store format
*
*/


// INCLUDE FILES
#include "NSmlDSDataStoreFormatUtils.h"

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CSmlDataStoreFormatUtils::SetSyncTypeMaskFromResource
// Sets sync type mask.
// -----------------------------------------------------------------------------
//
void TSmlDataStoreFormatUtils::SetSyncTypeMaskFromResource( TSmlSyncTypeMask& aSyncTypeMask, TUint16 aSyncTypeFromResource )
	{
	if ( KSmlSyncType_TwoWay & aSyncTypeFromResource )
		{
		aSyncTypeMask.SetSupported( ESmlTwoWay );
		}
	else
		{
		aSyncTypeMask.SetNotSupported( ESmlTwoWay );
		}
	
	if ( KSmlSyncType_OneWayFromSvr & aSyncTypeFromResource )
		{
		aSyncTypeMask.SetSupported( ESmlOneWayFromServer );
		}
	else
		{
		aSyncTypeMask.SetNotSupported( ESmlOneWayFromServer );
		}
	
	if ( KSmlSyncType_OneWayFromClnt & aSyncTypeFromResource ) 
		{
		aSyncTypeMask.SetSupported( ESmlOneWayFromClient );
		}
	else
		{
		aSyncTypeMask.SetNotSupported( ESmlOneWayFromClient );	
		}
	
	if ( KSmlSyncType_SlowSync & aSyncTypeFromResource ) 
		{
		aSyncTypeMask.SetSupported( ESmlSlowSync );
		}
	else
		{
		aSyncTypeMask.SetNotSupported( ESmlSlowSync );
		}
	
	if ( KSmlSyncType_RefreshFromSvr & aSyncTypeFromResource ) 
		{
		aSyncTypeMask.SetSupported( ESmlRefreshFromServer );	
		}
	else
		{
		aSyncTypeMask.SetNotSupported( ESmlRefreshFromServer );	
		}
	
	if ( KSmlSyncType_RefreshFromClnt & aSyncTypeFromResource ) 
		{
		aSyncTypeMask.SetSupported( ESmlRefreshFromClient );
		}
	else
		{
		aSyncTypeMask.SetNotSupported( ESmlRefreshFromClient );
		}
	}

// End of File
