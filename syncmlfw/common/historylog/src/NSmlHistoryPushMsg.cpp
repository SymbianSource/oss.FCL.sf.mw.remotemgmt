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
* Description:  SAN message history entry
*
*/

#include <SyncMLHistory.h>
#include <SyncMLAlertInfo.h>
#include <nsmlconstants.h>

// ---------------------------------------------------------
// CSyncMLHistoryPushMsg::InternalizeL(RReadStream& aStream)
// Creates entry from given stream
// ---------------------------------------------------------
void CSyncMLHistoryPushMsg::InternalizeL(RReadStream& aStream)
	{
	
	CSyncMLHistoryEntry::InternalizeL(aStream);

	TPckgBuf<TTime> time(iTimeStamp);
	aStream >> time;
	iResultCode = aStream.ReadInt32L();
	iRcvCount = aStream.ReadInt32L();
	
	aStream >> iDigest;
	
	TInt count = aStream.ReadInt32L();
		
	for (TInt i = 0; i < count; i++)
		{
		CSmlAlertInfo* info = CSmlAlertInfo::NewLC();
		info->InternalizeL(aStream);
		iAlerts.AppendL(info);
		CleanupStack::Pop();
		}	
	}

// ---------------------------------------------------------
// CSyncMLHistoryPushMsg::NewL()
// Two phase constructor
// ---------------------------------------------------------
EXPORT_C CSyncMLHistoryPushMsg* CSyncMLHistoryPushMsg::NewL()
	{
	return new (ELeave) CSyncMLHistoryPushMsg;
	}

// ---------------------------------------------------------
// CSyncMLHistoryPushMsg::NewL()
// Destructor
// ---------------------------------------------------------
EXPORT_C CSyncMLHistoryPushMsg::~CSyncMLHistoryPushMsg()
	{
	iAlerts.ResetAndDestroy();
	iAlerts.Close();	
	}

// ---------------------------------------------------------
// CSyncMLHistoryPushMsg::ExternalizeL(RWriteStream& aStream) const
// Writes entry to given stream
// ---------------------------------------------------------	
EXPORT_C void CSyncMLHistoryPushMsg::ExternalizeL(RWriteStream& aStream) const
	{
	CSyncMLHistoryEntry::ExternalizeL(aStream);
	
	
	TPckgBuf<TTime> time(iTimeStamp);
	aStream << time;
	aStream.WriteInt32L(iResultCode);
	aStream.WriteInt32L(iRcvCount);
	aStream << iDigest;
	
	TInt count = iAlerts.Count();
	aStream.WriteInt32L( count );
	
	for (TInt i = 0; i < count; i++)
		{
		iAlerts[i]->ExternalizeL(aStream);
		}
	}

// ---------------------------------------------------------
// CSyncMLHistoryPushMsg::Profile() const
// Returns profile id
// ---------------------------------------------------------	
EXPORT_C TSmlProfileId CSyncMLHistoryPushMsg::Profile() const
	{
	TSmlProfileId profileId = KNSmlNullId;
	
	TInt count = iAlerts.Count();
	
	for (TInt i = 0; i < count; i++)
		{
		if ( ( profileId = iAlerts[i]->Profile() ) != KNSmlNullId )
			break;
		}
	return profileId;
	}

// ---------------------------------------------------------
// CSyncMLHistoryPushMsg::TimeStamp() const
// Returns entry creation time
// ---------------------------------------------------------	
EXPORT_C TTime CSyncMLHistoryPushMsg::TimeStamp() const
	{
	return iTimeStamp;	
	}

// ---------------------------------------------------------
// CSyncMLHistoryPushMsg::ResultCode() const
// Returns the set result code
// ---------------------------------------------------------	
EXPORT_C TInt CSyncMLHistoryPushMsg::ResultCode() const
	{
	return iResultCode;
	}

// ---------------------------------------------------------
// CSyncMLHistoryPushMsg::ReceivedCount() const
// Returns receive count
// ---------------------------------------------------------	
EXPORT_C TInt CSyncMLHistoryPushMsg::ReceivedCount() const
	{
	return iRcvCount;
	}

// ---------------------------------------------------------
// CSyncMLHistoryPushMsg::MsgDigest() const
// Returns the set digest
// ---------------------------------------------------------		
EXPORT_C const TDesC8& CSyncMLHistoryPushMsg::MsgDigest() const
	{
	return iDigest;
	}

// ---------------------------------------------------------
// CSyncMLHistoryPushMsg::AlertCount() const
// Returns the count of alerts stored in entry
// ---------------------------------------------------------			
EXPORT_C TInt CSyncMLHistoryPushMsg::AlertCount() const
	{
	return iAlerts.Count();
	}

// ---------------------------------------------------------
// CSyncMLHistoryPushMsg::Alert(TInt aIndex) const
// Returns the alert from specified index
// ---------------------------------------------------------		
EXPORT_C const CSmlAlertInfo& CSyncMLHistoryPushMsg::Alert(TInt aIndex) const
	{
	return *(iAlerts[aIndex]);
	}

// ---------------------------------------------------------
// CSyncMLHistoryPushMsg::IncReceivedCount()
// Increments receive count
// ---------------------------------------------------------	
EXPORT_C void CSyncMLHistoryPushMsg::IncReceivedCount()
	{
	iRcvCount++;
	}

// ---------------------------------------------------------
// CSyncMLHistoryPushMsg::SetMsgDigest(const TDesC8& aMsgDigest)
// Sets digest
// ---------------------------------------------------------	
EXPORT_C void CSyncMLHistoryPushMsg::SetMsgDigest(const TDesC8& aMsgDigest)
	{
	iDigest.Zero();
	iDigest.Copy(aMsgDigest);
	}

// ---------------------------------------------------------
// CSyncMLHistoryPushMsg::AddAlertsL(RPointerArray<CSmlAlertInfo>& aAlertInfoArray) 
// Adds alerts to entry
// Ownership of CSmlAlertInfo objects is transferred.
// ---------------------------------------------------------		
EXPORT_C void CSyncMLHistoryPushMsg::AddAlertsL(RPointerArray<CSmlAlertInfo>& aAlertInfoArray)
	{
	ResetAlerts();
			
	while ( aAlertInfoArray.Count() )
		{
		CSmlAlertInfo* temp = aAlertInfoArray[0];
		iAlerts.AppendL( temp );
		aAlertInfoArray.Remove(0);
		}
		
	}

// ---------------------------------------------------------
// CSyncMLHistoryPushMsg::ResetAlerts()
// Deletes alerts from entry
// ---------------------------------------------------------			
EXPORT_C void CSyncMLHistoryPushMsg::ResetAlerts()
	{
	iAlerts.ResetAndDestroy();
	}

// ---------------------------------------------------------
// CSyncMLHistoryPushMsgSetResultCode(TInt aResultCode)
// Sets result code
// ---------------------------------------------------------			
EXPORT_C void CSyncMLHistoryPushMsg::SetResultCode(TInt aResultCode)
	{
	iResultCode = aResultCode;
	}

// ---------------------------------------------------------
// CSyncMLHistoryPushMsg::CSyncMLHistoryPushMsg()
// Constructor
// ---------------------------------------------------------	
CSyncMLHistoryPushMsg::CSyncMLHistoryPushMsg()
: CSyncMLHistoryEntry(KUidSmlHistoryEntryPushMsg), iResultCode(KErrNone), iRcvCount(0), iAlerts(4)
	{
	iTimeStamp.UniversalTime();
	}

//End of File
