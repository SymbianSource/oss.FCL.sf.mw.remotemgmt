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
* Description:  Interface to observe suite plug-ins
*
*/


#ifndef DIAGSUITEOBSERVER_H
#define DIAGSUITEOBSERVER_H

// FORWARD DECLARTION
class MDiagSuitePlugin;

/**
*  Diagnostics Framework Suite Plug-in Observer
*
*  This interface is called by Suite Plug-ins to notify the engine
*  of its suite execution completion.
*
*  @since S60 v5.0
*/
class MDiagSuiteObserver
    {
public:
    /**
    * Notify engine of suite plug-in execution completion.
    * This should be called when PreTestExecutionL() or
    * PostTestExecutionL() is called.
    *
    * @param aSender - reference to sender object.
    */
    virtual void ContinueExecutionL( const MDiagSuitePlugin& aSender ) = 0;
    };


#endif // DIAGSUITEOBSERVER_H

// End of File

