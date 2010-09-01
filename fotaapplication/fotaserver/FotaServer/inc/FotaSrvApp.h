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
* Description:   Header file for CFotaSrvApp
*
*/




#ifndef __FOTASRVAPP_H_
#define __FOTASRVAPP_H_

// INCLUDES
#include <eikapp.h>
#include <aknapp.h>
#include "FotaServer.h"
#include "FotaSrvUI.h"
#include "fotaConst.h"

// CLASS DECLARATION

/**
* Fota server application
 *
 *  @lib    fotaserver
 *  @since  S60 v3.1
 */
class CFotaSrvApp : public CAknApplication
    {

public:
	CFotaServer* Server();

    CFotaSrvApp();

    virtual ~CFotaSrvApp();

    /**
     * Show or hide UI
     *
     * @since   S60   v3.1
     * @param   aVisible    Tells whether to hide / show the ui
     * @return  none
     */
    void SetUIVisibleL( const TBool aVisible, const TBool aAlsoFSW=EFalse);

private:
    /**
    * From CApaApplication, creates FotaSrvDocument document object.
    * @since   S60   v3.1
    * @param    None    
	* @return A pointer to the created document object.
	*/
    CApaDocument* CreateDocumentL();
    
    /**
    * From CApaApplication, returns application's UID (KUidFotaServer).
    * @since   S60   v3.1
    * @param    None
	* @return   The value of KUidFotaServer.
    */
    TUid AppDllUid() const;
    
    /**
    * From CApaApplication
    * @since   S60   v3.1
    * @param   aAppServer returned server
	* @return  None
    */
    void NewAppServerL( CApaAppServer*& aAppServer );

private:


    /**
     * Fota server.
     * Own. 
     */
    CFotaServer*    iFotaServer;


    /**
     * Document
     * Own. 
     */
    CFotaSrvDocument* iDocument;

    };

#endif

// End of File
