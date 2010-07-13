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
* Description: Implementation of DM Device Dialogs
*/

#ifndef __DMDEVDEBUG_H_
#define __DMDEVDEBUG_H_
#include <e32debug.h>
#if 0
#ifdef _DEBUG
	#define _DMDEV_DEBUG
#endif

   #ifdef _DMDEV_DEBUG

        #include <e32std.h>
        #include <f32file.h>
        #include <flogger.h>
        #include <e32svr.h>

        _LIT( KLogFile, "dmdevdialog.log" );
        _LIT( KLogDirFullName, "c:\\logs\\" );
        _LIT( KLogDir, "Dmdd" );

        inline void FWrite (TRefByValue<const TDesC> aFmt,...)
            {
            VA_LIST list;
            VA_START( list, aFmt );
            RFileLogger::WriteFormat( KLogDir,KLogFile,EFileLoggingModeAppend ,TPtrC(aFmt) ,list );
            }


        #ifdef _DMDEV_DEBUG_RDEBUG 
            #ifndef __GNUC__          
                #define FLOG            RDebug::Print
            #else   // __GNUC__       
                #define FLOG(arg...)    RDebug::Print(arg);
            #endif // __GNUC__        
        #else // _DMDEV_DEBUG_RDEBUG
            #ifndef __GNUC__          
                #define FLOG            FWrite
            #else   // __GNUC__       
                #define FLOG(arg...)    FWrite(arg);
            #endif // __GNUC__        
        #endif // _DMDEV_DEBUG_RDEBUG

    #else   // _DMDEV_DEBUG    
        #ifndef __GNUC__
            #define FLOG
        #else
            #define FLOG(arg...)
        #endif // __GNUC__
    #endif // _DMDEV_DEBUG
#endif
#define FLOG(arg...)    RDebug::Print(arg);
#endif /*DMDEVDEBUG_H_*/
