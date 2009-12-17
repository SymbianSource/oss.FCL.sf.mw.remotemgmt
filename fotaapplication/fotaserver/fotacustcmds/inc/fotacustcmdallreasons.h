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
* Declaration of CFotaCustCmdAllReasons class.
*
*/

#ifndef FOTACUSTCMDALLREASONS_H
#define FOTACUSTCMDALLREASONS_H

#include <e32base.h>
#include <ssm/ssmcustomcommand.h>

/**
 *  Each command is implemented in a class derived from MSsmCustomCommand.
 *  It is also possible to implement multiple similar command in the same
 *  class and differentiate between them based on constructor parameters
 *  from the factory method.
 */
NONSHARABLE_CLASS( CFotaCustCmdAllReasons ) : public CBase, public MSsmCustomCommand
    {

public:

    /**
     * Two-phased constructor
     */
	static CFotaCustCmdAllReasons* NewL();

    /**
     * Destructor
     */
    virtual ~CFotaCustCmdAllReasons();

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
     * Takes action depending on the fota update state cenrep
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
	
    /**
     * Finds whether fota is schedduled or not 
     * @Params None
     * @return 
     * ETrue if  fota is scheduled else EFalse
     */
  TBool FindScheduleL();
  
private: // data

    /**
    * If the command needs command env in Execute function, it should store it
    * to member variable:
    *
    * Custom command environment. Not owned. Set in Initialize.
    * CSsmCustomCommandEnv* iEnv;
    */
	
	/*
	* Function description:
	* This function checks for dmEventNotifier. 
	* This checks for the specific scenario "when MMC is removed/inserted when phone is 
	* in switch off mode. Then it starts the DmEventNotifier.
	*
	* @params: None
	* @Return: void
	*/
    void checkDMEventNotifierL();

    };

#endif // FOTACUSTCMDALLREASONS_H
