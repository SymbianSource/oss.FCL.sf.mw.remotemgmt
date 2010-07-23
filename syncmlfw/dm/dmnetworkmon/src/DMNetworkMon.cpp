/*
 * Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies). 
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
 * Description:  SyncML DM NetMon
 *
 */

// USER INCLUDES
#include "DMNetworkMon.h"

#include <SyncMLClient.h>
#include <SyncMLClientDM.h>
#include <nsmldmauthinfo.h>
#include <nsmlconstants.h>
#include <AknGlobalNote.h>
#include <e32const.h>
#include <DevManInternalCRKeys.h>
#include "OnlineSupportLogger.h"

// CONSTANTS

const TInt KGlobalTimeout = 30; // In minutes
//const TInt KNsmlDMNetMonTimeoutKey = 13; // 0x0000013 defined in DM cenrep
const TInt KMicroSecond = 1000000;
const TInt KTimeUnit = 60;

// ============================== MEMBER FUNCTIONS ============================

// ----------------------------------------------------------------------------
// CDMNetworkMon::NewL
// Two Phase Construction
// ----------------------------------------------------------------------------
CDMNetworkMon* CDMNetworkMon::NewL()
    {
    LOGSTRING("CDMNetworkMon::NewL() begins");

    CDMNetworkMon* self = new (ELeave) CDMNetworkMon;
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);



    LOGSTRING("CDMNetworkMon::NewL() ends");

    return self;
    }

// ----------------------------------------------------------------------------
// CDMNetworkMon::CDMNetworkMon
// Constructor
// ----------------------------------------------------------------------------
CDMNetworkMon::CDMNetworkMon() : CActive(CActive::EPriorityHigh)
    {
    LOGSTRING("CDMNetworkMon::CDMNetworkMon() begins");

    CActiveScheduler::Add(this);

    LOGSTRING("CDMNetworkMon::CDMNetworkMon() ends");
    }

// ----------------------------------------------------------------------------
// CDMNetworkMon::ConstructL
// Second phase constructor
// ----------------------------------------------------------------------------
void CDMNetworkMon::ConstructL()
    {     
    LOGSTRING("CDMNetworkMon::ConstructL() begins");

    iSyncSessionOpen = EFalse;

    iCancelledNWEvent = EFalse;

    // Initial state
    iPrevState = ERegister;
    iState = ERegister;
    IssueRequest();

    iError = KErrNone;

    iGlobalTimer = CPeriodic::NewL (EPriorityNormal) ;
    iGlobalTimeout = ReadTimeoutFromCenrepL();
    TInt timeout = KGlobalTimeout * KTimeUnit * KMicroSecond;
    iGlobalTimer->Start ( TTimeIntervalMicroSeconds32(timeout), 
            TTimeIntervalMicroSeconds32(timeout), 
            TCallBack(Timeout, this ) );		

    LOGSTRING("CDMNetworkMon::ConstructL() ends");	
    }

// ----------------------------------------------------------------------------
// CDMNetworkMon::~CDMNetworkMon
// Destructor*
// ----------------------------------------------------------------------------
CDMNetworkMon::~CDMNetworkMon()
    {
    LOGSTRING("CDMNetworkMon::~CDMNetworkMon() begins");

    if(!iCancelledNWEvent)
    {
    // Un-register for events
    iConnectionMonitor.CancelNotifications();
    iConnectionMonitor.Close();

    }

    // Cleanup sync session
    CloseSyncSession();

    LOGSTRING("CDMNetworkMon::~CDMNetworkMon() ends");
    }

// ----------------------------------------------------------------------------
// CDMNetworkMon::RunL
// ---------------------------------------------------------------------------
void CDMNetworkMon::RunL()
    {
    switch ( iState ) 
        {
        case ERegister:
            LOGSTRING("CDMNetworkMon::RunL() State = ERegister");
            RegisterL();
            break;

            // Monitor the operation
        case EMonitor:
            LOGSTRING("CDMNetworkMon::RunL() State = EMonitor");
            StartOperationL();				 
            break;

        case ENetworkUp:
            LOGSTRING("CDMNetworkMon::RunL() State = ENetworkUp");  
            NetworkUp(this);          
            break;

        case EDMJobOk:
            LOGSTRING("CDMNetworkMon::RunL() State = EDMJobOk");
            StartDeviceManagementSessionL();
            break; 

        case EComplete:
            LOGSTRING("CDMNetworkMon::RunL() State = EComplete");
            Complete();
            break;

        default:
            break;
        }
    }

// ----------------------------------------------------------------------------
// CDMNetworkMon::RunError
// ----------------------------------------------------------------------------
TInt CDMNetworkMon::RunError ( TInt aError )
    {
    DBG_FILE_CODE(aError, _S8("CDMNetworkMon::RunError() The Error occurred is "));
    return KErrNone;
    }

// ----------------------------------------------------------------------------
// CDMNetworkMon::DoCancel
// ----------------------------------------------------------------------------
void CDMNetworkMon::DoCancel()
    {
    LOGSTRING("CDMNetworkMon::DoCancel() begins");

    iCancelledNWEvent = ETrue;

    // Un-register for events
    iConnectionMonitor.CancelNotifications();
    iConnectionMonitor.Close();

    // Cleanup sync session
    CloseSyncSession();

    LOGSTRING("CDMNetworkMon::DoCancel() ends");
    }

// ----------------------------------------------------------------------------
// CDMNetworkMon::RegisterL
// ----------------------------------------------------------------------------
void CDMNetworkMon::RegisterL()
    {
    LOGSTRING("CDMNetworkMon::RegisterL() begins");

    // Register for events
    iConnectionMonitor.ConnectL();
    
#ifndef __WINS__
    iConnectionMonitor.NotifyEventL(*this);  
#endif

    TInt err = iConnectionMonitor.SetUintAttribute( EBearerIdAll, 
            0, 
            KSignalStrengthThreshold, 
            1 );


    // Wait for events
    iState = EMonitor;
    IssueRequest();

    LOGSTRING("CDMNetworkMon::RegisterL() ends");
    }

// ----------------------------------------------------------------------------
// CDMNetworkMon::PacketDataAvailable
// ----------------------------------------------------------------------------
TBool CDMNetworkMon::PacketDataAvailable()
    {
    LOGSTRING("CDMNetworkMon::PacketDataAvailable() begins");

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
        LOGSTRING("CDMNetworkMon::PacketDataAvailable() Invalid connection ID encountered");

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



    LOGSTRING("CDMNetworkMon::PacketDataAvailable() ends");

    return val;
    }

// ----------------------------------------------------------------------------
// CDMNetworkMon::StartOperation
// ----------------------------------------------------------------------------
void CDMNetworkMon::StartOperationL()
    {
    LOGSTRING("CDMNetworkMon::StartOperation() begins");

    // check for previous state.. If previous state is NetworkDown then do not launch
    // device management session. wait for network up.

    LOGSTRING2("StartOperation() iPrevState = %i",iPrevState );

    LOGSTRING2("StartOperation() iState = %i",iState );

    if(iPrevState!=ENetworkDown)
        StartDeviceManagementSessionL();

    // if previous state is ENetworkDown then wait for Network Up event

    LOGSTRING("CDMNetworkMon::StartOperation() ends");
    }



// ----------------------------------------------------------------------------
// CDMNetworkMon::CancelGlobalTimer
// ----------------------------------------------------------------------------
void CDMNetworkMon::CancelGlobalTimer()
    {
    LOGSTRING("CDMNetworkMon::CancelGlobalTimer() begins");

    if ( iGlobalTimer )
        {
        iGlobalTimer->Cancel();
        delete iGlobalTimer;
        iGlobalTimer = NULL;
        }

    LOGSTRING("CDMNetworkMon::CancelGlobalTimer() ends");
    }

// ----------------------------------------------------------------------------
// CDMNetworkMon::NetworkStable
// ----------------------------------------------------------------------------
void CDMNetworkMon::NetworkStable()
    {
    LOGSTRING("CDMNetworkMon::NetworkStable() begins");

    // Un-register for events
    //iConnectionMonitor.CancelNotifications();

    iState = EDMJobOk;
    IssueRequest();

    LOGSTRING("CDMNetworkMon::NetworkStable() ends");
    }

// ----------------------------------------------------------------------------
// CDMNetworkMon::NetworkUp
// ----------------------------------------------------------------------------
TInt CDMNetworkMon::NetworkUp(TAny* aPtr)
    {
    LOGSTRING("CDMNetworkMon::NetworkUp2() begins");

    CDMNetworkMon* ptr = (CDMNetworkMon*) aPtr;

    if ( ptr->PacketDataAvailable() )
        {
        ptr->NetworkStable();
        }



    LOGSTRING("CDMNetworkMon::NetworkUp2() ends");

    return KErrNone;
    }

// ----------------------------------------------------------------------------
// CDMNetworkMon::OtherSyncStatus
// ----------------------------------------------------------------------------
void CDMNetworkMon::OtherSyncStatus()
    {
    LOGSTRING("CDMNetworkMon::OtherSyncStatus() begins");

    iState = EDMJobOk;
    IssueRequest();

    LOGSTRING("CDMNetworkMon::OtherSyncStatus() ends");
    }



// ----------------------------------------------------------------------------
// CDMNetworkMon::StartDeviceManagementSessionL()
// ----------------------------------------------------------------------------
void CDMNetworkMon::StartDeviceManagementSessionL()
    {

    LOGSTRING("CDMNetworkMon::StartDeviceManagementSessionL() begins");

    TBool roamingdisabled(EFalse);

    //check if roaming can be enabled from cenrep
    //if(get from cenrep is 1) check if  in roaming 
    //else roamingdisabled is EFalse
    roamingdisabled = IsRoamingDisabled();

    if(roamingdisabled)
        {
        iState = EComplete;
        IssueRequest();
        return;
        }    

    TRAPD(err, CreateDeviceManagementSessionL());

    if(err!=KErrNone)
        {
        CloseSyncSession();
        LOGSTRING2("Error = %i", err);
        iState = EComplete;
        IssueRequest();
        return;  
        }

    LOGSTRING("CDMNetworkMon::StartDeviceManagementSessionL() ends");
    }

// ----------------------------------------------------------------------------
// CDMNetworkMon::CreateSyncJobL
// ----------------------------------------------------------------------------
void CDMNetworkMon::CreateDeviceManagementSessionL()
    {

    // close some old Sync Session if existing....
    CloseSyncSession();


    // Open new Sync session
    OpenSyncSessionL();

    LOGSTRING("CDMNetworkMon::CreateDeviceManagementSessionL() begins");



    TBuf8<256> serverid;;	    
    TInt iapid = 0;

    const TUid KUidDeviceManagementKeys = 
    {
            0x101F9A0A 	
    };

    CRepository *repository= CRepository::NewLC ( KUidDeviceManagementKeys ) ;


    TInt err_get = repository->Get(KDevManServerIdKey ,
            serverid);

    LOGTEXT( serverid);

    err_get =  repository->Get(KDevManIapIdKey, iapid);

    LOGSTRING2( "IAPID = %i", iapid);


    CleanupStack::PopAndDestroy(); // CRepository

    // already operation is completed
    if(serverid == KNullDesC8())
        {
        User::Leave(KErrCompletion);
        }	



    RSyncMLDevManProfile ProfileToSearch;
    RArray<TSmlProfileId> arr;
    iSyncSession.ListProfilesL( arr, ESmlDevMan );
    TInt ret = 0;
    TInt ProfileId = KErrNotFound;
    CleanupClosePushL(arr);   	
    for ( TInt index = 0; index < arr.Count(); index++ )
        {
        TRAPD( error, ProfileToSearch.OpenL(iSyncSession, arr[index], ESmlOpenRead ) );
        LOGSTRING2( "error = %i", error);
        if ( error == KErrNone )
            {	            	            	        
            if ( serverid.Compare(ProfileToSearch.ServerId() ) == 0 )
                {
                LOGSTRING("Server profile found");
                ret = 1;                    
                ProfileId = (TInt)arr[index];
                ProfileToSearch.Close();
                break;
                }	            		    
            }
        ProfileToSearch.Close();  
        }
    CleanupStack::PopAndDestroy( &arr );

    if(ret == 1)
        {

        RSyncMLDevManJob dmJob;

        TInt IAPID = -2;
        TBuf<10> genalertap,temp;
        genalertap.Zero();
        temp.Zero();	  
        genalertap.Append(KNSmlDMJobIapPrefix);
        temp.Num(iapid);//Decimal Iap
        if( temp.Length() <= KNSmlHalfTransportIdLength && 
                iapid >= -2)
            {
            genalertap.AppendFill('0',KNSmlHalfTransportIdLength-temp.Length());
            genalertap.Append(temp);
            TLex gavalue(genalertap);
            gavalue.Val(IAPID);
            LOGSTRING("DM JOB CREATED");
            TRAPD(err, dmJob.CreateL( iSyncSession, ProfileId, IAPID));
            LOGSTRING("DM JOB CREATED END");
            if(err!=KErrNone)
                {
                LOGSTRING2( "error dmJob.CreateL = %i", err);
                User::Leave(err);
                LOGSTRING("DM JOB CREATED END");
                }	
            LOGSTRING("Session created");
            }

        dmJob.Close();
        }

    iSyncSession.RequestEventL( *this ); 

    LOGSTRING("CDMNetworkMon::CreateDeviceManagementSessionL() ends");
    }

// ----------------------------------------------------------------------------
// CDMNetworkMon::OpenSyncSessionL
// ----------------------------------------------------------------------------
void CDMNetworkMon::OpenSyncSessionL()
    {
    LOGSTRING("CDMNetworkMon: OpenSyncSessionL() Beigns");

    if (!iSyncSessionOpen)
        {
        TRAPD(err, iSyncSession.OpenL());

        if (err != KErrNone)
            {
            LOGSTRING("CDMNetworkMon: RSyncMLSession::OpenL failed ");
            User::Leave(err);
            }
        }
    iSyncSessionOpen = ETrue;

    LOGSTRING("CDMNetworkMon: OpenSyncSessionL() Ends");
    }

// ----------------------------------------------------------------------------
// CDMNetworkMon::Complete
// ----------------------------------------------------------------------------
void CDMNetworkMon::Complete()
    {
    LOGSTRING("CDMNetworkMon::Complete() begins");

    Cancel();

    // Close the server
    CActiveScheduler::Stop();

    LOGSTRING("CDMNetworkMon::Complete() ends");
    }

// ----------------------------------------------------------------------------
// CDMNetworkMon::CloseSyncSession
// ----------------------------------------------------------------------------
void CDMNetworkMon::CloseSyncSession()
    {
    LOGSTRING("CDMNetworkMon::CloseSyncSession() begins");

    if (iSyncSessionOpen)
        {
        iSyncSession.CancelEvent();
        iSyncSession.Close();
        iSyncSessionOpen = EFalse;
        }

    LOGSTRING("CDMNetworkMon::CloseSyncSession() ends");
    }


// ----------------------------------------------------------------------------
// CDMNetworkMon::EventL
// ----------------------------------------------------------------------------
void CDMNetworkMon::EventL ( const CConnMonEventBase& aEvent )
    {
    DBG_FILE_CODE(aEvent.ConnectionId(), _S8(" CDMNetworkMon::EventL() The Connection ID is:"));
    const CConnMonNetworkStatusChange * eventNetworkStatus;
    eventNetworkStatus = ( const CConnMonNetworkStatusChange* ) &aEvent;

    LOGSTRING2( "Event Network = %i",aEvent.EventType());
    LOGSTRING( "Event Visited");

    switch ( aEvent.EventType() )
        {    
        case EConnMonPacketDataAvailable:
            {                                   
            if ( aEvent.ConnectionId() == EBearerIdGPRS ||
                    aEvent.ConnectionId() == EBearerIdWCDMA )
                {

                LOGSTRING("CDMNetworkMon::EventL() Packet Data is available");
                iConMonBearerId =  TConnMonBearerId(aEvent.ConnectionId());

                // Once packetdata is available set the state to ENetworkUp 
                // check if previous state is NetworkDown then set to Network Up and 
                // again device management session can be started

                if(iPrevState == ENetworkDown)
                    {
                    iState = ENetworkUp;
                    IssueRequest();
                    }
                }

            break;
            }  

        case EConnMonNetworkStatusChange:
            {
            LOGSTRING("CDMNetworkMon::EventL() Packet Data is EConnMonNetworkStatusChange");
            LOGSTRING2( "Event Network Status= %i",eventNetworkStatus->NetworkStatus());
            switch(eventNetworkStatus->NetworkStatus())
                {
                //Network Down Scenario
                case EConnMonStatusNotAvailable:
                    //General to Offline Scenario
                case EConnMonStatusUnattached:
                    //Incoming Voice Call Scenario
                case EConnMonStatusSuspended:
                    {
                    iState = ENetworkDown;    
                    }
                    break;
                } 
            }  
            break;

        case EConnMonPacketDataUnavailable:
            {
            LOGSTRING("CDMNetworkMon::EventL() Packet Data is EConnMonPacketDataUnavailable");
            iState = ENetworkDown;
            }
            break;

        }
    }

// ----------------------------------------------------------------------------
// CDMNetworkMon::EventL
// ----------------------------------------------------------------------------
void CDMNetworkMon::IssueRequest()
    {
    TRequestStatus* status = &iStatus;
    User::RequestComplete(status, KErrNone);
    SetActive();
    }

// ----------------------------------------------------------------------------
// CDMNetworkMon::OnSyncMLSessionEvent
// ----------------------------------------------------------------------------
void CDMNetworkMon::OnSyncMLSessionEvent(TEvent aEvent, 
        TInt /*aIdentifier*/, 
        TInt /*aError*/, 
        TInt /*aAdditionalData*/)
    {    
    LOGSTRING2( "Event = %i", aEvent);
    LOGSTRING2( "iState = %i", iState);	
    switch ( aEvent )
        {
        case EJobStart:
            LOGSTRING("CDMNetworkMon::OnSyncMLSessionEvent() Sync job started");
            break;

        case EJobStartFailed:
        case EJobRejected:
        case ETransportTimeout:
        case EServerSuspended:
        case EServerTerminated:	 
        case EJobStop:
            {
            #ifdef __WINS__
            {
            //No monitoring required for emulator.
            iState = EComplete;
            IssueRequest(); 
            
            }
            #else
            {    
              if(iState != ENetworkDown && iError == KErrNone)
                {
                LOGSTRING("CDMNetworkMon::OnSyncMLSessionEvent() Sync job stopped and job stop not because of Network Down");

                iState = EComplete;
                IssueRequest();	
                }
              else
                {	  
                if(iState == ENetworkDown)
                    {
                    LOGSTRING("CDMNetworkMon::OnSyncMLSessionEvent() Sync job stopped and job stop because of Network Down");
                    iPrevState = ENetworkDown;
                    }
                iState = EMonitor;
                IssueRequest();
                }
            }
            #endif
            }
            break;
        }
    }

// ----------------------------------------------------------------------------
// CDMNetworkMon::UpdateInterruptFlagL
// ----------------------------------------------------------------------------
TBool CDMNetworkMon::IsRoamingDisabled()
    {

    TBool roamingDisabled(EFalse);
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


    if ( roamingStatus == ENetworkRegistrationRoaming )
        {
        LOGSTRING("CDMNetworkMon::IsRoamingDisabled() Device NOT in home network");           
        roamingDisabled =  ETrue;	
        }

    return roamingDisabled;
    }




// ----------------------------------------------------------------------------
// CDMNetworkMon::Timeout
// ----------------------------------------------------------------------------
TInt CDMNetworkMon::Timeout(TAny* aPtr)
    {
    LOGSTRING("CDMNetworkMon::Timeout() begins");

    CDMNetworkMon* ptr = (CDMNetworkMon*) aPtr;

    DBG_FILE_CODE(ptr->iTimerCount, _S8("CDMNetworkMon::Timeout() The time out count is"));

    ptr->iTimerCount = ptr->iTimerCount + 1;

    if(ptr->iTimerCount == (ptr->iGlobalTimeout * 2))
        {
        ptr->iTimerCount = 0;

        ptr->CancelGlobalTimer();
        ptr->Complete();    
        }

    LOGSTRING("CDMNetworkMon::Timeout() ends");

    return KErrNone;
    }

// ----------------------------------------------------------------------------
// CDMNetworkMon::ReadTimeoutFromCenrepL
// ----------------------------------------------------------------------------
TInt CDMNetworkMon::ReadTimeoutFromCenrepL()
    {
    LOGSTRING("CDMNetworkMon::ReadTimeoutFromCenrepL() begins");

    TInt timeoutInHours = 24;

    // Read the value from cenrep  
    CRepository* repository = CRepository::NewLC(KCRUidDeviceManagementInternalKeys);
    repository->Get(KDevManNetMonTimeoutKey, timeoutInHours);
    CleanupStack::PopAndDestroy(repository);

    LOGSTRING("CDMNetworkMon::ReadTimeoutFromCenrepL() ends");

    return timeoutInHours; 
    }

// -----------------------------------------------------------------------------
// CDMNetworkMon::OnSyncMLSyncError (from MSyncMLProgressObserver)
//
// -----------------------------------------------------------------------------
//
void CDMNetworkMon::OnSyncMLSyncError(TErrorLevel aErrorLevel, TInt aError, TInt /*aTaskId*/, TInt /*aInfo1*/, TInt /*aInfo2*/)
    {
    LOGSTRING2("CDMNetworkMon::OnSyncMLSyncError ERROR = %i",  aError);
    iError = aError;
    }


// -----------------------------------------------------------------------------
// CDMNetworkMon::OnSyncMLSyncProgress (from MSyncMLProgressObserver)
//
// -----------------------------------------------------------------------------
//
void CDMNetworkMon::OnSyncMLSyncProgress(TStatus aStatus, TInt aInfo1, TInt /*aInfo2*/)
    {
    DBG_FILE( _S8("CDMNetworkMon::OnSyncMLSyncProgress START") );

    
    LOGSTRING2("CDMNetworkMon::OnSyncMLSyncProgress Status = %i", aStatus);

    DBG_FILE( _S8("CDMNetworkMon::OnSyncMLSyncProgress END") );
    }


// -----------------------------------------------------------------------------
// CDMNetworkMon::OnSyncMLDataSyncModifications (from MSyncMLProgressObserver)
//
// -----------------------------------------------------------------------------
//
void CDMNetworkMon::OnSyncMLDataSyncModifications(TInt aTaskId, 
        const TSyncMLDataSyncModifications& aClientModifications,
        const TSyncMLDataSyncModifications& aServerModifications)
    {
    //Do Nothing
    }



// End of file

