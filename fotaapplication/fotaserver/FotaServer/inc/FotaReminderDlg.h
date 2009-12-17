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
* Description:   Header file for CFotaReminderDlg
*
*/



#ifndef CFOTAREMINDERDLG_H
#define CFOTAREMINDERDLG_H

#include <AknCapServerClient.h>
#include <aknradiobuttonsettingpage.h>


class CFotaReminderDlg: public CAknRadioButtonSettingPage
	{
	public: //Constructors & destructors
	
	/**
    * Two-phased constructor.
    */
	static CFotaReminderDlg* NewLC(TInt& aSelectedItem, CDesCArray* aItemArray, TInt& aEvent);
	static CFotaReminderDlg* NewL(TInt& aSelectedItem, CDesCArray* aItemArray, TInt& aEvent);
	
	/**
    * Destructor
    */	
	~CFotaReminderDlg();
	
	/**
    * Two-phased constructor.
    */
	virtual	void ConstructL();
	
	public:	//other methods

	/**
    * Used to activate the radiobuttonsettingpage.
    * @param None
    * @return None
    */
	void ActivateL();

	/**
    * Process events from softkeys
    * @param aCommandId is the command id
    * @return None
    */
	virtual void ProcessCommandL( TInt aCommandId );
	
	/**
	* This routine routes the keys to the editor.
	* @param aKeyEvent is the key event
	* @param aType is the type of key event
	* @return indicates whether or not the key event was used by this control. 
	*/
	TKeyResponse OfferKeyEventL( const TKeyEvent& aKeyEvent, TEventCode aType );
	
	private:
	
	/**
	* Constructor
	*/
	CFotaReminderDlg(TInt& aSelectedItem, CDesCArray* aItemArray, TInt& aEvent);
    
    /**
    * From CEikDialog: Determines if the dialog can be closed.
    * @param aButtonId Identifier of the button pressed.
    * @return whether to close the dialog or not.
    */
    TBool OkToExitL( TInt aButtonId );

    /**
    * Suppress the menu/app key when reminder dialog is displayed
    * @param ETrue to Suppress, else EFalse
    * @return None
    */
	
	void SuppressAppKey(TBool aValue);

	private:	//data
	
    CAknSetStyleListBox* iList;
    
	//UI Server used for suppressing the menu/app key
	RAknUiServer iAppKeyClient;
	
	//Stores the user selected item on the radio button page
	TInt& iSelectedItem;
	
	//Stores the key event generated on radio button page
	TInt& iEvent;
	
	//Holds the state of menu/app key supress
	TBool iIsAppKeySuppressed;
	};

#endif //CFOTAREMINDERDLG_H