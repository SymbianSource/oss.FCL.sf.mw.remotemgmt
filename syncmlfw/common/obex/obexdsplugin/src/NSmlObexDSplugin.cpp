/*
* Copyright (c) 2002-2004 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  SyncML Obex plugin for data syncronization
*
*/



// INCLUDE FILES
#include <nsmlsyncalerthandlerif.h>
#include <nsmlconstants.h> 
#include "NSmlObexDSplugin.h"

// CONSTANTS
const TUint KAlertEComInterface = 0x101F9A2C;


// ============================= LOCAL FUNCTIONS ===============================
namespace 
	{
	TInt CompareAlertsByPriorityL( const CImplementationInformation& aP1,
	                               const CImplementationInformation& aP2)
		{
		TInt ret( 0 );
		TInt comp = aP1.OpaqueData().Compare( aP2.OpaqueData() );
		
		if ( comp > 0 )
			{
			ret = 1;
			}
		else if ( comp < 0 )
			{
			ret = -1;
			}
		return ret;
		}
	}

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CNSmlObexDSplugin::NewL()
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CNSmlObexDSplugin* CNSmlObexDSplugin::NewL()
    {
    //DBG_BEGIN();
    DBG_FILE( _S8("CNSmlObexDSplugin::NewL: begin") );
    CNSmlObexDSplugin* self = new (ELeave) CNSmlObexDSplugin();
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
	DBG_FILE( _S8("CNSmlObexDSplugin::NewL: end") );    
    return self;
    }

// -----------------------------------------------------------------------------
// CNSmlObexDSplugin::CNSmlObexDSplugin()
// C++ default constructor.
// -----------------------------------------------------------------------------
//
CNSmlObexDSplugin::CNSmlObexDSplugin():
  iPtr( 0, 0),
  iSendPtr( 0, 0 ),
  iConnectionAlive( ETrue )
  	{
	}

// -----------------------------------------------------------------------------
// CNSmlObexDSplugin::ConstructL()
// Symbian 2nd phase constructor.
// -----------------------------------------------------------------------------
//
void CNSmlObexDSplugin::ConstructL()
    {
	iZeroPkg = ETrue;
	iObexContent = ENSmlDataSync;
	iState = EIdle;
	iResponseError = KErrNone ;
    // create OBEX object to receive obex transfer
    iObexBufData = CBufFlat::NewL( 10*KBufferSize );
    iObexBufObject = CObexBufObject::NewL( iObexBufData );
	iSessCancelled = ETrue;
	iSessionActive = EFalse;
	iThreadId = TThreadId( 0 );
	/* Needs a new SRCS interface **/
	iBtAddr = TBTDevAddr(0);
	iBtName = KNullDesC().AllocL();

	iServiceTimer = new (ELeave) CPluginServiceTimer( this, KPluginTimeOutValue );
	iServiceTimer->ConstructL();
	CActiveScheduler::Add( iServiceTimer );
	
	iGetPacketWaiterAo = CNSmlObexPacketWaiterAsync::NewL( iSs, iObexBufData, iObexBufObject ); 	
	CActiveScheduler::Add( iGetPacketWaiterAo );
    }

// -----------------------------------------------------------------------------
// CNSmlObexDSplugin::AlertLibrary()
// virtual from MNSmlAlertObexPluginContext
// -----------------------------------------------------------------------------
//
RLibrary& CNSmlObexDSplugin::AlertLibrary()
	{
	return iAlertLib;
	}

// -----------------------------------------------------------------------------
// CNSmlObexDSplugin::AlertThreadId()
// virtual from MNSmlAlertObexPluginContext
// -----------------------------------------------------------------------------
//
TThreadId* CNSmlObexDSplugin::AlertThreadId()
	{
	return &iThreadId;
	}

// -----------------------------------------------------------------------------
// CNSmlObexDSplugin::IsOBEXActive()
// 
// -----------------------------------------------------------------------------
//
TBool CNSmlObexDSplugin::IsOBEXActive()
    {
	return iSessionActive;
    }

// -----------------------------------------------------------------------------
// CNSmlObexDSplugin::~CNSmlObexDSplugin()
// Destructor.
// -----------------------------------------------------------------------------
//
CNSmlObexDSplugin::~CNSmlObexDSplugin()
    {
	DBG_FILE( _S8("CNSmlObexDSplugin::~CNSmlObexDSplugin(): begin") );
	
	Disconnect();

	if( iListener )
		{
		iListener->iSessCancelled = iSessCancelled;
		delete iListener;
		iListener = NULL;
		}
	
	if ( iServerThread )
		{
		//check, if server thread still running.
		if ( iServerThread->ExitType() == EExitPending ) 
			{
			//waiting for the server to finish.
			TRequestStatus serverLogon;
			iServerThread->Logon( serverLogon );
			User::WaitForRequest( serverLogon );
			}
		iServerThread->Close();
		delete iServerThread;
		iServerThread = NULL;
		}
	
    delete iObexBufData;
    iObexBufData = NULL;

    delete iObexBufObject;
    iObexBufObject = NULL;

	delete iData;
	iData = NULL;

	delete iBtName;
	if ( iAlertLib.Handle() )
		{
		iAlertLib.Close();
		}

	if( iServiceTimer )
		{
		iServiceTimer->Cancel();
		}
	
	delete iServiceTimer;
	iServiceTimer = NULL;
	
	if(iGetPacketWaiterAo)
		{
		delete iGetPacketWaiterAo;
		iGetPacketWaiterAo = NULL;
		}
	
	DBG_FILE( _S8("CNSmlObexDSplugin::~CNSmlObexDSplugin(): end") );
    }

// -----------------------------------------------------------------------------
// CNSmlObexDSplugin::ErrorIndication( TInt aError )
// 
// -----------------------------------------------------------------------------
//
#ifdef __NSML_DEBUG__
void CNSmlObexDSplugin::ErrorIndication( TInt aError )
    {
	iZeroPkg = ETrue;
	DBG_FILE_CODE( aError, _S8("CNSmlObexDSplugin::ErrorIndication...") );
	Disconnect();
    }
#else
void CNSmlObexDSplugin::ErrorIndication( TInt /*aError*/ )
    {
	iZeroPkg = ETrue;
	Disconnect();
    }
#endif

// -----------------------------------------------------------------------------
// CNSmlObexDSplugin::TransportUpIndication()
// 
// -----------------------------------------------------------------------------
//
void CNSmlObexDSplugin::TransportUpIndication()
    {
	DBG_FILE( _S8("CNSmlObexDSplugin::TransportUpIndication...") );
    }

// -----------------------------------------------------------------------------
// CNSmlObexDSplugin::TransportDownIndication()
// 
// -----------------------------------------------------------------------------
//
void CNSmlObexDSplugin::TransportDownIndication()
    {
	DBG_FILE( _S8("CNSmlObexDSplugin::TransportDownIndication...") );
	Disconnect();
    }

// -----------------------------------------------------------------------------
// CNSmlObexDSplugin::ObexConnectIndication( const TObexConnectInfo& /*aRemoteInfo*/, const TDesC8& /*aInfo*/ )
// 
// -----------------------------------------------------------------------------
//
void CNSmlObexDSplugin::ObexConnectIndication  (const TObexConnectInfo& /*aRemoteInfo*/, const TDesC8& /*aInfo*/)
    {
	_DBG_FILE("CNSmlObexDSplugin::ObexConnectIndication [w43 20.10.2003]");	
	TInt err( KErrNone );
	iStopped = EFalse;

	iStartTimer = ETrue;
    TSockAddr addr; 
    
    if ( iObexServer && iMediaType == ESrcsMediaBT )
        {
        iObexServer->RemoteAddr( addr );
    
        TRfcommSockAddr btAddr( addr ); 
        iBtAddr = btAddr.BTAddr();
        TInt nameError( KErrNone );
        TRAP( nameError, BluetoothNameByAddressL( addr ) );
        if ( nameError )
            {
            iResponseError = ConvertError( nameError ) ; 
			_DBG_FILE("CNSmlObexDSplugin::ObexConnectIndication [Error in getting Bluetooth Name]") ;
            }
        }
    if (iResponseError == KErrNone)
	    {
	      if(!iGetPacketWaiterAo)
	    	{
	    	TRAP_IGNORE (iGetPacketWaiterAo = CNSmlObexPacketWaiterAsync::NewL( iSs,
		   																		iObexBufData,
		   																		iObexBufObject ) ) ;
		   	CActiveScheduler::Add( iGetPacketWaiterAo );
			
			iGetPacketWaiterAo->SetObexServer(iObexServer) ; // Added for setting the server
	    	}
	 
		if( iListener )
			{
			_DBG_FILE("delete iListener");	
			delete iListener;
			iListener = NULL;
			}
		
		err = iSs.Connect();
		if( err == KErrNotFound )
			{
			// obexcommserver was not running, creating one into a new thread
			if (iServerThread)
				{
				iServerThread->Close();
				delete iServerThread;
				iServerThread = NULL;
				}
			
			iServerThread = new RThread();
			
			if ( iServerThread )
				{
				RSemaphore signal;
				signal.CreateLocal(0);
				
				err = iServerThread->Create( _L("ObexCommServerThread"),
											 CreateServerL,
											 1024*6,
											 KMinHeapSize,
											 KMinHeapSize*320,
											 &signal );
				if ( err == KErrNone )
					{
					iServerThread->Resume();
					signal.Wait();
					err = iSs.Connect();
					}
					
				signal.Close();
				}
			else
				{
				err = KErrNoMemory;
				}
			}
		
		if( err == KErrNone )
			{
			iSessCancelled = EFalse;
			iListener = new CDisListener( iSs, this );
			if (!iListener)
				{
				err = KErrNoMemory;
				}
				
			if( err == KErrNone )
				{
				iListener->ListenDisconnect();
				DBG_FILE( _S8("CNSmlObexDSplugin iServiceTimer->StopTimer()") );
				iServiceTimer->Cancel();
				iServiceTimer->StopTimer();
				}
			}
			
			if ( err )
			  {
			  iConnectionAlive = EFalse;
			  }
			else
			  {
			  iConnectionAlive = ETrue;
			  }
	    iResponseError = ConvertError( err );
	    DBG_FILE_CODE( iResponseError, _S8("CNSmlObexDSplugin::ObexConnectIndication [Error]..") );	
		}
    }

// -----------------------------------------------------------------------------
// CNSmlObexDSplugin::ObexDisconnectIndication( const TDesC8& /*aInfo*/ )
// 
// -----------------------------------------------------------------------------
//
void CNSmlObexDSplugin::ObexDisconnectIndication( const TDesC8& /*aInfo*/ )
    {
	_DBG_FILE("CNSmlObexDSplugin::ObexDisconnectIndication");
	Disconnect();
	}

// -----------------------------------------------------------------------------
// CNSmlObexDSplugin::PutRequestIndication()
// 
// -----------------------------------------------------------------------------
//
void CNSmlObexDSplugin::PutRequestIndication ()
    {
	_DBG_FILE("CNSmlObexDSplugin::PutRequestIndication");
	
	iState = EIdle;
	if ( iObexBufObject )
		{
		iObexBufData->Reset();
		iObexBufObject->Reset();
		}
	if( iStopped )
		{
		_DBG_FILE("CNSmlObexDSplugin service unavailable");
		iObexServer->RequestIndicationCallback(NULL);
		}
	else 
		{
		_DBG_FILE("CNSmlObexDSplugin::PutRequestIndication - END [service available]");		
		iObexServer->RequestIndicationCallback(iObexBufObject);		
		}	
    }

// -----------------------------------------------------------------------------
// CNSmlObexDSplugin::PutPacketIndication()
// 
// -----------------------------------------------------------------------------
//
TInt CNSmlObexDSplugin::PutPacketIndication()
    {
	_DBG_FILE("CNSmlObexDSplugin::PutPacketIndication");
	TInt bytes( this->iObexBufObject->BytesReceived() );
	DBG_FILE_CODE( bytes, _S8("reveived bytes") );
	if( iStopped )
		{
		_DBG_FILE("CNSmlObexDSplugin service unavailable");
		return KErrIrObexRespServiceUnavail;
		}
	return KErrNone;
    }

// -----------------------------------------------------------------------------
// CNSmlObexDSplugin::PutCompleteIndication()
// 
// -----------------------------------------------------------------------------
//
void CNSmlObexDSplugin::PutCompleteIndication () 
    {
	_DBG_FILE("CNSmlObexDSplugin::PutCompleteIndication");
	DBG_FILE_CODE( iResponseError, _S8("CNSmlObexDSplugin::PutCompleteIndication...iResponseError...") );
	if (iResponseError == KErrNone) 
		{
		if ( !iConnectionAlive )
		  	{
			_DBG_FILE("CNSmlObexDSplugin::PutCompleteIndication - connection not alive!");
			iResponseError = KErrServerBusy ;  
		  	}
	
		if (iResponseError == KErrNone)  
			{
			TInt err( KErrNone );

			err = CheckMimeType();

			_DBG_FILE("CNSmlObexDSplugin After CheckMimeType");

			if( err == KErrNone )
				{
				if ( iObexBufData->Size() > KNSmlLocalSyncWorkspaceSize )
				    {
				    _DBG_FILE("CNSmlObexDSplugin: Received package too large!");
					iResponseError = KErrIrObexRespReqEntityTooLarge ;
					}
				if (iResponseError == KErrNone) 
					{
					if ( iZeroPkg )
						{
						_DBG_FILE("CNSmlObexDSplugin..Forwarding Zero Pkg");
						DBG_DUMP((void*)iObexBufData->Ptr(0).Ptr(), iObexBufData->Size(), _S8("Zero Pkg") );

						#ifdef __NSML_DEBUG__
						_DBG_FILE("CNSmlObexDSplugin..Zero Pkg: CWbxml2XmlConverter::ConvertL() begin");
						CWbxml2XmlConverter* c = NULL;
						TRAP( err, c = CWbxml2XmlConverter::NewL() );
						if (c)
							{
							TRAP( err, c->ConvertL(iObexBufData->Ptr(0).Ptr(), iObexBufData->Size()) );
							DBG_DUMP((void*)c->Document().Ptr(), c->Document().Length(), _S8("Zero Pkg (XML)") );
							delete c;
							}
						_DBG_FILE("CNSmlObexDSplugin..Zero Pkg: CWbxml2XmlConverter::ConvertL() end");
						#endif // __NSML_DEBUG__
				
						TRAP( err, err = ForwardZeroPkgL( iObexContent ) );
						iResponseError = err ;
						iZeroPkg = EFalse;
						}
					else
						{
						_DBG_FILE("CNSmlObexDSplugin..SetReceivedPacket");
						DBG_DUMP((void*)iObexBufData->Ptr(0).Ptr(), iObexBufData->Size(), _S8("CNSmlObexDSplugin SetReceivedPacket") );

						#ifdef __NSML_DEBUG__
						_DBG_FILE("CNSmlObexDSplugin..SetReceivedPacket: CWbxml2XmlConverter::ConvertL() begin");
						CWbxml2XmlConverter* c = NULL;
						TRAP( err, c = CWbxml2XmlConverter::NewL() );
						if (c)
							{
							TRAP( err, c->ConvertL(iObexBufData->Ptr(0).Ptr(), iObexBufData->Size()) );
							DBG_DUMP((void*)c->Document().Ptr(), c->Document().Length(), _S8("SetReceivedPacket (XML)") );
							delete c;
							}
						_DBG_FILE("CNSmlObexDSplugin..SetReceivedPacket: CWbxml2XmlConverter::ConvertL() end");
						#endif // __NSML_DEBUG__
						
						if( !iStopped )
							{
							this->iObexBufObject->BytesReceived();
							this->iSs.SetReceivedPacket( this->iObexBufData->Ptr( 0 ) );
							}
						else
							{
							err = KErrAccessDenied;
							iResponseError = err ; 
							}
						}
					}
				}
			else if ( err == KErrNotSupported )
			    {
		    	_DBG_FILE("CNSmlObexDSplugin: Mimetype not supported!");
			    iResponseError = KErrIrObexRespUnsupMediaType;
			    }
			else
				{
				iResponseError = ConvertError( err );
				}
				
			if (iResponseError != KErrNone) 
				{
				iObexServer->RequestCompleteIndicationCallback(ERespNotAcceptable);
				}
			else
				{
				iObexServer->RequestCompleteIndicationCallback(ERespSuccess);
				}
			}		
		}			
	}

// -----------------------------------------------------------------------------
// CNSmlObexDSplugin::GetRequestIndication( CObexBaseObject* /*aRequiredObject*/ )
// 
// -----------------------------------------------------------------------------
//
void CNSmlObexDSplugin::GetRequestIndication (CObexBaseObject* /*aRequestedObject*/)
    {
	_DBG_FILE("CNSmlObexDSplugin::GetRequestIndication-- Begin");

	iObexBufObject->Reset();

	iState = EGetSendPacket;
	DBG_FILE_CODE( iResponseError, _S8("CNSmlObexDSplugin::GetRequestIndication...ResponseError : ") );
	if (iResponseError == KErrNone)
		{
		if(!iGetPacketWaiterAo)
			{
			_DBG_FILE( "CNSmlObexDSplugin::GetRequestIndication...NULL - 1 ");
			iState = EIdle;
			iObexServer->RequestIndicationCallback(NULL);
			}
		else
			{
			_DBG_FILE( "CNSmlObexDSplugin::GetRequestIndication...iGetPacketWaiterAo->WaitForGetData");
			iState = EIdle;
			iGetPacketWaiterAo->WaitForGetData();
			_DBG_FILE("CNSmlObexDSplugin::GetRequestIndication- End with WaitforGetdata");
			return ;				
			}
		}
		else
		{
		_DBG_FILE( "CNSmlObexDSplugin::GetRequestIndication...NULL - 2 ");
		iState = EIdle;
		iObexServer->RequestIndicationCallback(NULL);			
		}
	_DBG_FILE("CNSmlObexDSplugin::GetRequestIndication- End abnormal");
   }

// -----------------------------------------------------------------------------
// CNSmlObexDSplugin::GetPacketIndication()
// 
// -----------------------------------------------------------------------------
//
TInt CNSmlObexDSplugin::GetPacketIndication()
    {
	_DBG_FILE("CNSmlObexDSplugin::GetPacketIndication");
	if( iStopped )
		{
		_DBG_FILE("CNSmlObexDSplugin service unavailable");
		return KErrIrObexRespServiceUnavail;
		}
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CNSmlObexDSplugin::GetCompleteIndication()
// 
// -----------------------------------------------------------------------------
void CNSmlObexDSplugin::GetCompleteIndication ()
    {
	_DBG_FILE("CNSmlObexDSplugin::GetCompleteIndication");
    iResponseError = KErrNone ;
    iObexServer->RequestCompleteIndicationCallback(ERespSuccess);
    }

// -----------------------------------------------------------------------------
// CNSmlObexDSplugin::SetPathIndication(const CObex::TSetPathInfo& /*aPathInfo*/, const TDesC8& /*aInfo*/)
// 
// -----------------------------------------------------------------------------
void CNSmlObexDSplugin::SetPathIndication(const CObex::TSetPathInfo& /*aPathInfo*/, const TDesC8& /*aInfo*/)
    {
    _DBG_FILE("CNSmlObexDSplugin::SetPathIndication");
    iResponseError = KErrNone ;
    }

// -----------------------------------------------------------------------------
// CNSmlObexDSplugin::AbortIndication()
// 
// -----------------------------------------------------------------------------
void CNSmlObexDSplugin::AbortIndication()
    {
	_DBG_FILE("CNSmlObexDSplugin::AbortIndication");
	Disconnect();
    }

// -----------------------------------------------------------------------------
// CNSmlObexDSplugin::Disconnect()
// 
// -----------------------------------------------------------------------------
//
void CNSmlObexDSplugin::Disconnect()
    {
	_DBG_FILE("CNSmlObexDSplugin::Disconnect Begin");
	iZeroPkg = ETrue;

	if( iStartTimer != EFalse )
		{
		DBG_FILE( _S8("CNSmlObexDSplugin iServiceTimer->StartTimer()") );
		iServiceTimer->Cancel();
		iServiceTimer->StartTimer();
		}

	iState = EIdle;
	
	if( !iSessCancelled )
		{
		_DBG_FILE("CNSmlObexDSplugin::Calling iGetPacketWaiterAo DoCancel");
		iGetPacketWaiterAo->Cancel();
		_DBG_FILE("calling iSs.Disconnect()");
		iSs.Disconnect();
		iSs.Close();
		iSessCancelled = ETrue;		
		}
	
	if ( iServerThread )
		{
		_DBG_FILE("CNSmlObexDSplugin::Disconnect() closing thread");
		//check, if server thread still running.
		if ( iServerThread->ExitType() == EExitPending ) 
			{
			//waiting for the server to finish.
			TRequestStatus serverLogon;
			iServerThread->Logon( serverLogon );
			User::WaitForRequest( serverLogon );
			}
		iServerThread->Close();
		delete iServerThread;
		iServerThread = NULL;
		}
	
	if(iGetPacketWaiterAo)
		{
		delete iGetPacketWaiterAo;
		iGetPacketWaiterAo = NULL;
		}
	_DBG_FILE("CNSmlObexDSplugin::Disconnect End");
    }

// -----------------------------------------------------------------------------
// CNSmlObexDSplugin::CheckMimeType()
// 
// -----------------------------------------------------------------------------
//
TInt CNSmlObexDSplugin::CheckMimeType()
	{ 	
	_DBG_FILE("CNSmlObexDSplugin::CheckMimeType() - mimetype in dump...");
	
	TBuf8<256> mimeType;
	mimeType.Copy( this->iObexBufObject->Type() );
	TrimRightSpaceAndNull( mimeType );
	mimeType.LowerCase();

	DBG_DUMP( (void*)mimeType.Ptr(), mimeType.Length(), _S8("Received mimetype") );

	if ( (mimeType.Compare( KDataSyncMIME ) == 0) ||
		 (mimeType.Compare( KDataSyncDSAlertNotification ) == 0) )
		{		
		_DBG_FILE("CNSmlObexDSplugin... KDataSyncMIME OBEX content");
		iObexContent = ENSmlDataSync;
		return KErrNone;
		}
	else
		{
		_DBG_FILE("Unsupported mime type");
		return KErrNotSupported;
		}
	}

// -----------------------------------------------------------------------------
// CNSmlObexDSplugin::TrimRightSpaceAndNull()
// 
// -----------------------------------------------------------------------------
//
void CNSmlObexDSplugin::TrimRightSpaceAndNull( TDes8& aDes ) const
{
	aDes.TrimRight();
	if ( aDes.Length() > 0 )
		{
		if ( aDes[aDes.Length() - 1] == NULL )
			{
			aDes.SetLength( aDes.Length() - 1 );
			} 
		}
}

// -----------------------------------------------------------------------------
// CNSmlObexDSplugin::ForwardZeroPkgL( TNSmlObexContent aSyncSelection )
// 
// -----------------------------------------------------------------------------
//
TInt CNSmlObexDSplugin::ForwardZeroPkgL( TNSmlObexContent aSyncSelection )
	{
	_DBG_FILE("CNSmlObexDSplugin::ForwardZeroPkgL()");
	TInt err( ENSmlErrNotConsumed );

	if ( aSyncSelection == ENSmlDataSync )
		{
		CNSmlSyncAlertHandler* alert = 0;
		RImplInfoPtrArray implArray;
		TUid ifUid = { KAlertEComInterface };
		CleanupStack::PushL( PtrArrCleanupItemRArr( 
								CImplementationInformation, &implArray ) );
		REComSession::ListImplementationsL( ifUid, implArray );

		// sort alert handlers by priority (= opaque_data)
		implArray.Sort( TLinearOrder< CImplementationInformation >( CompareAlertsByPriorityL ));

		for( TInt j = 0; err == ENSmlErrNotConsumed && j < implArray.Count(); j++ )
			{
			alert = CNSmlSyncAlertHandler::NewL( implArray[ j ]->ImplementationUid() );

			if ( alert )
				{
				CleanupStack::PushL( alert );
				err = alert->StartSyncL( this->iObexBufData->Ptr( 0 ),
					iMediaType, 
					iBtAddr, 
					*iBtName, 
					this );
				CleanupStack::PopAndDestroy( alert );
				alert = 0;
				}
			}
		REComSession::FinalClose();
		CleanupStack::PopAndDestroy();	// implArray
		}
	else
		{
		err = KErrNotSupported;
		}
	return err;
	}

// -----------------------------------------------------------------------------
// CNSmlObexDSplugin::SetMediaType( TSrcsMediaType aMediaType )
// 
// -----------------------------------------------------------------------------
//
void CNSmlObexDSplugin::SetMediaType( TSrcsMediaType aMediaType )
    {
	DBG_FILE( _S8("CNSmlObexDSplugin::SetMediaType()") );
	iMediaType = aMediaType;
    }

// -----------------------------------------------------------------------------
// CNSmlObexDSplugin::CreateServer( TAny* aPtr )
// 
// -----------------------------------------------------------------------------
//
TInt CNSmlObexDSplugin::CreateServerL( TAny* aPtr )
	{
	__UHEAP_MARK;
	CTrapCleanup* cleanup=CTrapCleanup::New(); // get clean-up stack
	CActiveScheduler* scheduler = new (ELeave) CActiveScheduler();
	CActiveScheduler::Install(scheduler);	
	TRAPD( err, StartServerL( aPtr ) );
	__ASSERT_ALWAYS(err == KErrNone, User::Panic(_L("OBEXCommServer"), err));
	delete scheduler;
	delete cleanup; // destroy clean-up stack
	__UHEAP_MARKEND;
//	DBG_FILE(_S8("CNSmlObexDSplugin::CreateServerL"));	
	return KErrNone;
	}

// -----------------------------------------------------------------------------
// CNSmlObexDSplugin::StartServer( TAny* aPtr )
// 
// -----------------------------------------------------------------------------
//
void CNSmlObexDSplugin::StartServerL( TAny* aPtr )
	{
//	DBG_FILE(_S8("CNSmlObexDSplugin::StartServerL - Begin"));	
	CNSmlObexCommServer* server = CNSmlObexCommServer::NewL( KNSmlDSObexCommServerName() );
	RSemaphore* s = (RSemaphore*)aPtr;
	s->Signal();
	CActiveScheduler::Start();
//	DBG_FILE(_S8("CNSmlObexDSplugin::StartServerL - End"));	
	delete server;
	}

// -----------------------------------------------------------------------------
// CNSmlObexDSplugin::ConvertError( TInt aError )
//
// -----------------------------------------------------------------------------
//
TInt CNSmlObexDSplugin::ConvertError( TInt aError )
	{
	DBG_FILE_CODE( aError, _S8("ConvertError...") );
	TInt obexErr ( KErrIrObexRespInternalError );

	switch (aError)
	{
	case KErrNone:
		obexErr = KErrNone;
		break;

	case KErrNotFound:
		obexErr = KErrIrObexRespNotFound;
		break;

	case KErrGeneral:
	case KErrCancel:
	case KErrNoMemory:
		obexErr = KErrIrObexRespInternalError;
		break;

	case KErrNotSupported:
		obexErr = KErrIrObexRespBadRequest;
		break;

	case KErrArgument:
	case KErrTotalLossOfPrecision:
	case KErrBadHandle:
	case KErrOverflow:
	case KErrUnderflow:
	case KErrAlreadyExists:
	case KErrPathNotFound:
	case KErrDied:
	case KErrInUse:
	case KErrServerTerminated:
		obexErr = KErrIrObexRespInternalError;
		break;

	case KErrServerBusy:
		obexErr = KErrIrObexRespServiceUnavail;
		break;

	case KErrCompletion:
	case KErrNotReady:
	case KErrUnknown:
	case KErrCorrupt:
	case KErrAccessDenied:
	case KErrLocked:
	case KErrWrite:
	case KErrDisMounted:
	case KErrEof:
	case KErrDiskFull:
	case KErrBadDriver:
	case KErrBadName:
	case KErrCommsLineFail:
	case KErrCommsFrame:
	case KErrCommsOverrun:
	case KErrCommsParity:
	case KErrTimedOut:
	case KErrCouldNotConnect:
	case KErrCouldNotDisconnect:
	case KErrBadLibraryEntryPoint:
	case KErrBadDescriptor:
	case KErrAbort:
	case KErrTooBig:
	case KErrDivideByZero:
	case KErrBadPower:
	case KErrDirFull:
	case KErrHardwareNotAvailable:
		obexErr = KErrIrObexRespInternalError;
		break;
	default:
		obexErr = KErrIrObexRespInternalError;
		break;
	};
	DBG_FILE_CODE( obexErr, _S8("ConvertError returned...") );
	return obexErr;
}

// -----------------------------------------------------------------------------
// CNSmlObexDSplugin::SetObexServer( CObexServer* aObexServer )
//
// -----------------------------------------------------------------------------
//
TInt CNSmlObexDSplugin::SetObexServer( CObexServer* aObexServer )
    {
    DBG_FILE(_S8("CNSmlObexDSplugin::SetObexServer - Begin"));	
    iObexServer = aObexServer;
	
	iGetPacketWaiterAo->SetObexServer(aObexServer) ; // Added for setting the server
	
    return iObexServer->Start(this) ;
    }

// -----------------------------------------------------------------------------
// CNSmlObexDSplugin::BluetoothNameByAddressL( TSockAddr aAddr )
//
// -----------------------------------------------------------------------------
//
void CNSmlObexDSplugin::BluetoothNameByAddressL( TSockAddr aAddr )
    {
	RSocketServ	sockServ;
	RHostResolver resolver;	
	TNameEntry result;
	
	User::LeaveIfError( sockServ.Connect() );
	CleanupClosePushL( sockServ );
	
	User::LeaveIfError( resolver.Open( sockServ, KBTAddrFamily, KBTLinkManager ));
    CleanupClosePushL( resolver );
	
    TInquirySockAddr addr( aAddr );
	addr.SetIAC( KGIAC );
	addr.SetAction( KHostResName );

	delete iBtName;
	iBtName = NULL; 

	if ( resolver.GetByAddress( addr, result ) == KErrNone )
	    {
	    iBtName = result().iName.AllocL(); 
 	    }
	else 
	    {
	    iBtName = KNullDesC().AllocL();
	    }
	    
	CleanupStack::PopAndDestroy( 2, &sockServ );   //  resolver
    }

// -----------------------------------------------------------------------------
// CNSmlObexDSplugin::CancelIndicationCallback()
//
// -----------------------------------------------------------------------------
//
void CNSmlObexDSplugin::CancelIndicationCallback()
	{
	DBG_FILE(_S8("CNSmlObexDSplugin::CancelIndicationCallback"));	
	}

// =================== CNSmlObexPacketWaiterAsync class Implementation  =====================

// -----------------------------------------------------------------------------
// CNSmlObexPacketWaiterAsync* CNSmlObexPacketWaiterAsync::NewL(RNSmlDSObexServerSession& aSession,
//	CBufFlat* aObexBufData,	CObexBufObject* aObexBufObject)
//
// -----------------------------------------------------------------------------
//
CNSmlObexPacketWaiterAsync* CNSmlObexPacketWaiterAsync::NewL (RNSmlDSObexServerSession& aSession,
									  CBufFlat* aObexBufData,
									  CObexBufObject* aObexBufObject)
	{
	CNSmlObexPacketWaiterAsync* self = new (ELeave) CNSmlObexPacketWaiterAsync( aSession,
																				aObexBufData,
																				aObexBufObject );
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self); // self
	return self;	
	}

// -----------------------------------------------------------------------------
// CNSmlObexPacketWaiterAsync::CNSmlObexPacketWaiterAsync(RNSmlDSObexServerSession& aSession,
//	CBufFlat* aObexBufData,	CObexBufObject* aObexBufObject)
//
// -----------------------------------------------------------------------------
//
CNSmlObexPacketWaiterAsync::CNSmlObexPacketWaiterAsync(RNSmlDSObexServerSession& aSession,
	CBufFlat* aObexBufData,	CObexBufObject* aObexBufObject) : CActive( EPriorityStandard ),
	iSession(aSession), iObexBufData(aObexBufData), iObexBufObject(aObexBufObject), iPtr(NULL, 0) 
	{
	}

// -----------------------------------------------------------------------------
// CNSmlObexPacketWaiterAsync::ConstructL()
//
// -----------------------------------------------------------------------------
// 
void CNSmlObexPacketWaiterAsync::ConstructL()
	{
	iState = EIdle ;
    iData = HBufC8::NewL( KNSmlLocalSyncWorkspaceSize );
	iPtr.Set( iData->Des() );
	}

// -----------------------------------------------------------------------------
// CNSmlObexPacketWaiterAsync::~CNSmlObexPacketWaiterAsync ()
//
// -----------------------------------------------------------------------------
// 
CNSmlObexPacketWaiterAsync::~CNSmlObexPacketWaiterAsync ()
	{
	DBG_FILE( _S8("CNSmlObexPacketWaiterAsync::~CNSmlObexPacketWaiterAsync") );
	//Cancel() ;
	delete iData;
	}
	
// -----------------------------------------------------------------------------
// CNSmlObexPacketWaiterAsync::WaitForGetData()
//
// -----------------------------------------------------------------------------
// 
void CNSmlObexPacketWaiterAsync::WaitForGetData()
	{
	_DBG_FILE("CNSmlObexPacketWaiterAsync WaitForGetData: start");
	iState = EGetting ;
	iStatus = KRequestPending ;
	TRequestStatus &lRequestStatus = iStatus;
	iPtr.SetLength(0);
	iSession.GetSendPacket( iPtr, lRequestStatus );
	if( !IsActive() )
		{
		DBG_FILE( _S8("CNSmlObexPacketWaiterAsync::WaitForGetData : Before SetActive") );
		SetActive();
		DBG_FILE( _S8("CNSmlObexPacketWaiterAsync::WaitForGetData : After SetActive") );
		}
	_DBG_FILE("CNSmlObexPacketWaiterAsync WaitForGetData: end");
	return;
	}
	
// -----------------------------------------------------------------------------
// void CNSmlObexPacketWaiterAsync::RunL()
//
// -----------------------------------------------------------------------------
//	
void CNSmlObexPacketWaiterAsync::RunL()
	{
	TInt err( KErrNone );
	iState = EIdle ;
	if ( iStatus == KErrNone)
		{
		iObexBufData->Reset();
		_DBG_FILE( "CNSmlObexPacketWaiterAsync::RunL...iObexBufData->InsertL");
		TRAP( err, iObexBufData->InsertL( 0, this->Data() ) );
		
		if ( err != KErrNone )
			{
			_DBG_FILE( "CNSmlObexPacketWaiterAsync::RunL...err != KErrNone - 1");
			iObexServer->RequestIndicationCallback(NULL);
			}
		else
			{
			#ifdef __NSML_DEBUG__
			_DBG_FILE("CNSmlObexPacketWaiterAsync::RunL: CWbxml2XmlConverter begin");
			CWbxml2XmlConverter* c = NULL;
			TRAP( err, c = CWbxml2XmlConverter::NewL() );
			if (c)
				{
				TRAP( err, c->ConvertL(iObexBufData->Ptr(0).Ptr(), iObexBufData->Size()) );
				DBG_DUMP((void*)c->Document().Ptr(), c->Document().Length(), _S8("CNSmlObexPacketWaiterAsync GetRequestIndication (XML)") );
				delete c;
				}
			_DBG_FILE("CNSmlObexPacketWaiterAsync::RunL: CWbxml2XmlConverter end");
			#endif // __NSML_DEBUG__


			TRAP( err, iObexBufObject->SetDataBufL( iObexBufData ) );
			if ( err != KErrNone )
				{
				_DBG_FILE( "CNSmlObexPacketWaiterAsync::RunL...err != KErrNone - 2");
				iObexServer->RequestIndicationCallback(NULL);
				}
			else
				{
				_DBG_FILE( "CNSmlObexPacketWaiterAsync::RunL...iObexBufObject");
				iObexServer->RequestIndicationCallback(iObexBufObject);
				}			    
			}					
		}
	else
		{
		_DBG_FILE( "CNSmlObexPacketWaiterAsync::RunL...err != KErrNone - 3");
		iObexServer->RequestIndicationCallback(NULL);
		}
	}

// -----------------------------------------------------------------------------
// void CNSmlObexPacketWaiterAsync::DoCancel()
//
// -----------------------------------------------------------------------------
//
void CNSmlObexPacketWaiterAsync::DoCancel()
	{
	DBG_FILE( _S8("CNSmlObexPacketWaiterAsync::DoCancel") );
	if (iState == EGetting)
		{
		DBG_FILE( _S8("CNSmlObexPacketWaiterAsync::CancelGetSendPacket") );
		iSession.CancelGetSendPacket();	
		DBG_FILE( _S8("CNSmlObexPacketWaiterAsync::RequestIndicationCallback(NULL)") );
		iObexServer->RequestIndicationCallback(NULL);
		iState = EIdle ;
		}
	}

// -----------------------------------------------------------------------------
// void CNSmlObexPacketWaiterAsync::SetObexServer( CObexServer* aObexServer )
//
// -----------------------------------------------------------------------------
//
void CNSmlObexPacketWaiterAsync::SetObexServer( CObexServer* aObexServer )
	{
	iObexServer = aObexServer;
	}

// ========================== OTHER EXPORTED FUNCTIONS =========================

//End of File

