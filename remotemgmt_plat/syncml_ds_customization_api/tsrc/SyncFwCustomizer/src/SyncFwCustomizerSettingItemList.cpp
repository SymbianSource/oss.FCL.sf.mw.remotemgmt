/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: 
*
*/

#include <avkon.hrh>
#include <avkon.rsg>
#include <eikmenup.h>
#include <aknappui.h>
#include <eikcmobs.h>
#include <barsread.h>
#include <stringloader.h>
#include <gdi.h>
#include <eikedwin.h>
#include <eikenv.h>
#include <aknnumedwin.h>
#include <aknpopupfieldtext.h>
#include <eikappui.h>
#include <aknviewappui.h>
#include <SyncFwCustomizer.rsg>
#include <centralrepository.h>
#include <NSmlOperatorDataCRKeys.h>

#include "SyncFwCustomizerSettingItemList.h"
#include "SyncFwCustomizerSettingItemListSettings.h"
#include "SyncFwCustomizer.hrh"
#include "SyncFwCustomizerSettingItemList.hrh"
#include "SyncFwCustomizerSettingItemListView.h"

/**
 * Construct the CSyncFwCustomizerSettingItemList instance
 * @param aCommandObserver command observer
 */ 
CSyncFwCustomizerSettingItemList::CSyncFwCustomizerSettingItemList( 
		TSyncFwCustomizerSettingItemListSettings& aSettings, 
		MEikCommandObserver* aCommandObserver,
        CRepository& aRepository )
	: iSettings( aSettings ), 
	  iCommandObserver( aCommandObserver ), 
	  iRepository( aRepository )
	{
	}

/** 
 * Destroy any instance variables
 */
CSyncFwCustomizerSettingItemList::~CSyncFwCustomizerSettingItemList()
	{
	}

/**
 * Handle system notification that the container's size has changed.
 */
void CSyncFwCustomizerSettingItemList::SizeChanged()
	{
	if ( ListBox() ) 
		{
		ListBox()->SetRect( Rect() );
		}
	}

/**
 * Create one setting item at a time, identified by id.
 * CAknSettingItemList calls this method and takes ownership
 * of the returned value.  The CAknSettingItem object owns
 * a reference to the underlying data, which EditItemL() uses
 * to edit and store the value.
 */
CAknSettingItem* CSyncFwCustomizerSettingItemList::CreateSettingItemL( TInt aId )
	{
	switch ( aId )
		{
		case ESyncFwCustomizerSettingItemListViewEdit1:
			{			
			CAknTextSettingItem* item = new ( ELeave ) 
				CAknTextSettingItem( 
					aId,
					iSettings.ServerUrl() );
			return item;
			}
			
		case ESyncFwCustomizerSettingItemListViewSyncAdapterEditor:
			{			
			CAknIntegerEdwinSettingItem* item = new ( ELeave ) 
				CAknIntegerEdwinSettingItem( 
					aId,
					iSettings.OperatorAdapterUid() );
			return item;			
			}
			
		case ESyncFwCustomizerSettingItemListViewIntegerEditor2:
			{			
			CAknIntegerEdwinSettingItem* item = new ( ELeave ) 
				CAknIntegerEdwinSettingItem( 
					aId,
					iSettings.ProfileAdapterUid() );
			return item;
			}
			
		case ESyncFwCustomizerSettingItemListViewEdit2:
			{			
			CAknTextSettingItem* item = new ( ELeave ) 
				CAknTextSettingItem( 
					aId,
					iSettings.SwV() );
			return item;
			}
			
		case ESyncFwCustomizerSettingItemListViewEdit3:
			{			
			CAknTextSettingItem* item = new ( ELeave ) 
				CAknTextSettingItem( 
					aId,
					iSettings.Mod() );
			return item;
			}
			
		case ESyncFwCustomizerSettingItemListViewEnumeratedTextPopup1:
			{			
			CAknEnumeratedTextPopupSettingItem* item = new ( ELeave ) 
				CAknEnumeratedTextPopupSettingItem( 
					aId,
					iSettings.SyncProfileVisibility() );
			return item;
			}
			
		case ESyncFwCustomizerSettingItemListViewEdit4:
			{			
			CAknTextSettingItem* item = new ( ELeave ) 
				CAknTextSettingItem( 
					aId,
					iSettings.OperatorServerId() );
			return item;
			}
		}
		
	return NULL;
	}
	
/**
 * Edit the setting item identified by the given id and store
 * the changes into the store.
 * @param aIndex the index of the setting item in SettingItemArray()
 * @param aCalledFromMenu true: a menu item invoked editing, thus
 *	always show the edit page and interactively edit the item;
 *	false: change the item in place if possible, else show the edit page
 */
void CSyncFwCustomizerSettingItemList::EditItemL ( TInt aIndex, TBool aCalledFromMenu )
	{
	CAknSettingItem* item = ( *SettingItemArray() )[aIndex];
	switch ( item->Identifier() )
		{
		}
	
	CAknSettingItemList::EditItemL( aIndex, aCalledFromMenu );
	
	TBool storeValue = ETrue;
	switch ( item->Identifier() )
		{
		}
		
	if ( storeValue )
		{
		item->StoreL();
		SaveSettingValuesL();
		}	
	}

/**
 *	Handle the "Change" option on the Options menu.  This is an
 *	alternative to the Selection key that forces the settings page
 *	to come up rather than changing the value in place (if possible).
 */
void CSyncFwCustomizerSettingItemList::ChangeSelectedItemL()
	{
	if ( ListBox()->CurrentItemIndex() >= 0 )
		{
		EditItemL( ListBox()->CurrentItemIndex(), ETrue );
		}
	}

void CSyncFwCustomizerSettingItemList::ResetSelectedItemL()
    {
    TInt key = 0;
    switch( ListBox()->CurrentItemIndex() )
        {
        case 0:
            key = KNsmlOpDsOperatorSyncServerURL;
            break;
        case 1:
            key = KNsmlOpDsOperatorAdapterUid;
            break;
        case 2:
            key = KNsmlOpDsProfileAdapterUid;
            break;
        case 3:
            key = KNsmlOpDsDevInfoSwVValue;
            break;
        case 4:
            key = KNsmlOpDsDevInfoModValue;
            break;
        case 5:
            key =  KNsmlOpDsSyncProfileVisibility;
            break;
        case 6:
            key = KNsmlOpDsOperatorSyncServerId;
            break;
        default:
            iRepository.Reset();
            break;
        }

    iRepository.Reset( key );
    LoadSettingValuesL();
    LoadSettingsL();
    DrawDeferred();
    }

/**
 *	Load the initial contents of the setting items.  By default,
 *	the setting items are populated with the default values from
 * 	the design.  You can override those values here.
 *	<p>
 *	Note: this call alone does not update the UI.  
 *	LoadSettingsL() must be called afterwards.
 */
void CSyncFwCustomizerSettingItemList::LoadSettingValuesL()
	{
    TBuf<100> tmpDesc;  
    TBuf8<100> tmpDesc8;  
    TInt tmpInt = 0;
    
    iRepository.Get( KNsmlOpDsOperatorSyncServerURL, tmpDesc );
    iSettings.SetServerUrl( tmpDesc );
        
    iRepository.Get( KNsmlOpDsOperatorAdapterUid, tmpInt );     
    iSettings.SetOperatorAdapterUid( tmpInt ); 
    
    iRepository.Get( KNsmlOpDsProfileAdapterUid, tmpInt );     
    iSettings.SetProfileAdapterUid( tmpInt ); 
    
    iRepository.Get( KNsmlOpDsDevInfoSwVValue, tmpDesc8 );
    iSettings.SetSwV( tmpDesc8 );

    iRepository.Get( KNsmlOpDsDevInfoModValue, tmpDesc8 );
    iSettings.SetMod( tmpDesc8 );

    iRepository.Get( KNsmlOpDsSyncProfileVisibility, tmpInt );
    iSettings.SetSyncProfileVisibility( tmpInt );
    
    iRepository.Get( KNsmlOpDsOperatorSyncServerId, tmpDesc8 );
    iSettings.SetOperatorServerId( tmpDesc8 );   
	}
	
/**
 *	Save the contents of the setting items.  Note, this is called
 *	whenever an item is changed and stored to the model, so it
 *	may be called multiple times or not at all.
 */
void CSyncFwCustomizerSettingItemList::SaveSettingValuesL()
	{
    TBuf<100> tmpDesc;  
    TBuf8<100> tmpDesc8;  
    TInt tmpInt = 0;
    
    tmpDesc = iSettings.ServerUrl();
    iRepository.Set( KNsmlOpDsOperatorSyncServerURL, tmpDesc );
        
    tmpInt = iSettings.OperatorAdapterUid(); 
    iRepository.Set( KNsmlOpDsOperatorAdapterUid, tmpInt );     
    
    tmpInt = iSettings.ProfileAdapterUid(); 
    iRepository.Set( KNsmlOpDsProfileAdapterUid, tmpInt );     
    
    tmpDesc8.Copy( iSettings.SwV() );
    iRepository.Set( KNsmlOpDsDevInfoSwVValue, tmpDesc8 );

    tmpDesc8.Copy( iSettings.Mod() );
    iRepository.Set( KNsmlOpDsDevInfoModValue, tmpDesc8 );

    tmpInt = iSettings.SyncProfileVisibility();
    iRepository.Set( KNsmlOpDsSyncProfileVisibility, tmpInt );
    
    tmpDesc8.Copy( iSettings.OperatorServerId() );
    iRepository.Set( KNsmlOpDsOperatorSyncServerId, tmpDesc8 );
	}

/** 
 * Handle global resource changes, such as scalable UI or skin events (override)
 */
void CSyncFwCustomizerSettingItemList::HandleResourceChange( TInt aType )
	{
	CAknSettingItemList::HandleResourceChange( aType );
	SetRect( iAvkonViewAppUi->View( TUid::Uid( ESyncFwCustomizerSettingItemListViewId ) )->ClientRect() );
	}
				
/** 
 * Handle key event (override)
 * @param aKeyEvent key event
 * @param aType event code
 * @return EKeyWasConsumed if the event was handled, else EKeyWasNotConsumed
 */
TKeyResponse CSyncFwCustomizerSettingItemList::OfferKeyEventL( 
		const TKeyEvent& aKeyEvent, 
		TEventCode aType )
	{
	if ( aKeyEvent.iCode == EKeyLeftArrow 
		|| aKeyEvent.iCode == EKeyRightArrow )
		{
		// allow the tab control to get the arrow keys
		return EKeyWasNotConsumed;
		}
	
	return CAknSettingItemList::OfferKeyEventL( aKeyEvent, aType );
	}
				
