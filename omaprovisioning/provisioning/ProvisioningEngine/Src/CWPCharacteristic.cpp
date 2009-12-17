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
* Description:  One characteristic.
*
*/


//  INCLUDE FILES
#include "CWPCharacteristic.h"
#include <s32std.h>
#include <e32svr.h>
#include "MWPVisitor.h"
#include "CWPParameter.h"
#include "WPElementFactory.h"
#include "CWPEngine.pan"
#include "WPEngineDebug.h"

// CONSTANTS
/// Default data item number in Data() and SetData(), if none supplied
const TInt KDefaultDataNum = 0;
/// Data array granularity
const TInt KDataGranularity = 3;

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CWPCharacteristic::CWPCharacteristic
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CWPCharacteristic::CWPCharacteristic( TInt aType )
: iType( aType )
    {
    }

// -----------------------------------------------------------------------------
// CWPCharacteristic::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CWPCharacteristic::ConstructL()
    {
    }

// -----------------------------------------------------------------------------
// CWPCharacteristic::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CWPCharacteristic* CWPCharacteristic::NewL( TInt aType )
    {
    CWPCharacteristic* self = NewLC( aType );
    CleanupStack::Pop();
    
    return self;
    }

// -----------------------------------------------------------------------------
// CWPCharacteristic::NewLC
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CWPCharacteristic* CWPCharacteristic::NewLC( TInt aType )
    {
    CWPCharacteristic* self = new( ELeave ) CWPCharacteristic( aType );
    
    CleanupStack::PushL( self );
    self->ConstructL();
    
    return self;
    }

// Destructor
CWPCharacteristic::~CWPCharacteristic()
    {
    iElements.ResetAndDestroy();
    iElements.Close();
    iLinks.Close();
    if ( iData )
        {
        iData->ResetAndDestroy();
        }
    delete iData;
    delete iName;
    }

// -----------------------------------------------------------------------------
// CWPCharacteristic::InsertL
// -----------------------------------------------------------------------------
//
EXPORT_C void CWPCharacteristic::InsertL( MWPElement* aElement )
    {
    __ASSERT_DEBUG( aElement != NULL, Panic( EWPNullElement ) );

    User::LeaveIfError( iElements.Append( aElement ) );
    }

// -----------------------------------------------------------------------------
// CWPCharacteristic::InsertLinkL
// -----------------------------------------------------------------------------
//
EXPORT_C void CWPCharacteristic::InsertLinkL( CWPCharacteristic& aParameter )
    {
    __ASSERT_DEBUG( aParameter.Type() == KWPPxLogical 
        || aParameter.Type() == KWPNapDef,
        Panic( EWPIllegalLink ) );

    User::LeaveIfError( iLinks.Append( &aParameter ) );
    }

// -----------------------------------------------------------------------------
// CWPCharacteristic::SetNameL
// -----------------------------------------------------------------------------
//
EXPORT_C void CWPCharacteristic::SetNameL( const TDesC& aName )
    {
    HBufC* name = aName.AllocL();
    delete iName;
    iName = name;
    }

// -----------------------------------------------------------------------------
// CWPCharacteristic::Name
// -----------------------------------------------------------------------------
//
EXPORT_C const TDesC& CWPCharacteristic::Name() const
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
// CWPCharacteristic::SetDataL
// -----------------------------------------------------------------------------
//
EXPORT_C void CWPCharacteristic::SetDataL( const TDesC8& aData )
    {
    SetDataL( aData, KDefaultDataNum );
    }

// -----------------------------------------------------------------------------
// CWPCharacteristic::SetDataL
// -----------------------------------------------------------------------------
//
EXPORT_C void CWPCharacteristic::SetDataL( const TDesC8& aData, TInt aIndex )
    {
    HBufC8* data = aData.AllocLC();

    if ( !iData )
        {
        iData = new (ELeave) CArrayPtrSeg<HBufC8>( KDataGranularity );
        }

    // Check if there's already item in given index
    if ( iData->Count() > aIndex )
        {
        HBufC8* oldData = iData->At(aIndex);
        iData->Delete( aIndex );
        delete oldData;
        oldData = NULL;
        iData->InsertL( aIndex, data );
        CleanupStack::Pop( data );
        }
    else // Not as many items in array as pointed by index. Append as last item.
        {
        iData->AppendL( data );
        CleanupStack::Pop( data );
        }
    }

// -----------------------------------------------------------------------------
// CWPCharacteristic::DeleteAllData
// -----------------------------------------------------------------------------
//
EXPORT_C void CWPCharacteristic::DeleteAllData()
    {
    if ( iData )
        {
        iData->ResetAndDestroy();
        delete iData;
        iData = NULL;
        }   
    }

// -----------------------------------------------------------------------------
// CWPCharacteristic::Data
// -----------------------------------------------------------------------------
//
EXPORT_C const TDesC8& CWPCharacteristic::Data() const
    {
    return Data( KDefaultDataNum );
    }

// -----------------------------------------------------------------------------
// CWPCharacteristic::Data
// -----------------------------------------------------------------------------
//
EXPORT_C const TDesC8& CWPCharacteristic::Data( TInt aIndex ) const
    {
    if ( iData && iData->Count() > 0 && aIndex >= 0 && aIndex < iData->Count() )
        {
        return *iData->At( aIndex );
        }
    else
        {
        return KNullDesC8;
        }
    }

// -----------------------------------------------------------------------------
// CWPCharacteristic::InternalizeL
// -----------------------------------------------------------------------------
//
void CWPCharacteristic::InternalizeL(RReadStream& aStream)
    {
    TInt paramCount( aStream.ReadInt32L() );

    for( TInt i( 0 ); i < paramCount; i++ )
        {
        TInt type( aStream.ReadInt32L() );

        MWPElement* newElement = WPElementFactory::CreateLC( type );
        newElement->InternalizeL( aStream );
        User::LeaveIfError( iElements.Append( newElement ) );
        CleanupStack::Pop(); // newElement
        }
    }

// -----------------------------------------------------------------------------
// CWPCharacteristic::ExternalizeL
// -----------------------------------------------------------------------------
//
void CWPCharacteristic::ExternalizeL(RWriteStream& aStream) const
    {
    aStream.WriteInt32L( iElements.Count() );

    for( TInt i( 0 ); i < iElements.Count(); i++ )
        {
        const MWPElement* element = iElements[i];
        aStream.WriteInt32L( element->Type() );
        element->ExternalizeL( aStream );
        }
    }

// -----------------------------------------------------------------------------
// CWPCharacteristic::CallVisitorL
// -----------------------------------------------------------------------------
//
void CWPCharacteristic::CallVisitorL( MWPVisitor& aVisitor )
    {
    aVisitor.VisitL( *this );
    }

// -----------------------------------------------------------------------------
// CWPCharacteristic::Type
// -----------------------------------------------------------------------------
//
TInt CWPCharacteristic::Type() const
    {
    return iType;
    }

// -----------------------------------------------------------------------------
// CWPCharacteristic::AcceptL
// -----------------------------------------------------------------------------
//
EXPORT_C void CWPCharacteristic::AcceptL(MWPVisitor& aVisitor)
    {
    for( TInt i( 0 ); i < iElements.Count(); i++ )
        {
        iElements[i]->CallVisitorL( aVisitor );
        }

    for( TInt j( 0 ); j < iLinks.Count(); j++ )
        {
        aVisitor.VisitLinkL( *iLinks[j] );
        }
    }

// -----------------------------------------------------------------------------
// CWPCharacteristic::ParameterL
// -----------------------------------------------------------------------------
//
EXPORT_C void CWPCharacteristic::ParameterL( TInt aID, 
                                            CArrayFix<TPtrC>* aParameters )
    {
    aParameters->Reset();

    for( TInt i( 0 ); i < iElements.Count(); i++ )
        {
        if( iElements[i]->Type() == KWPParameter )
            {
            CWPParameter* parameter = STATIC_CAST(CWPParameter*, iElements[i]);

            if( parameter->ID() == aID )
                {
                aParameters->AppendL( parameter->Value() );
                }
            }
        }
    }

//  End of File  
