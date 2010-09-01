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
#include <s32mem.h>

#include <nsmldebug.h>
#include "nsmldshostconstants.h"
#include "Nsmldsasyncrequesthandler.h"
#include "Nsmldsdataproviderarray.h"

#ifdef __HOST_SERVER_MTEST__
#include "../../stif/DSHostServerTest/inc/fakedataprovider.h"
#else
#include <SmlDataProvider.h>
#endif

// ======================================= MEMBER FUNCTIONS =======================================

// ------------------------------------------------------------------------------------------------
// CNSmlDSAsyncCallBack::CNSmlDSAsyncCallBack
// ------------------------------------------------------------------------------------------------
CNSmlDSAsyncCallBack::CNSmlDSAsyncCallBack( CNSmlDSHostSession* aSession, 
                           TNSmlDSDataStoreElement* aDSItem, 
                           const RMessage2& aMessage, 
                           RequestFinishedFunction aReqFinishedFunc,
                           TAny* aPtr ) :
    CNSmlDSAsyncRequestHandler( EPriorityStandard, aSession, aDSItem, aMessage ), 
    iReqFinishedFunc( aReqFinishedFunc )
    {
    iPtr = aPtr;
    }
    
// ------------------------------------------------------------------------------------------------
// CNSmlDSAsyncCallBackForOpen::CNSmlDSAsyncCallBackForOpen
// ------------------------------------------------------------------------------------------------    
CNSmlDSAsyncCallBackForOpen::CNSmlDSAsyncCallBackForOpen( 
		CNSmlDSHostSession* aSession, 
        const RMessage2& aMessage,
        RequestFinishedFunction aReqFinishedFunc ) : 
    CNSmlDSAsyncCallBack(aSession, NULL, aMessage, aReqFinishedFunc ),
    iServerId(NULL), iRemoteDB(NULL)
	{
	}
	
// ------------------------------------------------------------------------------------------------
// CNSmlDSAsyncCallBack::~CNSmlDSAsyncCallBack()
// ------------------------------------------------------------------------------------------------
CNSmlDSAsyncCallBack::~CNSmlDSAsyncCallBack()
    {
    Cancel();
    	
    if ( iReqFinishedFunc )
        {
        RequestFinishedFunction ReqFinFunc = iReqFinishedFunc;
        iReqFinishedFunc = NULL;
        ( *iSession.*ReqFinFunc )( this, EFree );
        
        ASSERT( iPtr == NULL );
        }
    }
    
// ------------------------------------------------------------------------------------------------
// CNSmlDSAsyncCallBack::DoCancel()
// ------------------------------------------------------------------------------------------------
void CNSmlDSAsyncCallBack::DoCancel()
    {
    if ( iDSItem )
    	{
    	iDSItem->iDataStore->CancelRequest();
    	}

    if ( iReqFinishedFunc != NULL )
    	{
    	( *iSession.*iReqFinishedFunc )( this, ECanceled );
    	iMessage.Complete( KErrCancel);
    	}
    }

// ------------------------------------------------------------------------------------------------
// CNSmlDSAsyncCallBack::RunL()
// ------------------------------------------------------------------------------------------------
void CNSmlDSAsyncCallBack::RunL()
    {
    if ( iReqFinishedFunc == NULL )
        {
        iMessage.Complete( iStatus.Int() );
        }
    else
        {
        TInt status = ( *iSession.*iReqFinishedFunc )( this, EFinished );
        iMessage.Complete( status );
        }
    delete this;
    }

// End of File
