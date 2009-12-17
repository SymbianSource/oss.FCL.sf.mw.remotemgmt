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



#ifndef __NSMLERROR_H
#define __NSMLERROR_H

// INCLUDES
#include <e32base.h>
#include <nsmldefines.h>

// FORWARD DECLARATIONS
class CNSmlFutureReservation;

//  INCLUDES
class TNSmlError
	{
	// Public members
	public:  // enumerations
	//
    enum TNSmlAgentErrorCode
		{
		ESmlServerNotResponding = 9,
		ESmlCommandInvalid = 1000,
		ESmlSyncHdrMissing = 1001,
		ESmlVerDTDMissing = 1002,
		ESmlVerDTDNotSupported = 1003,
		ESmlVerProtoMissing = 1004,
		ESmlVerProtoNotSupported = 1005,
		ESmlMsgIDMissing = 1006,
		ESmlMsgIDInvalid = 1007,
		ESmlTargetLocURIMissing = 1008,
		ESmlTargetLocURIInvalid = 1009,
		ESmlSourceLocURIMissing = 1010,
		ESmlSourceLocURIInvalid = 1011,
		ESmlUnexpectedCmd = 1012,
		ESmlUnsupportedCmd = 1013,
		ESmlStatusMissing = 1101,
		ESmlNoMatchingStatusElement = 1102,
		ESmlMsgRefMissing = 1103,
		ESmlMsgRefInvalid = 1104,
		ESmlCmdRefMissing = 1105,
		ESmlCmdRefInvalid = 1106,
		ESmlCmdMissing = 1107,
		ESmlCmdInvalid = 1108,
		ESmlSourceRefInvalid = 1109,
		ESmlStatusDataMissing = 1110,
		ESmlStatusDataInvalid = 1111,
		ESmlCmdIDMissing = 1112,
		ESmlCmdIDInvalid = 1113,
		ESmlSyncTargetLocURIMissing = 1114,
		ESmlSyncTargetLocURIInvalid = 1115,
		ESmlSyncSourceLocURIMissing = 1116,
		ESmlSyncSourceLocURIInvalid = 1117,
		ESmlFormatNotSupported = 1118,
		ESmlFormatInvalid = 1119,
		ESmlTypeNotSupported = 1120,
		ESmlTypeInvalid = 1121,
		ESmlItemSourceLocURIMissing = 1122,
		ESmlItemDataMissing = 1123,
		ESmlItemTargetLocURIMissing = 1126,
		ESmlResultsInvalid = 1400,
		ESmlResultsMissing = 1401,
		ESmlResultsItemSourceMissing = 1402,
		ESmlResultsItemSourceInvalid = 1403,
		ESmlAlertMissingInInitialisation = 1404,
		ESmlAlertMissingInNextAlerting = 1405,
		ESmlAlertInvalid = 1406,
		ESmlAlertCodeMissing = 1407,
		ESmlAlertCodeInvalid = 1408,
		ESmlAlertTargetLocURIMissing = 1410,
		ESmlAlertTargetLocURIInvalid = 1411,
		ESmlAlertSourceLocURIMissing = 1412,
		ESmlAlertSourceLocURIInvalid = 1413,
		ESmlPutInvalid = 1414,
		ESmlPutItemSourceMissing = 1415,
		ESmlPutItemSourceInvalid = 1416,
		ESmlGetInvalid = 1417,
		ESmlGetItemTargetMissing = 1418,
		ESmlGetItemTargetInvalid = 1419,
		ESmlChalMissing = 1420,
		ESmlChalInvalid = 1421,
		ESmlMaxMsgSizeInvalid = 1422,
		ESmlServerIsBusy = 1500,
		ESmlErrorInSettings = 1600,
		ESmlTooLongMessage = 1602,
		ESmlLowMemory = 1603,
		ESmlIncompleteMessage = 1604,
		ESmlServerUnauthorized = 1700,
		ESmlCommunicationInterrupted = 8193,
		ESmlCommunicationError = 0x5011,
		ESmlUntrustedCert = 0x6011 
		};
		// SyncML Status Codes
	enum TNSmlSyncMLStatusCode
		{
		ESmlStatusInProgress = 101,
		ESmlStatusOK = 200,
		ESmlStatusItemAdded = 201,
		ESmlStatusAcceptedForProcessing = 202,
		ESmlStatusNonAuthoriveResponse = 203,
		ESmlStatusNoContent = 204,
		ESmlStatusResetContent = 205,
		ESmlStatusPartialContent = 206,
		ESmlStatusConflictResolvedWithMerge = 207,
		ESmlStatusConflictResolvedWithClientsCommand = 208,
		ESmlStatusConflictResolvedWithDuplicate = 209,
		ESmlStatusDeleteWithoutArchive = 210,
		ESmlStatusItemIsNotDeleted = 211,
		ESmlStatusAuthenticationAccepted = 212,
		ESmlStatusItemAccepted = 213,
		ESmlStatusOperationCancelled = 214,
		ESmlStatusNotExecuted = 215,
		ESmlStatusRollBackOK = 216,
        ESmlStatusNotPossibleInOfflineMode = 217, // 1.2 CHANGES: Offline mode
		ESmlStatusMultipleChoices = 300,
		ESmlStatusMovedPermanently = 301,
		ESmlStatusMovedTemporarily = 302,
		ESmlStatusSeeOtherURI = 303,
		ESmlStatusNotModified = 304,
		ESmlStatusUseProxy = 305,
		ESmlStatusBadRequest = 400,
		ESmlStatusUnauthorized = 401,
		ESmlStatusPaymentRequired = 402,
		ESmlStatusForbidden = 403,
		ESmlStatusNotFound = 404,
		ESmlStatusCommandNotAllowed = 405,
		ESmlStatusOptFeatureNotSupported = 406,
		ESmlStatusClientAuthenticationRequired = 407,
		ESmlStatusRequestTimeout = 408,
		ESmlStatusConflict = 409,
		ESmlStatusGone = 410,
		ESmlStatusSizeRequired = 411,
		ESmlStatusIncompleteCommand = 412,
		ESmlStatusEntityTooLarge = 413,
		ESmlStatusURITooLong = 414,
		ESmlStatusUnsupportedMediaTypeOrFormat = 415,
		ESmlStatusRequestedSizeTooBig = 416,
		ESmlStatusRetryLater = 417,
		ESmlStatusAlreadyExists = 418,
		ESmlStatusConflictResolvedWithServerData = 419,
		ESmlStatusDeviceFull = 420,
		ESmlStatusUnknownSearchGrammar = 421,
		ESmlStatusBadCGIScript = 422,
		ESmlStatusSoftDeleteUpdateConflict = 423,
		ESmlStatusSizeMismatch = 424,
		ESmlStatusMoveFailed = 428,
		ESmlStatusCommandFailed = 500,
		ESmlStatusCommandNotImplemented = 501,
		ESmlStatusBadGateway = 502,
		ESmlStatusServiceUnavailable = 503,
		ESmlStatusGatewayTimeout = 504,
		ESmlStatusVersionNotSupported = 505,
		ESmlStatusProcessingError = 506,
		ESmlStatusAtomicFailed = 507,
		ESmlStatusRefreshRequired = 508,
		ESmlStatusDataStoreFailure = 510,
		ESmlStatusServerFailure = 511,
		ESmlStatusSynchronisationFailed = 512,
		ESmlStatusProtocolVersionNotSupported = 513,
		ESmlStatusAtomicRollBackFailed = 516 
		};	
	protected:  //enumerations
	enum TNSmlCommonAgentMessage    
		{		
		ESystemError = 1,
		EServerSystemError,
		ESyncMLError,
		ESyncMLCommandNotSupported,
		ESyncMLVersionNotSupported,
		EUnsupportedContentTypeOrFormat,
		ESyncMLClientAuthenticationError,
		ESyncMLServerAuthenticationError,
		EServerBusy,
		EServerNotResponding,
		EInvalidURI,
		ECommunicationError,
		EHTTPAuthError,
        ENotPossibleInOfflineMode // 1.2 CHANGES: Offline mode
		};
	public:  //constructor	
	IMPORT_C TNSmlError();
	IMPORT_C virtual ~TNSmlError();
	
	public: //new functions
	IMPORT_C void SetErrorCode( TInt aErrorCode );
	// Sync Log Message code 
	IMPORT_C virtual TInt SyncLogMessageCode();
	IMPORT_C TInt SyncLogErrorCode();

	protected: //new functions
	IMPORT_C void virtual ErrorCodeConversion();
	// Private variables
	protected: // Data
	TInt iErrorCode;
    private: // Data
	// Reserved to maintain binary compability
	CNSmlFutureReservation* iReserved;
	};

#endif // __NSMLERROR_H

