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
* Description: Implementation of fotaserver component
* 	This is part of fotaapplication.
*
*/



#ifndef __FOTAIPCTYPES_H_
#define __FOTAIPCTYPES_H_

// INCLUDES 
#include <e32std.h>
#include <SyncMLDef.h>
#include "fotaengine.h"

// DATA TYPES
typedef TBuf16<200> TPkgIdList;

/**
 *  Inter process parameters for fotaserver
 *
 *  @lib    fotaengine.lib
 *  @since  S60 v3.1
 */
class TDownloadIPCParams
    {

public:
    TDownloadIPCParams (); 


    /**
     * Copy constructor.
     *
     * @since   S60 v3.1
     * @param   p Copy of another ipc
     * @return  None
    */
    TDownloadIPCParams (const TDownloadIPCParams& p ); 


    /**
     * Package id
     */
    TInt                            iPkgId;
    
    /**
     * Profile id
     */
    TSmlProfileId                   iProfileId;

    
    /**
     * Package name
     */
    TBuf8<KFotaMaxPkgNameLength>    iPkgName;

    
    /**
     * Package version
     */
    TBuf8<KFotaMaxPkgVersionLength> iPkgVersion;


    /**
     * Should DM server be notified via generic alert.
     * This is not saved to database!
     */
    TBool iSendAlert;


    /**
     * IAP Id used
     */
    TInt iIapId;

    /**
     * Total size of the package
     */
    TUint iPkgSize;


    /**
     * DM Session type
     */
    TInt iSessionType;


    /**
     * Should update happen later?
     */
    TBool iUpdateLtr;
    };


/**
 *  Extended inter process parameters for fotaserver
 *
 *  @lib    fotaengine.lib
 *  @since  S60 v3.1
 */
class TPackageState : public TDownloadIPCParams
    {

public:


    /**
     * Constructor
     *
     * @since   S60 v3.1
     * @param   aState  State
     * @param   aResult Result
     * @return  None
    */
    TPackageState( RFotaEngineSession::TState aState, TInt aResult);
    

    /**
     * Constructor
     *
     * @since   S60 v3.1
     * @param   aState  State
     * @return  None
    */
    TPackageState( RFotaEngineSession::TState aState);


    /**
     * Constructor
     *
     * @since   S60 v3.1
     * @param   p  Ipc params
     * @return  None
    */
    TPackageState( TDownloadIPCParams p);

    TPackageState();

    /**
     * Assignment operator
     *
     * @since   S60 v3.1
     * @param   a  Source 
     * @return  None
    */
    TPackageState& operator=(const TDownloadIPCParams& a);


    /**
     * Assignment operator
     *
     * @since   S60 v3.1
     * @param   a  Source 
     * @return  None
    */
    TPackageState& operator=(const TPackageState& a);


public:

    /**
     * State (see FUMO specification)
     */
    RFotaEngineSession::TState  iState;


    /**
     * Result code (FUMO)
     */
    TInt iResult;

    /**
     * Counter for generic alert raporting. If raporting fails, try again in
     * successive boots until counter runs to 0
     */
    TInt iSmlTryCount;

private :
	/*
	* Method to set the iSmlTryCount depending on Cenrep settings
	*/
	void SetSmlTryCount();
};



/**
 *  Update reminder task
 *
 *  @lib    fotaengine.lib
 *  @since  S60 v3.1
 */
class TFotaScheduledUpdate
{
    public:
    TFotaScheduledUpdate(const TInt aPkgId, const TInt aScheduleId)
        {
        iPkgId      = aPkgId;
        iScheduleId = aScheduleId;
        }

    TInt                            iPkgId;

    TInt                            iScheduleId;
};

#endif      // __FOTAIPCTYPES_H_   
            
// End of File
