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
* Description:
*
*/

#ifndef __SCPDBDEBUG_H__
#define __SCPDBDEBUG_H__
#include <e32std.h>
#include <f32file.h>
#include <flogger.h>
#include <e32svr.h>

#if defined (_DEBUG)
    _LIT( KLogFile2, "SCPDB.log" );
    _LIT( KLogDir2, "tarm" );
    
        inline void FDBWrite (TRefByValue<const TDesC> aFmt,...) {
            VA_LIST list;
            VA_START(list, aFmt);
            RFileLogger :: WriteFormat(KLogDir2, KLogFile2, EFileLoggingModeAppend, TPtrC(aFmt), list);
        }
    
    #define _SCPDB_LOG(arg...) FDBWrite(arg);
#else
    #define _SCPDB_LOG(arg...)
#endif // _DEBUG
#endif //__SCPDBDEBUG_H__

