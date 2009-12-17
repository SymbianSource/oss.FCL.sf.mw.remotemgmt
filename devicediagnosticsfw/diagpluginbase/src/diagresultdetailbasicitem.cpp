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
* Description:  Class definition of CDiagResultDetailBasicItem
*
*/


// SYSTEM INCLUDE FILES
#include <s32strm.h>            // RReadStream, RWriteStream

// USER INCLUDE FILES
#include "diagresultdetailbasicitem.h"

// CONSTANTS
const TInt KDiagResultDetailBasicMaxNameLen = 50;
const TInt KDiagResultDetailBasicMaxFieldValueLen = 1024;

// ======== MEMBER FUNCTIONS ========
// ---------------------------------------------------------------------------
// CDiagResultDetailBasicItem::NewL
// ---------------------------------------------------------------------------
//
CDiagResultDetailBasicItem* CDiagResultDetailBasicItem::NewL(
            TInt aFieldId,
            const TDesC8& aFieldName,
            TInt aValue )
    {
    CDiagResultDetailBasicItem* self = new( ELeave )
        CDiagResultDetailBasicItem();
    CleanupStack::PushL( self );

    self->iFieldId = aFieldId;
    self->iFieldName = aFieldName.AllocL();
    self->iFieldType = ETypeInt;
    self->iValue.iInt = aValue;
    
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// CDiagResultDetailBasicItem::NewL
// ---------------------------------------------------------------------------
//
CDiagResultDetailBasicItem* CDiagResultDetailBasicItem::NewL(
            TInt aFieldId,
            const TDesC8& aFieldName,
            const TDesC8& aValue )
    {
    CDiagResultDetailBasicItem* self = new( ELeave )
        CDiagResultDetailBasicItem();
    CleanupStack::PushL( self );

    self->iFieldId = aFieldId;
    self->iFieldName = aFieldName.AllocL();
    self->iFieldType = ETypeDes8;
    self->iValue.iBuffer8 = aValue.AllocL();
    
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// CDiagResultDetailBasicItem::NewL
// ---------------------------------------------------------------------------
//
CDiagResultDetailBasicItem* CDiagResultDetailBasicItem::NewL(
            TInt aFieldId,
            const TDesC8& aFieldName,
            const TDesC16& aValue )
    {
    CDiagResultDetailBasicItem* self = new( ELeave )
        CDiagResultDetailBasicItem();
    CleanupStack::PushL( self );

    self->iFieldId = aFieldId;
    self->iFieldName = aFieldName.AllocL();
    self->iFieldType = ETypeDes16;
    self->iValue.iBuffer16 = aValue.AllocL();
    
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// CDiagResultDetailBasicItem::NewL
// ---------------------------------------------------------------------------
//
CDiagResultDetailBasicItem* CDiagResultDetailBasicItem::NewL(
            RReadStream& aReadStream )
    {
    CDiagResultDetailBasicItem* self = new( ELeave )
        CDiagResultDetailBasicItem();
    CleanupStack::PushL( self );

    self->InternalizeL( aReadStream );
    
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// CDiagResultDetailBasicItem::CDiagResultDetailBasicItem
// ---------------------------------------------------------------------------
//
CDiagResultDetailBasicItem::CDiagResultDetailBasicItem()
    {
    }

// ---------------------------------------------------------------------------
// CDiagResultDetailBasicItem::~CDiagResultDetailBasicItem
// ---------------------------------------------------------------------------
//
CDiagResultDetailBasicItem::~CDiagResultDetailBasicItem()
    {
    delete iFieldName;
    iFieldName = NULL;

    switch ( Type() )
        {
        case ETypeDes16:
            delete iValue.iBuffer16;
            iValue.iBuffer16 = NULL;
            break;
        case ETypeDes8:
            delete iValue.iBuffer8;
            iValue.iBuffer8 = NULL;
            break;
        default:
            // do nothing
            break;
        }
    }

// ---------------------------------------------------------------------------
// CDiagResultDetailBasicItem::FieldId
// ---------------------------------------------------------------------------
//
TInt CDiagResultDetailBasicItem::FieldId() const
    {
    return iFieldId;
    }

// ---------------------------------------------------------------------------
// CDiagResultDetailBasicItem::FieldName
// ---------------------------------------------------------------------------
//
const TDesC8& CDiagResultDetailBasicItem::FieldName() const
    {
    return *iFieldName;
    }

// ---------------------------------------------------------------------------
// CDiagResultDetailBasicItem::Type
// ---------------------------------------------------------------------------
//
CDiagResultDetailBasicItem::TFieldType CDiagResultDetailBasicItem::Type() const
    {
    return iFieldType;
    }

// ---------------------------------------------------------------------------
// CDiagResultDetailBasicItem::GetValue
// ---------------------------------------------------------------------------
//
TInt CDiagResultDetailBasicItem::GetValue( TInt& aValue ) const
    {
    if ( Type() != ETypeInt )
        return KErrArgument;

    aValue = iValue.iInt;
    return KErrNone;
    }

// ---------------------------------------------------------------------------
// CDiagResultDetailBasicItem::GetValue
// ---------------------------------------------------------------------------
//
TInt CDiagResultDetailBasicItem::GetValue( TPtrC8& aValue ) const
    {
    if ( Type() != ETypeDes8 )
        return KErrArgument;
    
    aValue.Set( iValue.iBuffer8->Des() );
    return KErrNone;
    }

// ---------------------------------------------------------------------------
// CDiagResultDetailBasicItem::GetValue
// ---------------------------------------------------------------------------
//
TInt CDiagResultDetailBasicItem::GetValue( TPtrC16& aValue ) const
    {
    if ( Type() != ETypeDes16 )
        return KErrArgument;
    

    aValue.Set( iValue.iBuffer16->Des() );
    return KErrNone;
    }

// ---------------------------------------------------------------------------
// CDiagResultDetailBasicItem::InternalizeL
// ---------------------------------------------------------------------------
//
void CDiagResultDetailBasicItem::InternalizeL( RReadStream& aReadStream )
    {
    // first read id
    iFieldId = aReadStream.ReadInt32L();

    // read field Type
    iFieldType = ( TFieldType ) aReadStream.ReadUint32L();

    // read field name
    iFieldName = HBufC8::NewL( aReadStream, KDiagResultDetailBasicMaxNameLen );

    switch ( Type() )
        {
        case ETypeInt:
            iValue.iInt = aReadStream.ReadInt32L();
            break;
        case ETypeDes16:
            iValue.iBuffer16 = HBufC16::NewL( aReadStream, 
                KDiagResultDetailBasicMaxFieldValueLen );
            break;
        case ETypeDes8:
            iValue.iBuffer8 = HBufC8::NewL( aReadStream, 
                KDiagResultDetailBasicMaxFieldValueLen );
            break;
        default:
            User::Leave( KErrCorrupt );
        }
    }

// ---------------------------------------------------------------------------
// CDiagResultDetailBasicItem::ExternalizeL
// ---------------------------------------------------------------------------
//
void CDiagResultDetailBasicItem::ExternalizeL( RWriteStream& aWriteStream ) const
    {
    // first field id
    aWriteStream.WriteInt32L( iFieldId );

    // write field Type
    aWriteStream.WriteUint32L( ( TUint )Type() );

    // write field name
    aWriteStream << FieldName();

    switch ( Type() )
        {
        case ETypeInt:
            aWriteStream.WriteInt32L( iValue.iInt );
            break;
        case ETypeDes16:
            aWriteStream << *( iValue.iBuffer16 );
            break;
        case ETypeDes8:
            aWriteStream << *( iValue.iBuffer8 );
            break;
        default:
            User::Leave( KErrCorrupt );
        }
    }

// End of File

