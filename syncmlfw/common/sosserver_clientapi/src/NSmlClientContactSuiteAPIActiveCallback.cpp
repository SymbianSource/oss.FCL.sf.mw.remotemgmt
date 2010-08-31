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
#include <s32mem.h>
#include <SyncMLObservers.h>

#include "NSmlClientAPIDefs.h"
#include "NSmlClientAPIUtils.h"
#include "NSmlClientAPIActiveCallback.h"
#include "NSmlClientContactSuiteAPIActiveCallback.h"
#include "nsmlsosserverdefs.h"
#include "NSmlErrorCodeConversion.h"


//
// CSmlActiveContactSuiteProgressCallback
//


// -----------------------------------------------------------------------------
// CSmlActiveContactSuiteProgressCallback::NewL(
// Two-phase construction.
// -----------------------------------------------------------------------------
//
CSmlActiveContactSuiteProgressCallback* CSmlActiveContactSuiteProgressCallback::NewL( const CSmlContactSuiteActiveCallback* aCallback )
    {
    CSmlActiveContactSuiteProgressCallback* self = new (ELeave) CSmlActiveContactSuiteProgressCallback( aCallback );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop(); // self
    return self;
    }
    
// -----------------------------------------------------------------------------
// CSmlActiveContactSuiteProgressCallback::~CSmlActiveContactSuiteProgressCallback()
// Destructor.
// -----------------------------------------------------------------------------
//
CSmlActiveContactSuiteProgressCallback::~CSmlActiveContactSuiteProgressCallback()
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
// CSmlActiveContactSuiteProgressCallback::SetObserver()
// Sets the observer to be notified. Starts requesting progress 
// events if it is not yet started.
// -----------------------------------------------------------------------------
//
void CSmlActiveContactSuiteProgressCallback::SetObserver( MSyncMLProgressObserver& aObserver )
    {
    iObserver = &aObserver;
    
    if ( !iRequesting )
        {
        iRequesting = ETrue;
        Request();
        }
    }
    
// -----------------------------------------------------------------------------
// CSmlActiveContactSuiteProgressCallback::CancelProgress()
// Cancels progress notification.
// -----------------------------------------------------------------------------
//
void CSmlActiveContactSuiteProgressCallback::CancelProgress()
    {
    DoCancel();
    }
    
// -----------------------------------------------------------------------------
// CSmlActiveContactSuiteProgressCallback::DoCancel()
// -----------------------------------------------------------------------------
//
void CSmlActiveContactSuiteProgressCallback::DoCancel()
    {
    // cancel request to server
    iCallback->SendReceive( ECmdContactSuiteProgressRequestCancel );
    iRequesting = EFalse;
    }
    
// -----------------------------------------------------------------------------
// CSmlActiveContactSuiteProgressCallback::RunL()
// -----------------------------------------------------------------------------
//
void CSmlActiveContactSuiteProgressCallback::RunL()
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
    
TInt CSmlActiveContactSuiteProgressCallback::RunError ( TInt /*aError*/ )
    {
    	return KErrNone;
    }	
// -----------------------------------------------------------------------------
// CSmlActiveContactSuiteProgressCallback::Request()
// Sends asynchronous progress event request to server.
// -----------------------------------------------------------------------------
//
void CSmlActiveContactSuiteProgressCallback::Request()
    {
    iBufPtr.Zero();
    
    if ( !IsActive() )
        {
        SetActive();
        }
    
    if ( iCallback )
        {
        iCallback->SendReceive( ECmdContactSuiteProgressRequest, TIpcArgs( &iBufPtr ), iStatus );
        }
    }
    
// -----------------------------------------------------------------------------
// CSmlActiveContactSuiteProgressCallback::ConstructL()
// -----------------------------------------------------------------------------
//
void CSmlActiveContactSuiteProgressCallback::ConstructL()
    {
    iBuf = HBufC8::NewL( KNSmlMaxProgressMessageLength );
    iBufPtr.Set( iBuf->Des() );
    }
    
// -----------------------------------------------------------------------------
// CSmlActiveContactSuiteProgressCallback::CSmlActiveContactSuiteProgressCallback()
// Contructor.
// -----------------------------------------------------------------------------
//
CSmlActiveContactSuiteProgressCallback::CSmlActiveContactSuiteProgressCallback( const CSmlContactSuiteActiveCallback* aCallback )
    : CActive( EPriorityStandard ), iRequesting( EFalse ), iBufPtr( 0, NULL, 0 ), iCallback( aCallback )
    {
    CActiveScheduler::Add( this );
    }
    
// -----------------------------------------------------------------------------
// CSmlActiveContactSuiteProgressCallback::NotifyErrorL()
// Reads progress error related data and notifies observer.
// -----------------------------------------------------------------------------
//
void CSmlActiveContactSuiteProgressCallback::NotifyErrorL( RReadStream& aStream ) const
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
// CSmlActiveContactSuiteProgressCallback::NotifyProgressL()
// Reads progress event related data and notifies observer.
// -----------------------------------------------------------------------------
//
void CSmlActiveContactSuiteProgressCallback::NotifyProgressL( RReadStream& aStream ) const
    {
    TInt8 status = aStream.ReadInt8L();
    TInt32 info1 = aStream.ReadInt32L();
    TInt32 info2 = aStream.ReadInt32L();
    
    iObserver->OnSyncMLSyncProgress( (MSyncMLProgressObserver::TStatus)status, info1, info2 );
    }
    
// -----------------------------------------------------------------------------
// CSmlActiveContactSuiteProgressCallback::NotifyModificationsL()
// Reads progress information of modifications and notifies 
// observer.
// -----------------------------------------------------------------------------
//
void CSmlActiveContactSuiteProgressCallback::NotifyModificationsL( RReadStream& aStream )
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
// -----------------------------------------------------------------------------
// CSmlActiveCallback::CSmlActiveCallback()
// -----------------------------------------------------------------------------
//
CSmlContactSuiteActiveCallback::CSmlContactSuiteActiveCallback( RSyncMLSession& aSession )
    : iContactSuiteSession( aSession ), iActiveCallback(aSession)
    {
    }

// -----------------------------------------------------------------------------
// CSmlActiveCallback::~CSmlActiveCallback()
// Destructor.
// -----------------------------------------------------------------------------
//
CSmlContactSuiteActiveCallback::~CSmlContactSuiteActiveCallback()
    {
    //delete iEventCallback; iEventCallback = NULL;
    delete iProgressCallback; iProgressCallback = NULL;
    }

// -----------------------------------------------------------------------------
// CSmlActiveCallback::SetProgressObserverL()
// Sets the progress observer.
// -----------------------------------------------------------------------------
//
void CSmlContactSuiteActiveCallback::SetProgressObserverL( MSyncMLProgressObserver& aObserver )
    {
    if ( !iProgressCallback )
        {
        iProgressCallback = CSmlActiveContactSuiteProgressCallback::NewL( this );
        }
    
    iProgressCallback->SetObserver( aObserver );
    }


// -----------------------------------------------------------------------------
// CSmlActiveCallback::CancelProgress()
// Cancels progress notification.
// -----------------------------------------------------------------------------
//
void CSmlContactSuiteActiveCallback::CancelProgress()
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
void CSmlContactSuiteActiveCallback::SendReceive( TInt aCmd, const TIpcArgs& aArgs, TRequestStatus& aStatus ) const
    {
    iActiveCallback.SendReceive( aCmd, aArgs , aStatus );    
    }

// -----------------------------------------------------------------------------
// CSmlActiveCallback::SendReceive()
// Makes a synchronous IPC call to server. Used by event and progress 
// notifier objects.
// -----------------------------------------------------------------------------
//
void CSmlContactSuiteActiveCallback::SendReceive( TInt aCmd ) const
    {
    iActiveCallback.SendReceive( aCmd );    
    }
