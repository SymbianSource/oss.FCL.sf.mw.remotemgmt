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
* Description:  This file defines logging macros for SyncML notifiers
*
*/


#ifndef SMLNOTIFIER_DEBUG_H
#define SMLNOTIFIER_DEBUG_H


#ifdef _DEBUG

#include <e32svr.h>
#include <e32std.h>
#include <f32file.h>
#include <flogger.h>

_LIT( KLogFile,"sml_notiflog.txt" );
_LIT( KLogDirFullName,"c:\\logs\\SyncML\\" );
_LIT( KLogDir,"SyncML" );

// ===========================================================================
#ifdef __WINS__     // File logging for WINS
// ===========================================================================
// Declare the FPrint function
inline void FPrint( const TRefByValue<const TDesC> aFmt, ... )
    {
    VA_LIST list;
    VA_START( list, aFmt );
    RFileLogger::WriteFormat( KLogDir,
                              KLogFile,
                              EFileLoggingModeAppend,
                              aFmt,
                              list );
    }

#define FLOG( a )   { FPrint( a ); }
#define FTRACE( a ) { a; }

// ===========================================================================
#else               // RDebug logging for target HW
// ===========================================================================

// Declare the FPrint function
inline void FPrint( const TRefByValue<const TDesC> aFmt, ... )
    {
    VA_LIST list;
    VA_START(list,aFmt);
    TInt tmpInt = VA_ARG(list, TInt);
    TInt tmpInt2 = VA_ARG(list, TInt);
    TInt tmpInt3 = VA_ARG(list, TInt);
    TInt tmpInt4 = VA_ARG(list, TInt);
    TInt tmpInt5 = VA_ARG(list, TInt);
    TInt tmpInt6 = VA_ARG(list, TInt);
    TInt tmpInt7 = VA_ARG(list, TInt);
    TInt tmpInt8 = VA_ARG(list, TInt);
    VA_END(list);
    RDebug::Print(aFmt, tmpInt, tmpInt2, tmpInt3, tmpInt4, tmpInt5, tmpInt6, tmpInt7, tmpInt8);
    }

#define FLOG( a ) { RDebug::Print(a);  }
#define FTRACE( a ) { a; }
#endif //__WINS__

// ===========================================================================
#else // // No loggings --> Reduced binary size
// ===========================================================================
#define FLOG( a )
#define FTRACE( a )

#endif // _DEBUG


#endif // SMLNOTIFIER_DEBUG_H

// End of File
