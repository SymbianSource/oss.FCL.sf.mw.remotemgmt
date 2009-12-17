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


#ifndef __NSMLDSHOSTITEM_H__ 
#define __NSMLDSHOSTITEM_H__

// INCLUDES
#include <e32base.h>
#include <s32strm.h>
#include <SyncMLDef.h>
#include <SmlDataSyncDefs.h>

// FORWARD DECLARATIONS
class CSmlDataStoreFormat;
class RSmlFilterDef;
class MSmlSyncRelationship;
class CNSmlDbCaps;
class MSmlDataItemUidSet;
class CDesC8Array;

// CLASS DECLARATION

// ------------------------------------------------------------------------------------------------
// Class for transporting item parameters over IPC.
//
// @lib nsmldshostclient.lib
// ------------------------------------------------------------------------------------------------
class CNSmlDSHostItem : public CBase
	{
	public:
		/**
		* Creates new instance of CNSmlDSHostItem based class.
		* @return CNSmlDSHostItem*. Newly created instance.
		*/
		IMPORT_C static CNSmlDSHostItem* NewL();
		
		/**
		* Creates new instance of CNSmlDSHostItem based class.
		* @return CNSmlDSHostItem*. Newly created instance that is pushed to cleanup stack.
		*/
		IMPORT_C static CNSmlDSHostItem* NewLC();
		
		/**
		* C++ Destructor.
		*/
		IMPORT_C ~CNSmlDSHostItem();

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
		* Returns UID of item.
		* @return TSmlDbItemUid. UID of item.
		*/
		IMPORT_C TSmlDbItemUid Uid() const;
		
		/**
		* Sets UID of item.
		* @param aItemId. UID of item.
		*/
		IMPORT_C void SetUid( const TSmlDbItemUid aItemId );
		
		/**
		* Returns fieldChange info.
		* @return TBool. ETrue if fieldChange is used.
		*/
		IMPORT_C TBool FieldChange() const;
		
		/**
		* Sets fieldChange info.
		* @param aFieldChange. ETrue if fieldChange is used.
		*/
		IMPORT_C void SetFieldChange( const TBool aFieldChange );
		
		/**
		* Returns size of item.
		* @return TInt. Size of item.
		*/
		IMPORT_C TInt Size() const;
		
		/**
		* Sets size of item.
		* @param aSize. Size of item.
		*/
		IMPORT_C void SetSize( const TInt aSize );
		
		/**
		* Returns UID of parent of item.
		* @return TSmlDbItemUid. UID of parent of item.
		*/
		IMPORT_C TSmlDbItemUid ParentUid() const;
		
		/**
		* Sets UID of parent of item.
		* @param aParentId. UID of parent of item.
		*/
		IMPORT_C void SetParentUid( const TSmlDbItemUid aParentId );
		
		/**
		* Returns mime type of item.
		* @return HBufC8*. Mime type of item.
		*/
		IMPORT_C const HBufC8* MimeType() const;
		
		/**
		* Sets mime type of item.
		* @param aMimeType. Mime type of item.
		*/
		IMPORT_C void SetMimeTypeL( const TDesC8& aMimeType );
		
		/**
		* Returns mime version of item.
		* @return HBufC8*. Mime version of item.
		*/
		IMPORT_C const HBufC8* MimeVer() const;
		
		/**
		* Sets mime version of item.
		* @param aMimeType. Mime version of item.
		*/
		IMPORT_C void SetMimeVerL( const TDesC8& aMimeType );
		
	protected:
		/**
		* C++ constructor.
		*/	
		IMPORT_C CNSmlDSHostItem();
		
		/**
		* ConstrucL
		*/	
		IMPORT_C void ConstructL();
			
	protected:
		// UID of item
		TSmlDbItemUid	iUid;
		// FieldChange or not
		TBool			iFieldChange;
		// Size of item
		TInt			iSize;
		// UID of parent of item
		TSmlDbItemUid	iParentUid;

	private:
		// Mime type of item
		HBufC8*			iMimeType;
		// Mime version of item
		HBufC8*			iMimeVer;
	};

#endif // __NSMLDSHOSTITEM_H__ 

// End of File
