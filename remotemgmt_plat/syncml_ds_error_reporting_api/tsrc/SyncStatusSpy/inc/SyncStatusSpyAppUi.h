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

#ifndef SYNCSTATUSSPYAPPUI_H
#define SYNCSTATUSSPYAPPUI_H

#include <aknappui.h>
#include <eikmenub.h>
#include "SyncStatusSpySettingItemListSettings.h"
#include <DocumentHandler.h>


class CSyncStatusSpySettingItemList;

/**
 * @class	CSyncStatusSpyAppUi SyncStatusSpyAppUi.h
 * @brief The AppUi class handles application-wide aspects of the user interface, including
 *        container management and the default menu, control pane, and status pane.
 */

class CSyncStatusSpyAppUi : public CAknAppUi
	{
public: 
	// constructor and destructor
	CSyncStatusSpyAppUi();
	void ConstructL();
	virtual ~CSyncStatusSpyAppUi();

public: 
	// from CEikAppUi
	void HandleCommandL( TInt aCommand );
	void HandleResourceChangeL( TInt aType );	
	void HandleForegroundEventL(TBool aForeground);
	
private:
	void InitializeContainersL();
	

private: 
	CSyncStatusSpySettingItemList* iSyncStatusSpySettingItemList;
	CSyncStatusSpySettingItemListSettings* iSettings;
	CDocumentHandler* iDocHandler;
		
	};

#endif // SYNCSTATUSSPYAPPUI_H			
