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

#ifndef SYNCFWCUSTOMIZERSETTINGITEMLISTVIEW_H
#define SYNCFWCUSTOMIZERSETTINGITEMLISTVIEW_H

#include <aknview.h>
#include "SyncFwCustomizerSettingItemListSettings.h"

class CRepository;
class CSyncFwCustomizerSettingItemList;

/**
 * Avkon view class for SyncFwCustomizerSettingItemListView. It is register with the view server
 * by the AppUi. It owns the container control.
 * @class	CSyncFwCustomizerSettingItemListView SyncFwCustomizerSettingItemListView.h
 */						
			
class CSyncFwCustomizerSettingItemListView : public CAknView
	{
	
public:
	// constructors and destructor
	CSyncFwCustomizerSettingItemListView();
	static CSyncFwCustomizerSettingItemListView* NewL();
	static CSyncFwCustomizerSettingItemListView* NewLC();        
	void ConstructL();
	virtual ~CSyncFwCustomizerSettingItemListView();
						
	// from base class CAknView
	TUid Id() const;
	void HandleCommandL( TInt aCommand );
	
protected:
	// from base class CAknView
	void DoActivateL(
		const TVwsViewId& aPrevViewId,
		TUid aCustomMessageId,
		const TDesC8& aCustomMessage );
	void DoDeactivate();
	void HandleStatusPaneSizeChange();
	TBool HandleChangeSelectedSettingItemL();
	TBool ResetSelectedSettingItemL();
	TBool StartSyncL();

private:
	void SetupStatusPaneL();
	void CleanupStatusPane();

private:	
	CSyncFwCustomizerSettingItemList* iSyncFwCustomizerSettingItemList;
	TSyncFwCustomizerSettingItemListSettings* iSettings;
	CRepository* iRepository;
	};

#endif // SYNCFWCUSTOMIZERSETTINGITEMLISTVIEW_H
