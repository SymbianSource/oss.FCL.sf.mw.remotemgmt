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
* Description:  Map command buffering
*
*/


// INCLUDE FILES
#include <nsmlconstants.h>
#include "nsmldsmapcontainer.h"

// <MAPINFO_RESEND_MOD_BEGIN>
#include <nsmldebug.h>
// <MAPINFO_RESEND_MOD_END>

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CNSmlDSMapContainer::CMapItem::~CMapItem
// Destructor.
// -----------------------------------------------------------------------------
//
CNSmlDSMapContainer::CMapItem::~CMapItem()
	{
	delete iGlobalUid;
	}
	
// -----------------------------------------------------------------------------
// CNSmlDSMapContainer::CNSmlDSMapContainer
// C++ default constructor.
// -----------------------------------------------------------------------------
//
CNSmlDSMapContainer::CNSmlDSMapContainer()
	{
// <MAPINFO_RESEND_MOD_BEGIN>
	setAppendToStore( EFalse );
	iMapRemovable = EFalse;
// <MAPINFO_RESEND_MOD_END>
	}

// -----------------------------------------------------------------------------
// CNSmlDSMapContainer::~CNSmlDSMapContainer
// Destructor.
// -----------------------------------------------------------------------------
//
CNSmlDSMapContainer::~CNSmlDSMapContainer()
	{
	iMapItemList.ResetAndDestroy();
	}

// -----------------------------------------------------------------------------
// CNSmlDSMapContainer::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CNSmlDSMapContainer* CNSmlDSMapContainer::NewL()
	{
	CNSmlDSMapContainer* self = new ( ELeave ) CNSmlDSMapContainer;
	return self;
	}

// -----------------------------------------------------------------------------
// CNSmlDSMapContainer::MapListExists
// Checks whether the map list exists.
// -----------------------------------------------------------------------------
//
TBool CNSmlDSMapContainer::MapListExists() const
	{
	if ( iMapItemList.Count() == 0 )
		{
		return EFalse;
		}
		
	for ( TInt i = 0; i < iMapItemList.Count(); i++ )
		{
		if ( !iMapItemList[i]->iAlreadySent )
			{
			return ETrue;
			}
		}
		
	return EFalse;
	}
	
// -----------------------------------------------------------------------------
// CNSmlDSMapContainer::CreateNewMapItemL
// Creates new map item to the list.
// -----------------------------------------------------------------------------
//
void CNSmlDSMapContainer::CreateNewMapItemL( const TSmlDbItemUid aLocalUid, const TDesC8& aGlobalUid, const TInt aAtomicId )
	{
	CMapItem* item = new ( ELeave ) CMapItem;
	CleanupStack::PushL( item );
	item->iAlreadySent = EFalse;
	item->iAtomicId = aAtomicId;
	item->iGlobalUid = aGlobalUid.AllocL();
	item->iLocalUid = aLocalUid;

// <MAPINFO_RESEND_MOD_BEGIN>
	if( !MapListExists() )
		{
		iMapRemovable = EFalse;	
		}
// <MAPINFO_RESEND_MOD_BEGIN>

	iMapItemList.AppendL( item );
	
	CleanupStack::Pop(); // item
	}

// -----------------------------------------------------------------------------
// CNSmlDSMapContainer::RemoveFailedAtomics
// Removes those map items that were executed under a failed atomic command.
// -----------------------------------------------------------------------------
//
void CNSmlDSMapContainer::RemoveFailedAtomics( const TInt aAtomicId )
	{
	for ( TInt i = 0; i < iMapItemList.Count(); )
		{
		if ( iMapItemList[i]->iAtomicId == aAtomicId )
			{
			delete iMapItemList[i];
			iMapItemList.Remove( i );
			}
		else
			{
			++i;
			}
		}
		
	iMapItemList.Compress();
	}

// -----------------------------------------------------------------------------
// CNSmlDSMapContainer::MapItemList
// Returns the map item list, ownership is changed to a caller. 
// -----------------------------------------------------------------------------
//
SmlMapItemList_t* CNSmlDSMapContainer::MapItemListL()
	{
	SmlMapItemList_t* mapItemList( NULL );
	SmlMapItemList_t* mapItemListLast( NULL );
	
	for ( TInt i = 0; i < iMapItemList.Count(); i++ )
		{
		if ( iMapItemList[i]->iAlreadySent )
			{
			continue;
			}
			
		TBuf8<16> localUid;
		localUid.Num( iMapItemList[i]->iLocalUid );

		if ( !mapItemList )	
			{
			mapItemList = new ( ELeave ) SmlMapItemList_t;
			mapItemListLast = mapItemList;
			}
		else
			{
			mapItemListLast->next = new ( ELeave ) SmlMapItemList_t;
			mapItemListLast = mapItemListLast->next;
			}
			
		mapItemListLast->next = NULL;
		mapItemListLast->mapItem = new( ELeave ) SmlMapItem_t;
		mapItemListLast->mapItem->target = new( ELeave ) SmlTarget_t;
		mapItemListLast->mapItem->target->locName = NULL;
		PcdataNewL( mapItemListLast->mapItem->target->locURI, *iMapItemList[i]->iGlobalUid );
		mapItemListLast->mapItem->source = new( ELeave ) SmlSource_t;
		mapItemListLast->mapItem->source->locName = NULL;
		PcdataNewL( mapItemListLast->mapItem->source->locURI, localUid );
		}
		
	return mapItemList;
	}
	
// -----------------------------------------------------------------------------
// CNSmlDSMapContainer::SetMapItemList
// Sets the map item list, ownership is changed to this class.
// -----------------------------------------------------------------------------
//
void CNSmlDSMapContainer::SetMapItemList( SmlMapItemList_t* aMapItemList )
	{
// <MAPINFO_RESEND_MOD_BEGIN>
	iMapRemovable = EFalse;
// <MAPINFO_RESEND_MOD_END>
	delete aMapItemList;
	}
	
// -----------------------------------------------------------------------------
// CNSmlDSMapContainer::PcdataNewL
// Creates a new Pcdata element.
// -----------------------------------------------------------------------------
//
void CNSmlDSMapContainer::PcdataNewL( SmlPcdata_t*& aPcdata, const TDesC8& aContent ) const
	{
	aPcdata = new( ELeave ) SmlPcdata_t;
	aPcdata->SetDataL( aContent );
	aPcdata->contentType = SML_PCDATA_OPAQUE;   
	aPcdata->extension = SML_EXT_UNDEFINED; 	
	}

// -----------------------------------------------------------------------------
// CNSmlDSMapContainer::MapSourceParent
// Maps a SourceParent to an existing LUID.
// -----------------------------------------------------------------------------
//
TBool CNSmlDSMapContainer::MapSourceParent( const TDesC8& aSourceParent, TSmlDbItemUid& aUid ) const
	{
	for ( TInt i = 0; i < iMapItemList.Count(); i++ )
		{
		if ( *iMapItemList[i]->iGlobalUid == aSourceParent )
			{
			aUid = iMapItemList[i]->iLocalUid;
			return ETrue;
			}
		}
		
	return EFalse;
	}

// -----------------------------------------------------------------------------
// CNSmlDSMapContainer::MarkAllItemsSent
// Marks all items as sent.
// -----------------------------------------------------------------------------
//
void CNSmlDSMapContainer::MarkAllItemsSent()
	{
	for ( TInt i = 0; i < iMapItemList.Count(); i++ )
		{
		iMapItemList[i]->iAlreadySent = ETrue;
		}
	}

// <MAPINFO_RESEND_MOD_BEGIN>

// ------------------------------------------------------------------------------------------------
//CNSmlDSMapContainer::ExternalizeL
///To externalize the map information
// ------------------------------------------------------------------------------------------------
void CNSmlDSMapContainer::ExternalizeL( RWriteStream& aStream ) const
	{
	DBG_FILE(_S8("CNSmlDSMapContainer::ExternalizeL(): begin"));
	if( !isAppendToStore() )
		{
		TPckgBuf<TTime> anchor( iMapAnchor );
		aStream << anchor;
		}
	aStream.WriteInt8L(iMapItemList.Count());
	for( int i = 0 ; i < iMapItemList.Count() ; i++ )
		{
		if( !iMapItemList[i]->iLocalUid != NULL && iMapItemList[i]->iGlobalUid != NULL )
			{
			break;
			}
		TNSmlMapInfoItem mapItem( iMapItemList[i]->iLocalUid, *iMapItemList[i]->iGlobalUid, iMapItemList[i]->iAtomicId );
		TInt err;
		TRAP( err, aStream << mapItem );
		DBG_FILE_CODE( err, _S8("CNSmlDSMapContainer::ExternalizeL(): Item externalized!"));
		}
	DBG_FILE(_S8("CNSmlDSMapContainer::ExternalizeL(): end"));
	}

// ------------------------------------------------------------------------------------------------
//CNSmlDSMapContainer::InternalizeL
///To internalize the map information
// ------------------------------------------------------------------------------------------------
void CNSmlDSMapContainer::InternalizeL( RReadStream& aStream )
	{
	DBG_FILE(_S8("CNSmlDSMapContainer::InternalizeL(): begin"));
	TPckgBuf<TTime> lastMapAnchor;
	TRAPD( er, aStream >> lastMapAnchor );
	TInt count = 0;
	TInt8 mapItemCount = 0;
	if( er != KErrEof )
		{		
		TRAP( er, mapItemCount = aStream.ReadInt8L(); );
		}
	if( er != KErrEof )
		{
		setMapAnchor( lastMapAnchor() );
		if( mapItemCount > 0 )
			{
			iMapRemovable = EFalse;
			for( int i = 0; i < mapItemCount; i++ )
				{
				TNSmlMapInfoItem mapItem;
				TRAPD( er1, aStream >> mapItem );
				if( er1 == KErrEof )
					break;
				if( mapItem.iLUId != 0 && mapItem.iGUId.Length() != 0 )
					{
					CreateNewMapItemL( mapItem.iLUId, mapItem.iGUId, mapItem.iAtomicId );
					count++;
					}
				}
			}
		}
	DBG_FILE(_S8("CNSmlDSMapContainer::InternalizeL(): end"));
	}

// ------------------------------------------------------------------------------------------------
//CNSmlDSMapContainer::MapItemListSize
//To Retrieve the number of map items present in the map list
//-------------------------------------------------------------------------------------------------
TInt CNSmlDSMapContainer::MapItemListSize() const
	{
	return iMapItemList.Count();
	}
// ------------------------------------------------------------------------------------------------
//CNSmlDSMapContainer::setMapAnchor
//To set the time stamp of the map list items, for any Time comparisions required
// ------------------------------------------------------------------------------------------------
void CNSmlDSMapContainer::setMapAnchor(TTime aMapAnchor)
	{
	iMapAnchor=aMapAnchor;
	}

// ------------------------------------------------------------------------------------------------
//CNSmlDSMapContainer::getMapAnchor
//To get the time stamp of the last map list items externalized, for any Time comparisions required
// ------------------------------------------------------------------------------------------------
TTime CNSmlDSMapContainer::getMapAnchor()
	{
	return iMapAnchor;
	}

// ------------------------------------------------------------------------------------------------
//CNSmlDSMapContainer::isAppendToStore
//To check if the map store has to be overwritten or the new map items to be appended to the existing map store
// ------------------------------------------------------------------------------------------------
TBool CNSmlDSMapContainer::isAppendToStore() const
	{
	return iAppendToStore;
	}
// ------------------------------------------------------------------------------------------------
//CNSmlDSMapContainer::isAppendToStore
//To set the flag fro the map store for over writing or to appended the new map items to the existing map store
// ------------------------------------------------------------------------------------------------
void CNSmlDSMapContainer::setAppendToStore(TBool aAppend)
	{
	iAppendToStore = aAppend;
	}

// ------------------------------------------------------------------------------------------------
//CNSmlDSMapContainer::setRemoveMap()
// To Delay the removal of map information from the cache and to remove at a later stage by using isMapRemovable
// ---------------------------------------------------------
void CNSmlDSMapContainer::setRemoveMap(TBool aRemoveMap)
	{
	iMapRemovable = aRemoveMap;
	}
// ------------------------------------------------------------------------------------------------
//CNSmlDSMapContainer::isMapRemovable()
// To check if map information can be removed from the cache.
// ---------------------------------------------------------
TBool CNSmlDSMapContainer::isMapRemovable()
	{
	return iMapRemovable;
	}

// ------------------------------------------------------------------------------------------------
//TNSmlMapInfoItem::ExternalizeL
///To externalize a map item
// ------------------------------------------------------------------------------------------------
void TNSmlMapInfoItem::ExternalizeL(RWriteStream& aStream) const
	{
	aStream.WriteInt16L(iLUId);
	aStream << iGUId;
	aStream.WriteInt8L(iAtomicId);
	}
// ------------------------------------------------------------------------------------------------
//TNSmlMapInfoItem::InternalizeL
///To internalize a map item
// ------------------------------------------------------------------------------------------------
void TNSmlMapInfoItem::InternalizeL(RReadStream& aStream)
	{
	iLUId = aStream.ReadInt16L();
	aStream >> iGUId;
	iAtomicId = aStream.ReadInt8L();
	}
// ------------------------------------------------------------------------------------------------
//TNSmlMapInfoItem::TNSmlMapInfoItem
///Map item constructor
// ------------------------------------------------------------------------------------------------
TNSmlMapInfoItem::TNSmlMapInfoItem()
	{
	iLUId = 0;
	iGUId.Zero();
	iAtomicId = 0;
	}
// ------------------------------------------------------------------------------------------------
//TNSmlMapInfoItem::TNSmlMapInfoItem
///Map item overloaded constructor
// ------------------------------------------------------------------------------------------------
TNSmlMapInfoItem::TNSmlMapInfoItem( const TSmlDbItemUid aLUId, const TDesC8& aGUId, const TInt aAtomicId )
	{
	iLUId = aLUId;
	iGUId = aGUId;
	iAtomicId = aAtomicId;
	}
// ------------------------------------------------------------------------------------------------
//TNSmlMapInfoItem::TNSmlMapInfoItem
///Map item copy constructor
// ------------------------------------------------------------------------------------------------
TNSmlMapInfoItem::TNSmlMapInfoItem( const TNSmlMapInfoItem& aItem )
	{
	iLUId = aItem.iLUId;
	iGUId = aItem.iGUId;
	iAtomicId = aItem.iAtomicId;
	}

// ------------------------------------------------------------------------------------------------

// <MAPINFO_RESEND_MOD_END>
	
// End of File
