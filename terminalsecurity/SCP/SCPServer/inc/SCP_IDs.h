/*
* Copyright (c) 2000 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description: Implementation of terminalsecurity components
*
*/

#ifndef SCP_IDS_H
#define SCP_IDS_H

// LOCAL CONSTANTS

// Error codes
const TInt KErrSCPInvalidCode               = -101;
const TInt KErrSCPCodeChangeNotAllowed      = -102;

// Events
const TInt KSCPEventValidate                =   1;
const TInt KSCPEventPasswordChanged         =   2;
const TInt KSCPEventAuthenticationAttempted =   3;
const TInt KSCPEventConfigurationQuery      =   4;
const TInt KSCPEventRetrieveConfiguration   =   5;
const TInt KSCPEventPasswordChangeQuery     =   6;
const TInt KSCPEventReset                   =   7;

// Parameter IDs
const TInt KSCPParamEventStatus             =   1;
const TInt KSCPParamStatus                  =   2;
const TInt KSCPParamAction                  =   3;
const TInt KSCPParamUIMode                  =   4;
const TInt KSCPParamPromptText              =   5;
const TInt KSCPParamPassword                =   6;
const TInt KSCPParamID                      =   7;
const TInt KSCPParamValue                   =   8;
const TInt KSCPParamIdentity                =   9;
const TInt KSCPParamNoteIcon                =   10;
const TInt KSCPParamContext                 =   11;
const TInt KSCPParamStorage                 =   12;
const TInt KSCPParamNotifType               =   13;


// Value IDs
const TInt KSCPValueEventConsumed           =   1;

const TInt KSCPActionShowUI                 =   1;
const TInt KSCPActionForceChange            =   2;

const TInt KSCPUIDialog                     =   1;
const TInt KSCPUINote                       =   2;

const TInt KSCPUINoteWarning                =   1;
const TInt KSCPUINoteError                  =   2;

const TInt KSCPContextChangePsw             =   1;
const TInt KSCPContextQueryPsw              =   2;
const TInt KSCPContextRfs                   =   3;

const TInt KSCPStorageCommon                =   1;
const TInt KSCPStoragePrivate               =   2;

const TInt KSCPNotifTypeInteractive         =   1;
const TInt KSCPNotifTypeStatic              =   2;


#endif      // SCP_IDS_H   
            
// End of File

