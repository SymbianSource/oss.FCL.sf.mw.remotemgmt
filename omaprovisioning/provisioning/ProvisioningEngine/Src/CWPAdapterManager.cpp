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
* Description:  Manager for all adapters.
*
*/


//  INCLUDE FILES
#include "WPAdapterFactory.h"
#include "CWPAdapterManager.h"
#include "CWPAdapter.h"
#include "CWPRoot.h"
#include "CWPEngine.pan"
#include "MWPContextManager.h"
#include "MWPContextExtension.h"
#include "ProvisioningDebug.h"

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CWPAdapterManager::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CWPAdapterManager* CWPAdapterManager::NewL()
    {
    CWPAdapterManager* self = NewLC();
    CleanupStack::Pop();
    
    return self;
    }

// -----------------------------------------------------------------------------
// CWPAdapterManager::NewLC
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CWPAdapterManager* CWPAdapterManager::NewLC()
    {
    CWPAdapterManager* self = new( ELeave ) CWPAdapterManager();
    
    CleanupStack::PushL( self );
    self->ConstructL();
    
    return self;
    }

// -----------------------------------------------------------------------------
// CWPAdapterManager::CWPAdapterManager
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CWPAdapterManager::CWPAdapterManager()
    {
    }

// -----------------------------------------------------------------------------
// CWPAdapterManager::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CWPAdapterManager::ConstructL()
    {
    // Create the adapters
	FLOG( _L( "CWPAdapterManager::ConstructL : Enter" ) );
    iAdapters = WPAdapterFactory::CreateAdaptersL();
    FLOG( _L( "CWPAdapterManager::ConstructL : Exit" ) );
    }

// Destructor
CWPAdapterManager::~CWPAdapterManager()
    {
    if( iAdapters )
        {
        iAdapters->ResetAndDestroy();
        delete iAdapters;
        }
    REComSession::FinalClose();
    iItemIndexes.Close();
    }

// -----------------------------------------------------------------------------
// CWPAdapterManager::MwpceCount
// -----------------------------------------------------------------------------
//
TInt CWPAdapterManager::MwpceCount() const
    {
    TInt count( 0 );
    for( TInt i( 0 ); i < iAdapters->Count(); i++ )
        {
        MWPContextExtension* extension = NULL;
        if( iAdapters->At( i )->ContextExtension( extension ) == KErrNone )
            {
            count++;
            }
        }

    return count;
    }

// -----------------------------------------------------------------------------
// CWPAdapterManager::MwpcePoint
// -----------------------------------------------------------------------------
//
MWPContextExtension& CWPAdapterManager::MwpcePoint( TInt aIndex )
    {
    MWPContextExtension* extension = NULL;
    for( TInt i( 0 ); i < iAdapters->Count() && aIndex >= 0; i++ )
        {
        if( iAdapters->At( i )->ContextExtension( extension ) == KErrNone )
            {
            --aIndex;
            }
        }

    return *extension;
    }

// -----------------------------------------------------------------------------
// CWPAdapterManager::PopulateL
// -----------------------------------------------------------------------------
//
void CWPAdapterManager::PopulateL( CWPRoot& aRoot )
    {
    FLOG( _L( "[Provisioning] CWPAdapterManager::PopulateL" ) );
    if( iAdapters->Count() > 0 )
        {
        // Feed the document into adapters.
        TInt i( 0 );
        for( i = 0; i < iAdapters->Count(); i++ )
            {
            FTRACE(RDebug::Print(_L("[Provisioning] CWPAdapterManager::PopulateL() (%d) out of (%d)"), i, iAdapters->Count()));
            CWPAdapter* adapter = iAdapters->At( i );
            TRAPD(retVal, aRoot.AcceptL( *adapter ) );
            if (retVal != KErrNone )
                {
                FTRACE(RDebug::Print(_L("[Provisioning] CWPAdapterManager::PopulateL() aRoot.AcceptL leaved (%d)"), retVal));
                }
            }
        }

    // Update item indexes.
    iItemCount = 0;
    for( TInt i( 0 ); i < iAdapters->Count(); i++ )
        {
        FTRACE(RDebug::Print(_L("[Provisioning] CWPAdapterManager::Append (%d) out of (%d)"), i, iAdapters->Count()));        
        User::LeaveIfError( iItemIndexes.Append( iItemCount ) );
        TInt count( iAdapters->At( i )->ItemCount() );
        iItemCount += count;
        }
    FLOG( _L( "[Provisioning] CWPAdapterManager::PopulateL done" ) );
    }

// -----------------------------------------------------------------------------
// CWPAdapterManager::ItemCount
// -----------------------------------------------------------------------------
//
TInt CWPAdapterManager::ItemCount() const
    {
    return iItemCount;
    }

// -----------------------------------------------------------------------------
// CWPAdapterManager::SummaryTitle
// -----------------------------------------------------------------------------
//
const TDesC16& CWPAdapterManager::SummaryTitle(TInt aIndex) const
    {
    __ASSERT_DEBUG( aIndex < iItemCount, Panic( EWPIndexOverflow ) );

    TInt adapter( 0 );
    TInt index( 0 );
    ItemIndex( aIndex, adapter, index );
    return iAdapters->At( adapter )->SummaryTitle( index );
    }

// -----------------------------------------------------------------------------
// CWPAdapterManager::SummaryText
// -----------------------------------------------------------------------------
//
const TDesC16& CWPAdapterManager::SummaryText(TInt aIndex) const
    {
    __ASSERT_DEBUG( aIndex < iItemCount, Panic( EWPIndexOverflow ) );

    TInt adapter( 0 );
    TInt index( 0 );
    ItemIndex( aIndex, adapter, index );
    return iAdapters->At( adapter )->SummaryText( index );
    }

// -----------------------------------------------------------------------------
// CWPAdapterManager::DetailsL
// -----------------------------------------------------------------------------
//
TInt CWPAdapterManager::DetailsL(TInt aIndex, MWPPairVisitor& aVisitor)
    {
    __ASSERT_DEBUG( aIndex < iItemCount, Panic( EWPIndexOverflow ) );

    TInt adapter( 0 );
    TInt index( 0 );
    ItemIndex( aIndex, adapter, index );
    return iAdapters->At( adapter )->DetailsL( index, aVisitor );
    }

// -----------------------------------------------------------------------------
// CWPAdapterManager::SaveL
// -----------------------------------------------------------------------------
//
void CWPAdapterManager::SaveL( MWPContextManager& aManager, TInt aItem )
    {
    __ASSERT_DEBUG( aItem < iItemCount, Panic( EWPIndexOverflow ) );

    TInt index( 0 );
    TInt adapter( 0 );
    ItemIndex( aItem, adapter, index );
    CWPAdapter* a = iAdapters->At( adapter );
    a->SaveL( index );

    MWPContextExtension* extension = NULL;
    if( a->ContextExtension( extension ) == KErrNone )
        {
        aManager.SaveL( *extension, index );
        }

// Code block based on the APPREF & TO-APPREF parameter handling starts here.

    // Place [0] is reserved for APPID parameter, [1] for APPREF parameter 
    // and [2] for storage id i.e. id that identifies the saved settings in its storage.
    RPointerArray< HBufC8 > savingInfo;
    a->GetSavingInfoL( index, savingInfo );

    // If the adapter had something to tell to the other adapters...
    if( ( savingInfo.Count() == 3 ) && ( savingInfo[ 1 ]->Length() > 0) )
        {
        // Information of the saved item is interesting only if it carries a 
        // value for APPREF parameter.
        if ( savingInfo[ 1 ] && ( savingInfo[ 1 ]->Length() > 0 ) )
            {
            TInt adaptersQuantity( iAdapters->Count() );

            for( TInt adaptersIndex = 0; adaptersIndex < adaptersQuantity; 
                adaptersIndex++ )
                {
                CWPAdapter* theInfoReceivingAdapter = iAdapters->At( adaptersIndex );
                if ( theInfoReceivingAdapter->ItemCount() > 0 )
                    {
                    // Value of APPID parameter: savingInfo[ 0 ].
                    HBufC8* tmpAppId = savingInfo[ 0 ]->Des().AllocLC();//CS:++
                    // savingInfo[ 1 ] holds the value of APPREF parameter of the characteristic 
                    // that was saved.
                    HBufC8* tmpAppRef = savingInfo[ 1 ]->Des().AllocLC();//CS:++
                    // savingInfo[ 2 ] holds the value that is the identitier of the settings in  its 
                    // own storage.
                    HBufC8* tmpStorageId = savingInfo[ 2 ]->Des().AllocLC();//CS:++
                    theInfoReceivingAdapter->SettingsSavedL ( tmpAppId->Des(), 
                    tmpAppRef->Des(), tmpStorageId->Des() );
                    theInfoReceivingAdapter = NULL;
                    CleanupStack::PopAndDestroy( tmpStorageId );//CS:--
                    tmpStorageId = NULL;
                    CleanupStack::PopAndDestroy( tmpAppRef );//CS:--
                    tmpAppRef = NULL;
                    CleanupStack::PopAndDestroy( tmpAppId );//CS:--
                    tmpAppId = NULL;
                    }// if
                }// for

            }// if
	    }// if
	savingInfo.ResetAndDestroy();
    savingInfo.Close();
    
    // Tell the adapters that all the settings have been now saved.
    TInt quantityOfAdapter( iAdapters->Count() );
    CWPAdapter* tmpAdapter = NULL;
    if (  aItem == ( iItemCount - 1 ))
        {
        for ( TInt adapterIndex = 0; adapterIndex < quantityOfAdapter; adapterIndex++ )
            {
            tmpAdapter = iAdapters->At( adapterIndex );
            tmpAdapter->SavingFinalizedL();
            tmpAdapter = NULL;
            }// for
        }
    
// Code block based on the APPREF & TO-APPREF parameter handling ends here.
    }

// -----------------------------------------------------------------------------
// CWPAdapterManager::CanSetAsDefault
// -----------------------------------------------------------------------------
//
TBool CWPAdapterManager::CanSetAsDefault( TInt aItem ) const
    {
    __ASSERT_DEBUG( aItem < iItemCount, Panic( EWPIndexOverflow ) );

    TInt index( 0 );
    TInt adapter( 0 );
    ItemIndex( aItem, adapter, index );
    return iAdapters->At( adapter )->CanSetAsDefault( index );
    }

// -----------------------------------------------------------------------------
// CWPAdapterManager::SetAsDefaultL
// -----------------------------------------------------------------------------
//
void CWPAdapterManager::SetAsDefaultL( TInt aItem )
    {
    __ASSERT_DEBUG( aItem < iItemCount, Panic( EWPIndexOverflow ) );

    TInt index( 0 );
    TInt adapter( 0 );
    ItemIndex( aItem, adapter, index );
    iAdapters->At( adapter )->SetAsDefaultL( index );
    }

// -----------------------------------------------------------------------------
// CWPAdapterManager::ItemIndex
// -----------------------------------------------------------------------------
//
void CWPAdapterManager::ItemIndex(TInt aGlobalIndex, 
                                  TInt& aAdapter, 
                                  TInt& aAdapterIndex ) const
    {
    // Find the adapter which provides item number aGlobalIndex
    aAdapter = iItemIndexes.Count()-1;
    for( TInt i( 0 ); i < iItemIndexes.Count(); i++ )
        {
        if( iItemIndexes[i] <= aGlobalIndex )
            {
            aAdapter = i;
            }
        }

    // Find out the adapter local item number
    aAdapterIndex = aGlobalIndex - iItemIndexes[aAdapter];

    __ASSERT_DEBUG( aAdapter < iAdapters->Count(), Panic( EWPIndexOverflow ) );
    }

//  End of File  
