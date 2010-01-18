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
* Description: Implementation of terminalsecurity components
*
*/



#ifndef SCPQUERYDIALOG_H
#define SCPQUERYDIALOG_H

#include <AknQueryDialog.h>
#include <aknnotedialog.h>
#include <AknEcs.h>
#include "SCPClient.h"
#include "SCPLockObserver.h"
// LOCAL CONSTANTS
const TInt KSCPModeNormal = 0;
const TInt KSCPModeRestricted = 1;

/**
*  Definition for the dialog class
*/
class CSCPQueryDialog : public CAknTextQueryDialog, public MAknEcsObserver
	{
	public: // Construction and destruction
	
		/**
        * C++ Constructor.
        * @param aDataText TDes& (code which is entered in query)
		* @param aMinLength TInt (code min length)
		* @param aMaxLength TInt (code max length)
		* @param aMode TInt (mode ESecUiCodeEtelReqest\ESecUiNone)
		*/
		CSCPQueryDialog( TDes& aDataText,
                         RSCPClient::TSCPButtonConfig aButtonsShown, 
                         TInt aMinLength,
                         TInt aMaxLength,
                         TBool aECSSupport
                         );
		/**
        * Destructor.
        */
		~CSCPQueryDialog();
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
		void TryCancelQueryL(TInt aReason);
	protected://from CAknTextQueryDialog
		/**
		* From CAknTextQueryDialog This function is called by the UIKON dialog framework 
		* just before the dialog is activated, after it has called
		* PreLayoutDynInitL() and the dialog has been sized.
		*/
		void PreLayoutDynInitL();
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
		
		void HandleEcsEvent(CAknEcsDetector* aDetector, CAknEcsDetector::TState aUpdatedState);
		void ShowWarningNoteL();

	private:
		
	    /*
	    * SetIncallBubbleAllowedInUsualL
	    */
	  void SetIncallBubbleAllowedInUsualL(TBool aAllowed);
	  
	
	private: // DATA	
		TInt	                        iMinLength;
		TInt	                        iMaxLength;
		TInt	                        iMode;
		TBool	                        iFront;
		TInt	                        iAppKey;
		TInt            	            iVoiceKey1;
		TInt	                        iVoiceKey2;
		TInt                            iValidTextLen;
		RSCPClient::TSCPButtonConfig    iButtons;
        TBool                           iECSSupport;
        CAknEcsDetector*                iEcsDetector; // owned
        TBool                           iEMCallActivated;
        TBool                           iShowingEMNumber;
        TBool                           iPreviousCharacterWasInvalid;
        TBuf<KSCPPasscodeMaxLength+1>   iTextBuffer;
        TBuf<KSCPPasscodeMaxLength+1>   iTmpBuffer;
        TBool                           iPrioritySet;
        TBool                           iPriorityDropped;
        TBool							isCallSoftkeyAdded;
        /** An integer variable to define the input mode of the lock code query */
        TInt def_mode;
        TInt iKeyUsed;
        CSCPLockObserver* iDeviceLockStatusObserver;
		CSCPLockObserver* iCallStatusObserver;
	};
#endif

// End of file
