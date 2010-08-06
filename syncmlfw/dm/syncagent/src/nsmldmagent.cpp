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
* Description:  SyncML DM command handling 
*
*/


// INCLUDE FILES
#include <SyncMLObservers.h>
#include <SyncMLHistory.h>
#include <PolicyEngineClient.h>
#include <centralrepository.h>
#include "PMUtilInternalCRKeys.h"

#include <nsmlconstants.h>
#include <nsmldebug.h>
#include <nsmlunicodeconverter.h>
// DM specific includes
#include <nsmldmmodule.h>
// common includes with DS
#include "nsmlcliagconstants.h"
#include "NSmlAgentBase.h" 
#include "NSmlCmdsBase.h"
#include "NSmlStatusContainer.h"
#include "NSmlResultsContainer.h"
#include "NSmlResponseController.h"
#include "NSmlAuth.h"
#include "NSmlURI.h"
#include "nsmlagenttestdefines.h"
// DM specific includes
#include "nsmldmagconstants.h"
#include "NSmlDMAgent.h"
#include "NSmlDMCmds.h"
#include "nsmldmsettings.h"
#include "nsmldmerror.h"
#include "NSmlHistoryArray.h"
// FOTA
#include "NSmlDMGenericAlert.h"
#include "NSmlPrivateAPI.h"
// FOTA end
#include <featmgr.h>
const TInt KNotifierTimeout = 300000000; // 5 min timeout
_LIT8 ( KNSmlDMFotaNode, "FUMO" );

// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------
// CNSmlDMAgent::NewL()
// Creates new instance of CNSmlDMAgent. 
// Does not leave instance pointer to CleanupStack.
// ---------------------------------------------------------
//
CNSmlDMAgent* CNSmlDMAgent::NewL( MSyncMLProgressObserver* aDMObserver )
    {
	CNSmlDMAgent* self = new (ELeave) CNSmlDMAgent();
    CleanupStack::PushL( self );
	self->ConstructL( aDMObserver );
	CleanupStack::Pop();
    return self;
    }

// ---------------------------------------------------------
// CNSmlDMAgent::~CNSmlDMAgent()
// Destructor
// ---------------------------------------------------------
//
CNSmlDMAgent::~CNSmlDMAgent()
    {
	delete iDMTriggerMessage;
	delete iDMServerId;
	delete iDMServerPassword;
	delete iDMServerDigest;
	delete iSyncLog;
	// FOTA
	iGenAlerts.ResetAndDestroy();
	iGenAlerts.Close();
	// FOTA end	
if(FeatureManager::FeatureSupported(KFeatureIdSapPolicyManagement))
{
	delete iNotifierObserver;
}
FeatureManager::UnInitializeLib();
	}

// ---------------------------------------------------------
// CNSmlDMAgent::Configure()
// Starts the sync, returns immediately and the sync continues
// in RunL() function 
// ---------------------------------------------------------
//
void CNSmlDMAgent::Configure( TRequestStatus& aStatus,TInt aProfileId,TInt aTransportId,TNSmlSyncInitiation aSyncInitiation,TInt aIapId)
 	{
	DBG_FILE(_S8("CNSmlDMAgent::Configure"));
	
	CActiveScheduler::Add(this);
	iProfileID = aProfileId;
	iMediumType.iUid = aTransportId;
	iSyncInitiation = aSyncInitiation;
	iCallerStatus = &aStatus;
	iNewIapId = aIapId;
	*iCallerStatus = KRequestPending;
	TRAPD(leaveCode,iSyncLog = CSyncMLHistoryJob::NewL());
	
	if(leaveCode!=KErrNone)
		{
		User::RequestComplete( iCallerStatus, leaveCode );
		return;
		}

	SetActive();
	TRequestStatus* status = &iStatus;
	User::RequestComplete( status, KErrNone );
	}

// FOTA
// ---------------------------------------------------------
// CNSmlDMAgent::SetGenericAlertL()
//	Appends a Generic Alert to the internal list. 
//	The alert itself is sent in the later phase of the dm session.
//	@param aFwMgmtUri. The uri which has been used in the exe command whose final result is 
//	about to be reported.
//	@param aMetaType. Meta/Type that should be used in the alert.
//	@param aMetaFormat. Meta/Format that should be used in the alert.
//	@param aFinalResult. The final result value which is reported to remote server.
//	@param aCorrelator. Correlator value used in the original exec command. 
// ---------------------------------------------------------
//
void CNSmlDMAgent::SetGenericAlertL ( const TDesC8& aFwMgmtUri, const TDesC8& aMetaType, const TDesC8& aMetaFormat, 
						TInt aFinalResult, const TDesC8& aFwCorrelator )
	{
	CNSmlDMGenericAlert* genAlert = CNSmlDMGenericAlert::NewL ( aFwMgmtUri, aMetaType, aMetaFormat, aFinalResult, aFwCorrelator );
	iGenAlerts.AppendL ( genAlert );	
	}

// FOTA end

// ---------------------------------------------------------
// CNSmlDMAgent::SetGenericAlertL()
//	Appends a Generic Alert to the internal list. 
//	The alert itself is sent in the later phase of the dm session.
//	@param aFwMgmtUri. The uri which has been used in the exe command whose final result is 
//	about to be reported.
//	@param aMetaType. Meta/Type that should be used in the alert.
//	@param aMetaFormat. Meta/Format that should be used in the alert.
//	@param aFinalResult. The final result value which is reported to remote server.
//	@param aCorrelator. Correlator value used in the original exec command. 
// ---------------------------------------------------------
//
void CNSmlDMAgent::SetGenericAlertL ( const TDesC8& aCorrelator, const RArray<CNSmlDMAlertItem>& aItemList  )
    {
    CNSmlDMGenericAlert* genAlert = CNSmlDMGenericAlert::NewL ( aCorrelator, aItemList );
    iGenAlerts.AppendL ( genAlert );	
    }

//
//  Private member functions
//
// ---------------------------------------------------------
// CNSmlDMAgent::CNSmlDMAgent()
// Constructor
// ---------------------------------------------------------
//
CNSmlDMAgent::CNSmlDMAgent()
	{
	}
// ---------------------------------------------------------
// CNSmlDMAgent::ConstructL()
// Two-way construction. Constructor may leave in EPOC.
// ---------------------------------------------------------
//
void CNSmlDMAgent::ConstructL( MSyncMLProgressObserver* aDMObserver )
    {
	CNSmlAgentBase::ConstructL();
	iDMObserver = aDMObserver;
	iError = new (ELeave) TNSmlDMError;
	iSyncMLMIMEType = KNSmlDMAgentMIMEType().AllocL();
	iResultAlertCode = KNSmlDMAgentResultAlert().AllocL();
	iNextMessageCode = KNSmlDMAgentNextMessage().AllocL();
	//Session Abort Alert is used because there is no "End of Data Alert" in DM specs
	iEndOfDataAlertCode = KNSmlDMAgentSessionAbortAlert().AllocL(); 
	iSessionAbortAlertCode = KNSmlDMAgentSessionAbortAlert().AllocL();
	FeatureManager::InitializeLibL();
	iNewIapId = KErrNotFound;

	}

// ---------------------------------------------------------
// CNSmlDMAgent::SupportedAlert()
// Check from code if supported Alert  
// ---------------------------------------------------------
TBool CNSmlDMAgent::SupportedAlert( const TDesC8& aAlertCode ) const
{
if ( aAlertCode == KNSmlDMAgentServerInitAlert ||
        aAlertCode == KNSmlDMAgentClientInitAlert ||
        aAlertCode == KNSmlDMAgentResultAlert ||
        aAlertCode == KNSmlDMAgentNextMessage ||
        aAlertCode == KNSmlDMAgentSessionAbortAlert ||
        aAlertCode == KNSmlDMAgentDisplayAlert ||
        aAlertCode == KNSmlDMAgentContinueOrAbortAlert )
		{
		return ETrue;
		}
	return EFalse;
	}
//
//  Server Modifications state functions 
//
// ---------------------------------------------------------
// CNSmlDMAgent::ServerModificationsStateL(()
// Main state function to read both Initialization from server client package
// and Further server management operations package  
// ---------------------------------------------------------
void CNSmlDMAgent::ServerModificationsStateL()
    {
    _DBG_FILE(_S8("CNSmlDMAgent::ServerModificationsStateL begins"));

    // FOTA
    if ( !iChallengeRequest )
        {
        if ( iResetGenAlerts )
            {
            // Forward information to FOTA adapter if 1.1.2 otherwise any adapter 
            //which has sent GenAlert should be informed
            // It can now mark GenAlerts sent.
            if(!FeatureManager::FeatureSupported( KFeatureIdSyncMlDm112  ))
                {
                for ( TInt i(0); i<iGenAlerts.Count(); i++ )
                    {
                    if(iGenAlerts[i]->FinalResult() != 0)
                        {
                        iSyncMLCmds->MarkGenAlertsSentL(*iGenAlerts[i]->FwMgmtUri());    											
                        if(iGenAlerts[i]->FwMgmtUri()->Des().Find(KNSmlDMFotaNode)!=KErrNotFound)
                            {
                            iResetUserInitAlert= ETrue;	
                            }
                        }
                    else
                        {
                        // Generic alerts could be present for other adapters like SCOMO
                        iSyncMLCmds->MarkGenAlertsSentL(*(*iGenAlerts[i]->DataItem())[0].iSource);
                        }
                    }
                }
            else
                {
                iSyncMLCmds->MarkGenAlertsSentL();		
                }
            iResetGenAlerts = EFalse;
            iGenAlerts.ResetAndDestroy();
            }
        if ( iResetUserInitAlert )
            {
            iSyncMLCmds->ResetGenericUserAlertL();
            iResetUserInitAlert = EFalse;
            }
        }
    // FOTA end	

    if ( !iSyncMLCmds->ProcessReceivedDataL() )
        {
        DBG_FILE(_S8("CNSmlDMAgent::ServerModificationsStateL end of Data"));
        if ( iCurrServerModState == EServerModMessageReceived )
            {
            iBusyStatusReceived = iSyncMLCmds->BusyStatus();
            }
        else
            {
            if ( iCurrServerModState != EServerModPackageReceived )
                {
                User::Leave( TNSmlError::ESmlIncompleteMessage );
                }
            }
        iSyncMLCmds->EndOfServerMessageL();
        NextMainStateL();
        }
    //DBG_FILE(_S8("CNSmlDMAgent::ServerModificationsStateL ends"));
    }
// ---------------------------------------------------------
// CNSmlDMAgent::ServerModGetCmdStateL()
// 
// ---------------------------------------------------------
void CNSmlDMAgent::ServerModGetCmdStateL( SmlGet_t* aContent )
	{
	DBG_FILE(_S8("CNSmlDMAgent::ServerModGetCmdStateL begins"));
	if ( iCurrServerModState != EServerModWaitingCommands )
		{
		User::Leave( TNSmlError::ESmlUnexpectedCmd ); 
		}
	iSyncMLCmds->ProcessGetCmdL( aContent );	
	DBG_FILE(_S8("CNSmlDMAgent::ServerModGetCmdStateL ends"));
	}

// ---------------------------------------------------------
// CNSmlDMAgent::ServerModStartAtomicStateL()
// 
// ---------------------------------------------------------
void CNSmlDMAgent::ServerModStartAtomicStateL( SmlAtomic_t* aContent )
	{
	DBG_FILE(_S8("CNSmlDMAgent::ServerModStartAtomicStateL begins"));
	if ( iCurrServerModState != EServerModWaitingCommands )
		{
		User::Leave( TNSmlError::ESmlUnexpectedCmd ); 
		}
	if ( iNestedAtomicsCount > 0 )
		{
		iSyncMLCmds->StatusToUnsupportedCommandL( ETrue, KNSmlAgentAtomic, aContent->cmdID, aContent->flags );
		WriteWarningL( TNSmlError::ESmlUnexpectedCmd );
		}
	else
		{
		iSyncMLCmds->ProcessAtomicL( aContent );
		}
	++iNestedAtomicsCount;
	DBG_FILE(_S8("CNSmlDMAgent::ServerModStartAtomicStateL ends"));
	}
	

// ---------------------------------------------------------
// CNSmlDMAgent::ServerModEndAtomicStateL()
// 
// ---------------------------------------------------------
void CNSmlDMAgent::ServerModEndAtomicStateL()
	{
	DBG_FILE(_S8("CNSmlDMAgent::ServerModEndAtomicStateL begins"));
	--iNestedAtomicsCount;
	if ( iNestedAtomicsCount == 0 )
		{
		iSyncMLCmds->ProcessEndAtomicL();
		}
	DBG_FILE(_S8("CNSmlDMAgent::ServerModEndAtomicStateL ends"));
	}

// ---------------------------------------------------------
// CNSmlDMAgent::ServerModStartSequenceStateL()
// 
// ---------------------------------------------------------
void CNSmlDMAgent::ServerModStartSequenceStateL( SmlSequence_t* aContent )
	{
	DBG_FILE(_S8("CNSmlDMAgent::ServerModStartSequenceStateL begins"));
	if ( iCurrServerModState != EServerModWaitingCommands )
		{
		User::Leave( TNSmlError::ESmlUnexpectedCmd ); 
		}
	else
		{
		iSyncMLCmds->ProcessSequenceL( aContent );
		}
	DBG_FILE(_S8("CNSmlDMAgent::ServerModStartSequenceStateL ends"));
	}

// ---------------------------------------------------------
// CNSmlDMAgent::ServerModEndSequenceStateL()
// 
// ---------------------------------------------------------
void CNSmlDMAgent::ServerModEndSequenceStateL()
	{
	DBG_FILE(_S8("CNSmlDMAgent::ServerModEndSequenceStateL begins"));
	iSyncMLCmds->ProcessEndSequence();
	DBG_FILE(_S8("CNSmlDMAgent::ServerModEndSequenceStateL ends"));
	}

//
//  Finishing state functions 
//
// ---------------------------------------------------------
// CNSmlDMAgent::FinishingStateL()
// Finishing functions in the end of sync
// ---------------------------------------------------------
void CNSmlDMAgent::FinishingStateL()
	{
	DBG_FILE(_S8("CNSmlDMAgent::FinishingStateL begins"));
	FinishL();  
	TRAP_IGNORE( FinalizeSyncLogL());
	DBG_FILE(_S8("CNSmlDMAgent::FinishingStateL ends"));
	}

// ---------------------------------------------------------
// CNSmlDMAgent::ReadSettingsL()
// Reads settings for the DM session
// ---------------------------------------------------------
void CNSmlDMAgent::ReadSettingsL()
	{
	CNSmlDMProfile* profile = CNSmlDMSettings::NewLC()->ProfileL( iProfileID );
	if ( !profile )
		{
		User::Leave( TNSmlError::ESmlErrorInSettings );
		}
	else
		{
		CleanupStack::PushL( profile );
	
		if( iNewIapId > KErrNotFound )
			{
			DBG_FILE_CODE(iIAPId,_S8("CNSmlDMAgent::ReadSettingsL not using profiles IAP:"));	
			iIAPId = iNewIapId;
			}
		else
			{
			DBG_FILE_CODE(iIAPId,_S8("CNSmlDMAgent::ReadSettingsL using profiles IAP"));	
    		iIAPId = profile->IntValue( EDMProfileIAPId );
			}
		iIAPIdArray = new (ELeave) CArrayFixFlat<TUint32>(4);
		iIAPIdArray->AppendL( iIAPId );

		
		iSyncMLUserName = profile->StrValue( EDMProfileServerUsername ).AllocL();
		iSyncMLPassword = profile->StrValue( EDMProfileServerPassword ).AllocL();
		
		iSyncHTTPAuthUsed = profile->IntValue( EDMProfileHttpAuthUsed );	
		iSyncHTTPAuthUserName = profile->StrValue( EDMProfileHttpAuthUsername ).AllocL();	
		iSyncHTTPAuthPassword = profile->StrValue( EDMProfileHttpAuthPassword ).AllocL();	
		
		if ( iMediumType ==  KUidNSmlMediumTypeInternet )
			{
			iSyncServer= CNSmlURI::NewL( profile->StrValue( EDMProfileServerURL ), ETrue );
			}
		else
			{
			iSyncServer= CNSmlURI::NewL( profile->StrValue( EDMProfileServerURL ), EFalse );
			}
		iDMServerId = profile->StrValue( EDMProfileServerId ).AllocL();
		iDMServerPassword = profile->StrValue( EDMProfileClientPassword ).AllocL();
		iDMServerSessionId = profile->IntValue( EDMProfileSessionId );
		CleanupStack::PopAndDestroy( 2 ); //profile, settings
		}
	}


// ---------------------------------------------------------
// CNSmlDMAgent::InitSyncLogL()
// Sync Log (shown In UI) initilisation in the beginning of sync
// ---------------------------------------------------------
void CNSmlDMAgent::InitSyncLogL()
	{
	CNSmlDMProfile* profile = CNSmlDMSettings::NewLC()->ProfileL( iProfileID );
	if ( profile )
		{
		CleanupStack::PushL( profile );
		TTime now;
		now.HomeTime();
		CleanupStack::PopAndDestroy(2); //profile, settings
		}
	else
		{
		CleanupStack::PopAndDestroy(); //settings
		}
	}
// ---------------------------------------------------------
// CNSmlDMAgent::GetSessionIDL()
// Gets SessionID from Agent Log
// ---------------------------------------------------------
TInt CNSmlDMAgent::GetSessionIDL()
	{
	CNSmlDMProfile* profile = CNSmlDMSettings::NewLC()->ProfileL( iProfileID );
	TInt sessionID = 0;
	if ( !profile )
		{
		User::Leave( TNSmlError::ESmlErrorInSettings );
		}
	else
		{
		CleanupStack::PushL( profile );
		sessionID = profile->IntValue( EDMProfileSessionId );  
		CleanupStack::PopAndDestroy( 2 ); //profile, settings
		}
	return sessionID;
	}
// ---------------------------------------------------------
// CNSmlDMAgent::UpdateSessionIDL()
// Updates SessionID in Agent Log
// ---------------------------------------------------------
void CNSmlDMAgent::UpdateSessionIDL( TInt aSessionID )
	{
	CNSmlDMProfile* profile = CNSmlDMSettings::NewLC()->ProfileL( iProfileID );
	if ( !profile )
		{
		User::Leave( TNSmlError::ESmlErrorInSettings );
		}
	else
		{
		CleanupStack::PushL( profile );
		profile->SetIntValue( EDMProfileSessionId, aSessionID );
		profile->SaveL();
		CleanupStack::PopAndDestroy( 2 ); //profile, settings
		}
	}

// ---------------------------------------------------------
// CNSmlDMAgent::FinishL()
// 
// ---------------------------------------------------------
void CNSmlDMAgent::FinishL()
	{
	
	iSyncMLCmds->WriteMappingInfoToDbL();
	iSyncMLCmds->DisconnectFromOtherServers();
	if ( Interrupted() )
		{
		iDMObserver->OnSyncMLSyncError(MSyncMLProgressObserver::ESmlFatalError,iError->SyncLogErrorCode(),0,0,0);
		User::RequestComplete( iCallerStatus, iError->SyncLogErrorCode() );
		}
	else
		{
		iDMObserver->OnSyncMLSyncProgress(MSyncMLProgressObserver::ESmlCompleted,0,0);
		User::RequestComplete( iCallerStatus, KErrNone );
		}
	iEnd = ETrue;
	}

// ---------------------------------------------------------
// CNSmlDMAgent::InterruptedImmediately()
// 
// ---------------------------------------------------------
TBool CNSmlDMAgent::InterruptedImmediately()
	{
	return iDMImmediatelyInterruption;
	}

//
// Callback functions implementation
//
// ---------------------------------------------------------
// CNSmlDMAgent::smlAlertCmdFuncL
// Alert command from server.  
// ---------------------------------------------------------
Ret_t CNSmlDMAgent::smlAlertCmdFuncL( SmlAlertPtr_t aContent )
	{
	iCallbackCalled = ETrue;
	if ( !SupportedAlert( iSyncMLCmds->AlertCode( aContent ) ) )
		{
		iSyncMLCmds->StatusToUnsupportedCommandL( EFalse, KNSmlAgentAlert, aContent->cmdID, aContent->flags );
		}
	else
		{
		switch ( iCurrMainState )
			{
			case EServerAlertNext:
				ServerAlertNextAlertCmdStateL( aContent );
				break;
			case EServerModifications:
				ServerModAlertCmdStateL( aContent );
				break;
			default: 
				break;
			}
		}
	return 0;
	}

// ---------------------------------------------------------
// CNSmlAgentBase::smlExecCmdFunc
// Exec command from server
// ---------------------------------------------------------
Ret_t CNSmlDMAgent::smlExecCmdFuncL( SmlExecPtr_t aContent )
	{
	iCallbackCalled = ETrue;
	switch ( iCurrMainState )
		{
		case EServerModifications:
			// FOTA
			ServerModUpdatesStateL ( aContent );
			// FOTA end
			break;
		default:
			iSyncMLCmds->StatusToUnsupportedCommandL( ETrue, KNSmlAgentExec, aContent->cmdID, aContent->flags );
			break;
		}
	return 0;
	}


// ---------------------------------------------------------
// CNSmlAgentBase::smlCopyCmdFunc
// Exec command from server
// ---------------------------------------------------------
Ret_t CNSmlDMAgent::smlCopyCmdFuncL( SmlCopyPtr_t aContent )
	{
	iCallbackCalled = ETrue;
	switch ( iCurrMainState )
		{
		case EServerModifications:
			ServerModUpdatesStateL( KNSmlAgentCopy(), aContent );
			break;
		default:
			iSyncMLCmds->StatusToUnsupportedCommandL( ETrue, KNSmlAgentCopy, aContent->cmdID, aContent->flags );
			break;
		}
	return 0;
	}

// ---------------------------------------------------------
// CNSmlDMAgent::smlGetCmdFuncL
// Get command from server
// ---------------------------------------------------------
Ret_t CNSmlDMAgent::smlGetCmdFuncL( SmlGetPtr_t aContent )
	{
	iCallbackCalled = ETrue;
	switch ( iCurrMainState )
		{
		case EServerModifications:
			ServerModGetCmdStateL( aContent );
			break;
		default:
			iSyncMLCmds->StatusToUnsupportedCommandL( ETrue, KNSmlAgentGet, aContent->cmdID, aContent->flags );
			break;
		}
	return 0;
	}
// ---------------------------------------------------------
// CNSmlDMAgent::smlPutCmdFuncL
// Put command from server
// ---------------------------------------------------------
Ret_t CNSmlDMAgent::smlPutCmdFuncL( SmlPutPtr_t aContent )
	{
	iCallbackCalled = ETrue;
	iSyncMLCmds->StatusToUnsupportedCommandL( ETrue, KNSmlAgentPut, aContent->cmdID, aContent->flags );
	return 0;
	}
// ---------------------------------------------------------
// CNSmlDMAgent::smlResultsCmdFuncL
// Results command from server
// ---------------------------------------------------------
Ret_t CNSmlDMAgent::smlResultsCmdFuncL( SmlResultsPtr_t aContent )
	{
	iCallbackCalled = ETrue;
	iSyncMLCmds->StatusToUnsupportedCommandL( ETrue, KNSmlAgentResults, aContent->cmdID );
	return 0;
	}
// ---------------------------------------------------------
// CNSmlDMAgent::smlStatusCmdFuncL
// Status command from server
// ---------------------------------------------------------
Ret_t CNSmlDMAgent::smlStatusCmdFuncL( SmlStatusPtr_t aContent )
	{
	iCallbackCalled = ETrue;
	switch ( iCurrMainState )
		{
		case EServerModifications:
			ServerModStatusCmdStateL( aContent );
			break;
		case EServerAlertNext:
			ServerAlertNextStatusCmdStateL( aContent );
			break;
		default:
			break;
		}
	return 0;
	}

// ---------------------------------------------------------
// CNSmlDMAgent::smlStartMessageFuncL
// SyncHdr from server
// ---------------------------------------------------------
Ret_t CNSmlDMAgent::smlStartMessageFuncL( SmlSyncHdrPtr_t aContent )
	{
	iCallbackCalled = ETrue;
	switch ( iCurrMainState )
		{
		case EServerModifications:
			ServerModStartMessageStateL( aContent );
			break;
		case EServerAlertNext:
			ServerAlertNextStartMessageStateL( aContent );
			break;
		default:
			break;
		}
	return 0;
	}
// ---------------------------------------------------------
// CNSmlDMAgent::smlEndMessageFuncL
// End of SyncML message was reached
// ---------------------------------------------------------
Ret_t CNSmlDMAgent::smlEndMessageFuncL( Boolean_t aFinal )
	{
	iCallbackCalled = ETrue;
	switch ( iCurrMainState )
		{
		case EServerModifications:
			ServerModEndMessageStateL( aFinal );
			break;
		case EServerAlertNext:
			ServerAlertNextEndMessageStateL( aFinal );
			break;
		default:
			User::Leave( TNSmlError::ESmlUnexpectedCmd );
			break;
		}
	return 0;
	}
// ---------------------------------------------------------
// CNSmlDMAgent::smlStartSyncFuncL
// Sync command from server
// ---------------------------------------------------------
Ret_t CNSmlDMAgent::smlStartSyncFuncL( SmlSyncPtr_t aContent )
	{
	iCallbackCalled = ETrue;
	iSyncMLCmds->StatusToUnsupportedCommandL( ETrue, KNSmlAgentSync, aContent->cmdID, aContent->flags );
	WriteWarningL( TNSmlError::ESmlUnexpectedCmd );
	return 0;
	}
// ---------------------------------------------------------
// CNSmlDMAgent::smlEndSyncFuncL
// End of Sync command was reached
// ---------------------------------------------------------
Ret_t CNSmlDMAgent::smlEndSyncFuncL()
	{
	iCallbackCalled = ETrue;
	return 0;
	}
// ---------------------------------------------------------
// CNSmlDMAgent::smlStartAtomicFuncL
// Atomic command from server
// ---------------------------------------------------------
Ret_t CNSmlDMAgent::smlStartAtomicFuncL( SmlAtomicPtr_t aContent )
	{
	iCallbackCalled = ETrue;
	switch ( iCurrMainState )
		{
		case EServerModifications:
			ServerModStartAtomicStateL( aContent );
			break;
		default:
			iSyncMLCmds->StatusToUnsupportedCommandL( ETrue, KNSmlAgentAtomic, aContent->cmdID, aContent->flags );
			break;
		}
	
	return 0;
	}
// ---------------------------------------------------------
// CNSmlDMAgent::smlEndAtomicFuncL
// End of Atomic command was reached
// ---------------------------------------------------------
Ret_t CNSmlDMAgent::smlEndAtomicFuncL()
	{
	iCallbackCalled = ETrue;
	ServerModEndAtomicStateL();
	return 0;
	}

// ---------------------------------------------------------
// CNSmlDMAgent::smlStartSequenceFuncL
// Sequence command from server
// ---------------------------------------------------------
Ret_t CNSmlDMAgent::smlStartSequenceFuncL( SmlSequencePtr_t aContent )
	{
	iCallbackCalled = ETrue;
	switch ( iCurrMainState )
		{
		case EServerModifications:
			ServerModStartSequenceStateL( aContent );
			break;
		default:
			iSyncMLCmds->StatusToUnsupportedCommandL( ETrue, KNSmlAgentSequence, aContent->cmdID, aContent->flags );
			break;
		}
	
	return 0;
	}
// ---------------------------------------------------------
// CNSmlDMAgent::smlEndSequenceFuncL
// End of Sequence command was reached
// ---------------------------------------------------------
Ret_t CNSmlDMAgent::smlEndSequenceFuncL()
	{
	iCallbackCalled = ETrue;
	ServerModEndSequenceStateL();
	return 0;
	}

// ---------------------------------------------------------
// CNSmlDMAgent::NavigateMainStateL()
// Navigate according to state code
// ---------------------------------------------------------
void CNSmlDMAgent::NavigateMainStateL() 
    {
#ifdef __CANCEL_TEST
	iDMObserver->NotifyDisplayAlertL(KNullDesC,0,-99);
#endif
    switch ( iCurrMainState )
		{
		case EBeginning: 
if(FeatureManager::FeatureSupported(KFeatureIdSapPolicyManagement))
{
			if ( iCurrBeginningState == EBegin )
			{
				CheckForcedCertificateCheckStateL();
			}
}
			BeginningStateL();
			break;
		case EClientInitialisation:
			ClientInitialisationStateL();
			break;
		case EServerModifications:
			ServerModificationsStateL();
			break;
		case EDataUpdateStatus:
			DataUpdateStatusStateL();
			break;
		case EResultAlert:
			ResultAlertStateL();
			break;
		case EClientAlertNext:
			ClientAlertNextStateL();
			break;
		case EServerAlertNext:
			ServerAlertNextStateL();
			break;
		case EFinishing:
			FinishingStateL();
			break;
		case ESending:
if(FeatureManager::FeatureSupported(KFeatureIdSapPolicyManagement))
{
  //if loop commented for DM session When TARM enabled TSW:ARAA-73XHUM
			//if ( !CheckCertificateL())
			//{	
				//Start sending state...
				SendingStateL();
			//}
}
else
{
			SendingStateL();
}
break;
		case EReceiving:
			ReceivingStateL();
			break;
		default:
			break;
		}
    }

// ---------------------------------------------------------
// CNSmlDMAgent::NextMainStateL()
// Call a function which changes a state depending on type
// 
// ---------------------------------------------------------
//
void CNSmlDMAgent::NextMainStateL() 
	{
	if ( iCurrMainState == EReceiving )
		{
		iDMObserver->OnSyncMLSyncProgress(MSyncMLProgressObserver::ESmlReceivingServerCommands,0,0);
		}
	switch ( iCurrMainState )
		{
		case EBeginning:
			if ( Interrupted() )
				{
				DBG_FILE(_S8("CNSmlDMAgent::NextMainStateL EFinishing"));
				iCurrMainState = EFinishing;
				} 
			else
				{
				DBG_FILE(_S8("CNSmlDMAgent::NextMainStateL EClientInitialisation"));
				iCurrMainState = EClientInitialisation;
				}
			break;
		case EClientInitialisation:
			iPrevSyncState = iCurrMainState;
			DBG_FILE(_S8("CNSmlDMAgent::NextMainStateL ESending"));
			iCurrMainState = ESending;
			break;
		case EServerModifications:
			iPrevSyncState = iCurrMainState;
			if ( InterruptedImmediately() ) // immediately interruption needed?
				{
				DBG_FILE(_S8("CNSmlDMAgent::NextMainState EFinishing"));
				iCurrMainState = EFinishing;
				}
			else
				{
				if ( iBusyStatusReceived )
					{
					DBG_FILE(_S8("CNSmlDMAgent::NextMainStateL EResultAlert"));
					iCurrMainState = EResultAlert;
					}
				else
					{					
					if ( iChallengeRequest )
						{
						DBG_FILE(_S8("CNSmlDMAgent::NextMainStateL EClientInitialisation"));
						iCurrMainState = EClientInitialisation;
						}
					else
						{
						if ( iCurrServerModState == EServerModMessageReceived )
							{
							DBG_FILE(_S8("CNSmlDMAgent::NextMainStateL EClientAlertNext"));
							iCurrMainState = EClientAlertNext;
							}
						else
							{
							DBG_FILE(_S8("CNSmlDMAgent::NextMainStateL EDataUpdateStatus"));
							iCurrMainState = EDataUpdateStatus;
							}
						}
					}
				}
			break;
		case EDataUpdateStatus:
			iPrevSyncState = iCurrMainState;
			if ( iStatusPackage )
				{
				DBG_FILE(_S8("CNSmlDMAgent::NextMainStateL ESending"));
				iCurrMainState = ESending;
				}
			else
				{
				DBG_FILE(_S8("CNSmlDMAgent::NextMainStateL EFinishing"));
				iCurrMainState = EFinishing;
				}
			break;
		case EResultAlert:
			DBG_FILE(_S8("CNSmlDMAgent::NextMainStateL ESending"));
			iCurrMainState = ESending;
			break;
		case EClientAlertNext:
			DBG_FILE(_S8("CNSmlDMAgent::NextMainStateL ESending"));
			iCurrMainState = ESending;
			break;
		case EServerAlertNext:
			if ( Interrupted() )
				{
				DBG_FILE(_S8("CNSmlDMAgent::NextMainStateL EFinishing"));
				iCurrMainState = EFinishing;
				}
			else
				{
				DBG_FILE(_S8("CNSmlDMAgent::NextMainStateL Previous State"));
				iCurrMainState = iPrevSyncState;
				}
			break;
		case ESending:
			if ( Interrupted() ) 
				{
				DBG_FILE(_S8("CNSmlDMAgent::NextMainStateL EFinishing"));
				iCurrMainState = EFinishing;
				}
			else
				{
				DBG_FILE(_S8("CNSmlDMAgent::NextMainStateL EReceiving"));
				iCurrMainState = EReceiving;
				}
			break;
		case EReceiving:
			if ( ( !iFinalMessageFromClient ) && ( iPrevSyncState == EDataUpdateStatus ) )
				{
				DBG_FILE(_S8("CNSmlDMAgent::NextMainStateL EServerAlertNext"));
				iCurrMainState = EServerAlertNext;
				}
			else
				{
				DBG_FILE(_S8("CNSmlDMAgent::NextMainStateL EServerModifications"));
				iCurrMainState = EServerModifications; 
				}	
			break;
		default:
			break;
		}
	InitialiseSubStates();
	if ( iCurrMainState == ESending )
		{
		iDMObserver->OnSyncMLSyncProgress(MSyncMLProgressObserver::ESmlSendingModificationsToServer,0,0);
		}
	}

// ---------------------------------------------------------
// CNSmlDMAgent::InitialiseSubStates()
// 
// ---------------------------------------------------------
//
void CNSmlDMAgent::InitialiseSubStates()
	{
	iCurrBeginningState = EBegin;
	iCurrClientInitState = EInitStartingMessage;
	iCurrResultAlertState = EResultAlertStartingMessage;
	iCurrServerModState = EServerModWaitingStartMessage;
	iCurrDataUpdateStatusState = EStatusStartingMessage;
	iCurrResultAlertState = EResultAlertStartingMessage;
	iCurrClientAlertNextState = EClientAlertNextStartingMessage;
	iCurrServerAlertNextState = EServerAlertNextWaitingStartMessage;
	}
	
	

TBool CNSmlDMAgent::CheckCertificateL()
{
	if(!FeatureManager::FeatureSupported(KFeatureIdSapPolicyManagement))
	{
		User::Leave(KErrNotSupported);
	}
	TBool retVal = EFalse;

	//do certificate check...
	if ( iForcedCertificateCheck && iCommandIssued)
	{
		//get session certificate via CentRep interface
		CRepository * rep = 0;
		TRAPD( err, rep = CRepository::NewL( KCRUidPolicyManagementUtilInternalKeys ))
		TCertInfo info;
		
		if( err == KErrNone)
		{
			//read certificate
			TPckg<TCertInfo> certPck( info);
			err = rep->Get( KSyncMLSessionCertificate, certPck);
			delete rep;
		}

		TRole role = ENoRole;
		
		if ( err == KErrNone)
		{
			//connect to policy engine
			RPolicyEngine policyEngine;
			err = policyEngine.Connect();
		
			RPolicyManagement policyManagement;
			err = policyManagement.Open( policyEngine);

			//check certificate role
			policyManagement.CertificateRole( info, role);
			
			policyManagement.Close();
			policyEngine.Close();
		}
		
		if ( role == ENoRole)
		{	
			//if session doesn't have certificate or certificate doesn't have any role 
			//user acceptance is needed to resume the connection
			UserConfirmateConnectionL();	
			retVal = ETrue;
		}

		iForcedCertificateCheck = EFalse;
	}
	
	return retVal;
}

void CNSmlDMAgent::UserConfirmateConnectionL()
{
	if(!FeatureManager::FeatureSupported(KFeatureIdSapPolicyManagement))
	{
		User::Leave(KErrNotSupported);
	}
	delete iNotifierObserver;
	iNotifierObserver = 0;
	iNotifierObserver = new (ELeave) CNSmlAgentNotifierObserver( iStatus, 100);
	iNotifierObserver->ConnectToNotifierL( this);
	
}


void CNSmlDMAgent::CheckForcedCertificateCheckStateL()
{
	if(!FeatureManager::FeatureSupported(KFeatureIdSapPolicyManagement))
	{
		User::Leave(KErrNotSupported);
	}
	CRepository * rep = CRepository::NewLC( KCRUidPolicyManagementUtilInternalKeys );

	User::LeaveIfError( rep->Get( KSyncMLForcedCertificateCheckKey, iForcedCertificateCheck));
	CleanupStack::PopAndDestroy( rep);
}


	
	
//
// Beginning state functions 
//
//
// ---------------------------------------------------------
// CNSmlDMAgent::BeginStateL()
// 
// ---------------------------------------------------------
void CNSmlDMAgent::BeginStateL() 
	{
	iDMObserver->OnSyncMLSyncProgress(MSyncMLProgressObserver::ESmlConnecting,0,0);

	DBG_FILE(_S8("CNSmlDMAgent::BeginStateL Before InitSyncLogL"));
	InitSyncLogL();
	if ( iSyncInitiation == EServerAlerted )
		{
		DBG_FILE(_S8("CNSmlDMAgent::BeginStateL Before ParseTriggerMessageL"));
		}
	DBG_FILE(_S8("CNSmlDMAgent::BeginStateL Before ReadSettingsL"));
	ReadSettingsL();
	DBG_FILE(_S8("CNSmlDMAgent::BeginStateL Before CreateSyncmlCmdsInstanceL"));
	CreateSyncmlCmdsInstanceL();
	iCurrBeginningState = EBeginConnectingToServer;
	}

//
//  Client Initialisation state functions 
//

// ---------------------------------------------------------
// CNSmlDMAgent::InitStartingMessageStateL()
// 
// ---------------------------------------------------------
void CNSmlDMAgent::InitStartingMessageStateL()
	{
	InitStartingMessageStateBaseL();
	}
// ---------------------------------------------------------
// CNSmlDMAgent::InitAlertingStateL()
// Makes initialisation Alert command.
// May also generate one or many generic alerts and 
// a generic user alert (i.e. user request to update firmware)
// ---------------------------------------------------------
void CNSmlDMAgent::InitAlertingStateL()
	{
	if ( !Interrupted() )
		{
		TBuf8<4> alertCode;
		if ( iSyncInitiation == EClientInitiated )
			{
			alertCode = KNSmlDMAgentClientInitAlert;
			}
		else
			{
			alertCode =	KNSmlDMAgentServerInitAlert;
			}
		iSyncMLCmds->DoAlertL( alertCode );
		// FOTA
    for ( TInt i(0); i<iGenAlerts.Count(); i++ )
    {
    	// FOTA
    	if(iGenAlerts[i]->FinalResult() != 0)
      {	
      	iSyncMLCmds->DoGenericAlertL ( *iGenAlerts[i]->FwMgmtUri(), *iGenAlerts[i]->MetaType(), *iGenAlerts[i]->MetaFormat(),	
                        iGenAlerts[i]->FinalResult(), *iGenAlerts[i]->FwCorrelator()  );                
      }
      else
      {
      	// Other than FOTA case
      	iSyncMLCmds->DoGenericAlertL ( *iGenAlerts[i]->FwCorrelator(), *iGenAlerts[i]->DataItem());               
      }		
     }     		
		if ( iGenAlerts.Count() > 0 )
			{
			iResetGenAlerts = ETrue;			
			}
		iResetUserInitAlert = iSyncMLCmds->DoGenericUserAlertL ( iProfileID );
		// FOTA end
		}
	iCurrClientInitState = EInitDeviceInfo;
	}
// ---------------------------------------------------------
// CNSmlDMAgent::InitDeviceInfoStateL()
//  Device Info is sent in Replace command
// ---------------------------------------------------------
void CNSmlDMAgent::InitDeviceInfoStateL()
	{
	if ( !Interrupted() )
		{
		iSyncMLCmds->DoAddOrReplaceOrDeleteL(); 
		}
	iCurrClientInitState = EInitEndingMessage;
	}



// ---------------------------------------------------------
// CNSmlDMAgent::ServerModUpdatesStateL(()
// Handles Add, Replace or Delete command from server 
// ---------------------------------------------------------
void CNSmlDMAgent::ServerModUpdatesStateL( const TDesC8& aCmd, SmlGenericCmd_t* aContent )
	{
	DBG_FILE(_S8("CNSmlDMAgent::ServerModUpdatesStateL begins"));
	if ( iCurrServerModState != EServerModWaitingCommands )
		{
		User::Leave( TNSmlError::ESmlUnexpectedCmd ); 
		}
	iSyncMLCmds->ProcessUpdatesL( aCmd, aContent );	
	DBG_FILE(_S8("CNSmlDMAgent::ServerModUpdatesStateL ends"));
	}
// FOTA
// ---------------------------------------------------------
// CNSmlDMAgent::ServerModUpdatesStateL(()
//	Issues a process request for the arrived exec command.
//	@param aContent. The exec command structure, accordant 
//  with the dtd.
// ---------------------------------------------------------
//
void CNSmlDMAgent::ServerModUpdatesStateL ( SmlExec_t* aContent )
	{
	DBG_FILE(_S8("CNSmlDMAgent::ServerModUpdatesStateL (for exec) begins"));
	if ( iCurrServerModState != EServerModWaitingCommands )
		{
		User::Leave( TNSmlError::ESmlUnexpectedCmd ); 
		}
	iSyncMLCmds->ProcessExecCmdL ( aContent );	
	DBG_FILE(_S8("CNSmlDMAgent::ServerModUpdatesStateL (for exec) ends"));
	}
// FOTA end

// ---------------------------------------------------------
// CNSmlDMAgent::DataUpdateStatusStartingMessageStateL()
// Notify "Finalising" to UI, makes SyncHdr 
// ---------------------------------------------------------
void CNSmlDMAgent::DataUpdateStatusStartingMessageStateL()
	{
	DBG_FILE(_S8("CNSmlDMAgent::DataUpdateStatusStartingMessageStateL begins"));
	DataUpdateStatusStartingMessageStateBaseL();
	DBG_FILE(_S8("CNSmlDMAgent::DataUpdateStatusStartingMessageStateL ends"));
	}

// ---------------------------------------------------------
// CNSmlDMAgent::DataUpdateStatusMakingResultsStateL()
// Results to Get commands from a server 
// ---------------------------------------------------------
void CNSmlDMAgent::DataUpdateStatusMakingResultsStateL()
	{
	DBG_FILE(_S8("CNSmlDMAgent::DataUpdateStatusMakingResultsStateL begins"));
	if ( iSyncMLCmds->ResultsContainer() )
		{
		if ( iSyncMLCmds->ResultsContainer()->AnyResults() )
			{
			// there is something to send
			iStatusPackage = ETrue;
			}
		}
	CNSmlCmdsBase::TReturnValue ret = iSyncMLCmds->DoResultsL();
	if ( ret == CNSmlCmdsBase::EReturnBufferFull )
		{
		iBufferFull = ETrue;
		}
	iCurrDataUpdateStatusState = EStatusEndingMessage;
	DBG_FILE(_S8("CNSmlDMAgent::DataUpdateStatusMakingResultsStateL ends"));
	}



// ---------------------------------------------------------
// CNSmlDMAgent::NonceL()
// Nonce (used in MD5 auth) from DM Settings 
// ---------------------------------------------------------
HBufC* CNSmlDMAgent::NonceL() const
	{
	HBufC* nonce = NULL;
#ifndef	__MODULETEST
	CNSmlDMProfile* profile = CNSmlDMSettings::NewLC()->ProfileL( iProfileID );
	if ( !profile )
		{
		User::Leave( TNSmlError::ESmlErrorInSettings );
		}
	else
		{
		CleanupStack::PushL( profile );
		nonce = profile->NonceLC( EDMProfileServerNonce );
		CleanupStack::Pop(); //NonceLC
		CleanupStack::PopAndDestroy( 2 ); //profile, settings
		}
#endif
	return( nonce );
	}
// ---------------------------------------------------------
// CNSmlDMAgent::SetNonceL()
// Nonce from the server's challenge is stored to DM Settings
// ---------------------------------------------------------
void CNSmlDMAgent::SetNonceL( const TDesC& aNonce ) const
	{
#ifndef	__MODULETEST
	CNSmlDMProfile* profile = CNSmlDMSettings::NewLC()->ProfileL( iProfileID );
	if ( !profile )
		{
		User::Leave( TNSmlError::ESmlErrorInSettings );
		}
	else
		{
		CleanupStack::PushL( profile );
		profile->SetNonceL( EDMProfileServerNonce, aNonce );
		CleanupStack::PopAndDestroy( 2 ); //profile, settings
		}
#endif	
	}

// ---------------------------------------------------------
// CNSmlDMAgent::ServerUserName()
// 
// ---------------------------------------------------------
TPtrC CNSmlDMAgent::ServerUserName() const
	{
	return *iDMServerId;
	}
// ---------------------------------------------------------
// CNSmlDMAgent::ServerPassword()
// 
// ---------------------------------------------------------
TPtrC CNSmlDMAgent::ServerPassword() const 
	{
	return *iDMServerPassword;
	}
// ---------------------------------------------------------
// CNSmlDMAgent::ServerNonceAllocL()
// 
// ---------------------------------------------------------
HBufC* CNSmlDMAgent::ServerNonceAllocL() const
	{
	HBufC* nonce = NULL;
	CNSmlDMProfile* profile = CNSmlDMSettings::NewLC()->ProfileL( iProfileID );
	if ( !profile )
		{
		User::Leave( TNSmlError::ESmlErrorInSettings );
		}
	else
		{
		CleanupStack::PushL( profile );
		nonce = profile->NonceLC( EDMProfileClientNonce ); 
		CleanupStack::Pop(); //NonceLC
		CleanupStack::PopAndDestroy( 2 ); //profile, settings
		}
	return( nonce );
	}
// ---------------------------------------------------------
// CNSmlDMAgent::SetServerNonceL()
// 
// ---------------------------------------------------------
void CNSmlDMAgent::SetServerNonceL( const TDesC& aNonce ) const
	{
	CNSmlDMProfile* profile = CNSmlDMSettings::NewLC()->ProfileL( iProfileID );
	if ( !profile )
		{
		User::Leave( TNSmlError::ESmlErrorInSettings );
		}
	else
		{
		CleanupStack::PushL( profile );
		profile->SetNonceL( EDMProfileClientNonce, aNonce );
		CleanupStack::PopAndDestroy( 2 ); //profile, settings
		}
	}

// ---------------------------------------------------------
// CNSmlDMAgent::FinalizeSyncLog()
// Sync Log is written after sync
// ---------------------------------------------------------
void CNSmlDMAgent::FinalizeSyncLog()
	{
	TRAP_IGNORE( FinalizeSyncLogL() );
	}
	
// ---------------------------------------------------------
// CNSmlDMAgent::FinalizeSyncLogL()
// Sync Log is written after sync
// ---------------------------------------------------------
void CNSmlDMAgent::FinalizeSyncLogL()
	{
	if(iSyncLog)
		{
		TTime now;
		//now.HomeTime();//previously using
		now.UniversalTime();
		if(iError)
		iSyncLog->SetResult(now, iError->SyncLogErrorCode());
		else
		iSyncLog->SetResult(now, KErrGeneral);   

			CNSmlDMSettings* settings = CNSmlDMSettings::NewLC();
			CNSmlDMProfile* profile = settings->ProfileL( iProfileID );
			if(!profile)
			    {
			    iSyncLog = NULL;
			    CleanupStack::PopAndDestroy();  //setttings
			      return;
			    }
			CleanupStack::PushL( profile );
		  if( iError )
		  {
           if(iError->SyncLogErrorCode() == KErrNone)
           {
           RWriteStream& LastSyncStream = profile->LastSyncWriteStreamL();
           CleanupClosePushL( LastSyncStream );
           TPckgBuf<TTime> lastsynctime(now);
           LastSyncStream << lastsynctime;
           CleanupStack::PopAndDestroy();
           profile->LastSyncStreamCommitL();
           iSyncLog->SetLastSyncResult(now);//vijay
           }
          else
          {
			if(profile->HasLastSyncL())
			{
			RReadStream& Lastsyncstream=profile->LastSyncReadStreamL();
			CleanupClosePushL( Lastsyncstream );
	        TTime  SyncSuccessTime;
	        SyncSuccessTime.UniversalTime();
	        TPckgBuf<TTime> lastsynctime(SyncSuccessTime);
	        Lastsyncstream >> lastsynctime;
	        SyncSuccessTime = lastsynctime().Int64();
	        iSyncLog->SetLastSyncResult(SyncSuccessTime);
	        CleanupStack::PopAndDestroy();	
			}
          } 
        }
			CNSmlHistoryArray* array = CNSmlHistoryArray::NewL();
			CleanupStack::PushL( array );
			array->SetOwnerShip( ETrue );

			RReadStream& readStream = profile->LogReadStreamL();
			CleanupClosePushL( readStream );
			
			if ( readStream.Source()->SizeL() > 0 )
				{
				array->InternalizeL( readStream );
				}
				
			CleanupStack::PopAndDestroy(); // readStream

			array->AppendEntryL( iSyncLog );
			RWriteStream& writeStream = profile->LogWriteStreamL();
			CleanupClosePushL( writeStream );
			array->ExternalizeL( writeStream );
			CleanupStack::PopAndDestroy(); // writeStream
			profile->WriteStreamCommitL();

			CleanupStack::PopAndDestroy( 3 ); // array, settings, profile
			
			iSyncLog = NULL;
		}
	}


// ---------------------------------------------------------
// CNSmlDMAgent::CreateSyncmlCmdsInstanceL()
// 
// ---------------------------------------------------------
void CNSmlDMAgent::CreateSyncmlCmdsInstanceL()
	{
	if ( !iSyncMLCmds )
		{
		TInt numSessionID;
		TBuf8<16> stringSessionID;
		if ( iSyncInitiation == EServerAlerted )
			{
			numSessionID = iDMServerSessionId; 
			}
		else
			{
			numSessionID = GetSessionIDL();
			numSessionID++;
			}
		//Session id is greater than 16-bit max value
		if( numSessionID > KMaxInt16 || numSessionID < 0 )
		   {
		   	numSessionID = 0;
		   }	
		UpdateSessionIDL( numSessionID );
		stringSessionID.Num( numSessionID );
		if(!FeatureManager::FeatureSupported( KFeatureIdSyncMlDm112  ))
		{
		iSyncMLCmds = CNSmlDMCmds::NewL( this, stringSessionID, KNSmlDMAgentVerProto12, *iSyncMLUserName, iSyncServer, *iDMServerId, iDMObserver );
		}
		else
		{
		iSyncMLCmds = CNSmlDMCmds::NewL( this, stringSessionID, KNSmlDMAgentVerProto, *iSyncMLUserName, iSyncServer, *iDMServerId, iDMObserver );		
		}		
		}

	iSyncMLCmds->ServerAuth()->SetSyncAgent( this );
	if( !(( iMediumType == KUidNSmlMediumTypeBluetooth || iMediumType == KUidNSmlMediumTypeUSB || iMediumType == KUidNSmlMediumTypeIrDA  )  
			&& iDMServerPassword->Length() == 0 ))	
		{
#ifndef __NO_SERVERAUTH
		iSyncMLCmds->ServerAuth()->SetAuthRequiredL();
#endif
		}
	if ( iSyncInitiation == EServerAlerted )
		{
		if ( iDMServerDigest )
		    {
		    iSyncMLCmds->ServerAuth()->SetCredentialL( *iDMServerDigest );    
		    }		
		}
	}

// ---------------------------------------------------------
// CNSmlDMAgent::CheckCommandsAreReceivedL()
// Check that server has sent all expected commands, 
// empty function in DM
// ---------------------------------------------------------
void CNSmlDMAgent::CheckCommandsAreReceivedL()
	{
	}	

//
// Authentications 
//

// ---------------------------------------------------------
// CNSmlDMAgent::SetAuthTypeL()	
// Set authentication type to DM Settings 
// ---------------------------------------------------------
void CNSmlDMAgent::SetAuthTypeL( CNSmlAgentBase::TAuthenticationType aAuthType ) const
	{
#ifndef	__MODULETEST
	CNSmlDMProfile* profile = CNSmlDMSettings::NewLC()->ProfileL( iProfileID );
	if ( !profile )
		{
		User::Leave( TNSmlError::ESmlErrorInSettings );
		}
	else
		{
		CleanupStack::PushL( profile );
		profile->SetIntValue( EDMProfileAuthenticationRequired, aAuthType );
		profile->SaveL();
		CleanupStack::PopAndDestroy( 2 ); //profile, settings
		}
#endif
	}

// ---------------------------------------------------------
// CNSmlDMAgent::AuthTypeL()
// Read Authentication type from DM settings
// ---------------------------------------------------------

CNSmlAgentBase::TAuthenticationType CNSmlDMAgent::AuthTypeL() const
	{
	CNSmlAgentBase::TAuthenticationType authType = CNSmlAgentBase::ENoAuth;
	#ifndef	__MODULETEST
	CNSmlDMProfile* profile = CNSmlDMSettings::NewLC()->ProfileL( iProfileID );
	if ( !profile )
		{
		User::Leave( TNSmlError::ESmlErrorInSettings );
		}
	else
		{
		CleanupStack::PushL( profile );
		authType = (CNSmlAgentBase::TAuthenticationType) profile->IntValue( EDMProfileAuthenticationRequired );
		CleanupStack::PopAndDestroy( 2 ); //profile, settings
		}
	#endif
	return authType;
	}


// ---------------------------------------------------------
// CNSmlDMAgent::FreeResources()
// Release allocated resources
// ---------------------------------------------------------
//
void CNSmlDMAgent::FreeResources()
	{
	delete iDMTriggerMessage;
	iDMTriggerMessage = NULL;
	delete iDMServerId;
	iDMServerId = NULL;
	delete iDMServerPassword;
	iDMServerPassword = NULL;
	delete iDMServerDigest;
	iDMServerDigest = NULL;
if(FeatureManager::FeatureSupported(KFeatureIdSapPolicyManagement))
{
	delete iNotifierObserver;
	iNotifierObserver = NULL;
}
	FreeBaseResources();
	}


//
// Error Handling functions
//
// ---------------------------------------------------------
// CNSmlDMAgent::FinaliseWhenErrorL()
// This function is called if sync is breaked down immediately (Leave errors)
// This function MUST NOT leave.
// ---------------------------------------------------------
void CNSmlDMAgent::FinaliseWhenErrorL()
	{
	DBG_FILE(_S8("CNSmlDMAgent::FinaliseWhenErrorL begins"));
	if ( iSyncMLCmds )
		{
		iSyncMLCmds->WriteMappingInfoToDbL();
		iSyncMLCmds->DisconnectFromOtherServers();
		}
	DBG_FILE(_S8("CNSmlDMAgent::FinaliseWhenErrorL ends"));
	}

// ---------------------------------------------------------
// CNSmlDMAgent::Interrupted()
// 
// ---------------------------------------------------------
TBool CNSmlDMAgent::Interrupted()
	{
	return iDMInterrupted;
	}

// ---------------------------------------------------------
// CNSmlDMAgent::Interrupt()
// Synchronisation with all databases is interrupted
// Synchronisation level error code to Sync Log.
// ---------------------------------------------------------
void CNSmlDMAgent::Interrupt( TInt aErrorCode, TBool aImmediatelyInterruption, TBool aStatusError )
	{
	if ( !Interrupted() || aStatusError )
		{
		// Error code is updated only once, except errors due to
		// error Status codes from the server
		iError->SetErrorCode( aErrorCode );
		iDMInterrupted = ETrue;
		iSessionAbortAlertRequest = ETrue;
		iDMImmediatelyInterruption = aImmediatelyInterruption;
		}
	}

// ---------------------------------------------------------
// CNSmlDMAgent::InterruptL()
// The function is implemented, because base class may use this one
// In the case of DM, appIndex does not point anything.
// ---------------------------------------------------------
void CNSmlDMAgent::InterruptL( TInt /*appIndex*/, TInt aErrorCode, TBool aImmediatelyInterruption, TBool aStatusError )
	{
	Interrupt( aErrorCode, aImmediatelyInterruption, aStatusError );
	}

// ---------------------------------------------------------
// CNSmlDMAgent::WriteWarningL()
// Sync level warning message to Sync Log
// ---------------------------------------------------------
void CNSmlDMAgent::WriteWarningL( TNSmlError::TNSmlAgentErrorCode aWarningCode )
	{
	iError->SetErrorCode( aWarningCode );
	}
// ---------------------------------------------------------
// CNSmlDMAgent::WriteWarningL()
//  Database level warning message to Sync Log  
// ---------------------------------------------------------
void CNSmlDMAgent::WriteWarningL( TInt /*appIndex*/, TNSmlError::TNSmlAgentErrorCode aWarningCode )
	{
	WriteWarningL( aWarningCode );
	}
// ---------------------------------------------------------
// CNSmlDMAgent::WriteWarningL()
// Database level warning message to Sync Log
// ---------------------------------------------------------
void CNSmlDMAgent::WriteWarningL(  TInt /*appIndex*/, TNSmlError::TNSmlSyncMLStatusCode aWarningCode )
	{
	WriteWarningL( (TNSmlError::TNSmlAgentErrorCode) (TInt) aWarningCode );
	}
// ---------------------------------------------------------
// CNSmlDMAgent::CheckServerStatusCodeL()
// Status code validating
// ---------------------------------------------------------
void CNSmlDMAgent::CheckServerStatusCodeL( TInt aEntryID )
	{
	TDesC8& cmd = iSyncMLCmds->ResponseController()->Cmd( aEntryID );
	TNSmlError::TNSmlSyncMLStatusCode status = STATIC_CAST( TNSmlError::TNSmlSyncMLStatusCode, iSyncMLCmds->ResponseController()->StatusCode( aEntryID ) );
	TBool error (EFalse );
			
	switch ( status )
		{
		case TNSmlError::ESmlStatusAuthenticationAccepted:       //212
			if ( cmd == KNSmlAgentSyncHdr ) 
				{ 
				iAlreadyAuthenticated = ETrue;
				}
			error = EFalse;
			break;
		case  TNSmlError::ESmlStatusUnauthorized: // 401
			if ( cmd == KNSmlAgentSyncHdr  ) 
				{
				if ( iSyncMLCmds->ResponseController()->ChalType( aEntryID ).Length() != 0 )
					{
					if ( AuthenticationRequirementL( *iSyncMLCmds->ResponseController(), aEntryID ) )
						{
						iChallengeRequest = ETrue;
						SaveIfNonceL( *iSyncMLCmds->ResponseController(), aEntryID );
						iSyncMLCmds->ResponseController()->ResetL();
						return;
						}
					else
						{
						error = ETrue;
						}
					}
				else
					{
					error = ETrue;
					}
				}
			break;
		case  TNSmlError::ESmlStatusClientAuthenticationRequired: // 407 
			if ( cmd == KNSmlAgentSyncHdr ) 
				{
				if ( AuthenticationRequirementL( *iSyncMLCmds->ResponseController(), aEntryID ) )
					{
					iChallengeRequest = ETrue;
					SaveIfNonceL( *iSyncMLCmds->ResponseController(), aEntryID );
					iSyncMLCmds->ResponseController()->ResetL();
					return;
					}
				else
					{
					error = ETrue;
					}
				}
			else
				{
				error = ETrue;
				}
			break;
		default:
			error = EFalse;
			break;
		}
	if ( cmd == KNSmlAgentSyncHdr )
		{
		SaveIfNonceL( *iSyncMLCmds->ResponseController(), aEntryID );
		}
	if ( error )
		{
		InterruptL( iSyncMLCmds->ResponseController()->AppIndex( aEntryID ), status, ETrue, ETrue );
		}
	return;
	}

// FOTA
// ---------------------------------------------------------
// CNSmlDMGenericAlert::NewL()
//	Creates a new generic alert object.
//	@param aFwMgmtUri. The uri which has been used in the exe command whose final result is 
//	about to be reported.
//	@param aMetaType. Meta/Type that should be used in the alert.
//	@param aMetaFormat. Meta/Format that should be used in the alert.
//	@param aFinalResult. The final result value which is reported to remote server.
//	@param aCorrelator. Correlator value used in the original exec command.
//	@return CNSmlDMGenericAlert*. New instance of this class.
// ---------------------------------------------------------
//
CNSmlDMGenericAlert* CNSmlDMGenericAlert::NewL ( const TDesC8& aFwMgmtUri, const TDesC8& aMetaType, const TDesC8& aMetaFormat, 
													TInt aFinalResult, const TDesC8& aFwCorrelator )
    {
	CNSmlDMGenericAlert* self = new (ELeave) CNSmlDMGenericAlert();
	CleanupStack::PushL( self );
    self->ConstructL ( aFwMgmtUri, aMetaType, aMetaFormat, aFinalResult, aFwCorrelator );
    CleanupStack::Pop(); //self
	return self;
    }

// ---------------------------------------------------------
// CNSmlDMGenericAlert::NewL()
//	Creates a new generic alert object.
//	@param aFwMgmtUri. The uri which has been used in the exe command whose final result is 
//	about to be reported.
//	@param aMetaType. Meta/Type that should be used in the alert.
//	@param aMetaFormat. Meta/Format that should be used in the alert.
//	@param aFinalResult. The final result value which is reported to remote server.
//	@param aCorrelator. Correlator value used in the original exec command.
//	@return CNSmlDMGenericAlert*. New instance of this class.
// ---------------------------------------------------------
//
CNSmlDMGenericAlert* CNSmlDMGenericAlert::NewL ( const TDesC8& aCorrelator, const RArray<CNSmlDMAlertItem>& aItemList )
    {
    CNSmlDMGenericAlert* self = new (ELeave) CNSmlDMGenericAlert();
    CleanupStack::PushL( self );
    _LIT8(KDummy, "dummy");
    self->ConstructL ( KDummy(), KDummy(), KDummy(), 0, aCorrelator, &aItemList ); 
    CleanupStack::Pop(); //self
    return self;
    }

// ---------------------------------------------------------
// CNSmlDMGenericAlert::CNSmlDMGenericAlert()
//	C++ destructor.
// ---------------------------------------------------------
//
CNSmlDMGenericAlert::~CNSmlDMGenericAlert()
    {
    delete iFwMgmtUri;
    delete iMetaType;
    delete iMetaFormat;
    delete iFwCorrelator;
    if (iDataItem)
        {
        iDataItem->Close();	
        }
    delete iDataItem;
    }


// ---------------------------------------------------------
// CNSmlDMGenericAlert::CNSmlDMGenericAlert()
//	C++ constructor.
// ---------------------------------------------------------
//
CNSmlDMGenericAlert::CNSmlDMGenericAlert()
    {
    }
// ---------------------------------------------------------
// CNSmlDMGenericAlert::ConstructL()
//	Symbian 2nd phase constructor 
// ---------------------------------------------------------
//
void CNSmlDMGenericAlert::ConstructL( const TDesC8& aFwMgmtUri, const TDesC8& aMetaType, const TDesC8& aMetaFormat, 
        TInt aFinalResult, const TDesC8& aFwCorrelator, const RArray<CNSmlDMAlertItem>* aItemList )
    {
    if(aFinalResult != 0)
        {
        iFwMgmtUri = aFwMgmtUri.AllocL();
        iMetaType = aMetaType.AllocL();
        iMetaFormat = aMetaFormat.AllocL();
        }
    iFwCorrelator = aFwCorrelator.AllocL();
    iFinalResult = aFinalResult;   


    TInt count = 0;
    if (aItemList)
    	count = aItemList->Count();

    if( count > 0)
        { 
        iDataItem = new (ELeave) RArray<CNSmlDMAlertItem>();   // new (ELeave) RArray<TDes8<255> > ();  

        for(TInt i =0; i< count ; i++)
            {    
            CNSmlDMAlertItem* tempItem = new (ELeave) CNSmlDMAlertItem ;
            CleanupStack::PushL( tempItem );
            tempItem->iSource = ((*aItemList)[i].iSource)->AllocL();
            tempItem->iTarget = ((*aItemList)[i].iTarget)->AllocL();
            tempItem->iMetaType = ((*aItemList)[i].iMetaType)->AllocL();
            tempItem->iMetaFormat = ((*aItemList)[i].iMetaFormat)->AllocL();
            tempItem->iMetaMark = ((*aItemList)[i].iMetaMark)->AllocL();
            tempItem->iData = ((*aItemList)[i].iData)->AllocL();
            iDataItem->AppendL(*tempItem);
            CleanupStack::PopAndDestroy( tempItem );
            }
        }   

    }

// FOTA end


// --------------------------------------------------------------------------
// CNSmlNotifierObserver::CNSmlNotifierObserver(TRequestStatus& aStatus, CNSmlThreadEngine* aEngine, CNSmlThreadParams& aParams)
// Constructor
// --------------------------------------------------------------------------
//	
CNSmlAgentNotifierObserver::CNSmlAgentNotifierObserver(TRequestStatus& aStatus, TInt aCreatorId)
: CActive(0), iCallerStatus(aStatus), iCreatorId(aCreatorId)
	{
	CActiveScheduler::Add(this);
	}

// --------------------------------------------------------------------------
// CNSmlNotifierObserver::~CNSmlNotifierObserver()
// Destructor
// --------------------------------------------------------------------------
//	
CNSmlAgentNotifierObserver::~CNSmlAgentNotifierObserver()
	{
	// StartNotifier called to avoid Notifier server panic, if 
	// notifier does not exist anymore.
	TBuf8<1> dummy;	
//	iNotifier.StartNotifier(KNullUid, dummy, dummy); // KNullUid should do also..
//	iNotifier.CancelNotifier( KUidNotifier );
//	iNotifier.Close();
	iNotifierTimeOut.Cancel();
	Cancel();
	}

// --------------------------------------------------------------------------
// CNSmlNotifierObserver::ConnectToNotifierL( const TSyncMLAppLaunchNotifParams& aParam )
// Starts connection with notifier
// --------------------------------------------------------------------------
//		
void CNSmlAgentNotifierObserver::ConnectToNotifierL( CNSmlAgentBase * aNSmlAgentBase)
	{
	iTimeOut = EFalse;
	iNSmlAgentBase = aNSmlAgentBase;
	
	if ( !IsActive() )
		{
		SetActive();
		}

    iNotifierTimeOut.LaunchNotifierTimer( this );
//    User::LeaveIfError( iNotifier.Connect() );
//   iNotifier.StartNotifierAndGetResponse( iStatus, KUidNotifier, data, iResBuf );
	}

// --------------------------------------------------------------------------
// CNSmlNotifierObserver::NotifierTimeOut()
// Starts notifier time out timer
// --------------------------------------------------------------------------
//
void CNSmlAgentNotifierObserver::NotifierTimeOut()
	{
	iTimeOut = ETrue;
	// StartNotifier called to avoid Notifier server panic, if 
	// notifier does not exist anymore.
	TBuf8<1> dummy;	
//	iNotifier.StartNotifier(KNullUid, dummy, dummy); // KNullUid should do also..
	
//	iNotifier.CancelNotifier( KUidNotifier );
	}

// --------------------------------------------------------------------------
// CNSmlNotifierObserver::DoCancel()
// From base class
// --------------------------------------------------------------------------
//	
void CNSmlAgentNotifierObserver::DoCancel()
	{
	}

// --------------------------------------------------------------------------
// CNSmlNotifierObserver::RunL()
// From base class
// --------------------------------------------------------------------------
//	
void CNSmlAgentNotifierObserver::RunL()
{
	    
    TInt ret = iStatus.Int();
    if ( ret == KErrCancel || iTimeOut )
    {
    	iNSmlAgentBase->DoOutsideCancel();
    }
    else
    {
	    if ( ret == KErrNone )
    	{	   
        
	   	
	    	// Check the response and error code. If there is a fail, dump the job.        
	    	// Also compare sid to creator id saved for current job to secure that listener owns the job.
/*		    if ( ret != KErrNone || sid != iThreadParams.iCurrentJob.CreatorId() )
	       	{
	        	if ( ret == KErrNone && sid != iThreadParams.iCurrentJob.CreatorId() )
	        	{
#ifndef __WINS__	        	
		        	ret = KErrPermissionDenied;
#endif	        	
	        	}
	        }*/
	    	
    	}
    }
    
	TRequestStatus* sStatus = &iCallerStatus;
	User::RequestComplete( sStatus, KErrNone );
}

// --------------------------------------------------------------------------
// CNSmlNotifierTimeOut::~CNSmlNotifierTimeOut()
// Constructor
// --------------------------------------------------------------------------
//
CNSmlAgentNotifierTimeOut::CNSmlAgentNotifierTimeOut()
:CActive(0)
	{
	CActiveScheduler::Add(this);
	}

// --------------------------------------------------------------------------
// CNSmlNotifierTimeOut::~CNSmlNotifierTimeOut()
// Destructor
// --------------------------------------------------------------------------
//
CNSmlAgentNotifierTimeOut::~CNSmlAgentNotifierTimeOut()
	{
	Cancel();
	iTimeOutTimer.Close();
	}

// --------------------------------------------------------------------------
// CNSmlNotifierTimeOut::LaunchNotifierTimer( CNSmlNotifierObserver*  aObserver )
// Starts notifier time out timer
// --------------------------------------------------------------------------
//
void CNSmlAgentNotifierTimeOut::LaunchNotifierTimer( CNSmlAgentNotifierObserver*  aObserver )
	{
	
	if ( !IsActive() )
		{
		SetActive();
		}
		
	iTimeOutTimer.CreateLocal();
	iTimeOutTimer.After( iStatus, KNotifierTimeout );
	iObserver = aObserver;
	}
	
// --------------------------------------------------------------------------
// CNSmlNotifierTimeOut::DoCancel()
// From base class 
// --------------------------------------------------------------------------
//
void CNSmlAgentNotifierTimeOut::DoCancel()
	{
	iTimeOutTimer.Cancel();
	}

// --------------------------------------------------------------------------
// CNSmlNotifierTimeOut::RunL()
// From base class 
// --------------------------------------------------------------------------
//
void CNSmlAgentNotifierTimeOut::RunL()
	{
	if ( iStatus.Int() != KErrCancel )
		{
		iObserver->NotifierTimeOut();	
		}
	}



// ---------------------------------------------------------
// CreateDMAgentL( MNSmlDMAgentObserver* aDMObserver )
// Return the instance of the CNSmlDMAgent
// ---------------------------------------------------------
//
EXPORT_C CNSmlDMAgent* CreateDMAgentL( MSyncMLProgressObserver* aDMObserver )
	{
	return CNSmlDMAgent::NewL( aDMObserver );
	}

//  End of File  

