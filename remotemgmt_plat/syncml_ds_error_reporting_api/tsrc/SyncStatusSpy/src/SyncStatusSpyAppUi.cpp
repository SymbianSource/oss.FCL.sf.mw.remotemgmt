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
#include <SyncStatusSpy.rsg>

#include <bautils.h>				// BaflUtils
#include <APGWGNAM.H> //CApaWindowGroupName
#include <DataSyncInternalPSKeys.h>
#include <NSmlConstants.h>
#include <e32property.h>


#include "SyncStatusSpyAppUi.h"
#include "SyncStatusSpySettingItemList.h"
#include "SyncStatusSpySettingItemList.hrh"

#include "CenrepUtils.h"
#include <apmstd.h> // TDataType

/**
 * Construct the CSyncStatusSpyAppUi instance
 */ 
CSyncStatusSpyAppUi::CSyncStatusSpyAppUi()
	{
	
	}

/** 
 * The appui's destructor removes the container from the control
 * stack and destroys it.
 */
CSyncStatusSpyAppUi::~CSyncStatusSpyAppUi()
	{
	if ( iSyncStatusSpySettingItemList != NULL )
		{
		RemoveFromStack( iSyncStatusSpySettingItemList );
		delete iSyncStatusSpySettingItemList;
		iSyncStatusSpySettingItemList = NULL;
		delete iSettings;
		iSettings = NULL;
		}
	delete iDocHandler;
	}

/**
 * @brief Completes the second phase of Symbian object construction. 
 * Put initialization code that could leave here. 
 */ 
void CSyncStatusSpyAppUi::ConstructL()
	{
	
	BaseConstructL( EAknEnableSkin  | 
					 EAknEnableMSK ); 
	InitializeContainersL();

	CCenrepUtils::CreateSyncLogDirL();
	iDocHandler = CDocumentHandler::NewL();
	
	// security policies for P&S reading and writing
	_LIT_SECURITY_POLICY_PASS( KNSmlPSWritePolicy ); // no checks done when writing
	_LIT_SECURITY_POLICY_PASS( KNSmlPSReadPolicy ); // no checks done when reading
		
	// Define P&S data field for sync ON/OFF flag (may fail e.g. if exists already)
	RProperty::Define( KPSUidDataSynchronizationInternalKeys, // category
                       KDataSyncStatus, // field
                       RProperty::EInt, // type
                       KNSmlPSReadPolicy, // read policy
                       KNSmlPSWritePolicy ); // write policy
	}


void CSyncStatusSpyAppUi::InitializeContainersL()
	{
	iSettings = CSyncStatusSpySettingItemListSettings::NewL();
	iSyncStatusSpySettingItemList = new ( ELeave ) CSyncStatusSpySettingItemList( *iSettings, this );
	iSyncStatusSpySettingItemList->SetMopParent( this );
	iSyncStatusSpySettingItemList->ConstructFromResourceL( R_SYNC_STATUS_SPY_SETTING_ITEM_LIST_SYNC_STATUS_SPY_SETTING_ITEM_LIST );
	iSyncStatusSpySettingItemList->ActivateL();
	AddToStackL( iSyncStatusSpySettingItemList );
	}


/**
 * Handle a command for this appui (override)
 * @param aCommand command id to be handled
 */
void CSyncStatusSpyAppUi::HandleCommandL( TInt aCommand )
	{
	_LIT( KPrivateFile,"C:\\logs\\Sync\\SyncStatus.txt" );
	TBool commandHandled = EFalse;
	
	switch ( aCommand )
		{ // code to dispatch to the AppUi's menu and CBA commands is generated here
		case ESyncStatusSpyAppUiPublishMenuItemCommand:
	        RProperty::Set( KPSUidDataSynchronizationInternalKeys,
							KDataSyncStatus,
							EDataSyncRunning12 );	
	        
	        // Set sync stopped to P&S
	        RProperty::Set( KPSUidDataSynchronizationInternalKeys, KDataSyncStatus, EDataSyncNotRunning );
	        commandHandled = ETrue;
			break;
		
		case ESyncStatusSpyAppUiView_logMenuItemCommand:
		    {
		    TDataType  empty;
			iDocHandler->OpenFileEmbeddedL( KPrivateFile, empty );
			commandHandled = ETrue;
		    }
			break;
			
		case ESyncStatusSpyAppUiHideMenuItemCommand:
		    { 
		    TApaTask task(iEikonEnv->WsSession( ));
			 task.SetWgId(CEikonEnv::Static()->RootWin().Identifier());
			 task.SendToBackground(); 
			 commandHandled = ETrue;
		    }
			break;
			
		case ESyncStatusSpyAppUiDelete_logMenuItemCommand:
			CCenrepUtils::ClearSyncLogL();
			commandHandled = ETrue;
			break;
			
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
void CSyncStatusSpyAppUi::HandleResourceChangeL( TInt aType )
	{
	CAknAppUi::HandleResourceChangeL( aType );

	}

/** 
 * Override of the HandleForegroundEventL virtual function
 */
void CSyncStatusSpyAppUi::HandleForegroundEventL(TBool aForeground)
	{
	// Call Base class method
	CAknAppUi::HandleForegroundEventL(aForeground);

	if(aForeground)
		{
		iSyncStatusSpySettingItemList->LoadSettingValuesL();
		iSyncStatusSpySettingItemList->LoadSettingsL();
		iSyncStatusSpySettingItemList->DrawDeferred();
        }
	}
