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
 * Description: This class implements the MMC insertion, removal service
 *
 */

//System includes
#include <centralrepository.h>
//User includes
#include "MMCService.h"
#include "DmEventNotifierDebug.h"

// ====================== MEMBER FUNCTIONS ===================================

// ---------------------------------------------------------------------------
// CMmcService::NewL
// ---------------------------------------------------------------------------
CMmcService* CMmcService::NewL()
    {
    FLOG(_L("CMmcService::NewL >>"));

    TInt value (KErrNotFound);

    RProperty::Get(KMMCPSKey.iConditionCategory, KMMCPSKey.iConditionKey, value);
    CMmcService* self (NULL);
    if(value == 0)
        {
        FLOG(_L("MMC is removed, hence setting for insertion..."));
        self = new (ELeave) CMmcService(KMMCPSKeyConditionInsert);
        }
    else //(value == 1)
        {
        FLOG(_L("MMC is inserted, hence setting for removal..."))
        self = new (ELeave) CMmcService(KMMCPSKeyConditionRemove);
        }

    CleanupStack::PushL(self);

    self->ConstructL();
    self->iLookingFor = value;
    CleanupStack::Pop(self);
    FLOG(_L("CMmcService::NewL <<"));
    return self;
    }

// ---------------------------------------------------------------------------
// CMmcService::NewLC
// ---------------------------------------------------------------------------
CMmcService* CMmcService::NewLC()
    {
    FLOG(_L("CMmcService::NewLC >>"));

    CMmcService* self = CMmcService::NewL();
    CleanupStack::PushL(self);

    FLOG(_L("CMmcService::NewLC <<"));
    return self;
    }

// ---------------------------------------------------------------------------
// CMmcService::ConstructL
// ---------------------------------------------------------------------------
void CMmcService::ConstructL()
    {
    FLOG(_L("CMmcService::ConstructL >>"));

    FLOG(_L("CMmcService::ConstructL <<"));
    }

// ---------------------------------------------------------------------------
// CMmcService::CMmcService
// ---------------------------------------------------------------------------
CMmcService::CMmcService(const TPSKeyCondition& aPSKeyCondition):CDmEventServiceBase(aPSKeyCondition, EMmcService)
            {
            FLOG(_L("CMmcService::CMmcService >>"));

            FLOG(_L("CMmcService::CMmcService <<"));
            }

// ---------------------------------------------------------------------------
// CMmcService::~CMmcService
// ---------------------------------------------------------------------------
CMmcService::~CMmcService()
    {

    }

// ---------------------------------------------------------------------------
// CMmcService::IsKeyValid
// ---------------------------------------------------------------------------
TBool CMmcService::IsKeyValid()
    {
    FLOG(_L("CMmcService::IsKeyValid >>"));
    TBool ret (EFalse);
    TInt value (KErrNone);

    //Just read the key to find if it exists
    if (RProperty::Get(KMMCPSKey.iConditionCategory, KMMCPSKey.iConditionKey, value) == KErrNone)
        ret = ETrue;

    FLOG(_L("CMmcService::IsKeyValid, return = %d >>"), ret);
    return ret;
    }


// ---------------------------------------------------------------------------
// CMmcService::WaitForRequestCompleteL
// ---------------------------------------------------------------------------
void CMmcService::WaitForRequestCompleteL()
    {
    FLOG(_L("CMmcService::WaitForRequestCompleteL >>"));
    TInt value (KErrNone);

    iOperation = ENoOpn;
    TInt err = RProperty::Get(KMMCPSKey.iConditionCategory, KMMCPSKey.iConditionKey, value);
    FLOG(_L("err = %d"), err);

    switch (value)
        {
        case 0:
            {
            iOperation = EOpnRemoved;
            }
            break;
        case 1: 
            {
            iOperation = EOpnInserted;
            }
            break;
        default:
            {
            iOperation = EOpnUnknown;
            }
            break;
        }
    FLOG(_L("CMmcService::WaitForRequestCompleteL <<"));
    }

// ---------------------------------------------------------------------------
// CMmcService::TaskName
// ---------------------------------------------------------------------------
const TDesC& CMmcService::TaskName()
    {
    return  KMmcTaskName();
    }

void CMmcService::UpdateMmcStatus()
    {
    FLOG(_L("Looking for %d"), iLookingFor);

    if (iLookingFor != KErrNotFound)
        {
        CRepository * rep = CRepository::NewL(TUid::Uid(KAppUidDmEventNotifier));
        rep->Set(KMMCStatus, iLookingFor);
        delete rep; rep = NULL;
        }
    else 
        {
        FLOG(_L("Can't set value %d to cenrep"), iLookingFor);
        }
    }
