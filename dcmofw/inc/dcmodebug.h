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
* Description:  DCMO framework debug purpose
*
*/

#ifndef DEBUG_DCMO_HEADER
#define DEBUG_DCMO_HEADER

// INCLUDES

#include <f32file.h>
#include <flogger.h>

// CONSTANTS
_LIT( KDcmoLogFolderName, "dcmo" );
_LIT( KDcmoLogFileName, "dcmo.log" );
// MACROS
#ifdef _DEBUG
#define _DCMO_FILE_DEBUG
#endif

#ifdef _DCMO_FILE_DEBUG
		#define RDEBUG(X)					RFileLogger::Write( KDcmoLogFolderName, KDcmoLogFileName, EFileLoggingModeAppend, _L(X) )
		#define RDEBUG_2(X,Y)			RFileLogger::WriteFormat( KDcmoLogFolderName, KDcmoLogFileName, EFileLoggingModeAppend, _L(X),Y )	
		#define RDEBUG_3(X,Y,Z)	  RFileLogger::WriteFormat( KDcmoLogFolderName, KDcmoLogFileName, EFileLoggingModeAppend, _L(X),Y,Z )	
#else

	#ifdef _DEBUG
		#define RDEBUG(X)								RDebug::Print(_L(X))
		#define RDEBUG_2(X,Y)						RDebug::Print(_L(X),Y)
		#define RDEBUG_3(X,Y,Z)					RDebug::Print(_L(X),Y,Z)
	#else
		#define RDEBUG(X)								
		#define RDEBUG_2(X,Y)						
		#define RDEBUG_3(X,Y,Z)	
	#endif // _DEBUG
	
#endif // _DCMO_FILE_DEBUG

#endif // DEBUG_HEADER_

