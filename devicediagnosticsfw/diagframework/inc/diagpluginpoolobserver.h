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
* Description:  Observer interface to use with CDiagPluginPool 
*
*/


#ifndef DIAGPLUGINPOOLOBSERVER_H
#define DIAGPLUGINPOOLOBSERVER_H

// INCLUDES
#include <e32cmn.h>     // TUid
#include <e32def.h>     // TInt

// FORWARD DECLARATIONS
class MDiagPlugin;

/**
*  Diagnostics Framework Plugin Pool Observer
*
*  This interface is called by CDiagPluginPool to notify the plug-in 
*  loading progress. This is used only for LoadAsyncL() scenario.
*
*  @since S60 v5.0
*/
class MDiagPluginPoolObserver 
    {
public:

    /**
    * Notify client of plug-in loading progress.
    *
    * @param aCurrPlugin   Current plugin index in loading process. 
    *                       Use it to display loading progress.
    * @param aPluginCount  Totals number of plug-ins
    * @param aLoadedPluginUid  Uid of the plug-in just loaded.
    */
    virtual void LoadProgressL( TUint aCurrentPlugin, 
                                TUint aPluginCount, 
                                const TUid& aLoadedPluginUid ) = 0;


    /**
    * Notify client of plug-in loading completion
    *
    * @param aError     KErrNone - Success
    *                   KErrCorrupt  - One ore more plugin could not
    *                   be loaded. 
    *                   KErrNoMemory - Not enough memory.
    */
    virtual void LoadCompletedL( TInt aError ) = 0;
    };


#endif // DIAGPLUGINPOOLOBSERVER_H

// End of File

