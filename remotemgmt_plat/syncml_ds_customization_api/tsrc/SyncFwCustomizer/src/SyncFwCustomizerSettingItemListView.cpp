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

#include <aknviewappui.h>
#include <eikmenub.h>
#include <avkon.hrh>
#include <akncontext.h>
#include <akntitle.h>
#include <stringloader.h>
#include <barsread.h>
#include <eikbtgpc.h>
#include <SyncFwCustomizer.rsg>
#include <centralrepository.h>
#include <NSmlOperatorDataCRKeys.h>
#include <syncmlclient.h>
#include <syncmlclientds.h>
#include <syncmldef.h>
#include <AknQueryDialog.h>

#include "SyncFwCustomizer.hrh"
#include "SyncFwCustomizerSettingItemListView.h"
#include "SyncFwCustomizerSettingItemList.hrh"
#include "SyncFwCustomizerSettingItemList.h"

const TInt KOperatorProfileId = 4;

/**
 * First phase of Symbian two-phase construction. Should not contain any
 * code that could leave.
 */
CSyncFwCustomizerSettingItemListView::CSyncFwCustomizerSettingItemListView()
	{
	}

/** 
 * The view's destructor removes the container from the control
 * stack and destroys it.
 */
CSyncFwCustomizerSettingItemListView::~CSyncFwCustomizerSettingItemListView()
	{
	delete iRepository;
	}

/**
 * Symbian two-phase constructor.
 * This creates an instance then calls the second-phase constructor
 * without leaving the instance on the cleanup stack.
 * @return new instance of CSyncFwCustomizerSettingItemListView
 */
CSyncFwCustomizerSettingItemListView* CSyncFwCustomizerSettingItemListView::NewL()
	{
	CSyncFwCustomizerSettingItemListView* self = CSyncFwCustomizerSettingItemListView::NewLC();
	CleanupStack::Pop( self );
	return self;
	}

/**
 * Symbian two-phase constructor.
 * This creates an instance, pushes it on the cleanup stack,
 * then calls the second-phase constructor.
 * @return new instance of CSyncFwCustomizerSettingItemListView
 */
CSyncFwCustomizerSettingItemListView* CSyncFwCustomizerSettingItemListView::NewLC()
	{
	CSyncFwCustomizerSettingItemListView* self = new ( ELeave ) CSyncFwCustomizerSettingItemListView();
	CleanupStack::PushL( self );
	self->ConstructL();
	return self;
	}

/**
 * Second-phase constructor for view.  
 * Initialize contents from resource.
 */ 
void CSyncFwCustomizerSettingItemListView::ConstructL()
	{
	BaseConstructL( R_SYNC_FW_CUSTOMIZER_SETTING_ITEM_LIST_SYNC_FW_CUSTOMIZER_SETTING_ITEM_LIST_VIEW );
	iRepository = CRepository::NewL( KCRUidOperatorDatasyncInternalKeys );	
	}

/**
 * @return The UID for this view
 */
TUid CSyncFwCustomizerSettingItemListView::Id() const
	{
	return TUid::Uid( ESyncFwCustomizerSettingItemListViewId );
	}

/**
 * Handle a command for this view (override)
 * @param aCommand command id to be handled
 */
void CSyncFwCustomizerSettingItemListView::HandleCommandL( TInt aCommand )
	{
	TBool commandHandled = EFalse;
	switch ( aCommand )
		{	// code to dispatch to the AknView's menu and CBA commands is generated here
		case ESyncFwCustomizerSettingEdit:
			commandHandled = HandleChangeSelectedSettingItemL();
			break;
        case ESyncFwCustomizerSettingReset:
            commandHandled = ResetSelectedSettingItemL();
            break;
		case ESyncFwCustomizerSettingSync:
			commandHandled = StartSyncL();
			break;
		
		default:
			break;
		}
		
	if ( !commandHandled ) 
		{
		if ( aCommand == EAknSoftkeyExit )
			{
			AppUi()->HandleCommandL( EEikCmdExit );
			}
		}	
	}

/**
 *	Handles user actions during activation of the view, 
 *	such as initializing the content.
 */
void CSyncFwCustomizerSettingItemListView::DoActivateL( 
		const TVwsViewId& /*aPrevViewId*/,
		TUid /*aCustomMessageId*/,
		const TDesC8& /*aCustomMessage*/ )
	{
	SetupStatusPaneL();	
	
	if ( iSyncFwCustomizerSettingItemList == NULL )
		{
		iSettings = TSyncFwCustomizerSettingItemListSettings::NewL(*iRepository);
		iSyncFwCustomizerSettingItemList = new ( ELeave ) CSyncFwCustomizerSettingItemList( *iSettings, this, *iRepository );
		iSyncFwCustomizerSettingItemList->SetMopParent( this );
		iSyncFwCustomizerSettingItemList->ConstructFromResourceL( R_SYNC_FW_CUSTOMIZER_SETTING_ITEM_LIST_SYNC_FW_CUSTOMIZER_SETTING_ITEM_LIST );
		iSyncFwCustomizerSettingItemList->ActivateL();
		iSyncFwCustomizerSettingItemList->LoadSettingValuesL();
		iSyncFwCustomizerSettingItemList->LoadSettingsL();
		AppUi()->AddToStackL( *this, iSyncFwCustomizerSettingItemList );
		} 
	}

/**
 */
void CSyncFwCustomizerSettingItemListView::DoDeactivate()
	{
	CleanupStatusPane();
	
	if ( iSyncFwCustomizerSettingItemList != NULL )
		{
		AppUi()->RemoveFromStack( iSyncFwCustomizerSettingItemList );
		delete iSyncFwCustomizerSettingItemList;
		iSyncFwCustomizerSettingItemList = NULL;
		delete iSettings;
		iSettings = NULL;
		}
	}

/** 
 * Handle status pane size change for this view (override)
 */
void CSyncFwCustomizerSettingItemListView::HandleStatusPaneSizeChange()
	{
	CAknView::HandleStatusPaneSizeChange();
	
	// this may fail, but we're not able to propagate exceptions here
	TVwsViewId view;
	AppUi()->GetActiveViewId( view );
	if ( view.iViewUid == Id() )
		{
		TInt result;
		TRAP( result, SetupStatusPaneL() );
		}
	}

void CSyncFwCustomizerSettingItemListView::SetupStatusPaneL()
	{
	// reset the context pane
	TUid contextPaneUid = TUid::Uid( EEikStatusPaneUidContext );
	CEikStatusPaneBase::TPaneCapabilities subPaneContext = 
		StatusPane()->PaneCapabilities( contextPaneUid );
	if ( subPaneContext.IsPresent() && subPaneContext.IsAppOwned() )
		{
		CAknContextPane* context = static_cast< CAknContextPane* > ( 
			StatusPane()->ControlL( contextPaneUid ) );
		context->SetPictureToDefaultL();
		}
	
	// setup the title pane
	TUid titlePaneUid = TUid::Uid( EEikStatusPaneUidTitle );
	CEikStatusPaneBase::TPaneCapabilities subPaneTitle = 
		StatusPane()->PaneCapabilities( titlePaneUid );
	if ( subPaneTitle.IsPresent() && subPaneTitle.IsAppOwned() )
		{
		CAknTitlePane* title = static_cast< CAknTitlePane* >( 
			StatusPane()->ControlL( titlePaneUid ) );
		TResourceReader reader;
		iEikonEnv->CreateResourceReaderLC( reader, R_SYNC_FW_CUSTOMIZER_SETTING_ITEM_LIST_TITLE_RESOURCE );
		title->SetFromResourceL( reader );
		CleanupStack::PopAndDestroy(); // reader internal state
		}		
	}

void CSyncFwCustomizerSettingItemListView::CleanupStatusPane()
	{
	}

/** 
 * Handle the selected event.
 * @param aCommand the command id invoked
 * @return ETrue if the command was handled, EFalse if not
 */
TBool CSyncFwCustomizerSettingItemListView::HandleChangeSelectedSettingItemL()
	{
	iSyncFwCustomizerSettingItemList->ChangeSelectedItemL();
	return ETrue;
	}

TBool CSyncFwCustomizerSettingItemListView::ResetSelectedSettingItemL( )
    {
    iSyncFwCustomizerSettingItemList->ResetSelectedItemL();
    return ETrue;    
    }

TBool CSyncFwCustomizerSettingItemListView::StartSyncL( )
	{
	// Open syncML session
	RSyncMLSession syncMLSession;
	RSyncMLDataSyncJob job;
	CleanupClosePushL(syncMLSession);
	CleanupClosePushL(job);
	syncMLSession.OpenL();
	
	// Need instance of data sync class
	// For creating a data sync job, need to specify id of the profile used for synchronization
	// If profile selected is PCSuite, it searches for the bluetooth device to sync with
	TInt profileId = KOperatorProfileId;
	CAknNumberQueryDialog* d = CAknNumberQueryDialog::NewL( profileId );
	d->ExecuteLD( R_SYNC_FW_CUSTOMIZER_NUMBER_QUERY );
	job.CreateL( syncMLSession, profileId );
	// close the job
	job.Close();
	// close the syncML session
	CleanupStack::PopAndDestroy(&job);
	CleanupStack::PopAndDestroy(&syncMLSession);
    return ETrue;    
	}
