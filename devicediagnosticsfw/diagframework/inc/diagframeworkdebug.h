/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Debugging trace definition for framework and plug-ins.
*
*/


#ifndef DIAGFRAMEWORKDEBUG_H
#define DIAGFRAMEWORKDEBUG_H

/**
* Diagnostics Framework Debug Traces
*
* This header file defines common debug trace macros for Diagnostics Framework.
* It is used by the diagnostics framework internally. However, it is 
* recomanded that application and plug-ins use the macros defined here
* in order to allow consistent logging.
*
* Debugging messages are enabled only on non DEBUG build. 
*
* These macros can be configured to use either Flogger (RFileLogger) or
* RDebug.  By default, it will use RFileLogger. If RDebug trace is needed,
* uncomment //#define DIAG_DEBUG_LOG_USE_RDEBUG below.
*
* The following macros are provided by this files.
*
* LOGTEXT( aDescriptor );   
*   - This prints a descriptor to log. E.g. if you have a TDesC type, then
*   you can use this. e.g.
*
*       void CMyClass::PrintSomeText( const TDesC& aText )
*           {
*           LOGTEXT( aText );
*           }
* 
* LOGSTRING( aFormat )
* LOGSTRING2( aFormat, a );
* LOGSTRING3( aFormat, a, b );
* LOGSTRING4( aFormat, a, b, c );
* LOGSTRING5( aFormat, a, b, c, d );
*   - These macros are used to print debugging messages with formatting text 
*   and several arguments. Formatting string is similar to printf.
*
*       void CMyClass::PrintSomeText( TInt aInt, TInt aHex )
*           {
*           LOGSTRING( "CMyClass::PrintSomeText() Enter" );
*           LOGSTRING3( "CMyClass::PrintSomeText() aInt = %d, aHex = 0x%08x", 
*                       aInt, 
*                       aHex );
*           }
*
*/

#define ENABLE_DIAG_DEBUG_LOG       // Enable diagnostics debug log
//#define DIAG_DEBUG_LOG_USE_RDEBUG   // Use RDebug::Print instead of flogger.

#if ( defined _DEBUG && defined ENABLE_DIAG_DEBUG_LOG )

#ifdef DIAG_DEBUG_LOG_USE_RDEBUG

#include <e32debug.h> // RDebug

#define LOGTEXT( aDescriptor )                              \
    {                                                       \
    RDebug::Print( aDescriptor );                           \
    }

#define LOGSTRING( aFormat )                                \
    {                                                       \
    _LIT( KFormat, aFormat );                               \
    RDebug::Print( KFormat );                               \
    }

#define LOGSTRING2( aFormat, a )                            \
    {                                                       \
    _LIT( KFormat, aFormat );                               \
    RDebug::Print( KFormat, a );                            \
    }

#define LOGSTRING3( aFormat, a, b )                         \
    {                                                       \
    _LIT( KFormat, aFormat );                               \
    RDebug::Print( KFormat, a, b );                         \
    }

#define LOGSTRING4( aFormat, a, b, c )                      \
    {                                                       \
    _LIT( KFormat, aFormat );                               \
    RDebug::Print( KFormat, a, b, c );                      \
    }

#define LOGSTRING5( aFormat, a, b, c, d )                   \
    {                                                       \
    _LIT( KFormat, aFormat );                               \
    RDebug::Print( KFormat, a, b, c, d );                   \
    }

#else // #ifdef DIAG_DEBUG_LOG_USE_RDEBUG

// SYSTEM INCLUDES
#include <flogger.h>

// CONSTANTS
_LIT( KDiagFrameworkLogFolder,  "DiagFramework" );
_LIT( KDiagFrameworkLogFile,    "LOG.TXT" );

// MACROS
#define LOGTEXT( aDescriptor )                              \
    {                                                       \
    RFileLogger::Write( KDiagFrameworkLogFolder(),          \
                        KDiagFrameworkLogFile(),            \
                        EFileLoggingModeAppend,             \
                        aDescriptor );                      \
    }

#define LOGSTRING( aFormat )                                \
    {                                                       \
    _LIT( KFormat, aFormat );                               \
    RFileLogger::Write( KDiagFrameworkLogFolder(),          \
                        KDiagFrameworkLogFile(),            \
                        EFileLoggingModeAppend,             \
                        KFormat() );                        \
    }

#define LOGSTRING2( aFormat, a )                            \
    {                                                       \
    _LIT( KFormat, aFormat );                               \
    RFileLogger::WriteFormat( KDiagFrameworkLogFolder(),    \
                              KDiagFrameworkLogFile(),      \
                              EFileLoggingModeAppend,       \
                              TRefByValue<const TDesC>( KFormat() ), \
                              a );                          \
    }

#define LOGSTRING3( aFormat, a, b )                         \
    {                                                       \
    _LIT( KFormat, aFormat );                               \
    RFileLogger::WriteFormat( KDiagFrameworkLogFolder(),    \
                              KDiagFrameworkLogFile(),      \
                              EFileLoggingModeAppend,       \
                              TRefByValue<const TDesC>( KFormat() ), \
                              a, b );                       \
    }

#define LOGSTRING4( aFormat, a, b, c )                      \
    {                                                       \
    _LIT( KFormat, aFormat );                               \
    RFileLogger::WriteFormat( KDiagFrameworkLogFolder(),    \
                              KDiagFrameworkLogFile(),      \
                              EFileLoggingModeAppend,       \
                              TRefByValue<const TDesC>( KFormat() ), \
                              a, b, c );                    \
    }

#define LOGSTRING5( aFormat, a, b, c, d )                   \
    {                                                       \
    _LIT( KFormat, aFormat );                               \
    RFileLogger::WriteFormat( KDiagFrameworkLogFolder(),    \
                              KDiagFrameworkLogFile(),      \
                              EFileLoggingModeAppend, \
                              TRefByValue<const TDesC>( KFormat() ), \
                              a, b, c, d );                 \
    }

#endif // #else DIAG_DEBUG_LOG_USE_RDEBUG

#else   // _DEBUG && ENABLE_DIAG_DEBUG_LOG

#define LOGTEXT( aDescriptor )
#define LOGSTRING( aFormat )
#define LOGSTRING2( aFormat, a )
#define LOGSTRING3( aFormat, a, b )
#define LOGSTRING4( aFormat, a, b, c )
#define LOGSTRING5( aFormat, a, b, c, d )

#endif  // else _DEBUG && ENABLE_DIAG_DEBUG_LOG

#endif // DIAGFRAMEWORKDEBUG_H

// End of File

