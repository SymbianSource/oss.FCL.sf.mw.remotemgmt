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
* Description: This header contains the class for Software (un)install service
*
*/
#ifndef __SWAPPLICATIONSERVICE__H_
#define __SWAPPLICATIONSERVICE__H_

// System includes
#include <e32base.h>
#include <swispubsubdefs.h> //Software Installer
#ifndef SYMBIAN_ENABLE_SPLIT_HEADERS
#include <schinfo.h>
#else
#include <schinfo.h>
#include <schinfointernal.h>
#endif
//User includes
#include "DmEventServiceBase.h"

//The publish subscribe key with condition. Do not alter this unless absolutely needed!
const TPSKeyCondition KSwPSKeyCondition(KUidSystemCategory,Swi::KUidSoftwareInstallKey, 0, TTaskSchedulerCondition::EGreaterThan);

//The task name for this service. Do not alter this unless absolutely needed!
_LIT(KSisTaskName, "SwTask");

/**
 *  Derives the base service and implementes service for software installation
 *
 *  @since  S60 v5.2
 */
NONSHARABLE_CLASS(CSwApplicationService) : public CDmEventServiceBase
    {

    public:
        /** Two phase construction **/
        static CSwApplicationService* NewL();

        static CSwApplicationService* NewLC();

        /** Virtual destructor **/
        virtual ~CSwApplicationService();

        /**
         * Waits until any Software operation is complete
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

    private:
        /** Constructors **/
        CSwApplicationService();

        CSwApplicationService(const TPSKeyCondition& aPSKeyCondition);

    private:
        /** Initializes the members of the class **/
        void ConstructL();

        /**
         * Checks if no operation is ongoing
         *
         * @since   S60   v5.2
         * @param   aValue - value of the publish subscribe key
         * @return  ETrue if idle, otherwise EFalse
         */
        TBool IsSwInIdle(TInt aValue);

    };

#endif // __SWAPPLICATIONSERVICE__H_
// End of File
