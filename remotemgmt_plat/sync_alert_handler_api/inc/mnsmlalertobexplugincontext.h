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
* Description:  obex plugin context for alert resource management
 *
*/



#ifndef _MNSMLALERTOBEXPLUGINCONTEXT_H_
#define _MNSMLALERTOBEXPLUGINCONTEXT_H_

/**
 * @brief Abstracts the services provided by Obex Plugin
 *
 * The obex plugin will need to pass an object that implements this
 * interface as the last parameter to the concrete alert handler. If
 * the alert handler needs some services from plugin (e.g. need to
 * store the RLibrary or the thread id in the plugin), it will invoke
 * these methods.
 *
 * These services are needed because the alert handler ECOM object
 * itself is destroyed after the sync is started, so it can't free the
 * resources.
 *
 */

class MNSmlAlertObexPluginContext
    {
    public:
        virtual RLibrary& AlertLibrary() = 0;
        virtual TThreadId* AlertThreadId() = 0;
    };

#endif
