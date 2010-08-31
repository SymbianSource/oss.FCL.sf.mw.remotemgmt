/*
* Copyright (c) 2002-2005 Nokia Corporation and/or its subsidiary(-ies). 
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


#include "nsmlhttp.h"
#include "nsmlhttpclient.h"
#include <e32property.h> 
#include <DevManInternalCRKeys.h>
#include <centralrepository.h>
#include "nsmlprivatepskeys.h"


#include "nsmlerror.h"
#ifdef __NSML_DEBUG__
#include "wbxml2xmlconverter.h"
#endif // __NSML_DEBUG__

#include <centralrepository.h>
#include <ssl.h>
#include "PMUtilInternalCRKeys.h"
#include "nsmlhttpPrivateCRKeys.h"
#include <nsmlunicodeconverter.h>
#include <ezcompressor.h>
#include <ezdecompressor.h>

#ifndef __WINS__
// This lowers the unnecessary compiler warning (armv5) to remark.
// "Warning:  #174-D: expression has no effect..." is caused by 
// DBG_ARGS8 macro in no-debug builds.
#pragma diag_remark 174
#endif
#include <featmgr.h>

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CNSmlHTTP::NewL()
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CNSmlHTTP* CNSmlHTTP::NewL()
    {
	CNSmlHTTP* self = new (ELeave) CNSmlHTTP();
	CleanupStack::PushL( self );
    self->ConstructL();
	CleanupStack::Pop(); //self
    return self;
    }
   
// -----------------------------------------------------------------------------
// CNSmlHTTP::CNSmlHTTP()
// constructor
// -----------------------------------------------------------------------------
//
CNSmlHTTP::CNSmlHTTP() 
: CActive( EPriorityStandard ), iReqBodySubmitBufferPtr( 0,0 )
	{
	CActiveScheduler::Add( this );
	}

// -----------------------------------------------------------------------------
// CNSmlHTTP::ConstructL()
// 2-phase constructor
// -----------------------------------------------------------------------------
//
void CNSmlHTTP::ConstructL() 
	{
	// construct shutdown timer
	DBG_FILE(_S8("CNSmlHTTP::ConstructL BEGIN"));
	FeatureManager::InitializeLibL();
	iShutdown = new (ELeave) CNSmlXptShutdownTimer( this );
	iShutdown->ConstructL();
	iNetworkStatusEngine = NULL;
	// do this only if session is DM 
	TInt session=0;    
    TInt r=RProperty::Get( KPSUidNSmlSOSServerKey, KNSmlSyncJobOngoing, session);                       
    DBG_FILE_CODE(session, _S8("CNSmlHTTP::ConstructL Current Session is (DM = 2, DS = 1) "));
    if( session == ESyncMLDMSession )//for dm session
       {
		TInt dmsessionTimeout = -1;
		CRepository *rep = NULL;
		TRAPD( err1, rep = CRepository::NewL( KCRUidDeviceManagementInternalKeys ))
		DBG_FILE_CODE(err1, _S8("CNSmlHTTP::ConstructL cenrep read error code "));
		if(err1 == KErrNone)
		{
			rep->Get( KDevManDMSessionTimeout, dmsessionTimeout );
			delete rep;
			DBG_FILE_CODE(dmsessionTimeout, _S8("CNSmlHTTP::ContructL, DMSessiontimeout feature value from cenrep"));
			if( dmsessionTimeout > KNSmlDMMaxSessionTimeout || dmsessionTimeout < KNSmlDMMinSessionTimeout)
			{
			dmsessionTimeout = -1;
			}
			DBG_FILE_CODE(dmsessionTimeout, _S8("CNSmlHTTP::ContructL, DMSessiontimeout feature value "));
		}
		if(dmsessionTimeout != -1)
		{
			iNetworkStatusEngine = new (ELeave) CNsmlNetworkStatusEngine( this );
			iNetworkStatusEngine->ConstructL();
			iNetworkStatusEngine->NotifyL();
		}
       }
	
	iPreemptRequest = 0; 

	iServerContentEncoding = ExptNone;
	iServerAcceptEncoding = ExptNone;
	iSession = ESyncMLSessionUnknown;
    RProperty::Get( KPSUidNSmlSOSServerKey, KNSmlSyncJobOngoing, iSession);                       
	// construct dialup agent
	iDialUpAgent = new (ELeave) CNSmlDialUpAgent();
	iDialUpAgent->ConstructL();

	iEngineState = ExptIdle;
	iTimeOut = EFalse;
	iLastPart = EFalse;
	iAuthRetryCount=0;
	iAuthUsed=0;
	
	iTransObs = CHttpEventHandler::NewL();
	iTransObs->ConstructL( this );
	iMaxMsgSize = KNSmlDefaultWorkspaceSize;
		
    if( iSession == ESyncMLDSSession )
        {        
        TInt value(0);
        TRAPD( err, ReadRepositoryL( KNSmlMaxMsgSizeKey, value) );      
        if ( err == KErrNone )
            {
            iMaxMsgSize = value;
            }
        }
	}

// ---------------------------------------------------------
// CNSmlHTTP::ReadRepositoryL(TInt aKey, TInt& aValue)
// 
// ---------------------------------------------------------
TInt CNSmlHTTP::ReadRepositoryL(TInt aKey, TInt& aValue)
    {    
    const TUid KRepositoryId = KCRUidNSmlDSEngine;

    CRepository* rep = CRepository::NewLC(KRepositoryId);
    TInt err = rep->Get(aKey, aValue);    
    CleanupStack::PopAndDestroy(rep);
    
    return err;
    }

// -----------------------------------------------------------------------------
// CNSmlHTTP::~CNSmlHTTP()
// desctructor
// -----------------------------------------------------------------------------
//
CNSmlHTTP::~CNSmlHTTP() 
	{
DBG_FILE(_S8("CNSmlHTTP::~CNSmlHTTP() BEGIN"));
if(FeatureManager::FeatureSupported(KFeatureIdSapPolicyManagement))
{
	DeleteCertificate(); 
}

	Cancel();
	delete iData;
	delete iReqBodySubmitBuffer;
	delete iURI;
	delete iMimeType;
	delete iHTTPusername;	
	delete iHTTPpassword;	
	delete iShutdown;
	iShutdown = NULL;
	if(iNetworkStatusEngine)
	{
		delete iNetworkStatusEngine;
		iNetworkStatusEngine = NULL;
	}
	iSess.Close();
	delete iTransObs;
	delete iDialUpAgent;
    delete iIAPidArray;
    FeatureManager::UnInitializeLib();
    DBG_FILE(_S8("CNSmlHTTP::~CNSmlHTTP() END"));
	}

// -----------------------------------------------------------------------------
// CNSmlHTTP::DoCancel()
// DoCancel() from CActive
// -----------------------------------------------------------------------------
//
void CNSmlHTTP::DoCancel() 
	{DBG_FILE(_S8("CNSmlHTTP::DoCancel() BEGIN"));
	iEngineState = ExptIdle;
	TInt cancelReason;
	
	iShutdown->Cancel();
	iDialUpAgent->Cancel();
	iSess.Close();

	if( iTimeOut )
		{DBG_FILE(_S8("CNSmlHTTP::DoCancel() timing out"));
		cancelReason = TNSmlHTTPErrCode::ENSmlHTTPErr_RequestTimeout;
		}
	else
		{
		cancelReason = KErrCancel;
		}
	// signal current (engine) thread request semaphore that this AO's 
	// request has completed
	TRequestStatus* status = &iStatus;		
	User::RequestComplete( status, cancelReason );
	// signal agent
	TRequestStatus* agentStatus = iAgentStatus;
	User::RequestComplete( agentStatus, cancelReason );
	DBG_FILE(_S8("CNSmlHTTP::DoCancel() END"));
	}

// -----------------------------------------------------------------------------
// CNSmlHTTP::CompleteRequest()
// called when asynchronous request should be completed with a purpose
// -----------------------------------------------------------------------------
//
void CNSmlHTTP::CompleteRequest()
{
	DBG_FILE(_S8("CNSmlHTTP::CompleteRequest BEGIN"));
		
		if(IsActive())
		{
		DBG_FILE(_S8("CNSmlHTTP::CompleteRequest iTrans.Cancel();"));
		iTrans.Cancel();
		DBG_FILE(_S8("CNSmlHTTP::CompleteRequest iTrans.Close();"));
		iTrans.Close();
		DBG_FILE(_S8("CNSmlHTTP::CompleteRequest isActive returned TRUE  "));
		iPreemptRequest++;
		DBG_FILE_CODE( iPreemptRequest, _S8("Increasing value of iPreemptRequest ") );
				TRequestStatus* status = &iStatus;
		User::RequestComplete( status, KErrTimedOut );
		}

	DBG_FILE(_S8("CNSmlHTTP::CompleteRequest ENDS "));
}
// CNSmlHTTP::RunL()
// Runl() from CActive
// called when asynchronous request completed
// -----------------------------------------------------------------------------
//
void CNSmlHTTP::RunL() 
	{
	DBG_FILE(_S8("CNSmlHTTP::RunL begins"));

	DBG_FILE_CODE( iEngineState, _S8("RunL iEgnineStatus is : ") );
	DBG_FILE_CODE( iStatus.Int(), _S8("RunL httpClient status is : ") );
	if(iEngineState == ExptOpenCommunication)
		{
		DBG_FILE(_S8("CNSmlHTTP::RunL before SetHttpConnectionInfoL"));
		DBG_FILE_CODE( iStatus.Int(), _S8("RunL HTTP client iStatus: ") );
		if( iStatus == KErrNone) 
			{  
			DBG_FILE(_S8("CNSmlHTTP::RunL Status was NO ERROR"));
			TRAPD( ret, SetHttpConnectionInfoL( EFalse ) );
			if ( ret != KErrNone )
				{
				DBG_FILE_CODE( ret, _S8("CNSmlHTTP::RunL SetHttpConnectionInfoL\
				 error:") );
				iShutdown->Cancel();
				TRequestStatus* status = iAgentStatus;
				User::RequestComplete( status, ret );
				iTimeOut = EFalse;
				return;
				}
			}
		DBG_FILE(_S8("CNSmlHTTP::RunL after SetHttpConnectionInfoL"));
		}

	iShutdown->Cancel();
	TRequestStatus* status = iAgentStatus;
	User::RequestComplete( status, iStatus.Int() );
	iTimeOut = EFalse;
	DBG_FILE(_S8("CNSmlHTTP::RunL ends"));
	}

// -----------------------------------------------------------------------------
// CNSmlHTTP::OpenCommunicationL
// Establishes a communication using the protocol service with the given ID.
// -----------------------------------------------------------------------------
//
void CNSmlHTTP::OpenCommunicationL( 
    CArrayFixFlat<TUint32>* aIAPidArray, 
    TDesC8& aURI, 
    TDesC8& aMimeType, 
    TRequestStatus &aStatus, 
    TDesC8& aHTTPusername, 
    TDesC8& aHTTPpassword, 
    TInt aHTTPauthused )
	{
	__ASSERT_ALWAYS( !IsActive(), User::Panic( _L("CNSmlHTTP"), 1 ) );
	if ( !iShutdown->IsActive() )
	    {
    	iShutdown->Start();    
	    }
	iAgentStatus = &aStatus;
	iEngineState = ExptOpenCommunication;

    iIAPidArray = new ( ELeave ) CArrayFixFlat<TUint32>(KSmlArrayGranularity);

    for ( TInt i = 0 ; i < aIAPidArray->Count() ; i++ )
        {
        iIAPidArray->AppendL( aIAPidArray->At( i ) );
        }
	
	iAuthUsed = aHTTPauthused;	

	// Open the RHTTPSession
	iSess.OpenL();

	// Install this class as the callback for authentication requests
	if( iAuthUsed ) 
		{
		InstallAuthenticationL(iSess);
		}

	delete iURI;
	iURI = NULL;
	iURI = HBufC8::NewL( aURI.Length() );
	TPtr8 iURIptr( iURI->Des() );
	iURIptr.Copy( aURI );

	delete iMimeType;
	iMimeType = NULL;
	iMimeType = HBufC8::NewL( aMimeType.Length() );
	TPtr8 iMIMEptr( iMimeType->Des() );
	iMIMEptr.Copy( aMimeType );

	delete iHTTPusername;
	iHTTPusername = NULL;
	if(aHTTPusername.Length()!=0)
		{
		iHTTPusername = HBufC8::NewL( aHTTPusername.Length() );
		TPtr8 iHTTPusernameptr( iHTTPusername->Des() );
		iHTTPusernameptr.Copy( aHTTPusername );
		}
	else
		{
		iHTTPusername = KNullDesC8().AllocL();
		}

	delete iHTTPpassword;
	iHTTPpassword = NULL;
	if(aHTTPpassword.Length()!=0)
		{
		iHTTPpassword = HBufC8::NewL( aHTTPpassword.Length() );
		TPtr8 iHTTPpasswordptr( iHTTPpassword->Des() );
		iHTTPpasswordptr.Copy( aHTTPpassword );
		}
	else
		{
		iHTTPpassword = KNullDesC8().AllocL();
		}

	TRAPD( err, ProcessRequestL() );
	User::LeaveIfError( err );
	}

// -----------------------------------------------------------------------------
// CNSmlHTTP::CloseCommunicationL( TRequestStatus &aStatus )
// Closes a previously opened communication.
// -----------------------------------------------------------------------------
//
void CNSmlHTTP::CloseCommunicationL( TRequestStatus &aStatus )
	{
	__ASSERT_ALWAYS( !IsActive(), User::Panic( _L("CNSmlHTTP"), 1 ) );
	iAgentStatus = &aStatus;
	iEngineState = ExptCloseCommunication;
	TRAPD( err, ProcessRequestL() );
	User::LeaveIfError( err );
	}

// -----------------------------------------------------------------------------
// CNSmlHTTP::ReceiveDataL
// Read data across a connection.
// -----------------------------------------------------------------------------
//
void CNSmlHTTP::ReceiveDataL( TDes8& aStartPtr, TRequestStatus &aStatus )
	{
	__ASSERT_ALWAYS( !IsActive(), User::Panic( _L("CNSmlHTTP"), 1 ) );
	iAgentStatus = &aStatus;
	if ( !iShutdown->IsActive() )
	    {
    	iShutdown->Start();    
	    }
	iEngineState = ExptReceiveData;
	GetResponseBodyL( aStartPtr );

	DBG_DUMP((void*)aStartPtr.Ptr(), aStartPtr.Length(), 
	_S8("ReceiveDataL (WBXML)"));
#ifdef __NSML_DEBUG__
	_DBG_FILE("CNSmlHTTP::ReceiveDataL(): CWbxml2XmlConverter::ConvertL()\
	 begin");
	CWbxml2XmlConverter* c = CWbxml2XmlConverter::NewLC();
	c->ConvertL(aStartPtr.Ptr(), aStartPtr.Length());
	DBG_DUMP((void*)c->Document().Ptr(), c->Document().Length(), 
	_S8("ReceiveDataL (XML)") );
	CleanupStack::PopAndDestroy(); // c
	_DBG_FILE("CNSmlHTTP::ReceiveDataL(): CWbxml2XmlConverter::ConvertL() end");
#endif // __NSML_DEBUG__

	TRAPD( err, ProcessRequestL() );
	User::LeaveIfError( err );
	}


// -----------------------------------------------------------------------------
// CNSmlHTTP::DeleteCertificate()
// Delete HTTP connection certificate.
// -----------------------------------------------------------------------------
//
void CNSmlHTTP::DeleteCertificate( )
	{
	if(!FeatureManager::FeatureSupported(KFeatureIdSapPolicyManagement))
		{
			
		}
		else
	{
	CRepository *re = NULL;
	TRAPD( erx, re = CRepository::NewL ( 
	        KCRUidPolicyManagementUtilInternalKeys ) );
	if (erx == KErrNone )
		{
		re->Reset( KSyncMLSessionCertificate );
		delete re;
		re = NULL;
		}
	else
		{
		DBG_ARGS8(_S8("ERROR Failed to delete reposiritry key %d"), erx );
		}
	}
}

// -----------------------------------------------------------------------------
// CNSmlHTTP::SaveCertificateL( RHTTPTransaction &aTransaction )
// save HTTP connection certificate.
// -----------------------------------------------------------------------------
//
void CNSmlHTTP::SaveCertificateL( RHTTPTransaction &aTransaction )
	{
		if(!FeatureManager::FeatureSupported(KFeatureIdSapPolicyManagement))
		{
			User::Leave(KErrNotSupported);
		}
	TCertInfo info ;
	TInt errx( aTransaction.ServerCert( info ) );
	if ( errx == KErrNone )
		{
		CRepository *re = NULL;
		TRAPD( erx, re = CRepository::NewL ( 
		            KCRUidPolicyManagementUtilInternalKeys ) );
		if (erx == KErrNone )
			{
			TPckg<TCertInfo> pcert( info );
			errx = re->Create( KSyncMLSessionCertificate, pcert ) ;
			if ( errx == KErrNone )
				{
				DBG_ARGS8(_S8("Wrote reposotry key %S"), &pcert );
				}
			else
				{
				if ( errx == KErrAlreadyExists )
					{
					errx = re->Set( KSyncMLSessionCertificate, pcert ) ;
					if ( errx != KErrNone )
						{
						DBG_ARGS8( _S8("ERROR Failed to add reposiritry \
						key %d"), errx );
						}
					}
				else
					{
					DBG_ARGS8( _S8("ERROR Failed to create reposiritry \
					key %d"), errx );	
					}
				}	
			delete re ;
			re = NULL;
			}
		else
			{
			DBG_ARGS8(_S8("ERROR Failed to open reposiritry %d"), erx );	
			}
		}
	else
		{
		DBG_ARGS8(_S8("ERROR Failed to get certificate %d"), errx );	
		}

	}

// -----------------------------------------------------------------------------
// CNSmlHTTP::CompressL
// Compress the data.
// -----------------------------------------------------------------------------
//
void CNSmlHTTP::CompressL(TDesC8& aStartPtr)
    {   
    HBufC8* tempBufferPtr = HBufC8::NewLC(iMaxMsgSize);
    TPtr8 unCompressed( tempBufferPtr->Des());
    unCompressed.Copy( aStartPtr );        

    iReqBodySubmitBuffer = NULL;
    iReqBodySubmitBuffer = HBufC8::NewMaxL( iMaxMsgSize );

    iReqBodySubmitBufferPtr.Set( iReqBodySubmitBuffer->Des() );
    TRAPD(err, CEZCompressor::CompressL(iReqBodySubmitBufferPtr, unCompressed, Z_BEST_COMPRESSION));
    CleanupStack::PopAndDestroy();
    if(err == KErrNone)
        {
        iDocumentLength = iReqBodySubmitBufferPtr.Length();
        }
    }


// -----------------------------------------------------------------------------
// CNSmlHTTP::SendDataL
// Send data across a connection.
// -----------------------------------------------------------------------------
//
void CNSmlHTTP::SendDataL( 
    TDesC8& aStartPtr, 
    TBool /*aFinalPacket*/, 
    TRequestStatus &aStatus )
	{
	__ASSERT_ALWAYS( !IsActive(), User::Panic( _L("CNSmlHTTP"), 1 ) );
	iAgentStatus = &aStatus;
	if ( !iShutdown->IsActive() )
	    {
    	iShutdown->Start();    
	    }
	iEngineState = ExptSendData;

	DBG_DUMP((void*)aStartPtr.Ptr(), aStartPtr.Length(), 
	        _S8("SendDataL (WBXML)") );
#ifdef __NSML_DEBUG__
	_DBG_FILE("CNSmlHTTP::SendDataL(): CWbxml2XmlConverter::ConvertL() begin");
	CWbxml2XmlConverter* c = CWbxml2XmlConverter::NewLC();
	c->ConvertL(aStartPtr.Ptr(), aStartPtr.Length());
	DBG_DUMP((void*)c->Document().Ptr(), c->Document().Length(), 
	        _S8("SendDataL (XML)") );
	CleanupStack::PopAndDestroy(); // c
	_DBG_FILE("CNSmlHTTP::SendDataL(): CWbxml2XmlConverter::ConvertL() end");
#endif // __NSML_DEBUG__
	
	delete iReqBodySubmitBuffer;
    iReqBodySubmitBuffer = NULL;
	
	if( (iSession == ESyncMLDSSession) && (iServerAcceptEncoding == ExptDeflate) )
		{
		TRAPD( err, CompressL(aStartPtr) );
		User::LeaveIfError( err );
		}
	else
		{
    iReqBodySubmitBuffer = HBufC8::NewMaxL( iMaxMsgSize );
	iReqBodySubmitBufferPtr.Set( iReqBodySubmitBuffer->Des() );
	iReqBodySubmitBufferPtr.Copy( aStartPtr );
	iDocumentLength = aStartPtr.Length();
		}
	TRAPD( err, ProcessRequestL() );
	User::LeaveIfError( err );
	}

// -----------------------------------------------------------------------------
// CNSmlHTTP::DecompressL
// Decompress the data.
// -----------------------------------------------------------------------------
//
void CNSmlHTTP::DecompressL(TDes8& aStartPtr)
    {
    TPtr8 ptrCompressed( iData->Des() );     
    HBufC8* ideCompressed = HBufC8::NewLC(iMaxMsgSize);
    TPtr8 ptrdeCompressed( ideCompressed->Des() );

    TRAPD(err, CEZDecompressor::DecompressL(ptrdeCompressed,ptrCompressed));
    
    if(err == KErrNone)
        {    
        aStartPtr.Copy( ideCompressed->Des());        
        }
    CleanupStack::PopAndDestroy();
    }


// -----------------------------------------------------------------------------
// CNSmlHTTP::ProcessRequestL()
// wake up thread (or create one) to handle requested service 
// -----------------------------------------------------------------------------
//
void CNSmlHTTP::ProcessRequestL()
	{
	TRequestStatus* statusOwn = &this->iStatus;
	// set AO (CNSmlHTTP) active
	this->iStatus = KRequestPending;
	this->SetActive();
	// agent
	*iAgentStatus = KRequestPending;

	if ( iEngineState == ExptOpenCommunication )
		{
		this->iDialUpAgent->ConnectL( iIAPidArray, iStatus );
		}
	else if ( iEngineState == ExptCloseCommunication )
		{
		this->Cancel();	
		}
	else if ( iEngineState == ExptReceiveData )
		{
		User::RequestComplete( statusOwn, KErrNone ); 
		}
	else if ( iEngineState == ExptSendData )
		{
		RStringPool strP = iSess.StringPool();
		RStringF method;
		method = strP.StringF( HTTP::EPOST, RHTTPSession::GetTable() );

		RHTTPConnectionInfo connInfo = iSess.ConnectionInfo();
		connInfo.SetPropertyL ( strP.StringF(HTTP::EHttpSocketServ, 
		                        RHTTPSession::GetTable() ), 
		                        THTTPHdrVal (
		                        this->iDialUpAgent->iSocketServer.Handle() ) );
		TInt connPtr = REINTERPRET_CAST(TInt, &this->iDialUpAgent->iConnection);
		connInfo.SetPropertyL ( strP.StringF(HTTP::EHttpSocketConnection, 
		                        RHTTPSession::GetTable() ), 
		                        THTTPHdrVal (connPtr) );
		TInt session=0;    
    TInt r1=RProperty::Get( KPSUidNSmlSOSServerKey, KNSmlSyncJobOngoing, session);  
    if( session == ESyncMLDMSession )//for dm session
    { 
    	TInt dmsessionTimeout = -1;
		  CRepository *rep = NULL;
		  TRAPD( err1, rep = CRepository::NewL( KCRUidDeviceManagementInternalKeys ));
		  DBG_FILE_CODE(err1, _S8("CNSmlHTTP::ConstructL cenrep read error code "));
		  if(err1 == KErrNone)
		  {
			rep->Get( KDevManDMSessionTimeout, dmsessionTimeout );
			delete rep;
			DBG_FILE_CODE(dmsessionTimeout, _S8("DMSessiontimeout feature value from cenrep"));
		  	if( dmsessionTimeout < KNSmlDMMaxSessionTimeout && dmsessionTimeout > KNSmlDMMinSessionTimeout)
			  {                    
		      THTTPHdrVal immediateShutdown = strP.StringF(HTTP::ESocketShutdownImmediate, RHTTPSession::GetTable() );
		      connInfo.SetPropertyL ( 
            strP.StringF(HTTP::ESocketShutdownMode, RHTTPSession::GetTable()), 
            immediateShutdown );                        
        } 
      }    
    }
if(FeatureManager::FeatureSupported(KFeatureIdSapPolicyManagement))
{
    TInt Session=0;    
    TInt r=RProperty::Get( KPSUidNSmlSOSServerKey, KNSmlSyncJobOngoing, Session);                       
    if( Session == ESyncMLDMSession )//for dm session
       {
        THTTPHdrVal secDlgNo( strP.StringF(HTTP::EDialogNoPrompt,
                             RHTTPSession::GetTable() ) );
        connInfo.SetPropertyL ( strP.StringF(HTTP::ESecureDialog, 
                                RHTTPSession::GetTable() ), secDlgNo );
       DBG_FILE_CODE( Session, _S8("In HTTP No prompt set ") );                            
       }       
}


		InvokeHttpMethodL( iURI->Des(), method );
		}
	}

// -----------------------------------------------------------------------------
// CNSmlHTTP::ChangeTargetURIL( TDesC8& aURI )
// Changes target URI
// -----------------------------------------------------------------------------
//
void CNSmlHTTP::ChangeTargetURIL( TDesC8& aURI )
	{
	delete iURI;
	iURI = NULL;
	iURI = HBufC8::NewL( aURI.Length() );
	TPtr8 iURIptr( iURI->Des() );
	iURIptr.Copy( aURI );
	}

// -----------------------------------------------------------------------------
// CNSmlHTTP::SetErrorStatus( Tint aError ) 
// Returns the internal http error status code for 4xx-5xx statuses
// -----------------------------------------------------------------------------
//
TInt CNSmlHTTP::SetErrorStatus( TInt aError ) 
	{
	TInt error = aError;
	DBG_FILE_CODE( aError, _S8("http Error code in SetError CNSmlHTTP  ") );
	if( aError == 400 )
		{
		return TNSmlHTTPErrCode::ENSmlHTTPErr_BadRequest;
		}
	else if ( aError == 401)
		{
		return TNSmlHTTPErrCode::ENSmlHTTPErr_Unauthorized;
		}
	else if ( aError == 402)
		{
		return TNSmlHTTPErrCode::ENSmlHTTPErr_PaymentRequired;
		}
	else if ( aError == 403)
		{
		return TNSmlHTTPErrCode::ENSmlHTTPErr_Forbidden;
		}
	else if ( aError == 404)
		{
		return TNSmlHTTPErrCode::ENSmlHTTPErr_NotFound;
		}
	else if ( aError == 405)
		{
		return TNSmlHTTPErrCode::ENSmlHTTPErr_MethodNotAllowed;
		}
	else if ( aError == 406)
		{
		return TNSmlHTTPErrCode::ENSmlHTTPErr_NotAcceptable;
		}
	else if ( aError == 407)
		{
		return TNSmlHTTPErrCode::ENSmlHTTPErr_ProxyAuthenticationRequired;
		}
	else if ( aError == 408)
		{
		return TNSmlHTTPErrCode::ENSmlHTTPErr_RequestTimeout;
		}
	else if ( aError == 409)
		{
		return TNSmlHTTPErrCode::ENSmlHTTPErr_Conflict;
		}
	else if ( aError == 410)
		{
		return TNSmlHTTPErrCode::ENSmlHTTPErr_Gone;
		}
	else if ( aError == 411)
		{
		return TNSmlHTTPErrCode::ENSmlHTTPErr_LengthRequired;
		}
	else if ( aError == 412)
		{
		return TNSmlHTTPErrCode::ENSmlHTTPErr_PreconditionFailed;
		}
	else if ( aError == 413)
		{
		return TNSmlHTTPErrCode::ENSmlHTTPErr_RequestEntityTooLarge;
		}
	else if ( aError == 414)
		{
		return TNSmlHTTPErrCode::ENSmlHTTPErr_RequestURITooLong;
		}
	else if ( aError == 415)
		{
		return TNSmlHTTPErrCode::ENSmlHTTPErr_UnsupportedMediaType;
		}
	else if ( aError == 416)
		{
		return TNSmlHTTPErrCode::ENSmlHTTPErr_RequestedRangeNotSatisfiable;
		}
	else if ( aError == 417)
		{
		return TNSmlHTTPErrCode::ENSmlHTTPErr_ExpectationFailed;
		}
	else if ( aError == 500)
		{
		return TNSmlHTTPErrCode::ENSmlHTTPErr_InternalServerError;
		}
	else if ( aError == 501)
		{
		return TNSmlHTTPErrCode::ENSmlHTTPErr_NotImplemented;
		}
	else if ( aError == 502)
		{
		return TNSmlHTTPErrCode::ENSmlHTTPErr_BadGateway;
		}
	else if ( aError == 503)
		{
		return TNSmlHTTPErrCode::ENSmlHTTPErr_ServiceUnavailable;
		}
	else if ( aError == 504)
		{
		return TNSmlHTTPErrCode::ENSmlHTTPErr_GatewayTimeout;
		}
	else if ( aError == 505)
		{
		return TNSmlHTTPErrCode::ENSmlHTTPErr_HTTPVersionNotSupported;
		}
	else
		{
		return error;
		}
	}

// -----------------------------------------------------------------------------
// CNSmlHTTP::GetCredentialsL (const TUriC8 &aURI,RString aRealm,
//   RStringF aAuthenticationType,RString &aUsername,RString &aPassword)
// Gets some credentials
// -----------------------------------------------------------------------------
//
TBool CNSmlHTTP::GetCredentialsL (
    const TUriC8 &/*aURI*/,
    RString aRealm,
    RStringF /*aAuthenticationType*/,
    RString &aUsername,
    RString &aPassword)
	{

	if(iAuthRetryCount > 1)
		{
		return EFalse;
		}

	TRAPD(err, aUsername = aRealm.Pool().OpenStringL(*iHTTPusername));
	if (!err)
		{
		TRAP(err, aPassword = aRealm.Pool().OpenStringL(*iHTTPpassword));
		}

	if (!err) 
		{
		iAuthRetryCount++;
 		return ETrue;
		}
	else
		{	
		return EFalse;
		}
	}

// -----------------------------------------------------------------------------
// CNSmlHTTP::SetHttpConnectionInfoL( TBool aUseOwnConnection ) 
// Sets proxy for the connection, if it is configured for the IAP..
// -----------------------------------------------------------------------------
//
void CNSmlHTTP::SetHttpConnectionInfoL( TBool aUseOwnConnection ) 
	{
	DBG_FILE(_S8("CNSmlHTTP::SetHttpConnectionInfo begins"));    
	TInt result;
    TBuf<100> serviceType;
    TUint32 serviceId;
    TBuf<100> query;
    TBuf<100> proxyAddr;
    TBuf8<100> proxyAddr2;
    TUint32 proxyPort;
    TUint connCount;
    TBool useProxy;
    CCommsDatabase* TheDb;
    RStringF proxyName;  

    // Trick to get new values into use    
    iSess.Close();
    iSess.OpenL();

    RStringPool strPool = iSess.StringPool();


    // Remove first session properties just in case.
    RHTTPConnectionInfo connInfo = iSess.ConnectionInfo();
    
    // Clear RConnection and Socket Server instances
    connInfo.RemoveProperty(strPool.StringF(HTTP::EHttpSocketServ,
                            RHTTPSession::GetTable()));
    connInfo.RemoveProperty(strPool.StringF(HTTP::EHttpSocketConnection,
                            RHTTPSession::GetTable()));
    
    // Clear the proxy settings
    THTTPHdrVal proxyUsage(strPool.StringF(HTTP::EUseProxy,
                            RHTTPSession::GetTable()));
    connInfo.RemoveProperty(strPool.StringF(HTTP::EProxyUsage,
                            RHTTPSession::GetTable()));
    connInfo.RemoveProperty(strPool.StringF(HTTP::EProxyAddress,
                            RHTTPSession::GetTable()));
    
    if(!aUseOwnConnection)
		{
        // RConnection has been started, set proxy (if defined) and RConnection and
        // Socket Server session properties.
        
		DBG_FILE(_S8("CNSmlHTTP::SetHttpConnectionInfo !aUseOwnConnection")); 
        // Proxy
        result = this->iDialUpAgent->iConnection.
                                     EnumerateConnections(connCount);
        User::LeaveIfError(result);
        
        //
        // Get service and service type for this connection
        //
        query.Format(_L("%s\\%s"), IAP, IAP_SERVICE);
        result = this->iDialUpAgent->iConnection.GetIntSetting(query, 
                                                                serviceId);
        
        query.Format(_L("%s\\%s"), IAP, IAP_SERVICE_TYPE);
        result = this->iDialUpAgent->iConnection.GetDesSetting(query, 
                                                                serviceType);
        User::LeaveIfError(result);
        
        TheDb = CCommsDatabase::NewL();
        CleanupStack::PushL(TheDb);
        
        CCommsDbTableView* view = TheDb->OpenViewOnProxyRecordLC(serviceId, 
                                                                serviceType);
        result = view->GotoFirstRecord();
        
        if(result == KErrNone)
        	{
            // This IAP uses proxy, set it to http session
            view->ReadBoolL(TPtrC(PROXY_USE_PROXY_SERVER), useProxy);
            if(useProxy)
                {
				DBG_FILE(_S8("CNSmlHTTP::SetHttpConnectionInfo WE are using PROXY!")); 
	            
	            TRAPD(err ,view->ReadUintL(TPtrC(PROXY_PORT_NUMBER), proxyPort));
	            
	            if (err == KErrNone)
		            {
		            DBG_FILE(_S8("Proxy Port Number Found")); 
		            HBufC* k = view->ReadLongTextLC(TPtrC(PROXY_SERVER_NAME));
		            proxyAddr.Copy(k->Des());
		            proxyAddr.AppendFormat(_L(":%d"), proxyPort);
		            
		            proxyAddr2.Copy(proxyAddr);
		            
		            CleanupClosePushL(proxyName);
		            proxyName = iSess.StringPool().OpenFStringL(proxyAddr2);
		            connInfo.SetPropertyL(strPool.StringF(HTTP::EProxyUsage,
		                                    RHTTPSession::GetTable()), proxyUsage);
		            connInfo.SetPropertyL(strPool.StringF(HTTP::EProxyAddress,
		                                    RHTTPSession::GetTable()), proxyName);
		            CleanupStack::PopAndDestroy(); // proxyName
		            CleanupStack::PopAndDestroy(); //k
		            }
	            }
            
        	}
        CleanupStack::PopAndDestroy(2); // view, TheDb
        
		DBG_FILE(_S8("CNSmlHTTP::SetHttpConnectionInfo before SetPropertys")); 
        // RConnection and Socket Server
        connInfo.SetPropertyL ( 
            strPool.StringF(HTTP::EHttpSocketServ, RHTTPSession::GetTable()), 
            THTTPHdrVal (this->iDialUpAgent->iSocketServer.Handle()) );
        
        TInt connPtr1 = REINTERPRET_CAST(TInt, &this->iDialUpAgent->iConnection);
        connInfo.SetPropertyL ( 
            strPool.StringF(HTTP::EHttpSocketConnection, 
            RHTTPSession::GetTable() ), THTTPHdrVal (connPtr1) );    
        
		if( iAuthUsed ) 
			{
			DBG_FILE(_S8("CNSmlHTTP::SetHttpConnectionInfo \
			            before installing auth filter"));
			InstallAuthenticationL( iSess );
			}

		}

	DBG_FILE(_S8("CNSmlHTTP::SetHttpConnectionInfo ends"));    
	}

// -----------------------------------------------------------------------------
// CNSmlHTTP::GetNextDataPart( TPtrC8& aDataPart )
// 
// -----------------------------------------------------------------------------
//
TBool CNSmlHTTP::GetNextDataPart( TPtrC8& aDataPart )
	{
	aDataPart.Set( iReqBodySubmitBufferPtr );
	iLastPart = ETrue;
	return iLastPart;
	}

// -----------------------------------------------------------------------------
// CNSmlHTTP::ReleaseData()
// 
// -----------------------------------------------------------------------------
//
void CNSmlHTTP::ReleaseData()
	{
	
	// Fix for BPSS-7H7H5S
		
	// Clear out the submit buffer
	//TPtr8 buff = iReqBodySubmitBuffer->Des();
	//buff.Zero();
		
	if (iLastPart)
		return;
	// Notify HTTP of more data available immediately, since it's being read 
	//from file
	TRAPD( err, iTrans.NotifyNewRequestBodyPartL() );
	if ( err != KErrNone )
		User::Panic( KSmlHttpClientPanic, KCouldNotNotifyBodyDataPart );
	}

// -----------------------------------------------------------------------------
// CNSmlHTTP::OverallDataSize()
// 
// -----------------------------------------------------------------------------
//
TInt CNSmlHTTP::OverallDataSize()
	{
	return iDocumentLength;
	}

// -----------------------------------------------------------------------------
// CNSmlHTTP::Reset()
// 
// -----------------------------------------------------------------------------
//
TInt CNSmlHTTP::Reset()
	{
	// Reset is called when the posted data needs to be resend
	return KErrNone;
	}

// -----------------------------------------------------------------------------
// CNSmlHTTP::InvokeHttpMethodL( const TDesC8& aUri, RStringF aMethod )
// Invoke the http method
// This actually creates the transaction, sets the headers and 
// body and then starts the transaction 
// -----------------------------------------------------------------------------
//
void CNSmlHTTP::InvokeHttpMethodL( const TDesC8& aUri, RStringF aMethod )
	{
	TUriParser8 uri; 
	uri.Parse( aUri );
	iTrans = iSess.OpenTransactionL( uri, *iTransObs, aMethod );
	RHTTPHeaders hdr = iTrans.Request().GetHeaderCollection();

	// Add headers appropriate to all methods
	TInt error;
	HBufC* useragent = HBufC::NewLC( NCentralRepositoryConstants::KMaxUnicodeStringLength );
	TPtr ptr (useragent->Des() );
	CRepository* rep = CRepository::NewLC( KCRUidNsmlHttp );
	error = rep->Get( KNsmlHttpUAHeader, ptr );
	if( error == KErrNone )
	{
		HBufC8* uaheader;
	    NSmlUnicodeConverter::HBufC8InUTF8LC( *useragent, uaheader );
	    TPtr8 ptr8( uaheader->Des() );
	    SetHeaderL( hdr, HTTP::EUserAgent, uaheader->Des() );
	    DBG_ARGS8(_S8("CNSmlHTTP::InvokeHttpMethodL KNsmlHttpUAHeader key value %S"), &ptr8);
		CleanupStack::PopAndDestroy( uaheader );
	}
	else
	{
		DBG_FILE(_S8("CNSmlHTTP::InvokeHttpMethodL KNsmlHttpUAHeader key error "));
	}
	CleanupStack::PopAndDestroy( rep ); 
	CleanupStack::PopAndDestroy( useragent );  
	SetHeaderL( hdr, HTTP::ECacheControl, KSmlCacheControl );
	SetHeaderL( hdr, HTTP::EAcceptCharset, KSmlAcceptCharSet );
	SetHeaderL( hdr, HTTP::EAcceptLanguage , KSmlAcceptLanguage );
		
	if( iSession == ESyncMLDSSession )//for ds session
	  { 
	  if(iServerAcceptEncoding == ExptDeflate)
	      {
	      SetHeaderL( hdr, HTTP::EContentEncoding , KSmlContentDeflate );
	      }
	      SetHeaderL( hdr, HTTP::EAcceptEncoding , KSmlContentDeflate );
	  }

	// Add headers and body data for methods that use request bodies
	// Content type header
	TBuf8<KMaxContentTypeSize> contTypeBuf;
	contTypeBuf.Copy( iMimeType->Des() );
	RStringF contTypeStr;
	CleanupClosePushL( contTypeStr );
	contTypeStr = iSess.StringPool().OpenFStringL( contTypeBuf );
	THTTPHdrVal contType( contTypeStr );
	hdr.SetFieldL( iSess.StringPool().StringF( HTTP::EContentType, 
	                RHTTPSession::GetTable()), contType );
	CleanupStack::PopAndDestroy(); // contTypeStr
	

	// Accept header
	TBuf8<KMaxContentTypeSize> acceptTypeBuf;
	acceptTypeBuf.Copy( iMimeType->Des() );
	RStringF acceptTypeStr; 
	CleanupClosePushL( acceptTypeStr );
	acceptTypeStr = iSess.StringPool().OpenFStringL( acceptTypeBuf );
	THTTPHdrVal acceptType( acceptTypeStr );
	hdr.SetFieldL( iSess.StringPool().StringF( HTTP::EAccept, 
	                RHTTPSession::GetTable()), acceptType );
	CleanupStack::PopAndDestroy(); // acceptTypeStr

    // Expect: 100-continue
    THTTPHdrVal expectVal( iSess.StringPool().StringF( HTTP::E100Continue, 
                    RHTTPSession::GetTable() ) );
    hdr.SetFieldL( iSess.StringPool().StringF( HTTP::EExpect, 
                    RHTTPSession::GetTable() ), expectVal );

	RStringF expectStr( iSess.StringPool().StringF( HTTP::EExpect, 
	                    RHTTPSession::GetTable() ) );
	hdr.RemoveField( expectStr );
	
	MHTTPDataSupplier* dataSupplier = this;
	iTrans.Request().SetBody( *dataSupplier );

	// submit the transaction
	iTrans.SubmitL();
	}

// -----------------------------------------------------------------------------
// CNSmlHTTP::GetResponseBodyL( TDes8& aStartPtr )
// 
// -----------------------------------------------------------------------------
//
void CNSmlHTTP::GetResponseBodyL( TDes8& aStartPtr )
	{
	if( iData != NULL )
		{
		if (aStartPtr.MaxLength() < (iData->Des()).Length())
		{	
			User::LeaveIfError( TNSmlError::ESmlStatusSizeMismatch );
		}
		else
		{
		    if ( (iSession == ESyncMLDSSession) && (iServerContentEncoding == ExptDeflate) )
		        {		    
		        TRAPD( err, DecompressL( aStartPtr ) );		    
				User::LeaveIfError( err );
		        }
		    else		    
		        {
		aStartPtr.Copy( iData->Des() );
		        }
		}
		}
	}

// -----------------------------------------------------------------------------
// CNSmlHTTP::SetHeaderL( RHTTPHeaders aHeaders, TInt aHdrField, 
// const TDesC8& aHdrValue )
// 
// -----------------------------------------------------------------------------
//
void CNSmlHTTP::SetHeaderL( 
    RHTTPHeaders aHeaders, 
    TInt aHdrField, 
    const TDesC8& aHdrValue )
	{
	RStringF valStr;
	CleanupClosePushL( valStr );
	valStr = iSess.StringPool().OpenFStringL( aHdrValue );
	THTTPHdrVal val( valStr );
	aHeaders.SetFieldL( iSess.StringPool().StringF( aHdrField, 
	                    RHTTPSession::GetTable() ), val );
	CleanupStack::PopAndDestroy(); // valStr	
	}


// ========================== OTHER EXPORTED FUNCTIONS =========================

// -----------------------------------------------------------------------------
// CreateCNSmlHTTPL()
// -----------------------------------------------------------------------------
//
EXPORT_C CNSmlHTTP* CreateCNSmlHTTPL()
	{
	return CNSmlHTTP::NewL();
	}

//End of File

