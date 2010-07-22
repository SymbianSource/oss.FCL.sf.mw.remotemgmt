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
* Description: Implementation of fotaserver component
* 	This is part of fotaapplication.
*
*/

#ifndef __FMS_AOB_H__
#define __FMS_AOB_H__

#include <e32base.h>
#include <e32std.h>
#include <e32property.h>
#include <rconnmon.h>
#include <etel.h>
#include <fotaengine.h>
#include "fmsserver.h"
#include "fmsdebug.h"

class CFMSServer;

// CLASS DECLARATION     
class CFMSInterruptAob : public CActive,private MConnectionMonitorObserver
	{	
	friend class CFMSServer;	
	
	public:
	/**
	 * Default constructor
	 */
	CFMSInterruptAob();
	
	/**
	 * second phase constructor
	 */
	void ConstructL();
	
	/**     
     * Logs the new request/interrupt to  be monitored  
     * @param aType, interrupt reason
     * @return None
     */
	void StartL(TFmsIpcCommands aType);
	
	/**
	 * Starts Active object to monitor for the update interrupt in case of USB charger support
	 * @param aType, Interrupt reason
	 * @return None
	 */
	void StartBatteryMonitoringL(TFmsIpcCommands aType, TUint aLevel);

	/**     
     * Sets the memory size member variable to be monitored 
     * for the current interrupt 
     * @param aSize, Memory space required
     * @param aDrive, Drive used for download
     * @return None
     */
	void MemoryToMonitorL(TInt aSize, TDriveNumber aDrive);
	
	/**     
     * Sets the network type for the current interrupt 
     * @param aBearer
     * @return None
     */
	void NetworkTypeL(TInt aBearer);
	/**     
     * Launches FOTA install notifier 
     * @param None
     * @return None
     */
	void LaunchFotaScheduleUpdate();
	
	/*
	 * Destructor
	 */
	virtual ~CFMSInterruptAob();
	
	/**     
	 * Starts monitor for active call end 	
	 * @param aType, Reason to be monitored	 
	 * @return None
	 */
	void StartCallEndMonitoringL(TFmsIpcCommands aType);
	
	private:
	
	/**
     * From CActive
     * Called by Active scheduler when the asynchronous 
     * request completed     
     * @param None
     * @return None
     */
	void RunL();
	
	/**
     * From CActive
     * Called when a pending request has to be cancelled     
     * @param None
     * @return None
     */
	void DoCancel();
	
	 /**
     * From CActive
     * Handle a leave occurred in the RunL().
     * Note: If RunError function pointer is not given in construction,
     * the leave error is to be propagated back to the active scheduler!
     *
     * @since S60 v3.2
     * @param aError the error code of leave
     */
    TInt RunError( TInt aError );
    
    /**
     * Checks the package size is more than warning level of memory
     * @param aCritical to set EFalse/ETrue
     * @return TInt
     */
	TInt IsPkgSizeMoreThanWL(TBool& aCritical);
	
	/**
	  * Starts network notification registry
	  * @param None
	  * @return None
	  */
	TBool CheckNetworkL();
	
	/**
	  * Starts network notification registry
	  * @param None
	  * @return None
	  */
	void LaunchFotaEngineL();		
	
	/**
	  * Starts network notification registry
	  * @param None
	  * @return None
	  */
	TBool CheckGlobalRFStateL();
	
	/**
	  * Stops the FMS Server
	  * @param None
	  * @return None
	  */
	void StopServer();
	
	/**
	  * Checks the memory size to be monitored
	  * after adding the warning level
	  * @param None
	  * @return None
	  */
	void CheckMemSizeL();
	
	public: 
    
  	/**
    * Starts network notification registry
    * @param None
    * @return None
    */
	void NotifyL() ;
	
    /**
    * Stops network notifications.
    * @param None
    * @return None
    */
   	void StopNotify() ;
   	
   	/**
     * sets n/w to 3G.
     * @param None
     * @return None
     */
   	void SetWcdma() ;
   	
   	/**
     * Starts n/w registry check in Hard reeboot case
     * uses CPeriodic variable to call static method
     * @param None
     * @return None
     */
   	void StartNetworkRegistryCheckL();
   	
   	/**
     * checks n/w registry in Hard reeboot case
     * and destroys the CPeriodic if connection is there
     * and triggers fota engine
     * @param None
     * @return None
     */
	void NetworkRegistryCheckL();
	
	/**
	 * Launches Fota update note	 
	 * @param aPkgId, package Id of FOTA Update
	 * @param aProfilleId, Profile ID of FOTA
	 * @return None
	 */
	void LaunchFotaUpdate();
	
	private : 
	
	/**
	 * From MConnectionMonitorObserver
	 * Method involed by connection monitor with current event. 
	 * This method handles the events
	 * @param aConnMonEvent says about the event
	 * @return None
	 */
	void EventL( const CConnMonEventBase& aConnMonEvent ) ;
	
	/**
	 *CheckNetworkstatus and takes action based on that 
	 * @param none
	 * @return None
	 */
	void CheckNetworkStatusL();
	/**
	 * HandleAboveCriticallevel handle memory interrupt when it is above critical level 
	 * @param                   None
	 * @return                  None
	 */
	void HandleAboveCriticallevelL();
	
	/**
	 * HandleChargingInterruptL  handle charging interrupt when it is above critical level 
	 * @param                   None
	 * @return                  None
	 */
	
	void HandleChargingInterruptL();
	
	private:	  
	
	RConnectionMonitor iConnMon;
	RFs iFs;
	RProperty iProperty;
    TInt iConnStatus;	
    TInt iSize;
    TDriveNumber iDrive;
    TInt iBearer;
    TBool iWlanAvailable;
    TBool iGPRSAvail;
    TFmsIpcCommands iInterruptType;
    TFmsIpcCommands iUpdInterruptType;
    CFMSServer* iServer;
    RFotaEngineSession iFotaEngine;    
    CPeriodic* iPeriodicNwCheck;
    TBool iWcdma;
    
    /** 
     * RTelServer object used to keep an asynchronous request
     * for phone call end monitoring
     */ 
    RTelServer iTelServer;

    /** 
     * RPhone object used to keep an asynchronous request
     * for phone call end monitoring
     */
    RPhone     iPhone;
    
    /** 
     * RLine object used to keep an asynchronous request
     * for phone call end monitoring
     */
    RLine      iLine;
    
    /** 
     * To get hodl of current call status     
     */
    RCall::TStatus iCallStatus;

    TInt iChargeToMonitor;

	};
	

#endif
