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

#ifndef SYNCSTATUSSPYSETTINGITEMLIST_H
#define SYNCSTATUSSPYSETTINGITEMLIST_H


#include <aknsettingitemlist.h>


class MEikCommandObserver;
class CSyncStatusSpySettingItemListSettings;


/**
 * @class	CSyncStatusSpySettingItemList SyncStatusSpySettingItemList.h
 */
class CSyncStatusSpySettingItemList : public CAknSettingItemList
	{
public: // constructors and destructor

	CSyncStatusSpySettingItemList( 
			CSyncStatusSpySettingItemListSettings& settings, 
			MEikCommandObserver* aCommandObserver );
	virtual ~CSyncStatusSpySettingItemList();

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

	void LoadSettingValuesL();
	void SaveSettingValuesL();
		
private:
	// override of CAknSettingItemList
	void SizeChanged();

private:
	// current settings values
	CSyncStatusSpySettingItemListSettings& iSettings;
	MEikCommandObserver* iCommandObserver;

	};
#endif // SYNCSTATUSSPYSETTINGITEMLIST_H
