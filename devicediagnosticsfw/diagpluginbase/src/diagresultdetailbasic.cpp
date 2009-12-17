/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Class definition of CDiagResultDetailBasic
*
*/


// CLASS DECLARATION
#include "diagresultdetailbasicitem.h"  // CDiagResultDetailBasicItem

// SYSTEM INCLUDE FILES
#include <DiagResultDetailBasic.h>
#include <s32mem.h>                     // RBufReadStream / RBufWriteStream

// USER INCLUDE FILES
#include "diagpluginbase.pan"           // panic codes


// CONSTANTS
static const TInt KDiagResultDetailBasicBufferSize = 1024;

// ======== LOCAL FUNCTIONS ========
// ---------------------------------------------------------------------------
// Compares two items by order field id
// ---------------------------------------------------------------------------
//
static TInt CompareItemById( const CDiagResultDetailBasicItem& aFirst,
                             const CDiagResultDetailBasicItem& aSecond )
    {
    return aFirst.FieldId() - aSecond.FieldId();
    }

// ======== MEMBER FUNCTIONS ========
// ---------------------------------------------------------------------------
// CDiagResultDetailBasic::CDiagResultDetailBasic()
// ---------------------------------------------------------------------------
//
EXPORT_C CDiagResultDetailBasic::CDiagResultDetailBasic( 
        CDiagResultsDatabaseItem::TResult aResult )
    :   iResult( aResult )
    {
    }

// ---------------------------------------------------------------------------
// CDiagResultDetailBasic::~CDiagResultDetailBasic()
// ---------------------------------------------------------------------------
//
EXPORT_C CDiagResultDetailBasic::~CDiagResultDetailBasic()
    {
    iFields.ResetAndDestroy();
    iFields.Close();
    }

// ---------------------------------------------------------------------------
// CDiagResultDetailBasic::BaseConstructL
// ---------------------------------------------------------------------------
//
EXPORT_C void CDiagResultDetailBasic::BaseConstructL( RReadStream& aReadStream )
    {
    InternalizeL( aReadStream );
    }

// ---------------------------------------------------------------------------
// CDiagResultDetailBasic::BaseConstructL
// ---------------------------------------------------------------------------
//
EXPORT_C void CDiagResultDetailBasic::BaseConstructL( const CBufFlat& aBuf )
    {
    // if empty buffer, do nothing.
    if ( aBuf.Size() == 0 )
        {
        return;
        }

    RBufReadStream readStream;

    readStream.Open( aBuf, 0 );

    InternalizeL( readStream );

    readStream.Close();
    }

// ---------------------------------------------------------------------------
// CDiagResultDetailBasic::SetResult()
// ---------------------------------------------------------------------------
//
EXPORT_C void CDiagResultDetailBasic::SetResult( 
        CDiagResultsDatabaseItem::TResult aResult )
    {
    iResult = aResult;
    }

// ---------------------------------------------------------------------------
// CDiagResultDetailBasic::SetValueL
// ---------------------------------------------------------------------------
//
EXPORT_C void CDiagResultDetailBasic::SetValueL( TInt aFieldId,
                                                 const TDesC8& aFieldName,
                                                 TInt aValue )
    {
    CDiagResultDetailBasicItem* newItem = CDiagResultDetailBasicItem::NewL( 
        aFieldId,
        aFieldName,
        aValue );

    // no need to call clean up stack since ownership is
    // transferred.
    SetItemL( newItem );
    newItem = NULL;
    }


// ---------------------------------------------------------------------------
// CDiagResultDetailBasic::SetValueL
// ---------------------------------------------------------------------------
//
EXPORT_C void CDiagResultDetailBasic::SetValueL( TInt aFieldId,
                                                 const TDesC8& aFieldName,
                                                 const TDesC8& aValue )
                
    {
    CDiagResultDetailBasicItem* newItem = CDiagResultDetailBasicItem::NewL( 
        aFieldId,
        aFieldName,
        aValue );
    
    // no need to call clean up stack since ownership is
    // transferred.
    SetItemL( newItem );
    newItem = NULL;
    }


// ---------------------------------------------------------------------------
// CDiagResultDetailBasic::SetValueL
// ---------------------------------------------------------------------------
//
EXPORT_C void CDiagResultDetailBasic::SetValueL( TInt aFieldId,
                                                 const TDesC8& aFieldName,
                                                 const TDesC16& aValue )
                
    {
    CDiagResultDetailBasicItem* newItem = CDiagResultDetailBasicItem::NewL( 
        aFieldId,
        aFieldName,
        aValue );
    
    // no need to call clean up stack since ownership is
    // transferred.
    SetItemL( newItem );
    newItem = NULL;
    }


// ---------------------------------------------------------------------------
// CDiagResultDetailBasic::GetValue
// ---------------------------------------------------------------------------
//
EXPORT_C TInt CDiagResultDetailBasic::GetValue( TInt aFieldId,
                                                TInt& aValue ) const
    {
    TInt itemIndex = FindItem( aFieldId );

    if ( itemIndex < 0 )
        {
        // item not found
        return KErrNotFound;
        }

    return iFields[itemIndex]->GetValue( aValue );
    }

// ---------------------------------------------------------------------------
// CDiagResultDetailBasic::GetValueL
// ---------------------------------------------------------------------------
//
EXPORT_C TInt CDiagResultDetailBasic::GetValue( TInt aFieldId,
                                                TPtrC8& aValue ) const
    {
    TInt itemIndex = FindItem( aFieldId );

    if ( itemIndex < 0 )
        {
        // item not found
        return KErrNotFound;
        }
    
    return iFields[itemIndex]->GetValue( aValue );
    }

// ---------------------------------------------------------------------------
// CDiagResultDetailBasic::GetValueL
// ---------------------------------------------------------------------------
//
EXPORT_C TInt CDiagResultDetailBasic::GetValue( TInt aFieldId,
                                                TPtrC16& aValue ) const
    {
    TInt itemIndex = FindItem( aFieldId );

    if ( itemIndex < 0 )
        {
        // item not found
        return KErrNotFound;
        }
    
    return iFields[itemIndex]->GetValue( aValue );
    }

// ---------------------------------------------------------------------------
// CDiagResultDetailBasic::InternalizeL
// ---------------------------------------------------------------------------
//
void CDiagResultDetailBasic::InternalizeL( RReadStream& aReadStream )
    {
    iFields.ResetAndDestroy();

    // Make sure that we are dealing with the correct version of data format.
    TUint32 version = aReadStream.ReadUint32L();

    if ( version != ( TUint32 )KDiagResultDetailBasicVersion )
        {
        User::Leave( KErrCorrupt );
        }

    // Read the number of items in the list.
    TUint32 count = aReadStream.ReadUint32L();

    // Read each item
    for ( TUint i = 0; i < count; i++ )
        {
        CDiagResultDetailBasicItem* newItem = 
            CDiagResultDetailBasicItem::NewL( aReadStream );

        SetItemL( newItem ); // ownership passed.
        }
    }


// ---------------------------------------------------------------------------
// CDiagResultDetailBasic::ExternalizeL
// ---------------------------------------------------------------------------
//
EXPORT_C void CDiagResultDetailBasic::ExternalizeL(
        RWriteStream& aWriteStream ) const
    {
    // write only if there is something to write..
    if ( iFields.Count() > 0 )
        {
        aWriteStream.WriteUint32L( KDiagResultDetailBasicVersion );
        aWriteStream.WriteUint32L( iFields.Count() );

        for ( TInt i = 0; i < iFields.Count(); i++ )
            {
            iFields[i]->ExternalizeL( aWriteStream );
            }
        }
    }


// ---------------------------------------------------------------------------
// CDiagResultDetailBasic::ExternalizeToBufferL
// ---------------------------------------------------------------------------
//
EXPORT_C CBufFlat* CDiagResultDetailBasic::ExternalizeToBufferL() const
    {
    CBufFlat* buffer = CBufFlat::NewL( KDiagResultDetailBasicBufferSize );

    CleanupStack::PushL( buffer );

    RBufWriteStream bufStream;
    bufStream.Open( *buffer );

    ExternalizeL( bufStream );

    bufStream.CommitL();
    bufStream.Close();

    CleanupStack::Pop( buffer );

    return buffer;
    }


// ---------------------------------------------------------------------------
// CDiagResultDetailBasic::GetSupportedOutput
// ---------------------------------------------------------------------------
//
EXPORT_C void CDiagResultDetailBasic::GetSupportedOutput(
        RArray<TOutputFormat>& /* aOutputList */ ) const
    {
    }

// ---------------------------------------------------------------------------
// CDiagResultDetailBasic::IsOutputSupported
// ---------------------------------------------------------------------------
//
EXPORT_C TBool CDiagResultDetailBasic::IsOutputSupported(
        MDiagResultDetail::TOutputFormat /* aOutputFormat */ ) const
    {
    return EFalse;
    }

// ---------------------------------------------------------------------------
// CDiagResultDetailBasic::GetOutputL
// ---------------------------------------------------------------------------
//
EXPORT_C void CDiagResultDetailBasic::GetOutputL(
        MDiagResultDetail::TOutputFormat /* aOutputFormat */,
        RBuf& /* aBuffer */ ) const
    {
    User::Leave( KErrNotSupported );
    }

// ---------------------------------------------------------------------------
// CDiagResultDetailBasic::CreateIconL
// ---------------------------------------------------------------------------
//
EXPORT_C CGulIcon* CDiagResultDetailBasic::CreateIconL() const
    {
    User::Leave( KErrNotSupported );
    return NULL;
    }

// ---------------------------------------------------------------------------
// CDiagResultDetailBasic::SetItemL
// ---------------------------------------------------------------------------
//
void CDiagResultDetailBasic::SetItemL( CDiagResultDetailBasicItem* aNewItem )
    {
    // Check if an item with same field id already exists.
    TInt itemIndex = FindItem( aNewItem->FieldId() );
    
    if ( itemIndex >= 0 )
        {
        // duplicate found. Remove old one.
        CDiagResultDetailBasicItem* oldItem = iFields[itemIndex];
        iFields.Remove( itemIndex );
        delete oldItem;
        }

    CleanupStack::PushL( aNewItem );
    TLinearOrder<CDiagResultDetailBasicItem> order( *CompareItemById );

    // if a duplicate item is still found, leave.
    User::LeaveIfError( iFields.InsertInOrder( aNewItem, order ) );
    CleanupStack::Pop( aNewItem );
    }

// ---------------------------------------------------------------------------
// CDiagResultDetailBasic::FindItem
// ---------------------------------------------------------------------------
//
TInt CDiagResultDetailBasic::FindItem( TInt aFieldId ) const
    {
    for ( TInt i = 0; i < iFields.Count(); i++ )
        {
        if ( iFields[i]->FieldId() == aFieldId )
            {
            return i;
            }
        }
    return -1;
    }

// End of File

