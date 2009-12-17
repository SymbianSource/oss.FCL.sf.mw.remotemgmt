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
* Description:  DS data provider base.
*
*/


#include <SmlDataProvider.h>
#include <SyncMLDataFilter.h>
#include <SyncMLDef.h>
#include <ecom.h>

// ------------------------------------------------------------------------------------------------
// CSmlDataProvider::NewL
// Creates new dataprovider instance
// ------------------------------------------------------------------------------------------------
EXPORT_C CSmlDataProvider* CSmlDataProvider::NewL( TSmlDataProviderId aId )
{
	TUid ownuid = {aId};
	TAny* any = REComSession::CreateImplementationL(ownuid, _FOFF(CSmlDataProvider, iEComTag));
	
	CSmlDataProvider* object = reinterpret_cast<CSmlDataProvider*>( any );
	object->iDPId = aId;
	return object;
}


// ------------------------------------------------------------------------------------------------
// CSmlDataProvider::~CSmlDataProvider
// Destructor
// ------------------------------------------------------------------------------------------------
EXPORT_C CSmlDataProvider::~CSmlDataProvider()
{
	REComSession::DestroyedImplementation( iEComTag );
}


// ------------------------------------------------------------------------------------------------
// CSmlDataProvider::Identifier
// Returns the dataprovider id.
// ------------------------------------------------------------------------------------------------
EXPORT_C TSmlDataProviderId CSmlDataProvider::Identifier() const
{
	return iDPId;
}


// ------------------------------------------------------------------------------------------------
// CSmlDataProvider::OnFrameworkEvent
// Calls virtual method: DoOnFrameworkEvent
//
// ------------------------------------------------------------------------------------------------
EXPORT_C void CSmlDataProvider::OnFrameworkEvent( TSmlFrameworkEvent aEvent, TInt aParam1, TInt aParam2 )
{
	return DoOnFrameworkEvent( aEvent, aParam1, aParam2 );
}


// ------------------------------------------------------------------------------------------------
// CSmlDataProvider::SupportsOperation
// Calls virtual method: DoSupportsOperation
// ------------------------------------------------------------------------------------------------
EXPORT_C TBool CSmlDataProvider::SupportsOperation( TUid aOpId ) const
{
	return DoSupportsOperation( aOpId );
}


// ------------------------------------------------------------------------------------------------
// CSmlDataProvider::StoreFormatL
// Calls virtual method: DoStoreFormatL
// ------------------------------------------------------------------------------------------------
EXPORT_C const CSmlDataStoreFormat& CSmlDataProvider::StoreFormatL()
{
	return DoStoreFormatL();
}


// ------------------------------------------------------------------------------------------------
// CSmlDataProvider::ListStoresLC
// Calls virtual method: DoListStoresLC
// ------------------------------------------------------------------------------------------------
EXPORT_C CDesCArray* CSmlDataProvider::ListStoresLC()
{
	return DoListStoresLC();
}

// ------------------------------------------------------------------------------------------------
// CSmlDataProvider::DefaultStoreL
// Calls virtual method: DoDefaultStoreL
// ------------------------------------------------------------------------------------------------
EXPORT_C const TDesC& CSmlDataProvider::DefaultStoreL()
{
	return DoDefaultStoreL();
}


// ------------------------------------------------------------------------------------------------
// CSmlDataProvider::NewStoreInstanceLC
// Calls virtual method: DoNewStoreInstanceLC
// ------------------------------------------------------------------------------------------------
EXPORT_C CSmlDataStore* CSmlDataProvider::NewStoreInstanceLC()
{
	return DoNewStoreInstanceLC();
}

// ------------------------------------------------------------------------------------------------
// CSmlDataProvider::SupportedServerFiltersL
// This method returns the set of filters that can be used to send to the SyncML server.
// ------------------------------------------------------------------------------------------------
EXPORT_C const RPointerArray<CSyncMLFilter>& CSmlDataProvider::SupportedServerFiltersL()
{
	return DoSupportedServerFiltersL();
}

// ------------------------------------------------------------------------------------------------
// CSmlDataProvider::CheckSupportedServerFiltersL
// This method checks what filters are supported by server.
// ------------------------------------------------------------------------------------------------
EXPORT_C void CSmlDataProvider::CheckSupportedServerFiltersL( const CSmlDataStoreFormat& aServerDataStoreFormat, RPointerArray<CSyncMLFilter>& aFilters, TSyncMLFilterChangeInfo& aChangeInfo )
{
	return DoCheckSupportedServerFiltersL( aServerDataStoreFormat, aFilters, aChangeInfo );
}

// ------------------------------------------------------------------------------------------------
// CSmlDataProvider::CheckServerFiltersL
// This method updates dynamic filters up-to-date.
// ------------------------------------------------------------------------------------------------
EXPORT_C void CSmlDataProvider::CheckServerFiltersL( RPointerArray<CSyncMLFilter>& aFilters, TSyncMLFilterChangeInfo& aChangeInfo )
{
	return DoCheckServerFiltersL( aFilters, aChangeInfo );
}

// ------------------------------------------------------------------------------------------------
// CSmlDataProvider::GenerateRecordFilterQueryL
// This method generates a record filter query to be sent to the SyncML server for the provided filter.
// ------------------------------------------------------------------------------------------------
EXPORT_C HBufC* CSmlDataProvider::GenerateRecordFilterQueryLC( const RPointerArray<CSyncMLFilter>& aFilters, TSyncMLFilterMatchType aMatch, TDes& aFilterMimeType, TSyncMLFilterType& aFilterType, TDesC& aStoreName )
{
	return DoGenerateRecordFilterQueryLC( aFilters, aMatch, aFilterMimeType, aFilterType, aStoreName );
}

// ------------------------------------------------------------------------------------------------
// CSmlDataProvider::GenerateFieldFilterQueryL
// This method generates a field filter query to be sent to the SyncML server for the provided filter.
// ------------------------------------------------------------------------------------------------
EXPORT_C void CSmlDataProvider::GenerateFieldFilterQueryL( const RPointerArray<CSyncMLFilter>& aFilters, TDes& aFilterMimeType, RPointerArray<CSmlDataProperty>& aProperties, TDesC& aStoreName )
{
	return DoGenerateFieldFilterQueryL( aFilters, aFilterMimeType, aProperties, aStoreName );
}

// End of File
