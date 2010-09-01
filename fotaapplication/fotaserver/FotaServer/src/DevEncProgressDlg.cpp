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
* Description:   Progress dlg for download
*
*/



// INCLUDE FILES
#include "DevEncProgressDlg.h"
#include <AknGlobalProgressDialog.h>
#include <StringLoader.h>
#include <stringresourcereader.h>
#include <avkon.rsg>
#include "FotaSrvDebug.h"

// ========================= MEMBER FUNCTIONS ================================
EXPORT_C CDevEncProgressDlg* CDevEncProgressDlg::NewL(MDEProgressDlgObserver* aObserver, TInt aResource)
    {
    FLOG(_L("CDevEncProgressDlg::NewL >>"));
    
    CDevEncProgressDlg* self = CDevEncProgressDlg::NewLC(aObserver, aResource);
    CleanupStack::Pop();
    
    FLOG(_L("CDevEncProgressDlg::NewL <<"));
    return self;
    }

EXPORT_C CDevEncProgressDlg* CDevEncProgressDlg::NewLC(MDEProgressDlgObserver* aObserver, TInt aResource)
    {
    FLOG(_L("CDevEncProgressDlg::NewLC >>"));
    
    CDevEncProgressDlg* self = new (ELeave) CDevEncProgressDlg(aObserver);
    CleanupStack::PushL(self);
    
    self->ConstructL(aResource);
    
    FLOG(_L("CDevEncProgressDlg::NewL <<"));
    return self;    
    }

void CDevEncProgressDlg::ConstructL(TInt aResource)
    {
    FLOG(_L("CDevEncProgressDlg::ConstructL >>"));
    
    iProgressDialog = CAknGlobalProgressDialog::NewL();
    iStringResource = StringLoader::LoadL(aResource);
    
    FLOG(_L("CDevEncProgressDlg::ConstructL <<"));
    }

// ---------------------------------------------------------------------------
// CDevEncProgressDlg::CDevEncProgressDlg
// ---------------------------------------------------------------------------
CDevEncProgressDlg::CDevEncProgressDlg(MDEProgressDlgObserver* aObserver) : CActive(EPriorityMore),
                                                                            iProgressDialog (NULL),
                                                                            iStringResource (NULL),
                                                                            iObserver (aObserver)
    {
    CActiveScheduler::Add(this);
    }

// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
CDevEncProgressDlg::~CDevEncProgressDlg()
    {
    FLOG(_L("CDevEncProgressDlg::~CDevEncProgressDlg >>"));
    
    Cancel();
    
    delete iProgressDialog; iProgressDialog = NULL;
    
    delete iStringResource; iStringResource = NULL;
    
    iObserver = NULL;
    
    FLOG(_L("CDevEncProgressDlg::~CDevEncProgressDlg <<"));
    }

// ---------------------------------------------------------------------------
// CDevEncProgressDlg::OkToExitL
// Called by framework when the Softkey is pressed.
// ---------------------------------------------------------------------------
//
void CDevEncProgressDlg::DoCancel()
    {
    FLOG(_L("CDevEncProgressDlg::DoCancel >>"));
    
    if (iProgressDialog)
        {
        iProgressDialog->CancelProgressDialog();
        }
    
    FLOG(_L("CDevEncProgressDlg::DoCancel <<"));
    }

void CDevEncProgressDlg::RunL()
    {
    FLOG(_L("CDevEncProgressDlg::RunL >>"));
    
    if (iObserver)
        {
        iObserver->HandleDEProgressDialogExitL(iStatus.Int());
        }
    FLOG(_L("CDevEncProgressDlg::RunL <<"));
    }

void CDevEncProgressDlg::ShowProgressDialogL()
    {
    FLOG(_L("CDevEncProgressDlg::ShowProgressDialogL >>"));
    
    iProgressDialog->ShowProgressDialogL(iStatus,iStringResource->Des(),R_AVKON_SOFTKEYS_EMPTY);
    FLOG(_L("Step...1"));
    
    SetActive();
    FLOG(_L("Step...2"));
    FLOG(_L("CDevEncProgressDlg::ShowProgressDialogL <<"));
    }


void CDevEncProgressDlg::UpdateProgressDialogL(TInt aValue, TInt aFinalValue)
    {
    FLOG(_L("CDevEncProgressDlg::UpdateProgressDialogL, value = %d >>"), aValue);
    
    iProgressDialog->UpdateProgressDialog(aValue, aFinalValue);
    
    FLOG(_L("CDevEncProgressDlg::UpdateProgressDialogL <<"));
    }

void CDevEncProgressDlg::ProgressFinished()
    {
    FLOG(_L("CDevEncProgressDlg::ProgressFinished >>"));
   
    iProgressDialog->ProcessFinished();
    
    FLOG(_L("CDevEncProgressDlg::ProgressFinished <<"));
    }
// End of File
