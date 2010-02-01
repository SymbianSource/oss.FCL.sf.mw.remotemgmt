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
    _DMEVNT_DEBUG(_L("CMmcService::NewL >>"));

    TInt value (KErrNotFound);

    RProperty::Get(KMMCPSKey.iConditionCategory, KMMCPSKey.iConditionKey, value);
    CMmcService* self (NULL);
    if(value == 0)
        {
        _DMEVNT_DEBUG(_L("MMC is removed, hence setting for insertion..."));
        self = new (ELeave) CMmcService(KMMCPSKeyConditionInsert);
        }
    else //(value == 1)
        {
        _DMEVNT_DEBUG(_L("MMC is inserted, hence setting for removal..."))
        self = new (ELeave) CMmcService(KMMCPSKeyConditionRemove);
        }

    CleanupStack::PushL(self);

    self->ConstructL();
    self->iLookingFor = value;
    CleanupStack::Pop(self);
    _DMEVNT_DEBUG(_L("CMmcService::NewL <<"));
    return self;
    }

// ---------------------------------------------------------------------------
// CMmcService::NewLC
// ---------------------------------------------------------------------------
CMmcService* CMmcService::NewLC()
    {
    _DMEVNT_DEBUG(_L("CMmcService::NewLC >>"));

    CMmcService* self = CMmcService::NewL();
    CleanupStack::PushL(self);

    _DMEVNT_DEBUG(_L("CMmcService::NewLC <<"));
    return self;
    }

// ---------------------------------------------------------------------------
// CMmcService::ConstructL
// ---------------------------------------------------------------------------
void CMmcService::ConstructL()
    {
    _DMEVNT_DEBUG(_L("CMmcService::ConstructL >>"));

    _DMEVNT_DEBUG(_L("CMmcService::ConstructL <<"));
    }

// ---------------------------------------------------------------------------
// CMmcService::CMmcService
// ---------------------------------------------------------------------------
CMmcService::CMmcService(const TPSKeyCondition& aPSKeyCondition):CDmEventServiceBase(aPSKeyCondition, EMmcService)
            {
            _DMEVNT_DEBUG(_L("CMmcService::CMmcService >>"));

            _DMEVNT_DEBUG(_L("CMmcService::CMmcService <<"));
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
    _DMEVNT_DEBUG(_L("CMmcService::IsKeyValid >>"));
    TBool ret (EFalse);
    TInt value (KErrNone);

    //Just read the key to find if it exists
    if (RProperty::Get(KMMCPSKey.iConditionCategory, KMMCPSKey.iConditionKey, value) == KErrNone)
        ret = ETrue;

    _DMEVNT_DEBUG(_L("CMmcService::IsKeyValid, return = %d >>"), ret);
    return ret;
    }


// ---------------------------------------------------------------------------
// CMmcService::WaitForRequestCompleteL
// ---------------------------------------------------------------------------
void CMmcService::WaitForRequestCompleteL()
    {
    _DMEVNT_DEBUG(_L("CMmcService::WaitForRequestCompleteL >>"));
    TInt value (KErrNone);

    iOperation = ENoOpn;
    TInt err = RProperty::Get(KMMCPSKey.iConditionCategory, KMMCPSKey.iConditionKey, value);
    _DMEVNT_DEBUG(_L("err = %d"), err);

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
    _DMEVNT_DEBUG(_L("CMmcService::WaitForRequestCompleteL <<"));
    }

// ---------------------------------------------------------------------------
// CMmcService::TaskName
// ---------------------------------------------------------------------------
const TDesC& CMmcService::TaskName()
    {
    return  KMmcTaskName();
    }

void CMmcService::UpdateMmcStatusL()
    {
    _DMEVNT_DEBUG(_L("Looking for %d"), iLookingFor);

    if (iLookingFor != KErrNotFound)
        {
        CRepository * rep = CRepository::NewL(TUid::Uid(KAppUidDmEventNotifier));
        rep->Set(KMMCStatus, iLookingFor);
        delete rep; rep = NULL;
        }
    else 
        {
        _DMEVNT_DEBUG(_L("Can't set value %d to cenrep"), iLookingFor);
        }
    }
