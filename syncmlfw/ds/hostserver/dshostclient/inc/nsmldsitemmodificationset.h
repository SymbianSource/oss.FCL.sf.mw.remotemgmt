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
* Description:  Utilities for DS Loader Framework
*
*/


#ifndef __NSMLDSITEMMODIFICATIONSET_H__ 
#define __NSMLDSITEMMODIFICATIONSET_H__

// INCLUDES
#include <e32base.h>
#include <s32strm.h>
#include <SyncMLDef.h>
#include <SmlDataSyncDefs.h>

// FORWARD DECLARATIONS
class MSmlDataItemUidSet;

// CLASS DECLARATION

// ------------------------------------------------------------------------------------------------
// TNSmlDbItemModification
//
// @lib nsmldshostclient.lib
// ------------------------------------------------------------------------------------------------
class TNSmlDbItemModification
	{
	public:
			
		enum TNSmlDbItemModificationType
			{
			ENSmlDbItemAdd = 1,
			ENSmlDbItemDelete,
			ENSmlDbItemSoftDelete,
			ENSmlDbItemMove,
			ENSmlDbItemModify
			};
			
		/**
		* C++ constructor.
		* @param aItemId. UID of item.
		* @param aModType. Type of modification.
		*/
		IMPORT_C TNSmlDbItemModification( TSmlDbItemUid aItemId, TNSmlDbItemModificationType aModType );

		/**
		* C++ copy constructor.
		* @param aDim. source for copy
		*/	
		IMPORT_C TNSmlDbItemModification( const TNSmlDbItemModification& aDim );
		
		/**
		* Retuns the item UID.
		* @return TSmlDsItemUid. Retuns the item UID.
		*/
		IMPORT_C TSmlDbItemUid ItemId() const;
		
		/**
		* Retuns the modification type.
		* @return TNSmlModificationItem::TNSmlModificationType. Type of modification.
		*/
		IMPORT_C TNSmlDbItemModification::TNSmlDbItemModificationType ModificationType() const;

		/**
		* Assignment operator
		* @param aDim. source for assignment
		* @return TNSmlDbItemModification&. Reference to this object.
		*/		
		TNSmlDbItemModification& operator=( const TNSmlDbItemModification& aDim );
		
	private:

		TSmlDbItemUid 					iItemId;
		TNSmlDbItemModificationType 	iModType;
	};

// ------------------------------------------------------------------------------------------------
// Class for transporting modifications over IPC.
//
// @lib nsmldshostclient.lib
// ------------------------------------------------------------------------------------------------
class RNSmlDbItemModificationSet
	{
	public:
			
		/**
		* Adds item to modification set.
		* @param aItem. Item that will be added to list.
		* @return TInt. KErrNone or one of the system wide error codes.
		*/
		IMPORT_C TInt AddItemL( const TNSmlDbItemModification& aItem );
		
		/**
		* Removes the item at a specified position from the modification set.
		* @param aIndex Index of the item to be removed.
		*/
		IMPORT_C void RemoveItem( TInt aIndex );
		
		/**
		* Adds group of items to modification set.
		* @param aUidSet. Items that will be added to list.
		* @param aModType. Modification type of all items.
		*/             
		IMPORT_C void AddGroupL( const MSmlDataItemUidSet& aUidSet, TNSmlDbItemModification::TNSmlDbItemModificationType aModType );
		
		/**
		* Returns the number of items in the set.
		* @return TInt. KErrNone or one of the system wide error codes.
		*/
		IMPORT_C TInt ItemCount() const;
		
		/**
		* Returns the index of modification item.
		* @param aItemId. UID of item.
		* @return TInt. The index of specified item UID in the set, or -1 if the item UID is not present.
		*/
		IMPORT_C TInt ItemIndex( const TSmlDbItemUid aItemId ) const;
		
		/**
		* Retuns the modifcation item.
		* @param aIndex. Indes of item.
		* @return TNSmlDbItemModification.Retuns the item at the specified index in the set.
		*/
		IMPORT_C TNSmlDbItemModification ItemAt( TInt aIndex ) const;
		
		/**
		* Resets modification set.
		*/
		IMPORT_C void Reset();
		
		/**
		* Closes modification set.
		*/
		IMPORT_C void Close();
		
		/**
		* This method externalizes class to stream.
		* @param aStream. Externalize is done to this stream.
		*/
		IMPORT_C void ExternalizeL( RWriteStream& aStream ) const;
		
		/**
		* This method internalizes class from stream.
		* @param aStream. Internalize is done from this stream.
		*/
		IMPORT_C void InternalizeL( RReadStream& aStream );
		
		/**
		* calculates size needed for stream when externalized.
		* @return TInt. Size in bytes.
		*/
		IMPORT_C TInt StreamSize() const;
		
	private:

		RArray<TNSmlDbItemModification> iItemSet;
	};
	
#endif

// End of File
