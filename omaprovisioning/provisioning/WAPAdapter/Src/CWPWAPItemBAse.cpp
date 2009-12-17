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
* Description:  Base class for WAP setting items.
*
*/



// INCLUDE FILES
#include <e32base.h>
#include <msvapi.h>
#include <mmsclient.h> 
#include <mtclreg.h>
#include <ApDataHandler.h>
#include <ApAccessPointItem.h>
#include <ApUtils.h>
#include <f32file.h>
#include <barsc.h>
#include <bautils.h>
#include <CWPCharacteristic.h>
#include <CWPParameter.h>
#include <WPAdapterUtil.h>
#include <CWPAdapter.h>
#include "CWPWAPItemBAse.h"
#include "ProvisioningDebug.h"

// CONSTANTS
const TInt KNamesGranularity = 1;
const TInt KNameMaxLength = 50;

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CWPWAPItemBase::CWPWAPItemBase
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CWPWAPItemBase::CWPWAPItemBase(  const TDesC& aTitle,
                                 const TDesC& aDefaultName, 
                                 CWPCharacteristic& aCharacteristic, 
                                 CCommsDatabase*& aCommsDb, 
                                 CApDataHandler*& aAPHandler )
                                : iCharacteristic( aCharacteristic ), 
                                  iTitle( aTitle ), 
                                  iDefaultName( aDefaultName ), 
                                  iCommsDb( aCommsDb ), 
                                  iAPHandler( aAPHandler )
    {
    }

// -----------------------------------------------------------------------------
// CWPWAPItemBase::BaseConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CWPWAPItemBase::BaseConstructL()
    {
    iCharacteristic.AcceptL( *this );

    if( iName == KNullDesC )
        {
        iName.Set( iDefaultName );
        }
    }

// -----------------------------------------------------------------------------
// Destructor
// -----------------------------------------------------------------------------
CWPWAPItemBase::~CWPWAPItemBase()
    {
    delete iAddr;
    }

// -----------------------------------------------------------------------------
// CWPWAPItemBase::VisitL
// -----------------------------------------------------------------------------
//
void CWPWAPItemBase::VisitL( CWPCharacteristic& aCharacteristic )
    {
    if( aCharacteristic.Type() == KWPResource )
        {
        ResourceL( aCharacteristic );
        }
    }

// -----------------------------------------------------------------------------
// CWPWAPItemBase::VisitL
// -----------------------------------------------------------------------------
//
void CWPWAPItemBase::VisitL( CWPParameter& aParameter )
    {
    switch( aParameter.ID() )
        {
        case EWPParameterName:
            {
            iName.Set( aParameter.Value() );
            break;
            }

        case EWPParameterAddr:
            {
            if( !iAddr )
                {
                iAddr = aParameter.Value().AllocL();
                }
            break;
            }

        default:
            {
            break;
            }
        }
    }

// -----------------------------------------------------------------------------
// CWPWAPItemBase::VisitLinkL
// -----------------------------------------------------------------------------
//
void CWPWAPItemBase::VisitLinkL( CWPCharacteristic& aLink )
    {
    if( !iLink )
        {
        iLink = &aLink;

        if( iName == KNullDesC )
            {
            // Get the name
            CArrayFix<TPtrC>* names = new(ELeave) CArrayFixFlat<TPtrC>( KNamesGranularity );
            CleanupStack::PushL( names );
            aLink.ParameterL( EWPParameterName, names );
            if( names->Count() > 0 )
                {
                iName.Set( names->At(0) );
                }
            CleanupStack::PopAndDestroy(); // names
            }
        }
    }

// -----------------------------------------------------------------------------
// CWPWAPItemBase::Name
// -----------------------------------------------------------------------------
//
const TDesC& CWPWAPItemBase::Name() const
    {
    return iName;
    }

// -----------------------------------------------------------------------------
// CWPWAPItemBase::Title
// -----------------------------------------------------------------------------
//
const TDesC& CWPWAPItemBase::Title() const
    {
    return iTitle;
    }

// -----------------------------------------------------------------------------
// CWPWAPItemBase::Address
// -----------------------------------------------------------------------------
//
const TDesC& CWPWAPItemBase::Address() const
    {
    if( iAddr )
        {
        return *iAddr;
        }
    else
        {
        return KNullDesC;
        }
    }

// -----------------------------------------------------------------------------
// CWPWAPItemBase::SaveData
// -----------------------------------------------------------------------------
//
const TDesC8& CWPWAPItemBase::SaveData() const
    {
    if( iSaveItem().iSaveItemType != KWPWAPItemTypeNone )
        {
        return iSaveItem;
        }
    else
        {
        return KNullDesC8;
        }
    }

// -----------------------------------------------------------------------------
// CWPWAPItemBase::CreateDbL
// -----------------------------------------------------------------------------
//
void CWPWAPItemBase::CreateDbL()
    {
    if( !iCommsDb )
        {
        iCommsDb = CCommsDatabase::NewL();
        }

    if( !iAPHandler )
        {
        iAPHandler = CApDataHandler::NewLC( *iCommsDb );
        CleanupStack::Pop(); // iAPEngine
        }
    }

// -----------------------------------------------------------------------------
// CWPWAPItemBase::WriteHomePageL
// -----------------------------------------------------------------------------
//
void CWPWAPItemBase::WriteHomePageL( CApAccessPointItem& aItem )
    {
    FLOG( _L( "[Provisioning] CWPWAPItemBase::WriteHomePageL:" ) );

    if( iAddr )
        {
        if( aItem.ReadTextLengthL( EApWapStartPage ) > 0 )
            {
            FLOG( _L( "[Provisioning] CWPWAPItemBase::WriteHomePageL: EApWapStartPage " ) );
            // Make a copy of the AP
            CApAccessPointItem* newItem = CApAccessPointItem::NewLC();
            newItem->CopyFromL( aItem );
            
            // Create a proper name for the copy
            TBuf<KNameMaxLength> name( iName.Left( KNameMaxLength ) );
            MakeValidNameL( name, *iCommsDb );  
            
            // Write name 
            User::LeaveIfError( newItem->WriteTextL( EApWapAccessPointName, name ) );
            
            //Write IAP name
            User::LeaveIfError( newItem->WriteTextL( EApIapName, name ) );
            // Write MMSC address
            User::LeaveIfError( newItem->WriteLongTextL( EApWapStartPage, *iAddr ) );
            
            // Create the new access point
            TRAPD( err, iUID = iAPHandler->CreateFromDataL( *newItem ) );
            
            FTRACE(RDebug::Print(_L("[Provisioning] CWPWAPItemBase::WriteHomePageL: CreateFromDataL err (%d)"), err));
            FTRACE(RDebug::Print(_L("[Provisioning] CWPWAPItemBase::WriteHomePageL: CreateFromDataL iUID (%d)"), iUID));
            
            if( err == KErrLocked )
                {
                err = EWPCommsDBLocked;
                }
            User::LeaveIfError( err );

            iSaveItem().iSaveItemType = KWPWAPItemTypeAccesspoint;
            iSaveItem().iUID = iUID;

            CleanupStack::PopAndDestroy(); // newItem
            }
        else
            {
            FLOG( _L( "[Provisioning] CWPWAPItemBase::WriteHomePageL: EApWapStartPage 0 " ) );
            User::LeaveIfError( aItem.WriteLongTextL( EApWapStartPage, *iAddr ) );
            
            // Update the access point
            TBool nameChanged( EFalse );
            TRAPD( err, iAPHandler->UpdateAccessPointDataL( aItem, nameChanged ) );
            FTRACE(RDebug::Print(_L("[Provisioning] CWPWAPItemBase::WriteHomePageL: CreateFromDataL err (%d)"), err));
            if( err == KErrLocked )
                {
                err = EWPCommsDBLocked;
                }
            User::LeaveIfError( err );
            iUID = aItem.WapUid();
            FTRACE(RDebug::Print(_L("[Provisioning] CWPWAPItemBase::WriteHomePageL: CreateFromDataL iUID (%d)"), iUID));
            }
        }
    }

// -----------------------------------------------------------------------------
// CWPWAPItemBase::MakeValidNameL
// -----------------------------------------------------------------------------
//
void CWPWAPItemBase::MakeValidNameL( TDes& aName, CCommsDatabase& aDb ) const
    {
    CApUtils* utils = CApUtils::NewLC( aDb );
    utils->MakeValidNameL( aName );
    CleanupStack::PopAndDestroy(); // utils
    }

//  End of File  
