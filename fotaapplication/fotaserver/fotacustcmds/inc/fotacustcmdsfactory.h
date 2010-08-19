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
* Declaration of FotaCustCmdsFactory class.
*
*/

#ifndef FOTACUSTCMDSFACTORY_H
#define FOTACUSTCMDSFACTORY_H

#include <e32def.h>

class MSsmCustomCommand;

/**
 *  Creates custom command objects.
 *
 *  Custom commands are loaded by system state manager based on DLL name and
 *  function ordinal.
 *  Each DLL containing custom commands must offer factory methods for each
 *  custom command.
 *  The function ordinals must be exactly the same in emulator and HW builds
 *  or loading the custom command may have unpredictable results.
 *
 *  FotaCustCmdsFactory is a static class containing factory method of one
 *  custom command. It can be easily extended to contain factory methods of
 *  multiple custom commands by adding more member functions.
 *
 *  It is better to implement own factory method for each separate command
 *  than to try to use parameters of Execute function to distinguish between
 *  them.
 *  Note that similar commands can be implemented in the same command class -
 *  just the factory methods need to be different.
 */

class FotaCustCmdsFactory
    {

public:

    /**
     * Creates and returns a custom command of type CFotaCustCmdAllReasons.
     * This method has function ordinal 1 in fotacustcmds.dll.
     *
     * @return A custom command object.
     */
	IMPORT_C static MSsmCustomCommand* FotaCustCmdAllReasonsNewL();

    /**
     * Creates and returns a custom command of type CFotaCustCmdFirmwareUpdate.
     * This method has function ordinal 2 in fotacustcmds.dll.
     *
     * @return A custom command object.
     */
	IMPORT_C static MSsmCustomCommand* FotaCustCmdFirmwareUpdateNewL();

    /**
     * Creates and returns a custom command of type CFotaCustCmdFirstBoot.
     * This method has function ordinal 3 in fotacustcmds.dll.
     *
     * @return A custom command object.
     */
	IMPORT_C static MSsmCustomCommand* FotaCustCmdFirstBootNewL();

    // To add new custom commands to this DLL, add their factory methods here.

    };

#endif // FOTACUSTCMDSFACTORY