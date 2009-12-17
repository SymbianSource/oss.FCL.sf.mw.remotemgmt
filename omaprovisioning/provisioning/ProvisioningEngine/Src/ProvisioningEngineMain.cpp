/*
* Copyright (c) 2002-2006 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  DLL entry point
*
*/



// INCLUDE FILES
#include <e32std.h>
#include <flogger.h>
#include "CWPEngine.pan"
#include "WPEngineDebug.h"
#include <e32debug.h>
// CONSTANTS

/// Name of the ProvisioningEngine
_LIT( KEngineName, "ProvisioningEngine" );

#ifdef ENGINEDEBUG
/// Folder where the log resides
_LIT( KLogFolder, "provisioning" );

/// The name of the log file
_LIT( KLogFileName, "engine" );

/// The format in which the time is formatted in log
_LIT( KLogTimeFormat, "%02d.%02d:%02d:%06d ");

/// The length of the string produced by KLogTimeFormat
const TInt KLogTimeFormatLength = 16;

/// How many characters a log line can contain
const TInt KLogLineLength = 256;
#endif

// ========================== OTHER EXPORTED FUNCTIONS =========================

/**
* Panic handler
* @param aPanic reason for panic.
*/
GLDEF_C void Panic(TWPPanic aPanic)
    {
    User::Panic( KEngineName, aPanic );
    }

// -----------------------------------------------------------------------------
// Debug
// -----------------------------------------------------------------------------
//
#ifdef ENGINEDEBUG
GLDEF_C void Debug( TRefByValue<const TDesC> aText, ... )
    {
    RDebug::Print(aText);
    }
#else
GLDEF_C void Debug( TRefByValue<const TDesC> /*aText*/, ... )
    {
    }
#endif

//  End of File  
