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
* Description: This header contains the class for Java (un)install service
*
*/
#ifndef __JAVAPPLICATIONSERVICE__H_
#define __JAVAPPLICATIONSERVICE__H_

// System Includes
#include <e32base.h>
#include <e32property.h>    //Publish subscribe key
//Commented to avoid app layer dependency
//#include <javadomainpskeys.h> //Java installer
//Comment ends here
#include <swispubsubdefs.h> //Java Installer defined here
#ifndef SYMBIAN_ENABLE_SPLIT_HEADERS
#include <schinfo.h>
#else
#include <schinfo.h>
#include <schinfointernal.h>
#endif

//User includes
#include "DmEventServiceBase.h"

//Constants for java installer - done to avoid app layer dependency
#define KPSUidJavaLatestInstallation 0x10282567

//The publish subscribe key with condition. Do not alter this unless absolutely needed!
const TPSKeyCondition KJavaPSKeyCondition(KUidSystemCategory,KPSUidJavaLatestInstallation, 0, TTaskSchedulerCondition::EGreaterThan);

//The task name for this service. Do not alter this unless absolutely needed!
_LIT(KJavaTaskName, "JavaTask");

/**
 *  Derives the base service and implementes service for Java installation
 *
 *  @since  S60 v5.2
 */
NONSHARABLE_CLASS(CJavaApplicationService) : public CDmEventServiceBase
    {

    public:

        /** Two phase construction **/
        static CJavaApplicationService* NewL();

        static CJavaApplicationService* NewLC();

        /** Virtual destructor **/
        virtual ~CJavaApplicationService();

        /**
         * Waits until any Java operation is complete
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
        CJavaApplicationService();

        CJavaApplicationService(const TPSKeyCondition& aPSKeyCondition);

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

#endif // __JAVAPPLICATIONSERVICE__H_
// End of File
