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
* Description:  Alert queue and handler
*
*/


#include <centralrepository.h>
#include <DevManInternalCRKeys.h>
#include "NSmlAlertQueue.h"
#include "nsmldebug.h"
// ============================ MEMBER FUNCTIONS ===============================

// ---------------------------------------------------------
// CNSmlMessageQueue::NewL(MNSmlAlertObserver* aObserver)
// Two phase constructor
// ---------------------------------------------------------
//
EXPORT_C CNSmlMessageQueue* CNSmlMessageQueue::NewL( MNSmlAlertObserver* aObserver )
	{
	CNSmlMessageQueue* self = new (ELeave) CNSmlMessageQueue( aObserver );
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(); //self
	return self;
	}

// ---------------------------------------------------------
// CNSmlMessageQueue::~CNSmlMessageQueue()
// Destructor
// ---------------------------------------------------------
EXPORT_C CNSmlMessageQueue::~CNSmlMessageQueue()
	{
	DestroyItems();
	iAlertQueue.Reset();
	iAlertHandler->Cancel();
	delete iAlertHandler;
	}

// ---------------------------------------------------------
// CNSmlMessageQueue::AddMessageL(const TDesC8& aMessage, 
// TSmlUsageType aType, TSmlProtocolVersion aVersion, TSmlTransportId aBearerType )
// Adds new alert message to alert queue
// ---------------------------------------------------------
EXPORT_C void CNSmlMessageQueue::AddMessageL( 
    const TDesC8& aMessage, 
    TSmlUsageType aType, 
    TSmlProtocolVersion aVersion, 
    TSmlTransportId aBearerType )
	{
	CNSmlMessageItem* item = CNSmlMessageItem::NewL( aMessage, aType, 
	        aVersion, aBearerType );
	
	iAlertQueue.AddLast(*item);
	
	if ( iState == ENSmlIdle )
		{
		iAlertHandler->ProcessAlert();	
		}
	
	}

// ---------------------------------------------------------
// CNSmlMessageQueue::ServerSuspended( TBool aSuspend )
// Sets the suspend state 
// ---------------------------------------------------------
EXPORT_C void CNSmlMessageQueue::ServerSuspended( TBool aSuspend )
	{
	iState = ( aSuspend ) ? ENSmlSuspended : ENSmlIdle;
	
	if ( iState == ENSmlSuspended )
		{
		iAlertHandler->Cancel();
		}
	else if ( ! IsEmpty() )
		{
		iAlertHandler->ProcessAlert();
		}
	}

// ---------------------------------------------------------
// CNSmlMessageQueue::IsEmpty()
// 
// ---------------------------------------------------------
EXPORT_C TBool CNSmlMessageQueue::IsEmpty()
	{ 
	return iAlertQueue.IsEmpty();	
	}

// ---------------------------------------------------------
// CNSmlMessageQueue::CheckMessage( TBool& aMore, TSmlUsageType& aType, 
// TSmlProtocolVersion& aVersion, TSmlTransportId& aBearerType )
// Checks if there is alert message in queue. Returns also the usage type
// and protocol version of the message.
// ---------------------------------------------------------
void CNSmlMessageQueue::CheckMessage( 
    TBool& aMore, 
    TSmlUsageType& aType, 
    TSmlProtocolVersion& aVersion, 
    TSmlTransportId& aBearerType )
	{
	iState = ENSmlProcessing;
	
	iAlertIterator.SetToFirst();
	CNSmlMessageItem* item = iAlertIterator;
	
	aMore = ( item != NULL );
	
	if ( aMore )
		{
		aType = item->iUsageType;
		aVersion = item->iProtocol;
		aBearerType = item->iBearerType;
		}
	}


// ---------------------------------------------------------
// CNSmlMessageQueue::MessageSize()
// Returns the size of alert message
// ---------------------------------------------------------
TInt CNSmlMessageQueue::MessageSize()
	{
	iAlertIterator.SetToFirst();
	CNSmlMessageItem* item = iAlertIterator;
	
	if (item)
		{
		return item->Message().Length();
		}
		
	return 0;
	}

// ---------------------------------------------------------
// CNSmlMessageQueue::AlertMessage( TDes8& aMessage )
// Fetches the message from alert queue
// ---------------------------------------------------------
void CNSmlMessageQueue::AlertMessage( TDes8& aMessage )
	{
	iAlertIterator.SetToFirst();
	CNSmlMessageItem* item = iAlertIterator;
	
	if (item)
		{
		aMessage.Zero();

		aMessage.Append( item->Message() );
		
		item->iDlink.Deque();
		delete item;
		item = NULL;
		}
		
	}

// ---------------------------------------------------------
// CNSmlMessageQueue::CreateJobL( CSmlAlertInfo& aInfo, TBool& aQuit, const TPtrC8& aPackage )
// Creates job from supplied onformation and passes it to observer
// ---------------------------------------------------------
void CNSmlMessageQueue::CreateJobL( CSmlAlertInfo& aInfo, TBool& aQuit, const TPtrC8& aPackage  )
	{
    _DBG_FILE("CNSmlMessageQueue::CreateJobL: begin");
	if ( aInfo.JobControl() != CSmlAlertInfo::EDoNotCreateJob ) 
		{
		CNSmlAlertJobInfo jobInfo;
		jobInfo.iProfileId = aInfo.Profile();
		jobInfo.iType = aInfo.Protocol();
		jobInfo.iContentType = new (ELeave) CArrayFixFlat<TNSmlContentTypeInfo>(1);
		jobInfo.iTransportId = aInfo.Transport();
		jobInfo.iPackage.Set( aPackage );
		jobInfo.iSessionId = aInfo.SessionId();
    TInt SanSupport( KErrNone );
	CRepository* centrep = NULL;
    TRAPD( err, centrep = CRepository::NewL( KCRUidDeviceManagementInternalKeys) );    
    if( err == KErrNone )
    {
    centrep->Get( KDevManSANUIBitVariation, SanSupport );
    }
    delete centrep;
	if( SanSupport == 1 )
   	{
    	jobInfo.iUimode = aInfo.GetUimode();
   	}
	DBG_FILE_CODE(jobInfo.iUimode, _S8("CNSmlMessageQueue::CreateJobL() :Uimode result"));
		
		TInt count = aInfo.TaskIds().Count();
		const RArray<TInt>& tasks = aInfo.TaskIds();
		const RArray<TSmlSyncType>& syncTypes = aInfo.TaskSyncTypes();
		
		TNSmlContentTypeInfo info;
		
		for (TInt index = 0; index < count; index++)
			{
			info.iTaskId = tasks[index];
			info.iSyncType = syncTypes[index];
			jobInfo.iContentType->AppendL(info);
			}
		iAlertObserver->CreateJobL( jobInfo );
		}
	
	//Check next alert message
	iAlertIterator.SetToFirst();
	CNSmlMessageItem* item = iAlertIterator;
	
	aQuit = (item == NULL);
	
	if ( iState == ENSmlSuspended )
		{
		aQuit = ETrue;
		}
	else
		{
		iState = ENSmlIdle;		
		}
	_DBG_FILE("CNSmlMessageQueue::CreateJobL: end!");
	}

// -----------------------------------------------------------------------------
// CNSmlMessageQueue::DoDisconnect()
// -----------------------------------------------------------------------------
//
void CNSmlMessageQueue::DoDisconnect()
	{
	TRAP_IGNORE( iAlertObserver->DoDisconnectL() );
	}

// -----------------------------------------------------------------------------
// CNSmlMessageQueue::DestroyItems()
// -----------------------------------------------------------------------------
//
void CNSmlMessageQueue::DestroyItems()
	{
	CNSmlMessageItem* item;
	
	iAlertIterator.SetToFirst();
	
	while ( ( item = iAlertIterator++ ) != NULL )
        {
        item->iDlink.Deque();
        delete item;
        item = NULL;
        };

	}

// ---------------------------------------------------------
// CNSmlMessageQueue::CNSmlMessageQueue( MNSmlAlertObserver* aObserver )
// Constructor
// ---------------------------------------------------------
EXPORT_C CNSmlMessageQueue::CNSmlMessageQueue( MNSmlAlertObserver* aObserver )
: iAlertQueue( CNSmlMessageItem::iOffset ),iAlertIterator( iAlertQueue ), iAlertObserver( aObserver )
	{
	iState = ENSmlIdle;
	}

// ---------------------------------------------------------
// CNSmlMessageQueue::ConstructL()
// Second phase constructor
// ---------------------------------------------------------
void CNSmlMessageQueue::ConstructL()
	{
	iAlertHandler = CNSmlAlertHandler::NewL( this );
	}
