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

#ifndef SYNCFWCUSTOMIZERAPPUI_H
#define SYNCFWCUSTOMIZERAPPUI_H

#include <aknviewappui.h>

class CSyncFwCustomizerSettingItemListView;

/**
 * @class	CSyncFwCustomizerAppUi SyncFwCustomizerAppUi.h
 * @brief The AppUi class handles application-wide aspects of the user interface, including
 *        view management and the default menu, control pane, and status pane.
 */
class CSyncFwCustomizerAppUi : public CAknViewAppUi
	{
public: 
	// constructor and destructor
	CSyncFwCustomizerAppUi();
	virtual ~CSyncFwCustomizerAppUi();
	void ConstructL();

public:
	// from CCoeAppUi
	TKeyResponse HandleKeyEventL(
				const TKeyEvent& aKeyEvent,
				TEventCode aType );

	// from CEikAppUi
	void HandleCommandL( TInt aCommand );
	void HandleResourceChangeL( TInt aType );

	// from CAknAppUi
	void HandleViewDeactivation( 
			const TVwsViewId& aViewIdToBeDeactivated, 
			const TVwsViewId& aNewlyActivatedViewId );

private:
	void InitializeContainersL();

private: 
	CSyncFwCustomizerSettingItemListView* iSyncFwCustomizerSettingItemListView;
	};

#endif // SYNCFWCUSTOMIZERAPPUI_H			
