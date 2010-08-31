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
* Description:  DM Agent constants
*
*/



#ifndef __NSMLDMAGCONSTANTS_H
#define __NSMLDMAGCONSTANTS_H

// MACROS
// Alert codes
_LIT8( KNSmlDMAgentServerInitAlert, "1200" );
_LIT8( KNSmlDMAgentClientInitAlert, "1201" );
_LIT8( KNSmlDMAgentResultAlert, "1221" );
_LIT8( KNSmlDMAgentNextMessage, "1222" ); 
_LIT8( KNSmlDMAgentSessionAbortAlert, "1223" );
// FOTA
_LIT8( KNSmlDMAgentGenericAlert, "1226" );
// FOTA end
_LIT8( KNSmlDMAgentDisplayAlert, "1100" );
_LIT8( KNSmlDMAgentContinueOrAbortAlert, "1101" );
//#ifdef RD_DM_TEXT_INPUT_ALERT //for User Input Server alert

//#endif
// DM protocol version
_LIT8( KNSmlDMAgentVerProto, "DM/1.1" );
_LIT8( KNSmlDMAgentVerProto12, "DM/1.2" );
// DM MIME type
_LIT8( KNSmlDMAgentMIMEType, "application/vnd.syncml.dm+wbxml" );
// MINDT parameter
_LIT8( KNSmlDMAgentMINDT, "MINDT=" );
// MAXDT parameter
_LIT8( KNSmlDMAgentMAXDT, "MAXDT=" );
//#ifdef RD_DM_TEXT_INPUT_ALERT
// DEFAULT RESPONSE parameter
_LIT8( KNSmlDMAgentDR, "DR=" );
//MAXIMUM LENGTH parameter for user input alert
_LIT8( KNSmlDMAgentMAXLEN,"MAXLEN=" );
//Input Type paramaeter for User Input alert
_LIT8( KNSmlDMAgentIT,"IT=" );
//Echo Type paramaeter for User Input alert
_LIT8( KNSmlDMAgentET,"ET=" );
//#endif
const TUint16 KMaxInt16 = 65535;

#endif // __NSMLDMAGCONSTANTS_H
