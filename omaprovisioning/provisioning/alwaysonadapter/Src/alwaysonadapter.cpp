/*
* Copyright (c) 2006-2006 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Handles Always-On settings in OMA provisioning.
*
*/


// INCLUDE FILES
#include    <f32file.h>
#include    <CWPCharacteristic.h>
#include    <CWPParameter.h>
#include    <pdpcontextmanagerinternalcrkeys.h>
#include    <centralrepository.h>

#include    "WPAdapterUtil.h"
#include    "alwaysonadapter.h"
#include    "ProvisioningDebug.h"

// CONSTANTS
_LIT( KAlwaysOnAwonPdpcStr, "AWON-PDPC" );
_LIT( KAlwaysOnTRetryStr, "T-RETRY" );

// AlwaysON PDP context status
// Parameters shall be binary encoded using the in-line string.
// Max length for each item is 2bytes.
// Values are XY, where X is related to 3G flag and Y is related to 2G flag.
// Possible values are:
//  *   "00", AlwaysOn is OFF for both 3G and 2G
//  *   "10", AlwaysOn is ON for 3G, OFF for 2G
//  *   "01", AlwaysOn is OFF for 3G, ON for 2G
//  *   "11", AlwaysOn is ON for 3G, ON for 2G
const TInt KAwonPdpc_Off3G_Off2G = 00;
const TInt KAwonPdpc_Off3G_On2G  = 01;
const TInt KAwonPdpc_On3G_Off2G  = 10;
const TInt KAwonPdpc_On3G_On2G   = 11;

// ======== LOCAL FUNCTIONS ========

// ======== MEMBER FUNCTIONS ========

// -----------------------------------------------------------------------------
// CAlwaysOnAdapter::CAlwaysOnAdapter
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CAlwaysOnAdapter::CAlwaysOnAdapter()
    {
    }

// -----------------------------------------------------------------------------
// CAlwaysOnAdapter::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CAlwaysOnAdapter::ConstructL()
    {
    iPdpCenrep = CRepository::NewL( KCRUidPDPContextManager );
    }

// -----------------------------------------------------------------------------
// CAlwaysOnAdapter::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CAlwaysOnAdapter* CAlwaysOnAdapter::NewL()
    {
    CAlwaysOnAdapter* self = new( ELeave ) CAlwaysOnAdapter;
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );

    return self;
    }

// -----------------------------------------------------------------------------
// CAlwaysOnAdapter::~CAlwaysOnAdapter
// Destructor
// -----------------------------------------------------------------------------
//
CAlwaysOnAdapter::~CAlwaysOnAdapter()
    {
    delete iVConfigData;
    delete iTempVConfigData;
    delete iPdpCenrep;
    }

// -----------------------------------------------------------------------------
// CAlwaysOnAdapter::ItemCount
// -----------------------------------------------------------------------------
//
TInt CAlwaysOnAdapter::ItemCount() const
    {
    // SummaryTitle(), SummaryText(), SaveL(), SetAsDefaultL() and
    // CanSetAsDefault() are called ItemCount() times
    return iVConfigData ? 1 : 0;
    }

// -----------------------------------------------------------------------------
// CAlwaysOnAdapter::SummaryTitle
// -----------------------------------------------------------------------------
//
const TDesC16& CAlwaysOnAdapter::SummaryTitle( TInt /*aIndex*/ ) const
    {
    return KNullDesC();
    }

// -----------------------------------------------------------------------------
// CAlwaysOnAdapter::SummaryText
// -----------------------------------------------------------------------------
//
const TDesC16& CAlwaysOnAdapter::SummaryText( TInt /*aIndex*/ ) const
    {
    return KNullDesC();
    }

// -----------------------------------------------------------------------------
// CAlwaysOnAdapter::DetailsL
// -----------------------------------------------------------------------------
//
TInt CAlwaysOnAdapter::DetailsL( TInt /*aItem*/, MWPPairVisitor& /*aVisitor*/ )
    {
    // Detail view is a feature for later release.
    return KErrNotSupported;
    }

// -----------------------------------------------------------------------------
// CAlwaysOnAdapter::SaveL
// -----------------------------------------------------------------------------
//
void CAlwaysOnAdapter::SaveL( TInt /*aItem*/ )
    {
    FLOG( _L( "[Provisioning] CAlwaysOnAdapter::SaveL:" ) );
    __ASSERT_DEBUG( iVConfigData, User::Leave( KErrCorrupt ) );

    User::LeaveIfError( iPdpCenrep->Set( KPDPContextManagerEnableWhenHome,
        TInt( iVConfigData->iAwonPdpcHPLMN ) ) );
    User::LeaveIfError( iPdpCenrep->Set( KPDPContextManagerEnableWhenRoaming,
        TInt( iVConfigData->iAwonPdpcVPLMN ) ) );
    User::LeaveIfError( iPdpCenrep->Set( KPDPContextManagerRetryTimer,
        iVConfigData->iTRetry ) );

    FLOG( _L( "[Provisioning] CStreamingAdapter::SaveL: Done" ) );
    }

// -----------------------------------------------------------------------------
// CAlwaysOnAdapter::CanSetAsDefault
// -----------------------------------------------------------------------------
//
TBool CAlwaysOnAdapter::CanSetAsDefault( TInt /*aItem*/ ) const
    {
    return EFalse;
    }

// -----------------------------------------------------------------------------
// CAlwaysOnAdapter::SetAsDefault
// -----------------------------------------------------------------------------
//
void CAlwaysOnAdapter::SetAsDefaultL( TInt /*aItem*/ )
    {
    // This shouldn't be called because CanSetAsDefault
    // always returns EFalse.
    }

// -----------------------------------------------------------------------------
// CAlwaysOnAdapter::VisitL
// -----------------------------------------------------------------------------
//
void CAlwaysOnAdapter::VisitL( CWPCharacteristic& aCharacteristic )
    {
    switch( aCharacteristic.Type() )
        {
        case KWPVendorConfig:
            {
            // Init settings temporary storage
            InitTempVConfigL();

            // Accept characteristic
            aCharacteristic.AcceptL( *this );

            // Validate data
            if( IsVConfigValid() )
                {
                // data is valid -> assign it to member variable
                delete iVConfigData;
                iVConfigData = iTempVConfigData;
                iTempVConfigData = NULL;
                }
            else
                {
                // data is invalid
                delete iTempVConfigData;
                iTempVConfigData = NULL;
                }
            break;
            }
        default:
            break;
        }
    }

// -----------------------------------------------------------------------------
// CAlwaysOnAdapter::VisitL
//
//   Used to parse, the following always-on related provisioning settings
//
//   <characteristic type="VENDORCONFIG">
//          <parm name="NAME" value="MRBLOBBY"/>
//          <parm name="AWON-PDPC" value="11"/>
//          <parm name="T-RETRY" value="100"/>
//      </characteristic>
//   </characteristic>
// -----------------------------------------------------------------------------
//
void CAlwaysOnAdapter::VisitL( CWPParameter& aParameter )
    {
    switch( aParameter.ID() )
        {
        case EWPParameterName:
            iTempVConfigData->iName.Set( aParameter.Value() );
            break;
        case EWPNamedParameter:
            {
            if( aParameter.Name().Compare( KAlwaysOnAwonPdpcStr ) == 0 )
                {
                TInt awonPdpcTemp( KErrNotFound );
                ParseIntegerL( aParameter.Value(), awonPdpcTemp );
                ParseAwonPdpcValuesL( awonPdpcTemp );
                }
            else if( aParameter.Name().Compare( KAlwaysOnTRetryStr ) == 0 )
                {
                ParseIntegerL( aParameter.Value(), iTempVConfigData->iTRetry );
                }
            break;
            }
        default:
            break;
        }
    }

// -----------------------------------------------------------------------------
// CAlwaysOnAdapter::VisitLinkL
// -----------------------------------------------------------------------------
//
void CAlwaysOnAdapter::VisitLinkL( CWPCharacteristic& /*aCharacteristic*/ )
    {
    }

// -----------------------------------------------------------------------------
// CAlwaysOnAdapter::IsVConfigValid
// -----------------------------------------------------------------------------
//
TBool CAlwaysOnAdapter::IsVConfigValid()
    {
    TBool validity( ETrue );

    // Validity check:
    //
    // T-Retry must be >= 0
    //      ** 0 means infinity
    //      ** > 0 is retry timer value in seconds
    if( iTempVConfigData->iTRetry < 0 )
        {
        validity = EFalse;
        }

    // AWON-PDPC
    //      ** Allowed awon-pdpc values are speficied in the beginning of file
    // There is no need to validate AWON-PDPC because if parsing has succeeded
    // the values are inside correct range.

    return validity;
    }

// -----------------------------------------------------------------------------
// CAlwaysOnAdapter::InitTempVConfigL
// -----------------------------------------------------------------------------
//
void CAlwaysOnAdapter::InitTempVConfigL()
    {
    delete iTempVConfigData;
    iTempVConfigData = NULL;
    iTempVConfigData = new ( ELeave ) TAlwaysOnVConfigAdapterData;

    // Initialize iTempVConfigData's members
    iTempVConfigData->iName.Set( KNullDesC );
    TInt temp( 0 );
    User::LeaveIfError(
        iPdpCenrep->Get( KPDPContextManagerEnableWhenHome, temp ) );
    iTempVConfigData->iAwonPdpcHPLMN = TBool( temp );
    User::LeaveIfError(
        iPdpCenrep->Get( KPDPContextManagerEnableWhenRoaming, temp ) );
    iTempVConfigData->iAwonPdpcVPLMN = TBool( temp );
    User::LeaveIfError( iPdpCenrep->Get( KPDPContextManagerRetryTimer,
                                iTempVConfigData->iTRetry ) );

    }

// -----------------------------------------------------------------------------
// CAlwaysOnAdapter::ParseIntegerL
// -----------------------------------------------------------------------------
//
void CAlwaysOnAdapter::ParseIntegerL( const TDesC& aPtr, TInt& aInt )
    {
    TLex lex( aPtr );
    User::LeaveIfError( lex.Val( aInt ) );
    }

// -----------------------------------------------------------------------------
// CAlwaysOnAdapter::ParseAwonPdpcValuesL
// -----------------------------------------------------------------------------
//
void CAlwaysOnAdapter::ParseAwonPdpcValuesL( TInt aInt )
    {
    switch( aInt )
        {
        case KAwonPdpc_Off3G_Off2G:
            iTempVConfigData->iAwonPdpcHPLMN = EFalse;
            iTempVConfigData->iAwonPdpcVPLMN = EFalse;
            break;
        case KAwonPdpc_Off3G_On2G:
            iTempVConfigData->iAwonPdpcHPLMN = EFalse;
            iTempVConfigData->iAwonPdpcVPLMN = ETrue;
            break;
        case KAwonPdpc_On3G_Off2G:
            iTempVConfigData->iAwonPdpcHPLMN = ETrue;
            iTempVConfigData->iAwonPdpcVPLMN = EFalse;
            break;
        case KAwonPdpc_On3G_On2G:
            iTempVConfigData->iAwonPdpcHPLMN = ETrue;
            iTempVConfigData->iAwonPdpcVPLMN = ETrue;
            break;
        default:
            User::Leave( KErrNotSupported );
            break;
        }
    }

//  End of File
