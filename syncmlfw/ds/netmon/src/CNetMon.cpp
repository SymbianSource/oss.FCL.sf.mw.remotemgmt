/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Monitors the network for availability and starts any pending
*                data sync sessions.
*
*/


// USER INCLUDES
#include "CNetMon.h"

// CONSTANTS
const TInt KValidInterrupt = -1;
const TInt KIntReset = 0;
//const TInt KStabilityTimeout = 5; // In seconds
const TInt KGlobalTimeout = 30; // In minutes
const TInt KNsmlDsAutoStartTimeoutKey = 3; // 0x0000003 defined in DS cenrep
const TInt KMicroSecond = 1000000;
const TInt KTimeUnit = 60;
const TUid KRepositoryId              = { 0x2000CF7E };

// ============================== MEMBER FUNCTIONS ============================

// ----------------------------------------------------------------------------
// CNetMon::NewL
// Two Phase Construction
// ----------------------------------------------------------------------------
CNetMon* CNetMon::NewL()
{
    DBG_FILE(_S8("CNetMon::NewL() begins"));
    
    CNetMon* self = new (ELeave) CNetMon;
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    
    DBG_FILE(_S8("CNetMon::NewL() ends"));
         
    return self;
}

// ----------------------------------------------------------------------------
// CNetMon::CNetMon
// Constructor
// ----------------------------------------------------------------------------
CNetMon::CNetMon() : CActive(CActive::EPriorityHigh)
{
    DBG_FILE(_S8("CNetMon::CNetMon() begins"));
        
	CActiveScheduler::Add(this);
	
    DBG_FILE(_S8("CNetMon::CNetMon() ends"));
}

// ----------------------------------------------------------------------------
// CNetMon::ConstructL
// Second phase constructor
// ----------------------------------------------------------------------------
void CNetMon::ConstructL()
{     
    DBG_FILE(_S8("CNetMon::ConstructL() begins"));
   
	iSyncSessionOpen = EFalse;
    iConMonBearerId = TConnMonBearerId(-1);
    iTimerCount = 0;
    
    iPrgPrevStatus = TStatus(-1);
	iPrgCurStatus = TStatus(-1);
	
    // Initial state
    iState = ERegister;
    IssueRequest();
    
    // Attach to sync status P&S keys    
	TInt err = iProperty.Attach(KPSUidDataSynchronizationInternalKeys, 
	                            KDataSyncStatus);

	User::LeaveIfError(err);
	
	iGlobalTimer = CPeriodic::NewL (EPriorityNormal) ;
	iGlobalTimeout = ReadTimeoutFromCenrepL();
	TInt timeout = KGlobalTimeout * KTimeUnit * KMicroSecond;
	iGlobalTimer->Start ( TTimeIntervalMicroSeconds32(timeout), 
	                      TTimeIntervalMicroSeconds32(timeout), 
	                      TCallBack(Timeout, this ) );		
	
    DBG_FILE(_S8("CNetMon::ConstructL() ends"));	
}

// ----------------------------------------------------------------------------
// CNetMon::~CNetMon
// Destructor*
// ----------------------------------------------------------------------------
CNetMon::~CNetMon()
{
	 DBG_FILE(_S8("CNetMon::~CNetMon() begins"));
	 
	 DBG_FILE(_S8("CNetMon::~CNetMon() ends"));
}

// ----------------------------------------------------------------------------
// CNetMon::RunL
// ---------------------------------------------------------------------------
void CNetMon::RunL()
{
    switch ( iState ) 
    {
		case ERegister:
		    DBG_FILE(_S8("CNetMon::RunL() State = ERegister"));
		    RegisterL();
		    break;
		
		case EMonitor:
		    DBG_FILE(_S8("CNetMon::RunL() State = EMonitor"));
		    NetworkUp1();				 
		    // Wait for network up event
		    break;

        case ENetworkUp1:
            DBG_FILE(_S8("CNetMon::RunL() State = ENetworkUp1"));
            //InitTimerL();  
            NetworkUp2(this);          
            break;

		case ENetworkUp2:
			DBG_FILE(_S8("CNetMon::RunL() State = ENetworkUp2"));
		    OtherSyncStatus();
		    break;
		    
    	case ESyncOk:
    		DBG_FILE(_S8("CNetMon::RunL() State = ESyncOk"));
    	    StartSyncL();
    	    break;
		    
    	case EInitSync:
    		DBG_FILE(_S8("CNetMon::RunL() State = EInitSync"));
    	    // Wait for sync job to complete
    	    break;

		case EComplete:
			DBG_FILE(_S8("CNetMon::RunL() State = EComplete"));
		    Complete();
		    break;
		    
		default:
		    break;
    }
}

// ----------------------------------------------------------------------------
// CNetMon::RunError
// ----------------------------------------------------------------------------
TInt CNetMon::RunError ( TInt aError )
{
    DBG_FILE_CODE(aError, _S8("CNetMon::RunError() The Error occurred is "));
    return KErrNone;
}

// ----------------------------------------------------------------------------
// CNetMon::DoCancel
// ----------------------------------------------------------------------------
void CNetMon::DoCancel()
{
	DBG_FILE(_S8("CNetMon::DoCancel() begins"));
	
	//CancelTimer();

    // Un-register for events
    iConnectionMonitor.CancelNotifications();
    iConnectionMonitor.Close();

    // Cleanup sync session
    CloseSyncSession();
	
	DBG_FILE(_S8("CNetMon::DoCancel() ends"));
}

// ----------------------------------------------------------------------------
// CNetMon::RegisterL
// ----------------------------------------------------------------------------
void CNetMon::RegisterL()
{
    DBG_FILE(_S8("CNetMon::RegisterL() begins"));
    
    // Register for events
    iConnectionMonitor.ConnectL();
    iConnectionMonitor.NotifyEventL(*this);      
        
    // Wait for events
    iState = EMonitor;
    IssueRequest();
    
    DBG_FILE(_S8("CNetMon::RegisterL() ends"));
}

// ----------------------------------------------------------------------------
// CNetMon::PacketDataAvailable
// ----------------------------------------------------------------------------
TBool CNetMon::PacketDataAvailable()
{
    DBG_FILE(_S8("CNetMon::PacketDataAvailable() begins"));
    
    TRequestStatus status;
    TBool val = EFalse;     
    if( iConMonBearerId != TConnMonBearerId(-1))
    {
    	 iConnectionMonitor.GetBoolAttribute(iConMonBearerId, 
					                         0,
					                         KPacketDataAvailability, 
					                         val,
					                         status );
    	User::WaitForRequest(status);			
    }
    else
    {
    	DBG_FILE(_S8("CNetMon::PacketDataAvailable() Invalid connection ID encountered"));
    	
	    TRequestStatus gprsstatus;
		
		iConnectionMonitor.GetBoolAttribute(EBearerIdGPRS, 
						                   0, 
						                   KPacketDataAvailability, 
						                   val, 
						                   gprsstatus);

		User::WaitForRequest(gprsstatus);

		if(val == EFalse)
		{
			TRequestStatus wcdmastatus;
			iConnectionMonitor.GetBoolAttribute(EBearerIdWCDMA, 
							                   0, 
							                   KPacketDataAvailability, 
							                   val, 
							                   wcdmastatus);

			User::WaitForRequest(wcdmastatus);
			
			if(val != EFalse)
			{
				iConMonBearerId = EBearerIdWCDMA;
			}
				
		}
		else
			iConMonBearerId = EBearerIdGPRS;
    }

	DBG_FILE_CODE(iConMonBearerId, _S8("CNetMon::PacketDataAvailable() The Connection Id is"));   			
    
    DBG_FILE(_S8("CNetMon::PacketDataAvailable() ends"));
    
    return val;
}

// ----------------------------------------------------------------------------
// CNetMon::NetworkUp1
// ----------------------------------------------------------------------------
void CNetMon::NetworkUp1()
{
    DBG_FILE(_S8("CNetMon::NetworkUp1() begins"));
    
    // Check if network is already up
    // Compensates for this process startup time
        
    if ( PacketDataAvailable() )
    {
    	iState = ENetworkUp1;
    	IssueRequest();
    }
    
    DBG_FILE(_S8("CNetMon::NetworkUp1() ends"));
}

// ----------------------------------------------------------------------------
// CNetMon::CancelTimer
// ----------------------------------------------------------------------------
/*void CNetMon::CancelTimer()
{
    DBG_FILE(_S8("CNetMon::CancelTimer() begins"));
    
    if ( iStabilityTimer )
    {
	    iStabilityTimer->Cancel();
	    delete iStabilityTimer;
	    iStabilityTimer = NULL;
    }
    
    DBG_FILE(_S8("CNetMon::CancelTimer() ends"));
}*/

// ----------------------------------------------------------------------------
// CNetMon::CancelGlobalTimer
// ----------------------------------------------------------------------------
void CNetMon::CancelGlobalTimer()
{
    DBG_FILE(_S8("CNetMon::CancelGlobalTimer() begins"));
    
    if ( iGlobalTimer )
    {
	    iGlobalTimer->Cancel();
	    delete iGlobalTimer;
	    iGlobalTimer = NULL;
    }
    
    DBG_FILE(_S8("CNetMon::CancelGlobalTimer() ends"));
}

// ----------------------------------------------------------------------------
// CNetMon::NetworkStable
// ----------------------------------------------------------------------------
void CNetMon::NetworkStable()
{
    DBG_FILE(_S8("CNetMon::NetworkStable() begins"));

    // Un-register for events
    iConnectionMonitor.CancelNotifications();

 	iState = ENetworkUp2;
    IssueRequest();
    
    DBG_FILE(_S8("CNetMon::NetworkStable() ends"));
}

// ----------------------------------------------------------------------------
// CNetMon::NetworkUp2
// ----------------------------------------------------------------------------
TInt CNetMon::NetworkUp2(TAny* aPtr)
{
    DBG_FILE(_S8("CNetMon::NetworkUp2() begins"));
   
    CNetMon* ptr = (CNetMon*) aPtr;
    
    if ( ptr->PacketDataAvailable() )
    {
        ptr->NetworkStable();
    }

    //ptr->CancelTimer();       
    
    DBG_FILE(_S8("CNetMon::NetworkUp2() ends"));
        
    return KErrNone;
}

// ----------------------------------------------------------------------------
// CNetMon::OtherSyncStatus
// ----------------------------------------------------------------------------
void CNetMon::OtherSyncStatus()
{
    DBG_FILE(_S8("CNetMon::OtherSyncStatus() begins"));

	TInt syncStatus = -1;
	
	TInt err = iProperty.Get( KPSUidDataSynchronizationInternalKeys, 
	                          KDataSyncStatus, 
	                          syncStatus );
	                      
    if ( err != KErrNotFound )  
    {
        if ( syncStatus != EDataSyncNotRunning )
        {
            iProperty.Subscribe(iStatus);
            SetActive();
            
            DBG_FILE(_S8("CNetMon::OtherSyncStatus() Other sync is in progress. Waiting ..."));
            DBG_FILE(_S8("CNetMon::OtherSyncStatus() ends"));
            return;
        }
    }
    
    DBG_FILE(_S8("CNetMon::OtherSyncStatus() No other sync is in progress"));
    
    iState = ESyncOk;
    IssueRequest();
    
    DBG_FILE(_S8("CNetMon::OtherSyncStatus() ends"));
}


// ----------------------------------------------------------------------------
// CNetMon::InitTimerL
// ----------------------------------------------------------------------------
/*void CNetMon::InitTimerL()
{
    DBG_FILE(_S8("CNetMon::InitTimerL() begins"));

    CancelTimer();
    
	// Start timer and associate NetworkUp2
	TInt timeout =  KStabilityTimeout * KMicroSecond;        

	iStabilityTimer = CPeriodic::NewL (EPriorityNormal) ;
	iStabilityTimer->Start ( TTimeIntervalMicroSeconds32(timeout), 
	                         TTimeIntervalMicroSeconds32(timeout), 
	                         TCallBack(NetworkUp2, this ) );
		                         
	                        
    DBG_FILE(_S8("CNetMon::InitTimerL() ends"));	                         	                         
}*/

// ----------------------------------------------------------------------------
// CNetMon::StartSyncL
// ----------------------------------------------------------------------------
void CNetMon::StartSyncL()
{
    DBG_FILE(_S8("CNetMon::StartSyncL() begins"));

    TBool doSync = EFalse;
    
    // Open sync session
    OpenSyncSessionL();
    
    doSync = AutoStartPolicyOkL();
    
    if ( doSync )
    {
    	//Set the Interrupt flag in the CenRep
		UpdateInterruptFlagL(KValidInterrupt);    	
	    
    	// Trigger sync here	    
	    // Register for sync events    
	    iSyncSession.RequestEventL(*this);     // for MSyncMLEventObserver events      
	    iSyncSession.RequestProgressL(*this);  // for MSyncMLProgressObserver events
	    
	    
	    //Error fix for BPSS-7GYDJV
	    CreateSyncJobL();
	       
	    iState = EInitSync;
	    IssueRequest();
    }
    else 
    {
    	iState = EComplete;
	    IssueRequest();
    }
    
    DBG_FILE(_S8("CNetMon::StartSyncL() ends"));
}

// ----------------------------------------------------------------------------
// CNetMon::CreateSyncJobL
// ----------------------------------------------------------------------------
void CNetMon::CreateSyncJobL()
{
	DBG_FILE(_S8("CNetMon::CreateSyncJobL() begins"));
	
	CNsmlProfileUtil* profileutil = CNsmlProfileUtil::NewLC();			 	
	RArray<TSmlTaskId> taskIdArray;
	
	//Get the TaskIds' from the Cenrep
	profileutil->InternalizeTaskIdL(taskIdArray);
	
	// Create the job
    if(taskIdArray.Count() > 0)
    {
    	DBG_FILE(_S8("CNetMon::CreateSyncJobL() Sync Job Created with Task Id"));
    	iSyncJob.CreateL(iSyncSession, iLastUsedProfileId, taskIdArray );
    }    	
    else
    {
    	DBG_FILE(_S8("CNetMon::CreateSyncJobL() Sync Job Created"));
    	iSyncJob.CreateL(iSyncSession, iLastUsedProfileId);
    }
        
    taskIdArray.Close();
    CleanupStack::PopAndDestroy(profileutil);
    
    DBG_FILE(_S8("CNetMon::CreateSyncJobL() ends"));
}

// ----------------------------------------------------------------------------
// CNetMon::OpenSyncSessionL
// ----------------------------------------------------------------------------
void CNetMon::OpenSyncSessionL()
{
	DBG_FILE(_S8("CNetMon: OpenSyncSessionL() Beigns"));
	
	if (!iSyncSessionOpen)
	{
	    TRAPD(err, iSyncSession.OpenL());
	    
	    if (err != KErrNone)
    	{
	    	DBG_FILE(_S8("CNetMon: RSyncMLSession::OpenL failed "));
	    	User::Leave(err);
    	}
	}
	iSyncSessionOpen = ETrue;
	
	DBG_FILE(_S8("CNetMon: OpenSyncSessionL() Ends"));
}

// ----------------------------------------------------------------------------
// CNetMon::Complete
// ----------------------------------------------------------------------------
void CNetMon::Complete()
{
    DBG_FILE(_S8("CNetMon::Complete() begins"));
    
	Cancel();

    // Close the server
    CActiveScheduler::Stop();
    
    DBG_FILE(_S8("CNetMon::Complete() ends"));
}

// ----------------------------------------------------------------------------
// CNetMon::CloseSyncSession
// ----------------------------------------------------------------------------
void CNetMon::CloseSyncSession()
{
	DBG_FILE(_S8("CNetMon::CloseSyncSession() begins"));
	
	if (iSyncSessionOpen)
	{
		iSyncSession.CancelEvent();
		iSyncSession.Close();
		iSyncSessionOpen = EFalse;
	}
	
	DBG_FILE(_S8("CNetMon::CloseSyncSession() ends"));
}


// ----------------------------------------------------------------------------
// CNetMon::EventL
// ----------------------------------------------------------------------------
void CNetMon::EventL ( const CConnMonEventBase& aEvent )
{
	DBG_FILE_CODE(aEvent.ConnectionId(), _S8(" CNetMon::EventL() The Connection ID is:"));
	switch ( aEvent.EventType() )
    {    
    	case EConnMonPacketDataAvailable:
        {                                   
            if ( aEvent.ConnectionId() == EBearerIdGPRS ||
                 aEvent.ConnectionId() == EBearerIdWCDMA )
            {
                DBG_FILE(_S8("CNetMon::EventL() Packet Data is available"));
                iConMonBearerId =  TConnMonBearerId(aEvent.ConnectionId());
				iState = ENetworkUp1;
				IssueRequest();
            }
                        
            break;
        }  
    }
}

// ----------------------------------------------------------------------------
// CNetMon::EventL
// ----------------------------------------------------------------------------
void CNetMon::IssueRequest()
{
    TRequestStatus* status = &iStatus;
    User::RequestComplete(status, KErrNone);
    SetActive();
}

// ----------------------------------------------------------------------------
// CNetMon::OnSyncMLSessionEvent
// ----------------------------------------------------------------------------
void CNetMon::OnSyncMLSessionEvent(TEvent aEvent, 
                                   TInt /*aIdentifier*/, 
                                   TInt /*aError*/, 
                                   TInt /*aAdditionalData*/)
{    
	
    switch ( aEvent )
    {
    	case EJobStart:
    	     DBG_FILE(_S8("CNetMon::OnSyncMLSessionEvent() Sync job started"));
    	     break;
    	
		case EJobStartFailed:
		case EJobRejected:
		case ETransportTimeout:
		case EServerSuspended:
		case EServerTerminated:	 
	  	case EJobStop:
			 if ( TStatus(-1) != iPrgPrevStatus &&  iPrgPrevStatus < ESmlConnected)    		
			 {
				// Wait for events
				TInt err(KErrNone);
				DBG_FILE_CODE(iPrgPrevStatus,_S8("CNetMon::OnSyncMLSessionEvent() Again Waiting for Events"));
				iSyncJob.Close();
				TRAP(err, iConnectionMonitor.NotifyEventL(*this));
				iState = EMonitor;
			    IssueRequest();
			 }
			 else
			 {
			 	TInt err(KErrNone);
			 	DBG_FILE_CODE(iPrgPrevStatus,_S8("CNetMon::OnSyncMLSessionEvent() Going to Complete state"));
			 	//Reset the Interrupt Flag Set in the CenRep
			 	TRAP(err,UpdateInterruptFlagL(KIntReset));
			 	
			 	DBG_FILE(_S8("CNetMon::OnSyncMLSessionEvent() Sync job stopped"));
			 	
			 	iState = EComplete;
	     		IssueRequest();	
    	     }
			 break;
    }
}

// ----------------------------------------------------------------------------
// CNetMon::UpdateInterruptFlagL
// ----------------------------------------------------------------------------
void CNetMon::UpdateInterruptFlagL(TInt aValue)
{
	CNsmlProfileUtil* profileutil = CNsmlProfileUtil::NewLC();			 	
	profileutil->WriteInterruptFlagL(aValue);
	CleanupStack::PopAndDestroy(profileutil);
}
	

// ----------------------------------------------------------------------------
// CNetMon::AutoStartPolicyOkL
// ----------------------------------------------------------------------------
TBool CNetMon::AutoStartPolicyOkL()
{
    DBG_FILE(_S8("CNetMon::AutoStartPolicyOkL() begins"));
 
    // Read the profile cached
    CNsmlProfileUtil* profileCached = CNsmlProfileUtil::NewL();
    CleanupStack::PushL(profileCached);
    profileCached->InternalizeFromCenrepL();    
    iLastUsedProfileId = profileCached->ProfileId();

    // (1) Check if the profile exists    
    RArray<TSmlProfileId> profileIdList;
    iSyncSession.ListProfilesL(profileIdList, ESmlDataSync);
        
    TInt index = profileIdList.Find(iLastUsedProfileId);
    if ( index == KErrNotFound )
    {
        DBG_FILE(_S8("CNetMon::AutoStartPolicyOkL() Profile is DELETED"));
        DBG_FILE(_S8("CNetMon::AutoStartPolicyOkL() ends"));   
        CleanupStack::PopAndDestroy(profileCached);         
    	return EFalse;
    }
    
    DBG_FILE(_S8("CNetMon::AutoStartPolicyOkL() Profile is available"));
    
    // (2) Check profile is not modified    
    CNsmlProfileUtil* profileCurrent = CNsmlProfileUtil::NewL();
    CleanupStack::PushL(profileCurrent);
    profileCurrent->InternalizeFromSettingsDBL(profileCached->ProfileId());
    
    TBool profileStatus = profileCached->IsSame(*profileCurrent);
    
    // Cleanup
    CleanupStack::PopAndDestroy(profileCurrent);
    CleanupStack::PopAndDestroy(profileCached);
    
    if ( ! profileStatus ) 
    {
        DBG_FILE(_S8("CNetMon::AutoStartPolicyOkL() Profile is MODIFIED"));
        DBG_FILE(_S8("CNetMon::AutoStartPolicyOkL() ends"));            
    	return EFalse;
    }

	DBG_FILE(_S8("CNetMon::AutoStartPolicyOkL() Profile is SAME"));

    // (3) Check we are in roaming
    TRequestStatus status;
    TInt roamingStatus(0);
    if( iConMonBearerId != TConnMonBearerId(-1))
    {
	    iConnectionMonitor.GetIntAttribute(iConMonBearerId, 
	                                       0, 
	                                       KNetworkRegistration, 
	                                       roamingStatus, 
	                                       status);

	    User::WaitForRequest(status);
    }
    
    DBG_FILE_CODE(roamingStatus, _S8("CNetMon::AutoStartPolicyOkL(), The Roaming Status is:"));
    
    if ( roamingStatus == ENetworkRegistrationRoaming )
    {
        DBG_FILE(_S8("CNetMon::AutoStartPolicyOkL() Device NOT in home network"));
        DBG_FILE(_S8("CNetMon::AutoStartPolicyOkL() ends"));            
    	return EFalse;	
    }
    
    DBG_FILE(_S8("CNetMon::AutoStartPolicyOkL() Device in home network"));
    
    DBG_FILE(_S8("CNetMon::AutoStartPolicyOkL() ends"));
      	
	return ETrue;
}

// ----------------------------------------------------------------------------
// CNetMon::Timeout
// ----------------------------------------------------------------------------
TInt CNetMon::Timeout(TAny* aPtr)
{
    DBG_FILE(_S8("CNetMon::Timeout() begins"));
   
    CNetMon* ptr = (CNetMon*) aPtr;
    
    DBG_FILE_CODE(ptr->iTimerCount, _S8("CNetMon::Timeout() The time out count is"));
    
    ptr->iTimerCount = ptr->iTimerCount + 1;
    
    if(ptr->iTimerCount == (ptr->iGlobalTimeout * 2))
    {
    	ptr->iTimerCount = 0;
        
	    ptr->CancelGlobalTimer();
	    ptr->Complete();    
    }
    
    DBG_FILE(_S8("CNetMon::Timeout() ends"));
        
    return KErrNone;
}

// ----------------------------------------------------------------------------
// CNetMon::ReadTimeoutFromCenrepL
// ----------------------------------------------------------------------------
TInt CNetMon::ReadTimeoutFromCenrepL()
{
    DBG_FILE(_S8("CNetMon::ReadTimeoutFromCenrepL() begins"));
   
    TInt timeoutInHours = 0;
  
    // Read the value from cenrep  
    CRepository* repository = CRepository::NewLC(KRepositoryId);
    TInt err = repository->Get(KNsmlDsAutoStartTimeoutKey, timeoutInHours);
    CleanupStack::PopAndDestroy(repository);
    User::LeaveIfError(err);    
    
    DBG_FILE_CODE(timeoutInHours, _S8("CNetMon::ReadTimeoutFromCenrepL() The value read from the cenrep is"));
    
    //TInt timeout = timeoutInHours * 60 * 60 * 1000000;

    DBG_FILE(_S8("CNetMon::ReadTimeoutFromCenrepL() ends"));
        
    return timeoutInHours; 
}

// -----------------------------------------------------------------------------
// CNetMon::OnSyncMLSyncError (from MSyncMLProgressObserver)
//
// -----------------------------------------------------------------------------
//
void CNetMon::OnSyncMLSyncError(TErrorLevel aErrorLevel, TInt aError, TInt /*aTaskId*/, TInt /*aInfo1*/, TInt /*aInfo2*/)
{
	DBG_FILE( _S8("CNetMon::OnSyncMLSyncError START") );

	DBG_FILE_CODE( aErrorLevel, _S8("CNetMon::OnSyncMLSyncError Error Level is") );
	DBG_FILE_CODE( aError, _S8("CNetMon::OnSyncMLSyncError Error is") );

	DBG_FILE( _S8("CNetMon::OnSyncMLSyncError END") );
}


// -----------------------------------------------------------------------------
// CNetMon::OnSyncMLSyncProgress (from MSyncMLProgressObserver)
//
// -----------------------------------------------------------------------------
//
void CNetMon::OnSyncMLSyncProgress(TStatus aStatus, TInt aInfo1, TInt /*aInfo2*/)
{
	DBG_FILE( _S8("CNetMon::OnSyncMLSyncProgress START") );

	DBG_FILE_CODE( aInfo1, _S8("CNetMon::OnSyncMLSyncProgress Info1 is") );
	DBG_FILE_CODE( aStatus, _S8("CNetMon::OnSyncMLSyncProgress Status is") );

	iPrgPrevStatus = iPrgCurStatus;

	iPrgCurStatus = aStatus; 
			
	DBG_FILE( _S8("CNetMon::OnSyncMLSyncProgress END") );
}


// -----------------------------------------------------------------------------
// CNetMon::OnSyncMLDataSyncModifications (from MSyncMLProgressObserver)
//
// -----------------------------------------------------------------------------
//
void CNetMon::OnSyncMLDataSyncModifications(TInt aTaskId, 
                      const TSyncMLDataSyncModifications& aClientModifications,
                      const TSyncMLDataSyncModifications& aServerModifications)
{
	//Do Nothing
}



// End of file

