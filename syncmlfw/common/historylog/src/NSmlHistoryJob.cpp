/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Job history entry
*
*/

#include <SyncMLHistory.h>
#include <SyncMLAlertInfo.h>
#include <SyncMLErr.h> // sync error codes
#include <nsmlconstants.h>
#include "NSmlErrorCodeConversion.h"

// ---------------------------------------------------------
// CSyncMLHistoryJob::TTaskInfo::TTaskInfo()
// Constructor
// ---------------------------------------------------------
EXPORT_C CSyncMLHistoryJob::TTaskInfo::TTaskInfo() :
	iTaskId( KErrNotFound ),
	iError( KErrNone ),
	iItemsAdded( 0 ),
	iItemsChanged( 0 ),
	iItemsDeleted( 0 ),
	iItemsMoved( 0 ),
	iItemsFailed( 0 ),
	iServerItemsAdded( 0 ),
	iServerItemsChanged( 0 ),
	iServerItemsDeleted( 0 ),
	iServerItemsMoved( 0 ),
	iServerItemsFailed( 0 ),
	iSyncType( ESmlTwoWay )
	{
	}

// ---------------------------------------------------------
// CSyncMLHistoryJob::TTaskInfo::TTaskInfo::InternalizeL(RReadStream& aStream)
// Reads task info from given stream
// ---------------------------------------------------------
EXPORT_C void CSyncMLHistoryJob::TTaskInfo::TTaskInfo::InternalizeL(RReadStream& aStream)
	{
	iTaskId = (TSmlTaskId) aStream.ReadInt32L();
	iError = aStream.ReadInt32L();
	iItemsAdded = aStream.ReadInt32L();
	iItemsChanged = aStream.ReadInt32L();
	iItemsDeleted = aStream.ReadInt32L();
	iItemsMoved = aStream.ReadInt32L();
	iItemsFailed = aStream.ReadInt32L();
	iServerItemsAdded = aStream.ReadInt32L();
	iServerItemsChanged = aStream.ReadInt32L();
	iServerItemsDeleted = aStream.ReadInt32L();
	iServerItemsMoved = aStream.ReadInt32L();
	iServerItemsFailed = aStream.ReadInt32L();
	iSyncType = (TSmlSyncType) aStream.ReadInt32L();
	}

// ---------------------------------------------------------
// CSyncMLHistoryJob::TTaskInfo::TTaskInfo::ExternalizeL(RWriteStream& aStream) const
// Writes task info to given stream
// ---------------------------------------------------------
EXPORT_C void CSyncMLHistoryJob::TTaskInfo::TTaskInfo::ExternalizeL(RWriteStream& aStream) const
	{
	aStream.WriteInt32L((TInt) iTaskId);
	aStream.WriteInt32L(iError);
	aStream.WriteInt32L(iItemsAdded);
	aStream.WriteInt32L(iItemsChanged);
	aStream.WriteInt32L(iItemsDeleted);
	aStream.WriteInt32L(iItemsMoved);
	aStream.WriteInt32L(iItemsFailed);
	aStream.WriteInt32L(iServerItemsAdded);
	aStream.WriteInt32L(iServerItemsChanged);
	aStream.WriteInt32L(iServerItemsDeleted);
	aStream.WriteInt32L(iServerItemsMoved);
	aStream.WriteInt32L(iServerItemsFailed);
	aStream.WriteInt32L( (TInt) iSyncType);
	}

// ---------------------------------------------------------
// CSyncMLHistoryJob::NewL()
// Two phase constructor
// ---------------------------------------------------------	
EXPORT_C CSyncMLHistoryJob* CSyncMLHistoryJob::NewL()
	{
	return new (ELeave) CSyncMLHistoryJob;
	}

// ---------------------------------------------------------
// CSyncMLHistoryJob::NewL(TSmlProfileId aProfileId, const RArray<TSmlTaskId>& aTasks)
// Two phase constructor
// ---------------------------------------------------------	
EXPORT_C CSyncMLHistoryJob* CSyncMLHistoryJob::NewL(TSmlProfileId aProfileId, const RArray<TSmlTaskId>& aTasks)
	{
	CSyncMLHistoryJob * self = new (ELeave) CSyncMLHistoryJob;
	CleanupStack::PushL(self);
	self->ConstructL(aProfileId, aTasks);
	CleanupStack::Pop(); // self
	return self;
	}

// ---------------------------------------------------------
// CSyncMLHistoryJob::~CSyncMLHistoryJob()
// Destructor
// ---------------------------------------------------------		
EXPORT_C CSyncMLHistoryJob::~CSyncMLHistoryJob()
	{
	iTaskInfo.Reset();
	iTaskInfo.Close();
	}

// ---------------------------------------------------------
// CSyncMLHistoryJob::ExternalizeL(RWriteStream& aStream) const
// Writes entry to given stream
// ---------------------------------------------------------	
EXPORT_C void CSyncMLHistoryJob::ExternalizeL(RWriteStream& aStream) const
	{
	CSyncMLHistoryEntry::ExternalizeL(aStream);
	
	TPckgBuf<TTime> start(iStartTime);
	aStream << start;
	
	TPckgBuf<TTime> end(iFinishTime);
	aStream << end;

	TPckgBuf<TTime> lastsync(iLastSuccessSync);
	aStream << lastsync;
		
	aStream.WriteInt32L(iResultCode);
	aStream.WriteInt32L(iProfileId);
		
	TInt count = iTaskInfo.Count();
	aStream.WriteInt32L( count );
	
	for (TInt i = 0; i < count; i++)
		{
		iTaskInfo[i].ExternalizeL(aStream);
		}
	}

// ---------------------------------------------------------
// CSyncMLHistoryJob::Profile() const
// Returns profile id
// ---------------------------------------------------------	
EXPORT_C TSmlProfileId CSyncMLHistoryJob::Profile() const
	{
	return iProfileId;
	}

// ---------------------------------------------------------
// CSyncMLHistoryJob::TimeStamp() const
// Returns entry creation time
// ---------------------------------------------------------	
EXPORT_C TTime CSyncMLHistoryJob::TimeStamp() const
	{
	return iStartTime;
	}

// ---------------------------------------------------------
// CSyncMLHistoryJob::LastSuccessSyncTime() const
// Returns the set Last Success Synchronization time
// ---------------------------------------------------------	
EXPORT_C TTime CSyncMLHistoryJob::LastSuccessSyncTime() const 
	{
	return iLastSuccessSync;
	}

// ---------------------------------------------------------
// CSyncMLHistoryJob::ResultCode() const
// Returns the set result code
// ---------------------------------------------------------	
EXPORT_C TInt CSyncMLHistoryJob::ResultCode() const
	{
	return iResultCode;
	}

// ---------------------------------------------------------
// CSyncMLHistoryJob::FinishTime() const
// Returns the job's finish time
// ---------------------------------------------------------		
EXPORT_C TTime CSyncMLHistoryJob::FinishTime() const
	{
	return iFinishTime;
	}

// ---------------------------------------------------------
// CSyncMLHistoryJob::TaskCount() const
// Returns the job's task count
// ---------------------------------------------------------
EXPORT_C TInt CSyncMLHistoryJob::TaskCount() const
	{
	return iTaskInfo.Count();
	}

// ---------------------------------------------------------
// CSyncMLHistoryJob::TaskAt(TInt aIndex) const
// Returns the task into from specified index
// ---------------------------------------------------------	
EXPORT_C const CSyncMLHistoryJob::TTaskInfo& CSyncMLHistoryJob::TaskAt(TInt aIndex) const
	{
	return iTaskInfo[aIndex];
	}

// ---------------------------------------------------------
// CSyncMLHistoryJob::TaskById(TSmlTaskId aTaskId)
// Returns the task into from specified task
// ---------------------------------------------------------	
EXPORT_C CSyncMLHistoryJob::TTaskInfo& CSyncMLHistoryJob::TaskById(TSmlTaskId aTaskId)
	{
	TInt count = iTaskInfo.Count();
	
	TInt i(0);
	for (i = 0; i < count; i++)
		{
		if (aTaskId == iTaskInfo[i].iTaskId)
			{
			break;
			}
		}
	return iTaskInfo[i];
	}

// ---------------------------------------------------------
// CSyncMLHistoryJob::SetResult(TTime aFinishTime, TInt aResultCode)
// Sets the finish time and result code
// ---------------------------------------------------------		
EXPORT_C void CSyncMLHistoryJob::SetResult(TTime aFinishTime, TInt aResultCode)
	{
	iFinishTime = aFinishTime;
	iResultCode = aResultCode;
	}

// ---------------------------------------------------------
// CSyncMLHistoryJob::SetLastSyncResult(TTime aSuccessSyncTime )
// Sets the Successful synchronization finish time
// ---------------------------------------------------------		
	EXPORT_C  void CSyncMLHistoryJob::SetLastSyncResult(TTime aSuccessSyncTime )
	{
	iLastSuccessSync=aSuccessSyncTime;
	}

// ---------------------------------------------------------
// CSyncMLHistoryJob::ConstructL(TSmlProfileId aProfileId, const RArray<TSmlTaskId>& aTasks)
// Second phase constructor
// ---------------------------------------------------------			
void CSyncMLHistoryJob::ConstructL(TSmlProfileId aProfileId, const RArray<TSmlTaskId>& aTasks)
	{
	iProfileId = aProfileId;
	
	TInt count( aTasks.Count() );
	
	for (TInt i = 0; i < count; i++)
		{
		TInt taskId = aTasks[i];
		TTaskInfo info;
		info.iTaskId = taskId;
		iTaskInfo.AppendL( info );
		}
	
	}

// ---------------------------------------------------------
// CSyncMLHistoryJob::InternalizeL(RReadStream& aStream)
// Reads job entry from given stream
// ---------------------------------------------------------	
void CSyncMLHistoryJob::InternalizeL(RReadStream& aStream)
	{
	CSyncMLHistoryEntry::InternalizeL(aStream);

	TPckgBuf<TTime> start(iStartTime);
	aStream >> start;
	iStartTime = start().Int64();
	
	TPckgBuf<TTime> end(iFinishTime);
	aStream >> end;
	iFinishTime = end().Int64();

	TPckgBuf<TTime> lastsync(iLastSuccessSync);
	aStream >> lastsync;
	iLastSuccessSync = lastsync().Int64();

	iResultCode = aStream.ReadInt32L();
	
	// Added for syncml engine error handling and displaying in view logs
	TInt32 error = iResultCode ;
	TNSmlErrorConversion errorconv( error );
	error = errorconv.Convert();
	if (error <= SyncMLError::KErrSyncEngineErrorBase)
		{
		iResultCode = error ;
		}
	
	iProfileId = aStream.ReadInt32L();
		
	TInt count = aStream.ReadInt32L();
		
	for (TInt i = 0; i < count; i++)
		{
		TTaskInfo info;
		info.InternalizeL(aStream);
		iTaskInfo.AppendL(info);
		}
	}

// ---------------------------------------------------------
// CSyncMLHistoryJob::	CSyncMLHistoryJob()
// Constructor
// ---------------------------------------------------------		
CSyncMLHistoryJob::	CSyncMLHistoryJob()
: CSyncMLHistoryEntry(KUidSmlHistoryEntryJob) , iResultCode(0), iProfileId( KNSmlNullId )
	{
	iStartTime.UniversalTime();
	}
//End of File
