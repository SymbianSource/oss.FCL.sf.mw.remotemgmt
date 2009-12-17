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
* Description:  Map command buffer
*
*/


#ifndef __NSMLDSMAPCONTAINER_H__
#define __NSMLDSMAPCONTAINER_H__

// INCLUDES
#include <e32base.h>
#include <SmlDataSyncDefs.h>
#include "smldtd.h"

// <MAPINFO_RESEND_MOD_BEGIN>

#include <s32strm.h>
const TUint32 KNSmlMaxUidSize = 32;

// ------------------------------------------------------------------------------------------------
// TNSmlMapInfoItem structure
// ------------------------------------------------------------------------------------------------
struct TNSmlMapInfoItem
	{
	public:
		TNSmlMapInfoItem();
		TNSmlMapInfoItem( const TSmlDbItemUid aLUId, const TDesC8& aGUId, const TInt aAtomicId );
		TNSmlMapInfoItem( const TNSmlMapInfoItem& aItem );

		// storing methods
		void ExternalizeL( RWriteStream& aStream ) const;
		void InternalizeL( RReadStream& aStream );
	public:
		TSmlDbItemUid iLUId;
		TBuf8<KNSmlMaxUidSize> iGUId;
		TInt iAtomicId;
	};
// <MAPINFO_RESEND_MOD_END>

// CLASS DECLARATION

/**
* Class that buffers mappings between local and global UID values.
*
* @lib nsmldsagent.lib
*/
class CNSmlDSMapContainer : public CBase 
	{
	public: // constructors and destructor
		/**
		* Symbian two-phased constructor.
		*/
		static CNSmlDSMapContainer* NewL(); 

		/**
		* Destructor.
		*/		
		~CNSmlDSMapContainer();
    
	public: // new functions
		/**
		* Checks whether the buffer has been created.
		* @return ETrue if the buffer exists.
		*/
		TBool MapListExists() const;
		
		/**
		* Creates a new item to the buffer.
		* @param aLUID Item's local UID.
		* @param aGUID Item's global UID.
		* @param aAtomic Item's atomic command id.
		*/
		void CreateNewMapItemL( const TSmlDbItemUid aLocalUid, const TDesC8& aGlobalUid, const TInt aAtomicId );
		
		/**
		* Gets the buffer. Ownership of the returned list is changed to 
		* the caller.
		* @return Pointer to the map item buffer.
		*/
		SmlMapItemList_t* MapItemListL();
		
		/**
		* Sets the buffer. This class takes the ownership of the buffer.
		* @param aMapItemList Pointer to the buffer.
		*/
		void SetMapItemList( SmlMapItemList_t* aMapItemList );


// <MAPINFO_RESEND_MOD_BEGIN>
		/**
		* Externalizes the map information.
		* @param aStream Write stream.
		*/
		void ExternalizeL( RWriteStream& aStream ) const;
		/**
		*  Intenalizes the map information.
		* @param aStream Read stream..
		*/
		void InternalizeL( RReadStream& aStream );
		/**
		* Retrieves number of map items present in the map list
		* @return Number of map items.
		*/
		TInt MapItemListSize() const;
		/**
		* Sets the time stamp of the map list items.
		* @param aMapAnchor Time of the anchor to be set.
		*/
		void setMapAnchor(TTime aMapAnchor);
		/**
		* Gets the time stamp of the last map list items externalized.
		* @return Time of the anchor.
		*/
		TTime getMapAnchor();
		/**
		* Checks if the map store has to be overwritten or the new map items to be appended to the existing map store.
		* @return Boolean value indicating if appending required.
		*/
		TBool isAppendToStore() const;
		/**
		* Sets the flag for the map store for overwriting or to appended the new map items to the existing map store.
		* @param aAppend Boolean value indicating if appending required.
		*/
		void setAppendToStore(TBool aAppend=ETrue);
		/**
		* Delay the removal of map information from the cache and to remove at a later stage by using isMapRemovable.
		* @param aRemoveMap 
		*/
		void setRemoveMap(TBool aRemoveMap=ETrue);
		/**
		* Checks if map information can be removed from the cache.
		* @return Boolean value indicating if map is removable.
		*/
		TBool isMapRemovable();
// <MAPINFO_RESEND_MOD_END>
		
		/**
		* Removes those map items that have the given atomic command id.
		* @param aAtomicId Items that have this atomic id will be removed.
		*/
		void RemoveFailedAtomics( const TInt aAtomicId );

		/**
		* Maps the given SourceParent value to an existing LUID.
		* @param aSourceParent SourceParent UID that should be mapped.
		* @param aUid On return contains the matching LUID.
		* @return ETrue if a matching LUID was found.
		*/
		TBool MapSourceParent( const TDesC8& aSourceParent, TSmlDbItemUid& aUid ) const;
		
		/**
		* Marks all items as sent. After this call none of the items currently
		* in the buffer is sent to the server. Items are preserved however so that they are
		* available if SourceParent <-> Local UID mapping is needed.
		*/
		void MarkAllItemsSent();
		
	private: // constructors & operators	
		/**
		* C++ constructor.
		*/
		CNSmlDSMapContainer();
		
		/**
		* Prohibit copy constructor.
		*/
		CNSmlDSMapContainer( const CNSmlDSMapContainer& aOther );
		
		/**
		* Prohibit assignment operator.
		*/
		CNSmlDSMapContainer& operator=( const CNSmlDSMapContainer& aOther );

	private: // new functions
		/**
		* Creates a new data element.
		* @param aPcdata Target where the data item is created.
		* @param aContent Content of the new item.
		*/
		void PcdataNewL( SmlPcdata_t*& aPcdata, const TDesC8& aContent ) const;
		
	private: // data
		class CMapItem : public CBase
			{
			public:
				~CMapItem();
				
			public:
				TInt iAtomicId;
				TBool iAlreadySent;
				TSmlDbItemUid iLocalUid;
				HBufC8* iGlobalUid;
			};
			
		RPointerArray<CMapItem> iMapItemList;
		
// <MAPINFO_RESEND_MOD_BEGIN>
	TTime iMapAnchor;
	TBool iAppendToStore;
	TBool iMapRemovable;
// <MAPINFO_RESEND_MOD_END>
		
	};

#endif // __NSMLDSMAPCONTAINER_H__

// End of File
