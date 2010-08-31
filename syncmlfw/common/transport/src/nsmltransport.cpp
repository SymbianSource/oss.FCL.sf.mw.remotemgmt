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
* Description:  SyncML transport interface
*
*/




#include <CoreApplicationUIsSDKCRKeys.h>
#include <btengdomaincrkeys.h>
#include <centralrepository.h>
#include "nsmltransport.h"
#include "nsmlhttp.h"
#include "NSmlObexClient.h"
#include "nsmlobexserverbinding.h"
#include "nsmlerror.h"  
#include <featmgr.h>
#include <cmconnectionmethoddef.h>
#include <cmmanagerext.h>
#include <cmpluginwlandef.h>


//============================================================
// CNSmlTransport definition
//============================================================
EXPORT_C CNSmlTransport::CNSmlTransport()
	{
	}
//------------------------------------------------------------
// desctructor
//------------------------------------------------------------
EXPORT_C CNSmlTransport::~CNSmlTransport() 
	{
	DBG_FILE(_S8("CNSmlTransport::~CNSmlTransport() begin"));
	if( iObexBinding )
		{
		DBG_FILE(_S8("delete iObexBinding"));
		delete iObexBinding;
		iObexBinding = NULL;
		iObexLibrary.Close();
		}
	if( iHTTPBinding )
		{
		DBG_FILE(_S8("delete iHTTPBinding"));
		delete iHTTPBinding;
		iHTTPBinding = NULL;
		iHTTPLibrary.Close();
		}
	if( iObexServerBinding )
		{
		DBG_FILE(_S8("delete iObexServerBinding"));
		delete iObexServerBinding;
		iObexServerBinding = NULL;
		iObexServerBindingLib.Close();
		}
	DBG_FILE(_S8("CNSmlTransport::~CNSmlTransport() end"));
	}
// ---------------------------------------------------------
// CNSmlTransport::NewL()
// Creates new instance of CNSmlTransport. 
// Does not leave instance pointer to CleanupStack.
// ---------------------------------------------------------
EXPORT_C CNSmlTransport* CNSmlTransport::NewL()
	{
	CNSmlTransport* self = CNSmlTransport::NewLC();
	CleanupStack::Pop();
	return( self );
	}
// ---------------------------------------------------------
// CNSmlTransport::NewLC()
// Creates new instance of CNSmlTransport 
// Leaves instance pointer to CleanupStack.
// ---------------------------------------------------------
EXPORT_C CNSmlTransport* CNSmlTransport::NewLC()
	{
	CNSmlTransport* self = new (ELeave) CNSmlTransport();
	CleanupStack::PushL( self );
	self->ConstructL();
	return( self );
	}
//------------------------------------------------------------
// 2-phase construct
//------------------------------------------------------------
EXPORT_C void CNSmlTransport::ConstructL() 
	{
	iObex = EFalse;
	iObexServer = EFalse;

	iBTObserver = 0;
	iBTDevAddr = TBTDevAddr( 0 );
	iServiceUid = TUUID( 0 );
		FeatureManager::InitializeLibL();  
		iWLANBearer = FeatureManager::FeatureSupported( KFeatureIdProtocolWlan );
		FeatureManager::UnInitializeLib();
	}

//------------------------------------------------------------
// CNSmlTransport::LoadObexServerBindingDllL()
// Loads OBEX client dll.
//------------------------------------------------------------
void CNSmlTransport::LoadObexServerBindingDllL()
	{
	DBG_FILE(_S8("CNSmlTransport::LoadObexServerBindingDllL()"));
	// Dynamically load DLL
	User::LeaveIfError( iObexServerBindingLib.Load( KSmlObexServerBindingLibName ) );
	if( iObexServerBindingLib.Type()[1] != KSmlTransBindingUid )
		{
		DBG_FILE(_S8("KSmlTransBindingUid incorrect..."));
		iObexServerBindingLib.Close();
		User::Leave( KErrNotFound );
		}	
	TNSmlCreateObexServerBindingFunc CreateCNsmlObexServerBindingL = (TNSmlCreateObexServerBindingFunc)iObexServerBindingLib.Lookup(1);
	iObexServerBinding =  (CNSmlObexServerBinding*)CreateCNsmlObexServerBindingL();
	}
//------------------------------------------------------------
// CNSmlTransport::LoadObexDllL()
// Loads OBEX client dll.
//------------------------------------------------------------
void CNSmlTransport::LoadObexDllL()
	{
	DBG_FILE(_S8("CNSmlTransport::LoadObexDllL()"));

	// Dynamically load DLL
	// Check what is the used medium type and load the correct DLL
	//  BT, IR, USB
	if ( iCurrMedium == KUidNSmlMediumTypeUSB )
		{
		User::LeaveIfError( iObexLibrary.Load( KSmlObexUsbClientLibName ) );
		}
	else
		{
		User::LeaveIfError( iObexLibrary.Load( KSmlObexClientLibName ) );
		}

	if( iObexLibrary.Type()[1] != KSmlTransBindingUid )
		{
		iObexLibrary.Close();
		User::Leave( KErrNotFound );
		}	
	TNSmlCreateObexClientFunc createObexClientL = (TNSmlCreateObexClientFunc)iObexLibrary.Lookup(1);
	iObexBinding =  (CNsmlObexClient*)createObexClientL();
	}

//------------------------------------------------------------
// CNSmlTransport::LoadHTTPDllL()
// Loads HTTP client dll.
//------------------------------------------------------------
void CNSmlTransport::LoadHTTPDllL()
	{
	DBG_FILE(_S8("CNSmlTransport::LoadHTTPDllL()"));
	// Dynamically load DLL
	User::LeaveIfError( iHTTPLibrary.Load( KSmlHTTPClientLibName ) );
	if( iHTTPLibrary.Type()[1] != KSmlTransBindingUid )
		{
		iHTTPLibrary.Close();
		User::Leave( KErrNotFound );
		}
	TNSmlCreateHTTPFunc createHTTPClientL = (TNSmlCreateHTTPFunc)iHTTPLibrary.Lookup(1);
	iHTTPBinding =  (CNSmlHTTP*)createHTTPClientL();
	}

// 1.2 CHANGES: Offline mode
//------------------------------------------------------------
// CNSmlTransport::IsInOfflineMode()
// Checks if device is at offline mode
//------------------------------------------------------------
TBool CNSmlTransport::IsInOfflineModeL()
    {
    TInt operationsAllowed( ECoreAppUIsNetworkConnectionAllowed );
	CRepository* rep = CRepository::NewLC( KCRUidCoreApplicationUIs );
	rep->Get(KCoreAppUIsNetworkConnectionAllowed, operationsAllowed );
	CleanupStack::PopAndDestroy(); //rep
	
    return ( operationsAllowed == ECoreAppUIsNetworkConnectionNotAllowed ) ? ETrue : EFalse;
    }
// Changes end

//------------------------------------------------------------
// CNSmlTransport::Connect
// Establishes a communication using the protocol service with the given ID.
//------------------------------------------------------------
EXPORT_C void CNSmlTransport::ConnectL( TUid aMediumType, TBool aServerAlerted, CArrayFixFlat<TUint32>* aIAPIdArray, TDesC8& aURI, TDesC8& aMimeType, TRequestStatus &aStatus, TDesC8& aHTTPusername, TDesC8& aHTTPpassword, TInt aHTTPauthused ) 
	{
	DBG_FILE(_S8("Connect starts..."));
	iAgentStatus = &aStatus;

	iCurrMedium = aMediumType;

	if( aMediumType != KUidNSmlMediumTypeInternet )
		{
		TNSmlObexTransport obexTransport;
		
		if ( aMediumType == KUidNSmlMediumTypeUSB )
			{
			obexTransport = EObexUsb;
			}
		else if ( aMediumType == KUidNSmlMediumTypeIrDA )
			{
			obexTransport = EObexIr;
			}
		else
			{
			obexTransport = EObexBt;

            if ( IsInOfflineModeL() )
            	{
			    TInt btActivationEnabled( EBTEnabledInOfflineMode );
		        CRepository* rep = CRepository::NewLC( KCRUidBluetoothEngine );
                TInt properr = rep->Get( KBTEnabledInOffline, btActivationEnabled );
				CleanupStack::PopAndDestroy();
                
                if ( btActivationEnabled == EBTDisabledInOfflineMode )
					{
					DBG_FILE(_S8("Offline mode set!"));
					*iAgentStatus = KRequestPending;
					TRequestStatus* status = iAgentStatus;
					User::RequestComplete( status, TNSmlError::ESmlStatusNotPossibleInOfflineMode );
					return;
					}			        
				}
			}

		Connect( obexTransport, aServerAlerted, aMimeType, aStatus );
		}
	else
		{        
			TBool errormsg = EFalse;
			if(IsInOfflineModeL()) 
			{
        // 1.2 CHANGES: Offline mode
			if( !iWLANBearer ) // offline mode and no WLAN support
            {
				errormsg = ETrue;
			}
			else
			{
            // used, so it is ok to use first one in the list.
            if ( aIAPIdArray->At(0) != 0xffffffff && aIAPIdArray->At(0) != 0xfffffffe ) 
			// 0xffffffff is same as -1 (KErrNotFound) and -2 for Default connection
                {
                TUint32 accesspointId = aIAPIdArray->At(0);
                RCmManagerExt  cmmanagerExt;
                cmmanagerExt.OpenL();
                CleanupClosePushL(cmmanagerExt);
                RCmConnectionMethodExt cm;
                cm = cmmanagerExt.ConnectionMethodL( accesspointId );
                CleanupClosePushL( cm );
                TUint32 bearer = 0;
                                
                TRAP_IGNORE( accesspointId = cm.GetIntAttributeL(CMManager::ECmIapId) );
                bearer = cm.GetIntAttributeL( CMManager::ECmBearerType );
                CleanupStack::PopAndDestroy( 2 ); //cmmanagerext,cm
                            
                if ( bearer != KUidWlanBearerType )
                    {
                    	errormsg = ETrue;
                    }
                }
			}
			}
			if(errormsg)
                {
                DBG_FILE(_S8("Offline mode set!"));
	    	    *iAgentStatus = KRequestPending;
		        TRequestStatus* status = iAgentStatus;
                User::RequestComplete( status, TNSmlError::ESmlStatusNotPossibleInOfflineMode );
                return;
                }
        // Changes end

		// New attributes username and password to support HTTP authentication
		Connect( aIAPIdArray, aURI, aMimeType, aStatus, aHTTPusername, aHTTPpassword, aHTTPauthused );
		}
	}


//------------------------------------------------------------
// CNSmlTransport::Connect
// Establishes a communication for OBEX.
//------------------------------------------------------------
void CNSmlTransport::Connect( TNSmlObexTransport aTransport,TBool aServerAlerted, TDesC8& aMimeType, TRequestStatus &aStatus )
	{
	DBG_FILE(_S8("OBEX Connect starts..."));
	iObex = ETrue;
	TInt err( KErrNone );
	iAgentStatus = &aStatus;
	if( !aServerAlerted ) // starting OBEX client
		{
		DBG_FILE(_S8("starting OBEX client..."));
		if( iObexBinding == NULL )
			{
			TRAP( err, LoadObexDllL() );
			if( err != KErrNone )
				{
				DBG_FILE(_S8("LoadObexDllL() failed!"));
				*iAgentStatus = KRequestPending;
				TRequestStatus* status = iAgentStatus;
				User::RequestComplete( status, err );
				}
			}

		if( iObexBinding && aTransport == EObexBt && !err )
			{
			// Set observer to get partners bluetooth address/name
			if( iBTObserver )
				{
				iObexBinding->SetExtObserver( iBTObserver );
				}
			// Set bluetooth connection info
			if( iBTDevAddr != TBTDevAddr( 0 ) || iServiceUid != TUUID( 0 ) )
				{
				iObexBinding->SetBTConnInfo( iBTDevAddr, iServiceUid );	
				}
			}

		if( iObexBinding && !err )
			{
			TRAP( err, iObexBinding->ConnectL( aTransport, aServerAlerted, aMimeType, aStatus ) );
			if( err != KErrNone )
				{
				DBG_FILE(_S8("iObexBinding->ConnectL() failed"));
				*iAgentStatus = KRequestPending;
				TRequestStatus* status = iAgentStatus;
				User::RequestComplete( status, err );
				}
			}
		}
	else
		{
		DBG_FILE(_S8("starting OBEX server binding..."));
		iObexServer = ETrue;
		if( iObexServerBinding == NULL )
			{
			TRAP( err, LoadObexServerBindingDllL() );
			if( err != KErrNone )
				{
				DBG_FILE(_S8("LoadObexServerBindingDllL() failed!"));
				*iAgentStatus = KRequestPending;
				TRequestStatus* status = iAgentStatus;
				User::RequestComplete( status, err );
				}
			}
		if( iObexServerBinding && !err )
			{
			iObexServerBinding->Connect( aTransport, aServerAlerted, aMimeType, aStatus );
			}
		}
	}

//------------------------------------------------------------
// CNSmlTransport::Connect
// Establishes a communication using the protocol service with the given ID.
//------------------------------------------------------------
void CNSmlTransport::Connect( CArrayFixFlat<TUint32>* aIAPIdArray, TDesC8& aURI, TDesC8& aMimeType, TRequestStatus &aStatus, TDesC8& aHTTPusername, TDesC8& aHTTPpassword, TInt aHTTPauthused )
	{
	DBG_FILE(_S8("HTTP Connect starts..."));
	TInt err( KErrNone );
	iAgentStatus = &aStatus;
	iObex = EFalse;
	if( iHTTPBinding == NULL )
		{
		TRAP( err, LoadHTTPDllL() );
		if( err != KErrNone )
			{
			DBG_FILE(_S8("LoadHTTPDllL() failed!"));
			*iAgentStatus = KRequestPending;
			TRequestStatus* status = iAgentStatus;
			User::RequestComplete( status, err );
			}
		}
	if( iHTTPBinding && !err )
		{
		TRAP( err, iHTTPBinding->OpenCommunicationL( aIAPIdArray, aURI, aMimeType, aStatus, aHTTPusername, aHTTPpassword, aHTTPauthused ) );
		if( err != KErrNone )
			{
			DBG_FILE(_S8("iHTTPBinding->OpenCommunicationL() failed!"));
			*iAgentStatus = KRequestPending;
			TRequestStatus* status = iAgentStatus;
			User::RequestComplete( status, err );
			}
		}
	}
//------------------------------------------------------------
// CNSmlTransport::Disconnect
// Closes a previously opened communication.
//------------------------------------------------------------
EXPORT_C void CNSmlTransport::Disconnect( )
	{
	DBG_FILE(_S8("CNSmlTransport::Disconnect( )"));
	if( iObex )
		{
		if( iObexBinding != NULL )
			{
			DBG_FILE(_S8("iObexBinding->Cancel() starts.."));	
			iObexBinding->Cancel();
			
			TRequestStatus* status = iAgentStatus;
			TInt err( KErrNone );
			TRAP(err, iObexBinding->CloseCommunicationL( *status ) );
			}
		if( iObexServerBinding != NULL )
			{
			DBG_FILE(_S8("iObexServerBinding->Cancel() starts.."));			
			iObexServerBinding->Cancel();
			iObexServerBinding->Disconnect();
			}
		}
	else
		{
		if( iHTTPBinding != NULL )
			{
			DBG_FILE(_S8("iHTTPBinding->Cancel() starts.."));			
			iHTTPBinding->Cancel();
			}
		}
	}
//------------------------------------------------------------
// CNSmlTransport::Receive
// Read data across a connection.
//------------------------------------------------------------
EXPORT_C void CNSmlTransport::Receive( TPtr8& aStartPtr, TRequestStatus &aStatus )
	{
	DBG_FILE(_S8("CNSmlTransport::Receive starts..."));
	TInt err( KErrNone );
	if( iObex )
		{
		if( iObexServer )
			{
			DBG_FILE(_S8("OBEX server binding Receive starts..."));
			this->iObexServerBinding->Receive( aStartPtr, aStatus );
			}
		else
			{
			DBG_FILE(_S8("OBEX ReceiveDataL starts..."));
			TRAP( err, this->iObexBinding->ReceiveDataL( aStartPtr, aStatus ) );
			if( err != KErrNone )
				{
				DBG_FILE(_S8("iObexBinding->ReceiveDataL failed!"));
				*iAgentStatus = KRequestPending;
				TRequestStatus* status = iAgentStatus;
				User::RequestComplete( status, err );
				}			
			}
		}
	else
		{
		DBG_FILE(_S8("HTTP ReceiveDataL starts..."));
		TRAP( err, this->iHTTPBinding->ReceiveDataL( aStartPtr, aStatus ) );
		if( err != KErrNone )
			{
			DBG_FILE(_S8("iHTTPBinding->ReceiveDataL failed!"));
			*iAgentStatus = KRequestPending;
			TRequestStatus* status = iAgentStatus;
			User::RequestComplete( status, err );
			}
		}
	}
//------------------------------------------------------------
// CNSmlTransport::Send
// Send data across a connection.
//------------------------------------------------------------
EXPORT_C void CNSmlTransport::Send( TDesC8& aStartPtr, TBool /*aFinalPacket*/, TRequestStatus &aStatus )
	{
	DBG_FILE(_S8("CNSmlTransport::Send starts..."));
	TInt err( KErrNone );

	if( iObex )
		{
		if ( iObexServer )
			{
			DBG_FILE(_S8("OBEX server binding SendDataL starts..."));
			this->iObexServerBinding->Send( aStartPtr, ETrue, aStatus );
			}
		else
			{
			DBG_FILE(_S8("OBEX client SendDataL starts..."));
			TRAP( err, this->iObexBinding->SendDataL( aStartPtr, ETrue, aStatus ) );
			if( err != KErrNone )
				{
				DBG_FILE(_S8("iObexBinding->SendDataL() failed!"));
				*iAgentStatus = KRequestPending;
				TRequestStatus* status = iAgentStatus;
				User::RequestComplete( status, err );
				}						
			}
		}
	else
		{
        // 1.2 CHANGES: Offline mode
        TBool offline = EFalse;
        TInt offlineError( KErrNone );
        TRAP( offlineError, offline = IsInOfflineModeL() );
        if ( offline && !iWLANBearer )
            {
            DBG_FILE(_S8("Offline mode selected!"));
            *iAgentStatus = KRequestPending;
		    TRequestStatus* status = iAgentStatus;
            User::RequestComplete( status, TNSmlError::ESmlStatusNotPossibleInOfflineMode );
            return;
            }
        // Changes end

		DBG_FILE(_S8("HTTP SendDataL starts..."));
		TRAP( err, this->iHTTPBinding->SendDataL( aStartPtr, ETrue, aStatus ) );
		if( err != KErrNone )
			{
			DBG_FILE(_S8("iHTTPBinding->SendDataL() failed!"));
			*iAgentStatus = KRequestPending;
			TRequestStatus* status = iAgentStatus;
			User::RequestComplete( status, err );
			}
		}
	}
//------------------------------------------------------------
// CNSmlTransport::ChangeTargetURIL
// Changes target URI
//------------------------------------------------------------
EXPORT_C void CNSmlTransport::ChangeTargetURIL( TDesC8& aURI )
	{
	if( iObex )
		{
		}
	else
		{
		DBG_FILE(_S8("HTTP ChangeTargetURIL starts..."));
		this->iHTTPBinding->ChangeTargetURIL( aURI );
		}
	}

//------------------------------------------------------------
// CNSmlTransport::SetBTConnInfo()
// Set BT device address and service class uid
//------------------------------------------------------------
EXPORT_C void CNSmlTransport::SetBTConnInfo( const TBTDevAddr aBTDevAddr,
											 const TUUID aUid )
	{
	iBTDevAddr = aBTDevAddr;
	iServiceUid = aUid;
	}

//------------------------------------------------------------
// CNSmlTransport::SetExtObserver( MExtBTSearcherObserver* aExtObserver )
// Set observer to get callbacks
//------------------------------------------------------------
EXPORT_C void CNSmlTransport::SetExtObserver( MExtBTSearcherObserver* aExtObserver )
	{
	iBTObserver = aExtObserver;
	}

//End of File

