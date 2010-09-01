/*
* Copyright (c) 2002-2006 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  One MMS setting item.
*
*/



// INCLUDE FILES
#include <e32base.h>
#include <msvapi.h>
#include <mmsclient.h> 
#include <mtclreg.h>
#include <ApDataHandler.h>
#include <ApAccessPointItem.h>
#include <CWPCharacteristic.h>
#include <CWPParameter.h>
#include <CWPAdapter.h>
#include <favouritesdb.h>
#include "CWPWAPMMSItem.h"
#include "WPWAPItemFactory.h"
#include "WPWAPUtil.h"

#ifdef __MESSAGING_API_V2__
#include <mmssettings.h>
#endif

// CONSTANTS
const TInt KUrlMaxLength = 255;

// -----------------------------------------------------------------------------
// CWPWAPMMSItem::CWPWAPMMSItem
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CWPWAPMMSItem::CWPWAPMMSItem(  const TDesC& aTitle, 
    const TDesC& aDefaultName, 
    CWPCharacteristic& aCharacteristic, 
    CCommsDatabase*& aDb, 
    CApDataHandler*& aAPHandler )
: CWPWAPItemBase( aTitle, aDefaultName, aCharacteristic, aDb, aAPHandler )
    {
    }

// -----------------------------------------------------------------------------
// CWPWAPMMSItem::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CWPWAPMMSItem::ConstructL()
    {
    BaseConstructL();
    }

// -----------------------------------------------------------------------------
// CWPWAPMMSItem::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CWPWAPMMSItem* CWPWAPMMSItem::NewL( const TDesC& aTitle, 
    const TDesC& aDefaultName, 
    CWPCharacteristic& aCharacteristic, 
    CCommsDatabase*& aDb, 
    CApDataHandler*& aAPHandler )
    {
    CWPWAPMMSItem* self = new(ELeave) CWPWAPMMSItem( aTitle, aDefaultName, 
        aCharacteristic, aDb, aAPHandler ); 
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
    }

// Destructor
CWPWAPMMSItem::~CWPWAPMMSItem()
    {
    iLinks.Close();
    }

// -----------------------------------------------------------------------------
// CWPWAPMMSItem::SaveL
// -----------------------------------------------------------------------------
//
void CWPWAPMMSItem::SaveL()
    {
    CreateDbL();

    // Find a proper GPRS access point
    CApAccessPointItem* item = FindGPRSLC();
    WriteHomePageL( *item );    
    CleanupStack::PopAndDestroy(); // item
    }

// -----------------------------------------------------------------------------
// CWPWAPMMSItem::CanSetAsDefault
// -----------------------------------------------------------------------------
//
TBool CWPWAPMMSItem::CanSetAsDefault() const
    {
    return ETrue;
    }


// -----------------------------------------------------------------------------
// CWPWAPMMSItem::SetAsDefaultL
// -----------------------------------------------------------------------------
//
void CWPWAPMMSItem::SetAsDefaultL()
    {
    // Open Messaging Server session
    CMsvSession* session = CMsvSession::OpenSyncL( *this );
    CleanupStack::PushL( session );

    // Read the MMS client information via registry
    CClientMtmRegistry* registry = CClientMtmRegistry::NewL( *session );
    CleanupStack::PushL( registry );
    
    CMmsClientMtm* mmsClient = 
        static_cast<CMmsClientMtm*>( registry->NewMtmL( KUidMsgTypeMultimedia ) );
    CleanupStack::PushL( mmsClient );

#ifdef __MESSAGING_API_V2__

    CMmsSettings* mmsSettings = CMmsSettings::NewL();
    CleanupStack::PushL( mmsSettings );
    
    mmsClient->RestoreSettingsL();

    mmsSettings->CopyL( mmsClient->MmsSettings() );
    TInt apCount =  mmsSettings->AccessPointCount();
    TInt index;
    for ( index = 0; index < apCount; index++)
	    {
	  	    mmsSettings->DeleteAccessPointL( 0 );	
	    }
    mmsSettings->AddAccessPointL( iUID, 0 );

    mmsClient->SetSettingsL( *mmsSettings );
    mmsClient->StoreSettingsL();

    CleanupStack::PopAndDestroy(); // mmsSettings

#else

    mmsClient->LoadMessageL();
    // Set the correct access point and save
    mmsClient->SetFirstAccessPoint( iUID );
    mmsClient->SaveMessageL();

#endif

    CleanupStack::PopAndDestroy(3); // mmsClient, registry, session
    }

// -----------------------------------------------------------------------------
// CWPWAPMMSItem::ValidateL
// -----------------------------------------------------------------------------
//
TBool CWPWAPMMSItem::ValidateL()
    {
    return iLinks.Count() > 0
        && iAddr
        && iAddr->Length() <=KUrlMaxLength
        && WPWAPUtil::CheckURI( *iAddr );
    }

// -----------------------------------------------------------------------------
// CWPWAPMMSItem::ResourceL
// -----------------------------------------------------------------------------
//
void CWPWAPMMSItem::ResourceL( CWPCharacteristic& aResource )
    { 
    RFavouritesDb   favouritesDb;
    MWPWAPItemBase* item = WPWAPItemFactory::CreateBookmarkLC( KNullDesC, 
        KNullDesC, aResource, iLink, favouritesDb ); // iDb is NULL but it's not used.

    // Not supporting resources without startpage.
    if ( item->CanSetAsDefault() && !iAddr )
        {
        iAddr = item->Address().AllocL();
        }
    CleanupStack::PopAndDestroy();
    }

// -----------------------------------------------------------------------------
// CWPWAPMMSItem::AddL
// -----------------------------------------------------------------------------
//
void CWPWAPMMSItem::AddL( RPointerArray<MWPWAPItemBase>& aShownItems,
                              RPointerArray<MWPWAPItemBase>& /*aHiddenItems*/ )
    {
    User::LeaveIfError( aShownItems.Append( this ) );
    }

// -----------------------------------------------------------------------------
// CWPWAPMMSItem::VisitLinkL
// -----------------------------------------------------------------------------
//
void CWPWAPMMSItem::VisitLinkL( CWPCharacteristic& aLink )
    {
    CWPWAPItemBase::VisitLinkL( aLink );
    User::LeaveIfError( iLinks.Append( &aLink ) );
    }

// -----------------------------------------------------------------------------
// CWPWAPMMSItem::HandleSessionEventL
// -----------------------------------------------------------------------------
//
void CWPWAPMMSItem::HandleSessionEventL(TMsvSessionEvent /*aEvent*/, 
    TAny* /*aArg1*/, TAny* /*aArg2*/, TAny* /*aArg3*/)
    {
    }

// -----------------------------------------------------------------------------
// CWPWAPMMSItem::FindGPRSL
// -----------------------------------------------------------------------------
//
CApAccessPointItem* CWPWAPMMSItem::FindGPRSLC() const
    {
    CApAccessPointItem* result = NULL;
    for( TInt i( 0 ); i < iLinks.Count() && result == NULL; i++ )
        {
        CWPCharacteristic* curr = iLinks[i];

        TPckgBuf<TUint32> uidPckg;
        const TInt pkgLength( uidPckg.MaxLength() );
        for( TInt dataNum( 0 ); 
            result == NULL && curr->Data( dataNum ).Length() == pkgLength; 
            dataNum++ )
            {
            uidPckg.Copy( curr->Data( dataNum ) );

            // Read the access point pointed to by TO-NAPID or TO-PROXY
            CApAccessPointItem* item = CApAccessPointItem::NewLC();
            iAPHandler->AccessPointDataL( uidPckg(), *item );

            if( item->BearerTypeL() == EApBearerTypeGPRS )
                {
                // Item left on cleanup stack
                result = item;
                }
            else
                {
                CleanupStack::PopAndDestroy(); // item
                }
            }
        }

    if( !result )
        {
        // Nothing left on cleanup stack. That doesn't matter as we leave.
        User::Leave( KErrNotFound );
        }

    return result;
    }

//  End of File  
