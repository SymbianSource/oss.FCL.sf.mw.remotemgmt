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
* Description:  LAWMO CallBack Interface definition
*
*/

#ifndef _MLAWMOCALLBACKINTERFACE_H__
#define _MLAWMOCALLBACKINTERFACE_H__

#include <e32base.h>

class MLawmoPluginWipeObserver
	{
public:

	/**
	 * Called to notify the observer that the wipe call has been completed
	 * by the Plugin at hand.
	 * @param aSession session
	 * @param aError   <code>KErrNone</code>, if opening the session succeeded;
	 *                 or one of the system-wide error codes, if opening the 
	 *                 session failed
	 */
	virtual void HandleWipeCompleted(TInt status)= 0;

    };

#endif  // _MLAWMOCALLBACKINTERFACE_H__

