/*
* Copyright (c) 2002-2006 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Helper class for saving Provisioning settings. Provides a progress note.
*
*/


// INCLUDE FILES
#include <AknQueryDialog.h>
#include <eikprogi.h>
#include <ProvisioningBC.rsg>
#include <commdb.h>
#include <CWPEngine.h>
#include <CWPAdapter.h>
#include <activefavouritesdbnotifier.h>
#include "CWPSaver.h"

// CONSTANTS
const TInt KMaxWaitTime = 2000000;
const TInt KRetryCount = 5;

// CLASS DECLARATION

// ========================== MEMBER FUNCTIONS ===============================

// ----------------------------------------------------------------------------
// C++ default constructor.
// ----------------------------------------------------------------------------
CWPSaver::CWPSaver( CWPEngine& aEngine, TBool aSetAsDefault )
                  : CActive( EPriorityStandard ),
                    iEngine( aEngine ), 
                    iSetAsDefault( aSetAsDefault ),
                    iCurrentItem( 0 ),
                    iResult( KErrNone ),
                    iNumAccessDenied(0)
    {
    }

// ----------------------------------------------------------------------------
// CWPSaver::PrepareLC
// ----------------------------------------------------------------------------
//
void CWPSaver::PrepareLC()
    {
    // Assume ownership of this.
    CleanupStack::PushL( this );

    iApDbNotifier = CActiveApDb::NewL( EDatabaseTypeIAP );
    iApDbNotifier->AddObserverL( this );
    
    User::LeaveIfError( iSession.Connect() );
    User::LeaveIfError( iBookmarkDb.Open( iSession, KBrowserBookmarks ) );
    iFavouritesNotifier = 
                new(ELeave) CActiveFavouritesDbNotifier( iBookmarkDb, *this );  

    iFavouritesNotifier->Start();

    iRetryTimer = CPeriodic::NewL( EPriorityStandard );

    // Set up the dialog and callback mechanism.
    iDialog = new(ELeave)CAknProgressDialog(
        reinterpret_cast<CEikDialog**>(&iDialog), EFalse );
    iDialog->SetCallback( this );
    iDialog->ExecuteLD(R_WAITNOTE_SAVE);
    CEikProgressInfo* progressInfo = iDialog->GetProgressInfoL();
    progressInfo->SetAndDraw(iCurrentItem);
    progressInfo->SetFinalValue(iEngine.ItemCount());
    }

// ----------------------------------------------------------------------------
// CWPSaver::ExecuteLD
// ----------------------------------------------------------------------------
//
TInt CWPSaver::ExecuteLD( TInt& aNumSaved )
    {
    PrepareLC();

    // Add us to active scheduler and make sure RunL() gets called.
    CActiveScheduler::Add( this );
    CompleteRequest();
    iWait.Start();

    // Progress note has been finished/cancelled. Cache the result
    // and delete this.
    TInt result( iResult );
    aNumSaved = iCurrentItem;
    CleanupStack::PopAndDestroy(); // this

    return result;
    }

// ----------------------------------------------------------------------------
// Destructor
// ----------------------------------------------------------------------------
CWPSaver::~CWPSaver()
    {
    Cancel();

    delete iApDbNotifier;

    if( iFavouritesNotifier )
        {
        iFavouritesNotifier->Cancel();
        delete iFavouritesNotifier;
        }

    iBookmarkDb.Close();
    iSession.Close();
    delete iRetryTimer;
    }

// ----------------------------------------------------------------------------
// CWPSaver::DoCancel
// ----------------------------------------------------------------------------
//
void CWPSaver::DoCancel()
    {
    }

// ----------------------------------------------------------------------------
// CWPSaver::RunL
// ----------------------------------------------------------------------------
//
void CWPSaver::RunL()
    {
    // Choose whether to save or set as default
    TInt err( KErrNone );
    if( iSetAsDefault )
        {
        TRAP( err, 
            if( iEngine.CanSetAsDefault( iCurrentItem ) )
                {
                iEngine.SetAsDefaultL( iCurrentItem );
                } );
        }
    else
        {
        TRAP( err, iEngine.SaveL( iCurrentItem ) );
        }

    // If CommsDB or BookmarkDB are locked, schedule a retry
    if( err == EWPCommsDBLocked || err == KErrLocked)
        {
        iWaitCommsDb = ETrue;
        DelayedCompleteRequestL();
        return;
        }
    else if( err == EWPBookmarksLocked )
        {
        iWaitFavourites = ETrue;
        DelayedCompleteRequestL();
        return;
        }
    else if( err == KErrNone || err == KErrAccessDenied)
        {
        if( err == 	KErrAccessDenied)
	          {
	          TInt 	aNumAccessDenied = GetNumAccessDenied();
	          aNumAccessDenied++;
	          SetNumAccessDenied(aNumAccessDenied);
	          iEngine.SetAccessDenied(ETrue);
	          }
    
        // Succesful save, so reset retry count
        iRetryCount = 0;

        // Normal progress
        if( iCurrentItem == iEngine.ItemCount()-1 )
            {
            iDialog->ProcessFinishedL();
            }
        else
            {
            CEikProgressInfo* progressInfo = iDialog->GetProgressInfoL();
            iCurrentItem++;
            progressInfo->SetAndDraw(iCurrentItem);
            CompleteRequest();
            }
        }
    else
        {    
        // For all other errors, pass them through.
        User::LeaveIfError( err );
        }
    }

// ----------------------------------------------------------------------------
// CWPSaver::RunError
// ----------------------------------------------------------------------------
//
TInt CWPSaver::RunError( TInt aError )
    {
    // There was a leave in RunL(). Store the error and
    // stop the dialog.
    iResult = aError;
    iWait.AsyncStop();
    delete iDialog;
    iDialog = NULL;

    return KErrNone;
    }

// ----------------------------------------------------------------------------
// CWPSaver::DialogDismissedL
// ----------------------------------------------------------------------------
//
void CWPSaver::DialogDismissedL( TInt aButtonId )
    {
    if( aButtonId < 0 )
        {
        iResult = KErrCancel;
        }

    iWait.AsyncStop();
    }

// ----------------------------------------------------------------------------
// CWPSaver::CompleteRequest
// ----------------------------------------------------------------------------
//
void CWPSaver::CompleteRequest()
    {
    // Schedule an immediate complete. Make sure that there
    // is no timer alive first
    Cancel();
    iRetryTimer->Cancel();

    SetActive();
    TRequestStatus* sp = &iStatus;
    User::RequestComplete( sp, KErrNone );
    }

// ----------------------------------------------------------------------------
// CWPSaver::DelayedCompleteRequestL
// ----------------------------------------------------------------------------
//
void CWPSaver::DelayedCompleteRequestL()
    {
    if( iRetryCount < KRetryCount )
        {
        // Schedule a delayed complete. Cancel first in case
        // an immediate request was scheduled.
        iRetryTimer->Cancel();
        iRetryTimer->Start( KMaxWaitTime, KMaxTInt32, TCallBack( Timeout, this ) );
        iRetryCount++;
        }
    else
        {
        User::Leave( KErrTimedOut );
        }
    }

// ----------------------------------------------------------------------------
// CWPSaver::Retry
// ----------------------------------------------------------------------------
//
void CWPSaver::Retry()
    {
    // Immediate retry. Mark that we're not waiting
    // for an event and complete request.
    iWaitCommsDb = EFalse;
    iWaitFavourites = EFalse;
    CompleteRequest();
    }

// ----------------------------------------------------------------------------
// CWPSaver::Timeout
// ----------------------------------------------------------------------------
//
TInt CWPSaver::Timeout(TAny* aSelf)
    {
    // There was a time-out. Retry saving even though we
    // didn't get a notify from database.
    CWPSaver* self = static_cast<CWPSaver*>( aSelf );
    self->Retry();

    return KErrNone;
    }

// ----------------------------------------------------------------------------
// CWPSaver::HandleApDbEventL
// ----------------------------------------------------------------------------
//
void CWPSaver::HandleApDbEventL( TEvent aEvent )
    {
    // We received an event from CommsDB. Retry if we're
    // waiting for it.
    if( iWaitCommsDb && aEvent == EDbAvailable )
        {
        Retry();
        }
    }

// ----------------------------------------------------------------------------
// CWPSaver::HandleFavouritesDbEventL
// ----------------------------------------------------------------------------
//
void CWPSaver::HandleFavouritesDbEventL( RDbNotifier::TEvent /*aEvent*/ )
    {
    // We received an event from BookmarkDB. Retry if we're
    // waiting for it.
    if( iWaitFavourites )
        {
        Retry();
        }
    }

TInt CWPSaver::GetNumAccessDenied()
    {
    return iNumAccessDenied;
    }	
void CWPSaver::SetNumAccessDenied(TInt aNumAccessDenied )
    {
    iNumAccessDenied = aNumAccessDenied;
    }	
//  End of File
