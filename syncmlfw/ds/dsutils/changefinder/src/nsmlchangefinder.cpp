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
* Description:  Sources
*
*/


// INCLUDE FILES
#include <s32strm.h>

#include <nsmldebug.h>
#include <nsmlchangefinder.h>

#ifndef __WINS__
// This lowers the unnecessary compiler warning (armv5) to remark.
// "Warning:  #174-D: expression has no effect..." is caused by 
// DBG_ARGS8 macro in no-debug builds.
#pragma diag_remark 174
#endif

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CNSmlChangeFinder::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CNSmlChangeFinder* CNSmlChangeFinder::NewL( MSmlSyncRelationship& aSyncRelationship, TKeyArrayFix aKey, TBool& aHasHistory, TInt aStreamUid )
	{
	_DBG_FILE("CNSmlChangeFinder::NewL(): begin");
	CNSmlChangeFinder* self = new (ELeave) CNSmlChangeFinder(aSyncRelationship, aKey, aStreamUid);
	CleanupStack::PushL(self);
	self->ConstructL( aHasHistory );
	CleanupStack::Pop(); // self
	_DBG_FILE("CNSmlChangeFinder::NewL(): end");
	return self;
	}

// -----------------------------------------------------------------------------
// CNSmlChangeFinder::CNSmlChangeFinder
// C++ default constructor.
// -----------------------------------------------------------------------------
//
CNSmlChangeFinder::CNSmlChangeFinder( MSmlSyncRelationship& aSyncRelationship, TKeyArrayFix aKey, TInt aStreamUid ) : iSyncRelationship(aSyncRelationship), iKey(aKey), iStreamUid(aStreamUid)
	{
	_DBG_FILE("CNSmlChangeFinder::CNSmlChangeFinder(): begin");
	_DBG_FILE("CNSmlChangeFinder::CNSmlChangeFinder(): end");
	}

// -----------------------------------------------------------------------------
// CNSmlChangeFinder::ConstructL
// Symbian 2nd phase constructor.
// -----------------------------------------------------------------------------
//
void CNSmlChangeFinder::ConstructL( TBool& aHasHistory )
	{
	_DBG_FILE("CNSmlChangeFinder::ConstructL(): begin");
	iOldSnapshot = new (ELeave) CArrayFixSeg<TNSmlSnapshotItem>(sizeof(TNSmlSnapshotItem));
	iCurrentSnapshot = new (ELeave) CArrayFixSeg<TNSmlSnapshotItem>(sizeof(TNSmlSnapshotItem));
	TUid uid = {iStreamUid};
	iDataStoreUid = KErrNotFound;
		
	aHasHistory = iSyncRelationship.IsStreamPresentL(uid);
	
	if (aHasHistory)
	    {
	    // Open stream for reading
	    RReadStream readStream;
        iSyncRelationship.OpenReadStreamLC(readStream, uid);
        
        // Read snapshot from the stream
        TNSmlSnapshotItem item;
        TInt itemCount(readStream.ReadInt32L());
        for (TInt i = 0; i < itemCount; ++i)
            {
            readStream >> item;
            iOldSnapshot->InsertIsqL(item, iKey);
            }
            
        // Read store UID from the stream
        readStream.ReadL( ( TUint8* ) &iDataStoreUid, 8 );
        iOldSnapshot->Compress();
        
        CleanupStack::PopAndDestroy(); // readStream
	    }
	
	
	_DBG_FILE("CNSmlChangeFinder::ConstructL(): end");
	}

// -----------------------------------------------------------------------------
// CNSmlChangeFinder::~CNSmlChangeFinder
// -----------------------------------------------------------------------------
//
EXPORT_C CNSmlChangeFinder::~CNSmlChangeFinder()
	{
	_DBG_FILE("CNSmlChangeFinder::~CNSmlChangeFinder(): begin");
	delete iOldSnapshot;
	delete iCurrentSnapshot;
	_DBG_FILE("CNSmlChangeFinder::~CNSmlChangeFinder(): end");
	}

// -----------------------------------------------------------------------------
// CNSmlChangeFinder::CloseL
// -----------------------------------------------------------------------------
//
EXPORT_C void CNSmlChangeFinder::CloseL()
	{
	_DBG_FILE("CNSmlChangeFinder::CloseL(): begin");
		
	TUid uid = {iStreamUid};
	RWriteStream writeStream;
	
	// Open stream for writing
    iSyncRelationship.OpenWriteStreamLC(writeStream, uid);
	
	// Write the snapshot to the stream
    TInt itemCount = iOldSnapshot->Count();
    writeStream.WriteInt32L(itemCount);
    for (TInt i = 0; i < itemCount; ++i)
        {
        writeStream << iOldSnapshot->At(i);
        }
        
    // Write store UID to the stream
    writeStream.WriteL( ( TUint8* ) &iDataStoreUid, 8 );
    writeStream.CommitL();
    
    CleanupStack::PopAndDestroy(); // writeStream
	
	_DBG_FILE("CNSmlChangeFinder::CloseL(): end");
	}

// -----------------------------------------------------------------------------
// CNSmlChangeFinder::ResetL
// -----------------------------------------------------------------------------
//
EXPORT_C void CNSmlChangeFinder::ResetL()
	{
	_DBG_FILE("CNSmlChangeFinder::ResetL(): begin");
	iOldSnapshot->Reset();
	if ( iCurrentSnapshot )
		{
		iCurrentSnapshot->Reset();
		}

    // When called after reseting old snapshot, CloseL() externalizes the empty snaphot
    // to used stream.
    CloseL();
    	
	_DBG_FILE("CNSmlChangeFinder::ResetL(): end");
	}

// -----------------------------------------------------------------------------
// CNSmlChangeFinder::FindChangedItemsL
// -----------------------------------------------------------------------------
//
EXPORT_C void CNSmlChangeFinder::FindChangedItemsL( CNSmlDataItemUidSet& aChangedUids )
	{
	_DBG_FILE("CNSmlChangeFinder::FindChangedItemsL(): begin");
	TNSmlSnapshotItem current;
	TInt index;

	aChangedUids.Reset();
	for ( TInt i = 0; i < iCurrentSnapshot->Count(); i++ )
		{
		current = iCurrentSnapshot->At(i);

		// Is this also found in old snapshot?
		if ( !iOldSnapshot->FindIsq(current, iKey, index) )
			{
			// It is -> has it been changed?
			TNSmlSnapshotItem old = iOldSnapshot->At(index);
			if ( current.Compare(old) > 0 && current.SoftDelete() == EFalse )
				{
				// Yep -> add to list
				User::LeaveIfError(aChangedUids.AddItem(current.ItemId()));
				}
			}
		}

	DBG_ARGS(_S("CNSmlChangeFinder::FindChangedItemsL(): Number of changes = %d"), aChangedUids.ItemCount());

	_DBG_FILE("CNSmlChangeFinder::FindChangedItemsL(): end");
	}

// -----------------------------------------------------------------------------
// CNSmlChangeFinder::FindDeletedItemsL
// -----------------------------------------------------------------------------
//
EXPORT_C void CNSmlChangeFinder::FindDeletedItemsL( CNSmlDataItemUidSet& aDeletedUids )
	{
	_DBG_FILE("CNSmlChangeFinder::FindDeletedItemsL(): begin");
	TNSmlSnapshotItem old;
	TInt index;
	
	aDeletedUids.Reset();
	for ( TInt i = 0; i < iOldSnapshot->Count(); i++ )
		{
		old = iOldSnapshot->At(i);
		// Does this old item appear in current snapshot?
		if ( iCurrentSnapshot->FindIsq(old, iKey, index) )
			{
			// No -> add to list
			User::LeaveIfError(aDeletedUids.AddItem(old.ItemId()));
			}
		}
	
	DBG_ARGS(_S("CNSmlChangeFinder::FindDeletedItemsL(): Number of changes = %d"), aDeletedUids.ItemCount());

	_DBG_FILE("CNSmlChangeFinder::FindDeletedItemsL(): end");
	}

// -----------------------------------------------------------------------------
// CNSmlChangeFinder::FindNewItems
// -----------------------------------------------------------------------------
//
EXPORT_C void CNSmlChangeFinder::FindNewItemsL( CNSmlDataItemUidSet& aNewUids )
	{
	_DBG_FILE("CNSmlChangeFinder::FindNewItemsL(): begin");
	TNSmlSnapshotItem current;
	TInt index;

	aNewUids.Reset();
	for ( TInt i = 0; i < iCurrentSnapshot->Count(); i++ )
		{
		current = iCurrentSnapshot->At(i);
		
		// Is this also found in old snapshot?
		if ( iOldSnapshot->FindIsq(current, iKey, index) )
			{
			// No -> add to list
			User::LeaveIfError(aNewUids.AddItem(current.ItemId()));
			}
		}

	DBG_ARGS(_S("CNSmlChangeFinder::FindNewItemsL(): Number of changes = %d"), aNewUids.ItemCount());
	
	_DBG_FILE("CNSmlChangeFinder::FindNewItemsL(): end");
	}

// -----------------------------------------------------------------------------
// CNSmlChangeFinder::FindMovedItemsL
// -----------------------------------------------------------------------------
//
EXPORT_C void CNSmlChangeFinder::FindMovedItemsL( CNSmlDataItemUidSet& aMovedUids )
	{
	_DBG_FILE("CNSmlChangeFinder::FindMovedItemsL(): begin");
	TNSmlSnapshotItem current;
	TInt index;

	aMovedUids.Reset();
	
	for ( TInt i = 0; i < iCurrentSnapshot->Count(); i++ )
		{
		current = iCurrentSnapshot->At(i);

		// Is this also found in old snapshot?
		if ( !iOldSnapshot->FindIsq(current, iKey, index) )
			{
			// It is -> has it been changed?
			TNSmlSnapshotItem old = iOldSnapshot->At(index);
			if ( current.Compare(old) == 0 && current.ParentId() != old.ParentId() )
				{
				// Yep -> add to list
				User::LeaveIfError(aMovedUids.AddItem(current.ItemId()));
				}
			}
		}

	DBG_ARGS(_S("CNSmlChangeFinder::FindMovedItemsL(): Number of changes = %d"), aMovedUids.ItemCount());

	_DBG_FILE("CNSmlChangeFinder::FindMovedItemsL(): end");
	}
	
// -----------------------------------------------------------------------------
// CNSmlChangeFinder::FindSoftDeletedItemsL
// -----------------------------------------------------------------------------
//
EXPORT_C void CNSmlChangeFinder::FindSoftDeletedItemsL( CNSmlDataItemUidSet& aSoftDeletedUids )
	{
	_DBG_FILE("CNSmlChangeFinder::FindSoftDeletedItemsL(): begin");
	TNSmlSnapshotItem current;
	TInt index;

	aSoftDeletedUids.Reset();
	
	for ( TInt i = 0; i < iCurrentSnapshot->Count(); i++ )
		{
		current = iCurrentSnapshot->At(i);

		// Is this also found in old snapshot?
		if ( !iOldSnapshot->FindIsq(current, iKey, index) )
			{
			// It is -> has it been changed?
			TNSmlSnapshotItem old = iOldSnapshot->At(index);
			if ( current.SoftDelete() && !old.SoftDelete() )
				{
				// Yes -> add to list
				User::LeaveIfError(aSoftDeletedUids.AddItem(current.ItemId()));
				}
			}
		}

	DBG_ARGS(_S("CNSmlChangeFinder::FindSoftDeletedItemsL(): Number of changes = %d"), aSoftDeletedUids.ItemCount());

	_DBG_FILE("CNSmlChangeFinder::FindSoftDeletedItemsL(): end");
	}

// -----------------------------------------------------------------------------
// CNSmlChangeFinder::ItemAddedL
// -----------------------------------------------------------------------------
//
EXPORT_C void CNSmlChangeFinder::ItemAddedL( const TNSmlSnapshotItem& aItem )
	{
	_DBG_FILE("CNSmlChangeFinder::ItemAddedL(): begin");
	iOldSnapshot->InsertIsqL(aItem, iKey);
	iOldSnapshot->Compress();
	_DBG_FILE("CNSmlChangeFinder::ItemAddedL(): end");
	}

// -----------------------------------------------------------------------------
// CNSmlChangeFinder::ItemDeleted
// -----------------------------------------------------------------------------
//
EXPORT_C void CNSmlChangeFinder::ItemDeleted( const TNSmlSnapshotItem& aItem )
	{
	_DBG_FILE("CNSmlChangeFinder::ItemDeleted(): begin");
	TInt index;
	if ( !iOldSnapshot->FindIsq(aItem, iKey, index) )
		{
		iOldSnapshot->Delete(index);
		iOldSnapshot->Compress();
		}
	_DBG_FILE("CNSmlChangeFinder::ItemDeleted(): end");
	}

// -----------------------------------------------------------------------------
// CNSmlChangeFinder::ItemUpdatedL
// -----------------------------------------------------------------------------
//
EXPORT_C void CNSmlChangeFinder::ItemUpdatedL( const TNSmlSnapshotItem& aItem )
	{
	_DBG_FILE("CNSmlChangeFinder::ItemUpdatedL(): begin");
	TInt index;
	if ( !iOldSnapshot->FindIsq(aItem, iKey, index) )
		{
		iOldSnapshot->At(index) = aItem;
		}
	else
		{
		iOldSnapshot->InsertIsqL(aItem, iKey);
		iOldSnapshot->Compress();
		}
	_DBG_FILE("CNSmlChangeFinder::ItemUpdatedL(): end");
	}

// -----------------------------------------------------------------------------
// CNSmlChangeFinder::ItemMovedL
// -----------------------------------------------------------------------------
//
EXPORT_C void CNSmlChangeFinder::ItemMovedL( const TNSmlSnapshotItem& aItem )
	{
	_DBG_FILE("CNSmlChangeFinder::ItemMovedL(): begin");
	TInt index;
	if ( !iOldSnapshot->FindIsq(aItem, iKey, index) )
		{
		iOldSnapshot->At(index) = aItem;
		}
	else
		{
		iOldSnapshot->InsertIsqL(aItem, iKey);
		iOldSnapshot->Compress();
		}
	_DBG_FILE("CNSmlChangeFinder::ItemMovedL(): end");
	}
	
// -----------------------------------------------------------------------------
// CNSmlChangeFinder::ItemSoftDeletedL
// -----------------------------------------------------------------------------
//
EXPORT_C void CNSmlChangeFinder::ItemSoftDeletedL( const TNSmlSnapshotItem& aItem )
	{
	_DBG_FILE("CNSmlChangeFinder::ItemSoftDeletedL(): begin");
	TInt index;
	if ( !iOldSnapshot->FindIsq(aItem, iKey, index) )
		{
		iOldSnapshot->At(index) = aItem;
		}
	else
		{
		iOldSnapshot->InsertIsqL(aItem, iKey);
		iOldSnapshot->Compress();
		}
	_DBG_FILE("CNSmlChangeFinder::ItemSoftDeletedL(): end");
	}

// -----------------------------------------------------------------------------
// CNSmlChangeFinder::CommitChangesL
// -----------------------------------------------------------------------------
//
EXPORT_C void CNSmlChangeFinder::CommitChangesL()
	{
	_DBG_FILE("CNSmlChangeFinder::CommitChangesL(): begin");
	iOldSnapshot->Reset();
	for ( TInt i = 0; i < iCurrentSnapshot->Count(); i++ )
		{
		iOldSnapshot->InsertIsqL( iCurrentSnapshot->At( i ), iKey );
		}
	_DBG_FILE("CNSmlChangeFinder::CommitChangesL(): end");
	}

// -----------------------------------------------------------------------------
// CNSmlChangeFinder::CommitChangesL
// -----------------------------------------------------------------------------
//
EXPORT_C void CNSmlChangeFinder::CommitChangesL( const MSmlDataItemUidSet& aUids )
	{
	_DBG_FILE("CNSmlChangeFinder::CommitChangesL(): begin");
	
	for ( TInt i = 0; i < aUids.ItemCount(); i++ )
		{
		TSmlDbItemUid itemId = aUids.ItemAt( i );
		TNSmlSnapshotItem temp( itemId );
		TInt indexOld( -1 );
		TInt indexNew( -1 );
		if ( !iOldSnapshot->FindIsq( temp, iKey, indexOld) )
			{
			if ( !iCurrentSnapshot->FindIsq(temp, iKey, indexNew) )
				{
				// Replace, moved or softdeleted
				iOldSnapshot->At(indexOld) = iCurrentSnapshot->At( indexNew );
				}
			else
				{
				// Delete
				iOldSnapshot->Delete(indexOld);
				}
			}
		else
			{
			// Add
			if ( !iCurrentSnapshot->FindIsq(temp, iKey, indexNew) )
			    {
			    iOldSnapshot->InsertIsqL( iCurrentSnapshot->At( indexNew ), iKey );
			    }
			}
		}
	_DBG_FILE("CNSmlChangeFinder::CommitChangesL(): end");
	}

// -----------------------------------------------------------------------------
// CNSmlChangeFinder::SetNewSnapshot
// -----------------------------------------------------------------------------
//
EXPORT_C void CNSmlChangeFinder::SetNewSnapshot( CArrayFixSeg<TNSmlSnapshotItem>* aNewSnapshot )
	{
	_DBG_FILE("CNSmlChangeFinder::SetNewSnapshot(): begin");
#ifdef __NSML_DEBUG__
	_DBG_FILE("CNSmlChangeFinder::SetNewSnapshot(): old snapshot");
	TNSmlSnapshotItem item;
	TInt i;

	for ( i = 0; i < iOldSnapshot->Count(); i++ )
		{
		TBuf8<80> b;
		item = iOldSnapshot->At(i);
		b = _L8("UID = ");
		b.AppendNum( item.ItemId() );
		b.Append(_L8(" LastMod(as Int64) = "));
		b.AppendNum(item.LastChangedDate().Int64());
		_DBG_FILE(b.PtrZ());
		}

	_DBG_FILE("CNSmlChangeFinder::SetNewSnapshot(): new snapshot");
	for ( i = 0; i < aNewSnapshot->Count(); i++ )
		{
		TBuf8<80> b;
		item = aNewSnapshot->At(i);
		b = _L8("UID = ");
		b.AppendNum( item.ItemId() );
		b.Append(_L8(" LastMod(as Int64) = "));
		b.AppendNum(item.LastChangedDate().Int64());
		_DBG_FILE(b.PtrZ());
		}
#endif //__NSML_DEBUG__
	delete iCurrentSnapshot;
	iCurrentSnapshot = aNewSnapshot;
	_DBG_FILE("CNSmlChangeFinder::SetNewSnapshot(): end");
	}
	
// -----------------------------------------------------------------------------
// CNSmlChangeFinder::DataStoreUid
// -----------------------------------------------------------------------------
//
EXPORT_C TInt64 CNSmlChangeFinder::DataStoreUid() const
	{
	_DBG_FILE("CNSmlChangeFinder::DataStoreUid(): begin");
	_DBG_FILE("CNSmlChangeFinder::DataStoreUid(): end");
	return iDataStoreUid;
	}

// -----------------------------------------------------------------------------
// CNSmlChangeFinder::SetDataStoreUid
// -----------------------------------------------------------------------------
//
EXPORT_C void CNSmlChangeFinder::SetDataStoreUid( TInt64 aUid )
	{
	_DBG_FILE("CNSmlChangeFinder::SetDataStoreUid(): begin");
	iDataStoreUid = aUid;
	_DBG_FILE("CNSmlChangeFinder::SetDataStoreUid(): end");
	}


// End of File
