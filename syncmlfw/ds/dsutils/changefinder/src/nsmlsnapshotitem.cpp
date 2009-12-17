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
#include <nsmlsnapshotitem.h>


// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// TNSmlSnapshotItem::TNSmlSnapshotItem
// C++ constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C TNSmlSnapshotItem::TNSmlSnapshotItem()
    : iSoftDelete( EFalse ), iLastChangedDate( Time::NullTTime() ), 
      iItemId( 0 ), iParentId( 0 )
	{
	_DBG_FILE("TNSmlSnapshotItem::TNSmlSnapshotItem(): begin");
	_DBG_FILE("TNSmlSnapshotItem::TNSmlSnapshotItem(): end");
	}

// -----------------------------------------------------------------------------
// TNSmlSnapshotItem::TNSmlSnapshotItem
// C++ constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C TNSmlSnapshotItem::TNSmlSnapshotItem( const TSmlDbItemUid& aItemId )
    : iSoftDelete( EFalse ), iLastChangedDate( Time::NullTTime() ),
      iItemId( aItemId ), iParentId( 0 )
	{
	_DBG_FILE("TNSmlSnapshotItem::TNSmlSnapshotItem(const TNSmlDbItemUid&): begin");
	_DBG_FILE("TNSmlSnapshotItem::TNSmlSnapshotItem(const TNSmlDbItemUid&): end");
	}

// -----------------------------------------------------------------------------
// TNSmlSnapshotItem::ExternalizeL
// -----------------------------------------------------------------------------
//
EXPORT_C void TNSmlSnapshotItem::ExternalizeL( RWriteStream& aStream ) const
	{
	_DBG_FILE("TNSmlSnapshotItem::ExternalizeL(): begin");
	TPckgBuf<TSmlDbItemUid> a(iItemId);
	aStream << a;
	TPckgBuf<TSmlDbItemUid> b(iParentId);
	aStream << b;
	TPckgBuf<TTime> c(iLastChangedDate);
	aStream << c;
	TPckgBuf<TBool> d(iSoftDelete);
	aStream << d;
	_DBG_FILE("TNSmlSnapshotItem::ExternalizeL(): end");
	}

// -----------------------------------------------------------------------------
// TNSmlSnapshotItem::InternalizeL
// -----------------------------------------------------------------------------
//
EXPORT_C void TNSmlSnapshotItem::InternalizeL( RReadStream& aStream )
	{
	_DBG_FILE("TNSmlSnapshotItem::InternalizeL(): begin");
	TPckgBuf<TSmlDbItemUid> a;
	aStream >> a;
	iItemId = a();
	TPckgBuf<TSmlDbItemUid> b;
	aStream >> b;
	iParentId = b();
	TPckgBuf<TTime> c;
	aStream >> c;
	iLastChangedDate = c();
	TPckgBuf<TBool> d;
	aStream >> d;
	iSoftDelete = d();
	_DBG_FILE("TNSmlSnapshotItem::InternalizeL(): end");
	}

// -----------------------------------------------------------------------------
// TNSmlSnapshotItem::LastChangedDate
// -----------------------------------------------------------------------------
//
EXPORT_C const TTime& TNSmlSnapshotItem::LastChangedDate() const
	{
	_DBG_FILE("TNSmlSnapshotItem::LastChangedDate(): begin");
	_DBG_FILE("TNSmlSnapshotItem::LastChangedDate(): end");
	return iLastChangedDate;
	}

// -----------------------------------------------------------------------------
// TNSmlSnapshotItem::SetLastChangedDate
// -----------------------------------------------------------------------------
//
EXPORT_C void TNSmlSnapshotItem::SetLastChangedDate( const TTime& aLastChangedDate )
	{
	_DBG_FILE("TNSmlSnapshotItem::SetLastChangedDate(): begin");
	iLastChangedDate = aLastChangedDate;
	_DBG_FILE("TNSmlSnapshotItem::SetLastChangedDate(): end");
	}

// -----------------------------------------------------------------------------
// TNSmlSnapshotItem::ItemId
// -----------------------------------------------------------------------------
//
EXPORT_C const TSmlDbItemUid& TNSmlSnapshotItem::ItemId() const
	{
	_DBG_FILE("TNSmlSnapshotItem::ItemId(): begin");
	_DBG_FILE("TNSmlSnapshotItem::ItemId(): end");
	return iItemId;
	}

// -----------------------------------------------------------------------------
// TNSmlSnapshotItem::SetItemId
// -----------------------------------------------------------------------------
//
EXPORT_C void TNSmlSnapshotItem::SetItemId( const TSmlDbItemUid& aItemId )
	{
	_DBG_FILE("TNSmlSnapshotItem::SetItemId(): begin");
	iItemId = aItemId;
	_DBG_FILE("TNSmlSnapshotItem::SetItemId(): end");
	}
	
// -----------------------------------------------------------------------------
// TNSmlSnapshotItem::ParentId
// -----------------------------------------------------------------------------
//
EXPORT_C const TSmlDbItemUid& TNSmlSnapshotItem::ParentId() const
	{
	_DBG_FILE("TNSmlSnapshotItem::ParentId(): begin");
	_DBG_FILE("TNSmlSnapshotItem::ParentId(): end");
	return iParentId;
	}

// -----------------------------------------------------------------------------
// TNSmlSnapshotItem::SetParentId
// -----------------------------------------------------------------------------
//
EXPORT_C void TNSmlSnapshotItem::SetParentId( const TSmlDbItemUid& aParentId )
	{
	_DBG_FILE("TNSmlSnapshotItem::SetParentId(): begin");
	iParentId = aParentId;
	_DBG_FILE("TNSmlSnapshotItem::SetParentId(): end");
	}
	
// -----------------------------------------------------------------------------
// TNSmlSnapshotItem::SoftDelete
// -----------------------------------------------------------------------------
//
EXPORT_C const TBool& TNSmlSnapshotItem::SoftDelete() const
	{
	_DBG_FILE("TNSmlSnapshotItem::SoftDelete(): begin");
	_DBG_FILE("TNSmlSnapshotItem::SoftDelete(): end");
	return iSoftDelete;
	}

// -----------------------------------------------------------------------------
// TNSmlSnapshotItem::SetSoftDelete
// -----------------------------------------------------------------------------
//
EXPORT_C void TNSmlSnapshotItem::SetSoftDelete( const TBool& aSoftDelete )
	{
	_DBG_FILE("TNSmlSnapshotItem::SetSoftDelete(): begin");
	iSoftDelete = aSoftDelete;
	_DBG_FILE("TNSmlSnapshotItem::SetSoftDelete(): end");
	}

// -----------------------------------------------------------------------------
// TNSmlSnapshotItem::Compare
// -----------------------------------------------------------------------------
//
EXPORT_C TInt TNSmlSnapshotItem::Compare( const TNSmlSnapshotItem& aItem ) const
	{
	_DBG_FILE("TNSmlSnapshotItem::Compare(): begin");
	TInt ret((iLastChangedDate > aItem.iLastChangedDate) ? 1 : (iLastChangedDate < aItem.iLastChangedDate) ? -1 : 0);
	_DBG_FILE("TNSmlSnapshotItem::Compare(): end");
	return ret;
	}

// End of File
