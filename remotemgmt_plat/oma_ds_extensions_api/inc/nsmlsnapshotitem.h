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
* Description:  Changefinder header
*
*/


#ifndef __NSMLSNAPSHOTITEM_H__
#define __NSMLSNAPSHOTITEM_H__

//  INCLUDES
#include <e32base.h>
#include <SmlDataSyncDefs.h>

// CLASS DECLARATION

/**
*  TNSmlSnapshotItem.
*
*  @lib nsmlchangefinder.lib
*/
class TNSmlSnapshotItem
	{
public:
	/**
    * C++ constructor
    */
	IMPORT_C TNSmlSnapshotItem();

	/**
    * C++ constructor.
    * @param aItemId UID value as TSmlDbItemUid.
    */
	IMPORT_C TNSmlSnapshotItem( const TSmlDbItemUid& aItemId );
	
	/**
    * Stream externalizer method.
    * @param aStream Stream this snapshot item will be externalized into.
    */
	IMPORT_C void ExternalizeL( RWriteStream& aStream ) const;

	/**
    * Stream interlizer method.
    * @param aStream Stream this snapshot item will be internalized from.
    */
	IMPORT_C void InternalizeL( RReadStream& aStream );
	
	/**
    * Getter for last modification date value.
    * @return const TTime& Last modification date.
    */
	IMPORT_C const TTime& LastChangedDate() const;

	/**
    * Setter for last modification date value.
    * @param aLastChangedDate Last modification date.
    */
	IMPORT_C void SetLastChangedDate( const TTime& aLastChangedDate );
	
	/**
    * Getter for item UID value.
    * @return const TSmlDbItemUid& item UID.
    */
	IMPORT_C const TSmlDbItemUid& ItemId() const;

	/**
    * Setter for item UID value.
    * @param aItemId item UID.
    */
	IMPORT_C void SetItemId( const TSmlDbItemUid& aItemId );

	/**
    * Getter for parent UID value.
    * @return const TSmlDbItemUid& parent UID.
    */
	IMPORT_C const TSmlDbItemUid& ParentId() const;

	/**
    * Setter for parent UID value.
    * @param aParentId parent UID.
    */
	IMPORT_C void SetParentId( const TSmlDbItemUid& aParentId );

	/**
    * Getter for soft delete value.
    * @return const TBool& soft delete value.
    */
	IMPORT_C const TBool& SoftDelete() const;

	/**
    * Setter for soft delete value.
    * @param aSoftDelete soft delete value.
    */
	IMPORT_C void SetSoftDelete( const TBool& aSoftDelete );

	/**
    * Compare method for comparing last change dates of two snapshot items. Returns < 0 if
    * this item is smaller than aItem, returns 0 if both items are equal and > 0 if
    * aItem is greater than this snapshot item.
    * @param aItem Item which this item is compared to.
	* @return TInt Comparison result.
    */
	IMPORT_C TInt Compare( const TNSmlSnapshotItem& aItem ) const;
	
private:
	TBool iSoftDelete;
	TTime iLastChangedDate;
	TSmlDbItemUid iItemId;
	TSmlDbItemUid iParentId;
	};

#endif // __NSMLSNAPSHOTITEM_H__

// End of File
