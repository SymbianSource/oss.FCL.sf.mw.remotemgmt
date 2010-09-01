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
* Description: 
*     
*
*/


#ifndef CWPCODEQUERYDIALOG_H
#define CWPCODEQUERYDIALOG_H

// INCLUDES
#include <AknQueryDialog.h>
#include <aknnotedialog.h>

// CLASS DECLARATION

/**
 * Sortable 
 */
class CWPCodeQueryDialog : public CAknTextQueryDialog
	{
	public:
        /**
        * C++ default constructor.
        */
		CWPCodeQueryDialog(TDes& aDataText);

        /**
        * Destructor.
        */
		~CWPCodeQueryDialog();

	public:
		/**
		* Allows dismissing of code queries. Only mandatory requirement is that PIN
		* queries are dismissed by the # 
		*
		* @param aKeyEvent TKeyEvent&
		* @return ETrue query is dismissed
		*		  EFalse not dismissed
		*/
		TBool NeedToDismissQueryL(const TKeyEvent& aKeyEvent);

	protected://from CAknTextQueryDialog
		/**
		* From CAknTextQueryDialog This function is called by the UIKON dialog framework 
		* just before the dialog is activated, after it has called
		* PreLayoutDynInitL() and the dialog has been sized.
		*/
//		void PreLayoutDynInitL();
		/**
		* From CAknTextQueryDialog This function is called by the UIKON framework 
		* if the user activates a button in the button panel. 
		* It is not called if the Cancel button is activated, 
		* unless the EEikDialogFlagNotifyEsc flag is set.
		* @param aButtonId  The ID of the button that was activated
		* @return           Should return ETrue if the dialog should exit, and EFalse if it should not.
		*/
		TBool OkToExitL(TInt aButtonId);
		/**
		* From CAknTextQueryDialog This function is called by the UIKON dialog framework 
        * just after a key is pressed
		* @param aKeyEvent TKeyEvent& 
		* @param aType TEventCode 
        */
		TKeyResponse OfferKeyEventL(const TKeyEvent& aKeyEvent, TEventCode aType);


	};

#endif // CWPCodeQueryDialog_H

// End of File
