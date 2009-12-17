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
* Description:  History entry array
*
*/


#include <SyncMLHistory.h>
#include <SyncMLAlertInfo.h>
#include "NSmlHistoryArray.h"



///////////////////////////////////////////////////////////////////////////

#ifdef _DEBUG

#include <flogger.h>

_LIT(KLogFile,"smlsync.txt");
//_LIT(KLogDirFullName,"c:\\logs\\");
_LIT(KLogDir,"smlsync");

// Declare the FPrint function
inline void FPrint(const TRefByValue<const TDesC> aFmt, ...)
    {
    VA_LIST list;
    VA_START(list,aFmt);
    RFileLogger::WriteFormat(KLogDir, KLogFile, EFileLoggingModeAppend, aFmt, list);
    }

// ===========================================================================
#ifdef __WINS__     // File logging for WINS
// ===========================================================================
#define FLOG(arg...)   { FPrint(arg); }
//#define FLOG(a) { RDebug::Print(a);  }
// ===========================================================================
#else               // RDebug logging for target HW
// ===========================================================================
#define FLOG(arg...)   { FPrint(arg); }
#endif //__WINS__


// ===========================================================================
#else // // No loggings --> Reduced binary size
// ===========================================================================
#define FLOG(arg...)

#endif // _DEBUG

/////////////////////////////////////////////////////////////////////////////



// ---------------------------------------------------------
// CNSmlHistoryArray::CNSmlHistoryArray()
// Two phase constructor
// ---------------------------------------------------------
EXPORT_C CNSmlHistoryArray* CNSmlHistoryArray::NewL()
	{
	CNSmlHistoryArray* self = new (ELeave) CNSmlHistoryArray;
	return self;	
	}
	
// ---------------------------------------------------------
// CNSmlHistoryArray::CNSmlHistoryArray()
// Constructor
// ---------------------------------------------------------
CNSmlHistoryArray::CNSmlHistoryArray()
: iHistoryOwned(EFalse), iSortOrder(CSyncMLHistoryEntry::ESortByTime)
	{
	
	}

// ---------------------------------------------------------
// CNSmlHistoryArray::CNSmlHistoryArray()
// Destructor
// ---------------------------------------------------------
EXPORT_C CNSmlHistoryArray::~CNSmlHistoryArray()
	{
	DeleteAllEntries();
	iHistory.Close();
	}
		
// ---------------------------------------------------------
// CNSmlHistoryArray::Count()
// Returns the item count
// ---------------------------------------------------------
EXPORT_C TInt CNSmlHistoryArray::Count()
	{
	return iHistory.Count();
	}

// ---------------------------------------------------------
// CNSmlHistoryArray::AppendEntryL(CSyncMLHistoryEntry* aEntry)
// Appends new entry to history array and removes the oldest item
// if more than five items would be in the array
// ---------------------------------------------------------
EXPORT_C void CNSmlHistoryArray::AppendEntryL(CSyncMLHistoryEntry* aEntry)
	{
	if ( aEntry )
		{
		ResetCorruptedHistroy () ;
		if ( aEntry->EntryType().iUid == KSmlHistoryEntryJobTypeValue )
			{
			DoJobLimitCheck();
			}
		else
			{
			DoPushMsgLimitCheck();
			}
		
		iHistory.AppendL( aEntry );
		}
	}

// ---------------------------------------------------------
// CNSmlHistoryArray::ResetCorruptedHistroy()
// This function will reset the history if it is corrupted
// ---------------------------------------------------------

void CNSmlHistoryArray::ResetCorruptedHistroy ()
{
	TTime earliest;
	earliest.UniversalTime();
	TTime historyTime;
	if (iHistory.Count() >= 1)
		{
		SortEntries(CSyncMLHistoryEntry::ESortByTime); 
		historyTime = iHistory[iHistory.Count() - 1]->TimeStamp() ;
		if (historyTime > earliest )
			{
			DeleteAllEntries() ;
			}	
		}
}

// ---------------------------------------------------------
// CNSmlHistoryArray::RemoveEntry(TInt aIndex)
// Removes entry from specified index
// ---------------------------------------------------------
EXPORT_C CSyncMLHistoryEntry* CNSmlHistoryArray::RemoveEntry(TInt aIndex)
	{
	if ( (aIndex >= 0 ) && ( aIndex < iHistory.Count() ) )
		{
		CSyncMLHistoryEntry* entry = iHistory[aIndex];
		iHistory.Remove(aIndex);
		return entry;
		}
	return NULL;
	}

// ---------------------------------------------------------
// CNSmlHistoryArray::DeleteAllEntriesL()
// Deletes all entries from the array
// ---------------------------------------------------------
EXPORT_C void CNSmlHistoryArray::DeleteAllEntries()
	{
	if (iHistoryOwned)
		{
		iHistory.ResetAndDestroy();		
		}
	else
		{
		iHistory.Reset();
		}
	}

// ---------------------------------------------------------
// CNSmlHistoryArray::DeleteAllEntriesL()
// Returns the entry from specified index
// ---------------------------------------------------------
EXPORT_C CSyncMLHistoryEntry& CNSmlHistoryArray::Entry(TInt aIndex)
	{
	return *iHistory[aIndex];
	}

// ---------------------------------------------------------
// CNSmlHistoryArray::SortEntries(CSyncMLHistoryEntry::TSortOrder aSortType)
// Sorts the array using specified sort type
// ---------------------------------------------------------
EXPORT_C void CNSmlHistoryArray::SortEntries(CSyncMLHistoryEntry::TSortOrder aSortType)
	{
	if ( aSortType == CSyncMLHistoryEntry::ESortByTime )
		{
		TLinearOrder<CSyncMLHistoryEntry> sort( CNSmlHistoryArray::SortByTime );	
		iHistory.Sort( sort );
		}
	else
		{
		TLinearOrder<CSyncMLHistoryEntry> sort( CNSmlHistoryArray::SortByType );	
		iHistory.Sort( sort );
		}
	}

// ---------------------------------------------------------
// CNSmlHistoryArray::SetOwnerShip(TBool aOwner)
// Changes the ownership of items. If ETrue array takes ownership.
// ---------------------------------------------------------
EXPORT_C void CNSmlHistoryArray::SetOwnerShip(TBool aOwner)
	{
	iHistoryOwned = aOwner;
	}

// ---------------------------------------------------------
// CNSmlHistoryArray::InternalizeL(RReadStream& aStream)
// Reads history array from given stream
// ---------------------------------------------------------
EXPORT_C void CNSmlHistoryArray::InternalizeL(RReadStream& aStream)
	{
	DeleteAllEntries();
	TInt count = aStream.ReadInt32L();
	
	for (TInt i = 0; i < count; i++)
		{
		CSyncMLHistoryEntry* entry = CSyncMLHistoryEntry::NewL(aStream);
		iHistory.AppendL(entry);
		}
	}

// ---------------------------------------------------------
// CNSmlHistoryArray::ExternalizeL(RWriteStream& aStream) const
// Writes history array to given stream
// ---------------------------------------------------------
EXPORT_C void CNSmlHistoryArray::ExternalizeL(RWriteStream& aStream) const
	{
	TInt count = iHistory.Count();
	aStream.WriteInt32L(count);
	for (TInt i = 0; i < count; i++)
		{
		iHistory[i]->ExternalizeL(aStream);
		}
	}

// ---------------------------------------------------------
// CNSmlHistoryArray::DoJobLimitCheck()
// Limits the amount of CSyncMLHistoryJobs. Oldest is removed.
// ---------------------------------------------------------
void CNSmlHistoryArray::DoJobLimitCheck()
	{
	RemoveHistoryJob();
	
	/*
	TInt count(0);
	TInt target(-1);
	TTime earliest;
	
	earliest.UniversalTime();
	
	for (TInt index = 0; index < iHistory.Count(); index++)
		{
		if ( iHistory[index]->EntryType().iUid == KSmlHistoryEntryJobTypeValue )
			{
			if (iHistory[index]->TimeStamp() < earliest )
				{
				earliest = iHistory[index]->TimeStamp();
				target = index;
				}
			count++;
			}
		}
		
	if (count > KNSmlMaxHistoryLogEntries)
		{
		delete RemoveEntry(target);
		}
	*/
	}


// ---------------------------------------------------------
// CNSmlHistoryArray::DoPushMsgLimitCheck()
// Limits the amount of CSyncMLHistoryPushMsg. Oldest is removed.
// ---------------------------------------------------------
void CNSmlHistoryArray::DoPushMsgLimitCheck()
	{
	TInt count(0);
	TInt target(-1);
	TTime earliest;
	
	earliest.UniversalTime();
	
	for (TInt index = 0; index < iHistory.Count(); index++)
		{
		if ( iHistory[index]->EntryType().iUid == KSmlHistoryEntryPushMsgTypeValue )
			{
			if (iHistory[index]->TimeStamp() < earliest )
				{
				earliest = iHistory[index]->TimeStamp();
				target = index;
				}
			count++;
			}
		}
		
	if (count > KNSmlMaxHistoryLogEntries)
		{
		delete RemoveEntry(target);
		}
	}

// ---------------------------------------------------------
// CNSmlHistoryArray::SortByType(const CSyncMLHistoryEntry& aLeft, const CSyncMLHistoryEntry& aRight)
// Sorts the array by entry type.
// ---------------------------------------------------------
TInt CNSmlHistoryArray::SortByType(const CSyncMLHistoryEntry& aLeft, const CSyncMLHistoryEntry& aRight)
	{
	TInt left = aLeft.EntryType().iUid;
	TInt right = aRight.EntryType().iUid;
	
	if ( left < right )
		{
		return -1; 	// Compared entry type is smaller
		}
	
	if ( left > right )
		{
		return 1;	// Compared entry type is bigger
		}
		
	return 0;
	}

// ---------------------------------------------------------
// CNSmlHistoryArray::SortByTime(const CSyncMLHistoryEntry& aLeft, const CSyncMLHistoryEntry& aRight)
// Sorts the array by entrie's timestamp
// ---------------------------------------------------------
TInt CNSmlHistoryArray::SortByTime(const CSyncMLHistoryEntry& aLeft, const CSyncMLHistoryEntry& aRight)
	{
	
	if ( aLeft.TimeStamp() < aRight.TimeStamp() )
		{
		return -1;	// Compared entry is earlier
		}
	
	if ( aLeft.TimeStamp() > aRight.TimeStamp() )
		{
		return 1;	// Compared entry is later
		}
			
	return 0;
	}


// ---------------------------------------------------------
// CNSmlHistoryArray::RemoveHistoryJob
// 
// ---------------------------------------------------------
void CNSmlHistoryArray::RemoveHistoryJob()
	{
	TInt err = KErrNone;
	
	TRAP(err, RemoveHistoryJobL());
	
	if (err != KErrNone)
		{
		FLOG( _L("### CNSmlHistoryArray::RemoveHistoryJob failed (%d) ###"), err );
		}
		
#ifdef _DEBUG	
	TRAP_IGNORE(LogHistoryArrayL());
	CheckHistoryJobCount();
#endif

	}


// ---------------------------------------------------------
// CNSmlHistoryArray::RemoveHistoryJobL
// 
// This function removes old history job entries from history
// array. If old history job entry contains tasks that do not
// exist in 5 most recent entries, old entry is not removed.
// This way sync information for a task is not lost.
// ---------------------------------------------------------
void CNSmlHistoryArray::RemoveHistoryJobL()
	{
    RArray<TInt> taskList;
    CleanupClosePushL(taskList);
      
	for (;;)
		{
		if (!RemoveHistoryJobL(taskList))
			{
			break;
			}
		}
	
    CleanupStack::PopAndDestroy(&taskList);
	}


// ---------------------------------------------------------
// CNSmlHistoryArray::RemoveHistoryJobL
// 
// ---------------------------------------------------------
TBool CNSmlHistoryArray::RemoveHistoryJobL(RArray<TInt>& aTaskList)
	{
	FLOG( _L("CNSmlHistoryArray::RemoveHistoryJobL START") );
	
	const TInt KMaxExtraHistoryLogEntries = 5;
	const TInt KMaxTotalCount = KNSmlMaxHistoryLogEntries +
	                            KMaxExtraHistoryLogEntries;

	SortEntries(CSyncMLHistoryEntry::ESortByTime);
	InitializeTaskListL(aTaskList);

	TInt historyJobCount = 0;
	TInt count = iHistory.Count();
	
    
    // first pass 5 (KNSmlMaxHistoryLogEntries) latest history
    // job entries and then check whether possible extra entries
    // can be removed
    for (TInt i=count-1; i>=0; i--)
    	{
        if (iHistory[i]->EntryType().iUid == KSmlHistoryEntryJobTypeValue)
        	{
        	historyJobCount++;
        	
        	if (historyJobCount > KNSmlMaxHistoryLogEntries)
        		{
         		const CSyncMLHistoryEntry* entry = iHistory[i];
        		const CSyncMLHistoryJob* historyJob = 
        		      CSyncMLHistoryJob::DynamicCast(entry);

         	    if (historyJobCount > KMaxTotalCount)
        		    {
        		    // history array is full - delete extra entry
        		    FLOG( _L("### history array overflow - delete extra entry ###") );
        		    delete RemoveEntry(i);
        		    
                  	FLOG( _L("CNSmlHistoryArray::RemoveHistoryJobL END") );
                	return ETrue;
         		    }
                
                if (CanRemove(historyJob, aTaskList))
                	{
                	FLOG(_L("remove history job entry"));
#ifdef _DEBUG                	
                	LogHistoryJobL(historyJob);
#endif                	
                	delete RemoveEntry(i);
                	
                	FLOG( _L("CNSmlHistoryArray::RemoveHistoryJobL END") );
                	return ETrue;
                	}
                else
                	{
                	FLOG(_L("leave extra history job entry"));
#ifdef _DEBUG                	
                	LogHistoryJobL(historyJob);
#endif                	
                	
                	UpdateTaskListL(historyJob, aTaskList);
                 	}
        		}
        	}
    	}
    	
    FLOG( _L("CNSmlHistoryArray::RemoveHistoryJobL END") );
    return EFalse;
	}


// ---------------------------------------------------------
// CNSmlHistoryArray::InitializeTaskListL
// 
// ---------------------------------------------------------
void CNSmlHistoryArray::InitializeTaskListL(RArray<TInt>& aTaskList)
	{
	aTaskList.Reset();
	TInt historyJobCount = 0;
	TInt count = iHistory.Count();
	
    // store sync tasks from 5 most recent history job entries
    for (TInt i=count-1; i>=0; i--)
    	{
        if (iHistory[i]->EntryType().iUid == KSmlHistoryEntryJobTypeValue)
        	{
        	historyJobCount++;
        	if (historyJobCount <= KNSmlMaxHistoryLogEntries)
        		{
        		const CSyncMLHistoryEntry* entry = iHistory[i];
        		const CSyncMLHistoryJob* historyJob = 
        		      CSyncMLHistoryJob::DynamicCast(entry);
        		
        		UpdateTaskListL(historyJob, aTaskList);
        		}
        	}
    	}
	}


// ---------------------------------------------------------
// CNSmlHistoryArray::UpdateTaskListL
// 
// ---------------------------------------------------------
void CNSmlHistoryArray::UpdateTaskListL(const CSyncMLHistoryJob* aHistoryJob, 
                                 RArray<TInt>& aTaskList)
	{
	TInt count = aHistoryJob->TaskCount();
	for (TInt i=0; i<count; i++)
		{
		const CSyncMLHistoryJob::TTaskInfo& taskInfo = 
		                         aHistoryJob->TaskAt(i);
		if (aTaskList.Find(taskInfo.iTaskId) == KErrNotFound)
			{
			User::LeaveIfError(aTaskList.Append(taskInfo.iTaskId));
			}
		}
	}


// ---------------------------------------------------------
// CNSmlHistoryArray::CanRemove
// 
// ---------------------------------------------------------
TBool CNSmlHistoryArray::CanRemove(const CSyncMLHistoryJob* aHistoryJob, 
                                   RArray<TInt>& aTaskList)
	{
	
	// check wheteher this history job has a task that does not 
	// exist in aTaskList
	TInt count = aHistoryJob->TaskCount();
	for (TInt i=0; i<count; i++)
		{
		const CSyncMLHistoryJob::TTaskInfo& taskInfo = 
		                         aHistoryJob->TaskAt(i);
		                         
		if (aTaskList.Find(taskInfo.iTaskId) == KErrNotFound)
			{
			return EFalse;
			}
		}
		
	return ETrue;
	}


#ifdef _DEBUG

// ---------------------------------------------------------
// CNSmlHistoryArray::CheckHistoryJobCount
// 
// ---------------------------------------------------------
void CNSmlHistoryArray::CheckHistoryJobCount()
	{
	_LIT(KPanicCategory,"CNSmlHistoryArray");

	const TInt KMaxExtraHistoryLogEntries = 5;
	TInt limit = KNSmlMaxHistoryLogEntries + KMaxExtraHistoryLogEntries;
	
	TInt historyJobCount = 0;
	TInt count = iHistory.Count();
	
    for (TInt i=count-1; i>=0; i--)
    	{
        if (iHistory[i]->EntryType().iUid == KSmlHistoryEntryJobTypeValue)
        	{
        	historyJobCount++;
        	
        	if (historyJobCount > limit)
        		{
       			User::Panic(KPanicCategory, KErrOverflow); 
        		}
        	}
    	}
	}


// ---------------------------------------------------------
// CNSmlHistoryArray::LogHistoryJobL
// 
// ---------------------------------------------------------
void CNSmlHistoryArray::LogHistoryJobL(const CSyncMLHistoryJob* aHistoryJob)
	{
	FLOG( _L("---- CSyncMLHistoryJob ----") );
	
	TBuf<128> buf1; TBuf<128> buf2; TBuf<128> buf3;
	
	TTime time = aHistoryJob->TimeStamp();
	GetDateTimeTextL(buf1, time);
	buf2.Format(_L("sync time: %S"), &buf1);
	FLOG(buf2);
	
	buf2 = KNullDesC;
	TInt count = aHistoryJob->TaskCount();
	for (TInt i=0; i<count; i++)
		{
		const CSyncMLHistoryJob::TTaskInfo& taskInfo = 
		                         aHistoryJob->TaskAt(i);
        buf3.Format(_L("%d "), taskInfo.iTaskId);		                         
		buf2.Append(buf3);
		}
		
	buf1.Format(_L("sync tasks: %S"), &buf2);
	FLOG(buf1);

	
	FLOG( _L("---- CSyncMLHistoryJob ----") );
	}
	
	
// ---------------------------------------------------------
// CNSmlHistoryArray::LogHistoryArrayL
// 
// ---------------------------------------------------------
void CNSmlHistoryArray::LogHistoryArrayL()
	{
	FLOG( _L("---- history array ----") );
	
	TInt historyJobCount = 0;
	TInt count = iHistory.Count();
	
	SortEntries(CSyncMLHistoryEntry::ESortByTime);
    
    for (TInt i=count-1; i>=0; i--)
    	{
        if (iHistory[i]->EntryType().iUid == KSmlHistoryEntryJobTypeValue)
        	{
        	historyJobCount++;
        	
       		const CSyncMLHistoryEntry* entry = iHistory[i];
       		const CSyncMLHistoryJob* historyJob = 
        		      CSyncMLHistoryJob::DynamicCast(entry);
          	LogHistoryJobL(historyJob);
         	}
    	}
    	
    FLOG( _L("---- history array ----") );
	}


// ---------------------------------------------------------
// CNSmlHistoryArray::GetDateTimeTextL
// 
// ---------------------------------------------------------
void CNSmlHistoryArray::GetDateTimeTextL(TDes& aText, TTime aDateTime)
	{
	TDateTime dt = aDateTime.DateTime();
	aText.Format(_L("%02d.%02d.%04d %02d:%02d:%02d"), dt.Day()+1, dt.Month()+1, dt.Year(),  dt.Hour(), dt.Minute(), dt.Second());
	}

#endif


