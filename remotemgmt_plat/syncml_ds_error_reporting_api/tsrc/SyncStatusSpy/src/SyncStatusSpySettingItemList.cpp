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
#include <eikappui.h>
#include <barsread.h>
#include <stringloader.h>
#include <aknnumedwin.h>
#include <eikenv.h>
#include <aknpopupfieldtext.h>
#include <SyncStatusSpy.rsg>
#include <centralrepository.h>
#include <NsmlOperatorErrorCRKeys.h>

#include "SyncStatusSpySettingItemList.h"
#include "SyncStatusSpySettingItemListSettings.h"
#include "SyncStatusSpySettingItemList.hrh"
#include "SyncStatusSpyAppUi.h"

/**
 * Construct the CSyncStatusSpySettingItemList instance
 * @param aCommandObserver command observer
 */ 
CSyncStatusSpySettingItemList::CSyncStatusSpySettingItemList( 
		CSyncStatusSpySettingItemListSettings& aSettings, 
		MEikCommandObserver* aCommandObserver )
	: iSettings( aSettings ), iCommandObserver( aCommandObserver )
	{

	}
/** 
 * Destroy any instance variables
 */
CSyncStatusSpySettingItemList::~CSyncStatusSpySettingItemList()
	{

	}

/**
 * Handle system notification that the container's size has changed.
 */
void CSyncStatusSpySettingItemList::SizeChanged()
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
CAknSettingItem* CSyncStatusSpySettingItemList::CreateSettingItemL( TInt aId )
	{
	switch ( aId )
		{
		case ESyncStatusSpyAppUiIntegerEditor1:
			{			
			CAknIntegerEdwinSettingItem* item = new ( ELeave ) 
				CAknIntegerEdwinSettingItem( 
					aId,
					iSettings.IntegerEditor1() );
			return item;
			}
		case ESyncStatusSpyAppUiIntegerEditor2:
			{			
			CAknIntegerEdwinSettingItem* item = new ( ELeave ) 
				CAknIntegerEdwinSettingItem( 
					aId,
					iSettings.IntegerEditor2() );
			return item;
			}
		case ESyncStatusSpyAppUiEnumeratedTextPopup2:
			{			
			CAknEnumeratedTextPopupSettingItem* item = new ( ELeave ) 
				CAknEnumeratedTextPopupSettingItem( 
					aId,
					iSettings.EnumeratedTextPopup2() );
			return item;
			}
		case ESyncStatusSpyAppUiEnumeratedTextPopup1:
			{			
			CAknEnumeratedTextPopupSettingItem* item = new ( ELeave ) 
				CAknEnumeratedTextPopupSettingItem( 
					aId,
					iSettings.EnumeratedTextPopup1() );
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
void CSyncStatusSpySettingItemList::EditItemL ( TInt aIndex, TBool aCalledFromMenu )
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
void CSyncStatusSpySettingItemList::ChangeSelectedItemL()
	{
	if ( ListBox()->CurrentItemIndex() >= 0 )
		{
		EditItemL( ListBox()->CurrentItemIndex(), ETrue );
		}
	}

/**
 *	Load the initial contents of the setting items.  By default,
 *	the setting items are populated with the default values from
 * 	the design.  You can override those values here.
 *	<p>
 *	Note: this call alone does not update the UI.  
 *	LoadSettingsL() must be called afterwards.
 */
void CSyncStatusSpySettingItemList::LoadSettingValuesL()
	{
	// load values into iSettings
	TInt value;
	CRepository* rep = NULL;
	TRAPD ( err, rep = CRepository::NewL( KCRUidOperatorDatasyncErrorKeys ) );
	if ( err == KErrNone )
	    {	
	    rep->Get( KNsmlOpDsErrorCode, value );
	    iSettings.SetIntegerEditor1( value );
	
	    rep->Get( KNsmlOpDsSyncProfUid, value );
	    iSettings.SetIntegerEditor2( value );
	
	    rep->Get( KNsmlOpDsSyncType, value );
	    iSettings.SetEnumeratedTextPopup2( value );
	
	    rep->Get( KNsmlOpDsSyncInitiation, value );
	    iSettings.SetEnumeratedTextPopup1( value );
	    
	    delete rep;
	    }
	}
	
/**
 *	Save the contents of the setting items.  Note, this is called
 *	whenever an item is changed and stored to the model, so it
 *	may be called multiple times or not at all.
 */
void CSyncStatusSpySettingItemList::SaveSettingValuesL()
	{
	// store values from iSettings
	CRepository* rep = NULL;
	TRAPD ( err, rep = CRepository::NewL( KCRUidOperatorDatasyncErrorKeys ) );
	if ( err == KErrNone )
	    {		
	    rep->Set( KNsmlOpDsErrorCode, iSettings.IntegerEditor1() );
	    rep->Set( KNsmlOpDsSyncProfUid, iSettings.IntegerEditor2() );
	    rep->Set( KNsmlOpDsSyncType, iSettings.EnumeratedTextPopup2() );
	    rep->Set( KNsmlOpDsSyncInitiation, iSettings.EnumeratedTextPopup1() );
	    
	    delete rep;
	    }
	}


/** 
 * Handle global resource changes, such as scalable UI or skin events (override)
 */
void CSyncStatusSpySettingItemList::HandleResourceChange( TInt aType )
	{
	CAknSettingItemList::HandleResourceChange( aType );
	SetRect( static_cast< CEikAppUi* >( iCoeEnv->AppUi() )->ClientRect() );
	}
				
/** 
 * Handle key event (override)
 * @param aKeyEvent key event
 * @param aType event code
 * @return EKeyWasConsumed if the event was handled, else EKeyWasNotConsumed
 */
TKeyResponse CSyncStatusSpySettingItemList::OfferKeyEventL( 
		const TKeyEvent& aKeyEvent, 
		TEventCode aType )
	{
	return CAknSettingItemList::OfferKeyEventL( aKeyEvent, aType );
	}
				
