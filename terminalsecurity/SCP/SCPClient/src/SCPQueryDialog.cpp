/*
* Copyright (c) 2000 Nokia Corporation and/or its subsidiary(-ies). 
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

#include <AknQueryDialog.h>
#include "SCPQueryDialog.h"
#include <eikseced.h>
#include <aknsoundsystem.h>
#include <StringLoader.h>
#include <aknappui.h> 
#include <avkon.rsg>
#include <SecUi.rsg>
// Include the SecUi definitions
#include <secui.hrh>
#include "SCPDebug.h"
#include <scpnotifier.rsg>
// For Central Repository
#include <centralrepository.h>
#include <AknIncallBubbleNotify.h>
#include <e32property.h>
#include <ctsydomainpskeys.h>
#include "SCPCodePrivateCRKeys.h"
/*#ifdef _DEBUG
#define __SCP_DEBUG
#endif // _DEBUG

// Define this so the precompiler in CW 3.1 won't complain about token pasting,
// the warnings are not valid
#pragma warn_illtokenpasting off

#ifdef __SCP_DEBUG
#define Dprint(a) RDebug::Print##a
#else
#define Dprint(a)
#endif // _DEBUG*/

const TInt KSCPSpecialDeleteEvent( 63529 );

// ================= MEMBER FUNCTIONS =======================
//
// ----------------------------------------------------------
// CSCPQueryDialog::CSCPQueryDialog()
// C++ constructor
// ----------------------------------------------------------
//
CSCPQueryDialog::CSCPQueryDialog(   TDes& aDataText,
                                    RSCPClient::TSCPButtonConfig aButtonsShown, 
                                    TInt aMinLength,
                                    TInt aMaxLength,
                                    TBool aECSSupport
                                    )
            : CAknTextQueryDialog(aDataText, ENoTone),
			  iMinLength(aMinLength),
			  iMaxLength(aMaxLength),
			  iValidTextLen( 0 ),
			  iButtons( aButtonsShown ),
			  iECSSupport( aECSSupport ),
			  iEcsDetector( NULL ),
			  iEMCallActivated( EFalse ),
			  iShowingEMNumber( EFalse ),
			  iPreviousCharacterWasInvalid( EFalse ),
			  iPrioritySet( EFalse ),
			  iPriorityDropped( EFalse ),
			  iKeyUsed ( NULL )
	{		 
        def_mode = 0;
        iAppKey = 0;
        iMode = KSCPModeNormal;
	}
	
//
// ----------------------------------------------------------
// CSCPQueryDialog::~CSCPQueryDialog()
// Destructor
// ----------------------------------------------------------
//
CSCPQueryDialog::~CSCPQueryDialog()
	{
	Dprint( (_L("CSCPQueryDialog::~CSCPQueryDialog()")) );
	
	    if ( AknLayoutUtils::PenEnabled() )
        {
        TRAP_IGNORE ( SetIncallBubbleAllowedInUsualL( ETrue ) );
        }

	
	if (iFront)
		{
		// Uncapture keys, if they were captured
		if ( iMode == KSCPModeRestricted )		
			{
			RWindowGroup& groupWin=iCoeEnv->RootWin();
			groupWin.CancelCaptureKeyUpAndDowns(iAppKey);
			groupWin.CancelCaptureKeyUpAndDowns(iVoiceKey2);
			groupWin.CancelCaptureKey(iVoiceKey1);						
			}

		if ( iPrioritySet )
		    {
    		// Return normal high-priority in case there are other notifiers active 
	    	// and were are not going to lose foregroung right after following call	
		    iEikonEnv->RootWin().SetOrdinalPosition(0,ECoeWinPriorityAlwaysAtFront);
		    }
		 						
		iEikonEnv->BringForwards(EFalse);	
		iEikonEnv->EikAppUi()->RemoveFromStack(this);
		

		static_cast<CAknAppUi*>(iEikonEnv->EikAppUi())->KeySounds()->ReleaseContext();
		static_cast<CAknAppUi*>(iEikonEnv->EikAppUi())->KeySounds()->PopContext();

		
		if ( iECSSupport )
            {
		    delete iEcsDetector;
            }
		
		iFront = EFalse;
		}
	
	if (iDeviceLockStatusObserver)
		delete iDeviceLockStatusObserver;
		if (iCallStatusObserver)
		delete iCallStatusObserver;
	}
//
// ----------------------------------------------------------
// CSCPQueryDialog::PreLayoutDynInitL()
// Called by framework before dialog is shown 
// ----------------------------------------------------------
//
void CSCPQueryDialog::PreLayoutDynInitL()
    {
    Dprint( (_L("CSCPQueryDialog::PreLayoutDynInitL()") ));
    
    /* CEikSecretEditor* queryControl = 
                    static_cast<CEikSecretEditor*>(
                        QueryControl()->ControlByLayoutOrNull( QueryControl()->QueryType() ) 
                       ); 
    queryControl->EnableSCT(EFalse); */
    
    CAknTextQueryDialog::PreLayoutDynInitL();
    
    //disable in call bubble.
    if ( AknLayoutUtils::PenEnabled() )
        {
        SetIncallBubbleAllowedInUsualL( EFalse );
        }

    
    // Create the ECS detector object if required
    if ( iECSSupport )
        {
        Dprint( (_L(" Creating ECS detector") ));
        iEcsDetector = CAknEcsDetector::NewL();
        iEcsDetector->SetObserver( this );
        }    
    
    Dprint( (_L("CSCPQueryDialog::PreLayoutDynInitL(): Set input mode") ));
        
    /* The default input mode of the lock code query must be set on the basis of the
    very first character of the current lock code. If the first character of the current
    lock code is numeric, the default input mode will also be numeric. Otherwise, the 
    default input mode will be alphabetic */
    
    CRepository* repository;
    repository = CRepository::NewL( KCRUidSCPParameters );
    CleanupStack::PushL( repository );
    
    User::LeaveIfError(repository->Get( KSCPLockCodeDefaultInputMode , def_mode) );
    
    CleanupStack::PopAndDestroy( repository );
    repository = NULL;   
    
    if (def_mode == 0)
    	SetDefaultInputMode( EAknEditorNumericInputMode ); 
    else
    	SetDefaultInputMode( EAknEditorSecretAlphaInputMode );
    
    // Set the mode, we use this to determine the functionality for special keys
    if ( ( iButtons == RSCPClient::SCP_OK ) || ( iButtons == RSCPClient::SCP_OK_CANCEL ) )
        {
        // Normal mode
        iMode = KSCPModeNormal;
        }
    else
        {
        // Restricted mode, used for example for Etel-originated queries
        iMode = KSCPModeRestricted;
        }
    
    Dprint( (_L("CSCPQueryDialog::PreLayoutDynInitL(): Set button config") ));
    // Set the softkey-configuration
    switch ( iButtons )
        {
            case ( RSCPClient::SCP_OK ):
            // Flow through
            case ( RSCPClient::SCP_OK_ETEL ):
                {
                // Hide the Cancel-softkey
                ButtonGroupContainer().MakeCommandVisible( EAknSoftkeyCancel, EFalse );
                break;
                }
            
            default:
                // Show both softkeys
            break;
        }
        
	QueryControl()->SetTextEntryLength( iMaxLength ); 
	
	// Add this higher than the environment filter, otherwise
	// we cannot capture keys from the EikSrvUi KeyFilter. 
	// Used because this query might be called from notifier		

	if ( iMode == KSCPModeNormal )
		{
		iEikonEnv->EikAppUi()->AddToStackL(this,
		                                   ECoeStackPriorityEnvironmentFilter+100,
		                                   ECoeStackFlagRefusesAllKeys);
		}
	else
		{
		iEikonEnv->EikAppUi()->AddToStackL(this,
		                                   ECoeStackPriorityEnvironmentFilter+100,
		                                   ECoeStackFlagRefusesFocus);
		}
	
	Dprint( (_L("CSCPQueryDialog::PreLayoutDynInitL(): Capture keys") ));
	// Capture app, voice and end keys if necessary
	if ( iMode == KSCPModeRestricted )		
		{		
		RWindowGroup& groupWin=iCoeEnv->RootWin();
		// Capture app key
		iAppKey = groupWin.CaptureKeyUpAndDowns(EStdKeyApplication0, 0, 0);
		// capture voice key
		iVoiceKey1 = groupWin.CaptureKey(EKeySide,0,0);
		iVoiceKey2 = groupWin.CaptureKeyUpAndDowns(EStdKeyDevice6, 0, 0);
		}

    RWsSession& wsSession = iEikonEnv->WsSession();
    TInt myWgId = iEikonEnv->RootWin().Identifier();
    
    TInt wgPrio = wsSession.GetWindowGroupOrdinalPriority(myWgId);
	Dprint( (_L("CSCPQueryDialog::PreLayoutDynInitL(): wgPrio %d"),wgPrio ));
	TInt var;
	RProperty::Get(KPSUidCtsyCallInformation, KCTsyCallState,var);
	Dprint( (_L("CSCPQueryDialog::PreLayoutDynInitL(): EPSCTsyCallStateNone %d"),var ));
    // we are already on forgeround, need to update priority differently
	if (var != EPSCTsyCallStateNone)
	{
		// If the call is made during device startup have the priority as normal
		if (iECSSupport && (iButtons == RSCPClient::SCP_OK))
		{
		iEikonEnv->RootWin().SetOrdinalPosition(1,ECoeWinPriorityNormal);
		}
	
	}
    else if ((wgPrio == ECoeWinPriorityAlwaysAtFront)&&(iECSSupport))
        {
        Dprint( (_L("CSCPQueryDialog::PreLayoutDynInitL(): ECoeWinPriorityAlwaysAtFront+1") ));	
        iEikonEnv->RootWin().SetOrdinalPosition(0,ECoeWinPriorityAlwaysAtFront+1);
        iPrioritySet = ETrue;
        }
        
   // this must be done always to keep the reference count in synch  
   // this does not have any effect if autoforwarding has not been set true (normal application.)
   iEikonEnv->BringForwards(ETrue, ECoeWinPriorityAlwaysAtFront+1);

	/// -- Change Window Priority for dialog and CBA 
	if (iECSSupport)
		{
		Dprint( (_L("CSCPQueryDialog::PreLayoutDynInitL(): Changing Window Priority") ));			
		DrawableWindow()->SetOrdinalPosition(0,ECoeWinPriorityAlwaysAtFront);
		ButtonGroupContainer().ButtonGroup()->AsControl()->DrawableWindow()->SetOrdinalPosition(0, ECoeWinPriorityAlwaysAtFront); 	
		}
	else
		{
		DrawableWindow()->SetOrdinalPosition(0,ECoeWinPriorityNormal); //
		ButtonGroupContainer().ButtonGroup()->AsControl()->DrawableWindow()->SetOrdinalPosition(0,ECoeWinPriorityNormal); 	  	
		}	

	Dprint( (_L("CSCPQueryDialog::PreLayoutDynInitL(): Key sounds") ));
	// Key sounds

	static_cast<CAknAppUi*>(iEikonEnv->EikAppUi())->KeySounds()->
	    PushContextL(R_AVKON_DEFAULT_SKEY_LIST);
	    	                                                             
	static_cast<CAknAppUi*>(iEikonEnv->EikAppUi())->KeySounds()->BringToForeground();
	static_cast<CAknAppUi*>(iEikonEnv->EikAppUi())->KeySounds()->LockContext();

	iFront = ETrue;
	
	Dprint( (_L("CSCPQueryDialog::CSCPLockObserver") ));
	iDeviceLockStatusObserver = CSCPLockObserver::NewL(this);
	iCallStatusObserver = CSCPLockObserver::NewL(this,ESecUiCallStateObserver);

	}
//
// ---------------------------------------------------------
// CSCPQueryDialog::OfferKeyEventL
// called by framework when any key is pressed
// ---------------------------------------------------------
//
TKeyResponse CSCPQueryDialog::OfferKeyEventL(const TKeyEvent& aKeyEvent, TEventCode aType)
	{
    Dprint( (_L("CSCPQueryDialog::OfferKeyEventL(%d, %d, %d)"), aKeyEvent.iScanCode, 
        aKeyEvent.iCode, aType ));
    
    //Handling for Key up events for special characters  
    if (aType == EEventKeyUp && iPriorityDropped && iKeyUsed
            == aKeyEvent.iScanCode)
        {
        DrawableWindow()->SetOrdinalPosition(0, ECoeWinPriorityAlwaysAtFront);
        ButtonGroupContainer().ButtonGroup()->AsControl()->DrawableWindow()->SetOrdinalPosition(
                0, ECoeWinPriorityAlwaysAtFront);
        iPriorityDropped = EFalse;
        iKeyUsed = NULL;
        Dprint((_L("CSCPQueryDialog::OfferKeyEventL(): Priority dropped")));
        return EKeyWasConsumed;
        }
    else
        {
        if (iPriorityDropped && iKeyUsed != aKeyEvent.iScanCode
                && (aKeyEvent.iScanCode == EStdKeyDevice0
                        || aKeyEvent.iScanCode == EStdKeyDevice1
                        || aKeyEvent.iScanCode == EStdKeyDevice2
                        || aKeyEvent.iScanCode == EStdKeyDevice3))
            {
            iPriorityDropped = EFalse;
            iKeyUsed = NULL;
            }

        }
    
    //Consume the characters which are typed before Key up events of special characters
    if (iPriorityDropped && iKeyUsed != NULL)
        {
        return EKeyWasConsumed;
        }

    	            		            
	// Check if the priority should be dropped, so that special dialogs can be shown
	if ( ( aType == EEventKeyDown ) && 
	     ( ( aKeyEvent.iScanCode == EStdKeyLeftShift ) || // Pen-key
	       ( aKeyEvent.iScanCode == EStdKeyRightShift ) || // Pen-key
	       ( aKeyEvent.iScanCode == EStdKeyLeftFunc )  || //Chr-key
	       ( aKeyEvent.iScanCode == EStdKeyNkpAsterisk ) || // * - key
	       ( aKeyEvent.iScanCode == '*' )                  // * - key
	     )
	   )
	    {
        iKeyUsed = aKeyEvent.iScanCode;
	    DrawableWindow()->SetOrdinalPosition(0,0);
	    ButtonGroupContainer().ButtonGroup()->AsControl()->DrawableWindow()->SetOrdinalPosition(0,0);
	    iPriorityDropped = ETrue;
	    
	    Dprint( (_L("CSCPQueryDialog::OfferKeyEventL(): Priority dropped") ));
	    }
	else
	    {
	    // Return priority, either no dialogs shown, or already exited.
	    if ( iPriorityDropped )
	        {
            DrawableWindow()->SetOrdinalPosition(0,ECoeWinPriorityAlwaysAtFront);
	        ButtonGroupContainer().ButtonGroup()->AsControl()->DrawableWindow()->SetOrdinalPosition(0,ECoeWinPriorityAlwaysAtFront);
	        iPriorityDropped = EFalse;
	        Dprint( (_L("CSCPQueryDialog::OfferKeyEventL(): Priority re-raised") ));
	        }
	    }
		
    if ( aKeyEvent.iCode == EKeyOK || aKeyEvent.iCode == EKeyEnter)
        {
        // Try to exit with OK as well
        TryExitL(EAknSoftkeyOk);
        return EKeyWasConsumed;
        }

	// app key
	if (aKeyEvent.iScanCode == EStdKeyApplication0)
		{
		if ( iButtons == RSCPClient::SCP_OK_CANCEL )
			{
			TryExitL(EAknSoftkeyCancel);
			return EKeyWasNotConsumed;	
			}
		return EKeyWasConsumed;
		}
		
	// end key
	if ( (aKeyEvent.iCode == EKeyPhoneEnd) || (aKeyEvent.iCode == EKeyEscape) )
        {
        if ( ( iButtons == RSCPClient::SCP_OK_CANCEL ) ||  
            ( iButtons == RSCPClient::SCP_OK_CANCEL_ETEL ) )
            {
            TryExitL(EAknSoftkeyCancel);          
            }
        return EKeyWasConsumed;
        }

	if (aKeyEvent.iCode == EKeyPhoneSend)
	    {
        //if (AknLayoutUtils::PenEnabled())
            {
            if (iECSSupport)
                {
                if (iEcsDetector->State()== CAknEcsDetector::ECompleteMatch)
                    {
                    iEcsDetector->AddChar( (TText)(EKeyPhoneSend) );
                    
                    }   
                else
                    {
                    Dprint( (_L("CSCPQueryDialog::OfferKeyEventL(): ShowWarningNoteL") ));
                    ShowWarningNoteL();
                    return EKeyWasConsumed;
                    }
                }
            else //consume the key to prevent dialler from opening
                {
                Dprint( (_L("CSCPQueryDialog::OfferKeyEventL(): EKeyPhoneSend :EKeyWasConsumed") ));
                return EKeyWasConsumed;
                }
            }
        }
	    
    if ( ( aType == EEventKey ) && ( aKeyEvent.iCode == KSCPSpecialDeleteEvent ) )
        {
        // This special event is sent to remove the previous character while "cycling" with
        // the numeric keys. Discard, if the previous character was already removed.
        if ( iPreviousCharacterWasInvalid )
            {
            return EKeyWasConsumed;
            }
        }
	               			
    if ( ( aType == EEventKey ) && ( iECSSupport ) )
        {        
        // Reset the field text
        QueryControl()->SetTextL( iTextBuffer );        
        }
            
	TKeyResponse ret = QueryControl()->OfferKeyEventL( aKeyEvent, aType );
                
    // Check that only alphanumeric letters are entered    
    if ( ( QueryControl() != NULL ) && ( QueryControl()->GetTextLength() > iValidTextLen ) )
        {                    
        // A new character was added to the editor
        iTmpBuffer.Zero();
        QueryControl()->GetText( iTmpBuffer );
        
        TChar newChar = iTmpBuffer[ iTmpBuffer.Length() - 1 ];
        TChar::TCategory newCat = newChar.GetCategory();
        if ( newChar.IsSpace() || newChar < 1 || newChar > 126 )
        /* if ( !newChar.IsAlphaDigit() && newCat != TChar::ENdCategory ) */
            {
            // Remove the last character, not allowed
            iTmpBuffer.SetLength( iTmpBuffer.Length() - 1 );
            QueryControl()->SetTextL( iTmpBuffer );
            
            iPreviousCharacterWasInvalid = ETrue;
            
            // Hide the OK key if the first character was invalid
            if ( iValidTextLen == 0 )
                {
                if (AknLayoutUtils::PenEnabled())
                    {
                    ButtonGroupContainer().DimCommand(EAknSoftkeyOk, ETrue);
                    }
                else
                    {
                    ButtonGroupContainer().MakeCommandVisible(EAknSoftkeyOk, EFalse);
                    }

                }
            }
        else
            {
            iValidTextLen++;
            iPreviousCharacterWasInvalid = EFalse;            	        	            
            }            
        }
    else if ( ( QueryControl() != NULL ) && ( QueryControl()->GetTextLength() < iValidTextLen ) )
        {
        // Character removed, make sure we're back on track,
        // though likely only backspace was pressed
        iValidTextLen = QueryControl()->GetTextLength();
        }   
        
//Additional modifier set by query dialog, while simulating event
// As raw key events are used to simulate, modifiers and scan codes cannot be set at a time
// In the 1st event, modifiers are set and in subsequent event, scan codes will be set.
if(aKeyEvent.iModifiers & EModifierNumLock &&
        aKeyEvent.iModifiers & EModifierKeypad && aKeyEvent.iModifiers & EModifierSpecial
)
    {
   
    return EKeyWasConsumed;
    }
    if ( iECSSupport )
        {
        // Save the text
        if ( aType == EEventKey ) 
            {
            QueryControl()->GetText( iTextBuffer );
            }
        if ( aType == EEventKey ) 
            {
            iEcsDetector->SetBuffer(iTextBuffer.Left(KAknEcsMaxMatchingLength));
            }
        if ( aType == EEventKey )
            {
        if ( iEMCallActivated )
            {
            TryExitL( ESecUiEmergencyCall );
            return EKeyWasConsumed;
            }
        }
        // Retrieve the editor control ptr, we know it is a secret editor..
        CEikSecretEditor* queryControl = static_cast<CEikSecretEditor*>( 
            QueryControl()->ControlByLayoutOrNull( QueryControl()->QueryType() ) );                
        
         // Further check to ensure that the matched number is the entire buffer
         // Get the matched text and see if is the same length as the current query length
        if ( iShowingEMNumber && (QueryControl()->GetTextLength()== iEcsDetector->CurrentMatch().Length()))
            {
            // The previous keyDown-event resulted in a complete Em-number match.
                              
            // An emergency number is entered, show the text in the field
            QueryControl()->SetTextL( iEcsDetector->CurrentMatch() );            
            if ( queryControl != NULL )
                {
                queryControl->RevealSecretText( ETrue );
                if (isCallSoftkeyAdded == EFalse)
                {
                	isCallSoftkeyAdded = ETrue;
                HBufC* cbaLabel = NULL;
				Dprint( (_L("CSCPQueryDialog::OfferKeyEventL(): R_AVKON_SOFTKEY_CALL_TEXT") ));
                TRAPD ( err, cbaLabel =
                        StringLoader::LoadL( R_AVKON_SOFTKEY_CALL_TEXT) );
                if ( err == KErrNone )
                {
                	ButtonGroupContainer().RemoveCommandFromStack(0,EAknSoftkeyOk);
                TRAP ( err, ButtonGroupContainer().AddCommandToStackL( 0, EAknSoftkeyEmergencyCall,
                        *cbaLabel) );
				ButtonGroupContainer().DrawDeferred();
                delete cbaLabel;
                }
              }
                }            
            }
        else
            {
            // The input doesn't match a Em-number
            if ( queryControl != NULL )
                {
                queryControl->RevealSecretText( EFalse );
			if(aType == EEventKey)
			{
                HBufC* cbaLabel = NULL;
                TRAPD ( err , cbaLabel= StringLoader::LoadL(R_SCPDIALOG_OK_TEXT) );
                if ( err == KErrNone )
                {
					if (isCallSoftkeyAdded)
					{
						isCallSoftkeyAdded = EFalse;
						ButtonGroupContainer().RemoveCommandFromStack(0,EAknSoftkeyEmergencyCall);
					}
					else
					{
					ButtonGroupContainer().RemoveCommandFromStack(0,EAknSoftkeyOk);
					}
                TRAP ( err , ButtonGroupContainer().AddCommandToStackL(0,EAknSoftkeyOk,*cbaLabel) );
                ButtonGroupContainer().DrawDeferred();
                delete cbaLabel;
				}
            	  }
                }
            }
        }                  
     
    Dprint( (_L("CSCPQueryDialog::OfferKeyEventL() exiting") ));
              
    if ( ret == EKeyWasConsumed ) // The editor control used up this keyevent
        {
        return ret;
        }         
    else
        {
        return CAknTextQueryDialog::OfferKeyEventL(aKeyEvent,aType);
        }    
	}
//
// ---------------------------------------------------------
// CSCPQueryDialog::NeedToDismissQueryL()
// Handles '#' key called by CAknTextQueryDialog::OfferKeyEventL()
// ---------------------------------------------------------
//
TBool CSCPQueryDialog::NeedToDismissQueryL(const TKeyEvent& /*aKeyEvent*/)
	{
	return EFalse;
	}
//
// ---------------------------------------------------------
// CSCPQueryDialog::OkToExitL()
// called by framework when the Softkey is pressed
// ---------------------------------------------------------
//
TBool CSCPQueryDialog::OkToExitL(TInt aButtonId)
	{
	
	     Dprint( (_L("CSCPQueryDialog::OkToExitL ( aButtonId :%d, "), aButtonId ));
	TInt ret = EFalse;
	
    if ( aButtonId == EAknSoftkeyOk )
		{	        		
		Dprint( (_L("CSCPQueryDialog::OkToExitL - EAknSoftkeyOk") ));		
		TInt textLength = QueryControl()->GetTextLength();
		if ( textLength < iMinLength )
			{	
			// The code was too short -> play error tone & clear editor
			if ( textLength != 0 )
				{
				CAknKeySoundSystem* soundSystem;
	            if ( iEikonEnv->AppUi() )
		            {		            
		            soundSystem = static_cast<CAknAppUi*>(iEikonEnv->EikAppUi())->KeySounds();
		            soundSystem->PlaySound( EAvkonSIDErrorTone );
		            } 
				QueryControl()->SetTextL( KNullDesC );
				iTextBuffer.Zero();
				iValidTextLen = 0;

                // Hide the OK key
                //ButtonGroupContainer().MakeCommandVisible( EAknSoftkeyOk, EFalse );
               // ButtonGroupContainer().DimCommand(EAknSoftkeyOk, ETrue);

                if (AknLayoutUtils::PenEnabled())
                    {
                    ButtonGroupContainer().DimCommand(EAknSoftkeyOk, ETrue);
                    }
                else
                    {
                    ButtonGroupContainer().MakeCommandVisible(EAknSoftkeyOk, EFalse);
                    }
                }
            ret = EFalse;
            }	
		else
			{		
			ret = CAknTextQueryDialog::OkToExitL(aButtonId);
			}			    
		}
				
	// Cancel is allowed, if the key is active
	if ( aButtonId == EAknSoftkeyCancel )
		{
    	Dprint( (_L("CSCPQueryDialog::OkToExitL EAknSoftkeyCancel ")) );	
	    ret = CAknTextQueryDialog::OkToExitL(aButtonId);
		}
		
    // Emergency call, exit
    if ( aButtonId == ESecUiEmergencyCall )
        {
		Dprint( (_L("CSCPQueryDialog::OkToExitL(): ESecUiEmergencyCall") ));
        ret = ETrue;
        }
    if (aButtonId == EAknSoftkeyEmergencyCall)
        {
        //add EKeyPhonesend to ecs detector. 
        ret = ETrue;
        Dprint( (_L("CSCPQueryDialog::OkToExitL(): adding EKeyPhoneSend to detector") ));
         iEcsDetector->AddChar( (TText)(EKeyPhoneSend ) );
        CAknTextQueryDialog::OkToExitL(aButtonId);
        }
   
	Dprint( (_L("CSCPQueryDialog::OkToExitL() done") ));   
	return ret;
	}


// ---------------------------------------------------------
// CSCPQueryDialog::HandleEcsEvent()
// 
// ---------------------------------------------------------
//
void CSCPQueryDialog::HandleEcsEvent(CAknEcsDetector* aDetector, 
                                     CAknEcsDetector::TState aUpdatedState)
    {   
    (void)aDetector; // Not used
    
    TInt err;
    if ( ( aUpdatedState == CAknEcsDetector::ECompleteMatchThenSendKey ) || 
         ( aUpdatedState == CAknEcsDetector::ECallAttempted ) )
        {
        // Call attempted, cancel the query
        iEMCallActivated = ETrue; // OfferKeyEventL will close the dialog         
        }        
    else if ( aUpdatedState == CAknEcsDetector::ECompleteMatch )    
        {                
        iShowingEMNumber = ETrue;
        }
    else if ( iShowingEMNumber )
        {
        // Cancel Em-number display
        iShowingEMNumber = EFalse;
        
        if ( aUpdatedState == CAknEcsDetector::EEmpty )
            {
            // Key timeout: OfferKeyEventL won't be called, so reset the display here
            TRAP( err, QueryControl()->SetTextL( iTextBuffer ) );
            (void)err;
            
            // Retrieve the editor control ptr, we know it is a secret editor..
            CEikSecretEditor* queryControl = 
                static_cast<CEikSecretEditor*>(
                    QueryControl()->ControlByLayoutOrNull( QueryControl()->QueryType() ) 
                   );            
            if ( queryControl != NULL )
                {
                queryControl->RevealSecretText( EFalse );
                }
           // }        
                HBufC* cbaLabel = NULL;
				Dprint( (_L("CSCPQueryDialog::HandleEcsEvent(): R_SCPDIALOG_OK_TEXT") ));
                TRAP ( err, cbaLabel= StringLoader::LoadL(R_SCPDIALOG_OK_TEXT) );
                if ( err == KErrNone )
                {
                	if (isCallSoftkeyAdded)
						{
							isCallSoftkeyAdded = EFalse;
							ButtonGroupContainer().RemoveCommandFromStack(0,EAknSoftkeyEmergencyCall);
						}
						else
						{
							ButtonGroupContainer().RemoveCommandFromStack(0,EAknSoftkeyOk);
						}
                	TRAP ( err , ButtonGroupContainer().AddCommandToStackL(0, EAknSoftkeyOk, *cbaLabel) );
                	ButtonGroupContainer().DrawDeferred();
                	delete cbaLabel;
                }
            }
        }    
    }
 
void CSCPQueryDialog::ShowWarningNoteL()
    {
    
    CAknNoteDialog* noteDlg = new (ELeave) CAknNoteDialog();
    noteDlg->PrepareLC(R_SCPDIALOG_EMERGENCYCALLS_ONLY);
    noteDlg->SetTimeout(CAknNoteDialog::ELongTimeout);
    noteDlg->SetTone(CAknNoteDialog::EErrorTone);
    
    noteDlg->DrawableWindow()->SetOrdinalPosition(0,ECoeWinPriorityAlwaysAtFront+1); //
    noteDlg->ButtonGroupContainer().ButtonGroup()->AsControl()->DrawableWindow()->SetOrdinalPosition(0,ECoeWinPriorityAlwaysAtFront+1); 
    
    noteDlg->RunLD();

    }
    
// -----------------------------------------------------------------------------
// CSCPQueryDialog::SetIncallBubbleAllowedInUsualL()
// -----------------------------------------------------------------------------
//
void CSCPQueryDialog::SetIncallBubbleAllowedInUsualL(TBool aAllowed)
    {
    CAknIncallBubble *incallBubble =  CAknIncallBubble::NewL();
    CleanupStack::PushL(incallBubble);
    incallBubble->SetIncallBubbleAllowedInUsualL( aAllowed );
    CleanupStack::PopAndDestroy();
    }
	
// Call from the SCPObserver
	
void CSCPQueryDialog::TryCancelQueryL(TInt aReason)
    {
    Dprint( (_L("CSCPQueryDialog::TryCancelQueryL()")) );
    
	if (EPSCTsyCallStateDisconnecting == aReason)
	{
	Dprint( (_L("CSCPQueryDialog::TryCancelQueryL() -SetOrdinalPosition ->0")) );
	iEikonEnv->RootWin().SetOrdinalPosition(0,ECoeWinPriorityAlwaysAtFront+1);
	iPrioritySet = ETrue;
	}
	else
	{
	Dprint( (_L("CSCPQueryDialog::TryExitL(EAknSoftkeyCancel)")) );
    TryExitL(EAknSoftkeyCancel);
	}
    }

    
// End of file

