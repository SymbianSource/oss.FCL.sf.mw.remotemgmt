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

#ifndef SYNCFWCUSTOMIZERSETTINGITEMLIST_H
#define SYNCFWCUSTOMIZERSETTINGITEMLIST_H

#include <aknsettingitemlist.h>

class MEikCommandObserver;
class TSyncFwCustomizerSettingItemListSettings;
class CRepository;

/**
 * @class	CSyncFwCustomizerSettingItemList SyncFwCustomizerSettingItemList.h
 */
class CSyncFwCustomizerSettingItemList : public CAknSettingItemList
	{
public: // constructors and destructor

	CSyncFwCustomizerSettingItemList( 
			TSyncFwCustomizerSettingItemListSettings& settings, 
			MEikCommandObserver* aCommandObserver,
	        CRepository& aRepository );
	virtual ~CSyncFwCustomizerSettingItemList();

public:

	// from CCoeControl
	void HandleResourceChange( TInt aType );

	// overrides of CAknSettingItemList
	CAknSettingItem* CreateSettingItemL( TInt id );
	void EditItemL( TInt aIndex, TBool aCalledFromMenu );
	TKeyResponse OfferKeyEventL( 
			const TKeyEvent& aKeyEvent, 
			TEventCode aType );

public:
	// utility function for menu
	void ChangeSelectedItemL();
	void ResetSelectedItemL();
	void LoadSettingValuesL();
	void SaveSettingValuesL();
		
private:
	// override of CAknSettingItemList
	void SizeChanged();

private:
	// current settings values
	TSyncFwCustomizerSettingItemListSettings& iSettings;
	MEikCommandObserver* iCommandObserver;
	CRepository& iRepository;	
	};

#endif // SYNCFWCUSTOMIZERSETTINGITEMLIST_H
