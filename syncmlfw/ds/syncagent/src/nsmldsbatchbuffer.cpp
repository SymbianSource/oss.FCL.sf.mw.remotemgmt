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


// INCLUDE FILES
#include "nsmldbcaps.h"
#include "nsmldsbatchbuffer.h"

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CNSmlDSBatchBuffer::CNSmlDSBatchItem::CNSmlDSBatchBuffer
// C++ constructor.
// -----------------------------------------------------------------------------
//
CNSmlDSBatchBuffer::CNSmlDSBatchItem::CNSmlDSBatchItem()
    {
    }

// -----------------------------------------------------------------------------
// CNSmlDSBatchBuffer::CNSmlDSBatchItem::~CNSmlDSBatchBuffer
// Destructor.
// -----------------------------------------------------------------------------
//
CNSmlDSBatchBuffer::CNSmlDSBatchItem::~CNSmlDSBatchItem()
    {
    delete iItemData;
    delete iMimeType;
    delete iMimeVersion;
    delete iGUid;
    }
    
// -----------------------------------------------------------------------------
// CNSmlDSBatchBuffer::CNSmlDSBatchBuffer
// C++ constructor.
// -----------------------------------------------------------------------------
//
CNSmlDSBatchBuffer::CNSmlDSBatchBuffer()
    {   
    }
    
// -----------------------------------------------------------------------------
// CNSmlDSBatchBuffer::~CNSmlDSBatchBuffer
// Destructor.
// -----------------------------------------------------------------------------
//
CNSmlDSBatchBuffer::~CNSmlDSBatchBuffer()
    {
    iBuffer.ResetAndDestroy();
    }
        
// -----------------------------------------------------------------------------
// CNSmlDSBatchBuffer::CreateNewItemL
// Creates a new item to the buffer.
// -----------------------------------------------------------------------------
//
void CNSmlDSBatchBuffer::CreateNewItemL( const TDesC8& aCommand )
    {
    CNSmlDSBatchItem* newItem = new ( ELeave ) CNSmlDSBatchItem;
    
    newItem->iStatus = 0;
    newItem->iNumberOfResults = 0;
    newItem->iCommand = aCommand;
    newItem->iUid = KNullDataItemId;
    newItem->iUid = KNullDataItemId;
    newItem->iStatusEntryId = 0;

    iBuffer.AppendL( newItem );
    }
    
// -----------------------------------------------------------------------------
// CNSmlDSBatchBuffer::SetStatus
// Sets the status id of the given item.
// -----------------------------------------------------------------------------
//
void CNSmlDSBatchBuffer::SetStatus( const TInt aIndex, const TInt aStatus )
    {
    iBuffer[aIndex]->iStatus = aStatus;
    }
    
// -----------------------------------------------------------------------------
// CNSmlDSBatchBuffer::SetStatusEntryId
// Sets the status container id of the current item.
// -----------------------------------------------------------------------------
//
void CNSmlDSBatchBuffer::SetStatusEntryId( const TInt aEntry )
    {
    iBuffer[iBuffer.Count() - 1]->iStatusEntryId = aEntry;
    }
    
// -----------------------------------------------------------------------------
// CNSmlDSBatchBuffer::Reset
// Resets the buffer.
// -----------------------------------------------------------------------------
//
void CNSmlDSBatchBuffer::Reset()
    {
    iBuffer.ResetAndDestroy();
    }
    
// -----------------------------------------------------------------------------
// CNSmlDSBatchBuffer::Count
// Returns the number of items in the buffer.
// -----------------------------------------------------------------------------
//
TInt CNSmlDSBatchBuffer::Count() const
    {
    return iBuffer.Count();
    }
    
// -----------------------------------------------------------------------------
// CNSmlDSBatchBuffer::Status
// Returns given item's status code.
// -----------------------------------------------------------------------------
//
TInt CNSmlDSBatchBuffer::Status( const TInt aIndex ) const
    {
    return iBuffer[aIndex]->iStatus;
    }
    
// -----------------------------------------------------------------------------
// CNSmlDSBatchBuffer::StatusEntryId
// Returns given item's status container id.
// -----------------------------------------------------------------------------
//
TInt CNSmlDSBatchBuffer::StatusEntryId( const TInt aIndex ) const
    {
    return iBuffer[aIndex]->iStatusEntryId;
    }
    
// -----------------------------------------------------------------------------
// CNSmlDSBatchBuffer::NumberOfResults
// Returns the number of operations associated with the given item.
// -----------------------------------------------------------------------------
//
TInt CNSmlDSBatchBuffer::NumberOfResults( const TInt aIndex ) const
    {
    return iBuffer[aIndex]->iNumberOfResults;
    }

// -----------------------------------------------------------------------------
// CNSmlDSBatchBuffer::Increase
// Increases item's operation counter by one.
// -----------------------------------------------------------------------------
//
void CNSmlDSBatchBuffer::Increase()
    {
    ++iBuffer[iBuffer.Count() - 1]->iNumberOfResults;
    }
    
// -----------------------------------------------------------------------------
// CNSmlDSBatchBuffer::SetGUidL
// Sets current item's GUID.
// -----------------------------------------------------------------------------
//
void CNSmlDSBatchBuffer::SetGUidL( const TDesC8& aGUid )
    {
    iBuffer[iBuffer.Count() - 1]->iGUid = aGUid.AllocL();
    }
    
// -----------------------------------------------------------------------------
// CNSmlDSBatchBuffer::Command
// Gets given item's associated command.
// -----------------------------------------------------------------------------
//
const TDesC8& CNSmlDSBatchBuffer::Command( const TInt aIndex ) const
    {
    return iBuffer[aIndex]->iCommand;
    }
    
// -----------------------------------------------------------------------------
// CNSmlDSBatchBuffer::Uid
// Returns given item's UID.
// -----------------------------------------------------------------------------
//
TSmlDbItemUid CNSmlDSBatchBuffer::Uid( const TInt aIndex ) const
    {
    return iBuffer[aIndex]->iUid;
    }
    
// -----------------------------------------------------------------------------
// CNSmlDSBatchBuffer::Uid
// Returns a reference to the current item's UID.
// -----------------------------------------------------------------------------
//
TSmlDbItemUid& CNSmlDSBatchBuffer::Uid() const
    {
    return iBuffer[iBuffer.Count() - 1]->iUid;
    }
    
// -----------------------------------------------------------------------------
// CNSmlDSBatchBuffer::GUid
// Returns given item's GUID.
// -----------------------------------------------------------------------------
//
const TDesC8& CNSmlDSBatchBuffer::GUid( const TInt aIndex ) const
    {
    return *iBuffer[aIndex]->iGUid;
    }

// -----------------------------------------------------------------------------
// CNSmlDSBatchBuffer::StoreItemDataL
// Stores data to the current item.
// -----------------------------------------------------------------------------
//
void CNSmlDSBatchBuffer::StoreItemDataL( TPtrC8 aData )
    {
    iBuffer[iBuffer.Count() - 1]->iItemData = aData.AllocL();
    }
    
// -----------------------------------------------------------------------------
// CNSmlDSBatchBuffer::ItemData
// Returns a pointer to the given item's data.
// -----------------------------------------------------------------------------
//
TPtrC8 CNSmlDSBatchBuffer::ItemData( TInt aIndex ) const
    {
    return *iBuffer[aIndex]->iItemData;
    }

// -----------------------------------------------------------------------------
// CNSmlDSBatchBuffer::SetParent
// Sets current item's parent.
// -----------------------------------------------------------------------------
//
void CNSmlDSBatchBuffer::SetParent( const TSmlDbItemUid aParent )
    {
    iBuffer[iBuffer.Count() - 1]->iParent = aParent;
    }
        
// -----------------------------------------------------------------------------
// CNSmlDSBatchBuffer::Parent
// Returns the given item's parent.
// -----------------------------------------------------------------------------
//
TSmlDbItemUid CNSmlDSBatchBuffer::Parent( const TInt aIndex ) const
    {
    return iBuffer[aIndex]->iParent;
    }
        
// -----------------------------------------------------------------------------
// CNSmlDSBatchBuffer::SetMetaInformationL
// Sets the current item's meta information.
// -----------------------------------------------------------------------------
//
void CNSmlDSBatchBuffer::SetMetaInformationL( const CNSmlDbMetaHandler* aMetaHandler )
    {
    iBuffer[iBuffer.Count() - 1]->iMimeType = aMetaHandler->Type().AllocL();
    iBuffer[iBuffer.Count() - 1]->iMimeVersion = aMetaHandler->Version().AllocL();
    }
        
// -----------------------------------------------------------------------------
// CNSmlDSBatchBuffer::MetaVersion
// Returns the given item's meta version.
// -----------------------------------------------------------------------------
//
TPtrC8 CNSmlDSBatchBuffer::MimeVersion( const TInt aIndex ) const
    {
    return *iBuffer[aIndex]->iMimeVersion;
    }

// -----------------------------------------------------------------------------
// CNSmlDSBatchBuffer::MetaVersion
// Returns the given item's meta type.
// -----------------------------------------------------------------------------
//
TPtrC8 CNSmlDSBatchBuffer::MimeType( const TInt aIndex ) const
    {
    return *iBuffer[aIndex]->iMimeType;
    }

// End of File
