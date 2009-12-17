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
* Description:  Implementation of common client api.
*
*/


// INCLUDE FILES

#include <e32base.h>
#include <s32mem.h>
#include <eikdll.h>
#include <f32file.h>
#include <ecom.h>
#include <implementationinformation.h>
#include <featmgr.h>
#include <SyncMLClient.h>

#include <nsmlconstants.h>
#include <nsmldebug.h>
#include <nsmldsconstants.h>
#include "NSmlClientAPIDefs.h"
#include "NSmlClientAPIUtils.h"
#include "NSmlClientAPIActiveCallback.h"
#include "nsmlsosserverdefs.h"


// ============================= LOCAL FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// LaunchServerL()
// Launches the server.
// -----------------------------------------------------------------------------
//
TInt LaunchServerL()
	{	
	_DBG_FILE("SyncML Client API LaunchServerL(): begin");
	
	TInt res = KErrNone;

	// DLL launch
	RProcess server;
	res = server.Create(KServerExeName, KNullDesC);

	// Loading failed.
	User::LeaveIfError( res );
	
	TRequestStatus status;
	server.Rendezvous(status);

	if (status != KRequestPending)
		{
		server.Kill(0);		// abort startup
		server.Close();
		return KErrGeneral;   // status can be KErrNone: don't return status.Int()
		}
	else
		{
		server.Resume();	// logon OK - start the server
		}
		
	User::WaitForRequest(status);
	
	server.Close();

	_DBG_FILE("SyncML Client API LaunchServerL(): end");
	
	return status.Int();
	}
	

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// RSyncMLSession::RSyncMLSession()
// Constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C RSyncMLSession::RSyncMLSession()
	: RSessionBase(), iData( NULL ), iEventCallback( NULL )
	{
	}

// -----------------------------------------------------------------------------
// RSyncMLSession::OpenL()
// Opens sync session.
// -----------------------------------------------------------------------------
//
EXPORT_C void RSyncMLSession::OpenL()
	{
	_DBG_FILE("RSyncMLSession::OpenL(): begin");
	
	CClientSessionData::PanicIfAlreadyCreated( iData );
	
	const TVersion KServerVersion( KNSmlSOSServerVersionMajor, KNSmlSOSServerVersionMinor, 0 );
	
	TInt res = KErrNone;
	res = CreateSession( KSOSServerName, KServerVersion, KDefaultMessageSlots );
	
	if ( res != KErrNone )
		{
		res = LaunchServerL();
		User::LeaveIfError( res );
		res = CreateSession( KSOSServerName, KServerVersion, KDefaultMessageSlots );
		}

	User::LeaveIfError( res );
	
	CleanupClosePushL( *this ); // if leave occures, Close is called by cleanup
	
	// init feature manager and data
	FeatureManager::InitializeLibL();
	iData = new (ELeave) CClientSessionData;
	
	CleanupStack::Pop(); // this
	_DBG_FILE("RSyncMLSession::OpenL(): end");
	}

// -----------------------------------------------------------------------------
// RSyncMLSession::RequestEventL()
// Begins notifing the given observer of sync events.
// -----------------------------------------------------------------------------
//
EXPORT_C void RSyncMLSession::RequestEventL( MSyncMLEventObserver& aEventObserver )
	{
	_DBG_FILE("RSyncMLSession::RequestEventL(): begin");
	
	if ( !iEventCallback )
		{
		// callback not yet created, create
		iEventCallback = new (ELeave) CSmlActiveCallback( *this );
		}
	
	iEventCallback->SetEventObserverL( aEventObserver );
	
	_DBG_FILE("RSyncMLSession::RequestEventL(): end");
	}
	
// -----------------------------------------------------------------------------
// RSyncMLSession::CancelEvent()
// Cancels event notification.
// -----------------------------------------------------------------------------
//
EXPORT_C void RSyncMLSession::CancelEvent()
	{
	_DBG_FILE("RSyncMLSession::CancelEvent(): begin");
	
	if ( iEventCallback )
		{
		iEventCallback->CancelEvent();
		}
	
	_DBG_FILE("RSyncMLSession::CancelEvent(): end");
	}

// -----------------------------------------------------------------------------
// RSyncMLSession::RequestProgressL()
// Begins notifing the given observer of sync progress.
// -----------------------------------------------------------------------------
//
EXPORT_C void RSyncMLSession::RequestProgressL( MSyncMLProgressObserver& aProgressObserver )
	{
	_DBG_FILE("RSyncMLSession::RequestProgressL(): begin");
	
	if ( !iEventCallback )
		{
		// callback not yet created, create
		iEventCallback = new (ELeave) CSmlActiveCallback( *this );
		}

	iEventCallback->SetProgressObserverL( aProgressObserver );
	
	_DBG_FILE("RSyncMLSession::RequestProgressL(): end");
	}
	
// -----------------------------------------------------------------------------
// RSyncMLSession::CancelProgress()
// Cancels progress notification.
// -----------------------------------------------------------------------------
//
EXPORT_C void RSyncMLSession::CancelProgress()
	{
	_DBG_FILE("RSyncMLSession::CancelProgress(): begin");
	
	if ( iEventCallback )
		{
		iEventCallback->CancelProgress();
		}
	
	_DBG_FILE("RSyncMLSession::CancelProgress(): end");
	}

// -----------------------------------------------------------------------------
// RSyncMLSession::CurrentJobL()
// Fetches the id and usage type of currently running job.
// -----------------------------------------------------------------------------
//
EXPORT_C void RSyncMLSession::CurrentJobL( TSmlJobId& aJobId, TSmlUsageType& aUsageType ) const
	{
	_DBG_FILE("RSyncMLSession::CurrentJobL(): begin");
	
	TPckgBuf<TInt> jobId( aJobId );
	TPckgBuf<TInt> usageType( (TInt)aUsageType );
	
	User::LeaveIfError( SendReceive( ECmdJobListCurrent, TIpcArgs( &jobId, &usageType ) ) );
	
	aJobId = jobId();
	aUsageType = (TSmlUsageType) usageType();
	
	_DBG_FILE("RSyncMLSession::CurrentJobL(): end");
	}
	
// -----------------------------------------------------------------------------
// RSyncMLSession::ListQueuedJobsL()
// List the jobs that are currently in server's job queue.
// -----------------------------------------------------------------------------
//
EXPORT_C void RSyncMLSession::ListQueuedJobsL( RArray<TSmlJobId>& aArray, TSmlUsageType aType ) const
	{
	_DBG_FILE("RSyncMLSession::ListQueuedJobsL(): begin");
	
	// check that usage type is supported
	NSmlClientAPIFeatureHandler::LeaveIfUsageTypeNotSupportedL( aType );
	
	ReceiveArrayL( aArray, ECmdJobListStart, ECmdJobListNext, aType );
	
	_DBG_FILE("RSyncMLSession::ListQueuedJobsL(): end");
	}

// -----------------------------------------------------------------------------
// RSyncMLSession::ListProfilesL()
// List the profiles of given usage type.
// -----------------------------------------------------------------------------
//
EXPORT_C void RSyncMLSession::ListProfilesL( RArray<TSmlProfileId>& aArray, TSmlUsageType aType ) const
	{
	_DBG_FILE("RSyncMLSession::ListProfilesL(): begin");
	
	// check that usage type is supported
	NSmlClientAPIFeatureHandler::LeaveIfUsageTypeNotSupportedL( aType );
	
	ReceiveArrayL( aArray, ECmdProfileListStart, ECmdProfileListNext, aType );
	
	_DBG_FILE("RSyncMLSession::ListProfilesL(): end");
	}
	
// -----------------------------------------------------------------------------
// RSyncMLSession::ListTransportsL()
// Lists the transports available. Uses FeatureManager to find out
// which transports are at use.
// -----------------------------------------------------------------------------
//
EXPORT_C void RSyncMLSession::ListTransportsL( RArray<TSmlTransportId>& aTransportIds ) const
	{
	_DBG_FILE("RSyncMLSession::ListTransportsL(): begin");
	aTransportIds.Reset();
	
	//Checking if internet transport type is supported
	if ( FeatureManager::FeatureSupported( KFeatureIdSyncMlDsOverHttp  ) )
		{
	    aTransportIds.AppendL( KUidNSmlMediumTypeInternet.iUid );
		}
		
	// if obex is supported, see which obex transports are supported
	if ( FeatureManager::FeatureSupported( KFeatureIdSyncMlObex ) )
		{
		// bluetooth
		if ( FeatureManager::FeatureSupported( KFeatureIdBt ) )
			{
			aTransportIds.AppendL( KUidNSmlMediumTypeBluetooth.iUid );
			}
		}
		
	_DBG_FILE("RSyncMLSession::ListTransportsL(): end");
	}

// -----------------------------------------------------------------------------
// RSyncMLSession::ListDataProvidersL()
// Lists the DS adapters available. Uses ECom services 
// for the task.
// -----------------------------------------------------------------------------
//
EXPORT_C void RSyncMLSession::ListDataProvidersL( RArray<TSmlDataProviderId>& aDataProviderIds ) const
	{
	_DBG_FILE("RSyncMLSession::ListDataProvidersL(): begin");
	
	// check that ds sync is supported
	NSmlClientAPIFeatureHandler::LeaveIfDataSyncNotSupportedL();
	
	aDataProviderIds.Reset();
	
	RImplInfoPtrArray implArray;
	CleanupStack::PushL( PtrArrCleanupItemRArr( CImplementationInformation, &implArray ) );
	TUid ifUid = { KNSmlDSInterfaceUid };
	REComSession::ListImplementationsL( ifUid, implArray );
	
	// append adapter id's to the given array
	for ( TInt i = 0; i < implArray.Count(); i++ )
		{
		CImplementationInformation* implInfo = implArray[i];
		aDataProviderIds.AppendL( implInfo->ImplementationUid().iUid );
		}
	
	REComSession::FinalClose();
	CleanupStack::PopAndDestroy(); // implArray
	
	_DBG_FILE("RSyncMLSession::ListDataProvidersL(): end");
	}

// -----------------------------------------------------------------------------
// RSyncMLSession::DeleteProfileL()
// Deletes the given profile.
// -----------------------------------------------------------------------------
//
EXPORT_C void RSyncMLSession::DeleteProfileL( TSmlProfileId aId )
	{
	_DBG_FILE("RSyncMLSession::DeleteProfileL(): begin");
	
    TIpcArgs args( aId );
    User::LeaveIfError( SendReceive( ECmdProfileDelete, args ) );
    
	_DBG_FILE("RSyncMLSession::DeleteProfileL(): end");
	}

// -----------------------------------------------------------------------------
// RSyncMLSession::ListProtocolVersionsL()
// Returns the default supported protocol for given usage type. 
// On return, aVersions array includes all the protocol 
// versions supported.
// -----------------------------------------------------------------------------
//
EXPORT_C TSmlProtocolVersion RSyncMLSession::ListProtocolVersionsL( TSmlUsageType aUsageType, RArray<TSmlProtocolVersion>& aVersions ) const
	{
	_DBG_FILE("RSyncMLSession::ListProtocolVersionsL(): begin");
	
	// check that given usage type is supported
	NSmlClientAPIFeatureHandler::LeaveIfUsageTypeNotSupportedL( aUsageType );
	
	TSmlProtocolVersion defaultVersion( ESmlVersion1_1_2 );
	
	switch ( aUsageType )
		{
		case ESmlDataSync:
			defaultVersion = ESmlVersion1_2;
			aVersions.AppendL( ESmlVersion1_2 );
			break;
		case ESmlDevMan:
		if(!FeatureManager::FeatureSupported( KFeatureIdSyncMlDm112  ))
		{
			defaultVersion = ESmlVersion1_2;
			aVersions.AppendL( ESmlVersion1_2 );
		}
		else
		{
			defaultVersion = ESmlVersion1_1_2;
		}
			break;
		default:
			break;
		}

	// both dm and ds support 1.1.2
	aVersions.AppendL( ESmlVersion1_1_2 );
	
	_DBG_FILE("RSyncMLSession::ListProtocolVersionsL(): end");
	
	return defaultVersion;
	}

// -----------------------------------------------------------------------------
// RSyncMLSession::Close()
// Closes the session.
// -----------------------------------------------------------------------------
//
EXPORT_C void RSyncMLSession::Close()
	{
	_DBG_FILE("RSyncMLSession::Close(): begin");
	
	// uninitialize feature manager
	FeatureManager::UnInitializeLib();

	delete iEventCallback;
	iEventCallback = NULL;
	
	delete iData;
	iData = NULL;
	
	// close session	
	RSessionBase::Close();
	
	_DBG_FILE("RSyncMLSession::Close(): end");
	}

// -----------------------------------------------------------------------------
// RSyncMLSession::ReceiveArrayL()
// Private utility method for receiving arrays of integers.
// -----------------------------------------------------------------------------
//
void RSyncMLSession::ReceiveArrayL( RArray<TInt>& aArray, TInt aStart, TInt aNext, TInt aUsageType ) const
	{
	_DBG_FILE("RSyncMLSession::ReceiveArrayL(): begin");
	
	aArray.Reset();
	
	iData->SetBufferSizeL( KNSmlMaxIntFetchCount*KSizeofTInt32 + KSizeofTInt8 );
	TPtr8& dataPtr = iData->DataBufferPtr();

	// IPC
    TIpcArgs args( &dataPtr, aUsageType );
    TInt err = SendReceive( aStart, args );

	if ( err == KErrNone || err == KErrOverflow )
		{
		iData->ReadIntegersToArrayL( aArray ); // integers to array
		
		while ( err == KErrOverflow )
			{
			dataPtr.Zero();
			
			// new IPC
			args.Set( 0, &dataPtr );
		    err = SendReceive( aNext, args );
		    
		    if ( err == KErrNone || err == KErrOverflow )
		    	{
				iData->ReadIntegersToArrayL( aArray ); // integers to array
		    	}
			}
		}
	
	User::LeaveIfError( err );
	
	_DBG_FILE("RSyncMLSession::ReceiveArrayL(): end");
	}


//
// RSyncMLSettings ( Not Supported )
//

// -----------------------------------------------------------------------------
// RSyncMLSettings::OpenL()
// -----------------------------------------------------------------------------
//
EXPORT_C void RSyncMLSettings::OpenL( RSyncMLSession& )
	{
	User::Leave( KErrNotSupported );
	}
	
// -----------------------------------------------------------------------------
// RSyncMLSettings::GetValueL()
// -----------------------------------------------------------------------------
//
EXPORT_C TInt RSyncMLSettings::GetValueL( TSmlGlobalSetting ) const
	{
	User::Leave( KErrNotSupported );
	return 0;
	}
	
// -----------------------------------------------------------------------------
// RSyncMLSettings::SetValueL()
// -----------------------------------------------------------------------------
//
EXPORT_C void RSyncMLSettings::SetValueL( TSmlGlobalSetting, TInt )
	{
	User::Leave( KErrNotSupported );
	}
	
// -----------------------------------------------------------------------------
// RSyncMLSettings::Close()
// -----------------------------------------------------------------------------
//
EXPORT_C void RSyncMLSettings::Close()
	{
	}
	

//
// RSyncMLHistoryLog
//


// -----------------------------------------------------------------------------
// RSyncMLHistoryLog::RSyncMLHistoryLog()
// Constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C RSyncMLHistoryLog::RSyncMLHistoryLog()
	: RSubSessionBase(), iData( NULL )
	{
	}
	
// -----------------------------------------------------------------------------
// RSyncMLHistoryLog::OpenL()
// Opens history log sub-session.
// -----------------------------------------------------------------------------
//
EXPORT_C void RSyncMLHistoryLog::OpenL( RSyncMLSession& aSession, TSmlProfileId aProfileId )
	{
	_DBG_FILE("RSyncMLHistoryLog::OpenL(): begin");
	
	CClientSessionData::PanicIfAlreadyCreated( iData );
	
	CHistoryLogSessionData* data = CHistoryLogSessionData::NewLC();
	
	TPckgBuf<TInt> dataSize;
	TIpcArgs args( &dataSize, aProfileId );
    
	// open the sub-session
	User::LeaveIfError( CreateSubSession( aSession, ECmdHistoryLogOpen, args ) );
	
	data->SetProfile( aProfileId );
	
	iData = data;
	CleanupStack::Pop(); // data
	
	CleanupClosePushL( *this ); // if leave occures, Close is called by cleanup
	
	if ( dataSize() > 0 )
		{
		// dataSize contains now the size of the data buffer that is received next
		iData->SetBufferSizeL( dataSize() );
		TPtr8 dataPtr = iData->DataBufferPtr();
		
		TIpcArgs args2( &dataPtr );
		User::LeaveIfError( SendReceive( ECmdHistoryLogGet, args2 ) );
		
		// internalize the buffer to data
		RDesReadStream readStream;
		readStream.Open( dataPtr );
		CleanupClosePushL( readStream );
		iData->InternalizeL( readStream );

		CleanupStack::PopAndDestroy(); // readStream
		}
		
	CleanupStack::Pop(); // this
	
	_DBG_FILE("RSyncMLHistoryLog::OpenL(): end");
	}
	
// -----------------------------------------------------------------------------
// RSyncMLHistoryLog::SortEntries()
// Sorts history log entries according to the given order.
// -----------------------------------------------------------------------------
//
EXPORT_C void RSyncMLHistoryLog::SortEntries( CSyncMLHistoryEntry::TSortOrder aSortOrder )
	{
	_DBG_FILE("RSyncMLHistoryLog::SortEntries(): begin");
	static_cast<CHistoryLogSessionData*>(iData)->History().SortEntries( aSortOrder );
	_DBG_FILE("RSyncMLHistoryLog::SortEntries(): end");
	}
	
// -----------------------------------------------------------------------------
// RSyncMLHistoryLog::DeleteAllEntriesL()
// Empties the history for this profile (from both server and client 
// side buffer).
// -----------------------------------------------------------------------------
//
EXPORT_C void RSyncMLHistoryLog::DeleteAllEntriesL()
	{
	_DBG_FILE("RSyncMLHistoryLog::DeleteAllEntriesL(): begin");
	
	// send reset to server
	User::LeaveIfError( SendReceive( ECmdHistoryLogReset ) );
	
	// delete entries from buffer
	static_cast<CHistoryLogSessionData*>(iData)->History().DeleteAllEntries();
	
	_DBG_FILE("RSyncMLHistoryLog::DeleteAllEntriesL(): end");
	}
	
// -----------------------------------------------------------------------------
// RSyncMLHistoryLog::Profile()
// Returns the id of the profile that was given in OpenL, i.e the profile
// whose history has been opened.
// -----------------------------------------------------------------------------
//
EXPORT_C TSmlProfileId RSyncMLHistoryLog::Profile() const
	{
	return static_cast<CHistoryLogSessionData*>(iData)->Profile();
	}
	
// -----------------------------------------------------------------------------
// RSyncMLHistoryLog::Count()
// Returns the count of history entries.
// -----------------------------------------------------------------------------
//
EXPORT_C TInt RSyncMLHistoryLog::Count() const
	{
	return static_cast<CHistoryLogSessionData*>(iData)->History().Count();
	}
	
// -----------------------------------------------------------------------------
// RSyncMLHistoryLog::Entry()
// Returns the history entry in the given index.
// -----------------------------------------------------------------------------
//
EXPORT_C const CSyncMLHistoryEntry& RSyncMLHistoryLog::Entry( TInt aIndex ) const
	{
	return static_cast<CHistoryLogSessionData*>(iData)->History().Entry( aIndex );
	}
	
// -----------------------------------------------------------------------------
// RSyncMLHistoryLog::Close()
// Close sub-session.
// -----------------------------------------------------------------------------
//
EXPORT_C void RSyncMLHistoryLog::Close()
	{
	_DBG_FILE("RSyncMLHistoryLog::Close(): begin");
	CloseSubSession( ECmdHistoryLogClose );
	
	delete iData;
	iData = NULL;
	_DBG_FILE("RSyncMLHistoryLog::Close(): end");
	}
	


//
// RSyncMLJobBase
//


// -----------------------------------------------------------------------------
// RSyncMLJobBase::Identifier()
// Return the id of the job.
// -----------------------------------------------------------------------------
//
EXPORT_C TSmlJobId RSyncMLJobBase::Identifier() const
	{
	return static_cast<CJobBaseSessionData*>(iData)->Identifier();
	}
	
// -----------------------------------------------------------------------------
// RSyncMLJobBase::Profile()
// Returns the id of the profile used in the job.
// -----------------------------------------------------------------------------
//
EXPORT_C TSmlProfileId RSyncMLJobBase::Profile() const
	{
	return static_cast<CJobBaseSessionData*>(iData)->Profile();
	}
	
// -----------------------------------------------------------------------------
// RSyncMLJobBase::Close()
// Closes sub-session.
// -----------------------------------------------------------------------------
//
EXPORT_C void RSyncMLJobBase::Close()
	{
	_DBG_FILE("RSyncMLJobBase::Close(): begin");
	CloseSubSession( ECmdJobClose );
	
	delete iData;
	iData = NULL;
	_DBG_FILE("RSyncMLJobBase::Close(): end");
	}
	
// -----------------------------------------------------------------------------
// RSyncMLJobBase::RSyncMLJobBase()
// Constructor.
// -----------------------------------------------------------------------------
//
RSyncMLJobBase::RSyncMLJobBase()
	: RSubSessionBase(), iData( NULL )
	{
	}
	
// -----------------------------------------------------------------------------
// RSyncMLJobBase::GetJobL()
// Fetches the job's data from server.
// -----------------------------------------------------------------------------
//
void RSyncMLJobBase::GetJobL( TInt aSize, TBool )
	{
	_DBG_FILE("RSyncMLJobBase::GetJobL(): begin");
	
	// Create data buffer
	iData->SetBufferSizeL( aSize );
	TPtr8 dataPtr = iData->DataBufferPtr();
	
	TIpcArgs args( &dataPtr );
	User::LeaveIfError( SendReceive( ECmdJobGet, args ) );
	
	// internalize data to iData
	RDesReadStream readStream;
	readStream.Open( dataPtr );
	CleanupClosePushL( readStream );
	iData->InternalizeL( readStream );

	CleanupStack::PopAndDestroy(); // readStream
	
	_DBG_FILE("RSyncMLJobBase::GetJobL(): end");
	}
	

//
// RSyncMLProfileBase
//

// -----------------------------------------------------------------------------
// RSyncMLProfileBase::SetCreatorId()
// Sets the creator id to data buffer.
// -----------------------------------------------------------------------------
//
EXPORT_C void RSyncMLProfileBase::SetCreatorId( TSmlCreatorId aCreatorId )
	{
	static_cast<CProfileBaseSessionData*>(iData)->SetCreatorId( aCreatorId );
	}
	
// -----------------------------------------------------------------------------
// RSyncMLProfileBase::SetDisplayNameL()
// Sets the name of the profile to data buffer.
// -----------------------------------------------------------------------------
//
EXPORT_C void RSyncMLProfileBase::SetDisplayNameL( const TDesC& aDisplayName )
	{
	static_cast<CProfileBaseSessionData*>(iData)->SetDisplayNameL( aDisplayName );
	}
	
// -----------------------------------------------------------------------------
// RSyncMLProfileBase::SetUserNameL()
// Sets the username used with remote server to data buffer.
// -----------------------------------------------------------------------------
//
EXPORT_C void RSyncMLProfileBase::SetUserNameL( const TDesC8& aUserName )
	{
	static_cast<CProfileBaseSessionData*>(iData)->SetUserNameL( aUserName );
	}
	
// -----------------------------------------------------------------------------
// RSyncMLProfileBase::SetPasswordL()
// Sets the user's password on the remote sync server to data
// buffer.
// -----------------------------------------------------------------------------
//
EXPORT_C void RSyncMLProfileBase::SetPasswordL( const TDesC8& aPassword )
	{
	static_cast<CProfileBaseSessionData*>(iData)->SetPasswordL( aPassword );
	}
	
// -----------------------------------------------------------------------------
// RSyncMLProfileBase::SetServerIdL()
// Sets the server id.
// -----------------------------------------------------------------------------
//
EXPORT_C void RSyncMLProfileBase::SetServerIdL( const TDesC8& aServerId )
	{
	static_cast<CProfileBaseSessionData*>(iData)->SetServerIdL( aServerId );
	}
	
// -----------------------------------------------------------------------------
// RSyncMLProfileBase::SetServerPasswordL()
// Sets server passwd.
// -----------------------------------------------------------------------------
//
EXPORT_C void RSyncMLProfileBase::SetServerPasswordL( const TDesC8& aServerPassword )
	{
	static_cast<CProfileBaseSessionData*>(iData)->SetServerPasswordL( aServerPassword );
	}
	
// -----------------------------------------------------------------------------
// RSyncMLProfileBase::SetSanUserInteractionL()
// Sets server alerted user interaction.
// -----------------------------------------------------------------------------
//
EXPORT_C void RSyncMLProfileBase::SetSanUserInteractionL( TSmlServerAlertedAction aSanAction )
	{
	static_cast<CProfileBaseSessionData*>(iData)->SetSanAction( aSanAction );
	}
	
// -----------------------------------------------------------------------------
// RSyncMLProfileBase::SetProtocolVersionL()
// Sets the SyncML protocol version used with this profile.
// -----------------------------------------------------------------------------
//
EXPORT_C void RSyncMLProfileBase::SetProtocolVersionL( TSmlProtocolVersion aProtocolVersion )
	{
	static_cast<CProfileBaseSessionData*>(iData)->SetProtocolVersionL( aProtocolVersion );
	}
	
// -----------------------------------------------------------------------------
// RSyncMLProfileBase::DeleteConnectionL()
// Since only one connection is supported with a profile, 
// the connection cannot be deleted. Leaves with KErrNotSupported.
// -----------------------------------------------------------------------------
//
EXPORT_C void RSyncMLProfileBase::DeleteConnectionL( TSmlTransportId /*aConnection*/ )
	{
	_DBG_FILE("RSyncMLProfileBase::DeleteConnectionL(): begin");
	
	User::Leave( KErrNotSupported );
	
	_DBG_FILE("RSyncMLProfileBase::DeleteConnectionL(): end");
	}
	
// -----------------------------------------------------------------------------
// RSyncMLProfileBase::DeleteConnectionByConnectionIdL()
// Since only one connection is supported with a profile, 
// the connection cannot be deleted. Leaves with KErrNotSupported.
// -----------------------------------------------------------------------------
//
EXPORT_C void RSyncMLProfileBase::DeleteConnectionByConnectionIdL( TSmlConnectionId aConnection )
	{
	DeleteConnectionL( aConnection );
	}
	
// -----------------------------------------------------------------------------
// RSyncMLProfileBase::Identifier()
// Returns the id of this profile.
// -----------------------------------------------------------------------------
//
EXPORT_C TSmlProfileId RSyncMLProfileBase::Identifier() const
	{
	return static_cast<CProfileBaseSessionData*>(iData)->Identifier();
	}
	
// -----------------------------------------------------------------------------
// RSyncMLProfileBase::Type()
// Returns the type of this profile (DS or DM).
// -----------------------------------------------------------------------------
//
EXPORT_C TSmlUsageType RSyncMLProfileBase::Type() const
	{
	return static_cast<CProfileBaseSessionData*>(iData)->Type();
	}
	
// -----------------------------------------------------------------------------
// RSyncMLProfileBase::CreatorId()
// Returns the creator id.
// -----------------------------------------------------------------------------
//
EXPORT_C TSmlCreatorId RSyncMLProfileBase::CreatorId() const
	{
	return static_cast<CProfileBaseSessionData*>(iData)->CreatorId();
	}
	
// -----------------------------------------------------------------------------
// RSyncMLProfileBase::DisplayName()
// Returns the name of this profile.
// -----------------------------------------------------------------------------
//
EXPORT_C const TDesC& RSyncMLProfileBase::DisplayName() const
	{
	return static_cast<CProfileBaseSessionData*>(iData)->DisplayName();
	}
	
// -----------------------------------------------------------------------------
// RSyncMLProfileBase::UserName()
// Returns the username used with the remote server.
// -----------------------------------------------------------------------------
//
EXPORT_C const TDesC8& RSyncMLProfileBase::UserName() const
	{
	return static_cast<CProfileBaseSessionData*>(iData)->UserName();
	}
	
// -----------------------------------------------------------------------------
// RSyncMLProfileBase::Password()
// Returns the user's password to remote server.
// -----------------------------------------------------------------------------
//
EXPORT_C const TDesC8& RSyncMLProfileBase::Password() const
	{
	return static_cast<CProfileBaseSessionData*>(iData)->Password();
	}
	
// -----------------------------------------------------------------------------
// RSyncMLProfileBase::ServerId()
// Returns the server id.
// -----------------------------------------------------------------------------
//
EXPORT_C const TDesC8& RSyncMLProfileBase::ServerId() const
	{
	return static_cast<CProfileBaseSessionData*>(iData)->ServerId();
	}
	
// -----------------------------------------------------------------------------
// RSyncMLProfileBase::ServerPassword()
// Returns the server password.
// -----------------------------------------------------------------------------
//
EXPORT_C const TDesC8& RSyncMLProfileBase::ServerPassword() const
	{
	return static_cast<CProfileBaseSessionData*>(iData)->ServerPassword();
	}
	
// -----------------------------------------------------------------------------
// RSyncMLProfileBase::SanUserInteraction()
// Return server alerted user interaction value.
// -----------------------------------------------------------------------------
//
EXPORT_C TSmlServerAlertedAction RSyncMLProfileBase::SanUserInteraction() const
	{
	return static_cast<CProfileBaseSessionData*>(iData)->SanAction();
	}
	
// -----------------------------------------------------------------------------
// RSyncMLProfileBase::ProtocolVersion()
// Returns the version of the syncML protocol used with this 
// profile.
// -----------------------------------------------------------------------------
//
EXPORT_C TSmlProtocolVersion RSyncMLProfileBase::ProtocolVersion() const
	{
	return static_cast<CProfileBaseSessionData*>(iData)->ProtocolVersion();
	}
	
// -----------------------------------------------------------------------------
// RSyncMLProfileBase::ListConnectionsL()
// Returns the list of connections supported by this profile. 
// Since currently only one connection per profile is supported, 
// the list contains only one connection on return.
// -----------------------------------------------------------------------------
//
EXPORT_C void RSyncMLProfileBase::ListConnectionsL( RArray<TSmlTransportId>& aArray ) const
	{
	_DBG_FILE("RSyncMLProfileBase::ListConnectionsL(): begin");
	
	// there should never be more than one connection per profile, ECmdConnList used
	// also as "next" command
	ReceiveArrayL( aArray, ECmdConnList, ECmdConnList );
	
	_DBG_FILE("RSyncMLProfileBase::ListConnectionsL(): end");
	}

// -----------------------------------------------------------------------------
// RSyncMLProfileBase::ListConnectionsByConnectionIdL(
// Lists connections.
// -----------------------------------------------------------------------------
//
EXPORT_C void RSyncMLProfileBase::ListConnectionsByConnectionIdL( RArray<TSmlConnectionId>& aArray ) const
	{
	_DBG_FILE("RSyncMLProfileBase::ListConnectionsByConnectionIdL(): begin");
	
	ListConnectionsL( aArray );
	
	_DBG_FILE("RSyncMLProfileBase::ListConnectionsByConnectionIdL(): end");
	}
	
// -----------------------------------------------------------------------------
// RSyncMLProfileBase::IsReadOnly()
// Returns ETrue if this profile has been opened in read-only mode, 
// EFalse otherwise.
// -----------------------------------------------------------------------------
//
EXPORT_C TBool RSyncMLProfileBase::IsReadOnly() const
	{
	return ( static_cast<CProfileBaseSessionData*>(iData)->OpenMode() == ESmlOpenRead );
	}

// -----------------------------------------------------------------------------
// RSyncMLProfileBase::DeleteAllowed()
// Returns whether or not deletion is allowed.
// -----------------------------------------------------------------------------
//
EXPORT_C TBool RSyncMLProfileBase::DeleteAllowed() const
	{
	return static_cast<CProfileBaseSessionData*>(iData)->DeleteAllowed();
	}

// -----------------------------------------------------------------------------
// RSyncMLProfileBase::ProfileLocked()
// Returns whether or not profile is locked or not .
// -----------------------------------------------------------------------------
//
EXPORT_C TInt RSyncMLProfileBase::ProfileLocked(TBool aReadWriteValue, TBool aLockValue) const
	{
		if(Type() == ESmlDataSync)
       return  KErrNotSupported;

	if (aReadWriteValue == EFalse)	
	  { 
	  	return static_cast<CProfileBaseSessionData*>(iData)->ProfileLocked();
	  }	
	else
	  {
	   static_cast<CProfileBaseSessionData*>(iData)->SetProfileLock(aLockValue);
	   return KErrNone;
	  }
	      
	}
// -----------------------------------------------------------------------------
// RSyncMLProfileBase::UpdateL()
// Sends the data in iData to server. The data is updated to 
// the profile in settings db.
// -----------------------------------------------------------------------------
//
EXPORT_C void RSyncMLProfileBase::UpdateL()
	{
	_DBG_FILE("RSyncMLProfileBase::UpdateL(): begin");
	
	// check that we're allowed to write
	if ( IsReadOnly() )
		{
		User::Leave( KErrLocked );
		}
	
	// use iData as a buffer where the data is externalized to
	iData->SetBufferSizeL();
	
    RBufWriteStream bufStream( iData->DataBuffer() );
	CleanupClosePushL( bufStream );
	
	// externalize
	iData->ExternalizeL( bufStream );
	
	TPtr8 bufPtr = iData->DataBufferPtr();
	// update data to server
	TPckgBuf<TInt> id;
	TIpcArgs args( &id, &bufPtr );
    User::LeaveIfError( SendReceive( ECmdProfileSet, args ) );

	static_cast<CProfileBaseSessionData*>(iData)->SetIdentifier( id() );

	CleanupStack::PopAndDestroy(); // bufStream
	
	_DBG_FILE("RSyncMLProfileBase::UpdateL(): end");
	}

// -----------------------------------------------------------------------------
// RSyncMLProfileBase::Close()
// Closes the sub-session to server side profile and deletes 
// the client side data buffer.
// -----------------------------------------------------------------------------
//
EXPORT_C void RSyncMLProfileBase::Close()
	{
	_DBG_FILE("RSyncMLProfileBase::Close(): begin");
	CloseSubSession( ECmdProfileClose );
	
	delete iData;
	iData = NULL;
	_DBG_FILE("RSyncMLProfileBase::Close(): end");
	}

// -----------------------------------------------------------------------------
// RSyncMLProfileBase::Session()
// Returns teh session object associated with this sub-session.
// -----------------------------------------------------------------------------
//
const RSessionBase RSyncMLProfileBase::Session()
	{
	return RSubSessionBase::Session();
	}


// -----------------------------------------------------------------------------
// RSyncMLProfileBase::RSyncMLProfileBase()
// Protected contructor, called by child classes.
// -----------------------------------------------------------------------------
//
RSyncMLProfileBase::RSyncMLProfileBase()
	: RSubSessionBase(), iData( NULL )
	{
	}
	
// -----------------------------------------------------------------------------
// RSyncMLProfileBase::GetProfileL()
// Fetches the profiles data.
// -----------------------------------------------------------------------------
//
void RSyncMLProfileBase::GetProfileL( TInt aSize )
	{
	_DBG_FILE("RSyncMLProfileBase::GetProfileL(): begin");
	
	// Create data buffer
	iData->SetBufferSizeL( aSize );
	TPtr8 bufPtr = iData->DataBufferPtr();
	
	TIpcArgs args( &bufPtr );
	User::LeaveIfError( SendReceive( ECmdProfileGet, args ) );
	
	// internalize the buffer to iData
	RDesReadStream readStream;
	readStream.Open( bufPtr );
	CleanupClosePushL( readStream );
	iData->InternalizeL( readStream );

	CleanupStack::PopAndDestroy(); // readStream
	
	_DBG_FILE("RSyncMLProfileBase::GetProfileL(): end");
	}

// -----------------------------------------------------------------------------
// RSyncMLProfileBase::ReceiveArrayL()
// Receives array of integers using the given IPC-commands.
// -----------------------------------------------------------------------------
//
void RSyncMLProfileBase::ReceiveArrayL( RArray<TInt>& aArray, TInt aStart, TInt aNext ) const
	{
	_DBG_FILE("RSyncMLProfileBase::ReceiveArrayL(): begin");
	
	aArray.Reset();
	
	// Create data buffer
	iData->SetBufferSizeL( KNSmlMaxIntFetchCount*KSizeofTInt32 + KSizeofTInt8 );
    TPtr8& dataPtr = iData->DataBufferPtr();

	// IPC
    TIpcArgs args( &dataPtr );
    TInt err = SendReceive( aStart, args );

	if ( err == KErrNone || err == KErrOverflow )
		{
		iData->ReadIntegersToArrayL( aArray ); // integers to array
		
		while ( err == KErrOverflow )
			{
			dataPtr.Zero();
			
			// new IPC
			args.Set( 0, &dataPtr );
		    err = SendReceive( aNext, args );
		    
		    if ( err == KErrNone || err == KErrOverflow )
		    	{
				iData->ReadIntegersToArrayL( aArray ); // integers to array
		    	}
			}
		}
	
	User::LeaveIfError( err );
	
	_DBG_FILE("RSyncMLProfileBase::ReceiveArrayL(): end");
	}


//
// RSyncMLConnection
//


// -----------------------------------------------------------------------------
// RSyncMLConnection::RSyncMLConnection()
// Constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C RSyncMLConnection::RSyncMLConnection()
	: RSubSessionBase(), iData( NULL )
	{
	}
	
// -----------------------------------------------------------------------------
// RSyncMLConnection::CreateL()
// Changes the transport used with this profile.
// -----------------------------------------------------------------------------
//
EXPORT_C void RSyncMLConnection::CreateL( RSyncMLProfileBase& aProfile, TSmlTransportId aIdentifier )
	{
	_DBG_FILE("RSyncMLConnection::CreateL(): begin");
	
	// check that profile did not change
	if ( aProfile.Identifier() != Profile() )
		{
		User::Leave( KErrArgument );
		}
	
	// check that profile is in read/write mode, i.e. we're allowed to write to profile
	if ( aProfile.IsReadOnly() )
		{
		User::Leave( KErrLocked );
		}
	
	// init iData
	delete iData;
	iData = NULL;
	iData = CConnectionSessionData::NewL( aIdentifier, aProfile.Type(), ETrue );
	
	// set profile id
	static_cast<CConnectionSessionData*>(iData)->SetProfile( aProfile.Identifier() );
	
	_DBG_FILE("RSyncMLConnection::CreateL(): end");
	}
	
// -----------------------------------------------------------------------------
// RSyncMLConnection::OpenL()
// Open a sub-session to sync server and fetches the connection 
// data of the given profile to client side buffer.
// -----------------------------------------------------------------------------
//
EXPORT_C void RSyncMLConnection::OpenL( RSyncMLProfileBase& aProfile, TSmlTransportId aIdentifier )
	{
	_DBG_FILE("RSyncMLConnection::OpenL(): begin");
	
	CClientSessionData::PanicIfAlreadyCreated( iData );
	
	CConnectionSessionData* data = CConnectionSessionData::NewLC( aIdentifier, aProfile.Type() );
	
	TPckgBuf<TInt> dataSize;
	TIpcArgs args( &dataSize, aProfile.Identifier(), aIdentifier );
    
	// open the sub-session
	RSessionBase session = static_cast<RSessionBase>( aProfile.Session() );
	User::LeaveIfError( CreateSubSession( session, ECmdConnOpen, args ) );
	
	// set profile id and open mode to data
	data->SetProfile( aProfile.Identifier() );
	if ( aProfile.IsReadOnly() )
		{
		data->SetOpenMode( ESmlOpenRead );
		}
	
	iData = data;
	CleanupStack::Pop(); // data
	
	CleanupClosePushL( *this ); // if leave occures, Close is called by cleanup
	
	// dataSize contains now the size of the data buffer that is received next
	iData->SetBufferSizeL( dataSize() );
	TPtr8 bufPtr = iData->DataBufferPtr();
	
	TIpcArgs args2(&bufPtr);
	User::LeaveIfError( SendReceive( ECmdConnGet, args2 ) );
	
	// internalize the buffer to data
	RDesReadStream readStream;
	readStream.Open( bufPtr );
	CleanupClosePushL( readStream );
	iData->InternalizeL( readStream );
	
	CleanupStack::PopAndDestroy(); // readStream
	CleanupStack::Pop(); // this
	
	_DBG_FILE("RSyncMLConnection::OpenL(): end");
	}
	
// -----------------------------------------------------------------------------
// RSyncMLConnection::OpenByConnectionIdL()
// Open a sub-session to sync server and fetches the connection 
// data of the given profile to client side buffer.
// -----------------------------------------------------------------------------
//
EXPORT_C void RSyncMLConnection::OpenByConnectionIdL( RSyncMLProfileBase& aProfile, TSmlConnectionId aIdentifier )
	{
	_DBG_FILE("RSyncMLConnection::OpenByConnectionIdL(): begin");
	OpenL( aProfile, aIdentifier );
	_DBG_FILE("RSyncMLConnection::OpenByConnectionIdL(): end");
	}
	
// -----------------------------------------------------------------------------
// RSyncMLConnection::Identifier()
// Returns the transport id of this connection.
// -----------------------------------------------------------------------------
//
EXPORT_C TSmlTransportId RSyncMLConnection::Identifier() const
	{
	return static_cast<CConnectionSessionData*>(iData)->Identifier();
	}
	
// -----------------------------------------------------------------------------
// RSyncMLConnection::Profile()
// Return the id of the profile who owns this connection.
// -----------------------------------------------------------------------------
//
EXPORT_C TSmlProfileId RSyncMLConnection::Profile() const
	{
	if ( !iData )
	    {
	    User::Leave( KErrArgument );    
	    }
	return static_cast<CConnectionSessionData*>(iData)->Profile();
	}
	
// -----------------------------------------------------------------------------
// RSyncMLConnection::ConnectionId()
// Return the id of this connection.
// -----------------------------------------------------------------------------
//
EXPORT_C TSmlConnectionId RSyncMLConnection::ConnectionId() const
	{
	return Identifier();
	}
	
// -----------------------------------------------------------------------------
// RSyncMLConnection::Priority()
// -----------------------------------------------------------------------------
//
EXPORT_C TUint RSyncMLConnection::Priority() const
	{
	return static_cast<CConnectionSessionData*>(iData)->Priority();
	}
	
// -----------------------------------------------------------------------------
// RSyncMLConnection::RetryCount()
// -----------------------------------------------------------------------------
//
EXPORT_C TUint RSyncMLConnection::RetryCount() const
	{
	return static_cast<CConnectionSessionData*>(iData)->RetryCount();
	}
	
// -----------------------------------------------------------------------------
// RSyncMLConnection::ServerURI()
// Return server uri.
// -----------------------------------------------------------------------------
//
EXPORT_C const TDesC8& RSyncMLConnection::ServerURI() const
	{
	return static_cast<CConnectionSessionData*>(iData)->ServerURI();
	}
	
// -----------------------------------------------------------------------------
// RSyncMLConnection::GetPropertyL()
// Return the value of the given transport property. If not found, leaves 
// with KErrNotFound.
// -----------------------------------------------------------------------------
//
EXPORT_C const TDesC8& RSyncMLConnection::GetPropertyL( const TDesC8& aName ) const
	{
	return static_cast<CConnectionSessionData*>(iData)->GetPropertyL( aName );
	}
	
// -----------------------------------------------------------------------------
// RSyncMLConnection::SetPriority()
// -----------------------------------------------------------------------------
//
EXPORT_C void RSyncMLConnection::SetPriority( TUint aPriority )
	{
	static_cast<CConnectionSessionData*>(iData)->SetPriority( aPriority );
	}
	
// -----------------------------------------------------------------------------
// RSyncMLConnection::SetRetryCount()
// -----------------------------------------------------------------------------
//
EXPORT_C void RSyncMLConnection::SetRetryCount( TUint aRetryCount )
	{
	static_cast<CConnectionSessionData*>(iData)->SetRetryCount( aRetryCount );
	}
	
// -----------------------------------------------------------------------------
// RSyncMLConnection::SetServerURIL()
// Sets server uri.
// -----------------------------------------------------------------------------
//
EXPORT_C void RSyncMLConnection::SetServerURIL( const TDesC8& aServerURI )
	{
	static_cast<CConnectionSessionData*>(iData)->SetServerURIL( aServerURI );
	}
	
// -----------------------------------------------------------------------------
// RSyncMLConnection::SetPropertyL()
// Sets a value for a transport property.
// -----------------------------------------------------------------------------
//
EXPORT_C void RSyncMLConnection::SetPropertyL( const TDesC8& aName, const TDesC8& aValue )
	{
	static_cast<CConnectionSessionData*>(iData)->SetPropertyL( aName, aValue );
	}
	
// -----------------------------------------------------------------------------
// RSyncMLConnection::IsReadOnly()
// Returns whether this connection is read-only or not.
// -----------------------------------------------------------------------------
//
EXPORT_C TBool RSyncMLConnection::IsReadOnly() const
	{
	return ( static_cast<CConnectionSessionData*>(iData)->OpenMode() == ESmlOpenRead );
	}
	
// -----------------------------------------------------------------------------
// RSyncMLConnection::UpdateL()
// Updates the data set to client-side buffer to server.
// -----------------------------------------------------------------------------
//
EXPORT_C void RSyncMLConnection::UpdateL()
	{
	_DBG_FILE("RSyncMLConnection::UpdateL(): begin");
	
	// check that we're allowed to write
	if ( IsReadOnly() )
		{
		User::Leave( KErrLocked );
		}
		
	// Create data buffer	
	// use iData as a buffer where the data is externalized to
	iData->SetBufferSizeL();
	RBufWriteStream bufStream( iData->DataBuffer() );
	
	CleanupClosePushL( bufStream );
	
	// externalize
	iData->ExternalizeL( bufStream );
	
	// Get refrence to data buffer    
    TPtr8 bufPtr = iData->DataBufferPtr();	
    
	// update data to server
	TIpcArgs args(&bufPtr);
    User::LeaveIfError( SendReceive( ECmdConnSet, args ) );

	CleanupStack::PopAndDestroy( &bufStream ); // stream
	
	_DBG_FILE("RSyncMLConnection::UpdateL(): end");
	}
	
// -----------------------------------------------------------------------------
// RSyncMLConnection::Close()
// Closes sub-session.
// -----------------------------------------------------------------------------
//
EXPORT_C void RSyncMLConnection::Close()
	{
	_DBG_FILE("RSyncMLConnection::Close(): begin");
	CloseSubSession( ECmdConnClose );
	
	delete iData;
	iData = NULL;
	_DBG_FILE("RSyncMLConnection::Close(): end");
	}
	


//
// RSyncMLTransport
//


// -----------------------------------------------------------------------------
// RSyncMLTransport::RSyncMLTransport()
// Constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C RSyncMLTransport::RSyncMLTransport()
	: RSubSessionBase(), iData( NULL )
	{
	}
	
// -----------------------------------------------------------------------------
// RSyncMLTransport::OpenL()
// Creates a list of transport properties using the given 
// transport id.
// -----------------------------------------------------------------------------
//
EXPORT_C void RSyncMLTransport::OpenL( RSyncMLSession& /*aSession*/, TSmlTransportId aId )
	{
	_DBG_FILE("RSyncMLTransport::OpenL(): begin");
	
	CClientSessionData::PanicIfAlreadyCreated( iData );
	iData = CTransportSessionData::NewL( aId, ESmlDataSync );
	
	_DBG_FILE("RSyncMLTransport::OpenL(): end");
	}

// -----------------------------------------------------------------------------
// RSyncMLTransport::Identifier()
// Returns the transport id, whose properties this object 
// contains.
// -----------------------------------------------------------------------------
//
EXPORT_C TSmlTransportId RSyncMLTransport::Identifier() const
	{
	return static_cast<CTransportSessionData*>(iData)->Identifier();
	}
	
// -----------------------------------------------------------------------------
// RSyncMLTransport::DisplayName()
// Returns the display name.
// -----------------------------------------------------------------------------
//
EXPORT_C const TDesC& RSyncMLTransport::DisplayName() const
	{
	return static_cast<CTransportSessionData*>(iData)->DisplayName();
	}

// -----------------------------------------------------------------------------
// RSyncMLTransport::SupportsCapability(
// Not supported, returns EFalse.
// -----------------------------------------------------------------------------
//
EXPORT_C TBool RSyncMLTransport::SupportsCapability( TSmlTransportCap /*aCapability*/ ) const
	{
	return EFalse;
	}
	
// -----------------------------------------------------------------------------
// RSyncMLTransport::Properties()
// Returns the list of transport properties related to this 
// transport.
// -----------------------------------------------------------------------------
//
EXPORT_C const CSyncMLTransportPropertiesArray& RSyncMLTransport::Properties() const
	{
	return static_cast<CTransportSessionData*>(iData)->Properties();
	}

// -----------------------------------------------------------------------------
// RSyncMLTransport::StartListeningL()
// Not supported.
// -----------------------------------------------------------------------------
//
EXPORT_C void RSyncMLTransport::StartListeningL() const
	{
	User::Leave( KErrNotSupported );
	}
	
// -----------------------------------------------------------------------------
// RSyncMLTransport::StopListeningL()
// Not supported.
// -----------------------------------------------------------------------------
//
EXPORT_C void RSyncMLTransport::StopListeningL() const
	{
	User::Leave( KErrNotSupported );
	}

// -----------------------------------------------------------------------------
// RSyncMLTransport::Close()
// Frees the resources used by this object.
// -----------------------------------------------------------------------------
//
EXPORT_C void RSyncMLTransport::Close()
	{
	_DBG_FILE("RSyncMLTransport::Close(): begin");
	
	delete iData;
	iData = NULL;
	
	_DBG_FILE("RSyncMLTransport::Close(): end");
	}

//  End of File  
