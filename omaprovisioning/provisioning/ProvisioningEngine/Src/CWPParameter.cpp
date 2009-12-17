/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  One parameter
*
*/


//  INCLUDE FILES
#include <s32file.h>
#include "CWPParameter.h"
#include "MWPVisitor.h"
#include "WPElementFactory.h"

// CONSTANTS
/// Number of bits in half-word
const TInt KBitsPerWord = 16;
/// Mask to extract a half-word
const TInt KWordMask = 0xffff;

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CWPParameter::CWPParameter
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CWPParameter::CWPParameter()
    {
    }

// -----------------------------------------------------------------------------
// CWPParameter::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CWPParameter::ConstructL()
    {
    iValue = KNullDesC().AllocL();
    }

// -----------------------------------------------------------------------------
// CWPRoot::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CWPParameter* CWPParameter::NewL()
    {
    CWPParameter* self = NewLC();
    CleanupStack::Pop();
    
    return self;
    }

// -----------------------------------------------------------------------------
// CWPRoot::NewLC
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CWPParameter* CWPParameter::NewLC()
    {
    CWPParameter* self = new( ELeave ) CWPParameter;
    
    CleanupStack::PushL( self );
    self->ConstructL();
    
    return self;
    }

// Destructor
CWPParameter::~CWPParameter()
    {
    delete iName;
    delete iValue;
    }

// -----------------------------------------------------------------------------
// CWPParameter::CallVisitorL
// -----------------------------------------------------------------------------
//
void CWPParameter::CallVisitorL( MWPVisitor& aVisitor )
    {
    aVisitor.VisitL( *this );
    }

// -----------------------------------------------------------------------------
// CWPParameter::InternalizeL
// -----------------------------------------------------------------------------
//
EXPORT_C void CWPParameter::InternalizeL(RReadStream& aStream)
    {
    TInt parameterID( aStream.ReadInt32L() );
    TUint length( aStream.ReadInt32L() );
    // Lengths packed into one 32-bit word for compatibility
    TInt valueLength( length & KWordMask );
    TInt nameLength( (length >> KBitsPerWord) & KWordMask );
    
    HBufC* value = HBufC::NewL( aStream, valueLength );
    delete iValue;
    iValue = value;
    
    if( nameLength > 0 )
        {
        HBufC* name = HBufC::NewL( aStream, nameLength );
        delete iName;
        iName = name;
        }

    iParameterID = parameterID;
    }

// -----------------------------------------------------------------------------
// CWPParameter::ExternalizeL
// -----------------------------------------------------------------------------
//
EXPORT_C void CWPParameter::ExternalizeL(RWriteStream& aStream) const
    {
    aStream.WriteInt32L( iParameterID );

    // Lengths packed into one 32-bit word for compatibility
    TUint length( 0 );
    if( iValue )
        {
        length += iValue->Length();
        }
    if( iName )
        {
        length += iName->Length() << KBitsPerWord;
        }

    aStream.WriteUint32L( length );

    aStream << *iValue;

    if( iName )
        {
        aStream << *iName;
        }
    }

// -----------------------------------------------------------------------------
// CWPParameter::Type
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CWPParameter::Type() const
    {
    return KWPParameter;
    }

// -----------------------------------------------------------------------------
// CWPParameter::SetID
// -----------------------------------------------------------------------------
//
EXPORT_C void CWPParameter::SetID( TInt aID )
    {
    iParameterID = aID;
    }


// -----------------------------------------------------------------------------
// CWPParameter::ID
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CWPParameter::ID() const
    {
    return iParameterID;
    }

// -----------------------------------------------------------------------------
// CWPParameter::SetNameL
// -----------------------------------------------------------------------------
//
EXPORT_C void CWPParameter::SetNameL( const TDesC& aName )
    {
    HBufC* name = aName.AllocL();
    delete iName;
    iName = name;
    }

// -----------------------------------------------------------------------------
// CWPParameter::Name
// -----------------------------------------------------------------------------
//
EXPORT_C const TDesC& CWPParameter::Name() const
    {
    if( iName )
        {
        return *iName;
        }
    else
        {
        return KNullDesC;
        }
    }

// -----------------------------------------------------------------------------
// CWPParameter::SetValueL
// -----------------------------------------------------------------------------
//
EXPORT_C void CWPParameter::SetValueL( const TDesC& aValue )
    {
    HBufC* value = aValue.AllocL();
    delete iValue;
    iValue = value;
    }

// -----------------------------------------------------------------------------
// CWPParameter::Value
// -----------------------------------------------------------------------------
//
EXPORT_C const TDesC& CWPParameter::Value() const
    {
    if( iValue )
        {
        return *iValue;
        }
    else
        {
        return KNullDesC;
        }
    }


//  End of File  
