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

/**
 *	Generated helper class which manages the settings contained 
 *	in 'SyncStatusSpySettingItemList'.  Each CAknSettingItem maintains
 *	a reference to data in this class so that changes in the setting
 *	item list can be synchronized with this storage.
 */
 

#include <e32base.h>
#include <stringloader.h>
#include <barsread.h>
#include <SyncStatusSpy.rsg>
#include "SyncStatusSpySettingItemListSettings.h"


#include "ErrorCodeNotifier.h"
#include "SyncTypeNotifier.h"
#include "ProfileIdNotifier.h"
#include "SyncInitiationNotifier.h"

/**
 * C/C++ constructor for settings data, cannot throw
 */
CSyncStatusSpySettingItemListSettings::CSyncStatusSpySettingItemListSettings()
	{
	}

/**
 * Two-phase constructor for settings data
 */
CSyncStatusSpySettingItemListSettings* CSyncStatusSpySettingItemListSettings::NewL()
	{
	CSyncStatusSpySettingItemListSettings* data = new( ELeave ) CSyncStatusSpySettingItemListSettings;
	CleanupStack::PushL( data );
	data->ConstructL();
	CleanupStack::Pop( data );
	return data;
	}

CSyncStatusSpySettingItemListSettings::~CSyncStatusSpySettingItemListSettings()
	{
	delete iErrorCodeNotifier;
	delete iProfileIdNotifier;
	delete iSyncTypeNotifier;
	delete iSyncInitiationNotifier;
	}

/**
 *	Second phase for initializing settings data
 */
void CSyncStatusSpySettingItemListSettings::ConstructL()
	{

	SetIntegerEditor1( 0 );
	SetIntegerEditor2( 0 );
	SetEnumeratedTextPopup2( 0 );
	SetEnumeratedTextPopup1( 0 );


	TRAP_IGNORE( iErrorCodeNotifier = CErrorCodeNotifier::NewL() );
	TRAP_IGNORE( iProfileIdNotifier = CProfileIdNotifier::NewL() );
	TRAP_IGNORE( iSyncTypeNotifier = CSyncTypeNotifier::NewL() );
	TRAP_IGNORE( iSyncInitiationNotifier = CSyncInitiationNotifier::NewL() );
	
	}
	

TInt& CSyncStatusSpySettingItemListSettings::IntegerEditor1()
	{
	return iIntegerEditor1;
	}

void CSyncStatusSpySettingItemListSettings::SetIntegerEditor1(const TInt& aValue)
	{
	iIntegerEditor1 = aValue;
	}

TInt& CSyncStatusSpySettingItemListSettings::IntegerEditor2()
	{
	return iIntegerEditor2;
	}

void CSyncStatusSpySettingItemListSettings::SetIntegerEditor2(const TInt& aValue)
	{
	iIntegerEditor2 = aValue;
	}

TInt& CSyncStatusSpySettingItemListSettings::EnumeratedTextPopup2()
	{
	return iEnumeratedTextPopup2;
	}

void CSyncStatusSpySettingItemListSettings::SetEnumeratedTextPopup2(const TInt& aValue)
	{
	iEnumeratedTextPopup2 = aValue;
	}

TInt& CSyncStatusSpySettingItemListSettings::EnumeratedTextPopup1()
	{
	return iEnumeratedTextPopup1;
	}

void CSyncStatusSpySettingItemListSettings::SetEnumeratedTextPopup1(const TInt& aValue)
	{
	iEnumeratedTextPopup1 = aValue;
	}

