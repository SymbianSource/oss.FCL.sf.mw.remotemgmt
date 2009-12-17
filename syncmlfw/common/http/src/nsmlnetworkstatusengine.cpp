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
* Description:  SyncML Network status observer 
*				 This is used in http to listen to gprs events 
*
*/


#include "nsmlhttp.h"
#include <DevManInternalCRKeys.h>
#include <centralrepository.h>
#include "nsmlconstants.h"
#ifndef __WINS__
// This lowers the unnecessary compiler warning (armv5) to remark.
// "Warning:  #174-D: expression has no effect..." is caused by 
// DBG_ARGS8 macro in no-debug builds.
#pragma diag_remark 174
#endif
// -----------------------------------------------------------------------------
// StaticDoStopDMSession()
// Global function to handle the timeout 
// -----------------------------------------------------------------------------
//	

static TInt StaticDoStopDMSession(TAny *aPtr)
    {

    DBG_FILE(_S8("StaticDoStopDMSession start "));
               
    CNsmlNetworkStatusEngine* srv = (CNsmlNetworkStatusEngine*) aPtr;
    TRAPD( err,  srv->StopDMSession() );
    if(err) 
        {
       	DBG_FILE_CODE(err,_S8("StaticDoStopDMSession err after calling StopDMSession"));
        }

    DBG_FILE(_S8(" StaticDoStopDMSession ENDS "));
    return err;
    }
    
// -----------------------------------------------------------------------------
// CNsmlNetworkStatusEngine::CNsmlNetworkStatusEngine()
// -----------------------------------------------------------------------------
//

CNsmlNetworkStatusEngine::CNsmlNetworkStatusEngine( CNSmlHTTP* aAgent ) 
	: iAgent( aAgent ) {}
	
// -----------------------------------------------------------------------------
// CNsmlNetworkStatusEngine::ConstructL()
// -----------------------------------------------------------------------------
//
	
void CNsmlNetworkStatusEngine::ConstructL( )
{
	DBG_FILE(_S8("CNsmlNetworkStatusEngine::ConstructL, BEGIN "));
	iTimerOn = EFalse;
	iConnectionMonitor.ConnectL();
	iTimedExecuteStopSession = NULL;
	DBG_FILE(_S8("CNsmlNetworkStatusEngine::ConstructL, END "));	 
}

// -----------------------------------------------------------------------------
// CNsmlNetworkStatusEngine::~CNsmlNetworkStatusEngine()
// -----------------------------------------------------------------------------
//
	
CNsmlNetworkStatusEngine::~CNsmlNetworkStatusEngine( )
{
	DBG_FILE(_S8("~CNsmlNetworkStatusEngine Destructor starts  "));
	StopNotify();
	iConnectionMonitor.Close();
	if ( iTimedExecuteStopSession ) 
    {
     iTimedExecuteStopSession->Cancel();
     delete iTimedExecuteStopSession;
     iTimedExecuteStopSession = NULL;
     }
    DBG_FILE(_S8("~CNsmlNetworkStatusEngine Destructor ENDs  "));     
            
}

// -----------------------------------------------------------------------------
// CNsmlNetworkStatusEngine::NotifyL()
// Registers with connection monitor for notification
// -----------------------------------------------------------------------------
//

void CNsmlNetworkStatusEngine::NotifyL()
{
	DBG_FILE(_S8("CNsmlNetworkStatusEngine::NotifyL Begin  "));
	iConnectionMonitor.NotifyEventL( *this );
	DBG_FILE(_S8("CNsmlNetworkStatusEngine::NotifyL, after registering  "));
   // Threholds
	TInt    err = iConnectionMonitor.SetUintAttribute( EBearerIdGPRS, 
                                               0, 
                                               KSignalStrengthThreshold, 
                                               1 );    
	DBG_FILE(_S8("CNsmlNetworkStatusEngine::NotifyL : END "));
                                                                                          
}

// -----------------------------------------------------------------------------
// CNsmlNetworkStatusEngine::StopNotify()
// Cancels notification with connection monitor
// -----------------------------------------------------------------------------
//

void CNsmlNetworkStatusEngine::StopNotify()
{
	iConnectionMonitor.CancelNotifications();
}	

// -----------------------------------------------------------------------------
// CNsmlNetworkStatusEngine::StopDMSession()
// Stops DM session by calling http cancel 
// -----------------------------------------------------------------------------
//	
void CNsmlNetworkStatusEngine::StopDMSession()
	{
	DBG_FILE(_S8("CNsmlNetworkStatusEngine::StopDMSession STARTS   "));
	iTimerOn = EFalse;
	DBG_FILE(_S8("CNsmlNetworkStatusEngine::StopDMSession calling http->Cancel"));
	iAgent->iTimeOut =ETrue; //added for showing timeout note
	iAgent->Cancel();
	DBG_FILE(_S8("CNsmlNetworkStatusEngine::StopDMSession ENDS    "));
	}
	

// -----------------------------------------------------------------------------
// CNsmlNetworkStatusEngine::EventL()
// Method involed by connection monitor with current event. 
// This method handles the events
// -----------------------------------------------------------------------------
//

void CNsmlNetworkStatusEngine::EventL( const CConnMonEventBase& aConnMonEvent )
{

	DBG_FILE(_S8("CNsmlNetworkStatusEngine::EventL, BEGIN"));
	DBG_ARGS8(_S8("CNsmlNetworkStatusEngine::EventL event is ==  %d"), aConnMonEvent.EventType()  );
	switch ( aConnMonEvent.EventType() )
	{
	case EConnMonNetworkStatusChange:
	{
	const CConnMonNetworkStatusChange* eventNetworkStatus;
    eventNetworkStatus = ( const CConnMonNetworkStatusChange* ) &aConnMonEvent;
    iNetwStatus   = eventNetworkStatus->NetworkStatus();
    DBG_ARGS8(_S8("CNsmlNetworkStatusEngine::EventL EConnMonNetworkStatusChange: Network status event is ==  %d"), iNetwStatus  );   	
    switch( eventNetworkStatus->NetworkStatus() )
	{
		case EConnMonStatusActive:
		DBG_FILE(_S8("Network status ACTIVE"));
	    if(iTimerOn)
	    {
			iTimerOn = EFalse;
		    iSuspendDuration.HomeTime();
		    TTimeIntervalSeconds duration;
		    iSuspendDuration.SecondsFrom(iSuspendedTime,duration);
		    DBG_FILE_CODE(duration.Int(), _S8("CNsmlNetworkStatusEngine::EventL suspendDuration is "));        	 
			if ( iTimedExecuteStopSession ) 
			{
				iTimedExecuteStopSession->Cancel();
				delete iTimedExecuteStopSession;
				iTimedExecuteStopSession = NULL;
			}
		    if(duration.Int() >= KDMMaxHttpAutoResumeDurationSec)
			{
				iAgent->CompleteRequest();
			}
	    } // if itimerOn
	           
        break;
        case EConnMonStatusSuspended:
        DBG_FILE(_S8("HTTP  Network status SUSPEND and Timer will be set ON "));
	   	if(!iTimerOn)
		{
	    	CRepository *rep = NULL;
			TRAPD( err1, rep = CRepository::NewL( KCRUidDeviceManagementInternalKeys ))
			iDMSmlSessionTimeout = -1;
			if(err1 == KErrNone)
			{
				TInt dmsessionTimeout = -1;
				rep->Get( KDevManDMSessionTimeout, dmsessionTimeout );
				delete rep;
				DBG_FILE_CODE(dmsessionTimeout, _S8("CNsmlNetworkStatusEngine::EventL session timeout duration from cenrep is "));        	 
				if( dmsessionTimeout < KNSmlDMMinSessionTimeout  || dmsessionTimeout > KNSmlDMMaxSessionTimeout )
				{
					dmsessionTimeout = -1;
					iDMSmlSessionTimeout = -1;
				}
				else
				{
					iDMSmlSessionTimeout =  dmsessionTimeout * 60* 1000000;
					iTimerOn = ETrue;
	    			iSuspendedTime.HomeTime();
	 		    	iTimedExecuteStopSession = CPeriodic::NewL (EPriorityNormal) ;
					iTimedExecuteStopSession->Start (
		    		TTimeIntervalMicroSeconds32(iDMSmlSessionTimeout)
		    		, TTimeIntervalMicroSeconds32(iDMSmlSessionTimeout)
		    		, TCallBack(StaticDoStopDMSession,this) ) ;
				}
			
			} // if KErrnone
			
	    	
	    } //if !iTimerON
		break;   
	    default: //for eventNetworkStatus->NetworkStatus()
		break;
     }
    break;
    }

	default: // for EConnMonNetworkStatusChange
	    break;
	}
}

//End of File
