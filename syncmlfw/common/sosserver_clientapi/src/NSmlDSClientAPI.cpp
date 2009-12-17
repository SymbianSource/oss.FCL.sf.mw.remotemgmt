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
* Description:  Implementation of DS specific client api.
*
*/


#include <s32mem.h>
#include <SyncMLClientDS.h>
#include <nsmldebug.h>

#include "NSmlClientAPIUtils.h"
#include "nsmlsosserverdefs.h"


// ============================ MEMBER FUNCTIONS ===============================

//
// RSyncMLDataSyncJob
//

// -----------------------------------------------------------------------------
// RSyncMLDataSyncJob::RSyncMLDataSyncJob()
// Constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C RSyncMLDataSyncJob::RSyncMLDataSyncJob()
	: RSyncMLJobBase()
	{
	}
	
// -----------------------------------------------------------------------------
// RSyncMLDataSyncJob::CreateL()
// Creates DS job.
// -----------------------------------------------------------------------------
//
EXPORT_C void RSyncMLDataSyncJob::CreateL( RSyncMLSession& aSession, TSmlProfileId aProfileId )
	{
	_DBG_FILE("RSyncMLDataSyncJob::CreateL() ECmdDataSyncJobCreateForProfile: begin");
	
	// check that job is not yet open and ds sync is supported
	CClientSessionData::PanicIfAlreadyCreated( iData );
	NSmlClientAPIFeatureHandler::LeaveIfDataSyncNotSupportedL();
	
	CDataSyncJobSessionData* data = new (ELeave) CDataSyncJobSessionData();
	CleanupStack::PushL( data );

	// add profile id to parameters and write the buffer
	data->AddParamL( aProfileId );
	data->WriteIntegersToBufferL();
	
	TPtr8 dataPtr = data->DataBufferPtr();
	TPckgBuf<TInt> jobId;
	TIpcArgs args( &jobId, &dataPtr );
    
	// open the sub-session
	User::LeaveIfError( CreateSubSession( aSession, ECmdDataSyncJobCreateForProfile, args ) );
	
	data->SetProfile( aProfileId );
	data->SetIdentifier( jobId() );
	
	iData = data;
	CleanupStack::Pop(); // data
	
	_DBG_FILE("RSyncMLDataSyncJob::CreateL(): end");
	}
	
// -----------------------------------------------------------------------------
// RSyncMLDataSyncJob::CreateL()
// Creates DS job, transport override.
// -----------------------------------------------------------------------------
//
EXPORT_C void RSyncMLDataSyncJob::CreateL( RSyncMLSession& aSession, TSmlProfileId aProfileId, TSmlConnectionId aTransportId )
	{
	_DBG_FILE("RSyncMLDataSyncJob::CreateL() ECmdDataSyncJobCreateForTransport: begin");
	
	// check that job is not yet open and ds sync is supported
	CClientSessionData::PanicIfAlreadyCreated( iData );
	NSmlClientAPIFeatureHandler::LeaveIfDataSyncNotSupportedL();
	
	CDataSyncJobSessionData* data = new (ELeave) CDataSyncJobSessionData();
	CleanupStack::PushL( data );

	// add ids to parameters and write the buffer
	data->AddParamL( aProfileId );
	data->AddParamL( aTransportId );
	data->WriteIntegersToBufferL();
	
	TPtr8 dataPtr = data->DataBufferPtr();
	TPckgBuf<TInt> jobId;
	TIpcArgs args( &jobId, &dataPtr );
    
	// open the sub-session
	User::LeaveIfError( CreateSubSession( aSession, ECmdDataSyncJobCreateForTransport, args ) );
	
	data->SetProfile( aProfileId );
	data->SetIdentifier( jobId() );
	
	iData = data;
	CleanupStack::Pop(); // data
	
	_DBG_FILE("RSyncMLDataSyncJob::CreateL(): end");
	}
	
// -----------------------------------------------------------------------------
// RSyncMLDataSyncJob::CreateL()
// Creates DS job, sync type override.
// -----------------------------------------------------------------------------
//
EXPORT_C void RSyncMLDataSyncJob::CreateL( RSyncMLSession& aSession, TSmlProfileId aProfileId, TSmlSyncType aSyncType )
	{
	_DBG_FILE("RSyncMLDataSyncJob::CreateL() ECmdDataSyncJobCreateForProfileST: begin");
	
	// check that job is not yet open and ds sync is supported
	CClientSessionData::PanicIfAlreadyCreated( iData );
	NSmlClientAPIFeatureHandler::LeaveIfDataSyncNotSupportedL();
	
	CDataSyncJobSessionData* data = new (ELeave) CDataSyncJobSessionData();
	CleanupStack::PushL( data );

	// add ids to parameters and write the buffer
	data->AddParamL( aProfileId );
	data->AddParamL( aSyncType );
	data->WriteIntegersToBufferL();
	
	TPtr8 dataPtr = data->DataBufferPtr();
	TPckgBuf<TInt> jobId;
	TIpcArgs args( &jobId, &dataPtr );
    
	// open the sub-session
	User::LeaveIfError( CreateSubSession( aSession, ECmdDataSyncJobCreateForProfileST, args ) );
	
	data->SetProfile( aProfileId );
	data->SetIdentifier( jobId() );
	
	iData = data;
	CleanupStack::Pop(); // data
	
	_DBG_FILE("RSyncMLDataSyncJob::CreateL(): end");
	}
	
// -----------------------------------------------------------------------------
// RSyncMLDataSyncJob::CreateL()
// Creates DS job, transport + sync type override.
// -----------------------------------------------------------------------------
//
EXPORT_C void RSyncMLDataSyncJob::CreateL( RSyncMLSession& aSession, TSmlProfileId aProfileId, TSmlConnectionId aTransportId, TSmlSyncType aSyncType )
	{
	_DBG_FILE("RSyncMLDataSyncJob::CreateL() ECmdDataSyncJobCreateForTransportST: begin");
	
	// check that job is not yet open and ds sync is supported
	CClientSessionData::PanicIfAlreadyCreated( iData );
	NSmlClientAPIFeatureHandler::LeaveIfDataSyncNotSupportedL();
	
	CDataSyncJobSessionData* data = new (ELeave) CDataSyncJobSessionData();
	CleanupStack::PushL( data );

	// add ids to parameters and write the buffer
	data->AddParamL( aProfileId );
	data->AddParamL( aTransportId );
	data->AddParamL( aSyncType );
	data->WriteIntegersToBufferL();
	
	TPtr8 dataPtr = data->DataBufferPtr();
	TPckgBuf<TInt> jobId;
	TIpcArgs args( &jobId, &dataPtr );
    
	// open the sub-session
	User::LeaveIfError( CreateSubSession( aSession, ECmdDataSyncJobCreateForTransportST, args ) );
	
	data->SetProfile( aProfileId );
	data->SetIdentifier( jobId() );
	
	iData = data;
	CleanupStack::Pop(); // data
	
	_DBG_FILE("RSyncMLDataSyncJob::CreateL(): end");
	}
	
// -----------------------------------------------------------------------------
// RSyncMLDataSyncJob::CreateL()
// Creates DS job, tasks override.
// -----------------------------------------------------------------------------
//
EXPORT_C void RSyncMLDataSyncJob::CreateL( RSyncMLSession& aSession, TSmlProfileId aProfileId, const RArray<TSmlTaskId>& aTasks )
	{
	_DBG_FILE("RSyncMLDataSyncJob::CreateL() ECmdDataSyncJobCreateForTasks: begin");
	
	// check that job is not yet open and ds sync is supported
	CClientSessionData::PanicIfAlreadyCreated( iData );
	NSmlClientAPIFeatureHandler::LeaveIfDataSyncNotSupportedL();
	
	CDataSyncJobSessionData* data = new (ELeave) CDataSyncJobSessionData();
	CleanupStack::PushL( data );

	// add ids to parameters and write the buffer
	data->AddParamL( aProfileId );
	data->WriteIntegersToBufferL( &aTasks );
	
	TPtr8 dataPtr = data->DataBufferPtr();
	TPckgBuf<TInt> jobId;
	TIpcArgs args( &jobId, &dataPtr );
    
	// open the sub-session
	User::LeaveIfError( CreateSubSession( aSession, ECmdDataSyncJobCreateForTasks, args ) );
	
	iData = data;
	CleanupStack::Pop(); // data
	
	CleanupClosePushL( *this );  // if leave occures, Close is called by cleanup
	
	data->SetProfile( aProfileId );
	data->SetIdentifier( jobId() );
	data->SetTasksL( aTasks );
	
	CleanupStack::Pop(); // this
	
	_DBG_FILE("RSyncMLDataSyncJob::CreateL(): end");
	}
	
// -----------------------------------------------------------------------------
// RSyncMLDataSyncJob::CreateL()
// Creates DS job, transport + tasks override.
// -----------------------------------------------------------------------------
//
EXPORT_C void RSyncMLDataSyncJob::CreateL( RSyncMLSession& aSession, TSmlProfileId aProfileId, TSmlConnectionId aTransportId, const RArray<TSmlTaskId>& aTasks )
	{
	_DBG_FILE("RSyncMLDataSyncJob::CreateL() ECmdDataSyncJobCreateForTransportAndTasks: begin");
	
	// check that job is not yet open and ds sync is supported
	CClientSessionData::PanicIfAlreadyCreated( iData );
	NSmlClientAPIFeatureHandler::LeaveIfDataSyncNotSupportedL();
	
	CDataSyncJobSessionData* data = new (ELeave) CDataSyncJobSessionData();
	CleanupStack::PushL( data );

	// add ids to parameters and write the buffer
	data->AddParamL( aProfileId );
	data->AddParamL( aTransportId );
	data->WriteIntegersToBufferL( &aTasks );
	
	TPtr8 dataPtr = data->DataBufferPtr();
	TPckgBuf<TInt> jobId;
	TIpcArgs args( &jobId, &dataPtr );
    
	// open the sub-session
	User::LeaveIfError( CreateSubSession( aSession, ECmdDataSyncJobCreateForTransportAndTasks, args ) );
	
	iData = data;
	CleanupStack::Pop(); // data
	
	CleanupClosePushL( *this );  // if leave occures, Close is called by cleanup
	
	data->SetProfile( aProfileId );
	data->SetIdentifier( jobId() );
	data->SetTasksL( aTasks );
	
	CleanupStack::Pop(); // this
	
	_DBG_FILE("RSyncMLDataSyncJob::CreateL(): end");
	}
	
// -----------------------------------------------------------------------------
// RSyncMLDataSyncJob::CreateL()
// Creates DS job, sync type + tasks override.
// -----------------------------------------------------------------------------
//
EXPORT_C void RSyncMLDataSyncJob::CreateL( RSyncMLSession& aSession, TSmlProfileId aProfileId, TSmlSyncType aSyncType, const RArray<TSmlTaskId>& aTasks )
	{
	_DBG_FILE("RSyncMLDataSyncJob::CreateL() ECmdDataSyncJobCreateForTasksST: begin");
	
	// check that job is not yet open and ds sync is supported
	CClientSessionData::PanicIfAlreadyCreated( iData );
	NSmlClientAPIFeatureHandler::LeaveIfDataSyncNotSupportedL();
	
	CDataSyncJobSessionData* data = new (ELeave) CDataSyncJobSessionData();
	CleanupStack::PushL( data );

	// add ids to parameters and write the buffer
	data->AddParamL( aProfileId );
	data->AddParamL( aSyncType );
	data->WriteIntegersToBufferL( &aTasks );
	
	TPtr8 dataPtr = data->DataBufferPtr();
	TPckgBuf<TInt> jobId;
	TIpcArgs args( &jobId, &dataPtr );
    
	// open the sub-session
	User::LeaveIfError( CreateSubSession( aSession, ECmdDataSyncJobCreateForTasksST, args ) );
	
	iData = data;
	CleanupStack::Pop(); // data
	
	CleanupClosePushL( *this );  // if leave occures, Close is called by cleanup
	
	data->SetProfile( aProfileId );
	data->SetIdentifier( jobId() );
	data->SetTasksL( aTasks );
	
	CleanupStack::Pop(); // this
	
	_DBG_FILE("RSyncMLDataSyncJob::CreateL(): end");
	}
	
// -----------------------------------------------------------------------------
// RSyncMLDataSyncJob::CreateL()
// Creates DS job, transport + sync type + tasks override.
// -----------------------------------------------------------------------------
//
EXPORT_C void RSyncMLDataSyncJob::CreateL( RSyncMLSession& aSession, TSmlProfileId aProfileId, TSmlConnectionId aTransportId, TSmlSyncType aSyncType, const RArray<TSmlTaskId>& aTasks )
	{
	_DBG_FILE("RSyncMLDataSyncJob::CreateL() ECmdDataSyncJobCreateForTransportAndTasksST: begin");
	
	// check that job is not yet open and ds sync is supported
	CClientSessionData::PanicIfAlreadyCreated( iData );
	NSmlClientAPIFeatureHandler::LeaveIfDataSyncNotSupportedL();
	
	CDataSyncJobSessionData* data = new (ELeave) CDataSyncJobSessionData();
	CleanupStack::PushL( data );

	// add ids to parameters and write the buffer
	data->AddParamL( aProfileId );
	data->AddParamL( aTransportId );
	data->AddParamL( aSyncType );
	data->WriteIntegersToBufferL( &aTasks );
	
	TPtr8 dataPtr = data->DataBufferPtr();
	TPckgBuf<TInt> jobId;
	TIpcArgs args( &jobId, &dataPtr );
    
	// open the sub-session
	User::LeaveIfError( CreateSubSession( aSession, ECmdDataSyncJobCreateForTransportAndTasksST, args ) );
	
	iData = data;
	CleanupStack::Pop(); // data
	
	CleanupClosePushL( *this );  // if leave occures, Close is called by cleanup
	
	data->SetProfile( aProfileId );
	data->SetIdentifier( jobId() );
	data->SetTasksL( aTasks );
	
	CleanupStack::Pop(); // this
	
	_DBG_FILE("RSyncMLDataSyncJob::CreateL(): end");
	}
	
// -----------------------------------------------------------------------------
// RSyncMLDataSyncJob::OpenL()
// Opens a job currently in servers job queue. If not found, leaves with 
// KErrNotFound.
// -----------------------------------------------------------------------------
//
EXPORT_C void RSyncMLDataSyncJob::OpenL( RSyncMLSession& aSession, TSmlJobId aJobId )
	{
	_DBG_FILE("RSyncMLDataSyncJob::OpenL(): begin");
	
	CClientSessionData::PanicIfAlreadyCreated( iData );
	
	CDataSyncJobSessionData* data = new (ELeave) CDataSyncJobSessionData();
	CleanupStack::PushL( data );

	// open the sub-session and get size of data to be received
	TPckgBuf<TInt> dataSize;
	TIpcArgs args( &dataSize, aJobId );
	User::LeaveIfError( CreateSubSession( aSession, ECmdJobOpen, args ) );
	
	data->SetIdentifier( aJobId );
	
	iData = data;
	CleanupStack::Pop(); // data
	
	CleanupClosePushL( *this ); // if leave occures, Close is called by cleanup
	
	// get the data: the profile id and list of task ids
	GetJobL( dataSize(), EFalse );
	
	CleanupStack::Pop(); // this
	
	_DBG_FILE("RSyncMLDataSyncJob::OpenL(): end");
	}
	
// -----------------------------------------------------------------------------
// RSyncMLDataSyncJob::StopL()
// If this job has started to execute in server, it is stopped and an 
// event is emitted. If the job is still in job queue, it is merely 
// removed from the queue. If not found, leaves with KErrNotFound.
// -----------------------------------------------------------------------------
//
EXPORT_C void RSyncMLDataSyncJob::StopL()
	{
	_DBG_FILE("RSyncMLDataSyncJob::StopL(): begin");
	
	TInt jobId = Identifier();
	TIpcArgs args( jobId );
	User::LeaveIfError( SendReceive( ECmdJobStop, args ) );
	
	_DBG_FILE("RSyncMLDataSyncJob::StopL(): end");
	}
	
// -----------------------------------------------------------------------------
// RSyncMLDataSyncJob::TaskIDs()
// Returns the task id associated with this job (if given in CreateL).
// -----------------------------------------------------------------------------
//
EXPORT_C const RArray<TSmlTaskId>& RSyncMLDataSyncJob::TaskIDs() const
	{
	return static_cast<CDataSyncJobSessionData*>(iData)->TaskIds();
	}
	

//
// RSyncMLDataSyncProfile
//

// -----------------------------------------------------------------------------
// RSyncMLDataSyncProfile::RSyncMLDataSyncProfile()
// Constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C RSyncMLDataSyncProfile::RSyncMLDataSyncProfile()
	: RSyncMLProfileBase()
	{
	}

// -----------------------------------------------------------------------------
// RSyncMLDataSyncProfile::CreateL()
// Opens a sub-session to server and creates a new profile 
// to server side.
// -----------------------------------------------------------------------------
//
EXPORT_C void RSyncMLDataSyncProfile::CreateL( RSyncMLSession& aSession )
	{
	_DBG_FILE("RSyncMLDataSyncProfile::CreateL(): begin");
	
	// check that profile is not yet open and ds sync is supported
	CClientSessionData::PanicIfAlreadyCreated( iData );
	NSmlClientAPIFeatureHandler::LeaveIfDataSyncNotSupportedL();
		
	CDataSyncProfileSessionData* data = CDataSyncProfileSessionData::NewLC( ETrue );
	
	// create sub-session and new profile
	User::LeaveIfError( CreateSubSession( aSession, ECmdProfileCreateDS ) );

	iData = data;
	CleanupStack::Pop(); // data
	
	_DBG_FILE("RSyncMLDataSyncProfile::CreateL(): end");
	}
		
// -----------------------------------------------------------------------------
// RSyncMLDataSyncProfile::OpenL()
// Opens a sub-session to server to an existing profile and 
// gets the profile's data to iData.
// -----------------------------------------------------------------------------
//
EXPORT_C void RSyncMLDataSyncProfile::OpenL( RSyncMLSession& aSession, TSmlProfileId aProfileId, TSmlOpenMode aOpenMode )
	{
	_DBG_FILE("RSyncMLDataSyncProfile::OpenL(): begin");
	
	CClientSessionData::PanicIfAlreadyCreated( iData );
	
	CDataSyncProfileSessionData* data = CDataSyncProfileSessionData::NewLC();
	
	TPckgBuf<TInt> dataSize;
	TIpcArgs args( &dataSize, aProfileId, aOpenMode );
    
	// open the sub-session
	User::LeaveIfError( CreateSubSession( aSession, ECmdProfileOpenDS, args ) );
	
	data->SetIdentifier( aProfileId );
	data->SetOpenMode( (TSmlOpenMode)aOpenMode );
	
	iData = data;
	CleanupStack::Pop(); // data
	
	CleanupClosePushL( *this ); // if leave occures, Close is called by cleanup
	
	// dataSize contains now the size of the data buffer that is received next -> get data
	GetProfileL( dataSize() );
	
	CleanupStack::Pop(); // this
	
	_DBG_FILE("RSyncMLDataSyncProfile::OpenL(): end");
	}

// -----------------------------------------------------------------------------
// RSyncMLDataSyncProfile::SetCreatorId()
// Sets creator id.
// -----------------------------------------------------------------------------
//
EXPORT_C void RSyncMLDataSyncProfile::SetCreatorId( TSmlCreatorId aCreatorId )
	{
	RSyncMLProfileBase::SetCreatorId( aCreatorId );
	}
		
// -----------------------------------------------------------------------------
// RSyncMLDataSyncProfile::SetDisplayNameL()
// Sets display name for this profile.
// -----------------------------------------------------------------------------
//
EXPORT_C void RSyncMLDataSyncProfile::SetDisplayNameL( const TDesC& aDisplayName )
	{
	RSyncMLProfileBase::SetDisplayNameL( aDisplayName );
	}
		
// -----------------------------------------------------------------------------
// RSyncMLDataSyncProfile::SetUserNameL()
// Sets user name.
// -----------------------------------------------------------------------------
//
EXPORT_C void RSyncMLDataSyncProfile::SetUserNameL( const TDesC8& aUserName )
	{
	RSyncMLProfileBase::SetUserNameL( aUserName );
	}
		
// -----------------------------------------------------------------------------
// RSyncMLDataSyncProfile::SetPasswordL()
// Sets password.
// -----------------------------------------------------------------------------
//
EXPORT_C void RSyncMLDataSyncProfile::SetPasswordL( const TDesC8& aPassword )
	{
	RSyncMLProfileBase::SetPasswordL( aPassword );
	}

// -----------------------------------------------------------------------------
// RSyncMLDataSyncProfile::DeleteConnectionL()
// Not supported (only one connection per profile).
// -----------------------------------------------------------------------------
//
EXPORT_C void RSyncMLDataSyncProfile::DeleteConnectionL( TSmlTransportId aTransportId )
	{
	RSyncMLProfileBase::DeleteConnectionL( aTransportId );
	}
	
// -----------------------------------------------------------------------------
// RSyncMLDataSyncProfile::ListTasksL()
// Lists tasks associated with this profile.
// -----------------------------------------------------------------------------
//
EXPORT_C void RSyncMLDataSyncProfile::ListTasksL( RArray<TSmlTaskId>& aArray ) const
	{
	_DBG_FILE("RSyncMLDataSyncProfile::ListTasksL(): begin");
	
	ReceiveArrayL( aArray, ECmdTaskListStart, ECmdTaskListNext );
	
	_DBG_FILE("RSyncMLDataSyncProfile::ListTasksL(): end");
	}

// -----------------------------------------------------------------------------
// RSyncMLDataSyncProfile::DeleteTaskL()
// Deletes given task.
// -----------------------------------------------------------------------------
//
EXPORT_C void RSyncMLDataSyncProfile::DeleteTaskL( TSmlTaskId aTaskId )
	{
	_DBG_FILE("RSyncMLDataSyncProfile::DeleteTaskL(): begin");
	
    TIpcArgs args( aTaskId );
    User::LeaveIfError( SendReceive( ECmdTaskDelete, args ) );
    
	_DBG_FILE("RSyncMLDataSyncProfile::DeleteTaskL(): end");
	}

// -----------------------------------------------------------------------------
// RSyncMLDataSyncProfile::UpdateL()
// Sends profile data to server -> data updated.
// -----------------------------------------------------------------------------
//
EXPORT_C void RSyncMLDataSyncProfile::UpdateL()
	{
	RSyncMLProfileBase::UpdateL();
	}


//
// RSyncMLTask
//


// -----------------------------------------------------------------------------
// RSyncMLTask::RSyncMLTask()
// Constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C RSyncMLTask::RSyncMLTask()
	: iData( NULL )
	{
	}
	
// -----------------------------------------------------------------------------
// RSyncMLTask::CreateL()
// Creates new task for given profile.
// -----------------------------------------------------------------------------
//
EXPORT_C void RSyncMLTask::CreateL( RSyncMLDataSyncProfile& aProfile, TSmlDataProviderId aDataProviderId,
									const TDesC& aServerDataSource, const TDesC& aClientDataSource )
	{
	_DBG_FILE("RSyncMLTask::CreateL(): begin");
	
	// check that we're allowed to write
	if ( aProfile.IsReadOnly() )
		{
		User::Leave( KErrLocked );
		}
		
	CClientSessionData::PanicIfAlreadyCreated( iData );
	
    TIpcArgs args( aProfile.Identifier(), aDataProviderId );
	
	// create sub-session and new task
	RSessionBase session = static_cast<RSessionBase>( aProfile.Session() );
	User::LeaveIfError( CreateSubSession( session, ECmdCreateTask, args ) );
	
	CleanupClosePushL( *this ); // if leave occures, Close is called by cleanup
	
	iData = CTaskSessionData::NewL( ETrue );
	static_cast<CTaskSessionData*>(iData)->SetProfile( aProfile.Identifier() );
	static_cast<CTaskSessionData*>(iData)->SetDataProvider( aDataProviderId );
	static_cast<CTaskSessionData*>(iData)->SetServerDataSourceL( aServerDataSource );
	static_cast<CTaskSessionData*>(iData)->SetClientDataSourceL( aClientDataSource );
	
	CleanupStack::Pop(); // this
	
	_DBG_FILE("RSyncMLTask::CreateL(): end");
	}

// -----------------------------------------------------------------------------
// RSyncMLTask::CreateL()
// Creates new task. Filters are copied.
// Ownership is moved from caller.
// -----------------------------------------------------------------------------
//
EXPORT_C void RSyncMLTask::CreateL( RSyncMLDataSyncProfile& aProfile, TSmlDataProviderId aDataProviderId,
									const TDesC& aServerDataSource, const TDesC& aClientDataSource, 
									RPointerArray<CSyncMLFilter>& aFilterArray )
	{
	_DBG_FILE("RSyncMLTask::CreateL(): begin");
	
	// check that we're allowed to write
	if ( aProfile.IsReadOnly() )
		{
		User::Leave( KErrLocked );
		}
		
	CClientSessionData::PanicIfAlreadyCreated( iData );
	
	TPckgBuf<TInt> dataSize;
	
	TIpcArgs args( aProfile.Identifier(), aDataProviderId, &dataSize );
	
	// create sub-session and new task
	RSessionBase session = static_cast<RSessionBase>( aProfile.Session() );
	User::LeaveIfError( CreateSubSession( session, ECmdCreateTask, args ) );
	
	CleanupClosePushL( *this ); // if leave occures, Close is called by cleanup
	
	iData = CTaskSessionData::NewL( ETrue );
	static_cast<CTaskSessionData*>(iData)->SetProfile( aProfile.Identifier() );
	static_cast<CTaskSessionData*>(iData)->SetDataProvider( aDataProviderId );
	static_cast<CTaskSessionData*>(iData)->SetServerDataSourceL( aServerDataSource );
	static_cast<CTaskSessionData*>(iData)->SetClientDataSourceL( aClientDataSource );
	
	if ( aProfile.ProtocolVersion() == ESmlVersion1_2 )
		{
		//Copy filters		
		static_cast<CTaskSessionData*>(iData)->CopyTaskSupportedFiltersL( aFilterArray );
		}
	
	CleanupStack::Pop(); // this
	}
	
// -----------------------------------------------------------------------------
// RSyncMLTask::OpenL()
// Opens given task.
// -----------------------------------------------------------------------------
//
EXPORT_C void RSyncMLTask::OpenL( RSyncMLDataSyncProfile& aProfile, TSmlTaskId aTaskId )
	{
	_DBG_FILE("RSyncMLTask::OpenL(): begin");
	
	CClientSessionData::PanicIfAlreadyCreated( iData );
	
	CTaskSessionData* data = CTaskSessionData::NewLC();
	
	TPckgBuf<TInt> dataSize;
	TIpcArgs args( &dataSize, aProfile.Identifier(), aTaskId );
    
	// open the sub-session
	RSessionBase session = static_cast<RSessionBase>( aProfile.Session() );
	User::LeaveIfError( CreateSubSession( session, ECmdTaskOpen, args ) );
	
	data->SetIdentifier( aTaskId );
	data->SetProfile( aProfile.Identifier() );
	if ( aProfile.IsReadOnly() )
		{
		data->SetReadOnly( ETrue );
		}
	
	iData = data;
	CleanupStack::Pop(); // data
	
	CleanupClosePushL( *this ); // if leave occures, Close is called by cleanup
	
	// Create data buffer
	// dataSize contains now the size of the data buffer that is received next
	iData->SetBufferSizeL( dataSize() );	
	TPtr8 bufPtr = iData->DataBufferPtr();
	
	TIpcArgs args2( &bufPtr );
	User::LeaveIfError( SendReceive( ECmdTaskGet, args2 ) );
	
	// internalize the buffer to iData
	RDesReadStream readStream;
	readStream.Open( bufPtr );
	CleanupClosePushL( readStream );
	
	iData->InternalizeL( readStream );
	
	CleanupStack::PopAndDestroy(); // readStream
	CleanupStack::Pop(); // this
	
	_DBG_FILE("RSyncMLTask::OpenL(): end");
	}
	
// -----------------------------------------------------------------------------
// RSyncMLTask::Identifier()
// Returns the id of this task.
// -----------------------------------------------------------------------------
//
EXPORT_C TSmlTaskId RSyncMLTask::Identifier() const
	{
	return static_cast<CTaskSessionData*>(iData)->Identifier();
	}
	
// -----------------------------------------------------------------------------
// RSyncMLTask::Profile()
// Returns the id of the profile this task belongs to.
// -----------------------------------------------------------------------------
//
EXPORT_C TSmlProfileId RSyncMLTask::Profile() const
	{
	return static_cast<CTaskSessionData*>(iData)->Profile();
	}
	
// -----------------------------------------------------------------------------
// RSyncMLTask::CreatorId()
// Returns creator id.
// -----------------------------------------------------------------------------
//
EXPORT_C TSmlCreatorId RSyncMLTask::CreatorId() const
	{
	return static_cast<CTaskSessionData*>(iData)->CreatorId();
	}
	
// -----------------------------------------------------------------------------
// RSyncMLTask::DisplayName()
// Returns display name.
// -----------------------------------------------------------------------------
//
EXPORT_C const TDesC& RSyncMLTask::DisplayName() const
	{
	return static_cast<CTaskSessionData*>(iData)->DisplayName();
	}
	
// -----------------------------------------------------------------------------
// RSyncMLTask::ServerDataSource()
// Returns the name of the remote data source.
// -----------------------------------------------------------------------------
//
EXPORT_C const TDesC& RSyncMLTask::ServerDataSource() const
	{
	return static_cast<CTaskSessionData*>(iData)->ServerDataSource();
	}
	
// -----------------------------------------------------------------------------
// RSyncMLTask::ClientDataSource()
// Returns the name of the local data source.
// -----------------------------------------------------------------------------
//
EXPORT_C const TDesC& RSyncMLTask::ClientDataSource() const
	{
	return static_cast<CTaskSessionData*>(iData)->ClientDataSource();
	}
	
// -----------------------------------------------------------------------------
// RSyncMLTask::DataProvider()
// Returns the id of the data provider that is used with this task.
// -----------------------------------------------------------------------------
//
EXPORT_C TSmlDataProviderId RSyncMLTask::DataProvider() const
	{
	return static_cast<CTaskSessionData*>(iData)->DataProvider();
	}
	
// -----------------------------------------------------------------------------
// RSyncMLTask::DefaultSyncType()
// Returns default sync type.
// -----------------------------------------------------------------------------
//
EXPORT_C TSmlSyncType RSyncMLTask::DefaultSyncType() const
	{
	return static_cast<CTaskSessionData*>(iData)->DefaultSyncType();
	}
	
// -----------------------------------------------------------------------------
// RSyncMLTask::FilterMatchType()
// Not supported.
// -----------------------------------------------------------------------------
//
EXPORT_C TSyncMLFilterMatchType RSyncMLTask::FilterMatchType() const
	{
	return static_cast<CTaskSessionData*>(iData)->FilterMatchType();
	}
	
// -----------------------------------------------------------------------------
// RSyncMLTask::Enabled()
// Returns whether or not this task is enabled.
// -----------------------------------------------------------------------------
//
EXPORT_C TBool RSyncMLTask::Enabled() const
	{
	return static_cast<CTaskSessionData*>(iData)->Enabled();
	}
	
// -----------------------------------------------------------------------------
// RSyncMLTask::SetCreatorId()
// Sets the creator id.
// -----------------------------------------------------------------------------
//
EXPORT_C void RSyncMLTask::SetCreatorId( TSmlCreatorId aCreatorId )
	{
	static_cast<CTaskSessionData*>(iData)->SetCreatorId( aCreatorId );
	}
	
// -----------------------------------------------------------------------------
// RSyncMLTask::SetDisplayNameL()
// Sets the display name.
// -----------------------------------------------------------------------------
//
EXPORT_C void RSyncMLTask::SetDisplayNameL( const TDesC& aDisplayName )
	{
	static_cast<CTaskSessionData*>(iData)->SetDisplayNameL( aDisplayName );
	}
	
// -----------------------------------------------------------------------------
// RSyncMLTask::SetDefaultSyncTypeL()
// Sets default sync type.
// -----------------------------------------------------------------------------
//
EXPORT_C void RSyncMLTask::SetDefaultSyncTypeL( TSmlSyncType aSyncType )
	{
	static_cast<CTaskSessionData*>(iData)->SetDefaultSyncType( aSyncType );
	}
	
// -----------------------------------------------------------------------------
// RSyncMLTask::SetFilterMatchTypeL()
// Not supported.
// -----------------------------------------------------------------------------
//
EXPORT_C void RSyncMLTask::SetFilterMatchTypeL( TSyncMLFilterMatchType aType ) const
	{
	static_cast<CTaskSessionData*>(iData)->SetFilterMatchType( aType );
	}
	
// -----------------------------------------------------------------------------
// RSyncMLTask::SetEnabledL()
// Sets whether or not this task is enabled.
// -----------------------------------------------------------------------------
//
EXPORT_C void RSyncMLTask::SetEnabledL( TBool aEnabled )
	{
	static_cast<CTaskSessionData*>(iData)->SetEnabled( aEnabled );
	}
	
// -----------------------------------------------------------------------------
// RSyncMLTask::SupportedServerFiltersL()
// Not supported.
// -----------------------------------------------------------------------------
//
EXPORT_C RPointerArray<CSyncMLFilter>& RSyncMLTask::SupportedServerFiltersL( TSyncMLFilterChangeInfo& aChangeInfo ) const
	{
	TPckgBuf<TInt> dataSize;	
	TInt taskId( static_cast<CTaskSessionData*>(iData)->Identifier() );
		
	TIpcArgs args( &dataSize, taskId );
	User::LeaveIfError( SendReceive( ECmdTaskFilterBufferSize, args ) );
	
	// Create data buffer
	static_cast<CTaskSessionData*>(iData)->SetFilterBufSize( dataSize() );
	iData->SetBufferSizeL( static_cast<CTaskSessionData*>(iData)->FilterBufSize() );
	
	TPtr8 bufPtr = iData->DataBufferPtr();
	
	TIpcArgs args2( &bufPtr );
	User::LeaveIfError( SendReceive( ECmdTaskSupportedFilters, args2 ) );
	
	static_cast<CTaskSessionData*>(iData)->SetTaskSupportedFiltersL( bufPtr );

	aChangeInfo = static_cast<CTaskSessionData*>(iData)->FilterChangeInfo();
	return static_cast<CTaskSessionData*>(iData)->FilterArray();
	}
	
// -----------------------------------------------------------------------------
// RSyncMLTask::SupportsUserSelectableMatchType()
// Not supported.
// -----------------------------------------------------------------------------
//
EXPORT_C TBool RSyncMLTask::SupportsUserSelectableMatchType() const
	{
	return static_cast<CTaskSessionData*>(iData)->FilterMatchType() == ESyncMLMatchDisabled ? EFalse : ETrue; 
	}
	
// -----------------------------------------------------------------------------
// RSyncMLTask::IsReadOnly()
// Return whether or not this task (sub-session) is in read-only mode.
// -----------------------------------------------------------------------------
//
EXPORT_C TBool RSyncMLTask::IsReadOnly() const
	{
	return static_cast<CTaskSessionData*>(iData)->IsReadOnly();
	}
	
// -----------------------------------------------------------------------------
// RSyncMLTask::UpdateL()
// Updates data to server.
// -----------------------------------------------------------------------------
//
EXPORT_C void RSyncMLTask::UpdateL()
	{
	_DBG_FILE("RSyncMLTask::UpdateL(): begin");
	
	// check that we're allowed to write
	if ( IsReadOnly() )
		{
		User::Leave( KErrLocked );
		}
	
	//get filter size
	TInt filterBufSize =  static_cast<CTaskSessionData*>(iData)->FilterDataSize();
	static_cast<CTaskSessionData*>(iData)->SetFilterBufSize( filterBufSize );
	
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
    User::LeaveIfError( SendReceive( ECmdTaskSet, args ) );

	static_cast<CTaskSessionData*>(iData)->SetIdentifier( id() );
	
	CleanupStack::PopAndDestroy(); // bufStream
	
	_DBG_FILE("RSyncMLTask::UpdateL(): end");
	}
	
// -----------------------------------------------------------------------------
// RSyncMLTask::Close()
// Closes sub-session.
// -----------------------------------------------------------------------------
//
EXPORT_C void RSyncMLTask::Close()
	{
	_DBG_FILE("RSyncMLTask::Close(): begin");
	
	CloseSubSession( ECmdTaskClose );
	
	delete iData;
	iData = NULL;
	
	_DBG_FILE("RSyncMLTask::Close(): end");
	}
	

//
// RSyncMLDataProvider
//

// -----------------------------------------------------------------------------
// RSyncMLDataProvider::RSyncMLDataProvider()
// Constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C RSyncMLDataProvider::RSyncMLDataProvider()
	: iData( NULL )
	{
	}
	
// -----------------------------------------------------------------------------
// RSyncMLDataProvider::OpenL()
// Opens given data provider.
// -----------------------------------------------------------------------------
//
EXPORT_C void RSyncMLDataProvider::OpenL( RSyncMLSession& aSession, TSmlDataProviderId aDataProviderId )
	{
	_DBG_FILE("RSyncMLDataProvider::OpenL(): begin");
	
	// check that data provider is not yet open and ds sync is supported
	CClientSessionData::PanicIfAlreadyCreated( iData );
	NSmlClientAPIFeatureHandler::LeaveIfDataSyncNotSupportedL();
		
	CDataProviderSessionData* data = new (ELeave) CDataProviderSessionData();
	CleanupStack::PushL( data );
	
	TPckgBuf<TInt> dataSize;
	TIpcArgs args( &dataSize, aDataProviderId );
    
	// open the sub-session
	User::LeaveIfError( CreateSubSession( aSession, ECmdDataProviderOpen, args ) );
	
	data->SetIdentifier( aDataProviderId );
	
	iData = data;
	CleanupStack::Pop(); // data
	
	CleanupClosePushL( *this ); // if leave occures, Close is called by cleanup
	
	// dataSize contains now the size of the data buffer that is received next
	iData->SetBufferSizeL( dataSize() );	
	TPtr8 bufPtr = iData->DataBufferPtr();
	
	TIpcArgs args2( &bufPtr );
	User::LeaveIfError( SendReceive( ECmdDataProviderGet, args2 ) );
	
	// internalize the buffer to data
	RDesReadStream readStream;
	readStream.Open( bufPtr );
	CleanupClosePushL( readStream );
	iData->InternalizeL( readStream );

	CleanupStack::PopAndDestroy(); // readStream
	CleanupStack::Pop(); // this
	
	_DBG_FILE("RSyncMLDataProvider::OpenL(): end");
	}
	
// -----------------------------------------------------------------------------
// RSyncMLDataProvider::Identifier()
// Return the id of this data provider.
// -----------------------------------------------------------------------------
//
EXPORT_C TSmlDataProviderId RSyncMLDataProvider::Identifier() const
	{
	return static_cast<CDataProviderSessionData*>(iData)->Identifier();
	}
	
// -----------------------------------------------------------------------------
// RSyncMLDataProvider::Version()
// Return the version of this data provider.
// -----------------------------------------------------------------------------
//
EXPORT_C TVersion RSyncMLDataProvider::Version() const
	{
	return static_cast<CDataProviderSessionData*>(iData)->Version();
	}
	
// -----------------------------------------------------------------------------
// RSyncMLDataProvider::DisplayName()
// Returns the display name of this data provider.
// -----------------------------------------------------------------------------
//
EXPORT_C const TDesC& RSyncMLDataProvider::DisplayName() const
	{
	return static_cast<CDataProviderSessionData*>(iData)->DisplayName();
	}
	
// -----------------------------------------------------------------------------
// RSyncMLDataProvider::MimeTypeCount()
// Returns the amount of mime types this data provider has.
// -----------------------------------------------------------------------------
//
EXPORT_C TInt RSyncMLDataProvider::MimeTypeCount() const
	{
	return static_cast<CDataProviderSessionData*>(iData)->MimeTypeCount();
	}
	
// -----------------------------------------------------------------------------
// RSyncMLDataProvider::MimeType()
// Return the mime type in given index.
// -----------------------------------------------------------------------------
//
EXPORT_C const TDesC& RSyncMLDataProvider::MimeType( TInt aIndex ) const
	{
	return static_cast<CDataProviderSessionData*>(iData)->MimeType( aIndex );
	}
	
// -----------------------------------------------------------------------------
// RSyncMLDataProvider::MimeVersion()
// Return the mime version in the given index.
// -----------------------------------------------------------------------------
//
EXPORT_C const TDesC& RSyncMLDataProvider::MimeVersion( TInt aIndex ) const
	{
	return static_cast<CDataProviderSessionData*>(iData)->MimeVersion( aIndex );
	}
	
// -----------------------------------------------------------------------------
// RSyncMLDataProvider::DefaultDataStoreName()
// Return the name of the default data store.
// -----------------------------------------------------------------------------
//
EXPORT_C const TDesC& RSyncMLDataProvider::DefaultDataStoreName() const
	{
	return static_cast<CDataProviderSessionData*>(iData)->DefaultDataStoreName();
	}
	
// -----------------------------------------------------------------------------
// RSyncMLDataProvider::AllowsMultipleDataStores()
// Returns whether or not this data provider supports multiple data stores.
// -----------------------------------------------------------------------------
//
EXPORT_C TBool RSyncMLDataProvider::AllowsMultipleDataStores() const
	{
	return static_cast<CDataProviderSessionData*>(iData)->AllowsMultipleDataStores();
	}
	
// -----------------------------------------------------------------------------
// RSyncMLDataProvider::GetDataStoreNamesL()
// On return, aArray contains the names of all supported data stores 
// including the default store which is listed first.
// -----------------------------------------------------------------------------
//
EXPORT_C void RSyncMLDataProvider::GetDataStoreNamesL( CDesCArray& aArray ) const
	{
	static_cast<CDataProviderSessionData*>(iData)->GetDataStoreNamesL( aArray );
	}
	
// -----------------------------------------------------------------------------
// RSyncMLDataProvider::RequiredProtocolVersionL()
// Returns the protocol version this data provider requires.
// -----------------------------------------------------------------------------
//
EXPORT_C TSmlProtocolVersion RSyncMLDataProvider::RequiredProtocolVersionL() const
	{
	return static_cast<CDataProviderSessionData*>(iData)->RequiredProtocolVersion();
	}
	
// -----------------------------------------------------------------------------
// RSyncMLDataProvider::Close()
// Closes the sub-session.
// -----------------------------------------------------------------------------
//
EXPORT_C void RSyncMLDataProvider::Close()
	{
	_DBG_FILE("RSyncMLDataProvider::Close(): begin");
	
	CloseSubSession( ECmdDataProviderClose );
	
	delete iData;
	iData = NULL;
	
	_DBG_FILE("RSyncMLDataProvider::Close(): end");
	}
	






