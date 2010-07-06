/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description: Implementation of Dm device dialog client
*
*/

#ifndef DEVDIALOG_CLIENT_DEBUG_H_
#define DEVDIALOG_CLIENT_DEBUG_H_
#ifdef _DEBUG
   #ifdef _DEVDIALOG_DEBUG
#endif
        #include <e32std.h>
        #include <f32file.h>
        #include <flogger.h>
        #include <e32svr.h>

        _LIT( KLogFile, "devdiag.log" );
        _LIT( KLogDirFullName, "c:\\logs\\" );
        _LIT( KLogDir, "DMDD" );

        inline void FWrite (TRefByValue<const TDesC> aFmt,...)
            {
            VA_LIST list;
            VA_START( list, aFmt );
            RFileLogger::WriteFormat( KLogDir,KLogFile,EFileLoggingModeAppend ,TPtrC(aFmt) ,list );
            }


        #ifdef __DEVDIALOG_DEBUG_RDEBUG 
            #ifndef __GNUC__          
                #define FLOG            RDebug::Print
            #else   // __GNUC__       
                #define FLOG(arg...)    RDebug::Print(arg);
            #endif // __GNUC__        
        #else //__DEVDIALOG_DEBUG_RDEBUG
            #ifndef __GNUC__          
                #define FLOG            FWrite
            #else   // __GNUC__       
                #define FLOG(arg...)    FWrite(arg);
            #endif // __GNUC__        
        #endif // __DEVDIALOG_DEBUG_RDEBUG

    #else   // _DEVDIALOG_DEBUG
        #ifndef __GNUC__
            #define FLOG
        #else
            #define FLOG(arg...)
        #endif // __GNUC__
    #endif // _DEVDIALOG_DEBUG


#endif /*FMSCLIENTDEBUG_H_*/
