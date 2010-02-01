/*
 * Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
 * Description: This is the entry point of the application
 *
 */

//System include
#include <schinfo.h>
#include <schtask.h>
#include <s32file.h>
#include <e32math.h>
#include <e32cons.h>
#include <e32property.h>

//User include
#include "DmEventScheduler.h"
#include "DmEventNotifierDebug.h"

// -----------------------------------------------------------------------------
// InvokeNotifiersL - This function is called to notify servers on any event
// -----------------------------------------------------------------------------
LOCAL_D void InvokeNotifiersL(RFile& aTaskFile)
    {
    _DMEVNT_DEBUG(_L("DmEventHandler InvokeNotifiersL >>"));

    CFileStore*         store;
    RStoreReadStream    instream;
    // Get tasks from scheduler's store
    store = CDirectFileStore::FromLC(aTaskFile);
    instream.OpenLC(*store,store->Root());
    TInt count = instream.ReadInt32L();
    _DMEVNT_DEBUG(_L("SwInstallhandler  task count  %d"),count );

    CScheduledTask* task = CScheduledTask::NewLC(instream);
    HBufC* b = const_cast<HBufC*>(&(task->Data()));
    TPtr ptr = b->Des();
    TName taskname(ptr);

    CleanupStack::PopAndDestroy (task);
    CleanupStack::PopAndDestroy( &instream );
    CleanupStack::PopAndDestroy( store );


    _DMEVNT_DEBUG(_L("Waiting for completion..."));
    CDmEventScheduler* sch = CDmEventScheduler::NewLC();
    sch->WaitAndCreateConditionScheduleL(taskname);
    CleanupStack::PopAndDestroy(sch);

    _DMEVNT_DEBUG(_L("DmEventHandler InvokeNotifiersL <<"));
    }


// -----------------------------------------------------------------------------
// ExecuteL - This function is called to check if check the purpose of invocation
//  of this executable
// -----------------------------------------------------------------------------
LOCAL_D TInt ExecuteL()
    {
    _DMEVNT_DEBUG(_L("DmEventHandler ExecuteL...>>"));
    TInt err = KErrNoMemory;

    RFile file;

    // Adopt the task file from the Task Scheduler
    err = file.AdoptFromCreator(TScheduledTaskFile::FsHandleIndex(),
            TScheduledTaskFile::FileHandleIndex());
    _DMEVNT_DEBUG(_L("  err  %d"),err );


    if (KErrNone == err)
        {
        TRAP(err, InvokeNotifiersL(file));
        _DMEVNT_DEBUG(_L("Error = %d"), err);
        }
    else if (KErrNotFound == err)
        {
        //If err =-1, certain that there is no task scheduled. So, start and register the condition schedule
        CDmEventScheduler* sch = CDmEventScheduler::NewLC();
        sch->CreateConditionScheduleL();
        CleanupStack::PopAndDestroy(sch);
        }
    else
        __LEAVE_IF_ERROR(err);

    file.Close();

    _DMEVNT_DEBUG(_L("DmEventHandler ExecuteL...<<"));
    return err;
    }


// -----------------------------------------------------------------------------
// Execute - This is called by the entry point
// -----------------------------------------------------------------------------
LOCAL_D TInt Execute()
    {
    __UHEAP_MARK;
    _DMEVNT_DEBUG(_L("DmEventHandler Execute() >>"));

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
        delete cleanupStack; cleanupStack = NULL;
        }
    delete scheduler; scheduler = NULL;
    
    _DMEVNT_DEBUG(_L("DmEventHandler Execute() <<"));
    __UHEAP_MARKEND;
    return err;
    }


// -----------------------------------------------------------------------------
// E32Main - The entry point of this executable
// -----------------------------------------------------------------------------
GLDEF_C TInt E32Main()
    {
    return Execute();
    }
// End of File
