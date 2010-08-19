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


#ifndef _LAW_DEBUG_HEADER_
#define _LAW_DEBUG_HEADER_

#include <f32file.h>
#include <flogger.h>
// CONSTANTS
_LIT( KLawLogFolderName, "law" );
_LIT( KLawLogFileName, "law.log" );
// MACROS
#ifdef _DEBUG
#define _LAW_FILE_DEBUG
#endif

#ifdef _LAW_FILE_DEBUG
		#define RLDEBUG(X)					RFileLogger::Write( KLawLogFolderName, KLawLogFileName, EFileLoggingModeAppend, _L(X) )
		#define RLDEBUG_2(X,Y)			RFileLogger::WriteFormat( KLawLogFolderName, KLawLogFileName, EFileLoggingModeAppend, _L(X),Y )	
		#define RLDEBUG_3(X,Y,Z)	  RFileLogger::WriteFormat( KLawLogFolderName, KLawLogFileName, EFileLoggingModeAppend, _L(X),Y,Z )	
#else

	#if defined (_DEBUG)
		#define RLDEBUG(X)								RDebug::Print(_L(X))
		#define RLDEBUG_2(X,Y)						RDebug::Print(_L(X),Y)
		#define RLDEBUG_3(X,Y,Z)					RDebug::Print(_L(X),Y,Z)
	#else
		#define RLDEBUG(X)								
		#define RLDEBUG_2(X,Y)						
		#define RLDEBUG_3(X,Y,Z)	
	#endif // _DEBUG
	
#endif // _LAW_FILE_DEBUG




#endif // _LAW_DEBUG_HEADER_



