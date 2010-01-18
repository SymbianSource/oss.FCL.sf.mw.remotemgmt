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
 * Description: This class implements the condition based schedule.
 *
 */
//System includes
#include <eikenv.h>
#include <e32property.h>
#include <centralrepository.h>

//User includes
#include "DmEventScheduler.h"
#include "DmEventNotifierCommon.h"
#include "DmEventNotifierInternal.h"
#include "SwApplicationService.h"   //Software service
#include "JavaApplicationService.h" //Java service
#include "MMCService.h"             //MMC service
#include "DmEventNotifierDebug.h"

#include "SCPEventHandler.h" // dll for event handling (viz: uninstallation)

// ====================== MEMBER FUNCTIONS ===================================

// ---------------------------------------------------------------------------
// CDmEventScheduler::CDmEventScheduler()
// ---------------------------------------------------------------------------
CDmEventScheduler::CDmEventScheduler()
    {
    }

// ---------------------------------------------------------------------------
// CDmEventScheduler::~CDmEventScheduler()
// ---------------------------------------------------------------------------
CDmEventScheduler::~CDmEventScheduler()
    {
    FLOG(_L("CDmEventScheduler::~CDmEventScheduler >>"));

    iScheduler.Close();

    iServices.DeleteAll();
    iServices.Reset();

    FLOG(_L("CDmEventScheduler::~CDmEventScheduler <<"));
    }


// ---------------------------------------------------------------------------
// CDmEventScheduler::NewL()
// ---------------------------------------------------------------------------
CDmEventScheduler* CDmEventScheduler::NewL()
    { 
    FLOG(_L("CDmEventScheduler::NewL >>"));

    CDmEventScheduler* self = new (ELeave) CDmEventScheduler();
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop();

    FLOG(_L("CDmEventScheduler::NewL <<"));
    return self;
    }

CDmEventScheduler* CDmEventScheduler::NewLC()
    {
    FLOG(_L("CDmEventScheduler::NewLC >>"));

    CDmEventScheduler* self = CDmEventScheduler::NewL();
    CleanupStack::PushL(self);

    FLOG(_L("CDmEventScheduler::NewLC <<"));
    return self;
    }

// ---------------------------------------------------------------------------
// CDmEventScheduler::ConstructL()
// ---------------------------------------------------------------------------
void CDmEventScheduler::ConstructL()
    {
    FLOG(_L("CDmEventScheduler::ConstructL >>"));

    __LEAVE_IF_ERROR(iScheduler.Connect());

    TFileName handler(KDmEventNotifierExe);

    User::LeaveIfError(iScheduler.Register(handler, CActive::EPriorityStandard));

    iServices[ESoftwareService] = CSwApplicationService::NewL();

    iServices[EJavaService] = CJavaApplicationService::NewL();

    iServices[EMmcService] = CMmcService::NewL();

    FLOG(_L("CDmEventScheduler::ConstructL <<"));
    }

// ---------------------------------------------------------------------------
// CDmEventScheduler::CreateConditionScheduleL()
// ---------------------------------------------------------------------------
void CDmEventScheduler::CreateConditionScheduleL(CDmEventServiceBase* aService)
    {
    FLOG(_L("CDmEventScheduler::CreateConditionSchedule >>"));

    //Create new schedule
    TInt err(KErrNone);
    TBool schenabled (EFalse);
    TSchedulerItemRef conditionScheduleHandle;
    TTsTime defaultTime;
    TTime time;
    time.HomeTime();
    time = time.operator+(TTimeIntervalYears(10)); //Schedule will automatically expire after 10 years!
    defaultTime.SetLocalTime( time );
    TInt newConditionTaskId = -1;

    if (aService == NULL)
        {

        TRAP(err, DeleteScheduleL());
        FLOG(_L("Deleted existing schedule, err = %d"),err);


        //Performing for Software Installer
        FLOG(_L("Registering for Software Installer..."));
        if (iServices[ESoftwareService]->IsKeyValid())
            {
            CSchConditionArray* aConditions;

            aConditions = CreateConditionLC(iServices[ESoftwareService]);

            err = CreateConditionScheduleWithTaskL(iConditionScheduleHandle,
                    iScheduler,  defaultTime, newConditionTaskId, aConditions, iServices[ESoftwareService] );

            schenabled = (KErrNone == err);
            CleanupStack::PopAndDestroy(); //aConditions
            }
        else
            {
            FLOG(_L("Can't read PS key, hence not registering!"));
            }
        //Performing for Java Installer
        FLOG(_L("Registering for Java Installer..."));
        if (iServices[EJavaService]->IsKeyValid())
            {
            CSchConditionArray* aConditions;

            aConditions = CreateConditionLC(iServices[EJavaService]);

            err = CreateConditionScheduleWithTaskL(iConditionScheduleHandle,
                    iScheduler,  defaultTime, newConditionTaskId, aConditions, iServices[EJavaService] );

            schenabled = (KErrNone == err);
            CleanupStack::PopAndDestroy(); //aConditions
            }
        else
            {
            FLOG(_L("Can't read PS key, hence not registering!"));
            }

        //Performing for Mmc observer    
        FLOG(_L("Registering for Mmc observer..."));
        if (iServices[EMmcService]->IsKeyValid())
            {
            CSchConditionArray* aConditions;

            aConditions = CreateConditionLC(iServices[EMmcService]);

            err = CreateConditionScheduleWithTaskL(iConditionScheduleHandle,
                    iScheduler,  defaultTime, newConditionTaskId, aConditions, iServices[EMmcService] );

            schenabled = (KErrNone == err);
            CleanupStack::PopAndDestroy(); //aConditions
            
            if (err == KErrNone)
                {
                //Set cenrep value for mmc accordingly
                
                CMmcService *ptr = (CMmcService *) iServices[EMmcService];
                ptr->UpdateMmcStatus();
                }
            }
        else
            {
            FLOG(_L("Can't read PS key, hence not registering!"));
            }
        }
    else
        {
        FLOG(_L("Scheduling for only the expired service"));
        CSchConditionArray* aConditions;

        aConditions = CreateConditionLC(aService);

        err = CreateConditionScheduleWithTaskL(iConditionScheduleHandle,
                iScheduler,  defaultTime, newConditionTaskId, aConditions, aService );

        CleanupStack::PopAndDestroy(); //aConditions
        
        schenabled = (KErrNone == err);
        }
    
    SetScheduleEnabledL(schenabled);
    
    FLOG(_L("CDmEventScheduler::CreateConditionSchedule, error = %d <<"), err);
    }

// ---------------------------------------------------------------------------
// CDmEventScheduler::WaitAndCreateConditionScheduleL()
// ---------------------------------------------------------------------------
void CDmEventScheduler::WaitAndCreateConditionScheduleL(TName aTaskName)
    {
    FLOG(_L("CDmEventScheduler::WaitAndCreateConditionScheduleL >>"));

    SetScheduleEnabledL(EHandlerNotRegistered);

    CDmEventServiceBase* service = FindExpiredService(aTaskName);
 //   CleanupStack::PushL(service); // Not necessary, as service pointer is changed in the code

    if ( service )
        {
        service->WaitForRequestCompleteL();
        }

    //Read the status from aService
    THandlerServiceId srvid;
    THandlerOperation opn;

		if( service )
		{
    	service->GetServiceIdAndOperation(srvid,opn);
    }

    TBool mmcservice (EFalse);
    if (service == iServices[EMmcService])
        {
        mmcservice = ETrue;
        }

    if (iServices[EMmcService])
        {
        delete iServices[EMmcService]; iServices[EMmcService] = NULL;
        iServices[EMmcService] = CMmcService::NewL();
        }

    if (mmcservice)
        {
        service = iServices[EMmcService];
        }
    CreateConditionScheduleL(service);

    if ( opn != ENoOpn )
        {
        TRAPD(err, NotifyRegisteredServersL(srvid, opn));
        FLOG(_L("Notification error = %d"), err);
        }
    else
        {
        FLOG(_L("Operation got cancelled. Skipping notification"));
        }

   // CleanupStack::Pop(service); //don't destroy as object is not owned

    FLOG(_L("CDmEventScheduler::WaitAndCreateConditionScheduleL <<"));
    }

// ---------------------------------------------------------------------------
// CDmEventScheduler::CreateConditionScheduleWithTaskL()
// ---------------------------------------------------------------------------
TInt CDmEventScheduler::CreateConditionScheduleWithTaskL(
        TSchedulerItemRef& aSchedulerItem, 
        RScheduler& aScheduler,
        const TTsTime& aDefaultRunTime,
        TInt& aNewTaskId,
        CArrayFixFlat<TTaskSchedulerCondition>* &aConditions,
        CDmEventServiceBase* aService)
    {
    FLOG(_L("CDmEventScheduler::CreateConditionScheduleWithTaskL >>"));

    TInt ret(KErrNone);

    aSchedulerItem.iName = KAppDmEventNotifierUid.Name();


    ret = aScheduler.CreatePersistentSchedule(aSchedulerItem,
            *aConditions, aDefaultRunTime);

    if (KErrNone == ret)
        {
        TTaskInfo taskInfo;
        taskInfo.iTaskId = 0;
        taskInfo.iName = aService->TaskName();
        taskInfo.iPriority = 2;
        taskInfo.iRepeat = 0;
        HBufC* data = aService->TaskName().AllocLC();

        ret = aScheduler.ScheduleTask(taskInfo, *data, aSchedulerItem.iHandle);
        aNewTaskId = taskInfo.iTaskId;

        CleanupStack::PopAndDestroy(); // data
        }
    else
        {
        //Creating persistent schedule failed, do something...
        FLOG(_L("Creation of persistent scheduled failed, error = %d"), ret);
        }

    FLOG(_L("CDmEventScheduler::CreateConditionScheduleWithTaskL, ret = %d <<"), ret);
    return ret;
    }

// ---------------------------------------------------------------------------
// CDmEventScheduler::DeleteScheduleL
// ---------------------------------------------------------------------------
void CDmEventScheduler::DeleteScheduleL()
    {
    FLOG(_L("CDmEventScheduler::DeleteScheduleL >>"));

    RScheduler                              sc;
    CArrayFixFlat<TSchedulerItemRef>*       aSchRefArray = new CArrayFixFlat <TSchedulerItemRef>(KScheduleReferenceMax);
    TScheduleFilter                         aFilter(EAllSchedules);
    User::LeaveIfError( sc.Connect() );
    CleanupClosePushL( sc );
    CleanupStack::PushL(aSchRefArray);

    User::LeaveIfError( sc.GetScheduleRefsL( *aSchRefArray,aFilter) );
    FLOG(_L("Schedule items: "));
    for ( TInt i=0; i<aSchRefArray->Count(); ++i  )
        {
        TSchedulerItemRef it = (*aSchRefArray)[i];
        if ( it.iName == KAppDmEventNotifierUid.Name()  )
            {
            TScheduleState2 sc_state;
            CSchConditionArray* sc_entries  = new CSchConditionArray (KConditionArraySizeMax); 
            TTsTime sc_time;
            CArrayFixFlat<TTaskInfo>* sc_tasks  = new CArrayFixFlat <TTaskInfo>(KTaskArraySizeMax);

            CleanupStack::PushL( sc_entries );
            CleanupStack::PushL( sc_tasks );
            FLOG (_L("%d. schedule handle: %d name:'%S'"),i,it.iHandle, &(it.iName) );

            TInt err = sc.GetScheduleL ( it.iHandle , sc_state, *sc_entries, sc_time, *sc_tasks);

            if (KErrNone == err) 
                {
                for ( TInt j=0; j<sc_tasks->Count();++j)
                    {
                    TTaskInfo sc_task = (*sc_tasks)[j];
                    FLOG(_L("         schedule task  %d  '%S'"),sc_task.iTaskId,&(sc_task.iName) );

                    err = sc.DeleteTask(sc_task.iTaskId);
                    FLOG(_L("Deleted task state, error = %d"), err);
                    }
                }
            else
                {
                FLOG(_L("Getting schedule error = %d"), err);
                }

            err = sc.DeleteSchedule(it.iHandle );
            FLOG(_L("Deleted schedule %d, error = %d"),i+1, err);

            CleanupStack::PopAndDestroy( sc_tasks );
            CleanupStack::PopAndDestroy( sc_entries );
            }
        }
    CleanupStack::PopAndDestroy( aSchRefArray );
    CleanupStack::PopAndDestroy(&sc);
    FLOG(_L("CDmEventScheduler::DeleteScheduleL <<"));
    }

// ---------------------------------------------------------------------------
// CDmEventScheduler::CreateConditionLC
// ---------------------------------------------------------------------------
CSchConditionArray* CDmEventScheduler::CreateConditionLC(CDmEventServiceBase* aService)
    {
    FLOG(_L("CDmEventScheduler::CreateConditionLC >>"));

    CSchConditionArray* conditions = new (ELeave) CSchConditionArray(KConditionArraySize);
    CleanupStack::PushL(conditions); //To be poped/destroyed by the caller

    TTaskSchedulerCondition Condition;
    TPSKeyCondition pskeycondition = aService->GetPSKeyCondition();
    Condition.iCategory = pskeycondition.iPskey.iConditionCategory;
    Condition.iKey      = pskeycondition.iPskey.iConditionKey;
    Condition.iState    = pskeycondition.iSchState;
    Condition.iType     = pskeycondition.iSchType;

    conditions->AppendL(Condition);

    FLOG(_L("CDmEventScheduler::CreateConditionLC <<"));
    return conditions;
    }

// ---------------------------------------------------------------------------
// CDmEventScheduler::FindExpiredService
// ---------------------------------------------------------------------------
CDmEventServiceBase* CDmEventScheduler::FindExpiredService(TName& aTaskName)
    {
    FLOG(_L("CDmEventScheduler::FindExpiredService >>"));
    CDmEventServiceBase* ret (NULL);
    if (iServices[ESoftwareService]->TaskName().Compare(aTaskName) == KErrNone)
        {
        FLOG(_L("SW Operation detected..."))
        ret = iServices[ESoftwareService];
        }
    else if (iServices[EJavaService]->TaskName().Compare(aTaskName) == KErrNone)
        {
        FLOG(_L("Java Inst Operation detected..."))
        ret = iServices[EJavaService];
        }
    else if (iServices[EMmcService]->TaskName().Compare(aTaskName) == KErrNone)
        {
        FLOG(_L("MMC Operation detected..."))
        ret = iServices[EMmcService];
        }
    else
        {
        //Should not land here...
        FLOG(_L("Unknown trigger"));
        }

    FLOG(_L("CDmEventScheduler::FindExpiredService, ret = %X <<"), ret);
    return ret;
    }

// ---------------------------------------------------------------------------
// CDmEventScheduler::NotifyRegisteredServicesL
// ---------------------------------------------------------------------------
void CDmEventScheduler::NotifyRegisteredServersL(THandlerServiceId aServiceId, THandlerOperation aOpn)
    {
    FLOG(_L("CDmEventScheduler::NotifyRegisteredServersL, serviceid = %d, operation = %d >>"), aServiceId, aOpn);
    //Read whome to notify...
    CRepository* cenrep (NULL);
    cenrep = CRepository::NewLC( KAppDmEventNotifierUid );
    TInt notifyscp (KErrNotFound);
    TInt error = cenrep->Get(KDmEventNotifierEnabled, notifyscp);

    TInt notifyam (KErrNotFound);
    error = cenrep->Get(KSCPNotifyEvent,notifyam);

    CleanupStack::PopAndDestroy(cenrep);

    if (notifyscp)
        {
            FLOG(_L("CDmEventScheduler::NotifyRegisteredServersL: Invocation of SCPEventHandler"));
            TRAPD ( err, {
            		CSCPEventHandler* handler = CSCPEventHandler::NewL();
            		handler->NotifyChangesL(aServiceId, aOpn);
            		delete handler; 
            		handler = NULL;
            	});
	            FLOG(_L("CDmEventScheduler::NotifyRegisteredServersL: SCPEventHandler completed with err: %d"), err);
				}
    if (notifyam)
        {
        //Notify AM server on the happened operation
        }

    FLOG(_L("CDmEventScheduler::NotifyRegisteredServersL notifyscp = %d, notifyam = %d<<"),notifyscp,notifyam);
    }

// ---------------------------------------------------------------------------
// CDmEventScheduler::IsScneduleEnabledL
// ---------------------------------------------------------------------------
TBool CDmEventScheduler::IsScheduleEnabledL()
    {
    FLOG(_L("CDmEventScheduler::IsScheduleEnabledL >>"));
    CRepository* cenrep (NULL);
    TInt error (KErrNone);
    TInt value (KErrNotFound);

    TRAP(error, cenrep = CRepository::NewL( KAppDmEventNotifierUid ));

    FLOG(_L("Cenrep file read status = %d"), error);
    User::LeaveIfError(error);
    CleanupStack::PushL(cenrep);

    error = cenrep->Get(KDmEventNotifierEnabled, value);

    CleanupStack::PopAndDestroy(cenrep);

    FLOG(_L("CDmEventScheduler::IsScheduleEnabledL, value = %d, error = %d <<"),value, error);
    return (value == EHandlerRegistered);
    }

// ---------------------------------------------------------------------------
// CDmEventScheduler::SetScheduleEnabledL
// ---------------------------------------------------------------------------
void CDmEventScheduler::SetScheduleEnabledL(TBool aValue)
    {
    FLOG(_L("CDmEventScheduler::SetScheduleEnabledL, aValue = %d >>"), aValue);

    CRepository* cenrep (NULL);
    TInt error (KErrNone);

    cenrep = CRepository::NewL( KAppDmEventNotifierUid );
    CleanupStack::PushL(cenrep);

    error = cenrep->Set(KDmEventNotifierEnabled, aValue);

    CleanupStack::PopAndDestroy(cenrep);

    FLOG(_L("CDmEventScheduler::SetScheduleEnabledL, error = %d <<"),error);
    }
//End of file
