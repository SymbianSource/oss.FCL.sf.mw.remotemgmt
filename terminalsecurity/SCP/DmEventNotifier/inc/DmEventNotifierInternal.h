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
* Description: This header contains all the common constants that are used by the application.
*
*/
#ifndef __DMEVENTNOTIFIERINTERNAL_H__
#define __DMEVENTNOTIFIERINTERNAL_H__


//System Includes
#include <e32std.h>
#include <e32base.h>
#ifndef SYMBIAN_ENABLE_SPLIT_HEADERS
#include <schinfo.h>
#else
#include <schinfo.h>
#include <schinfointernal.h>
#endif

//User includes
#include "DmEventNotifierCRKeys.h"

//Defines
#define __LEAVE_IF_ERROR(x) if(KErrNone!=x) {_DMEVNT_DEBUG(_L("LEAVE in %s: %d"), __FILE__, __LINE__); User::Leave(x); }

//Size of the schedule condition array
#define KConditionArraySize 1

//Max size of the schedule condition array
#define KConditionArraySizeMax 5

//Max size of the schedule reference
#define KScheduleReferenceMax 5

//Max size of the task array
#define KTaskArraySizeMax 5

//Max number of services present
#define KMaxServices 3

//Name of the executable
_LIT(KDmEventNotifierExe, "dmeventnotifier.exe");

//A Type to hold the conditions
typedef CArrayFixFlat<TTaskSchedulerCondition>  CSchConditionArray;

//Structure to hold one publish subscribe key values
struct TPSKey 
    {
    //Member variables
    const TUid    iConditionCategory;   //Holds the publish subscribe category
    const TUint   iConditionKey;        //Holds the publish subscribe key

    //Member functions
    TPSKey() : iConditionCategory(KNullUid), iConditionKey(KErrNone) {}

    TPSKey(TUid aConditionCategory, TUint aConditionKey): iConditionCategory(aConditionCategory),iConditionKey(aConditionKey){};

    TBool operator == (TPSKey aKey)
        {
        return (iConditionCategory == aKey.iConditionCategory && iConditionKey == aKey.iConditionKey);
        }
    };

//Structure to hold one publish subscribe key along with the expiry condition 
struct TPSKeyCondition
    {
    //Member variables
    const TPSKey  iPskey;       //Holds the publish subscribe key
    const TInt    iSchState;    //The value to be checked against
    const TTaskSchedulerCondition::TConditionType iSchType; //The condition on the value

    //Member Functions
    TPSKeyCondition() : iSchState(KErrNone), iSchType(TTaskSchedulerCondition::EEquals) {};

    TPSKeyCondition( TUid aConditionCategory, TUint aConditionKey, TInt aState, TTaskSchedulerCondition::TConditionType aSchType) : iPskey(aConditionCategory,aConditionKey), iSchState(aState), iSchType(aSchType){};

    TPSKeyCondition(const TPSKeyCondition& aPsKeyCondition) : iPskey(aPsKeyCondition.iPskey), iSchState(aPsKeyCondition.iSchState),iSchType(aPsKeyCondition.iSchType) {};

    };

#endif // __DMEVENTNOTIFIERINTERNAL_H__
