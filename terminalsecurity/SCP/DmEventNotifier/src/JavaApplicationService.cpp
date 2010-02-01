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
* Description: This class implements the Java (un)install service
*
*/

//User includes
#include "JavaApplicationService.h"
#include "DmEventNotifierDebug.h"

// ====================== MEMBER FUNCTIONS ===================================

// ---------------------------------------------------------------------------
// CJavaApplicationService::NewL
// ---------------------------------------------------------------------------
CJavaApplicationService* CJavaApplicationService::NewL()
    {
    _DMEVNT_DEBUG(_L("CJavaApplicationService::NewL >>"));

    CJavaApplicationService* self = new (ELeave) CJavaApplicationService(KJavaPSKeyCondition);
    CleanupStack::PushL(self);

    self->ConstructL();

    CleanupStack::Pop(self);
    _DMEVNT_DEBUG(_L("CJavaApplicationService::NewL <<"));
    return self;
    }

// ---------------------------------------------------------------------------
// CJavaApplicationService::NewLC
// ---------------------------------------------------------------------------
CJavaApplicationService* CJavaApplicationService::NewLC()
    {
    _DMEVNT_DEBUG(_L("CJavaApplicationService::NewLC >>"));

    CJavaApplicationService* self = CJavaApplicationService::NewL();
    CleanupStack::PushL(self);

    _DMEVNT_DEBUG(_L("CJavaApplicationService::NewLC <<"));
    return self;
    }

// ---------------------------------------------------------------------------
// CJavaApplicationService::ConstructL
// ---------------------------------------------------------------------------
void CJavaApplicationService::ConstructL()
    {
    _DMEVNT_DEBUG(_L("CJavaApplicationService::ConstructL >>"));

    _DMEVNT_DEBUG(_L("CJavaApplicationService::ConstructL <<"));
    }

// ---------------------------------------------------------------------------
// CJavaApplicationService::CJavaApplicationService
// ---------------------------------------------------------------------------
CJavaApplicationService::CJavaApplicationService(const TPSKeyCondition& aPSKeyCondition):CDmEventServiceBase(aPSKeyCondition, EJavaService)
        {
        _DMEVNT_DEBUG(_L("CJavaApplicationService::CJavaApplicationService >>"));

        _DMEVNT_DEBUG(_L("CJavaApplicationService::CJavaApplicationService <<"));
        }

// ---------------------------------------------------------------------------
// CJavaApplicationService::~CJavaApplicationService
// ---------------------------------------------------------------------------
CJavaApplicationService::~CJavaApplicationService()
    {

    }

// ---------------------------------------------------------------------------
// CJavaApplicationService::IsKeyValid
// ---------------------------------------------------------------------------
TBool CJavaApplicationService::IsKeyValid()
    {
    _DMEVNT_DEBUG(_L("CJavaApplicationService::IsKeyValid >>"));
    TBool ret (EFalse);
    TInt value (KErrNone);

    //Just read the key to find if it exists
    if (RProperty::Get(KJavaPSKeyCondition.iPskey.iConditionCategory, KJavaPSKeyCondition.iPskey.iConditionKey, value) == KErrNone)
        ret = ETrue;

    _DMEVNT_DEBUG(_L("CJavaApplicationService::IsKeyValid, return = %d >>"), ret);
    return ret;
    }


// ---------------------------------------------------------------------------
// CJavaApplicationService::WaitForRequestCompleteL
// ---------------------------------------------------------------------------
void CJavaApplicationService::WaitForRequestCompleteL()
    {
    _DMEVNT_DEBUG(_L("CJavaApplicationService::WaitForRequestCompleteL >>"));

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
// CJavaApplicationService::IsSwInIdle
// ---------------------------------------------------------------------------
TBool CJavaApplicationService::IsSwInIdle(TInt aValue)
    {
    _DMEVNT_DEBUG(_L("CJavaApplicationService::IsSwInIdle, value = %d >> "), aValue);
    TBool ret (EFalse);

    TInt operation(aValue & KJavaOperationMask);
    TInt operationStatus(aValue & KJavaStatusMask);


    _DMEVNT_DEBUG(_L("operation %d, status %d"), operation, operationStatus);

    if (EJavaStatusSuccess == operationStatus) 
        {
        switch (operation)
            {
            case EJavaInstall:
                {
                _DMEVNT_DEBUG(_L("Uninstallation in progress"));
                iOperation = EOpnInstall;
                }
                break;
            case EJavaUninstall:
                {
                _DMEVNT_DEBUG(_L("Restore in progress"));
                iOperation = EOpnUninstall;
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

    _DMEVNT_DEBUG(_L("CJavaApplicationService::IsSwInIdle, ret = %d << "),ret);
    return ret;
    }

// ---------------------------------------------------------------------------
// CJavaApplicationService::TaskName
// ---------------------------------------------------------------------------
const TDesC& CJavaApplicationService::TaskName()
    {
    return  KJavaTaskName();
    }
