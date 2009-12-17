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
* Description:  Error code conversion
*
*/



#include <in_iface.h>
#include "nsmlerror.h"
#include "nsmlhttp.h"


// ---------------------------------------------------------
// TNSmlError::TNSmlError()
// Constructor, nothing special in here.
// ---------------------------------------------------------
//
EXPORT_C TNSmlError::TNSmlError() :
    iErrorCode( KErrNone ),
    iReserved( NULL )
	{
	}

// ---------------------------------------------------------
// TNSmlError::~TNSmlError()
// Destructor.
// ---------------------------------------------------------
//
EXPORT_C TNSmlError::~TNSmlError()
	{
	}

// ---------------------------------------------------------
// TNSmlError::SetErrorCode( TInt aErrorCode )
// Sets error code.
// ---------------------------------------------------------
//	
EXPORT_C void TNSmlError::SetErrorCode( TInt aErrorCode )
	{
	iErrorCode = aErrorCode;
	ErrorCodeConversion();
	}

// ---------------------------------------------------------
// TNSmlError::SyncLogMessage()
// Converts native error code to message code for UI
// 
// ---------------------------------------------------------	
EXPORT_C TInt TNSmlError::SyncLogMessageCode()
	{
	TNSmlCommonAgentMessage syncLogMessage;
	if ( iErrorCode >= ESmlVerDTDMissing &&
		 iErrorCode <= ESmlVerProtoNotSupported )	
		{
		 syncLogMessage = ESyncMLVersionNotSupported;
		}
	else
	if ( iErrorCode == ESmlUnsupportedCmd )
		{
		syncLogMessage = ESyncMLCommandNotSupported;
		}
	else
	if ( iErrorCode >= ESmlFormatNotSupported &&
		 iErrorCode <= ESmlTypeInvalid )
		{
		syncLogMessage = EUnsupportedContentTypeOrFormat;
		}
	else
	if ( iErrorCode >= ESmlCommandInvalid &&
		 iErrorCode <= ESmlItemTargetLocURIMissing )
		{
		syncLogMessage = ESyncMLError;
		}
	else
	if ( iErrorCode >= ESmlResultsInvalid &&
		 iErrorCode <= ESmlMaxMsgSizeInvalid )
		{
		syncLogMessage = ESyncMLError;	
		}
	else
	if ( iErrorCode == ESmlServerIsBusy )
		{
		syncLogMessage = EServerBusy;
		}
	else
	if ( iErrorCode == ESmlTooLongMessage )
		{
		syncLogMessage = ESyncMLError;
		}
	else
	if ( iErrorCode == ESmlIncompleteMessage )
		{
		syncLogMessage = ESyncMLError;
		}
	else
	if ( iErrorCode == ESmlServerUnauthorized )
		{
		syncLogMessage = ESyncMLServerAuthenticationError;
		}
	else
	if ( iErrorCode == ESmlServerNotResponding )
		{
		syncLogMessage = EServerNotResponding;
		}
	else
	if ( iErrorCode == TNSmlHTTPErrCode::ENSmlHTTPErr_NotFound || iErrorCode == TNSmlHTTPErrCode::ENSmlHTTPErr_MethodNotAllowed )
		{
		syncLogMessage = EInvalidURI;
		}
	else
	if ( iErrorCode == TNSmlHTTPErrCode::ENSmlHTTPErrCodeBase + 401 )	
		{
		syncLogMessage = EHTTPAuthError;
		}
	else
	if ( ( iErrorCode >= TNSmlHTTPErrCode::ENSmlHTTPErrCodeBase + 400 &&
		 iErrorCode <= TNSmlHTTPErrCode::ENSmlHTTPErrCodeBase + 599 ) || ( iErrorCode == ESmlCommunicationError ) )	
		{
		syncLogMessage = ECommunicationError;
		}
	else
	if ( iErrorCode >= ESmlStatusMultipleChoices &&
		 iErrorCode <= ESmlStatusUseProxy )	
		{
		syncLogMessage = ESyncMLError;
		}
	else
	if ( iErrorCode == ESmlStatusBadRequest || iErrorCode == ESmlStatusPaymentRequired )
		{
		syncLogMessage = ESyncMLError;
		}
	else
	if ( iErrorCode == ESmlStatusUnauthorized )
		{
		syncLogMessage = ESyncMLClientAuthenticationError;
		}
	else
	if ( iErrorCode == ESmlStatusForbidden || iErrorCode == ESmlStatusNotFound 
		|| iErrorCode == ESmlStatusGone)	
		{
		syncLogMessage = ESyncMLError;
		}
	else
	if ( iErrorCode == ESmlStatusCommandNotAllowed )
		{
		syncLogMessage = ESyncMLError;
		}
	else
	if ( iErrorCode == ESmlStatusOptFeatureNotSupported || iErrorCode == ESmlStatusCommandNotImplemented )	
		{
		syncLogMessage = ESyncMLCommandNotSupported;
		}
	else
	if ( iErrorCode == ESmlStatusClientAuthenticationRequired )
		{
		syncLogMessage = ESyncMLClientAuthenticationError;
		}
	else
	if ( iErrorCode == ESmlStatusRequestTimeout || iErrorCode == ESmlStatusSizeRequired 
		|| iErrorCode == ESmlStatusIncompleteCommand || iErrorCode == ESmlStatusEntityTooLarge
		|| iErrorCode == ESmlStatusRequestedSizeTooBig || iErrorCode == ESmlStatusUnknownSearchGrammar
		|| iErrorCode == ESmlStatusBadCGIScript || iErrorCode == ESmlStatusBadGateway
		|| iErrorCode == ESmlStatusAtomicFailed )
		{
		syncLogMessage = ESyncMLError;
		}
	else
	if ( iErrorCode == ESmlStatusURITooLong )	
		{
		syncLogMessage = ESyncMLError;
		}
	else
	if ( iErrorCode == ESmlStatusUnsupportedMediaTypeOrFormat )
		{
		syncLogMessage = EUnsupportedContentTypeOrFormat;
		}
	else
	if ( iErrorCode == ESmlStatusRetryLater || iErrorCode == ESmlStatusDeviceFull 
		|| iErrorCode == ESmlStatusCommandFailed || iErrorCode == ESmlStatusServiceUnavailable 
		|| iErrorCode == ESmlStatusGatewayTimeout || iErrorCode == ESmlStatusProcessingError 
		|| iErrorCode == ESmlStatusDataStoreFailure || iErrorCode == ESmlStatusServerFailure 
		|| iErrorCode == ESmlStatusSynchronisationFailed )
		{
		syncLogMessage = EServerSystemError;
		}
	else
	if ( iErrorCode == ESmlStatusVersionNotSupported 
		|| iErrorCode == ESmlStatusProtocolVersionNotSupported )
		{
		syncLogMessage = ESyncMLVersionNotSupported;
		}
    // 1.2 CHANGES: Offline mode
    else
    if ( iErrorCode == ESmlStatusNotPossibleInOfflineMode )
        {
        syncLogMessage = ENotPossibleInOfflineMode;
        }
    // Changes end
	else	
		{
		syncLogMessage = ESystemError;
		}
	return syncLogMessage;
	}
// ---------------------------------------------------------
// TNSmlError::SyncLogErrorCode()
// 
// ---------------------------------------------------------	
EXPORT_C TInt TNSmlError::SyncLogErrorCode()
	{
	return iErrorCode;
	}
// ---------------------------------------------------------
// TNSmlError::ErrorCodeConversion()
// Converts some error code to Symbian error codes 
// ---------------------------------------------------------	
EXPORT_C void TNSmlError::ErrorCodeConversion()
	{
	if ( iErrorCode == ESmlLowMemory || iErrorCode == ESmlStatusDeviceFull )
		{
		iErrorCode = KErrDiskFull;
		}
	}

