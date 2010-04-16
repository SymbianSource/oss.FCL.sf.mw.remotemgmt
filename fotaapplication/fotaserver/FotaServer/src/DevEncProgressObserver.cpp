/*
 * Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies). 
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
 * Description:   
 *
 */



// INCLUDE FILES

//User Includes

#include "DevEncProgressObserver.h"
#include "DevEncController.h"
#include "DevEncProgressDlg.h"
#include "FotaSrvDebug.h"
#include <eikprogi.h>
#include <fotaConst.h>
// -----------------------------------------------------------------------------
// CDevEncProgressObserver::NewL
// Symbian 2-Phase construction, NewL used for creating object of this class
// This method can leave
// -----------------------------------------------------------------------------

EXPORT_C CDevEncProgressObserver* CDevEncProgressObserver::NewL(CDevEncController* aObserver, TInt aResource)
    {
    CDevEncProgressObserver* self = CDevEncProgressObserver::NewLC(aObserver, aResource);
    CleanupStack::Pop(self);
    return self;	
    }

// -----------------------------------------------------------------------------
// CDevEncProgressObserver::NewLC
// Symbian 2-Phase construction, NewLC used for creating object of this class
// This method can leave
// -----------------------------------------------------------------------------

EXPORT_C CDevEncProgressObserver* CDevEncProgressObserver::NewLC(CDevEncController* aObserver, TInt aResource)
    {
    CDevEncProgressObserver* self = new (ELeave) CDevEncProgressObserver (aObserver);
    CleanupStack::PushL(self);
    self->ConstructL(aResource);

    return self;
    }

// -----------------------------------------------------------------------------
// CDevEncProgressObserver::ConstructL
// Symbian 2-Phase construction, ConstructL used for constructing the members of this class
// This method can leave
// -----------------------------------------------------------------------------

void CDevEncProgressObserver::ConstructL(TInt aResource)
    {
    FLOG(_L("CDevEncProgressObserver::ConstructL >>"));
    
    iProgressDlg = CDevEncProgressDlg::NewL(this, aResource);
    
    FLOG(_L("CDevEncProgressObserver::ConstructL <<"));
    }

// -----------------------------------------------------------------------------
// CDevEncProgressObserver::CDevEncProgressObserver
// C++ Constructor
// This method shouldn't leave
// -----------------------------------------------------------------------------

CDevEncProgressObserver::CDevEncProgressObserver(CDevEncController* aObserver) : 
iObserver(aObserver), iProgressDlg (NULL), iPeriodicTimer(NULL), iEncMemorySession (NULL)
            {

            }

// -----------------------------------------------------------------------------
// CDevEncProgressObserver::~CDevEncProgressObserver
// C++ Desctructor
// This method shouldn't leave
// -----------------------------------------------------------------------------

CDevEncProgressObserver::~CDevEncProgressObserver()
    {
    FLOG(_L("CDevEncProgressObserver::~CDevEncProgressObserver >>"));

    iEncMemorySession = NULL;

    if (iPeriodicTimer)
        {
        FLOG(_L("1..........."));
        iPeriodicTimer->Cancel();
        FLOG(_L("2..........."));
        delete iPeriodicTimer;
        iPeriodicTimer = NULL;
        }

    if (iProgressDlg)
        {
        iProgressDlg->ProgressFinished();
        delete iProgressDlg;
        iProgressDlg = NULL;
        }

    FLOG(_L("CDevEncProgressObserver::~CDevEncProgressObserver <<"));
    }

static TInt StaticTimerExpiry(TAny *aPtr)
    {
    FLOG(_L("CDevEncProgressObserver StaticTimerExpiry() >>"));

    CDevEncProgressObserver* obj = (CDevEncProgressObserver*) aPtr;
    TRAPD( err,  obj->CheckProgressL());
    FLOG(_L("ERROR =  %d"),err);
    
    FLOG(_L("CDevEncProgressObserver StaticTimerExpiry() <<"));
    return err;
    }

// -----------------------------------------------------------------------------
// CDevEncProgressObserver::StartMonitoringL
// Monitors for connection status
// This method don't leave
// -----------------------------------------------------------------------------

void CDevEncProgressObserver::StartMonitoringL(CDevEncSession* aSession)
    {
    FLOG(_L("CDevEncProgressObserver::StartMonitoringL >>"));

    __ASSERT_ALWAYS( aSession, User::Panic(KFotaPanic, KErrArgument) );
    
    iEncMemorySession = aSession;
    iProgressDlg->ShowProgressDialogL();

    iPeriodicTimer = CPeriodic::NewL (EPriorityMore) ;
    iPeriodicTimer->Start (
            TTimeIntervalMicroSeconds32(KNfeTimeInterval)
            ,TTimeIntervalMicroSeconds32(KNfeTimeInterval)
            ,TCallBack(StaticTimerExpiry,this) );

    FLOG(_L("CDevEncProgressObserver::StartMonitoringL <<"));
    }

// -----------------------------------------------------------------------------
// CDevEncProgressObserver::RunL()
// Called when event accomplished
// -----------------------------------------------------------------------------
//
void CDevEncProgressObserver::CheckProgressL()
    {
    FLOG(_L("CDevEncProgressObserver::CheckProgressL >>"));

    TInt progress = GetStatusL();
    FLOG(_L("NFE Progress = %d"),progress);
    if (progress != KProgressComplete)
        {
        iProgressDlg->UpdateProgressDialogL(progress, 100);
        FLOG(_L("nfe progress = %d"), progress);
        }
    else
        {
        FLOG(_L("nfe progress complete"));

        if (iPeriodicTimer && iPeriodicTimer->IsActive())
            {
            iPeriodicTimer->Cancel();
            }

        iProgressDlg->UpdateProgressDialogL(progress, 100);
        iProgressDlg->ProgressFinished();

        delete iProgressDlg; iProgressDlg = NULL;

        iObserver->ReportDevEncOpnCompleteL(KErrNone);
        }
    FLOG(_L("CDevEncProgressObserver::CheckProgressL <<"));
    }




TInt CDevEncProgressObserver::GetStatusL()
    {
    FLOG(_L("CDevEncProgressObserver::GetStatus >>"));
    TInt ret (0);
    User::LeaveIfError(iEncMemorySession->Progress(ret));

    FLOG(_L("CDevEncProgressObserver::GetStatus << progress = %d"), ret);
    return ret;
    }

TBool CDevEncProgressObserver::HandleDEProgressDialogExitL(TInt aButtonId)
    {
    //Do nothing
    return ETrue;
    }
// End of File 
