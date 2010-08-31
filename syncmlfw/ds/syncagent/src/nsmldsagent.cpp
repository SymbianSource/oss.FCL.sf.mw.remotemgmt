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
* Description:  DS Sync flow
*
*/

#define SYNCML_V3
// INCLUDE FILES
#include <SyncMLHistory.h>
#include <e32property.h>
#include <DataSyncInternalPSKeys.h>
#include <nsmlconstants.h>
#include <nsmldebug.h>
#include <cmpluginwlandef.h>
#include <cmmanager.h>
#include <cmconnectionmethod.h>

// common includes with DM
#include "nsmlagenttestdefines.h"
#include "nsmlcliagconstants.h"
#include "NSmlAgentBase.h" 
#include "NSmlCmdsBase.h"
#include "NSmlStatusContainer.h"
#include "NSmlResponseController.h"
#include "NSmlURI.h"
#include "NSmlHistoryArray.h"

// DS specific includes
#include "nsmldsagconstants.h"
#include "NSmlDSAgent.h"
#include "NSmlDSCmds.h"
#include "nsmldscontent.h"
#include "nsmldserror.h"
#include "nsmldssettings.h"
#include "nsmldsoperatorsettings.h"
#include "nsmlagentlog.h"
#include "nsmlroam.h"
#include "nsmloperatorerrorcrkeys.h"
//RD_AUTO_RESTART
#include <e32base.h>
#include <centralrepository.h> 
#include "SyncMLErr.h"
#include "nsmldsconstants.h"
//RD_AUTO_RESTART
_LIT( KNetMon,"\\netmon.exe" );
_LIT( KAutoRestart,"netmon" );

#ifndef __WINS__
// This lowers the unnecessary compiler warning (armv5) to remark.
// "Warning:  #174-D: expression has no effect..." is caused by 
// DBG_ARGS8 macro in no-debug builds.
#pragma diag_remark 174
#endif

// CONSTANTS
const TInt KGranularity = 12;
const TInt KErrorCodeRangeFirst = 400;
const TInt KErrorCodeRangeLast = 516;

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CNSmlDSAgent::CNSmlDSAgent
// C++ constructor.
// -----------------------------------------------------------------------------
//
CNSmlDSAgent::CNSmlDSAgent(): 
        iServerStatusCodeArray ( RArray< TInt >( KGranularity ) )
    {
	}

// -----------------------------------------------------------------------------
// CNSmlDSAgent::ConstructL
// Symbian 2nd phase constructor.
// -----------------------------------------------------------------------------
//
void CNSmlDSAgent::ConstructL( MSyncMLProgressObserver* aDSObserver )
    {
    DBG_FILE(_S8("CNSmlDSAgent::ConstructL begins")); 
	CNSmlAgentBase::ConstructL();
	iDSObserver = aDSObserver;
	iDSContent = CNSmlDSContent::NewL();
	iError = new ( ELeave ) TNSmlDSError;
	iSyncMLMIMEType = KNSmlDSAgentMIMEType().AllocL();
	iResultAlertCode = KNSmlDSAgentResultAlert().AllocL();
	iNextMessageCode = KNSmlDSAgentNextMessage().AllocL();
	iEndOfDataAlertCode = KNSmlDSAgentNoEndOfData().AllocL();
	iUpdateLastAnchor=ETrue;
	
	//RD_AUTO_RESTART
	//Check whether the Auto Restart Feature is enabled or not 
	TInt keyVal;
	iAutoRestartInitiatedSync = EFalse;
	TRAPD (err ,ReadRepositoryL(KNsmlDsAutoRestart, keyVal));
	if(err==KErrNone && keyVal==1)
	{	
		iDSNetmon = CNsmlDSNetmon::NewL(*this);  
		DBG_FILE(_S8("CNSmlDSAgent::ConstructL DSNetmon is Launched"));
		
		CNsmlProfileUtil* profileUtil = CNsmlProfileUtil::NewLC();	
		profileUtil->IsValidResumeL(iAutoRestartInitiatedSync);	
		profileUtil->WriteInterruptFlagL(0);
		CleanupStack::PopAndDestroy(profileUtil);
	}
	else
	{
		iDSNetmon = NULL;
		DBG_FILE(_S8("CNSmlDSAgent::ConstructL DSNetmon is not Launched"));
	}
	//RD_AUTO_RESTART
	
    iRepositorySSC = CRepository::NewL( KCRUidOperatorDatasyncErrorKeys );
    CNSmlDSOperatorSettings* settings = CNSmlDSOperatorSettings::NewLC();
    iErrorReportingEnabled = settings->SyncErrorReportingEnabled();
    if ( iErrorReportingEnabled )
        {
        settings->PopulateStatusCodeListL( iServerStatusCodeArray );
        }
    CleanupStack::PopAndDestroy( settings );
	
	// security policies for P&S reading and writing
	_LIT_SECURITY_POLICY_S0( KNSmlPSWritePolicy, KNSmlSOSServerPolicyUID.iUid ); // SID check (sosserver) when writing
	_LIT_SECURITY_POLICY_PASS( KNSmlPSReadPolicy ); // no checks done when reading
	
	// Define P&S data field for sync ON/OFF flag (may fail e.g. if exists already)
	RProperty::Define( KPSUidDataSynchronizationInternalKeys, // category
                       KDataSyncStatus, // field
                       RProperty::EInt, // type
                       KNSmlPSReadPolicy, // read policy
                       KNSmlPSWritePolicy ); // write policy
	DBG_FILE(_S8("CNSmlDSAgent::ConstructL ends"));                        
	
	}

// -----------------------------------------------------------------------------
// CNSmlDSAgent::NewL
// Symbian two-phased constructor.
// -----------------------------------------------------------------------------
//
CNSmlDSAgent* CNSmlDSAgent::NewL( MSyncMLProgressObserver* aDSObserver )
    {
	CNSmlDSAgent* self = new (ELeave) CNSmlDSAgent();
	CleanupStack::PushL( self );
    self->ConstructL( aDSObserver );
	CleanupStack::Pop(); // self
    return self;
    }

// -----------------------------------------------------------------------------
// CNSmlDSAgent::~CNSmlDSAgent
// Destructor.
// -----------------------------------------------------------------------------
//
CNSmlDSAgent::~CNSmlDSAgent()
    {   
    delete iSyncLog;
	delete iDSContent;	
	delete iDSServerId;	
	delete iServerAlertPackage;
		
	if ( iDSSyncTypeArray )
		{	
		iDSSyncTypeArray->Reset();		
		delete iDSSyncTypeArray;
		}	
	//RD_AUTO_RESTART
	if(iDSNetmon)
	{
		delete iDSNetmon;		
		iDSNetmon = NULL;
	}
    //RD_AUTO_RESTART
	
    iServerStatusCodeArray.Close();
    delete iRepositorySSC;
	}

// -----------------------------------------------------------------------------
// CNSmlDSAgent::Synchronise
// Starts the sync, returns immediately and the initiated sync continues in the 
// RunL() function.
// -----------------------------------------------------------------------------
//
EXPORT_C void CNSmlDSAgent::Synchronise( TRequestStatus& aStatus,
		                      			 TSmlProfileId aProfileId,
		                      			 TSmlConnectionId aConnectionId,
							  			 CArrayFix<TNSmlContentSpecificSyncType>* aTypeArray,
							  			 TNSmlSyncInitiation aSyncInitiation,
							  			 TInt aSessionId,
							  			 HBufC8* aAlertPackage )
	{
	
	CActiveScheduler::Add( this );
	
	iCallerStatus = &aStatus;
	iSyncInitiation = aSyncInitiation;
	iProfileID = aProfileId;
	iMediumType.iUid = aConnectionId;
	iSessionId = aSessionId;
	
	*iCallerStatus = KRequestPending;
	
	TRAPD( err, PrepareSyncL( aTypeArray ) );
	
	if ( err != KErrNone )
		{
		DoErrorEvent( MSyncMLProgressObserver::ESmlFatalError, err, iDSContent->TaskId(), 0, 0 );
		User::RequestComplete( iCallerStatus, err );
		return;
		}
		
	if ( iSyncInitiation == EServerAlerted && aAlertPackage )
		{
		TRAP( err, iServerAlertPackage = aAlertPackage->AllocL() );
		
		if ( err != KErrNone )
			{
			DoErrorEvent( MSyncMLProgressObserver::ESmlFatalError, err, iDSContent->TaskId(), 0, 0 );
			User::RequestComplete( iCallerStatus, err );
			return;
			}
		}

	SetActive();
	TRequestStatus* status = &iStatus;
	User::RequestComplete( status, KErrNone );
	}

//
//  Private member functions
//
//
//  Begin state sub-functions 
//
// ---------------------------------------------------------
// CNSmlDSAgent::BeginSubStartStateL()
// 
// ---------------------------------------------------------
void CNSmlDSAgent::BeginSubStartStateL()
	{
	DBG_FILE(_S8("CNSmlDSAgent::BeginSubStartStateL() begins"));

	iDSObserver->OnSyncMLSyncProgress( MSyncMLProgressObserver::ESmlConnecting, 0, 0 );
	iDSCurrBeginSubState = EBeginSubReadSettings;
	
	DBG_FILE(_S8("CNSmlDSAgent::BeginSubStartStateL() ends"));
	}
	
// ---------------------------------------------------------
// CNSmlDSAgent::BeginSubReadSettingsStateL()
// 
// ---------------------------------------------------------
void CNSmlDSAgent::BeginSubReadSettingsStateL()
	{
	DBG_FILE(_S8("CNSmlDSAgent::BeginSubReadSettingsStateL() begins"));
	
	ReadSettingsL();
	iDSCurrBeginSubState = EBeginSubSyncmlCmdsInstance;
	
	DBG_FILE(_S8("CNSmlDSAgent::BeginSubReadSettingsStateL() ends"));
	}
	
// ---------------------------------------------------------
// CNSmlDSAgent::BeginSubSyncmlCmdsInstanceStateL()
// 
// ---------------------------------------------------------
void CNSmlDSAgent::BeginSubSyncmlCmdsInstanceStateL()
	{
	DBG_FILE(_S8("CNSmlDSAgent::BeginSubSyncmlCmdsInstanceStateL() begins"));
	CreateSyncmlCmdsInstanceL();
	DBG_FILE(_S8("CNSmlDSAgent::BeginSubSyncmlCmdsInstanceStateL() ends"));
	iDSCurrBeginSubState = EBeginSubServerAlerting;
	}
	
// ---------------------------------------------------------
// CNSmlDSAgent::BeginSubServerAlertingStateL()
// 
// ---------------------------------------------------------
void CNSmlDSAgent::BeginSubServerAlertingStateL()
	{
	DBG_FILE(_S8("CNSmlDSAgent::BeginSubServerAlertingStateL() begins"));
	
	if ( iSyncInitiation == TNSmlSyncInitiation( EServerAlerted ) )
		{
		DBG_FILE(_S8("CNSmlDSAgent::BeginSubServerAlertingStateL() Before ServerAlertStateL"));
		ServerAlertStateL();
		}
		
	iDSContent->InitAllSyncLogEventsL( *iSyncLog );
	iDSCurrBeginSubState = EBeginSubOpenContentInstances; 
	
	DBG_FILE(_S8("CNSmlDSAgent::BeginSubServerAlertingStateL() ends"));
	}
	
// ---------------------------------------------------------
// CNSmlDSAgent::BeginSubOpenContentInstancesStateL()
// 
// ---------------------------------------------------------
void CNSmlDSAgent::BeginSubOpenContentInstancesStateL()
	{
	DBG_FILE(_S8("CNSmlDSAgent::BeginSubOpenContentInstancesStateL() begins"));
	TBool endOfOpening = EFalse; 
	OpenDatabaseInstancesL( endOfOpening );
	if ( endOfOpening )
		{
		// <MAPINFO_RESEND_MOD_BEGIN>
		iDSCurrBeginSubState = EBeginSubScanMapInfo;
		// iDSCurrBeginSubState = EBeginSubCreateLUIDBuffers;
		// <MAPINFO_RESEND_MOD_END>
		}
	DBG_FILE(_S8("CNSmlDSAgent::BeginSubOpenContentInstancesStateL() ends"));
	}
	
// ---------------------------------------------------------
// CNSmlDSAgent::BeginSubCreateLUIDBuffersStateL()
// 
// ---------------------------------------------------------
void CNSmlDSAgent::BeginSubCreateLUIDBuffersStateL()
	{
	DBG_FILE(_S8("CNSmlDSAgent::BeginSubCreateLUIDBuffersStateL() begins"));
	TBool endOfCreating( EFalse );
	
	CreateLUIDBuffersL( endOfCreating );
	
	if ( endOfCreating )
		{
		iCurrBeginningState = EBeginConnectingToServer;
		}
		
	DBG_FILE(_S8("CNSmlDSAgent::BeginSubCreateLUIDBuffersStateL() ends"));
	}

//
//  Server Alert state functions 
//

// -----------------------------------------------------------------------------
// CNSmlDSAgent::ServerAlertStateL
// Marks server alerted databases as alerted
// -----------------------------------------------------------------------------
//
void CNSmlDSAgent::ServerAlertStateL()	
	{
	// parse syncml 1.1 server alert package
	if ( iProtocolVer == ESmlVersion1_1_2 )
		{
		TPtr8 buffer( iSyncMLCmds->BufferAreaForParsingL() );
		
		if ( iServerAlertPackage->Length() > buffer.MaxLength() )
			{
			return;
			}
			
		buffer.Copy( *iServerAlertPackage );
		delete iServerAlertPackage;
		iServerAlertPackage = NULL;
		
		iDSCurrServerAlertState = EServerAlertWaitingStartMessage;
		
		while ( iSyncMLCmds->ProcessReceivedDataL() ) { }
			
		if ( iDSCurrServerAlertState != EServerAlertMessageReceived )
			{
			User::Leave( TNSmlError::ESmlIncompleteMessage );
			}
		}
		
	iDSContent->RemoveNotServerAlertedContents();
	
	if ( iProtocolVer == ESmlVersion1_2 )
		{
		if ( iDSContent->AllDatabasesAreInterrupted() )
			{
			if ( iError->SyncLogErrorCode() )
				{
				User::Leave( iError->SyncLogErrorCode() );
				}
			else
				{
				User::Leave( TNSmlDSError::ESmlDSNoValidDatabases );
				}
			}
		}
	}

// -----------------------------------------------------------------------------
// CNSmlDSAgent::ServerAlertStartMessageStateL
// Handles SyncHdr in Server Alert Package from server.
// -----------------------------------------------------------------------------
//
void CNSmlDSAgent::ServerAlertStartMessageStateL( SmlSyncHdr_t* aSyncHdr )
	{
	if ( iDSCurrServerAlertState != EServerAlertWaitingStartMessage )
		{
		User::Leave( TNSmlError::ESmlUnexpectedCmd ); 
		}
		
	iSyncMLCmds->ProcessSyncHdrL( aSyncHdr );
	
	iDSCurrServerAlertState = EServerAlertWaitingAlerts;
	}

// -----------------------------------------------------------------------------
// CNSmlDSAgent::ServerAlertAlertCmdStateL
// Handles Alert command in Server Alert Package from server.
// -----------------------------------------------------------------------------
//
void CNSmlDSAgent::ServerAlertAlertCmdStateL( SmlAlert_t* aAlert )
	{
	if ( iDSCurrServerAlertState != EServerAlertWaitingAlerts ) 
		{
		User::Leave( TNSmlError::ESmlUnexpectedCmd ); 
		}

	if ( iSyncMLCmds->AlertCode( aAlert ) == KNSmlAgentDisplayAlert )
		{
		iSyncMLCmds->ProcessAlertCmdL( aAlert, EFalse, EFalse, ETrue );
		}
	else
		{
		iSyncMLCmds->ProcessAlertCmdL( aAlert, EFalse, ETrue );
		}
	}

// -----------------------------------------------------------------------------
// CNSmlDSAgent::ServerAlertEndMessageStateL
// Processes the end of the alert command in server alert package.
// -----------------------------------------------------------------------------
//
void CNSmlDSAgent::ServerAlertEndMessageStateL( TBool /*aFinal*/ )
	{
	if ( iDSCurrServerAlertState != EServerAlertWaitingAlerts ) 
		{
		User::Leave( TNSmlError::ESmlUnexpectedCmd ); 
		}

	iDSCurrServerAlertState = EServerAlertMessageReceived;
	}

//
//  Client Initialisation state functions 
//

// ---------------------------------------------------------
// CNSmlDSAgent::InitStartingMessageStateL()
// "Initialising" event to UI, initialises interruption flags
// set by possible first initialisation attempt, makes SyncHdr  
// ---------------------------------------------------------
void CNSmlDSAgent::InitStartingMessageStateL()
	{
	iDSContent->InitInterruptionFlags();
	iDSContent->SetToFirst();
	InitStartingMessageStateBaseL();
	}
	
//RD_SUSPEND_RESUME	
// -----------------------------------------------------------------------------
// CNSmlDSAgent::MapAlertCode
// Maps the Alert code
// -----------------------------------------------------------------------------
// 
TPtrC8 CNSmlDSAgent::MapAlertCode(TNSmlPreviousSyncType aAlertCode)

{
   TPtrC8 prevAlert;
   if ( aAlertCode == ESyncDSTwoWay )
		{
		prevAlert.Set( KNSmlDSTwoWay);
		}
   else if ( aAlertCode == ESyncDSSlowSync)
		{
		prevAlert.Set(KNSmlDSSlowSync);
		}
   else if ( aAlertCode == ESyncDSOneWayFromClient)
		{
		prevAlert.Set(KNSmlDSOneWayFromClient);
		}
	else if ( aAlertCode == ESyncDSRefreshFromClient)
		{
		prevAlert.Set(KNSmlDSRefreshFromClient);
		}
	else if ( aAlertCode == ESyncDSOneWayFromServer  )
		{
		prevAlert.Set(KNSmlDSOneWayFromServer);
		}
	else if(aAlertCode ==ESyncDSRefreshFromServer )
	   {
		prevAlert.Set(KNSmlDSRefreshFromServer);
	   }		
	else if(aAlertCode ==ESyncDSTwoWayByServer )
	   {
		prevAlert.Set(KNSmlDSTwoWayByServer);
	   }   
    else if(aAlertCode ==ESyncDSOneWayFromClientByServer )
	   {
		prevAlert.Set(KNSmlDSOneWayFromClientByServer);
	   }
    else if(aAlertCode ==ESyncDSRefreshFromClientByServer )
	   {
		prevAlert.Set(KNSmlDSRefreshFromClientByServer);
	   }
    else if(aAlertCode ==ESyncDSOneWayFromServerByServer )
	   {
		prevAlert.Set(KNSmlDSOneWayFromServerByServer);
	   }
   else if(aAlertCode ==ESyncDSRefreshFromServerByServer )
	   {
		prevAlert.Set(KNSmlDSRefreshFromServerByServer);
	   }
  
  return(prevAlert);
	   	
	
}
//RD_SUSPEND_RESUME	

// CNSmlDSAgent::InitAlertingStateL
// Makes Alert command for each database. If there is a previously completed
// sync the client suggests two-way sync else slow sync is proposed.
// -----------------------------------------------------------------------------
//
void CNSmlDSAgent::InitAlertingStateL()
	{

	if ( iDSClientNextSyncAnchor == TTime( 0 ) )
		{
		 iDSClientNextSyncAnchor.UniversalTime();
		}
	
	if ( iDSContent->SetToFirst() )	
		{
	
		 do
			{		
			   TBuf8<3> syncType( iDSContent->SyncType() ); 
			   TTime clientLastSyncAnchor( iDSContent->LastSyncAnchorL() );
			   TNSmlPreviousSyncType prevSyncType=iDSContent->PreviousClientSyncTypeL();
			   iDSContent->SetSessionResumedL(EFalse);		   
			    if (!iDSContent->Interrupted())
				{
				   	 //RD_SUSPEND_RESUME	
				    if(iDSContent->SuspendedStateL() == EStateNotStarted)
				     {				   				   	
				   	    if ( syncType == KNSmlDSTwoWay )
					    {
					   		// <MAPINFO_RESEND_MOD_BEGIN>
							if ( ( !iDSContent->HasSyncHistoryL() || iDSContent->SlowSyncRequestL() || clientLastSyncAnchor == TTime( 0 ) )&& !iDSContent->MapExists() )
							// if ( !iDSContent->HasSyncHistoryL() || iDSContent->SlowSyncRequestL() || clientLastSyncAnchor == TTime( 0 )   )
							// <MAPINFO_RESEND_MOD_BEGIN>
							{								
								syncType = KNSmlDSSlowSync;
							
							}
                		 }
                	  }
				    //checks for protocol version, stage of interruption ,sync history to send resume command
			        else if (iProtocolVer==ESmlVersion1_2 && iDSContent->HasSyncHistoryL() &&  
			                          prevSyncType!=ETypeNotSet && syncType!= KNSmlDSRefreshFromServer)
				     { 
				         //Client rejects to the Resume session when the Sync type is changed in the UI
				      	 //Based on the section "Refusing to Resume a session" in the OMA-Protocol spec
				      	 //Two way sync will be proposed by the client instead of the Resume session
				         if(syncType.Compare(MapAlertCode(prevSyncType))==0)
			  	            {
			  	               	 syncType = KNSmlDSResume;
			  	               	 iDSContent->SetSessionResumedL(ETrue);
			  	            }  
			  	         else 
			  	            {
			  	            	syncType=KNSmlDSSlowSync;
			  	            	iDSContent->SetSessionResumedL(EFalse);
			  	            }
					  }
					 //RD_SUSPEND_RESUME
                
				   iSyncMLCmds->DoAlertL( syncType, &clientLastSyncAnchor, &iDSClientNextSyncAnchor );
				   iDSContent->SetInitAlertWaiting();  	
				   	   	
			    }
			   	
		
			}	
		  while ( iDSContent->SetToNext() );
		}
	
	iCurrClientInitState = EInitDeviceInfo;
	}
	
// -----------------------------------------------------------------------------
// CNSmlDSAgent::InitDeviceInfoStateL
// If there is some database which is not previously synced with the server then
// DevInfo is sent in a Put command.
// -----------------------------------------------------------------------------
//
void CNSmlDSAgent::InitDeviceInfoStateL()
	{
	if ( iDSContent->SetToFirst() )
		{
		do
			{
			if ( ( !iDSContent->HasSyncHistoryL() || iDSContent->LastSyncAnchorL() == TTime( 0 ) ) && !iDSContent->Interrupted() )
				{
				iSyncMLCmds->DoPutL();
				iDSDeviceInfoRequestByClient = ETrue;
				break;
				}
			}
		while ( iDSContent->SetToNext() );
		}

	iCurrClientInitState = EInitAskingDeviceInfo;
	}
	
// ---------------------------------------------------------
// CNSmlDSAgent::InitAskingDeviceInfoStateL()
// If there is one database which is not previosly synced with the
// server, Device Info is asked Get command
// ---------------------------------------------------------
void CNSmlDSAgent::InitAskingDeviceInfoStateL()
	{
	if ( iDSDeviceInfoRequestByClient )
		{
		iSyncMLCmds->DoGetL();
		}
	iCurrClientInitState = EInitEndingMessage;
	}
	
//
//  Server Initialisation state functions 
//
// ---------------------------------------------------------
// CNSmlDSAgent::ServerInitialisationStateL()
// Main state function to read Sync Initialization Package from Server 
// ---------------------------------------------------------
void CNSmlDSAgent::ServerInitialisationStateL()
	{
	//DBG_FILE(_S8("CNSmlDSAgent::ServerInitialisationStateL begins"));
	
	TBool messageReceived( EFalse );
	
	if ( iDSCurrServerInitState == EServerInitEnd )
		{
		messageReceived = ETrue;
		}
	else
		{
		messageReceived = !iSyncMLCmds->ProcessReceivedDataL();
		}
		
	if ( messageReceived )
		{
		switch ( iDSCurrServerInitState )
			{
			case EServerInitMessageReceived:
				{			
				iBusyStatusReceived = iSyncMLCmds->BusyStatus();
				NextMainStateL();
				break;
				}
				
			case EServerInitPackageReceived:
				{
				if ( !iChallengeRequest )
					{
					CheckStatusCodesAreReceivedL();
					CheckCommandsAreReceivedL();
					TInt retVal( iDSContent->SetTypeAndDevCapsL() );
					
					if ( retVal != KErrNone )
						{
						InterruptL( iDSContent->CurrentIndex(), TNSmlError::ESmlTypeNotSupported, EFalse, EFalse );
						}
					}
					
				iDSLUIDBufferingStarted = EFalse;
				iDSCurrServerInitState = EServerInitEnd;
				break;
				}
				
			case EServerInitEnd:
				{
				TBool endOfInits( EFalse );
				TInt resultCode( KErrNone );

				iDSContent->SetDbAndTimeInitsL( endOfInits, resultCode, iDSLUIDBufferingStarted );
				iDSLUIDBufferingStarted = ETrue;
					
				User::LeaveIfError( resultCode );
				
				if ( endOfInits )
					{
					NextMainStateL();
					}
				break;
				}
				
			default:		
				{
				User::Leave( TNSmlError::ESmlIncompleteMessage );
				}
			}
		}
	//DBG_FILE(_S8("CNSmlDSAgent::ServerInitialisationStateL ends"));
	}
	
// ---------------------------------------------------------
// CNSmlDSAgent::ServerInitStartMessageStateL()
// Handles SyncHdr in Initialization Package from Server 
// ---------------------------------------------------------
void CNSmlDSAgent::ServerInitStartMessageStateL( SmlSyncHdr_t* aSyncHdr )
	{
	DBG_FILE(_S8("CNSmlDSAgent::ServerInitStartMessageStateL begins"));
	if ( iDSCurrServerInitState != EServerInitWaitingStartMessage )
		{
		User::Leave( TNSmlError::ESmlUnexpectedCmd ); 
		}
	iSyncMLCmds->ProcessSyncHdrL( aSyncHdr );
	iDSCurrServerInitState = EServerInitWaitingCommands;
	DBG_FILE(_S8("CNSmlDSAgent::ServerInitStartMessageStateL ends"));
	}
	
// ---------------------------------------------------------
// CNSmlDSAgent::ServerInitStatusCmdStateL()
// Handles Status command in Initialization Package from Server 
// ---------------------------------------------------------
void CNSmlDSAgent::ServerInitStatusCmdStateL( SmlStatus_t* aStatus )
	{
	DBG_FILE(_S8("CNSmlDSAgent::ServerInitStatusCmdStateL begins"));
	if ( iChallengeRequest  )
		{
		return;
		}
	if ( iDSCurrServerInitState != EServerInitWaitingCommands ) 
		{
		User::Leave( TNSmlError::ESmlUnexpectedCmd ); 
		}
	DBG_FILE(_S8("CNSmlDSAgent::ServerInitStatusCmdStateL ends"));
	iSyncMLCmds->ProcessStatusCmdL( aStatus );
	}
	
// ---------------------------------------------------------
// CNSmlDSAgent::ServerInitResultsCmdStateL()
// Handles Results command in Initialization Package from Server 
// ---------------------------------------------------------
void CNSmlDSAgent::ServerInitResultsCmdStateL( SmlResults_t* aResults )
	{
	DBG_FILE(_S8("CNSmlDSAgent::ServerInitResultsCmdStateL begins"));
	if ( iChallengeRequest  )
		{
		return;
		}
	if ( iDSCurrServerInitState != EServerInitWaitingCommands ) 
		{
		User::Leave( TNSmlError::ESmlUnexpectedCmd ); 
		}
	if ( !iDSDeviceInfoRequestByClient )
		{
		User::Leave( TNSmlError::ESmlUnexpectedCmd ); 
		}
	else
		{
		iSyncMLCmds->ProcessResultsCmdL( aResults );
		iDSDeviceInfoRequestByClient = EFalse;
		}
	DBG_FILE(_S8("CNSmlDSAgent::ServerInitResultsCmdStateL ends"));
	}
	
// ---------------------------------------------------------
// CNSmlDSAgent::ServerInitAlertCmdStateL()
// Handles Alert command in Initialization Package from Server 
// ---------------------------------------------------------
void  CNSmlDSAgent::ServerInitAlertCmdStateL( SmlAlert_t* aAlert )
	{
	DBG_FILE(_S8("CNSmlDSAgent::ServerInitAlertCmdStateL begins"));
	if ( iChallengeRequest  )
		{
		return;
		}
	if ( iDSCurrServerInitState != EServerInitWaitingCommands ) 
		{
		User::Leave( TNSmlError::ESmlUnexpectedCmd ); 
		}
	// Alert command may be init Alert, Next Alert or Display Alert
	TBool nextAlert = EFalse;
	TBool displayAlert = EFalse;

	if ( iSyncMLCmds->AlertCode( aAlert ) == KNSmlDSAgentNextMessage )
		{
		if ( !iFinalMessageFromClient )
			{
			nextAlert = ETrue;
			}
		}
	else
		{
		if ( iSyncMLCmds->AlertCode( aAlert ) == KNSmlAgentDisplayAlert )
			{
			displayAlert = ETrue;
			}
		}

	iSyncMLCmds->ProcessAlertCmdL( aAlert, nextAlert, EFalse, displayAlert );
	DBG_FILE(_S8("CNSmlDSAgent::ServerInitAlertCmdStateL ends"));
	}
	
// ---------------------------------------------------------
// CNSmlDSAgent::ServerInitPutCmdStateL()
// Handles Put command in Initialization Package from Server 
// ---------------------------------------------------------
void CNSmlDSAgent::ServerInitPutCmdStateL( SmlPut_t* aPut )
	{
	DBG_FILE(_S8("CNSmlDSAgent::ServerInitPutCmdStateL begins"));
	if ( iChallengeRequest  )
		{
		return;
		}
	if ( iDSCurrServerInitState != EServerInitWaitingCommands ) 
		{
		User::Leave( TNSmlError::ESmlUnexpectedCmd ); 
		}
	iSyncMLCmds->ProcessPutCmdL( aPut );
	DBG_FILE(_S8("CNSmlDSAgent::ServerInitPutCmdStateL ends"));
	}
	
// ---------------------------------------------------------
// CNSmlDSAgent::ServerInitGetCmdStateL()
// Handles Get command in Initialization Package from Server 
// ---------------------------------------------------------
void CNSmlDSAgent::ServerInitGetCmdStateL( SmlGet_t* aGet )
	{
	DBG_FILE(_S8("CNSmlDSAgent::ServerInitGetCmdStateL begins"));
	if ( iChallengeRequest  )
		{
		return;
		}
	if ( iDSCurrServerInitState != EServerInitWaitingCommands ) 
		{
		User::Leave( TNSmlError::ESmlUnexpectedCmd ); 
		}
	iSyncMLCmds->ProcessGetCmdL( aGet );
	iDSDeviceInfoRequestedByServer = ETrue;
	DBG_FILE(_S8("CNSmlDSAgent::ServerInitGetCmdStateL ends"));
	}
	
// ---------------------------------------------------------
// CNSmlDSAgent::ServerInitEndMessageStateL()
// 
// ---------------------------------------------------------
void  CNSmlDSAgent::ServerInitEndMessageStateL( TBool aFinal )
	{
	DBG_FILE(_S8("CNSmlDSAgent::ServerInitEndMessageStateL begins"));
	if ( iDSCurrServerInitState != EServerInitWaitingCommands ) 
		{
		User::Leave( TNSmlError::ESmlUnexpectedCmd ); 
		}
	if ( aFinal )
		{
		iDSCurrServerInitState = EServerInitPackageReceived;
		}
	else
		{
		iDSCurrServerInitState = EServerInitMessageReceived;
		}
	DBG_FILE(_S8("CNSmlDSAgent::ServerInitEndMessageStateL ends"));
	}
	
//
//  Client Modifications state functions 
//
// ---------------------------------------------------------
// CNSmlDSAgent::ClientModificationsStateL()
// Makes Client Modification Package to Server
// Navigate according to state code
// ---------------------------------------------------------
void CNSmlDSAgent::ClientModificationsStateL()
	{
	switch ( iDSCurrClientModState )
		{
		case EClientModStartingMessage:
			ClientModStartingMessageStateL();
			break;
		case EClientModMakingStatus:
			ClientModMakingStatusStateL();
			break;
		case EClientModMakingResults:
			ClientModMakingResultsStateL();
			break;
		case EClientModStartingSync:
			ClientModStartingSyncStateL();
			break;
		case EClientModMakingUpdates:
			ClientModMakingUpdatesStateL();
			break;
		case EClientModEndingSync:
			ClientModEndingSyncStateL();
			break;
		case EClientModEndingMessage:
			ClientModEndingMessageStateL();
			break;
		default:
			User::Leave( KErrGeneral );
		} 
	}
	
// ---------------------------------------------------------
// CNSmlDSAgent::ClientModStartingMessageStateL()
// "Synchronising" event to UI, makes SyncHdr  
// ---------------------------------------------------------
void CNSmlDSAgent::ClientModStartingMessageStateL()
	{
	DBG_FILE(_S8("CNSmlDSAgent::ClientModStartingMessageStateL begins"));
	iSyncMLCmds->DoSyncHdrL();
	iBufferFull = EFalse;
	iDSCurrClientModState = EClientModMakingStatus;
	DBG_FILE(_S8("CNSmlDSAgent::ClientModStartingMessageStateL ends"));
	}
	
// ---------------------------------------------------------
// CNSmlDSAgent::ClientModMakingStatusStateL()
// Writes all buffered status elements
// ---------------------------------------------------------
void CNSmlDSAgent::ClientModMakingStatusStateL()
	{
	DBG_FILE(_S8("ClientModMakingStatusStateL begins"));
	iSyncMLCmds->StatusContainer()->Begin();
	TBool found( ETrue ); 
	
	while( found )
		{
		SmlStatus_t* status;
		found = iSyncMLCmds->StatusContainer()->NextStatusElement( status, ETrue ); 
		if ( found )
			{
			iSyncMLCmds->DoStatusL( status );
			}
		}
		
	iDSCurrClientModState = EClientModMakingResults;
	DBG_FILE(_S8("CNSmlDSAgent::ClientModMakingStatusStateL ends"));
	}
	
// ---------------------------------------------------------
// CNSmlDSAgent::ClientModMakingResultsStateL()
// Device Info to a server in Results command (if asked )
// ---------------------------------------------------------
void CNSmlDSAgent::ClientModMakingResultsStateL()
	{
	DBG_FILE(_S8("CNSmlDSAgent::ClientModMakingResultsStateL begins"));
	if ( iDSDeviceInfoRequestedByServer && !iDSContent->AllDatabasesAreInterrupted() )
		{
		iSyncMLCmds->DoResultsL();
		iDSDeviceInfoRequestedByServer = EFalse;
		}
	if ( iFinalMessageFromClient )
		{
		iDSContent->SetToFirst();
		}
	else
		{
		iDSContent->SetIndexToSaved();
		}
	iDSCurrClientModState = EClientModStartingSync;
	DBG_FILE(_S8("CNSmlDSAgent::ClientModMakingResultsStateL ends"));
	}
	
// ---------------------------------------------------------
// CNSmlDSAgent::ClientModStartingSyncStateL()
// Makes Sync command 
// ---------------------------------------------------------
void CNSmlDSAgent::ClientModStartingSyncStateL()
	{
	DBG_FILE(_S8("CNSmlDSAgent::ClientModStartingSyncStateL begins"));
	CNSmlCmdsBase::TReturnValue ret;
	if ( !iDSContent->Interrupted() )
		{
		ret = iSyncMLCmds->DoStartSyncL();
		switch ( ret )
			{
			case CNSmlCmdsBase::EReturnOK:
				iDSCurrClientModState = EClientModMakingUpdates;
				if ( !iDSContent->ClientItemCountAsked() )
					{
					iDSContent->SetClientItemCountAsked();
					iDSObserver->OnSyncMLSyncProgress( MSyncMLProgressObserver::ESmlSendingModificationsToServer, iDSContent->ClientItemCount(), iDSContent->TaskId() );
					}
				break;
			case CNSmlCmdsBase::EReturnBufferFull:
				iDSCurrClientModState = EClientModEndingMessage;
				iBufferFull = ETrue;
				break;
			default:
				break;
			}
		}
	else
		// skip interrupted database
		{
		if ( !iDSContent->SetToNext() )
			{
			iDSCurrClientModState = EClientModEndingMessage;
			}
		}
	DBG_FILE(_S8("CNSmlDSAgent::ClientModStartingSyncStateL ends"));
	}

// ---------------------------------------------------------
// CNSmlDSAgent::ClientModMakingUpdatesStateL()
// Makes Replace or Delete command
// ---------------------------------------------------------
void CNSmlDSAgent::ClientModMakingUpdatesStateL()
	{
	DBG_FILE(_S8("CNSmlDSAgent::ClientModMakingUpdatesStateL begins"));
	
	//RD_SUSPEND_RESUME
	// Client modifications are not sent,if the previous session was interrupted at Server modifications state.Commented for Usability issue.
	/*if(iDSContent->SuspendedState()== EStateServerModification && iDSContent->GetSessionResumed())
		{
		  	iDSCurrClientModState = EClientModEndingSync;	
		}else */
	if ( iDSContent->SyncType() == KNSmlDSOneWayFromServer ||
		 iDSContent->SyncType() == KNSmlDSOneWayFromServerByServer ||
		 iDSContent->SyncType() == KNSmlDSRefreshFromServer ||
		 iDSContent->SyncType() == KNSmlDSRefreshFromServerByServer )
		{
		iDSCurrClientModState = EClientModEndingSync;
		}
	else
		{
		switch ( iSyncMLCmds->DoAddOrReplaceOrDeleteL() )
			{
			case CNSmlCmdsBase::EReturnEndData:
				iDSCurrClientModState = EClientModEndingSync;
				break;
				
			case CNSmlCmdsBase::EReturnBufferFull:
				iDSCurrClientModState = EClientModEndingSync;
				iBufferFull = ETrue;
				break;
				
			default:
				break;
			}
		}
		
	DBG_FILE(_S8("CNSmlDSAgent::ClientModMakingUpdatesStateL ends"));
	}
	
// ---------------------------------------------------------
// CNSmlDSAgent::ClientModEndingSyncStateL()
// End of Sync command 
// ---------------------------------------------------------
void CNSmlDSAgent::ClientModEndingSyncStateL()
	{
	DBG_FILE(_S8("CNSmlDSAgent::ClientModEndingSyncStateL begins"));

	iSyncMLCmds->DoEndSyncL();

	if ( iBufferFull )
		{
		iDSCurrClientModState = EClientModEndingMessage;
		}
	else
		{
		if ( iDSContent->SetToNext() )
			{
			iDSCurrClientModState = EClientModStartingSync;
			}
		else
			{
			iDSCurrClientModState = EClientModEndingMessage;
			}
		}

	DBG_FILE(_S8("CNSmlDSAgent::ClientModEndingSyncStateL ends"));
	}
	
// ---------------------------------------------------------
// CNSmlDSAgent::ClientModEndingMessageStateL()
// End of SyncMl Message, read amounts are notified to UI
// ---------------------------------------------------------
void CNSmlDSAgent::ClientModEndingMessageStateL()
	{
	DBG_FILE(_S8("CNSmlDSAgent::ClientModEndingMessageStateL begins"));
	if ( iBufferFull )
		{
		iSyncMLCmds->DoEndMessageL( EFalse );
		iFinalMessageFromClient = EFalse;
		iDSContent->SaveCurrentIndex();
		}
	else
		{
		iSyncMLCmds->DoEndMessageL();
		iFinalMessageFromClient = ETrue;
		}
	NextMainStateL();
	DBG_FILE(_S8("CNSmlDSAgent::ClientModEndingMessageStateL ends"));
	}
	
// ---------------------------------------------------------
// CNSmlDSAgent::ServerModificationsStateL(()
// Main state function to read Server Modifications to Client Package  
// ---------------------------------------------------------
void CNSmlDSAgent::ServerModificationsStateL()
	{
	DBG_FILE(_S8("ServerModificationsStateL begins"));
	
	if ( iCurrServerModState == EServerModWaitingStartMessage )
		{

// <MAPINFO_RESEND_MOD_BEGIN>
        iDSContent->SetMapRemovable();
		iDSContent->RemoveAllRemovableMap();
// <MAPINFO_RESEND_MOD_BEGIN>

		iDSContent->SetSyncStatusToAll( ENoServerDataUpdated );
		}
		
	if ( !iSyncMLCmds->ProcessReceivedDataL() )
		{
		if ( iCurrServerModState == EServerModMessageReceived )
			{
			iBusyStatusReceived = iSyncMLCmds->BusyStatus();
			CheckStatusCodesAreReceivedL( EFalse );
			}
		else
			{
			if ( iCurrServerModState == EServerModPackageReceived )
				{
				CheckStatusCodesAreReceivedL();
				CheckCommandsAreReceivedL();
				iDSContent->SetSyncStatusToAll( ECompletedOK );
				}
			else
				{
				User::Leave( TNSmlError::ESmlIncompleteMessage );
				}
			}
		NextMainStateL();
		}
	
	DBG_FILE(_S8("CNSmlDSAgent::ServerModificationsStateL ends"));
	}
	
//
// Server Modifications state functions
//
// ---------------------------------------------------------
// CNSmlDSAgent::ServerModStartSyncStateL(()
// Handles Sync command in Server Modifications to Client Package
// ---------------------------------------------------------
void CNSmlDSAgent::ServerModStartSyncStateL( SmlSync_t* aSync )
	{
	DBG_FILE(_S8("CNSmlDSAgent::ServerModStartSyncStateL begins"));
	
	if ( iCurrServerModState != EServerModWaitingCommands )
		{
		User::Leave( TNSmlError::ESmlUnexpectedCmd ); 
		}
	
	iSyncMLCmds->ProcessSyncL( aSync );
	
	iCurrServerModState = EServerModWaitingUpdateCommands;
	
	DBG_FILE(_S8("CNSmlDSAgent::ServerModStartSyncStateL ends"));
	}

// ---------------------------------------------------------
// CNSmlDSAgent::ServerModEndSyncStateL(()
// Handles end of Sync command
// ---------------------------------------------------------
void CNSmlDSAgent::ServerModEndSyncStateL()
	{
	DBG_FILE(_S8("CNSmlDSAgent::ServerModEndSyncStateL begins"));

	iSyncMLCmds->ProcessEndSyncL();

	iCurrServerModState = EServerModWaitingCommands;
	DBG_FILE(_S8("CNSmlDSAgent::ServerModEndSyncStateL ends"));
	}
	
// -----------------------------------------------------------------------------
// CNSmlDSAgent::ServerModStartAtomicStateL
// Handles the Atomic command.
// -----------------------------------------------------------------------------
//
void CNSmlDSAgent::ServerModStartAtomicStateL( SmlAtomic_t* aContent )
	{
	if ( ( iCurrServerModState != EServerModWaitingUpdateCommands ) && ( iCurrServerModState != EServerModWaitingCommands ) )
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
	}
	
// -----------------------------------------------------------------------------
// CNSmlDSAgent::ServerModEndAtomicStateL
// Handles the end of the Atomic command.
// -----------------------------------------------------------------------------
//
void CNSmlDSAgent::ServerModEndAtomicStateL()
	{
	if ( --iNestedAtomicsCount == 0 )
		{
		iSyncMLCmds->ProcessEndAtomicL();
		}
	}

//
//  Map Acknowledgement state functions 
//
// ---------------------------------------------------------
// CNSmlDSAgent::MapAcknowledgeStateL()
// Main state function to read Map Acknowledgement from Server Package
// ---------------------------------------------------------
void CNSmlDSAgent::MapAcknowledgeStateL()
	{
	if ( !iSyncMLCmds->ProcessReceivedDataL() )
		{
		if ( iDSCurrMapAcknowledgeState == EMapAckMessageReceived )
			{
			iBusyStatusReceived = iSyncMLCmds->BusyStatus();
			}
		else
			{
			if ( iDSCurrMapAcknowledgeState == EMapAckPackageReceived )
				{
				CheckStatusCodesAreReceivedL();
				iDSContent->SetSyncStatusToAll( ECompletedOK );
				}
			else
				{
				if ( iDSWaitingForMapAcknowledgement )
					{
					User::Leave( TNSmlError::ESmlIncompleteMessage );
					}
				}
			}
		NextMainStateL();
		}
	}
	
// ---------------------------------------------------------
// CNSmlDSAgent::MapAckStartMessageStateL
// Handles SyncHdr in Map Acknowledgement from Server Package
// ---------------------------------------------------------
void  CNSmlDSAgent::MapAckStartMessageStateL( SmlSyncHdr_t* aSyncHdr )
	{
	DBG_FILE(_S8("CNSmlDSAgent::MapAckStartMessageStateL begins"));
	if ( iDSCurrMapAcknowledgeState != EMapAckWaitingStartMessage )
		{
		User::Leave( TNSmlError::ESmlUnexpectedCmd ); 
		}
	iSyncMLCmds->ProcessSyncHdrL( aSyncHdr );
	iDSCurrMapAcknowledgeState = EMapAckWaitingCommands;
	DBG_FILE(_S8("CNSmlDSAgent::MapAckStartMessageStateL ends"));
	}
	
// ---------------------------------------------------------
// CNSmlDSAgent::MapAckStatusCmdStateL()
// Handles Status command in Map Acknowledgement from Server Package 
// ---------------------------------------------------------
void CNSmlDSAgent::MapAckStatusCmdStateL( SmlStatus_t* aStatus )
	{
	DBG_FILE(_S8("CNSmlDSAgent::MapAckStatusCmdStateL begins"));
	if ( iDSCurrMapAcknowledgeState != EMapAckWaitingCommands ) 
		{
		User::Leave( TNSmlError::ESmlUnexpectedCmd ); 
		}
	iSyncMLCmds->ProcessStatusCmdL ( aStatus );
	DBG_FILE(_S8("CNSmlDSAgent::MapAckStatusCmdStateL ends"));
	}
	
// ---------------------------------------------------------
// CNSmlDSAgent::MapAckAlertCmdStateL()
// Handles Alert command in Map Acknowledgement from Server Package 
// ---------------------------------------------------------
void  CNSmlDSAgent::MapAckAlertCmdStateL( SmlAlert_t* aAlert )
	{
	DBG_FILE(_S8("CNSmlDSAgent::MapAckAlertCmdStateL begins"));
	if ( iDSCurrMapAcknowledgeState != EMapAckWaitingCommands ) 
		{
		User::Leave( TNSmlError::ESmlUnexpectedCmd ); 
		}

	if ( iSyncMLCmds->AlertCode( aAlert ) == KNSmlAgentDisplayAlert )
		{
		iSyncMLCmds->ProcessAlertCmdL( aAlert, EFalse, EFalse, ETrue );
		}
	else
		{
		if ( !iFinalMessageFromClient )
			{
			iSyncMLCmds->ProcessAlertCmdL( aAlert, ETrue );
			}
		}

	DBG_FILE(_S8("CNSmlDSAgent::MapAckAlertCmdStateL ends"));
	}
	
// ---------------------------------------------------------
// CNSmlDSAgent::MapAckEndMessageStateL()
// Handles end of SyncML message
// ---------------------------------------------------------
void  CNSmlDSAgent::MapAckEndMessageStateL( TBool aFinal )
	{
	DBG_FILE(_S8("CNSmlDSAgent::MapAckEndMessageStateL begins"));
	
	if ( iDSCurrMapAcknowledgeState != EMapAckWaitingCommands ) 
		{
		User::Leave( TNSmlError::ESmlUnexpectedCmd ); 
		}
		
	if ( aFinal )
		{
			
// <MAPINFO_RESEND_MOD_BEGIN>
		iDSContent->RemoveAllRemovableMap();
// <MAPINFO_RESEND_MOD_BEGIN>
			
		iDSCurrMapAcknowledgeState = EMapAckPackageReceived;
		}
	else
		{
		iDSCurrMapAcknowledgeState = EMapAckMessageReceived;
		}
		
	DBG_FILE(_S8("CNSmlDSAgent::MapAckEndMessageStateL ends"));
	}

//
//  Finishing state functions 
//

// ---------------------------------------------------------
// CNSmlDSAgent::FinishingStateL()
// Finishing functions in the end of sync
// ---------------------------------------------------------
void CNSmlDSAgent::FinishingStateL()
	{
	DBG_FILE(_S8("CNSmlDSAgent::FinishingStateL begins"));
	
	TInt index = iDSContent->CurrentIndex();
	TInt keyVal;
	TRAPD (srerr ,ReadRepositoryL(KNsmlDsSuspendResume, keyVal));
	
   if(srerr == KErrNone && keyVal == 1)
   {
		
	    TNSmlSyncSuspendedState suspendedState;
	    if(iDSContent->SetToFirst())
	    {
	        do
	        {
	           suspendedState = CurrentState();	
	           iDSContent->SetSuspendedStateL(suspendedState);
	        }while( iDSContent->SetToNext() );
	   }
	   
	    if(iDSContent->SetToFirst())
	    {
		   do
	  	   {
		    TBuf8<3> syncType( iDSContent->SyncType()); 
	        TNSmlPreviousSyncType prevSyncType=MapAlertCode(syncType);
	        //Sync Type of the Current Sync session is stored.
	        iDSContent->SetPreviousSyncTypeL( prevSyncType );	
		   }while( iDSContent->SetToNext() );
         }
	
	   if(iDSContent->SetToFirst())
	   { 
		   do
	       { 
		    TBuf8<3> clientSyncType( iDSContent->ClientSyncType());
	        TNSmlPreviousSyncType prevClientType=MapAlertCode(clientSyncType);
	        //Sync Type proposed by Client in the Current Sync session is stored.
	        iDSContent->SetPreviousClientSyncTypeL( prevClientType );	
		   }while( iDSContent->SetToNext() );
	   }
		
	}
	//RD_SUSPEND_RESUME
	iDSContent->SetIndex(index);
	if( !iPacketDataUnAvailable && iUpdateLastAnchor )
	     iDSContent->UpdateSyncAnchorsL( iDSClientNextSyncAnchor );
	iDSContent->FinalizeDatabasesL();
	FinishL();
	FinalizeSyncLogL();
	//RD_AUTO_RESTART (BPSS-7GBFF3)
	//Check whether the Auto Restart Feature is enabled or not
	TRAPD (arerr ,ReadRepositoryL(KNsmlDsAutoRestart, keyVal));
	
	if(arerr == KErrNone && keyVal == 1)
	{
		if(!iPacketDataUnAvailable && !iAutoRestartInitiatedSync )
		{
			DBG_FILE(_S8("CNSmlDSAgent::FinishingStateL About to check the Profile ID"));
			
			if(CheckProfileIdL())
			{
				TerminateAutoRestart();
			}	
		}
	}
	//RD_AUTO_RESTART
	
	DBG_FILE(_S8("CNSmlDSAgent::FinishingStateL ends"));
	}

// ---------------------------------------------------------
// CNSmlDSAgent::OpenDatabaseInstancesL
// Database instances (content data adapters are created)
// In the creation they are harcoded by the name,later on referenced
// by an index.
// ---------------------------------------------------------
void CNSmlDSAgent::OpenDatabaseInstancesL( TBool& aEnd )
	{
	TInt resultCode;
	TBool ret( EFalse );
		
	if ( !iDSDatabaseOpeningStarted )
		{
		ret = iDSContent->SetToFirst();
		iDSDatabaseOpeningStarted = ETrue;
		}
	else
		{
		ret = iDSContent->SetToNext();
		}
			
	if ( ret )
		{
		iDSContent->OpenContentInstanceL( resultCode );

		switch ( resultCode )
			{
			case KErrNone:
			    {
			    TBool status( EFalse );        
			    
			    TRAPD( error, status = iDSContent->IsSupportedSyncTypeL( iDSContent->SyncType() ));
			    if ( error == KErrNone )
			        {
			        if ( !status )
					    {
					    InterruptL( iDSContent->CurrentIndex(), TNSmlDSError::ESmlDSUnsupportedSyncType, ETrue, EFalse );
					    iDSContent->SetInterruptedBeforeSync();
					    }        
			        }
			    else
			        {
			        InterruptL( iDSContent->CurrentIndex(), error, ETrue, EFalse );
					iDSContent->SetInterruptedBeforeSync();    
			        }
				
			    }
				break;
				
			case KErrNotFound:
				InterruptL( iDSContent->CurrentIndex(), TNSmlDSError::ESmlDSLocalDatabaseError, ETrue, EFalse );
				iDSContent->SetInterruptedBeforeSync();
				break;
			
			default:
				User::Leave( resultCode );
			}
		}
	else
		{
		aEnd = ETrue;
		}	
	}
	
// ---------------------------------------------------------
// CNSmlDSAgent::CreateLUIDBuffersL
// 
// ---------------------------------------------------------
void CNSmlDSAgent::CreateLUIDBuffersL( TBool& aEnd )
	{
	TBool ret( EFalse );

	if ( !iDSLUIDBufferingStarted )
		{
		ret = iDSContent->SetToFirst();
		iDSLUIDBufferingStarted = ETrue;
		}
	else
		{
		ret = iDSContent->SetToNext();
		}
		
	if ( ret )
		{
		if ( !iDSContent->Interrupted() )
		    {
			TInt resultCode(KErrNone);
			TRAPD( error, iDSContent->CreateLUIDBufferL( resultCode ));

			if ( error != KErrNone || resultCode != KErrNone )
			    {
			    InterruptL( iDSContent->CurrentIndex(), TNSmlDSError::ESmlDSLocalDatabaseError, EFalse, ETrue );			    
			    }
			}
		}
	else
		{
		aEnd = ETrue;
		}
	}

// ---------------------------------------------------------
// CNSmlDSAgent::CreateSyncmlCmdsInstanceL()
// 
// ---------------------------------------------------------
void CNSmlDSAgent::CreateSyncmlCmdsInstanceL()
	{
	if ( !iSyncMLCmds )
		{
		TBool versionCheck( ETrue );
		TBuf8<16> sessionIdString;
		
		if ( iSessionId == 0 )
			{
			iSessionId = GetSessionIDL();
			++iSessionId;
			UpdateSessionIDL( iSessionId );
			}
			
		sessionIdString.Num( iSessionId );

		if ( iSyncInitiation == EServerAlerted )
			{
			versionCheck = EFalse;
			}

		if ( iProtocolVer == ESmlVersion1_1_2 )
			{
			iSyncMLCmds = CNSmlDSCmds::NewL( this, sessionIdString, KNSmlDSAgentVerProto, KNSmlSyncMLPublicId, *iSyncMLUserName, iSyncServer, *iDSContent, *iDSObserver, versionCheck );
			}
		else if ( iProtocolVer == ESmlVersion1_2 )
			{
			iSyncMLCmds = CNSmlDSCmds::NewL( this, sessionIdString, KNSmlDSAgentVerProto12, KNSmlSyncMLPublicId12, *iSyncMLUserName, iSyncServer, *iDSContent, *iDSObserver, versionCheck );		
			}
			
	    TInt value(0);
	    TRAPD (err , ReadRepositoryL(KNSmlMaxMsgSizeKey, value));	    
	    if ( ( iMediumType == KUidNSmlMediumTypeInternet ) && ( err == KErrNone ) )
	        {
            iSyncMLCmds->SetMaximumWorkspaceSize( value );	        
	        }
	    else if ( iMediumType != KUidNSmlMediumTypeInternet )
			{
			iSyncMLCmds->SetMaximumWorkspaceSize( KNSmlLocalSyncWorkspaceSize );
			}
		}
	}

// ---------------------------------------------------------
// CNSmlDSAgent::InfoMessageToSyncLogL()
// Info messages are written during the sync
// ---------------------------------------------------------
void CNSmlDSAgent::InfoMessageToSyncLogL( TInt appIndex, TInt aErrorCode )
	{
	iDSContent->SetIndex( appIndex );
	DoErrorEvent( MSyncMLProgressObserver::ESmlWarning, aErrorCode, iDSContent->TaskId(), 0, 0 );
	}

// ---------------------------------------------------------
// CNSmlDSAgent::ReadSettingsL()
// Reads sync settings
// ---------------------------------------------------------
void CNSmlDSAgent::ReadSettingsL()
	{
	DBG_FILE(_S8("CNSmlDSAgent::ReadSettingsL BEGIN"));                
	CNSmlDSProfile* profile = CNSmlDSSettings::NewLC()->ProfileL( iProfileID );
	//RD_AUTO_RESTART
	if(iDSNetmon)
	{
		iDSNetmon->iProfileID=iProfileID; 	
	}
	//RD_AUTO_RESTART
	if ( !profile )
		{
		User::Leave( TNSmlError::ESmlErrorInSettings );
		}

	CleanupStack::PushL( profile );
	
	SetDSSessionInfoL(profile);
		
	if ( iMediumType.iUid == -1 )
		{
		iMediumType.iUid = profile->IntValue( EDSProfileTransportId );  
		iIAPId = profile->IntValue( EDSProfileIAPId );
		}
    else if ( iMediumType.iUid == KUidNSmlMediumTypeInternet.iUid )
	    {
	    iIAPId = profile->IntValue( EDSProfileIAPId ); 
	    }
	//RD_AUTO_RESTART
	ReplaceIAPIdL();
 	//RD_AUTO_RESTART
	DBG_FILE_CODE(iIAPId , _S8("CNSmlDSAgent::ReadSettingsL, The Access Point ID for the Sync Session is :"));                
			
	iIAPIdArray = new ( ELeave ) CArrayFixFlat<TUint32>( KSmlArrayGranularity );
	iIAPIdArray->AppendL( iIAPId );

    if ( profile->IntValue( EDSProfileAutoChangeIAP ) )
        {            
		RDbNamedDatabase* dBase = CNSmlDSSettings::NewLC()->Database();
		CNSmlRoamTable* roam = CNSmlRoamTable::NewLC( *dBase );
		CNSmlIAPArray* iapArray( roam->GetItemsForIdL( iProfileID ) );
		CleanupStack::PushL( iapArray );

		for ( TInt index = 0 ; index < iapArray->Count() ; index++ )
			{
			iIAPIdArray->AppendL( iapArray->At( index ) );
			}
                
		iapArray->Reset();
		CleanupStack::PopAndDestroy( 3 ); // iapArray, roam, settings
		}

	iDSServerId = profile->StrValue( EDSProfileServerId ).AllocL();
	iSyncMLUserName = profile->StrValue( EDSProfileSyncServerUsername ).AllocL();
	iSyncMLPassword = profile->StrValue( EDSProfileSyncServerPassword ).AllocL();
	iSyncHTTPAuthUserName = profile->StrValue( EDSProfileHttpAuthUsername ).AllocL();	
	iSyncHTTPAuthPassword = profile->StrValue( EDSProfileHttpAuthPassword ).AllocL();	
	iSyncHTTPAuthUsed = profile->IntValue( EDSProfileHttpAuthUsed );						
	iProtocolVer = static_cast<TSmlProtocolVersion>( profile->IntValue( EDSProfileProtocolVersion ) );
	
	// set sync started to P&S (if fails -> sync not interrupted)
	if ( iProtocolVer == ESmlVersion1_1_2 )
	    {
        RProperty::Set( KPSUidDataSynchronizationInternalKeys,
                        KDataSyncStatus,
                        EDataSyncRunning );	    
	    }
    else // TSmlProtocolVersion::ESmlVersion1_2
        {
        RProperty::Set( KPSUidDataSynchronizationInternalKeys,
                        KDataSyncStatus,
                        EDataSyncRunning12 );	    
        }
    
    if ( iRepositorySSC )
        {
        iRepositorySSC->Set( KNsmlOpDsSyncErrorCode, KErrNone );
        iRepositorySSC->Set( KNsmlOpDsSyncProfId, profile->IntValue( EDSProfileId ) );
        iRepositorySSC->Set( KNsmlOpDsSyncInitiation, iSyncInitiation );
        }

	TBool ifInternet = ETrue ; // CR: 403-1188
	if ( iMediumType == KUidNSmlMediumTypeInternet )
		{
		iSyncServer = CNSmlURI::NewL( profile->StrValue( EDSProfileServerURL ), ETrue );
		}
	else
		{
		iSyncServer = CNSmlURI::NewL( profile->StrValue( EDSProfileServerURL ), EFalse );
		ifInternet = EFalse ; // CR: 403-1188
		}
			
	if ( !profile->iContentTypes )
		{
		User::Leave( TNSmlError::ESmlErrorInSettings );
		}
	
	RArray<TSmlTaskId> taskIdArray;
	

	TBool isRemotedatabaseDefined(ETrue);
	for ( TInt i = 0; i < profile->iContentTypes->Count(); i++ )	
		{
		TBool addContent( EFalse );
		TBool serverAlert(EFalse);
		CNSmlDbCaps::ENSmlSyncTypes syncType( CNSmlDbCaps::EAllTypes );
        
        if ( iDSSyncTypeArray )
			{
			if ( iDSSyncTypeArray->Count() > 0 )
			    {
			    for ( TInt j = 0; j < iDSSyncTypeArray->Count(); j++ )
                    {
                    if ( (*profile->iContentTypes)[i]->IntValue( EDSAdapterTableId ) == iDSSyncTypeArray->At( j ).iTaskId)
                        {
                        if ( iSyncInitiation == EClientInitiated )
                        	{
                        	if ( (*profile->iContentTypes)[i]->IntValue( EDSAdapterEnabled ) )
                        		{
                             	addContent = ETrue;
                                serverAlert = ETrue;
                                syncType = iDSSyncTypeArray->At( j ).iType;
                        		}
	                       	}
                        else 
                        	{
                        	addContent = ETrue;
                            serverAlert = ETrue;
                            syncType = iDSSyncTypeArray->At( j ).iType;
                        	}
                        
                        break;
                        }
                    }
			    }
            else
    			{
    			addContent = (*profile->iContentTypes)[i]->IntValue( EDSAdapterEnabled );
    			}
			}
		else
			{
			addContent = (*profile->iContentTypes)[i]->IntValue( EDSAdapterEnabled );
			}
			
		if ( addContent )
			{
			iDSContent->AddNewContentL();
			iDSContent->SetImplementationUID( (*profile->iContentTypes)[i]->IntValue( EDSAdapterImplementationId ) );
			
			CNSmlURI* database = CNSmlURI::NewLC( *iImeiCode );				
			database->SetDatabaseL( (*profile->iContentTypes)[i]->StrValue( EDSAdapterClientDataSource ) );
			TRAPD( localDatabaseError, iDSContent->SetLocalDatabaseL( *database ) );
			
			if ((*profile->iContentTypes)[i]->StrValue( EDSAdapterClientDataSource )  != database->DataBaseWithoutRelativePrefix())
				{
				(*profile->iContentTypes)[i]->SetStrValue( EDSAdapterClientDataSource ,database->DataBaseWithoutRelativePrefix());
				profile->SaveL() ;
				}
			CleanupStack::PopAndDestroy(); // database

			if ( localDatabaseError )
			    {
			    DBG_ARGS(_S("CNSmlDSAgent::ReadSettingsL: local database error %d"), localDatabaseError);
			    iDSContent->RemoveContent();
			    /* CR: 403-1188 */
			    if ( ifInternet      == EFalse 		   &&
			    	 iSyncInitiation == EServerAlerted &&
			    	 (*profile->iContentTypes)[i]->IntValue( EDSAdapterImplementationId ) == KUidNSmlAdapterEMail.iUid) 
				    {
				    CNSmlDSContentType* type = profile->ContentType( KUidNSmlAdapterEMail.iUid );
					type->SetIntValue (EDSAdapterEnabled, EFalse) ;				    	
					profile->SaveL() ;
				    } // CR: 403-1188
				continue;
			    }
				 
			database = CNSmlURI::NewLC( iSyncServer->HostNameWithPortL() );
			database->SetDatabaseL( (*profile->iContentTypes)[i]->StrValue( EDSAdapterServerDataSource ) );
			if(database->Database().Length() < 1)
			    {
			    isRemotedatabaseDefined = EFalse;
			    }
			iDSContent->SetRemoteDatabaseL( *database );
			CleanupStack::PopAndDestroy(); //database

			if ( iProtocolVer == ESmlVersion1_1_2 )
				{
				HBufC* hostAddress = iSyncServer->HostName().AllocLC();

				if ( !iDSContent->InitContentInstanceL( *hostAddress ) )
					{
					iDSContent->RemoveContent();
					CleanupStack::PopAndDestroy(); // hostAddress
					continue;
					}
						
				CleanupStack::PopAndDestroy(); // hostAddress
				}
			else if ( iProtocolVer == ESmlVersion1_2 )
				{
				if ( !iDSContent->InitContentInstanceL( *iDSServerId ) )
					{
					iDSContent->RemoveContent();
					continue;
					}
					
			    RReadStream readStream( (*profile->iContentTypes)[i]->FilterReadStreamL() );
			    CleanupClosePushL( readStream );
				    
				TInt numberOfFilters(0);
				TInt error( KErrNone );
				TRAP(error, numberOfFilters = readStream.ReadInt32L() );    
			    
			    RPointerArray<CSyncMLFilter> filters;
			    CleanupStack::PushL( TCleanupItem( DeleteRPointerArray, &filters ) );
			    
                for ( TInt filterIndex = 0; filterIndex < numberOfFilters; filterIndex++ )
                    {
                    CSyncMLFilter* filter = CSyncMLFilter::NewLC( readStream );
                    filters.AppendL( filter );
                    CleanupStack::Pop(); // filter
                    }
                
                iDSContent->SetUsedFiltersL( filters, static_cast<TSyncMLFilterMatchType>( (*profile->iContentTypes)[i]->IntValue( EDSAdapterFilterMatchType ) ) );
				    
                // ownership of array items is moved to iDSContent -> no need for ResetAndDestroy
                filters.Close();
                
                CleanupStack::Pop(); // filters				    
			    CleanupStack::PopAndDestroy(); // readStream
				}

			if ( syncType == CNSmlDbCaps::EAllTypes )
				{
				TSmlSyncType type = static_cast<TSmlSyncType>( (*profile->iContentTypes)[i]->IntValue( EDSAdapterSyncType ) );
					
				switch ( type )
					{
					case ESmlTwoWay:
						syncType = CNSmlDbCaps::ETwoWaySync;
						break;
							
					case ESmlOneWayFromServer:
						syncType = CNSmlDbCaps::EOneWaySyncFromServer;
						break;
							
					case ESmlOneWayFromClient:
						syncType = CNSmlDbCaps::EOneWaySyncFromClient;
						break;
							
					case ESmlSlowSync:
						syncType = CNSmlDbCaps::ESlowTwoWaySync;
						break;
							
					case ESmlRefreshFromServer:
						syncType = CNSmlDbCaps::ERefreshSyncFromServer;
						break;
							
					case ESmlRefreshFromClient:
						syncType = CNSmlDbCaps::ERefreshSyncFromClient;
						break;
							
					default:
						User::Panic( _L("Sync type conversion error" ), type );
					}
				}
				
			if ( syncType != CNSmlDbCaps::ESyncTypeNone )
				{
				iDSContent->SetSyncTypeL( syncType );
				taskIdArray.Append( (*profile->iContentTypes)[i]->IntValue( EDSAdapterTableId ) );
				iDSContent->TaskId( (*profile->iContentTypes)[i]->IntValue( EDSAdapterTableId ) );
				
				if ( serverAlert && iProtocolVer == ESmlVersion1_2 )
					{
					iDSContent->SetAsServerAlerted();
					}
				}
			else
				{
				iDSContent->RemoveContent();
				}
			}
		}
	
	iSyncLog = CSyncMLHistoryJob::NewL( iProfileID, taskIdArray );

	if ( iDSContent->Count() == 0 && iSyncInitiation == EClientInitiated )
		{
		User::Leave( TNSmlError::ESmlErrorInSettings );
		}
		// BPSS-7NZESW : If remote database value is empty,leave
		//erorr log : Invalid remote database
	if(!isRemotedatabaseDefined)
	    {
	    User::Leave( TNSmlError::ESmlStatusNotFound );
	    }
	taskIdArray.Close();	
	CleanupStack::PopAndDestroy( 2 ); // profile, settings
	DBG_FILE(_S8("CNSmlDSAgent::ReadSettingsL END"));
	}

// ---------------------------------------------------------
// CNSmlDSAgent::ReplaceIAPIdL()
// Gets IAPID from the Cenrep and if necessary will Replace 
// the existing one
// ---------------------------------------------------------
void CNSmlDSAgent::ReplaceIAPIdL()
{
	//Check whether the Auto Restart Feature is enabled or not
	TInt keyVal;
	TRAPD (err ,ReadRepositoryL(KNsmlDsAutoRestart, keyVal));
	
	if(err==KErrNone && keyVal==1)
	{
		TInt LastUsedIAPId = 0;
		
		CNsmlProfileUtil* profileUtil = CNsmlProfileUtil::NewLC();	
				
		if(iAutoRestartInitiatedSync)
		{
			//Read the IAPID from the Cenrep
			profileUtil->AccessPointIdL(LastUsedIAPId);
			if(LastUsedIAPId > 0)
			{
                //Check Whether the IAPID still exists
                RCmManager  cmmanager;
                cmmanager.OpenL();
                CleanupClosePushL(cmmanager);
                RCmConnectionMethod cm;
                TRAPD(err, cm = cmmanager.ConnectionMethodL( LastUsedIAPId ));
                CleanupClosePushL( cm );
                TUint32 bearer = 0;
                if( err == KErrNone )
                {
                    bearer = cm.GetIntAttributeL( CMManager::ECmBearerType );        
                    DBG_FILE_CODE( bearer ,_S8("CNSmlDSAgent::ReadSettingsL, BearerType"));
                    if ( bearer == KUidWlanBearerType )
                    {
                        DBG_FILE(_S8("CNSmlDSAgent::ReadSettingsL, The Access Point ID is invalid WLAN"));
                        iPacketDataUnAvailable = ETrue;
                        StopDSSession();
                    }
                    else
                    {
                        iIAPId = LastUsedIAPId;      
                        DBG_FILE_CODE(iIAPId , _S8("CNSmlDSAgent::ReadSettingsL, The Access Point ID Replaced from the CenRep:"));
                    }
                }
                else
                {
                    DBG_FILE_CODE( err ,_S8("CNSmlDSAgent::ReadSettingsL, The Access Point ID is invalid"));
                    iPacketDataUnAvailable = ETrue;
                    StopDSSession();
                }              
                CleanupStack::PopAndDestroy( 2 ); //cmmanagerext,cm			
			}
		}		
		CleanupStack::PopAndDestroy(profileUtil);
	}
}

// ---------------------------------------------------------
// CNSmlDSAgent::GetSessionIDL()
// Gets SessionID from Agent Log
// ---------------------------------------------------------
TInt CNSmlDSAgent::GetSessionIDL()
	{
	CNSmlDSAgentLog* agentLog( CNSmlDSAgentLog::NewLC() );
	TInt sessionID( 0 );

	if ( iProtocolVer == ESmlVersion1_1_2 )
		{
		HBufC* searchKey = iSyncServer->HostName().AllocLC();
		sessionID = agentLog->SessionIDL( *searchKey );
		CleanupStack::PopAndDestroy(); // searchKey
		}
	else if ( iProtocolVer == ESmlVersion1_2 )
		{
		sessionID = agentLog->SessionIDL( *iDSServerId );
		}

	CleanupStack::PopAndDestroy(); // agentLog

	return sessionID;
	}
	
// ---------------------------------------------------------
// CNSmlDSAgent::UpdateSessionIDL()
// Updates SessionID in Agent Log
// ---------------------------------------------------------
void CNSmlDSAgent::UpdateSessionIDL( TInt aSessionID )
	{
	CNSmlDSAgentLog* agentLog( CNSmlDSAgentLog::NewLC() );

	if ( iProtocolVer == ESmlVersion1_1_2 )
		{
		HBufC* searchKey = iSyncServer->HostName().AllocLC();
		agentLog->SetSessionIDL( *searchKey, aSessionID );
		CleanupStack::PopAndDestroy(); // searchKey
		}
	else if ( iProtocolVer == ESmlVersion1_2 )
		{
		agentLog->SetSessionIDL( *iDSServerId, aSessionID );
		}

	CleanupStack::PopAndDestroy(); // agentLog
	}
	
// ---------------------------------------------------------
// CNSmlDSAgent::FinishL()
// 
// ---------------------------------------------------------
void CNSmlDSAgent::FinishL()
	{
	iDSObserver->OnSyncMLSyncProgress( MSyncMLProgressObserver::ESmlDisconnected, 0, 0 );
		
	if ( iDSContent->AllDatabasesAreInterrupted() )
		{
		DoErrorEvent( MSyncMLProgressObserver::ESmlFatalError, iError->SyncLogErrorCode(), 0, 0, 0 );
		User::RequestComplete( iCallerStatus, iError->SyncLogErrorCode() );
		}
	else if ( iDSContent->AnyDatabaseIsInterrupted()  )
		{
		DoErrorEvent( MSyncMLProgressObserver::ESmlWarning, iError->SyncLogErrorCode(), 0, 0, 0 );
		iError->SetErrorCode( KErrNone );
		User::RequestComplete( iCallerStatus, KErrNone );
		}	
	//RD_AUTO_RESTART (BPSS-7GBGHP)
	else if(iPacketDataUnAvailable)
	{
		DBG_FILE(_S8("CNSmlDSAgent::FinishL(), Setting the Dialog Text"));
		
		DoErrorEvent(MSyncMLProgressObserver::ESmlFatalError,
					 TNSmlError::ESmlCommunicationError, 0, 0, 0 );
		iError->SetErrorCode( TNSmlError::ESmlCommunicationError);
		User::RequestComplete( iCallerStatus, TNSmlError::ESmlCommunicationError );
	}
	//RD_AUTO_RESTART		
	else
		{
		User::RequestComplete( iCallerStatus, KErrNone );
		}
		
	iDSObserver->OnSyncMLSyncProgress( MSyncMLProgressObserver::ESmlCompleted, 0, 0 );
	
	iEnd = ETrue;
	}

// ---------------------------------------------------------
// CNSmlDSAgent::DoCancel
// Cancels ongoing asynchronous operations.
// ---------------------------------------------------------
void CNSmlDSAgent::DoCancel()
	{
	if ( iCommandIssued )
		{
		TInt err( KErrNone );
		TRAP( err, iDSContent->CancelRequestL() );
		}
	CNSmlAgentBase::DoCancel();
	}

//
// Callback functions implementation
//
// ---------------------------------------------------------
// CNSmlDSAgent::smlAlertCmdFuncL
// Alert command from server.  
// ---------------------------------------------------------
Ret_t CNSmlDSAgent::smlAlertCmdFuncL( SmlAlertPtr_t aContent )
	{
	iCallbackCalled = ETrue;
	
	if ( iSyncMLCmds->AlertCode( aContent ) < KNSmlDSMeaningfulAlertMin ||
		 iSyncMLCmds->AlertCode( aContent ) > KNSmlDSMeaningfulAlertMax )
		{
		iSyncMLCmds->StatusToUnsupportedCommandL( ETrue, KNSmlAgentAlert, aContent->cmdID, aContent->flags );
		}
	else
		{
		switch ( iCurrMainState )
			{
			case EBeginning:
				ServerAlertAlertCmdStateL( aContent );
				break;
				
			case EServerInitialisation:
				ServerInitAlertCmdStateL( aContent );
				break;
				
			case EServerAlertNext:
				ServerAlertNextAlertCmdStateL( aContent );
				break;
				
			case EServerModifications:
				ServerModAlertCmdStateL( aContent );
				break;
				
			case EMapAcknowledge:
				MapAckAlertCmdStateL( aContent );
				break;
				
			default: 
				iSyncMLCmds->StatusToUnsupportedCommandL( ETrue, KNSmlAgentAlert, aContent->cmdID, aContent->flags );
			}
		}
		
	return 0;
	}
	
// ---------------------------------------------------------
// CNSmlDSAgent::smlGetCmdFuncL
// Get command from server
// ---------------------------------------------------------
Ret_t CNSmlDSAgent::smlGetCmdFuncL( SmlGetPtr_t aContent )
	{
	iCallbackCalled = ETrue;
	switch ( iCurrMainState )
		{
		case EServerInitialisation:
			ServerInitGetCmdStateL( aContent );
			break;
		default:
			iSyncMLCmds->StatusToUnsupportedCommandL( ETrue, KNSmlAgentGet, aContent->cmdID, aContent->flags );
		}
	return 0;
	}
	
// ---------------------------------------------------------
// CNSmlDSAgent::smlPutCmdFuncL
// Put command from server
// ---------------------------------------------------------
Ret_t CNSmlDSAgent::smlPutCmdFuncL( SmlPutPtr_t aContent )
	{
	iCallbackCalled = ETrue;
	switch ( iCurrMainState )
		{
		case EServerInitialisation:
			ServerInitPutCmdStateL( aContent );
			break;
		default:
			iSyncMLCmds->StatusToUnsupportedCommandL( ETrue, KNSmlAgentPut, aContent->cmdID, aContent->flags );
		}
	return 0;
	}
	
// ---------------------------------------------------------
// CNSmlDSAgent::smlResultsCmdFuncL
// Results command from server
// ---------------------------------------------------------
Ret_t CNSmlDSAgent::smlResultsCmdFuncL( SmlResultsPtr_t aContent )
	{
	iCallbackCalled = ETrue;
	switch ( iCurrMainState )
		{
		case EServerInitialisation:
			ServerInitResultsCmdStateL( aContent );
			break;
		default:
			iSyncMLCmds->StatusToUnsupportedCommandL( ETrue, KNSmlAgentResults, aContent->cmdID );
		}
	return 0;
	}
	
// ---------------------------------------------------------
// CNSmlDSAgent::smlStatusCmdFuncL
// Status command from server
// ---------------------------------------------------------
Ret_t CNSmlDSAgent::smlStatusCmdFuncL( SmlStatusPtr_t aContent )
	{
	iCallbackCalled = ETrue;
	switch ( iCurrMainState )
		{
		case EServerInitialisation:
			ServerInitStatusCmdStateL( aContent );
			break;
		case EServerModifications:
			ServerModStatusCmdStateL( aContent );
			break;
		case EMapAcknowledge:
			MapAckStatusCmdStateL( aContent );
			break;
		case EServerAlertNext:
			ServerAlertNextStatusCmdStateL( aContent );
			break;
		default:
			User::Leave( TNSmlError::ESmlUnexpectedCmd );
		}
	return 0;
	}
	
// ---------------------------------------------------------
// CNSmlDSAgent::smlStartMessageFuncL
// SyncHdr from server
// ---------------------------------------------------------
Ret_t CNSmlDSAgent::smlStartMessageFuncL( SmlSyncHdrPtr_t aContent )
	{
	iCallbackCalled = ETrue;
	
	switch ( iCurrMainState )
		{
		case EBeginning:
			ServerAlertStartMessageStateL( aContent );
			break;
			
		case EServerInitialisation:
			ServerInitStartMessageStateL( aContent );
			break;
			
		case EServerModifications:
			ServerModStartMessageStateL( aContent );
			break;
			
		case EMapAcknowledge:
			MapAckStartMessageStateL( aContent );
			break;
			
		case EServerAlertNext:
			ServerAlertNextStartMessageStateL( aContent );
			break;
			
		default:
			User::Leave( TNSmlError::ESmlUnexpectedCmd );
		}
		
	return 0;
	}
	
// ---------------------------------------------------------
// CNSmlDSAgent::smlEndMessageFuncL
// End of SyncML message was reached
// ---------------------------------------------------------
Ret_t CNSmlDSAgent::smlEndMessageFuncL( Boolean_t aFinal )
	{
	iCallbackCalled = ETrue;
	
	switch ( iCurrMainState )
		{
		case EBeginning:
			ServerAlertEndMessageStateL( aFinal );
			break;
			
		case EServerInitialisation:
			ServerInitEndMessageStateL( aFinal );
			break;
			
		case EServerModifications:
			ServerModEndMessageStateL( aFinal );
			break;
			
		case EMapAcknowledge:
			MapAckEndMessageStateL( aFinal );
			break;
			
		case EServerAlertNext:
			ServerAlertNextEndMessageStateL( aFinal );
			break;
			
		default:
			User::Leave( TNSmlError::ESmlUnexpectedCmd );
		}
		
	return 0;
	}
	
// ---------------------------------------------------------
// CNSmlDSAgent::smlStartSyncFuncL
// Sync command from server
// ---------------------------------------------------------
Ret_t CNSmlDSAgent::smlStartSyncFuncL( SmlSyncPtr_t aContent )
	{
	iCallbackCalled = ETrue;

	switch ( iCurrMainState )
		{
		case EServerModifications:
			ServerModStartSyncStateL( aContent );
			break;
		default:
			iSyncMLCmds->StatusToUnsupportedCommandL( ETrue, KNSmlAgentSync, aContent->cmdID, aContent->flags );
		}
	return 0;
	}
	
// ---------------------------------------------------------
// CNSmlDSAgent::smlEndSyncFuncL
// End of Sync command was reached
// ---------------------------------------------------------
Ret_t CNSmlDSAgent::smlEndSyncFuncL()
	{
	iCallbackCalled = ETrue;
	switch ( iCurrMainState )
		{
		case EServerModifications:
			ServerModEndSyncStateL();
			break;
		default:
			break;
		}
	return 0;
	}
	
// -----------------------------------------------------------------------------
// CNSmlDSAgent::smlStartAtomicFuncL
// Atomic command from server.
// -----------------------------------------------------------------------------
//
Ret_t CNSmlDSAgent::smlStartAtomicFuncL( SmlAtomicPtr_t aContent )
	{
	iCallbackCalled = ETrue;

	switch ( iCurrMainState )
		{
		case EServerModifications:
			ServerModStartAtomicStateL( aContent );
			break;
			
		default:
			iSyncMLCmds->StatusToUnsupportedCommandL( EFalse, KNSmlAgentAtomic, aContent->cmdID, aContent->flags );
			break;
		}

	return 0;
	}
	
// -----------------------------------------------------------------------------
// CNSmlDSAgent::smlEndAtomicFuncL
// The end of the Atomic command was reached.
// -----------------------------------------------------------------------------
//
Ret_t CNSmlDSAgent::smlEndAtomicFuncL()
	{
	iCallbackCalled = ETrue;

	ServerModEndAtomicStateL();
	
	return 0;
	}
	
// ---------------------------------------------------------
// CNSmlDSAgent::smlStartSequenceFuncL
// Sequence command from server, Sequence is not supported
// ---------------------------------------------------------
Ret_t CNSmlDSAgent::smlStartSequenceFuncL( SmlSequencePtr_t aContent )
	{
	iCallbackCalled = ETrue;
	iSyncMLCmds->StatusToUnsupportedCommandL( EFalse, KNSmlAgentSequence, aContent->cmdID, aContent->flags );
	return 0;
	}
	
// ---------------------------------------------------------
// CNSmlDSAgent::smlEndSequenceFuncL
// 
// ---------------------------------------------------------
Ret_t CNSmlDSAgent::smlEndSequenceFuncL()
	{
	iCallbackCalled = ETrue;
	return 0;
	}


// ---------------------------------------------------------
// CNSmlDSAgent::smlMoveCmdFuncL
// 
// ---------------------------------------------------------
Ret_t CNSmlDSAgent::smlMoveCmdFuncL( SmlMovePtr_t aContent )
	{
	iCallbackCalled = ETrue;
	switch ( iCurrMainState )
		{
		case EServerModifications:
			ServerModUpdatesStateL( KNSmlAgentMove(), aContent );
			break;
		default:
			iSyncMLCmds->StatusToUnsupportedCommandL( ETrue, KNSmlAgentAdd, aContent->cmdID, aContent->flags );
		}
	return 0;
	}

// ---------------------------------------------------------
// CNSmlDSAgent::NavigateMainStateL()
// Navigate according to state code
// ---------------------------------------------------------
void CNSmlDSAgent::NavigateMainStateL() 
    {
    switch ( iCurrMainState )
		{
		case EBeginning:
			BeginningStateL();
			break;
		case EClientInitialisation:
			ClientInitialisationStateL();
			break;
		case EServerInitialisation:
			ServerInitialisationStateL();
			break;
// <MAPINFO_RESEND_MOD_BEGIN>
		case EClientResendMapInfo:
			ClientResendMapInfoStateL();
			break;
// <MAPINFO_RESEND_MOD_END>
		case EClientModifications:
			ClientModificationsStateL();
			break;
		case EServerModifications:
			ServerModificationsStateL();
			break;
		case EDataUpdateStatus:
			DataUpdateStatusStateL();
			break;
		case EMapAcknowledge:
			MapAcknowledgeStateL();
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
			SendingStateL();
			break;
		case EReceiving:
			ReceivingStateL();
			break;
		default:
			User::Leave( KErrGeneral );
		}
    }

// ---------------------------------------------------------
// CNSmlDSAgent::NextMainStateL()
// Change main state
// ---------------------------------------------------------
void CNSmlDSAgent::NextMainStateL() 
	{
	switch ( iCurrMainState )
		{
		case EBeginning:
			if ( iSyncInitiation == EClientInitiated && iDSContent->AllDatabasesAreInterruptedImmediately() )
				{
				iCurrMainState = EFinishing;
				} 
			else
				{
				iDSObserver->OnSyncMLSyncProgress( MSyncMLProgressObserver::ESmlConnected, 0, 0 );
				iDSObserver->OnSyncMLSyncProgress( MSyncMLProgressObserver::ESmlLoggingOn, 0, 0 );
				iCurrMainState = EClientInitialisation;
				}
			break;
			
		case EClientInitialisation:
			iPrevSyncState = iCurrMainState;
			iCurrMainState = ESending;
			break;
			
		case EServerInitialisation:
			iPrevSyncState = iCurrMainState;
			if ( iDSContent->AllDatabasesAreInterruptedImmediately() )
				{
				iCurrMainState = EFinishing;
				}
			else
				{
				if ( iBusyStatusReceived )
					{
					iCurrMainState = EResultAlert;
					}
				else
					{
					if ( iChallengeRequest )
						{
						iCurrMainState = EClientInitialisation;
						}
					else
						{
						if ( iDSCurrServerInitState == EServerInitMessageReceived )
							{
							iCurrMainState = EClientAlertNext;
							}
						else
							{
							iDSObserver->OnSyncMLSyncProgress( MSyncMLProgressObserver::ESmlLoggedOn, 0, 0 );
// <MAPINFO_RESEND_MOD_BEGIN>
							iCurrMainState = EClientResendMapInfo;
							DBG_FILE(_S8("CNSmlDSAgent::NextMainStateL Moved to EClientResendMapInfo..."));
							// iCurrMainState = EClientModifications;
// <MAPINFO_RESEND_MOD_END>
							}
						}
					}
				}
			break;
// <MAPINFO_RESEND_MOD_BEGIN>
		case EClientResendMapInfo:
			iPrevSyncState = iCurrMainState;
			if(iMapResendStatusPackage)
				{
				iCurrMainState = ESending;
				}
			else
				{
				iCurrMainState = EClientModifications;
				}
			break;
// <MAPINFO_RESEND_MOD_END>	
		case EClientModifications:
			iPrevSyncState = iCurrMainState;
			iCurrMainState = ESending;
			break;
			
		case EServerModifications:
			iPrevSyncState = iCurrMainState;
			if ( iDSContent->AllDatabasesAreInterruptedImmediately() )
				{
				iCurrMainState = EFinishing;
				}
			else
				{
				if ( iBusyStatusReceived )
					{
					iCurrMainState = EResultAlert;
					}
				else
					{
					if ( iCurrServerModState == EServerModMessageReceived )
						{
						iCurrMainState = EClientAlertNext;
						}
					else
						{
						iCurrMainState = EDataUpdateStatus;
						iDSObserver->OnSyncMLSyncProgress( MSyncMLProgressObserver::ESmlSendingMappingsToServer, 0, 0 );
						}
					}
				}
			break;
			
		case EDataUpdateStatus:
			iPrevSyncState = iCurrMainState;
			if ( iStatusPackage  && ( !iDSContent->AllDatabasesOneWayFromClient() ) )
				{
				iCurrMainState = ESending;
				}
			else
				{
				iCurrMainState = EFinishing;
				}
			break;
			
		case EMapAcknowledge:
// <MAPINFO_RESEND_MOD_BEGIN>
			if(iPrevSyncState==EClientResendMapInfo)
				{
				iCurrMainState = EClientModifications;
				// If map exitst remove map info, think to implement.
				iDSContent->CleanMapInfo();
				}
			else
				{
// <MAPINFO_RESEND_MOD_END>
			iPrevSyncState = iCurrMainState;
			if ( iDSContent->AllDatabasesAreInterruptedImmediately() )
				{
				iCurrMainState = EFinishing;
				}
			else
				{
				if ( iBusyStatusReceived )
					{
					iCurrMainState = EResultAlert;
					}
				else
					{
					if ( iDSCurrMapAcknowledgeState == EMapAckMessageReceived )
						{	
						iCurrMainState = EClientAlertNext;
						}
					else
						{
						iCurrMainState = EFinishing;
						}
					}
				}
// <MAPINFO_RESEND_MOD_BEGIN>
				}
// <MAPINFO_RESEND_MOD_END>
			break;
			
		case EResultAlert:
			iCurrMainState = ESending;
			break;
			
		case EClientAlertNext:
			iCurrMainState = ESending;
			break;
			
		case EServerAlertNext:
			if ( iDSContent->AllDatabasesAreInterruptedImmediately() )
				{
				iCurrMainState = EFinishing;
				}
			else
				{
				iCurrMainState = iPrevSyncState;
				}
			break;
			
		case ESending:
			if ( iDSContent->AllDatabasesAreInterrupted() ) 
				{
				iCurrMainState = EFinishing;
				}
			else
				{
				iCurrMainState = EReceiving;
				}
			break;
			
		case EReceiving:
			if ( ( !iFinalMessageFromClient ) &&
				 ( iPrevSyncState == EClientModifications || iPrevSyncState == EDataUpdateStatus ) )
				{
				iCurrMainState = EServerAlertNext;
				}
			else
			if ( iPrevSyncState == EClientInitialisation || iPrevSyncState == EServerInitialisation )
				{
				iCurrMainState = EServerInitialisation;
				}
			else
			if ( iPrevSyncState == EClientModifications || iPrevSyncState == EServerModifications )
				{
				iCurrMainState = EServerModifications;
				}	
			else
				{
				iCurrMainState = EMapAcknowledge;
				};
				
			iDSContent->UpdateInterruptedFlags();
			break;
			
		default:
			break;
		}
		
	InitialiseSubStates();
	}

// ---------------------------------------------------------
// CNSmlDSAgent::InitialiseSubStates()
// 
// ---------------------------------------------------------
//
void CNSmlDSAgent::InitialiseSubStates()
	{
	iCurrBeginningState = EBegin;
	iDSCurrBeginSubState = EBeginSubStart;
	iCurrClientInitState = EInitStartingMessage;
	iDSCurrServerInitState = EServerInitWaitingStartMessage;
	iDSCurrClientModState = EClientModStartingMessage;
	iCurrResultAlertState = EResultAlertStartingMessage;
	iCurrServerModState = EServerModWaitingStartMessage;
	iCurrDataUpdateStatusState = EStatusStartingMessage;
	iDSCurrMapAcknowledgeState = EMapAckWaitingStartMessage;
	iCurrResultAlertState = EResultAlertStartingMessage;
	iCurrClientAlertNextState = EClientAlertNextStartingMessage;
	iCurrServerAlertNextState = EServerAlertNextWaitingStartMessage;
	}
	
//
// Beginning state functions 
//
//
// ---------------------------------------------------------
// CNSmlDSAgent::BeginStateL()
// 
// ---------------------------------------------------------
void CNSmlDSAgent::BeginStateL() 
	{
	switch ( iDSCurrBeginSubState )
		{
		case EBeginSubStart:
			BeginSubStartStateL();
			break;
		case EBeginSubReadSettings:
			BeginSubReadSettingsStateL();
			break;
		case EBeginSubSyncmlCmdsInstance:
			BeginSubSyncmlCmdsInstanceStateL();
			break;
		case EBeginSubServerAlerting:
			BeginSubServerAlertingStateL();
			break;
		case EBeginSubOpenContentInstances:
			BeginSubOpenContentInstancesStateL();
			break;
// <MAPINFO_RESEND_MOD_BEGIN>
		case EBeginSubScanMapInfo:
			BeginSubScanMapInfo();
			break;
// <MAPINFO_RESEND_MOD_END>
		case EBeginSubCreateLUIDBuffers:
			BeginSubCreateLUIDBuffersStateL();
			break;
		default:
			User::Leave( KErrGeneral );
		}
	}

//
//  Server Modifications state functions 
//
// ---------------------------------------------------------
// CNSmlDSAgent::ServerModUpdatesStateL(()
// Handles Add, Replace or Delete command from server 
// ---------------------------------------------------------
void CNSmlDSAgent::ServerModUpdatesStateL( const TDesC8& aCmd, SmlGenericCmd_t* aContent )
	{
	DBG_FILE(_S8("CNSmlDSAgent::ServerModUpdatesStateL begins"));
	if ( iCurrServerModState != EServerModWaitingUpdateCommands )
		{
		iSyncMLCmds->StatusToUnsupportedCommandL( ETrue, aCmd, aContent->cmdID, aContent->flags );
		}
	if ( iDSContent->ContentIndexIsSet() )
		{
		if ( iDSContent->SyncType() == KNSmlDSOneWayFromClient ||
			 iDSContent->SyncType() == KNSmlDSOneWayFromClientByServer ||
			 iDSContent->SyncType() == KNSmlDSRefreshFromClient ||
			 iDSContent->SyncType() == KNSmlDSRefreshFromClientByServer )
			{
			iSyncMLCmds->StatusToUnsupportedCommandL( ETrue, aCmd, aContent->cmdID, aContent->flags );
			}
		else
			{
			iSyncMLCmds->ProcessUpdatesL( aCmd, aContent );
			}
		}
	else
		{
		iSyncMLCmds->ProcessUpdatesL( aCmd, aContent );
		}
	DBG_FILE(_S8("CNSmlDSAgent::ServerModUpdatesStateL ends"));
	}

// ---------------------------------------------------------
// CNSmlDSAgent::DataUpdateStatusStartingMessageStateL()
// Notify "Finalising" to UI, makes SyncHdr 
// ---------------------------------------------------------
void CNSmlDSAgent::DataUpdateStatusStartingMessageStateL()
	{
	DBG_FILE(_S8("CNSmlDSAgent::DataUpdateStatusStartingMessageStateL begins"));
	DataUpdateStatusStartingMessageStateBaseL();
	iDSContent->SetToFirst();
	DBG_FILE(_S8("CNSmlDSAgent::DataUpdateStatusStartingMessageStateL ends"));
	}

// ---------------------------------------------------------
// CNSmlDSAgent::DataUpdateStatusMakingMapStateL()
// Makes Map command(s)
// ---------------------------------------------------------
void CNSmlDSAgent::DataUpdateStatusMakingMapStateL()
	{
	DBG_FILE(_S8("CNSmlDSAgent::DataUpdateStatusMakingMapStateL begins"));
	CNSmlCmdsBase::TReturnValue ret;
	ret = iSyncMLCmds->DoMapL();
	switch ( ret )
		{
		case CNSmlCmdsBase::EReturnOK:
			iDSWaitingForMapAcknowledgement = ETrue;
			// there is something to send
			iStatusPackage = ETrue;
			if ( !iDSContent->SetToNext() )
				{
				iCurrDataUpdateStatusState = EStatusEndingMessage;
				}
			break;
		case CNSmlCmdsBase::EReturnEndData:
			if ( !iDSContent->SetToNext() )
				{
				iCurrDataUpdateStatusState = EStatusEndingMessage;
				}
			break;
		case CNSmlCmdsBase::EReturnBufferFull:
			iBufferFull = ETrue;
			iCurrDataUpdateStatusState = EStatusEndingMessage;
			break;
		default:
			break;
		}
	DBG_FILE(_S8("CNSmlDSAgent::DataUpdateStatusMakingMapStateL ends"));
	}

//
// Alert for next state functions 
//

// -----------------------------------------------------------------------------
// CNSmlDSAgent::ClientAlertNextMakingMapStateL
// Makes Map command(s).
// -----------------------------------------------------------------------------
//
void CNSmlDSAgent::ClientAlertNextMakingMapStateL()
	{
	DBG_FILE(_S8("CNSmlDSAgent::ClientAlertNextMakingMapStateL begins"));
	
	iDSContent->SetToFirst();
	
	do
		{
		if ( iSyncMLCmds->DoMapL() == CNSmlCmdsBase::EReturnBufferFull )
			{
			break;
			}
		}
	while ( iDSContent->SetToNext() ); 
	
	iCurrClientAlertNextState = EClientAlertNextEndingMessage;
	
	DBG_FILE(_S8("CNSmlDSAgent::ClientAlertNextMakingMapStateL ends"));
	}

// -----------------------------------------------------------------------------
// CNSmlDSAgent::NonceL
// Nonce (used in MD5 auth) from Agent Log 
// -----------------------------------------------------------------------------
//
HBufC* CNSmlDSAgent::NonceL() const
	{
	CNSmlDSAgentLog* agentLog = CNSmlDSAgentLog::NewLC();
	HBufC* nonce( NULL );
	
	if ( iProtocolVer == ESmlVersion1_1_2 )
		{
		HBufC* searchKey( iSyncServer->HostName().AllocLC() );
		nonce = agentLog->NonceL( *searchKey );
		CleanupStack::PopAndDestroy();  // searchKey
		}
	else if ( iProtocolVer == ESmlVersion1_2 )
		{
		nonce = agentLog->NonceL( *iDSServerId );
		}

	CleanupStack::PopAndDestroy(); // agentLog

	return nonce;
	}
	
// -----------------------------------------------------------------------------
// CNSmlDSAgent::SetNonceL
// Nonce from the server's challenge is stored to the Agent Log.
// -----------------------------------------------------------------------------
//
void CNSmlDSAgent::SetNonceL( const TDesC& aNonce ) const
	{
	CNSmlDSAgentLog* agentLog = CNSmlDSAgentLog::NewLC();

	if ( iProtocolVer == ESmlVersion1_1_2 )
		{
		HBufC* searchKey( iSyncServer->HostName().AllocLC() );
		agentLog->SetNonceL( *searchKey, aNonce );
		CleanupStack::PopAndDestroy();  // searchKey
		}
	else if ( iProtocolVer == ESmlVersion1_2 )
		{
		agentLog->SetNonceL( *iDSServerId, aNonce );
		}

	CleanupStack::PopAndDestroy(); // agentLog
	}
	
// -----------------------------------------------------------------------------
// CNSmlDSAgent::FinalizeSyncLog
// Writes the sync history after synchronisation.
// -----------------------------------------------------------------------------
//
void CNSmlDSAgent::FinalizeSyncLog()
	{
	TInt err( KErrNone );
	TRAP( err, FinalizeSyncLogL() );
	}

// -----------------------------------------------------------------------------
// CNSmlDSAgent::FinalizeSyncLogL
// Writes the sync history after synchronisation.
// -----------------------------------------------------------------------------
//
void CNSmlDSAgent::FinalizeSyncLogL()
	{
	if ( iSyncLog )
		{
		iDSContent->FinalizeSyncLogEvents();
	
		TTime time;
		time.UniversalTime();
		iSyncLog->SetResult( time, iError->SyncLogErrorCode() );
	
		CNSmlDSSettings* settings = CNSmlDSSettings::NewLC();
		CNSmlDSProfile* profile = settings->ProfileL( iProfileID );
		CleanupStack::PushL( profile );
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
    
    // Set sync stopped to P&S
    RProperty::Set( KPSUidDataSynchronizationInternalKeys, KDataSyncStatus, EDataSyncNotRunning );
  
    if ( iRepositorySSC )
        {
        iRepositorySSC->Set( KNsmlOpDsSyncInitiation, EDataSyncNotRunning );
        }
	
	ResetDSSessionInfoL();
	}

// -----------------------------------------------------------------------------
// CNSmlDSAgent::WriteWarningL
// Sync level warning message to the Sync Log.
// -----------------------------------------------------------------------------
//
void CNSmlDSAgent::WriteWarningL( TNSmlError::TNSmlAgentErrorCode aWarningCode )
	{
	iDSContent->SetToFirst();
	
	do
		{
		InfoMessageToSyncLogL( iDSContent->CurrentIndex(), aWarningCode );
		iDSContent->IncreaseWarnings();
		}
	while ( iDSContent->SetToNext() );	
	}
	
// -----------------------------------------------------------------------------
// CNSmlDSAgent::WriteWarningL
// Datastore level warning message to the Sync Log.
// -----------------------------------------------------------------------------
//
void CNSmlDSAgent::WriteWarningL( TInt appIndex, TNSmlError::TNSmlAgentErrorCode aWarningCode )
	{
	InfoMessageToSyncLogL( appIndex, aWarningCode );
	iDSContent->SetIndex( appIndex );
	iDSContent->IncreaseWarnings();
	}
	
// -----------------------------------------------------------------------------
// CNSmlDSAgent::WriteWarningL
// Datastore level warning message to the Sync Log.
// -----------------------------------------------------------------------------
//
void CNSmlDSAgent::WriteWarningL( TInt appIndex, TNSmlError::TNSmlSyncMLStatusCode aWarningCode )
	{
	InfoMessageToSyncLogL( appIndex, aWarningCode );
	iDSContent->SetIndex( appIndex );
	iDSContent->IncreaseWarnings();
	}

// ---------------------------------------------------------
// CNSmlDSAgent::CheckServerStatusCodeL()
// Status code validating
// ---------------------------------------------------------
void CNSmlDSAgent::CheckServerStatusCodeL( TInt aEntryID )
	{
	TDesC8& cmd = iSyncMLCmds->ResponseController()->Cmd( aEntryID );
	TNSmlError::TNSmlSyncMLStatusCode status = STATIC_CAST( TNSmlError::TNSmlSyncMLStatusCode, iSyncMLCmds->ResponseController()->StatusCode( aEntryID ) );
	TBool error( EFalse );
			
    // Store status code to cenrep if it is on the list
    if ( iErrorReportingEnabled )
        {
        StoreServerStatusCode( status );
        }
        
	switch ( status )
		{
		case TNSmlError::ESmlStatusInProgress:
			if ( cmd != KNSmlAgentSyncHdr )
				{
				error = ETrue;
				}
			break;
			
		case TNSmlError::ESmlStatusOK: 
		case TNSmlError::ESmlStatusItemAdded:
		case TNSmlError::ESmlStatusAcceptedForProcessing:
		case TNSmlError::ESmlStatusNonAuthoriveResponse:
		case TNSmlError::ESmlStatusNoContent:
		case TNSmlError::ESmlStatusResetContent:
		case TNSmlError::ESmlStatusConflictResolvedWithMerge:
		case TNSmlError::ESmlStatusConflictResolvedWithClientsCommand:
		case TNSmlError::ESmlStatusConflictResolvedWithDuplicate:
		case TNSmlError::ESmlStatusConflict:
		case TNSmlError::ESmlStatusConflictResolvedWithServerData:
		case TNSmlError::ESmlStatusSoftDeleteUpdateConflict:
		case TNSmlError::ESmlStatusDeleteWithoutArchive:
		case TNSmlError::ESmlStatusItemIsNotDeleted:
		case TNSmlError::ESmlStatusItemAccepted:
		case TNSmlError::ESmlStatusAlreadyExists:
			break;
			
		case TNSmlError::ESmlStatusAuthenticationAccepted: // 212
			if ( cmd == KNSmlAgentSyncHdr ) 
				{ 
				iAlreadyAuthenticated = ETrue;
				}
			break;
			
		case  TNSmlError::ESmlStatusUnauthorized: // 401
			if ( cmd == KNSmlAgentSyncHdr && iCurrMainState == EServerInitialisation ) 
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
						// If authentication has failed then rest of the package is not
					    // important anymore.
						User::Leave( status );
						}
					}
				else
					{
					// If authentication has failed then rest of the package is not
					// important anymore.
					User::Leave( status );
					}
				}
			break;
			
		case  TNSmlError::ESmlStatusClientAuthenticationRequired: // 407 
			if ( cmd == KNSmlAgentSyncHdr && iCurrMainState == EServerInitialisation ) 
				{
				if ( AuthenticationRequirementL( *iSyncMLCmds->ResponseController(), aEntryID ) )
					{
					iChallengeRequest = ETrue;
					SaveIfNonceL( *iSyncMLCmds->ResponseController(), aEntryID );
					iSyncMLCmds->ResponseController()->ResetL();
					return;
					}
				}
			error = ETrue;
			break;
			
		case TNSmlError::ESmlStatusRefreshRequired:
			if ( cmd == KNSmlAgentSync )
				{
				TInt appIndex( -1 );
				appIndex = iSyncMLCmds->ResponseController()->AppIndex( aEntryID );
				iDSContent->SetIndex( appIndex );
				iDSContent->SetSlowSyncReqForNextSyncL( ETrue ); 
				}
			//RD_SUSPEND_RESUME
		    if(iDSContent->SetToFirst())
		    {
		       do
		       {
		       	if ( iDSContent->GetSessionResumed() )
			    {
			      if(cmd==KNSmlAgentAlert)
			      {
			  		iDSContent->SetRefreshRequiredL(ETrue);
			  	  }
			     }
		       }while( iDSContent->SetToNext() );
		       	
			}	
			//RD_SUSPEND_RESUME	
			break;
			
		default:
			error = ETrue;
		}
		
	if ( cmd == KNSmlAgentSyncHdr )
		{
		SaveIfNonceL( *iSyncMLCmds->ResponseController(), aEntryID );
		}
		
	if ( cmd == KNSmlAgentAlert )
		{
		if ( iSyncMLCmds->ResponseController()->ResponseDetail( aEntryID ) == CNSmlResponseController::EResponseInitAlert )
			{ 
			if ( status == TNSmlError::ESmlStatusOptFeatureNotSupported )
				{
				status = (TNSmlError::TNSmlSyncMLStatusCode) TNSmlDSError::ESmlDSUnsupportedSyncType;
				}
			else
				{
				if ( status == TNSmlError::ESmlStatusCommandFailed ||
					 status == TNSmlError::ESmlStatusCommandNotAllowed ) 
					{
					iDSContent->SetIndex( iSyncMLCmds->ResponseController()->AppIndex ( aEntryID ) );
					if ( !iDSContent->IsSupportedSyncTypeByServerL() )
						{
						status = (TNSmlError::TNSmlSyncMLStatusCode) TNSmlDSError::ESmlDSUnsupportedSyncType;
						}
					else
						{
						if ( status == TNSmlError::ESmlStatusCommandFailed )
							{
							status = TNSmlError::ESmlStatusNotFound;
							}
						}
					}
				}
			}
		}
		
	if ( cmd == KNSmlAgentAdd || cmd == KNSmlAgentReplace || cmd == KNSmlAgentPartialReplace || cmd == KNSmlAgentDelete || cmd == KNSmlAgentMove || cmd == KNSmlAgentSoftDelete )
		{
		if ( !iSyncMLCmds->ResponseController()->MoreData( aEntryID ) )
			{
			iDSContent->SetIndex( iSyncMLCmds->ResponseController()->AppIndex ( aEntryID ) );
			TLex8 lex( iSyncMLCmds->ResponseController()->LUID( aEntryID ) );
			TInt val;
			lex.Val( val );
			iDSContent->SetAsReceivedL( val );
			}
		}
		
	if ( !error ) 
		{
		if ( cmd == KNSmlAgentAdd || cmd == KNSmlAgentReplace || cmd == KNSmlAgentPartialReplace )
			{
			if ( status == TNSmlError::ESmlStatusItemAdded )
				{
				iDSContent->IncreaseItemsAdded();
				}
			else
				{
				if ( status != TNSmlError::ESmlStatusItemAccepted )
					{
					iDSContent->IncreaseItemsChanged();
					}
				}
			}
			
		if ( cmd == KNSmlAgentDelete || cmd == KNSmlAgentSoftDelete ) 
			{
			if ( status != TNSmlError::ESmlStatusItemIsNotDeleted )
				{
				iDSContent->IncreaseItemsDeleted();
				}
			}
			
		if (cmd == KNSmlAgentMove)
			{
			if ( status == TNSmlError::ESmlStatusOK  )
				{
				iDSContent->IncreaseItemsMoved();
				}
			}
		}
	else
		{
		if ( cmd == KNSmlAgentAdd || cmd == KNSmlAgentReplace || cmd == KNSmlAgentPartialReplace || cmd == KNSmlAgentDelete || cmd == KNSmlAgentMove || cmd == KNSmlAgentSoftDelete )
			{
			iDSContent->IncreaseItemsFailed();
			}
		else
			{
			InterruptL( iSyncMLCmds->ResponseController()->AppIndex( aEntryID ), status, ETrue, ETrue );
			}
		}
	}

// -----------------------------------------------------------------------------
// CNSmlDSAgent::CheckCommandsAreReceivedL
// Checks that server has sent all expected commands.
// -----------------------------------------------------------------------------
//
void CNSmlDSAgent::CheckCommandsAreReceivedL()
	{
	if ( iDSDeviceInfoRequestByClient )
		{
		Interrupt( TNSmlDSError::ESmlDSDeviceInfoMissing, ETrue, EFalse );
		}

	iDSContent->SetToFirst();

	do
		{
		if ( !iDSContent->Interrupted() )
			{
			if ( iDSContent->WaitingInitAlert() )
				{
				InterruptL( iDSContent->CurrentIndex(), TNSmlError::ESmlAlertMissingInInitialisation, EFalse, EFalse );
				}
			}
		}
	while ( iDSContent->SetToNext() );	
	}	

//
// Authentications 
//

// -----------------------------------------------------------------------------
// CNSmlDSAgent::AuthTypeL
// Gets authentication type (basic/MD5) from the Agent Log.
// -----------------------------------------------------------------------------
//
CNSmlAgentBase::TAuthenticationType CNSmlDSAgent::AuthTypeL() const
	{
	CNSmlAgentBase::TAuthenticationType authType( ENoAuth );
	CNSmlDSAgentLog* agentLog = CNSmlDSAgentLog::NewLC();

	if ( iProtocolVer == ESmlVersion1_1_2 )
		{
		HBufC* searchKey( iSyncServer->HostName().AllocLC() );
		authType = (CNSmlAgentBase::TAuthenticationType)agentLog->AuthTypeL( *searchKey );
		CleanupStack::PopAndDestroy(); // searchKey
		}
	else if ( iProtocolVer == ESmlVersion1_2 )
		{
		authType = (CNSmlAgentBase::TAuthenticationType)agentLog->AuthTypeL( *iDSServerId );
		}

	CleanupStack::PopAndDestroy(); // agentLog

	return authType;
	}
	
// -----------------------------------------------------------------------------
// CNSmlDSAgent::SetAuthTypeL	
// Saves authentication type to the Agent Log.
// -----------------------------------------------------------------------------
//
void CNSmlDSAgent::SetAuthTypeL( CNSmlAgentBase::TAuthenticationType aAuthType ) const
	{
	CNSmlDSAgentLog* agentLog = CNSmlDSAgentLog::NewLC();

	if ( iProtocolVer == ESmlVersion1_1_2 )
		{
		HBufC* searchKey( iSyncServer->HostName().AllocLC() );
		agentLog->SetAuthTypeL( *searchKey, (TNSmlAgentLogAuthenticationType)aAuthType );
		CleanupStack::PopAndDestroy(); // searchKey
		}
	else if ( iProtocolVer == ESmlVersion1_2 )
		{
		agentLog->SetAuthTypeL( *iDSServerId, (TNSmlAgentLogAuthenticationType)aAuthType );
		}

	CleanupStack::PopAndDestroy(); // agentLog
	}

// -----------------------------------------------------------------------------
// CNSmlDSAgent::FreeResources
// Releases allocated resources.
// -----------------------------------------------------------------------------
//
void CNSmlDSAgent::FreeResources()
	{
	FreeBaseResources();
	if(iEnd)
		{
		delete iDSContent;
		iDSContent = NULL;
		}
	}

//
// Error Handling functions
//
// ---------------------------------------------------------
// CNSmlDSAgent::Interrupt()
// Synchronisation with all databases is interrupted
// Synchronisation level error code to Sync Log.
// ---------------------------------------------------------
void CNSmlDSAgent::Interrupt( TInt aErrorCode, TBool aImmediatelyInterruption, TBool aStatusError )
	{
	DBG_FILE(_S8("CNSmlDSAgent::Interrupt()"));
	
	if ( !iDSContent->AllDatabasesAreInterrupted() || aStatusError )
		{
		// Error code is updated only once, except errors due to
		// error Status codes from the server
		DoErrorEvent( MSyncMLProgressObserver::ESmlFatalError, aErrorCode, iDSContent->TaskId(), 0, 0 );
		}

	TBool ret( iDSContent->SetToFirst() ); 
	
	while ( ret )
		{

		if ( !iDSContent->Interrupted() )
			{
			iDSContent->SetInterrupted();
			}
			
		if ( aImmediatelyInterruption )
			{
			// if immediately interrupted then no response is sent
			iDSContent->SetImmediatelyInterrupted();
			}
			
		if ( aStatusError )
			{
			// error due to error Status from a server
			iDSContent->SetServerStatusError();
			}
			
		ret = iDSContent->SetToNext();
		
		}
	}

//RD_SUSPEND_RESUME
// ---------------------------------------------------------
// CNSmlDSAgent::MapAlertCode()
// Maps the alert code
// ---------------------------------------------------------

TNSmlPreviousSyncType CNSmlDSAgent::MapAlertCode(TDes8& aAlertCode)
{
    TNSmlPreviousSyncType alertcode=ETypeNotSet;
    
	if ( aAlertCode == KNSmlDSTwoWay )
		{
		alertcode = ESyncDSTwoWay;
		}
	else if ( aAlertCode == KNSmlDSSlowSync )
		{
		alertcode = ESyncDSSlowSync;
		}
	else if ( aAlertCode == KNSmlDSOneWayFromClient )
		{
		alertcode = ESyncDSOneWayFromClient;
		}
	else if ( aAlertCode == KNSmlDSRefreshFromClient )
		{
		alertcode = ESyncDSRefreshFromClient;
		}
	else if ( aAlertCode == KNSmlDSOneWayFromServer )
		{
		alertcode = ESyncDSOneWayFromServer;
		}
	else if(aAlertCode ==KNSmlDSRefreshFromServer)
	   {
		alertcode =ESyncDSRefreshFromServer;
	   }		
	else if(aAlertCode ==KNSmlDSTwoWayByServer)
	   {
		alertcode =ESyncDSTwoWayByServer;
	   }   
    else if(aAlertCode ==KNSmlDSOneWayFromClientByServer)
	   {
		alertcode =ESyncDSOneWayFromClientByServer;
	   }
    else if(aAlertCode ==KNSmlDSRefreshFromClientByServer)
	   {
		alertcode =ESyncDSRefreshFromClientByServer;
	   }
    else if(aAlertCode ==KNSmlDSOneWayFromServerByServer)
	   {
		alertcode =ESyncDSOneWayFromServerByServer;
	   }
   else if(aAlertCode ==KNSmlDSRefreshFromServerByServer)
	   {
		alertcode =  ESyncDSRefreshFromServerByServer;
	   }
  
  return(alertcode);
	   	
}
//RD_SUSPEND_RESUME

// CNSmlDSAgent::InterruptL()
// Synchronisation with a given database is interrupted
// ---------------------------------------------------------
void CNSmlDSAgent::InterruptL( TInt appIndex, TInt aErrorCode, TBool aImmediatelyInterruption, TBool aStatusError )
	{
	DBG_FILE(_S8("CNSmlDSAgent::InterruptL()"));
	if ( !iDSContent->SetIndex( appIndex ) )
		{
		// if index does not point to a database, interrupt all databases
		Interrupt( aErrorCode, aImmediatelyInterruption, aStatusError );
		}
	else
		{
		iDSContent->SetIndex( appIndex );
		if ( ( !iDSContent->Interrupted() )  || 
			 ( aStatusError && !iDSContent->ServerStatusError() ) )
			{
			// only one message to Sync Log expect Status code errors from a server 
			iDSContent->SetInterrupted();
			InfoMessageToSyncLogL( appIndex, aErrorCode ); 
			if ( aStatusError )
				{
				iDSContent->SetServerStatusError();
				}
			}
		if ( aImmediatelyInterruption )
			{
			// if immediately, any respose is not send
			iDSContent->SetImmediatelyInterrupted();
			}
		}
	}
	
// -----------------------------------------------------------------------------
// CNSmlDSAgent::FinaliseWhenErrorL
// Gets called if sync is broken down immediately (Leave errors)
// -----------------------------------------------------------------------------
//
void CNSmlDSAgent::FinaliseWhenErrorL()
	{
	DBG_FILE(_S8("CNSmlDSAgent::FinaliseWhenErrorL begins"));
	
// <MAPINFO_RESEND_MOD_BEGIN>
	if( iDSContent->ResendUsed() )
		{
		DBG_FILE(_S8("CNSmlDSAgent::FinaliseWhenErrorL - calling iDSContent->PackupRequestL..."));
		iDSContent->PackupRequestL( iDSClientNextSyncAnchor );
		}
// <MAPINFO_RESEND_MOD_END>
	
	//RD_SUSPEND_RESUME
	TInt keyVal;
	TRAPD (srerr ,ReadRepositoryL(KNsmlDsSuspendResume, keyVal));
	if(srerr == KErrNone && keyVal == 1)
	{
	    TNSmlSyncSuspendedState suspendedState;
	    if(iDSContent->SetToFirst())
	    {
	       do
	        {
	           suspendedState = CurrentState();	
	           iDSContent->SetSuspendedStateL(suspendedState);
	        }while( iDSContent->SetToNext() );
	   }
	   if(iDSContent->SetToFirst())
	   {
		 do
	    	{
		      TBuf8<3> syncType( iDSContent->SyncType()); 
	          TNSmlPreviousSyncType prevalertcode=MapAlertCode(syncType);
	          //Sync Type of the Current Sync session is stored.
	          iDSContent->SetPreviousSyncTypeL( prevalertcode );	
		     }while( iDSContent->SetToNext() );
	   }
       if(iDSContent->SetToFirst())
	   {
	      do
		  {
		     TBuf8<3> clientSyncType( iDSContent->ClientSyncType()); 
	         TNSmlPreviousSyncType prevClientType=MapAlertCode(clientSyncType);
	         //Sync Type proposed by Client in the Current Sync session is stored.
	         iDSContent->SetPreviousClientSyncTypeL( prevClientType );	
		  }while( iDSContent->SetToNext() );
	   }	
	}
	//RD_SUSPEND_RESUME
	
	iDSContent->FinalizeDatabasesL();
	
	iDSObserver->OnSyncMLSyncProgress( MSyncMLProgressObserver::ESmlDisconnected, 0, 0 );

	if(iPacketDataUnAvailable)
	{
		DoErrorEvent(MSyncMLProgressObserver::ESmlFatalError,
					 TNSmlError::ESmlCommunicationError, 0, 0, 0 );
	}
	
	//RD_AUTO_RESTART (BPSS-7GBFF3)
	DBG_FILE_CODE(iPacketDataUnAvailable,_S8("CNSmlDSAgent::FinaliseWhenErrorL PacketData is"));
	//Check whether the Auto Restart Feature is enabled or not
	TRAPD (arerr ,ReadRepositoryL(KNsmlDsAutoRestart, keyVal));
	
	if(arerr == KErrNone && keyVal == 1)
	{
		if(!iPacketDataUnAvailable && !iAutoRestartInitiatedSync )
		{
		
			DBG_FILE(_S8("CNSmlDSAgent::FinaliseWhenErrorL About to check the Profile ID"));
			
			if(CheckProfileIdL())
			{
				TerminateAutoRestart();
			}	
		}
	}
	//RD_AUTO_RESTART
	
	DBG_FILE(_S8("CNSmlDSAgent::FinaliseWhenErrorL ends"));
	}

// -----------------------------------------------------------------------------
// CNSmlDSAgent::Interrupted
// Checks if all datastores are interrupted.
// -----------------------------------------------------------------------------
//
TBool CNSmlDSAgent::Interrupted()
	{
	return iDSContent->AllDatabasesAreInterrupted();
	}
// -----------------------------------------------------------------------------
// CNSmlDSAgent::LaunchAutoRestart
// Launch the NETMON exe after saving the Profile Info to the CenRep and Killing
// the current DSSync session
// -----------------------------------------------------------------------------
//
void CNSmlDSAgent::LaunchAutoRestartL(TInt aError)
{
	DBG_FILE(_S8("CNSmlDSAgent::LaunchAutoRestart() begins"));
	
	
	//Check whether the Users' Network is in Roaming
	if(iDSNetmon)
	{
		if(iDSNetmon->IsRoaming())
		{
			DBG_FILE(_S8("CNSmlDSAgent::LaunchAutoRestart() Users' Network in Roaming Leaving the fuction"));
			User::Leave(aError);
		}
	}
	
	//Fix for BPSS-7H7JPF
	//Check whether the Auto-Restart is to be launched for the Profile that is already interrupted
	if(!iAutoRestartInitiatedSync)
	{
		TerminateAutoRestart();	
	}
	
		
	//Save the Profile Information
	SaveProfileInfoL();
	
	//Kill the DS Sync Sesssion
	StopDSSession();
	
	//Launch the NETMON exe
	LaunchNETMONL();
	
	DBG_FILE(_S8("CNSmlDSAgent::LaunchAutoRestart() ends"));
	
}

// -----------------------------------------------------------------------------
// CNSmlDSAgent::SaveProfileInfo
// Save the Profile Information in to the Central Repository
// -----------------------------------------------------------------------------
//
void CNSmlDSAgent::SaveProfileInfoL()
{
	DBG_FILE(_S8("CNSmlDSAgent::SaveProfileInfo() begins"));
	
	CNsmlProfileUtil* profileUtil = CNsmlProfileUtil::NewLC();
	profileUtil->InternalizeFromSettingsDBL(this->iProfileID);
	profileUtil->ExternalizeToCenrepL();
	profileUtil->WriteAccessPointIdL(iNetmonAPId);
	
	//Error fix for BPSS-7GYDJV
	RArray<TSmlTaskId> taskIdArray;
	if ( iDSSyncTypeArray && iDSSyncTypeArray->Count() > 0)
	{
		TInt count = iDSSyncTypeArray->Count();
		DBG_FILE_CODE(count, _S8("CNSmlDSAgent::SaveProfileInfo() TaskId Count"));
		for ( TInt i = 0; i < count; i++ )
		{
			TSmlTaskId taskid;
			taskid = iDSSyncTypeArray->At( i ).iTaskId;
			taskIdArray.AppendL( taskid );
		}
	}
	else
	{
		DBG_FILE(_S8("CNSmlDSAgent::SaveProfileInfo() No TaskId"));
		taskIdArray.Reset();
	}	
	profileUtil->ExternalizeTaskIdL(taskIdArray);	
	taskIdArray.Close();	
	
	CleanupStack::PopAndDestroy(profileUtil);
	
	DBG_FILE(_S8("CNSmlDSAgent::SaveProfileInfo() ends"));
}

// -----------------------------------------------------------------------------
// CNSmlDSAgent::LaunchNETMON
// -----------------------------------------------------------------------------
//
void CNSmlDSAgent::LaunchNETMONL()
{	
	DBG_FILE(_S8("CNSmlDSAgent::LaunchNETMONL() begins"));
	 //Kill if any existing NetMon exe is running
	 //TerminateAutoRestart();
	 //Starts the new Netmon process to register for the Connmon events
     //This exe will start the Sync session once the Network is available
     RProcess rp;
     TInt err = rp.Create(KNetMon,KNullDesC);
     User::LeaveIfError(err);
     rp.Resume();
     
     DBG_FILE(_S8("CNSmlDSAgent::LaunchNETMONL() ends"));
}

// -----------------------------------------------------------------------------
// CNSmlDSAgent::StopDSSession
// Kill the current DS Sync Session
// -----------------------------------------------------------------------------
//
void CNSmlDSAgent::StopDSSession()
{
	DBG_FILE(_S8("CNSmlDSAgent::StopDSSession() begins"));
	
	iCurrMainState = EFinishing;	
	Cancel();

	DBG_FILE(_S8("CNSmlDSAgent::StopDSSession() ends"));
}


// -----------------------------------------------------------------------------
// CNSmlDSAgent::DoErrorEvent
// Unsures error event
// -----------------------------------------------------------------------------
//
void CNSmlDSAgent::DoErrorEvent( MSyncMLProgressObserver::TErrorLevel
                                 aErrorLevel,
                                 TInt aError, TInt aTaskId,
                                 TInt aInfo1, TInt aInfo2 )
    {
    if ( !iErrorNotified )
        {
        iDSObserver->OnSyncMLSyncError( aErrorLevel, aError,
                                        aTaskId, aInfo1, aInfo2 );    
        iErrorNotified = ETrue;
        }    
    }
    
// -----------------------------------------------------------------------------
// CNSmlDSAgent::PrepareSyncL
// Prepares sync.
// -----------------------------------------------------------------------------
//
void CNSmlDSAgent::PrepareSyncL( CArrayFix<TNSmlContentSpecificSyncType>* aTypeArray )
	{
	iDSContent->InitContentDataL();
	
	if ( aTypeArray )
		{
		iDSSyncTypeArray = new ( ELeave ) CArrayFixFlat<TNSmlContentSpecificSyncType>( 4 );
		
		for ( TInt i = 0; i < aTypeArray->Count(); i++ )
			{
			TNSmlContentSpecificSyncType type;
			type.iTaskId = aTypeArray->At( i ).iTaskId;
			type.iType = aTypeArray->At( i ).iType;
			iDSSyncTypeArray->AppendL( type );
			}
		}
	}

// <MAPINFO_RESEND_MOD_BEGIN>

// ---------------------------------------------------------
// CNSmlDSAgent::BeginSubScanMapInfo()
//
// ---------------------------------------------------------
void CNSmlDSAgent::BeginSubScanMapInfo()
	{
	DBG_FILE(_S8("CNSmlDSAgent::BeginSubScanMapInfo() begins"));
	TBool endOfMapLoading = EFalse;
	TRAPD( err,ScanMapInfoL( endOfMapLoading ) );
	if( err != KErrNone || endOfMapLoading )
		{
		DBG_FILE_CODE(  endOfMapLoading, _S8("CNSmlDSAgent::BeginSubScanMapInfo(): endOfMapLoading: ") );
		DBG_FILE_CODE(  err, _S8("CNSmlDSAgent::BeginSubScanMapInfo(): Moved to SubCreateLUIDBuffers...") );
		iDSCurrBeginSubState = EBeginSubCreateLUIDBuffers;
		}
	DBG_FILE(_S8("CNSmlDSAgent::BeginSubScanMapInfo() ends"));
	}

//
//  Client Resend MapInformation state functions
//
// ---------------------------------------------------------
// CNSmlDSAgent::ClientResendMapInfoStateL()
// If mapinfo exists as a result of unsuccessfull last sync,
// it Makes Client Map Informtaion Package to be resent to Server
// Navigate according to state code
// ---------------------------------------------------------
void CNSmlDSAgent::ClientResendMapInfoStateL()
	{
	switch ( iCurrClientResendMapInfoState )
		{
		case EResendMapInfoStartingMessage:
			ClientResendMapInfoStartingMessageStateL();
			break;
		case EResendMapInfoMakingStatus:
			ClientResendMapInfoMakingStatusStateL();
			break;
		case EResendMapInfoMakingResults:
			ClientResendMapInfoMakingResultsStateL();
			break;
		case EResendMapInfoStartingSync:
			ClientResendMapInfoStartingSyncStateL();
			break;
		case EResendMapInfoEndingSync:
			ClientResendMapInfoEndingSyncStateL();
			break;
		case EResendMapInfoMakingMap:
			ClientResendMapInfoMakingMapStateL();
			break;
		case EResendMapInfoEndingMessage:
			ClientResendMapInfoEndingMessageStateL();
			break;
		}
	}

void CNSmlDSAgent::ClientResendMapInfoStartingMessageStateL()
	{
	DBG_FILE(_S8("CNSmlAgentBase::ClientResendMapInfoStartingMessageStateL begins"));
	iMapResendStatusPackage=EFalse;
	iBufferFull = EFalse;
	TBool mapFound=EFalse;
	iDSContent->SetToFirst();
	if(iDSContent->MapExists())
		{
		mapFound=ETrue;
		}
	while(iDSContent->SetToNext() && !mapFound)
		{
		if(iDSContent->MapExists())
			{
			mapFound=ETrue;
			}
		}
	if(mapFound)
		{
		DBG_FILE(_S8("CNSmlAgentBase::ClientResendMapInfoStartingMessageStateL Mapfound"));
		iSyncMLCmds->DoSyncHdrL();
		iDSContent->SetToFirst();
		iCurrClientResendMapInfoState = EResendMapInfoMakingStatus;
		}
	else
		{
		NextMainStateL();
		}
	DBG_FILE(_S8("CNSmlAgentBase::ClientResendMapInfoStartingMessageStateL ends"));
	}

void CNSmlDSAgent::ClientResendMapInfoMakingStatusStateL()
	{
	DBG_FILE(_S8("CNSmlDSAgent::ClientResendMapInfoMakingStatusStateL begins"));
	iSyncMLCmds->StatusContainer()->Begin();
	TBool found( ETrue );
	while( found )
		{
		SmlStatus_t* status;
		found = iSyncMLCmds->StatusContainer()->NextStatusElement( status, ETrue );
		if ( found )
			{
			iSyncMLCmds->DoStatusL( status );
			}
		}
	iCurrClientResendMapInfoState = EResendMapInfoMakingResults;
	DBG_FILE(_S8("CNSmlAgent::ClientResendMapInfoMakingStatusStateL ends"));
	}


void CNSmlDSAgent::ClientResendMapInfoMakingResultsStateL()
	{
	DBG_FILE(_S8("CNSmlDSAgent::ClientModMakingResultsStateL begins"));
	if ( iDSDeviceInfoRequestedByServer && !iDSContent->AllDatabasesAreInterrupted() )
		{
		iSyncMLCmds->DoResultsL();
		iDSDeviceInfoRequestedByServer = EFalse;
		}
	if ( iFinalMessageFromClient )
		{
		iDSContent->SetToFirst();
		}
	else
		{
		iDSContent->SetIndexToSaved();
		}
	iCurrClientResendMapInfoState = EResendMapInfoStartingSync;
	DBG_FILE(_S8("CNSmlAgent::ClientResendMapInfoMakingResultsStateL ends"));
	}
// ---------------------------------------------------------
// CNSmlDSAgent::ClientResendMapInfoEndingSyncStateL()
// Makes Sync command
// ---------------------------------------------------------
void CNSmlDSAgent::ClientResendMapInfoStartingSyncStateL()
	{
	DBG_FILE(_S8("CNSmlDSAgent::ClientResendMapInfoStartingSyncStateL begins"));
	CNSmlCmdsBase::TReturnValue ret;
	if ( !iDSContent->Interrupted() )
		{
		ret = iSyncMLCmds->DoStartSyncL();
		switch ( ret )
			{
			case CNSmlCmdsBase::EReturnOK:
				iCurrClientResendMapInfoState = EResendMapInfoEndingSync;
	                	iDSObserver->OnSyncMLSyncProgress( MSyncMLProgressObserver::ESmlSendingMappingsToServer, 0, 0 );
				break;
			case CNSmlCmdsBase::EReturnBufferFull:
				iCurrClientResendMapInfoState = EResendMapInfoEndingSync;
				iBufferFull = ETrue;
				break;
			default:
				break;
			}
		}
	else
		// skip interrupted database
		{
		if ( !iDSContent->SetToNext() )
			{
			iCurrClientResendMapInfoState = EResendMapInfoEndingSync;
			}
		}
	DBG_FILE(_S8("CNSmlDSAgent::ClientResendMapInfoStartingSyncStateL ends"));
	}
// ---------------------------------------------------------
// CNSmlDSAgent::ClientResendMapInfoEndingSyncStateL()
// End of Sync command
// ---------------------------------------------------------
void CNSmlDSAgent::ClientResendMapInfoEndingSyncStateL()
	{
	DBG_FILE(_S8("CNSmlDSAgent::ClientResendMapInfoEndingSyncStateL begins"));
	iSyncMLCmds->DoEndSyncL();
	if ( iBufferFull )
		{
		iCurrClientResendMapInfoState = EResendMapInfoEndingMessage;
		}
	else
		{
		if ( iDSContent->SetToNext() )
			{
			iCurrClientResendMapInfoState = EResendMapInfoStartingSync;
			}
		else
			{
			iCurrClientResendMapInfoState = EResendMapInfoMakingMap;
			iDSContent->SetToFirst();
			}
		}
	DBG_FILE(_S8("CNSmlDSAgent::ClientResendMapInfoEndingSyncStateL ends"));
	}

void CNSmlDSAgent::ClientResendMapInfoMakingMapStateL()
	{
	DBG_FILE(_S8("CNSmlAgent::ClientResendMapInfoMakingMapStateL begins"));
	CNSmlCmdsBase::TReturnValue ret=CNSmlCmdsBase::EReturnOK;
	ret = iSyncMLCmds->DoMapL();
	switch ( ret )
		{
		case CNSmlCmdsBase::EReturnOK:
			iDSWaitingForMapAcknowledgement = ETrue;
			// there is something to send
			iMapResendStatusPackage = ETrue;
			if ( !iDSContent->SetToNext() )
				{
				iCurrClientResendMapInfoState = EResendMapInfoEndingMessage;
				}
			break;
		case CNSmlCmdsBase::EReturnEndData:
			if ( !iDSContent->SetToNext() )
				{
				iCurrClientResendMapInfoState = EResendMapInfoEndingMessage;
				}
			break;
		case CNSmlCmdsBase::EReturnBufferFull:
			iBufferFull = ETrue;
			iCurrClientResendMapInfoState = EResendMapInfoEndingMessage;
			break;
		default:
			break;
		}
	DBG_FILE(_S8("CNSmlAgent::ClientResendMapInfoMakingMapStateL ends"));
	}


void CNSmlDSAgent::ClientResendMapInfoEndingMessageStateL()
	{
	DBG_FILE(_S8("CNSmlAgentBase::ClientResendMapInfoEndingMessageStateL begins"));
	iSyncMLCmds->DoEndMessageL( EFalse );

	if ( iBufferFull )
		{
		iFinalMessageFromClient = EFalse;
		}
	else
		{
		iFinalMessageFromClient = ETrue;
		}

	NextMainStateL();
	DBG_FILE(_S8("CNSmlAgentBase::ClientResendMapInfoEndingMessageStateL ends"));
	}

// ------------------------------------------------------------------------------------------------------------------
// CNSmlDSAgent::ScanMapInfoL
// Map information is searched for all the databases physical storage and calls functions to load map if exists
// ------------------------------------------------------------------------------------------------------------------
void CNSmlDSAgent::ScanMapInfoL( TBool& aEnd )
	{
	DBG_FILE(_S8("CNSmlDSAgent::ScanMapInfoL() begins"));

	if ( !iCommandIssued )
		{
		TBool ret = EFalse;
		if ( !iDSMapOpeningStarted )
			{
			ret = iDSContent->SetToFirst();
			iDSMapOpeningStarted = ETrue;
			}
		else
			{
			ret = iDSContent->SetToNext();
			}
		if ( ret )
			{
			iDSContent->ScanMapContentL(iStatus);
			iCommandIssued = ETrue;
			}
		else
			{
			aEnd = ETrue;
			}
		}
	else // command was completed
		{
		iCommandIssued = EFalse;
		if ( iStatus.Int() == KErrNotFound )
			{
			DBG_FILE_CODE(  iStatus.Int(), _S8("CNSmlDSAgent::ScanMapInfoL(): Error! (1)") );
			//InterruptL( iDSContent->CurrentIndex(), TNSmlDSError::ESmlDSLocalDatabaseError, ETrue, EFalse );
			//iDSContent->SetInterruptedBeforeSync();
			}
		else
			{
			DBG_FILE_CODE(  iStatus.Int(), _S8("CNSmlDSAgent::ScanMapInfoL(): Error! (2)") );
			User::LeaveIfError( iStatus.Int() );
			}
		}
	DBG_FILE(_S8("CNSmlDSAgent::ScanMapInfoL() ends"));
	}
// <MAPINFO_RESEND_MOD_END>

// CreateDSAgentL( MNSmlDSAgentObserver* aDSObserver )
// Return the instance of the CNSmlDSAgent
// ---------------------------------------------------------
//
EXPORT_C CNSmlDSAgent* CreateDSAgentL( MSyncMLProgressObserver* aDSObserver )
	{
	return CNSmlDSAgent::NewL( aDSObserver );
	}


//RD_SUSPEND_RESUME
// ------------------------------------------------------------------------------------------------------------------
// CNSmlDSAgent::SyncFailure()
// 
// ------------------------------------------------------------------------------------------------------------------
TBool  CNSmlDSAgent::SyncFailure()
{
   DBG_FILE_CODE(iError->SyncLogErrorCode(),_S8("CNSmlDSAgent::SyncFailure( ).Error code"));
   if(iError->SyncLogErrorCode())
      return(ETrue);
   else
      return(EFalse);
}

// ------------------------------------------------------------------------------------------------------------------
// CNSmlDSAgent::CurrentStateInFinishing()
// 
// ------------------------------------------------------------------------------------------------------------------

TNSmlSyncSuspendedState CNSmlDSAgent::CurrentStateInFinishing()
{
    TNSmlSyncSuspendedState state=EStateNotStarted;
    
    TBool chk=SyncFailure();
	DBG_FILE_CODE(chk,_S8("CNSmlDSAgent::CurrentStateInFinishing( )..Network failure check"));
	 
	if(iDSContent->AnyDatabaseIsInterrupted() || iPacketDataUnAvailable
			                             || chk )
			{
				switch(iPrevSyncState)
				{
					case EBeginning:
					case EClientInitialisation:
					case EServerInitialisation:
					case EResultAlert:
					     //if any sync failure occurs at pkt#1/pkt#2 stage, do not update
					     //sync anchors	     
					     if(chk)
					     	iUpdateLastAnchor=EFalse;	
					     state = EStateNotStarted;
						break;
					case EClientResendMapInfo:
					case EClientModifications:
						state = EStateClientModification;
						break;
					case EServerModifications:
						state = EStateServerModification;
						break;
					case EDataUpdateStatus:
						state = EStateDataUpdateStatus;
						break;
					case EMapAcknowledge:
						state = EStateMapAcknowledge;
						break;
						
				}
			}
			else
			{
				state = EStateNotStarted;
			}
			
	return(state);
}

// ------------------------------------------------------------------------------------------------------------------
// CNSmlDSAgent::CurrentStateInReceiving()
// 
// ------------------------------------------------------------------------------------------------------------------

TNSmlSyncSuspendedState CNSmlDSAgent::CurrentStateInReceiving()
{
   TNSmlSyncSuspendedState state=EStateNotStarted;
   if (iPrevSyncState == EClientInitialisation || iPrevSyncState == EServerInitialisation
						|| iPrevSyncState == EClientResendMapInfo )
	{
				state = EStateNotStarted;
	}
	else if(iPrevSyncState == EClientModifications)
	{
			state = EStateClientModification;
	}
	else if (iPrevSyncState == EDataUpdateStatus)
	{
			state = EStateDataUpdateStatus;
	}
	else if (iPrevSyncState == EServerModifications )
	{
			state = EStateServerModification;
	}
	else if (iPrevSyncState == EMapAcknowledge )
	{
			state = EStateMapAcknowledge;
	}
	
	return state;
}

// ------------------------------------------------------------------------------------------------------------------
// CNSmlDSAgent::CurrentStateInClientAlertNext()
// 
// ------------------------------------------------------------------------------------------------------------------

TNSmlSyncSuspendedState CNSmlDSAgent::CurrentStateInClientAlertNext()
{
	TNSmlSyncSuspendedState state=EStateNotStarted;
	if (iPrevSyncState == EServerModifications)
	{
	   state = EStateServerModification;
	}
	else if (iPrevSyncState == EMapAcknowledge)
	{
		state = EStateMapAcknowledge;
	}
	return(state);
}


// ------------------------------------------------------------------------------------------------------------------
// CNSmlDSAgent::CurrentStateInServerAlertNext()
// 
// ------------------------------------------------------------------------------------------------------------------

TNSmlSyncSuspendedState CNSmlDSAgent::CurrentStateInServerAlertNext()
{
	TNSmlSyncSuspendedState state=EStateNotStarted;
    if(iPrevSyncState == EClientModifications)
	{
	  state = EStateClientModification;
	}
	else if (iPrevSyncState == EDataUpdateStatus)
	{
	  state = EStateDataUpdateStatus;
	}
	return(state);
}

// ------------------------------------------------------------------------------------------------------------------
// CNSmlDSAgent::CurrentState()
// 
// ------------------------------------------------------------------------------------------------------------------


TNSmlSyncSuspendedState CNSmlDSAgent::CurrentState( )
{

	TNSmlSyncSuspendedState state = EStateNotStarted;	
	
	switch(iCurrMainState)
		{
		case EBeginning:
		case EClientInitialisation:
		case EServerInitialisation:
		case EResultAlert:
		     if(SyncFailure())
		     {
		       iUpdateLastAnchor=EFalse;	
		     }
		     state=EStateNotStarted;
		     break;
		case EFinishing:
			 state=CurrentStateInFinishing();
			 break;
	// <MAPINFO_RESEND_MOD_BEGIN>
		case EClientResendMapInfo:
// <MAPINFO_RESEND_MOD_END>
		case EClientModifications:
			state = EStateClientModification;
			break;
		case EServerModifications:
			state = EStateServerModification;
			break;
		case EDataUpdateStatus:
			state = EStateDataUpdateStatus;
			break;
		case EMapAcknowledge:
			state = EStateMapAcknowledge;
			break;
		
		case EClientAlertNext:
		     state=CurrentStateInClientAlertNext();
		     break;
		   
		case EServerAlertNext:
		     state=CurrentStateInServerAlertNext();
			 break;
		case ESending:
		case EReceiving:
		 	 {
	 		  state=CurrentStateInReceiving();			
			  break;
			 }
		default:
			state = EStateNotStarted;
			break;
		
		}
		DBG_FILE_CODE(state,_S8("CNSmlDSAgent::CurrentStateInFinishing( )..Network failure check"));
		if(state!=EStateNotStarted && iUpdateLastAnchor)
		   iUpdateLastAnchor=EFalse;
		return state;
	}
// ------------------------------------------------------------------------------------------------------------------
// CNSmlDSAgent::ReadRepositoryL()
// 
// ------------------------------------------------------------------------------------------------------------------
void CNSmlDSAgent::ReadRepositoryL(TInt aKey, TInt& aValue)
{
	const TUid KRepositoryId = KCRUidDataSyncInternalKeys;
	
    CRepository* rep = CRepository::NewLC(KRepositoryId);
    TInt err = rep->Get(aKey, aValue);
	User::LeaveIfError(err);
	CleanupStack::PopAndDestroy(rep);
}
//RD_SUSPEND_RESUME

//RD_AUTO_RESTART
// ------------------------------------------------------------------------------------------------------------------
// CNSmlDSAgent::CheckProfileIdL()
// 
// ------------------------------------------------------------------------------------------------------------------
TBool CNSmlDSAgent::CheckProfileIdL()
{
	DBG_FILE(_S8("CNSmlDSAgent::CheckProfileId() begins"));
	
	CNsmlProfileUtil* profileUtil = CNsmlProfileUtil::NewLC();	
	profileUtil->InternalizeFromCenrepL();
	TInt cachedprofileId = profileUtil->ProfileId();
	CleanupStack::PopAndDestroy(profileUtil);
	
	DBG_FILE_CODE(iProfileID, _S8("CNSmlDSAgent::CheckProfileId() Profile ID is"));
	DBG_FILE_CODE(cachedprofileId, _S8("CNSmlDSAgent::CheckProfileId() cached Profile ID is"));
	
	if(iProfileID == cachedprofileId)
	{
		return ETrue;
	}
		
	else
	{
		return EFalse;
	}
}

// ------------------------------------------------------------------------------------------------------------------
// CNSmlDSAgent::TerminateAutoRestart()
// 
// ------------------------------------------------------------------------------------------------------------------
void CNSmlDSAgent::TerminateAutoRestart()
{
	DBG_FILE(_S8("CNSmlDSAgent::TerminateAutoRestart() begins"));
	
	TFullName processName;
	TFindProcess process;
	while ( process.Next( processName ) != KErrNotFound )
	{
		//DBG_ARGS(_S("CNSmlDSAgent::TerminateAutoRestart() The Process name is %S"), &processName);
		
		if ( ( processName.Find( KAutoRestart ) != KErrNotFound ) )
		{
			RProcess rprocess;
			if (rprocess.Open(process, EOwnerProcess) == KErrNone)
			{
				DBG_FILE(_S8("CNSmlDSAgent::TerminateAutoRestart() Process is found, Killing the Process"));
				rprocess.Terminate(KErrNone);
				rprocess.Close();				
				DBG_FILE(_S8("CNSmlDSAgent::TerminateAutoRestart() Process killed successfully"));				
			}
		}
	}
	
	DBG_FILE(_S8("CNSmlDSAgent::TerminateAutoRestart() ends"));
}
// ------------------------------------------------------------------------------------------------------------------
// CNSmlDSAgent::IsSyncClientInitiated()
// 
// ------------------------------------------------------------------------------------------------------------------
TBool CNSmlDSAgent::IsSyncClientInitiated()
{
	DBG_FILE_CODE(iSyncInitiation, _S8("CNSmlDSAgent::IsSyncClientInitiated() The Sync Initiation status is"));
	
	if(iSyncInitiation == EClientInitiated)
		return ETrue;
	else
		return EFalse;
}
//RD_AUTO_RESTART

// ------------------------------------------------------------------------------------------------------------------
// CNSmlDSAgent::SetDSSessionInfoL()
// 
// ------------------------------------------------------------------------------------------------------------------
void CNSmlDSAgent::SetDSSessionInfoL(CNSmlDSProfile* aProfile)
    {
            
    CRepository* rep = CRepository::NewLC(KNsmlDsSessionInfoKey);
    TInt err = rep->Set(EDSSessionProfileId, aProfile->IntValue( EDSProfileId ));
    err = rep->Set(EDSSessionProfileName, aProfile->StrValue( EDSProfileDisplayName ));
    err = rep->Set(EDSSessionServerId, aProfile->StrValue( EDSProfileServerId ));
    User::LeaveIfError(err);
    CleanupStack::PopAndDestroy(rep);
    
    
    }

// ------------------------------------------------------------------------------------------------------------------
// CNSmlDSAgent::ResetDSSessionInfoL()
// 
// ------------------------------------------------------------------------------------------------------------------
void CNSmlDSAgent::ResetDSSessionInfoL()
    {
	
    CRepository* rep = CRepository::NewLC(KNsmlDsSessionInfoKey);
    TInt err = rep->Set(EDSSessionProfileId, KErrNotFound);
    err = rep->Set(EDSSessionProfileName, _L(""));
    err = rep->Set(EDSSessionServerId, _L(""));
    User::LeaveIfError(err);
    CleanupStack::PopAndDestroy(rep);
    }

// ------------------------------------------------------------------------------------------------------------------
// CNSmlDSAgent::StoreServerStatusCode(TInt aServerStatusCode)
// @description This function stores Sync ML Server Status code to cenrep for Operator profile sync if matched with configured list of codes,
//              and the same code can be used by any client for error logging.
//              If there are multiple status codes during sync, the last server status code is stored.
// @param aServerStatusCode Sync ML server status code while sync ongoing.
// ------------------------------------------------------------------------------------------------------------------
void CNSmlDSAgent::StoreServerStatusCode( TInt aServerStatusCode ) const
    {
    DBG_FILE(_S8("CNSmlDSAgent::StoreServerStatusCode() begins"));

    if( ( iServerStatusCodeArray.Count() == 0 && 
      ( aServerStatusCode >=  KErrorCodeRangeFirst ) && 
      ( aServerStatusCode <=  KErrorCodeRangeLast ) ) ||
      ( iServerStatusCodeArray.Find( aServerStatusCode ) != KErrNotFound ) )
        {
        TInt error = iRepositorySSC->Set( KNsmlOpDsSyncErrorCode, aServerStatusCode );
        if ( error != KErrNone )
            {
            DBG_FILE(_S8("Error in storing the server status code in cenrep"));
            }
        }

    DBG_FILE(_S8("CNSmlDSAgent::StoreServerStatusCode() ends"));
    }
// End of file  
