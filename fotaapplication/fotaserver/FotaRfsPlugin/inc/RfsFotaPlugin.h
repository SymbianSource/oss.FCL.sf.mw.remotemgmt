/*
* Copyright (c) 2005-2006 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:   Handles restory factory settings
*
*/




#ifndef RfsFotaPlugin_H
#define RfsFotaPlugin_H

//  INCLUDES
#include <e32base.h>
#include <fotastorage.h>

#include "rfsPlugin.h"

// CLASS DECLARATION

/**
 *  Ensures that there is disk space reservation in place after 1st boot
 *
 *  @lib    fotaserver.exe
 *  @since  S60 v3.1
 */
class CRfsFotaPlugin: public CRFSPlugin
    {
public:


    /**
    * Two-phased constructor.
     *
     * @since   S60   v3.1
     * @param	aInitParams initial parameters
     * @return  Error code
     */
    static CRfsFotaPlugin* NewL(TAny* aInitParams);

    virtual ~CRfsFotaPlugin();


    /**
     * Handle restory factory settings. Run at boot up.
     *
     * @since   S60   v3.1
	 * @param	aType   type of boot
     * @return  none
     */
    void RestoreFactorySettingsL( const TRfsReason aType );


    /**
    * GetScriptL
    *
	* @param    aType
	* @param    aPath
    * @return   None
    */
    void GetScriptL( const TRfsReason aType, TDes& aPath );

    
    /**
    * GetScriptL
    *
	* @param    aType
	* @param    aCommand
    * @return   None
    */
    void ExecuteCustomCommandL( const TRfsReason aType, TDesC& aCommand );


private:

    CRfsFotaPlugin();

    
    /**
    * Constructor
    *
	* @param    aInitParams
    * @return   None
    */
	CRfsFotaPlugin(TAny* aInitParams);

    void ConstructL();


    
    /**
    * Load fota storage plugin
    *
	* @param    None
    * @return   None
    */
    void            LoadStoragePluginL ();

    
    /**
    * unload fota storage plugin
    *
	* @param    None
    * @return   None
    */
    void            UnLoadStoragePluginL ();

private: // data    


    /**
     * Destructor key of fotastorageplugin.
     */
    TUid                iStorageDtorKey;


    /**
     * Fotastorage plugin. Used for disk space reservation file handling
     */
    CFotaStorage*       iStorage;

    };


#endif      // RfsFotaPlugin_H

// End of File
