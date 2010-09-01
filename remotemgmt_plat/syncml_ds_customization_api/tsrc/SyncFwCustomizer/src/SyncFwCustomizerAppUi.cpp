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

#include <eikmenub.h>
#include <akncontext.h>
#include <akntitle.h>
#include <SyncFwCustomizer.rsg>

#include "SyncFwCustomizerAppUi.h"
#include "SyncFwCustomizerSettingItemList.hrh"
#include "SyncFwCustomizer.hrh"
#include "SyncFwCustomizerSettingItemListView.h"

/**
 * Construct the CSyncFwCustomizerAppUi instance
 */ 
CSyncFwCustomizerAppUi::CSyncFwCustomizerAppUi()
	{
	}

/** 
 * The appui's destructor removes the container from the control
 * stack and destroys it.
 */
CSyncFwCustomizerAppUi::~CSyncFwCustomizerAppUi()
	{
	}

void CSyncFwCustomizerAppUi::InitializeContainersL()
	{
	iSyncFwCustomizerSettingItemListView = CSyncFwCustomizerSettingItemListView::NewL();
	AddViewL( iSyncFwCustomizerSettingItemListView );
	SetDefaultViewL( *iSyncFwCustomizerSettingItemListView );
	}

/**
 * Handle a command for this appui (override)
 * @param aCommand command id to be handled
 */
void CSyncFwCustomizerAppUi::HandleCommandL( TInt aCommand )
	{
	TBool commandHandled = EFalse;
	switch ( aCommand )
		{ // code to dispatch to the AppUi's menu and CBA commands is generated here
		default:
			break;
		}
		
	if ( !commandHandled ) 
		{
		if ( aCommand == EAknSoftkeyExit || aCommand == EEikCmdExit )
			{
			Exit();
			}
		}
	}

/** 
 * Override of the HandleResourceChangeL virtual function
 */
void CSyncFwCustomizerAppUi::HandleResourceChangeL( TInt aType )
	{
	CAknViewAppUi::HandleResourceChangeL( aType );
	}
				
/** 
 * Override of the HandleKeyEventL virtual function
 * @return EKeyWasConsumed if event was handled, EKeyWasNotConsumed if not
 * @param aKeyEvent 
 * @param aType 
 */
TKeyResponse CSyncFwCustomizerAppUi::HandleKeyEventL(
		const TKeyEvent& aKeyEvent,
		TEventCode aType )
	{
	// The inherited HandleKeyEventL is private and cannot be called
	return EKeyWasNotConsumed;
	}

/** 
 * Override of the HandleViewDeactivation virtual function
 *
 * @param aViewIdToBeDeactivated 
 * @param aNewlyActivatedViewId 
 */
void CSyncFwCustomizerAppUi::HandleViewDeactivation( 
		const TVwsViewId& aViewIdToBeDeactivated, 
		const TVwsViewId& aNewlyActivatedViewId )
	{
	CAknViewAppUi::HandleViewDeactivation( 
			aViewIdToBeDeactivated, 
			aNewlyActivatedViewId );
	}

/**
 * @brief Completes the second phase of Symbian object construction. 
 * Put initialization code that could leave here. 
 */ 
void CSyncFwCustomizerAppUi::ConstructL()
	{
	BaseConstructL( EAknEnableSkin  | 
					 EAknEnableMSK ); 
	InitializeContainersL();
	}

