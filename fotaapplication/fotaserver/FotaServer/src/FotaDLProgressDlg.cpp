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
#include "FotaDLProgressDlg.h"

// ========================= MEMBER FUNCTIONS ================================

// ---------------------------------------------------------------------------
// CFotaDLProgressDlg::CFotaDLProgressDlg
// ---------------------------------------------------------------------------
CFotaDLProgressDlg::CFotaDLProgressDlg( CEikDialog** aSelfPtr
            , TBool aVisibilityDelayOff, MDLProgressDlgObserver* aObserver, TInt aNoteType)
                : CAknProgressDialog( aSelfPtr, aVisibilityDelayOff )
                    ,iNoteType(aNoteType)
                    ,iObserver( aObserver )
                    
    {
    }

// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
CFotaDLProgressDlg::~CFotaDLProgressDlg()
    {
    if (iSelfPtr) 
        {
        *iSelfPtr = NULL;
        iSelfPtr = NULL;
        }
    }

// ---------------------------------------------------------------------------
// CFotaDLProgressDlg::OkToExitL
// Called by framework when the Softkey is pressed.
// ---------------------------------------------------------------------------
//
TBool CFotaDLProgressDlg::OkToExitL( TInt aButtonId )
    {
    TBool result( ETrue );
    iObserver->HandleDLProgressDialogExitL( aButtonId );
    return result;
    }

// ---------------------------------------------------------------------------
// CFotaDLProgressDlg::OfferKeyEventL
// ---------------------------------------------------------------------------
TKeyResponse CFotaDLProgressDlg::OfferKeyEventL(const TKeyEvent& /*aKeyEvent*/
                                                    ,TEventCode /*aType*/)
    {
    return EKeyWasNotConsumed;
    }

// End of File
