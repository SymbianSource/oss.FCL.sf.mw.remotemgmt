/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  BT device and service searcher
*
*/



// INCLUDE FILES
#include    <btnotif.h>
#include    "Btobexsearcher.h"
#include	"NSmlObexServiceSearcher.h"
#include	"ExtBTSearcherObserver.h"

// CONSTANTS
_LIT( KBTTransportName,"RFCOMM" );

// LOCAL CONSTANTS AND MACROS
_LIT( KBTObexSearcher, "BT Obex searcher" );
const TInt KInvalidStatePanicVal = -1;


// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CBTObexSearcher::CBTObexSearcher
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CBTObexSearcher::CBTObexSearcher()
    {
    }

// -----------------------------------------------------------------------------
// CBTObexSearcher::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CBTObexSearcher::ConstructL( const TBTConnInfo& aBTConnInfo )
    {
	BaseConstructL();
    iServiceSearcher = CNSmlObexServiceSearcher::NewL( aBTConnInfo );
    iSettings = CBTEngSettings::NewL( this );
    }

// -----------------------------------------------------------------------------
// CBTObexSearcher::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CBTObexSearcher* CBTObexSearcher::NewL( const TBTConnInfo& aBTConnInfo )
    {
    CBTObexSearcher* self = new( ELeave ) CBTObexSearcher;
    
    CleanupStack::PushL( self );
    self->ConstructL( aBTConnInfo );
    CleanupStack::Pop();

    return self;
    }

//----------------------------------------------------------------------------
// ~CBTObexSearcher()
//----------------------------------------------------------------------------
//
CBTObexSearcher::~CBTObexSearcher()
    {
	Cancel();
    delete iServiceSearcher;
    delete iSettings;   
/*    if ( iNotifier.Handle() )
        {
        iNotifier.Close();
        }
*/        
    }

//----------------------------------------------------------------------------
// CBTObexSearcher::RunL()
//----------------------------------------------------------------------------
//
void CBTObexSearcher::RunL()
	{

	TInt err( iStatus.Int() );
	if ( err != KErrNone )
		{
		switch ( iState )
			{
            case EIdle:
                {
                if( iWaitingForBTPower )
                    {
                    NotifyDeviceErrorL( err );
                    }
                break;
                }
			case EDeviceSearch:
				{
				NotifyDeviceErrorL( err );
				//Notify external observer
				if ( iExtObserver )
					{
					iExtObserver->BTDeviceErrorL( err );
					}
				break;
				}
			case EServiceSearch:
				{
				NotifyServiceErrorL( err );
				break;
				}
			default:
				{
				User::Panic( KBTObexSearcher, KInvalidStatePanicVal );
				break;
				}

			}
		}
	else
		{
		switch ( iState )
			{
            case EIdle:
                {
                if( iWaitingForBTPower && iOffline() != EFalse )
                    {
 /*                   if ( iNotifier.Handle() )
				        {
				        iNotifier.Close();
				        }
 */
                    TInt err = iSettings->ChangePowerStateTemporarily( );
                    if( err )
                        {
                        NotifyDeviceErrorL( err );
                        }
                    }
                else
                    {
                    NotifyDeviceErrorL( KErrCancel );
                    }
                break;
                }
			case EDeviceSearch:
				{
				NotifyDeviceFoundL();
				//Notify external observer
				if ( iExtObserver )
					{
					iExtObserver->BTDeviceFoundL( iServiceSearcher->ResponseParams() );
					}

				break;
				}
			case EServiceSearch:
				{
				NotifyServiceFoundL();
				break;
				}
			default:
				{
				User::Panic( KBTObexSearcher, KInvalidStatePanicVal );
				break;
				}

			}
		}
	}
// ----------------------------------------------------------------------------
// CBTObexSearcher::RunError
// ----------------------------------------------------------------------------
TInt CBTObexSearcher::RunError ( TInt /*aError*/ )
    {
        return KErrNone;
	}

//----------------------------------------------------------------------------
// CBTObexSearcher::DoCancel()
//----------------------------------------------------------------------------
//
void CBTObexSearcher::DoCancel()
	{	
	delete iServiceSearcher;
	iServiceSearcher = 0;
	}

//----------------------------------------------------------------------------
// CBTObexSearcher::SearchDeviceL()
//----------------------------------------------------------------------------
//
void CBTObexSearcher::SearchDeviceL()
	{
    iWaitingForBTPower = ETrue;
    TBTPowerStateValue power = EBTPowerOff;
    User::LeaveIfError( iSettings->GetPowerState( power ) );
    TBool offline = EFalse;
    
    if( power == EBTPowerOff )
        {
        offline = CheckOfflineModeL();
        }
    if( !offline )
        {
    	// Even if BT is already on, we still register as "temp BT user".
        User::LeaveIfError( iSettings->ChangePowerStateTemporarily(  ) );
        }
        if( power == EBTPowerOn )
        {
        // start to search immediately.
        PowerStateChanged( power );
        }
   	}

//----------------------------------------------------------------------------
// CBTObexSearcher::SearchServiceL()
//----------------------------------------------------------------------------
//
void CBTObexSearcher::SearchServiceL()
	{
	SetActive();
	iState = EServiceSearch;
	iStatus = KRequestPending;
	iServiceSearcher->FindServiceL( iStatus );
	}

//----------------------------------------------------------------------------
// CBTObexSearcher::CreateObexClientL()
//----------------------------------------------------------------------------
//
CObexClient* CBTObexSearcher::CreateObexClientL()
	{
	TObexBluetoothProtocolInfo protocolInfo;

	protocolInfo.iTransport.Copy( KBTTransportName );
	protocolInfo.iAddr.SetBTAddr( iServiceSearcher->BTDevAddr() );
	protocolInfo.iAddr.SetPort( iServiceSearcher->Port() );

	CObexClient* obexClient = CObexClient::NewL( protocolInfo );
	return obexClient;
	}


//----------------------------------------------------------------------------
// CBTObexSearcher::PowerStateChanged
// Bluetooth power state has changed; start searching devices if it is on.
//----------------------------------------------------------------------------
//
void CBTObexSearcher::PowerStateChanged( TBTPowerStateValue aState )
    {
    if( iWaitingForBTPower && aState == EBTPowerOn )
        {
        iWaitingForBTPower = EFalse;
        TRAPD( err, iServiceSearcher->SelectDeviceByDiscoveryL( iStatus ) );
        if( !err )
            {
            iState = EDeviceSearch;
            SetActive();
            }
        else
            {
            TRAP_IGNORE(NotifyDeviceErrorL( err ));
            }
        }
    }


//----------------------------------------------------------------------------
// CBTObexSearcher::VisibilityModeChanged
// Notification of changes in the discoverability mode, ignored.
//----------------------------------------------------------------------------
//
void CBTObexSearcher::VisibilityModeChanged( TBTVisibilityMode /*aState*/ )
    {
    
    }


//----------------------------------------------------------------------------
// CBTObexSearcher::CheckOfflineModeL
// Checks the offline mode and queries the user for turning BT on if required.
//----------------------------------------------------------------------------
//
TBool CBTObexSearcher::CheckOfflineModeL()
    {
    TCoreAppUIsNetworkConnectionAllowed offline = ECoreAppUIsNetworkConnectionNotAllowed;
    TBTEnabledInOfflineMode offlineAllowed = EBTDisabledInOfflineMode;
    User::LeaveIfError( iSettings->GetOfflineModeSettings( offline, offlineAllowed ) );
    if( offline == ECoreAppUIsNetworkConnectionNotAllowed && 
         offlineAllowed == EBTEnabledInOfflineMode )
        {
/*        User::LeaveIfError( iNotifier.Connect() );
        TBTGenericQueryNotiferParamsPckg pckg;
        pckg().iMessageType = EBTActivateOffLineQuery;
        pckg().iNameExists = EFalse;
        iNotifier.StartNotifierAndGetResponse( iStatus, KBTGenericQueryNotifierUid, 
                                                pckg, iOffline );
        SetActive();
*/        
        }
    else if( offline == ECoreAppUIsNetworkConnectionNotAllowed && 
            offlineAllowed == EBTDisabledInOfflineMode )
           {
           NotifyDeviceErrorL( KErrNotSupported );
           }
    return ( offline == ECoreAppUIsNetworkConnectionNotAllowed );
    }
//  End of File    
