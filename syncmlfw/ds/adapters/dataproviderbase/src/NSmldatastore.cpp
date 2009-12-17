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
* Description:  DS data store base
*
*/


#include <SmlDataProvider.h>
#include <SyncMLDef.h>		
#include <SyncMLDataFilter.h>
#include <ecom.h>


// ------------------------------------------------------------------------------------------------
// CSmlDataStore::OpenL
// Opens the created datastore
// ------------------------------------------------------------------------------------------------
EXPORT_C void CSmlDataStore::OpenL( const TDesC& aStoreName, MSmlSyncRelationship& aContext, TRequestStatus& aStatus )
{
	DoOpenL( aStoreName, aContext, aStatus );
}

// ------------------------------------------------------------------------------------------------
// CSmlDataStore::CancelRequest
// ------------------------------------------------------------------------------------------------
EXPORT_C void CSmlDataStore::CancelRequest()
{
	DoCancelRequest();
}

// ------------------------------------------------------------------------------------------------
// CSmlDataStore::StoreName
// ------------------------------------------------------------------------------------------------
EXPORT_C const TDesC& CSmlDataStore::StoreName() const
{
	return DoStoreName(); 
}

// ------------------------------------------------------------------------------------------------
// CSmlDataStore::BeginTransactionL
// ------------------------------------------------------------------------------------------------
EXPORT_C void CSmlDataStore::BeginTransactionL()
{
	DoBeginTransactionL();
}

// ------------------------------------------------------------------------------------------------
// CSmlDataStore::CommitTransactionL
// ------------------------------------------------------------------------------------------------
EXPORT_C void CSmlDataStore::CommitTransactionL( TRequestStatus& aStatus )
{
	DoCommitTransactionL( aStatus );
}

// ------------------------------------------------------------------------------------------------
// CSmlDataStore::RevertTransaction
// ------------------------------------------------------------------------------------------------
EXPORT_C void CSmlDataStore::RevertTransaction( TRequestStatus& aStatus )
{
	DoRevertTransaction( aStatus );
}

// ------------------------------------------------------------------------------------------------
// CSmlDataStore::BeginBatchL
// ------------------------------------------------------------------------------------------------
EXPORT_C void CSmlDataStore::BeginBatchL()
{
	DoBeginBatchL();
}

// ------------------------------------------------------------------------------------------------
// CSmlDataStore::CommitBatchL
// ------------------------------------------------------------------------------------------------
EXPORT_C void CSmlDataStore::CommitBatchL( RArray<TInt>& aResultArray, TRequestStatus& aStatus )
{
	DoCommitBatchL( aResultArray, aStatus );
}

// ------------------------------------------------------------------------------------------------
// CSmlDataStore::CancelBatch
// ------------------------------------------------------------------------------------------------
EXPORT_C void CSmlDataStore::CancelBatch()
{
	DoCancelBatch();
}

// ------------------------------------------------------------------------------------------------
// CSmlDataStore::SetRemoteStoreFormatL
// ------------------------------------------------------------------------------------------------
EXPORT_C void CSmlDataStore::SetRemoteStoreFormatL( const CSmlDataStoreFormat& aServerDataStoreFormat )
{
	DoSetRemoteStoreFormatL( aServerDataStoreFormat );
}

// ------------------------------------------------------------------------------------------------
// CSmlDataStore::SetRemoteMaxObjectSize
// ------------------------------------------------------------------------------------------------
EXPORT_C void CSmlDataStore::SetRemoteMaxObjectSize( TInt aServerMaxObjectSize )
{
	DoSetRemoteMaxObjectSize( aServerMaxObjectSize );
}

// ------------------------------------------------------------------------------------------------
// CSmlDataStore::MaxObjectSize
// ------------------------------------------------------------------------------------------------
EXPORT_C TInt CSmlDataStore::MaxObjectSize() const
{
	return DoMaxObjectSize();
}

// ------------------------------------------------------------------------------------------------
// CSmlDataStore::OpenItemL
// ------------------------------------------------------------------------------------------------
EXPORT_C void CSmlDataStore::OpenItemL( TSmlDbItemUid aUid, TBool& aFieldChange, TInt& aSize, TSmlDbItemUid& aParent, TDes8& aMimeType, TDes8& aMimeVer, TRequestStatus& aStatus )
{
	DoOpenItemL( aUid, aFieldChange, aSize, aParent, aMimeType, aMimeVer, aStatus );
}

// ------------------------------------------------------------------------------------------------
// CSmlDataStore::CreateItemL
// ------------------------------------------------------------------------------------------------
EXPORT_C void CSmlDataStore::CreateItemL( TSmlDbItemUid& aUid, TInt aSize, TSmlDbItemUid aParent, const TDesC8& aMimeType, const TDesC8& aMimeVer, TRequestStatus& aStatus )
{
	DoCreateItemL( aUid, aSize, aParent, aMimeType, aMimeVer, aStatus );
}

// ------------------------------------------------------------------------------------------------
// CSmlDataStore::ReplaceItemL
// ------------------------------------------------------------------------------------------------
EXPORT_C void CSmlDataStore::ReplaceItemL(TSmlDbItemUid aUid, TInt aSize, TSmlDbItemUid aParent, TBool aFieldChange, TRequestStatus& aStatus)
{
	DoReplaceItemL( aUid, aSize, aParent, aFieldChange, aStatus );
}

// ------------------------------------------------------------------------------------------------
// CSmlDataStore::ReadItemL
// ------------------------------------------------------------------------------------------------
EXPORT_C void CSmlDataStore::ReadItemL( TDes8& aBuffer )
{
	DoReadItemL( aBuffer );
}

// ------------------------------------------------------------------------------------------------
// CSmlDataStore::WriteItemL
// ------------------------------------------------------------------------------------------------
EXPORT_C void CSmlDataStore::WriteItemL( const TDesC8& aData )
{
	DoWriteItemL( aData );
}

// ------------------------------------------------------------------------------------------------
// CSmlDataStore::CommitItemL
// ------------------------------------------------------------------------------------------------
EXPORT_C void CSmlDataStore::CommitItemL( TRequestStatus& aStatus )
{
	DoCommitItemL( aStatus );
}

// ------------------------------------------------------------------------------------------------
// CSmlDataStore::CloseItem
// ------------------------------------------------------------------------------------------------
EXPORT_C void CSmlDataStore::CloseItem()
{
	DoCloseItem();
}

// ------------------------------------------------------------------------------------------------
// CSmlDataStore::MoveItemL
// ------------------------------------------------------------------------------------------------
EXPORT_C void CSmlDataStore::MoveItemL( TSmlDbItemUid aUid, TSmlDbItemUid aNewParent, TRequestStatus& aStatus )
{
	DoMoveItemL( aUid, aNewParent, aStatus );
}

// ------------------------------------------------------------------------------------------------
// CSmlDataStore::DeleteItemL
// ------------------------------------------------------------------------------------------------
EXPORT_C void CSmlDataStore::DeleteItemL( TSmlDbItemUid aUid, TRequestStatus& aStatus )
{
	DoDeleteItemL( aUid, aStatus );
}

// ------------------------------------------------------------------------------------------------
// CSmlDataStore::SoftDeleteItemL
// ------------------------------------------------------------------------------------------------
EXPORT_C void CSmlDataStore::SoftDeleteItemL( TSmlDbItemUid aUid, TRequestStatus& aStatus )
{
	DoSoftDeleteItemL( aUid, aStatus );
}

// ------------------------------------------------------------------------------------------------
// CSmlDataStore::DeleteAllItemsL
// ------------------------------------------------------------------------------------------------
EXPORT_C void CSmlDataStore::DeleteAllItemsL( TRequestStatus& aStatus )
{
	DoDeleteAllItemsL( aStatus );
}

// ------------------------------------------------------------------------------------------------
// CSmlDataStore::HasSyncHistory
// ------------------------------------------------------------------------------------------------
EXPORT_C TBool CSmlDataStore::HasSyncHistory() const
{
	return DoHasSyncHistory();
}

// ------------------------------------------------------------------------------------------------
// CSmlDataStore::AddedItems
// ------------------------------------------------------------------------------------------------
EXPORT_C const MSmlDataItemUidSet& CSmlDataStore::AddedItems() const
{
	return DoAddedItems();
}

// ------------------------------------------------------------------------------------------------
// CSmlDataStore::DeletedItems
// ------------------------------------------------------------------------------------------------
EXPORT_C const MSmlDataItemUidSet& CSmlDataStore::DeletedItems() const
{
	return DoDeletedItems();
}

// ------------------------------------------------------------------------------------------------
// CSmlDataStore::SoftDeletedItems
// ------------------------------------------------------------------------------------------------
EXPORT_C const MSmlDataItemUidSet& CSmlDataStore::SoftDeletedItems() const
{
	return DoSoftDeletedItems();
}

// ------------------------------------------------------------------------------------------------
// CSmlDataStore::ModifiedItems
// ------------------------------------------------------------------------------------------------
EXPORT_C const MSmlDataItemUidSet& CSmlDataStore::ModifiedItems() const
{
	return DoModifiedItems();
}

// ------------------------------------------------------------------------------------------------
// CSmlDataStore::MovedItems
// ------------------------------------------------------------------------------------------------
EXPORT_C const MSmlDataItemUidSet& CSmlDataStore::MovedItems() const
{
	return DoMovedItems();
}

// ------------------------------------------------------------------------------------------------
// CSmlDataStore::ResetChangeInfoL
// ------------------------------------------------------------------------------------------------
EXPORT_C void CSmlDataStore::ResetChangeInfoL( TRequestStatus& aStatus )
{
	DoResetChangeInfoL( aStatus );
}

// ------------------------------------------------------------------------------------------------
// CSmlDataStore::CommitChangeInfoL
// ------------------------------------------------------------------------------------------------
EXPORT_C void CSmlDataStore::CommitChangeInfoL( TRequestStatus& aStatus, const MSmlDataItemUidSet& aItems )
{
	DoCommitChangeInfoL( aStatus, aItems );
}

// ------------------------------------------------------------------------------------------------
// CSmlDataStore::CommitChangeInfoL
// ------------------------------------------------------------------------------------------------
EXPORT_C void CSmlDataStore::CommitChangeInfoL( TRequestStatus& aStatus )
{
	DoCommitChangeInfoL( aStatus );
}

// End of File
