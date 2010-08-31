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
* Description:  IrDA device and service searcher
*
*/



// INCLUDE FILES
#include "Irdaobexsearcher.h"
#include "ExtBTSearcherObserver.h"
#include <obex.h>


// CONSTANTS
_LIT( KIrTransportName,"IrTinyTP" );
_LIT8( KClassName,"SYNCML-SYNC" );
_LIT( KHostName, "*" );

_LIT( KIrObexSearcher, "Ir Obex searcher" );
const TInt KInvalidStatePanicVal = -1;
const TInt KDeviceSearchRetries = 10;		// amount of tries to find the device
const TInt KDeviceSearchFirstTry = 1;
const TInt KDeviceSearchTimeout = 500000;	// microseconds


// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CIrDAObexSearcher::CIrDAObexSearcher
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CIrDAObexSearcher::CIrDAObexSearcher()
	: iConnectionRetry( KDeviceSearchFirstTry )
    {
    }

// -----------------------------------------------------------------------------
// CIrDAObexSearcher::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CIrDAObexSearcher::ConstructL()
    {
	BaseConstructL();

	User::LeaveIfError( iSocketServer.Connect() );

	iSocketServer.FindProtocol( KIrTransportName(), iProtocolInfo );

	User::LeaveIfError( iHostResolver.Open(	iSocketServer, 
		iProtocolInfo.iAddrFamily, iProtocolInfo.iProtocol ) );

	User::LeaveIfError( iSocket.Open( iSocketServer, 
		iProtocolInfo.iAddrFamily, iProtocolInfo.iSockType, iProtocolInfo.iProtocol ) );

	TPckgBuf<TUint> buf(1);
	User::LeaveIfError( iSocket.SetOpt( KDiscoverySlotsOpt, KLevelIrlap, buf ) );

	iRetryTimer.CreateLocal();
    }

// -----------------------------------------------------------------------------
// CIrDAObexSearcher::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CIrDAObexSearcher* CIrDAObexSearcher::NewL()
    {
    CIrDAObexSearcher* self = new( ELeave ) CIrDAObexSearcher;
    
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();

    return self;
    }

//----------------------------------------------------------------------------
// ~CIrDAObexSearcher()
//----------------------------------------------------------------------------
//
CIrDAObexSearcher::~CIrDAObexSearcher()
    {
	Cancel();
	iRetryTimer.Close();
	iSocket.Close();
	iHostResolver.Close();
	iSocketServer.Close();
    }

//----------------------------------------------------------------------------
// CIrDAObexSearcher::RunL()
//----------------------------------------------------------------------------
//
void CIrDAObexSearcher::RunL()
	{
	TInt err( iStatus.Int() );

	if ( iState != EDeviceSearchTimeout )
		{
		iInitialErr = err;
		}

	if ( err != KErrNone )
		{
		switch ( iState )
			{
			case EDeviceSearch:
				{
				if ( iConnectionRetry < KDeviceSearchRetries )
					{
					// Try again after short pause
					iConnectionRetry++;
					//DBG_ARGS8( "Failed, trying again %d more time(s)",
					//				KDeviceSearchRetries - iConnectionRetry );

					iState = EDeviceSearchTimeout;

					iRetryTimer.After( iStatus, KDeviceSearchTimeout );
					SetActive();
					}
				else
					{
					NotifyDeviceErrorL( iInitialErr );
					// Reinitialize retry counter
					iConnectionRetry = KDeviceSearchFirstTry;
					}
				break;
				}
			case EDeviceSearchTimeout:
				{
				// We should never end up here, but if we do, continue
				// as if nothing happened.
				SearchDeviceL();
				break;
				}
			case EServiceSearch:
				{
				NotifyServiceErrorL( iInitialErr );
				break;
				}
			default:
				{
				User::Panic( KIrObexSearcher, KInvalidStatePanicVal );
				break;
				}
			}
		}
	else
		{
		switch ( iState )
			{
			case EDeviceSearch:
				{
				NotifyDeviceFoundL();
				break;
				}
			case EDeviceSearchTimeout:
				{
				SearchDeviceL();
				break;
				}
			case EServiceSearch:
				{
				NotifyServiceFoundL();
				break;
				}
			default:
				{
				User::Panic( KIrObexSearcher, KInvalidStatePanicVal );
				break;
				}

			}
		}
	}

TInt CIrDAObexSearcher::RunError ( TInt /*aError*/ )
    {
        return KErrNone;
    }	
//----------------------------------------------------------------------------
// CIrDAObexSearcher::DoCancel()
//----------------------------------------------------------------------------
//
void CIrDAObexSearcher::DoCancel()
	{
	iRetryTimer.Cancel();
	}

//----------------------------------------------------------------------------
// CIrDAObexSearcher::SearchDeviceL()
//----------------------------------------------------------------------------
//
void CIrDAObexSearcher::SearchDeviceL()
	{
	//DBG_FILE( "CIrDAObexSearcher::SearchDeviceL()" );
	
	iState = EDeviceSearch;

	iHostResolver.GetByName( KHostName, iLog, iStatus ); // Finds all IrLAP stations.
	SetActive();
	}

//----------------------------------------------------------------------------
// CIrDAObexSearcher::SearchServiceL()
//----------------------------------------------------------------------------
//
void CIrDAObexSearcher::SearchServiceL()
	{
	//DBG_FILE( "CIrDAObexSearcher::SearchServiceL()" );

	//Just simulate successful service search.
	//This function could be advanced by executing
	//IAS-queries to remote phone and deciding
	//the success of SearchServiceL depending on the
	//responses of the IAS queries.
	iState = EServiceSearch;
	TRequestStatus* status = &iStatus;
	User::RequestComplete( status, KErrNone );
	SetActive();
	}
 
//----------------------------------------------------------------------------
// CIrDAObexSearcher::CreateObexClientL()
//----------------------------------------------------------------------------
//
CObexClient* CIrDAObexSearcher::CreateObexClientL()
	{
	//DBG_FILE( "CIrDAObexSearcher::CreateObexClientL()" );

	TObexIrProtocolInfo pInfo;
	pInfo.iTransport.Copy( KIrTransportName );

	if( iRetryConn )
		{
		// ISA device
		pInfo.iClassName.Copy( _L("OBEX") );
		}
	else
		{
		pInfo.iClassName.Copy( KClassName );
		iRetryConn = ETrue;
		}

	pInfo.iAttributeName.Copy( _L8("IrDA:TinyTP:LsapSel") );
	pInfo.iAddr = iLog().iAddr;

	CObexClient* client = CObexClient::NewL( pInfo );
	return client;
	}


//  End of File  
