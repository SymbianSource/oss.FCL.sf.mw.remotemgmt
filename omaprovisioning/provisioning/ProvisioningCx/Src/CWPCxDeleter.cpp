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
#include "CWPCxDeleter.h"
#include <aknquerydialog.h>
#include <eikprogi.h>
#include <ProvisioningCx.rsg>
#include <commdb.h>
#include <CWPEngine.h>
#include <CWPAdapter.h>
#include <ActiveFavouritesDbNotifier.h>

// CONSTANTS
const TInt KMaxWaitTime = 2000000;
const TInt KRetryCount = 5;

// CLASS DECLARATION

// ================= MEMBER FUNCTIONS =======================

// C++ default constructor.
CWPCxDeleter::CWPCxDeleter( CWPEngine& aEngine, TUint32 aContext )
: CActive( EPriorityStandard ), iEngine( aEngine ), 
  iContext( aContext ),
  iCurrentItem( 0 ), iResult( KErrNone )
    {
    }

// ----------------------------------------------------------------------------
// CWPCxDeleter::ExecuteLD
// ----------------------------------------------------------------------------
//
void CWPCxDeleter::PrepareLC()
    {
    // Assume ownership of this.
    CleanupStack::PushL( this );

    iApDbNotifier = CActiveApDb::NewL( EDatabaseTypeIAP );
    iApDbNotifier->AddObserverL( this );
    
    User::LeaveIfError( iSession.Connect() );
    User::LeaveIfError( iBookmarkDb.Open( iSession, KBrowserBookmarks ) );
    iFavouritesNotifier = new(ELeave) CActiveFavouritesDbNotifier( iBookmarkDb, *this );

    iFavouritesNotifier->Start();

    iRetryTimer = CPeriodic::NewL( EPriorityStandard );

    // Set up the dialog and callback mechanism.
    iDialog = new(ELeave)CAknProgressDialog(
        reinterpret_cast<CEikDialog**>(&iDialog), EFalse );
    iDialog->SetCallback( this );
    iDialog->ExecuteLD(R_WAITNOTE_DELETE);
    CEikProgressInfo* progressInfo = iDialog->GetProgressInfoL();
    progressInfo->SetAndDraw(iCurrentItem);
    progressInfo->SetFinalValue(iEngine.ContextDataCountL(iContext));
    }

// ----------------------------------------------------------------------------
// CWPCxDeleter::ExecuteLD
// ----------------------------------------------------------------------------
//
TInt CWPCxDeleter::ExecuteLD( TInt& aNumDeleted )
    {
    PrepareLC();

    // Add us to active scheduler and make sure RunL() gets called.
    CActiveScheduler::Add( this );
    CompleteRequest();
    iWait.Start();

    // Progress note has been finished/cancelled. Cache the result
    // and delete this.
    TInt result( iResult );
    aNumDeleted = iCurrentItem;
    CleanupStack::PopAndDestroy(); // this

    return result;
    }

// Destructor
CWPCxDeleter::~CWPCxDeleter()
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
// CWPCxDeleter::DoCancel
// ----------------------------------------------------------------------------
//
void CWPCxDeleter::DoCancel()
    {
    }

// ----------------------------------------------------------------------------
// CWPCxDeleter::RunL
// ----------------------------------------------------------------------------
//
void CWPCxDeleter::RunL()
    {
    // Choose whether to save or set as default
    TBool more( EFalse );
    TRAPD( err, more = iEngine.DeleteContextDataL( iContext ) );

    // If CommsDB or BookmarkDB are locked, schedule a retry
    if( err == EWPCommsDBLocked )
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
    else if( err != KErrNone )
        {
        // For all other errors, pass them through.
        User::LeaveIfError( err );
        }
    // Succesful save, so reset retry count
    iRetryCount = 0;

    // Normal progress
    if( !more )
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

// ---------------------------------------------------------
// CWPCxDeleter::RunError
// ---------------------------------------------------------
//
TInt CWPCxDeleter::RunError( TInt aError )
    {
    // There was a leave in RunL(). Store the error and
    // stop the dialog.
    iResult = aError;
    iWait.AsyncStop();
    delete iDialog;
    iDialog = NULL;

    return KErrNone;
    }

// ---------------------------------------------------------
// CWPCxDeleter::DialogDismissedL
// ---------------------------------------------------------
//
void CWPCxDeleter::DialogDismissedL( TInt aButtonId )
    {
    if( aButtonId < 0 )
        {
        iResult = KErrCancel;
        }

    iWait.AsyncStop();
    }

// ----------------------------------------------------------------------------
// CWPCxDeleter::CompleteRequest
// ----------------------------------------------------------------------------
//
void CWPCxDeleter::CompleteRequest()
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
// CWPCxDeleter::DelayedCompleteRequestL
// ----------------------------------------------------------------------------
//
void CWPCxDeleter::DelayedCompleteRequestL()
    {
    if( iRetryCount < KRetryCount )
        {
        // Schedule a delayed complete.
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
// CWPCxDeleter::Retry
// ----------------------------------------------------------------------------
//
void CWPCxDeleter::Retry()
    {
    // Immediate retry. Mark that we're not waiting
    // for an event and complete request.

    iWaitCommsDb = EFalse;
    iWaitFavourites = EFalse;
    CompleteRequest();
    }

// ----------------------------------------------------------------------------
// CWPCxDeleter::Timeout
// ----------------------------------------------------------------------------
//
TInt CWPCxDeleter::Timeout(TAny* aSelf)
    {
    // There was a time-out. Retry saving even though we
    // didn't get a notify from database.
    CWPCxDeleter* self = STATIC_CAST(CWPCxDeleter*, aSelf);
    self->Retry();

    return KErrNone;
    }

// ----------------------------------------------------------------------------
// CWPCxDeleter::HandleApDbEventL
// ----------------------------------------------------------------------------
//
void CWPCxDeleter::HandleApDbEventL( TEvent aEvent )
    {
    // We received an event from CommsDB. Retry if we're
    // waiting for it.
    if( iWaitCommsDb && aEvent == EDbAvailable )
        {
        Retry();
        }
    }

// ----------------------------------------------------------------------------
// CWPCxDeleter::HandleFavouritesDbEventL
// ----------------------------------------------------------------------------
//
void CWPCxDeleter::HandleFavouritesDbEventL( RDbNotifier::TEvent /*aEvent*/ )
    {
    // We received an event from BookmarkDB. Retry if we're
    // waiting for it.
    if( iWaitFavourites )
        {
        Retry();
        }
    }

//  End of File

