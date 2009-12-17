/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description: Monitors the network for availability and starts any pending data sync sessions. 
*
*/

#ifndef C_NETMON_H
#define C_NETMON_H

// INCLUDE FILES
#include <e32base.h>
#include <rconnmon.h>
#include <nifvar.h>
#include <SyncMLClient.h>
#include <SyncMLClientDS.h>
#include <SyncMLObservers.h>
#include "NSmlDSAgent.h"
#include "NSmlAgentBase.h"

#include <e32property.h>
#include <DataSyncInternalPSKeys.h>

// USER INCLUDES
#include "NsmlProfileUtil.h"

// NetMon states
enum TNetMonStates { ERegister = 0, 
                     EComplete };

//Forward class declaration
class CNSmlDSAgent;


/** 
 */
class CNsmlDSNetmon : public CActive,
                public MConnectionMonitorObserver
{
    public:
    
		/**
		 * Two phase construction
		 */
		static CNsmlDSNetmon* NewL(CNSmlDSAgent& aDSAgent);
		
		/**
		 * Destructor
		 */
		~CNsmlDSNetmon();   

    protected: 

		// From base class CActive

		/**
		 * Implements cancellation of an outstanding request
		 */
		void DoCancel();

		/**
		 * Handles an active objects request completion event.
		 */
		void RunL();

		/**
		 * If the RunL function leaves, then the active scheduler call
		 * RunError() to handle the leave.
		 * @param aError - The error code
		 */
		TInt RunError ( TInt aError );
		
	public: //New functions
		
		TBool LaunchAutoRestartL();	
		
		TBool IsRoaming();
		void StartTimerL();
		void StopTimer();

	public:
	    /**
	     * ProfileId
	     */
	    TSmlProfileId iProfileID;
	    
    private:
    
		/**
		 * Constructor
		 */
		CNsmlDSNetmon();
		
		/**
		 * Second phase constructor
		 */
		void ConstructL(CNSmlDSAgent& aDSAgent);

		/**
		 * Resgiter for events
		 */
		void RegisterL();

		/** 
		 * Close the server
		 */
		void Complete();

		/**
		 * Set active
		 */
		void IssueRequest();
		
		// From base class MConnectionMonitorObserver
        void EventL ( const CConnMonEventBase& aConnMonEvent );
        
        /**
		 * Read the Global timeout from cenrep.
		 */	
		TInt ReadTimeoutFromCenrepL();
		
    private:
    
        /**
         *Instance of the DSAgent
         */
         CNSmlDSAgent* iDSAgent;


		/**
		 * Connection Monitor Server Instance
		 */
		RConnectionMonitor iConnectionMonitor;
	   
		/**
		 * States
		 */
		TNetMonStates iState;
		
		/**
		 * Time @ which the packet data is resumed
		 */
		TTime iPacketDataAvailableTime;
		
		/**
		 * Time @ which the packet data is lost
		 */
		TTime iPacketDataUnAvailableTime;
		
		/**
		 * Auto Restart Timer
		 */
		CPeriodic* iAutoRestartTimer;
		
		/**
		 * Roaming Status
		 */
		TInt iRoamingStatus;
	
};

#endif // End of file
