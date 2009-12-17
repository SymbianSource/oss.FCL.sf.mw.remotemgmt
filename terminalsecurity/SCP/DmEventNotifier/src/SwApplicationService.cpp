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
* Description: This class implements the Software (un)install service
*
*/
//User include
#include "SwApplicationService.h"
#include "DmEventNotifierDebug.h"

// ====================== MEMBER FUNCTIONS ===================================

// ---------------------------------------------------------------------------
// CSwApplicationService::NewL
// ---------------------------------------------------------------------------
CSwApplicationService* CSwApplicationService::NewL()
    {
    FLOG(_L("CSwApplicationService::NewL >>"));

    CSwApplicationService* self = new (ELeave) CSwApplicationService(KSwPSKeyCondition);
    CleanupStack::PushL(self);

    self->ConstructL();

    CleanupStack::Pop(self);
    FLOG(_L("CSwApplicationService::NewL <<"));
    return self;
    }

// ---------------------------------------------------------------------------
// CSwApplicationService::NewLC
// ---------------------------------------------------------------------------
CSwApplicationService* CSwApplicationService::NewLC()
    {
    FLOG(_L("CSwApplicationService::NewLC >>"));

    CSwApplicationService* self = CSwApplicationService::NewL();
    CleanupStack::PushL(self);

    FLOG(_L("CSwApplicationService::NewLC <<"));
    return self;
    }

// ---------------------------------------------------------------------------
// CSwApplicationService::ConstructL
// ---------------------------------------------------------------------------
void CSwApplicationService::ConstructL()
    {
    FLOG(_L("CSwApplicationService::ConstructL >>"));

    FLOG(_L("CSwApplicationService::ConstructL <<"));
    }

// ---------------------------------------------------------------------------
// CSwApplicationService::CSwApplicationService
// ---------------------------------------------------------------------------
CSwApplicationService::CSwApplicationService(const TPSKeyCondition& aPSKeyCondition):CDmEventServiceBase(aPSKeyCondition, ESoftwareService)
        {
        FLOG(_L("CSwApplicationService::CSwApplicationService >>"));

        FLOG(_L("CSwApplicationService::CSwApplicationService <<"));
        }

// ---------------------------------------------------------------------------
// CSwApplicationService::~CSwApplicationService
// ---------------------------------------------------------------------------
CSwApplicationService::~CSwApplicationService()
    {

    }

// ---------------------------------------------------------------------------
// CSwApplicationService::IsKeyValid
// ---------------------------------------------------------------------------
TBool CSwApplicationService::IsKeyValid()
    {
    FLOG(_L("CSwApplicationService::IsKeyValid >>"));
    TBool ret (EFalse);
    TInt value (KErrNone);

    //Just read the key to find if it exists
    if (RProperty::Get(KSwPSKeyCondition.iPskey.iConditionCategory, KSwPSKeyCondition.iPskey.iConditionKey, value) == KErrNone)
        ret = ETrue;

    FLOG(_L("CSwApplicationService::IsKeyValid, return = %d >>"), ret);
    return ret;
    }

// ---------------------------------------------------------------------------
// CSwApplicationService::WaitForRequestCompleteL
// ---------------------------------------------------------------------------
void CSwApplicationService::WaitForRequestCompleteL()
    {
    FLOG(_L("CSwApplicationService::WaitForRequestCompleteL >>"));

    TRequestStatus status (KErrNone);
    RProperty prop;
    TInt value (KErrNone);

    iOperation = ENoOpn;
    TPSKey pskey = GetPSKeyCondition().iPskey;
    do {
        FLOG(_L("Waiting for IDLE state..."))
        __LEAVE_IF_ERROR( prop.Attach(pskey.iConditionCategory, pskey.iConditionKey));
    
        prop.Subscribe(status);
        User::WaitForRequest(status);
    
        __LEAVE_IF_ERROR( prop.Get(pskey.iConditionCategory, pskey.iConditionKey, value));
    }while (IsSwInIdle(value));

    FLOG(_L("CSwApplicationService::WaitForRequestCompleteL >>"));
    }

// ---------------------------------------------------------------------------
// CSwApplicationService::IsSwInIdle
// ---------------------------------------------------------------------------
TBool CSwApplicationService::IsSwInIdle(TInt aValue)
    {
    FLOG(_L("CSwApplicationService::IsSwInIdle, value = %d >> "), aValue);

    TInt operation(aValue & Swi::KSwisOperationMask);
    TInt operationStatus(aValue & Swi::KSwisOperationStatusMask);
    TBool ret (EFalse);

    FLOG(_L("operation %d, status %d"), operation, operationStatus);

    if (Swi::ESwisStatusSuccess == operationStatus) 
        {
        switch (operation)
            {
            case Swi::ESwisInstall: 
                {
                FLOG(_L("Installation in progress"));
                iOperation = EOpnInstall;
                }
                break;
            case Swi::ESwisUninstall:
                {
                FLOG(_L("Uninstallation in progress"));
                iOperation = EOpnUninstall;
                }
                break;
            case Swi::ESwisRestore:
                {
                FLOG(_L("Restore in progress"));
                iOperation = EOpnRestore;
                }
                break;
            default:
                {
                FLOG(_L("Unknown operation"));
                iOperation = EOpnUnknown;
                }
            }
        }
    ret = (operation != Swi::ESwisNone)? ETrue:EFalse;
    FLOG(_L("CSwApplicationService::IsSwInIdle, ret = %d << "),ret);
    return ret;
    }

// ---------------------------------------------------------------------------
// CSwApplicationService::TaskName
// ---------------------------------------------------------------------------
const TDesC& CSwApplicationService::TaskName()
    {
    return  KSisTaskName();
    }
