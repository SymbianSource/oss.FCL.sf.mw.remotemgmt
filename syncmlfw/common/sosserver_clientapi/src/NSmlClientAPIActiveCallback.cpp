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
* Description:  Implementation for event/progress notifiers
*
*/


// This define should be moved somewhere else (symbian headers) !!
#ifndef SYNCML_V3
#define SYNCML_V3
#endif

#include <s32mem.h>
#include <SyncMLObservers.h>

#include "NSmlClientAPIDefs.h"
#include "NSmlClientAPIUtils.h"
#include "NSmlClientAPIActiveCallback.h"
#include "nsmlsosserverdefs.h"
#include "NSmlErrorCodeConversion.h"


// ============================ MEMBER FUNCTIONS ===============================

//
// CSmlActiveCallback
//


// -----------------------------------------------------------------------------
// CSmlActiveCallback::CSmlActiveCallback()
// -----------------------------------------------------------------------------
//
CSmlActiveCallback::CSmlActiveCallback( RSyncMLSession& aSession )
	: iSession( aSession )
	{
	}

// -----------------------------------------------------------------------------
// CSmlActiveCallback::~CSmlActiveCallback()
// Destructor.
// -----------------------------------------------------------------------------
//
CSmlActiveCallback::~CSmlActiveCallback()
	{
	delete iEventCallback; iEventCallback = NULL;
	delete iProgressCallback; iProgressCallback = NULL;
	}
	
// -----------------------------------------------------------------------------
// CSmlActiveCallback::SetEventObserverL()
// Sets the event observer and starts event notification.
// -----------------------------------------------------------------------------
//
void CSmlActiveCallback::SetEventObserverL( MSyncMLEventObserver& aObserver )
	{
	if ( !iEventCallback )
		{
		iEventCallback = CSmlActiveEventCallback::NewL( this );
		}
	
	iEventCallback->SetObserver( aObserver );
	}

// -----------------------------------------------------------------------------
// CSmlActiveCallback::SetProgressObserverL()
// Sets the progress observer.
// -----------------------------------------------------------------------------
//
void CSmlActiveCallback::SetProgressObserverL( MSyncMLProgressObserver& aObserver )
	{
	if ( !iProgressCallback )
		{
		iProgressCallback = CSmlActiveProgressCallback::NewL( this );
		}
	
	iProgressCallback->SetObserver( aObserver );
	}

// -----------------------------------------------------------------------------
// CSmlActiveCallback::CancelEvent()
// Cancels event notification.
// -----------------------------------------------------------------------------
//
void CSmlActiveCallback::CancelEvent()
	{
	if ( iEventCallback )
		{
		iEventCallback->CancelEvent();
		delete iEventCallback; iEventCallback = NULL;
		}
	}
	
// -----------------------------------------------------------------------------
// CSmlActiveCallback::CancelProgress()
// Cancels progress notification.
// -----------------------------------------------------------------------------
//
void CSmlActiveCallback::CancelProgress()
	{
	if ( iProgressCallback )
		{
		iProgressCallback->CancelProgress();
		delete iProgressCallback; iProgressCallback = NULL;
		}
	}

// -----------------------------------------------------------------------------
// CSmlActiveCallback::SendReceive()
// Makes an asynchronous IPC call to server. Used by event and progress 
// notifier objects.
// -----------------------------------------------------------------------------
//
void CSmlActiveCallback::SendReceive( TInt aCmd, const TIpcArgs& aArgs, TRequestStatus& aStatus ) const
	{
	iSession.SendReceive( aCmd, aArgs, aStatus );
	}

// -----------------------------------------------------------------------------
// CSmlActiveCallback::SendReceive()
// Makes a synchronous IPC call to server. Used by event and progress 
// notifier objects.
// -----------------------------------------------------------------------------
//
void CSmlActiveCallback::SendReceive( TInt aCmd ) const
	{
	iSession.SendReceive( aCmd );
	}


//
// CSmlActiveEventCallback
//


// -----------------------------------------------------------------------------
// CSmlActiveEventCallback::NewL()
// Two-phase construction.
// -----------------------------------------------------------------------------
//
CSmlActiveEventCallback* CSmlActiveEventCallback::NewL( const CSmlActiveCallback* aCallback )
	{
	CSmlActiveEventCallback* self = new (ELeave) CSmlActiveEventCallback( aCallback );
	CleanupStack::PushL( self );
	self->ConstructL();
	CleanupStack::Pop(); // self
	return self;
	}
	
// -----------------------------------------------------------------------------
// CSmlActiveEventCallback::~CSmlActiveEventCallback()
// Destructor.
// -----------------------------------------------------------------------------
//
CSmlActiveEventCallback::~CSmlActiveEventCallback()
	{
	Cancel();
	if ( iRequesting )
		{
		CancelEvent();
		}
	
	delete iBuf;
	iBuf = NULL;
	}
	
// -----------------------------------------------------------------------------
// CSmlActiveEventCallback::SetObserver()
// Sets the observer to be notified. Starts requesting events 
// if it is not yet started.
// -----------------------------------------------------------------------------
//
void CSmlActiveEventCallback::SetObserver( MSyncMLEventObserver& aObserver )
	{
	iObserver = &aObserver;
	
	if ( !iRequesting )
		{
		iRequesting = ETrue;
		Request();
		}
	}
	
// -----------------------------------------------------------------------------
// CSmlActiveEventCallback::CancelEvent()
// Cancels event notification.
// -----------------------------------------------------------------------------
//
void CSmlActiveEventCallback::CancelEvent()
	{
	DoCancel();
	}
	
// -----------------------------------------------------------------------------
// CSmlActiveEventCallback::DoCancel()
// Cancels event notification.
// -----------------------------------------------------------------------------
//
void CSmlActiveEventCallback::DoCancel()
	{
	// cancel request to server
	iCallback->SendReceive( ECmdEventRequestCancel );
	iRequesting = EFalse;
	}
	
// -----------------------------------------------------------------------------
// CSmlActiveEventCallback::RunL()
// Processes the data received from server and informs 
// observer. In the end sends the request again so that the 
// observer will be informed as long as it is registered
// to this object.
// -----------------------------------------------------------------------------
//
void CSmlActiveEventCallback::RunL()
	{
	// check status
	if ( iStatus.Int() == KErrNone )
		{
		// open a readstream to iEventBuf's data
		RDesReadStream readStream( iBufPtr );
		CleanupClosePushL( readStream );
		
		// read the fist interger in databuffer -> the type of message
		TInt8 type = readStream.ReadInt8L();
		if ( type != ENSmlTypeEvent )
			{
			User::Panic( KNSmlClientAPIPanic, KErrArgument );
			}

		// read data and inform observer
		MSyncMLEventObserver::TEvent event = static_cast<MSyncMLEventObserver::TEvent>( readStream.ReadInt8L() );
		TInt32 id = readStream.ReadInt32L();
		TInt32 error = readStream.ReadInt32L();
		TInt32 additional = readStream.ReadInt32L();
		
		TNSmlErrorConversion errorconv( error );
		error = errorconv.Convert();		
		
		CleanupStack::PopAndDestroy(); // readStream
		
		iObserver->OnSyncMLSessionEvent( event, id, error, additional );
		if ( iRequesting )
			{
			Request();
			}
		}
	else if ( iStatus.Int() == KErrServerTerminated )
		{
		iObserver->OnSyncMLSessionEvent( MSyncMLEventObserver::EServerTerminated, 0, KErrServerTerminated, 0 );
		}
#ifdef __CLIENT_API_MT_
	else
		{
		CActiveScheduler::Stop();
		}
#endif
	}

TInt CSmlActiveEventCallback::RunError ( TInt /*aError*/ )
    {
    	return KErrNone;
    }	
// -----------------------------------------------------------------------------
// CSmlActiveEventCallback::Request()
// Makes an event request to server.
// -----------------------------------------------------------------------------
//
void CSmlActiveEventCallback::Request()
	{
	iBufPtr.Zero();
	
	if ( !IsActive() )
		{
		SetActive();
		}
	
	if ( iCallback )
		{
		iCallback->SendReceive( ECmdEventRequest, TIpcArgs( &iBufPtr ), iStatus );
		}
	}
	
// -----------------------------------------------------------------------------
// CSmlActiveEventCallback::ConstructL()
// Two-phase construction.
// -----------------------------------------------------------------------------
//
void CSmlActiveEventCallback::ConstructL()
	{
	iBuf = HBufC8::NewL( KNSmlMaxEventMessageLength );
	iBufPtr.Set( iBuf->Des() );
	}

// -----------------------------------------------------------------------------
// CSmlActiveEventCallback::CSmlActiveEventCallback(
// Contructor.
// -----------------------------------------------------------------------------
//
CSmlActiveEventCallback::CSmlActiveEventCallback( const CSmlActiveCallback* aCallback )
	: CActive( EPriorityStandard ), iRequesting( EFalse ), iBufPtr( 0, NULL, 0 ), iCallback( aCallback )
	{
	CActiveScheduler::Add( this );
	}
	


//
// CSmlActiveProgressCallback
//


// -----------------------------------------------------------------------------
// CSmlActiveProgressCallback::NewL(
// Two-phase construction.
// -----------------------------------------------------------------------------
//
CSmlActiveProgressCallback* CSmlActiveProgressCallback::NewL( const CSmlActiveCallback* aCallback )
	{
	CSmlActiveProgressCallback* self = new (ELeave) CSmlActiveProgressCallback( aCallback );
	CleanupStack::PushL( self );
	self->ConstructL();
	CleanupStack::Pop(); // self
	return self;
	}
	
// -----------------------------------------------------------------------------
// CSmlActiveProgressCallback::~CSmlActiveProgressCallback()
// Destructor.
// -----------------------------------------------------------------------------
//
CSmlActiveProgressCallback::~CSmlActiveProgressCallback()
	{
	Cancel();
	if ( iRequesting )
		{
		CancelProgress();
		}
	
	delete iBuf;
	iBuf = NULL;
	}
	
// -----------------------------------------------------------------------------
// CSmlActiveProgressCallback::SetObserver()
// Sets the observer to be notified. Starts requesting progress 
// events if it is not yet started.
// -----------------------------------------------------------------------------
//
void CSmlActiveProgressCallback::SetObserver( MSyncMLProgressObserver& aObserver )
	{
	iObserver = &aObserver;
	
	if ( !iRequesting )
		{
		iRequesting = ETrue;
		Request();
		}
	}
	
// -----------------------------------------------------------------------------
// CSmlActiveProgressCallback::CancelProgress()
// Cancels progress notification.
// -----------------------------------------------------------------------------
//
void CSmlActiveProgressCallback::CancelProgress()
	{
	DoCancel();
	}
	
// -----------------------------------------------------------------------------
// CSmlActiveProgressCallback::DoCancel()
// -----------------------------------------------------------------------------
//
void CSmlActiveProgressCallback::DoCancel()
	{
	// cancel request to server
	iCallback->SendReceive( ECmdProgressRequestCancel );
	iRequesting = EFalse;
	}
	
// -----------------------------------------------------------------------------
// CSmlActiveProgressCallback::RunL()
// -----------------------------------------------------------------------------
//
void CSmlActiveProgressCallback::RunL()
	{
	// check status
	if ( iStatus.Int() == KErrNone )
		{
		// open a readstream to iEventBuf's data
		RDesReadStream readStream( iBufPtr );
		CleanupClosePushL( readStream );
		
		// read the fist interger in databuffer -> the type of message
		TInt8 type = readStream.ReadInt8L();
		if ( type != ENSmlTypeProgressEvent )
			{
			User::Panic( KNSmlClientAPIPanic, KErrArgument );
			}

		// check which progress event was received, and notify observer
		TNSmlProgressEventType eventType = (TNSmlProgressEventType)readStream.ReadInt8L();
		switch( eventType )
			{
			case ENSmlSyncError:
				NotifyErrorL( readStream );
				break;
			case ENSmlSyncProgress:
				NotifyProgressL( readStream );
				break;
			case ENSmlModifications:
				NotifyModificationsL( readStream );
				break;
			default:
				User::Panic( KNSmlClientAPIPanic, KErrArgument );
				break;
			}
		
		CleanupStack::PopAndDestroy(); // readStream
		
		if ( iRequesting )
			{
			Request();
			}
		}
#ifdef __CLIENT_API_MT_
	else
		{
		CActiveScheduler::Stop();
		}
#endif
	}
TInt CSmlActiveProgressCallback::RunError ( TInt /*aError*/ )
    {
    	return KErrNone;
    }	
// -----------------------------------------------------------------------------
// CSmlActiveProgressCallback::Request()
// Sends asynchronous progress event request to server.
// -----------------------------------------------------------------------------
//
void CSmlActiveProgressCallback::Request()
	{
	iBufPtr.Zero();
	
	if ( !IsActive() )
		{
		SetActive();
		}
	
	if ( iCallback )
		{
		iCallback->SendReceive( ECmdProgressRequest, TIpcArgs( &iBufPtr ), iStatus );
		}
	}
	
// -----------------------------------------------------------------------------
// CSmlActiveProgressCallback::ConstructL()
// -----------------------------------------------------------------------------
//
void CSmlActiveProgressCallback::ConstructL()
	{
	iBuf = HBufC8::NewL( KNSmlMaxProgressMessageLength );
	iBufPtr.Set( iBuf->Des() );
	}
	
// -----------------------------------------------------------------------------
// CSmlActiveProgressCallback::CSmlActiveProgressCallback()
// Contructor.
// -----------------------------------------------------------------------------
//
CSmlActiveProgressCallback::CSmlActiveProgressCallback( const CSmlActiveCallback* aCallback )
	: CActive( EPriorityStandard ), iRequesting( EFalse ), iBufPtr( 0, NULL, 0 ), iCallback( aCallback )
	{
	CActiveScheduler::Add( this );
	}
	
// -----------------------------------------------------------------------------
// CSmlActiveProgressCallback::NotifyErrorL()
// Reads progress error related data and notifies observer.
// -----------------------------------------------------------------------------
//
void CSmlActiveProgressCallback::NotifyErrorL( RReadStream& aStream ) const
	{
	TInt8 errorLevel = aStream.ReadInt8L();
	TInt32 error = aStream.ReadInt32L();
	TInt32 taskId = aStream.ReadInt32L();
	TInt32 info1 = aStream.ReadInt32L();
	TInt32 info2 = aStream.ReadInt32L();
	
	TNSmlErrorConversion errorconv( error );
	error = errorconv.Convert();
	
	iObserver->OnSyncMLSyncError( (MSyncMLProgressObserver::TErrorLevel)errorLevel, error, taskId, info1, info2 );
	}
	
// -----------------------------------------------------------------------------
// CSmlActiveProgressCallback::NotifyProgressL()
// Reads progress event related data and notifies observer.
// -----------------------------------------------------------------------------
//
void CSmlActiveProgressCallback::NotifyProgressL( RReadStream& aStream ) const
	{
	TInt8 status = aStream.ReadInt8L();
	TInt32 info1 = aStream.ReadInt32L();
	TInt32 info2 = aStream.ReadInt32L();
	
	iObserver->OnSyncMLSyncProgress( (MSyncMLProgressObserver::TStatus)status, info1, info2 );
	}
	
// -----------------------------------------------------------------------------
// CSmlActiveProgressCallback::NotifyModificationsL()
// Reads progress information of modifications and notifies 
// observer.
// -----------------------------------------------------------------------------
//
void CSmlActiveProgressCallback::NotifyModificationsL( RReadStream& aStream )
	{
#ifdef SYNCML_V3
	TInt32 taskId = aStream.ReadInt32L();
	
	iClientMods.iNumAdded = aStream.ReadInt32L();
	iClientMods.iNumReplaced = aStream.ReadInt32L();
	iClientMods.iNumMoved = aStream.ReadInt32L();
	iClientMods.iNumDeleted = aStream.ReadInt32L();
	iClientMods.iNumFailed = aStream.ReadInt32L();
	
	iServerMods.iNumAdded = aStream.ReadInt32L();
	iServerMods.iNumReplaced = aStream.ReadInt32L();
	iServerMods.iNumMoved = aStream.ReadInt32L();
	iServerMods.iNumDeleted = aStream.ReadInt32L();
	iServerMods.iNumFailed = aStream.ReadInt32L();
	
	iObserver->OnSyncMLDataSyncModifications( taskId, iClientMods, iServerMods );
#endif
	}
	






