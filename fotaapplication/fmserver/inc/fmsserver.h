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

#ifndef __FMS_SERVER_H__
#define __FMS_SERVER_H__

#include <e32base.h>
#include <e32std.h>
#include <e32property.h>
#include <flogger.h>
#include <rconnmon.h>
#include <f32file.h>
#include <fotaengine.h>
#include "fmsclientserver.h"
#include "fmsinterruptaob.h"
#include "fmsdebug.h"

_LIT( KFotaInterruptFileName, "fotainterrupt.txt" );
const TInt64 KThresholdDiskSpace = 305152;
const TInt64 KDiskSpaceNotCL = 102400;
const TInt KRequestTriggerWaitTime(1000000);

class CFMSSession;
class CFMSServer;
class CFMSInterruptAob;

	
class CFMSServer: public CServer2
	{
	friend class CFMSSession;
	friend class CFMSInterruptAob;
	
	public:
	
	/**
	 * Destructor
	 */ 
	virtual ~CFMSServer();
	
	/**
	 * Static method which creates the server instance
	 * @param None
	 * @return CServer2*
	 */ 
	static CServer2*  NewLC();
	
	/**
	 * Stops the session and closes the server
	 * @param None
	 * @return None
	 */
	void DropSession();
	
	/**
	 * Starts Active object to monitor for the interrupt
	 * @param aType, Interrupt reason
	 * @return None
	 */
	void StartMonitoringL(TFmsIpcCommands aType);

	/**
	 * Starts Active object to monitor for the update interrupt
	 * @param aType, Interrupt reason
	 * @param aLevel, Level of the battery charge to monitor
	 * @return None
	 */
	void StartBatteryMonitoringL(TFmsIpcCommands aType, TUint aLevel);

/**
	 * Starts Active object to monitor for the update interrupt
	 * @param aType, Interrupt reason
	 * @return None
	 */
	void StartUpdateInterruptMonitoringL(TFmsIpcCommands aType);
	/**
	 * Calls Active object's method to set the drive & size 
	 * for memory interrupt
	 * @param aSize, Interrupt aDrive
	 * @return None
	 */
	void MemoryToMonitorL(TInt aSize, TDriveNumber aDrive);
	
	/**
	 * Calls Active object's method to set the network bearer
	 * @param aBearer
	 * @return None
	 */
	void NetworkTypeL(TInt aBearer);
	
	/**
	 * Writes the file with all the interrupt information
	 * @param aReason ,Interrupt reason
	 * @param aBearer ,Bearer id
	 * @param aDrive  ,Drive to Download
	 * @param aSize   , memory size to monitor
	 * @param aWcdmaBearer , 3G bearer or not
	 * @return None
	 */
    void WriteToFile(TInt aReason, TInt aBearer, TDriveNumber aDrive, 
    		TInt aSize, TBool aWcdmaBearer );
    
    /**
	 * Reads the file for the interrupt information
	 * @param aReason ,Interrupt reason
	 * @param aBearer ,Bearer id
	 * @param aDrive  ,Drive to Download
	 * @param aSize   , memory size to monitor
	 * @param aWcdmaBearer , 3G bearer or not
	 * @return TBool, says reading success or not
	 */
    TBool ReadFromFile(TInt& aReason, TInt& aBearer, TInt& aDrive, TInt& aSize
    		 , TInt& aWcdmaBearer );
    
    /**
	 * Checks for any interrupt pending
	 * @param None
	 * @return TBool
	 */
    TBool RequestPending();
    
    /**
	 * Checks the network as part of other interrupts
	 * before triggering fota.
	 * In case of network down, it keeps network interrupt
	 * @param aBearer ,Bearer id
	 * @param aDrive  ,Drive to Download
	 * @param aSize   , memory size to monitor
	 * @param aWcdmaBearer , 3G bearer or not
	 * @return None
	 */
    void CheckNetworkL(TInt& aBearerId,TInt& aDrive, TInt& aSize,TBool& aWcdma);
    
    /**
	 * Locks the session, so that new session will not be started
	 * before the previous interrupt monitory completion
	 * @param None
	 * @return None
	 */
    void LockSession();
    
    /**
	 * Computes the variation configured
	 * @param None
	 * @return None
	 */
    void FindVariation();
    
    /**
	 * Checks Charger monitoring supported or not
	 * @param None
	 * @return TBool, which says Charger monitory support
	 */
    TBool ChargerTobeMonitered();
    
    /**
	 * Checks  Network monitoring supported or not
	 * @param None
	 * @return TBool, which says  Network monitory support
	 */
    TBool NetworkTobeMonitered();
    
    /**
	 * Checks Phone startup monitoring supported or not
	 * @param None
	 * @return TBool, which says Phone startup monitory support
	 */
    TBool MoniterAfterPhoneRestart();
    
    /**
	 * Checks Memory monitoring supported or not
	 * @param None
	 * @return TBool, which says Memory monitory support
	 */
    TBool MemoryTobeMonitered();
    
    /**
	 * Checks for WLAN
	 * @param None
	 * @return TBool, which says WLAN availability
	 */
    TBool CheckWlanL();
    
    /**
	 * Sets the Bool value which decides file deletion
	 * @param aValue, says whether the file to be deletd or not
	 * @return None
	 */
    void DeleteFile(TBool aValue);
    
    /**
	 * Checks network and trigger fota engine in hard reboot case
	 * @param None
	 * @return None
	 */
    void TriggerFotaL();
    
    /**
	 * For asynchronous network request to be 
	 * logged on active object, using CPeriodic
	 * @param None
	 * @return None
	 */
	void AsyncSessionRequestL();
		
	/**
	 * Destroys the CPeriodic object
	 * So that not to call the static method again
	 * @param None
	 * @return None
	 */
	void StopAsyncRequest();
	
	/**
     * Calls Active objects SetWcdma to set the bearer to 3G
     * @param None
     * @return None
     */
   	void SetWcdma() ;
   	
   	/**
   	 * Checks whether a phone call is active or not
   	 * @param aStatus, on return contains the call status
   	 * @return None
   	 */
   	void CheckPhoneCallActiveL(TInt& aStatus);
   	
   	/**
   	 * Starts montioring for active phone call end
   	 * @param aPkgId, package Id of FOTA Update
   	 * @param aProfileId, Profile ID of FOTA
   	 * @return phone call active or not at that moment
   	 */
   	TBool MonitorPhoneCallEndL();
   	
   	/**
   	 * Logs Asynchronous montioring for active phone call end
   	 * To avoid performance issues(like thread blocking call)
   	 * @param None
   	 * @return None
   	 */
   	void LogAsyncCallMonitorL();
   	
   	void CreateScheduledReminderL();
   	
   	void DeleteScheduledRemindersL();
   	
	private:
	
	/**
	 * second phase constructor
	 */
	void ConstructL();
	
	/**
	 * Default constructor
	 */
	CFMSServer();
	
	protected:
	
	/**
	 * Calls when a New session is getting created
	 * @param aVersion
	 * @param aMessage
	 * @return CSession2*
	 */
	CSession2* NewSessionL(const TVersion& aVersion,const RMessage2& aMessage)  const;
	
	private:
	RFs iFs; //for file creation, writing & Deleting
	RFile iFile;
	TInt iSessionCount;	
	CFMSInterruptAob iFMSInterruptAob;
    TBool iNetworkMon;
    TBool iChargerMon;
    TBool iPhoneRestartMon;
    TBool iMemoryMon;	
    TBool iDeleteFile;
    CPeriodic* iLogAsyncRequest;
	};
	
#endif
