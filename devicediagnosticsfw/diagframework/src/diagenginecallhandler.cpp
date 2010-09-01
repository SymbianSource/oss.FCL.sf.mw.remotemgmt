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
* Description:  Class definition of CDiagEngineCallHandler
*
*/


// CLASS DECLARATION
#include "diagenginecallhandler.h"   

// SYSTEM INCLUDE FILES
#include <DiagFrameworkDebug.h>             // LOGSTRING
#include <badesca.h>                        // CDesCArrayFlat

// USER INCLUDE FILES
#include "diagenginecallhandlerobserver.h"  // MDiagEngineCallHandlerObserver


// CONSTANTS

// ======== LOCAL FUNCTIONS ========

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// CDiagEngineCallHandler::NewL
// ---------------------------------------------------------------------------
//
CDiagEngineCallHandler* CDiagEngineCallHandler::NewL( 
        MDiagEngineCallHandlerObserver& aObserver )
    {
    CDiagEngineCallHandler* self = new( ELeave )CDiagEngineCallHandler( aObserver );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }


// ---------------------------------------------------------------------------
// CDiagEngineCallHandler::CDiagEngineCallHandler
// ---------------------------------------------------------------------------
//
CDiagEngineCallHandler::CDiagEngineCallHandler( 
        MDiagEngineCallHandlerObserver& aObserver )
    :   CActive( EPriorityStandard ),
        iObserver( aObserver ),
        iCallStatus(),
        iCallStatusPckg( iCallStatus )
    {
    }

// ---------------------------------------------------------------------------
// CDiagEngineCallHandler::ConstructL
// ---------------------------------------------------------------------------
//
void CDiagEngineCallHandler::ConstructL()
    {
    CActiveScheduler::Add( this );

    iCallIgnoreList = new( ELeave )CDesCArrayFlat( 1 );
    
    iTelephony = CTelephony::NewL();

    // Get call initial status.
    TInt err = iTelephony->GetLineStatus( CTelephony::EVoiceLine, iCallStatusPckg );

    if ( err != KErrNone )
        {
        // This can happen even if hw failure does not occur. E.g.
        // user puts the phone in offline mode, or sim card is missing.
        // In this situation, assume that phone is offline and continue
        // since it does not actually affect engine execution.
        LOGSTRING2( "CDiagEngineCallHandler::ConstructL() "
            L"Unable to get voice line status. Err = %d", err )
        iState = EDiagEngineCallHandlerStateIdle;
        }
    else
        {
        LOGSTRING2( "CDiagEngineCallHandler::ConstructL() Call status = %d",
            iCallStatus.iStatus )
        if ( iCallStatus.iStatus != CTelephony::EStatusIdle )
            {
            iState = EDiagEngineCallHandlerStateBusy;
            }
        }

    RequestNotify();
    }

// ---------------------------------------------------------------------------
// CDiagEngineCallHandler::~CDiagEngineCallHandler
// ---------------------------------------------------------------------------
//
CDiagEngineCallHandler::~CDiagEngineCallHandler()
    {
    Cancel();

    delete iTelephony;
    iTelephony = NULL;

    delete iCallIgnoreList;
    iCallIgnoreList = NULL;
    }

// ---------------------------------------------------------------------------
// CDiagEngineCallHandler::AddIgnoreNumberL
// ---------------------------------------------------------------------------
//
void CDiagEngineCallHandler::AddIgnoreNumberL( const TDesC& aNumber )
    {
    LOGSTRING( "CDiagEngineCallHandler::AddIgnoreNumberL: Number = " )
    LOGTEXT( aNumber )

    iCallIgnoreList->AppendL( aNumber );
    }

// ---------------------------------------------------------------------------
// CDiagEngineCallHandler::RemoveIgnoreNumberL
// ---------------------------------------------------------------------------
//
void CDiagEngineCallHandler::RemoveIgnoreNumberL( const TDesC& aNumber )
    {
    LOGSTRING( "CDiagEngineCallHandler::RemoveIgnoreNumberL: Number = " )
    LOGTEXT( aNumber )

    TInt position = 0;

    TInt err = iCallIgnoreList->Find( aNumber, position );

    if ( err == KErrNone )
        {
        // element found. Remove.
        iCallIgnoreList->Delete( position );
        }
    else
        {
        LOGSTRING2( "CDiagEngineCallHandler::RemoveIgnoreNumberL:"
                L"Not found. Err = %d", err )
        User::Leave( err );
        }
    }

// ---------------------------------------------------------------------------
// CDiagEngineCallHandler::CurrentState
// ---------------------------------------------------------------------------
//
TDiagEngineCallHandlerState CDiagEngineCallHandler::CurrentState() const
    {
    return iState;
    }

// ---------------------------------------------------------------------------
// CDiagEngineCallHandler::RequestNotify
// ---------------------------------------------------------------------------
//
void CDiagEngineCallHandler::RequestNotify()
    {
    Cancel();
    iTelephony->NotifyChange(
        iStatus, CTelephony::EVoiceLineStatusChange, iCallStatusPckg );
    SetActive();
    }


// ---------------------------------------------------------------------------
// from CActive
// CDiagEngineCallHandler::RunL()
// ---------------------------------------------------------------------------
//
void CDiagEngineCallHandler::RunL()
    {
    TBool notifyObserver = EFalse;
    
    LogCallStatus();    

    switch ( iCallStatus.iStatus )
        {
        case CTelephony::EStatusIdle:
            notifyObserver = HandleIdle();
            break;

        case CTelephony::EStatusDialling:   // fall through
        case CTelephony::EStatusRinging:    
            notifyObserver = HandleCall();
            break;

        default:
            // Ignore. 
            break;
        }

    if ( iStatus != KErrCancel )
        {
        RequestNotify();
        }

    if ( notifyObserver )
        {
        iObserver.CallHandlerStateChangedL( iState );
        }
    }

// ---------------------------------------------------------------------------
// from CActive
// CDiagEngineCallHandler::DoCancel()
// ---------------------------------------------------------------------------
//
void CDiagEngineCallHandler::DoCancel()
    {
    if ( iTelephony )
        {
        iTelephony->CancelAsync( CTelephony::EVoiceLineStatusChangeCancel );
        }
    }
    

// ---------------------------------------------------------------------------
// CDiagEngineCallHandler::HandleIdle
// ---------------------------------------------------------------------------
//
TBool CDiagEngineCallHandler::HandleIdle()
    {
    LOGSTRING( "CDiagEngineCallHandler::HandleIdle()" )

    TBool notifyObserver = EFalse;
    if ( iState != EDiagEngineCallHandlerStateIdle )
        {
        LOGSTRING( "CDiagEngineCallHandler::HandleIdle() Changing to IDLE" )
        iState = EDiagEngineCallHandlerStateIdle;
        notifyObserver = ETrue;
        }

    return notifyObserver;
    }

// ---------------------------------------------------------------------------
// CDiagEngineCallHandler::HandleCall
// ---------------------------------------------------------------------------
//
TBool CDiagEngineCallHandler::HandleCall()
    {
    LOGSTRING( "CDiagEngineCallHandler::HandleCall()" )

    TBool notifyObserver = EFalse;

    if ( iState == EDiagEngineCallHandlerStateIdle )
        {
        LOGSTRING( "CDiagEngineCallHandler::HandleCall() was in Idle" )

        // Get phone number
        TBuf<CTelephony::KMaxTelNumberSize> number;

        CTelephony::TRemotePartyInfoV1 remInfoUse;
        CTelephony::TCallInfoV1        callInfoUse;
        CTelephony::TCallSelectionV1   callSelectionUse;

        callSelectionUse.iLine = CTelephony::EVoiceLine;
        callSelectionUse.iSelect = CTelephony::EInProgressCall;

        CTelephony::TRemotePartyInfoV1Pckg  remParty( remInfoUse );
        CTelephony::TCallInfoV1Pckg         callInfo( callInfoUse );
        CTelephony::TCallSelectionV1Pckg    callSelection( callSelectionUse );

        iTelephony->GetCallInfo( callSelection, callInfo, remParty );

        LOGSTRING( "CDiagEngineCallHandler::HandleCall() Remote Number = " )
        LOGTEXT( remInfoUse.iRemoteNumber.iTelNumber )
    
        LOGSTRING( "CDiagEngineCallHandler::HandleCall() Dialed Number = " )
        LOGTEXT( callInfoUse.iDialledParty.iTelNumber )

        if ( iCallStatus.iStatus == CTelephony::EStatusRinging )
            {
            number.Copy( remInfoUse.iRemoteNumber.iTelNumber );
            }
        else if ( iCallStatus.iStatus == CTelephony::EStatusDialling )
            {
            number.Copy( callInfoUse.iDialledParty.iTelNumber );
            }
        else
            {
            // number not valid.
            }

        // check if the number is in ignore list
        TInt position = 0;
        TInt err = iCallIgnoreList->Find( number, position );

        if ( err == KErrNone )
            {
            // number found.
            LOGSTRING( "CDiagEngineCallHandler::HandleCall() Ignore: Number =" )
            LOGTEXT( number )
            }
        else
            {
            // number found.
            LOGSTRING( "CDiagEngineCallHandler::HandleCall() Going to Busy State. Number = " )
            LOGTEXT( number )

            iState = EDiagEngineCallHandlerStateBusy;

            notifyObserver = ETrue;
            }
        }

    return notifyObserver;
    }

// ---------------------------------------------------------------------------
// CDiagEngineCallHandler::LogCallStatus
// ---------------------------------------------------------------------------
//
void CDiagEngineCallHandler::LogCallStatus() const
    {
    #ifdef _DEBUG

    switch ( iCallStatus.iStatus )
        {
        case CTelephony::EStatusUnknown:
            LOGSTRING( "CDiagEngineCallHandler::LogCallStatus():"
                    L" Call Status = EStatusUnknown" )
            break;

        case CTelephony::EStatusIdle:
            LOGSTRING( "CDiagEngineCallHandler::LogCallStatus():"
                    L" Call Status = EStatusIdle" )
            break;

        case CTelephony::EStatusDialling:
            LOGSTRING( "CDiagEngineCallHandler::LogCallStatus():"
                    L" Call Status = EStatusDialling" )
            break;

        case CTelephony::EStatusRinging:
            LOGSTRING( "CDiagEngineCallHandler::LogCallStatus():"
                    L" Call Status = EStatusRinging" )
            break;

        case CTelephony::EStatusAnswering:
            LOGSTRING( "CDiagEngineCallHandler::LogCallStatus():"
                    L" Call Status = EStatusAnswering" )
            break;

        case CTelephony::EStatusConnecting:
            LOGSTRING( "CDiagEngineCallHandler::LogCallStatus():"
                    L" Call Status = EStatusConnecting" )
            break;

        case CTelephony::EStatusConnected:
            LOGSTRING( "CDiagEngineCallHandler::LogCallStatus():"
                    L" Call Status = EStatusConnected" )
            break;

        case CTelephony::EStatusReconnectPending:
            LOGSTRING( "CDiagEngineCallHandler::LogCallStatus():"
                    L" Call Status = EStatusReconnectPending" )
            break;

        case CTelephony::EStatusDisconnecting:
            LOGSTRING( "CDiagEngineCallHandler::LogCallStatus():"
                    L" Call Status = EStatusDisconnecting" )
            break;

        case CTelephony::EStatusHold:
            LOGSTRING( "CDiagEngineCallHandler::LogCallStatus():"
                    L" Call Status = EStatusHold" )
            break;

        case CTelephony::EStatusTransferring:
            LOGSTRING( "CDiagEngineCallHandler::LogCallStatus():"
                    L"Call Status = EStatusTransferring" )
            break;

        case CTelephony::EStatusTransferAlerting:
            LOGSTRING( "CDiagEngineCallHandler::LogCallStatus():"
                    L"Call Status = EStatusTransferAlerting" )
            break;

        default:
            LOGSTRING2( "CDiagEngineCallHandler::LogCallStatus():"
                    L"Unknown Call Status = %d", iCallStatus.iStatus )
            break;
        }

    #endif // _DEBUG
    }

// End of File

