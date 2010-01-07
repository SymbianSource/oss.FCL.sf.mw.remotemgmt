/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:   Enables and disables debug prints.
*
*/



#ifndef WPDESTINATIONNWDEBUG_H
#define WPDESTINATIONNWDEBUG_H

#include <e32debug.h>

#ifdef _DEBUG
#define DBG_PRINT( p )          RDebug::Print( _L( p ) )
#define DBG_PRINT_2( p, i )     RDebug::Print( _L( p ), i )
#define DBG_PRINT_3( p, i, u )  RDebug::Print( _L( p ), i, u )
#else
#define DBG_PRINT( p )
#define DBG_PRINT_2( p, i )
#define DBG_PRINT_3( p, i, u )
#endif // _DEBUG


#endif // WPDESTINATIONNWDEBUG_H
