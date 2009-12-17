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
* Description:  DM tree etc.
*
*/




#ifndef __NSMLDMTREECONSTANTS_H
#define __NSMLDMTREECONSTANTS_H

const TInt KNSmlDmNoStatus = 0;
const TInt KNSmlDmStatusOK = 200;
const TInt KNSmlDmStatusAcceptedForProcessing = 202;
const TInt KNSmlDmStatusNoContent = 204;
const TInt KNSmlDmStatusNotExecuted = 215;
const TInt KNSmlDmStatusAtomicRollBackOK = 216;
const TInt KNSmlDmStatusUseProxy = 305;
const TInt KNSmlDmStatusUnauthorized = 401;
const TInt KNSmlDmStatusForbidden = 403;
const TInt KNSmlDmStatusNotFound = 404;
const TInt KNSmlDmStatusCommandNotAllowed = 405;
const TInt KNSmlDmStatusOptionalFeatureNotSupported = 406;
const TInt KNSmlDmStatusAuthenticationRequired = 407;
const TInt KNSmlDmStatusIncompleteCommand = 412;
const TInt KNSmlDmStatusRequestEntityTooLarge = 413;
const TInt KNSmlDmStatusURITooLong = 414;
const TInt KNSmlDmStatusUnsupportedMediatypeOrFormat = 415;
const TInt KNSmlDmStatusAlreadyExists = 418; 
const TInt KNSmlDmStatusDeviceFull = 420; 
const TInt KNSmlDmStatusPermissionDenied = 425; 
const TInt KNSmlDmStatusCommandFailed = 500;
const TInt KNSmlDmStatusAtomicFailed = 507;
const TInt KNSmlDmStatusDataStoreFailure = 510;
const TInt KNSmlDmStatusAtomicRollBackFailed = 516;

const TInt KNSmlDmStatusLargestOK = 205;

const TInt KNSmlDmStatusSuccess = 1200;
const TInt KNSmlDmStatusClientError = 1400;
const TInt KNSmlDmStatusUserCancelled = 1401;
const TInt KNSmlDmStatusDownloadFailed = 1402;
const TInt KNSmlDmStatusAltDwnldAuthFail = 1403;
const TInt KNSmlDmStatusDownFailOOM = 1404;
const TInt KNSmlDmStatusInstallFailed = 1405;
const TInt KNSmlDmStatusInstallOOM = 1406;
const TInt KNSmlDmStatusPkgValidationFail = 1407;
const TInt KNSmlDmStatusRemoveFailed = 1408;
const TInt KNSmlDmStatusActivateFailed = 1409;
const TInt KNSmlDmStatusDeactivateFailed = 1410;
const TInt KNSmlDmStatusNotImplemented = 1411;
const TInt KNSmlDmStatusUndefError = 1412;
const TInt KNSmlDmStatusOperationReject = 1413;
const TInt KNSmlDmStatusAltDwnldSrvError = 1500;
const TInt KNSmlDmStatusAltDwnldSrvUnavailable = 1501;

#endif // __NSMLDMTREECONSTANTS_H