/*
* Copyright (c) 2005-2006 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:   Handles scheduled update, starts fotaserver and request upd
*
*/



#include <schinfo.h>
#include <schtask.h>
#include <s32file.h>
#include <e32math.h>
#include <e32cons.h>

#include "FotaSchedDebug.h"
#include "fotaengine.h"
#include "../../inc/FotaIPCTypes.h"

// Constants

// -----------------------------------------------------------------------------
// UpdateFirmwareL
//
// -----------------------------------------------------------------------------
//
LOCAL_D void UpdateFirmwareL(RFile& aTaskFile)
	{
    FLOG(_L("fotaschedulehandler UpdateFirmwareL(RFile& aTaskFile)"));
    TInt err;
    CFileStore*         store;
    RStoreReadStream    instream;
    // Get tasks from scheduler's store
    store = CDirectFileStore::FromLC(aTaskFile);
    instream.OpenLC(*store,store->Root());
    TInt count = instream.ReadInt32L();
    FLOG(_L("fotaschedulehandler  task count  %d"),count );
	for (TInt i=0;i<count;i++)
        {
		CScheduledTask* task = CScheduledTask::NewLC(instream);
        HBufC* b = const_cast<HBufC*>(&(task->Data()));
        TPtr ptr = b->Des();
        HBufC8* b8 = HBufC8::NewLC( b->Length() );
        b8->Des().Copy(ptr);
        TFotaScheduledUpdate fotareminder(-1,-1);
        TPckg<TFotaScheduledUpdate>   fotareminderpkg(fotareminder);
        fotareminderpkg.Copy(b8->Des());
        CleanupStack::PopAndDestroy( b8 );
        FLOG(_L("fotaschedulehandler packageid:%d  sched:%d"),fotareminder.iPkgId, fotareminder.iScheduleId);
        RFotaEngineSession fota;
        fota.OpenL();
        CleanupClosePushL( fota );
        err = fota.ScheduledUpdateL (fotareminder);
        if(err){} // to remove compiler warning
        FLOG(_L("fotaschedulehandler       2........ err %d"),err);
        fota.Close();
        FLOG(_L("fotaschedulehandler       3 "));
        CleanupStack::PopAndDestroy( &fota); 
		CleanupStack::PopAndDestroy(task);
        }
	CleanupStack::PopAndDestroy( &instream ); 
	CleanupStack::PopAndDestroy( store ); 
	}


// -----------------------------------------------------------------------------
// ExecuteL
//
// -----------------------------------------------------------------------------
//
LOCAL_D TInt ExecuteL()
	{
	TInt err = KErrNoMemory;
		
	RFile file;
		
	// Adopt the task file from the Task Scheduler
	err = file.AdoptFromCreator(TScheduledTaskFile::FsHandleIndex(),
									TScheduledTaskFile::FileHandleIndex());
    FLOG(_L("  err  %d"),err );
	User::LeaveIfError(err);
	
	TRAP(err, UpdateFirmwareL(file));
	
	file.Close();
	
	User::LeaveIfError(err);
	
	return err;
	}


// -----------------------------------------------------------------------------
// Execute
//
// -----------------------------------------------------------------------------
//
LOCAL_D TInt Execute()
	{
    FLOG(_L("fotaschedulehandler Execute()"));
	TInt err = KErrNoMemory;
	
	// construct and install active scheduler
	CActiveScheduler* scheduler = new CActiveScheduler;
	if (!scheduler)
		{
		return err;
		}
	CActiveScheduler::Install(scheduler);
	CTrapCleanup* cleanupStack = CTrapCleanup::New();
	if (cleanupStack)
		{
        TRAP(err, ExecuteL());
		delete cleanupStack;
		}
    delete scheduler;
	return err;
	}


// -----------------------------------------------------------------------------
// E32Main
//
// -----------------------------------------------------------------------------
//
GLDEF_C TInt E32Main()
	{
	return Execute();
	}



