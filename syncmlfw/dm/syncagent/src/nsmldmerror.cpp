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
* Description:  error code conversion
*
*/




#include <in_iface.h>
#include "nsmldmerror.h"


// ---------------------------------------------------------
// TNSmlDMError::TNSmlDMError()
// Constructor, nothing special in here.
// ---------------------------------------------------------
//
TNSmlDMError::TNSmlDMError():TNSmlError()
	{
	}

// ---------------------------------------------------------
// TNSmlDMError::SyncLogMessage()
// Converts native error code to message code for UI
// 
// ---------------------------------------------------------	
TInt TNSmlDMError::SyncLogMessageCode()
	{
	TInt syncLogMessage;
	if ( iErrorCode == ESmlServerSessionAbort )
		{
		syncLogMessage = ESessionAborted;
		}
	else
		{
		TNSmlError::TNSmlCommonAgentMessage commonMessage = (TNSmlCommonAgentMessage) TNSmlError::SyncLogMessageCode();
		if ( commonMessage == TNSmlError::TNSmlCommonAgentMessage( TNSmlError::EServerSystemError ) )
			{
			syncLogMessage = EServerSystemError;
			}
		else
		if ( commonMessage == TNSmlError::TNSmlCommonAgentMessage( TNSmlError::ESyncMLError ) )
			{
			syncLogMessage = ESyncMLError;
			}
		else
		if ( commonMessage == TNSmlError::TNSmlCommonAgentMessage( TNSmlError::ESyncMLCommandNotSupported ) )
			{
			syncLogMessage = ESyncMLCommandNotSupported;
			}
		else
		if ( commonMessage == TNSmlError::TNSmlCommonAgentMessage( TNSmlError::ESyncMLVersionNotSupported ) )
			{
			syncLogMessage = ESyncMLVersionNotSupported;
			}
		else
		if ( commonMessage == TNSmlError::TNSmlCommonAgentMessage( TNSmlError::EUnsupportedContentTypeOrFormat ) )
			{
			syncLogMessage = EUnsupportedContentTypeOrFormat;
			}
		else
		if ( commonMessage == TNSmlError::TNSmlCommonAgentMessage( TNSmlError::ESyncMLClientAuthenticationError ) )
			{
			syncLogMessage = ESyncMLClientAuthenticationError;
			}
		else
		if ( commonMessage == TNSmlError::TNSmlCommonAgentMessage( TNSmlError::ESyncMLServerAuthenticationError ) )
			{
			syncLogMessage = ESyncMLServerAuthenticationError;
			}
		else
		if ( commonMessage == TNSmlError::TNSmlCommonAgentMessage( TNSmlError::EServerBusy ) )
			{
			syncLogMessage = EServerBusy;
			}
		else
		if ( commonMessage == TNSmlError::TNSmlCommonAgentMessage( TNSmlError::EServerNotResponding ) )
			{
			syncLogMessage = EServerNotResponding;
			}
		else
		if ( commonMessage == TNSmlError::TNSmlCommonAgentMessage( TNSmlError::EInvalidURI ) )
			{
			syncLogMessage = EInvalidURI;
			}
		else
		if ( commonMessage == TNSmlError::TNSmlCommonAgentMessage( TNSmlError::ECommunicationError ) )
			{
			syncLogMessage = ECommunicationError;
			}
         // 1.2 CHANGES: Offline mode
        else
		if ( commonMessage == TNSmlError::TNSmlCommonAgentMessage( TNSmlError::ENotPossibleInOfflineMode ) )
			{
			syncLogMessage = ENotPossibleInOfflineMode;
			}
        // Changes end
		else
			{
			syncLogMessage = ESystemError;
			}
		}		
	return syncLogMessage;
	}

// ---------------------------------------------------------
// TNSmlDMError::ErrorCodeConversion()
// Converts some error code to Symbian error codes 
// ---------------------------------------------------------	
void TNSmlDMError::ErrorCodeConversion()
	{
	TNSmlError::ErrorCodeConversion();
	}

