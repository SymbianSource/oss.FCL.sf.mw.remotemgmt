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
* Description:  Conversion for private error codes to public syncML errors.
*
*/



// INCLUDE FILES
#include    "NSmlErrorCodeConversion.h"
#include	"nsmlerror.h"
#include	"nsmldserror.h"
#include	"nsmldmerror.h"
#include	"nsmlhttp.h"
#include	<SyncMLErr.h>

using namespace SyncMLError;

// CONSTANTS
const TNSmlErrorConversion::TNSmlErrorMapping TNSmlErrorConversion::KErrors[] = 
	{
		/* Transport related */
		{ TNSmlHTTPErrCode::ENSmlHTTPErr_BadRequest, 				KErrTransportRequest },
		{ TNSmlHTTPErrCode::ENSmlHTTPErr_MethodNotAllowed, 			KErrTransportRequest },
		{ TNSmlHTTPErrCode::ENSmlHTTPErr_Forbidden, 				KErrTransportRequest },
		{ TNSmlHTTPErrCode::ENSmlHTTPErr_Conflict, 					KErrTransportRequest },
		{ TNSmlHTTPErrCode::ENSmlHTTPErr_Gone, 						KErrTransportRequest },
		{ TNSmlHTTPErrCode::ENSmlHTTPErr_RequestEntityTooLarge, 	KErrTransportRequest },
		{ TNSmlHTTPErrCode::ENSmlHTTPErr_RequestURITooLong, 		KErrTransportRequest },
		{ TNSmlHTTPErrCode::ENSmlHTTPErr_UnsupportedMediaType, 		KErrTransportRequest },
		{ TNSmlHTTPErrCode::ENSmlHTTPErr_RequestedRangeNotSatisfiable, 	KErrTransportRequest },
		{ TNSmlHTTPErrCode::ENSmlHTTPErr_ExpectationFailed, 		KErrTransportRequest },
		{ TNSmlHTTPErrCode::ENSmlHTTPErr_NotImplemented, 			KErrTransportRequest },
		{ TNSmlHTTPErrCode::ENSmlHTTPErr_ServiceUnavailable, 		KErrTransportRequest },
		{ TNSmlHTTPErrCode::ENSmlHTTPErr_HTTPVersionNotSupported, 	KErrTransportRequest },
		{ TNSmlHTTPErrCode::ENSmlHTTPErr_NotAcceptable, 			KErrTransportHeader },
		{ TNSmlHTTPErrCode::ENSmlHTTPErr_LengthRequired, 			KErrTransportHeader },
		{ TNSmlHTTPErrCode::ENSmlHTTPErr_PreconditionFailed,  		KErrTransportHeader },
		{ TNSmlHTTPErrCode::ENSmlHTTPErr_NotFound, 					KErrTransportSvrNoUri },
		{ TNSmlHTTPErrCode::ENSmlHTTPErr_Unauthorized, 				KErrTransportAuthenticationFailure },
		{ TNSmlHTTPErrCode::ENSmlHTTPErr_ProxyAuthenticationRequired, 	KErrTransportAuthenticationFailure },
		{ TNSmlHTTPErrCode::ENSmlHTTPErr_BadGateway, 				KErrTransportDisconnected },
		{ TNSmlHTTPErrCode::ENSmlHTTPErr_PaymentRequired, 			KErrTransportDisconnected },
		{ TNSmlHTTPErrCode::ENSmlHTTPErr_RequestTimeout, 			KErrTransportDisconnected },
		{ TNSmlHTTPErrCode::ENSmlHTTPErr_InternalServerError, 		KErrTransportDisconnected },
		{ TNSmlHTTPErrCode::ENSmlHTTPErr_GatewayTimeout, 			KErrTransportDisconnected },
		/* Engine related*/
		{ TNSmlError::ESmlServerNotResponding,		KErrConnectionError },
		{ TNSmlError::ESmlCommunicationError, 		KErrCannotCommunicateWithServer },
		{ TNSmlError::ESmlServerUnauthorized, 		KErrInvalidCredentials },
		{ TNSmlError::ESmlStatusUnauthorized, 		KErrAuthenticationFailure },
		{ TNSmlError::ESmlStatusClientAuthenticationRequired, 		KErrAuthenticationFailure },
		{ TNSmlError::ESmlChalMissing, 				KErrCannotCommunicateWithServer },
		{ TNSmlError::ESmlChalInvalid, 				KErrCannotCommunicateWithServer },
		{ TNSmlError::ESmlVerDTDMissing, 			KErrVerDTDMissing },
		{ TNSmlError::ESmlVerProtoMissing, 			KErrProtoMissing },
		{ TNSmlError::ESmlSyncHdrMissing, 			KErrSyncHdrMissing },
		{ TNSmlError::ESmlTargetLocURIMissing, 		KErrTargetLocMissing },
		{ TNSmlError::ESmlTargetLocURIInvalid, 		KErrTargetLocMissing },
		{ TNSmlError::ESmlSyncTargetLocURIMissing, 	KErrTargetLocMissing },
		{ TNSmlError::ESmlSyncTargetLocURIInvalid, 	KErrTargetLocMissing },
		{ TNSmlError::ESmlSourceLocURIMissing, 		KErrSourceLocMissing },
		{ TNSmlError::ESmlSourceLocURIInvalid, 		KErrSourceLocMissing },
		{ TNSmlError::ESmlSyncSourceLocURIMissing, 	KErrSourceLocMissing },
		{ TNSmlError::ESmlSyncSourceLocURIInvalid, 	KErrSourceLocMissing },
		{ TNSmlError::ESmlVerDTDNotSupported, 		KErrVersionDTDNotSupported },
		{ TNSmlError::ESmlVerProtoNotSupported, 	KErrProtoNotSupported },
		{ TNSmlError::ESmlCmdInvalid, 				KErrStatusCommandNotTheSame },
		{ TNSmlError::ESmlStatusDataInvalid, 		KErrStatusCommandNotTheSame },
		{ TNSmlError::ESmlCmdIDInvalid, 			KErrStatusCommandNotTheSame },
		{ TNSmlError::ESmlCmdMissing, 				KErrMissingStatusCommands },
		{ TNSmlError::ESmlStatusDataMissing, 		KErrMissingStatusCommands },
		{ TNSmlError::ESmlCmdIDMissing, 			KErrMissingStatusCommands },
		{ TNSmlError::ESmlIncompleteMessage, 		KErrXmlParsingError },
		{ TNSmlDSError::ESmlDSDeviceInfoMissing,	KErrMissingStatusCommands },
		{ TNSmlDSError::ESmlDSDeviceInfoInvalid, 	KErrMissingStatusCommands },
		{ TNSmlDSError::ESmlDSLocalDatabaseError, 	KErrClientDatabaseNotFound },
		{ TNSmlError::ESmlStatusNotFound, 			KErrServerDatabaseNotFound },
		{ TNSmlError::ESmlStatusGone, 				KErrServerDatabaseNotFound },
		{ TNSmlError::ESmlMaxMsgSizeInvalid, 		KErrServerDatabaseNotFound },
		{ TNSmlError::ESmlCommandInvalid, 			KErrCommandInvalid },
		{ TNSmlError::ESmlUnexpectedCmd, 			KErrCommandInvalid },
		{ TNSmlError::ESmlUnsupportedCmd, 			KErrCommandInvalid },
		{ TNSmlError::ESmlStatusCommandNotImplemented,	KErrCommandInvalid },
		{ TNSmlError::ESmlMsgIDMissing, 			KErrCommandInvalid },
		{ TNSmlError::ESmlTargetLocURIMissing, 		KErrCommandInvalid },
		{ TNSmlError::ESmlTargetLocURIInvalid, 		KErrCommandInvalid },
		{ TNSmlDSError::ESmlDSDeviceInfoMissing, 	KErrCommandInvalid },
		{ TNSmlDSError::ESmlDSDatastoreSourceRefInvalid,	KErrCommandInvalid },
		{ TNSmlError::ESmlMsgIDInvalid, 			KErrCommandInvalid },
		{ TNSmlError::ESmlSourceRefInvalid, 		KErrCommandInvalid },
		{ TNSmlError::ESmlItemSourceLocURIMissing, 	KErrCommandInvalid },
		{ TNSmlError::ESmlItemDataMissing, 			KErrCommandInvalid },
		{ TNSmlError::ESmlItemTargetLocURIMissing, 	KErrCommandInvalid },
		{ TNSmlError::ESmlErrorInSettings, 			KErrCommandInvalid },
		{ TNSmlError::ESmlLowMemory, 				KErrCommandInvalid },
		{ TNSmlError::ESmlServerIsBusy, 			KErrCommandInvalid },
		{ TNSmlError::ESmlStatusNotPossibleInOfflineMode,	KErrPhoneOnOfflineMode },
		{ TNSmlDMError::ESmlServerSessionAbort,		KErrServerDatabaseNotFound },
		{ TNSmlDSError::ESmlDSServerIdNotMatching,  KErrServerIdNotMatching },
		{ TNSmlDSError::ESmlDSNoValidDatabases,		KErrClientDatabaseNotFound },
		{ TNSmlDSError::ESmlDSDatastoreSourceRefMissing,	KErrCommandInvalid },
		{ TNSmlDSError::ESmlDSDatastoreRxPrefCTTypeMissing,	KErrCommandInvalid },
		{ TNSmlDSError::ESmlDSDatastoreRxCTTypeNotMatching,	KErrCommandInvalid },
		{ TNSmlDSError::ESmlDSDatastoreTxPrefCTTypeMissing,	KErrCommandInvalid },
		{ TNSmlDSError::ESmlDSDatastoreTxCTTypeNotMatching,	KErrCommandInvalid },
		{ TNSmlDSError::ESmlDSLocalDatabaseChanged,	KErrCommandInvalid },
		{ TNSmlDSError::ESmlDSUnsupportedSyncType,	KErrCommandInvalid },
		{ TNSmlError::ESmlCommunicationInterrupted,	KErrCannotCommunicateWithServer },				
		{ TNSmlError::ESmlUntrustedCert,	KErrInvalidCert },
		{ KErrNone, 								KErrNone }
	};
	
const TNSmlErrorConversion::TNSmlErrorMappingRange TNSmlErrorConversion::KErrorRanges[] =
	{
		{ TNSmlError::ESmlResultsInvalid, 		TNSmlError::ESmlGetItemTargetInvalid, 	KErrCommandInvalid },
		{ TNSmlError::ESmlStatusMissing, 		TNSmlError::ESmlCmdRefInvalid, 			KErrCommandInvalid },
		{ TNSmlError::ESmlFormatNotSupported, 	TNSmlError::ESmlTypeInvalid, 			KErrCommandInvalid },
		{ KErrNone, 							KErrNone, 								KErrNone }
	};

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// TNSmlErrorConversion::TNSmlErrorConversion
// C++ constructor 
// -----------------------------------------------------------------------------
//
EXPORT_C TNSmlErrorConversion::TNSmlErrorConversion( 
	TInt aErrorCode ) 
	: iErrorCode( aErrorCode )
    {
    }
   
// -----------------------------------------------------------------------------
// TNSmlErrorConversion::SetErrorCode
// sets error code
// -----------------------------------------------------------------------------
//
void TNSmlErrorConversion::SetErrorCode( TInt aErrorCode )
	{
	iErrorCode = aErrorCode;
	}

// -----------------------------------------------------------------------------
// TNSmlErrorConversion::Convert
// converts error code.
// -----------------------------------------------------------------------------
//
EXPORT_C TInt TNSmlErrorConversion::Convert() const
    {
    if ( iErrorCode == KErrNone )
    	{
    	return KErrNone;
    	}
    	
    if ( iErrorCode < 0 )
    	{
    	// no conversion for system errors.
    	return iErrorCode;
    	}
    	
    TInt error( KErrNone );
    
    for ( TInt i(0); ( error = KErrors[i].iErrorCode ) != KErrNone; i++ )
    	{
    	if ( error == iErrorCode )
    		{
    		return KErrors[i].iSyncMLError;
    		}
    	}
    	
    TInt errorRangeStart( KErrNone );
    	
	for ( TInt i(0); ( errorRangeStart = KErrorRanges[i].iErrorCodeRangeStart ) != KErrNone; i++ )
    	{
    	if ( iErrorCode >= errorRangeStart && iErrorCode <= KErrorRanges[i].iErrorCodeRangeEnd )
    		{
    		return KErrorRanges[i].iSyncMLError;
    		}
    	}
    	
    return SyncMLError::KErrGeneral;
    }

//  End of File
