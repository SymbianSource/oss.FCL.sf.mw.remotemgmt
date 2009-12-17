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
    FLOG(_L("CJavaApplicationService::NewL >>"));

    CJavaApplicationService* self = new (ELeave) CJavaApplicationService(KJavaPSKeyCondition);
    CleanupStack::PushL(self);

    self->ConstructL();

    CleanupStack::Pop(self);
    FLOG(_L("CJavaApplicationService::NewL <<"));
    return self;
    }

// ---------------------------------------------------------------------------
// CJavaApplicationService::NewLC
// ---------------------------------------------------------------------------
CJavaApplicationService* CJavaApplicationService::NewLC()
    {
    FLOG(_L("CJavaApplicationService::NewLC >>"));

    CJavaApplicationService* self = CJavaApplicationService::NewL();
    CleanupStack::PushL(self);

    FLOG(_L("CJavaApplicationService::NewLC <<"));
    return self;
    }

// ---------------------------------------------------------------------------
// CJavaApplicationService::ConstructL
// ---------------------------------------------------------------------------
void CJavaApplicationService::ConstructL()
    {
    FLOG(_L("CJavaApplicationService::ConstructL >>"));

    FLOG(_L("CJavaApplicationService::ConstructL <<"));
    }

// ---------------------------------------------------------------------------
// CJavaApplicationService::CJavaApplicationService
// ---------------------------------------------------------------------------
CJavaApplicationService::CJavaApplicationService(const TPSKeyCondition& aPSKeyCondition):CDmEventServiceBase(aPSKeyCondition, EJavaService)
        {
        FLOG(_L("CJavaApplicationService::CJavaApplicationService >>"));

        FLOG(_L("CJavaApplicationService::CJavaApplicationService <<"));
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
    FLOG(_L("CJavaApplicationService::IsKeyValid >>"));
    TBool ret (EFalse);
    TInt value (KErrNone);

    //Just read the key to find if it exists
    if (RProperty::Get(KJavaPSKeyCondition.iPskey.iConditionCategory, KJavaPSKeyCondition.iPskey.iConditionKey, value) == KErrNone)
        ret = ETrue;

    FLOG(_L("CJavaApplicationService::IsKeyValid, return = %d >>"), ret);
    return ret;
    }


// ---------------------------------------------------------------------------
// CJavaApplicationService::WaitForRequestCompleteL
// ---------------------------------------------------------------------------
void CJavaApplicationService::WaitForRequestCompleteL()
    {
    FLOG(_L("CJavaApplicationService::WaitForRequestCompleteL >>"));

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
// CJavaApplicationService::IsSwInIdle
// ---------------------------------------------------------------------------
TBool CJavaApplicationService::IsSwInIdle(TInt aValue)
    {
    FLOG(_L("CJavaApplicationService::IsSwInIdle, value = %d >> "), aValue);
    TBool ret (EFalse);

    TInt operation(aValue & KJavaOperationMask);
    TInt operationStatus(aValue & KJavaStatusMask);


    FLOG(_L("operation %d, status %d"), operation, operationStatus);

    if (EJavaStatusSuccess == operationStatus) 
        {
        switch (operation)
            {
            case EJavaInstall:
                {
                FLOG(_L("Uninstallation in progress"));
                iOperation = EOpnInstall;
                }
                break;
            case EJavaUninstall:
                {
                FLOG(_L("Restore in progress"));
                iOperation = EOpnUninstall;
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

    FLOG(_L("CJavaApplicationService::IsSwInIdle, ret = %d << "),ret);
    return ret;
    }

// ---------------------------------------------------------------------------
// CJavaApplicationService::TaskName
// ---------------------------------------------------------------------------
const TDesC& CJavaApplicationService::TaskName()
    {
    return  KJavaTaskName();
    }
