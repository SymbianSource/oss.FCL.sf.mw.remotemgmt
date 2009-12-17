/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:   methods for CFotaReminderDlg
*
*/



#include <fotaserver.rsg>
#include "FotaReminderDlg.h"

#include "FotaDB.h"
#include "FotaSrvDebug.h"

// ---------------------------------------------------------------------------
// CFotaReminderDlg::NewLC
// Two-phase construction
// ---------------------------------------------------------------------------
//
CFotaReminderDlg* CFotaReminderDlg::NewLC(TInt& aSelectedItem, CDesCArray* aItemArray, TInt& aEvent)
	{
	CFotaReminderDlg* self = new (ELeave) CFotaReminderDlg(aSelectedItem, aItemArray, aEvent);
	CleanupStack::PushL(self);
	self->ConstructL();
	
	return self;
	}

// ---------------------------------------------------------------------------
// CFotaReminderDlg::NewL
// Two-phase construction
// ---------------------------------------------------------------------------
//
CFotaReminderDlg* CFotaReminderDlg::NewL(TInt& aSelectedItem, CDesCArray* aItemArray, TInt& aEvent)
	{
	CFotaReminderDlg* self = CFotaReminderDlg::NewLC(aSelectedItem, aItemArray, aEvent);
	CleanupStack::Pop(self);
	return self;
	}

// ---------------------------------------------------------------------------
// CFotaReminderDlg::~CFotaReminderDlg
// Destructor.
// ---------------------------------------------------------------------------
//
CFotaReminderDlg::~CFotaReminderDlg()
	{
	FLOG(_L("CFotaReminderDlg::~CFotaReminderDlg   >>"));
	iAppKeyClient.Close();
	FLOG(_L("CFotaReminderDlg::~CFotaReminderDlg   <<"));
	}

// ---------------------------------------------------------------------------
// CFotaReminderDlg::CFotaReminderDlg
// Constructor.
// ---------------------------------------------------------------------------
//	
CFotaReminderDlg::CFotaReminderDlg(TInt& aSelectedItem, CDesCArray* aItemArray, TInt& aEvent):
								CAknRadioButtonSettingPage( R_SETTING_REMAINDER, aSelectedItem, aItemArray ),
    	                        iSelectedItem(aSelectedItem),
								iEvent(aEvent),
								iIsAppKeySuppressed(EFalse)
	{
		
	}

// ---------------------------------------------------------------------------
// CFotaReminderDlg::~CFotaReminderDlg
// Two-phase construction.
// ---------------------------------------------------------------------------
//	
void CFotaReminderDlg::ConstructL()
	{
	FLOG(_L("CFotaReminderDlg::ConstructL   >>"));
	
	SuppressAppKey(ETrue);

	CAknRadioButtonSettingPage::ConstructL();
	iList = ListBoxControl();

	FLOG(_L("CFotaReminderDlg::ConstructL   <<"));    
	}

// ---------------------------------------------------------------------------
// CFotaReminderDlg::ActivateL
// Used to activate the radiobuttonsettingpage.
// ---------------------------------------------------------------------------
//
void CFotaReminderDlg::ActivateL()
	{
	FLOG(_L("CFotaReminderDlg::ActivateL   >>"));

	//iList->SetCurrentItemIndexAndDraw(0);
	CAknRadioButtonSettingPage::ActivateL();

	FLOG(_L("CFotaReminderDlg::ActivateL   <<"));
	}
	
// ---------------------------------------------------------------------------
// CFotaReminderDlg::OkToExitL
// Determines if the dialog can be closed.
// ---------------------------------------------------------------------------
//
TBool CFotaReminderDlg::OkToExitL( TInt aButtonId )
	{
   	FLOG(_L("CFotaReminderDlg::OkToExitL   >>"));

    TBool IsAccepted = aButtonId;

    if ( IsAccepted )
        {
        iSelectedItem = iList->CurrentItemIndex();
        }

	SuppressAppKey(EFalse);
	FLOG(_L("CFotaReminderDlg::OkToExitL   <<"));

    return ETrue;
    }
    	

// ---------------------------------------------------------------------------
// CFotaReminderDlg::ProcessCommandL
// Process events from softkeys
// ---------------------------------------------------------------------------
//
void CFotaReminderDlg::ProcessCommandL( TInt aCommandId )
	{
   	FLOG(_L("CFotaReminderDlg::ProcessCommandL   >>"));

    if ( aCommandId == EAknSoftkeyOk )
    	{
    	AttemptExitL(ETrue);
    	}
    	
    if (aCommandId == EAknSoftkeyCancel ||
         aCommandId == EAknCmdExit)
        {
        AttemptExitL( EFalse );
        }

   	FLOG(_L("CFotaReminderDlg::ProcessCommandL   <<"));
	}


// ---------------------------------------------------------------------------
// CFotaReminderDlg::OfferKeyEventL
// This routine routes the keys to the editor.
// ---------------------------------------------------------------------------
//	
TKeyResponse CFotaReminderDlg::OfferKeyEventL(
    const TKeyEvent& aKeyEvent,
    TEventCode aType )
    {
	FLOG(_L("CFotaReminderDlg::OfferKeyEventL   >>"));
   
	/*if (iResumeOnFG && iIsDlgOpen)
    {
		TApaTaskList taskList(CEikonEnv::Static()->WsSession());
		TApaTask task=taskList.FindApp(TUid::Uid(KFotaServerUid));
		if(task.Exists())
			task.BringToForeground();

    }
    
    iResumeOnFG = EFalse;
    //Menu key is pressed when reminder dialog is shown
 	TBool aStartDmUi (EFalse);   
    if (aKeyEvent.iCode == EKeyApplication0)
    {
    	iResumeOnFG = ETrue;
		aStartDmUi = ETrue;	
    }*/
   
    iEvent = aType;
    TKeyResponse aResp (EKeyWasConsumed );
    aResp = CAknRadioButtonSettingPage::OfferKeyEventL( aKeyEvent, aType );
    
    FLOG(_L("CFotaReminderDlg::OfferKeyEventL   <<"));
    return aResp;
    }

// ---------------------------------------------------------------------------
// CFotaReminderDlg::SuppressAppKey
// Used to suppress the menu/app key
// ---------------------------------------------------------------------------
//
void CFotaReminderDlg::SuppressAppKey(TBool aValue)
	{
	FLOG(_L("CFotaReminderDlg::SuppressAppKey   >>"));

	if (iIsAppKeySuppressed!=aValue)
		{	
		
		TInt error = iAppKeyClient.ConnectAndSendAppsKeySuppress(aValue);
		FLOG(_L("Return for suppress (%d)....%d"),aValue,error);
		iIsAppKeySuppressed = aValue;
		}

	FLOG(_L("CFotaReminderDlg::SuppressAppKey   <<"));
	}
		
