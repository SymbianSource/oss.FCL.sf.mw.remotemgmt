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
* Description:  Client module of DS Host Servers
*
*/


// ------------------------------------------------------------------------------------------------
// Includes
// ------------------------------------------------------------------------------------------------
#include <s32mem.h>
#include <SmlDataProvider.h>

#include "nsmldshostclient.h"
#include "nsmldshostconstants.h"

#ifdef __USE_FAKE_CLIENT__
#include "fakeclient.cpp"
#else

// ------------------------------------------------------------------------------------------------
// CNSmlDSHostClient - public methods
// ------------------------------------------------------------------------------------------------
// ------------------------------------------------------------------------------------------------
// CNSmlDSHostClient::NewL
// Creates new instance of CNSmlDSHostClient based class.
// ------------------------------------------------------------------------------------------------
EXPORT_C CNSmlDSHostClient* CNSmlDSHostClient::NewL()
	{
	CNSmlDSHostClient* self = NewLC();
	CleanupStack::Pop();
	return self;
	}

// ------------------------------------------------------------------------------------------------
// CNSmlDSHostClient::NewLC
// Creates new instance of CNSmlDSHostClient based class.
// ------------------------------------------------------------------------------------------------
EXPORT_C CNSmlDSHostClient* CNSmlDSHostClient::NewLC()
	{
	CNSmlDSHostClient* self = new (ELeave) CNSmlDSHostClient();
	CleanupStack::PushL( self );
	self->ConstructL();
	return self;
	}
	
// ------------------------------------------------------------------------------------------------
// CNSmlDSHostClient::~CNSmlDSHostClient
// C++ Destructor.
// ------------------------------------------------------------------------------------------------
EXPORT_C CNSmlDSHostClient::~CNSmlDSHostClient()
	{
	iClient1.Close();
	iClient2.Close();
	iDataProviders1.Close();
	iDataProviders2.Close();
	}

// ------------------------------------------------------------------------------------------------
// CNSmlDSHostClient::CreateDataProvidersL
// Creates Data Providers.
// ------------------------------------------------------------------------------------------------
EXPORT_C void CNSmlDSHostClient::CreateDataProvidersL( const RArray<TSmlDataProviderId>& aIds, RArray<TInt>& aResultArray )
	{
	RArray<TSmlDataProviderId> tempIdsArray;
	CleanupClosePushL( tempIdsArray );
	RArray<TInt> tempResultArray;
	CleanupClosePushL( tempResultArray );
	
	// Find Data Providers from Host Server 2
	iClient2.CreateDataProvidersL( aIds, aResultArray );
	if ( aIds.Count() != aResultArray.Count() )
		{
		User::Leave( KErrUnknown );
		}
	for ( TInt i = 0; i < aIds.Count(); i++ )
		{
		if ( aResultArray[ i ] != KErrNone )
			{
			tempIdsArray.AppendL( aIds[ i ] );
			}
		else
			{
			// Add succesfully loaded Data Providers to client array
			iDataProviders2.AppendL( aIds[ i ] );
			}
		}
	
	// Find Data Providers from Host Server 1
	if ( tempIdsArray.Count() > 0)
		{		
		iClient1.CreateDataProvidersL( tempIdsArray, tempResultArray );
		if ( tempIdsArray.Count() != tempResultArray.Count() )
			{
			User::Leave( KErrUnknown );
			}
		for ( TInt j = 0; j < tempIdsArray.Count(); j++ )
			{
			for ( TInt k = 0; k < aIds.Count(); k++ )
				{
				if ( aIds[ k ] == tempIdsArray[ j ] )
					{
					aResultArray[ k ] = tempResultArray[ j ];
					}
				}
			if ( tempResultArray[ j ] == KErrNone )
				{
				// Add succesfully loaded Data Providers to client array
				iDataProviders1.AppendL( tempIdsArray[ j ] );
				}
			}
		}
		CleanupStack::PopAndDestroy(2); //tempResultArray, tempIdsArray
	}
	
// ------------------------------------------------------------------------------------------------
// CNSmlDSHostClient::CreateAllDataProvidersL
// Creates all possible Data Providers.
// ------------------------------------------------------------------------------------------------
EXPORT_C void CNSmlDSHostClient::CreateAllDataProvidersL( RArray<TSmlDataProviderId>& aIds )
	{
	// Find Data Providers from Host server 2
	// Find Data Providers from Host server 1 except those Data Providers that were found from
	// previous server
	RArray<TSmlDataProviderId> tmpArray;
	CleanupClosePushL( tmpArray );
	
	iClient2.CreateAllDataProvidersL( aIds, tmpArray );
	iDataProviders2.Reset();
	
	for ( TInt i = 0; i < aIds.Count(); i++ )
		{
		iDataProviders2.AppendL( aIds[ i ] );
		}
		
	iClient1.CreateAllDataProvidersL( tmpArray, aIds );
	iDataProviders1.Reset();
	
	for ( TInt i = 0; i < tmpArray.Count(); i++ )
		{
		iDataProviders1.AppendL( tmpArray[i] );
		aIds.AppendL( tmpArray[i] );
		}
	
	CleanupStack::PopAndDestroy(); //tmpArray
	}

// ------------------------------------------------------------------------------------------------
// CNSmlDSHostClient::DataProviderInformationL
// 
// ------------------------------------------------------------------------------------------------	
EXPORT_C TNSmlDPInformation* CNSmlDSHostClient::DataProviderInformationL( const TSmlDataProviderId aId, TInt& aResultCode ) const
	{
	TNSmlDPInformation* ret = NULL;
	
	switch( SessionForDataProvider( aId ) )
		{
		case ( ENSmlHostServer1 ):
			ret = iClient1.DataProviderInformationL( aId, aResultCode );
			break;
		case ( ENSmlHostServer2 ):
			ret = iClient2.DataProviderInformationL( aId, aResultCode );
			break;
		case ( ENSmlHostServerEnd ):
		default:
			User::Leave( KErrNotFound );
		}
	aResultCode = HostErrorConversion( aResultCode );
	return ret;
	}	

// ------------------------------------------------------------------------------------------------
// CNSmlDSHostClient::SupportsOperationL
// Asks if Data Provider supports some operation.
// ------------------------------------------------------------------------------------------------
EXPORT_C TBool CNSmlDSHostClient::SupportsOperationL( TUid aOpId, TSmlDataProviderId aId, TInt& aResultCode ) const
	{
	TBool ret( EFalse );
	
	switch( SessionForDataProvider( aId ) )
		{
		case ( ENSmlHostServer1 ):
			ret = iClient1.SupportsOperationL( aOpId, aId, aResultCode );
			break;
		case ( ENSmlHostServer2 ):
			ret = iClient2.SupportsOperationL( aOpId, aId, aResultCode );
			break;
		case ( ENSmlHostServerEnd ):
		default:
			User::Leave( KErrNotFound );
		}
	aResultCode = HostErrorConversion( aResultCode );
	return ret;
	}

// ------------------------------------------------------------------------------------------------
// CNSmlDSHostClient::StoreFormatL
// Creates Data Store format of Data Provider.
// ------------------------------------------------------------------------------------------------
EXPORT_C CNSmlDbCaps* CNSmlDSHostClient::StoreFormatL( const TSmlDataProviderId aId, TInt& aResultCode )
	{
	CNSmlDbCaps* ret = NULL;

	switch( SessionForDataProvider( aId ) )
		{
		case ( ENSmlHostServer1 ):
			ret = iClient1.StoreFormatL( aId, aResultCode );
			break;
		case ( ENSmlHostServer2 ):
			ret = iClient2.StoreFormatL( aId, aResultCode );
			break;
		case ( ENSmlHostServerEnd ):
		default:
			User::Leave( KErrNotFound );
		}
	aResultCode = HostErrorConversion( aResultCode );
	return ret;
	}
	
// ------------------------------------------------------------------------------------------------
// CNSmlDSHostClient::ListStoresL
// Creates list of Data Store names of Data Provider.
// ------------------------------------------------------------------------------------------------	
EXPORT_C void CNSmlDSHostClient::ListStoresL( CDesCArray* aNameList, const TSmlDataProviderId aId, TInt& aResultCode )
	{
	switch( SessionForDataProvider( aId ) )
		{
		case ( ENSmlHostServer1 ):
			iClient1.ListStoresL( aNameList, aId, aResultCode );
			break;
		case ( ENSmlHostServer2 ):
			iClient2.ListStoresL( aNameList, aId, aResultCode );
			break;
		case ( ENSmlHostServerEnd ):
		default:
			User::Leave( KErrNotFound );
		}
	aResultCode = HostErrorConversion( aResultCode );
	}

// ------------------------------------------------------------------------------------------------
// CNSmlDSHostClient::DefaultStoreL
// Creates default name for Data Store of Data Provider.
// ------------------------------------------------------------------------------------------------	
EXPORT_C HBufC* CNSmlDSHostClient::DefaultStoreL( const TSmlDataProviderId aId, TInt& aResultCode )
	{
	HBufC* ret = NULL;

	switch( SessionForDataProvider( aId ) )
		{
		case ( ENSmlHostServer1 ):
			ret = iClient1.DefaultStoreL( aId, aResultCode );
			break;
		case ( ENSmlHostServer2 ):
			ret = iClient2.DefaultStoreL( aId, aResultCode );
			break;
		case ( ENSmlHostServerEnd ):
		default:
			User::Leave( KErrNotFound );
		}
	aResultCode = HostErrorConversion( aResultCode );
	return ret;
	}

// ------------------------------------------------------------------------------------------------
// CNSmlDSHostClient::SupportedServerFiltersL
// This method returns the set of filters that can be used to send to the Sync Partner.
// ------------------------------------------------------------------------------------------------	
EXPORT_C RPointerArray<CSyncMLFilter>* CNSmlDSHostClient::SupportedServerFiltersL( const TSmlDataProviderId aId, 
	TSyncMLFilterMatchType& aMatchType, TSyncMLFilterChangeInfo& aChangeInfo, TInt& aResultCode ) const
	{
	RPointerArray<CSyncMLFilter>* result = NULL;
	switch( SessionForDataProvider( aId ) )
		{
		case ( ENSmlHostServer1 ):
			result = iClient1.SupportedServerFiltersL( aId, aMatchType, aChangeInfo, aResultCode );
			break;
		case ( ENSmlHostServer2 ):
			result = iClient2.SupportedServerFiltersL( aId, aMatchType, aChangeInfo, aResultCode );
			break;
		case ( ENSmlHostServerEnd ):
		default:
			User::Leave( KErrNotFound );
		}
		
	aResultCode = HostErrorConversion( aResultCode );
	
	return result;
	}

// ------------------------------------------------------------------------------------------------
// CNSmlDSHostClient::CheckServerFiltersL
// This method updates dynamic filters up-to-date.
// ------------------------------------------------------------------------------------------------		
EXPORT_C void CNSmlDSHostClient::CheckServerFiltersL( const TSmlDataProviderId aId, RPointerArray<CSyncMLFilter>& aFilters, TSyncMLFilterChangeInfo& aChangeInfo, TInt& aResultCode ) const
	{
	switch( SessionForDataProvider( aId ) )
		{
		case ( ENSmlHostServer1 ):
			iClient1.CheckServerFiltersL( aId, aFilters, aChangeInfo, aResultCode );
			break;
		case ( ENSmlHostServer2 ):
			iClient2.CheckServerFiltersL( aId, aFilters, aChangeInfo, aResultCode );
			break;
		case ( ENSmlHostServerEnd ):
		default:
			User::Leave( KErrNotFound );
		}
		
	aResultCode = HostErrorConversion( aResultCode );
	}

// ------------------------------------------------------------------------------------------------
// CNSmlDSHostClient::CheckSupportedServerFiltersL
// This method returns the set of filters that can be used to send to the Sync Partner.
// ------------------------------------------------------------------------------------------------	
EXPORT_C void CNSmlDSHostClient::CheckSupportedServerFiltersL( const TSmlDataProviderId aId, 
	const CNSmlDbCaps& aServerDataStoreFormat, const CArrayFix<TNSmlFilterCapData>& aFilterInfoArr, 
	RPointerArray<CSyncMLFilter>& aFilters, TSyncMLFilterChangeInfo& aChangeInfo, TInt& aResultCode ) const
	{
	switch( SessionForDataProvider( aId ) )
		{
		case ( ENSmlHostServer1 ):
			iClient1.CheckSupportedServerFiltersL( aId, aServerDataStoreFormat, aFilterInfoArr, aFilters, aChangeInfo, aResultCode );
			break;
		case ( ENSmlHostServer2 ):
			iClient2.CheckSupportedServerFiltersL( aId, aServerDataStoreFormat, aFilterInfoArr, aFilters, aChangeInfo, aResultCode );
			break;
		case ( ENSmlHostServerEnd ):
		default:
			User::Leave( KErrNotFound );
		}
		
	aResultCode = HostErrorConversion( aResultCode );
	}
	
// ------------------------------------------------------------------------------------------------
// CNSmlDSHostClient::GetFilterL
// Get Filters.
// ------------------------------------------------------------------------------------------------	
EXPORT_C void CNSmlDSHostClient::GetFilterL( const TSmlDataProviderId aId, const TDesC& aStoreName, 
	const RPointerArray<CSyncMLFilter>& aFilterArray, CNSmlFilter*& aFilter, 
	TSyncMLFilterMatchType aMatchType, TInt& aResultCode )
	{
	switch( SessionForDataProvider( aId ) )
		{
		case ( ENSmlHostServer1 ):
			iClient1.GetFilterL( aId, aStoreName, aFilterArray, aFilter, aMatchType, aResultCode );
			break;
		case ( ENSmlHostServer2 ):
			iClient2.GetFilterL( aId, aStoreName, aFilterArray, aFilter, aMatchType, aResultCode );
			break;
		case ( ENSmlHostServerEnd ):
		default:
			User::Leave( KErrNotFound );
		}
		
	aResultCode = HostErrorConversion( aResultCode );
	}

// ------------------------------------------------------------------------------------------------
// CNSmlDSHostClient::OpenL
// Opens the data store specified by aStoreName asynchronously.
// ------------------------------------------------------------------------------------------------	
EXPORT_C void CNSmlDSHostClient::OpenL( const TSmlDataProviderId aId, const TDesC& aStoreName, const TDesC& aServerId, const TDesC& aRemoteDB, TInt& aResultCode )
	{
	switch( SessionForDataProvider( aId ) )
		{
		case ( ENSmlHostServer1 ):
			iClient1.OpenL( aId, aStoreName, aServerId, aRemoteDB, aResultCode );
			break;
		case ( ENSmlHostServer2 ):
			iClient2.OpenL( aId, aStoreName, aServerId, aRemoteDB, aResultCode );
			break;
		case ( ENSmlHostServerEnd ):
		default:
			User::Leave( KErrNotFound );
		}
	aResultCode = HostErrorConversion( aResultCode );
	}

// ------------------------------------------------------------------------------------------------
// CNSmlDSHostClient::CancelRequestL
// Cancel the current asynchronous request.
// ------------------------------------------------------------------------------------------------	
EXPORT_C void CNSmlDSHostClient::CancelRequestL( const TSmlDataProviderId aId, const TDesC& aStoreName, TInt& aResultCode )
	{
	switch( SessionForDataProvider( aId ) )
		{
		case ( ENSmlHostServer1 ):
			iClient1.CancelRequest( aId, aStoreName, aResultCode );
			break;
		case ( ENSmlHostServer2 ):
			iClient2.CancelRequest( aId, aStoreName, aResultCode );
			break;
		case ( ENSmlHostServerEnd ):
		default:
			User::Leave( KErrNotFound );
		}
	aResultCode = HostErrorConversion( aResultCode );
	}

// ------------------------------------------------------------------------------------------------
// CNSmlDSHostClient::BeginTransactionL
// Starts the transaction mode.
// ------------------------------------------------------------------------------------------------	
EXPORT_C void CNSmlDSHostClient::BeginTransactionL( const TSmlDataProviderId aId, const TDesC& aStoreName, TInt& aResultCode )
	{
	switch( SessionForDataProvider( aId ) )
		{
		case ( ENSmlHostServer1 ):
			iClient1.BeginTransaction( aId, aStoreName, aResultCode );
			break;
		case ( ENSmlHostServer2 ):
			iClient2.BeginTransaction( aId, aStoreName, aResultCode );
			break;
		case ( ENSmlHostServerEnd ):
		default:
			User::Leave( KErrNotFound );
		}
		
	aResultCode = HostErrorConversion( aResultCode );
	}

// ------------------------------------------------------------------------------------------------
// CNSmlDSHostClient::CommitTransactionL
// Method will be called at the end of a successful transaction.
// ------------------------------------------------------------------------------------------------	
EXPORT_C void CNSmlDSHostClient::CommitTransactionL( const TSmlDataProviderId aId, const TDesC& aStoreName, TInt& aResultCode )
	{
	switch( SessionForDataProvider( aId ) )
		{
		case ( ENSmlHostServer1 ):
			iClient1.CommitTransaction( aId, aStoreName, aResultCode );
			break;
		case ( ENSmlHostServer2 ):
			iClient2.CommitTransaction( aId, aStoreName, aResultCode );
			break;
		case ( ENSmlHostServerEnd ):
		default:
			User::Leave( KErrNotFound );
		}
		
	aResultCode = HostErrorConversion( aResultCode );
	}

// ------------------------------------------------------------------------------------------------
// CNSmlDSHostClient::RevertTransactionL
// Method will be called to abort an ongoing transaction.
// ------------------------------------------------------------------------------------------------	
EXPORT_C void CNSmlDSHostClient::RevertTransactionL( const TSmlDataProviderId aId, const TDesC& aStoreName, TInt& aResultCode )
	{
	switch( SessionForDataProvider( aId ) )
		{
		case ( ENSmlHostServer1 ):
			iClient1.RevertTransaction( aId, aStoreName, aResultCode );
			break;
		case ( ENSmlHostServer2 ):
			iClient2.RevertTransaction( aId, aStoreName, aResultCode );
			break;
		case ( ENSmlHostServerEnd ):
		default:
			User::Leave( KErrNotFound );
		}
		
	aResultCode = HostErrorConversion( aResultCode );
	}

// ------------------------------------------------------------------------------------------------
// CNSmlDSHostClient::BeginBatchL
// Starts the batch mode.
// ------------------------------------------------------------------------------------------------	
EXPORT_C void CNSmlDSHostClient::BeginBatchL( const TSmlDataProviderId aId, const TDesC& aStoreName, TInt& aResultCode )
	{
	switch( SessionForDataProvider( aId ) )
		{
		case ( ENSmlHostServer1 ):
			iClient1.BeginBatch( aId, aStoreName, aResultCode );
			break;
		case ( ENSmlHostServer2 ):
			iClient2.BeginBatch( aId, aStoreName, aResultCode );
			break;
		case ( ENSmlHostServerEnd ):
		default:
			User::Leave( KErrNotFound );
		}
	aResultCode = HostErrorConversion( aResultCode );		
	}
	
// ------------------------------------------------------------------------------------------------
// CNSmlDSHostClient::CommitBatchL
// Method will be called at the end of the batch mode.
// ------------------------------------------------------------------------------------------------	
EXPORT_C void CNSmlDSHostClient::CommitBatchL( RArray<TInt>& aResultArray, const TSmlDataProviderId aId, const TDesC& aStoreName, TInt& aResultCode )
	{
	switch( SessionForDataProvider( aId ) )
		{
		case ( ENSmlHostServer1 ):
			iClient1.CommitBatchL( aResultArray, aId, aStoreName, aResultCode );
			break;
		case ( ENSmlHostServer2 ):
			iClient2.CommitBatchL( aResultArray, aId, aStoreName, aResultCode );
			break;
		case ( ENSmlHostServerEnd ):
		default:
			User::Leave( KErrNotFound );
		}
	aResultCode = HostErrorConversion( aResultCode );
	}

// ------------------------------------------------------------------------------------------------
// CNSmlDSHostClient::CancelBatchL
// Method will be called to abort an ongoing batch mode.
// ------------------------------------------------------------------------------------------------	
EXPORT_C void CNSmlDSHostClient::CancelBatchL( const TSmlDataProviderId aId, const TDesC& aStoreName, TInt& aResultCode )
	{
	switch( SessionForDataProvider( aId ) )
		{
		case ( ENSmlHostServer1 ):
			iClient1.CancelBatch( aId, aStoreName, aResultCode );
			break;
		case ( ENSmlHostServer2 ):
			iClient2.CancelBatch( aId, aStoreName, aResultCode );
			break;
		case ( ENSmlHostServerEnd ):
		default:
			User::Leave( KErrNotFound );
		}
	aResultCode = HostErrorConversion( aResultCode );
	}
	
// ------------------------------------------------------------------------------------------------
// CNSmlDSHostClient::SetRemoteDataStoreFormatL
// Sets the Sync Partner Data Format.
// ------------------------------------------------------------------------------------------------	
EXPORT_C void CNSmlDSHostClient::SetRemoteDataStoreFormatL( const CNSmlDbCaps& aServerDataStoreFormat, const TSmlDataProviderId aId, const TDesC& aStoreName, TInt& aResultCode )
	{
	switch( SessionForDataProvider( aId ) )
		{
		case ( ENSmlHostServer1 ):
			iClient1.SetRemoteDataStoreFormatL( aServerDataStoreFormat, aId, aStoreName, aResultCode );
			break;
		case ( ENSmlHostServer2 ):
			iClient2.SetRemoteDataStoreFormatL( aServerDataStoreFormat, aId, aStoreName, aResultCode );
			break;
		case ( ENSmlHostServerEnd ):
		default:
			User::Leave( KErrNotFound );
		}
	aResultCode = HostErrorConversion( aResultCode );
	}
	
// ------------------------------------------------------------------------------------------------
// CNSmlDSHostClient::SetRemoteMaxObjectSizeL
// Sets the SyncML server Sync Partner maximum object size.
// ------------------------------------------------------------------------------------------------	
EXPORT_C void CNSmlDSHostClient::SetRemoteMaxObjectSizeL( TInt aServerMaxObjectSize, const TSmlDataProviderId aId, const TDesC& aStoreName, TInt& aResultCode )
	{
	switch( SessionForDataProvider( aId ) )
		{
		case ( ENSmlHostServer1 ):
			iClient1.SetRemoteMaxObjectSizeL( aServerMaxObjectSize, aId, aStoreName, aResultCode );
			break;
		case ( ENSmlHostServer2 ):
			iClient2.SetRemoteMaxObjectSizeL( aServerMaxObjectSize, aId, aStoreName, aResultCode );
			break;
		case ( ENSmlHostServerEnd ):
		default:
			User::Leave( KErrNotFound );
		}
	aResultCode = HostErrorConversion( aResultCode );
	}
	
// ------------------------------------------------------------------------------------------------
// CNSmlDSHostClient::MaxObjectSizeL
// Gets the Data Store maximum object size which is reported to the SyncML partner.
// ------------------------------------------------------------------------------------------------	
EXPORT_C TInt CNSmlDSHostClient::MaxObjectSizeL( const TSmlDataProviderId aId, const TDesC& aStoreName, TInt& aResultCode ) const
	{
	TInt ret(0);
	switch( SessionForDataProvider( aId ) )
		{
		case ( ENSmlHostServer1 ):
			ret = iClient1.MaxObjectSize( aId, aStoreName, aResultCode );
			break;
		case ( ENSmlHostServer2 ):
			ret = iClient2.MaxObjectSize( aId, aStoreName, aResultCode );
			break;
		case ( ENSmlHostServerEnd ):
		default:
			User::Leave( KErrNotFound );
		}
		
	aResultCode = HostErrorConversion( aResultCode );
	return ret;
	}	

// ------------------------------------------------------------------------------------------------
// CNSmlDSHostClient::OpenItemL
// Opens item at Data Store.
// ------------------------------------------------------------------------------------------------	
EXPORT_C void CNSmlDSHostClient::OpenItemL( TSmlDbItemUid aUid, TBool& aFieldChange, TInt& aSize, TSmlDbItemUid& aParent, HBufC8*& aMimeType, HBufC8*& aMimeVer, const TSmlDataProviderId aId, const TDesC& aStoreName, TInt& aResultCode )
	{
	switch( SessionForDataProvider( aId ) )
		{
		case ( ENSmlHostServer1 ):
			iClient1.OpenItemL( aUid, aFieldChange, aSize, aParent, aMimeType, aMimeVer, aId, aStoreName, aResultCode );
			break;
		case ( ENSmlHostServer2 ):
			iClient2.OpenItemL( aUid, aFieldChange, aSize, aParent, aMimeType, aMimeVer, aId, aStoreName, aResultCode );
			break;
		case ( ENSmlHostServerEnd ):
		default:
			User::Leave( KErrNotFound );
		}
	aResultCode = HostErrorConversion( aResultCode );
	}

// ------------------------------------------------------------------------------------------------
// CNSmlDSHostClient::CreateItemL
// Creates new item to Data Store.
// ------------------------------------------------------------------------------------------------	
EXPORT_C void CNSmlDSHostClient::CreateItemL( TSmlDbItemUid& aUid, TInt aSize, TSmlDbItemUid aParent, const TDesC8& aMimeType, const TDesC8& aMimeVer, const TSmlDataProviderId aId, const TDesC& aStoreName, TInt& aResultCode )
	{
	switch( SessionForDataProvider( aId ) )
		{
		case ( ENSmlHostServer1 ):
			iClient1.CreateItemL( aUid, aSize, aParent, aMimeType, aMimeVer, aId, aStoreName, aResultCode );
			break;
		case ( ENSmlHostServer2 ):
			iClient2.CreateItemL( aUid, aSize, aParent, aMimeType, aMimeVer, aId, aStoreName, aResultCode );
			break;
		case ( ENSmlHostServerEnd ):
		default:
			User::Leave( KErrNotFound );
		}
	aResultCode = HostErrorConversion( aResultCode );
	}
	
// ------------------------------------------------------------------------------------------------
// CNSmlDSHostClient::ReplaceItemL
// Replaces old item at Data Store.
// ------------------------------------------------------------------------------------------------	
EXPORT_C void CNSmlDSHostClient::ReplaceItemL( TSmlDbItemUid aUid, TInt aSize, TSmlDbItemUid aParent, TBool aFieldChange, TSmlDataProviderId aId, const TDesC& aStoreName, TInt& aResultCode )
	{
	switch( SessionForDataProvider( aId ) )
		{
		case ( ENSmlHostServer1 ):
			iClient1.ReplaceItemL( aUid, aSize, aParent, aFieldChange, aId, aStoreName, aResultCode );
			break;
		case ( ENSmlHostServer2 ):
			iClient2.ReplaceItemL( aUid, aSize, aParent, aFieldChange, aId, aStoreName, aResultCode );
			break;
		case ( ENSmlHostServerEnd ):
		default:
			User::Leave( KErrNotFound );
		}
	aResultCode = HostErrorConversion( aResultCode );
	}

// ------------------------------------------------------------------------------------------------
// CNSmlDSHostClient::ReadItemL
// Reads data from item at Data Store. Item must be opened before this method can be called.
// This method is called until aBuffer is not used totally or method leaves with KErrEof.
// ------------------------------------------------------------------------------------------------	
EXPORT_C void CNSmlDSHostClient::ReadItemL( TDes8& aBuffer, const TSmlDataProviderId aId, const TDesC& aStoreName, TInt& aResultCode )
	{
	switch( SessionForDataProvider( aId ) )
		{
		case ( ENSmlHostServer1 ):
			iClient1.ReadItemL( aBuffer, aId, aStoreName, aResultCode );
			break;
		case ( ENSmlHostServer2 ):
			iClient2.ReadItemL( aBuffer, aId, aStoreName, aResultCode );
			break;
		case ( ENSmlHostServerEnd ):
		default:
			User::Leave( KErrNotFound );
		}
	aResultCode = HostErrorConversion( aResultCode );
	}
		
// ------------------------------------------------------------------------------------------------
// CNSmlDSHostClient::WriteItemL
// Writes data to item to Data Provider. CreateItemL or ReplaceItemL method must be called before
// this method can be called. This method is called until all data to current item is written.
// ------------------------------------------------------------------------------------------------	
EXPORT_C void CNSmlDSHostClient::WriteItemL( const TDesC8& aData, const TSmlDataProviderId aId, const TDesC& aStoreName, TInt& aResultCode )
	{
	switch( SessionForDataProvider( aId ) )
		{
		case ( ENSmlHostServer1 ):
			iClient1.WriteItemL( aData, aId, aStoreName, aResultCode );
			break;
		case ( ENSmlHostServer2 ):
			iClient2.WriteItemL( aData, aId, aStoreName, aResultCode );
			break;
		case ( ENSmlHostServerEnd ):
		default:
			User::Leave( KErrNotFound );
		}
	aResultCode = HostErrorConversion( aResultCode );
	}
	
// ------------------------------------------------------------------------------------------------
// CNSmlDSHostClient::CommitItemL
// After item is written to Data Provider it can be saved to the Data Store.
// This method can be called just after WriteItemL method.
// ------------------------------------------------------------------------------------------------	
EXPORT_C void CNSmlDSHostClient::CommitItemL( const TSmlDataProviderId aId, const TDesC& aStoreName, TInt& aResultCode )
	{
	switch( SessionForDataProvider( aId ) )
		{
		case ( ENSmlHostServer1 ):
			iClient1.CommitItem( aId, aStoreName, aResultCode );
			break;
		case ( ENSmlHostServer2 ):
			iClient2.CommitItem( aId, aStoreName, aResultCode );
			break;
		case ( ENSmlHostServerEnd ):
		default:
			User::Leave( KErrNotFound );
		}
	aResultCode = HostErrorConversion( aResultCode );
	}
	
// ------------------------------------------------------------------------------------------------
// CNSmlDSHostClient::CloseItemL
// Closes opened item.
// ------------------------------------------------------------------------------------------------	
EXPORT_C void CNSmlDSHostClient::CloseItemL( const TSmlDataProviderId aId, const TDesC& aStoreName, TInt& aResultCode )
	{
	switch( SessionForDataProvider( aId ) )
		{
		case ( ENSmlHostServer1 ):
			iClient1.CloseItem( aId, aStoreName, aResultCode );
			break;
		case ( ENSmlHostServer2 ):
			iClient2.CloseItem( aId, aStoreName, aResultCode );
			break;
		case ( ENSmlHostServerEnd ):
		default:
			User::Leave( KErrNotFound );
		}
	aResultCode = HostErrorConversion( aResultCode );
	}
	
// ------------------------------------------------------------------------------------------------
// CNSmlDSHostClient::MoveItemL
// Moves item to new location.
// ------------------------------------------------------------------------------------------------	
EXPORT_C void CNSmlDSHostClient::MoveItemL( TSmlDbItemUid aUid, TSmlDbItemUid aNewParent, const TSmlDataProviderId aId, const TDesC& aStoreName, TInt& aResultCode )
	{
	switch( SessionForDataProvider( aId ) )
		{
		case ( ENSmlHostServer1 ):
			iClient1.MoveItemL( aUid, aNewParent, aId, aStoreName, aResultCode );
			break;
		case ( ENSmlHostServer2 ):
			iClient2.MoveItemL( aUid, aNewParent, aId, aStoreName, aResultCode );
			break;
		case ( ENSmlHostServerEnd ):
		default:
			User::Leave( KErrNotFound );
		}
	aResultCode = HostErrorConversion( aResultCode );
	}
		
// ------------------------------------------------------------------------------------------------
// CNSmlDSHostClient::DeleteItemL
// Deletes one item at Data Store permanently.
// ------------------------------------------------------------------------------------------------	
EXPORT_C void CNSmlDSHostClient::DeleteItemL( TSmlDbItemUid aUid, const TSmlDataProviderId aId, const TDesC& aStoreName, TInt& aResultCode )
	{
	switch( SessionForDataProvider( aId ) )
		{
		case ( ENSmlHostServer1 ):
			iClient1.DeleteItemL( aUid, aId, aStoreName, aResultCode );
			break;
		case ( ENSmlHostServer2 ):
			iClient2.DeleteItemL( aUid, aId, aStoreName, aResultCode );
			break;
		case ( ENSmlHostServerEnd ):
		default:
			User::Leave( KErrNotFound );
		}
	aResultCode = HostErrorConversion( aResultCode );
	}
	
// ------------------------------------------------------------------------------------------------
// CNSmlDSHostClient::SoftDeleteItemL
// Soft deletes one item at Data Store.
// ------------------------------------------------------------------------------------------------	
EXPORT_C void CNSmlDSHostClient::SoftDeleteItemL( TSmlDbItemUid aUid, const TSmlDataProviderId aId, const TDesC& aStoreName, TInt& aResultCode )
	{
	switch( SessionForDataProvider( aId ) )
		{
		case ( ENSmlHostServer1 ):
			iClient1.SoftDeleteItemL( aUid, aId, aStoreName, aResultCode );
			break;
		case ( ENSmlHostServer2 ):
			iClient2.SoftDeleteItemL( aUid, aId, aStoreName, aResultCode );
			break;
		case ( ENSmlHostServerEnd ):
		default:
			User::Leave( KErrNotFound );
		}
	aResultCode = HostErrorConversion( aResultCode );
	}
	
// ------------------------------------------------------------------------------------------------
// CNSmlDSHostClient::DeleteAllItemsL
// Deletes all items at Data Store permanently.
// ------------------------------------------------------------------------------------------------	
EXPORT_C void CNSmlDSHostClient::DeleteAllItemsL( const TSmlDataProviderId aId, const TDesC& aStoreName, TInt& aResultCode )
	{
	switch( SessionForDataProvider( aId ) )
		{
		case ( ENSmlHostServer1 ):
			iClient1.DeleteAllItems( aId, aStoreName, aResultCode );
			break;
		case ( ENSmlHostServer2 ):
			iClient2.DeleteAllItems( aId, aStoreName, aResultCode );
			break;
		case ( ENSmlHostServerEnd ):
		default:
			User::Leave( KErrNotFound );
		}
	aResultCode = HostErrorConversion( aResultCode );
	}

// ------------------------------------------------------------------------------------------------
// CNSmlDSHostClient::HasSyncHistoryL
// Checks if the Data Store has sync history. If not then slow sync is proposed to Sync Partner.
// ------------------------------------------------------------------------------------------------	
EXPORT_C TBool CNSmlDSHostClient::HasSyncHistoryL( const TSmlDataProviderId aId, const TDesC& aStoreName, TInt& aResultCode ) const
	{
	TBool ret( EFalse );
	
	switch( SessionForDataProvider( aId ) )
		{
		case ( ENSmlHostServer1 ):
			ret = iClient1.HasSyncHistory( aId, aStoreName, aResultCode );
			break;
		case ( ENSmlHostServer2 ):
			ret = iClient2.HasSyncHistory( aId, aStoreName, aResultCode );
			break;
		case ( ENSmlHostServerEnd ):
		default:
			User::Leave( KErrNotFound );
		}
	aResultCode = HostErrorConversion( aResultCode );
	return ret;
	}
	
// ------------------------------------------------------------------------------------------------
// CNSmlDSHostClient::AddedItemsL
// The Data Provider returns UIDs of items that are added after previous synchronization.
// If the Data Provider uses hierarchical synchronization then added folders must be placed
// first (from root to leaves) to UID set and finally items.
// ------------------------------------------------------------------------------------------------	
EXPORT_C void CNSmlDSHostClient::AddedItemsL( RNSmlDbItemModificationSet& aUidSet, const TSmlDataProviderId aId, const TDesC& aStoreName, TInt& aResultCode ) const
	{
	switch( SessionForDataProvider( aId ) )
		{
		case ( ENSmlHostServer1 ):
			iClient1.AddedItemsL( aUidSet, aId, aStoreName, aResultCode );
			break;
		case ( ENSmlHostServer2 ):
			iClient2.AddedItemsL( aUidSet, aId, aStoreName, aResultCode );
			break;
		case ( ENSmlHostServerEnd ):
		default:
			User::Leave( KErrNotFound );
		}
	aResultCode = HostErrorConversion( aResultCode );
	}
	
// ------------------------------------------------------------------------------------------------
// CNSmlDSHostClient::DeletedItemsL
// The Data Provider returns UIDs of items that are deleted after previous synchronization.
// If the Data Provider uses hierarchical synchronization then deleted items must be placed
// first to UID set and folders after items (from leaves to root).
// ------------------------------------------------------------------------------------------------	
EXPORT_C void CNSmlDSHostClient::DeletedItemsL( RNSmlDbItemModificationSet& aUidSet, const TSmlDataProviderId aId, const TDesC& aStoreName, TInt& aResultCode ) const
	{
	switch( SessionForDataProvider( aId ) )
		{
		case ( ENSmlHostServer1 ):
			iClient1.DeletedItemsL( aUidSet, aId, aStoreName, aResultCode );
			break;
		case ( ENSmlHostServer2 ):
			iClient2.DeletedItemsL( aUidSet, aId, aStoreName, aResultCode );
			break;
		case ( ENSmlHostServerEnd ):
		default:
			User::Leave( KErrNotFound );
		}
	aResultCode = HostErrorConversion( aResultCode );
	}

// ------------------------------------------------------------------------------------------------
// CNSmlDSHostClient::SoftDeleteItemsL
// The Data Provider returns UIDs of items that are soft deleted after previous synchronization.
// If the Data Provider uses hierarchical synchronization then soft deleted items must be placed
// first to UID set and folders after items (from leaves to root).
// ------------------------------------------------------------------------------------------------	
EXPORT_C void CNSmlDSHostClient::SoftDeleteItemsL( RNSmlDbItemModificationSet& aUidSet, const TSmlDataProviderId aId, const TDesC& aStoreName, TInt& aResultCode ) const
	{
	switch( SessionForDataProvider( aId ) )
		{
		case ( ENSmlHostServer1 ):
			iClient1.SoftDeleteItemsL( aUidSet, aId, aStoreName, aResultCode );
			break;
		case ( ENSmlHostServer2 ):
			iClient2.SoftDeleteItemsL( aUidSet, aId, aStoreName, aResultCode );
			break;
		case ( ENSmlHostServerEnd ):
		default:
			User::Leave( KErrNotFound );
		}
	aResultCode = HostErrorConversion( aResultCode );
	}

// ------------------------------------------------------------------------------------------------
// CNSmlDSHostClient::ModifiedItemsL
// The Data Provider returns UIDs of items that are modified after previous synchronization.
// If the Data Provider uses hierarchical synchronization then modified folders must be placed
// first (from root to leaves) to UID set and finally items.
// ------------------------------------------------------------------------------------------------	
EXPORT_C void CNSmlDSHostClient::ModifiedItemsL( RNSmlDbItemModificationSet& aUidSet, const TSmlDataProviderId aId, const TDesC& aStoreName, TInt& aResultCode ) const
	{
	switch( SessionForDataProvider( aId ) )
		{
		case ( ENSmlHostServer1 ):
			iClient1.ModifiedItemsL( aUidSet, aId, aStoreName, aResultCode );
			break;
		case ( ENSmlHostServer2 ):
			iClient2.ModifiedItemsL( aUidSet, aId, aStoreName, aResultCode );
			break;
		case ( ENSmlHostServerEnd ):
		default:
			User::Leave( KErrNotFound );
		}
	aResultCode = HostErrorConversion( aResultCode );
	}

// ------------------------------------------------------------------------------------------------
// CNSmlDSHostClient::MovedItemsL
// The Data Provider returns UIDs of items that are moved after previous synchronization.
// If the Data Provider uses hierarchical synchronization then moved folders must be placed
// first (from root to leaves) to UID set and finally items.
// ------------------------------------------------------------------------------------------------	
EXPORT_C void CNSmlDSHostClient::MovedItemsL( RNSmlDbItemModificationSet& aUidSet, const TSmlDataProviderId aId, const TDesC& aStoreName, TInt& aResultCode ) const
	{
	switch( SessionForDataProvider( aId ) )
		{
		case ( ENSmlHostServer1 ):
			iClient1.MovedItemsL( aUidSet, aId, aStoreName, aResultCode );
			break;
		case ( ENSmlHostServer2 ):
			iClient2.MovedItemsL( aUidSet, aId, aStoreName, aResultCode );
			break;
		case ( ENSmlHostServerEnd ):
		default:
			User::Leave( KErrNotFound );
		}
	aResultCode = HostErrorConversion( aResultCode );
	}

// ------------------------------------------------------------------------------------------------
// CNSmlDSHostClient::AllItemsL
// The Data Provider returns UIDs of items that are added, deleted, modified, softdeleted
// or moved after previous synchronization.
// ------------------------------------------------------------------------------------------------	
EXPORT_C void CNSmlDSHostClient::AllItemsL( RNSmlDbItemModificationSet& aUidSet, const TSmlDataProviderId aId, const TDesC& aStoreName, TInt& aResultCode ) const
	{
	switch( SessionForDataProvider( aId ) )
		{
		case ( ENSmlHostServer1 ):
			iClient1.AllItemsL( aUidSet, aId, aStoreName, aResultCode );
			break;
		case ( ENSmlHostServer2 ):
			iClient2.AllItemsL( aUidSet, aId, aStoreName, aResultCode );
			break;
		case ( ENSmlHostServerEnd ):
		default:
			User::Leave( KErrNotFound );
		}
	aResultCode = HostErrorConversion( aResultCode );
	}

// ------------------------------------------------------------------------------------------------
// CNSmlDSHostClient::ResetChangeInfoL
// Reset change info from the Data Provider. The result of this method is that the Data Provider
// sends just ADD commands to Sync Partner.
// ------------------------------------------------------------------------------------------------	
EXPORT_C void CNSmlDSHostClient::ResetChangeInfoL( const TSmlDataProviderId aId, const TDesC& aStoreName, TInt& aResultCode )
	{
	switch( SessionForDataProvider( aId ) )
		{
		case ( ENSmlHostServer1 ):
			iClient1.ResetChangeInfo( aId, aStoreName, aResultCode );
			break;
		case ( ENSmlHostServer2 ):
			iClient2.ResetChangeInfo( aId, aStoreName, aResultCode );
			break;
		case ( ENSmlHostServerEnd ):
		default:
			User::Leave( KErrNotFound );
		}
	aResultCode = HostErrorConversion( aResultCode );
	}
		
// ------------------------------------------------------------------------------------------------
// CNSmlDSHostClient::CommitChangeInfoL
// This method is called after some changes are synchronized to Sync Partner. If some changes
// were synchronized correctly then those UIDs are included to aItems.
// ------------------------------------------------------------------------------------------------	
EXPORT_C void CNSmlDSHostClient::CommitChangeInfoL( const MSmlDataItemUidSet& aItems, const TSmlDataProviderId aId, const TDesC& aStoreName, TInt& aResultCode )
	{
	switch( SessionForDataProvider( aId ) )
		{
		case ( ENSmlHostServer1 ):
			iClient1.CommitChangeInfoL( aItems, aId, aStoreName, aResultCode );
			break;
		case ( ENSmlHostServer2 ):
			iClient2.CommitChangeInfoL( aItems, aId, aStoreName, aResultCode );
			break;
		case ( ENSmlHostServerEnd ):
		default:
			User::Leave( KErrNotFound );
		}
	aResultCode = HostErrorConversion( aResultCode );
	}

// ------------------------------------------------------------------------------------------------
// CNSmlDSHostClient::CommitChangeInfoL
// This method is called after some changes are synchronized to Sync Partner. This method is used if
// all changes were synchronized correctly.
// ------------------------------------------------------------------------------------------------	
EXPORT_C void CNSmlDSHostClient::CommitChangeInfoL( const TSmlDataProviderId aId, const TDesC& aStoreName, TInt& aResultCode )
	{
	switch( SessionForDataProvider( aId ) )
		{
		case ( ENSmlHostServer1 ):
			iClient1.CommitChangeInfo( aId, aStoreName, aResultCode );
			break;
		case ( ENSmlHostServer2 ):
			iClient2.CommitChangeInfo( aId, aStoreName, aResultCode );
			break;
		case ( ENSmlHostServerEnd ):
		default:
			User::Leave( KErrNotFound );
		}
	aResultCode = HostErrorConversion( aResultCode );
	}

// ------------------------------------------------------------------------------------------------
// CNSmlDSHostClient::UpdateServerIdL
// Replaces the give old server ID with new one.
// ------------------------------------------------------------------------------------------------		
EXPORT_C void CNSmlDSHostClient::UpdateServerIdL( TDesC& aOldServerId, TDesC& aNewValue, TInt& aResultCode )
	{
        iClient1.UpdateServerIdL( aOldServerId, aNewValue, aResultCode );
        iClient2.UpdateServerIdL( aOldServerId, aNewValue, aResultCode );
	aResultCode = HostErrorConversion( aResultCode );
	}

// ------------------------------------------------------------------------------------------------
// CNSmlDSHostClient - private methods
// ------------------------------------------------------------------------------------------------
// ------------------------------------------------------------------------------------------------
// CNSmlDSHostClient::CNSmlDSHostClient
// C++ constructor.
// ------------------------------------------------------------------------------------------------
CNSmlDSHostClient::CNSmlDSHostClient()
	{
	}

// ------------------------------------------------------------------------------------------------
// CNSmlDSHostClient::ConstrucL
// ConstrucL
// ------------------------------------------------------------------------------------------------
void CNSmlDSHostClient::ConstructL()
	{
	TVersion DSHostServerVersion( KNSmlDSHostServerVersionMajor, 
		KNSmlDSHostServerVersionMinor, KNSmlDSHostServerVersionBuild );
			
	User::LeaveIfError( iClient1.Connect( KNSmlDSHostServer1Name, DSHostServerVersion ) );
	User::LeaveIfError( iClient2.Connect( KNSmlDSHostServer2Name, DSHostServerVersion ) );
	User::LeaveIfError( iClient1.CreateChunk() );
	User::LeaveIfError( iClient1.SendChunkHandle() );
	User::LeaveIfError( iClient2.CreateChunk() );
	User::LeaveIfError( iClient2.SendChunkHandle() );
	}

// ------------------------------------------------------------------------------------------------
// CNSmlDSHostClient::SessionForDataProvider
// Return session to correct server that has current Data Provider.
// ------------------------------------------------------------------------------------------------
TInt CNSmlDSHostClient::SessionForDataProvider( const TSmlDataProviderId aId ) const
	{
	for ( TInt i = 0; i < iDataProviders1.Count(); i++ ) 
		{
		if ( iDataProviders1[i] == aId )
			{
			return ENSmlHostServer1;
			}
		}
	for ( TInt i = 0; i < iDataProviders2.Count(); i++ ) 
		{
		if ( iDataProviders2[i] == aId )
			{
			return ENSmlHostServer2;
			}
		}
	return ENSmlHostServerEnd;
	}
	
// ------------------------------------------------------------------------------------------------
// CNSmlDSHostClient::HostErrorConversion
// ------------------------------------------------------------------------------------------------
TInt CNSmlDSHostClient::HostErrorConversion( TInt aStatusFromHost )
	{
	TInt ret ( aStatusFromHost );
	
	// in module tests no conversion.
#ifndef __HOST_SERVER_MTEST__
	switch( aStatusFromHost )
		{
	case ENSmlDSHostServerErrNone:
		ret = KErrNone;
		break;
	case ENSmlDSHostServerErrDPNotCreated:
		ret = KErrNotFound;
		break;
	case ENSmlDSHostServerErrDSNotOpened:
		ret = KErrNotFound;
		break;
	case ENSmlDSHostServerErrAsynchCallOutstanding:
		ret = KErrServerBusy;
		break;
	default:
		ret = aStatusFromHost;
		}
#endif
	return ret;
	}

#endif // __USE_FAKE_CLIENT__

// End of File
