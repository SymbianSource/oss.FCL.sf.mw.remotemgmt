/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Dumper application
*
*/


//  INCLUDE FILES
#include "CWPCxView.h"
#include <aknviewappui.h>
#include <aknnotewrappers.h>
#include <aknquerydialog.h>
#include <StringLoader.h>
#include <ProvisioningCx.rsg>
#include <featmgr.h>
#include <bldvariant.hrh>
#include <eikmenup.h>
#include <eikmenub.h>
#include "CWPEngine.h"
#include "CWPCxContainer.h"
#include "CWPCxDeleter.h"
#include "ProvisioningCx.hrh"
#include <csxhelp/prov.hlp.hrh>

// CONSTANTS

// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------
// CWPCxView::ConstructL
// Symbian second phase constructor
// ---------------------------------------------------------
//
void CWPCxView::ConstructL()
    {
    BaseConstructL( R_PROVISIONINGCX_VIEW1 );
    FeatureManager::InitializeLibL();
    iEngine = CWPEngine::NewL();
    iEngine->RegisterContextObserverL( this );
    }

// ---------------------------------------------------------
// CWPCxView::~CWPCxView
// ---------------------------------------------------------
//
CWPCxView::~CWPCxView()
    {
    if ( iContainer )
        {
        AppUi()->RemoveFromStack( iContainer );
        }

    delete iContainer;
    delete iEngine;
    FeatureManager::UnInitializeLib();
    }

// ---------------------------------------------------------
// TUid CWPCxView::Id
// ---------------------------------------------------------
//
TUid CWPCxView::Id() const
    {
    return KProvisioningCxViewId;
    }

// ---------------------------------------------------------
// CWPCxView::HandleCommandL
// ---------------------------------------------------------
//
void CWPCxView::HandleCommandL(TInt aCommand)
    {   
    switch ( aCommand )
        {
        case EProvisioningCxCmdDeleteContext:
            {
            DeleteContextL();
            break;
            }
        case EAknSoftkeyBack:
            {
            AppUi()->HandleCommandL(EEikCmdExit);
            break;
            }
        case EProvisioningCxCmdSelect:
            {
            LaunchPopupL();
            break;
            }
        default:
            {
            AppUi()->HandleCommandL(aCommand);
            break;
            }
        }
    }

// ---------------------------------------------------------
// CWPCxView::HandleClientRectChange
// ---------------------------------------------------------
//
void CWPCxView::HandleClientRectChange()
    {
    if ( iContainer )
        {
        iContainer->SetRect( ClientRect() );
        }
    }

// ---------------------------------------------------------
// CWPCxView::DoActivateL
// ---------------------------------------------------------
//
void CWPCxView::DoActivateL( const TVwsViewId& /*aPrevViewId*/,
                            TUid /*aCustomMessageId*/,
                            const TDesC8& /*aCustomMessage*/)
    {
    iContainer = new (ELeave) CWPCxContainer( *iEngine, *this );
    iContainer->ConstructL( ClientRect() );
    iContainer->SetMopParent( this );
    AppUi()->AddToStackL( *this, iContainer );
    #ifdef __SERIES60_32__
 		MenuBar()->SetContextMenuTitleResourceId( R_PROVISIONINGCX_CONTEXT_MENUBAR );
		
		CEikButtonGroupContainer*	bgc(NULL);
		CCoeControl* 				MSK(NULL);
		CEikCba* 					cba(NULL);
		MopGetObject				(bgc);
		TInt 						count(iContainer->ContextCount());
		
		
		if (bgc)
			{
			cba = ( static_cast<CEikCba*>( bgc->ButtonGroup() ) ); // downcast from MEikButtonGroup
			if (cba)
			   {
			   MSK = cba->Control(3); // MSK's position is 3
			   }

			MSK->MakeVisible((count > 0));
	   		}

 	#endif
    }

// ---------------------------------------------------------
// CWPCxView::DoDeactivate
// ---------------------------------------------------------
//
void CWPCxView::DoDeactivate()
    {
    if ( iContainer )
        {
        AppUi()->RemoveFromStack( iContainer );
        }
    
    delete iContainer;
    iContainer = NULL;
    }

// ---------------------------------------------------------
// CWPCxView::ContextChangeL
// ---------------------------------------------------------
//
void CWPCxView::ContextChangeL( RDbNotifier::TEvent /*aEvent*/ )
    {
    iContainer->UpdateContextsL();
    
    #ifdef __SERIES60_32__
		CEikButtonGroupContainer*	bgc(NULL);
		CCoeControl* 				MSK(NULL);
		CEikCba* 					cba(NULL);
		MopGetObject				(bgc);
		TInt 						count(iContainer->ContextCount());
		
		
		if (bgc)
		   {
           cba = ( static_cast<CEikCba*>( bgc->ButtonGroup() ) ); // downcast from MEikButtonGroup
           if (cba)
               {
               MSK = cba->Control(3); // MSK's position is 3
               }
           
           MSK->MakeVisible((count > 0));
		   }

 	#endif
    }

// ------------------------------------------------------------------------------
// CWPCxView::::DynInitMenuPaneL
// ------------------------------------------------------------------------------
//
void CWPCxView::DynInitMenuPaneL(
    TInt aResourceId,CEikMenuPane* aMenuPane)
    {
    switch( aResourceId )
        {
        case R_PROVISIONINGCX_APP_MENU:
            {
            if( !FeatureManager::FeatureSupported(KFeatureIdHelp) )
                {
                aMenuPane->SetItemDimmed( EAknCmdHelp, ETrue );
                }
            
            if( iContainer->ContextCount() == 0 )
                {
                aMenuPane->SetItemDimmed( EProvisioningCxCmdDeleteContext, ETrue );
                }
            
            break;
            }
            
        case R_PROVISIONINGCX_CONTEXT_MENU:
            {
            if( iContainer->ContextCount() == 0 )
                {
                aMenuPane->SetItemDimmed( EProvisioningCxCmdDeleteContext, ETrue );
                }
            break;
            }
        }
    }


// ------------------------------------------------------------------------------
// CWPCxView::::DeleteContextL
// ------------------------------------------------------------------------------
//
void CWPCxView::DeleteContextL()
    {
	if(iContainer->ContextCount() == 0 )
		{
		return;
		}
    TPtrC name( iContainer->CurrentContextName() );
    HBufC* query = StringLoader::LoadLC( R_QTN_OP_DELETE_CONTEXT, name );

    CAknQueryDialog* dlg = CAknQueryDialog::NewL();
    if ( dlg->ExecuteLD( R_PROVISIONCX_QUERY_DELETE, *query ) )
        {                
        TUint32 cx( iContainer->CurrentContext() );
        CWPCxDeleter* deleter = new(ELeave) CWPCxDeleter( *iEngine, cx );
        TInt numDeleted( 0 );
        TInt err( deleter->ExecuteLD( numDeleted ) );
        if( err == KErrNone )
            {
            iEngine->DeleteContextL( cx );
            } 
        else if( err != KErrCancel )
            {
            // Some problem in saving or user cancelled.
            HBufC* text = StringLoader::LoadLC( R_TEXT_QTN_OP_DEL_CONT_NO );
            CAknErrorNote* note = new(ELeave)CAknErrorNote( ETrue );
            note->ExecuteLD(*text);
            CleanupStack::PopAndDestroy(); // text
            }
        }
    CleanupStack::PopAndDestroy(); // query
    }


// ----------------------------------------------------
// CWPCxView::LaunchPopupL
// ----------------------------------------------------
//
void CWPCxView::LaunchPopupL()
    {
    CEikMenuBar* menu = MenuBar();

    menu->SetMenuTitleResourceId( R_PROVISIONINGCX_CONTEXT_MENUBAR );
    TRAPD( err, menu->TryDisplayMenuBarL() );
    menu->SetMenuTitleResourceId( R_PROVISIONINGCX_MENUBAR_VIEW1 );

    User::LeaveIfError( err );

    }

// End of File
