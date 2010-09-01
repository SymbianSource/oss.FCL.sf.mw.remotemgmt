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
* Description:  Methods to parse the server alert messages for data sync and device
*                Management frameworks. Launches respective application 
*                in appropriate conditions
*
*/



// INCLUDE FILES
#include <eikenv.h>          // Eikon environment
#include <AknQueryDialog.h>
#include <StringLoader.h>    // Localisation stringloader
#include <SyncMLClientDS.h>
#include <SyncMLClientDM.h>
#include <SyncMLClient.h>
#include <rconnmon.h>
#include <ecom/ecom.h>
#include <centralrepository.h>
#include <SyncMLNotifierDomainCRKeys.h>
#include <SyncMLNotifier.rsg>           // Own resources
#include "SyncMLAppLaunchNotifier.h"    // Class declaration
#include "SyncMLTimedMessageQuery.h"
#include "SyncMLTimedQueryDialog.h"
#include "SyncMLNotifDebug.h"
#include <aknnotewrappers.h>
#include <DevManInternalCRKeys.h>
#include "nsmlconstantdefs.h"
#include "CPreSyncPlugin.h"
#include "SyncMLPreSyncPluginInterface.h"

// CONSTANTS
_LIT( KSmlNPanicCategory, "SyncMLNotifier");

// medium type uids
//const TUid KUidNSmlMediumTypeInternet  = { 0x101F99F0 };
const TUid KUidNSmlMediumTypeBluetooth = { 0x101F99F1 };
const TUid KUidNSmlMediumTypeUSB       = { 0x101F99F2 };
const TUid KUidNSmlMediumTypeIrDA      = { 0x101F99F3 };
const TUid KUidSmlSyncApp              = { 0x101F6DE5 };


enum TASpBearerType
    {
    EAspBearerInternet = 0,
    EAspBearerUsb = 1,
    EAspBearerBlueTooth = 2,
    EAspBearerIrda = 3,
    EAspBearerWsp = 4,
    EAspBearerSmlUsb = 5,
    EAspBearerLast = 6
    };

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CSyncMLAppLaunchNotifier::CSyncMLAppLaunchNotifier
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CSyncMLAppLaunchNotifier::CSyncMLAppLaunchNotifier() :
    iAlwaysAsk( EFalse )
    {
    iBearerType = KErrNotFound;
    }

// -----------------------------------------------------------------------------
// CSyncMLDSNotifier::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CSyncMLAppLaunchNotifier* CSyncMLAppLaunchNotifier::NewL()
    {
    FLOG(_L("[SmlNotif]\t CSyncMLAppLaunchNotifier::NewL()"));
    CSyncMLAppLaunchNotifier* self = new (ELeave) CSyncMLAppLaunchNotifier();
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    FLOG(_L("[SmlNotif]\t CSyncMLAppLaunchNotifier::NewL() completed"));
    return self;
    }

    
// -----------------------------------------------------------------------------
// Destructor
// -----------------------------------------------------------------------------
//
CSyncMLAppLaunchNotifier::~CSyncMLAppLaunchNotifier()
    {
	delete iObserver;
    delete iDMSyncService;
    delete iDSSyncService;
    Cancel();   // Free own resources
    }

// -----------------------------------------------------------------------------
// CSyncMLAppLaunchNotifier::RetrieveSyncParams
// Leaves if the job identifier is not found.
// -----------------------------------------------------------------------------
//
void CSyncMLAppLaunchNotifier::RetrieveSyncParamsL(
    TSyncServiceParam& aParam,
    TDes& aServerName,
    TSmlServerAlertedAction& aUserInteraction )
    {
    FLOG(_L("[SmlNotif]\t CSyncMLAppLaunchNotifier::RetrieveSyncParamsL()"));
    TSmlJobId currentJobId;
    TSmlUsageType usageType;
    
    // sync session
    RSyncMLSession syncSession;
    syncSession.OpenL();
    CleanupClosePushL( syncSession );
    syncSession.CurrentJobL( currentJobId, usageType );

    if ( currentJobId == iJobId &&
         usageType == ESmlDataSync )
        {
        RSyncMLDataSyncProfile syncProfile;
        syncProfile.OpenL( syncSession, iProfileId, ESmlOpenRead );
        CleanupClosePushL( syncProfile );
        
        aParam.iProfileId = iProfileId;
        aParam.iJobId = iJobId;
        aServerName = syncProfile.DisplayName();
        aUserInteraction = syncProfile.SanUserInteraction();
        
        // Check if always ask is selected as accesspoint
        RSyncMLConnection connection;
        TRAPD( iapError, connection.OpenL( syncProfile, KUidNSmlMediumTypeInternet.iUid ) );
        CleanupClosePushL( connection );

        if ( !iapError )
            {
            const TDesC8& iap = connection.GetPropertyL( KNSmlIAPId );
            if ( iap.Compare( KNSmlAlwaysAsk() ) == 0 )
                {
                iAlwaysAsk = ETrue;
                }            
            
            TInt id = connection.Identifier();
            if (id == KUidNSmlMediumTypeInternet.iUid)
                {
                iBearerType = EAspBearerInternet;
                }
            else if (id == KUidNSmlMediumTypeBluetooth.iUid)
                {
                iBearerType = EAspBearerBlueTooth;
                }
            else if (id == KUidNSmlMediumTypeUSB.iUid)
                {
                iBearerType = EAspBearerUsb;
                }
            else if (id == KUidNSmlMediumTypeIrDA.iUid)
                {
                iBearerType = EAspBearerIrda;
                }
            else
                {
                iBearerType = KErrNotFound;
                }            
            }

        connection.Close();
        CleanupStack::Pop( &connection );
        
        syncProfile.Close();
        CleanupStack::Pop( &syncProfile );
        }
    else
        {
        FTRACE( FPrint(
            _L("[SmlNotif]\t CSyncMLAppLaunchNotifier::RetrieveSyncParamsL() Could not find job %d"),
            iJobId ) );
        User::Leave( KErrNotFound );
        }
    
    syncSession.Close();
    CleanupStack::Pop( &syncSession );

    FTRACE( FPrint(
        _L("[SmlNotif]\t CSyncMLAppLaunchNotifier::RetrieveSyncParamsL() completed, aServerName = \"%S\""),
        &aServerName ) );
    }

// -----------------------------------------------------------------------------
// CSyncMLAppLaunchNotifier::RetrieveMgmtParamsL
// Leaves if the job identifier is not found.
// -----------------------------------------------------------------------------
//
void CSyncMLAppLaunchNotifier::RetrieveMgmtParamsL(
    TSyncServiceParam& aParam,
    TDes& aServerName,
    TSmlServerAlertedAction& aUserInteraction )
    {
    FLOG(_L("[SmlNotif]\t CSyncMLAppLaunchNotifier::RetrieveMgmtParamsL()"));

    TSmlJobId currentJobId;
    TSmlUsageType usageType;
    
    RSyncMLSession syncSession;
    
    syncSession.OpenL();
    CleanupClosePushL( syncSession );
    
    syncSession.CurrentJobL( currentJobId, usageType );
    
    if ( currentJobId == iJobId &&
         usageType == ESmlDevMan )
        {
        RSyncMLDevManProfile syncProfile;
        syncProfile.OpenL( syncSession, iProfileId, ESmlOpenRead );
        CleanupClosePushL( syncProfile );
        
        aParam.iProfileId = iProfileId;
        aParam.iJobId = iJobId;
        aServerName = syncProfile.DisplayName();
        aUserInteraction = syncProfile.SanUserInteraction();
        syncProfile.Close();
        CleanupStack::Pop( &syncProfile );
        }
    else
        {
        FTRACE( FPrint(
            _L("[SmlNotif]\t CSyncMLAppLaunchNotifier::RetrieveMgmtParamsL() Could not find job %d"),
            iJobId ) );
        User::Leave( KErrNotFound );
        }
    
    syncSession.Close();
    CleanupStack::Pop( &syncSession );

    FTRACE( FPrint(
        _L("[SmlNotif]\t CSyncMLAppLaunchNotifier::RetrieveMgmtParamsL() completed, aServerName = \"%S\""),
        &aServerName ) );
    }

// -----------------------------------------------------------------------------
// CSyncMLAppLaunchNotifier::IsSilent
// Only automatic acception from the profile is checked, since notifier has no
// information about the preferences included in the server alert.
// -----------------------------------------------------------------------------
//
TBool CSyncMLAppLaunchNotifier::IsSilent(
    TSmlServerAlertedAction& aUserInteraction )
    {
    TBool retval = EFalse;
    if ( aUserInteraction == ESmlEnableSync )
        {
        retval = ETrue;
        }
    return retval;
    }
    
// -----------------------------------------------------------------------------
// CSyncMLAppLaunchNotifier::RegisterL
// -----------------------------------------------------------------------------
//
CSyncMLAppLaunchNotifier::TNotifierInfo CSyncMLAppLaunchNotifier::RegisterL()
    {
    FLOG(_L("[SmlNotif]\t CSyncMLAppLaunchNotifier::RegisterL()"));
    iInfo.iUid = KSyncMLAppLaunchNotifierUid;
    iInfo.iChannel = KSmlAppLaunchChannel;
    iInfo.iPriority = ENotifierPriorityVHigh;
    FLOG(_L("[SmlNotif]\t CSyncMLAppLaunchNotifier::RegisterL() completed"));
    return iInfo;
    }

// -----------------------------------------------------------------------------
// CSyncMLAppLaunchNotifier::Cancel
// Release all own resources (member variables)
// -----------------------------------------------------------------------------
//
void CSyncMLAppLaunchNotifier::Cancel()
    {
    FLOG(_L("[SmlNotif]\t CSyncMLAppLaunchNotifier::Cancel()"));

    CSyncMLNotifierBase::Cancel();

    FLOG(_L("[SmlNotif]\t CSyncMLAppLaunchNotifier::Cancel() completed"));
    }

// -----------------------------------------------------------------------------
// CSyncMLAppLaunchNotifier::GetParamsL
// Initialize parameters and check if device is already
// in registry. Jump to RunL as soon as possible.
// -----------------------------------------------------------------------------
//
void CSyncMLAppLaunchNotifier::GetParamsL( const TDesC8& aBuffer,
                                                 TInt aReplySlot,
                                           const RMessagePtr2& aMessage )
    {
    FLOG(_L("[SmlNotif]\t CSyncMLAppLaunchNotifier::GetParamsL()"));

    if( iReplySlot != NULL || iNeedToCompleteMessage )
        {
        User::Leave( KErrInUse );
        }
    
    iMessage = aMessage;
    iNeedToCompleteMessage = ETrue;
    iReplySlot = aReplySlot;
    iAlwaysAsk = EFalse;

	TSyncMLAppLaunchNotifParams param;
 	TPckgC<TSyncMLAppLaunchNotifParams> pckg( param );
 	pckg.Set(aBuffer);

    iSmlProtocol = pckg().iSessionType;
    iJobId = pckg().iJobId;
    iProfileId = pckg().iProfileId;
 TInt SanSupport( KErrNone );
CRepository* centrep = NULL;
    TRAPD( err, centrep = CRepository::NewL( KCRUidDeviceManagementInternalKeys) );    
    User::LeaveIfError( err );
    centrep->Get( KDevManSANUIBitVariation, SanSupport );
    delete centrep;
	if( SanSupport == EON )
   	{
     iUimode = pckg().iUimode;
   	}    
    // Call SetActive() so RunL() will be called by the active scheduler
    //
    SetActive();
    iStatus = KRequestPending;
    TRequestStatus* stat = &iStatus;
    User::RequestComplete( stat, KErrNone );

    FTRACE( FPrint(
        _L("[SmlNotif]\t CSyncMLAppLaunchNotifier::GetParamsL() completed, iSmlProtocol = %d, iJobId = %d, iProfileId = %d"),
        iSmlProtocol, iJobId, iProfileId ) );
    }

// -----------------------------------------------------------------------------
// CAspSyncHandler::ReadRepositoryL
//
// -----------------------------------------------------------------------------
//
void CSyncMLAppLaunchNotifier::ReadRepositoryL(TInt aKey, TInt& aValue)
    {
    CRepository* rep = CRepository::NewLC(KCRUidNSmlDSApp);
    TInt err = rep->Get(aKey, aValue);
    User::LeaveIfError(err);
    
    CleanupStack::PopAndDestroy(rep);
    }

void CSyncMLAppLaunchNotifier::ShowRoamingMessageL(TInt keypress, TUint profileId)
    {
    TBool roaming = EFalse;
    
    if( (keypress == EAknSoftkeyYes || keypress == EAknSoftkeyOk) &&  (iSmlProtocol == ESyncMLSyncSession) )
       {
       TBool bCanSync = ETrue;
       
       TInt aValue = 0;
       ReadRepositoryL(KNSmlDSRoamingFeature, aValue);
       IsRoamingL(roaming);
       if(( roaming ) && (aValue == EAspRoamingSettingFeatureEnabled))
           {                
              if (iBearerType == EAspBearerInternet)           
               {
               CPreSyncPluginInterface* syncPluginInterface = CPreSyncPluginInterface::NewL();
               CPreSyncPlugin* syncPlugin = 
                   syncPluginInterface->InstantiateRoamingPluginLC(profileId);

               // Turn lights on and deactivate apps -key
               TurnLightsOn();  
               SuppressAppSwitching( ETrue );

               if(syncPlugin->IsSupported())
                   {
                   bCanSync = syncPlugin->CanSyncL();
                   }

               SuppressAppSwitching( EFalse );
               
               CleanupStack::PopAndDestroy(syncPlugin);
               //syncPluginInterface->UnloadPlugIns();
               delete  syncPluginInterface;   

               if(!bCanSync)
                   {
                   iNeedToCompleteMessage=EFalse;
                   iReplySlot = NULL;
                   iMessage.Complete( KErrCancel );
                   return;
                   }
                   
               }                       
           }            
       }
    }
// -----------------------------------------------------------------------------
// CSyncMLAppLaunchNotifier::RunL
// Ask user response and return it to caller.
// -----------------------------------------------------------------------------
//
void CSyncMLAppLaunchNotifier::RunL()
    {
    FLOG(_L("[SmlNotif]\t CSyncMLAppLaunchNotifier::RunL()"));
		stringholder =NULL;
		
    TLanguage language = User::Language();  
    // Load the parameters and set the query text according to the session type.
    switch( iSmlProtocol )
        {
        case ESyncMLSyncSession:
            {
            FLOG( _L("[SmlNotif]\t CSyncMLAppLaunchNotifier::RunL() Synchronisation session") );

            TBuf<KSyncMLMaxProfileNameLength> serverName;
            RetrieveSyncParamsL( param, serverName, uiAction );
            stringholder = StringLoader::LoadL( R_SML_INIT_DS_SERVER_PROMPT,
                                                serverName );
            if(!stringholder)
                return;
            }
			break;
        case ESyncMLMgmtSession:
            {
            	TBool status = HandleDMSessionL();
            	if( !status )
            		return;
            }
            break;
        default: // Unknown value
            {
            FLOG(_L("[SmlNotif]\t CSyncMLAppLaunchNotifier::RunL() unknown protocol value"));
            User::Leave( KErrArgument ); // Leave handling completes the message
            return;
            }
        }

    CleanupStack::PushL( stringholder );
    	
   	centrep = NULL;
    TRAPD( err, centrep = CRepository::NewL( KCRUidDeviceManagementInternalKeys) );    
    if(err)
    {
    	centrep = NULL;
    }

    TInt keypress( 0 );
    TBool  silent = EFalse;
		TInt SanSupport( KErrNone ),Timeout( KErrNone),CustomNotes(KErrNone);;
    if(centrep)
    {centrep->Get( KDevManSANUIBitVariation, SanSupport );
     centrep->Get( KDevManServerAlertTimeout, Timeout );
     centrep->Get( KDevManUINotesCustomization, CustomNotes);	
    }
    else
    {
    	  SanSupport = KErrNone;
        Timeout = KDefaultTimeoutforNotes; 
        CustomNotes = KErrNone ;
    }   
    Timeout = Timeout * 60 ;// converting to mins
    delete centrep;
		if(( SanSupport == EON )&& ( iSmlProtocol == ESyncMLMgmtSession))
   	{
    if( iUimode > ESANUserInteractive && iUimode < ESANNotSpecified )
    	{
    	iUimode = ESANNotSpecified;
    	}
    switch( iUimode ) 	
    	{
    		case ESANNotSpecified://not specified
    		case ESANUserInteractive://user interactive session
    		      silent = EFalse;    		      
    		      break;
    		case ESANSilent://Background
    		case ESANUserInformative://Informative session
    		      silent =  ETrue;//IsSilent( uiAction );     		      
    		      break;  
    	}
   }
   else
       {
       	silent =  IsSilent( uiAction );
       }
    if ( iAlwaysAsk && iSmlProtocol == ESyncMLMgmtSession )	//check with synergy    
    	{
    	silent = EFalse;
    	}
    if ( !silent || iAlwaysAsk )
        {
        // Turn lights on and deactivate apps -key
        TurnLightsOn();  
        SuppressAppSwitching( ETrue );

        CSyncMLTimedQueryDialog* dlg =
                CSyncMLTimedQueryDialog::NewL( *stringholder,
                                               Timeout );    
            
        // Pushed dialog is popped inside RunLD
        dlg->PrepareLC( R_SML_CONFIRMATION_QUERY );
        
        	dlg->ButtonGroupContainer().SetCommandSetL(
        	        					R_AVKON_SOFTKEYS_YES_NO__YES );
        keypress = dlg->RunLD();
        dlg = NULL;
        // Activate apps -key again
        //
        SuppressAppSwitching( EFalse );
        }

    CleanupStack::PopAndDestroy( stringholder );
    
    ShowRoamingMessageL(keypress, param.iProfileId);

    if ( iNeedToCompleteMessage ) // Notifier is not cancelled
        {
        	HandleCompleteMessageL(keypress, silent, SanSupport, Timeout, CustomNotes);
        }

    iNeedToCompleteMessage = EFalse;
    iReplySlot = NULL;

    iSmlProtocol = ESyncMLUnknownSession;
    iJobId = 0;

    FLOG(_L("[SmlNotif]\t CSyncMLAppLaunchNotifier::RunL() completed"));
    }

// ----------------------------------------------------------------------------
// CSyncMLAppLaunchNotifier::RunError
// ----------------------------------------------------------------------------
TInt CSyncMLAppLaunchNotifier::RunError ( TInt aError )
    {
    FTRACE( FPrint(
								_L("[SmlNotif]\t CSyncMLAppLaunchNotifier::RunError() The Error occurred is   %d"), aError ) );
    return KErrNone;
    }	
    
// -----------------------------------------------------------------------------
// CSyncMLNotifierBase::SyncServiceL
// -----------------------------------------------------------------------------
//
CSyncService* CSyncMLAppLaunchNotifier::SyncServiceL( TUint aServiceId )
    {
    FLOG(_L("[SmlNotif]\t CSyncMLAppLaunchNotifier::SyncService"));

    if ( aServiceId == KDevManServiceStart )
        {
        if ( !iDMSyncService )
            {
            FLOG(_L("[SmlNotif]\t CSyncMLAppLaunchNotifier::SyncServiceL() DM NewL"));
            iDMSyncService = CSyncService::NewL( NULL, KDevManServiceStart );
            }
        return iDMSyncService;
        }
    else
        {
        if ( !iDSSyncService )
            {
            FLOG(_L("[SmlNotif]\t CSyncMLAppLaunchNotifier::SyncServiceL() DS NewL"));
            iDSSyncService = CSyncService::NewL( NULL, KDataSyncServiceStart );
            }
        return iDSSyncService;
        }
    }
//------------------------------------------------------------------------------
//CSyncMLAppLaunchNotifier::IsRoaming(TBool& ret)
//Returns true if roaming otherwise returns false
 
//------------------------------------------------------------------------------

void CSyncMLAppLaunchNotifier::IsRoamingL(TBool &ret)
	{
	  FLOG(_L("[SmlNotif]\t CSyncMLAppLaunchNotifier::IsRoaming() Begin"));
		RConnectionMonitor monitor;
	  TRequestStatus status;
		// open RConnectionMonitor object
		monitor.ConnectL();
		CleanupClosePushL( monitor );
		TInt netwStatus(0);
		monitor.GetIntAttribute( EBearerIdGPRS, // See bearer ids from TConnMonBearerId
	                         0, 
	                         KNetworkRegistration,
	                         netwStatus, 
	                         status );
	  User::WaitForRequest( status );
	  if ( status.Int() == KErrNone )
	 	   {
				switch(netwStatus)
				{
					case ENetworkRegistrationRoaming :
					{
						FLOG(_L("[SmlNotif]\t CSyncMLAppLaunchNotifier::IsRoaming() ENetworkRegistrationRoaming"));
						ret=true;
						break;
					}
		   		default:
			 		{
			 			FLOG(_L("[SmlNotif]\t CSyncMLAppLaunchNotifier::IsRoaming() Roaming status "));
			 			FTRACE( FPrint(
					  _L("[SmlNotif]\t CSyncMLAppLaunchNotifier::IsRoamingL() cell is not in roaming, network status is  %d"), netwStatus ) );
			 			break;	
			 		}
			}
			CleanupStack::PopAndDestroy();
		}
		else
			{
				FTRACE( FPrint(
			  _L("[SmlNotif]\t CSyncMLAppLaunchNotifier::IsRoamingL() RConnectinMonitor status.Int() returned error %d"), status.Int() ) );
			}
	}
//------------------------------------------------------------------------------
//CSyncMLAppLaunchNotifier::HandleDMSessionL()
//Handles the DM Session 
//------------------------------------------------------------------------------

TBool CSyncMLAppLaunchNotifier::HandleDMSessionL()
{
		FLOG(_L("[SmlNotif]\t CSyncMLAppLaunchNotifier::HandleDMSessionL begins"));
		TBool ret(ETrue);
		TInt dmroamingBlock = 0;
    TBool roaming = EFalse;
    TLanguage language = User::Language();
	  FLOG( _L("[SmlNotif]\t CSyncMLAppLaunchNotifier::RunL() Management session") );
		CRepository* cRepository=NULL;
		TRAPD ( error, cRepository = CRepository::NewL ( KCRUidNSmlNotifierDomainKeys ) );
		if ( error == KErrNone )
		{
			CleanupStack::PushL( cRepository );
			cRepository->Get ( KNSmlDMBlockedInRoaming, dmroamingBlock );
			if(dmroamingBlock==1)
			{
				FLOG( _L("[SmlNotif]\t CSyncMLAppLaunchNotifier::RunL() roaming feature is enabled") );
				TRAPD(rError,IsRoamingL(roaming));
				if( (rError==KErrNone) && roaming )
				{
					FLOG( _L("[SmlNotif]\t CSyncMLAppLaunchNotifier::RunL() phone is in roaming, DM session is blocked ") );
					CleanupStack::PopAndDestroy( cRepository );//cRepository
					iNeedToCompleteMessage=EFalse;
					iReplySlot = NULL;
					ret = EFalse;
					return ret;
				}
				else
				{
					FTRACE( FPrint(
								_L("[SmlNotif]\t CSyncMLAppLaunchNotifier::RunL() error in getting Network status  %d"), rError ) );
          FTRACE( FPrint(
          			_L("[SmlNotif]\t CSyncMLAppLaunchNotifier::RunL() error in getting Network status  %d"),roaming ) );
									}
				}
				else
				{
					FLOG( _L("[SmlNotif]\t CSyncMLAppLaunchNotifier::RunL() DM blocking feature disabled ") );
				}
				CleanupStack::PopAndDestroy( cRepository );//cRepository
			}
			else
			{
				FTRACE( FPrint(
            _L("[SmlNotif]\t CSyncMLAppLaunchNotifier::RunL() cenrep creation error code  %d"),
            error ) );
			}
      TBuf<KSyncMLMaxProfileNameLength> serverName;
      RetrieveMgmtParamsL( param, DMSyncServerName, uiAction );            
      TInt CustomNotes(KErrNone), SanSupport( KErrNone );  
      CRepository* centrep = NULL;  
      TRAPD(err1, centrep = CRepository::NewL ( KCRUidDeviceManagementInternalKeys ) );
      if((centrep) && (err1 == KErrNone))
      {
      	centrep->Get( KDevManUINotesCustomization, CustomNotes);
        centrep->Get( KDevManSANUIBitVariation, SanSupport );
      }
      else
      {
      	CustomNotes = 0;
        SanSupport  = 0;	
      }
      delete centrep;             
                                                        
      if(CustomNotes && SanSupport && IsLanguageSupportedL())
      {
      	stringholder = StringLoader::LoadL( R_QTN_DM_QUERY_ACCEPT_POSTSALE);	//This note will be shown in chinese variant itself 
      }
      else
      { 
      	stringholder = StringLoader::LoadL( R_SML_INIT_DM_SERVER_PROMPT,DMSyncServerName );
      }      
      FLOG(_L("[SmlNotif]\t CSyncMLAppLaunchNotifier::HandleDMSessionL ends"));
      return ret;
}

//------------------------------------------------------------------------------
//CSyncMLAppLaunchNotifier::HandleCompleteMessageL(TInt &keypress, TBool &silent, TInt &SanSupport, TInt &Timeout, TInt &CustomNotes)
// Handle the complete message
//------------------------------------------------------------------------------
	
void CSyncMLAppLaunchNotifier::HandleCompleteMessageL(TInt &keypress, TBool &silent, TInt &SanSupport, TInt &Timeout, TInt &CustomNotes)
{
	TLanguage language = User::Language();
	TInt err = KErrNone;
	_LIT_SECURITY_POLICY_S0(KWritePolicy,KUidSmlSyncApp.iUid);
	_LIT_SECURITY_POLICY_C1( KReadPolicy, ECapabilityReadDeviceData );
	                  
	if( keypress == EAknSoftkeyYes || keypress == EAknSoftkeyOk || silent ) // User has accepted the dialog
  {
		TInt retval = 1; // Default for ESyncMLSyncSession or silent
	if( iSmlProtocol == ESyncMLMgmtSession && SanSupport == EON && silent && iUimode == ESANUserInformative )
	{
		// Turn lights on and deactivate apps -key
  	TurnLightsOn(); 	  
   	if(CustomNotes && IsLanguageSupportedL()) 
   	{
   		FLOG(_L("[SmlNotif]\t CSyncMLAppLaunchNotifier::informative session inside if "));
  		stringholder = StringLoader::LoadLC(R_QTN_DM_INFO_NOTE_POSTSALE);	 //This note will be shown in chinese variant only
   	}
   	else
   	{
    	stringholder = StringLoader::LoadLC(R_QTN_DM_INFO_NOTE_UPDATING , DMSyncServerName );
    }   
		CAknInformationNote* informationNote = new (ELeave) CAknInformationNote ();
		informationNote->ExecuteLD ( stringholder->Des() );
		informationNote = NULL;
		CleanupStack::PopAndDestroy( stringholder );	
   }

	if ( retval )
	{
		const TUid KUidFakeUID = { 0 };
	  TSyncMLAppLaunchNotifRetVal pckg;
	  pckg.iSecureId = KUidFakeUID;
	           
	  switch( iSmlProtocol )
	  {
	  	case ESyncMLSyncSession:
	    					param.iServiceId = KDataSyncServiceStart;
	             	break;
	    case ESyncMLMgmtSession:
                
                  //Define a property for Native Disclaimer 
                  
	              err = RProperty::Define(KDisclaimerProperty, KDisclaimerInteger, RProperty::EInt, KReadPolicy, KWritePolicy);
	              if (err != KErrAlreadyExists)
	                  {
                      User::LeaveIfError(err);
	                  }
	              param.iServiceId = KDevManServiceStart;
								if( SanSupport == EON )
								{													
	              	param.iSilent = iUimode;	                    	                               
								}
				iObserver = new (ELeave) CDMDisclaimerObserver();
				pckg.iSecureId = SyncServiceL( param.iServiceId )->StartSyncL( param );
				FLOG(_L("[SmlNotif]\t WaitonDisclaimerL called"));
				iObserver->WaitOnDisclaimerL(this);
	              break;
	    default:
	              // This branch should never be reached, since the option
	              // is handled in the previous switch statement.
	              User::Panic( KSmlNPanicCategory, KErrCorrupt );
	              break;
	  }	
	 iMessage.WriteL( iReplySlot, TPckgBuf<TSyncMLAppLaunchNotifRetVal>(pckg) );
	 if(iSmlProtocol == ESyncMLSyncSession)
	     {
         iMessage.Complete(KErrNone);

	     }
	 }
	 else
	 {
	  	// Complete the message with result code indicating that
		  // the user cancelled the query.
	    iMessage.Complete( KErrCancel );
	 }
  }
  else
  {
  	// Complete the message with result code indicating that
    // the user cancelled the query.
    iMessage.Complete( KErrCancel );
  }
}

// -----------------------------------------------------------------------------
// CSyncMLAppLaunchNotifier::IsLanguageSupportedL
// Returns True if the specified language is supported.
// -----------------------------------------------------------------------------
//
TBool CSyncMLAppLaunchNotifier::IsLanguageSupportedL()
{
	FLOG(_L("[SmlNotif]\t CSyncMLAppLaunchNotifier::IsLanguageSupportedL begins"));
	TBool retVal(EFalse);
	TLanguage language = User::Language();
	if(language == ELangEnglish || language == ELangInternationalEnglish ||
 		   language == ELangTaiwanChinese ||language == ELangHongKongChinese || language == ELangPrcChinese)
 		   retVal = ETrue;
 	FLOG(_L("[SmlNotif]\t CSyncMLAppLaunchNotifier::IsLanguageSupportedL ends"));
 	return retVal;
}
// -----------------------------------------------------------------------------
// CSyncMLAppLaunchNotifier::CompleteMessageL
// Completes the message according to whether Privacy Policy disclaimer is accepted
// -----------------------------------------------------------------------------
//
void CSyncMLAppLaunchNotifier::CompleteMessageL(TInt aDisclaimerAccepted)
    {
    FLOG(_L("[SmlNotif]\t CSyncMLAppLaunchNotifier::CompleteMessageL begins"));
    if(aDisclaimerAccepted == 1)
        {
        iMessage.Complete(KErrNone);
        }
    else
        {
        iMessage.Complete(KErrCancel);
        }
    FLOG(_L("[SmlNotif]\t CSyncMLAppLaunchNotifier::CompleteMessageL Ends"));
    }

//---------------------------------------------------------------------------------
// CDMDisclaimerObserver::CDMDisclaimerObserver
// Constructor
//---------------------------------------------------------------------------------
CDMDisclaimerObserver::CDMDisclaimerObserver()
: CActive(0)
    {
    CActiveScheduler::Add(this);
    }

//---------------------------------------------------------------------------------
// CDMDisclaimerObserver::~CDMDisclaimerObserver
// Destructor
//---------------------------------------------------------------------------------
CDMDisclaimerObserver::~CDMDisclaimerObserver()
    {
    Cancel();
    iDisclaimerProperty.Close();
    }

//---------------------------------------------------------------------------------
// CDMDisclaimerObserver::WaitOnDisclaimerL
// Subscribes to Property set when Privacy policy disclaimer is accepted
//---------------------------------------------------------------------------------
void CDMDisclaimerObserver::WaitOnDisclaimerL( CSyncMLAppLaunchNotifier* aPtr)
    {    
    FLOG(_L("[SmlNotif]\t CDMDisclaimerObserver::WaitOnDisclaimerL begins"));
    iNot = aPtr;
    // subscribe;
	if(!IsActive())
		{
		TInt err= iDisclaimerProperty.Attach(KDisclaimerProperty, KDisclaimerInteger, EOwnerThread);		
        //User::LeaveIfError(err);
		iStatus=KRequestPending;
		iDisclaimerProperty.Subscribe(iStatus);
        SetActive();
		}            
	FLOG(_L("[SmlNotif]\t CDMDisclaimerObserver::WaitOnDisclaimerL ends"));
    }

// --------------------------------------------------------------------------
// CDMDisclaimerObserver::DoCancel()
// From base class
// --------------------------------------------------------------------------
//
void CDMDisclaimerObserver::DoCancel()
    {
	 if( iStatus == KRequestPending )
    	{	    	
    	TRequestStatus* status = &iStatus;
    	User::RequestComplete( status, KErrCancel );
    	}
    }    
// --------------------------------------------------------------------------
// CDMDisclaimerObserver::RunL()
// Calls CompleteMessageL 
// --------------------------------------------------------------------------
//
void CDMDisclaimerObserver::RunL()
    {
    FLOG(_L("[SmlNotif]\t CDMDisclaimerObserver::RunL begins"));
    iDisclaimerProperty.Get(iPropertyVal);
     
    iNot->CompleteMessageL(iPropertyVal);
        
    FLOG(_L("[SmlNotif]\t CDMDisclaimerObserver::RunL End"));
    }

// --------------------------------------------------------------------------
// CDMDisclaimerObserver::RunError()
// --------------------------------------------------------------------------
//
TInt CDMDisclaimerObserver::RunError(TInt aError)
    {
    FTRACE( FPrint( _L("[SmlNotif]\t CDMDisclaimerObserver::RunError() Error = %d"), aError ) );
    return KErrNone;
    }
    
//  End of File  
