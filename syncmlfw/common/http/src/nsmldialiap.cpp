/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  SyncML HTTP client
*
*/

#include <CoreApplicationUIsSDKCRKeys.h>
#include <centralrepository.h>
#include <featmgr.h>   // FeatureManager
#include <cmdefconnvalues.h>
#include <cmmanager.h>
#include <cmpluginwlandef.h>
#include <etelpckt.h>
#include "nsmldialiap.h"
#include <nsmlconstants.h>
#include "nsmlerror.h"


//------------------------------------------------------------
// CNSmlDialUpAgent::CNSmlDialUpAgent() 
// constructor
//------------------------------------------------------------
CNSmlDialUpAgent::CNSmlDialUpAgent() 
	: CActive( EPriorityStandard ){}
//------------------------------------------------------------
// CNSmlDialUpAgent::~CNSmlDialUpAgent()
// desctructor
//------------------------------------------------------------
CNSmlDialUpAgent::~CNSmlDialUpAgent()
	{
	FeatureManager::UnInitializeLib();
	Cancel();
	// close handles for sure
	if( !iCancelCalled && iSocketConnection )
		{
		iConnection.SetOpt( KCOLProvider, KConnDisableTimers, EFalse );		
		iConnection.Close();
		iSocketServer.Close();
		}
	}
// --------------------------------------------------------------------
// CNSmlDialUpAgent::ConstructL()
// 2-phase
// --------------------------------------------------------------------
void CNSmlDialUpAgent::ConstructL()
    {
	DBG_FILE( _S8("CNSmlDialUpAgent::ConstructL") );
	CActiveScheduler::Add( this );
	iCancelCalled = EFalse;    
	iSocketConnection = EFalse;    
	FeatureManager::InitializeLibL();  
	}
//------------------------------------------------------------
// CNSmlDialUpAgent::DoCancel()
// DoCancel() from CActive
//------------------------------------------------------------
void CNSmlDialUpAgent::DoCancel() 
	{
	DBG_FILE( _S8("CNSmlDialUpAgent::DoCancel...") );
	iCancelCalled = ETrue;
	if(iSocketConnection)
	{
	iConnection.SetOpt( KCOLProvider, KConnDisableTimers, EFalse );
	iConnection.Close();
	iSocketServer.Close();
	iSocketConnection = EFalse;
	}
	}
//------------------------------------------------------------
// CNSmlDialUpAgent::RunL()
// Runl() from CActive
// called when asynchronous request completed
//------------------------------------------------------------
void CNSmlDialUpAgent::RunL() 
	{
	DBG_FILE_CODE( iStatus.Int(), _S8("CNSmlDialUpAgent::RunL with status") );
	
	/**
	   In 2G Calls , data and voice are not allowed in same channel
	   If the error code is KErrGprsServicesNotAllowed and if access point is 
	   not Always ask, every minute retry is made , max for 35 times
	**/
	TInt isAccessPointDefined = EFalse;
	if (static_cast<TInt32>(iIAPid) == -2)
	{
		RCmManager cmmgr;
		cmmgr.OpenL();
		TCmDefConnValue defConnValue;
		cmmgr.ReadDefConnL(defConnValue);
		cmmgr.Close();
		if(defConnValue.iType == ECmDefConnDestination)
		{
			isAccessPointDefined = ETrue;
		}
	}
	else
	{
		if(static_cast<TInt32>(iIAPid)!=-1)
		{
		isAccessPointDefined = ETrue;	
		}
	}
	if( iRetryCounter<KNSmlConNumberOfRetries && !iTimerReturn && 
		iStatus.Int()==KErrGprsServicesNotAllowed && 
		isAccessPointDefined)
	    {
	      DBG_FILE( _S8("CNSmlDialUpAgent::RunL() ,Timer is started"));
		    DBG_FILE_CODE(iRetryCounter, _S8("CNSmlDialUpAgent::RunL() for Iteration "));
		    
		    iTimerReturn=ETrue;
	    	iTimer.CreateLocal();
	      iTimer.After( iStatus, TTimeIntervalMicroSeconds32(KNSmlConRetryInterval));
	      iConnection.Close();
		  iSocketServer.Close();
		  iSocketConnection = EFalse;
	      SetActive();
	      return;
	    }
	 else if(iRetryCounter<KNSmlConNumberOfRetries && iTimerReturn)
	    {
	        DBG_FILE_CODE(iRetryCounter, _S8("CNSmlDialUpAgent::RunL() Retrying connection"));
	        Cancel();
	        iTimerReturn=EFalse;
	        iRetryCounter++;
	       	TRAPD( ret, StartDatacallL() );
	        if ( ret != KErrNone )
	            {
		            TRequestStatus* status = &iStatus;
				    		User::RequestComplete( status, ret );		
	            }
	        return;    
        }
    if ( iStatus.Int() != KErrNone && iIAPCnt < iIAPArray->Count() )
        {
        this->Cancel();        
        iIAPid = iIAPArray->At( iIAPCnt );
        DoSettingsL();

        iIAPCnt++;

        TRAPD( ret, StartDatacallL() );
        if ( ret != KErrNone )
            {
            TRequestStatus* status = &iStatus;
		    User::RequestComplete( status, ret );		
            }
        }
    else
        {
	    if ( iStatus.Int() == KErrCancel )
	    	{
	    	this->Cancel();
            iStatus = KErrCancel;
	    	}
	    else if ( iStatus.Int() != KErrNone )
	    	{
	    	this->Cancel();
            iStatus = TNSmlError::ESmlCommunicationError;
	    	}
	    if(iSocketConnection)
	    {
	    iConnection.SetOpt( KCOLProvider, KConnDisableTimers, ETrue );
	    }
	    TRequestStatus* status = iEngineStatus;
	    User::RequestComplete( status, iStatus.Int() );			
        }
	}
// ----------------------------------------------------------------------------
// CNSmlDialUpAgent::RunError
// ----------------------------------------------------------------------------
TInt CNSmlDialUpAgent::RunError ( TInt aError )
    {
    DBG_FILE_CODE(aError, _S8("CNSmlDialUpAgent::RunError() The Error occurred is "));
    return KErrNone;
    }	
    
// ---------------------------------------------------------
// CNSmlDialUpAgent::ConnectL( TUint aIAPid, TRequestStatus &aStatus )
// Opens data call handle 
// This is asynchronous request 
// ---------------------------------------------------------
void CNSmlDialUpAgent::ConnectL( CArrayFixFlat<TUint32>* aIAPIdArray, TRequestStatus &aStatus )
	{
	//DBG_FILE_CODE( aIAPid, _S8("CNSmlDialUpAgent::ConnectL") );
	// aStatus set pending from HTTP
	iCancelCalled = EFalse;
	iEngineStatus = &aStatus;

    iIAPCnt = 0;
    iIAPArray = aIAPIdArray;


	TBool connected( EFalse );
	TRAPD( err, ( connected = IsConnectedL() ) );
	if( err != KErrNone )
		{
		TRequestStatus* status = &iStatus;
		User::RequestComplete( status, err );		
		}
	if( !connected )
		{
        iIAPid = iIAPArray->At( iIAPCnt );
        DoSettingsL();

        iIAPCnt++;

        TRAPD( ret, StartDatacallL() );
        if ( ret != KErrNone )
            {
            TRequestStatus* status = &iStatus;
		    User::RequestComplete( status, ret );		
            }
/*
			#ifdef __WINS__
	            if ( !IsConnectedL() )
		            {						
					if ( !IsConnectedL() )	// Double check
						{
						this->Cancel();
						iIAPCnt++;
						}
					else
						{
						callStarted = ETrue;
						}
	                }
		        else
			        {
				    callStarted = ETrue;
					}
			#else
	            if ( !IsConnectedL() )
	                {
		            this->Cancel();
			        iIAPCnt++;
				    }
	            else
		            {
			        callStarted = ETrue;
				    }
			#endif
*/

		}
	else
		{
		TRAPD( error, AttachToConnectionL() );
		TRequestStatus* status = &iStatus;
		User::RequestComplete( status, error );		
		}  
	}
// ---------------------------------------------------------
// CNSmlDialUpAgent::StartDatacallL()
// Opens data call handle 
// This is asynchronous request 
// ---------------------------------------------------------
void CNSmlDialUpAgent::StartDatacallL()
    {
    DBG_FILE( _S8("CNSmlDialUpAgent::StartDatacallL") );
    SetActive();
    User::LeaveIfError( iSocketServer.Connect() );		
    User::LeaveIfError( iConnection.Open( iSocketServer ) );
    iSocketConnection = ETrue;
    if ( static_cast<TInt32>(iIAPid) == -1 )
        {
        iConnection.Start( iPref, iStatus );
        }
    else if( static_cast<TInt32>(iIAPid) == -2 ) // default connection 
        {

        iConnection.Start( iStatus );	

        }
    else
        {
        iConnection.Start( iPref, iStatus );
        }
    }
	
// ---------------------------------------------------------
// CNSmlDialUpAgent::AttachToConnectionL()
// Attach to connection allready active 
// 
// ---------------------------------------------------------
void CNSmlDialUpAgent::AttachToConnectionL()
	{
	DBG_FILE( _S8("CNSmlDialUpAgent::AttachToConnectionL") );
	iStatus = KRequestPending;
	SetActive();
	TConnectionInfo connectionInfo;
	TConnectionInfoBuf connInfo( connectionInfo );
	
	User::LeaveIfError( iSocketServer.Connect() );
	User::LeaveIfError( iConnection.Open( iSocketServer ) );
	
	TUint count( 0 );
	User::LeaveIfError( iConnection.EnumerateConnections ( count ) );
	DBG_FILE_CODE( count, _S8("Number of connections ") );
	
	if( count == 0 )
		{
		iConnection.Close();
		iSocketServer.Close();
		User::Leave( KErrNotFound );
		}
	else{
		for( TUint i=1; i<=count; ++i )
			{
			User::LeaveIfError( iConnection.GetConnectionInfo( i, connInfo ) );
			DBG_FILE_CODE( connInfo().iIapId, _S8("Comparing IDs ") );			
			if( connInfo().iIapId == iIAPid )
				{
				DBG_FILE_CODE( iIAPid, _S8("Attaching to existing connection, IDs match") );
				User::LeaveIfError( iConnection.Attach( connInfo, RConnection::EAttachTypeNormal ) );				
				break;
				}
			}
		}
	}
// ---------------------------------------------------------
// CNSmlDialUpAgent::IsConnectedL()
// Checks if data call is up or connecting
// ---------------------------------------------------------
TBool CNSmlDialUpAgent::IsConnectedL() const
	{
	DBG_FILE( _S8("CNSmlDialUpAgent::IsConnected") );
	TInt err( KErrNone );
	TBool isConnected( EFalse );
	TConnectionInfo connectionInfo;
	TConnectionInfoBuf connInfo( connectionInfo );

	RSocketServ serv;
	CleanupClosePushL( serv );
	User::LeaveIfError( serv.Connect() );

	RConnection conn;
	CleanupClosePushL( conn );
	User::LeaveIfError( conn.Open( serv ) );

	TUint count( 0 );
	User::LeaveIfError( conn.EnumerateConnections ( count ) );
	DBG_FILE_CODE( count, _S8("Number of connections ") );
	
	if( count == 0 )
		{
		CleanupStack::PopAndDestroy( 2 ); // conn, serv
		return isConnected;
		}
	else{
		for( TUint i=1; i<=count; ++i )
			{
			err = conn.GetConnectionInfo( i, connInfo );
			if( err != KErrNone )
				{
				CleanupStack::PopAndDestroy( 2 ); // conn, serv
				User::Leave( err );				
				}	
			DBG_FILE_CODE( connInfo().iIapId, _S8("Comparing IDs ") );
			if( connInfo().iIapId == iIAPid )
				{
				DBG_FILE( _S8("Connection allready active") );
				isConnected = ETrue;
				break;
				}
		    }
		}
	CleanupStack::PopAndDestroy( 2 ); // conn, serv
	return isConnected;
	}
// ---------------------------------------------------------
// CNSmlDialUpAgent::DoSettingsL()
//  
//  
// ---------------------------------------------------------
void CNSmlDialUpAgent::DoSettingsL()
    {
    DBG_FILE( _S8("CNSmlDialUpAgent::DoSettingsL") );
    iOffline=IsInOfflineModeL();
    if( static_cast<TInt32>(iIAPid) == -1 )
        {		
        if(iOffline)
            {		
            iPref.SetDialogPreference( ECommDbDialogPrefPrompt );
            iPref.SetBearerSet(KUidWlanBearerType);	
            }
        else
            {		
            iPref.SetDialogPreference( ECommDbDialogPrefPrompt );		
            //iPref.SetBearerSet(EApBearerTypeAllBearers);
            }				
        }
    else if( iOffline &&  ( static_cast<TInt32>(iIAPid) == -2 ) ) // default connection
        {
        iPref.SetDialogPreference( ECommDbDialogPrefPrompt );
        iPref.SetBearerSet(KUidWlanBearerType);	
        }
    else
        {
        iPref.SetIapId( iIAPid );
        iPref.SetDialogPreference( ECommDbDialogPrefDoNotPrompt );
        }
    }
//----------------------------------------------------------
//CNsmlDialUpAgent::IsOfflineModeL()
TBool CNSmlDialUpAgent::IsInOfflineModeL()
    {
    TInt operationsAllowed( ECoreAppUIsNetworkConnectionAllowed );
	CRepository* rep = CRepository::NewLC( KCRUidCoreApplicationUIs );
	rep->Get(KCoreAppUIsNetworkConnectionAllowed, operationsAllowed );
	CleanupStack::PopAndDestroy(); //rep
	
    return ( operationsAllowed == ECoreAppUIsNetworkConnectionNotAllowed ) ? ETrue : EFalse;
    }
