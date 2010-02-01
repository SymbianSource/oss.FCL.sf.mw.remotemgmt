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
    _DMEVNT_DEBUG(_L("CSwApplicationService::NewL >>"));

    CSwApplicationService* self = new (ELeave) CSwApplicationService(KSwPSKeyCondition);
    CleanupStack::PushL(self);

    self->ConstructL();

    CleanupStack::Pop(self);
    _DMEVNT_DEBUG(_L("CSwApplicationService::NewL <<"));
    return self;
    }

// ---------------------------------------------------------------------------
// CSwApplicationService::NewLC
// ---------------------------------------------------------------------------
CSwApplicationService* CSwApplicationService::NewLC()
    {
    _DMEVNT_DEBUG(_L("CSwApplicationService::NewLC >>"));

    CSwApplicationService* self = CSwApplicationService::NewL();
    CleanupStack::PushL(self);

    _DMEVNT_DEBUG(_L("CSwApplicationService::NewLC <<"));
    return self;
    }

// ---------------------------------------------------------------------------
// CSwApplicationService::ConstructL
// ---------------------------------------------------------------------------
void CSwApplicationService::ConstructL()
    {
    _DMEVNT_DEBUG(_L("CSwApplicationService::ConstructL >>"));

    _DMEVNT_DEBUG(_L("CSwApplicationService::ConstructL <<"));
    }

// ---------------------------------------------------------------------------
// CSwApplicationService::CSwApplicationService
// ---------------------------------------------------------------------------
CSwApplicationService::CSwApplicationService(const TPSKeyCondition& aPSKeyCondition):CDmEventServiceBase(aPSKeyCondition, ESoftwareService)
        {
        _DMEVNT_DEBUG(_L("CSwApplicationService::CSwApplicationService >>"));

        _DMEVNT_DEBUG(_L("CSwApplicationService::CSwApplicationService <<"));
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
    _DMEVNT_DEBUG(_L("CSwApplicationService::IsKeyValid >>"));
    TBool ret (EFalse);
    TInt value (KErrNone);

    //Just read the key to find if it exists
    if (RProperty::Get(KSwPSKeyCondition.iPskey.iConditionCategory, KSwPSKeyCondition.iPskey.iConditionKey, value) == KErrNone)
        ret = ETrue;

    _DMEVNT_DEBUG(_L("CSwApplicationService::IsKeyValid, return = %d >>"), ret);
    return ret;
    }

// ---------------------------------------------------------------------------
// CSwApplicationService::WaitForRequestCompleteL
// ---------------------------------------------------------------------------
void CSwApplicationService::WaitForRequestCompleteL()
    {
    _DMEVNT_DEBUG(_L("CSwApplicationService::WaitForRequestCompleteL >>"));

    TRequestStatus status (KErrNone);
    RProperty prop;
    TInt value (KErrNone);

    iOperation = ENoOpn;
    TPSKey pskey = GetPSKeyCondition().iPskey;
    do {
        _DMEVNT_DEBUG(_L("Waiting for IDLE state..."))
        __LEAVE_IF_ERROR( prop.Attach(pskey.iConditionCategory, pskey.iConditionKey));
    
        prop.Subscribe(status);
        User::WaitForRequest(status);
    
        __LEAVE_IF_ERROR( prop.Get(pskey.iConditionCategory, pskey.iConditionKey, value));
    }while (IsSwInIdle(value));

    _DMEVNT_DEBUG(_L("CSwApplicationService::WaitForRequestCompleteL >>"));
    }

// ---------------------------------------------------------------------------
// CSwApplicationService::IsSwInIdle
// ---------------------------------------------------------------------------
TBool CSwApplicationService::IsSwInIdle(TInt aValue)
    {
    _DMEVNT_DEBUG(_L("CSwApplicationService::IsSwInIdle, value = %d >> "), aValue);

    TInt operation(aValue & Swi::KSwisOperationMask);
    TInt operationStatus(aValue & Swi::KSwisOperationStatusMask);
    TBool ret (EFalse);

    _DMEVNT_DEBUG(_L("operation %d, status %d"), operation, operationStatus);

    if (Swi::ESwisStatusSuccess == operationStatus) 
        {
        switch (operation)
            {
            case Swi::ESwisInstall: 
                {
                _DMEVNT_DEBUG(_L("Installation in progress"));
                iOperation = EOpnInstall;
                }
                break;
            case Swi::ESwisUninstall:
                {
                _DMEVNT_DEBUG(_L("Uninstallation in progress"));
                iOperation = EOpnUninstall;
                }
                break;
            case Swi::ESwisRestore:
                {
                _DMEVNT_DEBUG(_L("Restore in progress"));
                iOperation = EOpnRestore;
                }
                break;
            default:
                {
                _DMEVNT_DEBUG(_L("Unknown operation"));
                iOperation = EOpnUnknown;
                }
            }
        }
    ret = (operation != Swi::ESwisNone)? ETrue:EFalse;
    _DMEVNT_DEBUG(_L("CSwApplicationService::IsSwInIdle, ret = %d << "),ret);
    return ret;
    }

// ---------------------------------------------------------------------------
// CSwApplicationService::TaskName
// ---------------------------------------------------------------------------
const TDesC& CSwApplicationService::TaskName()
    {
    return  KSisTaskName();
    }
