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
* Description:  Client module of DS Host Servers
*
*/


#ifndef __NSMLDSHOSTCLIENTSESSION_H__ 
#define __NSMLDSHOSTCLIENTSESSION_H__

// ------------------------------------------------------------------------------------------------
// Includes
// ------------------------------------------------------------------------------------------------
#include <e32base.h>
#include <e32std.h>
#include <badesca.h>
#include <SyncMLDef.h>
#include <SmlDataProvider.h>
#include <SyncMLDataFilter.h>

// ------------------------------------------------------------------------------------------------
// Class forwards
// ------------------------------------------------------------------------------------------------
struct TNSmlFilterCapData;
class CNSmlFilter;
class RSmlFilter;
class RSmlFilterDef;
class CNSmlDbCaps;
struct TNSmlDPInformation;
class RNSmlDbItemModificationSet;

// ------------------------------------------------------------------------------------------------
//	client for DS Host Servers.
//
//  @lib nsmldshostclient.lib
// ------------------------------------------------------------------------------------------------
class RNSmlDSHostClient : public RSessionBase
	{
	enum TNSmlDSItemState
		{
		ENSmlClosed,
		ENSmlItemCreating,
		ENSmlItemUpdating
		};
private:
	// ------------------------------------------------------------------------------------------------
	// Buffer + dynamic streams for that buffer. 
	//
	// @lib nsmldshostserver.lib
	// ------------------------------------------------------------------------------------------------
		struct TStreamBuffers
			{
			CBufBase* iBuffer;
			RWriteStream* iWrite;
			RReadStream* iRead;
			};
public: 
	/**
	* Connects client to Host Server.
	* @param aServerName. The name of server.
	* @param aVersion. The version of server.
	* @return TInt. KErrNone or one of the system wide error codes.
	*/
	TInt Connect( const TDesC& aServerName, const TVersion& aVersion );
	
	/**
	* Closes connection between client and Host Server.
	*/
	void Close();
	
	/**
	* Creates chunk that is used to transfer data between client and server.
	* @return TInt. KErrNone or one of the system wide error codes.
	*/
	TInt CreateChunk() const;
	
	/**
	* Sends Chunk handle to SyncML DS Host Servers.
	* @return TInt. KErrNone or one of the system wide error codes.
	*/
	TInt SendChunkHandle() const;
	
	/**
	* Creates Data Providers.
	* @param aIds. The array of IDs of Data Providers.
	* @param aResultArray. The result for each Data Provider. KErrNone or one of the system wide error codes.
	* 		 Order of results must be same as IDs of Data Providers.
	*/
	void CreateDataProvidersL( const RArray<TSmlDataProviderId>& aIds, RArray<TInt>& aResultArray );
	
	/**
	* Creates all possible Data Providers.
	* @param aIds. At return the array of IDs of Data Providers.
	* @param aExceptIds. Those Data Providers are not created.
	*/
	void CreateAllDataProvidersL( RArray<TSmlDataProviderId>& aIds, const RArray<TSmlDataProviderId>& aExceptIds );
	
	/**
	* fetches information on data provider
	* The caller has the responsibility to free all the memory reserved for returned struct.
	* @param aResultCode. The result of method. KErrNone if everything goes correctly.
	* @return TNSmlDPInformation*. Struct filled with fetched information. 
	*/
	TNSmlDPInformation* DataProviderInformationL( const TSmlDataProviderId aId, TInt& aResultCode ) const;
	
	/**
	* Asks if Data Provider supports some operation.
	* @param aOpId. The ID of Operation.
	* @param aId. The ID of Data Provider.
	* @param aResultCode. The result of method. KErrNone if everything goes correctly.
	* @return TBool, ETrue if operation is supported and EFalse if not.
	*/
	TBool SupportsOperationL( TUid aOpId, const TSmlDataProviderId aId, TInt& aResultCode ) const;
	
	/**
	* Creates Data Store format of Data Provider.
	* @param aDataStoreFormat. At return the datastore format of the Data Provider.
	* @param aId. The ID of Data Provider.
	* @param aResultCode. The result of method. KErrNone if everything goes correctly.
	* @return CNSmlDbCaps*.
	*/
	CNSmlDbCaps* StoreFormatL(  const TSmlDataProviderId aId, TInt& aResultCode );
	
	/**
	* Creates list of Data Store names of Data Provider.
	* @param aNameList. returns the names of datastores that the Data Provider can open.
	* @param aId. The ID of Data Provider.
	* @param aResultCode. The result of method. KErrNone if everything goes correctly.
	*/
	void ListStoresL( CDesCArray* aNameList, const TSmlDataProviderId aId, TInt& aResultCode );
	
	/**
	* Creates default name for Data Store of Data Provider.
	* @param aId. The ID of Data Provider.
	* @param aResultCode. The result of method. KErrNone if everything goes correctly.
	* @return HBufC* that includes default Data Store name.
	*/
	HBufC* DefaultStoreL( const TSmlDataProviderId aId, TInt& aResultCode ) const;
			
	/**
	* This method returns the set of filters that can be used to send to the Sync Partner.
	* @param aId. The ID of Data Provider.
	* @param aResultCode. The result of method. KErrNone if everything goes correctly.
	* @return RPointerArray<CSyncMLFilter>*. Array, that includes filter.
	*/
	RPointerArray<CSyncMLFilter>* SupportedServerFiltersL( const TSmlDataProviderId aId, TSyncMLFilterMatchType& aMatchType, TSyncMLFilterChangeInfo& aChangeInfo, TInt& aResultCode ) const;
	
	/**
	* This method updates dynamic filters up-to-date.
	* @param aFilters. The array that includes filters
	* @param aChangeInfo. The change information about changes that data provider did
	* @param aId. The ID of Data Provider.
	* @param aResultCode. The result of method. KErrNone if everything goes correctly.
	*/
	void CheckServerFiltersL( const TSmlDataProviderId aId, RPointerArray<CSyncMLFilter>& aFilters, TSyncMLFilterChangeInfo& aChangeInfo, TInt& aResultCode ) const;
	
	/**
	* This method checks what filters are supported by server.
	* @param aServerDataStoreFormat. The store format of Sync Partner.
	* @param aFilterInfoArr. Array of filter information
	* @param aFilters. The array that includes filters
	* @param aChangeInfo. The change information about changes that data provider did
	* @param aId. The ID of Data Provider.
	* @param aResultCode. The result of method. KErrNone if everything goes correctly.
	*/
	void CheckSupportedServerFiltersL( const TSmlDataProviderId aId, const CNSmlDbCaps& aServerDataStoreFormat, const CArrayFix<TNSmlFilterCapData>& aFilterInfoArr, RPointerArray<CSyncMLFilter>& aFilters, TSyncMLFilterChangeInfo& aChangeInfo, TInt& aResultCode ) const;
	
	/**
	* Get Filters.
	* @param aId. The ID of Data Provider.
	* @param aFilterArray. The filters to be used for the query generation
	* @param aFilter. Results
	* @param aMatchType. The filter match type to be used
	* @param aResultCode. The result of method. KErrNone if everything goes correctly.
	*/	
	void GetFilterL( const TSmlDataProviderId aId, const TDesC& aStoreName, const RPointerArray<CSyncMLFilter>& aFilterArray, CNSmlFilter*& aFilter, TSyncMLFilterMatchType aMatchType, TInt& aResultCode );
	
	/**
	* Opens the data store specified by aStoreName asynchronously.
	* @param aContext. Identifies the specific synchronisation relationship to use.
	* @param aId. The ID of Data Provider.
	* @param aStoreName. The name of the data store that is used.
	* @param aResultCode. The result of method. KErrNone if everything goes correctly.
	*/
	void OpenL( const TSmlDataProviderId aId, const TDesC& aStoreName, const TDesC& aServerId, const TDesC& aRemoteDB, TInt& aResultCode ) const;
			
	/**
	* Cancel the current asynchronous request.
	* @param aId. The ID of Data Provider.
	* @param aStoreName. The name of the data store that is used.
	* @param aResultCode. The result of method. KErrNone if everything goes correctly.
	*/
	void CancelRequest( const TSmlDataProviderId aId, const TDesC& aStoreName, TInt& aResultCode ) const;
	
	/**
	* Starts the transaction mode.
	* @param aId. The ID of Data Provider.
	* @param aStoreName. The name of the data store that is used.
	* @param aResultCode. The result of method. KErrNone if everything goes correctly.
	*/
	void BeginTransaction( const TSmlDataProviderId aId, const TDesC& aStoreName, TInt& aResultCode );
	
	/**
	* Method will be called at the end of a successful transaction.
	* @param aId. The ID of Data Provider.
	* @param aStoreName. The name of the data store that is used.
	* @param aResultCode. The result of method. KErrNone if everything goes correctly.
	*/
	void CommitTransaction( const TSmlDataProviderId aId, const TDesC& aStoreName, TInt& aResultCode );
	
	/**
	* Method will be called to abort an ongoing transaction.
	* @param aId. The ID of Data Provider.
	* @param aStoreName. The name of the data store that is used.
	* @param aResultCode. The result of method. KErrNone if everything goes correctly.
	*/
	void RevertTransaction( const TSmlDataProviderId aId, const TDesC& aStoreName, TInt& aResultCode );
	
	/**
	* Starts the batch mode.
	* @param aId. The ID of Data Provider.
	* @param aStoreName. The name of the data store that is used.
	* @param aResultCode. The result of method. KErrNone if everything goes correctly.
	*/
	void BeginBatch( const TSmlDataProviderId aId, const TDesC& aStoreName, TInt& aResultCode );
	
	/**
	* Method will be called at the end of the batch mode.
	* @param aResultArray is used to store result for each operation
	*		 (in the order they were submitted) during batch.
	* @param aId. The ID of Data Provider.
	* @param aStoreName. The name of the data store that is used.
	* @param aResultCode. The result of method. KErrNone if everything goes correctly.
	*/
	void CommitBatchL( RArray<TInt>& aResultArray, const TSmlDataProviderId aId, const TDesC& aStoreName, TInt& aResultCode );
	
	/**
	* Method will be called to abort an ongoing batch mode.
	* @param aId. The ID of Data Provider.
	* @param aStoreName. The name of the data store that is used.
	* @param aResultCode. The result of method. KErrNone if everything goes correctly.
	*/
	void CancelBatch( const TSmlDataProviderId aId, const TDesC& aStoreName, TInt& aResultCode );
	
	/**
	* Sets the SyncML server (SyncPartner) Data Format
	* @param aServerDataStoreFormat. The data store format of Sync Partner.
	* @param aId. The ID of Data Provider.
	* @param aStoreName. The name of the data store that is used.
	* @param aResultCode. The result of method. KErrNone if everything goes correctly.
	*/
	void SetRemoteDataStoreFormatL( const CNSmlDbCaps& aServerDataStoreFormat, const TSmlDataProviderId aId, const TDesC& aStoreName, TInt& aResultCode );
	
	/**
	* Sets the SyncML server (SyncPartner) Data Format
	* @param aServerMaxObjectSize. The maximum object size of Sync Partner.
	* @param aId. The ID of Data Provider.
	* @param aStoreName. The name of the data store that is used.
	* @param aResultCode. The result of method. KErrNone if everything goes correctly.
	*/
	void SetRemoteMaxObjectSizeL( TInt aServerMaxObjectSize, const TSmlDataProviderId aId, const TDesC& aStoreName, TInt& aResultCode ) const;
	
	/**
	* Gets the maximum object size of Data Store which is reported to the SyncML partner.
	* @param aId. The ID of Data Provider.
	* @param aStoreName. The name of the data store that is used.
	* @param aResultCode. The result of method. KErrNone if everything goes correctly.
	* @return TInt. The maximum object size of Data Store.
	*/
	TInt MaxObjectSize( const TSmlDataProviderId aId, const TDesC& aStoreName, TInt& aResultCode  ) const;
	
	/**
	* Opens item at Data Store.
	* @param aUid. The UID of item.
	* @param aFieldChange. Data Provider change to ETrue if there is only some field changes at item.
	* @param aSize. The Data Provider puts the size of item to this parameter.
	* @param aParent. Data Provider puts the UID of parent of item to this parameter.
	* @param aMimeType. Data Provider puts the mime type of item to this parameter.
	* @param aMimeVer. Data Provider puts the mime version of item to this parameter.
	* @param aId. The ID of Data Provider.
	* @param aStoreName. The name of the data store that is used.
	* @param aResultCode. The result of method. KErrNone if everything goes correctly.
	*/
	void OpenItemL( TSmlDbItemUid aUid, TBool& aFieldChange, TInt& aSize, TSmlDbItemUid& aParent, HBufC8*& aMimeType, HBufC8*& aMimeVer, const TSmlDataProviderId aId, const TDesC& aStoreName, TInt& aResultCode ) const;
	
	/**
	* Creates new item to Data Store.
	* @param aUid. The Data Provider puts the UID of new item to this parameter. It can be also put just after commit method.
	* @param aSize. The size of new item.
	* @param aParent. The UID of parent of new item.
	* @param aMimeType. The mime type of new item.
	* @param aMimeVer. The mime version of new item.
	* @param aId. The ID of Data Provider.
	* @param aStoreName. The name of the data store that is used.
	* @param aResultCode. The result of method. KErrNone if everything goes correctly.
	*/
	void CreateItemL( TSmlDbItemUid& aUid, TInt aSize, TSmlDbItemUid aParent, const TDesC8& aMimeType, const TDesC8& aMimeVer, const TSmlDataProviderId aId, const TDesC& aStoreName, TInt& aResultCode );
	
	/**
	* Replaces old item at Data Store.
	* @param aUid. The UID of item. If item is added to the Data Store then The Data Provider
	*		 puts the UID of new item to this parameter. It can be also put just after commit method.
	* @param aSize. The size of item.
	* @param aParent. The UID of parent of item.
	* @param aFieldChange. If this is true then replace is only partial,
	* @param aId. The ID of Data Provider.
	* @param aStoreName. The name of the data store that is used.
	* @param aResultCode. The result of method. KErrNone if everything goes correctly.
	*/
	void ReplaceItemL( TSmlDbItemUid aUid, TInt aSize, TSmlDbItemUid aParent, TBool aFieldChange, TSmlDataProviderId aId, const TDesC& aStoreName, TInt& aResultCode );
	
	/**
	* Reads data from item at Data Store. Item must be opened before this method can be called.
	* This method is called until aBuffer is not used totally or method leaves with KErrEof.
	* @param aBuffer. The Data Provider puts readed data to this parameter,
	* @param aId. The ID of Data Provider.
	* @param aStoreName. The name of the data store that is used.
	* @param aResultCode. The result of method. KErrNone if everything goes correctly.
	*/
	void ReadItemL( TDes8& aBuffer, const TSmlDataProviderId aId, const TDesC& aStoreName, TInt& aResultCode ) const;
	
	/**
	* Writes data to item to Data Provider. CreateItemL or ReplaceItemL method must be called before
	* this method can be called. This method is called until all data to current item is written.
	* @param aData. The data that is written to item at Data Store,
	* @param aId. The ID of Data Provider.
	* @param aStoreName. The name of the data store that is used.
	* @param aResultCode. The result of method. KErrNone if everything goes correctly.
	*/
	void WriteItemL( const TDesC8& aData, const TSmlDataProviderId aId, const TDesC& aStoreName, TInt& aResultCode );
	
	/**
	* After item is written to Data Provider it can be saved to the Data Store.
	* This method can be called just after WriteItemL method.
	* @param aId. The ID of Data Provider.
	* @param aStoreName. The name of the data store that is used.
	* @param aResultCode. The result of method. KErrNone if everything goes correctly.
	*/
	void CommitItem( const TSmlDataProviderId aId, const TDesC& aStoreName, TInt& aResultCode );
	
	/**
	* Closes opened item.
	* This method can be called just if some item is open.
	* @param aId. The ID of Data Provider.
	* @param aStoreName. The name of the data store that is used.
	* @param aResultCode. The result of method. KErrNone if everything goes correctly.
	*/
	void CloseItem( const TSmlDataProviderId aId, const TDesC& aStoreName, TInt& aResultCode );
	
	/**
	* Moves item to new location.
	* @param aUid. The UID of item.
	* @param aNewParent. The UID of new parent of item.
	* @param aId. The ID of Data Provider.
	* @param aStoreName. The name of the data store that is used.
	* @param aResultCode. The result of method. KErrNone if everything goes correctly.
	*/
	void MoveItemL( TSmlDbItemUid aUid, TSmlDbItemUid aNewParent, const TSmlDataProviderId aId, const TDesC& aStoreName, TInt& aResultCode );
	
	/**
	* Deletes one item at Data Store permanently.
	* @param aUid. The UID of item.
	* @param aId. The ID of Data Provider.
	* @param aStoreName. The name of the data store that is used.
	* @param aResultCode. The result of method. KErrNone if everything goes correctly.
	*/
	void DeleteItemL( TSmlDbItemUid aUid, const TSmlDataProviderId aId, const TDesC& aStoreName, TInt& aResultCode ) const;
	
	/**
	* Soft deletes one item at Data Store.
	* @param aUid. The UID of item.
	* @param aId. The ID of Data Provider.
	* @param aStoreName. The name of the data store that is used.
	* @param aResultCode. The result of method. KErrNone if everything goes correctly.
	*/
	void SoftDeleteItemL( TSmlDbItemUid aUid, const TSmlDataProviderId aId, const TDesC& aStoreName, TInt& aResultCode ) const;
	
	/**
	* Deletes all items at Data Store permanently.
	* @param aId. The ID of Data Provider.
	* @param aStoreName. The name of the data store that is used.
	* @param aResultCode. The result of method. KErrNone if everything goes correctly.
	*/
	void DeleteAllItems( const TSmlDataProviderId aId, const TDesC& aStoreName, TInt& aResultCode ) const;
	
	/**
	* Checks if the Data Store has sync history. If not then slow sync is proposed to Sync Partner.
	* @param aId. The ID of Data Provider.
	* @param aStoreName. The name of the data store that is used.
	* @param aResultCode. The result of method. KErrNone if everything goes correctly.
	* @return TBool. ETrue, if Data Store has sync history, EFalse otherwise.
	*/
	TBool HasSyncHistory( const TSmlDataProviderId aId, const TDesC& aStoreName, TInt& aResultCode ) const;
	
	/**
	* The Data Provider returns UIDs of items that are added after previous synchronization.
	* If the Data Provider uses hierarchical synchronization then added folders must be placed
	* first (from root to leaves) to UID set and finally items.
	* @param aUidSet. The set of added items.
	* @param aId. The ID of Data Provider.
	* @param aStoreName. The name of the data store that is used.
	* @param aResultCode. The result of method. KErrNone if everything goes correctly.
	*/
	void AddedItemsL( RNSmlDbItemModificationSet& aUidSet, const TSmlDataProviderId aId, const TDesC& aStoreName, TInt& aResultCode ) const;
	
	/**
	* The Data Provider returns UIDs of items that are deleted after previous synchronization.
	* If the Data Provider uses hierarchical synchronization then deleted items must be placed
	* first to UID set and folders after items (from leaves to root).
	* @param aUidSet. The set of deleted items.
	* @param aId. The ID of Data Provider.
	* @param aStoreName. The name of the data store that is used.
	* @param aResultCode. The result of method. KErrNone if everything goes correctly.
	*/
	void DeletedItemsL( RNSmlDbItemModificationSet& aUidSet, const TSmlDataProviderId aId, const TDesC& aStoreName, TInt& aResultCode ) const;
	
	/**
	* The Data Provider returns UIDs of items that are soft deleted after previous synchronization.
	* If the Data Provider uses hierarchical synchronization then soft deleted items must be placed
	* first to UID set and folders after items (from leaves to root).
	* @param aUidSet. The set of soft deleted items.
	* @param aId. The ID of Data Provider.
	* @param aStoreName. The name of the data store that is used.
	* @param aResultCode. The result of method. KErrNone if everything goes correctly.
	*/
	void SoftDeleteItemsL( RNSmlDbItemModificationSet& aUidSet, const TSmlDataProviderId aId, const TDesC& aStoreName, TInt& aResultCode ) const;
	
	/**
	* The Data Provider returns UIDs of items that are modified after previous synchronization.
	* If the Data Provider uses hierarchical synchronization then modified folders must be placed
	* first (from root to leaves) to UID set and finally items.
	* @param aUidSet. The set of modified items.
	* @param aId. The ID of Data Provider.
	* @param aStoreName. The name of the data store that is used.
	* @param aResultCode. The result of method. KErrNone if everything goes correctly.
	*/
	void ModifiedItemsL( RNSmlDbItemModificationSet& aUidSet, const TSmlDataProviderId aId, const TDesC& aStoreName, TInt& aResultCode ) const;
	
	/**
	* The Data Provider returns UIDs of items that are moved after previous synchronization.
	* If the Data Provider uses hierarchical synchronization then moved folders must be placed
	* first (from root to leaves) to UID set and finally items.
	* @param aUidSet. The set of moved items.
	* @param aId. The ID of Data Provider.
	* @param aStoreName. The name of the data store that is used.
	* @param aResultCode. The result of method. KErrNone if everything goes correctly.
	*/
	void MovedItemsL( RNSmlDbItemModificationSet& aUidSet, const TSmlDataProviderId aId, const TDesC& aStoreName, TInt& aResultCode ) const;
			
	/**
	* The Data Provider returns UIDs of items that are added, deleted, modified, softdeleted or moved after previous synchronization.
	* @param aUidSet. The set of items.
	* @param aId. The ID of Data Provider.
	* @param aStoreName. The name of the data store that is used.
	* @param aResultCode. The result of method. KErrNone if everything goes correctly.
	*/
	void AllItemsL( RNSmlDbItemModificationSet& aUidSet, const TSmlDataProviderId aId, const TDesC& aStoreName, TInt& aResultCode ) const;
	
	/**
	* Reset change info from the Data Provider. The following synchronization will be slow sync.
	* @param aId. The ID of Data Provider.
	* @param aStoreName. The name of the data store that is used.
	* @param aResultCode. The result of method. KErrNone if everything goes correctly.
	*/
	void ResetChangeInfo( const TSmlDataProviderId aId, const TDesC& aStoreName, TInt& aResultCode ) const;
	
	/**
	* This method is called after some changes are synchronized to Sync Partner. If some changes
	* were synchronized correctly then those UIDs are included to aItems.
	* @param aItems. The UIDs of items that were correctly synchronized to Sync Partner.
	* @param aId. The ID of Data Provider.
	* @param aStoreName. The name of the data store that is used.
	* @param aResultCode. The result of method. KErrNone if everything goes correctly.
	*/
	void CommitChangeInfoL( const MSmlDataItemUidSet& aItems, const TSmlDataProviderId aId, const TDesC& aStoreName, TInt& aResultCode );
	
	/**
	* This method is called after some changes are synchronized to Sync Partner. This method is used if
	* all changes were synchronized correctly.
	* @param aId. The ID of Data Provider.
	* @param aStoreName. The name of the data store that is used.
	* @param aResultCode. The result of method. KErrNone if everything goes correctly.
	*/
	void CommitChangeInfo( const TSmlDataProviderId aId, const TDesC& aStoreName, TInt& aResultCode ) const;
	
	/**
	* Replaces the give old server ID with new one.
    * @param aOldServerId Old remote server ID.
	* @param aNewValue New remote server ID.
	* @param aResultCode. The result of method. KErrNone if everything goes correctly.
	*/
	void UpdateServerIdL( TDesC& aOldServerId, TDesC& aNewValue, TInt& aResultCode  );
	
private:
	/**
	* Starts Host Server.
	* @param aServerExeName. The name of server.
	* @return TInt. KErrNone or one of the system wide error codes.
	*/
	TInt LaunchServer( const TDesC& aServerName ) const;
	
	/**
	* write stream with dynamic buffer.
	* @return RWriteStream&. write stream to memory.
	*/
	TStreamBuffers* StreamBufferLC() const;
	
	/**
	* handles clean up of stream buffer.
	* @param aP. read stream.
	*/
	static void CleanupStreamBuffer( TAny* aP );
	
	/**
	* Ensures that chunk has at least required size or max size of reserved memory.
	* @param aRequiredSize. required size.
	* 
	*/
	void AdjustChunkLC( TInt aRequiredSize ) const;
	
	/**
	* Adjusts memory so that at least needed size is reserved.
	* @param iNeededSize. Memory needed for chunk in bytes.
	*/
	void AdjustChunkIfNeededLC( TInt iNeededSize ) const;
	
	/**
	* restores chunk memory.
	* @param aP. pointer to TMemPtr
	*/
	static void CancelAdjust( TAny* aP );
	
	/**
	* internalizes filters from stream
	* @param aStream. source stream
	* @param aFilters. filters read from stream.
	*/
	void InternalizeFiltersL( RReadStream& aStream, RPointerArray<CSyncMLFilter>& aFilters ) const;
	
	/**
	* externalizes filters to stream
	* @param aStream. target stream
	* @param aFilters. filters to write.
	*/
	void ExternalizeFiltersL( RWriteStream& aStream, const RPointerArray<CSyncMLFilter>& aFilters ) const;
	
	/**
	* creates filter from chunk
	* @param aFilter. filter created
	*/
	void GetFilterFromChunkL( CNSmlFilter*& aFilter );
	
	void AddFilterPropertiesL( const RPointerArray<CSmlDataProperty>& properties, CNSmlFilter& aFilter ) const;

private:

	mutable RChunk					iChunk;				// Handle to chunk that is used to transfer data between client and server.
	RPointerArray<TSmlDbItemUid> 	iAddedUidsBuffer;	// Added Uids are transferred back to client after Commit.
	TSmlDbItemUid*					iAddedUidBuffer;	// Buffer for normal add command.
	TInt							iMode;				// Normal, Batch or Transaction.
	TNSmlDSItemState				iItemState;
	};
	
#endif
	
// End of File
