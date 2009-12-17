/*
* Copyright (c) 2004 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  SyncML Obex plugin for Device Management
*
*/


#include <nsmlconstants.h>
#include <NSmlPrivateAPI.h>
#include <nsmlsyncalerthandlerif.h>
#include "NSmlObexDMplugin.h"

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CNSmlObexDMplugin::NewL()
// 
// -----------------------------------------------------------------------------
//
EXPORT_C CNSmlObexDMplugin* CNSmlObexDMplugin::NewL()
    {
    DBG_FILE( _S8("CNSmlObexDMplugin::NewL: begin") );
    CNSmlObexDMplugin* self = new (ELeave) CNSmlObexDMplugin();
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    DBG_FILE( _S8("CNSmlObexDMplugin::NewL: end") );
    return self;
    }

// -----------------------------------------------------------------------------
// CNSmlObexDMplugin::CNSmlObexDMplugin()
// 
// -----------------------------------------------------------------------------
//
CNSmlObexDMplugin::CNSmlObexDMplugin(): iPtr( 0, 0), iSendPtr( 0, 0 )
	{
	}

// -----------------------------------------------------------------------------
// CNSmlObexDMplugin::ConstructL()
// 
// -----------------------------------------------------------------------------
//
void CNSmlObexDMplugin::ConstructL()
    {
	iZeroPkg = ETrue;
	iObexContent =  ENSmlDeviceManagement;
	iState = EIdle;
    // create OBEX object to receive obex transfer
    iObexBufData = CBufFlat::NewL( 10*KBufferSize );
    iObexBufObject = CObexBufObject::NewL( iObexBufData );
	iSessCancelled = ETrue;
	iSessionActive = EFalse;

	iServiceTimer = new (ELeave) CPluginServiceTimer( this, KPluginTimeOutValue );
	iServiceTimer->ConstructL();
	CActiveScheduler::Add( iServiceTimer );

	iGetPacketWaiter = new(ELeave) CNSmlObexPacketWaiter( iSs ); 
	iGetPacketWaiter->ConstructL();
	}

// -----------------------------------------------------------------------------
// CNSmlObexDMplugin::GetServerCallback()
// Get MObexServerNotify object to bind to CObexServer
// -----------------------------------------------------------------------------
//
MObexServerNotify* CNSmlObexDMplugin::GetServerCallback()
    {
	return this;
    }

// -----------------------------------------------------------------------------
// CNSmlObexDMplugin::IsOBEXActive()
// 
// -----------------------------------------------------------------------------
//
TBool CNSmlObexDMplugin::IsOBEXActive()
    {
	return iSessionActive;
    }

// -----------------------------------------------------------------------------
// CNSmlObexDMplugin::~CNSmlObexDMplugin()
// 
// -----------------------------------------------------------------------------
//
CNSmlObexDMplugin::~CNSmlObexDMplugin()
    {
	DBG_FILE( _S8("CNSmlObexDMplugin::~CNSmlObexDMplugin(): begin") );
	Disconnect();

    delete iObexBufData;
    iObexBufData = NULL;

    delete iObexBufObject;
    iObexBufObject = NULL;

	if( iListener )
		{
		iListener->iSessCancelled = iSessCancelled;
		delete iListener;
		iListener = NULL;
		}

	delete iData;
	iData = NULL;

	if( iServiceTimer )
		{
		iServiceTimer->Cancel();
		}
	
	delete iServiceTimer;
	iServiceTimer = NULL;

	delete iGetPacketWaiter;
	iGetPacketWaiter = NULL;

	DBG_FILE( _S8("CNSmlObexDMplugin::~CNSmlObexDMplugin(): end") );
    }

// -----------------------------------------------------------------------------
// CNSmlObexDMplugin::ErrorIndication( TInt aError )
// 
// -----------------------------------------------------------------------------
//
void CNSmlObexDMplugin::ErrorIndication( TInt aError )
    {
	iZeroPkg = ETrue;
	DBG_FILE_CODE( aError, _S8("CNSmlObexDMplugin::ErrorIndication...") );
	Disconnect();
    }

// -----------------------------------------------------------------------------
// CNSmlObexDMplugin::TransportUpIndication()
// 
// -----------------------------------------------------------------------------
//
void CNSmlObexDMplugin::TransportUpIndication()
    {
	DBG_FILE( _S8("CNSmlObexDMplugin::TransportUpIndication...") );
    }

// -----------------------------------------------------------------------------
// CNSmlObexDMplugin::TransportDownIndication()
// 
// -----------------------------------------------------------------------------
//
void CNSmlObexDMplugin::TransportDownIndication()
    {
	DBG_FILE( _S8("CNSmlObexDMplugin::TransportDownIndication...") );
	Disconnect();
    }

// -----------------------------------------------------------------------------
// CNSmlObexDMplugin::ObexConnectIndication( const TObexConnectInfo& /*aRemoteInfo*/, const TDesC8& /*aInfo*/ )
// 
// -----------------------------------------------------------------------------
//
TInt CNSmlObexDMplugin::ObexConnectIndication( const TObexConnectInfo& /*aRemoteInfo*/, const TDesC8& /*aInfo*/ )
    {
	_DBG_FILE("CNSmlObexDMplugin::ObexConnectIndication [w45 14.11.2003]");	
	TInt err( KErrNone );
	iStopped = EFalse;

	iStartTimer = ETrue;

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
		RSemaphore signal;
		signal.CreateLocal(0);
		RThread t;
		TRAP( err, t.Create(_L("ObexCommServerThread"), CreateServer, 1024*6, KMinHeapSize, KMinHeapSize*250, &signal) );
		t.Resume();
		t.Close();
		signal.Wait();
		signal.Close();

		err = iSs.Connect();
		}
	
	if( err == KErrNone )
		{
		iSessCancelled = EFalse;
		iListener = new CDisListener( iSs, this );
		if ( !iListener )
			{
			err = KErrNoMemory;
			}
			
		if( err == KErrNone )
			{
			iListener->ListenDisconnect();
			DBG_FILE( _S8("CNSmlObexDMplugin iServiceTimer->StopTimer()") );
			iServiceTimer->Cancel();
			iServiceTimer->StopTimer();
			}
		}
    return ConvertError( err );
    }

// -----------------------------------------------------------------------------
// CNSmlObexDMplugin::ObexDisconnectIndication( const TDesC8& /*aInfo*/ )
// 
// -----------------------------------------------------------------------------
//
void CNSmlObexDMplugin::ObexDisconnectIndication( const TDesC8& /*aInfo*/ )
    {
	_DBG_FILE("CNSmlObexDMplugin::ObexDisconnectIndication");
	Disconnect();
	}

// -----------------------------------------------------------------------------
// CNSmlObexDMplugin::PutRequestIndication()
// 
// -----------------------------------------------------------------------------
//
CObexBufObject* CNSmlObexDMplugin::PutRequestIndication()
    {
	_DBG_FILE("CNSmlObexDMplugin::PutRequestIndication");
	iState = EIdle;
	if ( iObexBufObject )
		{
		iObexBufData->Reset();
		iObexBufObject->Reset();
		}
	if( iStopped )
		{
		_DBG_FILE("CNSmlObexDMplugin service unavailable");
		return NULL;
		}
    return iObexBufObject;
    }

// -----------------------------------------------------------------------------
// CNSmlObexDMplugin::PutPacketIndication()
// 
// -----------------------------------------------------------------------------
//
TInt CNSmlObexDMplugin::PutPacketIndication()
    {
	_DBG_FILE("CNSmlObexDMplugin::PutPacketIndication");
	TInt bytes = this->iObexBufObject->BytesReceived();
	DBG_FILE_CODE( bytes, _S8("reveived bytes") );
	if( iStopped )
		{
		_DBG_FILE("CNSmlObexDMplugin service unavailable");
		return KErrIrObexRespServiceUnavail;
		}
	return KErrNone;
    }

// -----------------------------------------------------------------------------
// CNSmlObexDMplugin::PutCompleteIndication()
// 
// -----------------------------------------------------------------------------
//
TInt CNSmlObexDMplugin::PutCompleteIndication()
    {
	_DBG_FILE("CNSmlObexDMplugin::PutCompleteIndication");
	TInt err( KErrNone );

	err = CheckMimeType();

	_DBG_FILE("CNSmlObexDMplugin After CheckMimeType");

	if( err == KErrNone )
		{
		if ( iObexBufData->Size() > KNSmlLocalSyncWorkspaceSize )
		    {
        	_DBG_FILE("CNSmlObexDMplugin: Received package too large!");
		    return KErrIrObexRespReqEntityTooLarge;
		    }
		
		if ( iZeroPkg )
			{
			_DBG_FILE("CNSmlObexDMplugin..Forwarding Zero Pkg");
			DBG_DUMP((void*)iObexBufData->Ptr(0).Ptr(), iObexBufData->Size(), _S8("Zero Pkg") );

			#ifdef __NSML_DEBUG__
			_DBG_FILE("CNSmlObexDMplugin..Zero Pkg: CWbxml2XmlConverter::ConvertL() begin");
			CWbxml2XmlConverter* c = NULL;
			TRAP( err, c = CWbxml2XmlConverter::NewL() );
			if (c)
				{
				TRAP( err, c->ConvertL(iObexBufData->Ptr(0).Ptr(), iObexBufData->Size()) );
				DBG_DUMP((void*)c->Document().Ptr(), c->Document().Length(), _S8("Zero Pkg (XML)") );
				delete c;
				}
			_DBG_FILE("CNSmlObexDMplugin..Zero Pkg: CWbxml2XmlConverter::ConvertL() end");
			#endif // __NSML_DEBUG__
	
			TRAP( err, err = ForwardZeroPkgL( iObexContent ) );
			iZeroPkg = EFalse;
			}
		else
			{
			_DBG_FILE("CNSmlObexDMplugin..SetReceivedPacket");
			DBG_DUMP((void*)iObexBufData->Ptr(0).Ptr(), iObexBufData->Size(), _S8("CNSmlObexDMplugin SetReceivedPacket") );

			#ifdef __NSML_DEBUG__
			_DBG_FILE("CNSmlObexDMplugin..SetReceivedPacket: CWbxml2XmlConverter::ConvertL() begin");
			CWbxml2XmlConverter* c = NULL;
			TRAP( err, c = CWbxml2XmlConverter::NewL() );
			if (c)
				{
				TRAP( err, c->ConvertL(iObexBufData->Ptr(0).Ptr(), iObexBufData->Size()) );
				DBG_DUMP((void*)c->Document().Ptr(), c->Document().Length(), _S8("SetReceivedPacket (XML)") );
				delete c;
				}
			_DBG_FILE("CNSmlObexDMplugin..SetReceivedPacket: CWbxml2XmlConverter::ConvertL() end");
			#endif // __NSML_DEBUG__
			
			if( !iStopped )
				{
				this->iObexBufObject->BytesReceived();
				this->iSs.SetReceivedPacket( this->iObexBufData->Ptr( 0 ) );
				}
			else{
				err = KErrAccessDenied;
				}
			}
		}
	else if ( err == KErrNotSupported )
	    {
    	_DBG_FILE("CNSmlObexDMplugin: Mimetype not supported!");
	    return KErrIrObexRespUnsupMediaType;
	    }
	
	return ConvertError( err );
    }

// -----------------------------------------------------------------------------
// CNSmlObexDMplugin::GetRequestIndication( CObexBaseObject* /*aRequiredObject*/ )
// 
// -----------------------------------------------------------------------------
//
CObexBufObject* CNSmlObexDMplugin::GetRequestIndication( CObexBaseObject* /*aRequiredObject*/ )
    {
	_DBG_FILE("CNSmlObexDMplugin::GetRequestIndication");

	TInt err( KErrNone );
	iObexBufObject->Reset();

	iState = EGetSendPacket;
	err = iGetPacketWaiter->WaitForGetData();
	iState = EIdle;

	if ( err == KErrNone )
		{
		iObexBufData->Reset();
		TRAP( err, iObexBufData->InsertL( 0, iGetPacketWaiter->Data() ) );
		}
	if ( err != KErrNone )
		{
		return NULL;
		}

	#ifdef __NSML_DEBUG__
	_DBG_FILE("CNSmlObexDMplugin GetRequestIndication: CWbxml2XmlConverter begin");
	CWbxml2XmlConverter* c = NULL;
	TRAP( err, c = CWbxml2XmlConverter::NewL() );
	if (c)
		{
		TRAP( err, c->ConvertL(iObexBufData->Ptr(0).Ptr(), iObexBufData->Size()) );
		DBG_DUMP((void*)c->Document().Ptr(), c->Document().Length(), _S8("CNSmlObexDMplugin GetRequestIndication (XML)") );
		delete c;
		c = NULL;
		}
	_DBG_FILE("CNSmlObexDMplugin GetRequestIndication: CWbxml2XmlConverter end");
	#endif // __NSML_DEBUG__


	TRAP( err, iObexBufObject->SetDataBufL( iObexBufData ) );
	if ( err != KErrNone )
		{
		return NULL;
		}
    return iObexBufObject;
   }

// -----------------------------------------------------------------------------
// CNSmlObexDMplugin::GetPacketIndication()
// 
// -----------------------------------------------------------------------------
//
TInt CNSmlObexDMplugin::GetPacketIndication()
    {
	_DBG_FILE("CNSmlObexDMplugin::GetPacketIndication");
	if( iStopped )
		{
		_DBG_FILE("CNSmlObexDMplugin service unavailable");
		return KErrIrObexRespServiceUnavail;
		}
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CNSmlObexDMplugin::GetCompleteIndication()
// 
// -----------------------------------------------------------------------------
//
TInt CNSmlObexDMplugin::GetCompleteIndication()
    {
	_DBG_FILE("CNSmlObexDMplugin::GetCompleteIndication");
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CNSmlObexDMplugin::SetPathIndication(const CObex::TSetPathInfo& /*aPathInfo*/, const TDesC8& /*aInfo*/)
// 
// -----------------------------------------------------------------------------
//
TInt CNSmlObexDMplugin::SetPathIndication(const CObex::TSetPathInfo& /*aPathInfo*/, const TDesC8& /*aInfo*/)
    {
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CNSmlObexDMplugin::AbortIndication()
// 
// -----------------------------------------------------------------------------
//
void CNSmlObexDMplugin::AbortIndication()
    {
	_DBG_FILE("CNSmlObexDMplugin::AbortIndication");
	Disconnect();
    }

// -----------------------------------------------------------------------------
// CNSmlObexDMplugin::Disconnect()
// 
// -----------------------------------------------------------------------------
//
void CNSmlObexDMplugin::Disconnect()
    {
	_DBG_FILE("CNSmlObexDMplugin::Disconnect");
	iZeroPkg = ETrue;

	if( iStartTimer != EFalse )
		{
		DBG_FILE( _S8("CNSmlObexDMplugin iServiceTimer->StartTimer()") );
		iServiceTimer->Cancel();
		iServiceTimer->StartTimer();
		}
	
	iState = EIdle;
	if( !iSessCancelled )
		{
		_DBG_FILE("calling iSs.Disconnect()");
		iSs.Disconnect();
		iSs.Close();
		iSessCancelled = ETrue;
		}
    }

// -----------------------------------------------------------------------------
// CNSmlObexDMplugin::CheckMimeType()
// 
// -----------------------------------------------------------------------------
//
TInt CNSmlObexDMplugin::CheckMimeType()
	{ 	
	_DBG_FILE("CNSmlObexDMplugin::CheckMimeType() - mimetype in dump...");
	
	TBuf8<256> mimeType;
	mimeType.Copy( this->iObexBufObject->Type() );
	mimeType.LowerCase();
	TrimRightSpaceAndNull( mimeType );

	DBG_DUMP( (void*)mimeType.Ptr(), mimeType.Length(), _S8("Received mimetype") );

	if ( (mimeType.Compare( KDataSyncDM ) == 0) ||
		 (mimeType.Compare( KDataSyncDMAlertNotification ) == 0) )
		{		
		_DBG_FILE("CNSmlObexDMplugin... KDataSyncDM OBEX content");
		iObexContent = ENSmlDeviceManagement;	
		return KErrNone;
		}
	else
		{
		_DBG_FILE("Unsupported mime type");
		return KErrNotSupported;
		}
	}

// -----------------------------------------------------------------------------
// CNSmlObexDMplugin::TrimRightSpaceAndNull()
// 
// -----------------------------------------------------------------------------
//
void CNSmlObexDMplugin::TrimRightSpaceAndNull( TDes8& aDes ) const
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
// CNSmlObexDMplugin::ForwardZeroPkgL( TNSmlObexContent aSyncSelection )
// 
// -----------------------------------------------------------------------------
//
TInt CNSmlObexDMplugin::ForwardZeroPkgL( TNSmlObexContent aSyncSelection )
	{
	_DBG_FILE("CNSmlObexDMplugin::ForwardZeroPkgL()");
	TInt err( KErrNone );

	if ( aSyncSelection == ENSmlDeviceManagement )
		{
		_DBG_FILE("CNSmlObexDMplugin... Before StartSync");
		
		RNSmlPrivateAPI privateApi;
		TRAPD(openErr, privateApi.OpenL());
	
		if ( openErr != KErrNone )
			{
			return ENSmlErrUndefined;
			}
	
		TRAPD(connErr, privateApi.SendL(this->iObexBufData->Ptr(0), ESmlDevMan,
		    ESmlVersion1_1_2,KUidNSmlMediumTypeBluetooth ));
	
		if ( connErr != KErrNone )
			{
			privateApi.Close();
			return ENSmlErrUndefined;
			}
		
		privateApi.Close();
				
		}
	else
		{
		err = KErrNotSupported;
		}
	return err;
	}

// -----------------------------------------------------------------------------
// CNSmlObexDMplugin::SetMediaType( TSrcsMediaType aMediaType )
// 
// -----------------------------------------------------------------------------
//
void CNSmlObexDMplugin::SetMediaType( TSrcsMediaType aMediaType )
    {
	DBG_FILE( _S8("CNSmlObexDMplugin::SetMediaType()") );
	iMediaType = aMediaType;
    }

// -----------------------------------------------------------------------------
// CNSmlObexDMplugin::CreateServer( TAny* aPtr )
// 
// -----------------------------------------------------------------------------
//
TInt CNSmlObexDMplugin::CreateServer( TAny* aPtr )
	{
	__UHEAP_MARK;
	CTrapCleanup* cleanup = CTrapCleanup::New(); // get clean-up stack
	CActiveScheduler* scheduler = new CActiveScheduler();
	CActiveScheduler::Install(scheduler);	
	TRAPD( err, StartServerL( aPtr ) );
	__ASSERT_ALWAYS(err == KErrNone, User::Panic(_L("OBEXCommServer"), err));
	delete scheduler;
	delete cleanup; // destroy clean-up stack
	__UHEAP_MARKEND;
	return KErrNone;
	}

// -----------------------------------------------------------------------------
// CNSmlObexDMplugin::StartServer( TAny* aPtr )
// 
// -----------------------------------------------------------------------------
//
void CNSmlObexDMplugin::StartServerL( TAny* aPtr )
	{
	CNSmlObexCommServer* server = CNSmlObexCommServer::NewL( KNSmlDMObexCommServerName() );
	RSemaphore* s = (RSemaphore*)aPtr;
	s->Signal();
	CActiveScheduler::Start();
	delete server;
	}

// -----------------------------------------------------------------------------
// CNSmlObexDMplugin::ConvertError( TInt aError )
// -----------------------------------------------------------------------------
//
TInt CNSmlObexDMplugin::ConvertError( TInt aError )
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
};

// -----------------------------------------------------------------------------
// CNSmlObexDMplugin::SetObexServer( CObexServer* aObexServer )
// -----------------------------------------------------------------------------
//
TInt CNSmlObexDMplugin::SetObexServer( CObexServer* aObexServer )
    {
    DBG_FILE(_S8("CNSmlObexDMplugin::SetObexServer - Begin"));                          
    iObexServer = aObexServer;      
    return iObexServer->Start(this) ;
    }

// ========================== OTHER EXPORTED FUNCTIONS =========================

//End of File

