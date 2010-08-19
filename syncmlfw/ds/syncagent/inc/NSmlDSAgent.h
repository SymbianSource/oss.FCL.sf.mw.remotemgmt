/*
* Copyright (c) 2004 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  DS Sync flow
*
*/


#ifndef __NSMLDSAGENT_H__
#define __NSMLDSAGENT_H__

// INCLUDES
#include <e32base.h>
#include <SyncMLObservers.h>
#include "nsmldbcaps.h"
#include "nsmldsdefines.h"
#include "NSmlAgentBase.h"
#include "nsmldsagconstants.h"

#include "nsmlagentlog.h"
//RD_AUTO_RESTART
#include "nsmldsnetmon.h"
#include <rconnmon.h>
// FORWARD DECLARATIONS
class MSyncMLProgressObserver;
class CNSmlDSContent;
//RD_AUTO_RESTART
class CNsmlDSNetmon;
class CNSmlDSProfile;
class CNSmlDSSettings;
class CRepository;
class CNSmlDSOperatorSettings;
// CLASS DECLARATION

/**
* OMA DS protocol engine.
*
* @lib nsmldsagent.lib
*/
class CNSmlDSAgent : public CNSmlAgentBase
	{
    public: // constructors and destructor
    	/**
    	* Symbian two-phased constructor.
    	* @param aObserver Observer callback through which a sync client is informed.
    	*/
    	static CNSmlDSAgent* NewL( MSyncMLProgressObserver* aObserver );
		  	
		/**
		* Destructor.
		*/
    	virtual ~CNSmlDSAgent();

    public: // new functions
	    /**
	    * Start the synchronisation process with a remote server.
	    * @param aStatus Asynchronous call result code.
	    * @param aProfileId ID of the used profile.
	    * @param aConnectionId ID of the used medium.
	    * @param aTypeArray Used to specify content specific synchronisation types.
	    * @param aSyncInitiation Specifies whether the session is client or server initiated.
	    * @param aSessionId Used session id.
	    * @param aAlertPackage SyncML 1.1 server alert package.
	    */
		virtual void Synchronise( TRequestStatus& aStatus,
		                      TSmlProfileId aProfileId,
		                      TSmlConnectionId aConnectionId = -1,
							  CArrayFix<TNSmlContentSpecificSyncType>* aTypeArray = NULL,
							  TNSmlSyncInitiation aSyncInitiation = EClientInitiated,
							  TInt aSessionId = 0,
							  HBufC8* aAlertPackage = NULL );
		//RD_AUTO_RESTART
		/** 
		 * Launches the Auto-restart exe if the Sync is interrupted
		 * because of Network failures
		 * Before launching the Netmon.exe,Profile details will be stored
		 * into Cenrep
		 */
		void LaunchAutoRestartL(TInt aError);           //from CNSmlAgentBase							  
		TBool IsSyncClientInitiated();
		//RD_AUTO_RESTART	
    
    public: // enumerations     
    
	//  Status when synchronisation is completed (normally or abnormally) 
	enum TNSmlAgentSyncStatus
		{
		ECompletedOK = 0,
		ECompletedWithInfo,
		ENothingIsSent,
		ENoServerDataUpdated,
		EServerDataPartiallyUpdated
		};
	
    private: // enumerations
		
	//
    // DS specific state coding enumerations, common enums with DM
	// are specified in CNSmlAgentBase
	// 

	// sub-states under begin state
	enum TBeginSubState
		{
		EBeginSubStart,
		EBeginSubReadSettings,
		EBeginSubSyncmlCmdsInstance,
		EBeginSubServerAlerting,
		EBeginSubOpenContentInstances,
// <MAPINFO_RESEND_MOD_BEGIN>
		EBeginSubScanMapInfo,
// <MAPINFO_RESEND_MOD_END>
		EBeginSubCreateLUIDBuffers
		};
		
	//server alerting level state coding
	enum TServerAlertState
		{
		EServerAlertWaitingStartMessage,
		EServerAlertWaitingAlerts,
		EServerAlertMessageReceived
		};

	// server initialisation level state coding
	enum TServerInitState
		{
		EServerInitWaitingStartMessage,
		EServerInitWaitingCommands,
		EServerInitMessageReceived,
		EServerInitPackageReceived,
		EServerInitEnd
		};

// <MAPINFO_RESEND_MOD_BEGIN>
	// Client resend map info level state coding
	enum TClientResendMapInfoState
		{
		EResendMapInfoStartingMessage,
		EResendMapInfoMakingAlert,
		EResendMapInfoMakingStatus,
		EResendMapInfoMakingResults,
		EResendMapInfoStartingSync,
		EResendMapInfoEndingSync,
		EResendMapInfoMakingMap,
		EResendMapInfoEndingMessage
		};


	void ClientResendMapInfoStartingMessageStateL();
	void ClientResendMapInfoMakingAlertStateL();
	void ClientResendMapInfoMakingStatusStateL();
	void ClientResendMapInfoMakingMapStateL();
	void ClientResendMapInfoMakingResultsStateL();
	void ClientResendMapInfoStartingSyncStateL();
	void ClientResendMapInfoEndingSyncStateL();
	void ClientResendMapInfoEndingMessageStateL();
	
// <MAPINFO_RESEND_MOD_END>

	// client modifications level state coding
	enum TClientModificationsState
		{
		EClientModStartingMessage,
		EClientModMakingStatus,
		EClientModMakingResults,
		EClientModStartingSync,
		EClientModMakingUpdates,
		EClientModEndingSync,
		EClientModEndingMessage
		};
		
	// map acknowledge level state coding
	enum TMapAcknowledgeState
		{
		EMapAckWaitingStartMessage,
		EMapAckWaitingCommands,
		EMapAckMessageReceived,
		EMapAckPackageReceived
		};

	private: // constructors & operators
		/**
		* Standard C++ constructor.
		*/
		CNSmlDSAgent();
		
		/**
		* Symbian 2nd phase constructor.
    	* @param aObserver Observer callback through which the sync client is informed.
		*/	
    	void ConstructL( MSyncMLProgressObserver* aObserver );
    	
		/**
		* Prohibit copy constructor.
		*/
		CNSmlDSAgent( const CNSmlDSAgent& aOther );
		
		/**
		* Prohibit assignment operator.
		*/
		CNSmlDSAgent& operator=( const CNSmlDSAgent& aOther );
		
	//
	// Begin sub-state functions 
	//
	void BeginSubStartStateL();
	void BeginSubReadSettingsStateL();
	void BeginSubSyncmlCmdsInstanceStateL();
	void BeginSubServerAlertingStateL();
	void BeginSubOpenContentInstancesStateL();
	void BeginSubCreateLUIDBuffersStateL();	
	
// <MAPINFO_RESEND_MOD_BEGIN>
	void BeginSubScanMapInfo();
// <MAPINFO_RESEND_MOD_END>
	
    //
	// Server Alerting state functions 
	//
	void ServerAlertStateL();
	void ServerAlertStartMessageStateL( SmlSyncHdr_t* aSyncHdr );
	void ServerAlertAlertCmdStateL( SmlAlert_t* aAlert );
	void ServerAlertEndMessageStateL( TBool aFinal );
	//
	// Server Initiliasation state functions 
	//
	void ServerInitialisationStateL();
	void ServerInitStartMessageStateL( SmlSyncHdr_t* aSyncHdr );
	void ServerInitStatusCmdStateL( SmlStatus_t* aStatus );
	void ServerInitResultsCmdStateL( SmlResults_t* aResults );
	void ServerInitAlertCmdStateL( SmlAlert_t* aAlert );
	void ServerInitPutCmdStateL( SmlPut_t* aPut );
	void ServerInitGetCmdStateL( SmlGet_t* aGet );
	void ServerInitEndMessageStateL( TBool aFinal );
	//
	// Client Modifications state functions 
	//
	void ClientModificationsStateL();
	void ClientModStartingMessageStateL();
	void ClientModMakingStatusStateL();
	void ClientModMakingResultsStateL();	
	void ClientModStartingSyncStateL();
	void ClientModMakingUpdatesStateL();
	void ClientModEndingSyncStateL();
	void ClientModEndingMessageStateL();
// <MAPINFO_RESEND_MOD_BEGIN>
	void ClientResendMapInfoStateL();
// <MAPINFO_RESEND_MOD_END>
	
	//
	// Server Modifications state functions 
	//
	void ServerModificationsStateL();
	void ServerModStartSyncStateL( SmlSync_t* aSync );
	void ServerModEndSyncStateL();
	void ServerModStartAtomicStateL( SmlAtomic_t* aContent );
	void ServerModEndAtomicStateL();
	//
	// Map Acknowledgement state functions
	//
	void MapAcknowledgeStateL();
	void MapAckStartMessageStateL( SmlSyncHdr_t* aSyncHdr );
	void MapAckStatusCmdStateL( SmlStatus_t* aStatus );
	void MapAckAlertCmdStateL( SmlAlert_t* aAlert );
	void MapAckEndMessageStateL( TBool aFinal );
	//
	// Finishing state functions 
	//
	void FinishingStateL();                                      
	//  Open Database related instances
	void OpenDatabaseInstancesL( TBool& aEnd );
	
// <MAPINFO_RESEND_MOD_BEGIN>
	//Scans for MapInfo and loads if exists
	void ScanMapInfoL(TBool &aEnd);
// <MAPINFO_RESEND_MOD_END>
	
	//
	void CreateLUIDBuffersL( TBool& aEnd );
	//  Create SyncML commands instance
	void CreateSyncmlCmdsInstanceL();
	//
	// Sync Log functions
	//
	void InfoMessageToSyncLogL( TInt appIndex, TInt aErrorCode );
	//  Read SyncML DS settings
	void ReadSettingsL(); 
	// Session ID get/update
	TInt GetSessionIDL();                                        
	void UpdateSessionIDL( TInt aSessionID );   
	// Finishing 
	void FinishL(); 
	
	//Launches the Netmon exe
	void LaunchNETMONL();
	//Saves the profile-info to cenrep
	void SaveProfileInfoL();
	//Stops the current ongoing session
	void StopDSSession();
	TBool CheckProfileIdL();
	void TerminateAutoRestart();
	void ReplaceIAPIdL();
	//RD_AUTO_RESTART
	private: // functions from base classes 
		/**
		* From CActive. Cancels the currently ongoing asynchronous operation.
		*/
		void DoCancel();

    	/**
    	* From MWBXMLSyncMLCallbacks.
    	* @param aContent Alert command structure.
    	* @return Parser API return code.
    	*/
		Ret_t smlAlertCmdFuncL( SmlAlertPtr_t aContent);
		
    	/**
    	* From MWBXMLSyncMLCallbacks.
    	* @param aContent Get command structure.
    	* @return Parser API return code.
    	*/
		Ret_t smlGetCmdFuncL( SmlGetPtr_t aContent );
		
    	/**
    	* From MWBXMLSyncMLCallbacks.
    	* @param aContent Put command structure.
    	* @return Parser API return code.
    	*/
		Ret_t smlPutCmdFuncL( SmlPutPtr_t aContent );
		
    	/**
    	* From MWBXMLSyncMLCallbacks.
    	* @param aContent Results command structure.
    	* @return Parser API return code.
    	*/
		Ret_t smlResultsCmdFuncL( SmlResultsPtr_t aContent );
		
    	/**
    	* From MWBXMLSyncMLCallbacks.
    	* @param aContent Status command structure.
    	* @return Parser API return code.
    	*/
		Ret_t smlStatusCmdFuncL( SmlStatusPtr_t aContent );
		
    	/**
    	* From MWBXMLSyncMLCallbacks.
    	* @param aContent Sync header structure.
    	* @return Parser API return code.
    	*/
		Ret_t smlStartMessageFuncL( SmlSyncHdrPtr_t aContent );
		
    	/**
    	* From MWBXMLSyncMLCallbacks.
    	* @param final Last part of the command (yes/no).
    	* @return Parser API return code.
    	*/
		Ret_t smlEndMessageFuncL( Boolean_t final );
		
    	/**
    	* From MWBXMLSyncMLCallbacks.
    	* @param aContent Sync command structure.
    	* @return Parser API return code.
    	*/
		Ret_t smlStartSyncFuncL( SmlSyncPtr_t aContent );
		
    	/**
    	* From MWBXMLSyncMLCallbacks.
    	* @return Parser API return code.
    	*/
		Ret_t smlEndSyncFuncL();
		
    	/**
    	* From MWBXMLSyncMLCallbacks.
    	* @param aContent Atomic command structure.
    	* @return Parser API return code.
    	*/
		Ret_t smlStartAtomicFuncL( SmlAtomicPtr_t aContent );
		
    	/**
    	* From MWBXMLSyncMLCallbacks.
    	* @return Parser API return code.
    	*/
		Ret_t smlEndAtomicFuncL();
		
    	/**
    	* From MWBXMLSyncMLCallbacks.
    	* @param aContent Sequence command structure.
    	* @return Parser API return code.
    	*/
		Ret_t smlStartSequenceFuncL( SmlSequencePtr_t aContent );
		
    	/**
    	* From MWBXMLSyncMLCallbacks.
    	* @return Parser API return code.
    	*/
		Ret_t smlEndSequenceFuncL();
		
    	/**
    	* From MWBXMLSyncMLCallbacks.
    	* @param aContent Move command structure.
    	* @return Parser API return code.
    	*/
		Ret_t smlMoveCmdFuncL( SmlMovePtr_t aContent );
	
	//
    // Main state functions 
	//
		/**
		* From CNSmlAgentBase. Calls the correct main state function depending
		* on the current state.
		*/
		void NavigateMainStateL();
		
		/**
		* From CNSmlAgentBase. Advances the state machine to the next main state.
		* The next state depends on the current and (possibly) previous state.
		*/
		void NextMainStateL();
		
	void InitialiseSubStates();  //from CNSmlAgentBase
	//
	// Beginning state functions 
	//
	void BeginStateL();        //from CNSmlAgentBase
				
	// Client Initialisation state functions 
	//
	void InitStartingMessageStateL();  //from CNSmlAgentBase
	void InitAlertingStateL();         //from CNSmlAgentBase
	void InitDeviceInfoStateL();       //from CNSmlAgentBase  
	void InitAskingDeviceInfoStateL(); //from CNSmlAgentBase
	//
	// Server Modifications state functions 
	//
	void ServerModUpdatesStateL( const TDesC8& aCmd, SmlGenericCmd_t* aContent ); //from CNSmlAgentBase
	//
	// Data Update Status state functions
	//
	void DataUpdateStatusStartingMessageStateL();                //from CNSmlAgentBase 
	void DataUpdateStatusMakingMapStateL();                      //from CNSmlAgentBase
	//
	// Client Alert For Next states functions
	//
	void ClientAlertNextMakingMapStateL();                       //from CNSmlAgentBase
	///
	// Agent Log functions
	//
	HBufC* NonceL() const;                                        //from CNSmlAgentBase
	void SetNonceL( const TDesC& aNonce ) const;                  //from CNSmlAgentBase  
	//
	// Sync Log functions
	//
	void FinalizeSyncLog();                                       //from CNSmlAgentBase
	void FinalizeSyncLogL();

	//
	// Synchronisation Log functions
	void WriteWarningL( TNSmlError::TNSmlAgentErrorCode aWarningCode );    //from CNSmlAgentBase
	void WriteWarningL( TInt appIndex, TNSmlError::TNSmlAgentErrorCode aWarningCode ); //from CNSmlAgentBase
	void WriteWarningL( TInt appIndex, TNSmlError::TNSmlSyncMLStatusCode aWarningCode ); //from CNSmlAgentBase
		
	void CheckServerStatusCodeL( TInt aEntryID );                //from CNSmlAgentBase
	void CheckCommandsAreReceivedL();							 //from CNSmlAgentBase
	//
	// Authentication functions
	//
	CNSmlAgentBase::TAuthenticationType AuthTypeL() const;            //from CNSmlAgentBase 
	void SetAuthTypeL( CNSmlAgentBase::TAuthenticationType aAuthType ) const; //from CNSmlAgentBase
	//
	// Free resources
	//
	void FreeResources(); //from CNSmlAgentBase
	
	//
	// Error Handling functions
	//
	void Interrupt( TInt aErrorCode, TBool aImmediatelyInterruption, TBool aStatusError ); //from CNSmlAgentBase 
	void InterruptL( TInt appIndex, TInt aErrorCode, TBool aImmediatelyInterruption, TBool aStatusError ); //from CNSmlAgentBase 
	void FinaliseWhenErrorL();          //from CNSmlAgentBase 
	TBool Interrupted();                //from CNSmlAgentBase  

	void PrepareSyncL( CArrayFix<TNSmlContentSpecificSyncType>* aTypeArray );
	void DoErrorEvent( MSyncMLProgressObserver::TErrorLevel aErrorLevel,
	                   TInt aError, TInt aTaskId, TInt aInfo1, TInt aInfo2 );
	
	
	//RD_SUSPEND_RESUME
	//stores the suspendedsync status
	TNSmlSyncSuspendedState CurrentState( );
	TNSmlSyncSuspendedState CurrentStateInFinishing();
	TNSmlSyncSuspendedState CurrentStateInReceiving();
	TNSmlSyncSuspendedState CurrentStateInClientAlertNext();
	TNSmlSyncSuspendedState CurrentStateInServerAlertNext();
	
	TBool SyncFailure();
	void StoreServerStatusCode( TInt aServerStatusCode ) const;
	public:
	/**
	* Mapping of the Alert code is done
	*/
	static TNSmlPreviousSyncType MapAlertCode(TDes8& aAlertCode);
	static TPtrC8 MapAlertCode(TNSmlPreviousSyncType aAlertCode);
	
	/**
	* Reads the Cenrep Key for the Suspend & Resume feature
	* The feature will be enabled if the aKey value is set to 1
	*/
	
	static void ReadRepositoryL(TInt aKey, TInt& aValue);
	//RD_SUSPEND_RESUME
	
	/**
	* Publishes and resets sync session details
	*/	
	void SetDSSessionInfoL(CNSmlDSProfile* aProfile);
	void ResetDSSessionInfoL();
	
    private: // data	
	//
    // DS specific state codes
	//
	TBeginSubState iDSCurrBeginSubState; 
	TServerInitState iDSCurrServerInitState;
	
// <MAPINFO_RESEND_MOD_BEGIN>
	TClientResendMapInfoState iCurrClientResendMapInfoState;
	TBool iMapResendStatusPackage;
	TBool iDSMapOpeningStarted;
// <MAPINFO_RESEND_MOD_END>
	
	TServerAlertState iDSCurrServerAlertState;
	TClientModificationsState iDSCurrClientModState;
	TMapAcknowledgeState iDSCurrMapAcknowledgeState;
	// Sync Type (given for all contents);
	CArrayFix<TNSmlContentSpecificSyncType>* iDSSyncTypeArray;
	// Client's current Sync Anchor 
	TTime iDSClientNextSyncAnchor;
	TBool iDSDeviceInfoRequestByClient;
	TBool iDSDeviceInfoRequestedByServer;
	// switch for Map Acknowledgement
	TBool iDSWaitingForMapAcknowledgement;
	// 
	TBool iDSDatabaseOpeningStarted;
	//
	TBool iDSLUIDBufferingStarted;
	HBufC* iDSServerId;

	// DS protocol version
	TSmlProtocolVersion iProtocolVer;

	// progress observer
	MSyncMLProgressObserver* iDSObserver;
	
	
	// session id
	TInt iSessionId;
	// 1.1 server alert package
	HBufC8* iServerAlertPackage;
	
	//
	// Owned classes
	//
	CNSmlDSContent* iDSContent;
	TBool iErrorNotified;
     //RD_AUTO_RESTART
	CNsmlDSNetmon *iDSNetmon;
	TBool iAutoRestartInitiatedSync;
	//RD_SUSPEND_RESUME
	TBool iUpdateLastAnchor;
	//For storing Server Status Code
	TBool iErrorReportingEnabled;
	CRepository* iRepositorySSC;
	RArray< TInt > iServerStatusCodeArray;
	};
	
typedef CNSmlDSAgent* (*TNSmlCreateDSAgentFunc) ( MSyncMLProgressObserver* aObserver );

/**
* Creates new instance of the CNSmlDSAgent class.
* @param aObserver Progress callback observer.
* @return The newly created instance.
*/
IMPORT_C CNSmlDSAgent* CreateDSAgentL( MSyncMLProgressObserver* aObserver );
	
#endif // __NSMLDSAGENT_H__
            
// End of File
