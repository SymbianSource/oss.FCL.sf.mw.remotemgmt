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
* Declaration of CFotaCustCmdFirmwareUpdate class.
*
*/

#ifndef FOTACUSTCMDFIRMWAREUPDATE_H
#define FOTACUSTCMDFIRMWAREUPDATE_H

#include <e32base.h>
#include <ssm/ssmcustomcommand.h>

/**
 *  Each command is implemented in a class derived from MSsmCustomCommand.
 *  It is also possible to implement multiple similar command in the same
 *  class and differentiate between them based on constructor parameters
 *  from the factory method.
 */
NONSHARABLE_CLASS( CFotaCustCmdFirmwareUpdate ) : public CBase, public MSsmCustomCommand
    {

public:

    /**
     * Two-phased constructor
     */
	static CFotaCustCmdFirmwareUpdate* NewL();

    /**
     * Destructor
     */
    virtual ~CFotaCustCmdFirmwareUpdate();

private: // From MSsmCustomCommand

    /**
     * @see MSsmCustomCommand
     */
    TInt Initialize( CSsmCustomCommandEnv* aCmdEnv );

    /**
     * @see MSsmCustomCommand
     */
	void Execute( const TDesC8& aParams, TRequestStatus& aRequest );
	
	  /**
     * invokes fotaengine after fota update
     * @Params None
     * @return  None
     *
     */
	void ExecuteL();

    /**
     * @see MSsmCustomCommand
     */
	void ExecuteCancel();

    /**
     * @see MSsmCustomCommand
     */
	void Close();

    /**
     * @see MSsmCustomCommand
     */
	void Release();

private: // data

    /**
    * If the command needs command env in Execute function, it should store it
    * to member variable:
    *
    * Custom command environment. Not owned. Set in Initialize.
    * CSsmCustomCommandEnv* iEnv;
    */

    };

#endif // FOTACUSTCMDFIRMWAREUPDATE_H
