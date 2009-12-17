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
* Description:  array for storing dataproviders and datastores.
*
*/


// INCLUDE FILES
#include <s32std.h>

#include <nsmldebug.h>
#include "nsmldshostconstants.h"
#include "Nsmldsasyncrequesthandler.h"
#include "Nsmldsdataproviderarray.h"

// ======================================= MEMBER FUNCTIONS =======================================

// ------------------------------------------------------------------------------------------------
// CNSmlDSAsyncRequestHandler::~CNSmlDSAsyncRequestHandler
// ------------------------------------------------------------------------------------------------    
CNSmlDSAsyncRequestHandler::~CNSmlDSAsyncRequestHandler()
	{
	if ( iDSItem )
    	{
    	iDSItem->iDSAO = NULL;
    	}
	}
	
// ------------------------------------------------------------------------------------------------
// CNSmlDSAsyncRequestHandler::CallDSAsyncL
// ------------------------------------------------------------------------------------------------
CSmlDataStore& CNSmlDSAsyncRequestHandler::CallDSAsyncLC()
    {
    AddToSchedulerL();
    if ( !iDSItem )
	    {
	    User::Leave( KErrUnknown );
	    }
    	
    if ( !iDSItem->iDataStore )
	    {
	    User::Leave( KErrUnknown );
	    }
	SetRequestStatusOnLeaveLC();
	SetActive();
	    
    return *( iDSItem->iDataStore );
    }
    
// ------------------------------------------------------------------------------------------------
// CNSmlDSAsyncRequestHandler::SetRequestStatusOnLeaveLC
// ------------------------------------------------------------------------------------------------	
void CNSmlDSAsyncRequestHandler::SetRequestStatusOnLeaveLC()
    {
    CleanupStack::PushL(TCleanupItem ( SetRequestStatusOnLeaveCleanup, this ) );
    }    
    
    
// ------------------------------------------------------------------------------------------------
// CNSmlDSAsyncRequestHandler::SetRequestStatusOnLeaveCleanup
// ------------------------------------------------------------------------------------------------
void CNSmlDSAsyncRequestHandler::SetRequestStatusOnLeaveCleanup( TAny* aP )
	{
	CNSmlDSAsyncRequestHandler* self = reinterpret_cast<CNSmlDSAsyncCallBack*>( aP );
    TRequestStatus *status = &self->iStatus;
    User::RequestComplete( status, KErrNone );
	}
    
// ------------------------------------------------------------------------------------------------
// CNSmlDSAsyncRequestHandler::SetActive
// ------------------------------------------------------------------------------------------------
void CNSmlDSAsyncRequestHandler::SetActive()
	{
	CActive::SetActive();
	}    
	
// ------------------------------------------------------------------------------------------------
// CNSmlDSAsyncRequestHandler::CNSmlDSAsyncRequestHandler
// ------------------------------------------------------------------------------------------------
CNSmlDSAsyncRequestHandler::CNSmlDSAsyncRequestHandler( TInt aPriority, 
		CNSmlDSHostSession* aSession, 
        TNSmlDSDataStoreElement* aDSItem, 
        const RMessagePtr2& aMessage ) :
    CActive( aPriority ), iSession( aSession ), 
        iDSItem( aDSItem ), iMessage( aMessage )
    {
    ASSERT( aSession != NULL );
    }

// ------------------------------------------------------------------------------------------------
// CNSmlDSAsyncRequestHandler::AddToSchedulerL
// ------------------------------------------------------------------------------------------------
void CNSmlDSAsyncRequestHandler::AddToSchedulerL()
	{
	ASSERT( iDSItem != NULL );
	ASSERT( iDSItem->iDataStore != NULL );
	
	if ( iDSItem->iDSAO )
        {
        //error: asyncronous call already outstanding. Only one allowed at any one time.
        User::Leave( ENSmlDSHostServerErrAsynchCallOutstanding );
        }
	iDSItem->iDSAO = this;
    CActiveScheduler::Add( this );
	}

// ------------------------------------------------------------------------------------------------
// CNSmlDSAsyncRequestHandler::RunError
// ------------------------------------------------------------------------------------------------
TInt CNSmlDSAsyncRequestHandler::RunError( TInt aError )
	{
	iMessage.Complete( aError );
	delete this;
	return KErrNone;
	}
	
// End of File
