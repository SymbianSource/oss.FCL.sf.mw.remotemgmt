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
* Description:  History entry
*
*/


#include <s32mem.h>
#include <SyncMLHistory.h>
#include <SyncMLAlertInfo.h>

// ---------------------------------------------------------
// CSyncMLHistoryEntry::CSyncMLHistoryEntry(TUid aUid)
// Constructor
// ---------------------------------------------------------
CSyncMLHistoryEntry::CSyncMLHistoryEntry(TUid aUid)
: iEntryType(aUid), iEntryId(0), iSpare(0)
	{
	}

// ---------------------------------------------------------
// CSyncMLHistoryEntry::NewL(TUid aEntryType)
// Two phase constructor
// ---------------------------------------------------------
EXPORT_C CSyncMLHistoryEntry* CSyncMLHistoryEntry::NewL(TUid aEntryType)
	{
	if (aEntryType == KUidSmlHistoryEntryJob)
		{		
		CSyncMLHistoryJob* self =CSyncMLHistoryJob::NewL();		
		return self;	
		}
	else
	if (aEntryType == KUidSmlHistoryEntryPushMsg)
		{
		CSyncMLHistoryPushMsg* self = CSyncMLHistoryPushMsg::NewL();			
		return self;
		}
	
	return NULL;
	}

// ---------------------------------------------------------
// CSyncMLHistoryEntry::NewL(RReadStream& aStream)
// Two phase constructor
// ---------------------------------------------------------	
EXPORT_C CSyncMLHistoryEntry* CSyncMLHistoryEntry::NewL(RReadStream& aStream)
	{
	const TInt entryType = aStream.ReadInt32L();
	
	TUid entryUid;
	entryUid.iUid = entryType;
	
	CSyncMLHistoryEntry* self = CSyncMLHistoryEntry::NewL(entryUid);
	CleanupStack::PushL(self);
	self->InternalizeL(aStream);
	CleanupStack::Pop();
	return self;
	}

// ---------------------------------------------------------
// CSyncMLHistoryEntry::NewL(const CSyncMLHistoryEntry& aEntry)
// Two phase constructor
// ---------------------------------------------------------	
EXPORT_C CSyncMLHistoryEntry* CSyncMLHistoryEntry::NewL(const CSyncMLHistoryEntry& aEntry)
	{
	TUid entryType = aEntry.EntryType();
	
	CSyncMLHistoryEntry* entry = CSyncMLHistoryEntry::NewL(aEntry.iEntryType);
	CleanupStack::PushL(entry);
	
	CBufBase* buffer = CBufFlat::NewL(1024);
	CleanupStack::PushL(buffer);
	
	RBufWriteStream writeStream(*buffer);
	writeStream.PushL();
	aEntry.ExternalizeL(writeStream);
	writeStream.CommitL();
	
	CleanupStack::PopAndDestroy(); //writeStream
	
	RBufReadStream readStream(*buffer);
	readStream.PushL();
	entry->InternalizeL(readStream);
	
	CleanupStack::PopAndDestroy(2); //readStream, buffer
	CleanupStack::Pop(); //entry
	
	return entry;
	}

// ---------------------------------------------------------
// CSyncMLHistoryEntry::~CSyncMLHistoryEntry()
// Destructor
// ---------------------------------------------------------		
EXPORT_C CSyncMLHistoryEntry::~CSyncMLHistoryEntry()
	{
	
	}

// ---------------------------------------------------------
// CSyncMLHistoryEntry::ExternalizeL(RWriteStream& aStream) const
// Writes entry to given stream
// ---------------------------------------------------------		
EXPORT_C void CSyncMLHistoryEntry::ExternalizeL(RWriteStream& aStream) const
	{
	aStream.WriteInt32L( iEntryType.iUid  );
	aStream.WriteInt32L( iEntryId );
	aStream.WriteInt32L( iSpare );
	}

// ---------------------------------------------------------
// CSyncMLHistoryEntry::EntryType() const
// Returns entry type identifier
// ---------------------------------------------------------		
EXPORT_C TUid CSyncMLHistoryEntry::EntryType() const
	{
	return iEntryType;
	}

// ---------------------------------------------------------
// CSyncMLHistoryEntry::EntryId() const
// Returns entry identifier
// ---------------------------------------------------------		
EXPORT_C TInt CSyncMLHistoryEntry::EntryId() const
	{
	return iEntryId;
	}

// ---------------------------------------------------------
// CSyncMLHistoryEntry::DoDynamicCast(TUid aEntryType, CSyncMLHistoryEntry* aEntry)
// Checks is entry is valid.
// ---------------------------------------------------------		
EXPORT_C CSyncMLHistoryEntry* CSyncMLHistoryEntry::DoDynamicCast(TUid aEntryType, CSyncMLHistoryEntry* aEntry)
	{
	if (aEntryType != aEntry->EntryType())
		{
		return NULL;
		}
		
	return aEntry;
	}

// ---------------------------------------------------------
// CSyncMLHistoryEntry::DoDynamicCast(TUid aEntryType, const CSyncMLHistoryEntry* aEntry)
// Checks is entry is valid.
// ---------------------------------------------------------		
EXPORT_C const CSyncMLHistoryEntry* CSyncMLHistoryEntry::DoDynamicCast(TUid aEntryType, const CSyncMLHistoryEntry* aEntry)
	{
	if (aEntryType != aEntry->EntryType())
		{
		return NULL;
		}
		
	return aEntry;
	}

// ---------------------------------------------------------
// CSyncMLHistoryEntry::SetEntryId(TInt aEntryId)
// Sets entry identifier
// ---------------------------------------------------------	
EXPORT_C void CSyncMLHistoryEntry::SetEntryId(TInt aEntryId)
	{
	iEntryId = aEntryId;
	}

// ---------------------------------------------------------
// CSyncMLHistoryEntry::Reserved(TUid)
// Reserved for future use
// ---------------------------------------------------------		
EXPORT_C TAny* CSyncMLHistoryEntry::Reserved(TUid)
	{
	return 0;
	}

// ---------------------------------------------------------
// CSyncMLHistoryEntry::InternalizeL(RReadStream& aStream)
// Reads item from stream
// ---------------------------------------------------------
void CSyncMLHistoryEntry::InternalizeL(RReadStream& aStream)
	{
	iEntryId = aStream.ReadInt32L();
	iSpare = aStream.ReadInt32L();
	}
