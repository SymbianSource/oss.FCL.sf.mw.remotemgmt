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
* Description:  This is the implementation for the Diagnostics Network 
*                 registration status watcher.
*
*/


// CLASS DECLARATION
#include <DiagNetworkRegStatusWatcher.h>

// SYSTM INCLUDE FILES
#include <e32base.h>                        
#include <DiagFrameworkDebug.h>             // Debugging macros
#include <DiagNetworkRegStatusObserver.h>   // MDiagNetworkRegStatusObserver


// ============================ MEMBER FUNCTIONS =============================

// ---------------------------------------------------------------------------
// Static two-phase constructor.
// ---------------------------------------------------------------------------
//
EXPORT_C CDiagNetworkRegStatusWatcher* CDiagNetworkRegStatusWatcher::NewL(
    MDiagNetworkRegStatusObserver& aCallBack )
    {
    CDiagNetworkRegStatusWatcher* self = new ( ELeave )
                                       CDiagNetworkRegStatusWatcher( aCallBack );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }


// ---------------------------------------------------------------------------
// Destructor.
// ---------------------------------------------------------------------------
//
EXPORT_C CDiagNetworkRegStatusWatcher::~CDiagNetworkRegStatusWatcher()
    {
    // cancel any pending request
    Cancel();
    
    delete iTelephony;
    iTelephony = NULL;
    
    LOGSTRING( "CDiagNetworkRegStatusWatcher::ConstructL() Destroying Network registration status watcher" )            
    }

// ---------------------------------------------------------------------------
// The default constructor.
// ---------------------------------------------------------------------------
//
CDiagNetworkRegStatusWatcher::CDiagNetworkRegStatusWatcher
    ( MDiagNetworkRegStatusObserver& aCallBack )
    : CActive( EPriorityStandard ),
    iState( EStateInit ),
    iRegV1(),
    iRegV1Pckg( iRegV1 ),
    iCallBack( aCallBack )
    {
    CActiveScheduler::Add( this );
    }


// ---------------------------------------------------------------------------
// The second phase constructor.
// ---------------------------------------------------------------------------
//

void CDiagNetworkRegStatusWatcher::ConstructL()
    {
    LOGSTRING( "CDiagNetworkRegStatusWatcher::ConstructL() Creating Network registration status watcher" )
    iTelephony = CTelephony::NewL();     
    iPreviouslyRegistered = EFalse;
    }


// ---------------------------------------------------------------------------
// This function starts the network registration status monitor process. This
// involves first checking for the current status and then registering for any
// changes that may occur.
// ---------------------------------------------------------------------------
//

EXPORT_C void CDiagNetworkRegStatusWatcher::StartObserver()
    {
    // get the current status of network registration
    iState = EStateWaitForInitialStatus;
    iTelephony->GetNetworkRegistrationStatus( iStatus, iRegV1Pckg );
    SetActive();        
    }


// ---------------------------------------------------------------------------
// This function stops the network registration monitor process
// ---------------------------------------------------------------------------
//
EXPORT_C void CDiagNetworkRegStatusWatcher::StopObserver()
    {
    Cancel();
    }

// ---------------------------------------------------------------------------
// Handles the completion of active requests.
// ---------------------------------------------------------------------------
//
void CDiagNetworkRegStatusWatcher::RunL()
    {
    switch ( iState )
        {
        case EStateWaitForInitialStatus:
            {
            HandleInitialStatusL();
            }
            break;

        case EStateWaitForStatusChange:
            {
            HandleNetworkStatusChangeL();
            }
            break;
            
        default:
            break;                                    
        }
    }



// ---------------------------------------------------------------------------
// Handles the request for initial network registration status
// ---------------------------------------------------------------------------
//
void CDiagNetworkRegStatusWatcher::HandleInitialStatusL()
    {
    if ( iStatus == KErrNone )
        {  
        // get the current registration status
        TBool currentRegStatus = IsDeviceRegisteredOnNetwork();

        LOGSTRING2( "CDiagNetworkRegStatusWatcher::HandleInitialStatus : current status = %d", currentRegStatus )
        
        // we have to continue monitoring changes in network registration status
        // so set the next asynchronous request
        iState = EStateWaitForStatusChange;            
        iTelephony->NotifyChange( iStatus, 
                    CTelephony::ENetworkRegistrationStatusChange, iRegV1Pckg );
        SetActive();

        // inform the client about the initial registration status
        iPreviouslyRegistered = currentRegStatus;
        iCallBack.InitialNetworkRegistrationStatusL( currentRegStatus );                
        }
    else
        {
        // inform the client that device is not registered
        iCallBack.InitialNetworkRegistrationStatusL( EFalse );
        iPreviouslyRegistered = EFalse;        
        }
    }
    


// ---------------------------------------------------------------------------
// Handles the events related to Network registration status change
// ---------------------------------------------------------------------------
//
void CDiagNetworkRegStatusWatcher::HandleNetworkStatusChangeL()
    {
    if ( iStatus == KErrNone )
        {
        // get the current registration status
        TBool currentRegStatus = IsDeviceRegisteredOnNetwork();

        LOGSTRING2( "CDiagNetworkRegStatusWatcher::HandleNetworkStatusChange : current status = %d", currentRegStatus )
        
        // we have to continue monitoring changes in network registration status
        // so set the next asynchronous request
        iState = EStateWaitForStatusChange;            
        iTelephony->NotifyChange( iStatus, 
                    CTelephony::ENetworkRegistrationStatusChange, iRegV1Pckg );
        SetActive();

        // if the current registration state is different from previous state,
        // inform the client
        if ( currentRegStatus != iPreviouslyRegistered )
            {
            // inform the client about the network status change
            iCallBack.NetworkRegistrationStatusChangeL( currentRegStatus );
            }
        iPreviouslyRegistered = currentRegStatus;
        }
    else
        {
        iCallBack.NetworkRegistrationStatusChangeL( EFalse );
        iPreviouslyRegistered = EFalse;            
        }
    }



// ---------------------------------------------------------------------------
// This function wraps the logic if the device is currently registered or not
// ---------------------------------------------------------------------------
//
TBool CDiagNetworkRegStatusWatcher::IsDeviceRegisteredOnNetwork()
    {
    TBool regStatus = EFalse;
    switch ( iRegV1.iRegStatus )
        {
            case CTelephony::ERegisteredOnHomeNetwork:
            case CTelephony::ERegisteredRoaming:
                {    
                regStatus = ETrue; 
                }
                break;
            default:
                {
                // the device lost the network coverage
                regStatus = EFalse;
                }
                break;                    
        }
    return regStatus;
    }

// ---------------------------------------------------------------------------
// Handles the cancellation of active requests.
// ---------------------------------------------------------------------------
//
void CDiagNetworkRegStatusWatcher::DoCancel()
    {
    switch ( iState )
        {
            case EStateWaitForInitialStatus:
                {
                iTelephony->CancelAsync( CTelephony::EGetNetworkRegistrationStatusCancel );
                }
                break;

            case EStateWaitForStatusChange:
                {
                iTelephony->CancelAsync( CTelephony::ENetworkRegistrationStatusChangeCancel );
                }
                break;
                
            default:
                break;                                    
        }
    }

// End of file

