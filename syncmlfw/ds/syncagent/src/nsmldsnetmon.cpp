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
* Description:  The Conmon Active object listening for the Network unavalibility
                 events and launches the Auto-restart exe
*
*/


// USER INCLUDES
#include "nsmldsnetmon.h"
#include <nsmldebug.h>

//Constants
const TInt KNsmlDsAutoStartTimeoutKey = 4; // 0x0000004 defined in DS cenrep
const TUid KRepositoryId              = { 0x2000CF7E };

// -----------------------------------------------------------------------------
// StaticDoStopDSSession()
// Global function to handle the timeout 
// -----------------------------------------------------------------------------
//	

static TInt StaticLaunchAutoRestartL(TAny *aPtr)
{
	CNsmlDSNetmon* srv = (CNsmlDSNetmon*) aPtr;	
	//Stop the DS Session
	return srv->LaunchAutoRestartL();
}

// ============================== MEMBER FUNCTIONS ============================

// ----------------------------------------------------------------------------
// CNsmlDSNetmon::NewL
// Two Phase Construction
// ----------------------------------------------------------------------------
CNsmlDSNetmon* CNsmlDSNetmon::NewL(CNSmlDSAgent& aDSAgent)
{
    CNsmlDSNetmon* self = new (ELeave) CNsmlDSNetmon;
    CleanupStack::PushL(self);
    self->ConstructL(aDSAgent);
    CleanupStack::Pop(); 
    return self;
}

// ----------------------------------------------------------------------------
// CNsmlDSNetmon::CNsmlDSNetmon
// Constructor
// ----------------------------------------------------------------------------
CNsmlDSNetmon::CNsmlDSNetmon() : CActive(CActive::EPriorityStandard)
{
	CActiveScheduler::Add(this);
}

// ----------------------------------------------------------------------------
// CNsmlDSNetmon::ConstructL
// Second phase constructor
// ----------------------------------------------------------------------------
void CNsmlDSNetmon::ConstructL(CNSmlDSAgent& aDSAgent)
{ 
	DBG_FILE(_S8("CNsmlDSNetmon::ConstructL() begins"));
	
    // Initial state
    iState = ERegister;
    iDSAgent=&aDSAgent;
    IssueRequest();
    
    //Auto_Restart
    iDSAgent->iPacketDataUnAvailable = EFalse;    
    
    iRoamingStatus = 0;
    
    iAutoRestartTimer = NULL;
    
    DBG_FILE(_S8("CNsmlDSNetmon::ConstructL() ends"));
}

// ----------------------------------------------------------------------------
// CNsmlDSNetmon::~CNsmlDSNetmon
// Destructor
// ----------------------------------------------------------------------------
CNsmlDSNetmon::~CNsmlDSNetmon()
{	
	DBG_FILE(_S8("CNsmlDSNetmon::~CNsmlDSNetmon() begins"));
	
	Cancel();
	Complete();
	
	if ( iAutoRestartTimer ) 
	{
		iAutoRestartTimer->Cancel();
		delete iAutoRestartTimer;
		iAutoRestartTimer = NULL;
	}
	
	DBG_FILE(_S8("CNsmlDSNetmon::~CNsmlDSNetmon() ends"));
}

// ----------------------------------------------------------------------------
// CNsmlDSNetmon::DoCancel
// ----------------------------------------------------------------------------
void CNsmlDSNetmon::DoCancel()
{
}

// ----------------------------------------------------------------------------
// CNsmlDSNetmon::RunL
// ----------------------------------------------------------------------------
void CNsmlDSNetmon::RunL()
{
    switch ( iState ) 
    {
		case ERegister:
		    RegisterL();
		    break;
		
        case EComplete:
		    Complete();
		    break;
		    
		default:
		    break;
    }
}

// ----------------------------------------------------------------------------
// CNsmlDSNetmon::RunError
// ----------------------------------------------------------------------------
TInt CNsmlDSNetmon::RunError ( TInt aError )
{
    return KErrNone;
}

// ----------------------------------------------------------------------------
// CNsmlDSNetmon::RegisterL
// ----------------------------------------------------------------------------
void CNsmlDSNetmon::RegisterL()
{
	DBG_FILE(_S8("CNsmlDSNetmon::RegisterL() Begins"));
	
    // Register for events
    iConnectionMonitor.ConnectL();
    iConnectionMonitor.NotifyEventL(*this);    
    
    TInt err = iConnectionMonitor.SetUintAttribute( EBearerIdAll, 
         	        	                           0, 
            	                                   KSignalStrengthThreshold, 
                	                               1 );   
	//Check whether the Users' Network is in Roaming
	iRoamingStatus = 0;
	TRequestStatus gprsstatus;
	
	iConnectionMonitor.GetIntAttribute(EBearerIdGPRS, 
	                   0, 
	                   KNetworkRegistration, 
	                   iRoamingStatus, 
	                   gprsstatus);

	User::WaitForRequest(gprsstatus);

	TRequestStatus wcdmastatus;
	if(iRoamingStatus == 0)
	{
		iConnectionMonitor.GetIntAttribute(EBearerIdWCDMA, 
                   0, 
                   KNetworkRegistration, 
                   iRoamingStatus, 
                   wcdmastatus);

		User::WaitForRequest(wcdmastatus);
	}
	DBG_FILE_CODE(iRoamingStatus, _S8("CNsmlDSNetmon::RegisterL(), The Roaming Status is:"));
    
    
    DBG_FILE(_S8("CNsmlDSNetmon::RegisterL() ends"));
}

// ----------------------------------------------------------------------------
// CNsmlDSNetmon::Complete
// ----------------------------------------------------------------------------
void CNsmlDSNetmon::Complete()
{
	DBG_FILE(_S8("CNsmlDSNetmon::Complete() begins"));
	
     // Un-register for events
    iConnectionMonitor.CancelNotifications();
    iConnectionMonitor.Close();

    
    DBG_FILE(_S8("CNsmlDSNetmon::Complete() end"));
}

// ----------------------------------------------------------------------------
// CNsmlDSNetmon::EventL
// ----------------------------------------------------------------------------
void CNsmlDSNetmon::EventL ( const CConnMonEventBase& aEvent )
{    
   DBG_FILE(_S8("CNsmlDSNetmon::EventL() Begins"));
   
   DBG_FILE_CODE(aEvent.ConnectionId(), _S8("The Connection ID is:"));	
   DBG_FILE_CODE(aEvent.EventType(), _S8("The Event Type is:"));
   
   const CConnMonNetworkStatusChange * eventNetworkStatus;
   eventNetworkStatus = ( const CConnMonNetworkStatusChange* ) &aEvent;
   
   const CConnMonConnectionStatusChange* eventConnectionStatus;
   eventConnectionStatus = ( const CConnMonConnectionStatusChange* ) &aEvent;
   
   DBG_FILE_CODE(eventNetworkStatus->NetworkStatus(), _S8("The Network Status is:"));
   DBG_FILE_CODE(eventConnectionStatus->ConnectionStatus(), _S8("The Connection Status is:"));

    //Fix for BPSS-7GBGV7
    if( iDSAgent->IsSyncClientInitiated() && 
        (iDSAgent->iAllowAutoRestart ) )
	    
	{
		switch(aEvent.ConnectionId())
	 	{
	 		case EBearerIdGPRS:
	 		case EBearerIdWCDMA:
	 		{
	 			switch(aEvent.EventType())
	 			{
	 				case EConnMonNetworkStatusChange:
	 				{
						switch(eventNetworkStatus->NetworkStatus())
						{
							//Network Down Scenario
							case EConnMonStatusNotAvailable:
							//General to Offline Scenario
							case EConnMonStatusUnattached:
							//Incoming Voice Call Scenario
							case EConnMonStatusSuspended:
							{
								//Launch NetMon
								StartTimerL();
							}
							break;
							
							//Network Up Scenario
							case EConnMonStatusActive:
							{
								//Kill NetMon
								StopTimer();
							}
							break;
						}
					
	 				}
	 				break;
	 				
	 				//Offline to General Scenario
	 				case EConnMonPacketDataAvailable:
	 				{
	 					//Kill Netmon
	 					StopTimer();
	 				}
	 				break;
	 			}
	 		}
	 		break;
	 		
	 		default:
	 			DBG_FILE(_S8("CNsmlDSNetmon::EventL() In the Default Case"));
		 		if( eventConnectionStatus->ConnectionStatus() == KConnectionClosed )
				{
					//Launch NetMon
					StartTimerL();
				}
	 		break;
	 		
	 	}
	}
	
	DBG_FILE(_S8("CNsmlDSNetmon::EventL() Ends"));
}
                  
// ----------------------------------------------------------------------------
// CNsmlDSNetmon::StartTimerL
// ----------------------------------------------------------------------------
void CNsmlDSNetmon::StartTimerL()
{
   if(!iDSAgent->iPacketDataUnAvailable)
   {   		
   		DBG_FILE(_S8("CNsmlDSNetmon::EventL() Packet Data is not Available"));
   		
   		iDSAgent->iPacketDataUnAvailable = ETrue;
   	
		TInt timeout = ReadTimeoutFromCenrepL();
		
		if(timeout <= 0)
		{
			DBG_FILE(_S8("CNsmlDSNetmon::EventL() Timer Value is not Valid Launching the Netmon"));
			LaunchAutoRestartL();
		}

		else
		{
			DBG_FILE(_S8("CNsmlDSNetmon::EventL() Starting the Timer"));
		
			iPacketDataUnAvailableTime.HomeTime();
			
			if ( iAutoRestartTimer ) 
			{
				iAutoRestartTimer->Cancel();
				delete iAutoRestartTimer;
				iAutoRestartTimer = NULL;
			}
			iAutoRestartTimer = CPeriodic::NewL (EPriorityNormal) ;
			iAutoRestartTimer->Start (
					TTimeIntervalMicroSeconds32( timeout )
				  , TTimeIntervalMicroSeconds32( timeout )
				  , TCallBack(StaticLaunchAutoRestartL,this) ) ;
		}
		         		
   	}
}
// ----------------------------------------------------------------------------
// CNsmlDSNetmon::StopTimer
// ----------------------------------------------------------------------------
void CNsmlDSNetmon::StopTimer()
{
	if(iDSAgent->iPacketDataUnAvailable)
 	{
     	DBG_FILE(_S8("CNsmlDSNetmon::EventL() Packet Data is Available"));
		
 		iDSAgent->iPacketDataUnAvailable = EFalse;
 		
 		iPacketDataAvailableTime.HomeTime();
	    TTimeIntervalSeconds duration;
	    iPacketDataAvailableTime.SecondsFrom(iPacketDataUnAvailableTime,duration);
	    
		if ( iAutoRestartTimer ) 
		{
			iAutoRestartTimer->Cancel();
			delete iAutoRestartTimer;
			iAutoRestartTimer = NULL;
		}
	    //if(duration.Int() >= (5 * 60))
		//{
			//Invoke the NETMON exe and Kill the session
		//	iDSAgent->LaunchAutoRestartL(KErrNone);
		//}
 	}
}

// ----------------------------------------------------------------------------
// CNsmlDSNetmon::LaunchAutoRestart
// ----------------------------------------------------------------------------
TBool CNsmlDSNetmon::LaunchAutoRestartL()
{
	iDSAgent->LaunchAutoRestartL(KErrNone);
	return ETrue;
}


// ----------------------------------------------------------------------------
// CNsmlDSNetmon::IssueRequest
// ----------------------------------------------------------------------------
void CNsmlDSNetmon::IssueRequest()
{
    TRequestStatus* status = &iStatus;
    User::RequestComplete(status, KErrNone);
    SetActive();
}

// ----------------------------------------------------------------------------
// CNsmlDSNetmon::IsRoaming
// ----------------------------------------------------------------------------
TBool CNsmlDSNetmon::IsRoaming()
{
	if(iRoamingStatus == ENetworkRegistrationRoaming)
		return ETrue;
	else
		return EFalse;
}

// ----------------------------------------------------------------------------
// CNsmlDSNetmon::ReadTimeoutFromCenrepL
// ----------------------------------------------------------------------------
TInt CNsmlDSNetmon::ReadTimeoutFromCenrepL()
{
    DBG_FILE(_S8("CNsmlDSNetmon::ReadTimeoutFromCenrepL() begins"));
   
    TInt timeoutInMinutes = 0;
  
    // Read the value from cenrep  
    CRepository* repository = CRepository::NewLC(KRepositoryId);
    TInt err = repository->Get(KNsmlDsAutoStartTimeoutKey, timeoutInMinutes);
    User::LeaveIfError(err);    
    CleanupStack::PopAndDestroy(repository);
    
    TInt timeout = timeoutInMinutes * 60 * 1000000;

    DBG_FILE(_S8("CNsmlDSNetmon::ReadTimeoutFromCenrepL() ends"));
        
    return timeout; 
}


// End of file

