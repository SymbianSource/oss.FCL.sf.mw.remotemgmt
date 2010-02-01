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
* Description: This header contains the class for MMC insertion, removal service
*
*/
#ifndef __MMCSERVICE__H_
#define __MMCSERVICE__H_

//System includes
#include <e32base.h>
#include <e32property.h>
#include <UikonInternalPSKeys.h>
#ifndef SYMBIAN_ENABLE_SPLIT_HEADERS
#include <schinfo.h>
#else
#include <schinfo.h>
#include <schinfointernal.h>
#endif

//User includes
#include "DmEventServiceBase.h"

//The publish subscribe key with condition. Do not alter this unless absolutely needed!
const TPSKey KMMCPSKey(KPSUidUikon,KUikMMCInserted);
//Condition to check when MMC is removed
const TPSKeyCondition KMMCPSKeyConditionInsert(KMMCPSKey.iConditionCategory, KMMCPSKey.iConditionKey, 1, TTaskSchedulerCondition::EEquals);
//Condition to check when MMC is present
const TPSKeyCondition KMMCPSKeyConditionRemove(KMMCPSKey.iConditionCategory, KMMCPSKey.iConditionKey, 0, TTaskSchedulerCondition::EEquals);

//The task name for this service. Do not alter this unless absolutely needed!
_LIT(KMmcTaskName, "MmcTask");

/**
 *  Derives the base service and implementes service for MMC card insertion and removal
 *
 *  @since  S60 v5.2
 */
NONSHARABLE_CLASS(CMmcService) : public CDmEventServiceBase
    {

    public:
        /** Two phase construction **/
        static CMmcService* NewL();

        static CMmcService* NewLC();

        /** Virtual destructor **/
        virtual ~CMmcService();

        /**
         * Waits until any MMC activity is complete
         *
         * @since   S60   v5.2
         * @return  None. Can leave with system wide error 
         */
        void WaitForRequestCompleteL();

        /**
         * This functions tells if the publish subscribe key exists or not. It is called
         * before registering the condition schedule. 
         *
         * @since   S60   v5.2
         * @return ETrue if exists, otherwise EFalse
         */
        TBool IsKeyValid();

        /**
         * Returns the task name of the service, which should be unique in this application.
         *
         * @since   S60   v5.2
         * @return task name
         */
        const TDesC& TaskName();
        
        void UpdateMmcStatusL();

    private:
        /** Constructors **/
        CMmcService();

        CMmcService(const TPSKeyCondition& aPSKeyCondition);

    private:
        /** Initializes the members of the class **/
        void ConstructL();
        
        TInt iLookingFor;

    };

#endif // __MMCSERVICE__H_
// End of File
