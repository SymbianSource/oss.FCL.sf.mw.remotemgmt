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
#include <ecom.h>
#include <implementationinformation.h>
//#include <syncmlnotifier.h>
#include <e32property.h>
#include <DevManInternalCRKeys.h>
#include <centralrepository.h>
#include <nsmlconstants.h>
#include <nsmldebug.h>
#include <nsmldsconstants.h>

#include "nsmlsosserver.h"
#include "nsmltransport.h"
#include "nsmlsosthread.h"
#include "nsmlprivatepskeys.h"

// --------------------------------------------------------------------------
// CNSmlSOSHandler* CNSmlSOSHandler::NewL()
// --------------------------------------------------------------------------
//
CNSmlSOSHandler* CNSmlSOSHandler::NewL()
    {
	CNSmlSOSHandler* self= new (ELeave) CNSmlSOSHandler();
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(); // self
	return self;    
    }

// --------------------------------------------------------------------------
// void CNSmlSOSHandler::ConstructL()
// --------------------------------------------------------------------------
//
void CNSmlSOSHandler::ConstructL()
    {
    iMsgQueue = CNSmlMessageQueue::NewL(this);
    iContactSuiteObserverProfileId = 0;
    }

// --------------------------------------------------------------------------
// CNSmlSOSHandler::CNSmlSOSHandler()
// --------------------------------------------------------------------------
//
CNSmlSOSHandler::CNSmlSOSHandler() : CActive(EPriorityStandard), iSessionCount(0)
    {     
    CActiveScheduler::Add(this);
    }

// --------------------------------------------------------------------------
// CNSmlSOSHandler::~CNSmlSOSHandler()
// --------------------------------------------------------------------------
//
CNSmlSOSHandler::~CNSmlSOSHandler()
    {
    Cancel();
    delete iMsgQueue;
    
    delete iParams;
    delete iThreadObserver;
  	
    iProfileLocks.Close();
    iSessionLib.Close();
    iEventMsgs.Close();
    iProgressMsgs.Close();
	iContactSuiteProgressMsgs.Close();
    
    iWrtStream.Close();
    }

// --------------------------------------------------------------------------
// void CNSmlSOSHandler::LockProfile( const TInt aProfId )
// Adds profile to the array of locked profiles.
// --------------------------------------------------------------------------
//
void CNSmlSOSHandler::LockProfile( const TInt aProfId )
    {
    iProfileLocks.Append( aProfId );
    }

// --------------------------------------------------------------------------
// void CNSmlSOSHandler::RemoveLock( const TInt aProfId )
// Removes lock from profile.
// --------------------------------------------------------------------------
//
void CNSmlSOSHandler::RemoveLock( const TInt aProfId )
    {
    TInt index = iProfileLocks.Find( aProfId );

    if ( index != KErrNotFound )
        {
        iProfileLocks.Remove(index);
        }
    }

// --------------------------------------------------------------------------
// TBool CNSmlSOSHandler::IsLocked( const TInt aProfId )
// Checks whether profile is locked or not.
// --------------------------------------------------------------------------
//
TBool CNSmlSOSHandler::IsLocked( const TInt aProfId )
    {
    if ( iProfileLocks.Find( aProfId ) != KErrNotFound )
        {
        return ETrue;
        }
    return EFalse;
    }

// --------------------------------------------------------------------------
// TInt CNSmlSOSHandler::AddJobL( CNSmlJob* aJob, TInt& aJobId )
// Adds a created job to the job queue.
// --------------------------------------------------------------------------
//
TInt CNSmlSOSHandler::AddJobL( CNSmlJob* aJob, TInt& aJobId )
    {
    aJobId = CreateJobId();
    aJob->SetJobId( aJobId );
    if ( !iCurrentJob )
        {
        iCurrentJob = aJob;
        
        if ( iCurrentJob->UsageType() == ESmlDataSync )
            {
            StartDSSyncL();
            }
        else
            {
            StartDMSyncL();
            }
        }
    else
        {
        if ( SearchSimilarJobs( aJob ) )
            {
            delete aJob;
            aJob = NULL;
            return KErrAlreadyExists;
            }
        CNSmlJob* tempJob = iCurrentJob;
        
        while ( tempJob->iNext )
            {            
            tempJob = tempJob->iNext;
            }

        tempJob->iNext = aJob;    
        }
    return KErrNone;
    }

// --------------------------------------------------------------------------
// CNSmlJob* CNSmlSOSHandler::FindJob( TInt aJobId )
// Finds job from queue.
// --------------------------------------------------------------------------
//
CNSmlJob* CNSmlSOSHandler::FindJob( TInt aJobId )
    {
    if ( iCurrentJob )
        {
        CNSmlJob* job = iCurrentJob;
        if ( job->JobId() == aJobId )
            {
            return iCurrentJob; 
            }
        while ( job->iNext )
            {
            job = job->iNext;
            if ( job->JobId() == aJobId )
                {
                return job;
                }
            }
        }
    return NULL;   
    }

// --------------------------------------------------------------------------
// TInt CNSmlSOSHandler::CreateJobId()
// Creates an id to the new job.
// --------------------------------------------------------------------------
//
TInt CNSmlSOSHandler::CreateJobId()
    {
    return iJobIndex++;
    }

// --------------------------------------------------------------------------
// TInt CNSmlSOSHandler::StopJob( const TInt aJobId, const TInt aCreatorId )
// Removes job from queue or cancels the job if currently running.
// --------------------------------------------------------------------------
//
TInt CNSmlSOSHandler::StopJob( const TInt aJobId, const TInt aCreatorId )
    {
    if ( !iCurrentJob )
        {
        return KErrNotFound;
        }
    if ( iCurrentJob->JobId() == aJobId )
        {
        if ( iCurrentJob->CreatorId() != aCreatorId )
            {
            return KErrPermissionDenied;
            }
        else
            {
            if ( iCurrentJob->UsageType() == ESmlDataSync )
                {
                CancelDSSync();
                }
            else
                {
                CancelDMSync();
                }           
            return KErrNone;
            }
        }
    else
        {
        CNSmlJob* tempJob = iCurrentJob;
        CNSmlJob* prevJob = iCurrentJob;
        while ( tempJob->iNext )
            {
            prevJob = tempJob;
            tempJob = tempJob->iNext;
            if ( tempJob->JobId() == aJobId )
                {
 				if ( tempJob->CreatorId() != aCreatorId )
                    {
                    return KErrPermissionDenied;
                    }
                else
                    {
                    CNSmlJob* nextJob = tempJob->iNext;
                    TRAP_IGNORE( CompleteEventMsgL( MSyncMLEventObserver::EJobStop, tempJob->JobId(), KErrNone ));  
                    delete tempJob;
                    prevJob->iNext = nextJob;
                    tempJob = nextJob;
                    return KErrNone;
                    }
                }            
            }
        }
    return KErrNotFound;
    }

// --------------------------------------------------------------------------
// void CNSmlSOSHandler::CurrentJob( TInt& aId, TSmlUsageType& aType )
// Return the id and type of a job currently running.
// --------------------------------------------------------------------------
//
void CNSmlSOSHandler::CurrentJob( TInt& aId, TSmlUsageType& aType )
    {
    if ( iCurrentJob )
        {
        aId = iCurrentJob->JobId();
        aType = iCurrentJob->UsageType();       
        }
    else    // No job running.
        {
        aId = KNSmlNullId;
        }
    }

// --------------------------------------------------------------------------
// void CNSmlSOSHandler::QueuedJobsL( RArray<TInt>& aArray, const TSmlUsageType aType )
// Returns an array of ids of queued jobs of seleted type.
// --------------------------------------------------------------------------
//
void CNSmlSOSHandler::QueuedJobsL( RArray<TInt>& aArray, const TSmlUsageType aType )
    {
    // Start from current job. If the usage type matches, append id to the array.
    if ( iCurrentJob )
        {
        CNSmlJob* job = iCurrentJob;

        if ( job->UsageType() == aType )
            {
            aArray.AppendL( job->JobId() );    
            }
        while ( job->iNext )
            {
            job = job->iNext;
            if ( job->UsageType() == aType )
                {
                aArray.AppendL( job->JobId() );    
                }
            }
        }
    }

// --------------------------------------------------------------------------
// void CNSmlSOSHandler::AddProgressMsgL( const RMessage2& aMessage )
// Adds progress message to the message array.
// --------------------------------------------------------------------------
//
void CNSmlSOSHandler::AddProgressMsgL( const RMessage2& aMessage )
    {
    iProgressMsgs.AppendL( aMessage );
    }

// --------------------------------------------------------------------------
// void CNSmlSOSHandler::AddProgressMsgL( const RMessage2& aMessage )
// Adds progress message to the message array.
// --------------------------------------------------------------------------
//
void CNSmlSOSHandler::AddContactSuiteProgressMsgL( const RMessage2& aMessage )
    {
        iContactSuiteProgressMsgs.AppendL( aMessage );
    }


// --------------------------------------------------------------------------
// TBool CNSmlSOSHandler::CompleteBufProgressMsg( const TDesC8& aBuf, 
// const CNSmlSOSSession* aSession )
// Write data to the progress message and completes it.
// --------------------------------------------------------------------------
//
TBool CNSmlSOSHandler::CompleteBufProgressMsg( 
const TDesC8& aBuf, 
const CNSmlSOSSession* aSession  )
    {
    _DBG_FILE("CNSmlSOSHandler::CompleteBufProgressMsg : Begin");
    TBool request( EFalse );    
        
    // Session info is stored to job and compared to session of the message.
    for( TInt i(0); i < iProgressMsgs.Count(); i++ )
        {
        if ( aSession == iProgressMsgs[i].Session() )
            {     
            _DBG_FILE("Owner session of job found. Progress completed.");    
            request = ETrue;
                        
            iProgressMsgs[i].Write( 0, aBuf, 0 );
            iProgressMsgs[i].Complete( KErrNone );
            iProgressMsgs.Remove(i);        
            }
        }    
    _DBG_FILE("CNSmlSOSHandler::CompleteBufProgressMsg : End");    
    return request;     
    }

// --------------------------------------------------------------------------
// TBool CNSmlSOSHandler::CompleteBufProgressMsg( const TDesC8& aBuf, 
// const CNSmlSOSSession* aSession )
// Write data to the progress message and completes it.
// --------------------------------------------------------------------------
//
TBool CNSmlSOSHandler::CompleteBufContactSuiteProgressMsg( 
const TDesC8& aBuf, 
const CNSmlSOSSession* aSession  )
    {
    _DBG_FILE("CNSmlSOSHandler::CompleteBufProgressMsg : Begin");
    TBool request( EFalse );    
        
	    // Session info is stored to job and compared to session of the message.
    for( TInt i(0); i < iContactSuiteProgressMsgs.Count(); i++ )
        {
            request = ETrue;

		    if(!iContactSuiteProgressMsgs[i].IsNull())
                {
                iContactSuiteProgressMsgs[i].Write( 0, aBuf, 0 );
                iContactSuiteProgressMsgs[i].Complete( KErrNone );
                }
            iContactSuiteProgressMsgs.Remove(i);
        }    

    _DBG_FILE("CNSmlSOSHandler::CompleteBufProgressMsg : End");    
    return request;     
    }

// --------------------------------------------------------------------------
// void CNSmlSOSHandler::CancelContactSuiteProgressMsg( const CNSmlSOSSession* aSession )
// Completes cancelled progress message and removes it from array.
// --------------------------------------------------------------------------
//
void CNSmlSOSHandler::CancelContactSuiteProgressMsg( const CNSmlSOSSession* aSession )
    {
    for ( TInt i(0) ; i < iContactSuiteProgressMsgs.Count() ; i++ )
        {
        //if ( iContactSuiteProgressMsgs[i].Session() == aSession )
            {
            iContactSuiteProgressMsgs[i].Complete( KErrCancel );
            iContactSuiteProgressMsgs.Remove(i);
            }
        }
    }

// --------------------------------------------------------------------------
// void CNSmlSOSHandler::CancelProgressMsg( const CNSmlSOSSession* aSession )
// Completes cancelled progress message and removes it from array.
// --------------------------------------------------------------------------
//
void CNSmlSOSHandler::CancelProgressMsg( const CNSmlSOSSession* aSession )
    {
    for ( TInt i(0) ; i < iProgressMsgs.Count() ; i++ )
        {
        if ( iProgressMsgs[i].Session() == aSession )
            {
            iProgressMsgs[i].Complete( KErrCancel );
            iProgressMsgs.Remove(i);
            }
        }
    }

// --------------------------------------------------------------------------
// void CNSmlSOSHandler::AddEventMsgL( const RMessage2& aMessagePtr )
// Adds event message to the message array.
// --------------------------------------------------------------------------
//
void CNSmlSOSHandler::AddEventMsgL( const RMessage2& aMessagePtr )
    {
    iEventMsgs.AppendL( aMessagePtr );
    }

// --------------------------------------------------------------------------
// void CNSmlSOSHandler::CompleteEventMsgL( MSyncMLEventObserver::TEvent aEvent, TInt aId, TInt aError, TInt aAdditional )
// Writes data to all event messages and completes them.
// --------------------------------------------------------------------------
//
void CNSmlSOSHandler::CompleteEventMsgL( MSyncMLEventObserver::TEvent aEvent, TInt aId, TInt aError, TInt aAdditional )
    {
    if ( iEventMsgs.Count() )
        {
        HBufC8* buf = HBufC8::NewLC(( (2*KSizeofTInt8) + (3*KSizeofTInt32) ));
        TPtr8 bufPtr = buf->Des();

        RDesWriteStream stream;
        stream.Open(bufPtr);
        CleanupClosePushL(stream);

        stream.WriteInt8L( (TInt8) ENSmlTypeEvent );
        stream.WriteInt8L( (TInt8) aEvent );
        stream.WriteInt32L( aId );
        stream.WriteInt32L( aError );
        stream.WriteInt32L( aAdditional );
        stream.CommitL();

        while ( iEventMsgs.Count() > 0 )
            {
            if(!iEventMsgs[0].IsNull()&& iEventMsgs[0].Handle())
                {
                iEventMsgs[0].Write( 0, bufPtr, 0 );
                iEventMsgs[0].Complete( KErrNone );
                }
            iEventMsgs.Remove(0);
            }
        CleanupStack::PopAndDestroy(&stream); 
        CleanupStack::PopAndDestroy(buf);
        }
    }

// --------------------------------------------------------------------------
// void CNSmlSOSHandler::CancelEventMsg( TInt aHandle )
// Completes cancelled event message and removes it from array.
// --------------------------------------------------------------------------
//
void CNSmlSOSHandler::CancelEventMsg( TInt aHandle )
    {
    for ( TInt i(0) ; i < iEventMsgs.Count() ; i++ )
        {
        if ( iEventMsgs[i].Handle() == aHandle )
            {
            iEventMsgs[i].Complete( KErrCancel );
            iEventMsgs.Remove(i);
            }
        }
    }

// --------------------------------------------------------------------------
// void CNSmlSOSHandler::ServerAlertL( TDesC8& aData, TSmlUsageType aType, TSmlProtocolVersion aVersion )
// Add server alert message to message queue.
// --------------------------------------------------------------------------
//
void CNSmlSOSHandler::ServerAlertL( TDesC8& aData, TSmlUsageType aType, TSmlProtocolVersion aVersion, TInt aBearerType )
    {
    iMsgQueue->AddMessageL( aData, aType, aVersion, aBearerType );
    }


// FOTA

// --------------------------------------------------------------------------
// void CNSmlSOSHandler::GenericAlertL( HBufC8* aMgmtUri, HBufC8* aMetaType, HBufC8* aMetaFormat, TInt aFinalResult, HBufC8* aCorrelator )
// Adds Generic Alert to DM Agent if it is currently running.
// --------------------------------------------------------------------------
//
void CNSmlSOSHandler::GenericAlertL( HBufC8* aMgmtUri, HBufC8* aMetaType, HBufC8* aMetaFormat, TInt aFinalResult, HBufC8* aCorrelator )
	{
	if ( !iParams || !iParams->iThreadEngine )
		{
		User::Leave( KErrNotFound );
		}
	
	iParams->iThreadEngine->DMAgentL()->SetGenericAlertL( *aMgmtUri, *aMetaType, *aMetaFormat, aFinalResult, *aCorrelator );
	}

// FOTA end

// --------------------------------------------------------------------------
// void CNSmlSOSHandler::GenericAlertL( HBufC8* aMgmtUri, HBufC8* aMetaType, HBufC8* aMetaFormat, HBufC8* aCorrelator, HBufC8* aTargetUri, HBufC8* aMark, HBufC8* aData )
// Adds Generic Alert to DM Agent if it is currently running.
// --------------------------------------------------------------------------
//
void CNSmlSOSHandler::GenericAlertL( HBufC8* aCorrelator, RArray<CNSmlDMAlertItem>* aItem )
	{
	if ( !iParams || !iParams->iThreadEngine )
		{
		User::Leave( KErrNotFound );
		}
	
	iParams->iThreadEngine->DMAgentL()->SetGenericAlertL( *aCorrelator, *aItem );
	}

// --------------------------------------------------------------------------
// void CNSmlSOSHandler::IncreaseSessionCount()
// --------------------------------------------------------------------------
//
void CNSmlSOSHandler::IncreaseSessionCount()
    {
    iSessionCount++;
    }

// --------------------------------------------------------------------------
// void CNSmlSOSHandler::DecreaseSessionCount()
// --------------------------------------------------------------------------
//
void CNSmlSOSHandler::DecreaseSessionCount()
    {
    iSessionCount--;
    if ( !iSessionCount && !iCurrentJob && iMsgQueue->IsEmpty() ) 
        {
        CActiveScheduler::Stop();
        }
    }

// --------------------------------------------------------------------------
// void CNSmlSOSHandler::ServerSuspendedL( const TBool aSuspended )
// Sets suspended state. 
// --------------------------------------------------------------------------
//
void CNSmlSOSHandler::ServerSuspendedL( const TBool aSuspended )
    {
    
    if ( iSuspended != aSuspended )
        {
        CompleteEventMsgL( MSyncMLEventObserver::EServerSuspended, 0, 0, aSuspended );         
        }
        
    iSuspended = aSuspended; 

    iMsgQueue->ServerSuspended( iSuspended ); // Send suspend state to message queue.

    if ( iSuspended )
        {
        // Cancel job if running.
        if ( iCurrentJob )
            {
            if ( iCurrentJob->UsageType() == ESmlDataSync )
                {
                CancelDSSync();    
                }
            else // ESmlDevMan
                {
                CancelDMSync();
                }
            }
        }
    else
        {
        // Start job if queued.
        if ( iCurrentJob && iCurrentJob->JobRunning() == EFalse )
            {
            if ( iCurrentJob->UsageType() == ESmlDataSync )
                {
                StartDSSyncL();    
                }
            else // ESmlDevMan
                {
                StartDMSyncL();
                }
            }
        // If there are no sessions or jobs and message queue is empty
        // then server can be stopped.
        if ( !iSessionCount && !iCurrentJob && iMsgQueue->IsEmpty() )
            {
            CActiveScheduler::Stop();
            }
        }
    }

// --------------------------------------------------------------------------
// TBool CNSmlSOSHandler::IsSuspended()
// --------------------------------------------------------------------------
//
TBool CNSmlSOSHandler::IsSuspended()
    {
    return iSuspended;
    }

// --------------------------------------------------------------------------
// void CNSmlSOSHandler::StartDSSyncL()
// Starts DS sync job.
// --------------------------------------------------------------------------
//
void CNSmlSOSHandler::StartDSSyncL()
    {
    _DBG_FILE("CNSmlSOSHandler::StartDSSyncL(): begin");
	static _LIT_SECURITY_POLICY_PASS(KAllowAllPolicy);
   	static _LIT_SECURITY_POLICY_C1(KAllowWriteDeviceDataPolicy, ECapabilityWriteDeviceData);
    TInt sessiontype=1;
    TInt r=RProperty::Define(KPSUidNSmlSOSServerKey,KNSmlSyncJobOngoing,RProperty::EInt,KAllowAllPolicy,KAllowWriteDeviceDataPolicy);
	if ( r != KErrNone && r != KErrAlreadyExists )
    	{
    	User::LeaveIfError(r);
    	}
    //sessiontype=1 for DS session 	              
    TInt r2=RProperty::Set(KPSUidNSmlSOSServerKey,KNSmlSyncJobOngoing,sessiontype);
    DBG_FILE_CODE( sessiontype, _S8("P&S key set to 1 for ds session") );    
    //Set job as running from now on
    iCurrentJob->SetJobRunning( ETrue );
    
    // client or server iniated sync
	TNSmlSyncInitiation syncInit = EClientInitiated;
	
    // Get non-overrided data from settings.
    CNSmlDSSettings* settings = CNSmlDSSettings::NewLC();
    CNSmlDSProfile* prof = settings->ProfileL( iCurrentJob->ProfileId() );
    if ( !prof )    // profile not found, delete job.
        {
        CleanupStack::PopAndDestroy(settings);
        FinishCurrentJobL();      
	    _DBG_FILE("CNSmlSOSHandler::StartDSSyncL(): profile not found!");
        return;
        }

    CleanupStack::PushL(prof);    
	
	if ( iCurrentJob->iPending || prof->IntValue( EDSProfileHidden ) )
		{
		syncInit = EServerAlerted;	
		}
		
    iParams = new (ELeave) CNSmlThreadParams( *iCurrentJob, this );
	iParams->iCSArray = new(ELeave) CArrayFixFlat<TNSmlContentSpecificSyncType>(1);
	iParams->iSyncInit = syncInit;
		
	TNSmlContentSpecificSyncType css;
    // Tasks and sync type are overrided in a same struct -> must be handled together.
   
    RArray<TInt> tasks;
    iCurrentJob->TaskIds( tasks );
    CleanupClosePushL(tasks);
	if (iCurrentJob->iCtInfo)
		{
		for (TInt i = 0; i < iCurrentJob->iCtInfo->Count(); i++)
        	{
            css.iType = ConvertSyncTypes( iCurrentJob->iCtInfo->At(i).iSyncType );   
        	css.iTaskId = iCurrentJob->iCtInfo->At(i).iTaskId;
        	iParams->iCSArray->AppendL(css);
        	}
        	
		}
	else
    if ( tasks.Count() || ( iCurrentJob->SyncType() >= ESmlTwoWay &&
                           iCurrentJob->SyncType() <= ESmlRefreshFromClient ) )
        {
        
        TSmlSyncType sType;
    
        // If tasks are not overrided, get tasks from db.
        if ( !tasks.Count() )
            {
            ReadTasksFromDBL( tasks );
            }

        for ( TInt i(0) ; i < tasks.Count() ; i++ )
            {            
            if ( iCurrentJob->SyncType() >= ESmlTwoWay &&
                 iCurrentJob->SyncType() <= ESmlRefreshFromClient )
                {
                sType = iCurrentJob->SyncType();
                }
            else    // If sync type is not overridden, get sync type from db.
                {
                sType = ReadSyncTypeForTaskL( tasks[i] );
                }
                
            // Convert sync types to match CNSmlDbCaps sync types.
            css.iType = ConvertSyncTypes( sType );
            css.iTaskId = tasks[i];
            iParams->iCSArray->AppendL(css);
            }
        }
       
	iStatus = KRequestPending;
	        		
	iThreadObserver = new (ELeave) CNSmlThreadObserver( iStatus );
	
	//Start observing DM session thread.
	TInt error = TNSmlThreadLauncher::RunJobSession( *iParams, *iThreadObserver );
	if (  error != KErrNone )
		{
		HandleEventMsg( MSyncMLEventObserver::EJobStartFailed, error );	
		return;
		}
	
	HandleEventMsg( MSyncMLEventObserver::EJobStart );    

    CleanupStack::PopAndDestroy(&tasks);
    
    CleanupStack::PopAndDestroy(prof);
    CleanupStack::PopAndDestroy(settings);

    if ( !IsActive() )
        {
        SetActive(); // Set object active to start listening RequestComplete() in RunL.
        }
    
    _DBG_FILE("CNSmlSOSHandler::StartDSSyncL(): end");
    }

// --------------------------------------------------------------------------
// CNSmlDbCaps::ENSmlSyncTypes CNSmlSOSHandler::ConvertSyncTypes(TSmlSyncType aSyncType)
// --------------------------------------------------------------------------
//
CNSmlDbCaps::ENSmlSyncTypes CNSmlSOSHandler::ConvertSyncTypes(TSmlSyncType aSyncType)
	{
	CNSmlDbCaps::ENSmlSyncTypes syncType;
	
	switch ( aSyncType )
        {
        case ESmlTwoWay:
            syncType = CNSmlDbCaps::ETwoWaySync;
            break;
        case ESmlOneWayFromServer:
            syncType = CNSmlDbCaps::EOneWaySyncFromServer;
            break;
        case ESmlOneWayFromClient:
            syncType = CNSmlDbCaps::EOneWaySyncFromClient;
            break;
        case ESmlSlowSync:
            syncType = CNSmlDbCaps::ESlowTwoWaySync;
            break;
        case ESmlRefreshFromServer:
            syncType = CNSmlDbCaps::ERefreshSyncFromServer;
            break;
        case ESmlRefreshFromClient: 
            syncType = CNSmlDbCaps::ERefreshSyncFromClient;
            break;
        default:
            syncType = CNSmlDbCaps::ETwoWaySync;
            break;
 		}
 	return syncType;
	}
// --------------------------------------------------------------------------
// void CNSmlSOSHandler::CancelDSSync()
// --------------------------------------------------------------------------
//
void CNSmlSOSHandler::CancelDSSync()
    {
    _DBG_FILE("CNSmlSOSHandler::CancelDSSync(): begin");
   
    if (( iParams) && (iParams->iThreadEngine ))
    	{
    	iParams->iThreadEngine->CancelJob();	
    	}
       
    _DBG_FILE("CNSmlSOSHandler::CancelDSSync(): end");
    }

// --------------------------------------------------------------------------
// void CNSmlSOSHandler::StartDMSyncL()
// Starts DM configuration job.
// --------------------------------------------------------------------------
//
void CNSmlSOSHandler::StartDMSyncL()
    {
    _DBG_FILE("CNSmlSOSHandler::StartDMSyncL(): begin");
	static _LIT_SECURITY_POLICY_PASS(KAllowAllPolicy);
   	static _LIT_SECURITY_POLICY_C1(KAllowWriteDeviceDataPolicy, ECapabilityWriteDeviceData);
    TInt sessiontype = 2;
    TInt r=RProperty::Define(KPSUidNSmlSOSServerKey,KNSmlSyncJobOngoing,RProperty::EInt,KAllowAllPolicy,KAllowWriteDeviceDataPolicy);
	if ( r != KErrNone && r != KErrAlreadyExists )
    	{
    	User::LeaveIfError(r);
        }
    //sessiontype=2 for dm session                  
    TInt r2=RProperty::Set(KPSUidNSmlSOSServerKey,KNSmlSyncJobOngoing,sessiontype);
    DBG_FILE_CODE( sessiontype, _S8("P&S key set to 2 for dm session") );    
    //Set job as running from now on
    iCurrentJob->SetJobRunning( ETrue );

	TNSmlSyncInitiation syncInit = EClientInitiated;
    CNSmlDMSettings* settings = CNSmlDMSettings::NewLC();
    CNSmlDMProfile* prof = settings->ProfileL( iCurrentJob->ProfileId() );
    if(prof)
    	{
      	CleanupStack::PushL( prof );    
    		if ( iCurrentJob->iPending || prof->IntValue( EDMProfileHidden ))
        {
        	syncInit = EServerAlerted;
        }

    		if ( !iCurrentJob->TransportId() )
        {        
           iCurrentJob->SetTransportId( prof->IntValue( EDMProfileTransportId ) );
        }
        CleanupStack::PopAndDestroy( prof );
      }
    CleanupStack::PopAndDestroy( settings );
 

    switch ( iCurrentJob->JobType() )
        {
        case EDMJobProfile:
        case EDMJobTransport:
        	    {        		                		
        		iStatus = KRequestPending;
        		iParams = new (ELeave) CNSmlThreadParams( *iCurrentJob, this );
        		iParams->iSyncInit = syncInit; 
        		        		
        		iThreadObserver = new (ELeave) CNSmlThreadObserver( iStatus );
        		
        		//Start observing DM session thread.
        		TInt error = TNSmlThreadLauncher::RunJobSession( *iParams, *iThreadObserver );
   		    	if (  error != KErrNone )
   		    		{
   		    		HandleEventMsg( MSyncMLEventObserver::EJobStartFailed, error );   		    		
   		    		return;
   		    		}
        	    }
            break;
        default:
            HandleEventMsg( MSyncMLEventObserver::EJobStartFailed );
            break;
        }
    HandleEventMsg( MSyncMLEventObserver::EJobStart, iCurrentJob->JobId() );

 	if ( !IsActive() )
        {
        SetActive(); // Set object active to start listening RequestComplete() in RunL.
        }   

    _DBG_FILE("CNSmlSOSHandler::StartDMSyncL(): end");
    }

// --------------------------------------------------------------------------
// void CNSmlSOSHandler::CancelDMSync()
// --------------------------------------------------------------------------
//
void CNSmlSOSHandler::CancelDMSync()
    {
    _DBG_FILE("CNSmlSOSHandler::CancelDMSync(): begin");
    
    if (( iParams) && (iParams->iThreadEngine ))
    	{
    	iParams->iThreadEngine->CancelJob();	
    	}
        
    _DBG_FILE("CNSmlSOSHandler::CancelDMSync(): end");
    }

// --------------------------------------------------------------------------
// TBool CNSmlSOSHandler::SearchSimilarJobs( const CNSmlJob* aJob )
// Searches queued job if created job already exists.
// --------------------------------------------------------------------------
//
TBool CNSmlSOSHandler::SearchSimilarJobs( const CNSmlJob* aJob )
    {
    // Goes through all the jobs and returns ETrue if similar exists.
    CNSmlJob* tempJob = iCurrentJob;
        
    while ( tempJob->iNext )
       {
       RArray<TInt> tempTasks;
       tempJob->TaskIds( tempTasks );
       
       RArray<TInt> tasks;
       aJob->TaskIds( tasks );
       
       TInt match(0);
       if ( tempJob->ProfileId()       == aJob->ProfileId()       &&
            tempTasks.Count()          == tasks.Count()           &&
            tempJob->SyncType()        == aJob->SyncType()        &&
            tempJob->TransportId()     == aJob->TransportId() )
            {
            // Also check if all the task ids match if there are any.
            if( !tasks.Count() )
                {
                tempTasks.Close();
                tasks.Close();
                return ETrue;
                }
            for( TInt i(0); i < tasks.Count(); i++ )
                {    
                if( tasks.operator[](i) == tempTasks.operator[](i) )
                    {
                    match++;   
                    }
                if ( match == tasks.Count() )
                    {
                    tempTasks.Close();
                    tasks.Close();
                    return ETrue;
                    }
                }
            }
            
	   tempTasks.Close();
       tasks.Close();
       
       tempJob = tempJob->iNext;
       }
    return EFalse;
    }

// --------------------------------------------------------------------------
// void CNSmlSOSHandler::ReadTasksFromDBL( RArray<TInt>& aArray )
// --------------------------------------------------------------------------
//
void CNSmlSOSHandler::ReadTasksFromDBL( RArray<TInt>& aArray )
    {
    // Get non-overrided data from settings.
    CNSmlDSSettings* settings = CNSmlDSSettings::NewLC();
    CNSmlDSProfile* prof = settings->ProfileL( iCurrentJob->ProfileId() );

    if ( prof )
        {        
        CleanupStack::PushL(prof);
        RImplInfoPtrArray implArray;
        CleanupStack::PushL(PtrArrCleanupItemRArr( CImplementationInformation, &implArray ) );    
        TUid ifUid = { KNSmlDSInterfaceUid };
        REComSession::ListImplementationsL( ifUid, implArray );

        for ( TInt i = 0 ; i < implArray.Count() ; i++ )
            {    
            CImplementationInformation* implInfo = implArray[i];
            CNSmlDSContentType* ctype = prof->ContentType( implInfo->ImplementationUid().iUid );
            if ( ctype )
                {            
                aArray.AppendL( ctype->IntValue( EDSAdapterTableId ) );
                }
            }        
        CleanupStack::PopAndDestroy(&implArray); 
        CleanupStack::PopAndDestroy(prof);
        REComSession::FinalClose();        
        }    
    CleanupStack::PopAndDestroy(settings);
    }

// --------------------------------------------------------------------------
// TSmlSyncType CNSmlSOSHandler::ReadSyncTypeForTaskL( const TInt aTaskId )
// --------------------------------------------------------------------------
//
TSmlSyncType CNSmlSOSHandler::ReadSyncTypeForTaskL( const TInt aTaskId )
    {
    CNSmlDSSettings* settings = CNSmlDSSettings::NewLC();
    TSmlSyncType syncType( ESmlTwoWay );
    CNSmlDSProfile* prof = settings->ProfileL( iCurrentJob->ProfileId() );
    if ( prof )
        {
        CNSmlDSContentType* ctype = prof->ContentTypeId( aTaskId ); 
        // content type is deleted in CNSmlDSProfile destructor.
        syncType = (TSmlSyncType) ctype->IntValue( EDSAdapterSyncType );
            
        delete prof;
        prof = NULL;
        }
    CleanupStack::PopAndDestroy(settings);
    return syncType;
    }

// --------------------------------------------------------------------------
// void CNSmlSOSHandler::FinishCurrentJobL()
// Removes finished job from queue and starts next from queue if one exists.
// --------------------------------------------------------------------------
//
void CNSmlSOSHandler::FinishCurrentJobL()
    {
    _DBG_FILE("CNSmlSOSHandler::FinishCurrentJobL(): begin");
        
	if ( iParams )
    	{
       	delete iParams;
       	iParams = NULL;	
       	}
       	
     if (iThreadObserver)
    	{
    	delete iThreadObserver;
    	iThreadObserver = NULL;        	
    	}
     
    if ( iCurrentJob->iNext ) // If there is a job waiting, remove current and start next.  
        {
        CNSmlJob* nextJob = iCurrentJob->iNext;
        delete iCurrentJob;
        iCurrentJob = nextJob;

        if ( iCurrentJob->UsageType() == ESmlDataSync )
            {
            StartDSSyncL();
            }
        else
            {
            StartDMSyncL();
            }
        }
    else // No jobs at queue.
        {
        delete iCurrentJob;
        iCurrentJob = NULL;
        // if no clients connected and no jobs queued -> stop the server.
        if ( !iSessionCount && iMsgQueue->IsEmpty() )
            {
            _DBG_FILE("CNSmlSOSHandler::FinishCurrentJobL(): CActiveScheduler::Stop()");
            CActiveScheduler::Stop();   
            }
        }    
     _DBG_FILE("CNSmlSOSHandler::FinishCurrentJobL(): end");
    }

// --------------------------------------------------------------------------
// void CNSmlSOSHandler::DoCancel()
// --------------------------------------------------------------------------
//
void CNSmlSOSHandler::DoCancel()
    {
    }

// --------------------------------------------------------------------------
// void CNSmlSOSHandler::RunL()
// --------------------------------------------------------------------------
//
void CNSmlSOSHandler::RunL()
    {
    _DBG_FILE("CNSmlSOSHandler::RunL() : Begin");
    
    HandleEventMsg( MSyncMLEventObserver::EJobStop, iStatus.Int() );        
    // Remove current job and start next if one exists.  
    TRAP_IGNORE( FinishCurrentJobL() );       
    
    _DBG_FILE("CNSmlSOSHandler::RunL() : End");
    }

// --------------------------------------------------------------------------
// void CNSmlSOSHandler::CreateJobL( CNSmlAlertJobInfo& aJobInfo )
// Creates server alerted job got from message queue.
// --------------------------------------------------------------------------
//
void CNSmlSOSHandler::CreateJobL( CNSmlAlertJobInfo& aJobInfo )
    {
    CNSmlJob* job = new (ELeave) CNSmlJob;
    CleanupStack::PushL(job);
    job->SetProfId( aJobInfo.iProfileId );
    job->SetTransportId( aJobInfo.iTransportId );
    job->SetUsageType( aJobInfo.iType );
    job->iUimode = aJobInfo.iUimode;   
    if ( aJobInfo.iType == ESmlDataSync )
        {        
        if ( aJobInfo.iPackage.Length() )
            {
            job->iPackage = aJobInfo.iPackage.AllocL();
            job->iSessionId = aJobInfo.iSessionId;
            }
            
        job->iCtInfo = new (ELeave) CArrayFixFlat<TNSmlContentTypeInfo>(1);

        for ( TInt i=0 ; i < aJobInfo.iContentType->Count() ; i++ )
            {
            job->AddTaskIdL( aJobInfo.iContentType->At(i).iTaskId);
            job->iCtInfo->AppendL( aJobInfo.iContentType->At(i) );
            }

        job->SetJobType( EDSJobTaskTransST );
        
        CNSmlDSSettings* settings = CNSmlDSSettings::NewLC();
        CNSmlDSProfile* prof = settings->ProfileL( aJobInfo.iProfileId );
        if ( prof )
            {
            if ( !prof->IntValue( EDSProfileHidden ) )
                {
                job->iPending = ETrue; // Notifier is not called if profile is hidden.
                }
            delete prof;
            prof = NULL;
            }
        CleanupStack::PopAndDestroy(settings);
        }
    else  // ESmlDevMan
        {
        job->SetJobType( EDMJobTransport );
        CNSmlDMSettings* settings = CNSmlDMSettings::NewLC();
        CNSmlDMProfile* prof = settings->ProfileL( aJobInfo.iProfileId );
        if ( prof )
            {
            if ( !prof->IntValue( EDMProfileHidden ) )
                {
                job->iPending = ETrue; // Notifier is not called if profile is hidden.
                }
            delete prof;
            prof = NULL;
            }
        CleanupStack::PopAndDestroy(settings);
        }        

    TInt jobId(0);
    // Add created job to the queue.
    AddJobL( job, jobId );
    CleanupStack::Pop(job);
    }

// --------------------------------------------------------------------------
// CNSmlSOSHandler::DoDisconnectL()
// Disconnects local connection.
// --------------------------------------------------------------------------
//
void CNSmlSOSHandler::DoDisconnectL()
	{
	CNSmlTransport* transport = CNSmlTransport::NewLC();
		
	TRequestStatus status;
	TDesC8 temp = KNullDesC8();
	
	HBufC8* mimeType = KNSmlDSAgentMIMEType().AllocLC();	
	transport->ConnectL( KUidNSmlMediumTypeUSB, ETrue, NULL, temp, *mimeType , status, temp, temp, 0 ); 
	User::WaitForRequest(status);
		
	if (status.Int() == KErrNone)
		{
		transport->Disconnect();
		}
	CleanupStack::PopAndDestroy( mimeType );
	CleanupStack::PopAndDestroy( transport );
	}
	
// --------------------------------------------------------------------------
// Progress observer
// void CNSmlSOSHandler::OnSyncMLSyncError( TErrorLevel aErrorLevel, TInt aError, TInt aTaskId, TInt aInfo1, TInt aInfo2 )
// --------------------------------------------------------------------------
//
void CNSmlSOSHandler::OnSyncMLSyncError( TErrorLevel aErrorLevel, TInt aError, TInt aTaskId, TInt aInfo1, TInt aInfo2 )
    {
    // do not try to buffer, if there is not active session
    if ( iCurrentJob->OwnerSession() != NULL )
        {
        TNSmlProgressEvent event;
        event.iEventType = ENSmlSyncError;        
        event.iErrorLevel = aErrorLevel;   
        event.iInfo1 = aError;
        event.iInfo2 = aTaskId;   
        event.iInfo3 = aInfo1;
        event.iInfo4 = aInfo2;

        iCurrentJob->OwnerSessionNonConst()->StoreToProgessEventBuffer( event );            
        }      
    }

// --------------------------------------------------------------------------
// HBufC8* CNSmlSOSHandler::WriteSyncErrorL( TErrorLevel aErrorLevel, TInt aError, TInt aTaskId, TInt aInfo1, TInt aInfo2 )
// --------------------------------------------------------------------------
//
HBufC8* CNSmlSOSHandler::WriteSyncErrorL( TErrorLevel aErrorLevel, TInt aError, TInt aTaskId, TInt aInfo1, TInt aInfo2 )
	{
    HBufC8* buf = HBufC8::NewLC( ( 3*KSizeofTInt8 + ( 4*KSizeofTInt32 ) ) );
    TPtr8 bufPtr = buf->Des();

    RDesWriteStream stream;
    stream.Open( bufPtr );
    CleanupClosePushL( stream );

    stream.WriteInt8L( (TInt8) ENSmlTypeProgressEvent );
    stream.WriteInt8L( (TInt8) ENSmlSyncError );
    stream.WriteInt8L( (TInt8) aErrorLevel );

    stream.WriteInt32L( aError );
    stream.WriteInt32L( aTaskId );
    stream.WriteInt32L( aInfo1 );
    stream.WriteInt32L( aInfo2 );
    stream.CommitL();
    
    CleanupStack::PopAndDestroy( &stream ); 
    CleanupStack::Pop( buf );
   	
   	return buf;
	}

// --------------------------------------------------------------------------
// void CNSmlSOSHandler::OnSyncMLSyncProgress( MSyncMLProgressObserver::TStatus aStatus, TInt aInfo1, TInt aInfo2 )
// --------------------------------------------------------------------------
//
void CNSmlSOSHandler::OnSyncMLSyncProgress( MSyncMLProgressObserver::TStatus aStatus, TInt aInfo1, TInt aInfo2 )
    {
    // do not try to buffer, if there is not active session
    if ( iCurrentJob->OwnerSession() != NULL )
        {
        TNSmlProgressEvent event;
        event.iEventType = ENSmlSyncProgress;
        event.iProgressStatus = aStatus;    
        event.iInfo1 = aInfo1;
        event.iInfo2 = aInfo2;   
        event.iInfo3 = 0;
        event.iInfo4 = 0;
    
        iCurrentJob->OwnerSessionNonConst()->StoreToProgessEventBuffer( event );
        }                	
    }

// --------------------------------------------------------------------------
// HBufC8* CNSmlSOSHandler::WriteSyncProgressL( TStatus aStatus, TInt aInfo1, TInt aInfo2 )
// --------------------------------------------------------------------------
//
HBufC8* CNSmlSOSHandler::WriteSyncProgressL( TStatus aStatus, TInt aInfo1, TInt aInfo2 )
	{
	HBufC8* buf = HBufC8::NewLC( (3*KSizeofTInt8 + (2*KSizeofTInt32) ));
    TPtr8 bufPtr = buf->Des();

    RDesWriteStream stream;
    stream.Open( bufPtr );
    CleanupClosePushL( stream );

    stream.WriteInt8L( (TInt8) ENSmlTypeProgressEvent );
    stream.WriteInt8L( (TInt8) ENSmlSyncProgress );
    stream.WriteInt8L( (TInt8) aStatus );

    stream.WriteInt32L( aInfo1 );
    stream.WriteInt32L( aInfo2 );
    stream.CommitL(); 
    
    CleanupStack::PopAndDestroy( &stream );
    CleanupStack::Pop( buf );
    
    return buf;
	}

// --------------------------------------------------------------------------
// void CNSmlSOSHandler::OnSyncMLDataSyncModifications( TInt aTaskId, const TSyncMLDataSyncModifications& aClientMods, const TSyncMLDataSyncModifications& aServerMods )
// --------------------------------------------------------------------------
//
void CNSmlSOSHandler::OnSyncMLDataSyncModifications( TInt aTaskId, const TSyncMLDataSyncModifications& aClientMods,
                                                                   const TSyncMLDataSyncModifications& aServerMods )
    {
    
    // do not try to buffer, if there is not active session
    if ( iCurrentJob->OwnerSession() != NULL )
        {
        TNSmlProgressEvent event;
        event.iEventType = ENSmlModifications;
        event.iClientMods = aClientMods;
        event.iServerMods = aServerMods;
        event.iInfo1 = aTaskId;
        event.iInfo2 = 0;   
        event.iInfo3 = 0;
        event.iInfo4 = 0;
    
        iCurrentJob->OwnerSessionNonConst()->StoreToProgessEventBuffer( event );            
        }
    }

// --------------------------------------------------------------------------
// HBufC8* CNSmlSOSHandler::WriteSyncModificationsL( TInt aTaskId, const TSyncMLDataSyncModifications& aClientMods, const TSyncMLDataSyncModifications& aServerMods )
// --------------------------------------------------------------------------
//
HBufC8* CNSmlSOSHandler::WriteSyncModificationsL( TInt aTaskId, const TSyncMLDataSyncModifications& aClientMods,
                                                                const TSyncMLDataSyncModifications& aServerMods )
	{
	HBufC8* buf = HBufC8::NewLC( (2*KSizeofTInt8 + (11*KSizeofTInt32) ));
    TPtr8 bufPtr = buf->Des();

    RDesWriteStream stream;
    stream.Open(bufPtr);
    CleanupClosePushL(stream);

    stream.WriteInt8L( (TInt8) ENSmlTypeProgressEvent );
    stream.WriteInt8L( (TInt8) ENSmlModifications );

    stream.WriteInt32L( aTaskId );

    stream.WriteInt32L( aClientMods.iNumAdded );
    stream.WriteInt32L( aClientMods.iNumReplaced );
    stream.WriteInt32L( aClientMods.iNumMoved );
    stream.WriteInt32L( aClientMods.iNumDeleted );
    stream.WriteInt32L( aClientMods.iNumFailed );

    stream.WriteInt32L( aServerMods.iNumAdded );
    stream.WriteInt32L( aServerMods.iNumReplaced );
    stream.WriteInt32L( aServerMods.iNumMoved );
    stream.WriteInt32L( aServerMods.iNumDeleted );
    stream.WriteInt32L( aServerMods.iNumFailed );
    stream.CommitL();
    
    CleanupStack::PopAndDestroy( &stream ); 
    CleanupStack::Pop( buf );
    
    return buf;
	}

// --------------------------------------------------------------------------
// CNSmlSOSHandler::HandleEventMsg( MSyncMLEventObserver::TEvent aEvent, 
// TInt aStatus )
// --------------------------------------------------------------------------
//
void CNSmlSOSHandler::HandleEventMsg( 
    MSyncMLEventObserver::TEvent aEvent,
    TInt aStatus )
    {
    _DBG_FILE("CNSmlSOSHandler::HandleEventMsg : Begin");
    TBool found( EFalse );
    
    for (TInt i(0); i < iEventMsgs.Count(); i++ )    
        {
        if ( iEventMsgs[i].Session() == iCurrentJob->OwnerSession() )
            {
            found = ETrue;
            }             
        } 
    
    if ( !found && !iCurrentJob->iSilentJob)
        {                              
        if ( iCurrentJob->OwnerSession() )
            {
            _DBG_FILE("HandleEventMsg : Event buffered");
            iCurrentJob->OwnerSessionNonConst()->StoreToEventBuffer( aEvent, iCurrentJob->JobId(), aStatus );            
            }        
        }
    else
        {        
        _DBG_FILE("HandleEventMsg : Event completed");                      
        TRAP_IGNORE( CompleteEventMsgL( aEvent, iCurrentJob->JobId(), aStatus ));        
        }
    _DBG_FILE("CNSmlSOSHandler::HandleEventMsg : End");                      
    }

// --------------------------------------------------------------------------
// CNSmlSOSHandler::ClearSessionFromJob( const CNSmlSOSSession* aSession )
// Resets session from job.
// --------------------------------------------------------------------------
//
void CNSmlSOSHandler::ClearSessionFromJob( const CNSmlSOSSession* aSession )
    {
        
    CNSmlJob* tempJob = iCurrentJob;
        
    while ( tempJob )
        {            
        if (tempJob->OwnerSession() == aSession )        
            {
            tempJob->SetOwnerSession( NULL );    
            }
        tempJob = tempJob->iNext;
        }    
    }

void CNSmlSOSHandler::RegisterContactSuiteObserversWithProfileId( TInt aProfileId )
    {
    iContactSuiteObserverProfileId = aProfileId;
    }

// --------------------------------------------------------------------------
// CNSmlSOSHandler::ClosePendingMessages( const CNSmlSOSSession* aSession )
// Resets session from job.
// --------------------------------------------------------------------------
//
void CNSmlSOSHandler::ClosePendingMessages( const CNSmlSOSSession* aSession )
    {
    TInt count = iProgressMsgs.Count();
    TInt i (0);
    while ( i < count )
        {
        if ( aSession == iProgressMsgs[i].Session() )
            {     
            _DBG_FILE("Close pending progress message");                                        
            iProgressMsgs.Remove(i); 
            count = iProgressMsgs.Count();       
            }   
        else
            {
            i++;
            }             
        }
    
	
	count = iContactSuiteProgressMsgs.Count(); 
    i = 0;
    while ( i < count )
        {
                
            _DBG_FILE("Close pending event message");                                        
            iContactSuiteProgressMsgs.Remove(i); 
            count = iContactSuiteProgressMsgs.Count();                                
            i++;       
        }


    count = iEventMsgs.Count(); 
    i = 0;
    while ( i < count )
        {
        if ( aSession == iEventMsgs[i].Session() )
            {     
            _DBG_FILE("Close pending event message");                                        
            iEventMsgs.Remove(i); 
            count = iEventMsgs.Count();                       
            } 
         else
            {
            i++;
            }    
        }
    }

// End of file
