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
* Description:  terminalSecurity Debug macro definitions
*
*/



#ifndef SCPDEBUG_H
#define SCPDEBUG_H

   _LIT( KLogFile, "SCP.log" );
   _LIT( KLogDir, "tarm" );
   
//#define SCP_ENABLE_DEBUG
#if defined (_DEBUG)

#include <e32std.h>
#include <f32file.h>
#include <flogger.h>
#include <e32svr.h>

        inline void FWrite (TRefByValue<const TDesC> aFmt,...)
            {
            VA_LIST list;
            VA_START( list, aFmt );
            RFileLogger::WriteFormat( KLogDir,KLogFile,EFileLoggingModeAppend ,TPtrC(aFmt) ,list );
            }

				#define Dprint(arg...)    FWrite arg;
#else
				#define Dprint(arg...)
#endif  // _DEBUG
       
#endif      // SCPDEBUG_H