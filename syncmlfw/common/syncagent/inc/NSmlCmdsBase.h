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
* Description:  Common Agent SyncML command handling
*
*/


#ifndef __NSMLCMDSBASE_H
#define __NSMLCMDSBASE_H

// INCLUDES
#include <e32base.h>
#include <s32mem.h>

#include "smlmetinfdtd.h"
#include "nsmlerror.h"
#include "nsmlagenttestdefines.h"
#include "smlsyncmltags.h"
#include "NSmlPrivateAPI.h"

// FORWARD DECLARATIONS
class CNSmlAgentBase;
class CNSmlURI;   
class CNSmlStatusContainer;
class CNSmlResponseController;
class CNSmlResultsContainer;
class CWBXMLSyncMLGenerator;
class CWBXMLParser;
class CWBXMLSyncMLDocHandler;
class CNSmlPhoneInfo;
class CNSmlAuth;
class CNSmlFutureReservation;

// CLASS DECLARATION

/**
*  CNSmlCmdsBase implements common parts (for DS and DM) of SyncML commands handling  
*  
*/
class CNSmlCmdsBase:public CBase
	{
    public:  // Constructors and destructor
    
	IMPORT_C CNSmlCmdsBase();	
    IMPORT_C virtual ~CNSmlCmdsBase();

	public: //enumerations
	enum TReturnValue
		{
		EReturnOK = 0,
        EReturnNotFound,
		EReturnSkipped,
		EReturnEndData,
		EReturnBufferFull
		};

    public: // new functions
	virtual void DoAlertL( const TDesC8& aAlertCode, TTime* aLastSyncAnchor = NULL, TTime* aNextSyncAnchor = NULL ) = 0;
    virtual void DoPutL() = 0;
	virtual TReturnValue DoResultsL() = 0;
	virtual void DoGetL() = 0;
	virtual TReturnValue DoStartSyncL() = 0;
	virtual void DoEndSyncL() = 0;
	virtual TReturnValue DoAddOrReplaceOrDeleteL() = 0;
	virtual TReturnValue DoMapL() = 0;
	virtual void DoEndMessageL( TBool aFinal = ETrue ) = 0;
	virtual void ProcessResultsCmdL( SmlResults_t* aResults ) = 0;
	virtual void ProcessPutCmdL( SmlPut_t* aPut ) = 0;
	virtual void ProcessGetCmdL( SmlGet_t* aGet ) = 0;
	virtual void ProcessAlertCmdL( SmlAlert_t* aAlert, TBool aNextAlert = EFalse, TBool aServerAlert = EFalse, TBool aDisplayAlert = EFalse ) = 0;
	virtual void ProcessSyncL( SmlSync_t* aSync ) = 0;
	virtual void ProcessEndSyncL() = 0;
	virtual void ProcessUpdatesL( const TDesC8& aCmd, SmlGenericCmd_t* aContent ) = 0;
	virtual void ProcessAtomicL( SmlAtomic_t* aAtomic ) = 0;
	virtual void ProcessEndAtomicL() = 0;
	virtual void ProcessSequenceL( SmlSequence_t* aSequence) = 0;
	virtual void ProcessEndSequence() = 0;
	IMPORT_C virtual void ProcessCopyCmdL(SmlCopy_t* aCopy);
	IMPORT_C virtual void ProcessExecCmdL(SmlExec_t* aExec);
	IMPORT_C virtual void ProcessMapCmdL(SmlMap_t* aContent);
	IMPORT_C virtual void ProcessSearchCmdL(SmlSearch_t* aSearch);
	IMPORT_C virtual void FetchIDMappingTableL(const TInt aType);
	IMPORT_C virtual void CloseIDMappingTableL();
	IMPORT_C virtual TBool DeleteIDMappingTableL(const TDesC& aId, const TDesC& aDatabase);

	IMPORT_C virtual void EndOfServerMessageL() const;
	IMPORT_C virtual TInt WriteMappingInfoToDbL() const;
	
	IMPORT_C virtual void DoSyncHdrL();
	IMPORT_C TReturnValue DoStatusL( SmlStatus_t* aStatus );
	// FOTA
	IMPORT_C virtual void DoGenericAlertL (  const TDesC8& aFwMgmtUri, const TDesC8& aMetaType, const TDesC8& aMetaFormat, TInt aFinalResult, const TDesC8& aFwCorrelator );
	IMPORT_C virtual TBool DoGenericUserAlertL ( TInt aProfileId );
	IMPORT_C virtual void ResetGenericUserAlertL();
	IMPORT_C virtual void MarkGenAlertsSentL();
	IMPORT_C virtual void MarkGenAlertsSentL(const TDesC8& aURI);
	IMPORT_C virtual void DisconnectFromOtherServers();
	// FOTA end
	IMPORT_C TBool ProcessReceivedDataL();
	IMPORT_C virtual void ProcessSyncHdrL ( SmlSyncHdr_t* aSyncHdr );
	IMPORT_C virtual void ProcessStatusCmdL( SmlStatus_t* aStatus );
	IMPORT_C TPtr8 AlertCode( SmlAlert_t* aAlert ) const;
	IMPORT_C void StatusToUnsupportedCommandL( TBool aIllegal, const TDesC8& aCmd, const SmlPcdata_t* aCmdRef, Flag_t aFlags = 0 );
	TPtrC8 GeneratedDocument() const;
	IMPORT_C TPtr8 BufferAreaForParsingL();
	IMPORT_C TBool BusyStatus() const;
	IMPORT_C CNSmlStatusContainer* StatusContainer() const;
	IMPORT_C CNSmlResponseController* ResponseController() const;
	IMPORT_C CNSmlResultsContainer* ResultsContainer() const;
	IMPORT_C CNSmlAuth* ServerAuth() const;
	IMPORT_C CNSmlURI* RespURI() const;
	IMPORT_C void DoMetaL( SmlPcdata_t*& aMeta, const SmlMetInfMetInf_t* aMetaData ) const;
	IMPORT_C void DoMetInfLC( SmlMetInfMetInf_t*& aMeta ) const;   
	IMPORT_C void PcdataNewL( SmlPcdata_t*& aPcdata, const TDesC8& aContent ) const;
	IMPORT_C void DoSourceL( sml_source_s*& aSource, const TDesC& aLocURIData ) const; 
	IMPORT_C void DoSourceL( sml_source_s*& aSource, const TDesC8& aLocURIData ) const; 
	IMPORT_C void DoSourceL( sml_source_s*& aSource, const TDesC& aLocURIData, const TDesC& aLocNameData ) const;
	IMPORT_C void DoTargetL( sml_target_s*& aTarget, const TDesC& aLocURIData ) const; 
	IMPORT_C void DoTargetL( sml_target_s*& aTarget, const TDesC8& aLocURIData ) const; 
	IMPORT_C void DoTargetL( sml_target_s*& aTarget, const TDesC& aLocURIData, const TDesC& aLocNameData ) const;
	IMPORT_C void PCDATAToUnicodeLC( const SmlPcdata_t& aPcdata, HBufC*& aUnicodeData ) const;
	IMPORT_C void ResetLargeObjectBuffer();	        
    IMPORT_C TBool WaitingLargeObjectChunk() const;
	IMPORT_C void SetMaximumWorkspaceSize( const TInt aMaxSize );
        IMPORT_C virtual void DoGenericAlertL (  const TDesC8& aCorrelator, const RArray<CNSmlDMAlertItem>& aData );

	protected: //new functions
	IMPORT_C void ConstructL( CNSmlAgentBase* aAgentBase, const TDesC8& aSessionID, const TDesC8& aVerProto, const TInt aPublicId, const HBufC& aSyncMLUserName, CNSmlURI* aSyncServer, TBool aVersionCheck = ETrue );
	IMPORT_C void ConstructL( CNSmlAgentBase* aAgentBase, const TDesC8& aSessionID, const TDesC8& aVerProto, const HBufC& aSyncMLUserName, CNSmlURI* aSyncServer, TBool aVersionCheck = ETrue );

	IMPORT_C void DoCmdIdL( SmlPcdata_t*& aCmdID);
	IMPORT_C void DoAnchorsL( SmlMetInfMetInf_t& aMetInf, const TDesC8* aLast, const TDesC8* aNext ) const;
	IMPORT_C void DoMaxObjSizeL( SmlMetInfMetInf_t& aMetInf, TInt aMaxObjSize ) const;
	
	IMPORT_C void SubtractConsumedFromPcData( SmlItem_t*& aItem, TInt aConsumed );
	IMPORT_C TInt StatusDataToGenericCommandL( const TDesC8& aCmd, const SmlGenericCmd_t* aContent, const SmlItem_t* aItem, TNSmlError::TNSmlSyncMLStatusCode aStatusCode, const TDesC8* aStatusItem = NULL );
	IMPORT_C TInt StatusDataToGetCommandL( const SmlGet_t* aContent, const SmlItem_t* aItem, TNSmlError::TNSmlSyncMLStatusCode aStatusCode, const TDesC8* aStatusItem = NULL );
	IMPORT_C TInt StatusDataToCommandL( const TDesC8& aCmd, const SmlPcdata_t* aCmdID, TUint aFlags, const SmlItem_t* aItem, TNSmlError::TNSmlSyncMLStatusCode aStatusCode, const TDesC8* aStatusItem = NULL );
	IMPORT_C TInt CreateAndInitResponseItemL( const TDesC8& aCommand );
	IMPORT_C TBool TargetIsUnderItem( const SmlItemList_t* aItemList ) const; 
	IMPORT_C TBool SourceIsUnderItem( const SmlItemList_t* aItemList ) const;
	IMPORT_C HBufC8* MetaTypeInUtf8LC( const SmlPcdata_t* aMeta ) const;
	IMPORT_C HBufC* MetaTypeLC( const SmlPcdata_t* aMeta ) const;
	IMPORT_C HBufC8* MetaFormatInUtf8LC( const SmlPcdata_t* aMeta ) const;
	IMPORT_C TBool IsFlagSet( const TUint& aValue, const TUint& aBit ) const;
	IMPORT_C void TrimRightSpaceAndNull (TDes8& aDes ) const;
	IMPORT_C TNSmlError::TNSmlSyncMLStatusCode MoreDataL( const HBufC8* aUID, const SmlItem_t* aCurrentItem, const SmlPcdata_t* aMetaInCommand, CBufBase*& aTotalItem );
	IMPORT_C TInt ServerMaxObjSize( const SmlPcdata_t* aMeta ) const;
	IMPORT_C TInt ServerObjectSize( const SmlPcdata_t* aMeta ) const;
	
	private: //functions
		
	CNSmlCmdsBase( const CNSmlCmdsBase& aOther );
	CNSmlCmdsBase& operator=( const CNSmlCmdsBase& aOther );

	//
	public:     // Data
#if defined (__NOTRANSPORT) || defined (_DEBUG)
	TPtr8 BufferAreaForDebugDumpL();
#endif
	
    protected:    // Data	
	HBufC8* iVerDTD;
	TInt iVersionPublicId;

	// protocol version
	HBufC8* iVerProto;
	// Current SyncML SessionID  
	HBufC8* iSessionID;
	// Current SyncML MsgID of a client
	TInt iCurrMsgID;
	// Current SyncML CmdID of a client
	TInt iCurrCmdID;
	// Current SyncML MsgID of a server
	HBufC8* iCurrServerMsgID;
	// No Response Status to the message sent by Server
	TBool iStatusToServerNoResponse;
	// Target/LocURI element which client has sent to server
	CNSmlURI* iSentSyncHdrTarget;
	// Source/LocURI element which client has sent to server
	CNSmlURI* iSentSyncHdrSource;
	// URI for response from a server
	CNSmlURI* iRespURI;
	// switches for checking SyncML version
	TBool iVersionCheck;
	TBool iVersionIsChecked;
	// workspace size
	TInt iWorkspaceSize;
	// maximum workspace size
	TInt iMaxWorkspaceSize;
	// bufferArea
	HBufC8* iBufferArea;
	// read stream;
	RDesReadStream iReadStream;
	// switch for authentication (client)
	TBool iAlreadyAuthenticated;
	// switch for Busy Status
	TBool iBusyStatusReceived;
	// switch for Result Alert
	TBool iResultAlertIssued;
	// switch for Data Update Status package;
	TBool iStatusPackage;
	// Indicates invalid device
	TBool iUnknownDevice;
	// User Name
	HBufC* iSyncMLUserName;
	// SyncML server
	CNSmlURI* iSyncServer;
	// partially sent large object item
	SmlItem_t* iLargeObjectItem;
	// buffer for received data;
	CBufFlat* iRecBuf;
	// size of the large object of a server
	TInt iServerLargeObjectSize;
	// identifier in first chunk in server's large object
	HBufC8* iLargeObjectUID;
	// max. object size in SyncHdr level
	TInt iSynchHdrMaxObjSize;
	// max object size of a server
	TInt iServerMaxObjectSize;
	//
	// Used classes
	//
	CNSmlAgentBase* iAgent;
	//
	// Owned classes
	//
	CNSmlStatusContainer* iStatusToServer;
	CNSmlResponseController* iResponseController;
	CNSmlResultsContainer* iResultsToServer;
	CWBXMLSyncMLGenerator* iGenerator;
	CWBXMLParser* iParser;
	CWBXMLSyncMLDocHandler* iDocHandler;
	CNSmlPhoneInfo* iPhoneInfo;
	CNSmlAuth* iServerAuth;
	// Reserved to maintain binary compability
	CNSmlFutureReservation* iReserved;

	TInt iAmountReceived;
	TBool iMoreData;
	TInt iItemSizeInStream;
 	};

#endif      // __NSMLCMDSBASE_H
            
// End of File
