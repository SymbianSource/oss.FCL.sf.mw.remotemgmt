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
* Description:  Provides item buffering during synchronisation.
*
*/


#ifndef __NSMLDSBATCHBUFFER_H__
#define __NSMLDSBATCHBUFFER_H__

// INCLUDES
#include <e32base.h>
#include <SmlDataSyncDefs.h>
#include "nsmlcliagconstants.h"

// FORWARD DECLARATIONS
class CNSmlDbMetaHandler;

// CLASS DECLARATION

/**
* Class used to buffer batched items during synchronisation.
*
* @lib nsmldsagent.lib
*/
class CNSmlDSBatchBuffer : public CBase
    {
    public: // constructors and destructor
        /**
        * C++ constructor.
        */
        CNSmlDSBatchBuffer();
        
        /**
        * Destructor.
        */
        ~CNSmlDSBatchBuffer();
        
    public: // new functions
        /**
        * Creates a new item to the buffer.
        * @param aCommand New item's associated command.
        */
        void CreateNewItemL( const TDesC8& aCommand );
        
        /**
        * Sets the status of the given item.
        * @param aIndex Index to the item in the buffer.
        * @param aStatus Item's status.
        */      
        void SetStatus( const TInt aIndex, const TInt aStatus );
        
        /**
        * Sets the item's id in the status container.
        * @param aEntry Item's status id.
        */
        void SetStatusEntryId( const TInt aEntry );
        
        /**
        * Resets the buffer and frees all associated resources.
        */
        void Reset();
        
        /**
        * Increases the operation counter of the current item by one.
        */
        void Increase();
        
        /**
        * Sets the current item's GUID.
        * @param aGUid New GUID value.
        */
        void SetGUidL( const TDesC8& aGUid );
        
        /**
        * Returns the number of items in the buffer.
        * @return The number of items in the buffer.
        */
        TInt Count() const;
        
        /**
        * Returns the given item's status code.
        * @param aIndex Index to an item.
        * @return Given item's status.
        */
        TInt Status( const TInt aIndex ) const;
        
        /**
        * Returns the given item's status container id.
        * @param aIndex Index to an item.
        * @return Given item's status container id.
        */
        TInt StatusEntryId( const TInt aIndex ) const;
        
        /**
        * Returns the number of operations associated with the given item.
        * @param aIndex Index to an item.
        * @return Number of operations.
        */
        TInt NumberOfResults( const TInt aIndex ) const;
        
        /**
        * Returns the command associated with the given item.
        * @param aIndex Index to an item.
        * @return Item's associated command.
        */      
        const TDesC8& Command( const TInt aIndex ) const;
        
        /**
        * Returns a reference to the current item's UID.
        * @return Reference to the UID.
        */
        TSmlDbItemUid& Uid() const;
        
        /**
        * Returns the UID of the given item.
        * @param aIndex Index to an item.
        * @return Given item's UID.
        */
        TSmlDbItemUid Uid( const TInt aIndex ) const;
        
        /**
        * Returns the GUID of the given item.
        * @param aIndex Index to an item.
        * @return Given item's GUID.
        */
        const TDesC8& GUid( const TInt aIndex ) const;

        /**
        * Sets current item's parent UID.
        * @param aParent Item's parent.
        */      
        void SetParent( const TSmlDbItemUid aParent );
        
        /**
        * Returns the given item's parent.
        * @param aIndex Index to an item.
        * @return Given item's parent.
        */
        TSmlDbItemUid Parent( const TInt aIndex ) const;
        
        /**
        * Sets current item's meta information.
        * @param aMetaHandler Item's meta information.
        */
        void SetMetaInformationL( const CNSmlDbMetaHandler* aMetaHandler );
        
        /**
        * Returns the given item's meta version.
        * @param aIndex Index to an item.
        * @return Given item's meta version.
        */
        TPtrC8 MimeVersion( const TInt aIndex ) const;
        
        /**
        * Returns the given item's meta type.
        * @param aIndex Index to an item.
        * @return Given item's meta version.
        */
        TPtrC8 MimeType( const TInt aIndex ) const;
        
        /**
        * Copies the given data under the current item.
        * @param aData Data to be stored.
        */
        void StoreItemDataL( TPtrC8 aData );                
        
        /**
        * Returns a pointer to the given item's data.
        * @param aIndex Index to an item.
        * @return Pointer to the item's data.
        */
        TPtrC8 ItemData( const TInt aIndex ) const;
        
    private: // constructor & operators
        /**
        * Prohibit copy constructor
        */
        CNSmlDSBatchBuffer( const CNSmlDSBatchBuffer& aOther );
        
        /**
        * Prohibit assignment operator.
        */
        CNSmlDSBatchBuffer& operator=( const CNSmlDSBatchBuffer aOther );
        
    private: // data
        // buffered item
        class CNSmlDSBatchItem : public CBase
            {
            public:
                CNSmlDSBatchItem();
                ~CNSmlDSBatchItem();
                TInt iStatus;
                TInt iNumberOfResults;
                TBuf8<KNSmlMaxCmdNameLength> iCommand;
                TSmlDbItemUid iUid;
                HBufC8* iGUid;
                TSmlDbItemUid iParent;
                TInt iStatusEntryId;
                HBufC8* iItemData;
                HBufC8* iMimeType;
                HBufC8* iMimeVersion;
            };
        
        // array of buffered items
        RPointerArray<CNSmlDSBatchItem> iBuffer;
    };

#endif // __NSMLDSBATCHBUFFER_H__

// End of File
