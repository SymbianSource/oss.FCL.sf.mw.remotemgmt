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


#ifndef __NSMLDSCMDS_H__
#define __NSMLDSCMDS_H__

// INCLUDES
#include <e32base.h>
#include <SmlDataSyncDefs.h>
#include "NSmlCmdsBase.h"
#include "WBXMLDevInfParser.h"
#include "nsmlagentlog.h"

// FORWARD DECLARATIONS
class CNSmlAgentBase;
class CNSmlDSContent;
class MSyncMLProgressObserver;
class CWBXMLDevInfGenerator;
class CWBXMLDevInfDocHandler;
class CNSmlDbCaps;
class CNSmlDbMetaHandler;
class CNSmlDSBatchBuffer;
class CNSmlDSOperatorSettings;

inline void DeleteRPointerArray( TAny* aPtr )
	{
	( REINTERPRET_CAST ( RPointerArray<CNSmlDbCaps>*, aPtr ) )->ResetAndDestroy();
	}

// CLASS DECLARATION

/**
* CNSmlDSCmds implements DS specific parts of SyncML commands handling.
*  
* @lib nsmldsagent.lib
*/
class CNSmlDSCmds : public CNSmlCmdsBase, public MWBXMLDevInfCallbacks
	{
    public: // constructors and destructor
    	/**
    	* Symbian 2-phased constructor.
		* @param aAgent Used synchronisation agent.
		* @param aSessionID Sync session Id.
		* @param aVerProto DS protocol version.
		* @param aPublicId Public Id of the used DS protocol. 
		* @param aSyncMLUserName SyncML server username.
		* @param aSyncServer Used SyncML server's URI.
    	* @param aDSContent Object used to handle datastore specific operations.
    	* @param aDSObserver Observer callback.
		* @param aVersionCheck Whether the protocol version is checked or not.
    	*/
    	static CNSmlDSCmds* NewL( CNSmlAgentBase* aAgent, const TDesC8& aSessionID, const TDesC8& aVerProto, const TInt aPublicId, const HBufC& aSyncMLUserName, CNSmlURI* aSyncServer, CNSmlDSContent& aDSContent, MSyncMLProgressObserver& aDSObserver, TBool aVersionCheck );   

     	/**
     	* Destructor.
     	*/
     	virtual ~CNSmlDSCmds();

    public: // functions from base classes
    	/**
    	* From CNSmlCmdsBase. Builds a structure for the Alert command and calls 
    	* the WBXML generator.
    	* @param aAlertCode Aler code.
    	* @param aLastSyncAnchor Anchor/Last value to be inserted to the command.
    	* @param aNextSyncAnchor Anchor/Next value to be inserted to the command.
    	*/
    	void DoAlertL( const TDesC8& aAlertCode, TTime* aLastSyncAnchor = NULL, TTime* aNextSyncAnchor = NULL );
    	
    	/**
    	* From CNSmlCmdsBase. Builds a structure for the Put command. A DevInfo
    	* structure is constructed inside the Put. Finally the WBXML generator
    	* is called.
    	*/
		void DoPutL();
		
		/**
		* From CNSmlCmdsBase. Builds a structure for the Results command. A DevInfo
		* structure is constructed inside the Results element. As a last step the
		* WBXML generator is called.
		* @return EReturnOK if the operation succeeded.
		*/
		CNSmlCmdsBase::TReturnValue DoResultsL();
		
		/**
		* From CNSmlCmdsBase. Builds a structure for the Get command. This is used
		* to ask DevInfo from the server. At the end of this function the WBXML 
		* generator is called.
		*/
		void DoGetL();
		
		/**
		* From CNSmlCmdsBase. Builds a structure for the Sync command. Calls the 
		* WBXML generator.
		* @return EReturnOK if completed successfully.
		*/
		CNSmlCmdsBase::TReturnValue DoStartSyncL();
		
		/**
		* From CNSmlCmdsBase. Completes the generation of the Sync command.
		*/
		void DoEndSyncL();
		
		/**
		* From CNSmlCmdsBase. Builds a structure for Add, Replace, Delete or Move
		* command. Content's data is retrieved via the CNSmlDSContent::FetchNextItemL
		* method. Finally the WBXML generator is called.
		* @return EReturnOK if completed without errors.
		*/
		CNSmlCmdsBase::TReturnValue DoAddOrReplaceOrDeleteL();
		
		/**
		* From CNSmlCmdsBase. Builds a structure for the Map command. Map items are
		* retrieved by the CNSmlDSMapContainer::MapItemListL method. The WBXML
		* generator is called.
		* @return EReturnOK if completed successfully.
		*/
		CNSmlCmdsBase::TReturnValue DoMapL();
		
		/**
		* From CNSmlCmdsBase. Handles the Results command received from a server.
		* The ParseDeviceInfoL function is called in order to process the received
		* DevInfo structure.
		* @param aResults The received Results element.
		*/
		void ProcessResultsCmdL( SmlResults_t* aResults );
		
		/**
		* From CNSmlCmdsBase. Handles the Put command received from a server.
		* The ParseDeviceInfoL function is called in order to process the received
		* DevInfo structure.
		* @param aPut The received Put element.
		*/
		void ProcessPutCmdL( SmlPut_t* aPut );
		
		/**
		* From CNSmlCmdsBase. Handles the Get command received from a server.
		* @param aGet The received Get element.
		*/
		void ProcessGetCmdL( SmlGet_t* aGet );
		
		/**
		* From CNSmlCmdsBase. Handles the Alert command received from a server.
		* The CNSmlDSContent::SetIndexByLocalDatabaseL() function is called
		* to find the correct data store. In case of initialization Alert command
		* the received sync type is validated.
		* @param aAlert The received Alert element.
		* @param aNextAlert ETrue if the Alert is Next Alert.
		* @param aServerAlert ETrue if the Alert is Server Alert.
		* @param aDisplayAlert ETrue if the Alert is Display Alert.
		*/
		void ProcessAlertCmdL( SmlAlert_t* aAlert, TBool aNextAlert = EFalse, TBool aServerAlert = EFalse, TBool aDisplayAlert = EFalse );
		
		/**
		* From CNSmlCmdsBase. Handles the Sync command received from a server. 
		* The CNSmlDSContent::SetIndexByLocalDatabaseL() function is called
		* to find the correct data store.
		* @param aSync The received Sync element.
		*/
		void ProcessSyncL( SmlSync_t* aSync );
		
		/**
		* From CNSmlCmdsBase. Handles the end of the received Sync command.
		*/
		void ProcessEndSyncL();
		
		/**
		* From CNSmlCmdsBase. Handles the received Add, Replace, Delete and Move
		* commands. The UpdateL() function is called to handle items in the command.
		* @param aCmd Type of the received command.
		* @param aContent Content of the received command.
		*/
		void ProcessUpdatesL( const TDesC8& aCmd, SmlGenericCmd_t* aContent );
		
		/**
		* From CNSmlCmdsBase. Handles the received Atomic command. Inside Atomic all
		* operations to data stores are performed in a transactional way. The received 
		* Atomic command fails if the used data store does not support transactions.
		* @param aAtomic The received Atomic command.
		*/
		void ProcessAtomicL( SmlAtomic_t* aAtomic );
		
		/**
		* From CNSmlCmdsBase. Handles the end of a previously received Atomic command.
		* All transactions are committed and their results checked. If any of the operations
		* has failed then all of them will be rolled back.
		*/
		void ProcessEndAtomicL();
		
		/**
		* From CNSmlCmdsBase. An empty implementation.
		* @param aSequence The received Sequence element.
		*/
		void ProcessSequenceL( SmlSequence_t* aSequence);
		
		/**
		* From CNSmlCmdsBase. An empty implementation.
		*/
		void ProcessEndSequence();
		
		/**
		* From CNSmlCmdsBase. Generates the end tag of the SyncML element.
		* @param aFinal ETrue if this is the last message from the client.
		*/
		void DoEndMessageL( TBool aFinal );
		
        /**
         * Checks if received Alert Code is a sync type and tries to convert
         * it to Sync Type (TSmlSyncType). If conversion is succesful, the
         * value is stored into Cenrep file (KCRUidOperatorDatasyncErrorKeys).
         * @param aAlertCode received alert code.
         */
        void StoreSyncType( const TDes8& aAlertCode );

		        
    private: // constructors and operators
    	/**
    	* C++ constructor.
    	* @param aDSContent Object used to handle datastore specific operations.
    	* @param aDSObserver Observer callback.
    	*/
		CNSmlDSCmds( CNSmlDSContent& aDSContent, MSyncMLProgressObserver& aDSObserver );
		
		/**
		* Symbian 2nd phase constructor.
		* @param aAgent Used synchronisation agent.
		* @param aSessionID Sync session Id.
		* @param aVerProto DS protocol version.
		* @param aPublicId Public Id of the used DS protocol. 
		* @param aSyncMLUserName SyncML server username.
		* @param aSyncServer Used SyncML server's URI.
		* @param aVersionCheck Whether the protocol version is checked or not.
		*/
		void ConstructL( CNSmlAgentBase* aAgent, const TDesC8& aSessionID, const TDesC8& aVerProto, const TInt aPublicId, const HBufC& aSyncMLUserName, CNSmlURI* aSyncServer, TBool aVersionCheck );
		
		/**
		* Prohibit copy constructor.
		*/
		CNSmlDSCmds( const CNSmlDSCmds& aOther );

		/**
		* Prohibit assignment operator.
		*/
		CNSmlDSCmds& operator=( const CNSmlDSCmds& aOther );
    
	private: // functions from base classes
		/**
		* From MWBXMLDevInfCallbacks. Callback function used by the DevInfo parser.
		* @param aContent Pointer to the parsed content.
		* @return Return code defined in the SyncML toolkit.
		*/
		Ret_t smlDeviceInfoL( SmlDevInfDevInfPtr_t aContent );

	private: // new functions
	TBool MatchServerAlertL( const SmlAlert_t* aAlert, TInt aStatusID );
	TPtrC8 DoDeviceInfoL(TBool aConvert = EFalse);
	void SwitchAlertCode(TDes8& aAlertCode);
	void DoTargetWithFilterL( sml_target_s*& aTarget, const TDesC& aLocURIData ) const;
	void FreeDeviceInfo();
	void ParseDeviceInfoL( TInt aStatusID, const TPtrC8& aDeviceInfo );
	void ProcessDeviceInfoL( const SmlDevInfDevInf_t& aContent );
	void ProcessCTCapL( SmlDevInfCtCapList_t& aCapList, const TDesC8& aCTType ) const;
	void ProcessFilterCapL ( SmlDevInfFilterCapList_t& aFilterCapList, const TDesC8& aCTType ) const;

	void DoNumberOfChangesL( SmlPcdata_t*& aNoc ) const;
	void UpdateL( const TDesC8& aCmd, const SmlGenericCmd_t* aContent, const SmlItem_t* aItem, const TDesC8& aUID, const TSmlDbItemUid aParent );
	TBool IsDevInfoContentTypeOkL( const SmlPcdata_t* aMeta, TInt aStatusID ); 
	TBool IsLUIDOkLC( const TDesC8& aCmd, const SmlGenericCmd_t* aContent, const SmlItem_t* aItem, HBufC8*& aLUID );
	TBool IsGUIDOkLC( const TDesC8& aCmd, const SmlGenericCmd_t* aContent, const SmlItem_t* aItem, HBufC8*& aGUID );
	TBool IsParentOkL( const sml_source_or_target_parent_s* aParent, TSmlDbItemUid& aGUID );
	TBool IsParentOkLC( const sml_source_or_target_parent_s* aParent, HBufC8*& aGUID );
	TBool IsFormatOkL( const TDesC8& aCmd, const SmlGenericCmd_t* aContent, const SmlItem_t* aItem );
	TBool IsChrFormat( const SmlPcdata_t* aMeta, TPtrC8& aFormat ) const;
	TBool CheckAndSetReceivedMediaTypeL( const TDesC8& aCmd, const SmlGenericCmd_t* aContent, const SmlItem_t* aItem );
	TBool MediaTypeSetOkL( const SmlPcdata_t* aMeta, TBool& aFound, TPtr8& aMediaType,const TDesC8& aCmd ) const;
	HBufC* AlertDataLC( const SmlItemList_t* aItemList ) const;
	void AppendCTCapsL( CNSmlDbCaps& aDbCaps,const SmlDevInfDatastore_t* aDataStore, SmlDevInfCtCapList_t* aCtCapList ) const;
	
	TInt AlertParameter ( const SmlPcdata_t* aData, const TDesC8& aParamID ) const;
		/**
		* Performs mapping between datastore operation return values and SyncML error codes.
		* @param aIndex Index to the current batch item.
		* @param aStatus Datastore operation's return value that should be mapped.
		*/
		void MapStatusCodeL( const TInt aIndex, const TInt aStatus );
		
	TBool IsVersionOkL ( const SmlDevInfDevInf_t& aContent, TInt aStatusID );
	TBool IsRxTxOkL( SmlDevInfDatastoreList_t* aCurrDatastore, TInt aStatusID, RPointerArray<CNSmlDbCaps>& aDbCapArray );
	TBool UpdateServerIdInSettingsL( const SmlDevInfDevInf_t& aContent, TInt aStatusID );

		/**
		* Buffers the received chunked item.
		* @param aUid UID of the received item.
		* @param aCurrItem Current item's data.
		* @param aMetaInCommand Current command's meta data.
		* @param aBuffer Buffer where the received chunk is read.
		* @return ESmlStatusOK if the whole item was received,
		* ESmlStatusItemAccepted if the item was accepted or an error code.
		*/
		TNSmlError::TNSmlSyncMLStatusCode GetChunkL( const TDesC8& aUid, const SmlItem_t* aCurrentItem, const SmlPcdata_t* aMetaInCommand, CBufBase*& aBuffer );
		
		/**
		* Commits batched items.
		*/     	
		void CommitBatchL();
		
		/**
		* Converts UID from literal to numeric presentation.
		* @param aLiteralUid UID in literal form.
		* @param aNumericUid On return contains UID in numeric form.
		* @result KErrNone if the conversion succeeded.
		*/
		TInt ConvertUid( const TDesC8& aLiteralUid, TSmlDbItemUid& aNumericUid );

		/**
		 * Adds operator specific extensions (XNam, XVal) to device info.
		 * The XNam and XVal to be included in the device info are read from
		 * an ECom plugin, which implements Device Info Extension Data Container 
		 * plugin interface (UID: 0x2002DC7C).
		 * 
		 * @param aDevInf Device info object, to which the operator extensions
		 *  are added.
		 */
		void InsertOperatorExtensionDevInfFieldsL( SmlDevInfDevInfPtr_t& aDevInf );

		/**
		 * Appends the given dev info extension item to the extension list.
		 * 
		 * @param aExtList Extension list, to which the new item is to be added.
		 *  On return, the list includes the given new item.
		 * @param aExtItem Extension item to be added to the extension list.
		 */
		void AppendToExtensionListL( SmlDevInfExtListPtr_t aExtList, 
			SmlDevInfExtPtr_t aExtItem );

		/**
		 * Appends a new XVal (i.e. extension value) item with the given XVal 
		 * string to the XVal list.
		 * 
		 * @param aXValList XVal list, to which the the new XVal item is to be added.
		 *  On return, the list includes the given new item.
		 * @param aXVal A string, which is added to the XVal list as a new item.
		 */
		void AppendToXValListL( SmlPcdataListPtr_t aXValList,
			const TDesC8& aXVal );

	private: // data	
		// batched items buffer
		CNSmlDSBatchBuffer* iBatchBuffer;
		// device info switch
		TBool iDSDeviceInfoParsed;
		// MsgID of Get
		HBufC8* iDSGetMsgId;
		// CmdID of Get 
		HBufC8* iDSGetCmdId;
		// command name of a partially sent large object
		HBufC8* iDSLargeObjectCmdName;
		// Target in Sync from a server
		CNSmlURI* iDSPreviousSyncTargetURI;
		// class for content specific processing
		CNSmlDSContent& iDSContent;
		// observer callback
		MSyncMLProgressObserver& iDSObserver;
		// WBXML generator
		CWBXMLDevInfGenerator* iDSDevGenerator; 
		// buffer for received data
		CBufFlat* iDataBuffer;
		// flag to indicate if the item to be sent is a large object
		TBool iLargeData;
		// batch mode on/off
		TBool iBatchModeOn;
		// id of the ongoing atomic command
		TInt iAtomicId;
		// atomic transaction mode on/off
		TBool iAtomicModeOn;
		// atomic command has failed
		TBool iAtomicHasFailed;
		// UID of the received large object
		HBufC8* iLargeUid;
		// true if an item is opened
		TBool iItemOpened;
		// total number of sent bytes of a chunked item
		TInt iBytesSent;
		// UID generated by a datastore adapter
		TSmlDbItemUid iNewUid;
	};

#endif // __NSMLDSCMDS_H__
            
// End of File
