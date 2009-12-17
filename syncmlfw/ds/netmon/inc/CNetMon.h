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
* Description:  Monitors the network for availability and starts any pending
*                data sync sessions.
*
*/


#ifndef C_NETMON_H
#define C_NETMON_H

// INCLUDE FILES
#include <e32base.h>
#include <rconnmon.h>
#include <SyncMLClient.h>
#include <SyncMLClientDS.h>
#include <SyncMLObservers.h>
#include <e32property.h>
#include <centralrepository.h>
#include <DataSyncInternalPSKeys.h>

// USER INCLUDES
#include "nsmldebug.h"
#include "NsmlProfileUtil.h"

// NetMon states
enum TNetMonStates { ERegister = 0, 
                     EMonitor, 
                     ENetworkUp1, 
                     ENetworkUp2,
                     ESyncOk,
                     EInitSync,
                     EComplete };

/** 
 * Monitors the network for availability and starts any pending 
 * data sync sessions.
 */
class CNetMon : public CActive,
                public MConnectionMonitorObserver,
                public MSyncMLEventObserver,
                public MSyncMLProgressObserver
{
    public:
    
		/**
		 * Two phase construction
		 */
		static CNetMon* NewL();
		
		/**
		 * Destructor
		 */
		~CNetMon();   

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
	    
    private:
    
		/**
		 * Constructor
		 */
		CNetMon();
		
		/**
		 * Second phase constructor
		 */
		void ConstructL();

		/**
		 * Resgiter for events
		 */
		void RegisterL();

		/**
		 * Trigger sync
		 */
		void StartSyncL();

		/** 
		 * Close the server
		 */
		void Complete();

		/**
		 * Set active
		 */
		void IssueRequest();
		
		/**
		 * Initiates stability check timer
		 */
		//void InitTimerL();
		
		/**
		 * Cancels stability check timer
		 */
		//void CancelTimer();
		
	    /**
		 * Cancels global auto restart timer
		 */
		void CancelGlobalTimer();
		
		/**
		 * Checks if packet data is available using conmon
		 */
		TBool PacketDataAvailable();
		
		/**
		 * Moves the state to NetworkUp2 since it is stable
		 */
		void NetworkStable();
		
		/**
		 * Check if network is already up
		 */
		void NetworkUp1();
		
	    /**
		 * Check if network is really up. Callback method
		 */
		static TInt NetworkUp2(TAny* arg);
		
		/**
		 * Check if any other sync is in progress
		 */
		void OtherSyncStatus();
		
		/**
		 * This functions checks if all auto restart policies are ok
		 */
		TBool AutoStartPolicyOkL();		 
		
		/**
		 * Global timeout for auto restart has expired.
		 */
		static TInt Timeout(TAny* arg);                		
				
		/**
		 * Read the Global timeout from cenrep.
		 */	
		TInt ReadTimeoutFromCenrepL();
		
		/**
		 * Create the Sync Job.
		 */	
		void CreateSyncJobL();
		
		/**
		 * From base class MConnectionMonitorObserver
		 */
        void EventL ( const CConnMonEventBase& aConnMonEvent );
 
        /**
         * From base class MSyncMLEventObserver
         */
        void OnSyncMLSessionEvent(TEvent aEvent, 
                                  TInt aIdentifier, 
                                  TInt aError, 
                                  TInt aAdditionalData); 
        
        //from MSyncMLProgressObserver
	    
	    /**
	    * Receives notification of a synchronisation error.
        * @param aErrorLevel  The error level.
	    * @param aError		  The type of error. This is one of the SyncMLError error values.	
	    * @param aTaskId      The ID of the task for which the error occurred.
	    * @param aInfo1       An integer that can contain additional information about the error. Normally 0.
	    * @param aInfo2       An integer that can contain additional information about the error. Normally 0.
	    * @return             None.
	    */
	    void OnSyncMLSyncError(TErrorLevel aErrorLevel, TInt aError, TInt aTaskId, TInt aInfo1, TInt aInfo2);
       	
       	/**
	    * Receives notification of synchronisation progress.
	    * @param aStatus	The current status, e.g. 'Connecting'.
	    * @param aInfo1	An integer that can contain additional information about the progress.
	    * @param aInfo2	An integer that can contain additional information about the progress.
	    */
	    void OnSyncMLSyncProgress(TStatus aStatus, TInt aInfo1, TInt aInfo2);
	    
	    /**
	    * Receives notification of modifications to synchronisation tasks.
        * @param aTaskId               The ID of the task.
	    * @param aClientModifications  Modifications made on the clients Data Store.
	    * @param aServerModifications  Modifications made on the server Data Store.
	    */
	    void OnSyncMLDataSyncModifications(TInt aTaskId, const TSyncMLDataSyncModifications& aClientModifications, const TSyncMLDataSyncModifications& aServerModifications);
                          
		                                  
        /**
         * Set/Reset the interrupt flag in the CenRep
         */	
		void UpdateInterruptFlagL(TInt aValue);                                  	
		void OpenSyncSessionL();
		
		void CloseSyncSession();
		
    private:

		/**
		 * Connection Monitor Server Instance
		 */
		RConnectionMonitor iConnectionMonitor;
	   
		/**
		 * States
		 */
		TNetMonStates iState;
		
		/**
		 * SyncML session
		 */
		RSyncMLSession iSyncSession;
		
		/**
		 * Data sync job
		 */
		RSyncMLDataSyncJob iSyncJob;
		
		/**
		 * Stability Timer
		 */
		CPeriodic* iStabilityTimer;
		
	    /**
		 * Global Timer
		 */
		CPeriodic* iGlobalTimer;
		
		/**
		 * Profile id to be used
		 */
		TSmlProfileId iLastUsedProfileId;	
		
		/**
		 * Sync status P&S keys
		 */	
		RProperty iProperty;
	
		/**
		 * Connection Id used for the Sync Session
		 */
		TConnMonBearerId iConMonBearerId;
		/**
		 * Global timeout from cenrep.
		 */	
		TInt iGlobalTimeout;
		
		/**
		 * Timer count
		 */	
		TInt iTimerCount;

		TBool iSyncSessionOpen;
		
		TStatus iPrgPrevStatus;
		
		TStatus iPrgCurStatus;
};

#endif // End of file
