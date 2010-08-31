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
* Description:  SyncML Obex client
*
*/


#include "NSmlObexClient.h"
#include "NSmlObexServiceSearcher.h"
#include "ObexSearcherBase.h"
#include "Obexsearcherfactory.h"
#include "ExtBTSearcherObserver.h"
#include <nsmlconstants.h>
#include <nsmldebug.h>

#include "nsmlerror.h"
#include "wbxml2xmlconverter.h"

//============================================================
// CNsmlObexClient definition
//============================================================


//------------------------------------------------------------
// CNsmlObexClient::NewL()
//------------------------------------------------------------
CNsmlObexClient* CNsmlObexClient::NewL()
    {
	CNsmlObexClient* self = new (ELeave) CNsmlObexClient();
	CleanupStack::PushL( self );
    self->ConstructL( ObexName );
	CleanupStack::Pop(); //self
    return self;
    }

//------------------------------------------------------------
// CNsmlObexClient::CNsmlObexClient()
//------------------------------------------------------------
CNsmlObexClient::CNsmlObexClient(): CActive(CActive::EPriorityStandard),
  iState(EDisconnected), iDataPtr( 0, 0 )
    {
    CActiveScheduler::Add( this );
    }

//------------------------------------------------------------
// CNsmlObexClient* CreateCNsmlObexClientL()
//------------------------------------------------------------
EXPORT_C CNsmlObexClient* CreateCNsmlObexClientL()
	{
	return CNsmlObexClient::NewL();
	}

//------------------------------------------------------------
// CNsmlObexClient::ConstructL( const TDesC8& /*aName*/ )
//------------------------------------------------------------
void CNsmlObexClient::ConstructL( const TDesC8& /*aName*/ )
    {
	iDataBuf = CBufFlat::NewL( KNSmlObexClientGranularity );
    iCurrObject = CObexBufObject::NewL( iDataBuf );
    }

//------------------------------------------------------------
// CNsmlObexClient::~CNsmlObexClient()
//------------------------------------------------------------
CNsmlObexClient::~CNsmlObexClient()
    {
	if( iClient )
		{
		iClient->Disconnect(iStatus);	
	 	User::After( 500000 );
		delete iClient;
		User::WaitForRequest( iStatus );
		}
 
	Cancel();
	delete iDataBuf;
	delete iMimeType;
    delete iCurrObject;
	delete iObexSearcher;
    }

//------------------------------------------------------------
// CNsmlObexClient::DoCancel()
//------------------------------------------------------------
void CNsmlObexClient::DoCancel()
    {
	TRequestStatus* status = iAgentStatus; 

	delete iClient;
	iClient = NULL;

	if( *status == KRequestPending )
		{
		User::RequestComplete( status, KErrCancel );
		}
    }

//------------------------------------------------------------
// CNsmlObexClient::RunL()
//------------------------------------------------------------
void CNsmlObexClient::RunL()
    {
    TInt error( iStatus.Int() );    
	TRequestStatus* status = iAgentStatus;
    
    DBG_FILE_CODE( iStatus.Int(), _S8("CNsmlObexClient::RunL \
	                Before error conversion : ") );
    ErrorConversion( iStatus.Int(), error );
            
    if ( iStatus != KErrNone )
        {
		if ( iState == EGettingConnection && 
			 iStatus == KErrIrObexClientPeerDoesNotHaveObex )
			{
			iState = EConnectionFailed;
			ConnectToServerL();
			}
		else if ( iState == EDisconnecting && iStatus == KErrDisconnected )
			{
			iState = EDisconnected;
			User::RequestComplete( status, error );
			}
		else
			{
			iState = EDisconnected;
			User::RequestComplete( status, KErrCancel );
			}
		}
    else 
        {
        switch ( iState )
            {
            case EGettingConnection:
			case EConnectionFailed:
                iState = EWaitingToSend;
				User::RequestComplete( status, error );			
                break;

            case EWaitingToSend:
				iState = EWaitingToReceive;
				User::RequestComplete( status, error );
                break;

           case EWaitingToReceive:
				iDataPtr = this->iDataBuf->Ptr( 0 );				
				iState = EWaitingToSend;
				User::RequestComplete( status, error );
DBG_DUMP((void*)iDataPtr.Ptr(), iDataPtr.Length(), _S8("ReceiveDataL (WBXML)"));
#ifdef __NSML_DEBUG__
_DBG_FILE("CNsmlObexClient::RunL: CWbxml2XmlConverter::ConvertL() begin");
CWbxml2XmlConverter* c;
c = CWbxml2XmlConverter::NewLC();
c->ConvertL(iDataPtr.Ptr(), iDataPtr.Length());
DBG_DUMP((void*)c->Document().Ptr(), c->Document().Length(), _S8("ReceiveDataL (XML)") );
CleanupStack::PopAndDestroy(); // c
_DBG_FILE("CNsmlObexClient::RunL: CWbxml2XmlConverter::ConvertL() end");
#endif // __NSML_DEBUG__
                break;

		   case EDisconnecting:
                iState = EDisconnected;
				User::RequestComplete( iAgentStatus, KErrNone );
                break;

            default:
                Panic( EBTObjectExchangeSdpRecordDelete );
                break;
            };
        }
    }

TInt CNsmlObexClient::RunError ( TInt aError )
    {
    DBG_FILE_CODE(aError, _S8("CNsmlObexClient::RunError() The Error occurred is "));
    return KErrNone;
    }	
//------------------------------------------------------------
// CNsmlObexClient::ConnectL( TNSmlObexTransport aTransport, TBool /*aServerAlerted*/, TDesC8& aMimeType, TRequestStatus &aStatus )
//------------------------------------------------------------
void CNsmlObexClient::ConnectL( 
								TNSmlObexTransport aTransport,
								TBool /*aServerAlerted*/,
								TDesC8& aMimeType,
								TRequestStatus &aStatus )
    {    
	iAgentStatus = &aStatus;
	// agent
	*iAgentStatus = KRequestPending;

	delete iMimeType;
	iMimeType = NULL;
	iMimeType = HBufC8::NewL( aMimeType.Length() );
	TPtr8 iMIMEptr( iMimeType->Des() );
	iMIMEptr.Copy( aMimeType );

    if ( iState == EDisconnected && !IsActive() )
        {

		//Delete old and create new device/service searcher
		delete iObexSearcher;
		iObexSearcher = NULL;

		//Handle bluetooth as a special case:
		if ( aTransport == EObexBt )
			{
			iObexSearcher = CObexSearcherFactory::CreateBTSearcherL( iBTConnInfo );
			iObexSearcher->SetObserver( this );
			iObexSearcher->SetExtObserver( iExtObserver );

			//Search device first if the device address is undefined
			if ( iBTConnInfo.iDevAddr == TBTDevAddr() )
				{
				iObexSearcher->SearchDeviceL();
				}
			else
				{
				//else skip device search and jump straight to the service search
				iObexSearcher->SearchServiceL();
				}
			}
		else
			{
			//Other cases, only IrDA so far
			iObexSearcher = CObexSearcherFactory::CreateObexSearcherL( aTransport );
			iObexSearcher->SetObserver( this );
			iObexSearcher->SetExtObserver( iExtObserver );
			iObexSearcher->SearchDeviceL();
			}

        }
    else
        {
        //debug  here
        User::Leave( KErrInUse );
        }    
    }

//------------------------------------------------------------
// CNsmlObexClient::ConnectToServerL()
//------------------------------------------------------------
void CNsmlObexClient::ConnectToServerL()
    {    
    if (iClient)
        {
        delete iClient;
        iClient = NULL;
        }

	//Let the obex searcher create the obex client
	iClient = iObexSearcher->CreateObexClientL();
    
	iCurrObject->SetTargetL( KClientTargetHeader ); 
    iClient->Connect( *iCurrObject, iStatus );    
    SetActive();
    }

//------------------------------------------------------------
// CNsmlObexClient::SendDataL( TDesC8& aStartPtr, TBool /*aFinalPacket*/, TRequestStatus &aStatus )
//------------------------------------------------------------
void CNsmlObexClient::SendDataL( TDesC8& aStartPtr, TBool /*aFinalPacket*/, TRequestStatus &aStatus )
    {
	iAgentStatus = &aStatus;
	// agent
	*iAgentStatus = KRequestPending;

	if ( iState == EWaitingToReceive )
		{
		iClient->Abort();
		iState = EWaitingToSend;
		}
    if ( iState != EWaitingToSend )
        {
        User::Leave( KErrDisconnected );
        }
    else if ( IsActive() ) 
        {
        User::Leave( KErrInUse );
        }

    iCurrObject->Reset();
	
	DBG_DUMP((void*)aStartPtr.Ptr(), aStartPtr.Length(), 
	        _S8("SendDataL (WBXML)") );
#ifdef __NSML_DEBUG__
	_DBG_FILE("CNsmlObexClient::SendDataL: CWbxml2XmlConverter::ConvertL() begin");
	CWbxml2XmlConverter* c = CWbxml2XmlConverter::NewLC();
	c->ConvertL(aStartPtr.Ptr(), aStartPtr.Length());
	DBG_DUMP((void*)c->Document().Ptr(), c->Document().Length(), 
	        _S8("SendDataL (XML)") );
	CleanupStack::PopAndDestroy(); // c
	_DBG_FILE("CNsmlObexClient::SendDataL: CWbxml2XmlConverter::ConvertL() end");
#endif // __NSML_DEBUG__
		        
	iDocumentLength = aStartPtr.Length();

	iDataBuf->Reset();
	iDataBuf->InsertL( 0, aStartPtr );
    TRAPD( err, iCurrObject->SetDataBufL( iDataBuf ) );

    if( KErrNone == err )
		{
        TBuf8<KNameLen> str;
		str.Copy( this->iMimeType->Des() );
        iCurrObject->SetTypeL( str );

        iClient->Put( *iCurrObject, iStatus );
		SetActive ();
		}
    }
//------------------------------------------------------------
// CNsmlObexClient::ReceiveDataL( TDes8& aStartPtr, TRequestStatus &aStatus )
//------------------------------------------------------------
void CNsmlObexClient::ReceiveDataL( TPtr8& aStartPtr, TRequestStatus &aStatus )
    {
	iAgentStatus = &aStatus;
	// agent
	*iAgentStatus = KRequestPending;
	
	iDataPtr.Set( aStartPtr );

    if ( iState != EWaitingToReceive )
        {
        User::Leave( KErrDisconnected );
        }
    else if ( IsActive() ) 
        {
        User::Leave( KErrInUse );
        }
	iDataBuf->Reset();
	iCurrObject->Reset();

	TBuf8<KNameLen> str;
	str.Copy( this->iMimeType->Des() );
	iCurrObject->SetTypeL( str );

	iClient->Get( *iCurrObject, iStatus );
	
	DBG_DUMP((void*)aStartPtr.Ptr(), aStartPtr.Length(), 
	_S8("ReceiveDataL (WBXML)"));
#ifdef __NSML_DEBUG__
	_DBG_FILE("CNsmlObexClient::ReceiveDataL: CWbxml2XmlConverter::ConvertL()\
	 begin");
	CWbxml2XmlConverter* c = CWbxml2XmlConverter::NewLC();
	c->ConvertL(aStartPtr.Ptr(), aStartPtr.Length());
	DBG_DUMP((void*)c->Document().Ptr(), c->Document().Length(), 
	_S8("ReceiveDataL (XML)") );
	CleanupStack::PopAndDestroy(); // c
	_DBG_FILE("CNsmlObexClient::ReceiveDataL: CWbxml2XmlConverter::ConvertL() end");
#endif // __NSML_DEBUG__

	SetActive ();
    }
//------------------------------------------------------------
// CNsmlObexClient::CloseCommunicationL( TRequestStatus &aStatus )
//------------------------------------------------------------
void CNsmlObexClient::CloseCommunicationL( TRequestStatus &aStatus )
	{
	iAgentStatus = &aStatus;

	DisconnectL();

	if( *iAgentStatus == KRequestPending )
		{
		User::RequestComplete( iAgentStatus, KErrCancel );
		}
	}
//------------------------------------------------------------
// CNsmlObexClient::StopL()
//------------------------------------------------------------
void CNsmlObexClient::StopL()
   {
	if ( iClient )
		{
		delete iClient;
		iClient = NULL;
		}
    }
//------------------------------------------------------------
// CNsmlObexClient::DisconnectL()
//------------------------------------------------------------
void CNsmlObexClient::DisconnectL()
    {
	if ( iObexSearcher )
		{
		iObexSearcher->Cancel();
		}
	
	StopL();
	iState = EDisconnected;
	}

//------------------------------------------------------------
// CNsmlObexClient::IsBusy()
//------------------------------------------------------------
TBool CNsmlObexClient::IsBusy()
    {
    return IsActive();
    }

//------------------------------------------------------------
// CNsmlObexClient::IsConnected()
//------------------------------------------------------------
TBool CNsmlObexClient::IsConnected()
    {
    return iState != EDisconnected;
    }

//----------------------------------------------------------------------------
// CNsmlObexClient::HandleDeviceFoundL()
//----------------------------------------------------------------------------
//
void CNsmlObexClient::HandleDeviceFoundL()
	{
	TRAPD( err,	iObexSearcher->SearchServiceL(); )
	
	DBG_FILE_CODE( err, _S8("CNsmlObexClient::HandleDeviceFoundL \
	                Before error conversion : ") );
	ErrorConversion( err, err );
	
	if ( err != KErrNone )
		{
		TRequestStatus* status = iAgentStatus;
		User::RequestComplete( status, err );
		}
	}

//----------------------------------------------------------------------------
// CNsmlObexClient::HandleDeviceErrorL( TInt aErr )
//----------------------------------------------------------------------------
//
void CNsmlObexClient::HandleDeviceErrorL( TInt aErr )
	{		
	DBG_FILE_CODE( aErr, _S8("CNsmlObexClient::HandleDeviceErrorL \
	                Before error conversion : ") );
	ErrorConversion( aErr, aErr );
	
	TRequestStatus* status = iAgentStatus;
	User::RequestComplete( status, aErr );
	}

//----------------------------------------------------------------------------
// CNsmlObexClient::HandleServiceFoundL()
//----------------------------------------------------------------------------
//
void CNsmlObexClient::HandleServiceFoundL()
	{
	TRAPD( err,
		iState = EGettingConnection;
		ConnectToServerL();
		)
	
	DBG_FILE_CODE( err, _S8("CNsmlObexClient::HandleServiceFoundL \
	                Before error conversion : ") );
	ErrorConversion( err, err );
	
	if ( err != KErrNone )
		{
		TRequestStatus* status = iAgentStatus;
		User::RequestComplete( status, err );
		}
	}

//----------------------------------------------------------------------------
// CNsmlObexClient::HandleServiceErrorL( TInt aErr )
//----------------------------------------------------------------------------
//
void CNsmlObexClient::HandleServiceErrorL( TInt aErr )
	{
	DBG_FILE_CODE( aErr, _S8("CNsmlObexClient::HandleServiceErrorL \
	                Before error conversion : ") );		
	ErrorConversion( aErr, aErr );
	
	TRequestStatus* status = iAgentStatus;
	User::RequestComplete( status, aErr );
	}

//----------------------------------------------------------------------------
// CNsmlObexClient::SetBTConnInfo( const TBTDevAddr aBTDevAddr, const TUUID aUid )
//----------------------------------------------------------------------------
//
void CNsmlObexClient::SetBTConnInfo( const TBTDevAddr aBTDevAddr, const TUUID aUid )
	{
	TBTConnInfo info;
	info.iDevAddr = aBTDevAddr;
	info.iServiceClass = aUid;

	iBTConnInfo = info;
	}

//----------------------------------------------------------------------------
// CNsmlObexClient::SetExtObserver( MExtBTSearcherObserver* aExtObserver )
//----------------------------------------------------------------------------
//
void CNsmlObexClient::SetExtObserver( MExtBTSearcherObserver* aExtObserver )
	{
	iExtObserver = aExtObserver;
	}

//----------------------------------------------------------------------------
// CNsmlObexClient::ErrorConversion( const TInt aError, TInt& aErrCode )
//----------------------------------------------------------------------------
//
void CNsmlObexClient::ErrorConversion( const TInt aError, TInt& aErrCode )
    {
    if ( aError > -6000 && aError <= -1 )
			{
    	aErrCode = KErrCancel;     
        return;        
    	}
    else if ( aError > -7000 && aError <= -6000 )
			{
      // Conversion of EPageTimedOut and ERemoteHostTimeout errors
      aErrCode = TNSmlError::ESmlCommunicationError;     
      return;
      }
        
    aErrCode = aError;
    }
    
//End of File

