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
* Description: This header contains the base class that each service derives.
*
*/
#ifndef __DMEVENTSERVICEBASE_H__
#define __DMEVENTSERVICEBASE_H__

//User include
#include "DmEventNotifierCommon.h"
#include "DmEventNotifierInternal.h"

/**
 *  The base class which defines the interface for a service
 *
 *  @since  S60 v5.2
 */
class CDmEventServiceBase : public CBase
    {
public:

    /**
     * Returns the publish subscribe key with the condition
     *
     * @since   S60   v5.2
     * @return P/S key with condition 
     */
    TPSKeyCondition GetPSKeyCondition() { return iPSKeyCondition;}

    /**
     * This function will wait until a service operation is complete. For example, during software uninstallation
     * this function returns only when the uninstallation is complete.
     *
     * @since   S60   v5.2
     * @return None. Can leave with system wide error
     */

    virtual void WaitForRequestCompleteL() = 0;

    /**
     * This functions tells if the publish subscribe key exists or not. It is called
     * before registering the condition schedule. 
     *
     * @since   S60   v5.2
     * @return ETrue if exists, otherwise EFalse
     */
    virtual TBool IsKeyValid() = 0;

    /**
     * Returns the task name of the service, which should be unique in this application.
     *
     * @since   S60   v5.2
     * @return task name
     */
    virtual const TDesC& TaskName() = 0;

    /**
     * Gets the service id and operation
     *
     * @since   S60   v5.2
     * @return task name
     */
    void GetServiceIdAndOperation(THandlerServiceId& aService, THandlerOperation& aOperation )
        {
        aService = iServiceId;
        aOperation = iOperation;
        }
protected:

    /**
     * The Constructor
     *
     * @since   S60   v5.2
     * @param   aPsKeyCondition - which holds the publish subscribe key detail and condition
     * @return  the object 
     */
    CDmEventServiceBase(const TPSKeyCondition& aPsKeyCondition, THandlerServiceId aServiceId): 
    iPSKeyCondition(aPsKeyCondition), iServiceId(aServiceId), iOperation(ENoOpn){};

protected:

    /**
     * The service
     */
    THandlerServiceId iServiceId;

    /**
     * The operation in the service
     */
    THandlerOperation iOperation;


private:

    /**
     * Holds the publish subscribe key detail and condition
     */
    const TPSKeyCondition iPSKeyCondition;

    };

#endif /* __DMEVENTSERVICEBASE_H__ */
// End of File
