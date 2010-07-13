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
* Description:  Base class in Commmon Agent
*
*/


#ifndef __NSMLAGENTBASE_H__
#define __NSMLAGENTBASE_H__

#include <e32base.h>
#include <SyncMLDef.h>
#include "WBXMLSyncMLParser.h"
#include "nsmlerror.h"
#include "nsmlagenttestdefines.h"

//DATA TYPES
enum TNSmlSyncInitiation    
	{		
	EClientInitiated = 1,
	EServerAlerted
	};

// FORWARD DECLARATIONS
class CSyncMLHistoryJob;
class CNSmlStatusContainer;
class CNSmlResponseController;
class CNSmlTransport;
class CNSmlCmdsBase;
class CNSmlURI;
class CNSmlFutureReservation;

// CLASS DECLARATION

/**
*  CNSmlAgentBase implements common parts (both for DS and DM)   
*  
*/
class CNSmlAgentBase:public CActive, public MWBXMLSyncMLCallbacks
	{
	public: // enumerations 
	enum TAuthenticationType
		{
		ENoAuth,
		EBasic,
		EMD5
		};

    public:  // Constructors and destructor
    IMPORT_C CNSmlAgentBase();            
    IMPORT_C virtual ~CNSmlAgentBase();

    public: 
	// New functions
	virtual TAuthenticationType AuthTypeL() const = 0;
	virtual void Interrupt( TInt aErrorCode, TBool aImmediatelyInterruption, TBool aStatusError ) = 0;
	virtual void InterruptL( TInt appIndex, TInt aErrorCode, TBool aImmediatelyInterruption, TBool aStatusError ) = 0;
	virtual void WriteWarningL( TNSmlError::TNSmlAgentErrorCode aWarningCode ) = 0;  
	virtual void WriteWarningL( TInt appIndex, TNSmlError::TNSmlAgentErrorCode aWarningCode ) = 0;
	virtual void WriteWarningL( TInt appIndex, TNSmlError::TNSmlSyncMLStatusCode aWarningCode ) = 0;
	virtual void CheckServerStatusCodeL( TInt aEntryID ) = 0;
	virtual TBool Interrupted() = 0;
	TBool AlreadyAuthenticated() const;  
	HBufC8* BasicCredentialL() const;
	HBufC8* Md5CredentialL( TBool aServerAuth = EFalse ) const;
	IMPORT_C virtual HBufC* ServerNonceAllocL() const;
	IMPORT_C virtual void SetServerNonceL( const TDesC& aNonce ) const;
	IMPORT_C void SetEndOfDataAlertRequest();
    IMPORT_C CSyncMLHistoryJob* SyncLog();
	IMPORT_C TBool CallbackCalled() const;
	IMPORT_C void SetOffCallbackCalled();
	IMPORT_C virtual void DoOutsideCancel();
	//Rnd_AutoRestart
	IMPORT_C virtual void LaunchAutoRestartL(TInt aError);
	void ReadAcessPointL();
	//Rnd_AutoRestart
	
	//
	// Profile ID getter
	//
	IMPORT_C TInt ProfileId() const;

	IMPORT_C TRequestStatus& StatusReference();
    
	public:
	// Functions from base classes
	IMPORT_C void RunL();										   // from CActive       
	IMPORT_C void DoCancel();									   // from CActive 
    IMPORT_C Ret_t smlAddCmdFuncL( SmlAddPtr_t aContent );          // from MWBXMLSyncMLCallbacks
   	IMPORT_C Ret_t smlCopyCmdFuncL( SmlCopyPtr_t aContent);         // from MWBXMLSyncMLCallbacks      
	IMPORT_C Ret_t smlDeleteCmdFuncL( SmlDeletePtr_t aContent );    // from MWBXMLSyncMLCallbacks
	IMPORT_C Ret_t smlExecCmdFuncL( SmlExecPtr_t aContent );        // from MWBXMLSyncMLCallbacks
	IMPORT_C Ret_t smlMapCmdFuncL( SmlMapPtr_t aContent );          // from MWBXMLSyncMLCallbacks
	IMPORT_C Ret_t smlReplaceCmdFuncL( SmlReplacePtr_t aContent );  // from MWBXMLSyncMLCallbacks
	IMPORT_C Ret_t smlSearchCmdFuncL( SmlSearchPtr_t aContent );    // from MWBXMLSyncMLCallbacks
	IMPORT_C Ret_t smlMoveCmdFuncL( SmlMovePtr_t aContent );		// from MWBXMLSyncMLCallbacks (1.2 changes)

    protected: // enumerations
	//
    // State coding enumerations
	//
	//main level state coding
	enum TMainState
		{
		EBeginning,			
		EClientInitialisation,
		EServerInitialisation,
// <MAPINFO_RESEND_MOD_BEGIN>
		EClientResendMapInfo,
// <MAPINFO_RESEND_MOD_END>
		EClientModifications,
		EResultAlert,
		EServerModifications,
		EDataUpdateStatus,
		EMapAcknowledge,
		EFinishing,
		EClientAlertNext,
		EServerAlertNext,
		ESending,
		EReceiving,
		EAlert,
		EBusy
		};
		
	//beginning level state coding
	enum TBeginningState
		{
		EBegin,
		EBeginConnectingToServer
		};

	//client initialisation level state coding
	enum TClientInitState
		{
		EInitStartingMessage,
		EInitMakingStatus,
		EInitAlerting,
		EInitDeviceInfo,
		EInitAskingDeviceInfo,
		EInitEndingMessage
		};

			
	//server modifications level state coding
	enum TServerModificationsState
		{
		EServerModWaitingStartMessage,
		EServerModWaitingCommands,
		EServerModWaitingUpdateCommands,
		EServerModMessageReceived,
		EServerModPackageReceived
		};

	//data update status level state coding
	enum TDataUpdateStatusState
		{
		EStatusStartingMessage,
		EStatusMakingAlert,
		EStatusMakingStatus,
		EStatusMakingMap,
		EStatusMakingResults,
		EStatusEndingMessage
		};

	//result alert from client level state coding
	enum TResultAlertState
		{
		EResultAlertStartingMessage,
		EResultAlertMakingAlert,
		EResultAlertEndingMessage
		};

	// client alerting for next state coding	
	enum TClientAlertNextState
		{
		EClientAlertNextStartingMessage,
		EClientAlertNextMakingAlert,
		EClientAlertNextMakingStatus,
		EClientAlertNextMakingMap,
		EClientAlertNextEndingMessage
		};
	// server alerting for next state coding	
	enum TServerAlertNextState
		{
		EServerAlertNextWaitingStartMessage,
		EServerAlertNextWaitingCommands,
		EServerAlertNextMessageReceived
		};
		
	protected:   //new functions
	//
	// Pure virtual state functions
	//
	virtual void NavigateMainStateL() = 0;
	virtual void NextMainStateL() = 0;
	virtual void InitialiseSubStates() = 0; 
    virtual void BeginStateL() = 0;
	virtual void InitStartingMessageStateL() = 0; 
	virtual void InitAlertingStateL() = 0;
	virtual void InitDeviceInfoStateL() = 0;
	virtual void ServerModUpdatesStateL( const TDesC8& aCmd, SmlGenericCmd_t* aContent ) =0;
	virtual void DataUpdateStatusStartingMessageStateL() = 0;
	//
	// Pure virtual Nonce read/update functions
	//
	virtual HBufC* NonceL() const = 0;
	virtual void SetNonceL( const TDesC& aNonce ) const = 0;
	//
	// Pure virtual Sync Log functions
	//
	virtual void FinalizeSyncLog() = 0;
	//
	virtual void CheckCommandsAreReceivedL() = 0;
	//
	// Pure virtual authentication type update
	//
	virtual void SetAuthTypeL( TAuthenticationType aAuthType ) const = 0;
	//
	// Pure virtual Resource Freeing 
	//
	virtual void FreeResources() = 0;
	//
	// Pure virtual Error Handling functions
	//
	virtual void FinaliseWhenErrorL() = 0;
	//
	//
	IMPORT_C void ConstructL();
	//
	// Beginning state functions 
	//
	IMPORT_C void BeginningStateL();
	//			
	// Client Initialisation state functions 
	//
	IMPORT_C void ClientInitialisationStateL();
	IMPORT_C void InitStartingMessageStateBaseL();
	IMPORT_C virtual void InitAskingDeviceInfoStateL();
	//
	// Server Modifications state functions 
	//
	IMPORT_C void ServerModStartMessageStateL( SmlSyncHdr_t* aSyncHdr );
	IMPORT_C void ServerModStatusCmdStateL( SmlStatus_t* aStatus );
	IMPORT_C void ServerModAlertCmdStateL( SmlAlert_t* aAlert );
	IMPORT_C void ServerModEndMessageStateL( TBool aFinal );
	//
	// Data Update Status state functions
	//
	IMPORT_C void DataUpdateStatusStateL();
	IMPORT_C void DataUpdateStatusStartingMessageStateBaseL();
	IMPORT_C virtual void DataUpdateStatusMakingMapStateL();
	IMPORT_C virtual void DataUpdateStatusMakingResultsStateL();
	//
	// Result Alert state functions 
	//
	IMPORT_C void ResultAlertStateL();
	//
	// Client Alert For Next state functions 
	//
	IMPORT_C void ClientAlertNextStateL();
	IMPORT_C virtual void ClientAlertNextMakingMapStateL();
	//
	// Server Alert For Next state functions 
	//
	IMPORT_C void ServerAlertNextStateL();
	IMPORT_C void ServerAlertNextStartMessageStateL( SmlSyncHdr_t* aSyncHdr );
	IMPORT_C void ServerAlertNextStatusCmdStateL( SmlStatus_t* aStatus );
	IMPORT_C void ServerAlertNextAlertCmdStateL( SmlAlert_t* aAlert );
	IMPORT_C void ServerAlertNextEndMessageStateL( TBool aFinal );
	//
	// Sending state functions 
	//
	IMPORT_C void SendingStateL();
	//
	// Receiving state functions 
	//
	IMPORT_C  void ReceivingStateL();
	//
	// Status code check				
	//
	IMPORT_C TBool CheckStatusCodesAreReceivedL( TBool aFinal = ETrue );
	//
	// Authentication functions
	//
	IMPORT_C void SaveIfNonceL( const CNSmlResponseController& aResponseController, TInt aEntryID ) const;
	IMPORT_C TBool AuthenticationRequirementL( const CNSmlResponseController& aResponseController, TInt aEntryID );
	IMPORT_C virtual TPtrC ServerUserName() const;
	IMPORT_C virtual TPtrC ServerPassword() const;

	//
	// Free resources
	//
	IMPORT_C void FreeBaseResources();
	
	//
	private: //functions
		
	CNSmlAgentBase( const CNSmlAgentBase& aOther );
	CNSmlAgentBase& operator=( const CNSmlAgentBase& aOther );
    //
	// Beginning state functions 
	//
	void BeginConnectingStateL();  
    //			
	// Client Initialisation state functions 
	//
    void InitMakingStatusStateL(); 
	void InitEndingMessageStateL(); 
    //
	// Data Update Status state functions
	//
	void DataUpdateStatusMakingAlertStateL();  
	void DataUpdateStatusMakingStatusStateL(); 
	void DataUpdateStatusEndingMessageStateL(); 
	//
	// Result Alert state functions 
	//
	void ResultAlertStartingMessageStateL();  
	void ResultAlertAlertingStateL();          
	void ResultAlertEndingMessageStateL();      
	//
	// Client Alert For Next state functions 
	//
	void ClientAlertNextStartingMessageStateL();  
	void ClientAlertNextAlertingStateL();         
	void ClientAlertNextMakingStatusStateL();     
	void ClientAlertNextEndingMessageStateL();    
	//
	// Transport functions
	//
	void ConnectIssueL();		
	void ConnectDoneL();		
	void SendDataIssueL();
	void SendDataDoneL();
	void ReceiveDataIssueL();
	void ReceiveDataDoneL();
	void Disconnect();
	//
	// Error Handling functions
	//
	void ErrorHandling( TInt aErrorCode );  
	
	public:     // Data
	
	//Rnd_AutoRestart
	TBool iPacketDataUnAvailable;
	TUint32 iNetmonAPId;
	TBool iAllowAutoRestart;
	//Rnd_AutoRestart
	
    protected:    // Data	
		
	//
    // State codes
	//
	TMainState iCurrMainState;
	TMainState iPrevSyncState;
	TBeginningState iCurrBeginningState;
	TClientInitState iCurrClientInitState;
	TServerModificationsState iCurrServerModState;
	TDataUpdateStatusState iCurrDataUpdateStatusState;
	TResultAlertState iCurrResultAlertState;
	TClientAlertNextState iCurrClientAlertNextState;
	TServerAlertNextState iCurrServerAlertNextState;
	//
	TRequestStatus* iCallerStatus;
	//	
	// IMEI code
	HBufC* iImeiCode;
	// Switch for autenthication
	TBool iAlreadyAuthenticated;
	// Switch for challenge request 
	TBool iChallengeRequest;
	// Switch for MD5 challenge request 
	TBool iMD5AlreadyRequested;
	// switch for asynchronous issues
	TBool iCommandIssued;
	// switch for checking SyncML version
	TBool iVersionIsChecked;
	// MaxMsgSize in a server 
	TInt iMaxMsgSizeInServer;
	// switch for Busy Status
	TBool iBusyStatusReceived;
	// switch for Result Alert
	TBool iResultAlertIssued;
	// switch for full workspace buffer
	TBool iBufferFull;
	// switch for final
	TBool iFinalMessageFromClient;
	// switch for Data Update Status package;
	TBool iStatusPackage;
	// switchs for end 
	TBool iEnd;
	TBool iCancelForced;
	
	// Common settings data
	TSmlProfileId iProfileID;
	HBufC* iSyncMLUserName;
	HBufC* iSyncMLPassword;

	HBufC* iSyncHTTPAuthUserName;	
	HBufC* iSyncHTTPAuthPassword;	
	TInt iSyncHTTPAuthUsed;			

	CNSmlURI* iSyncServer;
	TUid iMediumType;
	TInt iIAPId;
    CArrayFixFlat<TUint32>* iIAPIdArray;

	// Sync Initiation 
	TNSmlSyncInitiation iSyncInitiation;
	// SyncML MIME type;
	HBufC8* iSyncMLMIMEType;
	// Result Alert code
	HBufC8* iResultAlertCode;
	// End of Data Alert code
	HBufC8* iEndOfDataAlertCode;
    // Session Abort Alert code (DM Only)
	HBufC8* iSessionAbortAlertCode;
	// Next Message Alert code
	HBufC8* iNextMessageCode;
	// Switch for end of data alert
	TBool iEndOfDataAlertRequest;
	// Switch for session abort alert (DM Only)
	TBool iSessionAbortAlertRequest; 
	// Counter for nested Atomics
	TInt iNestedAtomicsCount;
    // 
	TBool iCallbackCalled;
	//
	// Used classes
	//
	CSyncMLHistoryJob* iSyncLog;
	//
	// Owned classes
	//
	CNSmlCmdsBase* iSyncMLCmds;
	TNSmlError* iError;
	//
	// Owned classes
	//
	CNSmlTransport* iTransport; 

	private:    // Data
	// Reserved to maintain binary compability
	CNSmlFutureReservation* iReserved;

	TPtr8 iBufferArea;
	
	TInt iSendRetries;
	// for module test
#ifdef __NOTRANSPORT
	TInt iTestCounter;
#endif
	};

#endif // __NSMLAGENTBASE_H__
            
// End of File
