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
#include "NSmlAdapterLog.h"
#include "Nsmldsdataproviderarray.h"
#include "Nsmldsasyncrequesthandler.h"

#ifdef __HOST_SERVER_MTEST__
#include "../../stif/DSHostServerTest/inc/fakedataprovider.h"
#else
#include <SmlDataProvider.h>
#endif

// ======================================= MEMBER FUNCTIONS =======================================

// ------------------------------------------------------------------------------------------------
// RNSmlDSDataProviderArray::RNSmlDSDataProviderArray()
// ------------------------------------------------------------------------------------------------
RNSmlDSDataProviderArray::RNSmlDSDataProviderArray()
    {
    }

// ------------------------------------------------------------------------------------------------
// RNSmlDSDataProviderArray::ResetAndDestroy
// ------------------------------------------------------------------------------------------------
void RNSmlDSDataProviderArray::ResetAndDestroy()
    {
    for ( TInt i = 0; i < Count(); i++ )
        {
        TNSmlDSDataProviderElement* DPI = ( *this )[i];
        RDataStoreElementArray* DSA = DPI->iDataStores;
        	
        for ( TInt j = 0; j < DSA->Count(); j++ )
            {
            TNSmlDSDataStoreElement* DSI = ( *DSA )[j];
            delete Destroy( DSI );
            }
        DSA->Close();
        delete DSA;
        DPI->iDataStores = NULL;
        delete DPI->iDataprovider;
        }
    RPointerArray<TNSmlDSDataProviderElement>::ResetAndDestroy();
    }

// ------------------------------------------------------------------------------------------------
// RNSmlDSDataProviderArray::DataProvider
// ------------------------------------------------------------------------------------------------
CSmlDataProvider* RNSmlDSDataProviderArray::DataProvider( TSmlDataProviderId aId )
    {
    TNSmlDSDataProviderElement* findDPI = DataProviderElement( aId );
    if ( findDPI )
        {
        return findDPI->iDataprovider;
        }
    return NULL;
    }
    
// ------------------------------------------------------------------------------------------------
// RNSmlDSDataProviderArray::DataProviderElement
// ------------------------------------------------------------------------------------------------
TNSmlDSDataProviderElement* RNSmlDSDataProviderArray::DataProviderElement( TSmlDataProviderId aId )
    {
    for ( TInt i = 0; i < Count(); i++ )
        {
        TNSmlDSDataProviderElement* test = ( *this )[i];
        if ( test->iDataprovider->Identifier() == aId )
            {
            return test;
            }
        }
    return NULL;
    }

// ------------------------------------------------------------------------------------------------
// RNSmlDSDataProviderArray::DataStore
// ------------------------------------------------------------------------------------------------
CSmlDataStore* RNSmlDSDataProviderArray::DataStore( TSmlDataProviderId aId, 
	const TDesC& aStoreName )
    {
    TNSmlDSDataProviderElement* findDPI = DataProviderElement( aId );

    TNSmlDSDataStoreElement* findDSI = DataStoreElement( findDPI, aStoreName );
    if ( findDSI )
        {
        return findDSI->iDataStore;
        }

    return NULL;
    }

// ------------------------------------------------------------------------------------------------
// RNSmlDSDataProviderArray::DataStoreElement
// ------------------------------------------------------------------------------------------------
TNSmlDSDataStoreElement* RNSmlDSDataProviderArray::DataStoreElement( 
	const TNSmlDSDataProviderElement* aItem, const TDesC& aStoreName )
    {
    if ( !aItem )
    	{
    	return NULL;
    	}

    RDataStoreElementArray* DSA = aItem->iDataStores;
    for ( TInt i = 0 ; i < DSA->Count(); i++ )
        {
        TNSmlDSDataStoreElement* test = ( *DSA )[i];
        if ( test->StoreName() == aStoreName )
            {
            return test;
            }
        }
    return NULL;
    }
    
// ------------------------------------------------------------------------------------------------
// RNSmlDSDataProviderArray::InsertL
// ------------------------------------------------------------------------------------------------
TInt RNSmlDSDataProviderArray::InsertL( CSmlDataProvider* aDataprovider )
    {
    if ( !aDataprovider )
    	{
    	User::Leave( KErrArgument );
    	}

    TInt ret( KErrAlreadyExists );
    
    TNSmlDSDataProviderElement* findDPI = DataProviderElement( aDataprovider->Identifier() );
    if ( !findDPI )
        {
        findDPI = new ( ELeave ) TNSmlDSDataProviderElement();
        CleanupStack::PushL( findDPI );
        findDPI->iDataStores = new ( ELeave ) RDataStoreElementArray();
        CleanupStack::PushL( findDPI->iDataStores );
        
        findDPI->iDataprovider = aDataprovider;
        AppendL ( findDPI );
        CleanupStack::Pop( 2 ); //findDPI->iDataStores, findDPI
        
        ret = KErrNone;
        }
    
    return ret;    
    }

// ------------------------------------------------------------------------------------------------
// RNSmlDSDataProviderArray::InsertL
// ------------------------------------------------------------------------------------------------
TNSmlDSDataStoreElement* RNSmlDSDataProviderArray::InsertL( 
	TNSmlDSDataProviderElement* aDataproviderItem, 
	CSmlDataStore* aDataStore, 
	HBufC* aDataStoreName )
    {
    if ( !aDataproviderItem ) 
    	{
    	User::Leave( KErrArgument );
    	}
    if ( !aDataStore ) return NULL;

    TNSmlDSDataStoreElement* findDSI = DataStoreElement( aDataproviderItem, *aDataStoreName );
    if ( findDSI )
        {
        User::Leave( KErrAlreadyExists );
        }
    
    // Appending new TNSmlDSDataStoreElement to array
    findDSI = new ( ELeave ) TNSmlDSDataStoreElement( aDataStoreName );
    CleanupStack::PushL( findDSI );
    aDataproviderItem->iDataStores->AppendL( findDSI );
    findDSI->iDataStore = aDataStore;
    CleanupStack::Pop( findDSI );
        
    return findDSI;
    }

// ------------------------------------------------------------------------------------------------
// RNSmlDSDataProviderArray::Remove
// ------------------------------------------------------------------------------------------------
CNSmlDSAsyncRequestHandler* RNSmlDSDataProviderArray::Remove( 
	TNSmlDSDataProviderElement* aDataproviderItem, 
	TNSmlDSDataStoreElement* aDataStoreItem )
	{
	if ( !aDataproviderItem || !aDataStoreItem )
		{
		return NULL;
		}
		
	if ( !aDataproviderItem->iDataStores )
		{
		return NULL;
		}
	
	TInt ind ( aDataproviderItem->iDataStores->Find( aDataStoreItem ) );
	if ( ind != KErrNotFound )
		{
		aDataproviderItem->iDataStores->Remove( ind );
		}
		
	return Destroy( aDataStoreItem );
	}
	
// ------------------------------------------------------------------------------------------------
// RNSmlDSDataProviderArray::Destroy
// frees the memory reserver for aDataStoreItem except iDSAO, which is returned. 
// ------------------------------------------------------------------------------------------------
CNSmlDSAsyncRequestHandler* RNSmlDSDataProviderArray::Destroy( 
	TNSmlDSDataStoreElement*& aDataStoreItem )
	{
	delete aDataStoreItem->iDataStore;
	aDataStoreItem->iDataStore = NULL;
	
    if ( aDataStoreItem->iBatchItemUids )
        {
        aDataStoreItem->iBatchItemUids->Close();
        delete aDataStoreItem->iBatchItemUids;
        aDataStoreItem->iBatchItemUids = NULL;
        }
        
   	delete aDataStoreItem->iStoreName;
    aDataStoreItem->iStoreName = NULL;
    
    delete aDataStoreItem->iDSFormat;
    aDataStoreItem->iDSFormat = NULL;
    
    delete aDataStoreItem->iAdpLog;
    aDataStoreItem->iAdpLog = NULL;
        
    CNSmlDSAsyncRequestHandler* dsao = aDataStoreItem->iDSAO;
    if ( dsao )
    	{
    	dsao->iDSItem = NULL;
    	}

    delete aDataStoreItem;
    aDataStoreItem = NULL;
    return dsao;
	}
	
// End of File
