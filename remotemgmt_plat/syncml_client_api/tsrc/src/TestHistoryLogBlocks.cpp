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
* Description:  This file contains all function definitions that can be called 
*		from cfg file and also contains definitions for some local functions. 
*		These functions will do the necessary operation by calling other class 
*		(source component) functions.
*
*/


// INCLUDE FILES
#include <e32svr.h>
#include <StifParser.h>
#include <Stiftestinterface.h>
#include <s32mem.h>
#include <e32base.h>
#include <SyncMLAlertInfo.h>
#include <SyncMLHistory.h>
#include "TestHistoryLog.h"

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CTestHistoryLog::Delete
// Delete here all resources allocated and opened from test methods. 
// Called from destructor. 
// -----------------------------------------------------------------------------
//
void CTestHistoryLog::Delete() 
    {    
    }
    
// -----------------------------------------------------------------------------
// CTestHistoryLog::RunMethodL
// Run specified method. Contains also table of test mothods and their names.
// -----------------------------------------------------------------------------
//
TInt CTestHistoryLog::RunMethodL( 
    CStifItemParser& aItem ) 
    {
   
    static TStifFunctionInfo const KFunctions[] =
        {  
        // Copy this line for every implemented function.
        // First string is the function name used in TestScripter script file.
        // Second is the actual implementation member function. 
        ENTRY( "CreateHistoryArray", CTestHistoryLog::CreateHistoryArrayL ),
        ENTRY( "CreateHistoryArrayOwned", CTestHistoryLog::CreateHistoryArrayOwnedL ),
        ENTRY( "DeleteHistoryArray",	CTestHistoryLog::DeleteHistoryArray ),
        ENTRY( "ResetHistoryArray",	CTestHistoryLog::ResetHistoryArray ),
        ENTRY( "AddHistoryJobNull",	CTestHistoryLog::AddNullEntryL ),
        ENTRY( "AddNewJobEntry",	CTestHistoryLog::AddNewJobEntryL ),
        ENTRY( "AddNewPushMsgEntry", CTestHistoryLog::AddNewPushMsgEntryL ),
        ENTRY( "AddNewPushMsgAndTaskEntry", CTestHistoryLog::AddNewPushMsgAndTaskEntryL ),
        ENTRY( "RemoveEntry", CTestHistoryLog::RemoveEntry ),
        ENTRY( "SortByType", CTestHistoryLog::SortByType ),
        ENTRY( "SortByTime", CTestHistoryLog::SortByTime ),
        ENTRY( "DoStreaming", CTestHistoryLog::DoStreamingL ),
        ENTRY( "AddMoreThanFivePushMsgs", CTestHistoryLog::AddMoreThanFivePushMsgsL ),
        ENTRY( "AddMoreThanFiveJobs", CTestHistoryLog::AddMoreThanFiveJobsL ),
        ENTRY( "CreateAndDestroyAlertInfo", CTestHistoryLog::CreateAndDestroyAlertInfoL ),
        ENTRY( "CreateAndDestroyJob", CTestHistoryLog::CreateAndDestroyJobL ),
        ENTRY( "CreateAndDestroyPushMsg", CTestHistoryLog::CreateAndDestroyPushMsgL ),
        ENTRY( "SetGetAndResetAlertInfo", CTestHistoryLog::SetGetAndResetAlertInfoL ),
        };

    const TInt count = sizeof( KFunctions ) / 
                        sizeof( TStifFunctionInfo );

    return RunInternalL( KFunctions, count, aItem );

    }    


TInt CTestHistoryLog::CreateHistoryArrayL( CStifItemParser& /*aItem*/ )
	{
	iHistoryArray = CNSmlHistoryArray::NewL();
	iOwnerShip = EFalse;
	iHistoryArray->SetOwnerShip(iOwnerShip);
	return KErrNone;
	}
	
TInt CTestHistoryLog::CreateHistoryArrayOwnedL( CStifItemParser& /*aItem*/ )
	{
	iHistoryArray = CNSmlHistoryArray::NewL();
	iOwnerShip = ETrue;
	iHistoryArray->SetOwnerShip(iOwnerShip);
	return KErrNone;
	}
		
TInt CTestHistoryLog::DeleteHistoryArray( CStifItemParser& /*aItem*/ )
	{
	delete iHistoryArray;
	iHistoryArray = NULL;
	return KErrNone;
	}

TInt CTestHistoryLog::ResetHistoryArray( CStifItemParser& /*aItem*/ )
	{
	iHistoryArray->DeleteAllEntries();
	return KErrNone;
	}
	
TInt CTestHistoryLog::AddNullEntryL( CStifItemParser& /*aItem*/ )
	{
	iHistoryArray = CNSmlHistoryArray::NewL();
	iHistoryArray->AppendEntryL( NULL );
	delete iHistoryArray;
	iHistoryArray = NULL;
	return KErrNone;
	}
	
TInt CTestHistoryLog::CreateAndDestroyJobL( CStifItemParser& /*aItem*/ )
	{
	const TInt KTaskId3 = 12346;
	const TInt KTaskId4 = 12347;
	RArray<TInt> tasks;
	tasks.Append(KTaskId1);
	tasks.Append(KTaskId2);
	tasks.Append(KTaskId3);
	tasks.Append(KTaskId4);
	
	CSyncMLHistoryJob * entryJob = CSyncMLHistoryJob::NewL(1, tasks );
	CleanupStack::PushL(entryJob);
	tasks.Reset();
	tasks.Close();
	
	TTime now;
	now.UniversalTime();
	
	entryJob->SetResult(now, KErrorFive);
	CSyncMLHistoryJob::TTaskInfo& taskInfo = entryJob->TaskById(KTaskId3);
	
	taskInfo.iError = KErrorOne;
	taskInfo.iItemsAdded = KFive;
	taskInfo.iItemsChanged = KFour;
	taskInfo.iItemsDeleted = KThree;
	taskInfo.iItemsFailed = KTwo;
	taskInfo.iItemsMoved = KOne;
	
	taskInfo.iServerItemsAdded = KOne;
	taskInfo.iServerItemsChanged = KTwo;
	taskInfo.iServerItemsDeleted = KThree;
	taskInfo.iServerItemsFailed = KFour;
	taskInfo.iServerItemsMoved = KFive;
	
	CleanupStack::PopAndDestroy(entryJob);		
	return KErrNone;
	}

TInt CTestHistoryLog::AddNewJobEntryL( CStifItemParser& /*aItem*/ )
	{
	TInt arrayCount = iHistoryArray->Count() + 1;
	TInt profileId(1);	
	RArray<TInt> tasks;
	tasks.Append(KTaskId1);
	tasks.Append(KTaskId2);
	
	CSyncMLHistoryJob * entry = CSyncMLHistoryJob::NewL(profileId, tasks );
	CleanupStack::PushL(entry);
	
	tasks.Reset();
	tasks.Close();

	iHistoryArray->AppendEntryL(entry);
	
	TL(iHistoryArray->Count() == arrayCount);
	
	if (iOwnerShip)
		{
		CleanupStack::Pop(entry);
		}
	else
		{
		CleanupStack::PopAndDestroy(entry);		
		}	
	return KErrNone;
	}
	
TInt CTestHistoryLog::AddMoreThanFiveJobsL( CStifItemParser& /*aItem*/ )
	{
	for (TInt i = 0; i < KMaxMsgSize; i++)
		{
		RArray<TInt> tasks;
		tasks.Append(KTaskId1);
		tasks.Append(KTaskId2);
	
		CSyncMLHistoryJob * entry = CSyncMLHistoryJob::NewL(i, tasks );
		CleanupStack::PushL(entry);
	
		tasks.Reset();
		tasks.Close();

		iHistoryArray->AppendEntryL(entry);

	if (iOwnerShip)
		{
		CleanupStack::Pop(entry);
		}
	else
		{
		CleanupStack::PopAndDestroy(entry);
		}
		}
	TL(iHistoryArray->Count() == KMaxMsgSize);
	
	return KErrNone;
	}
		
TInt CTestHistoryLog::AddNewPushMsgEntryL( CStifItemParser& /*aItem*/ )
	{
	TInt arrayCount = iHistoryArray->Count() + 1;	
	
	CSyncMLHistoryPushMsg * entry = CSyncMLHistoryPushMsg::NewL();
	CleanupStack::PushL(entry);
	
	iHistoryArray->AppendEntryL(entry);
	
	TL(iHistoryArray->Count() == arrayCount);
	
	if (iOwnerShip)
		{
		CleanupStack::Pop(entry);
		}
	else
		{
		CleanupStack::PopAndDestroy(entry);		
		}		
	return KErrNone;
	}

TInt CTestHistoryLog::CreateAndDestroyPushMsgL( CStifItemParser& /*aItem*/ )
	{
	CSyncMLHistoryPushMsg * entry = CSyncMLHistoryPushMsg::NewL();
	CleanupStack::PushL(entry);
	
	entry->SetMsgDigest(KMsgDigest);
	entry->SetResultCode(KErrorEleven);
	entry->IncReceivedCount();
	
	CSmlAlertInfo* info = CSmlAlertInfo::NewLC();
	info->TaskIds().Append(1);
	info->TaskIds().Append(KTwo);
	info->TaskSyncTypes().Append( (TSmlSyncType)0);
	info->TaskSyncTypes().Append( (TSmlSyncType)1);
	info->SetConfirmation(ETrue);
	info->SetConnectionId(KUidNSmlMediumTypeInternet.iUid);
	info->SetErrorCode(KErrorEleven);
	info->SetJobControl(CSmlAlertInfo::ECreateJob);
	info->SetProfileId(1);
	info->SetProtocolType(ESmlDataSync);
	info->SetSessionId(KTwo);
	info->SetTransportId(KUidNSmlMediumTypeInternet.iUid);
	info->SetVendorSpecificInfoL(KVendorInfo);
	
	CleanupStack::PopAndDestroy(info);
	CleanupStack::PopAndDestroy(entry);	
	return KErrNone;
	}	

TInt CTestHistoryLog::AddMoreThanFivePushMsgsL( CStifItemParser& /*aItem*/ )
	{
	
	for (TInt i = 0; i < KMaxMsgSize; i++)
		{
		CSyncMLHistoryPushMsg * entry = CSyncMLHistoryPushMsg::NewL();
		CleanupStack::PushL(entry);
	
		CSmlAlertInfo info;
		info.TaskIds().Append(KOne);
		info.TaskIds().Append(KTwo);
	
		info.TaskSyncTypes().Append( (TSmlSyncType)0);
		info.TaskSyncTypes().Append( (TSmlSyncType)KOne);
	
		iHistoryArray->AppendEntryL(entry);

		if (iOwnerShip)
			{
			CleanupStack::Pop(entry);
			}
		else
			{
			CleanupStack::PopAndDestroy(entry);		
			}	
		}
	TL(iHistoryArray->Count() == KMaxMsgSize);
	
	return KErrNone;
	}
	
TInt CTestHistoryLog::AddNewPushMsgAndTaskEntryL( CStifItemParser& /*aItem*/ )
	{
	TInt arrayCount = iHistoryArray->Count() + 1;	
	
	CSyncMLHistoryPushMsg * entry = CSyncMLHistoryPushMsg::NewL();
	CleanupStack::PushL(entry);
	
	CSmlAlertInfo* info = CSmlAlertInfo::NewLC();
	info->TaskIds().Append(1);
	info->TaskIds().Append(KTwo);
	
	info->TaskSyncTypes().Append( (TSmlSyncType)0);
	info->TaskSyncTypes().Append( (TSmlSyncType)1);
	
	RPointerArray<CSmlAlertInfo> alertInfoArray;
	CleanupClosePushL(alertInfoArray);
	alertInfoArray.Append(info);
	entry->AddAlertsL(alertInfoArray);
	iHistoryArray->AppendEntryL(entry);
	
	//Given KOne constant in the below line just to pop one item 
	 //and not given 'alertInfoArray' as PushL doesnot take that
	CleanupStack::PopAndDestroy(KOne); //alertInfoArray
	CleanupStack::Pop(info);
		
	TL(iHistoryArray->Count() == arrayCount);	
	
	if (iOwnerShip)
		{
		CleanupStack::Pop(entry);
		}
	else
		{
		CleanupStack::PopAndDestroy(entry);
		}			
	return KErrNone;
	}

TInt CTestHistoryLog::RemoveEntry( CStifItemParser& /*aItem*/ )
	{
	CSyncMLHistoryEntry* tempPtrToHistoryArray;

	TL(iHistoryArray->Count() == KTwo);
	tempPtrToHistoryArray = iHistoryArray->RemoveEntry(1);
	delete tempPtrToHistoryArray;
	tempPtrToHistoryArray = NULL;
	
	TL(iHistoryArray->Count() == 1);
	tempPtrToHistoryArray = iHistoryArray->RemoveEntry(0);
	delete tempPtrToHistoryArray;
	tempPtrToHistoryArray = NULL;
	
	TL(iHistoryArray->Count() == 0);
	tempPtrToHistoryArray = iHistoryArray->RemoveEntry(-1);
	delete tempPtrToHistoryArray;
	tempPtrToHistoryArray = NULL;
	
	return KErrNone;
	}

TInt CTestHistoryLog::SortByType( CStifItemParser& /*aItem*/ )
	{
	CSyncMLHistoryEntry& hEntry = iHistoryArray->Entry(0);	
	CSyncMLHistoryEntry& hEntry2 = iHistoryArray->Entry(1);
	
	iHistoryArray->SortEntries( CSyncMLHistoryEntry::ESortByType);
	
	CSyncMLHistoryEntry& hEntry3 = iHistoryArray->Entry(0);
	CSyncMLHistoryEntry& hEntry4 = iHistoryArray->Entry(1);

	return KErrNone;
	}
	
TInt CTestHistoryLog::SortByTime( CStifItemParser& /*aItem*/ )
	{
	CSyncMLHistoryEntry& hEntry = iHistoryArray->Entry(0);	
	CSyncMLHistoryEntry& hEntry2 = iHistoryArray->Entry(1);
	
	iHistoryArray->SortEntries( CSyncMLHistoryEntry::ESortByTime);
	
	CSyncMLHistoryEntry& hEntry3 = iHistoryArray->Entry(0);
	CSyncMLHistoryEntry& hEntry4 = iHistoryArray->Entry(1);
	
	return KErrNone;
	}

TInt CTestHistoryLog::CreateAndDestroyAlertInfoL( CStifItemParser& /*aItem*/ )
	{
	CSmlAlertInfo* info = CSmlAlertInfo::NewLC();
	info->TaskIds().Append(1);
	info->TaskIds().Append(KTwo);
	info->TaskSyncTypes().Append( (TSmlSyncType)0);
	info->TaskSyncTypes().Append( (TSmlSyncType)1);
	info->SetConfirmation(ETrue);
	info->SetConnectionId(KUidNSmlMediumTypeInternet.iUid);
	info->SetErrorCode(KErrorEleven);
	info->SetJobControl(CSmlAlertInfo::ECreateJob);
	info->SetProfileId(1);
	info->SetProtocolType(ESmlDataSync);
	info->SetSessionId(KTwo);
	info->SetTransportId(KUidNSmlMediumTypeInternet.iUid);
	info->SetVendorSpecificInfoL(KVendorInfo);
	
	CleanupStack::PopAndDestroy(info);
	return KErrNone;
	}
	
TInt CTestHistoryLog::SetGetAndResetAlertInfoL( CStifItemParser& /*aItem */)
{
	//Defining Constants
	const TInt KBufSize = 10;
  TBuf8<KBufSize> vendorInfo;
  vendorInfo.Copy(KVendorInfo);  	
	
	//Creating Object
	CSmlAlertInfo* info = CSmlAlertInfo::NewLC();

	//Setting Info 
	info->TaskIds().Append(1);
	info->TaskIds().Append(KTwo);
	info->TaskSyncTypes().Append( (TSmlSyncType)0);
	info->TaskSyncTypes().Append( (TSmlSyncType)1);
	info->SetConfirmation(EFalse);
	info->SetConnectionId(KUidNSmlMediumTypeInternet.iUid);
	info->SetErrorCode(KErrorEleven);
	info->SetJobControl(CSmlAlertInfo::ECreateJob);
	info->SetProfileId(KOne);
	info->SetProtocolType(ESmlDataSync);
	info->SetSessionId(KTwo);
	info->SetTransportId(KUidNSmlMediumTypeInternet.iUid);
	info->SetVendorSpecificInfoL(vendorInfo);
	info->SetUimode(KTwelve);
	
	//Verifying the info
	TL( info->Connection() == KUidNSmlMediumTypeInternet.iUid );
	TL( info->ErrorCode() == KErrorEleven );
	TL( info->JobControl() == CSmlAlertInfo::ECreateJob );
	TL( info->Profile() == KOne );
	TL( info->Protocol() == ESmlDataSync );
	TL( info->SessionId() == KTwo );
	TL( info->Transport() == KUidNSmlMediumTypeInternet.iUid );	
	TBuf8<KBufSize> getVendorInfo = info->VendorSpecificInfo();
	TL(!(vendorInfo.Compare(getVendorInfo)));
	TL( info->GetUimode() == KTwelve );
	TL( info->CreateSession() );
	
	//Reset
	info->Reset();
	
	//Verification after Reset
	TL( info->JobControl() == CSmlAlertInfo::EDoNotCreateJob );
	TL( !info->CreateSession() );
	TL( info->VendorSpecificInfo() == KNullDesC8 );
	
	//Destroy the object
	CleanupStack::PopAndDestroy(info);
	//Return no error if everything passes
  return KErrNone; 
}	

TInt CTestHistoryLog::DoStreamingL( CStifItemParser& /*aItem*/ )
	{
	CSyncMLHistoryPushMsg * entry = CSyncMLHistoryPushMsg::NewL();
	CleanupStack::PushL(entry);
	
	entry->SetMsgDigest(KMsgDigest);
	entry->SetResultCode(KErrorEleven);
	entry->IncReceivedCount();
	
	RPointerArray<CSmlAlertInfo> alertInfoArray;
	CleanupClosePushL( alertInfoArray );
			
	CSmlAlertInfo* info = CSmlAlertInfo::NewLC();
	info->TaskIds().Append(1);
	info->TaskIds().Append(KTwo);
	info->TaskSyncTypes().Append( (TSmlSyncType)0);
	info->TaskSyncTypes().Append( (TSmlSyncType)1);
	info->SetConfirmation(ETrue);
	info->SetConnectionId(KUidNSmlMediumTypeInternet.iUid);
	info->SetErrorCode(KErrorEleven);
	info->SetJobControl(CSmlAlertInfo::ECreateJob);
	info->SetProfileId(1);
	info->SetProtocolType(ESmlDataSync);
	info->SetSessionId(KTwo);
	info->SetTransportId(KUidNSmlMediumTypeInternet.iUid);
	info->SetVendorSpecificInfoL(KVendorInfo);
	info->SetUimode(KTwelve);
	
	alertInfoArray.AppendL(info);
	entry->AddAlertsL(alertInfoArray);

	iHistoryArray->AppendEntryL(entry); 
	
	CleanupStack::Pop(info);
	//Given KOne constant in the below line just to pop one item 
	 //and not given 'alertInfoArray' as PushL doesnot take that
	CleanupStack::PopAndDestroy(KOne); //alertInfoArray
	CleanupStack::Pop(entry);
	
	TInt profileId(1);

	RArray<TInt> tasks;
	tasks.Append(KTaskId1);
	tasks.Append(KTaskId2);
	
	CSyncMLHistoryJob * entryJob = CSyncMLHistoryJob::NewL(profileId, tasks );
	CleanupStack::PushL(entryJob);
	tasks.Reset();
	tasks.Close();
	
	TTime lastsync = 0;
	
	TTime now;
	now.UniversalTime();
	
	entryJob->SetResult(now, KErrorFive);
	entryJob->SetLastSyncResult(lastsync);
	CSyncMLHistoryJob::TTaskInfo& taskInfo = entryJob->TaskById(KTaskId1);
	
	taskInfo.iError = KErrorOne;
	taskInfo.iItemsAdded = KFive;
	taskInfo.iItemsChanged = KFour;
	taskInfo.iItemsDeleted = KThree;
	taskInfo.iItemsFailed = KTwo;
	taskInfo.iItemsMoved = KOne;
	
	taskInfo.iServerItemsAdded = KOne;
	taskInfo.iServerItemsChanged = KTwo;
	taskInfo.iServerItemsDeleted = KThree;
	taskInfo.iServerItemsFailed = KFour;
	taskInfo.iServerItemsMoved = KFive;
	
	iHistoryArray->AppendEntryL(entryJob);
	
	if (iOwnerShip)
		{
		CleanupStack::Pop(entryJob);
		}
	else
		{
		CleanupStack::PopAndDestroy(entryJob);
		}
		
	CBufBase* buffer = CBufFlat::NewL(KMaxBufSize);
	CleanupStack::PushL(buffer);
	RBufWriteStream writeStream(*buffer);
	writeStream.PushL();
	
	iHistoryArray->ExternalizeL(writeStream);
	//Given KOne constant in the below line just to pop one item 
	 //and not given 'alertInfoArray' as PushL doesnot take that
	CleanupStack::PopAndDestroy(KOne); //writeStream
	
	RBufReadStream readStream(*buffer);
	readStream.PushL();
	iHistoryArray->InternalizeL(readStream);
	//Given KOne constant in the below line just to pop one item 
	 //and not given 'alertInfoArray' as PushL doesnot take that
	CleanupStack::PopAndDestroy(KOne); //readStream
	CleanupStack::PopAndDestroy(buffer);
	
	//Assinging CSyncMLHistoryEntry to CSyncMLHistoryPushMsg and verifies the values
	CSyncMLHistoryEntry& hEntry = iHistoryArray->Entry(0);
	CSyncMLHistoryPushMsg * pushEntry = CSyncMLHistoryPushMsg::DynamicCast(&hEntry);
	
	//Just verifying the EntryId
	TL(hEntry.EntryId() == 0);
	
	TL(pushEntry->MsgDigest() == KMsgDigest );
	TL(pushEntry->ResultCode() == KErrorEleven );
	TL(pushEntry->ReceivedCount() == 1 );
	TL(pushEntry->AlertCount() == 1 );
	TL(pushEntry->Profile() == 1 );
		
	const CSmlAlertInfo& alertInfo = pushEntry->Alert(0);
	
	TL(alertInfo.TaskIds().Count() == KTwo);	
	
	const RArray<TInt>& streamedTasks = alertInfo.TaskIds();
	const RArray<TSmlSyncType>& streamedSyncTypes = alertInfo.TaskSyncTypes();
	
	if( streamedTasks.Count() == KTwo )
	{
		TL(streamedTasks[0] == 1);
		TL(streamedTasks[1] == KTwo);
	}	
	if( streamedSyncTypes.Count() == KTwo )
	{
		TL(streamedSyncTypes[0] == 0);	
		TL(streamedSyncTypes[1] == 1);
	}	

	TL(alertInfo.ErrorCode() == KErrorEleven);
	TL(alertInfo.JobControl() == CSmlAlertInfo::ECreateJob);
	TL(alertInfo.CreateSession());
	TL(alertInfo.Profile() == 1);
	TL(alertInfo.Protocol() == ESmlDataSync);
	TL(alertInfo.SessionId() == KTwo);
	TL(alertInfo.Transport() == KUidNSmlMediumTypeInternet.iUid);
	TL(alertInfo.VendorSpecificInfo() == KVendorInfo);
	//end push msg

	//Assinging CSyncMLHistoryEntry to CSyncMLHistoryJob and verifies the values
	CSyncMLHistoryEntry& scEntry = iHistoryArray->Entry(1);
	CSyncMLHistoryJob * jobEntry = CSyncMLHistoryJob::DynamicCast(&scEntry);
	
	//Just verifying the EntryId
	TL(hEntry.EntryId() == 0);
	
	TL(jobEntry->FinishTime() == now);
	TL(jobEntry->ResultCode() == KErrorFive);
	TL(jobEntry->LastSuccessSyncTime() == lastsync);
	TL(jobEntry->Profile() == 1)
	
	TL(jobEntry->TaskCount() == KTwo);
	
	const CSyncMLHistoryJob::TTaskInfo& taskInfo2 = jobEntry->TaskAt(0);
	
	TL(taskInfo2.iError == KErrorOne);
	TL(taskInfo2.iItemsAdded == KFive);
	TL(taskInfo2.iItemsChanged == KFour);
	TL(taskInfo2.iItemsDeleted == KThree);
	TL(taskInfo2.iItemsFailed == KTwo);
	TL(taskInfo2.iItemsMoved == KOne);
	
	TL(taskInfo2.iServerItemsAdded == KOne);
	TL(taskInfo2.iServerItemsChanged == KTwo);
	TL(taskInfo2.iServerItemsDeleted == KThree);
	TL(taskInfo2.iServerItemsFailed == KFour);
	TL(taskInfo2.iServerItemsMoved == KFive);

	//----------------------Just verifying the -------------------
	CSyncMLHistoryEntry *copyObj = CSyncMLHistoryEntry::NewL(scEntry);
	CSyncMLHistoryJob * copyJobEntry = CSyncMLHistoryJob::DynamicCast(copyObj);

	//Comparing the value of EntryType from copyObj and scEntry
	TL(jobEntry->EntryType() == copyJobEntry->EntryType());
	
	return KErrNone;
	}

//  End of File
