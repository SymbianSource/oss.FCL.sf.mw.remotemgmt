/*
* Copyright (c) 2004 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Common Agent constants
*
*/


#ifndef __NSMLCLIAGCONSTANTS_H
#define __NSMLCLIAGCONSTANTS_H

// INCLUDES
#include <e32base.h>

// CONSTANTS
//maximum length of SyncML status code 
const TInt KNSmlAgentStatusCodeLength = 3;
//maximum length of LUID
const TInt KNSmlAgentLUIDLength = 64;
//default port
const TInt KNSmlDefaultPort = 80;
//granularity in status container array
const TInt KNSmlStatusArrayGranularity = 5;
//granularity in results container array
const TInt KNSmlResultsArrayGranularity = 5;
//buffer size in Unicode/UTF8 conversion
const TInt KNSmlConvBufferSize = 100;
//Values to Cmd element 
const TInt KNSmlMaxCmdNameLength = 8;

// MACROS
// SyncML version
//1.2 CHANGES: 1.1 and 1.2 version support
_LIT8( KNSmlAgentVerDTD11, "1.1" );
_LIT8( KNSmlAgentVerDTD12, "1.2" );
//end changes
//_LIT8( KNSmlAgentVerDTD, "1.2" );
// Constants for URI parsing
_LIT(KNSmlHttpHeader, "http://");
_LIT(KNSmlHttpsHeader, "https://");
_LIT(KNSmlIMEIHeader, "IMEI:");

_LIT8( KNSmlAgentAdd, "Add" );
_LIT8( KNSmlAgentAlert, "Alert" );
_LIT8( KNSmlAgentAtomic, "Atomic" );
_LIT8( KNSmlAgentCopy, "Copy" );
_LIT8( KNSmlAgentDelete, "Delete" );
_LIT8( KNSmlAgentSoftDelete, "SoftDele" );
_LIT8( KNSmlAgentExec, "Exec" );
_LIT8( KNSmlAgentGet, "Get" );
_LIT8( KNSmlAgentMap, "Map" );
_LIT8( KNSmlAgentPut, "Put" );
_LIT8( KNSmlAgentReplace, "Replace" );
_LIT8( KNSmlAgentPartialReplace, "PartRepl" );
_LIT8( KNSmlAgentResults, "Results" );
_LIT8( KNSmlAgentSearch, "Search" );
_LIT8( KNSmlAgentMove, "Move" );
_LIT8( KNSmlAgentSequence, "Sequence" );
_LIT8( KNSmlAgentSync, "Sync" );
_LIT8( KNSmlAgentSyncHdr, "SyncHdr" );
// Display alert
_LIT8( KNSmlAgentDisplayAlert, "100" );

// CmdID used to SyncHdr in Status commands
_LIT8( KNSmlAgentSyncHdrCmdID, "0" );
// Uri Scheme for IMEI code.
_LIT( KNSmlAgentImeiSchemePrefix, "IMEI:" );
// Relative URI prefix
_LIT( KNSmlAgentRelativeURIPrefix, "./" ); 
//URI delimeter
_LIT( KNSmlAgentURIDelimeter, "/" );
// Authentications
_LIT8( KNSmlAgentAuthBasic, "syncml:auth-basic" );
_LIT8( KNSmlAgentAuthMD5, "syncml:auth-md5" );
// Format
_LIT8( KNSmlAgentChrFormat, "chr" );
_LIT8( KNSmlAgentBase64Format, "b64" );

#endif // __NSMLCLIAGCONSTANTS_H

// End of File
