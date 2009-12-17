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
* Description: 
*     WPHandlerDebug contains debug macros.
*
*/


#ifndef WPHANDLERDEBUG_H 
#define WPHANDLERDEBUG_H

// MACROS

// Define this to enable debug output
//#define HANDLERDEBUG

// DEBUG can be used for outputting debug
#ifdef HANDLERDEBUG
#define DEBUG(TEXT) WPHandlerUtil::Debug( _L(#TEXT) );
#define DVA(TEXT, ARG1) WPHandlerUtil::Debug( _L(#TEXT), (ARG1) );
#define DVA2(TEXT, ARG1, ARG2) WPHandlerUtil::Debug( _L(#TEXT), (ARG1), (ARG2) );
#define DVA3(TEXT, ARG1, ARG2, ARG3) WPHandlerUtil::Debug( _L(#TEXT), (ARG1), (ARG2), (ARG3) );
#define DHEX(TEXT) WPHandlerUtil::BinDebug( TEXT );
#else
#define DEBUG(TEXT)
#define DVA(TEXT, ARG1)
#define DVA2(TEXT, ARG1, ARG2)
#define DVA3(TEXT, ARG1, ARG2, ARG3)
#define DHEX(TEXT) 
#endif

#endif /* WPHANDLERDEBUG_H*/
