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
* Description:  Database (content) specific data and functionality
*
*/


#ifndef __NSMLDSCONTENT_H__
#define __NSMLDSCONTENT_H__

// INCLUDES
#include <e32hashtab.h>
#include <e32base.h>
#include <SyncMLDef.h>
#include <SmlDataSyncDefs.h>
#include <SyncMLDataFilter.h>
#include "NSmlDSAgent.h"

// CONSTANTS
const TUint KNSmlTwoWayFlag = 1 << 0;
const TUint KNSmlSlowTwoWayFlag = 1 << 1;
const TUint KNSmlOneWayFromClientFlag = 1 << 2;
const TUint KNSmlRefreshFromClientFlag = 1 << 3;
const TUint KNSmlOneWayFromServerFlag = 1 << 4;
const TUint KNSmlRefreshFromServerFlag = 1 << 5;
const TUint KNSmlServerAlertedFlag = 1 << 6;

// <MAPINFO_RESEND_MOD_BEGIN>
const TInt KNSmlDefaultMapStreamUID = 0x101F6DF4;
// <MAPINFO_RESEND_MOD_END>

// FORWARD DECLARATIONS
class CNSmlDbCaps;
class CNSmlURI;
class CNSmlFilter;
class CNSmlDSHostClient;
class CNSmlDSContentItem;

/**
* Class that handles content (datastore) specific data processing.
*
* @lib nsmldsagent.lib
*/
class CNSmlDSContent : public CBase 
	{
	public: // constructors and destructor
		/**
		* Symbian two-phased constructor.
		*/
		static CNSmlDSContent* NewL();
		
		/**
		* Destructor.
		*/
		~CNSmlDSContent();
	
	public: // new functions
	
		/**
		* Creates DSHostClient
		* @return -
		*/
		void InitContentDataL();
		
		/**
		* Returns the index of the current content.
		* @return The index of the current content.
		*/
		TInt CurrentIndex() const;
		
		/**
		* Checks if the content index is set.
		* @return ETrue if the content index is set.
		*/
		TBool ContentIndexIsSet() const;
		
		/**
		* Returns the number of contents.
		* @return The number of contents.
		*/
		TInt Count() const;
		
		/**
		* Returns the implementation UID at the current content index.
		* @return UID of the current data provider.
		*/
		TInt ImplementationUID() const;
		
		/**
		* Returns the local database URI at the current content index.
		* @return URI of the current local database.
		*/
		CNSmlURI* LocalDatabase() const;
		
		/**
		* Returns the remote database URI at the current content index.
		* @return URI of the current remote database.
		*/
		CNSmlURI* RemoteDatabase() const;
		
		/**
		* Returns the synchronization type at the current content index.
		* @return Synchronization type.
		*/
		TPtrC8 SyncType() const;
		
		//RD_SUSPEND_RESUME
		/**
		* Returns the Synchronization Type proposed by the client in the Current Sync session
		*@return Synchronization type.
		*/
	    TPtrC8 	PrevClientSyncType();
	    
	    /**
		* Returns the Synchronization Type proposed by the client in the Current Sync session
		*@return Synchronization type.
		*/
	    TPtrC8  ClientSyncType();
	    
	    //RD_SUSPEND_RESUME
	    /**
		* Gets the UID of the current data item at the current content index.
		* @param aLocalUID On returns contains the current local UID.
		*/
		void CurrentUID( TSmlDbItemUid& aLocalUID ) const;
		
		/**
		* Gets the DevInfo structure at the current content index.
		* @param aDbCaps On return contains the current DevInfo structure.
		* @return KErrNone if successful, otherwise one of the system wide error codes.
		*/
		TInt DbCapabilitiesL(CNSmlDbCaps*& aDbCaps) const;
		
		/**
		* Checks if the given Rx element type is supported by the current data 
		* store.
		* @param aType The Rx type to be checked against.
		* @return ETrue if the given Rx type is supported.
		*/
		TBool IsSupportedRxL( const TDesC8& aType ) const;
		
		/**
		* Checks if the given Tx element type is supported by the current data 
		* store.
		* @param aType The Tx type to be checked against.
		* @return ETrue if the given Tx type is supported.
		*/
		TBool IsSupportedTxL( const TDesC8& aType ) const;
		
		/**
		* Checks if the given synchronization type is supported by the current
		* data store.
		* @param aSyncType The synchronization type to be checked.
		* @return ETrue if the given sync type is supported.
		*/
		TBool IsSupportedSyncTypeL( CNSmlDbCaps::ENSmlSyncTypes aSyncType ) const;
		
		/**
		* Checks if the given synchronization type is supported by the current
		* data store.
		* @param aSyncType The synchronization type to be checked.
		* @return ETrue if the given sync type is supported.
		*/
		TBool IsSupportedSyncTypeL( const TDesC8& aSyncType ) const;
		
		/**
		* Checks if the current data store supports hierarchical sync with the
		* given media type.
		* @param aMediaType Media type used during check.
		* @return ETrue if hierarchical sync is supported.
		*/
		TBool IsHierarchicalSyncSupportedL( const TPtr8 aMediaType ) ;
		
		/**
		* Checks if server supports the synchronization type used in the sync
		* session at the current content index.
		@ return ETrue if the sync type is supported.
		*/
		TBool IsSupportedSyncTypeByServerL() const;
		
		/**
		* Checks if synchronization session with the current data store has 
		* been interrupted.
		* @return ETrue if the sync session has been interrupted.
		*/
		TBool Interrupted() const;
		
		/**
		* Checks if an error due to a Status code received from the server has
		* been written to the history log at the current content index.
		* @return ETrue if an error has been written.
		*/
		TBool ServerStatusError() const;
		
		/**
		* Checks if the initialization Alert command is not yet received for
		* the current data store.
		* @return ETrue if initialization Alert is not received.
		*/
		TBool WaitingInitAlert() const;
		
		/**
		* Checks if slow sync request has been stored for the current data store.
		* @return ETrue if slow sync request has been stored.
		*/
		TBool SlowSyncRequestL() const;
		
		/**
		* Checks if synchronization of any content has been interrupted.
		* @return ETrue if some content is interrupted.
		*/
		TBool AnyDatabaseIsInterrupted() const;
		
		/**
		* Checks if synchronization of all data stores has been interrupted.
		* @ETrue if all contents are interrupted.
		*/
		TBool AllDatabasesAreInterrupted() const;
		
		/**
		* Checks if synchronization of all data stores has been interrupted
		* immediately. In that case no Status commands are sent.
		* @return ETrue if interrupted immediately.
		*/
		TBool AllDatabasesAreInterruptedImmediately() const;
		
		/**
		* Checks if synchronization type is one-way sync from client for all
		* data stores.
		* @ETrue if one-way sync from client is used for all data stores.
		*/
		TBool AllDatabasesOneWayFromClient() const;
		
		/**
		* Checks if there are map items to be sent at the current content index.
		* @ETrue if map items exist.
		*/
		TBool MapExists() const;
		
		/**
		* Checks if the number of synchronized items at the current content index
		* was asked previously.
		* @return ETrue if the count was asked already.
		*/
		TBool ClientItemCountAsked() const;

		/**
		* Checks if the number of synchronized items on the server at the current
		* content index was asked already.
		* @return ETrue if the count was asked.
		*/		
		TBool ServerItemCountAsked() const;
		
		/**
		* Returns the number of synchronized items in the client for the current
		* data store.
		* @return The number of synchronized items.
		*/
		TInt ClientItemCount() const;
		
		/**
		* Returns the number of synchronized items in the server for the current
		* data store.
		* @return The number of synchronized items.
		*/
		TInt ServerItemCount() const;
		
		/**
		* Returns the map item list at the current content index.
		* @return Map item list.
		*/
		SmlMapItemList_t* MapItemListL() const;
		
		/**
		* Returns the maximum object size in the server at the current content
		* index. The size is returned in bytes.
		* @return The maximum object size.
		*/
		TInt MaxObjSizeInServer() const;
		
		/**
		* Returns the maximum object size in content.
		* @return The maximum object size in content.
		*/
		TInt MaxObjSizeL() const;
		
		/**
		* Returns the sync anchor the client has sent to the server during the
		* previous synchronization session.
		* @return Sync anchor of the previous session.
		*/
		TTime LastSyncAnchorL() const;
		
		/**
		* Initializes interruption flags for all contents.
		*/
		void InitInterruptionFlags() const;
		
		/**
		* Creates a new map item to the current content's map item container.
		* @param aLUID Local UID.
		* @param aGUID Global UID.
		* @param aAtomicId Atomic command id associated with the item (0 = none).
		*/
		void CreateNewMapItemL( const TSmlDbItemUid aLUID, const TDesC8& aGUID, const TInt aAtomicId ) const;
		
// <MAPINFO_RESEND_MOD_BEGIN>
		
		/**
		* Sets the stream uid to be used for the map information streaming.
		* @param aMapStreamUid Uid of the stream.
		*/
		void SetMapStreamUID(TInt aMapStreamUid = KNSmlDefaultMapStreamUID);
	
		/**
		* Calls map loader function.
		* @param aStatus Request status.
		* @return If error occurs, system wide error code is returned. 
		*/
		TInt ScanMapContentL( TRequestStatus &aStatus );
	
		/**
		* Loads the map information form the physical storage to the cache.
		*/	
		void LoadMapInfoL();

		/**
		* Saves the map information to the physical storage and update the sync flags required for next sync to continue properly but not the last sync anchor
		* @param aAnchor Time anchor.
		*/
		void SaveMapInfoL( const TTime& aAnchor );

		/**
		*  Cleans up the map information from the cache as well as from physical storage for current database
		*/
		void CleanMapInfo();

		/**
		* Cleans up the map information in the cash and physical storage for all the databases.
		*/	
		void CleanAllMapInfo();	

		/**
		* Saves the map information to the physical storage.
		* @param aAnchor Time anchor.
		*/
		void PackupRequestL( const TTime& aAnchor );

		/**
		* Checks if a given index is a valid index of a database in a sync
		* @param TInt Index of a database.
		* @return Boolean value indicating if given index is valid.
		*/
		TBool CheckDbIndex(TInt aIndex);
	
		/**
		* To Delay the removal of map information from the cache to be removed at a later stage
		* @aRemoveMap Boolean value indicating if map is removable.
		*/
		void SetRemoveMap(TBool aRemoveMap = ETrue);
	
		/**
		* Checks if map information can be removed. 
		* @return Boolean value indicating if map information can be removed..
		*/
		TBool isMapRemovable();
	
		/**
		* Removes the map information form the cache for only set removable map for all the databases.
		*/
		void RemoveAllRemovableMap() const;
	
		/**
		* Removes the map information form the cache for only set removable map and corresponding to the current database under consideration.
		*/
		void RemoveRemovableMap() const;
	
		/**
		* Sets resend to be used. 
		* @param aResendUsed Boolean value indicating if mapinfo resend is used.
		*/
		void SetResendUsed( TBool aResendUsed = EFalse );

		/**
		* Checks if resend is used. 
		* @return Boolean value indicating if resend is used.
		*/
		TBool ResendUsed() const;
// <MAPINFO_RESEND_MOD_END>
		
		/**
		* Sets a list of map items to the current content.
		* @param aMapItemList List of map items. Ownership is changed for this class.
		*/
		void SetMapItemList( SmlMapItemList_t* aMapItemList ) const;
		
		/**
		* Resets the current content's map item container.
		*/
		void RemoveMap() const;
		
		//RD_SUSPEND_RESUME
		/**
		* Sets the synchronization type of the current data store.
		* @param aSyncType Synchronization type.
		*/		
		void SetSyncTypeL( const TDesC8& aSyncType ) const;
		/**
		* Sets the synchronization type of the current data store.
		* This Sync Type is proposed by the Client for each content and will be stored into the Agent Log db.
		* Helps in the Resumed session to check if the Sync Type proposed by client is different than the Suspened session.
		* @param aSyncType Synchronization type.
		*/	
		void SetClientSyncTypeL( const TDesC8& aSyncType ) const;
		
		//RD_SUSPEND_RESUME
		/**
		* Sets the synchronization type of the current data store.
		* @param aSyncType Synchronization type.
		*/		
    	void SetSyncTypeL( CNSmlDbCaps::ENSmlSyncTypes aSyncCap ) const;
    	
		/**
    	* Changes the initializations of the current data store during the sync
    	* session. If data store's sync type is slow sync or refresh sync from 
    	* client then ResetChangeInfoL is called. If the given sync type is
    	* refresh from server then also DeleteAllItemsL is called in addition 
    	* to ResetChangeInfoL.
    	* @param aEndOfInits EFalse on return if all data stores are processed.
    	* @param aResultCode On return contains the result code of the operation.
    	* @param aStartOfInits Should be set to ETrue if this is the first time 
    	* this function is called.
    	*/
		void SetDbAndTimeInitsL( TBool& aEndOfInits, TInt& aResultCode, TBool aStartOfInits);
		
		/**
		* Initializes History Log event of the current content.
		* @param aSyncLog Reference to the History Log.
		*/
		void InitSyncLogEventL( CSyncMLHistoryJob& aSyncLog );
		
		/**
		* Initializes History log events of all contents.
		* @param aSyncLog Reference to the History Log.
		*/
		void InitAllSyncLogEventsL( CSyncMLHistoryJob& aSyncLog );
		
		/**
		* Both the number of synchronized items and synchronization statuses of
		* all contents are written to the History Log.
		*/
		void FinalizeSyncLogEvents() const;
		
		/**
		* Adds a new content (data store) instance.
		*/
		void AddNewContentL();
		
		/**
		* Sets the implementation UID of the current content.
		* @param aImplUID Current content's implementation UID.
		*/
		void SetImplementationUID( TInt aImplUID ) const;
		
		/**
		* Sets the local database URI of the current content.
		* @param aLocalDatabase Local database URI.
		*/		
		void SetLocalDatabaseL( CNSmlURI& aLocalDatabase ) const;
		
		/**
		* Sets the remote database URI of the current content.
		* @param aRemoteDatabase Remote database URI.
		*/
		void SetRemoteDatabaseL( const CNSmlURI& aRemoteDatabase ) const;
		
		/**
		* Initializes the current content (data store).
		* @param aServerId ServerId of the current content.
		* @return ETrue if the operation succeeded.
		*/
		TBool InitContentInstanceL( TDesC& aServerId );
		
		/**
		* Opens the current data store.
		* @param aResultCode On return contains the return code of the operation.
		*/
		void OpenContentInstanceL( TInt& aResultCode ) const;
		
		/**
		* Creates local UID buffer for the current content.
		* @param aResultCode On return contains the return code of the operation.
		*/
		void CreateLUIDBufferL( TInt& aResultCode ) const;
		
		/**
		* Cancels an ongoing asynchronous operation.
		*/
		void CancelRequestL() const;
		
		/**
		* The current content is marked as waiting for the initialization Alert
		* command from the server.
		*/
		void SetInitAlertWaiting() const;
		
		/**
		* The current content is marked as not waiting for the initialization
		* Alert command from the server.
		*/
		void SetInitAlertReceived() const;
		
		/**
		* A slow synchronization request for the current content is passed 
		* on/off for the current content. This value is used in the next
		* synchronization session.
		* @param aRequest ETrue if slow sync flag should be turned on.
		*/
		void SetSlowSyncReqForNextSyncL( TBool aRequest ) const;
		/**
		* Sets the Value to ETrue if the Resume session is rejected by the Server
		* @param aRefreshRequired variable used to store the Status code returned by Server for Resume command
		*/
		void SetRefreshRequiredL(TBool aRefreshRequired);
		/**
		* Gets value stored for the variable aRefreshRequired.
		* aRefreshRequired is Set to ETrue if the Resume command rejected by server.
		*/
		TBool GetRefreshRequired();
		
		//RD_SUSPEND_RESUME
		/**
		* Sets the Value to ETrue if the Resume command is sent by the client for a content
		* @param iSessionResumed variable is set to ETrue if Resume command is sent by the client
		*/
		
		void SetSessionResumedL(TBool aSessionResumed);
				
		/**
		* Gets value stored for the variable iSessionResumed.
		* iSessionResumed is Set to ETrue if the Resume command sent by the client
		*/
		
		TBool GetSessionResumed();
		
		//RD_SUSPEND_RESUME
		/**
		* Sets the content index using the given local database URI.
		* @param aDatabase Local database URI.
		* @return ETrue if a matching content was found.
		*/
		TBool SetIndexByLocalDatabaseL( CNSmlURI& aDatabase );
		
		/**
		* Sets the content index using the given remote database URI.
		* @param aDatabase Remote database URI.
		* @param aRespURIExists ETrue if respURI is present.
		* @return ETrue if a matching content was found.
		*/
		TBool SetIndexByRemoteDatabaseL( CNSmlURI& aDatabase, TBool aRespURIExists );
		
		/**
		* Sets the content index using the given remote database URI, local
		* database URI and content type.
		* @param aRemoteDatabase Remote database URI.
		* @param aLocalDatabase Local database URI.
		* @param aType Content type.
		* @return ETrue if a matching content was found.
		*/
		TBool SetIndexByDatabasesAndTypeL( CNSmlURI& aRemoteDatabase, CNSmlURI& aLocalDatabase, const TDesC8& aType );
		
		/**
		* Marks the current content alerted by the server.
		*/
		void SetAsServerAlerted() const;
		
		/**
		* Sets the content index to the first content.
		* @return EFalse if there are no contents.
		*/
		TBool SetToFirst();
		
		/**
		* Advances the content index by one.
		* @return EFalse if there's no next content.
		*/
		TBool SetToNext();
		
		/**
		* Sets the content index to the given content.
		* @param aInd The new index.
		* @return EFalse if the given content does not exist.
		*/
		TBool SetIndex( TInt aInd );
		
		/**
		* Saves the current content index value.
		*/
		void SaveCurrentIndex();
		
		/**
		* Sets the content index to the saved value.
		*/
		void SetIndexToSaved();
		
		/**
		* Marks the current item of the current content as written to the 
		* outgoing SyncML package.
		*/
		void SetCurrentItemWritten() const;
		
		/**
		* Sets the current content as interrupted. This means that status codes
		* will be sent to the server but the synchronization does not proceed
		* further.
		*/
		void SetInterrupted();
		
		/**
		* Sets the current content as immediately interrupted. This means that
		* no further data about this content will be sent to the server.
		*/
		void SetImmediatelyInterrupted();
		
		/**
		* Marks that the error status received from the server which caused sync
		* interruption has been received by the current content.
		*/
		void SetServerStatusError();
		
		/**
		* Sets the current content to the interrupted state before the sync
		* session has been started.
		*/
		void SetInterruptedBeforeSync() const;
		
		/**
		* Sets all interruption flags off for all contents.
		*/
		void UpdateInterruptedFlags();
		
		/**
		* Sets the synchronization status of the current content.
		* @param aSyncStatus Synchronization status.
		*/
		void SetSyncStatus( CNSmlDSAgent::TNSmlAgentSyncStatus aSyncStatus ) const; 
		
		/**
		* Sets the synchronization status of all contents.
		* @param aSyncStatus Synchronization status.
		*/
		void SetSyncStatusToAll( CNSmlDSAgent::TNSmlAgentSyncStatus aSyncStatus ) const;
		
		/**
		* Sets device capabilities (CtCap elements) for all contents. Data
		* stores use them when building the items that will be sent to the
		* server.
		* @return KErrNone if there were no errors.
		*/
		TInt SetTypeAndDevCapsL() const;
		
		/**
		* Sets device capabilities (CtCap elements) for the current content. 
		* Data stores use them when building the items that will be sent to the
		* server.
		* @return KErrNone if there were no errors.
		*/
		TInt SetSavedPartnerDevCapsL();
		
		/**
		* Marks that client item count of the current content has been asked.
		*/
		void SetClientItemCountAsked() const;
		
		/**
		* Marks that server item count for the current content has been asked.
		* @param aCount Server item count.
		*/
		void SetServerItemCount( TInt aCount ) const;
		
		/**
		* Sets the maximum size of the object that the server can accept from 
		* the current content.
		* @param aSize Maximum object size (in bytes).
		*/
		void SetMaxObjSizeInServer( TInt aSize ) const;

		/**
		* Removes those mappings that were created under a failed atomic command.
		* @param aAtomicId Id of the failed atomic command.
		*/
		void RemoveFailedMappingsL( const TInt aAtomicId );
		
		/**
		* Sets the current data store to the batch mode.
		*/
		void BeginBatchL();
		
		/**
		* Cancels the ongoing batch mode.
		*/
		void CancelBatchL();
		
		/**
		* Orders the current data store to commit batched operations. Batch mode is
		* ended within this function.
		* @param aResultArray Array of results in the order operations were requested.
		* @param aResult On return contains the result code.
		*/
		void CommitBatchL( RArray<TInt>& aResultArray, TInt& aResultCode );
		
		/**
		* Checks if the current data store is in the batch mode.
		* @return ETrue is the batch mode is on, otherwise EFalse.
		*/
		TBool BatchModeOn() const;

		/**
		* Checks if the current data store is in the transaction mode.
		* @return ETrue is the transaction mode is on, otherwise EFalse.
		*/
		TBool AtomicModeOn() const;

		/**
		* Opens the item that is first in the current content's LUID buffer.
		* @param aLocalUid On return contains the opened item's local UID.
		* @param aCmdName On return contains the opened item's command type.
		* @param aSize On return contains the opened item's data size.
		* @param aMeta On return contains the opened item's meta information.
		* @return KErrNone if the item was opened, otherwise a system wide error code.
		*/
		TInt OpenNextItemL( TSmlDbItemUid& aLocalUid, TSmlDbItemUid& aParentUid, TDes8& aCmdName, TInt& aSize, CNSmlDbMeta& aMeta ) const;
		
		/**
		* Creates new item to the current datastore. The created item is left open.
		* @param aUid On return contains the new item's uid.
		* @param aSize New item's size.		
		* @param aMimeType New item's mime type.
		* @param aMimeVersion New item's mime version.
		* @param aParent New item's parent item.
		* @return KErrNone if successful.
		*/
	    TInt CreateItemL( TSmlDbItemUid& aUid, TInt aSize, const TPtrC8 aMimeType, const TPtrC8 aMimeVersion, TSmlDbItemUid aParent ) const;
		
		/**
		* Replaces a item in the current datastore with a new one. The item is left open.
		* @param aUid Uid of the item to be replaced.
		* @param aSize Item's size.
		* @param aParent Item's parent item.
		* @param aPartialUpdate ETrue if partial update should be used.
		* @return KErrNone if successful.
		*/
		TInt ReplaceItemL( const TSmlDbItemUid aUid, TInt aSize, TSmlDbItemUid aParent, TBool aPartialUpdate ) const;

		/**
		* Deletes the given item.
		* @param aUid Item's UID.
		* @param aSoftDelete If set then the item is only soft deleted.
		* @return KErrNone if successfull, otherwise a system wide error code.
		*/
		TInt DeleteItemL( const TSmlDbItemUid aUid, TBool aSoftDelete ) const;
	
		/**
		* Moves the given item under a different parent.
		* @param aUid Item to be moved.
		* @param aParent Item's new parent.
		* @return KErrNone if successfull, otherwise a system wide error code.
		*/
		TInt MoveItemL( const TSmlDbItemUid aUid, const TSmlDbItemUid aParent ) const;

		/**
		* Reads data from the currently open item. The number of bytes read is the same
		* as the given buffer's maximum size.
		* @param aBuffer Buffer where the read data is placed.
		*/
		void ReadItemL( TDes8& aBuffer );
		
		/**
		* Writes data to the currently opened item.
		* @param aData Buffer containing the data that should be written.
		*/
		void WriteItemL( const TDesC8& aData );

		/**
		* Closes the item that was previously opened with OpenNextItemL.
		*/
		void CloseItemL();
		
		/**
		* Commits the currently opened item.
		* @return KErrNone if successfull.
		*/
		TInt CommitItemL();

		/**
		* Checks if the current data store has been synced previously or not. Also the
		* situation where the actual underlying database has been changed is checked.
		* @return ETrue if the current datastore has been synced previously.
		*/ 
		TBool HasSyncHistoryL() const;
		
		/**
		* Maps the given SourceParent value to an existing LUID.
		* @param aSourceParent SourceParent that should be mapped.
		* @param aUid On return contains the matching LUID.
		* @return ETrue if a matching LUID was found.
		*/
		TBool MapSourceParent( const TDesC8& aSourceParent, TSmlDbItemUid& aUid );
		
		/**
		* Sets the item with the given local UID as received. The current data
		* store is called.
		* @param aLUID Local UID of the received item.
		*/
		void SetAsReceivedL( TSmlDbItemUid aLUID ) const;
		
		/**
		* Increases the number of warnings associated with the current content.
		*/
		void IncreaseWarnings() const; 
		
		/**
		* Stores the server supported synchronization types for the current
		* content.
		* @param aSyncTypeArray Array of supported sync types.
		*/
		void UpdateSyncTypeL( const CArrayFix<HBufC8*>& aSyncTypeArray ) const;
		
		/**
		* Stores the server supported CtCaps to the Agent Log.
		* @param aDbCaps DevInfo from where CtCaps are read.
		*/
		void UpdateCtCapsL( const RPointerArray<CNSmlDbCaps>& aDbCaps ) const;
		
		/**
		* Removes CtCaps of the current content from the Agent Log.
		*/
		void RemoveCtCapsL() const;
		
		/**
		* Stores whether the used server supports hierarchical sync or not.
		* @param aSupport ETrue if hierarchical sync is supported.
		*/
		void UpdateSupportHierarchicalSyncL( TInt aSupport ) const;
		
		/**
		* Finalizes all contents.
		*/
		void FinalizeDatabasesL() const;
		
		/**
		* Updates sync anchors of all contents with the given date.
		* @param aAnchor The new sync anchor value.
		*/
		void UpdateSyncAnchorsL( const TTime& aAnchor ) const;
		
		/**
		* Removes the current content from the content array.
		*/
		void RemoveContent();
		
		/**
		* Removes non-server alerted contents from the content array.
		*/
    	void RemoveNotServerAlertedContents();
    
		/**
		* Checks if an operation is supported by the data provider.
		* @param aOperation UID of the operation.
		* @return ETrue if the operation is supported.
		*/
		TBool SupportsOperationL( TUid aOperation );
		
		/**
		* Orders the current data store to start the transaction mode.
		*/
		void BeginTransActionL();
		
		/**
		* Orders the current data store to apply pending atomic operations.
		* @param aStatus Asynchronous request status.
		*/
		void CommitTransactionL( TInt& aResultCode );
		
		/**
		* Aborts the ongoing transaction.
		* @param aStatus Asynchronous request status.
		*/
		void RevertTransactionL( TInt& aResultCode );

		/**
		* Updates current synchronization profile's server ID.
		* @param aServerId New Server Id.
		*/ 
		TInt UpdateServerIdL( TDesC& aServerId ) const;

		/**
		* Returns current content's task id.
		* @return Task Id of the current content or 0 if no content set.
		*/
		TSmlTaskId TaskId() const;
	
		/**
		* Sets current content's task id.
		* @param aId Content's task id.
		*/
		void TaskId( const TSmlTaskId aId );
		
		/**
		* Increases the counter of added items.
		*/
		void IncreaseItemsAdded() const;

		/**
		* Increases the counter of changed items.
		*/
		void IncreaseItemsChanged() const;
	
		/**
		* Increases the counter of deleted items.
		*/
		void IncreaseItemsDeleted() const;
	
		/**
		* Increases the counter of moved items.
		*/
		void IncreaseItemsMoved() const;
	
		/**
		* Increases the counter of failed items.
		*/
		void IncreaseItemsFailed() const;
	
		/**
		* Increases the counter of items added on the server.
		*/
		void IncreaseServerItemsAdded() const;
	
		/**
		* Increases the counter of items changed on the server.
		*/
		void IncreaseServerItemsChanged() const;
	
		/**
		* Increases the counter of items deleted on the server.
		*/
		void IncreaseServerItemsDeleted() const;
	
		/**
		* Increases the counter of items moved on the server.
		*/
		void IncreaseServerItemsMoved() const;
	
		/**
		* Increases the counter of items failed on the server.
		*/
		void IncreaseServerItemsFailed() const;
		
		/**
		* Checks if some or all of the updates sent by the server are applied to
		* local databases.
		* @return ETrue if at least some updates are applied.
		*/
		TBool PartiallyUpdated() const;
		
		/**
		* Gets a list of current content's store names. Ownership is changed to the
		* caller.
		* @return List of store names.
		*/
		CDesCArray* StoreNamesL() const;
		
		/**
		* Checks if filters are used.
		* @return ETrue if filtering is used with the current content.
		*/
		TBool HasFilters() const;
		
		/**
		* Stores the set of filters used with the current content. 
		* @param aFilters Array of used filters.
		* @param aMatchType Filter match type.
		*/
		void SetUsedFiltersL( RPointerArray<CSyncMLFilter>& aFilters, TSyncMLFilterMatchType aFilterMatchType );

        /**
        * Gets used filters from data store adapter.
        * @param aFilter On return contains the constructed filter strcuture.
        * @return KErrNone if performed without errors.
        */
        TInt GetFilterL( CNSmlFilter*& aFilter );
        
        /**
        * Gets filter caps from the agent log.
        * @param aFilterCaps On return contains the stored filter cap elements.
        */
        void GetSavedFilterCapsL( CArrayFix<TNSmlFilterCapData>* aFilterCaps ) const;	
	    
	    /**
	    * Updates filter caps in the agent log.
	    * @param aFilterCaps New filter caps.
	    */	    
        void UpdateFilterCapsL( const CArrayFix<TNSmlFilterCapData>* aFilterCaps ) const;
        
        /**
        * Removes filter caps from the agent log.
        */
        void RemoveFilterCapsL() const;
        /**
        * Checks if the remote database is same as source ref
        */
		TBool CheckValidRemoteDbL(RPointerArray<CNSmlURI>& aSourceRefArr, TBool aResp) const;
	
		//RD_SUSPEND_RESUME	
		/**
        * Get suspended state from agentlog database
        */
		TNSmlSyncSuspendedState SuspendedStateL();
		
		/**
        * Save suspended state to agentlog database
        */
		void SetSuspendedStateL(TNSmlSyncSuspendedState aState);
		/**
        * Save Sync Type of Synchronization session in the AgentLog database.
        */		
		void SetPreviousSyncTypeL(TNSmlPreviousSyncType aPrevSyncType);
		
		/**
        * Gets Sync Type of the previous Suspened session from Agent Log database
        */
   		TNSmlPreviousSyncType PreviousSyncTypeL();
   		
   		
   		/** 
   		* Save Sync Type proposed by Client in a Synchronization session in the AgentLog database.
        */
		void SetPreviousClientSyncTypeL(TNSmlPreviousSyncType aPrevSyncType);
   		
   		/**
        * Gets Sync Type proposed by Client in the previous Suspened session from Agent Log database
        */
   		TNSmlPreviousSyncType PreviousClientSyncTypeL();
		
		//RD_SUSPEND_RESUME
		/**
        * Set mapremovable flag enabled
        */
		void SetMapRemovable();
		
		/**
        * Read a cenrep key
        */
		void ReadRepositoryL(TInt aKey, TInt& aValue);
		
	private: // constructors & operators
		/**
		* Standard C++ constructor,
		*/
		CNSmlDSContent();
		
		/**
		* Symbian 2nd phase constructor.
		*/	
		void ConstructL();
		
		/**
		* Prohibit copy constructor.
		*/
		CNSmlDSContent( const CNSmlDSContent& aOther );
		
		/**
		* Prohibit assignment operator.
		*/
		CNSmlDSContent& operator=( const CNSmlDSContent& aOther );
	
	private: // new functions
		/**
		* Reads given content's remote server DevInfo from the Agent Log.
		* @param aInd Index to a content.
		* @param aDbCaps On return contains remote server's DevInfo.
		*/
		void GetDevCapsFromAgentLogL( TInt aInd, CNSmlDbCaps*& aDbCaps ) const;
		
		/**
		* Convers literal sync type code to a DbCaps sync enumeration.
		* @aSyncType Synctype in literal form.
		* @aDbCapsSyncType On return contains the converted sync type.
		* @return ETrue if the conversion succeeded.
		*/
		TBool SyncTypeConv( const TDesC8& aSyncType, CNSmlDbCaps::ENSmlSyncTypes& aDbCapsSyncType ) const;
		
		/**
		* Checks if a bit in the given integer is set.
		* @param aValue UInt to check against.
		* @param aBit Bit mask to be checked.
		* @return ETrue if the bit is set.
		*/
		TBool IsFlagSet( const TUint& aValue, const TUint& aBit ) const;
		
		/**
		* Resets the given content's TaskInfo structure.
		* @param aSyncLog History Log where the TaskInfo is stored.
		* @param aInd Index to a content.
		*/
		void InitEventL( CSyncMLHistoryJob& aSyncLog, TInt aInd );

	
	private: // data
		// index to current content
		TInt iIndex;
		// saved index  (for multimessage Sync)
		TInt iSavedIndex;
		// array for contents 
		RPointerArray<CNSmlDSContentItem> iStores;
		// used to communicate with host server(s)
		CNSmlDSHostClient* iHostClient;
		// server used in the previous session
		HBufC* iOldServerId;
		
// <MAPINFO_RESEND_MOD_BEGIN>
	TUid iMapStreamUid;
	TBool iOldMapExists;
	TRequestStatus* iCallerStatus;
	TBool iResendUsed;
// <MAPINFO_RESEND_MOD_END>
	RPtrHashMap<TDesC8, TBool> iHierarchicalSupportTable;
		
	};

#endif // __NSMLDSCONTENT_H__

// End of File
 
