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

#ifndef SYNCFWCUSTOMIZER_PAN_H
#define SYNCFWCUSTOMIZER_PAN_H

/** SyncFwCustomizer application panic codes */
enum TSyncFwCustomizerPanics
	{
	ESyncFwCustomizerUi = 1
	// add further panics here
	};

inline void Panic(TSyncFwCustomizerPanics aReason)
	{
	_LIT(applicationName,"SyncFwCustomizer");
	User::Panic(applicationName, aReason);
	}

#endif // SYNCFWCUSTOMIZER_PAN_H
