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
* Description: This header contains the class which creates the condition schedule.
*
*/
#ifndef __DMEVENTSCHEDULER_H__
#define __DMEVENTSCHEDULER_H__

// System Includes
#include <e32base.h>
#include <e32def.h>
#include <csch_cli.h> // RScheduler
#include <schinfo.h>  // TSchedulerItemRef, TTaskInfo... 

//User includes
#include "DmEventServiceBase.h"
#include "DmEventNotifierCommon.h"
#include "DmEventNotifierInternal.h"

const TUid KAppDmEventNotifierUid = TUid::Uid(KAppUidDmEventNotifier); //UID3 from .mmp file

//Forward declarations
class CSwInstallObserver;

/**
 * Install scheduler is responsible for creating the condition schedules and notifying server
 * when they expire
 *
 *  @since  S60 v5.2
 */
NONSHARABLE_CLASS(CDmEventScheduler) : public CBase
    {

    public:

        /** Two phase construction **/
        static CDmEventScheduler* NewL();

        static CDmEventScheduler* NewLC();

        /** Destructor **/
        virtual ~CDmEventScheduler();

        /** Member functions **/
        /**
         * This function creates all the condition schedule required by the services
         *
         * @since   S60   v5.2
         * @param   aService - the service to be scheduled. If not specified all will be
         * scheduled
         * @return  None. Can leave with system wide errors  
         */
        void CreateConditionScheduleL(CDmEventServiceBase* aService = NULL);

        /**
         * This function creates one condition schedule with task for a service
         *
         * @since   S60   v5.2
         * @param   aSchedulerItem  - A unique id for the schedule
         * @param   aScheduler      - Task schedule client side interface
         * @param   aDefaultRunTime - Task schedule time
         * @param   aNewTaskId      - The new task in after it is created
         * @param   aConditions     - The condition array
         * @param   aService        - The service base pointer
         * @return  KErrNone or system wide error
         */
        TInt CreateConditionScheduleWithTaskL (TSchedulerItemRef& aSchedulerItem, 
                RScheduler& aScheduler,
                const TTsTime& aDefaultRunTime, 
                TInt& aNewTaskId,
                CArrayFixFlat<TTaskSchedulerCondition>* &aConditions,
                CDmEventServiceBase* aService);

        /**
         * This function will wait until a service operation is complete. For example, during software uninstallation
         * this function returns only when the uninstallation is complete.
         *
         * @since   S60   v5.2
         * @param   aTaskName - Name of the task
         * @return None. Can leave with system wide error
         */
        void WaitAndCreateConditionScheduleL(TName& aTaskName);

    private:

        /**
         * Default constructor
         *
         * @since   S60   v5.2
         * @param   NOne
         * @return 
         */
        CDmEventScheduler();

        /**
         * Constructs member variables
         *
         * @since   S60   v5.2
         * @return None. Can leave with system wide error.
         */
        void ConstructL();

        /**
         * Deletes any outstanding task and schedule
         *
         * @since   S60   v5.2
         * @return  None. Can leave with system wide error
         */
        void DeleteScheduleL();

        /**
         * Creates the condition array for the provided service
         *
         * @since   S60   v5.2
         * @param   aService - the service for which condition array is to be created
         * @return  Condition array. Can leave with system wide errors. 
         */
        CSchConditionArray* CreateConditionLC(CDmEventServiceBase* aService);

        /**
         * Finds the expired service when scheduler triggers it.
         *
         * @since   S60   v5.2
         * @param   aTaskName - the task name of the expired schedule
         * @return  base class pointer of the service
         */
        CDmEventServiceBase* FindExpiredService(TName& aTaskName);

        /**
         * This functions notifies other registered servers when a service expires
         *
         * @since   S60   v5.2
         * @param   aServiceId - the service that has expired
         * @param   aOpn - the operation the service has undergone
         * @return  None. Can leave with system wide errors 
         */
        void NotifyRegisteredServersL(THandlerServiceId aServiceId, THandlerOperation aOpn);

        /**
         * Checks whether the schedule is already created or not
         *
         * @since   S60   v5.2
         * @return  ETrue if schedule is already created; otherwise EFalse. Can leave
         * with system wide errors.
         */
        TBool IsScheduleEnabledL();

        /**
         * Once schedule is created successfully, this function is called to set the
         * schedule as enabled. otherwise it is called with disabled.
         *
         * @since   S60   v5.2
         * @param   Status of the schedule
         * @return  None. System wide errors 
         */
        void SetScheduleEnabledL(TBool aValue);

    private:

        /**
         * The client side symbian scheduler
         */
        RScheduler iScheduler;

        /**
         * The scheduler item reference
         */
        TSchedulerItemRef iConditionScheduleHandle;

        /**
         * Array to hold the services objects
         */
        TFixedArray<CDmEventServiceBase*, KMaxServices> iServices;

    };

#endif // __DMEVENTSCHEDULER_H__
