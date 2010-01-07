/*
* Copyright (c) 2007-2008 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:    Receives and stores Destination network settings.
*
*/




#include <e32base.h>
#include <CWPAdapter.h>
#include <CWPCharacteristic.h>
#include <CWPParameter.h>
#include <wpdestinationnwadapter.rsg>
#include <e32des8.h>
#include <WPAdapterUtil.h> // Adapter utils
#include <utf.h> // Unicode conversion
#include <cmmanagerext.h>
#include <cmdestinationext.h>
#include <cmconnectionmethodext.h>
#include <cmmanagerdef.h>

#include "cwpdestinationnwadapter.h"
#include "wpdestinationnwdebug.h"

_LIT( KDestinationNwAdapterName, "WPDestinationNwAdapter");
_LIT( KDestinationNwAppID,       "w904C" ); // OMA CP registration document.
_LIT8( KDestinationNwAppID8,     "w904C" ); // OMA CP registration document.


// ---------------------------------------------------------------------------
// CWPDestinationNwAdapter::CWPDestinationNwAdapter
// ---------------------------------------------------------------------------
//
CWPDestinationNwAdapter::CWPDestinationNwAdapter() : CWPAdapter()
    {
    }

// ---------------------------------------------------------------------------
// CWPDestinationNwAdapter::ConstructL
// ---------------------------------------------------------------------------
//
void CWPDestinationNwAdapter::ConstructL()
    {
    DBG_PRINT( "CWPDestinationNwAdapter::ConstructL - begin" );
    TFileName fileName;
    Dll::FileName( fileName );
    // Read settings title from localization file. The title is shown to UI
    // when OMA CP message is provisioned using WAPPUSH.
    iDefaultTitle = WPAdapterUtil::ReadHBufCL( 
        fileName,
        KDestinationNwAdapterName,
        R_QTN_SM_HEAD_DESTINATION_NETWORK );
    // Read default settings name from localization file.
    iDefaultName = WPAdapterUtil::ReadHBufCL( 
        fileName,
        KDestinationNwAdapterName,
        R_QTN_SM_DEFAULT_NAME_DESTINATION_NETWORK );
    DBG_PRINT( "CWPDestinationNwAdapter::ConstructL - end" );
    }

// ---------------------------------------------------------------------------
// CWPDestinationNwAdapter::NewL
// ---------------------------------------------------------------------------
//
CWPDestinationNwAdapter* CWPDestinationNwAdapter::NewL()
    {
    CWPDestinationNwAdapter* self = new (ELeave) 
        CWPDestinationNwAdapter;
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// CWPDestinationNwAdapter::~CWPDestinationNwAdapter
// ---------------------------------------------------------------------------
//
CWPDestinationNwAdapter::~CWPDestinationNwAdapter()
    {
    DBG_PRINT( "CWPDestinationNwAdapter::~CWPDestinationNwAdapter - begin" );
    delete iDefaultTitle;
    delete iDefaultName;
    iItems.ResetAndDestroy();
    iItems.Close();
    DBG_PRINT( "CWPDestinationNwAdapter::~CWPDestinationNwAdapter - end" );
    }

// ---------------------------------------------------------------------------
// CWPDestinationNwAdapter::Des8ToUint32L()
// Converts an 8 bit descriptor to TUint32.
// ---------------------------------------------------------------------------
//
TUint32 CWPDestinationNwAdapter::Des8ToUint32L( const TDesC8& aDes )
    {
    TLex8 lex( aDes );
    TUint32 value( 0 );
    User::LeaveIfError( lex.Val( value, EDecimal ) );
    return value;
    }

// ---------------------------------------------------------------------------
// CWPDestinationNwAdapter::ItemCount
// From class CWPAdapter.
// Checks the number of settings items.
// ---------------------------------------------------------------------------
//
TInt CWPDestinationNwAdapter::ItemCount() const
    {
    return iItems.Count();
    }

// ---------------------------------------------------------------------------
// From class CWPAdapter.
// CWPDestinationNwAdapter::SummaryTitle
// Returns the summary title.
// ---------------------------------------------------------------------------
//
const TDesC16& CWPDestinationNwAdapter::SummaryTitle( TInt /*aIndex*/ ) const
    {
    return *iDefaultTitle;
    }

// ---------------------------------------------------------------------------
// CWPDestinationNwAdapter::SummaryText
// From class CWPAdapter.
// Returns the summary text of a destination network.
// ---------------------------------------------------------------------------
//
const TDesC16& CWPDestinationNwAdapter::SummaryText( TInt aIndex ) const
    {
    if ( !iItems[aIndex]->SettingsName().Length() )
        {
        return *iDefaultName;
        }
    return iItems[aIndex]->SettingsName();
    }

// ---------------------------------------------------------------------------
// CWPDestinationNwAdapter::SaveL
// From class CWPAdapter.
// Calls settings items saving method.
// ---------------------------------------------------------------------------
//
void CWPDestinationNwAdapter::SaveL( TInt aIndex )
    {
    __ASSERT_ALWAYS( aIndex >= 0 && aIndex < iItems.Count(),
        User::Leave( KErrArgument ) );

    iItems[aIndex]->SaveL();
    }

// ---------------------------------------------------------------------------
// CWPDestinationNwAdapter::CanSetAsDefault
// From class CWPAdapter.
// Not supported; always return EFalse.
// ---------------------------------------------------------------------------
//
TBool CWPDestinationNwAdapter::CanSetAsDefault( TInt /*aIndex*/ ) const
    {
    return EFalse;
    }

// ---------------------------------------------------------------------------
// CWPDestinationNwAdapter::DetailsL
// From class CWPAdapter.
// Not supported.
// ---------------------------------------------------------------------------
//
TInt CWPDestinationNwAdapter::DetailsL( TInt /*aItem*/, 
    MWPPairVisitor& /*aVisitor */ )
    {
    return KErrNotSupported;
    }

// ---------------------------------------------------------------------------
// CWPDestinationNwAdapter::VisitL
// From class CWPAdapter.
// OMA CP characteristics are handled in this method.
// ---------------------------------------------------------------------------
//
void CWPDestinationNwAdapter::VisitL( 
    CWPCharacteristic& aCharacteristic )
    {
    DBG_PRINT( "CWPDestinationNwAdapter::VisitL( characteristic ) - begin" );
    switch ( aCharacteristic.Type() )
        {
        // This adapter only handles APPLICATION characteristic.
        case KWPApplication:
            {
            iCurrentItem = CWPDestinationNwItem::NewL();
            // Set default name for destination - replaced later if a name
            // has been provisioned.
            iCurrentItem->SetSettingsNameL( iDefaultName->Des() );
            aCharacteristic.AcceptL( *this );

            // Settings are for Destination network - append current item to
            // item array to be saved later.
            if ( 0 == iAppId.Compare( KDestinationNwAppID() ) )
                {
                iItems.AppendL( iCurrentItem );
                }
            // Settings are not for Destination network.
            else
                {
                delete iCurrentItem;
                iCurrentItem = NULL;
                }
            break;
            }
        // All other than APPLICATION are ignored.
        default:
            break;
        }
    DBG_PRINT( "CWPDestinationNwAdapter::VisitL( characteristic ) - end" );
    }

// ---------------------------------------------------------------------------
// CWPDestinationNwAdapter::VisitL
// From class CWPAdapter.
// Destination network parameters are handled in this method.
// ---------------------------------------------------------------------------
//
void CWPDestinationNwAdapter::VisitL( CWPParameter& aParameter )
    {
    DBG_PRINT( "CWPDestinationNwAdapter::VisitL( parameter ) - begin" );
    // tmpValue holds the value converted from Unicode to UTF8.
    HBufC8* tmpValue = HBufC8::NewLC( aParameter.Value().Length() ); // CS:1
    TPtr8 ptrTmpValue( tmpValue->Des() );
    CnvUtfConverter::ConvertFromUnicodeToUtf8( ptrTmpValue,
        aParameter.Value() );

    switch ( aParameter.ID() )
        {
        // APPLICATION/APPID
        case EWPParameterAppID:
            {
            iAppId.Set( aParameter.Value() );
            break;
            }

        // APPLICATION/NAME
        case EWPParameterName:
            {
            if ( iCurrentItem )
                {
                iCurrentItem->SetSettingsNameL( aParameter.Value() );
                }
            break;
            }

        // APPLICATION/APPREF
        case EWPParameterAppRef:
            {
            if ( iCurrentItem )
                {
                iCurrentItem->SetAppRefL( tmpValue->Des() );
                }
            break;
            }

        // Other settings are ignored.
        default:
            {
            break;
            }
        }
    CleanupStack::PopAndDestroy( tmpValue ); // CS:0
    DBG_PRINT( "CWPDestinationNwAdapter::VisitL( parameter ) - end" );
    }

// ---------------------------------------------------------------------------
// CWPDestinationNwAdapter::VisitLinkL
//
// ---------------------------------------------------------------------------
//
void CWPDestinationNwAdapter::VisitLinkL( CWPCharacteristic& aLink )
    {
    if ( KWPNapDef == aLink.Type() && iCurrentItem )
        {
        iCurrentItem->AddNapDefL( &aLink );
        }
    }

// ---------------------------------------------------------------------------
// CWPDestinationNwAdapter::GetSavingInfoL
// APPID is set to [0], APPREF to [1] and saving info (storage ID) to [2].
// ---------------------------------------------------------------------------
//
void CWPDestinationNwAdapter::GetSavingInfoL( TInt aIndex, 
    RPointerArray<HBufC8>& aSavingInfo )
    {
    __ASSERT_ALWAYS( aIndex >= 0 && aIndex < iItems.Count(),
        User::Leave( KErrArgument ) );

    aSavingInfo.AppendL( KDestinationNwAppID8().AllocL() );
    aSavingInfo.AppendL( iItems[aIndex]->AppRef().AllocL() );
    aSavingInfo.AppendL( iItems[aIndex]->SaveData().AllocL() );
    }

// ---------------------------------------------------------------------------
// CWPDestinationNwAdapter::ContextExtension
// From class MWPContextExtension.
// Returns a pointer to context extension.
// ---------------------------------------------------------------------------
//
TInt CWPDestinationNwAdapter::ContextExtension( 
    MWPContextExtension*& aExtension )
    {
    aExtension = this;
    return KErrNone;
    }

// ---------------------------------------------------------------------------
// CWPDestinationNwAdapter::SaveDataL
// From class MWPContextExtension.
// Returns ID of saved data.
// ---------------------------------------------------------------------------
//
const TDesC8& CWPDestinationNwAdapter::SaveDataL( TInt aIndex ) const
    {
    __ASSERT_ALWAYS( aIndex >= 0 && aIndex < iItems.Count(),
        User::Leave( KErrArgument ) );

    return iItems[aIndex]->SaveData();
    }

// ---------------------------------------------------------------------------
// CWPDestinationNwAdapter::DeleteL
// From class MWPContextExtension.
// Deletes a Destination network.
// ---------------------------------------------------------------------------
//
void CWPDestinationNwAdapter::DeleteL( const TDesC8& aSaveData )
    {
    DBG_PRINT( "CWPDestinationNwAdapter::DeleteL - begin" );
    RCmManagerExt cmManager;
    cmManager.OpenL();
    CleanupClosePushL( cmManager ); // CS:1
    TUint32 destinationId = Des8ToUint32L( aSaveData );
    RCmDestinationExt destination = cmManager.DestinationL( destinationId );
    CleanupClosePushL( destination ); // CS:2
    const TInt cmCount = destination.ConnectionMethodCount();
    // First remove IAPs from destination network because this adapter must
    // not delete IAPs.
    for ( TInt counter = 0; counter < cmCount; counter++ )
        {
        destination.RemoveConnectionMethodL( 
            destination.ConnectionMethodL( 0 ) );
        destination.UpdateL();
        }
    CleanupStack::Pop( &destination ); // CS:1
    destination.DeleteLD();
    CleanupStack::PopAndDestroy( &cmManager ); // CS:0
    DBG_PRINT( "CWPDestinationNwAdapter::DeleteL - end" );
    }

// ---------------------------------------------------------------------------
// CWPDestinationNwAdapter::Uid
// From class MWPContextExtension.
// Returns the UID of this adapter.
// ---------------------------------------------------------------------------
//
TUint32 CWPDestinationNwAdapter::Uid() const
    {
    return iDtor_ID_Key.iUid;
    }
// End of file.
