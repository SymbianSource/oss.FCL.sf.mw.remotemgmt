/*
* Copyright (c) 2005-2006 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  The firmware update result notifier.
*
*/



// INCLUDE FILES
#include <eikenv.h>          // Eikon environment
#include <uikon/eiksrvui.h>
#include <AknQueryDialog.h>
#include <aknmessagequerydialog.h>
#include <aknstaticnotedialog.h>
#include <centralrepository.h>  
#include <StringLoader.h>    // Localisation stringloader
#include <SyncMLClient.h>
#include <SyncMLClientDS.h>
#include <SyncMLClientDM.h>
#include <centralrepository.h>
#include <DevManInternalCRKeys.h>   
#include <SyncMLNotifierDomainCRKeys.h> 
#include <SyncMLNotifier.rsg>           // Own resources
#include <aknnotewrappers.h>
#include <e32property.h>

#include "SyncMLFwUpdNotifier.h"  // Class declaration
#include "SyncMLNotifDebug.h"

#include "SyncMLQueryDialog.h"
#include "SyncMLMessageQueryDialog.h"
#include "SyncMLNotifPrivateCRKeys.h"

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CSyncMLFwUpdNotifier::CSyncMLFwUpdNotifier
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CSyncMLFwUpdNotifier::CSyncMLFwUpdNotifier()
    {
    iRunLhang = EFalse;
    iDlg = NULL;
    iFotaEncryptReq = EFalse;
    }

// -----------------------------------------------------------------------------
// CSyncMLDSNotifier::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CSyncMLFwUpdNotifier* CSyncMLFwUpdNotifier::NewL()
    {
    CSyncMLFwUpdNotifier* self = new (ELeave) CSyncMLFwUpdNotifier();
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();
    return self;
    }

    
// -----------------------------------------------------------------------------
// Destructor
// -----------------------------------------------------------------------------
//
CSyncMLFwUpdNotifier::~CSyncMLFwUpdNotifier()
    {
    Cancel();   // Free own resources
    }


// -----------------------------------------------------------------------------
// CSyncMLFwUpdNotifier::RegisterL
// -----------------------------------------------------------------------------
//
CSyncMLFwUpdNotifier::TNotifierInfo CSyncMLFwUpdNotifier::RegisterL()
    {
    FLOG(_L("[SmlNotif]\t CSyncMLFwUpdNotifier::RegisterL()"));
    iInfo.iUid = KSyncMLFwUpdNotifierUid;
    iInfo.iChannel = KSmlFwUpdChannel;
    iInfo.iPriority = ENotifierPriorityHigh;
    FLOG(_L("[SmlNotif]\t CSyncMLFwUpdNotifier::RegisterL() completed"));
    return iInfo;
    }

// -----------------------------------------------------------------------------
// CSyncMLFwUpdNotifier::GetParamsL
// Initialize parameters and check if device is already
// in registry. Jump to RunL as soon as possible.
// -----------------------------------------------------------------------------
//
void CSyncMLFwUpdNotifier::GetParamsL( const TDesC8& aBuffer,
                                             TInt aReplySlot,
                                       const RMessagePtr2& aMessage )
    {
    FLOG(_L("[SmlNotif]\t CSyncMLFwUpdNotifier::GetParamsL()"));

    if( iReplySlot != NULL || iNeedToCompleteMessage )
        {
        User::Leave( KErrInUse );
        }
    iMessage = aMessage;
    iNeedToCompleteMessage = ETrue;
    iReplySlot = aReplySlot;

	TSyncMLFwUpdNotifParams param;
 	TPckgC<TSyncMLFwUpdNotifParams> pckg( param );
 	pckg.Set( aBuffer );

    iNoteType = pckg().iNoteType;
    iNoteInfo = pckg().iIntParam;
    iFotaMemoryNeeded = pckg().iMemoryNeeded;
    iFotaEncryptReq = pckg().iEncryptReq;
	
	iUserPostponeCount = pckg().iFotaUserPostponeCount;
    iMaxPostponeCount = pckg().iFotaMaxPostponeCount;
    iIntervalType = pckg().iIntervalType;
    iInterval = pckg().iInterval;
    
    if(pckg().iSelectedReminder.Length() > 0)
        iSelectedReminder.Copy(pckg().iSelectedReminder);

    // Call SetActive() so RunL() will be called by the active scheduler
    //
    SetActive();
    iStatus = KRequestPending;
    TRequestStatus* stat = &iStatus;
    User::RequestComplete( stat, KErrNone );

    FLOG(_L("[SmlNotif]\t CSyncMLFwUpdNotifier::GetParamsL() completed"));
    }

// -----------------------------------------------------------------------------
// CSyncMLFwUpdNotifier::RunL
// Ask user response and return it to caller.
// -----------------------------------------------------------------------------
//
void CSyncMLFwUpdNotifier::RunL()
    {
    FLOG(_L("[SmlNotif]\t CSyncMLFwUpdNotifier::RunL()"));

    TInt result( KErrNone );
    const TInt KNormal = 1;
		const TInt KForce = 0;

    // Turn lights on and deactivate apps -key
    //
    TurnLightsOn();
    SuppressAppSwitching( ETrue );

    switch( iNoteType )
        {
        case ESyncMLFwUpdErrorNote:
            {
            result = ShowUpdateErrorNoteL();
            break;
            }

        case ESyncMLFwUpdResultNote:
            {
            if (iFotaEncryptReq)
                {
                result = ShowUpdateResultWithEncryptionNoteL();
                }
            else
                {
                result = ShowUpdateResultNoteL();
                }
            break;
            }
		case ESyncMLFwUpdPostponeLimitQuery:
            {
            result = ShowPostLimitUpdateStartQueryL(KNormal);
       			break;
     				}
        
        case ESyncMLFwUpdForceQuery:
            {
            result = ShowPostLimitUpdateStartQueryL(KForce);
            break;
            }
            
        case ESyncMLFwUpdStartQuery:
            {
            result = ShowUpdateStartQueryL();
            break;
            }
        case ESyncMLFwUpdRebootNote:
            {
            CAknStaticNoteDialog* dlg = new ( ELeave ) CAknStaticNoteDialog;
            dlg->PrepareLC( R_SML_FOTA_RESTART_NOTE );

            HBufC* stringHolder = StringLoader::LoadLC( R_FOTA_REBOOT_NOTE );
            dlg->SetTextL( *stringHolder );
            CleanupStack::PopAndDestroy( stringHolder );

            result = dlg->RunLD();
            if ( result == KErrNone )
                {
                result = EAknSoftkeyOk;
                };
            
            break;
            }
     	case ESyncMLFwUpdNoReminder:
     		{
     		 result =  ShowNoReminderQueryL();
			 break;
            }
        case ESyncMLFwUpdLargeNotEnoughMemory:
        	{
        	 result = ShowNotEnoughMemoryL( iFotaMemoryNeeded , EFalse );
			 break;
        	}
        case ESyncMLFwUpdOmaDLNotEnoughMemory:
        	{
        	 result = ShowNotEnoughMemoryL( iFotaMemoryNeeded , ETrue );
			 break;
        	}	
        case ESyncMLFwUpdOmaDLUserCancel:
        	{
        	 result = ShowDownloadUserCancelL();
        	 break;	
        	}
        case ESyncMLFwUpdOmaDLPostponed:
        	{
        	 result = ShowDownloadPostponedL();
        	 break;	
        	}
        case ESyncMLFwUpdOmaDLCancelled:
        	{
        	 result = ShowDownloadCancelledL();
        	 break;	
        	}
        case ESyncMLFwUpdOmaDLResume:
        	{
        	 result = ShowDownloadResumeL();
        	 break;	
        	}
        case ESyncMLFwUpdStartQueryEnc:
            {
            result = ShowUpdateStartQueryEncryptionL();
            break;
            }
		case ESyncMLFwUpdPostponeNote:
            {
            result = ShowPostponeLimitNoteL();
            break; 
            }
        default: // Unhandled note type
            {
            FLOG(_L("[SMLNOTIF]\t CSyncMLDlgNotifier::RunL() Note type not handled"));
            User::Leave( KErrArgument );
            break;
            }
        }

    // Activate apps -key again
    //
    SuppressAppSwitching( EFalse );

		if ( iNeedToCompleteMessage )
			{
			// Translate the user key presses to message completion response
			FTRACE( FPrint( _L("[SmlNotif]\t User Key-Press for dialog = %d" ), result ) );
			switch( result ) 
				{
					case ESyncMLDlgRespKeyOk://all LSK press will fall here.
						iMessage.Complete( KErrNone );
						break;
					case ESyncMLDlgRespKeyCancel://all RSK press will fall here.
						iMessage.Complete (KErrCancel);
						break;
					case ESyncMLDlgRespKeyOthers: //all call-termination key press fall here.
						iMessage.Complete (KErrNotFound);
						break;
					default://rest of the cases and errors
						iMessage.Complete (KErrNone);
						break;
				}		
			}


    // Complete message and free resources
    //
    iNeedToCompleteMessage = EFalse;
    iReplySlot = NULL;

    iNoteType = ESyncMLFwUpdUnknown;

    FLOG(_L("[SmlNotif]\t CSyncMLFwUpdNotifier::RunL() completed"));
    }

// -----------------------------------------------------------------------------
// CSyncMLFwUpdNotifier::Cancel
// Release all own resources (member variables)
// -----------------------------------------------------------------------------
//
void CSyncMLFwUpdNotifier::Cancel()
    {
    FLOG(_L("[SmlNotif]\t CSyncMLFwUpdNotifier::Cancel()"));
    if(iRunLhang)
    	{
    	 FLOG(_L("[SmlNotif]\t CSyncMLFwUpdNotifier::Cancel() inside Runlhang"));
    	if(iDlg)
    		{
    		FLOG(_L("[SmlNotif]\t CSyncMLFwUpdNotifier::Cancel()before exit"));
    		TRAP_IGNORE(iDlg->ExitL());
    		if(iDlg)
    			{
        		FLOG(_L("[SmlNotif]\t CSyncMLFwUpdNotifier::Cancel()before delete"));
    			delete iDlg;
    			iDlg = NULL;
        		FLOG(_L("[SmlNotif]\t CSyncMLFwUpdNotifier::Cancel()after delete"));
    			}
    		}
    	}
    CSyncMLNotifierBase::Cancel();

    FLOG(_L("[SmlNotif]\t CSyncMLFwUpdNotifier::Cancel() completed"));
    }

// -----------------------------------------------------------------------------
// CSyncMLFwUpdNotifier::ShowUpdateErrorNoteL
// -----------------------------------------------------------------------------
//
TInt CSyncMLFwUpdNotifier::ShowUpdateErrorNoteL()
    {
    FTRACE( FPrint( _L(
        "[SmlNotif]\t CSyncMLFwUpdNotifier::ShowUpdateErrorNoteL() error = %d" ),
        iNoteInfo ) );

    HBufC* stringHolder( NULL );

    switch ( iNoteInfo )
        {
        case KErrDiskFull:
            {
            stringHolder = StringLoader::LoadLC( R_FOTA_NOT_PERMITTED );
            break;
            }
        case KErrCommsLineFail:
            {
            stringHolder = StringLoader::LoadLC( R_FOTA_LOST_CONNECTION );
            break;
            }
        case KErrCouldNotConnect:
            {
            stringHolder = StringLoader::LoadLC( R_FOTA_SERVER_ERROR );
            break;
            }
        case KErrBadPower:
            {
            stringHolder = StringLoader::LoadLC( R_FOTA_BATTERY_LOW );
            break;
            }
        case KErrBadUsbPower:
             {
             	stringHolder = StringLoader::LoadLC( R_FOTA_USB_BATTERY_LOW ); 
             	break;
            }
        case KErrCommsLineFail2:
            {
            stringHolder = StringLoader::LoadLC( R_FOTA_CONNECTION_FAIL2 );
            break;
            }
        case KErrGeneralResume:
            {
            stringHolder = StringLoader::LoadLC( R_FOTA_DOWNLOAD_GENERAL_RESUME );
            break;
            }
        case KErrGeneralNoResume:
            {
            stringHolder = StringLoader::LoadLC( R_FOTA_DOWNLOAD_NO_RESUME );
            break;
            }
        case KErrBadPowerForEnc:
            {
            stringHolder = StringLoader::LoadLC( R_FOTA_UPDATE_ENC_BATTERY_LOW );
            break;
            }
        case KErrDeviceMemoryBusy:
            {
            stringHolder = StringLoader::LoadLC( R_FOTA_DEVICE_MEMORY_BUSY );
            break;
            }
        default:
            {
            return KErrArgument; // Will be converted to KErrNone when completing
                                 // message, since this is not actually an error.
            }
        }

    
    TBool keypress(EFalse);
	CSyncMLMessageQueryDialog* msgDlg = CSyncMLMessageQueryDialog::NewL(keypress);
    msgDlg->SetMessageTextL(*stringHolder);
    
    CleanupStack::PopAndDestroy( stringHolder );
    
    msgDlg->PrepareLC( R_SML_MESSAGE_QUERY );  // Pushed dialog is popped inside RunLD

    stringHolder = StringLoader::LoadLC( R_FOTA_TITLE_PHONE_UPDATES );
    msgDlg->SetHeaderTextL( stringHolder->Des() );
    CleanupStack::PopAndDestroy( stringHolder );
    stringHolder = NULL;
    
   	msgDlg->ButtonGroupContainer().SetCommandSetL( 
    	                                  R_AVKON_SOFTKEYS_OK_EMPTY__OK );
    
    TInt retval (ESyncMLDlgRespKeyNone);
    iRunLhang = ETrue;
    iDlg = msgDlg; 
	retval = msgDlg->RunLD();
	iRunLhang = EFalse;
    msgDlg = NULL;
	iDlg = NULL;

	//call-termination key press is handled here.
	if (keypress)
    	retval = ESyncMLDlgRespKeyOthers;
    
    FTRACE( FPrint( _L(
        "[SmlNotif]\t CSyncMLFwUpdNotifier::ShowUpdateErrorNoteL() completed: retval = %d" ),
        retval ) );
        
    return retval;
    }

// -----------------------------------------------------------------------------
// CSyncMLFwUpdNotifier::ShowUpdateResultNoteL
// -----------------------------------------------------------------------------
//
TInt CSyncMLFwUpdNotifier::ShowUpdateResultNoteL()
    {
    FTRACE( FPrint( _L(
        "[SmlNotif]\t CSyncMLFwUpdNotifier::ShowUpdateResultNoteL() result = %d" ),
        iNoteInfo ) );

    HBufC* stringHolder( NULL );

    switch ( iNoteInfo )  // It is easier to add cases to switch structure
        {
        case KErrNone:
            {
            stringHolder = StringLoader::LoadLC( R_FOTA_NOTIF_COMPLETE );
            break;
            }
        default:
            {
            stringHolder = StringLoader::LoadLC( R_FOTA_MSG_QUERY_NOT_UPDATED );
            break;
            }
        }
        
    TBool keypress(EFalse);
	CSyncMLMessageQueryDialog* msgDlg = CSyncMLMessageQueryDialog::NewL(keypress);
    msgDlg->SetMessageTextL(*stringHolder);

    CleanupStack::PopAndDestroy( stringHolder );
    
    msgDlg->PrepareLC( R_SML_MESSAGE_QUERY );  // Pushed dialog is popped inside RunLD

    stringHolder = StringLoader::LoadLC( R_FOTA_TITLE_PHONE_UPDATES );
    msgDlg->SetHeaderTextL( stringHolder->Des() );
    CleanupStack::PopAndDestroy( stringHolder );
    stringHolder = NULL;
    
   	msgDlg->ButtonGroupContainer().SetCommandSetL( 
        									R_AVKON_SOFTKEYS_OK_EMPTY__OK );
    TInt retval(ESyncMLDlgRespKeyNone);
    iRunLhang = ETrue;
    iDlg = msgDlg; 
    retval = msgDlg->RunLD();
    iRunLhang = EFalse;
    msgDlg = NULL;
    iDlg = NULL;
	//call-termination key press has to be suppressed here. so keypress has no significance here!    
	/*if (keypress)
    	retval = EKeyOthers;*/
    	
    FTRACE( FPrint( _L(
        "[SmlNotif]\t CSyncMLFwUpdNotifier::ShowUpdateResultNoteL() completed: retval = %d" ),
        retval ) );
    
    return retval;
    }


// -----------------------------------------------------------------------------
// CSyncMLFwUpdNotifier::ShowPostLimitUpdateStartQueryL
// -----------------------------------------------------------------------------
//
TInt CSyncMLFwUpdNotifier::ShowPostLimitUpdateStartQueryL(TInt aQueryType)
{	    
	    FTRACE( FPrint( _L(
	        "[SmlNotif]\t CSyncMLFwUpdNotifier::ShowUpdateStartQueryL() profileId = %d" ),
	        aQueryType ) );
        
      HBufC* headerText = StringLoader::LoadLC( R_FOTA_TITLE_PHONE_UPDATES );
      TInt retval (ESyncMLDlgRespKeyNone);  
      
      TBool keypress(EFalse);
//      CSyncMLQueryDialog* dlg = CSyncMLQueryDialog::NewL(keypress);
      
      CSyncMLMessageQueryDialog* msgDlg = CSyncMLMessageQueryDialog::NewL(keypress);
      
      // Implementation of notes for postpone limit requirement.
        TBuf< KSyncMLMaxProfileNameLength > operatorName;
        RetrieveProfileNameL( operatorName );
        
        HBufC* stringHolder = NULL;
        retval = ESyncMLDlgRespKeyOk;       
        TBool bShowInstallQuery = ETrue;
        TInt fotaUpdateAppName (EFotaUpdateDM);
        TInt errAppName = RProperty::Get( KPSUidNSmlDMSyncApp, KNSmlCurrentFotaUpdateAppName, fotaUpdateAppName );
        TInt errShowInstall = RProperty::Get( KPSUidNSmlDMSyncApp, KDevManShowInstallNowQuery, bShowInstallQuery );

        FTRACE( FPrint(
                _L("CSyncMLFwUpdNotifier::ShowUpdateStartQueryL found key Show install query = %d"),
                bShowInstallQuery ) );


        if ( !errAppName && !errShowInstall && fotaUpdateAppName == EFotaUpdateNSC && !bShowInstallQuery )
            {
            FLOG(_L("[SmlNotif]\t CSyncMLFwUpdNotifier::ShowUpdateStartQueryL() do not show install now query "));  
            bShowInstallQuery = EFalse;
            }

        if ( !bShowInstallQuery )
            {
            FLOG(_L("[SmlNotif]\t CSyncMLFwUpdNotifier::ShowUpdateStartQueryL() should not query "));   
            }
        else
            {
            FLOG(_L("[SmlNotif]\t CSyncMLFwUpdNotifier::ShowUpdateStartQueryL() will query for Install now? "));
            if(aQueryType)
                {
                stringHolder = StringLoader::LoadLC( R_POST_LIMIT_FOTA_QUERY_INSTALL_NOW, operatorName );
                msgDlg->SetMessageTextL( *stringHolder );
                //dlg->SetPromptL( *stringHolder );
                }
            else
                {
                stringHolder = StringLoader::LoadLC( R_FOTA_QUERY_INSTALL_FORCE_NOW, operatorName );
                msgDlg->SetMessageTextL( *stringHolder );
                }
            CleanupStack::PopAndDestroy( stringHolder );
            stringHolder = NULL;
            retval = ESyncMLDlgRespKeyNone;
            if(aQueryType)
                {
                //retval = dlg->ExecuteLD( R_FOTA_INSTALL_CONFIRMATION_QUERY );
                msgDlg->PrepareLC( R_FOTA_INSTALL_MESSAGE_QUERY );
                msgDlg->SetHeaderTextL( headerText->Des() );
                retval = msgDlg->RunLD();
                }
            else
                {
                //retval = dlg->ExecuteLD( R_FOTA_FORCE_INSTALL_CONFIRMATION_QUERY );
                msgDlg->PrepareLC( R_FOTA_FORCE_INSTALL_CONFIRMATION_QUERY );
                msgDlg->SetHeaderTextL( headerText->Des() );
                msgDlg->RunLD();
                }
            //dlg = NULL;
            msgDlg = NULL;
            }   
        // set KDevManShowInstallNowQuery key back to value that it should show query
        TInt err = RProperty::Set(KPSUidNSmlDMSyncApp,KDevManShowInstallNowQuery,1 );
        FTRACE( FPrint(
                _L("Install query is set back to be shown, err = %d"),
                err ) );
        

        if ( !keypress && retval )
            {
            stringHolder = StringLoader::LoadLC( R_FOTA_UPDATE_2_WARNING );

            //keypress is EFalse here
            msgDlg = CSyncMLMessageQueryDialog::NewL(keypress);
            msgDlg->SetMessageTextL( *stringHolder );

            CleanupStack::PopAndDestroy( stringHolder );
            stringHolder = NULL;

            msgDlg->PrepareLC( R_SML_MESSAGE_QUERY );  // Pushed dialog is popped inside RunLD
            msgDlg->SetHeaderTextL( headerText->Des() );

            if(aQueryType)
                {
                msgDlg->ButtonGroupContainer().SetCommandSetL( 
                        R_SML_SOFTKEYS_ACCEPT_CANCEL__ACCEPT );
                }
            else
                {
                msgDlg->ButtonGroupContainer().SetCommandSetL( 
                        R_SML_SOFTKEYS_FORCE_ACCEPT__ACCEPT );
                }

            retval = ESyncMLDlgRespKeyNone;
            retval = msgDlg->RunLD();
            msgDlg = NULL;
            }
        
      CleanupStack::PopAndDestroy( headerText );
    
    FTRACE( FPrint( _L(
        "[SmlNotif]\t CSyncMLFwUpdNotifier::ShowUpdateStartQueryL() completed: retval = %d, keypress = %d" ),
        retval,keypress ) );

	//call-termination key press is handled here.
	if (keypress)
    	retval = ESyncMLDlgRespKeyOthers;
	
    return retval;
}

// -----------------------------------------------------------------------------
// CSyncMLFwUpdNotifier::ShowUpdateStartQueryL
// -----------------------------------------------------------------------------
//
TInt CSyncMLFwUpdNotifier::ShowUpdateStartQueryL()
    {
    FTRACE( FPrint( _L(
        "[SmlNotif]\t CSyncMLFwUpdNotifier::ShowUpdateStartQueryL() profileId = %d" ),
        iNoteInfo ) );

    HBufC* headerText = StringLoader::LoadLC( R_FOTA_TITLE_PHONE_UPDATES );
    TInt retval (ESyncMLDlgRespKeyNone);  
    TInt CustomNotes(KErrNone);
    CRepository* centrep = NULL;
    TRAPD( err, centrep = CRepository::NewL( KCRUidDeviceManagementInternalKeys) );    
    if(!err && centrep)
    {
      TInt err= centrep->Get( KDevManUINotesCustomization, CustomNotes);	
      if (err)
       CustomNotes = 0;
    }
    else
    {
      CustomNotes = 0;	
    }
    TBool keypress(EFalse);
    CSyncMLQueryDialog* dlg = CSyncMLQueryDialog::NewL(keypress);
    TLanguage language = User::Language();
    if((CustomNotes) && IsLanguageSupportedL())
    {       //This note will be shown in chinese and english variant only and localisation is not done
     CSyncMLMessageQueryDialog* msgDlg = CSyncMLMessageQueryDialog::NewL(keypress); 
     HBufC* stringHolder = StringLoader::LoadLC (R_QTN_FOTA_MSG_QUERY_UPDATE_CUSTOM_WARNING);     
     msgDlg->SetMessageTextL(*stringHolder);
     CleanupStack::PopAndDestroy( stringHolder );
     stringHolder = NULL;
     msgDlg->PrepareLC( R_SML_MESSAGE_QUERY );  // Pushed dialog is popped inside RunLD
     msgDlg->SetHeaderTextL( headerText->Des() );
     msgDlg->ButtonGroupContainer().SetCommandSetL( R_FOTA_INSTALL_QUERY_SOFTKEYS_NOW_LATER__NOW );
     retval = ESyncMLDlgRespKeyNone;
     retval = msgDlg->RunLD();
     msgDlg = NULL;
     FLOG(_L("[SmlNotif]\t CSyncMLFwUpdNotifier::ShowUpdateStartQueryL() after new changes in phone updates"));
    }
    else
    {
      TBuf< KSyncMLMaxProfileNameLength > profileName;
      RetrieveProfileNameL( profileName ); 
      
  HBufC* stringHolder = NULL;
	retval = ESyncMLDlgRespKeyOk;    	
	TBool bShowInstallQuery = ETrue;
	TInt fotaUpdateAppName (EFotaUpdateDM);
	TInt errAppName = RProperty::Get( KPSUidNSmlDMSyncApp, KNSmlCurrentFotaUpdateAppName, fotaUpdateAppName );
	TInt errShowInstall = RProperty::Get( KPSUidNSmlDMSyncApp, KDevManShowInstallNowQuery, bShowInstallQuery );

	FTRACE( FPrint(
	    _L("CSyncMLFwUpdNotifier::ShowUpdateStartQueryL found key Show install query = %d"),
	    bShowInstallQuery ) );


	if ( !errAppName && !errShowInstall && fotaUpdateAppName == EFotaUpdateNSC && !bShowInstallQuery )
	{
			FLOG(_L("[SmlNotif]\t CSyncMLFwUpdNotifier::ShowUpdateStartQueryL() do not show install now query "));	
			bShowInstallQuery = EFalse;
	}

	if ( !bShowInstallQuery )
	{
		
		FLOG(_L("[SmlNotif]\t CSyncMLFwUpdNotifier::ShowUpdateStartQueryL() should not query "));	
										
	}
	
	else
		
	{
		FLOG(_L("[SmlNotif]\t CSyncMLFwUpdNotifier::ShowUpdateStartQueryL() will query for Install now? "));		
    stringHolder = StringLoader::LoadLC( R_FOTA_QUERY_INSTALL_NOW, profileName );
    dlg->SetPromptL( *stringHolder );
    CleanupStack::PopAndDestroy( stringHolder );
    stringHolder = NULL;
    retval = ESyncMLDlgRespKeyNone;
    

    retval = dlg->ExecuteLD( R_FOTA_INSTALL_CONFIRMATION_QUERY );
	dlg = NULL;
	}	
	// set KDevManShowInstallNowQuery key back to value that it should show query
	TInt err = RProperty::Set(KPSUidNSmlDMSyncApp,KDevManShowInstallNowQuery,1 );
	FTRACE( FPrint(
	    _L("Install query is set back to be shown, err = %d"),
	    err ) );
	
    if ( !keypress && retval )
        {
        stringHolder = StringLoader::LoadLC( R_FOTA_UPDATE_1_WARNING );
                
        //keypress is EFalse here
        CSyncMLMessageQueryDialog* msgDlg = CSyncMLMessageQueryDialog::NewL(keypress);
        
        msgDlg->SetMessageTextL(*stringHolder);
        CleanupStack::PopAndDestroy( stringHolder );
        stringHolder = NULL;

        msgDlg->PrepareLC( R_SML_MESSAGE_QUERY );  // Pushed dialog is popped inside RunLD
        msgDlg->SetHeaderTextL( headerText->Des() );
        
       	msgDlg->ButtonGroupContainer().SetCommandSetL( R_SML_SOFTKEYS_ACCEPT_CANCEL__ACCEPT );
        
        retval = ESyncMLDlgRespKeyNone;;
        retval = msgDlg->RunLD();
        msgDlg = NULL;
		
		
		if ( !keypress && retval )
			{
	        stringHolder = StringLoader::LoadLC( R_FOTA_UPDATE_2_WARNING );
	        
	        //keypress is EFalse here
	        msgDlg = CSyncMLMessageQueryDialog::NewL(keypress);
            msgDlg->SetMessageTextL( *stringHolder );

	        CleanupStack::PopAndDestroy( stringHolder );
	        stringHolder = NULL;

	        msgDlg->PrepareLC( R_SML_MESSAGE_QUERY );  // Pushed dialog is popped inside RunLD
	        msgDlg->SetHeaderTextL( headerText->Des() );
	        
        	msgDlg->ButtonGroupContainer().SetCommandSetL( 
	        						R_SML_SOFTKEYS_ACCEPT_CANCEL__ACCEPT );
        
	        retval = ESyncMLDlgRespKeyNone;
	        retval = msgDlg->RunLD();
	        msgDlg = NULL;
			}
        }
        }
    

    CleanupStack::PopAndDestroy( headerText );
    
    FTRACE( FPrint( _L(
        "[SmlNotif]\t CSyncMLFwUpdNotifier::ShowUpdateStartQueryL() completed: retval = %d, keypress = %d" ),
        retval,keypress ) );

	//call-termination key press is handled here.
	if (keypress)
    	retval = ESyncMLDlgRespKeyOthers;
	
    return retval;
    }


// -----------------------------------------------------------------------------
// CSyncMLFwUpdNotifier::ShowUpdateStartQueryEncryptionL
// -----------------------------------------------------------------------------
//
TInt CSyncMLFwUpdNotifier::ShowUpdateStartQueryEncryptionL()
    {
    FTRACE( FPrint( _L(
        "[SmlNotif]\t CSyncMLFwUpdNotifier::ShowUpdateStartQueryEncryptionL() profileId = %d" ),
        iNoteInfo ) );

    HBufC* headerText = StringLoader::LoadLC( R_FOTA_TITLE_PHONE_UPDATES );
    TInt retval (ESyncMLDlgRespKeyNone);  
    TBool keypress(EFalse);

    CSyncMLMessageQueryDialog* msgDlg = CSyncMLMessageQueryDialog::NewL(keypress); 
    HBufC* stringHolder = StringLoader::LoadLC (R_FOTA_UPDATE_DEC_WARNING);     
    msgDlg->SetMessageTextL(*stringHolder);
    CleanupStack::PopAndDestroy( stringHolder );
   
    msgDlg->PrepareLC( R_SML_MESSAGE_QUERY );  // Pushed dialog is popped inside RunLD
    msgDlg->SetHeaderTextL( headerText->Des() );
    msgDlg->ButtonGroupContainer().SetCommandSetL( R_SML_SOFTKEYS_ACCEPT_CANCEL__ACCEPT );
    retval = ESyncMLDlgRespKeyNone;
    retval = msgDlg->RunLD();
    msgDlg = NULL;
    
    CleanupStack::PopAndDestroy(headerText);
    FLOG(_L("[SmlNotif]\t CSyncMLFwUpdNotifier::ShowUpdateStartQueryL() after new changes in phone updates"));
    return retval;
    }

// -----------------------------------------------------------------------------
// CSyncMLFwUpdNotifier::ShowUpdateResultNoteL
// -----------------------------------------------------------------------------
//
TInt CSyncMLFwUpdNotifier::ShowUpdateResultWithEncryptionNoteL()
    {
    FTRACE( FPrint( _L(
        "[SmlNotif]\t CSyncMLFwUpdNotifier::ShowUpdateResultWithEncryptionNoteL() result = %d" ),
        iNoteInfo ) );

    HBufC* firstStr( NULL );

    switch ( iNoteInfo )  // It is easier to add cases to switch structure
        {
        case KErrNone:
            {
            firstStr = StringLoader::LoadLC( R_FOTA_NOTIF_COMPLETE );
            break;
            }
        default:
            {
            firstStr = StringLoader::LoadLC( R_FOTA_MSG_QUERY_NOT_UPDATED );
            break;
            }
        }
    
    HBufC* secStr  = StringLoader::LoadLC(R_FOTA_UPDATE_ENC);
    
    HBufC* stringHolder = HBufC::NewLC(firstStr->Length() + secStr->Length());
    TPtr ptr (stringHolder->Des());
    ptr.Append(firstStr->Des());
    ptr.Append(secStr->Des());
    
    TBool keypress(EFalse);
    CSyncMLMessageQueryDialog* msgDlg = CSyncMLMessageQueryDialog::NewL(keypress);
    msgDlg->SetMessageTextL(*stringHolder);
    CleanupStack::PopAndDestroy( stringHolder );
    CleanupStack::PopAndDestroy(secStr);
    CleanupStack::PopAndDestroy(firstStr);

    
    msgDlg->PrepareLC( R_SML_MESSAGE_QUERY );  // Pushed dialog is popped inside RunLD

    stringHolder = StringLoader::LoadLC( R_FOTA_TITLE_PHONE_UPDATES );
    msgDlg->SetHeaderTextL( stringHolder->Des() );
    CleanupStack::PopAndDestroy( stringHolder );
    stringHolder = NULL;
    
    msgDlg->ButtonGroupContainer().SetCommandSetL( R_SML_SOFTKEYS_ACCEPT_CANCEL__ACCEPT );
    TInt retval(ESyncMLDlgRespKeyNone);
    iRunLhang = ETrue;
    iDlg = msgDlg; 
    retval = msgDlg->RunLD();
    iRunLhang = EFalse;
    msgDlg = NULL;
    iDlg = NULL;
    //call-termination key press has to be suppressed here. so keypress has no significance here!    
    /*if (keypress)
        retval = EKeyOthers;*/
        
    FTRACE( FPrint( _L(
        "[SmlNotif]\t CSyncMLFwUpdNotifier::ShowUpdateResultWithEncryptionNoteL() completed: retval = %d" ),
        retval ) );
    
    return retval;
    }


// -----------------------------------------------------------------------------
// CSyncMLFwUpdNotifier::RetrieveProfileNameL
// -----------------------------------------------------------------------------
//
void CSyncMLFwUpdNotifier::RetrieveProfileNameL( TDes& aServerName )
    {
    FLOG(_L("[SmlNotif]\t CSyncMLFwUpdNotifier::RetrieveProfileNameL()"));

    RSyncMLSession syncSession;
    RSyncMLDevManProfile syncProfile;
  
    syncSession.OpenL();
    CleanupClosePushL( syncSession );
    
    syncProfile.OpenL( syncSession, iNoteInfo, ESmlOpenRead );
    CleanupClosePushL( syncProfile );
    
    aServerName = syncProfile.DisplayName();

    CleanupStack::PopAndDestroy( &syncProfile );
    CleanupStack::PopAndDestroy( &syncSession );

    FTRACE( FPrint( _L(
        "[SmlNotif]\t CSyncMLFwUpdNotifier::RetrieveProfileNameL() completed: aServerName = %S" ),
        &aServerName ) );
    }

// -----------------------------------------------------------------------------
// CSyncMLFwUpdNotifier::ShowNoReminderQuery
// -----------------------------------------------------------------------------
//

TInt CSyncMLFwUpdNotifier::ShowNoReminderQueryL()
	{

    FLOG( _L(
        "[SmlNotif]\t CSyncMLFwUpdNotifier::ShowNoReminderQueryL() begin "));
        
   	TInt configFlags( 0 );
    CRepository* cenrep = NULL;
    cenrep = CRepository::NewL( KCRUidNSmlDMSyncApp );
    cenrep->Get( KNSmlDMFotaConfigurationKey, configFlags );
    delete cenrep;
    cenrep = NULL;
	TInt retval (KErrNone);
	
    // Show note if fota is not hidden
    if ( configFlags & KNSmlDMFotaEnableFotaViewFlag )
    	{
    	HBufC* headerText = StringLoader::LoadLC( R_FOTA_TITLE_PHONE_UPDATES );
    	HBufC* stringHolder = StringLoader::LoadLC( R_FOTA_UPDATE_REMINDER );
        
    	TBool keypress(EFalse);        
        CSyncMLMessageQueryDialog* msgDlg = CSyncMLMessageQueryDialog::NewL(keypress);
        msgDlg->SetMessageTextL(*stringHolder);
        CleanupStack::PopAndDestroy( stringHolder );
        stringHolder = NULL;

        msgDlg->PrepareLC( R_SML_MESSAGE_QUERY );  // Pushed dialog is popped inside RunLD
        msgDlg->SetHeaderTextL( headerText->Des() );
            
        msgDlg->ButtonGroupContainer().SetCommandSetL( R_AVKON_SOFTKEYS_OK_EMPTY__OK );
        retval = msgDlg->RunLD();
        msgDlg = NULL;

		//call-termination key press is handled here.
		if (keypress)
    		retval = ESyncMLDlgRespKeyOthers;
		
        CleanupStack::PopAndDestroy( headerText );
    	}
        
     FLOG( _L(
        "[SmlNotif]\t CSyncMLFwUpdNotifier::ShowNoReminderQueryL() Completed "));   
     
     return retval;
	}

// -----------------------------------------------------------------------------
// CSyncMLFwUpdNotifier::ShowNotEnoughMemory
// -----------------------------------------------------------------------------
//
TInt CSyncMLFwUpdNotifier::ShowNotEnoughMemoryL( TInt &aMemoryNeeded , TBool aOmaDL)
{
  	FLOG( _L(
        "[SmlNotif]\t CSyncMLFwUpdNotifier::ShowNotEnoughMemoryL() begin "));
        
  	HBufC* headerText = StringLoader::LoadLC( R_FOTA_TITLE_PHONE_UPDATES );  
  	
  	HBufC* stringHolder;
  	
  	if( aOmaDL )	
  	stringHolder = StringLoader::LoadLC( R_FOTA_OMADL_NO_FLEXIBLE_MEMORY , aMemoryNeeded );
  	else
  	stringHolder = StringLoader::LoadLC( R_FOTA_LARGE_NO_FLEXIBLE_MEMORY , aMemoryNeeded );
    
    TBool keypress(EFalse);  	
    CSyncMLMessageQueryDialog* msgDlg = CSyncMLMessageQueryDialog::NewL(keypress);
	msgDlg->SetMessageTextL(*stringHolder);
  	CleanupStack::PopAndDestroy( stringHolder );
  	stringHolder = NULL;
  
  	msgDlg->PrepareLC( R_SML_MESSAGE_QUERY );  // Pushed dialog is popped inside RunLD
  	msgDlg->SetHeaderTextL( headerText->Des() );
           
  	msgDlg->ButtonGroupContainer().SetCommandSetL( R_AVKON_SOFTKEYS_OK_EMPTY__OK );
  	TInt retval (ESyncMLDlgRespKeyNone);
  	retval = msgDlg->RunLD();
  	msgDlg = NULL;
  	CleanupStack::PopAndDestroy( headerText );
  
  	FLOG( _L(
        "[SmlNotif]\t CSyncMLFwUpdNotifier::ShowNotEnoughMemoryL() Completed "));    	
	
	//call-termination key press is handled here.
	if (keypress)
    	retval = ESyncMLDlgRespKeyOthers;
	
	return retval;
}
// -----------------------------------------------------------------------------
// CSyncMLFwUpdNotifier::ShowDownloadUserCancelL
// Shows the notification when the user cancels the download.
// And inform the user the download is suspended.
// -----------------------------------------------------------------------------
//
TInt CSyncMLFwUpdNotifier::ShowDownloadUserCancelL()
{

    FLOG( _L(
        "[SmlNotif]\t CSyncMLFwUpdNotifier::ShowDownloadUserCancelL() begin "));
        
  	TInt retval (KErrNone);
	
    HBufC* headerText = StringLoader::LoadLC( R_FOTA_TITLE_PHONE_UPDATES );
    HBufC* stringHolder = StringLoader::LoadLC( R_FOTA_USER_CANCEL );
        
    TBool endKeyPress(EFalse);        
    CSyncMLMessageQueryDialog* msgDlg = CSyncMLMessageQueryDialog::NewL(endKeyPress);
    msgDlg->SetMessageTextL(*stringHolder);
    CleanupStack::PopAndDestroy( stringHolder );
    stringHolder = NULL;

    msgDlg->PrepareLC( R_SML_MESSAGE_QUERY );  // Pushed dialog is popped inside RunLD
    msgDlg->SetHeaderTextL( headerText->Des() );
            
    msgDlg->ButtonGroupContainer().SetCommandSetL( R_AVKON_SOFTKEYS_YES_NO__YES );
    iRunLhang = ETrue;
    iDlg = msgDlg;
    retval = msgDlg->RunLD();
    iRunLhang = EFalse;
    msgDlg = NULL;    
    iDlg = NULL;
	//call-termination key press is handled here.
	if (endKeyPress)
    	retval = ESyncMLDlgRespKeyOthers;
		
    CleanupStack::PopAndDestroy( headerText );
        
     FLOG( _L(
        "[SmlNotif]\t CSyncMLFwUpdNotifier::ShowDownloadUserCancelL() Completed "));   
     
     return retval;
}
// -----------------------------------------------------------------------------
// CSyncMLFwUpdNotifier::ShowDownloadPostponedL
// Shows the notification that the download is postponed.
// -----------------------------------------------------------------------------
//
TInt CSyncMLFwUpdNotifier::ShowDownloadPostponedL()
{

    FLOG( _L(
        "[SmlNotif]\t CSyncMLFwUpdNotifier::ShowDownloadPostponedL() begin "));
    
    TInt retval (KErrNone);
    
    HBufC* stringHolder = StringLoader::LoadLC( R_FOTA_DOWNLOAD_POSTPONED );
    CAknInformationNote* infoNote = new (ELeave) CAknInformationNote;

    infoNote->ExecuteLD( *stringHolder );
    CleanupStack::PopAndDestroy( stringHolder );
  	
    FLOG( _L(
        "[SmlNotif]\t CSyncMLFwUpdNotifier::ShowDownloadPostponedL() Completed "));   
     
    return retval;
}
// -----------------------------------------------------------------------------
// CSyncMLFwUpdNotifier::ShowDownloadCancelledL
// Shows the notification that the download is cancelled.
// -----------------------------------------------------------------------------
//
TInt CSyncMLFwUpdNotifier::ShowDownloadCancelledL()
{

    FLOG( _L(
        "[SmlNotif]\t CSyncMLFwUpdNotifier::ShowDownloadCancelledL() begin "));
        
  	TInt retval (KErrNone);
	
    HBufC* stringHolder = StringLoader::LoadLC( R_FOTA_DOWNLOAD_CANCELLED );
    CAknInformationNote* infoNote = new (ELeave) CAknInformationNote;

    infoNote->ExecuteLD( *stringHolder );
    CleanupStack::PopAndDestroy( stringHolder );
        
    FLOG( _L(
        "[SmlNotif]\t CSyncMLFwUpdNotifier::ShowDownloadCancelledL() Completed "));   
     
    return retval;
}


// -----------------------------------------------------------------------------
// CSyncMLFwUpdNotifier::ShowPostponeLimitNoteL
// Shows the notification that the download is Postponed.
// -----------------------------------------------------------------------------
//
TInt CSyncMLFwUpdNotifier::ShowPostponeLimitNoteL()
{

    FLOG( _L(
        "[SmlNotif]\t CSyncMLFwUpdNotifier::ShowPostponeLimitNoteL() begin "));

    TInt retval (KErrNone);
    const TInt KTmpArraySize = 5;
		//const TInt KStrArraySize = 20;
    
    CDesCArrayFlat* aStrArr = new CDesCArrayFlat(KTmpArraySize); 
    CleanupStack::PushL(aStrArr);
    aStrArr->AppendL(iSelectedReminder);
    CArrayFixFlat<TInt>* aIntparam = new(ELeave) CArrayFixFlat<TInt>(KTmpArraySize);
    CleanupStack::PushL(aIntparam);
    TInt leftPostpone =  iMaxPostponeCount - iUserPostponeCount;
    if(iInterval > 1)
        aIntparam->AppendL(iInterval);
    aIntparam->AppendL(leftPostpone);
    aIntparam->AppendL(iMaxPostponeCount);
    
    HBufC* stringHolder( NULL );
    //stringHolder = StringLoader::LoadLC( R_FOTA_POSTPONE_UPDATE, *aStrArr, *aIntparam);
    if(iIntervalType == EMonthly)
    {
    	if(iInterval > 1)
    	    stringHolder = StringLoader::LoadLC( R_FOTA_POSTPONE_UPDATE_MINUTES, *aIntparam);
    	else
    	    stringHolder = StringLoader::LoadLC( R_FOTA_POSTPONE_UPDATE_ONE_MINUTE, *aIntparam);
  	}
  	else if(iIntervalType == EHourly)
    {
    	if(iInterval > 1)
    	    stringHolder = StringLoader::LoadLC( R_FOTA_POSTPONE_UPDATE_HOURS, *aIntparam);
    	else
    	    stringHolder = StringLoader::LoadLC( R_FOTA_POSTPONE_UPDATE_ONE_HOUR, *aIntparam);
    		
    }
    else if(iIntervalType == EDaily)
    {
    	if(iInterval > 1)
    	    stringHolder = StringLoader::LoadLC( R_FOTA_POSTPONE_UPDATE_DAYS, *aIntparam);
    	else
    	    stringHolder = StringLoader::LoadLC( R_FOTA_POSTPONE_UPDATE_ONE_DAY, *aIntparam);
    		
  	}
    CAknInformationNote* infoNote = new (ELeave) CAknInformationNote;

    infoNote->ExecuteLD( *stringHolder );

    
    CleanupStack::PopAndDestroy();
    CleanupStack::PopAndDestroy( aIntparam );
    CleanupStack::PopAndDestroy(aStrArr);
        
    FLOG( _L(
        "[SmlNotif]\t CSyncMLFwUpdNotifier::ShowPostponeLimitNoteL() Completed "));   
     
    return retval;
}



// -----------------------------------------------------------------------------
// CSyncMLFwUpdNotifier::ShowDownloadResumeL
// Shows the notification to the user for resuming the download.
// -----------------------------------------------------------------------------
//
TInt CSyncMLFwUpdNotifier::ShowDownloadResumeL()
{

    FLOG( _L(
        "[SmlNotif]\t CSyncMLFwUpdNotifier::ShowDownloadResumeL() begin "));
        
  	TInt retval (KErrNone);
	
    HBufC* headerText = StringLoader::LoadLC( R_FOTA_TITLE_PHONE_UPDATES );
    HBufC* stringHolder = StringLoader::LoadLC( R_FOTA_DOWNLOAD_RESUME );
        
    TBool endKeyPress(EFalse);        
    CSyncMLMessageQueryDialog* msgDlg = CSyncMLMessageQueryDialog::NewL(endKeyPress);
    msgDlg->SetMessageTextL(*stringHolder);
    CleanupStack::PopAndDestroy( stringHolder );
    stringHolder = NULL;

    msgDlg->PrepareLC( R_SML_MESSAGE_QUERY );  // Pushed dialog is popped inside RunLD
    msgDlg->SetHeaderTextL( headerText->Des() );
            
    msgDlg->ButtonGroupContainer().SetCommandSetL( R_AVKON_SOFTKEYS_YES_NO__YES );
    iRunLhang = ETrue;
    iDlg = msgDlg;
    retval = msgDlg->RunLD();
    iRunLhang = EFalse;
    iDlg = NULL;
    msgDlg = NULL;
    
	//call-termination key press is handled here.
	if (endKeyPress)
    	retval = ESyncMLDlgRespKeyOthers;
		
    CleanupStack::PopAndDestroy( headerText );
        
     FLOG( _L(
        "[SmlNotif]\t CSyncMLFwUpdNotifier::ShowDownloadResumeL() Completed "));   
     
     return retval;
}

// -----------------------------------------------------------------------------
// CSyncMLFwUpdNotifier::ShowDownloadResumeL
// Shows the notification to the user for resuming the download.
// -----------------------------------------------------------------------------
//
TBool CSyncMLFwUpdNotifier::IsLanguageSupportedL()
{
	TBool retVal(EFalse);
	TLanguage language = User::Language();
	if(language == ELangEnglish || language == ELangInternationalEnglish ||
 		   language == ELangTaiwanChinese ||language == ELangHongKongChinese || language == ELangPrcChinese)
 		   retVal = ETrue;
 	return retVal;
}

//  End of File  
