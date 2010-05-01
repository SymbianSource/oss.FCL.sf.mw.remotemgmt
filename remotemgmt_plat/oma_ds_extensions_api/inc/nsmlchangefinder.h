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
* Description:  Changefinder header
*
*/


#ifndef __NSMLCHANGEFINDER_H__
#define __NSMLCHANGEFINDER_H__

//  INCLUDES
#include <e32base.h>
#include <nsmlsnapshotitem.h>
#include <SmlDataProvider.h>

// CONSTANTS
const TInt KNSmlDefaultSnapshotStreamUID = 0x101F6DE4;

// CLASS DECLARATION

/**
*  CNSmlDataItemUidSet.
*
*  @lib nsmlchangefinder.lib
*/
class CNSmlDataItemUidSet : public CBase, public MSmlDataItemUidSet
	{
	public:

    	/**
        * Constructor.
        */
	    IMPORT_C CNSmlDataItemUidSet();

	    /**
        * Destructor.
        */
	    ~CNSmlDataItemUidSet();
	
	public: // New functions
	
		/**
		* Adds item to UID set.
		* @param aItemId. UID of item.
		* @return TInt. KErrNone or one of the system wide error codes.
		*/
		IMPORT_C TInt AddItem( TSmlDbItemUid aItemId );
		
		/**
		* Resets UID set.
		*/
		IMPORT_C void Reset();
		
		/**
		* This method internalizes class from stream.
		* @param aStream. Internalize is done from this stream.
		*/
		IMPORT_C void InternalizeL( RReadStream& aStream );
		
	private: // Functions from base classes

		/**
		* From MSmlDataItemUidSet Returns the number of items in the set.
		* @return TInt.
		*/
		TInt DoItemCount() const;
		
		/**
		* From MSmlDataItemUidSet Returns the index of UID.
		* @param aItemId. UID of item.
		* @return TInt. The index of specified item UID in the set, or -1 if the item UID is not present.
		*/
		TInt DoItemIndex( TSmlDbItemUid aItemId ) const;
		
		/**
		* From MSmlDataItemUidSet Returns the item UID.
		* @param aIndex. Indes of item.
		* @return TSmlDsItemUid.Retuns the item UID at the specified index in the set
		*/
		TSmlDbItemUid DoItemAt( TInt aIndex ) const;
		
		/**
		* From MSmlDataItemUidSet This method externalizes class to stream.
		* @param aStream. Externalize is done to this stream.
		*/
		void DoExternalizeL( RWriteStream& aStream ) const;
		
	private: // Data

		RArray<TSmlDbItemUid> iUidSet;	// Set of UIDs
	};


/**
*  CNSmlChangeFinder.
*
*  @lib nsmlchangefinder.lib
*/
class CNSmlChangeFinder : public CBase
	{
    public: // Constructor and destructor
    	/**
        * Two-phase constructor for CNSmlChangeFinder class.
        * @param aSyncRelationship Reference to MSmlSyncRelationship interface.
        * @param aKey Key that is used in sorting snapshot.
        * @param aHasHistory Boolean that is true if change finder has history for current uid.
        * @param aStreamUid Snapshot stream UID.
	    * @return CNSmlChangeFinder* Pointer to newly created instance.
        */
	    IMPORT_C static CNSmlChangeFinder* NewL( 
	        MSmlSyncRelationship& aSyncRelationship, TKeyArrayFix aKey,
	        TBool& aHasHistory, TInt aStreamUid = KNSmlDefaultSnapshotStreamUID );

    	/**
        * C++ destructor.
        */
    	IMPORT_C ~CNSmlChangeFinder();

    public: // New functions

    	/**
        * Closes and saves snapshot.
        */
    	IMPORT_C void CloseL();

    	/**
        * Resets (clears) snapshot.
        */
    	IMPORT_C void ResetL();

    	/**
        * Returns changed items.
        * Item is changed, not soft deleted currently or not only moved
    	* @param aChangedUids On return contains changed items.
        */
    	IMPORT_C void FindChangedItemsL( CNSmlDataItemUidSet& aChangedUids );

    	/**
        * Returns deleted items.
        * Item is deleted permanently
    	* @param aDeletedUids On return contains deleted items.
        */
    	IMPORT_C void FindDeletedItemsL( CNSmlDataItemUidSet& aDeletedUids );

    	/**
        * Returns new items.
    	* @param aNewUids On return contains new items.
        */
    	IMPORT_C void FindNewItemsL( CNSmlDataItemUidSet& aNewUids );
    	
    	/**
        * Returns moved items.
        * Item is moved, not changed.
    	* @param aMovedUids On return contains moved items.
        */
    	IMPORT_C void FindMovedItemsL( CNSmlDataItemUidSet& aMovedUids );
    	
    	/**
        * Returns soft deleted items.
        * Item is soft deleted, it can be also modified or moved.
    	* @param aSofDeletedUids On return contains soft deleted items.
        */
    	IMPORT_C void FindSoftDeletedItemsL( CNSmlDataItemUidSet& aSoftDeletedUids );

    	/**
        * Update snapshot for added item.
    	* @param aItem Item that needs update.
        */
    	IMPORT_C void ItemAddedL( const TNSmlSnapshotItem& aItem );

    	/**
        * Update snapshot for deleted item.
    	* @param aItem Item that needs update.
        */
    	IMPORT_C void ItemDeleted( const TNSmlSnapshotItem& aItem );

    	/**
        * Update snapshot for updated item.
    	* @param aItem Item that needs update.
        */
    	IMPORT_C void ItemUpdatedL( const TNSmlSnapshotItem& aItem );

    	/**
        * Update snapshot for moved item.
    	* @param aItem Item that needs update.
        */
    	IMPORT_C void ItemMovedL( const TNSmlSnapshotItem& aItem );
    	
    	/**
        * Update snapshot for soft deleted item.
    	* @param aItem Item that needs update.
        */
    	IMPORT_C void ItemSoftDeletedL( const TNSmlSnapshotItem& aItem );

    	/**
        * Update snapshot for all items.
        */
    	IMPORT_C void CommitChangesL();
    	
    	/**
        * Update snapshot for all items that are synchronized correctly..
    	* @param aUids UIDs that are synchronized correctly.
        */
    	IMPORT_C void CommitChangesL( const MSmlDataItemUidSet& aUids );
		
    	/**
        * Sets new (current) snapshot.
    	* @param aNewSnapshot New snapshot.
        */
    	IMPORT_C void SetNewSnapshot( CArrayFixSeg<TNSmlSnapshotItem>* aNewSnapshot );
    	
    	/**
        * Gets old UID of DataStore. 
    	* Return TInt. Old UID of DataStore. Returns -1 (KErrNotFound) if UID of DataStore is not set.
        */
    	IMPORT_C TInt64 DataStoreUid() const;
    	
    	/**
        * Sets new UID of DataStore.
    	* @param aUid. New UID of DataStore.
        */
    	IMPORT_C void SetDataStoreUid( TInt64 aUid );

    protected: // Constructors

    	CNSmlChangeFinder( MSmlSyncRelationship& aSyncRelationship,
    	    TKeyArrayFix aKey, TInt aStreamUid );
    	    
    	void ConstructL( TBool& aHasHistory );

    private: // Data
    
    	MSmlSyncRelationship& iSyncRelationship;
    	CArrayFixSeg<TNSmlSnapshotItem>* iCurrentSnapshot;
    	CArrayFixSeg<TNSmlSnapshotItem>* iOldSnapshot;
    	TKeyArrayFix iKey;
    	TInt iStreamUid;
    	TInt64 iDataStoreUid;
	};

#endif // __NSMLCHANGEFINDER_H__

// End of File
