/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:   Represent client's session at serverside
*
*/



#ifndef CFOTASRVSESSION_H
#define CFOTASRVSESSION_H

// INCLUDES
#include <eikdoc.h>
#include <AknServerApp.h>
#include <fotaengine.h>
#include "FotaServer.h"

// CLASS DECLARATION
/**
 *  Fotaserver's session class
 *
 *  @lib    fotaserver.exe
 *  @since  S60 v3.1
 */
class CFotaSrvSession : public CAknAppServiceBase
	{
public:

    enum TClient
	    {
        EUnknown = 0,
        EDMHostServer = 1,
        EOMADMAppUi = 2,
        EFotaServer  = 3,
        EStarter  = 4,
        EFMSServer = 5,
        EFotaTestApp = 6,
	ESoftwareChecker = 7,
	ESoftwareCheckerBackground = 8
        };

	CFotaSrvSession( );
	
    /**
	* C++ Destructor
	*/
	virtual ~CFotaSrvSession();

    /**
    * Getter for fotaserver
    *
    * @since   S60   v3.1
    * @param   None
    * @return  Fotaserver
    */
    CFotaServer*    FotaServer() const;

public: // new functions

    /**
    * Checks client secure id (dmhostserver,fotaserver,fotatester) 
    *
    * @since   S60   v3.1
    * @param   Message containing client details
    * @return  is it fotaserver
    */
    static TClient CheckClientSecureIdL (const RMessage2& aMessage);

protected: // from CSession2

    /**
    * CreateL
    *
    * @since   S60   v3.1
    * @param   none
    * @return  None
    */
    void CreateL();
	
	
    /**
    * service client request
    *
    * @since   S60   v3.1
    * @param   aMessage     message containing client request details
    * @return  None
    */
    void ServiceL( const RMessage2& aMessage );
	

    /**
    * Handle service error
    *
    * @since   S60   v3.1
	* @param aMessage  Msg that resulted in error
	* @param aError    Error code
	* @return None
	*/
	void ServiceError( const RMessage2& aMessage, TInt aError );

private:
     /**
     * Error caught while writing data into FS
     */ 
     TInt iError ;

    /**
     * document
     */
	//CEikDocument*   iDoc;
	};


#endif 
