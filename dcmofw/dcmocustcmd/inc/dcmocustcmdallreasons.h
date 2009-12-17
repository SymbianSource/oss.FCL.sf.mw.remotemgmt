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
* Declaration of CDCMOCustCmdAllReasons class.
*
*/

#ifndef DCMOCUSTCMDALLREASONS_H
#define DCMOCUSTCMDALLREASONS_H

#include <e32base.h>
#include <ssm/ssmcustomcommand.h>

/**
 *  Each command is implemented in a class derived from MSsmCustomCommand.
 *  It is also possible to implement multiple similar command in the same
 *  class and differentiate between them based on constructor parameters
 *  from the factory method.
 */
NONSHARABLE_CLASS( CDCMOCustCmdAllReasons ) : public CBase, public MSsmCustomCommand
    {

public:

    /**
     * Two-phased constructor
     */
	static CDCMOCustCmdAllReasons* NewL();

    /**
     * Destructor
     */
    virtual ~CDCMOCustCmdAllReasons();

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
     * Takes action depending on the DCMO update state cenrep
     * @Params None
     * @return  None
     *
     */
     
  void ExecuteL( );  
    
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

#endif // DCMOCUSTCMDALLREASONS_H
