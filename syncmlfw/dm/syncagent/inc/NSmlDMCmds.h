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


#ifndef __NSMLDMCMDS_H
#define __NSMLDMCMDS_H

// INCLUDES
#include <e32base.h>
// FOTA
#include <centralrepository.h>
// FOTA end
#include <nsmldmmodule.h>
#include "NSmlCmdsBase.h"
#include "nsmlagenttestdefines.h"
#ifdef __TEST_TREEMODULE
#include "nsmldmtestmodule.h"
#endif


// FORWARD DECLARATIONS
#ifdef __TEST_TREEMODULE
class CNSmlDmTestModule;
#else
class CNSmlDmModule;
#endif
class MSyncMLProgressObserver;


// CLASS DECLARATION

/**
*  CNSmlDMCmds implements DM specific SyncML commands handling  
*  
*/
#ifdef __TEST_TREEMODULE
class CNSmlDMCmds : public CNSmlCmdsBase, public MNSmlDmTestModuleCallBack
#else
class CNSmlDMCmds : public CNSmlCmdsBase, public MNSmlDmModuleCallBack
#endif
	{
    public:  // Constructors and destructor
    
	static CNSmlDMCmds* NewL( CNSmlAgentBase* aAgent, const TDesC8& aSessionID, const TDesC8& aVerProto, const HBufC& aSyncMLUserName, CNSmlURI* aSyncServer, const TDesC& aDMServerId, MSyncMLProgressObserver* aDMObserver );	
    ~CNSmlDMCmds();       //from CBase
 	
	private: //new functions

	void ConstructL( CNSmlAgentBase* aAgent, const TDesC8& aSessionID, const TDesC8& aVerProto, const HBufC& aSyncMLUserName, CNSmlURI* aSyncServer, const TDesC& aDMServerId, MSyncMLProgressObserver* aDMObserver );
		
	CNSmlDMCmds();
	CNSmlDMCmds( const CNSmlCmdsBase& aOther );
	CNSmlDMCmds& operator=( const CNSmlDMCmds& aOther );
		
	void DoDevInfoItemsL( const TDesC8& aURISegment ); 
	void FreeDMDevinfo();
	TInt AlertParameter ( const SmlPcdata_t* aData, const TDesC8& aParamID ) const;
//#ifdef RD_DM_TEXT_INPUT_ALERT //For User Input Server alert
//#endif
	//singlechoice
	void HandleAlertsL( SmlAlert_t* aAlert, TInt& aStatusId);
	void HandleAlertErrorL();
	void HandleConfirmationAlertL( SmlAlert_t* aAlert, TInt& aStatusId);
	void HandleDisplayAlertL( SmlAlert_t* aAlert, TInt& aStatusId);
	//singlechoice
	HBufC8* AlertDataLC( const SmlItemList_t* aItemList ) const;
	TInt InitStatusToAtomicOrSequenceL( const TDesC8& aCmd, const SmlAtomic_t* aAtomic ) const;

	private: // functions from base classes
    	
	void DoAlertL( const TDesC8& aAlertCode, TTime* aLastSyncAnchor = NULL, TTime* aNextSyncAnchor = NULL ); //from CNSmlCmdsBase
	void DoPutL();      //from CNSmlCmdsBase
	CNSmlCmdsBase::TReturnValue DoResultsL();  //from CNSmlCmdsBase
	void DoGetL();      //from CNSmlCmdsBase
	CNSmlCmdsBase::TReturnValue DoStartSyncL(); 
	void DoEndSyncL();
	CNSmlCmdsBase::TReturnValue DoAddOrReplaceOrDeleteL(); //from CNSmlCmdsBase
	CNSmlCmdsBase::TReturnValue DoMapL();                  //from CNSmlCmdsBase
	TReturnValue DoReplaceOrDeleteL();                     //from CNSmlCmdsBase
	void ProcessResultsCmdL( SmlResults_t* aResults );     //from CNSmlCmdsBase
	void ProcessPutCmdL( SmlPut_t* aPut );                 //from CNSmlCmdsBase
	void ProcessGetCmdL( SmlGet_t* aGet );                 //from CNSmlCmdsBase
	void ProcessAlertCmdL( SmlAlert_t* aAlert, TBool aNextAlert = EFalse, TBool aServerAlert = EFalse, TBool aDisplayAlert = EFalse ); //from CNSmlCmdsBase	
	void ProcessSyncL( SmlSync_t* aSync );             //from CNSmlCmdsBase
	void ProcessUpdatesL( const TDesC8& aCmd, SmlGenericCmd_t* aContent ); //from CNSmlCmdsBase
	void ProcessAtomicL( SmlAtomic_t* aAtomic );                 //from CNSmlCmdsBase
	void ProcessEndAtomicL();                                    //from CNSmlCmdsBase
	void ProcessSequenceL( SmlSequence_t* aSequence );           //from CNSmlCmdsBase
	void ProcessEndSequence();                                   //from CNSmlCmdsBase 
	void ProcessEndSyncL();	 	                                 //from CNSmlCmdsBase 
	// FOTA
	/**
	* Process the exec structure sent by the remote server.
	* @param aExec. The exec command structure, accordant with the dtd. 
	*/		
	void ProcessExecCmdL ( SmlExec_t* aExec );					 //from CNSmlCmdsBase
	/**
	* Makes a generic alert element (or possible many of them) 
	* and calls WBXML generator.
	* @param aFwMgmtUri. The uri which has been used in the exe command whose final result is 
	* about to be reported.
	* @param aMetaType. Meta/Type that should be used in the alert.
	* @param aMetaFormat. Meta/Format that should be used in the alert.
	* @param aFinalResult. The final result value which is reported to remote server.
	* @param aCorrelator. Correlator value used in the original exec command.
	*/		
	void DoGenericAlertL ( const TDesC8& aFwMgmtUri, const TDesC8& aMetaType, const TDesC8& aMetaFormat, TInt aFinalResult, const TDesC8& aFwCorrelator ); //from CNSmlCmdsBase					
	void DoGenericAlertL ( const TDesC8& aCorrelator, const RArray<CNSmlDMAlertItem>& aItemList ); //from CNSmlCmdsBase					
	/**
	* Makes a generic user alert element, if the update request is set to the
	* central repository by the FOTA UI.
	* @param aProfileId. The profile id of the current dm session. 
	* @return TBool. ETrue if the alert was generated, otherwise EFalse.
	*/		
	TBool DoGenericUserAlertL ( TInt aProfileId );	 			//from CNSmlCmdsBase
	/**
	* Resets the update request in the central repository by
	* writing '-1' value.
	*/		
	void ResetGenericUserAlertL();	 							//from CNSmlCmdsBase
	/**
	* When the generic alerts are successfully sent to the remote 
	* server, the FOTA adapter needs to be informed about this.
	* This command is chained through the DM engine.
	*/		
	void MarkGenAlertsSentL();	 								//from CNSmlCmdsBase
	void MarkGenAlertsSentL(const TDesC8& aURI) ;
	/**
	* Closes the connections to callback server and host servers.
	* Separate function is needed, since the disconnecting cannot 
	* be made after the active scheduler of the thread is stopped.
	*/		
	void DisconnectFromOtherServers();							//from CNSmlCmdsBase	
	// FOTA end	
	void DoEndMessageL( TBool aFinal );                          //from CNSmlCmdsBase
	void EndOfServerMessageL() const;                            //from CNSmlCmdsBase  

	void SetResultsL( TInt aResultsRef, const CBufBase& aObject, const TDesC8& aType, const TDesC8& aFormat, TInt aTotalSize); //from MNSmlDmModuleCallBack
	void SetStatusL( TInt aStatusRef, TInt aStatusCode ); //from MNSmlDmModuleCallBack

	TNSmlError::TNSmlSyncMLStatusCode GetChunkL(TDesC8& aLargeUri, const SmlItem_t* aCurrentItem, const SmlPcdata_t* aMetaInCommand, CBufBase*& aBuffer, TInt& aTotSizeOfLarge );
	
	/**
	* Updates the error code for the session,if any
	* @param aItem: pointer to sml elements
	* @param aStatusCode: holds the error code
	* @return aNone
	*/
	void UpdateErrorStatusCode(SmlItem_t* aItem,TNSmlError::TNSmlSyncMLStatusCode& aStatusCode);
	
	
	TBool IsHbSyncmlNotifierEnabledL();
	void ServerHbNotifierL(TSyncMLDlgNoteTypes& aNotetype, TDesC& aServerMsg);
	
	//
	private:     // Data
	struct TDMDevInfo
		{
		SmlItemList_t** iItemListPtr;
		HBufC8* iFormat;
		HBufC8* iType;
		HBufC8* iObject;
		TBool iFirst;
		TBool iResults;
		};

	// DM server ID	
	HBufC* iDMServerId;
	// Atomic transaction is on.
	TBool iDMAtomic;
	// ID of Atomic command
	TInt iDMAtomicID;
	// Sequence is on
	TBool iDMSequence;
    // ID of Sequence
	TInt iDMSequenceID; 
	// Atomic or Sequence is interrupted by an user
	TBool iDMNotExecuted;
	// Struct for DevInfo item
	TDMDevInfo* iDMDevInfo;
	// Switch for Callback results handling
	TBool iDMDevInfoResults;
	// Switch for Out of Disk message 
	TBool iDMDeviceFullWritten;
	// used classes
	MSyncMLProgressObserver* iDMObserver;
	
	HBufC8* iLargeObjectUri;
//	TBool iMoreDataForResults;
	TInt iLargeObjectTotalSize;
	TInt iBytesSent;
	
	// FOTA 
	CRepository* iRepository;
	// FOTA end
	
	// owned classes
#ifdef __TEST_TREEMODULE
    CNSmlDmTestModule* iDMModule; 
#else
	CNSmlDmModule* iDMModule;
#endif
	
	    /**
	    * Handle to chunk that is used to transfer data between client and server
	    */
	    RChunk iChunk;  
	 
	};

#endif      // __NSMLDMCMDS_H
            
// End of File
