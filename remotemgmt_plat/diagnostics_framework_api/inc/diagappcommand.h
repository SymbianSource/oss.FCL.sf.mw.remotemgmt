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
* Description:  List of commands that plug-ins can request from application.
*
*/


#ifndef DIAGAPPCOMMAND_H
#define DIAGAPPCOMMAND_H

/**
*  Diagnostics Application command types.
*
*  This is a list of command that plug-in can request from the application.
* 
*/
enum TDiagAppCommand
    {
    /**
    * EDiagAppCommandSwitchToMainView
    *
    * This command can be used by interactive view plug-in to notify the 
    * application that plug-in is done with current view and application can
    * switch to main view. View to switch to is determined by the application.
    *
    * aParam1:      No parameter is needed. Always pass NULL.
    * aParam2:      No parameter is needed. Always pass NULL.
    *
    * @code
    *   
    *   ExecutionParam().Engine().ExecuteAppCommandL( 
    *       EDiagAppCommandSwitchToMainView, 
    *       NULL,
    *       NULL );
    *   
    *   or
    *   
    *   TRAPD( err, ExecutionParam().Engine().ExecuteAppCommandL( 
    *                   EDiagAppCommandSwitchToMainView, 
    *                   NULL,
    *                   NULL ) );
    *   
    */
    EDiagAppCommandSwitchToMainView = 1
    };

#endif // DIAGAPPCOMMAND_H

// End of File

