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
* Description:  Implementation of DM specific client api.
*
*/


#include <s32mem.h>
#include <SyncMLClientDM.h>
#include <nsmldebug.h>

#include "NSmlClientAPIUtils.h"
#include "nsmlsosserverdefs.h"


// ============================ MEMBER FUNCTIONS ===============================

//
// RSyncMLDevManJob
//

// -----------------------------------------------------------------------------
// RSyncMLDevManJob::RSyncMLDevManJob()
// Constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C RSyncMLDevManJob::RSyncMLDevManJob()
	: RSyncMLJobBase()
	{
	}
	
// -----------------------------------------------------------------------------
// RSyncMLDevManJob::CreateL()
// Creates DM job.
// -----------------------------------------------------------------------------
//
EXPORT_C void RSyncMLDevManJob::CreateL( RSyncMLSession& aSession, TSmlProfileId aProfileId )
	{
	_DBG_FILE("RSyncMLDevManJob::CreateL() ECmdDevManJobCreate: begin");
	
	// check that profile is not yet open and dm sync is supported
	CClientSessionData::PanicIfAlreadyCreated( iData );
	NSmlClientAPIFeatureHandler::LeaveIfDevManNotSupportedL();
	
	CDevManJobSessionData* data = new (ELeave) CDevManJobSessionData();
	CleanupStack::PushL( data );

	// add profile id to parameters and write the buffer
	data->AddParamL( aProfileId );
	data->WriteIntegersToBufferL();
	
	TPtr8 dataPtr = data->DataBufferPtr();
	TPckgBuf<TInt> jobId;
	TIpcArgs args( &jobId, &dataPtr );
    
	// open the sub-session
	User::LeaveIfError( CreateSubSession( aSession, ECmdDevManJobCreate, args ) );
	
	data->SetProfile( aProfileId );
	data->SetIdentifier( jobId() );
	
	iData = data;
	CleanupStack::Pop(); // data
	
	_DBG_FILE("RSyncMLDevManJob::CreateL(): end");
	}
	
// -----------------------------------------------------------------------------
// RSyncMLDevManJob::CreateL()
// Creates DM job, transport override.
// -----------------------------------------------------------------------------
//
EXPORT_C void RSyncMLDevManJob::CreateL( RSyncMLSession& aSession, TSmlProfileId aProfileId, TSmlConnectionId aTransportId )
	{
	_DBG_FILE("RSyncMLDevManJob::CreateL() ECmdDevManJobCreateForTransport: begin");
	
	// check that profile is not yet open and dm sync is supported
	CClientSessionData::PanicIfAlreadyCreated( iData );
	NSmlClientAPIFeatureHandler::LeaveIfDevManNotSupportedL();
		
	CDevManJobSessionData* data = new (ELeave) CDevManJobSessionData();
	CleanupStack::PushL( data );

	// add profile id to parameters and write the buffer
	data->AddParamL( aProfileId );
	data->AddParamL( aTransportId );
	data->WriteIntegersToBufferL();
	
	TPtr8 dataPtr = data->DataBufferPtr();
	TPckgBuf<TInt> jobId;
	TIpcArgs args( &jobId, &dataPtr );
    
	// open the sub-session
	User::LeaveIfError( CreateSubSession( aSession, ECmdDevManJobCreateForTransport, args ) );
	
	data->SetProfile( aProfileId );
	data->SetIdentifier( jobId() );
	
	iData = data;
	CleanupStack::Pop(); // data
	
	_DBG_FILE("RSyncMLDevManJob::CreateL(): end");
	}
	
// -----------------------------------------------------------------------------
// RSyncMLDevManJob::OpenL().
// Opens given job. If not found, leaves with KErrNotFound.
// -----------------------------------------------------------------------------
//
EXPORT_C void RSyncMLDevManJob::OpenL( RSyncMLSession& aSession, TSmlJobId aJobId )
	{
	_DBG_FILE("RSyncMLDevManJob::OpenL(): begin");
	
	CClientSessionData::PanicIfAlreadyCreated( iData );
		
	CDevManJobSessionData* data = new (ELeave) CDevManJobSessionData();
	CleanupStack::PushL( data );

	// open the sub-session and get size of data to be received
	TPckgBuf<TInt> dataSize;
	TIpcArgs args( &dataSize, aJobId );
	User::LeaveIfError( CreateSubSession( aSession, ECmdJobOpen, args ) );
	
	data->SetIdentifier( aJobId );
	
	iData = data;
	CleanupStack::Pop(); // data
	
	CleanupClosePushL( *this ); // if leave occures, Close is called by cleanup
	
	// get the data
	GetJobL( dataSize(), EFalse );
	
	CleanupStack::Pop(); // this
	
	_DBG_FILE("RSyncMLDevManJob::OpenL(): end");
	}
	
// -----------------------------------------------------------------------------
// RSyncMLDevManJob::StopL()
// If this job has started to execute in server, it is stopped and an 
// event is emitted. If the job is still in job queue, it is merely 
// removed from the queue. If not found, leaves with KErrNotFound.
// -----------------------------------------------------------------------------
//
EXPORT_C void RSyncMLDevManJob::StopL()
	{
	_DBG_FILE("RSyncMLDevManJob::StopL(): begin");
	
	TInt jobId = Identifier();
	TIpcArgs args( jobId );
	User::LeaveIfError( SendReceive( ECmdJobStop, args ) );
	
	_DBG_FILE("RSyncMLDevManJob::StopL(): end");
	}
	

//
// RSyncMLDevManProfile
//

// -----------------------------------------------------------------------------
// RSyncMLDevManProfile::RSyncMLDevManProfile()
// Constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C RSyncMLDevManProfile::RSyncMLDevManProfile()
	: RSyncMLProfileBase()
	{
	}
	
// -----------------------------------------------------------------------------
// RSyncMLDevManProfile::OpenL()
// Opens given profile in read-only mode.
// -----------------------------------------------------------------------------
//
EXPORT_C void RSyncMLDevManProfile::OpenL( RSyncMLSession& aSession, TSmlProfileId aProfileId )
	{
	_DBG_FILE("RSyncMLDevManProfile::OpenL(): begin");
	
	OpenL( aSession, aProfileId, ESmlOpenRead );
	
	_DBG_FILE("RSyncMLDevManProfile::OpenL(): end");
	}
	
// -----------------------------------------------------------------------------
// RSyncMLDevManProfile::OpenL()
// Opens given profile.
// -----------------------------------------------------------------------------
//
EXPORT_C void RSyncMLDevManProfile::OpenL( RSyncMLSession& aSession, TSmlProfileId aProfileId, TSmlOpenMode aOpenMode )
	{
	_DBG_FILE("RSyncMLDevManProfile::OpenL(): begin");
	
	CClientSessionData::PanicIfAlreadyCreated( iData );
		
	CDevManProfileSessionData* data = CDevManProfileSessionData::NewLC();
	
	TPckgBuf<TInt> dataSize;
	TIpcArgs args( &dataSize, aProfileId, aOpenMode );
    
	// open the sub-session
	User::LeaveIfError( CreateSubSession( aSession, ECmdProfileOpenDM, args ) );
	
	data->SetIdentifier( aProfileId );
	data->SetOpenMode( (TSmlOpenMode)aOpenMode );
	
	iData = data;
	CleanupStack::Pop(); // data
	
	CleanupClosePushL( *this ); // if leave occures, Close is called by cleanup
	
	// dataSize contains now the size of the data buffer that is received next -> get data
	GetProfileL( dataSize() );
	
	CleanupStack::Pop(); // this
	
	_DBG_FILE("RSyncMLDevManProfile::OpenL(): end");
	}
	
// -----------------------------------------------------------------------------
// RSyncMLDevManProfile::CreateL()
// Creates new DM profile.
// -----------------------------------------------------------------------------
//
EXPORT_C void RSyncMLDevManProfile::CreateL( RSyncMLSession& aSession )
	{
	_DBG_FILE("RSyncMLDevManProfile::CreateL(): begin");
	
	// check that profile is not yet open and dm sync is supported
	CClientSessionData::PanicIfAlreadyCreated( iData );
	NSmlClientAPIFeatureHandler::LeaveIfDevManNotSupportedL();
		
	CDevManProfileSessionData* data = CDevManProfileSessionData::NewLC( ETrue );
	
	// create sub-session and new profile
	User::LeaveIfError( CreateSubSession( aSession, ECmdProfileCreateDM ) );

	iData = data;
	CleanupStack::Pop(); // data
	
	_DBG_FILE("RSyncMLDevManProfile::CreateL(): end");
	}
	


//
// RSyncMLDevMan ( Not supported )
//

// -----------------------------------------------------------------------------
// RSyncMLDevMan::RSyncMLDevMan()
// -----------------------------------------------------------------------------
//
EXPORT_C RSyncMLDevMan::RSyncMLDevMan()
	{
	}
	
// -----------------------------------------------------------------------------
// RSyncMLDevMan::OpenL()
// -----------------------------------------------------------------------------
//
EXPORT_C void RSyncMLDevMan::OpenL( RSyncMLSession& )
	{
	User::Leave( KErrNotSupported );
	}
	
// -----------------------------------------------------------------------------
// RSyncMLDevMan::ClearRootAclL()
// -----------------------------------------------------------------------------
//
EXPORT_C void RSyncMLDevMan::ClearRootAclL()
	{
	User::Leave( KErrNotSupported );
	}
	
// -----------------------------------------------------------------------------
// RSyncMLDevMan::SetUserInteractionNotifierTimeoutL()
// -----------------------------------------------------------------------------
//
EXPORT_C void RSyncMLDevMan::SetUserInteractionNotifierTimeoutL( TUint /*aSeconds*/ )
	{
	User::Leave( KErrNotSupported );
	}
	
// -----------------------------------------------------------------------------
// RSyncMLDevMan::UserInteractionNotifierTimeout()
// -----------------------------------------------------------------------------
//
EXPORT_C TUint RSyncMLDevMan::UserInteractionNotifierTimeout() const
	{
	return 0;
	}
	
// -----------------------------------------------------------------------------
// RSyncMLDevMan::Close()
// -----------------------------------------------------------------------------
//
EXPORT_C void RSyncMLDevMan::Close()
	{
	}
	





