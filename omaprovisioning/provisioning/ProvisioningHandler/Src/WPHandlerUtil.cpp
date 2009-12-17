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
* Description:  Factory for phone interface.
*
*/


//  INCLUDE FILES
#include <e32std.h>
#include <flogger.h>
#include "WPHandlerUtil.h"


#ifdef HANDLERDEBUG
// CONSTANTS
/// Folder where the log resides
_LIT( KLogFolder, "provisioning" );

/// The name of the log file
_LIT( KLogFileName, "handler" );

/// The format in which the time is formatted in log
_LIT( KLogTimeFormat, "%02d.%02d:%02d:%06d ");

/// The length of the string produced by KLogTimeFormat
const TInt KLogTimeFormatLength = 16;

/// How many characters a log line can contain
const TInt KLogLineLength = 256;

/// Number of bytes to represent in one line
const TInt KBytesPerLine = 16;

/// Char to prepend and append to ascii representation
const TUint16 KQuote = '\''; 

/// Replaces non-displayable chars
const TUint16 KDot = '.';

/// Number of characters printer per byte
const TInt KCharsPerByte = 5;

/// Padding size in one hex line
const TInt KPadSize = 2;

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// WPHandlerUtil::Debug
// -----------------------------------------------------------------------------
//

void WPHandlerUtil::Debug( TRefByValue<const TDesC> aText, ... )
    {
    VA_LIST args;
    VA_START( args, aText );

    TBuf<KLogLineLength> buf;
    buf.FormatList( aText, args );

    RFileLogger logger;
    TInt ret=logger.Connect();
    if (ret==KErrNone)
        {
        logger.SetDateAndTime( EFalse,EFalse );
        logger.CreateLog( KLogFolder, KLogFileName, EFileLoggingModeAppend );       
        TBuf<KLogTimeFormatLength> timeStamp;
        TTime now;
        now.HomeTime();
        TDateTime dateTime;
        dateTime = now.DateTime();
        timeStamp.Format( KLogTimeFormat, 
            dateTime.Hour(), dateTime.Minute(),
            dateTime.Second(), dateTime.MicroSecond() );
        buf.Insert( 0, timeStamp );

        logger.Write(buf);
        }

    logger.Close();

    VA_END( args );
    }
// ---------------------------------------------------------
// Hex() 
//
// ---------------------------------------------------------
void Hex( const TDesC8& aSource, TDes& aDest )
    {
    _LIT( KAlphabet, "0123456789ABCDEF" );
    TBuf<17> alphabet( KAlphabet );

    for( TInt i( 0 ); i < aSource.Length(); i++ )
        {
        TBuf8<2> buf;
        TInt8 val( aSource[i] );
        buf.Append( alphabet[ val & 0x0f ] );
        val >>= 4;
        buf.Append( alphabet[ val & 0x0f ] );

        aDest.Append( buf[1] );
        aDest.Append( buf[0] );
        aDest.Append( ' ' );
        }
    }


// ---------------------------------------------------------
// BinDebug() 
//
// ---------------------------------------------------------
void WPHandlerUtil::BinDebug( const TDesC8& aText )
    {
    TInt length( aText.Length() );

    for( TInt i( 0 ); i < length; i += KBytesPerLine )
        {
        TBuf<KBytesPerLine*KCharsPerByte + KPadSize> line;
        TPtrC8 curr( aText.Mid( i ) );
        curr.Set( curr.Left( KBytesPerLine ) );

        Hex( curr, line );
        line.Append( KQuote );
        for( TInt j( 0 ); j < curr.Length(); j++ )
            {
            TChar chr( curr[j] );
            if( chr == '%' )
                {
                line.Append( _L("%%") );
                }
            else if( chr.IsPrint() )
                {
                line.Append( chr );
                }
            else
                {
                line.Append( KDot );
                }
            }
        line.Append( KQuote );

        Debug( line );
        }
    }
#else
void WPHandlerUtil::Debug( TRefByValue<const TDesC> /*aText*/, ... )
    {
    }
void WPHandlerUtil::BinDebug( const TDesC8& /*aText*/ )
    {
    }
#endif

//  End of File  
