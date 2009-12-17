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
* Description: This header contains the debug macros used by the application
*
*/
#ifndef __DMEVENTNOTIFIERDEBUG_H__
#define __DMEVENTNOTIFIERDEBUG_H__

#include <e32std.h>
#include <f32file.h>
#include <flogger.h>
#include <e32svr.h>

_LIT( KLogFile, "DmEventNotifier.log" );
_LIT( KLogDirFullName, "c:\\logs\\" );
_LIT( KLogDir, "tarm" );

inline void FWrite (TRefByValue<const TDesC> aFmt,...)
    {
    VA_LIST list;
    VA_START( list, aFmt );
    RFileLogger::WriteFormat( KLogDir,KLogFile,EFileLoggingModeAppend ,TPtrC(aFmt) ,list );
    }

#if defined (_DEBUG)

#if defined(__WINS__)
    #define FLOG            RDebug::Print
    #define FLOG(arg...)    RDebug::Print(arg);
#else
    #define FLOG            FWrite
    #define FLOG(arg...)    FWrite(arg);
#endif

#else
	#define FLOG
	#define FLOG(arg...)
	
#endif //_DEBUG
#endif      // __DMEVENTNOTIFIERDEBUG_H__
// End of File
