/*
* ==============================================================================
*  Name        : PnpLogger.h
*  Part of     : PnpProvisioningApp
*  Description : This file defines logging interface macros
*  Version     :
*
*  Copyright (c) 2004 Nokia Corporation.
*  This material, including documentation and any related 
*  computer programs, is protected by copyright controlled by 
*  Nokia Corporation. All rights are reserved. Copying, 
*  including reproducing, storing, adapting or translating, any 
*  or all of this material requires the prior written consent of 
*  Nokia Corporation. This material also contains confidential 
*  information which may not be disclosed to others without the 
*  prior written consent of Nokia Corporation.
* ==============================================================================
*/

#ifndef __LOGGER_H__
#define __LOGGER_H__

#ifdef _DEBUG
    #define LOGGING_ENABLED
#endif
#ifdef LOGGING_ENABLED          // This must be enabled to use logging system

#define LOGGER_LOGGING          // Log to Logger



#ifdef LOGGER_LOGGING

//  INCLUDES
#include <flogger.h>

// LOG SETTINGS 
_LIT( KPnpLogFolder, "syncml" );
_LIT( KPnpLogFile, "syncml.TXT" );

#endif

// CONSTANTS  
// None.

// MACROS
/*
-----------------------------------------------------------------------------

    INTERNAL MACROs. 

    DO NOT USE THESE DIRECTLY !!! 
    SEE EXTERNAL MACROS

-----------------------------------------------------------------------------
*/

#ifdef LOGGER_LOGGING

#define INTRLOGTEXT( AAA )                                                                        \
    {                                                                                            \
    RFileLogger::Write( KPnpLogFolder(), KPnpLogFile(), EFileLoggingModeAppend, AAA );            \
    }
#define INTRLOGSTRING( AAA )                                                                    \
    {                                                                                            \
    _LIT( tempLogDes, AAA );                                                                    \
    RFileLogger::Write( KPnpLogFolder(), KPnpLogFile(), EFileLoggingModeAppend, tempLogDes() ); \
    }
#define INTRLOGSTRING2( AAA, BBB )                                                                \
    {                                                                                            \
    _LIT( tempLogDes, AAA );                                                                    \
    RFileLogger::WriteFormat( KPnpLogFolder(), KPnpLogFile(), EFileLoggingModeAppend, TRefByValue<const TDesC>( tempLogDes()), BBB ); \
    }
#define INTRLOGSTRING3( AAA, BBB, CCC )                                                            \
    {                                                                                            \
    _LIT( tempLogDes, AAA );                                                                    \
    RFileLogger::WriteFormat( KPnpLogFolder(), KPnpLogFile(), EFileLoggingModeAppend, TRefByValue<const TDesC>( tempLogDes()), BBB, CCC ); \
    }
#else
#define INTRLOGTEXT( AAA )
#define INTRLOGSTRING( AAA )
#define INTRLOGSTRING2( AAA, BBB )
#define INTRLOGSTRING3( AAA, BBB, CCC )
#endif

/*
-----------------------------------------------------------------------------

    EXTERNAL MACROs

    USE THESE MACROS IN YOUR CODE !

-----------------------------------------------------------------------------
*/


#define LOGTEXT( AAA )                { \
                                    INTRLOGTEXT( AAA ); \
                                    }   // Example: LOGTEXT( own_desc );

#define LOGSTRING( AAA )            { \
                                    INTRLOGSTRING( AAA ); \
                                    }   // Example: LOGSTRING( "Test" );

#define LOGSTRING2( AAA, BBB )        { \
                                    INTRLOGSTRING2( AAA, BBB ); \
                                    }  // Example: LOGSTRING( "Test %i", aValue );

#define LOGSTRING3( AAA, BBB, CCC )    { \
                                    INTRLOGSTRING3( AAA, BBB, CCC ); \
                                    } // Example: LOGSTRING( "Test %i %i", aValue1, aValue2 );


#else   // LOGGING_ENABLED

#define LOGTEXT( AAA ) 
#define LOGSTRING( AAA ) 
#define LOGSTRING2( AAA, BBB )    
#define LOGSTRING3( AAA, BBB, CCC )    

#endif  // LOGGING_ENABLED

// DATA TYPES
// None.

// FUNCTION PROTOTYPES
// None.

// FORWARD DECLARATIONS
// None.

// CLASS DECLARATION
// None.

#endif  // __LOGGER_H__