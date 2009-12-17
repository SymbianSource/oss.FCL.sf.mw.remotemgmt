/*
* Copyright (c) 2000 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description: Implementation of terminalsecurity components
*
*/


#ifndef DEBUG_HEADER_
#define DEBUG_HEADER_

// INCLUDES

#include <f32file.h>
#include <flogger.h>

// CONSTANTS
_LIT( KTarmLogFolderName, "tarm" );
_LIT( KTarmLogFileName, "tarm.log" );
// MACROS

#ifdef _TARM_FILE_DEBUG
		#define RDEBUG(X)					RFileLogger::Write( KTarmLogFolderName, KTarmLogFileName, EFileLoggingModeAppend, _L(X) )
		#define RDEBUG_2(X,Y)			RFileLogger::WriteFormat( KTarmLogFolderName, KTarmLogFileName, EFileLoggingModeAppend, _L(X),Y )	
		#define RDEBUG_3(X,Y,Z)	  RFileLogger::WriteFormat( KTarmLogFolderName, KTarmLogFileName, EFileLoggingModeAppend, _L(X),Y,Z )	
#else

	#if defined (_DEBUG)
		#define RDEBUG(X)								RDebug::Print(_L(X))
		#define RDEBUG_2(X,Y)						RDebug::Print(_L(X),Y)
		#define RDEBUG_3(X,Y,Z)					RDebug::Print(_L(X),Y,Z)
	#else
		#define RDEBUG(X)								
		#define RDEBUG_2(X,Y)						
		#define RDEBUG_3(X,Y,Z)	
	#endif // _DEBUG
	
#endif // _TARM_FILE_DEBUG




#endif // DEBUG_HEADER_



