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

// CONSTANTS
_LIT( KNSmlPanicCategory, " Host Server Session\\Changed items fetcher " );

// ======================================= MEMBER FUNCTIONS =======================================

// ------------------------------------------------------------------------------------------------
// CNSmlDSChangedItemsFetcher::NewLC
// ------------------------------------------------------------------------------------------------
CNSmlDSChangedItemsFetcher* CNSmlDSChangedItemsFetcher::NewLC( CNSmlDSHostSession* aSession, 
        TNSmlDSDataStoreElement* aDSItem, 
        const RMessage2& aMessage,
        RequestFinishedFunction aReqFinishedFunc )
	{
	CNSmlDSChangedItemsFetcher* self = new ( ELeave ) CNSmlDSChangedItemsFetcher(
		aSession, aDSItem, aMessage, aReqFinishedFunc );
	CleanupStack::PushL( self );
	self->ConstructL();
	return self;
	}

// ------------------------------------------------------------------------------------------------
// CNSmlDSChangedItemsFetcher::~CNSmlDSChangedItemsFetcher
// ------------------------------------------------------------------------------------------------
CNSmlDSChangedItemsFetcher::~CNSmlDSChangedItemsFetcher()
	{
	if (iChangedItems)
		{		
		iChangedItems->Close();
		delete iChangedItems;
		}
	}

// ------------------------------------------------------------------------------------------------
// CNSmlDSChangedItemsFetcher::FetchAllChangedItemsL
// ------------------------------------------------------------------------------------------------
void CNSmlDSChangedItemsFetcher::FetchAllChangedItemsL()
	{
	AddToSchedulerL();
    SetActive();
    TRequestStatus* status = &iStatus;
    User::RequestComplete( status, KErrNone );
	}
	
// ------------------------------------------------------------------------------------------------
// CNSmlDSChangedItemsFetcher::CNSmlDSChangedItemsFetcher
// ------------------------------------------------------------------------------------------------      
CNSmlDSChangedItemsFetcher::CNSmlDSChangedItemsFetcher( CNSmlDSHostSession* aSession, 
        			TNSmlDSDataStoreElement* aDSItem, 
        			const RMessage2& aMessage,
        			RequestFinishedFunction aReqFinishedFunc ) :
	CNSmlDSAsyncRequestHandler( EPriorityLow, aSession, aDSItem, aMessage ),  
	iChangedItems( NULL )
	{
	iNextModsToFetch = TNSmlDbItemModification::ENSmlDbItemAdd;
	iReqFinishedFunc = aReqFinishedFunc;
	ASSERT( iReqFinishedFunc != NULL );
	ASSERT( iDSItem != NULL );
    }

// ------------------------------------------------------------------------------------------------
// CNSmlDSChangedItemsFetcher::ConstructL
// ------------------------------------------------------------------------------------------------
void CNSmlDSChangedItemsFetcher::ConstructL()
	{
	iChangedItems = new ( ELeave ) RNSmlDbItemModificationSet();
	}	

// ------------------------------------------------------------------------------------------------
// CNSmlDSChangedItemsFetcher::DoCancel
// No need to do anything. Here to prevent base class DoCancel from getting called.
// ------------------------------------------------------------------------------------------------
void CNSmlDSChangedItemsFetcher::DoCancel()
	{
	
	}

// ------------------------------------------------------------------------------------------------
// CNSmlDSChangedItemsFetcher::RunL
// ------------------------------------------------------------------------------------------------
void CNSmlDSChangedItemsFetcher::RunL()
	{
	CSmlDataStore* ds = iDSItem->iDataStore;
	TBool fetchFinished( EFalse );
	TNSmlDbItemModification::TNSmlDbItemModificationType 
		toNextModsToFetch( TNSmlDbItemModification::ENSmlDbItemAdd );
	const MSmlDataItemUidSet* dius = NULL;
	
	//Order of items must be preserved for hierarchy sync.
	//1.Add, 2.Replace, 3.Move, 4.SoftDelete, 5.Delete.
	switch( iNextModsToFetch )
		{
		case TNSmlDbItemModification::ENSmlDbItemAdd:
			{		
			dius = &ds->AddedItems();
			toNextModsToFetch = TNSmlDbItemModification::ENSmlDbItemModify;
			break;
			}
		case TNSmlDbItemModification::ENSmlDbItemDelete:
			{
			dius = &ds->DeletedItems();
			fetchFinished = ETrue;
			break;
			}
		case TNSmlDbItemModification::ENSmlDbItemSoftDelete:
			{
			dius = &ds->SoftDeletedItems();
			toNextModsToFetch = TNSmlDbItemModification::ENSmlDbItemDelete;
			break;
			}
		case TNSmlDbItemModification::ENSmlDbItemMove:
			{
			dius = &ds->MovedItems();
			toNextModsToFetch = TNSmlDbItemModification::ENSmlDbItemSoftDelete;
			break;
			}
		case TNSmlDbItemModification::ENSmlDbItemModify:
			{
			dius = &ds->ModifiedItems();
			toNextModsToFetch = TNSmlDbItemModification::ENSmlDbItemMove;
			break;
			}
		default:
			User::Panic( KNSmlPanicCategory, KErrUnknown );
		}
		
	TInt status = KErrNone;	
	
	if ( dius )
		{
		iChangedItems->AddGroupL( *dius, iNextModsToFetch );
		}
	
	if ( fetchFinished )
		{
		TRAP( status, ( *iSession.*iReqFinishedFunc )( this ) );

		iMessage.Complete( status );
		delete this;
		}
	else
		{
		iNextModsToFetch = toNextModsToFetch;
		SetActive();
		TRequestStatus* rstatus = &iStatus;
    	User::RequestComplete( rstatus, KErrNone );
		}
	}

// ----------------------------------------------------------------------------
// CNSmlDSChangedItemsFetcher::RunError
// ----------------------------------------------------------------------------
TInt CNSmlDSChangedItemsFetcher::RunError ( TInt aError )
{
    return KErrNone;
}
// End of File
