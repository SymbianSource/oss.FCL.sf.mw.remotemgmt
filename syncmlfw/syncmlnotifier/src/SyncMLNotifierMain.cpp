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
* Description:  Methods for the SyncML Appserver starter notifier
*
*/



// INCLUDE FILES
#include <eikenv.h>

// All provided notifiers
#include    "SyncMLAppLaunchNotifier.h"
#include    "SyncMLDlgNotifier.h"
#include    "SyncMLFwUpdNotifier.h"

#include    "SyncMLNotifDebug.h"

// CONSTANTS

// ============================= LOCAL FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CleanupArray
// Deletes notifiers
// -----------------------------------------------------------------------------
LOCAL_C void CleanupArray( TAny* aArray )
    {
    FLOG(_L("[SmlNotif]\t CleanupArray()"));
    
    CArrayPtrFlat<MEikSrvNotifierBase2>*     
        subjects = static_cast<CArrayPtrFlat<MEikSrvNotifierBase2>*>(aArray);
    TInt lastInd = subjects->Count() - 1;
    for ( TInt i = lastInd; i >= 0; i-- )
        {
        subjects->At(i)->Release();	    	    
        }
    delete subjects;
    
    FLOG(_L("[SmlNotif]\t CleanupArray() completed"));
    }

// -----------------------------------------------------------------------------
// CreateSmlNotifiersL
// Instantiate notifiers
// -----------------------------------------------------------------------------
LOCAL_C CArrayPtrFlat<MEikSrvNotifierBase2>* CreateSmlNotifiersL()
    {
    FLOG(_L("[SmlNotif]\t CreateSmlNotifiersL"));

    CArrayPtrFlat<MEikSrvNotifierBase2>* notifiers =
        new ( ELeave ) CArrayPtrFlat<MEikSrvNotifierBase2>
        ( KSmlNotifierArrayIncrement );
    
    CleanupStack::PushL( TCleanupItem( CleanupArray, notifiers ) );
    
    CSyncMLAppLaunchNotifier* appLaunchNotifier = CSyncMLAppLaunchNotifier::NewL();
    CleanupStack::PushL( appLaunchNotifier );
    notifiers->AppendL( appLaunchNotifier );
    CleanupStack::Pop( appLaunchNotifier );

    CSyncMLDlgNotifier* dlgNotifier = CSyncMLDlgNotifier::NewL( appLaunchNotifier );
    CleanupStack::PushL( dlgNotifier );
    notifiers->AppendL( dlgNotifier );
    CleanupStack::Pop( dlgNotifier );

    CSyncMLFwUpdNotifier* fwUpdNotifier = CSyncMLFwUpdNotifier::NewL();
    CleanupStack::PushL( fwUpdNotifier );
    notifiers->AppendL( fwUpdNotifier );
    CleanupStack::Pop( fwUpdNotifier );

    CleanupStack::Pop( notifiers );
    
    FLOG(_L("[SmlNotif]\t CreateSmlNotifiersL completed"));
    return notifiers;
    }

// ========================== OTHER EXPORTED FUNCTIONS =========================

// -----------------------------------------------------------------------------
// NotifierArray
// Lib main entry point: Creates a notifiers array.
// -----------------------------------------------------------------------------
EXPORT_C CArrayPtr<MEikSrvNotifierBase2>* NotifierArray()
    {
    FLOG( _L("[SMLNOTIF]\t NotifierArray") );

    CArrayPtrFlat<MEikSrvNotifierBase2>* notifiers = NULL;
    TRAPD( err, notifiers = CreateSmlNotifiersL() );

    if( err != KErrNone )
        {
        FTRACE(FPrint(_L("[SmlNotif]\t Notifier creation failure! Error code: %d"), err));
        err = KErrNone;
        }

    FLOG(_L("[SmlNotif]\t NotifierArray completed"));

    return notifiers;
    }

//  End of File  
