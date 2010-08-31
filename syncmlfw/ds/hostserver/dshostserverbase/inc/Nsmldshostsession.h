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
* Description:  Session for DS host server.
*
*/


#ifndef __NSMLDSHOSTSERVERSESSION_H__
#define __NSMLDSHOSTSERVERSESSION_H__

// ------------------------------------------------------------------------------------------------
// Includes
// ------------------------------------------------------------------------------------------------
#include <e32base.h>
#include <stringpool.h>

#include <nsmldsconstants.h>
#include <nsmlconstants.h>
#include "nsmldshostconstants.h"
#include "nsmldshostitem.h"
#include "nsmldsitemmodificationset.h"
#include "Nsmldsasyncrequesthandler.h"
#include "Nsmldsdataproviderarray.h"

// ------------------------------------------------------------------------------------------------
// Class forwards
// ------------------------------------------------------------------------------------------------
class CSyncMLFilter;
class CSmlDataProvider;
class CSmlDataStore;

// ------------------------------------------------------------------------------------------------
// Class declarations
// ------------------------------------------------------------------------------------------------ 
// ------------------------------------------------------------------------------------------------
// class CNSmlDSHostSession
//
// @lib nsmldshostserver.lib
// ------------------------------------------------------------------------------------------------
class CNSmlDSHostSession : public CSession2
	{
private:	//DATA TYPES and INNER CLASSES
	typedef CNSmlDSAsyncCallBack::TCallBackOperation TCallBackOperation;
	
	// ------------------------------------------------------------------------------------------------
	// reads from and writes to memory through pointer.
	//
	// @lib nsmldshostserver.lib
	// ------------------------------------------------------------------------------------------------
	class TMemPtr
	    {
	public:
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
			
	public: //Constructor

		TMemPtr();
	private:
	    TMemPtr( const TMemPtr& op );
	public:

		/**
		* pointer to memory
		* @return TPtr8. Pointer to memory.
		*/
	    TPtr8 Des();
	    
		/**
		* pointer to memory area.
		* @param aPos. Start position.
		* @param aLength. Length of memory area.
		* @return TPtr8. Pointer to memory.
		*/    
	    TPtr8 Mid( TInt aPos, TInt aLength );
	    
	    /**
		* copies memory to aDest buffer.
		* @param aDest. destination for copy.
		*/
	    void CopyTo(TDes8& aDest) const;
	    
	    /**
		* write stream to memory.
		* @return RWriteStream&. write stream to memory.
		*/
	    RWriteStream& WriteStreamLC() const;
	    
   	    /**
		* write stream with dynamic buffer.
		* @return RWriteStream&. write stream to memory.
		*/
	    TStreamBuffers* StreamBufferLC() const;
	    
	    /**
		* read stream to memory.
		* @return RReadStream&. read stream to memory.
		*/
	    RReadStream& ReadStreamLC() const;
	    
	    /**
		* Adjusts memory so that at least needed size is reserved.
		* @param iNeededSize. Memory needed for chunk in bytes.
		*/
	    void AdjustChunkIfNeededL( TInt iNeededSize );
	    
	    /**
		* size of memory area.
		* @return TInt. size of memory area.
		*/
	    TInt Size() const;
	private:
	
		/**
		* Changes the number of bytes committed to the chunk
		* @param aNewSize. The number of bytes to be committed to this chunk
		* @return TInt. status code
		*/
		TInt Adjust( TInt aNewSize );

		/**
		* handles clean up of write stream.
		* @param aP. write stream.
		*/
	    static void CleanupWriteStream( TAny* aP );
	    
		/**
		* handles clean up of read stream.
		* @param aP. read stream.
		*/    
	    static void CleanupReadStream( TAny* aP );
	    
		/**
		* handles clean up of stream buffer.
		* @param aP. read stream.
		*/
	    static void CleanupStreamBuffer( TAny* aP );
	    
   		/**
		* restores chunk memory.
		* @param aP. pointer to TMemPtr
		*/
	    static void CancelAdjust( TAny* aP );
	    
		/**
		* modifiable pointer to memory.
		* @return TUint8*. pointer to memory.
		*/    
	    TUint8* Ptr8();
	    
		/**
		* const pointer to memory.
		* @return const TUint8*. read pointer to memory.
		*/    
	    const TUint8* Ptr8() const;

		/**
		* modifiable pointer to memory.
		* @return TAny*. pointer to memory.
		*/    
	    TAny* Ptr() const;
	    
	public:
		RChunk iChunk;
		
#ifdef __HOST_SERVER_MTEST__
		TInt iChunkSize;
#endif
	    };
	    
	typedef CNSmlDSHostSession::TMemPtr::TStreamBuffers TStreamBuffers;
	    
	// ------------------------------------------------------------------------------------------------
	// for transporting item parameters over IPC.
	//
	// @lib nsmldshostserver.lib
	// ------------------------------------------------------------------------------------------------
	class CNSmlServerDSHostItem : public CNSmlDSHostItem
		{
	public:	//Constructor

	    /**
	    * Two-phased constructor.
	    */
		static CNSmlServerDSHostItem* NewLC();
	public: //New functions

		/**
		* reference to uid member
		* @return TSmlDbItemUid&. reference to uid
		*/
		TSmlDbItemUid& Uid();
		
		/**
		* reference to fieldchange member.
		* @return TBool&. reference to fieldchange member.
		*/	
		TBool& FieldChange();
		
		/**
		* reference to size member.
		* @return TInt&. reference to size member.
		*/
		TInt& Size();
		
		/**
		* reference to parent uid member.
		* @return TSmlDbItemUid&. reference to parent uid member.
		*/	
		TSmlDbItemUid& ParentUid();
	public:		//Data

		TBuf8<64> iMimeTypePtr;
		TBuf8<64> iMimeVerPtr;
		TSmlDbItemUid* iCreateItemUid; //uid for CSmlDataStore::CreateItemL
		};
	
public: //constructors and destructors

    /**
	* Creates new instance of CNSmlDSHostSession.
	* @param aServer. the server instance, owner of session.
	* @return CNSmlDSHostSession*. Created instance.
	*/
	static CNSmlDSHostSession* NewL( CNSmlDSHostServer& aServer);
	~CNSmlDSHostSession();
	
    /**
	* Handles the servicing of a client request that has been passed to the server.
	* @param aMessage. message to service.
	*/
public:	//Functions from base classes.

	void ServiceL( const RMessage2 &aMessage );
	
private:	// New methods
	
    /**
	* Checks the message, then calls the correct method to service the message.
	* @param aMessage. Message to dispatch. 
	* @param aCompleteRequest. set to ETrue, if message should be completed after calling this method.
	* EFalse, if request is asynchronous, and completion is done by an active object.
	*/	
	void DispatchMessageL( const RMessage2 &aMessage, TBool& aCompleteRequest );
	
	/**
	* C++ constructor.
	* @param aServer.the server instance, owner of session.
	*/
	CNSmlDSHostSession( CNSmlDSHostServer& aServer);

    /**        
    * Symbian 2nd phase constructor
    */
	void ConstructL();
		
    /**
	* saves shared memory chunk handle from client for later use.
	* @param aMessage.Message to service.
	*/
	void HandleChunk( const RMessage2& aMessage );
	
    /**
	* Creates data providers. Sends status code for each to client.
	* @param aMessage. Message to service.
	*/
    void CreateDataProvidersL( const RMessage2& aMessage );
    
    /**
	* Creates data providers except those sent from client. Sends created data provider ids to client.
	* @param aMessage. Message to service.
	*/
    void CreateDataProvidersExceptL( const RMessage2& aMessage );
    
    /**
	* Creates one data provider, stores it, if creation succeeds.
	* @param aId. Data provider id to data provider to create.
	*/
    void CreateDataProviderL(TSmlDataProviderId aId);
    
    /**
	* Sends Data provider information to client.
	* @param aMessage. Message to service.
	*/
    void DPInformationL( const RMessage2& aMessage );
        
    /**
	* Calls CSmlDataProvider::SupportsOperation() and sends the result to client.
	* @param aMessage. Message to service.
	*/
    void SupportsOperationL( const RMessage2& aMessage );
    
    /**
	* Calls CSmlDataProvider::StoreFormatL() and sends the result to client.
	* @param aMessage. Message to service.
	*/
    void StoreFormatL( const RMessage2& aMessage );
    
    /**
	* Calls CSmlDataProvider::ListStoresLC() and sends the result to client.
	* @param aMessage. Message to service.
	*/
    void ListStoresL( const RMessage2& aMessage );
    
    /**
	* Calls CSmlDataProvider::DefaultStoreL() and sends the result to client.
	* @param aMessage. Message to service.
	*/
    void DefaultStoreL( const RMessage2& aMessage );
    
    /**
	* Calls CSmlDataProvider::SupportedServerFiltersL().
	* @param aMessage. Message to service.
	*/
    void SupportedServerFiltersL( const RMessage2& aMessage );
    
    /**
	* Calls CSmlDataProvider::CheckServerFiltersL().
	* @param aMessage. Message to service.
	*/
    void CheckServerFiltersL( const RMessage2& aMessage );
    
    /*
    * Calls CSmlDataProvider::CheckSupportedServerFiltersL.
    * @param aMessage. Message to service.
    */
    void CheckSupportedServerFiltersL( const RMessage2& aMessage );
    
    /**
	* get filters
	* @param aMessage. Message to service.
	*/    
    void GetFiltersL( const RMessage2& aMessage );
    
    /**
	* Opens the data store requested in message.
	* @param aMessage. Message to service.If open succeeds,
	* the data store is stored for later use.
	*/
    void OpenL( const RMessage2& aMessage );
    
    /**
	* Called after OpenL request completes. 
	* @param aDSAO. object, that handled the request.
	* @param aOperation. Operation to perform.
	*/
    TInt OpenFinishedL( CNSmlDSAsyncCallBack* aDSAO, TCallBackOperation aOperation );
    
    /**
	* Calls CSmlDataStore::CancelRequest.
	* @param aMessage.
	*/
    void CancelRequestL( const RMessage2& aMessage );
    
    /**
	* Calls CSmlDataStore::BeginTransactionL
	* @param aMessage.
	*/    
    void BeginTransactionL( const RMessage2& aMessage );
    
    /**
	* Calls asynchronously CSmlDataStore::CommitTransactionL
	* @param aMessage.
	*/        
    void CommitTransactionL( const RMessage2& aMessage );
    
    /**
	* Calls asynchronously CSmlDataStore::RevertTransaction
	* @param aMessage.
	*/
    void RevertTransactionL( const RMessage2& aMessage );
    
    /**
	* Calls CSmlDataStore::BeginBatchL
	* @param aMessage.
	*/
    void BeginBatchL( const RMessage2& aMessage );
    
    /**
	* Calls asynchronously CSmlDataStore::CommitBatchL. 
	* @param aMessage.
	*/
    void CommitBatchL( const RMessage2& aMessage );
    
    /**
	* Called after CommitBatchL request completes. 
	* @param aDSAO. object, that handled the request.
	* @param aOperation. Operation to perform.
	*/
    TInt CommitBatchRequestFinishedL( CNSmlDSAsyncCallBack* aDSAO, TCallBackOperation aOperation );
    
    /**
	* Calls CSmlDataStore::CancelBatch
	* @param aMessage.
	*/    
    void CancelBatchL( const RMessage2& aMessage );
    
    /**
	* Calls CSmlDataStore::SetRemoteStoreFormatL
	* @param aMessage.
	*/
    void SetRemoteDataStoreFormatL( const RMessage2& aMessage );
    
    /**
	* Calls CSmlDataStore::SetRemoteMaxObjectSize
	* @param aMessage.
	*/    
    void SetRemoteMaxObjectSizeL( const RMessage2& aMessage );
    
    /**
	* Calls CSmlDataStore::MaxObjectSize
	* @param aMessage.
	*/
    void MaxObjectSizeL( const RMessage2& aMessage );
    
    /**
	* Calls asynchronously CSmlDataStore::OpenItemL(). 
	* @param aMessage.
	*/
    void OpenItemL( const RMessage2& aMessage );
    
    /**
	* Called after OpenItemL request completes. 
	* @param aDSAO. object, that handled to request.
	* @param aOperation. Operation to perform.
	*/
    TInt OpenItemRequestFinishedL( CNSmlDSAsyncCallBack* iDSAO, TCallBackOperation aOperation );
    
    /**
	* Creates an CNSmlServerDSHostItem instance with values read from shared memory chunk.
	* Leaves object to cleanup stack.
	* @return CNSmlServerDSHostItem*. Instance created.
	*/
    CNSmlServerDSHostItem* DataStoreItemParamsLC();
    
    /**
	* write data store items to shared memoy chunk.
	* @param dshi. Object, that contains data store item - variables.
	*/    
    void WriteDataStoreItemParamsL( CNSmlDSHostItem* dshi );
    
    /**
	* Calls asynchronously CSmlDataStore::CreateItemL. 
	* @param aMessage.
	*/
    void CreateItemL( const RMessage2& aMessage );
    
    /**
	* Called after CreateItemL request completes. 
	* @param aDSAO. object, that handled the request.
	* @param aOperation. Operation to perform.
	*/    
    TInt CreateItemRequestFinishedL( CNSmlDSAsyncCallBack* aDSAO, TCallBackOperation aOperation );
    
    /**
	* Calls asynchronously CSmlDataStore::ReplaceItemL
	* @param aMessage.
	*/
    void ReplaceItemL( const RMessage2& aMessage );
    
    /**
	* Calls CSmlDataStore::ReadItemL
	* @param aMessage.
	*/
    void ReadItemL( const RMessage2& aMessage );
    
    /**
	* Calls CSmlDataStore::WriteItemL
	* @param aMessage.
	*/
    void WriteItemL( const RMessage2& aMessage );
    
    /**
	* Calls CSmlDataStore::CommitItemL. 
	* @param aMessage.
	*/
    void CommitItemL( const RMessage2& aMessage );
    
    /**
	* Called after CommitItemL request completes. 
	* @param aDSAO. object, that handled the request.
	* @param aOperation. Operation to perform.
	*/
    TInt CommitItemRequestFinishedL( CNSmlDSAsyncCallBack* aDSAO, TCallBackOperation aOperation );
    
    /**
	* Calls CSmlDataStore::CloseItem
	* @param aMessage.
	*/
    void CloseItemL( const RMessage2& aMessage );
    
    /**
	* Calls asynchronously CSmlDataStore::MoveItemL
	* @param aMessage.
	*/
    void MoveItemL( const RMessage2& aMessage );
    
    /**
	* Calls asynchronously CSmlDataStore::DeleteItemL
	* @param aMessage.
	*/
    void DeleteItemL( const RMessage2& aMessage );
    
    /**
	* Calls asynchronously CSmlDataStore::SoftDeleteItemL
	* @param aMessage.
	*/
    void SoftDeleteItemL( const RMessage2& aMessage );
    
    /**
	* Calls asynchronously CSmlDataStore::DeleteAllItemsL
	* @param aMessage.
	*/
    void DeleteAllItemsL( const RMessage2& aMessage );
    
    /**
	* Calls CSmlDataStore::HasSyncHistory
	* @param aMessage.
	*/
    void HasSyncHistoryL( const RMessage2& aMessage );
    
    /**
	* Calls CSmlDataStore::AddedItems
	* @param aMessage.
	*/
    void AddedItemsL( const RMessage2& aMessage );
    
    /**
	* Calls CSmlDataStore::DeletedItems
	* @param aMessage.
	*/
    void DeletedItemsL( const RMessage2& aMessage );
    
    /**
	* Calls CSmlDataStore::SoftDeletedItems
	* @param aMessage.
	*/    
    void SoftDeleteItemsL( const RMessage2& aMessage );
    
    /**
	* Calls CSmlDataStore::ModifiedItems
	* @param aMessage.
	*/    
    void ModifiedItemsL( const RMessage2& aMessage );
    
    /**
	* Calls CSmlDataStore::MovedItems
	* @param aMessage.
	*/
    void MovedItemsL( const RMessage2& aMessage );
    
    /**
	* calls asynchronously MovedItems, ModifiedItems, SoftDeletedItems, DeletedItems, AddedItems
	* @param aMessage.
	*/
    void AllItemsL( const RMessage2& aMessage );
    
    /**
	* Called after OpenL request completes. 
	* @param aDSAO. object, that handled the request.
	*/
    void AllItemsRequestFinishedL( CNSmlDSChangedItemsFetcher* aDSAO );
    
    /**
	* Calls CSmlDataStore:: ResetChangeInfoL
	* @param aMessage.
	*/    
    void ResetChangeInfoL( const RMessage2& aMessage );
    
    /**
	* Calls asynchronously CSmlDataStore::CommitChangeInfoL
	* @param aMessage.
	*/
    void CommitChangesL( const RMessage2& aMessage);
    
    /**
	* Called after CommitChangesL request completes. 
	* @param aDSAO. object, that handled the request.
	* @param aOperation. Operation to perform.
	*/
    TInt CommitChangesRequestFinishedL( CNSmlDSAsyncCallBack* aDSAO, TCallBackOperation aOperation );
    
    /**
	* Calls asynchronously CSmlDataStore::CommitChangeInfoL
	* @param aMessage.
	*/    
    void CommitAllChangesL( const RMessage2& aMessage );
    
    /**
    * Updates server id to adapter log.
    * @param aMessage.
    */
    void UpdateServerIdL( const RMessage2& aMessage );

    /**
	* searches data provider using its aId. 
	* @param aId. Data provider id to data provider to search. Leaves if not found.
	* @return CSmlDataProvider*. Data provider found. 
	*/
    CSmlDataProvider* DataProviderL( TSmlDataProviderId aId );
    
    /**
	* searches data provider item using data provider id. 
	* @param aId. Data provider id to data provider to search. Leaves if not found.
	* @return TNSmlDSDataProviderElement*. Data provider found. 
	*/
    TNSmlDSDataProviderElement* DataProviderItemL( TSmlDataProviderId aId );
    
    /**
	* finds data store from aMessage. Leaves if not found.
	* @param aMessage.
	* @return CSmlDataStore*. The data store requested in aMessage.
	*/
    CSmlDataStore* DataStoreL( const RMessage2& aMessage );
    
    /**
	* finds data store element from aMessage. Leaves if not found.
	* @param aMessage.
	* @return TNSmlDSDataStoreElement*. The data store requested in aMessage.
	*/
    TNSmlDSDataStoreElement* DataStoreItemL( const RMessage2& aMessage );
    
    /**
	* Is data store created.
	* @param aMessage. Message, that contains the data store. 
	* @return ETrue if data store in aMessage is created, EFalse if not.
	*/
    TBool HasDataStoreL( const RMessage2& aMessage );
    
    /**
	* Retrieves data store name from message.
	* @param aMessage.
	* @return HBufC*. Data store name from aMessage.
	*/
	static HBufC* DataStoreNameLC( const RMessage2& aMessage );
	
    /**
	* Read filters from stream.
	* @param aStream. stream, where filters are read.
	* @param aFilters. Receives filters read from stream.
	*/
	static void InternalizeFiltersL( RReadStream& aStream, RPointerArray<CSyncMLFilter>& aFilters );
	
    /**
	* Write filters to stream.
	* @param aStream. Stream to write
	* @param aFilters. filters to write.
	*/
	static void ExternalizeFiltersL( RWriteStream& aStream, const RPointerArray<CSyncMLFilter>& aFilters );
	

    /**
	* Panics the client
	* @param aMessage. Message from client to panic.
	* @param aReason. Reason code.
	*/
    void PanicClient( const RMessage2& aMessage, TInt aReason ) const;
    
    /**
	* returns shared memory chunk pointer.
	* @return shared memory chunk pointer.
	*/
    TMemPtr& MemPtrL();

    /**
	* copies data from stream buffer to chunk
	* @param aMemPtr. pointer to chunk
	* @param aSb. contains data to copy
	*/
    void StreamBufferToChunkL( TMemPtr& aMemPtr, TStreamBuffers* aSb );
    
private:	//Data

	TMemPtr iMemPtr;
	CNSmlDSHostServer& iServer;
    RNSmlDSDataProviderArray iDataProviders;
    RStringPool iStringPool;
	};
	
#include "Nsmldshostsession.inl"
#endif // __NSMLDSHOSTSERVERSESSION_H__

// End of File
