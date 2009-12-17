/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  This is the implementation of the Diagnostics Log Event
*                Eraser, which is used to delete OS log entries.
*
*/


// System Include Files
#include <e32base.h>                        // CActiveSchedulerWait
#include <logcli.h>                         // CLogClient
#include <logview.h>                        // CLogViewEvent
#include <DiagFrameworkDebug.h>             // Debugging macros

// Local Include Files
#include "diaglogeventeraser.h"             // CDiagLogEventEraser
#include "diaglogeventeraserobserver.h"     // MDiagLogEventEraserObserver


// ============================ MEMBER FUNCTIONS =============================

// ---------------------------------------------------------------------------
// Static two-phase constructor.
// ---------------------------------------------------------------------------
//
EXPORT_C CDiagLogEventEraser* CDiagLogEventEraser::NewL(
    MDiagLogEventEraserObserver& aObserver,
    CLogFilter* aFilter )
    {
    LOGSTRING( "CDiagLogEventEraser::NewL()" )

    CleanupStack::PushL( aFilter );
    CDiagLogEventEraser* self = new ( ELeave ) CDiagLogEventEraser(
        aObserver,
        aFilter );
    CleanupStack::Pop( aFilter );

    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );

    return self;
    }

// ---------------------------------------------------------------------------
// Destructor.
// ---------------------------------------------------------------------------
//
EXPORT_C CDiagLogEventEraser::~CDiagLogEventEraser()
    {
    LOGSTRING( "CDiagLogEventEraser::~CDiagLogEventEraser()" )

    Cancel();

    delete iLogFilter;
    delete iLogView;
    delete iLogClient;
    delete iWaitScheduler;

    iFsSession.Close();
    }

// ---------------------------------------------------------------------------
// Gets the filter to use when erasing logs.
// ---------------------------------------------------------------------------
//
EXPORT_C CLogFilter& CDiagLogEventEraser::Filter()
    {
    LOGSTRING( "CDiagLogEventEraser::Filter()" )
    return *iLogFilter;
    }

// ---------------------------------------------------------------------------
// Gets the log client used for the log erasure.
// ---------------------------------------------------------------------------
//
EXPORT_C CLogClient& CDiagLogEventEraser::LogClient()
    {
    LOGSTRING( "CDiagLogEventEraser::LogClient()" )
    return *iLogClient;
    }

// ---------------------------------------------------------------------------
// Begins asynchronous erasure of logs.  The observer will be notified of
// completion via callback.
// ---------------------------------------------------------------------------
//
EXPORT_C void CDiagLogEventEraser::StartAsyncEraseLogsL()
    {
    LOGSTRING( "CDiagLogEventEraser::StartAsyncEraseLogsL()" )

    __ASSERT_DEBUG( iState == EStateInitial, User::Invariant() );

    // Apply the filter.
    if( iLogView->SetFilterL( *iLogFilter, iStatus ) )
        {
        iState = EStateApplyingFilter;
        iIsDeletingSynchronous = EFalse;
        SetActive();
        }

    // There are no logs, so set this object active so that the callback will
    // be asynchronous.
    else
        {
        iState = EStateComplete;
        TRequestStatus* status = &iStatus;
        User::RequestComplete( status, KErrNone );
        SetActive();
        }
    }

// ---------------------------------------------------------------------------
// Begins synchronous erasure of logs.
// ---------------------------------------------------------------------------
//
EXPORT_C void CDiagLogEventEraser::EraseLogsL()
    {
    LOGSTRING( "CDiagLogEventEraser::EraseLogsL()" )

    __ASSERT_DEBUG( iState == EStateInitial, User::Invariant() );

    iError = KErrNone;

    // Apply the filter.  iWaitScheduler->Start() will return after the wait
    // scheduler is stopped.
    if( iLogView->SetFilterL( *iLogFilter, iStatus ) )
        {
        iState = EStateApplyingFilter;
        iIsDeletingSynchronous = ETrue;
        SetActive();
        iWaitScheduler->Start();
        }

    // If there is an error, iError will be set when the active object is run.
    User::LeaveIfError( iError );
    }

// ---------------------------------------------------------------------------
// The default constructor.
// ---------------------------------------------------------------------------
//
CDiagLogEventEraser::CDiagLogEventEraser(
    MDiagLogEventEraserObserver& aObserver,
    CLogFilter* aFilter )
:   CActive( EPriorityStandard ),
    iObserver( aObserver ),
    iLogFilter( aFilter )
    {
    LOGSTRING( "CDiagLogEventEraser::CDiagLogEventEraser()" )
    CActiveScheduler::Add( this );
    }

// ---------------------------------------------------------------------------
// The second phase constructor.
// ---------------------------------------------------------------------------
//
void CDiagLogEventEraser::ConstructL()
    {
    LOGSTRING( "CDiagLogEventEraser::ConstructL()" )

    User::LeaveIfError( iFsSession.Connect() );
    iWaitScheduler = new( ELeave ) CActiveSchedulerWait;
    iLogClient = CLogClient::NewL( iFsSession );
    iLogView = CLogViewEvent::NewL( *iLogClient );
    if ( !iLogFilter )
        {
        iLogFilter = CLogFilter::NewL();
        }
    }

// ---------------------------------------------------------------------------
// Handles the completion of log erasing and performs different operations
// depending on whether the erasing was asynchronous or not.
// ---------------------------------------------------------------------------
//
void CDiagLogEventEraser::HandleCompletionL( TInt aError )
    {
    LOGSTRING2( "CDiagLogEventEraser::HandleCompletionL( %d )", aError )

    // Reset the state so that the object can be reused.
    iState = EStateInitial;

    // For synchronous erasure, set the error code and stop the wait
    // scheduler, so that it will return in EraseLogsL.
    if ( iIsDeletingSynchronous )
        {
        iError = aError;
        iWaitScheduler->AsyncStop();
        }

    // For asynchronous erasure, inform the observer that erasing has
    // completed.
    else
        {
        iObserver.CompleteEventEraseL( aError );
        }
    }

// ---------------------------------------------------------------------------
// Handles the completion of active requests.
// ---------------------------------------------------------------------------
//
void CDiagLogEventEraser::RunL()
    {
    LOGSTRING( "CDiagLogEventEraser::RunL()" )

    // Check that the request completed successfully.
    if( iStatus != KErrNone )
        {
        HandleCompletionL( iStatus.Int() );
        return;
        }

    LOGSTRING2( "CDiagLogEventEraser::RunL() - State %d", iState )
    switch( iState )
        {
        case EStateApplyingFilter:
            {
            // The log view filter has been successfully applied.  Issue
            // another request to start processing log events.  Events are
            // read from last to first.  The reason for this arrangement is
            // that we are deleting entries while reading them.
            if( iLogView->LastL( iStatus ) )
                {
                iState = EStateReadingEntries;
                SetActive();
                }

            // There are no events in the view.
            else
                {
                HandleCompletionL( KErrNone );
                }

            break;
            }

        case EStateReadingEntries:
            {
            const CLogEvent& event = iLogView->Event();

            // The log event has been fetched successfully.  Ask the observer
            // if this event should be erased.  If so, issue another request
            // to begin erasing the event.
            if( iObserver.IsEventToBeDeleted( event ) )
                {
                iState = EStateDeletingEntry;
                iLogClient->DeleteEvent( event.Id(), iStatus );
                SetActive();
                }

            // The log event should not be erased, so get the next event.
            else if( iLogView->PreviousL( iStatus ) )
                {
                SetActive();
                }

            // There are no events in the view.
            else
                {
                HandleCompletionL( KErrNone );
                }

            break;
            }

        case EStateDeletingEntry:
            {
            // The log event was successfully deleted.  Issue another request
            // to read the next entry.
            if( iLogView->PreviousL( iStatus ) )
                {
                iState = EStateReadingEntries;
                SetActive();
                }

            // There are no events in the view.
            else
                {
                HandleCompletionL( KErrNone );
                }

            break;
            }

        case EStateComplete:
            {
            HandleCompletionL( KErrNone );
            break;
            }

        default:
            // This state should never be reached.
            LOGSTRING( "CDiagLogEventEraser::RunL() - Invalid State." )
            User::Invariant();
            break;
        }
    }

// ---------------------------------------------------------------------------
// Handles the cancellation of active requests.
// ---------------------------------------------------------------------------
//
void CDiagLogEventEraser::DoCancel()
    {
    LOGSTRING( "CDiagLogEventEraser::DoCancel()" )

    iLogView->Cancel();
    iLogClient->Cancel();
    iState = EStateInitial;
    }

// End of file
