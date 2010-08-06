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
* Description:  SyncML DM command handling 
*
*/



#ifndef __NSMLDMAGENT_H
#define __NSMLDMAGENT_H

// INCLUDES
#include <e32base.h>
#include <devicedialogsymbian.h>
#include <DevManInternalCRKeys.h>
#include "NSmlAgentBase.h"
#include "NSmlPrivateAPI.h"

// FORWARD DECLARATIONS
class MSyncMLProgressObserver;
class CNSmlDmModule;
class CSyncMLHistoryJob;
class CNSmlURI;
// FOTA
class CNSmlDMGenericAlert;
// FOTA end
class CNSmlAgentNotifierObserver;


// CLASS DECLARATION

/**
*  CNSmlDMAgent implements DM session flow logic   
*  
*/
class CNSmlDMAgent:public CNSmlAgentBase
	{
    public:  // Constructors and destructor
        
    static CNSmlDMAgent* NewL( MSyncMLProgressObserver* aObserver = NULL );
    virtual ~CNSmlDMAgent();       //from CBase

    public: // New functions
     
    virtual void Configure( TRequestStatus& aStatus, TInt aSettingsID,TInt aTransportId, TNSmlSyncInitiation aSyncInitiation=EClientInitiated, TInt aIapId=KErrNotFound );
	// FOTA
	/**
	* Appends a Generic Alert to the internal list. 
	* The alert itself is sent in the later phase of the dm session.
	* @param aFwMgmtUri. The uri which has been used in the exe command whose final result is 
	* about to be reported.
	* @param aMetaType. Meta/Type that should be used in the alert.
	* @param aMetaFormat. Meta/Format that should be used in the alert.
	* @param aFinalResult. The final result value which is reported to remote server.
	* @param aCorrelator. Correlator value used in the original exec command.
	*/
	virtual void SetGenericAlertL ( const TDesC8& aFwMgmtUri, const TDesC8& aMetaType, const TDesC8& aMetaFormat, TInt aFinalResult, const TDesC8& aFwCorrelator );
	// FOTA end
		virtual void SetGenericAlertL ( const TDesC8& aCorrelator, const RArray<CNSmlDMAlertItem>& aItemList  );
	
	private: // new functions
	
	CNSmlDMAgent();
	CNSmlDMAgent( const CNSmlDMAgent& aOther );
	CNSmlDMAgent& operator=( const CNSmlDMAgent& aOther );
    void ConstructL( MSyncMLProgressObserver* aObserver );
	TBool SupportedAlert( const TDesC8& aAlertCode ) const;
	void ServerModificationsStateL();   
	void ServerModGetCmdStateL( SmlGet_t* aContent );
	void ServerModStartAtomicStateL( SmlAtomic_t* aContent );
	void ServerModEndAtomicStateL();
	void ServerModStartSequenceStateL( SmlSequence_t* aContent );
	void ServerModEndSequenceStateL();
	// Finishing state functions 
	void FinishingStateL();                       
	//  Trigger message parsing 
	// Read SyncML DM settings
	void ReadSettingsL();                                
	//  Sync Log Initiation 
	void InitSyncLogL();  
	// Read/Update session ID
	TInt GetSessionIDL();                                 
	void UpdateSessionIDL( TInt aSessionIDL );      
	// Finish
	void FinishL();      
	// 
	TBool InterruptedImmediately();                                  
	//

	private: // Functions from base classes
	//
	// Parser callbacks 
	//
	Ret_t smlAlertCmdFuncL( SmlAlertPtr_t aContent);          //from MWBXMLSyncMLCallbacks   
	Ret_t smlExecCmdFuncL( SmlExecPtr_t aContent );
	Ret_t smlCopyCmdFuncL( SmlCopyPtr_t aContent );
	Ret_t smlGetCmdFuncL( SmlGetPtr_t aContent );             //from MWBXMLSyncMLCallbacks            
	Ret_t smlPutCmdFuncL( SmlPutPtr_t aContent );             //from MWBXMLSyncMLCallbacks  
	Ret_t smlResultsCmdFuncL( SmlResultsPtr_t aContent );     //from MWBXMLSyncMLCallbacks 
	Ret_t smlStatusCmdFuncL( SmlStatusPtr_t aContent );       //from MWBXMLSyncMLCallbacks   
	Ret_t smlStartMessageFuncL( SmlSyncHdrPtr_t aContent );   //from MWBXMLSyncMLCallbacks   
	Ret_t smlEndMessageFuncL( Boolean_t final );              //from MWBXMLSyncMLCallbacks
	Ret_t smlStartSyncFuncL( SmlSyncPtr_t aContent );         //from MWBXMLSyncMLCallbacks 
	Ret_t smlEndSyncFuncL();                                  //from MWBXMLSyncMLCallbacks
	Ret_t smlStartAtomicFuncL( SmlAtomicPtr_t aContent );     //from MWBXMLSyncMLCallbacks
	Ret_t smlEndAtomicFuncL();                                //from MWBXMLSyncMLCallbacks 
	Ret_t smlStartSequenceFuncL( SmlSequencePtr_t aContent ); //from MWBXMLSyncMLCallbacks
	Ret_t smlEndSequenceFuncL();                              //from MWBXMLSyncMLCallbacks		
	//
	// Main state functions 
	//
	void NavigateMainStateL();     //from CNSmlAgentBase
	void NextMainStateL();          //from CNSmlAgentBase
	void InitialiseSubStates();    //from CNSmlAgentBase
	//
	// Beginning state functions 
	//
	void BeginStateL();             //from CNSmlAgentBase
	//				
	// Client Initialisation state functions 
	//
	void InitStartingMessageStateL();  //from CNSmlAgentBase
	void InitAlertingStateL();         //from CNSmlAgentBase
	void InitDeviceInfoStateL();       //from CNSmlAgentBase
	
	//
	// Server Modifications state functions 
	//
	void ServerModUpdatesStateL( const TDesC8& aCmd, SmlGenericCmd_t* aContent ); //from CNSmlAgentBase
	// FOTA
	/**
	* Issues a process request for the arrived exec command.
	* @param aContent. The exec command structure, accordant with the dtd.
	*/		
	void ServerModUpdatesStateL( SmlExec_t* aContent );
	// FOTA end	
	
	//
	// Data Update Status state functions
	//
	void DataUpdateStatusStartingMessageStateL(); //from CNSmlAgentBase  
    void DataUpdateStatusMakingResultsStateL();   //from CNSmlAgentBase
	//
	// Nonce functions
	//
	HBufC* NonceL() const;                               //from CNSmlAgentBase  
	void SetNonceL( const TDesC& aNonce ) const;         //from CNSmlAgentBase
	// Server authentication functions                   
	TPtrC ServerUserName() const;                        //from CNSmlAgentBase  
	TPtrC ServerPassword() const;                        //from CNSmlAgentBase  
	HBufC* ServerNonceAllocL()const;                     //from CNSmlAgentBase   
	void SetServerNonceL( const TDesC& aNonce ) const;   //from CNSmlAgentBase  
	//
	// Sync Log functions
	//
	void FinalizeSyncLog();                               //from CNSmlAgentBase 
	void FinalizeSyncLogL();                               
	
		//
	void CreateSyncmlCmdsInstanceL();					  //from CNSmlAgentBase
	//
	void CheckCommandsAreReceivedL();                     //from CNSmlAgentBase 
	//
	// Authentication type 
	//
	void SetAuthTypeL( CNSmlAgentBase::TAuthenticationType aAuthType ) const;    //from CNSmlAgentBase 
	CNSmlAgentBase::TAuthenticationType AuthTypeL() const; //from CNSmlAgentBase
	
	TBool CheckCertificateL();		
	void UserConfirmateConnectionL();
	void CheckForcedCertificateCheckStateL();
	
	//
	// Free resources
	//
	void FreeResources();                                 //from CNSmlAgentBase
	
	//
	// Error Handling functions
	//
	void FinaliseWhenErrorL();                            //from CNSmlAgentBase 
	TBool Interrupted();                                  //from CNSmlAgentBase  
	void Interrupt( TInt aErrorCode, TBool aImmediatelyInterruption, TBool aStatusError ); //from CNSmlAgentBase
	void InterruptL( TInt appIndex, TInt aErrorCode, TBool aImmediatelyInterruption, TBool aStatusError ); //from CNSmlAgentBase
	void WriteWarningL( TNSmlError::TNSmlAgentErrorCode aWarningCode );  //from CNSmlAgentBase
	void WriteWarningL( TInt appIndex, TNSmlError::TNSmlAgentErrorCode aWarningCode ); //from CNSmlAgentBase
	void WriteWarningL( TInt appIndex, TNSmlError::TNSmlSyncMLStatusCode aWarningCode ); //from CNSmlAgentBase
	void CheckServerStatusCodeL( TInt aEntryID );         //from CNSmlAgentBase

	public:
	
	
	void SaveProfileInfoL();
	
	
    private:    // Data	
	TBool iForcedCertificateCheck;
	CNSmlAgentNotifierObserver * iNotifierObserver;		
	HBufC8* iDMTriggerMessage;
	HBufC* iDMServerId;
	HBufC* iDMServerPassword;
	TInt iDMServerSessionId;
	HBufC8* iDMServerDigest;
	TBool iDMInterrupted;
	TBool iDMImmediatelyInterruption;
	// FOTA
	RPointerArray<CNSmlDMGenericAlert> iGenAlerts;
	TBool iResetGenAlerts;
	TBool iResetUserInitAlert;
	// FOTA end	
	//
	// Used classes
	//
	MSyncMLProgressObserver* iDMObserver;
	//
	// Owned classes
	//
    //IapId sent by sosserver if client wants this iap for this dm session
	TInt iNewIapId;
	};
	
	
IMPORT_C CNSmlDMAgent* CreateDMAgentL( MSyncMLProgressObserver* aDMObserver );
typedef CNSmlDMAgent* (*TNSmlCreateDMAgentFunc) ( MSyncMLProgressObserver* aDMObserver );

/**
* CNSmlNotifierTimeOut class
* 
*/

class CNSmlAgentNotifierTimeOut : public CActive
	{
	public:
		CNSmlAgentNotifierTimeOut();
		~CNSmlAgentNotifierTimeOut();
		void LaunchNotifierTimer( CNSmlAgentNotifierObserver*  aObserver );
	protected:
		void DoCancel();
		void RunL();
	private:
		CNSmlAgentNotifierObserver* iObserver;
		RTimer iTimeOutTimer;
	};
	
/**
* CNSmlNotifierObserver class
* 
*/
class CNSmlAgentNotifierObserver : public CActive
	{
	public:
		CNSmlAgentNotifierObserver( TRequestStatus& aStatus, TInt aCreatorId );
		~CNSmlAgentNotifierObserver();
		void ConnectToNotifierL( CNSmlAgentBase * aNSmlAgentBase);
		void NotifierTimeOut();
	protected:
		void DoCancel();
		void RunL();
	private:
		TRequestStatus& iCallerStatus;
		

	
		TInt& iCreatorId;
		CNSmlAgentNotifierTimeOut iNotifierTimeOut;
		TBool iTimeOut;
		CNSmlAgentBase * iNSmlAgentBase;
	};
	

#endif      // __NSMLDMAGENT_H
            
// End of File
