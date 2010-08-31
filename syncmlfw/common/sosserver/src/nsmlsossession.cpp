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
* Description:  Symbian OS Server source.
*
*/

#include <s32mem.h>
#include <SyncMLErr.h>

#include <nsmlconstants.h>
#include <nsmldebug.h>
#include "nsmlsosserver.h"

#include "nsmldssettings.h"
#include "nsmldmsettings.h"
#include "nsmldshostclient.h"
#include <featmgr.h>
// CONSTANTS
const TInt KNSmlMutexLoopCounter = 5;

// LOCAL FUNCTION PROTOTYPES
// 
LOCAL_C TInt NSmlGrabMutex( RMutex& aMutex, const TDesC& aMutexName );
	
// --------------------------------------------------------------------------
// CNSmlSOSSession* CNSmlSOSSession::NewL( CNSmlSOSServer* aServer, CNSmlSOSHandler* aHandler )
// --------------------------------------------------------------------------
//
CNSmlSOSSession* CNSmlSOSSession::NewL( CNSmlSOSServer* aServer, CNSmlSOSHandler* aHandler )
	{
    _DBG_FILE("CNSmlSOSSession::NewL(): begin");
    
	CNSmlSOSSession* self= new (ELeave) CNSmlSOSSession( aServer, aHandler );
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(); // self
	
    _DBG_FILE("CNSmlSOSSession::NewL(): end");
	
	return self;
	}

// --------------------------------------------------------------------------
// CNSmlSOSSession::CNSmlSOSSession( CNSmlSOSServer* aServer, CNSmlSOSHandler* aHandler )
// --------------------------------------------------------------------------
//
CNSmlSOSSession::CNSmlSOSSession( CNSmlSOSServer* aServer, CNSmlSOSHandler* aHandler ) : iServer(aServer), iHandler(aHandler)
	{
	}

// --------------------------------------------------------------------------
// void CNSmlSOSSession::ConstructL()
// --------------------------------------------------------------------------
//
void CNSmlSOSSession::ConstructL()
	{FeatureManager::InitializeLibL();
    iHandler->IncreaseSessionCount();

    iObjCon = iServer->NewContainerL();
    iObjIndex = CObjectIx::NewL();
    iDsClient = CNSmlDSHostClient::NewL();
    iDSSettings = CNSmlDSSettings::NewL();
    iDMSettings = CNSmlDMSettings::NewL();
	}

// --------------------------------------------------------------------------
// CNSmlSOSSession::~CNSmlSOSSession()
// --------------------------------------------------------------------------
//
CNSmlSOSSession::~CNSmlSOSSession()
	{
    _DBG_FILE("CNSmlSOSSession::~CNSmlSOSSession(): begin");
    delete iObjIndex;
    delete iDMAuthInfo;
    iArray.Close();
    iEventBuffer.Reset();
    iEventBuffer.Close();
    
    //delete progress event buffer
    iProgressEventBuffer.Reset();
    iProgressEventBuffer.Close();
    
    iContactSuiteProgressEventBuffer.Reset();
    iContactSuiteProgressEventBuffer.Close();

    delete iDsClient;
    delete iDSSettings;
    delete iDMSettings;
    
    if ( iHandler )
        {
        iHandler->DecreaseSessionCount();    
        iHandler->ClearSessionFromJob( this );    
        }
     FeatureManager::UnInitializeLib();
    _DBG_FILE("CNSmlSOSSession::~CNSmlSOSSession(): end");
	}

// --------------------------------------------------------------------------
// void CNSmlSOSSession::ServiceL( const RMessage2& aMessage )
// --------------------------------------------------------------------------
//
void CNSmlSOSSession::ServiceL( const RMessage2& aMessage )
	{
    if ( iHandler->IsSuspended() && aMessage.Function() != ECmdEventRequest ) // Check if server is suspended.
        {
        _DBG_FILE("CNSmlSOSSession::ServiceL(): server suspended !");
        aMessage.Complete( SyncMLError::KErrSuspended ); // Server suspended, block the messege.
        return;
        }

	TRAPD(err,DispatchMessageL(aMessage));
	if ( err != KErrNone )
		{
		aMessage.Complete( err );
		}
	}

// --------------------------------------------------------------------------
// void CNSmlSOSSession::DispatchMessageL( const RMessage2& aMessage )
// --------------------------------------------------------------------------
//
void CNSmlSOSSession::DispatchMessageL( const RMessage2& aMessage )
	{
	switch( aMessage.Function() )
        {
        // requests
        case ECmdEventRequest:
            iEventMsgHandle = aMessage.Handle();
            iHandler->AddEventMsgL( aMessage );
            CompleteBufferedEventL();
            return;
        case ECmdEventRequestCancel:
            iHandler->CancelEventMsg(iEventMsgHandle);
            iEventBuffer.Reset();
            aMessage.Complete(KErrNone);
            return;
        case ECmdProgressRequest:
            iHandler->AddProgressMsgL( aMessage );
        // Handle current progress events from buffer
            CompleteBufferedProgressEventL();
            return;
		case ECmdContactSuiteProgressRequest:
            iHandler->AddContactSuiteProgressMsgL( aMessage );
        // Handle current progress events from buffer
            CompleteBufferedContactSuiteProgressEventL();
            return;
		case ECmdContactSuiteProgressRequestCancel:
            iHandler->CancelContactSuiteProgressMsg(this);
            iHandler->RegisterContactSuiteObserversWithProfileId( 0 );
        // Reset progress event buffer
            iContactSuiteProgressEventBuffer.Reset();
            aMessage.Complete(KErrNone);
            return;		    
		case ECmdContactSuiteRegisterObserversWithProfileId:
		    iHandler->RegisterContactSuiteObserversWithProfileId( aMessage.Int0() );
            return;
        case ECmdProgressRequestCancel:
            iHandler->CancelProgressMsg(this);
        // Reset progress event buffer
            iProgressEventBuffer.Reset();
            aMessage.Complete(KErrNone);
            return;
        // session
        case ECmdProfileDelete:
            DeleteProfileL( aMessage ); 
            return;
        case ECmdJobListCurrent:
            CurrentJobL( aMessage );
            return;
        case ECmdJobListStart:
            QueuedJobsL( aMessage );
            return;
        case ECmdJobListNext:
            PackIntDataL( aMessage ); 
            return;
        case ECmdProfileListStart:
            ProfileListL( aMessage ); 
            return;
        case ECmdProfileListNext:   
            PackIntDataL( aMessage ); 
            return;
        // profile
        case ECmdProfileCreateDM:
            CreateProfileL( aMessage, ESmlDevMan );
            return;
        case ECmdProfileCreateDS: 
            CreateProfileL( aMessage, ESmlDataSync );
            return;
        case ECmdProfileOpenDM: 
            OpenProfileL( aMessage, ESmlDevMan );
            return;
        case ECmdProfileOpenDS: 
            OpenProfileL( aMessage, ESmlDataSync );
            return;
        case ECmdProfileGet:
            GetProfileL( aMessage );
            return;
        case ECmdProfileSet:
            SetProfileL( aMessage );
            return;
        case ECmdProfileClose:
            CloseProfile( aMessage );
            return;
        case ECmdConnList:
            ConnectionListL( aMessage );
            return;
        case ECmdTaskDelete:
            DeleteTaskL( aMessage );
            return;
        case ECmdTaskListStart:  
            TaskListL( aMessage ); 
            return;
        case ECmdTaskListNext:   
            PackIntDataL( aMessage ); 
            return;
        // task
        case ECmdCreateTask:
            CreateTaskL( aMessage );
            return;
        case ECmdTaskOpen:
            OpenTaskL( aMessage );
            return;
        case ECmdTaskGet:
            GetTaskL( aMessage );
            return;
        case ECmdTaskFilterBufferSize:
            OpenTaskSupportedFiltersL( aMessage );
            return;
        case ECmdTaskSupportedFilters:
            GetTaskSupportedFiltersL( aMessage );
            return;
        case ECmdTaskSet:
            SetTaskL( aMessage );
            return;
        case ECmdTaskClose:
            CloseTaskL( aMessage );
            return;
        // connection
        case ECmdConnOpen:
            OpenConnectionL( aMessage );
            return;
        case ECmdConnGet:
            GetConnectionL( aMessage );
            return;
        case ECmdConnSet:
            SetConnectionL( aMessage );
            return;
        case ECmdConnClose:
            CloseConnectionL( aMessage );
            return;
        // history log
        case ECmdHistoryLogOpen:
            OpenHistoryLogL( aMessage );
            return;
        case ECmdHistoryLogGet:
            GetHistoryLogL( aMessage );
            return;
        case ECmdHistoryLogReset:
            ResetHistoryLogL( aMessage );
            return;
        case ECmdHistoryLogClose:
            CloseHistoryLogL( aMessage );
            return;
        // data provider
        case ECmdDataProviderOpen:
            OpenDataProviderL( aMessage );
            return;
		case ECmdDataProviderGet:
            GetDataProviderL( aMessage );
            return;
		case ECmdDataProviderClose:
            CloseDataProviderL( aMessage );
            return;
        // job 
        case ECmdDataSyncJobCreateForProfile:
            CreateJobL( EDSJobProfile, aMessage ); 
            return;
        case ECmdDataSyncJobCreateForProfileST:
            CreateJobL( EDSJobProfileST, aMessage ); 
            return;
        case ECmdDataSyncJobCreateForTasks:
            CreateJobL( EDSJobTasks, aMessage ); 
            return;
        case ECmdDataSyncJobCreateForTasksST:
            CreateJobL( EDSJobTasksST, aMessage ); 
            return;
        case ECmdDataSyncJobCreateForTransport:
            CreateJobL( EDSJobTransport, aMessage ); 
            return;
        case ECmdDataSyncJobCreateForTransportST:
            CreateJobL( EDSJobTransportST, aMessage ); 
            return;
        case ECmdDataSyncJobCreateForTransportAndTasks:
            CreateJobL( EDSJobTaskTrans, aMessage ); 
            return;
        case ECmdDataSyncJobCreateForTransportAndTasksST:
            CreateJobL( EDSJobTaskTransST, aMessage ); 
            return;
        case ECmdDevManJobCreate:
            CreateJobL( EDMJobProfile, aMessage ); 
            return;
        case ECmdDevManJobCreateForTransport:
            CreateJobL( EDMJobTransport, aMessage ); 
            return;
        case ECmdJobOpen:
            OpenJobL( aMessage );
            return;
        case ECmdJobGet:
            GetJobL( aMessage );
            return;
        case ECmdJobStop:
            StopJob( aMessage );
            return;
        case ECmdJobClose:
            CloseJobL( aMessage );
            return;
        case ECmdServerAlert:
            ServerAlertL( aMessage );
            return;
        case ECmdInitDMAuthInfo:
            InitDMAuthInfoL( aMessage );
            return;
        case ECmdGetDMAuthInfo:
            GetDMAuthInfoL( aMessage );
            return;
        case ECmdSetDMAuthInfo:
            SetDMAuthInfoL( aMessage );
            return;
        // FOTA
        case ECmdAddDMGenericAlert:
            AddDMGenericAlertL( aMessage );
            return;
        // FOTA end
	case ECmdAddGenericAlert:
            AddGenericAlertL( aMessage );
            return;
        default:
            aMessage.Panic( _L("CNSmlSOSSession"), KErrArgument );
            return;
        }
	}

// --------------------------------------------------------------------------
// void CNSmlSOSSession::ServerAlertL( const RMessage2& aMessage )
// Sends server alert parameters to handler.
// --------------------------------------------------------------------------
//
void CNSmlSOSSession::ServerAlertL( const RMessage2& aMessage )
    {
	_DBG_FILE("CNSmlSOSSession::ServerAlertL(): begin");

    HBufC8* buffer = HBufC8::NewLC( aMessage.GetDesLengthL(0) );
    TPtr8 bufPtr = buffer->Des();
    aMessage.ReadL( 0, bufPtr, 0 );

    iHandler->ServerAlertL( bufPtr, (TSmlUsageType) aMessage.Int1(), (TSmlProtocolVersion) aMessage.Int2(), aMessage.Int3() );

    CleanupStack::PopAndDestroy(buffer);
    aMessage.Complete(KErrNone);    
    
	_DBG_FILE("CNSmlSOSSession::ServerAlertL(): end");
    }

// --------------------------------------------------------------------------
// void CNSmlSOSSession::InitDMAuthInfoL( const RMessage2& aMessage )
// Reads DM authentication info from settings and returns size of data to client.
// --------------------------------------------------------------------------
//
void CNSmlSOSSession::InitDMAuthInfoL( const RMessage2& aMessage )
    {
    _DBG_FILE("CNSmlSOSSession::InitDMAuthInfoL(): begin");
    if ( aMessage.SecureId().iId != KUidDMServer.iUid && 
         aMessage.SecureId().iId != KUidWAPPush.iUid  &&
         aMessage.SecureId().iId != KUidSRCS.iUid )
        {
        aMessage.Complete(KErrPermissionDenied); // Complete message with error code.
        _DBG_FILE("CNSmlSOSSession::InitDMAuthInfoL(): incompatible secure id!");
        return;        
        }

    TInt ret(KErrNotFound);

    CNSmlDMSettings* settings = CNSmlDMSettings::NewLC();
    CNSmlDMProfile* prof = settings->ProfileL( aMessage.Int0() );
    
    if ( prof )
        {
        CleanupStack::PushL( prof );
        
        CBufBase* buffer = CBufFlat::NewL( KDefaultNSmlBufferGranularity );
	    CleanupStack::PushL( buffer );

        HBufC* svrNonce = prof->NonceLC( EDMProfileServerNonce );
        HBufC* cntNonce = prof->NonceLC( EDMProfileClientNonce );

	    RBufWriteStream stream( *buffer );  
	    CleanupClosePushL(stream);

        stream.WriteInt32L( svrNonce->Size() );
        stream << *svrNonce;

        stream.WriteInt32L( cntNonce->Size() );
        stream << *cntNonce;

        stream.WriteInt32L( prof->IntValue( EDMProfileAuthenticationRequired ) );
        stream.CommitL();

        CleanupStack::PopAndDestroy(&stream);        
        CleanupStack::PopAndDestroy(cntNonce); 
        CleanupStack::PopAndDestroy(svrNonce);
        
        delete iDMAuthInfo;
	    iDMAuthInfo = buffer;
	    CleanupStack::Pop( buffer );   
        
        CleanupStack::PopAndDestroy(prof);
        ret = KErrNone;
        }
    
    CleanupStack::PopAndDestroy(settings);

    TPckgBuf<TInt> sizeBuf(iDMAuthInfo->Size() );

    aMessage.WriteL( 1, sizeBuf );
    aMessage.Complete(ret);
    _DBG_FILE("CNSmlSOSSession::InitDMAuthInfoL(): end");
    }
  
// --------------------------------------------------------------------------
// void CNSmlSOSSession::GetDMAuthInfoL( const RMessage2& aMessage )
// Writes DM authentication data to client.
// --------------------------------------------------------------------------
// 
void CNSmlSOSSession::GetDMAuthInfoL( const RMessage2& aMessage )
    {
    _DBG_FILE("CNSmlSOSSession::GetDMAuthInfoL(): begin");
    
 #ifndef __WINS__
   if ( aMessage.SecureId().iId != KUidDMServer.iUid && 
         aMessage.SecureId().iId != KUidWAPPush.iUid  &&
         aMessage.SecureId().iId != KUidSRCS.iUid )
        {
        aMessage.Complete(KErrPermissionDenied); // Complete message with error code.
        _DBG_FILE("CNSmlSOSSession::GetDMAuthInfoL(): incompatible secure id!");
        return;        
        }
#endif

    if ( !iDMAuthInfo )
        {
        aMessage.Complete( KErrNotFound );
         _DBG_FILE("CNSmlSOSSession::GetDMAuthInfoL(): iDMAuthInfo is NULL");
        return;
        }
    aMessage.WriteL( 0, iDMAuthInfo->Ptr(0), 0 );
    aMessage.Complete( KErrNone );
    _DBG_FILE("CNSmlSOSSession::GetDMAuthInfoL(): end");
    }

// --------------------------------------------------------------------------
// void CNSmlSOSSession::SetDMAuthInfoL( const RMessage2& aMessage )
// Writes DM authentication info to the settings database.
// --------------------------------------------------------------------------
//
void CNSmlSOSSession::SetDMAuthInfoL( const RMessage2& aMessage )
    {
    _DBG_FILE("CNSmlSOSSession::SetDMAuthInfoL(): begin");
#ifndef __WINS__
    if ( aMessage.SecureId().iId != KUidDMServer.iUid && 
         aMessage.SecureId().iId != KUidWAPPush.iUid  &&
         aMessage.SecureId().iId != KUidSRCS.iUid )
        {
        aMessage.Complete(KErrPermissionDenied); // Complete message with error code.
        _DBG_FILE("CNSmlSOSSession::SetDMAuthInfoL(): incompatible secure id!");
        return;        
        }
#endif

    TInt ret(KErrNotFound);

    CNSmlDMSettings* settings = CNSmlDMSettings::NewLC();
    CNSmlDMProfile* prof = settings->ProfileL( aMessage.Int0() );

    if (prof)
        {
        CleanupStack::PushL(prof);    

        HBufC8* buffer = HBufC8::NewLC( aMessage.GetDesLengthL(1) );
        TPtr8 bufPtr = buffer->Des();
        aMessage.ReadL( 1, bufPtr, 0 );    

        RDesReadStream stream;
        stream.Open(bufPtr);
        CleanupClosePushL(stream);
        
        TInt maxlen(0);
        TInt pc(0);

        maxlen = stream.ReadInt32L();
        prof->SetNonceL( EDMProfileServerNonce, HBufC::NewLC(stream,maxlen)->Des() ); pc++;
        maxlen = stream.ReadInt32L();
        prof->SetNonceL( EDMProfileClientNonce, HBufC::NewLC(stream,maxlen)->Des() ); pc++;

        prof->SetIntValue(  EDMProfileAuthenticationRequired ,stream.ReadInt32L() );

        CleanupStack::PopAndDestroy(pc);
        
        ret = prof->SaveL();

        CleanupStack::PopAndDestroy(&stream);
        CleanupStack::PopAndDestroy(buffer);
        CleanupStack::PopAndDestroy(prof);
        }

    CleanupStack::PopAndDestroy(settings);

    aMessage.Complete( ret );
    _DBG_FILE("CNSmlSOSSession::SetDMAuthInfoL(): end");
    }


// FOTA

// --------------------------------------------------------------------------
// void CNSmlSOSSession::AddDMGenericAlert( const RMessage2& aMessage )
// Reads General Alert related data from aMessage and passes that data
// to handler.
// --------------------------------------------------------------------------
//
void CNSmlSOSSession::AddDMGenericAlertL( const RMessage2& aMessage )
	{
    _DBG_FILE("CNSmlSOSSession::AddDMGenericAlert(): begin");
    
	// get the buffer
	HBufC8* buffer = HBufC8::NewLC( aMessage.GetDesLengthL( 0 ) );
	TPtr8 bufPtr = buffer->Des();
	aMessage.ReadL( 0, bufPtr, 0 );
	
	// open stream to the buffer
	RDesReadStream stream;
	stream.Open( bufPtr );
	CleanupClosePushL( stream );
	
	// read data field by field
	
	// mgmt uri
	TInt length = stream.ReadInt32L();
	HBufC8* mgmtUri = HBufC8::NewLC( length );
	TPtr8 dataPtr = mgmtUri->Des();
	stream.ReadL( dataPtr, length );
	
	// meta type
	length = stream.ReadInt32L();
	HBufC8* metaType = HBufC8::NewLC( length );
	dataPtr.Set( metaType->Des() );
	stream.ReadL( dataPtr, length );
	
	// meta format
	length = stream.ReadInt32L();
	HBufC8* metaFormat = HBufC8::NewLC( length );
	dataPtr.Set( metaFormat->Des() );
	stream.ReadL( dataPtr, length );
	
	// correlator
	length = stream.ReadInt32L();
	HBufC8* correlator = HBufC8::NewLC( length );
	dataPtr.Set( correlator->Des() );
	stream.ReadL( dataPtr, length );
	
	TInt finalResult = stream.ReadInt32L();
	
	// give data to handler
	iHandler->GenericAlertL( mgmtUri, metaType, metaFormat, finalResult, correlator );
	
	CleanupStack::PopAndDestroy( 6 ); // correlator, metaFormat, metaType, mgmtUri, stream, buffer
	
    aMessage.Complete( KErrNone );
    
    _DBG_FILE("CNSmlSOSSession::AddDMGenericAlert(): end");
	}

// FOTA end

// --------------------------------------------------------------------------
// void CNSmlSOSSession::AddGenericAlert( const RMessage2& aMessage )
// Reads General Alert related data from aMessage and passes that data
// to handler.
// --------------------------------------------------------------------------
//
void CNSmlSOSSession::AddGenericAlertL( const RMessage2& aMessage )
	{
    _DBG_FILE("CNSmlSOSSession::AddGenericAlert(): begin");
    
	// get the buffer
	HBufC8* buffer = HBufC8::NewLC( aMessage.GetDesLengthL( 0 ) );
	TPtr8 bufPtr = buffer->Des();
	aMessage.ReadL( 0, bufPtr, 0 );
	
	// open stream to the buffer
	RDesReadStream stream;
	stream.Open( bufPtr );
	CleanupClosePushL( stream );
	
	// read data field by field
	
	// mgmt uri
	TInt length = stream.ReadInt32L();
	HBufC8* correlator = HBufC8::NewLC( length );
	TPtr8 dataPtr = correlator->Des();
	stream.ReadL( dataPtr, length );
	
	// mgmt Item
	TInt count = stream.ReadInt32L();
	RArray<CNSmlDMAlertItem> iItemArray;
	
	if(count > 0)
	{  
  	for(TInt i= 0; i<count; i++)
    {
    CNSmlDMAlertItem iItem; //  = new (ELeave) CNSmlDMAlertItem ();         
    // mgmt Sourceuri
		length = stream.ReadInt32L();
		iItem.iSource = HBufC8::NewLC( length );
		dataPtr.Set( iItem.iSource->Des());
		stream.ReadL( dataPtr, length );
	
		// mgmt Targeturi
		length = stream.ReadInt32L();
		iItem.iTarget = HBufC8::NewLC( length );
		dataPtr.Set( iItem.iTarget->Des());
		stream.ReadL( dataPtr, length );
	
		// meta type
		length = stream.ReadInt32L();
		iItem.iMetaType = HBufC8::NewLC( length );
		dataPtr.Set( iItem.iMetaType->Des() );
		stream.ReadL( dataPtr, length );
	
		// meta format
		length = stream.ReadInt32L();
		iItem.iMetaFormat = HBufC8::NewLC( length );
		dataPtr.Set( iItem.iMetaFormat->Des() );
		stream.ReadL( dataPtr, length );
		
		// meta Mark
    
    length = stream.ReadInt32L();
		iItem.iMetaMark = HBufC8::NewLC( length );
		dataPtr.Set( iItem.iMetaMark->Des() );
		stream.ReadL( dataPtr, length );        
		
		// Data
		   
    length = stream.ReadInt32L();
    iItem.iData = HBufC8::NewLC( length );
    dataPtr.Set( iItem.iData->Des());
    stream.ReadL( dataPtr, length );  
    
    iItemArray.AppendL(iItem);
  }
}   
	
//	TInt finalResult = stream.ReadInt32L();
	
	// give data to handler
	iHandler->GenericAlertL( correlator, &iItemArray  );
	if(count > 0)
	{  
  	for(TInt i= 0; i<count; i++)
    {
			CleanupStack::PopAndDestroy(6); // mgmtData, metaMark, metaFormat, metaType, mgmtTargetUri, mgmtUri
		}
	}
	
	CleanupStack::PopAndDestroy( 3 ); //  correlator, stream, buffer, 
	
    aMessage.Complete( KErrNone );
    
    _DBG_FILE("CNSmlSOSSession::AddGenericAlert(): end");
	}

// void CNSmlSOSSession::DeleteProfileL( const RMessage2& aMessage )
// Deletes profile with given usage type and id from settings.
// --------------------------------------------------------------------------
//
void CNSmlSOSSession::DeleteProfileL( const RMessage2& aMessage )
    {
    TInt profId = aMessage.Int0();
    // Check if client has required capabilities.
    if ( !aMessage.HasCapability( ECapabilityWriteUserData ) )
        {
        aMessage.Complete(KErrPermissionDenied); // Complete message with error code.
        return;        
        }
    // Check if profile is currently locked by another (or this) session.
    TInt ret = iHandler->IsLocked( profId );
    if ( ret != KErrNone )
        {
        aMessage.Complete(KErrLocked); // Complete message with error code.
        return;
        }
    
    ret = KErrNotFound; // this is returned if profile is not found
    
    TBool isHidden = EFalse;
    if ( profId < KMaxDataSyncID ) // DS profile
        {
       if(FeatureManager::FeatureSupported(KFeatureIdSapPolicyManagement))
       { 
			//Check Setting enforcement state  
			if ( CheckSettingEnforcementStateL( aMessage, EDataSyncEnforcement))
				{
				_DBG_FILE("CNSmlSOSSession::DeleteProfileL(): CheckSettingEnforcementStateL (DS) returned ETrue");
				return;
				}
	}
        // deleting hidden profile allowed only if request from dm server
        TBool deleteHidden = MessageFromDMServer( aMessage );
        
        CNSmlDSProfile* prof = iDSSettings->ProfileL( profId );
        if ( prof )
            {
            isHidden = prof->IntValue( EDSProfileHidden );
            TInt allowDelete = prof->IntValue( EDSProfileDeleteAllowed );
            
            delete prof;
            prof = NULL;
            if ( ( !allowDelete ) || ( !deleteHidden && isHidden ) )
                {
                // not allowed to delete, or attempted to delete hidden profile, but request 
                // is not from dm server
                aMessage.Complete( KErrAccessDenied );                
                return;
                }
            ret = iDSSettings->DeleteProfileL( profId );
            }
        }
    else    // DM Profile
        {
       if(FeatureManager::FeatureSupported(KFeatureIdSapPolicyManagement))
       { 
			//Check Setting enforcement state  
			if ( CheckSettingEnforcementStateL( aMessage, ESyncMLEnforcement) )
				{
				_DBG_FILE("CNSmlSOSSession::SetProfileL(): CheckSettingEnforcementStateL (DM) returned ETrue");
				return;
				}
	}
        CNSmlDMProfile* prof = iDMSettings->ProfileL( profId );
        if ( prof )
            {
            TInt allowDelete = prof->IntValue( EDMProfileDeleteAllowed );
            delete prof;
            prof = NULL;
            if ( !allowDelete )
                {
                aMessage.Complete( KErrAccessDenied );                
                return;
                }
            ret = iDMSettings->DeleteProfileL( profId );
            }
        }

    aMessage.Complete(ret);
    
    // send event message only if profile is not hidden
    if ( !isHidden )
    	{
	    iHandler->CompleteEventMsgL( MSyncMLEventObserver::EProfileDeleted, profId, ret );
    	}
    }

// --------------------------------------------------------------------------
// void CNSmlSOSSession::CurrentJobL( const RMessage2& aMessage )
// Returns id and type of currently processed job.
// --------------------------------------------------------------------------
//
void CNSmlSOSSession::CurrentJobL( const RMessage2& aMessage )
    {
    TInt id(0);
    TSmlUsageType type;
    iHandler->CurrentJob( id, type );

    TPckgBuf<TInt> jobId(id);
    TPckgBuf<TInt> jobType(type);

    aMessage.WriteL( 0, jobId );
    aMessage.WriteL( 1, jobType );

    aMessage.Complete(KErrNone);
    }

// --------------------------------------------------------------------------
// void CNSmlSOSSession::QueuedJobsL( const RMessage2& aMessage )
// --------------------------------------------------------------------------
//
void CNSmlSOSSession::QueuedJobsL( const RMessage2& aMessage )
    {
    // Fetch queued jobs into iArray from where they are read in PackIntDataL().
    iHandler->QueuedJobsL( iArray, (TSmlUsageType) aMessage.Int1() );
    PackIntDataL( aMessage );
    }

// --------------------------------------------------------------------------
// void CNSmlSOSSession::ProfileListL( const RMessage2& aMessage )
// Reads profile ids of selected type from settings data base.
// --------------------------------------------------------------------------
//
void CNSmlSOSSession::ProfileListL( const RMessage2& aMessage )
    {
    iArray.Reset();
    iArrayIndex = 0;
    if ( aMessage.Int1() == ESmlDataSync )
        {
        TBool includeHidden = MessageFromDMServer( aMessage ); // list hidden profiles, only if request from dm servers
        
        CNSmlDSProfileList* profList = new (ELeave) CArrayPtrFlat<CNSmlDSProfileListItem>( 8 );
        CleanupStack::PushL( PtrArrCleanupItem ( CNSmlDSProfileListItem, profList ) );
        iDSSettings->GetAllProfileListL( profList );
        
        for( TInt i = 0 ; i < profList->Count() ; i++ )
            {
            TInt id = profList->At(i)->IntValue( EDSProfileId );
            if ( !includeHidden )
            	{
            	CNSmlDSProfile* dsProf = iDSSettings->ProfileL( id );
            	if ( dsProf->IntValue( EDSProfileHidden ) )
            		{
            		// current profile is hidden and hidden profiles are not included
            		delete dsProf; dsProf = NULL;
	            	continue;
            		}
           		delete dsProf; dsProf = NULL;
            	}
            
	        iArray.AppendL( id );
            }
                    
        CleanupStack::PopAndDestroy(profList);
        }
    else 
        {
        TBool includeHidden = MessageFromDMServer( aMessage ); // list hidden profiles, only if request from dm servers
        
        CNSmlDMProfileList* profList = new (ELeave) CArrayPtrFlat<CNSmlDMProfileListItem>( 8 );
        CleanupStack::PushL( PtrArrCleanupItem ( CNSmlDMProfileListItem, profList ) );
        iDMSettings->GetProfileListL( profList );
        for( TInt i=0 ; i < profList->Count() ; i++ )
            {
            TInt id = profList->At(i)->IntValue( EDMProfileId );
            if ( !includeHidden )
            	{
            	CNSmlDMProfile* dmProf = iDMSettings->ProfileL( id );
            	if ( dmProf->IntValue( EDMProfileHidden ) )
            		{
            		// current profile is hidden and hidden profiles are not included
            		delete dmProf;
            		dmProf = NULL;
	            	continue;
            		}
           		delete dmProf;
           		dmProf = NULL;
            	}
            
	        iArray.AppendL( id );
            }
        CleanupStack::PopAndDestroy(profList);
        }        

    PackIntDataL( aMessage );
    }

// --------------------------------------------------------------------------
// void CNSmlSOSSession::PackIntDataL( const RMessage2& aMessage )
// Returns integer arrays to the client.
// --------------------------------------------------------------------------
//
void CNSmlSOSSession::PackIntDataL( const RMessage2& aMessage )
    {
    TInt startIndex(iArrayIndex);
    TInt countLeft = iArray.Count() - iArrayIndex;
    // Only the amount of KMaxArrayCount is added.
    const TInt count = ( countLeft > KMaxArrayCount ) ? KMaxArrayCount : countLeft;

    iArrayIndex += count;

    HBufC8* buffer = HBufC8::NewLC((count * KSizeofTInt32) + KSizeofTInt8);
    TPtr8 bufPtr = buffer->Des();

	RDesWriteStream stream;
	stream.Open( bufPtr );
	CleanupClosePushL(stream);
	stream.WriteUint8L(TUint(count));
	for ( TInt c = startIndex ; c < iArrayIndex ; c++ )
		{
		stream.WriteInt32L( iArray[c] );
		}
	stream.CommitL();
    aMessage.WriteL( 0, bufPtr, 0 );

    CleanupStack::PopAndDestroy(&stream);
    CleanupStack::PopAndDestroy(buffer);
    // If there are items left in the array, return KErrOverflow, else KErrNone.
    TInt ret = ( iArrayIndex < iArray.Count() ) ? KErrOverflow : KErrNone;

    aMessage.Complete( ret ); 
    }

// --------------------------------------------------------------------------
// void CNSmlSOSSession::CreateProfileL( const RMessage2& aMessage, 
// const TSmlUsageType aUsageType )
// Creates a new profile sub session.
// --------------------------------------------------------------------------
//
void CNSmlSOSSession::CreateProfileL( 
    const RMessage2& aMessage, 
    const TSmlUsageType aUsageType )
    {
    // Check if client has required capabilities.
    if ( !aMessage.HasCapability( ECapabilityWriteUserData ) )
        {
        aMessage.Complete( KErrPermissionDenied );
        return;
        }

if(FeatureManager::FeatureSupported(KFeatureIdSapPolicyManagement))
{
    //Check Setting enforcement state  
    KSettingEnforcements settingType = ESyncMLEnforcement;
    if ( aUsageType == ESmlDataSync)
        {
        _DBG_FILE("CNSmlSOSSession::CreateProfileL(): \
                    settingType = EDataSyncEnforcement");
    	settingType = EDataSyncEnforcement;
        }

    if ( CheckSettingEnforcementStateL( aMessage, settingType) )
        {
        _DBG_FILE("CNSmlSOSSession::CreateProfileL(): \
                    CheckSettingEnforcementStateL returned ETrue");
    	return;
        }
}

    CNSmlProfile* prof = new (ELeave) CNSmlProfile( aUsageType, *this );
    CleanupStack::PushL(prof);
    prof->SetCreatorId( aMessage.SecureId() );
    iObjCon->AddL( prof );
    CleanupStack::Pop(prof);
    TInt handle = iObjIndex->AddL( prof );
    TPckgBuf<TInt> handlePckg(handle);
    aMessage.WriteL(3, handlePckg, 0);
    aMessage.Complete(KErrNone);
    }

// --------------------------------------------------------------------------
// void CNSmlSOSSession::OpenProfileL( const RMessage2& aMessage, 
// const TSmlUsageType aUsageType )
// Creates a new profile sub session and reads data of existing profile.
// --------------------------------------------------------------------------
//
void CNSmlSOSSession::OpenProfileL( const RMessage2& aMessage, const TSmlUsageType aUsageType )
    {
    // Check if client has capabilities required for selected open mode.    
    if ( aMessage.Int2() == ESmlOpenRead )
        {
        if ( !aMessage.HasCapability( ECapabilityReadUserData ) )
            {
            aMessage.Complete( KErrPermissionDenied );
            return;
            }
        }
    else // aMessage.Int2() == ESmlOpenReadWrite 
        {
        if ( !aMessage.HasCapability( ECapabilityWriteUserData ) )
            {
            aMessage.Complete( KErrPermissionDenied );
            return;
            }
        }
    
    CNSmlProfile* prof = new (ELeave) CNSmlProfile( aUsageType, *this );
    CleanupStack::PushL(prof);
    iObjCon->AddL( prof );
    CleanupStack::Pop(prof);
    TInt handle = iObjIndex->AddL( prof );
    
    if ( aMessage.Int2() == ESmlOpenReadWrite )
        {        
        // Opening Read-Write mode, check if profile is already locked.
        if ( iHandler->IsLocked( aMessage.Int1() ) )
            {
            // Profile is already locked by another session.
            iObjIndex->Remove( handle );
            aMessage.Complete( KErrLocked );
            return;
            }
        // Lock the profile. 
        // Lock info is needed in handler and sub session to assure that only lock owner removes the lock.
        iHandler->LockProfile( aMessage.Int1() );
        prof->SetLocked();        
        }
    TPckgBuf<TInt> handlePckg(handle);
    aMessage.WriteL(3, handlePckg, 0);
    // Fecth and copy profile data to the object.
    TInt ret = prof->FetchDataL( aMessage.Int1(), MessageFromDMServer( aMessage ) );
    // Get size of the data buffer.
    if ( ret == KErrNone )
        {
        TPckgBuf<TInt> dataSize = prof->DataSize();
        aMessage.WriteL(0, dataSize, 0);
        }
    else // Opening subsession failed, remove subsession object.
        {
        iObjIndex->Remove( handle );
        }    
    aMessage.Complete(ret);
    }

// --------------------------------------------------------------------------
// void CNSmlSOSSession::GetProfileL( const RMessage2& aMessage )
// Writes profile data to the client.
// --------------------------------------------------------------------------
//
void CNSmlSOSSession::GetProfileL( const RMessage2& aMessage )
    {
    CNSmlProfile* prof = ProfileFromHandle( aMessage.Int3() );
    if ( !prof )
        {
        aMessage.Complete( KErrBadHandle );    
        return;
        }
    aMessage.WriteL( 0, prof->ReadData(), 0 );
    aMessage.Complete( KErrNone );
    }

// --------------------------------------------------------------------------
// void CNSmlSOSSession::ConnectionListL( const RMessage2& aMessage )
// Reads list of connections of opened profile fron settings.
// (Currently only one connection per profile is supported.)
// --------------------------------------------------------------------------
//
void CNSmlSOSSession::ConnectionListL( const RMessage2& aMessage )
    {
    iArray.Reset();
    iArrayIndex = 0;
    CNSmlProfile* prof = ProfileFromHandle( aMessage.Int3() );
    if( !prof )
        {
        aMessage.Complete( KErrBadHandle );
        return;
        }
    prof->ConnectionListL( iArray );
    PackIntDataL( aMessage );
    }

// --------------------------------------------------------------------------
// void CNSmlSOSSession::TaskListL( const RMessage2& aMessage )
// Reads list of tasks of selected profile from settings.
// --------------------------------------------------------------------------
//
void CNSmlSOSSession::TaskListL( const RMessage2& aMessage )
    {
    iArray.Reset();
    iArrayIndex = 0;
    CNSmlProfile* prof = ProfileFromHandle( aMessage.Int3() );
    if( !prof )
        {
        aMessage.Complete( KErrBadHandle );
        return;
        }
    // Get task ids and append them to iArray.
    prof->TaskListL( iArray );
    // PackIntDataL writes ids to aMessage.
    PackIntDataL( aMessage );
    }

// --------------------------------------------------------------------------
// void CNSmlSOSSession::DeleteTaskL( const RMessage2& aMessage )
// Deletes task from profile.
// --------------------------------------------------------------------------
//
void CNSmlSOSSession::DeleteTaskL( const RMessage2& aMessage )
    {
    CNSmlProfile* prof = ProfileFromHandle( aMessage.Int3() );
    if ( !prof )
        {
        aMessage.Complete( KErrBadHandle );
        return;
        }
    if ( !prof->IsLocked() ) // Profile hasn't been opened to write mode.
        {
        aMessage.Complete( KErrPermissionDenied );
        return;
        }
    TBool wasDeleted = prof->DeleteTaskL( aMessage.Int0() );
    
    if ( wasDeleted )
    	{
	    aMessage.Complete( KErrNone );
    	}
    else
    	{
	    aMessage.Complete( KErrNotFound );
    	}
    }

// --------------------------------------------------------------------------
// void CNSmlSOSSession::SetProfileL( const RMessage2& aMessage )
// Writes profile data to the database.
// --------------------------------------------------------------------------
//
void CNSmlSOSSession::SetProfileL( const RMessage2& aMessage )
    {
    TInt ret(KErrNone);
    // Check if client has required capabilities.
    if ( !aMessage.HasCapability( ECapabilityWriteUserData ) )
        {
        aMessage.Complete( KErrPermissionDenied );
        return;
        }
    
    CNSmlProfile* prof = ProfileFromHandle( aMessage.Int3() );
    if ( !prof )
        {
        aMessage.Complete( KErrBadHandle );
        return;
        }

if(FeatureManager::FeatureSupported(KFeatureIdSapPolicyManagement))
{
    //Check Setting enforcement state  
    KSettingEnforcements settingType = ESyncMLEnforcement;
    if ( prof->GetUsageType() == ESmlDataSync)
        {
        _DBG_FILE( "CNSmlSOSSession::SetProfileL(): settingType = \
                    EDataSyncEnforcement" );   
    	settingType = EDataSyncEnforcement;
        }

    if ( CheckSettingEnforcementStateL( aMessage, settingType ) )
        {
        _DBG_FILE("CNSmlSOSSession::SetProfileL(): \
                    CheckSettingEnforcementStateL returned ETrue");
    	return;
        }
}

    HBufC8* buffer = HBufC8::NewLC( aMessage.GetDesLengthL(1) );
    TPtr8 bufPtr = buffer->Des();
    aMessage.ReadL( 1, bufPtr, 0 );
    
    TInt profId = prof->ProfileId();

    MSyncMLEventObserver::TEvent event;
    if ( profId != KNSmlNullId )
        {
        event = MSyncMLEventObserver::EProfileChanged;
        }
    else
        {
        event = MSyncMLEventObserver::EProfileCreated;
        }
 
 	TBool isHidden = EFalse;
    ret = prof->UpdateDataL( profId, bufPtr, isHidden ); 
    
    CleanupStack::PopAndDestroy(buffer);   
    
    if ( !prof->IsLocked() ) // Lock profile if it is new.
        {
        iHandler->LockProfile( profId );
        prof->SetLocked();             
        }

    TPckgBuf<TInt> id(profId);

    aMessage.WriteL(0, id, 0);
    aMessage.Complete(ret);
    
    // send event msg only if profile is not hidden
    if ( !isHidden )
    	{
	    iHandler->CompleteEventMsgL( event, profId );
    	}
    }

// --------------------------------------------------------------------------
// void CNSmlSOSSession::CloseProfile( const RMessage2& aMessage )
// Closes profile sub session.
// --------------------------------------------------------------------------
//
void CNSmlSOSSession::CloseProfile( const RMessage2& aMessage )
    {
    CNSmlProfile* prof = ProfileFromHandle( aMessage.Int3() );
    if ( !prof )
        {
        aMessage.Complete( KErrBadHandle );
        return;
        }
    // Check if this profile object is locked and remove if needed.
    if( prof->IsLocked() )
        {
        iHandler->RemoveLock( prof->ProfileId() );
        }

    iObjIndex->Remove( aMessage.Int3() );
    aMessage.Complete(KErrNone);
    }

// --------------------------------------------------------------------------
// CNSmlProfile* CNSmlSOSSession::ProfileFromHandle( TUint aHandle )
// Finds sub session instance by sub session handle.
// --------------------------------------------------------------------------
//
CNSmlProfile* CNSmlSOSSession::ProfileFromHandle( TUint aHandle )
    {
    return (CNSmlProfile*)iObjIndex->At( aHandle );
    }

// --------------------------------------------------------------------------
// void CNSmlSOSSession::CreateTaskL( const RMessage2& aMessage )
// Creates a new task sub session.
// --------------------------------------------------------------------------
//
void CNSmlSOSSession::CreateTaskL( const RMessage2& aMessage )
    {
    CNSmlTask* task = new (ELeave) CNSmlTask( aMessage.Int0(), ETrue, *this );
    CleanupStack::PushL(task);
    task->SetDataProviderId( aMessage.Int1() );
    task->SetCreatorId( aMessage.SecureId() );
    
    iObjCon->AddL( task );
    CleanupStack::Pop();
    TInt handle = iObjIndex->AddL( task );
    
    TPckgBuf<TInt> handlePckg(handle);
    
    aMessage.WriteL(3, handlePckg, 0);
    aMessage.Complete(KErrNone);
    }

// --------------------------------------------------------------------------
// void CNSmlSOSSession::OpenTaskL( const RMessage2& aMessage )
// Creates a new task sub session and reads data of existing task.
// --------------------------------------------------------------------------
//
void CNSmlSOSSession::OpenTaskL( const RMessage2& aMessage )
    {
    CNSmlTask* task = new (ELeave) CNSmlTask( aMessage.Int1(), EFalse, *this );
    CleanupStack::PushL(task);
    iObjCon->AddL( task );
    CleanupStack::Pop(task);
    TInt handle = iObjIndex->AddL( task );
    TPckgBuf<TInt> handlePckg(handle);
    aMessage.WriteL(3, handlePckg, 0);
    // Fecth and copy data to the object.
    TInt ret = task->FetchDataL( aMessage.Int2() );

    if ( ret == KErrNone )
        {
        // Get size of the data buffer.
        TPckgBuf<TInt> dataSize = task->DataSize();
        aMessage.WriteL(0, dataSize, 0);
        }
    else // Opening subsession failed, remove subsession object.
        {
        iObjIndex->Remove( handle );
        }
    aMessage.Complete(ret);
    }

// --------------------------------------------------------------------------
// void CNSmlSOSSession::GetTaskL( const RMessage2& aMessage )
// Writes task data to the client.
// --------------------------------------------------------------------------
//
void CNSmlSOSSession::GetTaskL( const RMessage2& aMessage )
    {
    CNSmlTask* task = TaskFromHandle( aMessage.Int3() );
    if ( !task )
        {
        aMessage.Complete( KErrBadHandle );    
        return;
        }
    aMessage.WriteL( 0, task->ReadData(), 0 );
    aMessage.Complete(KErrNone);        
    }

// --------------------------------------------------------------------------
// void CNSmlSOSSession::OpenTaskSupportedFiltersL( const RMessage2& aMessage )
// Resolves the size of the filter buffer.
// --------------------------------------------------------------------------
//
void CNSmlSOSSession::OpenTaskSupportedFiltersL( const RMessage2& aMessage )
    {
    CNSmlTask* task = TaskFromHandle( aMessage.Int3() );
    if ( !task )
        {
        aMessage.Complete( KErrBadHandle );    
        return;
        }
    
    TPckgBuf<TInt> dataSize = task->PrepareFilterBufferL( aMessage.Int1() );
    aMessage.WriteL(0, dataSize, 0);
        
    aMessage.Complete(KErrNone);    
    }
    
// --------------------------------------------------------------------------
// void CNSmlSOSSession::GetTaskSupportedFiltersL( const RMessage2& aMessage )
// 
// --------------------------------------------------------------------------
//
void CNSmlSOSSession::GetTaskSupportedFiltersL( const RMessage2& aMessage )
    {
    CNSmlTask* task = TaskFromHandle( aMessage.Int3() );
    if ( !task )
        {
        aMessage.Complete( KErrBadHandle );    
        return;
        }
    aMessage.WriteL( 0, task->ReadFilter(), 0 );    
    aMessage.Complete(KErrNone);
    }

// --------------------------------------------------------------------------
// void CNSmlSOSSession::SetTaskL( const RMessage2& aMessage )
// Writes task data to the database.
// --------------------------------------------------------------------------
//
void CNSmlSOSSession::SetTaskL( const RMessage2& aMessage )
    {
    CNSmlTask* task = TaskFromHandle( aMessage.Int3() );
    if ( !task )
        {
        aMessage.Complete( KErrBadHandle );
        return;
        }

    HBufC8* buffer = HBufC8::NewLC( aMessage.GetDesLengthL(1) );
    TPtr8 bufPtr = buffer->Des();
    aMessage.ReadL( 1, bufPtr, 0 );

    TInt taskId(0);
    TInt ret = task->UpdateDataL( taskId, bufPtr );

    TPckgBuf<TInt> id(taskId);
    aMessage.WriteL(0, id, 0);

    CleanupStack::PopAndDestroy(buffer);
    aMessage.Complete(ret);
    }

// --------------------------------------------------------------------------
// void CNSmlSOSSession::CloseTaskL( const RMessage2& aMessage )
// Closes task sub session.
// --------------------------------------------------------------------------
//
void CNSmlSOSSession::CloseTaskL( const RMessage2& aMessage )
    {
    CNSmlTask* task = TaskFromHandle( aMessage.Int3() );
    if ( !task )
        {
        aMessage.Complete( KErrBadHandle );
        return;
        }

    iObjIndex->Remove( aMessage.Int3() );
    aMessage.Complete(KErrNone);
    }

// --------------------------------------------------------------------------
// CNSmlTask* CNSmlSOSSession::TaskFromHandle( TUint aHandle )
// Finds sub session instance by sub session handle.
// --------------------------------------------------------------------------
//
CNSmlTask* CNSmlSOSSession::TaskFromHandle( TUint aHandle )
    {
    return (CNSmlTask*)iObjIndex->At( aHandle );
    }

// --------------------------------------------------------------------------
// void CNSmlSOSSession::OpenConnectionL( const RMessage2& aMessage )
// Creates a new connection sub session and reads data from settings. 
// --------------------------------------------------------------------------
//
void CNSmlSOSSession::OpenConnectionL( const RMessage2& aMessage )
    {
    CNSmlConnection* conn = new (ELeave) CNSmlConnection( aMessage.Int1(), aMessage.Int2(), *this );
    CleanupStack::PushL(conn);
    iObjCon->AddL( conn );
    CleanupStack::Pop(conn);
    TInt handle = iObjIndex->AddL( conn );
    TPckgBuf<TInt> handlePckg(handle);
    aMessage.WriteL(3, handlePckg, 0);
    // Fecth and copy data to the object.
    TInt ret = conn->FetchDataL();

    if ( ret == KErrNone )
        {
        // Get size of the data buffer.
        TPckgBuf<TInt> dataSize = conn->DataSize();
        aMessage.WriteL(0, dataSize, 0);
        }
    else // Opening subsession failed, remove subsession object.
        {
        iObjIndex->Remove( handle );
        }
    aMessage.Complete(ret);
    }

// --------------------------------------------------------------------------
// void CNSmlSOSSession::GetConnectionL( const RMessage2& aMessage )
// Writes connection data to the client.
// --------------------------------------------------------------------------
//
void CNSmlSOSSession::GetConnectionL( const RMessage2& aMessage )
    {
    CNSmlConnection* conn = ConnectionFromHandle( aMessage.Int3() );
    if ( !conn )
        {
        aMessage.Complete( KErrBadHandle );
        return;    
        }

    aMessage.WriteL( 0, conn->ReadData(), 0 );
    aMessage.Complete( KErrNone );   
    }

// --------------------------------------------------------------------------
// void CNSmlSOSSession::SetConnectionL( const RMessage2& aMessage )
// Writes connection data to the database.
// --------------------------------------------------------------------------
//
void CNSmlSOSSession::SetConnectionL( const RMessage2& aMessage )
    {
    CNSmlConnection* conn = ConnectionFromHandle( aMessage.Int3() );
    if ( !conn )
        {
        aMessage.Complete( KErrBadHandle );
        return;
        }

    HBufC8* buffer = HBufC8::NewLC( aMessage.GetDesLengthL(0) );
    TPtr8 bufPtr = buffer->Des();
    aMessage.ReadL( 0, bufPtr, 0 );

    TInt ret = conn->UpdateDataL( bufPtr );   
    CleanupStack::PopAndDestroy(buffer);
    aMessage.Complete(ret);
    }

// --------------------------------------------------------------------------
// void CNSmlSOSSession::CloseConnectionL( const RMessage2& aMessage )
// Closes connection sub session.
// --------------------------------------------------------------------------
//
void CNSmlSOSSession::CloseConnectionL( const RMessage2& aMessage )
    {
    CNSmlConnection* conn = ConnectionFromHandle( aMessage.Int3() );
    if ( !conn )
        {
        aMessage.Complete( KErrBadHandle );
        return;
        }
    iObjIndex->Remove( aMessage.Int3() );
    aMessage.Complete(KErrNone);
    }

// --------------------------------------------------------------------------
// CNSmlConnection* CNSmlSOSSession::ConnectionFromHandle( TUint aHandle )
// Finds sub session instance by sub session handle.
// --------------------------------------------------------------------------
//
CNSmlConnection* CNSmlSOSSession::ConnectionFromHandle( TUint aHandle )
    {
    return (CNSmlConnection*)iObjIndex->At( aHandle );
    }

// --------------------------------------------------------------------------
// void CNSmlSOSSession::OpenHistoryLogL( const RMessage2& aMessage )
// Creates history log sub session and reads data from settings.
// --------------------------------------------------------------------------
//
void CNSmlSOSSession::OpenHistoryLogL( const RMessage2& aMessage )
    {
    CNSmlHistoryLog* log = new (ELeave) CNSmlHistoryLog( aMessage.Int1(), *this );
    CleanupStack::PushL(log);
    iObjCon->AddL( log );
    CleanupStack::Pop();
    TInt handle = iObjIndex->AddL( log );
    TPckgBuf<TInt> handlePckg(handle);
    aMessage.WriteL(3, handlePckg, 0);
    // Fecth and copy data to the object.
    TInt ret = log->FetchDataL();
    // Get size of the data buffer.
    if ( ret == KErrNone )
        {
        TPckgBuf<TInt> dataSize = log->DataSize();
        aMessage.WriteL(0, dataSize, 0);
        }
    else // Opening subsession failed, remove subsession object.
        {
        iObjIndex->Remove( handle );
        }
    aMessage.Complete(ret);
    }

// --------------------------------------------------------------------------
// void CNSmlSOSSession::GetHistoryLogL( const RMessage2& aMessage )
// Writes history log data to the client.
// --------------------------------------------------------------------------
//
void CNSmlSOSSession::GetHistoryLogL( const RMessage2& aMessage )
    {
    CNSmlHistoryLog* log = HistoryLogFromHandle( aMessage.Int3() );
    if ( !log )
        {
        aMessage.Complete( KErrBadHandle ); 
        return;   
        }
    aMessage.WriteL( 0, log->ReadData(), 0 );
    aMessage.Complete( KErrNone ); 
    }

// --------------------------------------------------------------------------
// void CNSmlSOSSession::ResetHistoryLogL( const RMessage2& aMessage )
// Resets history log from settings.
// --------------------------------------------------------------------------
//
void CNSmlSOSSession::ResetHistoryLogL( const RMessage2& aMessage )
    {
    CNSmlHistoryLog* log = HistoryLogFromHandle( aMessage.Int3() );
    if ( !log )
        {
        aMessage.Complete( KErrBadHandle );
        return;
        }
    log->ResetL();
    aMessage.Complete(KErrNone);
    }

// --------------------------------------------------------------------------
// void CNSmlSOSSession::CloseHistoryLogL( const RMessage2& aMessage )
// Closes history log sub session.
// --------------------------------------------------------------------------
//
void CNSmlSOSSession::CloseHistoryLogL( const RMessage2& aMessage )
    {
    CNSmlHistoryLog* log = HistoryLogFromHandle( aMessage.Int3() );
    if ( !log )
        {
        aMessage.Complete( KErrBadHandle );
        return;
        }
    iObjIndex->Remove( aMessage.Int3() );
    aMessage.Complete(KErrNone);
    }

// --------------------------------------------------------------------------
// CNSmlHistoryLog* CNSmlSOSSession::HistoryLogFromHandle( TUint aHandle )
// Finds sub session instance by sub session handle.
// --------------------------------------------------------------------------
//
CNSmlHistoryLog* CNSmlSOSSession::HistoryLogFromHandle( TUint aHandle )
    {
    return (CNSmlHistoryLog*)iObjIndex->At( aHandle );
    }

// --------------------------------------------------------------------------
// void CNSmlSOSSession::OpenDataProviderL( const RMessage2& aMessage )
// Creates data provider sub session and reads data from DS host client.
// --------------------------------------------------------------------------
//
void CNSmlSOSSession::OpenDataProviderL( const RMessage2& aMessage )
    {
    CNSmlDataProvider* dprov = new (ELeave) CNSmlDataProvider( aMessage.Int1(), *this );
    CleanupStack::PushL(dprov);
    iObjCon->AddL( dprov );
    CleanupStack::Pop(dprov);
    TInt handle = iObjIndex->AddL( dprov );
    TPckgBuf<TInt> handlePckg(handle);
    aMessage.WriteL(3, handlePckg, 0);
    // Fecth and copy data to the object.
    TRAPD(ret, dprov->FetchDataL() );
    // Get size of the data buffer.
    if ( ret == KErrNone )
        {
	    TPckgBuf<TInt> dataSize = dprov->DataSize();
    	aMessage.WriteL(0, dataSize, 0);
        }
    else // Opening subsession failed, remove subsession object.
        {
        iObjIndex->Remove( handle );
        }
        
    aMessage.Complete( ret );
    }

// --------------------------------------------------------------------------
// void CNSmlSOSSession::GetDataProviderL( const RMessage2& aMessage )
// Writes data provider data to the client.
// --------------------------------------------------------------------------
//
void CNSmlSOSSession::GetDataProviderL( const RMessage2& aMessage )
    {
    CNSmlDataProvider* dprov = DataProviderFromHandle( aMessage.Int3() );
    if ( !dprov )
        {
        aMessage.Complete( KErrBadHandle ); 
        return;   
        }
    aMessage.WriteL( 0, dprov->ReadData(), 0 );
    aMessage.Complete( KErrNone ); 
    }

// --------------------------------------------------------------------------
// void CNSmlSOSSession::CloseDataProviderL( const RMessage2& aMessage )
// Close data provider sub session.
// --------------------------------------------------------------------------
//
void CNSmlSOSSession::CloseDataProviderL( const RMessage2& aMessage )
    {
    CNSmlDataProvider* dprov = DataProviderFromHandle( aMessage.Int3() );
    if ( !dprov )
        {
        aMessage.Complete( KErrBadHandle );
        return;
        }
    iObjIndex->Remove( aMessage.Int3() );
    aMessage.Complete(KErrNone);
    }

// --------------------------------------------------------------------------
// CNSmlDataProvider* CNSmlSOSSession::DataProviderFromHandle( TUint aHandle )
// Finds sub session instance by sub session handle.
// --------------------------------------------------------------------------
//
CNSmlDataProvider* CNSmlSOSSession::DataProviderFromHandle( TUint aHandle )
    {
    return (CNSmlDataProvider*)iObjIndex->At( aHandle );
    }

// --------------------------------------------------------------------------
// void CNSmlSOSSession::CreateJobL( const TNSmlJobType aJobType, const RMessage2& aMessage )
// Creates job sub session.
// --------------------------------------------------------------------------
//
void CNSmlSOSSession::CreateJobL( const TNSmlJobType aJobType, const RMessage2& aMessage )
    {
    // Check if client has capabilities required for create job.
    if ( !aMessage.HasCapability( ECapabilityNetworkServices ) || 
         !aMessage.HasCapability( ECapabilityLocalServices ) )
        {
        aMessage.Complete( KErrPermissionDenied );
        return;
        }
        
    CNSmlJob* job = new (ELeave) CNSmlJob();
    CleanupStack::PushL(job);
    // Parse job data and set values to job.
    HBufC8* data = HBufC8::NewLC( KMaxJobDataSize );
    TPtr8 dataPtr = data->Des();
    aMessage.ReadL( 1, dataPtr, 0 );

    RDesReadStream stream;
    stream.Open(dataPtr);
    CleanupClosePushL(stream);

    job->SetOwnerSession( this ); 
    job->SetCreatorId( aMessage.SecureId() );
    job->SetJobType( aJobType );   
    job->SetProfId( stream.ReadInt32L() ); // Prof id is always first in stream.
    job->iPending = EFalse;

    switch( aJobType )
        {
        case EDSJobProfile:
            {
            job->SetUsageType( ESmlDataSync );
            break;
            }
        case EDSJobProfileST:
            {
            job->SetUsageType( ESmlDataSync );
            job->SetSyncType( (TSmlSyncType) stream.ReadInt32L() );
            break;
            }
        case EDSJobTasks:
            {
            job->SetUsageType( ESmlDataSync );
            TInt taskCount = stream.ReadInt8L(); 
            for( TInt i(0) ; i < taskCount ; i++ )
                {
                job->AddTaskIdL( stream.ReadInt32L() ); 
                }
            break;
            }
        case EDSJobTasksST:
            {
            job->SetUsageType( ESmlDataSync );
            job->SetSyncType( (TSmlSyncType) stream.ReadInt32L() );
            TInt taskCount = stream.ReadInt8L(); 
            for( TInt i(0) ; i < taskCount ; i++ )
                {
                job->AddTaskIdL( stream.ReadInt32L() ); 
                }
            break;
            }
        case EDSJobTransport:
            {
            job->SetUsageType( ESmlDataSync );
            job->SetTransportId( stream.ReadInt32L() );
            break;
            }
        case EDSJobTransportST:
            {
            job->SetUsageType( ESmlDataSync );
            job->SetTransportId( stream.ReadInt32L() );
            job->SetSyncType( (TSmlSyncType) stream.ReadInt32L() );
            break;
            }
        case EDSJobTaskTrans:
            {
            job->SetUsageType( ESmlDataSync );
            job->SetTransportId( stream.ReadInt32L() );
            TInt taskCount = stream.ReadInt8L(); 
            for( TInt i(0) ; i < taskCount ; i++ )
                {
                job->AddTaskIdL( stream.ReadInt32L() );
                }
            break;
            }
        case EDSJobTaskTransST:
            {
            job->SetUsageType( ESmlDataSync );
            job->SetTransportId( stream.ReadInt32L() );
            job->SetSyncType( (TSmlSyncType) stream.ReadInt32L() );
            TInt taskCount = stream.ReadInt8L(); 
            for( TInt i(0) ; i < taskCount ; i++ )
                {
                job->AddTaskIdL( stream.ReadInt32L() );
                }
            break;
            }
        case EDMJobProfile:
            {
            job->SetUsageType( ESmlDevMan );
            break;
            }
        case EDMJobTransport:
            {
            job->SetUsageType( ESmlDevMan );
            TBuf<10> checkapid;  //Maximum 8 chars only    
            checkapid.Num(stream.ReadInt32L());
            //Checks for IAP Id for this session,If transport id = 1022xxxx
            //1022 signifies the rest of 4 digits xxxx as IapId
            if( (checkapid.Left(KNSmlHalfTransportIdLength))
            		.Compare(KNSmlDMJobIapPrefix) == KErrNone ) 
            	{
            	TLex tempapid((checkapid.Left(KNSmlHalfTransportIdLength*2))
            			.Right(KNSmlHalfTransportIdLength));
            	TInt useiapid = KErrNotFound;
            	tempapid.Val(useiapid);            
            	DBG_FILE_CODE(useiapid, _S8("CNSmlSOSSession::CreateJobL: \
            			IapId for this session is"));
            	job->SetJobIapId(useiapid);
            	}
            	else
            	{
            	job->SetTransportId( stream.ReadInt32L() );
            	}
            break;
            }
        default:            
            break;

        }
    
    CheckJobDataL( job );
    
    CNSmlJobObject* jobObj = new (ELeave) CNSmlJobObject();
    CleanupStack::PushL(jobObj);
    iObjCon->AddL( jobObj );
    CleanupStack::Pop(jobObj);
    TInt handle = iObjIndex->AddL( jobObj );
    TPckg<TInt> handlePckg(handle);
    aMessage.WriteL( 3, handlePckg, 0);

    CleanupStack::PopAndDestroy(&stream); 
    CleanupStack::PopAndDestroy(data);

    TInt id(0);
    TInt ret = iHandler->AddJobL( job, id );
    CleanupStack::Pop(job);
    jobObj->SetJobId( id );
    TPckgBuf<TInt> jobId(id);
        
    aMessage.WriteL(0, jobId, 0 );
    aMessage.Complete( ret );
    }

// --------------------------------------------------------------------------
// void CNSmlSOSSession::OpenJobL( const RMessage2& aMessage )
// Creates job sub session and find job instance from job queue.
// --------------------------------------------------------------------------
//
void CNSmlSOSSession::OpenJobL( const RMessage2& aMessage )
    {
    // Find job data macthing with job id..
    CNSmlJob* job = iHandler->FindJob( aMessage.Int1() );
    if ( !job )
        {
        aMessage.Complete(KErrNotFound); 
        return;          
        }
    TInt currJob(0);
    TSmlUsageType type;
    iHandler->CurrentJob( currJob, type );

    if ( job->JobId() == currJob && !job->CreatorId() ) // Server alert job.
        {
        job->SetCreatorId(aMessage.SecureId()); // Set creator id to identify notifier.
        job->SetOwnerSession(this);
        }

    CNSmlJobObject* jobObj = new (ELeave) CNSmlJobObject();
    CleanupStack::PushL(jobObj);
    iObjCon->AddL( jobObj );
    CleanupStack::Pop(jobObj);
    TInt handle = iObjIndex->AddL( jobObj );
    TPckgBuf<TInt> handlePckg(handle);
    aMessage.WriteL(3, handlePckg, 0);

	jobObj->SetJobId( job->JobId() );
    TInt size = KSizeofTInt32; // Add size of profile id to data size.
    if ( job->UsageType() == ESmlDataSync )
        {
        RArray<TInt> tasks;
        job->TaskIds( tasks );
        TInt taskIdCount = tasks.Count();
        tasks.Close();
        size += KSizeofTInt8 + (taskIdCount * KSizeofTInt32); // Calculate size of task ids to data size.
        }
    // Get size of the data buffer.
    TPckgBuf<TInt> dataSize(size);

    aMessage.WriteL(0, dataSize, 0);
    aMessage.Complete( KErrNone );
    }

// --------------------------------------------------------------------------
// void CNSmlSOSSession::GetJobL( const RMessage2& aMessage )
// Writes job data to the client.
// --------------------------------------------------------------------------
//
void CNSmlSOSSession::GetJobL( const RMessage2& aMessage )
    {
    CNSmlJobObject* jobObj = JobFromHandle( aMessage.Int3() );
    if ( !jobObj )
        {
        aMessage.Complete( KErrBadHandle ); 
        return;   
        }
    
    CNSmlJob* job = iHandler->FindJob( jobObj->JobId() );
    if ( !job )
        {
        aMessage.Complete( KErrNotFound ); // job has been completed between open and get
        return;          
        }

    TInt profId = job->ProfileId();
    TInt dataSize = KSizeofTInt32;
    RArray<TInt> taskIds;
    
    if ( job->UsageType() == ESmlDataSync ) // Add task ids if DS job
        {
        RArray<TInt> taskIds;
        job->TaskIds( taskIds );
        dataSize += KSizeofTInt8 + ( taskIds.Count() * KSizeofTInt32 );
        taskIds.Close();
        }

    HBufC8* buf = HBufC8::NewLC( dataSize );
    TPtr8 bufPtr = buf->Des();

    RDesWriteStream stream;
    stream.Open( bufPtr );
    CleanupClosePushL(stream);

    stream.WriteInt32L( profId );
    
    if ( job->UsageType() == ESmlDataSync )
    	{
	    // Write count of tasks and then task ids.
    	stream.WriteInt8L( taskIds.Count() );
	    if ( taskIds.Count() ) // Add task ids if DS job
	        {
	        for( TInt i(0) ; i < taskIds.Count() ; i++ )
	            {
	            stream.WriteInt32L( taskIds[i] );
	            }
	        }
    	}

    taskIds.Close();
    stream.CommitL();
    aMessage.WriteL( 0, bufPtr, 0 );
    CleanupStack::PopAndDestroy(&stream); 
    CleanupStack::PopAndDestroy(buf);
    aMessage.Complete(KErrNone); 
    }

// --------------------------------------------------------------------------
// CNSmlJobObject* CNSmlSOSSession::JobFromHandle( TUint aHandle )
// Finds sub session instance by sub session handle.
// --------------------------------------------------------------------------
//
CNSmlJobObject* CNSmlSOSSession::JobFromHandle( TUint aHandle )
	{	
    return (CNSmlJobObject*)iObjIndex->At( aHandle );
	}

// --------------------------------------------------------------------------
// void CNSmlSOSSession::StopJob( const RMessage2& aMessage )
// Removes job from queue or cancels the job if currently running.
// --------------------------------------------------------------------------
//
void CNSmlSOSSession::StopJob( const RMessage2& aMessage )
    {
    CNSmlJobObject* jobObj = JobFromHandle( aMessage.Int3() );
    if( !jobObj )
        {
        aMessage.Complete(KErrBadHandle);
        return;
        }
    // Handler checks if Secure Id matches with one who created the job. 
    aMessage.Complete( iHandler->StopJob( jobObj->JobId(), aMessage.SecureId() ) );
    }

// --------------------------------------------------------------------------
// void CNSmlSOSSession::CloseJobL( const RMessage2& aMessage )
// Closes job sub session.
// --------------------------------------------------------------------------
//
void CNSmlSOSSession::CloseJobL( const RMessage2& aMessage )
    {
    CNSmlJobObject* jobObj = JobFromHandle( aMessage.Int3() );
    if ( !jobObj )
        {
        aMessage.Complete( KErrBadHandle );
        return;
        }
    iObjIndex->Remove( aMessage.Int3() );
    aMessage.Complete(KErrNone);
    }

// --------------------------------------------------------------------------
// void CNSmlSOSSession::CheckJobDataL( CNSmlJob* aJob )
// --------------------------------------------------------------------------
//
void CNSmlSOSSession::CheckJobDataL( CNSmlJob* aJob )
	{
	TInt ret = KErrNone;
	
    switch( aJob->JobType() )
        {
        case EDSJobProfile:
        case EDSJobProfileST:
        case EDSJobTasks:
        case EDSJobTasksST:
        case EDSJobTransport:
        case EDSJobTransportST:
        case EDSJobTaskTrans:
        case EDSJobTaskTransST:
        	{
		    CNSmlDSProfile* prof = iDSSettings->ProfileL( aJob->ProfileId() );
		    if ( !prof )    // profile not found, leave error
		        {
		        ret = KErrArgument;
		        }
		    
		    delete prof;
        	}
            break;
        case EDMJobProfile:
        case EDMJobTransport:
        	{
		    CNSmlDMProfile* prof = iDMSettings->ProfileL( aJob->ProfileId() );
		    if ( !prof )    // profile not found, leave error
		        {
		        ret = KErrArgument;
		        }
		    
		    delete prof;
        	}
            break;
        default:            
            break;

        }
    
    User::LeaveIfError( ret );
	}


// --------------------------------------------------------------------------
// TBool CNSmlSOSSession::CheckSettingEnforcementStateL( const RMessage2& 
// aMessage, KSettingEnforcements aSetting )
// Checks is setting enforcement activated. If activated block all messages 
// except messages from DM server
// --------------------------------------------------------------------------
//
TBool CNSmlSOSSession::CheckSettingEnforcementStateL( 
    const RMessage2& aMessage, 
    KSettingEnforcements aSetting )
    {
    	if(!FeatureManager::FeatureSupported(KFeatureIdSapPolicyManagement))
    	{
    		User::Leave(KErrNotSupported);
    	}
    _DBG_FILE( "CNSmlSOSSession::CheckSettingEnforcementStateL(): start" );
	//if message was sent by DM server it is always ok, but if it comes from 
	// other instance setting enforcement will be checked
	if ( !MessageFromDMServer( aMessage ) )
	    {
		CSettingEnforcementInfo* info = CSettingEnforcementInfo::NewL();
		CleanupStack::PushL( info);

		TBool enforcementActive = EFalse;
		User::LeaveIfError( info->EnforcementActive( aSetting, 
		                    enforcementActive ) );

		CleanupStack::PopAndDestroy( info);

		//complete message is client hasn't got permission to data
		if ( enforcementActive )
		    {
            aMessage.Complete( KErrPermissionDenied );
            return ETrue;
		    }
	    }

    _DBG_FILE("CNSmlSOSSession::CheckSettingEnforcementStateL(): end");
	return EFalse;
    }



// --------------------------------------------------------------------------
// TBool CNSmlSOSSession::MessageFromDMServer( const RMessage2& aMessage )
// Checks if message was sent by DM server.
// --------------------------------------------------------------------------
//
TBool CNSmlSOSSession::MessageFromDMServer( const RMessage2& aMessage )
	{
	TInt id = aMessage.SecureId().iId;
	if ( id == KUidDMServer.iUid || id == KUidDmFotaServer.iUid)
		{
		return ETrue;
		}
		
	return EFalse;
	}
// --------------------------------------------------------------------------
// void CNSmlSOSSession::StoreToEventBuffer( MSyncMLEventObserver::TEvent aEvent, TInt aJobId, TInt aStatus )
// Adds job event to buffer for later completion.
// --------------------------------------------------------------------------
//
void CNSmlSOSSession::StoreToEventBuffer( MSyncMLEventObserver::TEvent aEvent, TInt aJobId, TInt aStatus )
    {
    _DBG_FILE("CNSmlSOSSession::StoreToEventBuffer : Begin");
    TNSmlJobEvent jobEvent;
    jobEvent.iEvent = aEvent;
    jobEvent.iJobId = aJobId;
    jobEvent.iEndStatus = aStatus;
    iEventBuffer.Append( jobEvent );           
    _DBG_FILE("CNSmlSOSSession::StoreToEventBuffer : End");
    }

// --------------------------------------------------------------------------
// void CNSmlSOSSession::CompleteBufferedEvent()
// Completes delayed job event.
// --------------------------------------------------------------------------
//    
void CNSmlSOSSession::CompleteBufferedEventL()
    {
    _DBG_FILE("CNSmlSOSSession::CompleteBufferedEventL : Begin");
    if ( iEventBuffer.Count() > 0 )    
        {        
        TNSmlJobEvent jobEvent;
        jobEvent = iEventBuffer[0];        
        iHandler->CompleteEventMsgL( jobEvent.iEvent, jobEvent.iJobId, jobEvent.iEndStatus );                    
        DBG_FILE_CODE( jobEvent.iEvent, _S8("Buffered Event Completed"));
        iEventBuffer.Remove(0);
        }
    _DBG_FILE("CNSmlSOSSession::CompleteBufferedEventL : End");
    }

// --------------------------------------------------------------------------
// CNSmlSOSSession::StoreToProgessEventBuffer( const TNSmlProgressEvent& aProgressEventItem )
// Adds progress event to progress buffer.
// --------------------------------------------------------------------------
//    
void CNSmlSOSSession::StoreToProgessEventBuffer( const TNSmlProgressEvent& aProgressEventItem )
    {
    _DBG_FILE("CNSmlSOSSession::StoreToProgessEventBuffer : Begin");
    
    // Acquire mutex
    RMutex mutex;
	TInt mutexError = NSmlGrabMutex( mutex, KNSmlSOSServerMutexName() );
	
    switch ( aProgressEventItem.iEventType )
        {
        case ENSmlSyncError:
            iProgressEventBuffer.Append( aProgressEventItem );
            if( iHandler->GetContactSuiteRegisteredProfileId() == iHandler->GetCurrentOngoingSessionProfileId())
                {
                iContactSuiteProgressEventBuffer.Append( aProgressEventItem );
                }
        break;
        
        case ENSmlSyncProgress:
            iProgressEventBuffer.Append( aProgressEventItem );
            if( iHandler->GetContactSuiteRegisteredProfileId() == iHandler->GetCurrentOngoingSessionProfileId())
                {
                iContactSuiteProgressEventBuffer.Append( aProgressEventItem );
                }
        break;
        
        case ENSmlModifications:
            {
            if ( iProgressEventBuffer.Count() == 0 )
                {                        
                iProgressEventBuffer.Append( aProgressEventItem );    
                }
            else
                {
                // Add modifications to buffer
                BufferDoModifications( aProgressEventItem );                    
                }
            
            if( iHandler->GetContactSuiteRegisteredProfileId() == iHandler->GetCurrentOngoingSessionProfileId())
                {
                if ( iContactSuiteProgressEventBuffer.Count() == 0 )
                    {
                       iContactSuiteProgressEventBuffer.Append( aProgressEventItem );
                    }
                else
                    {
                    // Add modifications to buffer
                    BufferContactSuiteDoModifications( aProgressEventItem );                    
                    }
                }

            }                
        break;
            
        default:
        break;               
        };
        
    if ( !mutexError )
        {
        mutex.Signal(); // Finished buffering
        mutex.Close();
        }
    
    // Handle buffered progress events
    TRAP_IGNORE( CompleteBufferedProgressEventL() );
    if( iHandler->GetContactSuiteRegisteredProfileId() == iHandler->GetCurrentOngoingSessionProfileId())
        {
        TRAP_IGNORE( CompleteBufferedContactSuiteProgressEventL() );
        }
    
    _DBG_FILE("CNSmlSOSSession::StoreToProgessEventBuffer : End");   
    }

// --------------------------------------------------------------------------
// void CNSmlSOSSession::CompleteBufferedProgressEventL()
// Completes delayed progress event. Buffer is created and sent to client.
// --------------------------------------------------------------------------
//    
void CNSmlSOSSession::CompleteBufferedProgressEventL()
    {        
    _DBG_FILE("CNSmlSOSSession::CompleteBufferedProgressEventL : Begin");
    
    // Acquire mutex
    RMutex mutex;
	TInt mutexError = NSmlGrabMutex( mutex, KNSmlSOSServerMutexName() );
	if ( !mutexError )
	    {
	    CleanupClosePushL( mutex );
	    }
	    
    if ( iProgressEventBuffer.Count() > 0 )    
        {    
        HBufC8* buf = NULL;    
        TNSmlProgressEvent event = iProgressEventBuffer[0];
        
        switch ( iProgressEventBuffer[0].iEventType )
            {
            case ENSmlSyncError:
                {
                buf = iHandler->WriteSyncErrorL( event.iErrorLevel, event.iInfo1, event.iInfo2, event.iInfo3, event.iInfo4 );                        
                }
            break;
            
            case ENSmlSyncProgress:
                {
                buf = iHandler->WriteSyncProgressL( event.iProgressStatus, event.iInfo1, event.iInfo2 );                                
                }                
            break;
            
            case ENSmlModifications:
                {
                buf = iHandler->WriteSyncModificationsL( event.iInfo1, event.iClientMods, event.iServerMods );        
                }                
            break;
            
            default:
            break;               
            };
        
	   	        
        if ( iHandler->CompleteBufProgressMsg( *buf, this )) // if owner not found do not remove from buffer
            {
            iProgressEventBuffer.Remove(0);        
            }  
            
        delete buf;
        buf = NULL;   
        
        
        }
    
    if ( !mutexError )
        {
        // Release mutex
        mutex.Signal();   
        CleanupStack::PopAndDestroy( &mutex );
        }

    _DBG_FILE("CNSmlSOSSession::CompleteBufferedProgressEventL : End");
    }

void CNSmlSOSSession::CompleteBufferedContactSuiteProgressEventL()
    {        
    _DBG_FILE("CNSmlSOSSession::CompleteBufferedContactSuiteProgressEventL : Begin");
    
    if( iHandler->GetCurrentOngoingSessionProfileId() == 0 )
        {
        return;
        }
    // Acquire mutex
    RMutex mutex;
	TInt mutexError = NSmlGrabMutex( mutex, KNSmlSOSServerMutexName() );
	if ( !mutexError )
	    {
	    CleanupClosePushL( mutex );
	    }
	    
    if ( iContactSuiteProgressEventBuffer.Count() > 0 )    
        {    
        HBufC8* buf = NULL;    
        TNSmlProgressEvent event = iContactSuiteProgressEventBuffer[0];
        
        switch ( iContactSuiteProgressEventBuffer[0].iEventType )
            {
            case ENSmlSyncError:
                {
                buf = iHandler->WriteSyncErrorL( event.iErrorLevel, event.iInfo1, event.iInfo2, event.iInfo3, event.iInfo4 );                        
                }
            break;
            
            case ENSmlSyncProgress:
                {
                buf = iHandler->WriteSyncProgressL( event.iProgressStatus, event.iInfo1, event.iInfo2 );                                
                }                
            break;
            
            case ENSmlModifications:
                {
                buf = iHandler->WriteSyncModificationsL( event.iInfo1, event.iClientMods, event.iServerMods );        
                }                
            break;
            
            default:
            break;               
            };
        
	   	        
		/*
        if ( iHandler->CompleteBufProgressMsg( *buf, this )) // if owner not found do not remove from buffer
            {
            iProgressEventBuffer.Remove(0);        
            }  
        */
		if ( iHandler->CompleteBufContactSuiteProgressMsg( *buf, this )) // if owner not found do not remove from buffer
            {
            iContactSuiteProgressEventBuffer.Remove(0);        
            }  

        delete buf;
        buf = NULL;   
        
        
        }
    
    if ( !mutexError )
        {
        // Release mutex
        mutex.Signal();   
        CleanupStack::PopAndDestroy( &mutex );
        }

    _DBG_FILE("CNSmlSOSSession::CompleteBufferedProgressEventL : End");
    }


// --------------------------------------------------------------------------
// CNSmlSOSSession::HandleModifications( const TNSmlProgressEvent& aProgressEventItem )
// Handle buffered modifications.
// --------------------------------------------------------------------------
//  
void CNSmlSOSSession::BufferDoModifications( const TNSmlProgressEvent& aProgressEventItem )
    {
    _DBG_FILE("CNSmlSOSSession::BufferDoModifications : Begin");
    
    TInt last = iProgressEventBuffer.Count()-1;
         
    // Is the last event modifications event     
    TBool doSum = iProgressEventBuffer[last].iEventType == ENSmlModifications ? ETrue : EFalse;
    
    if ( doSum )   
        {        
        iProgressEventBuffer[last].iClientMods.iNumAdded += aProgressEventItem.iClientMods.iNumAdded; 
        iProgressEventBuffer[last].iClientMods.iNumReplaced += aProgressEventItem.iClientMods.iNumReplaced;
        iProgressEventBuffer[last].iClientMods.iNumMoved += aProgressEventItem.iClientMods.iNumMoved;
        iProgressEventBuffer[last].iClientMods.iNumDeleted += aProgressEventItem.iClientMods.iNumDeleted;
        iProgressEventBuffer[last].iClientMods.iNumFailed += aProgressEventItem.iClientMods.iNumFailed;

        iProgressEventBuffer[last].iServerMods.iNumAdded += aProgressEventItem.iServerMods.iNumAdded; 
        iProgressEventBuffer[last].iServerMods.iNumReplaced += aProgressEventItem.iServerMods.iNumReplaced;
        iProgressEventBuffer[last].iServerMods.iNumMoved += aProgressEventItem.iServerMods.iNumMoved;
        iProgressEventBuffer[last].iServerMods.iNumDeleted += aProgressEventItem.iServerMods.iNumDeleted;
        iProgressEventBuffer[last].iServerMods.iNumFailed += aProgressEventItem.iServerMods.iNumFailed;        
        }
    else
        {
        iProgressEventBuffer.Append( aProgressEventItem );       
        }

    _DBG_FILE("CNSmlSOSSession::BufferDoModifications : End");
    }

// --------------------------------------------------------------------------
// CNSmlSOSSession::HandleModifications( const TNSmlProgressEvent& aProgressEventItem )
// Handle buffered modifications.
// --------------------------------------------------------------------------
//  
void CNSmlSOSSession::BufferContactSuiteDoModifications( const TNSmlProgressEvent& aProgressEventItem )
    {
    _DBG_FILE("CNSmlSOSSession::BufferDoModifications : Begin");
    
    TInt last = iContactSuiteProgressEventBuffer.Count()-1;

    TBool doSum = iContactSuiteProgressEventBuffer[last].iEventType == ENSmlModifications ? ETrue : EFalse;
    if ( doSum )   
            {        
                iContactSuiteProgressEventBuffer[last].iClientMods.iNumAdded += aProgressEventItem.iClientMods.iNumAdded; 
                iContactSuiteProgressEventBuffer[last].iClientMods.iNumReplaced += aProgressEventItem.iClientMods.iNumReplaced;
                iContactSuiteProgressEventBuffer[last].iClientMods.iNumMoved += aProgressEventItem.iClientMods.iNumMoved;
                iContactSuiteProgressEventBuffer[last].iClientMods.iNumDeleted += aProgressEventItem.iClientMods.iNumDeleted;
                iContactSuiteProgressEventBuffer[last].iClientMods.iNumFailed += aProgressEventItem.iClientMods.iNumFailed;
        
                iContactSuiteProgressEventBuffer[last].iServerMods.iNumAdded += aProgressEventItem.iServerMods.iNumAdded; 
                iContactSuiteProgressEventBuffer[last].iServerMods.iNumReplaced += aProgressEventItem.iServerMods.iNumReplaced;
                iContactSuiteProgressEventBuffer[last].iServerMods.iNumMoved += aProgressEventItem.iServerMods.iNumMoved;
                iContactSuiteProgressEventBuffer[last].iServerMods.iNumDeleted += aProgressEventItem.iServerMods.iNumDeleted;
                iContactSuiteProgressEventBuffer[last].iServerMods.iNumFailed += aProgressEventItem.iServerMods.iNumFailed;
            }
        else
            {
            iContactSuiteProgressEventBuffer.Append( aProgressEventItem );
            }

    
    _DBG_FILE("CNSmlSOSSession::BufferDoModifications : End");
    }

// --------------------------------------------------------------------------
// CNSmlDSSettings& CNSmlSOSSession::DSSettings()
// Returns reference to ds settings.
// --------------------------------------------------------------------------
//  
TBool CNSmlSOSSession::ProgressEventsBuffered()
    {
    _DBG_FILE("CNSmlSOSSession::ProgressEventsBuffered() : Begin");
    return iProgressEventBuffer.Count() == 0 ? EFalse : ETrue;    
    
    }
    
// --------------------------------------------------------------------------
// CNSmlDSSettings& CNSmlSOSSession::DSSettings()
// Returns reference to ds settings.
// --------------------------------------------------------------------------
//  
CNSmlDSSettings& CNSmlSOSSession::DSSettings()
    {
    return *iDSSettings;
    }

// --------------------------------------------------------------------------
// CNSmlDMSettings& CNSmlSOSSession::DMSettings()
// Returns reference to dm settings.
// --------------------------------------------------------------------------
//   
CNSmlDMSettings& CNSmlSOSSession::DMSettings()
    {
    return *iDMSettings;    
    }

// --------------------------------------------------------------------------
// CNSmlDSHostClient& CNSmlSOSSession::HostClient()
// Returns reference to ds hoist client.
// --------------------------------------------------------------------------
//     
CNSmlDSHostClient& CNSmlSOSSession::HostClient()
    {
    return *iDsClient;    
    }

// ============================= LOCAL FUNCTIONS ===============================

// --------------------------------------------------------------------------------
// void NSmlGrabMutex( RMutex& aMutex, const TDesC& aMutexName )
// --------------------------------------------------------------------------------
LOCAL_C TInt NSmlGrabMutex( 
    RMutex& aMutex,             // Mutex to acquire.
    const TDesC& aMutexName )   // Name of the mutex.
	{
	TInt returnValue( KErrGeneral );
	TInt counter( 0 );
	while( counter < KNSmlMutexLoopCounter )
		{
		counter++;
		TInt ret = aMutex.CreateGlobal(aMutexName);
		if( ret == KErrNone ) // We created the mutex -> Issue wait()
			{
			aMutex.Wait();
			break;
			}
		if( ret == KErrAlreadyExists ) // Mutex already existed -> Open it
			{
			ret = aMutex.OpenGlobal(aMutexName);
			if( ret == KErrNone ) // We got handle to the mutex -> Issue wait()
				{
				aMutex.Wait();
				break;
				}
			}
		}	
    if ( counter < KNSmlMutexLoopCounter )
        {
        returnValue = KErrNone;
        }
    return returnValue;
	}
