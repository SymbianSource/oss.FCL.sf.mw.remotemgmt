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
* Description:  Constants which are used in DS agent
*
*/


#ifndef __NSMLDSAGCONSTANTS_H__
#define __NSMLDSAGCONSTANTS_H__

// INCLUDE FILES
#include <e32base.h>

// <MAPINFO_RESEND_MOD_BEGIN>
_LIT(KNSmlDSContentAtomicOperationName, "nsmldscontentatomicoperation");
// <MAPINFO_RESEND_MOD_END>

// CONSTANTS
// alert codes
_LIT8( KNSmlDSTwoWay, "200" );
_LIT8( KNSmlDSSlowSync, "201" );
_LIT8( KNSmlDSOneWayFromClient, "202" );
_LIT8( KNSmlDSRefreshFromClient, "203" );
_LIT8( KNSmlDSOneWayFromServer, "204" );
_LIT8( KNSmlDSRefreshFromServer, "205" );
_LIT8( KNSmlDSTwoWayByServer, "206" );
_LIT8( KNSmlDSOneWayFromClientByServer, "207" );
_LIT8( KNSmlDSRefreshFromClientByServer, "208" );
_LIT8( KNSmlDSOneWayFromServerByServer, "209" );
_LIT8( KNSmlDSRefreshFromServerByServer, "210" );
//RD_SUSPEND_RESUME
_LIT8( KNSmlDSResume, "225" );
//RD_SUSPEND_RESUME

_LIT8( KNSmlDSAgentResultAlert, "221" );
_LIT8( KNSmlDSAgentNextMessage, "222" );
_LIT8( KNSmlDSAgentNoEndOfData, "223" );  
_LIT8( KNSmlDSMeaningfulAlertMin, "100" );
_LIT8( KNSmlDSMeaningfulAlertMax, "250" );

_LIT8( KNSmlDSAgentServerAlertMin, "206" );
_LIT8( KNSmlDSAgentServerAlertMax, "210" );

// DS protocol version
_LIT8( KNSmlDSAgentVerProto, "SyncML/1.1" );
_LIT8( KNSmlDSAgentVerProto12, "SyncML/1.2" );

// DS MIME type
_LIT8( KNSmlDSAgentMIMEType, "application/vnd.syncml+wbxml" );

// device info
_LIT8( KNSmlDSAgentDeviceInfoContentType, "application/vnd.syncml-devinf+wbxml" );
_LIT( KNSmlDSAgentDeviceInfo11URI, "./devinf11" );
_LIT( KNSmlDSAgentDeviceInfo12URI, "./devinf12" );

// date format for sync anchors
_LIT8( KNSmlDSAgentAnchorDateFormat, "%04d%02d%02dT%02d%02d%02dZ" ); 

// maximum GUID size
_LIT8( KNSmlDSAgentMaxGUIDSize, "8" );

// minimum number of items in batch mode
const TInt KNSmlBatchLimit = 3;

_LIT8( KNSmlDSAgentMINDT, "MINDT=" );
// MAXDT parameter
_LIT8( KNSmlDSAgentMAXDT, "MAXDT=" );

_LIT8( KNSmlDSAgentDisplayAlert, "1100" );

#endif // __NSMLDSAGCONSTANTS_H__

// End of File
