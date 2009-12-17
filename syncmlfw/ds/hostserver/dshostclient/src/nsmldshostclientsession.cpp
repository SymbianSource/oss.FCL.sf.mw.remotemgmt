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
* Description:  Client Session for DS Host Servers
*
*/


// ------------------------------------------------------------------------------------------------
// Includes
// ------------------------------------------------------------------------------------------------
#include <s32mem.h>
#include <data_caging_path_literals.hrh>
#include <f32file.h>

#include <nsmlconstants.h>
#include <nsmlunicodeconverter.h>
#include "nsmldbcaps.h"
#include "nsmldshostclientsession.h"
#include "nsmldshostconstants.h"
#include "NSmldbcapsSerializer.h"
#include "nsmldsdpinformation.h"
#include "nsmldshostitem.h"
#include "nsmldsitemmodificationset.h"
#include "nsmlfilter.h"

// ------------------------------------------------------------------------------------------------
// Constants
// ------------------------------------------------------------------------------------------------
_LIT8( KNSmlFilterTypeEXCLUSIVE, "EXCLUSIVE" );
_LIT8( KNSmlFilterTypeINCLUSIVE, "INCLUSIVE" );
_LIT8( KNSmlFilterTypeCGI, "syncml::filtertype-cgi" );

// ------------------------------------------------------------------------------------------------
// RNSmlDSHostClient - public methods
// ------------------------------------------------------------------------------------------------
// ------------------------------------------------------------------------------------------------
// RNSmlDSHostClient::Connect
// Connects client to Host Servers.
// ------------------------------------------------------------------------------------------------
TInt RNSmlDSHostClient::Connect( const TDesC& aServerName, const TVersion& aVersion )
	{
	iMode = ENSmlNormalMode;
	TInt result( CreateSession( aServerName, aVersion ) );
	
	if ( result == KErrNotFound || result == KErrServerTerminated )
		{
		result = LaunchServer( aServerName );
		if ( result == KErrNone || result == KErrAlreadyExists )
			{
			result = CreateSession( aServerName, aVersion );
			}
		}
	return result;
	}

// ------------------------------------------------------------------------------------------------
// RNSmlDSHostClient::Close
// Closes connection between client and Host Server.
// ------------------------------------------------------------------------------------------------
void RNSmlDSHostClient::Close()
	{
	iChunk.Close();
	RSessionBase::Close();
	}

// ------------------------------------------------------------------------------------------------
// RNSmlDSHostClient::CreateChunk
// Creates chunk that is used to transfer data between client and server.
// ------------------------------------------------------------------------------------------------
TInt RNSmlDSHostClient::CreateChunk() const
	{
	return iChunk.CreateGlobal( KNullDesC, KNSmlDSHostChunkMinSize, KNSmlDSHostChunkMaxSize );
	}

// ------------------------------------------------------------------------------------------------
// RNSmlDSHostClient::SendChunkHandle
// Sends Chunk handle to SyncML DS Host Servers.
// ------------------------------------------------------------------------------------------------
TInt RNSmlDSHostClient::SendChunkHandle() const
	{
	TIpcArgs args;
 	args.Set(0, iChunk);
 	const TInt error = SendReceive( ENSmlHandleChunk, args );
 	return error;
	}
	
// ------------------------------------------------------------------------------------------------
// RNSmlDSHostClient::CreateDataProvidersL
// Creates Data Providers.
// ------------------------------------------------------------------------------------------------
void RNSmlDSHostClient::CreateDataProvidersL( const RArray<TSmlDataProviderId>& aIds, RArray<TInt>& aResultArray )
	{
	AdjustChunkIfNeededLC( sizeof( TInt32 ) * ( aIds.Count() + 1 ) );
	RMemWriteStream writeStream( iChunk.Base(), iChunk.Size() );
	CleanupClosePushL( writeStream );
	writeStream.WriteInt32L( aIds.Count() );
	
	for ( TInt i = 0; i < aIds.Count(); i++ )
		{
		writeStream.WriteInt32L( aIds[i] );
		}
		
	writeStream.CommitL();
	CleanupStack::PopAndDestroy(); // writeStream
	
	User::LeaveIfError( SendReceive( ENSmlDPOpen, TIpcArgs() ) );
	
	RMemReadStream readStream( iChunk.Base(), iChunk.Size() );
	CleanupClosePushL( readStream );
	TInt count( readStream.ReadInt32L() );
	
	for ( TInt j = 0; j < count; j++ )
		{
		aResultArray.AppendL( readStream.ReadInt32L() );
		}
		
	CleanupStack::PopAndDestroy(2); // readStream, AdjustChunkIfNeededLC
	}
	
// ------------------------------------------------------------------------------------------------
// RNSmlDSHostClient::CreateAllDataProvidersL
// Creates all possible Data Providers.
// ------------------------------------------------------------------------------------------------
void RNSmlDSHostClient::CreateAllDataProvidersL( RArray<TSmlDataProviderId>& aIds, const RArray<TSmlDataProviderId>& aExceptIds )
	{
	AdjustChunkIfNeededLC( sizeof( TInt32 ) * ( aIds.Count() + 1 ) );
	RMemWriteStream writeStream( iChunk.Base(), iChunk.Size() );
	CleanupClosePushL( writeStream );
	writeStream.WriteInt32L( aExceptIds.Count() );
	
	for ( TInt i = 0; i < aExceptIds.Count(); i++ )
		{
		writeStream.WriteInt32L( aExceptIds[ i ] );
		}
	
	writeStream.CommitL();
	CleanupStack::PopAndDestroy(); // writeStream
	
	User::LeaveIfError( SendReceive( ENSmlDPOpenExcept, TIpcArgs() ) );
	
	RMemReadStream readStream( iChunk.Base(), iChunk.Size() );
	CleanupClosePushL( readStream );
	TInt count( readStream.ReadInt32L() );
	
	for ( TInt j = 0; j < count; j++ )
		{
		aIds.AppendL( readStream.ReadInt32L() );
		}

	CleanupStack::PopAndDestroy(2); // readStream, AdjustChunkIfNeededLC
	}

// ------------------------------------------------------------------------------------------------
// RNSmlDSHostClient::DataProviderInformationL
// 
// ------------------------------------------------------------------------------------------------	
TNSmlDPInformation* RNSmlDSHostClient::DataProviderInformationL( const TSmlDataProviderId aId, TInt& aResultCode ) const
	{
	TNSmlDPInformation* info = NULL;
	aResultCode = SendReceive( ENSmlDPInformation, TIpcArgs( aId ) );
	CleanupStack::PushL( TCleanupItem ( CancelAdjust, const_cast<RNSmlDSHostClient*>( this ) ) );
	if ( aResultCode == KErrNone )
		{
		CNSmlDPInformation* dpi = CNSmlDPInformation::NewLC();
		
		RMemReadStream readStream( iChunk.Base(), iChunk.Size() );
		CleanupClosePushL( readStream );
		dpi->InternalizeL( readStream );
			
		CleanupStack::PopAndDestroy(); //readStream
		CleanupStack::Pop( dpi );
		info = dpi->InformationD();
		}
	CleanupStack::PopAndDestroy(); //CancelAdjust
	return info;
	}
// ------------------------------------------------------------------------------------------------
// RNSmlDSHostClient::SupportsOperation
// Asks if Data Provider supports some operation.
// ------------------------------------------------------------------------------------------------
TBool RNSmlDSHostClient::SupportsOperationL( TUid aOpId, const TSmlDataProviderId aId, TInt& aResultCode ) const
	{
	
	RMemWriteStream writeStream( iChunk.Base(), iChunk.Size() );
	CleanupClosePushL( writeStream );
	writeStream.WriteInt32L( aOpId.iUid );
	writeStream.CommitL();
	CleanupStack::PopAndDestroy(); // writeStream
	
	TPckgBuf<TInt> pckg;
		
	aResultCode = SendReceive( ENSmlDPSupportsOperation, TIpcArgs( aId, TIpcArgs::ENothing, &pckg ) );
	return pckg();
	}

// ------------------------------------------------------------------------------------------------
// RNSmlDSHostClient::StoreFormatL
// Creates Data Store format of Data Provider.
// ------------------------------------------------------------------------------------------------
CNSmlDbCaps* RNSmlDSHostClient::StoreFormatL( const TSmlDataProviderId aId, TInt& aResultCode )
	{
	TNSmlDbCapsSerializer ser;
	CNSmlDbCaps* dbcaps = NULL;
	
	aResultCode = SendReceive( ENSmlDPStoreFormat, TIpcArgs( aId ) );
	CleanupStack::PushL( TCleanupItem ( CancelAdjust, this ) );
	
	if ( aResultCode == KErrNone )
		{
		RMemReadStream readStream( iChunk.Base(), iChunk.Size() );
		CleanupClosePushL( readStream );
		dbcaps = ser.InternalizeL( readStream );
		CleanupStack::PopAndDestroy(); // readStream
		}
	CleanupStack::PopAndDestroy(); //CancelAdjust
		
	return dbcaps;
	}
	
// ------------------------------------------------------------------------------------------------
// RNSmlDSHostClient::ListStoresL
// Creates list of Data Store names of Data Provider.
// ------------------------------------------------------------------------------------------------	
void RNSmlDSHostClient::ListStoresL( CDesCArray* aNameList, const TSmlDataProviderId aId, TInt& aResultCode )
	{
	aNameList->Reset();
	aResultCode = SendReceive( ENSmlDPlListStores, TIpcArgs( aId ) );
	CleanupStack::PushL( TCleanupItem ( CancelAdjust, this ) );
	
	if ( aResultCode == KErrNone )
		{
		RMemReadStream readStream( iChunk.Base(), iChunk.Size() );
		CleanupClosePushL( readStream );
		TInt32 i(0), storeCount( readStream.ReadInt32L() );
		
		for (; i < storeCount; i++ )
			{
			TInt32 length( readStream.ReadInt32L() );
			HBufC* store = HBufC::NewLC( readStream, length );
			
			aNameList->AppendL( *store );
			CleanupStack::PopAndDestroy( store );
			}
			CleanupStack::PopAndDestroy(); // readStream
		}
		
	CleanupStack::PopAndDestroy(); //CancelAdjust
	}

// ------------------------------------------------------------------------------------------------
// RNSmlDSHostClient::DefaultStoreL
// Creates default name for Data Store of Data Provider.
// ------------------------------------------------------------------------------------------------	
HBufC* RNSmlDSHostClient::DefaultStoreL( const TSmlDataProviderId aId, TInt& aResultCode ) const
	{
	aResultCode = SendReceive( ENSmlDPDefaultStore, TIpcArgs( aId ) );
	if ( aResultCode )
		{
		return NULL;
		}
	
	RMemReadStream readStream( iChunk.Base(), iChunk.Size() );
	CleanupClosePushL( readStream );
	
	TInt tempLength( readStream.ReadInt32L() );
	HBufC* tempName = HBufC::NewL( readStream, tempLength );

	CleanupStack::PopAndDestroy(); // readStream
	return tempName;
	}

// ------------------------------------------------------------------------------------------------
// RNSmlDSHostClient::SupportedServerFiltersL
// This method returns the set of filters that can be used to send to the Sync Partner.
// ------------------------------------------------------------------------------------------------	
RPointerArray<CSyncMLFilter>* RNSmlDSHostClient::SupportedServerFiltersL( const TSmlDataProviderId aId, 
	TSyncMLFilterMatchType& aMatchType, TSyncMLFilterChangeInfo& aChangeInfo, TInt& aResultCode ) const
	{
	TPckgBuf<TSyncMLFilterMatchType> pckg;

	aResultCode = SendReceive( ENSmlServerFilters, TIpcArgs( aId, TIpcArgs::ENothing, &pckg ) );
	aMatchType = pckg();
	
	if ( aResultCode )
		{
		return NULL;
		}
		
	aChangeInfo = ESyncMLDefault;
	
	RPointerArray<CSyncMLFilter>* array = new ( ELeave ) RPointerArray<CSyncMLFilter>();
	CleanupRPtrArrayPushL( array );
	RMemReadStream readStream( iChunk.Base(), iChunk.Size() );
	
	CleanupClosePushL( readStream );
	InternalizeFiltersL( readStream, *array );
	CleanupStack::PopAndDestroy(); // readStream
	CleanupStack::Pop(); //array
	
	return array;
	}

// ------------------------------------------------------------------------------------------------
// RNSmlDSHostClient::CheckServerFiltersL
// This method updates dynamic filters up-to-date.
// ------------------------------------------------------------------------------------------------
void RNSmlDSHostClient::CheckServerFiltersL( const TSmlDataProviderId aId, RPointerArray<CSyncMLFilter>& aFilters, TSyncMLFilterChangeInfo& aChangeInfo, TInt& aResultCode ) const
	{
	TStreamBuffers* sb = StreamBufferLC();	
	sb->iWrite->WriteInt32L( aChangeInfo );	
	ExternalizeFiltersL( *sb->iWrite, aFilters );
	sb->iWrite->CommitL();
	
	AdjustChunkIfNeededLC( sb->iBuffer->Size() );
	
	RMemWriteStream writeStream( iChunk.Base(), iChunk.Size() );
	CleanupClosePushL( writeStream );
	writeStream.WriteL( *sb->iRead );
	writeStream.CommitL();
	CleanupStack::PopAndDestroy(); // writeStream
	
	aResultCode = SendReceive( ENSmlUpdateServerFilters, TIpcArgs( aId ) );
	
	RMemReadStream readStream( iChunk.Base(), iChunk.Size() );
	CleanupClosePushL( readStream );
	
	aChangeInfo = static_cast<TSyncMLFilterChangeInfo>( readStream.ReadInt32L() );
	InternalizeFiltersL( readStream, aFilters );
	CleanupStack::PopAndDestroy(3); // readStream, AdjustChunkIfNeededLC, sb
	}	

// ------------------------------------------------------------------------------------------------
// RNSmlDSHostClient::CheckSupportedServerFiltersL
// This method checks what filters are supported by server
// ------------------------------------------------------------------------------------------------	
void RNSmlDSHostClient::CheckSupportedServerFiltersL( const TSmlDataProviderId aId, 
	const CNSmlDbCaps& aServerDataStoreFormat, const CArrayFix<TNSmlFilterCapData>& aFilterInfoArr, 
	RPointerArray<CSyncMLFilter>& aFilters, TSyncMLFilterChangeInfo& aChangeInfo, TInt& aResultCode ) const
	{
	TNSmlDbCapsSerializer ser;
	TStreamBuffers* sb = StreamBufferLC();	
	
	sb->iWrite->WriteInt32L( aChangeInfo );
	ser.ExternalizeL( aServerDataStoreFormat, aFilterInfoArr, *sb->iWrite );
	ExternalizeFiltersL( *sb->iWrite, aFilters );
	sb->iWrite->CommitL();
	
	AdjustChunkIfNeededLC( sb->iBuffer->Size() );
	
	RMemWriteStream writeStream( iChunk.Base(), iChunk.Size() );
	CleanupClosePushL( writeStream );
	writeStream.WriteL( *sb->iRead );
	writeStream.CommitL();
	CleanupStack::PopAndDestroy(); // writeStream
	
	aResultCode = SendReceive( ENSmlCheckSupportedServerFilters, TIpcArgs( aId ) );
	
	if ( aResultCode == KErrNone )
		{
		RMemReadStream readStream( iChunk.Base(), iChunk.Size() );
		CleanupClosePushL( readStream );
		aChangeInfo = static_cast<TSyncMLFilterChangeInfo>( readStream.ReadInt32L() );
		InternalizeFiltersL( readStream, aFilters );
		CleanupStack::PopAndDestroy(); //readStream
		}
	
	CleanupStack::PopAndDestroy(2); //AdjustChunkIfNeededLC, sb
	}

// ------------------------------------------------------------------------------------------------
// CNSmlDSHostClient::GetFilterL
// Get Filters.
// ------------------------------------------------------------------------------------------------	
void RNSmlDSHostClient::GetFilterL( const TSmlDataProviderId aId, const TDesC& aStoreName, const RPointerArray<CSyncMLFilter>& aFilterArray, CNSmlFilter*& aFilter, TSyncMLFilterMatchType aMatchType, TInt& aResultCode )
	{
	TStreamBuffers* sb = StreamBufferLC();
	sb->iWrite->WriteInt32L( aMatchType );
	ExternalizeFiltersL( *sb->iWrite, aFilterArray );
	sb->iWrite->CommitL();
	
	AdjustChunkIfNeededLC( sb->iBuffer->Size() );
	
	RMemWriteStream writeStream( iChunk.Base(), iChunk.Size() );
	CleanupClosePushL( writeStream );
	writeStream.WriteL( *sb->iRead );
	writeStream.CommitL();
	CleanupStack::PopAndDestroy(); // writeStream

	aResultCode = SendReceive( ENSmlFilters, TIpcArgs( aId, &aStoreName ) );
	if ( aResultCode == KErrNone )
		{
		GetFilterFromChunkL( aFilter );
		}
	
	CleanupStack::PopAndDestroy(2); //AdjustChunkIfNeededLC, sb 
	}

// ------------------------------------------------------------------------------------------------
// CNSmlDSHostClient::GetFilterFromChunkL
// creates filter from chunk
// ------------------------------------------------------------------------------------------------		
void RNSmlDSHostClient::GetFilterFromChunkL( CNSmlFilter*& aFilter )
	{
	RMemReadStream readStream( iChunk.Base(), iChunk.Size() );
	CleanupClosePushL( readStream );
	TSyncMLFilterType filterType( static_cast<TSyncMLFilterType>( readStream.ReadInt32L() ) ); 
	HBufC* query = HBufC::NewLC( readStream, readStream.ReadUint32L() );
	HBufC* recordMimeType = HBufC::NewLC( readStream, readStream.ReadUint32L() );
	HBufC* fieldMimeType = HBufC::NewLC( readStream, readStream.ReadUint32L() );
	
	RStringPool pool;
	pool.OpenL();
	CleanupClosePushL( pool );			
	
	//properties
	RPointerArray<CSmlDataProperty> properties;
    CleanupStack::PushL( PtrArrCleanupItemRArr( 
			CSmlDataProperty, &properties ) );
			
	TInt count( readStream.ReadUint32L() );
	for ( TInt i = 0; i < count; i++ )
		{
		CSmlDataProperty* prop = CSmlDataProperty::NewLC( pool, readStream );
		properties.AppendL( prop );
		CleanupStack::Pop( prop );
		}
		
	aFilter = CNSmlFilter::NewLC();
		
	if ( filterType == ESyncMLTypeInclusive )
		{
		aFilter->SetFilterTypeL( KNSmlFilterTypeINCLUSIVE );
		}
	else if ( filterType == ESyncMLTypeExclusive )
		{
		aFilter->SetFilterTypeL( KNSmlFilterTypeEXCLUSIVE );
		}
		
	HBufC8* tmpbuf = NULL;
	NSmlUnicodeConverter::HBufC8InUTF8LC( *recordMimeType, tmpbuf );
	aFilter->SetFilterMetaTypeL( *tmpbuf );
	CleanupStack::PopAndDestroy();
	NSmlUnicodeConverter::HBufC8InUTF8LC( *fieldMimeType, tmpbuf );
	aFilter->SetFieldMetaTypeL( *tmpbuf );
	CleanupStack::PopAndDestroy();
	NSmlUnicodeConverter::HBufC8InUTF8LC( *query, tmpbuf );
	aFilter->SetRecordL( KNSmlFilterTypeCGI, *tmpbuf );
	CleanupStack::PopAndDestroy();
		
	AddFilterPropertiesL( properties, *aFilter );
	
	CleanupStack::Pop(); // aFilter
	CleanupStack::PopAndDestroy(6); // pool, properties, fieldMimeType, recordMimeType, query, readStream
	}

// ------------------------------------------------------------------------------------------------
// RNSmlDSHostClient::AddFilterPropertiesL
// ------------------------------------------------------------------------------------------------	
void RNSmlDSHostClient::AddFilterPropertiesL( const RPointerArray<CSmlDataProperty>& properties, CNSmlFilter& aFilter ) const
	{
	TNSmlDbCapsSerializer ser;
	for ( TInt i = 0; i < properties.Count(); i++ )
		{
		const CSmlDataProperty* prop = properties[i];
			
		CNSmlDevInfProp* dip = aFilter.AddFieldDataPropLC( prop->Field().Name().DesC() );
		ser.SetFromL( *prop, *dip );
		CleanupStack::Pop(); // dip
		}
	}

// ------------------------------------------------------------------------------------------------
// RNSmlDSHostClient::OpenL
// Opens the data store specified by aStoreName asynchronously.
// ------------------------------------------------------------------------------------------------	
void RNSmlDSHostClient::OpenL( const TSmlDataProviderId aId, const TDesC& aStoreName, const TDesC& aServerId, const TDesC& aRemoteDB, TInt& aResultCode ) const
	{
	RMemWriteStream writeStream( iChunk.Base(), iChunk.Size() );
	CleanupClosePushL( writeStream );
	
	writeStream.WriteUint32L( aServerId.Length() );
	writeStream << aServerId;
	writeStream.WriteUint32L( aRemoteDB.Length() );
	writeStream << aRemoteDB;
	
	CleanupStack::PopAndDestroy(); // writeStream
	
	aResultCode = SendReceive( ENSmlDSOpen, TIpcArgs( aId, &aStoreName ) );
	}

// ------------------------------------------------------------------------------------------------
// RNSmlDSHostClient::CancelRequest
// Cancel the current asynchronous request.
// ------------------------------------------------------------------------------------------------	
void RNSmlDSHostClient::CancelRequest( const TSmlDataProviderId aId, const TDesC& aStoreName, TInt& aResultCode ) const
	{
	aResultCode = SendReceive( ENSmlDSCancelRequest, TIpcArgs( aId, &aStoreName ) );
	}

// ------------------------------------------------------------------------------------------------
// RNSmlDSHostClient::BeginTransaction
// Starts the transaction mode.
// ------------------------------------------------------------------------------------------------	
void RNSmlDSHostClient::BeginTransaction( const TSmlDataProviderId aId, const TDesC& aStoreName, TInt& aResultCode )
	{
	iMode = ENSmlTransactionMode;
	aResultCode = SendReceive( ENSmlDSBeginTransaction, TIpcArgs( aId, &aStoreName ) );
	}

// ------------------------------------------------------------------------------------------------
// RNSmlDSHostClient::CommitTransaction
// Method will be called at the end of a successful transaction.
// ------------------------------------------------------------------------------------------------	
void RNSmlDSHostClient::CommitTransaction( const TSmlDataProviderId aId, const TDesC& aStoreName, TInt& aResultCode )
	{
	iMode = ENSmlNormalMode;
	aResultCode = SendReceive( ENSmlDSCommitTransaction, TIpcArgs( aId, &aStoreName ) );
	}

// ------------------------------------------------------------------------------------------------
// RNSmlDSHostClient::RevertTransaction
// Method will be called to abort an ongoing transaction.
// ------------------------------------------------------------------------------------------------	
void RNSmlDSHostClient::RevertTransaction( const TSmlDataProviderId aId, const TDesC& aStoreName, TInt& aResultCode )
	{
	iMode = ENSmlNormalMode;
	aResultCode = SendReceive( ENSmlDSRevertTransaction, TIpcArgs( aId, &aStoreName ) );
	}

// ------------------------------------------------------------------------------------------------
// RNSmlDSHostClient::BeginBatch
// Starts the batch mode.
// ------------------------------------------------------------------------------------------------	
void RNSmlDSHostClient::BeginBatch( const TSmlDataProviderId aId, const TDesC& aStoreName, TInt& aResultCode )
	{
	iMode = ENSmlBatchMode;
	iAddedUidsBuffer.Reset();
	aResultCode = SendReceive( ENSmlDSBeginBatch, TIpcArgs( aId, &aStoreName ) );
	}
	
// ------------------------------------------------------------------------------------------------
// RNSmlDSHostClient::CommitBatchL
// Method will be called at the end of the batch mode.
// ------------------------------------------------------------------------------------------------	
void RNSmlDSHostClient::CommitBatchL( RArray<TInt>& aResultArray, const TSmlDataProviderId aId, const TDesC& aStoreName, TInt& aResultCode )
	{
	iMode = ENSmlNormalMode;
	
	RArray<TInt> addUids;
	CleanupClosePushL( addUids );
	
	aResultCode = SendReceive( ENSmlDSCommitBatch, TIpcArgs( aId, &aStoreName ) );
	CleanupStack::PushL( TCleanupItem ( CancelAdjust, this ) );
	
	if ( aResultCode == KErrNone )
		{
		// First read results
		RMemReadStream readStream( iChunk.Base(), iChunk.Size() );
		CleanupClosePushL( readStream );
		TInt resultCount( readStream.ReadInt32L() );
		for ( TInt j = 0; j < resultCount; j++ )
			{
			aResultArray.AppendL( readStream.ReadInt32L() );
			}
		// Second read uids references that are added
		TInt uidsCount( readStream.ReadInt32L() );
		for ( TInt j = 0; j < uidsCount; j++ )
			{
			addUids.AppendL( readStream.ReadInt32L() );
			}
		CleanupStack::PopAndDestroy(); // readStream
		
		if ( addUids.Count() != iAddedUidsBuffer.Count() )
			{
			User::Leave( KErrUnknown );
			}
		else
			{
			for ( TInt i = 0; i < addUids.Count(); i++ )
				{
				*( iAddedUidsBuffer[ i ] ) = addUids[ i ];
				}
			}
		}
		
	CleanupStack::PopAndDestroy(2); //CancelAdjust, addUids
	}

// ------------------------------------------------------------------------------------------------
// RNSmlDSHostClient::CancelBatchL
// Method will be called to abort an ongoing batch mode.
// ------------------------------------------------------------------------------------------------	
void RNSmlDSHostClient::CancelBatch( const TSmlDataProviderId aId, const TDesC& aStoreName, TInt& aResultCode )
	{
	iMode = ENSmlNormalMode;
	aResultCode = SendReceive( ENSmlDSCancelBatch, TIpcArgs( aId, &aStoreName ) );
	}
	
// ------------------------------------------------------------------------------------------------
// RNSmlDSHostClient::SetRemoteDataStoreFormatL
// Sets the Sync Partner Data Format.
// ------------------------------------------------------------------------------------------------	
void RNSmlDSHostClient::SetRemoteDataStoreFormatL( const CNSmlDbCaps& aServerDataStoreFormat, const TSmlDataProviderId aId, const TDesC& aStoreName, TInt& aResultCode )
	{
	TNSmlDbCapsSerializer ser;
	TStreamBuffers* sb = StreamBufferLC();
	ser.ExternalizeL( aServerDataStoreFormat, *sb->iWrite );
	sb->iWrite->CommitL();
	AdjustChunkIfNeededLC( sb->iBuffer->Size() );
	RMemWriteStream writeStream( iChunk.Base(), iChunk.Size() );
	CleanupClosePushL( writeStream );
	writeStream.WriteL( *sb->iRead );
	CleanupStack::PopAndDestroy(); // writeStream
	
	aResultCode = SendReceive( ENSmlDSSetDataStoreFormat, TIpcArgs( aId, &aStoreName ) );
	CleanupStack::PopAndDestroy(2); // AdjustChunkIfNeededLC, sb
	}
	
// ------------------------------------------------------------------------------------------------
// RNSmlDSHostClient::SetRemoteMaxObjectSizeL
// Sets the SyncML server Sync Partner maximum object size.
// ------------------------------------------------------------------------------------------------	
void RNSmlDSHostClient::SetRemoteMaxObjectSizeL( TInt aServerMaxObjectSize, const TSmlDataProviderId aId, const TDesC& aStoreName, TInt& aResultCode ) const
	{
	RMemWriteStream writeStream( iChunk.Base(), iChunk.Size() );
	CleanupClosePushL( writeStream );
	writeStream.WriteInt32L( aServerMaxObjectSize );
	writeStream.CommitL();
	CleanupStack::PopAndDestroy(); // writeStream
	
	aResultCode = SendReceive( ENSmlDSRemoteMaxObjSize, TIpcArgs( aId, &aStoreName ) );
	}
	
// ------------------------------------------------------------------------------------------------
// RNSmlDSHostClient::MaxObjectSize
// Gets the Data Store maximum object size which is reported to the SyncML partner.
// ------------------------------------------------------------------------------------------------	
TInt RNSmlDSHostClient::MaxObjectSize( const TSmlDataProviderId aId, const TDesC& aStoreName, TInt& aResultCode ) const
	{
	TPckgBuf<TInt> pckg;
	aResultCode = SendReceive( ENSmlMaxObjSize, TIpcArgs( aId, &aStoreName, &pckg ) );
	return pckg();
	}	

// ------------------------------------------------------------------------------------------------
// RNSmlDSHostClient::OpenItemL
// Opens item at Data Store.
// ------------------------------------------------------------------------------------------------	
void RNSmlDSHostClient::OpenItemL( TSmlDbItemUid aUid, TBool& aFieldChange, TInt& aSize, TSmlDbItemUid& aParent, HBufC8*& aMimeType, HBufC8*& aMimeVer, const TSmlDataProviderId aId, const TDesC& aStoreName, TInt& aResultCode ) const
	{
	
	CNSmlDSHostItem* item = CNSmlDSHostItem::NewLC();
	item->SetUid( aUid );
	
	RMemWriteStream writeStream( iChunk.Base(), iChunk.Size() );
	CleanupClosePushL( writeStream );
	item->ExternalizeL( writeStream );
	writeStream.CommitL();
	CleanupStack::PopAndDestroy(); // writeStream
	
	aResultCode = SendReceive( ENSmlItemOpen, TIpcArgs( aId, &aStoreName ) );
	
	if ( aResultCode == KErrNone )
		{
		RMemReadStream readStream( iChunk.Base(), iChunk.Size() );
		CleanupClosePushL( readStream );
		item->InternalizeL( readStream );

		CleanupStack::PopAndDestroy(); // readStream
		
		aFieldChange = item->FieldChange();
		aSize = item->Size();
		if ( item->ParentUid() != -2 )
			{
			aParent = item->ParentUid();
			}
		aMimeType = item->MimeType()->AllocLC();
		aMimeVer = item->MimeVer()->AllocLC();
		CleanupStack::Pop(2); //aMimeVer, aMimeType
		}
	
	CleanupStack::PopAndDestroy(); // item
	}

// ------------------------------------------------------------------------------------------------
// RNSmlDSHostClient::CreateItemL
// Creates new item to Data Store.
// ------------------------------------------------------------------------------------------------	
void RNSmlDSHostClient::CreateItemL( TSmlDbItemUid& aUid, TInt aSize, TSmlDbItemUid aParent, const TDesC8& aMimeType, const TDesC8& aMimeVer, const TSmlDataProviderId aId, const TDesC& aStoreName, TInt& aResultCode )
	{
	CNSmlDSHostItem* item = CNSmlDSHostItem::NewLC();
	item->SetUid( aUid );
	item->SetSize( aSize );
	item->SetParentUid( aParent );
	item->SetMimeTypeL( aMimeType );
	item->SetMimeVerL( aMimeVer );
	
	RMemWriteStream writeStream( iChunk.Base(), iChunk.Size() );
	CleanupClosePushL( writeStream );
	item->ExternalizeL( writeStream );
	writeStream.CommitL();
	CleanupStack::PopAndDestroy(); // writeStream
	
	aResultCode = SendReceive( ENSmlItemCreate, TIpcArgs( aId, &aStoreName ) );
	
	if ( aResultCode == KErrNone )
		{
		RMemReadStream readStream( iChunk.Base(), iChunk.Size() );
		CleanupClosePushL( readStream );
		item->InternalizeL( readStream );

		CleanupStack::PopAndDestroy(); // readStream
		
		aUid = item->Uid();
		iAddedUidBuffer = &aUid;
		if ( iMode == ENSmlBatchMode )
			{
			iAddedUidsBuffer.AppendL( &aUid );
			}
		iItemState = ENSmlItemCreating;
		}
	
	CleanupStack::PopAndDestroy(); // item
	}
	
// ------------------------------------------------------------------------------------------------
// RNSmlDSHostClient::ReplaceItemL
// Replaces old item at Data Store.
// ------------------------------------------------------------------------------------------------	
void RNSmlDSHostClient::ReplaceItemL( TSmlDbItemUid aUid, TInt aSize, TSmlDbItemUid aParent, TBool aFieldChange, TSmlDataProviderId aId, const TDesC& aStoreName, TInt& aResultCode )
	{
	CNSmlDSHostItem* item = CNSmlDSHostItem::NewLC();
	item->SetUid( aUid );
	item->SetSize( aSize );
	item->SetParentUid( aParent );
	item->SetFieldChange( aFieldChange );
	
	RMemWriteStream writeStream( iChunk.Base(), iChunk.Size() );
	CleanupClosePushL( writeStream );
	item->ExternalizeL( writeStream );
	writeStream.CommitL();
	CleanupStack::PopAndDestroy( 2 ); // writeStream, item
	
	aResultCode = SendReceive( ENSmlItemReplace, TIpcArgs( aId, &aStoreName ) );
	if ( aResultCode == KErrNone )
		{
		iItemState = ENSmlItemUpdating;
		}
	}

// ------------------------------------------------------------------------------------------------
// RNSmlDSHostClient::ReadItemL
// Reads data from item at Data Store. Item must be opened before this method can be called.
// This method is called until aBuffer is not used totally or method leaves with KErrEof.
// ------------------------------------------------------------------------------------------------	
void RNSmlDSHostClient::ReadItemL( TDes8& aBuffer, const TSmlDataProviderId aId, const TDesC& aStoreName, TInt& aResultCode ) const
	{
	RMemWriteStream writeStream;
	RMemReadStream readStream;
	CleanupClosePushL( writeStream );
	CleanupClosePushL( readStream );
	TInt size( aBuffer.MaxSize() );
	TInt bytesRead(0), pos(0);
	aBuffer.Zero();
	
	do
		{
		writeStream.Open( iChunk.Base(), iChunk.Size() );
		writeStream.WriteInt32L( size );
		
		aResultCode = SendReceive( ENSmlItemRead, TIpcArgs( aId, &aStoreName ) );
		if ( aResultCode != KErrNone )
			{
			break;
			}
		
		readStream.Open( iChunk.Base(), iChunk.Size() );
		bytesRead = readStream.ReadInt32L();
		TInt ll( aBuffer.Length() );
		aBuffer.SetLength( ll + bytesRead );
		TPtr8 des = aBuffer.MidTPtr( pos );
		readStream.ReadL( des, bytesRead );
		pos += bytesRead;
		
		} while( size -= bytesRead );
		
	CleanupStack::PopAndDestroy(2); //readStream, writeStream
	}
		
// ------------------------------------------------------------------------------------------------
// RNSmlDSHostClient::WriteItemL
// Writes data to item to Data Provider. CreateItemL or ReplaceItemL method must be called before
// this method can be called. This method is called until all data to current item is written.
// ------------------------------------------------------------------------------------------------	
void RNSmlDSHostClient::WriteItemL( const TDesC8& aData, const TSmlDataProviderId aId, const TDesC& aStoreName, TInt& aResultCode )
	{
	AdjustChunkLC( aData.Size() + sizeof( TInt32 ) );
		
	TInt sizeOfChunk( iChunk.Size() - sizeof( TInt32 ) );
		
	RMemWriteStream writeStream;
	CleanupClosePushL( writeStream );
	
	TInt sizeOfData( aData.Size() ), pos(0);
	
	do
		{
		TInt size( Min<TInt>( sizeOfData, sizeOfChunk ) );
		writeStream.Open( iChunk.Base(), iChunk.Size() );
		
		writeStream.WriteInt32L( size );
		writeStream.WriteL( aData.Mid( pos, size ) );
		writeStream.CommitL();
		aResultCode = SendReceive( ENSmlItemWrite, TIpcArgs( aId, &aStoreName ) );
		
		if ( aResultCode != KErrNone )
			{
			break;
			}
		
		TInt32 bytesWritten(0);
		TPckg<TInt32> bytesWrittenPckg( bytesWritten );
    	bytesWrittenPckg.Copy( iChunk.Base(), sizeof( TInt32 ) );
		
		if ( sizeOfData <= bytesWritten ) break;
		
		sizeOfData -= bytesWritten;
		pos += bytesWritten;
		
		} while( sizeOfData > 0 );
		
	
	CleanupStack::PopAndDestroy(2); // writeStream, AdjustChunkLC
	}
	
// ------------------------------------------------------------------------------------------------
// RNSmlDSHostClient::CommitItem
// After item is written to Data Provider it can be saved to the Data Store.
// This method can be called just after WriteItemL method.
// ------------------------------------------------------------------------------------------------	
void RNSmlDSHostClient::CommitItem( const TSmlDataProviderId aId, const TDesC& aStoreName, TInt& aResultCode )
	{
	TPckgBuf<TInt> pckg;
	
	if ( iItemState == ENSmlClosed || ( iItemState == ENSmlItemCreating && !iAddedUidBuffer ) )
		{
		aResultCode = KErrArgument;
		}
	else
		{
		
		aResultCode = SendReceive( ENSmlItemCommit, TIpcArgs( aId, &aStoreName, &pckg ) );
		
		if ( iItemState == ENSmlItemCreating )
			{
			*iAddedUidBuffer = pckg();
			}
		}
		
	if (aResultCode == KErrNone)
		{
		iItemState = ENSmlClosed;
		}
	}
	
// ------------------------------------------------------------------------------------------------
// RNSmlDSHostClient::CloseItem
// Closes opened item.
// ------------------------------------------------------------------------------------------------	
void RNSmlDSHostClient::CloseItem( const TSmlDataProviderId aId, const TDesC& aStoreName, TInt& aResultCode )
	{
	aResultCode = SendReceive( ENSmlItemClose, TIpcArgs( aId, &aStoreName ) );
	
	if (aResultCode == KErrNone)
		{
		iItemState = ENSmlClosed;
		}
	}
	
// ------------------------------------------------------------------------------------------------
// RNSmlDSHostClient::MoveItemL
// Moves item to new location.
// ------------------------------------------------------------------------------------------------	
void RNSmlDSHostClient::MoveItemL( TSmlDbItemUid aUid, TSmlDbItemUid aNewParent, const TSmlDataProviderId aId, const TDesC& aStoreName, TInt& aResultCode )
	{
	CNSmlDSHostItem* item = CNSmlDSHostItem::NewLC();
	item->SetUid( aUid );
	item->SetParentUid( aNewParent );
	
	RMemWriteStream writeStream( iChunk.Base(), iChunk.Size() );
	CleanupClosePushL( writeStream );
	item->ExternalizeL( writeStream );
	writeStream.CommitL();
	CleanupStack::PopAndDestroy( 2 ); // writeStream, item
	
	aResultCode = SendReceive( ENSmlItemMove, TIpcArgs( aId, &aStoreName ) );
	}
		
// ------------------------------------------------------------------------------------------------
// RNSmlDSHostClient::DeleteItemL
// Deletes one item at Data Store permanently.
// ------------------------------------------------------------------------------------------------	
void RNSmlDSHostClient::DeleteItemL( TSmlDbItemUid aUid, const TSmlDataProviderId aId, const TDesC& aStoreName, TInt& aResultCode ) const
	{
	CNSmlDSHostItem* item = CNSmlDSHostItem::NewLC();
	item->SetUid( aUid );
	
	RMemWriteStream writeStream( iChunk.Base(), iChunk.Size() );
	CleanupClosePushL( writeStream );
	item->ExternalizeL( writeStream );
	writeStream.CommitL();
	CleanupStack::PopAndDestroy( 2 ); // writeStream, item
	
	aResultCode = SendReceive( ENSmlItemDelete, TIpcArgs( aId, &aStoreName ) );
	}
	
// ------------------------------------------------------------------------------------------------
// RNSmlDSHostClient::SoftDeleteItemL
// Soft deletes one item at Data Store.
// ------------------------------------------------------------------------------------------------	
void RNSmlDSHostClient::SoftDeleteItemL( TSmlDbItemUid aUid, const TSmlDataProviderId aId, const TDesC& aStoreName, TInt& aResultCode ) const
	{
	CNSmlDSHostItem* item = CNSmlDSHostItem::NewLC();
	item->SetUid( aUid );
	
	RMemWriteStream writeStream( iChunk.Base(), iChunk.Size() );
	CleanupClosePushL( writeStream );
	item->ExternalizeL( writeStream );
	writeStream.CommitL();
	CleanupStack::PopAndDestroy( 2 ); // writeStream, item

	aResultCode = SendReceive( ENSmlItemSoftDelete, TIpcArgs( aId, &aStoreName ) );
	}
	
// ------------------------------------------------------------------------------------------------
// RNSmlDSHostClient::DeleteAllItems
// Deletes all items at Data Store permanently.
// ------------------------------------------------------------------------------------------------	
void RNSmlDSHostClient::DeleteAllItems( const TSmlDataProviderId aId, const TDesC& aStoreName, TInt& aResultCode ) const
	{
	aResultCode = SendReceive( ENSmlItemDeleteAll, TIpcArgs( aId, &aStoreName ) );
	}

// ------------------------------------------------------------------------------------------------
// RNSmlDSHostClient::HasSyncHistory
// Checks if the Data Store has sync history. If not then slow sync is proposed to Sync Partner.
// ------------------------------------------------------------------------------------------------	
TBool RNSmlDSHostClient::HasSyncHistory( const TSmlDataProviderId aId, const TDesC& aStoreName, TInt& aResultCode ) const
	{
	TPckgBuf<TInt> pckg;
	aResultCode = SendReceive( ENSmlDSSyncHistory, TIpcArgs( aId, &aStoreName, &pckg ) );
	return pckg();
	}
	
// ------------------------------------------------------------------------------------------------
// RNSmlDSHostClient::AddedItemsL
// The Data Provider returns UIDs of items that are added after previous synchronization.
// If the Data Provider uses hierarchical synchronization then added folders must be placed
// first (from root to leaves) to UID set and finally items.
// ------------------------------------------------------------------------------------------------	
void RNSmlDSHostClient::AddedItemsL( RNSmlDbItemModificationSet& aUidSet, const TSmlDataProviderId aId, const TDesC& aStoreName, TInt& aResultCode ) const
	{
	aResultCode = SendReceive( ENSmlDSItemsAdded, TIpcArgs( aId, &aStoreName ) );
	CleanupStack::PushL( TCleanupItem ( CancelAdjust, const_cast<RNSmlDSHostClient*>( this ) ) );	
	if ( aResultCode == KErrNone )
		{
		RMemReadStream readStream( iChunk.Base(), iChunk.Size() );
		CleanupClosePushL( readStream );
		aUidSet.InternalizeL( readStream );
		CleanupStack::PopAndDestroy(); // readStream
		}
	
	CleanupStack::PopAndDestroy(); // CancelAdjust
	}
	
// ------------------------------------------------------------------------------------------------
// RNSmlDSHostClient::DeletedItemsL
// The Data Provider returns UIDs of items that are deleted after previous synchronization.
// If the Data Provider uses hierarchical synchronization then deleted items must be placed
// first to UID set and folders after items (from leaves to root).
// ------------------------------------------------------------------------------------------------	
void RNSmlDSHostClient::DeletedItemsL( RNSmlDbItemModificationSet& aUidSet, const TSmlDataProviderId aId, const TDesC& aStoreName, TInt& aResultCode ) const
	{
	aResultCode = SendReceive( ENSmlDSItemsDeleted, TIpcArgs( aId, &aStoreName ) );
	CleanupStack::PushL( TCleanupItem ( CancelAdjust, const_cast<RNSmlDSHostClient*>( this ) ) );
	if ( aResultCode == KErrNone )
		{
		RMemReadStream readStream( iChunk.Base(), iChunk.Size() );
		CleanupClosePushL( readStream );
		aUidSet.InternalizeL( readStream );
		CleanupStack::PopAndDestroy(); // readStream
		}
		
	CleanupStack::PopAndDestroy(); // CancelAdjust
	}

// ------------------------------------------------------------------------------------------------
// RNSmlDSHostClient::SoftDeleteItemsL
// The Data Provider returns UIDs of items that are soft deleted after previous synchronization.
// If the Data Provider uses hierarchical synchronization then soft deleted items must be placed
// first to UID set and folders after items (from leaves to root).
// ------------------------------------------------------------------------------------------------	
void RNSmlDSHostClient::SoftDeleteItemsL( RNSmlDbItemModificationSet& aUidSet, const TSmlDataProviderId aId, const TDesC& aStoreName, TInt& aResultCode ) const
	{
	aResultCode = SendReceive( ENSmlDSItemsSofDeleted, TIpcArgs( aId, &aStoreName ) );
	CleanupStack::PushL( TCleanupItem ( CancelAdjust, const_cast<RNSmlDSHostClient*>( this ) ) );
	if ( aResultCode == KErrNone )
		{
		RMemReadStream readStream( iChunk.Base(), iChunk.Size() );
		CleanupClosePushL( readStream );
		aUidSet.InternalizeL( readStream );
		CleanupStack::PopAndDestroy(); // readStream
		}
		
	CleanupStack::PopAndDestroy(); // CancelAdjust
	}

// ------------------------------------------------------------------------------------------------
// RNSmlDSHostClient::ModifiedItemsL
// The Data Provider returns UIDs of items that are modified after previous synchronization.
// If the Data Provider uses hierarchical synchronization then modified folders must be placed
// first (from root to leaves) to UID set and finally items.
// ------------------------------------------------------------------------------------------------	
void RNSmlDSHostClient::ModifiedItemsL( RNSmlDbItemModificationSet& aUidSet, const TSmlDataProviderId aId, const TDesC& aStoreName, TInt& aResultCode ) const
	{
	aResultCode = SendReceive( ENSmlDSItemsModified, TIpcArgs( aId, &aStoreName ) );
	CleanupStack::PushL( TCleanupItem ( CancelAdjust, const_cast<RNSmlDSHostClient*>( this ) ) );
	if ( aResultCode == KErrNone )
		{
		RMemReadStream readStream( iChunk.Base(), iChunk.Size() );
		CleanupClosePushL( readStream );
		aUidSet.InternalizeL( readStream );
		CleanupStack::PopAndDestroy(); // readStream
		}
		
	CleanupStack::PopAndDestroy(); // CancelAdjust
	}

// ------------------------------------------------------------------------------------------------
// RNSmlDSHostClient::MovedItemsL
// The Data Provider returns UIDs of items that are moved after previous synchronization.
// If the Data Provider uses hierarchical synchronization then moved folders must be placed
// first (from root to leaves) to UID set and finally items.
// ------------------------------------------------------------------------------------------------	
void RNSmlDSHostClient::MovedItemsL( RNSmlDbItemModificationSet& aUidSet, const TSmlDataProviderId aId, const TDesC& aStoreName, TInt& aResultCode ) const
	{
	aResultCode = SendReceive( ENSmlDSItemsMoved, TIpcArgs( aId, &aStoreName ) );
	CleanupStack::PushL( TCleanupItem ( CancelAdjust, const_cast<RNSmlDSHostClient*>( this ) ) );
	if ( aResultCode == KErrNone )
		{
		RMemReadStream readStream( iChunk.Base(), iChunk.Size() );
		CleanupClosePushL( readStream );
		aUidSet.InternalizeL( readStream );
		CleanupStack::PopAndDestroy(); // readStream
		}
		
	CleanupStack::PopAndDestroy(); // CancelAdjust
	}

// ------------------------------------------------------------------------------------------------
// RNSmlDSHostClient::AllItemsL
// The Data Provider returns UIDs of items that are added, deleted, modified, softdeleted
// or moved after previous synchronization.
// ------------------------------------------------------------------------------------------------	
void RNSmlDSHostClient::AllItemsL( RNSmlDbItemModificationSet& aUidSet, const TSmlDataProviderId aId, const TDesC& aStoreName, TInt& aResultCode ) const
	{
	aResultCode = SendReceive( ENSmlDSItemsAll, TIpcArgs( aId, &aStoreName ) );
	CleanupStack::PushL( TCleanupItem ( CancelAdjust, const_cast<RNSmlDSHostClient*>( this ) ) );
	if ( aResultCode == KErrNone )
		{
		RMemReadStream readStream( iChunk.Base(), iChunk.Size() );
		CleanupClosePushL( readStream );
		aUidSet.InternalizeL( readStream );
		CleanupStack::PopAndDestroy(); // readStream
		}
		
	CleanupStack::PopAndDestroy(); // CancelAdjust
	}

// ------------------------------------------------------------------------------------------------
// RNSmlDSHostClient::ResetChangeInfo
// Reset change info from the Data Provider. The result of this method is that the Data Provider
// sends just ADD commands to Sync Partner.
// ------------------------------------------------------------------------------------------------	
void RNSmlDSHostClient::ResetChangeInfo( const TSmlDataProviderId aId, const TDesC& aStoreName, TInt& aResultCode ) const
	{
	aResultCode = SendReceive( ENSmlDSResetChangeInfo, TIpcArgs( aId, &aStoreName ) );
	}
		
// ------------------------------------------------------------------------------------------------
// RNSmlDSHostClient::CommitChangeInfoL
// This method is called after some changes are synchronized to Sync Partner. If some changes
// were synchronized correctly then those UIDs are included to aItems.
// ------------------------------------------------------------------------------------------------	
void RNSmlDSHostClient::CommitChangeInfoL( const MSmlDataItemUidSet& aItems, const TSmlDataProviderId aId, const TDesC& aStoreName, TInt& aResultCode )
	{
	TStreamBuffers* sb = StreamBufferLC();
	aItems.ExternalizeL( *sb->iWrite );
	sb->iWrite->CommitL();
	AdjustChunkIfNeededLC( sb->iBuffer->Size() );
	
	RMemWriteStream writeStream( iChunk.Base(), iChunk.Size() );
	CleanupClosePushL( writeStream );
	writeStream.WriteL( *sb->iRead );
	writeStream.CommitL();
	
	aResultCode = SendReceive( ENSmlDSCommitChanges, TIpcArgs( aId, &aStoreName ) );
	CleanupStack::PopAndDestroy(3); // writeStream, AdjustChunkIfNeededLC, sb
	}

// ------------------------------------------------------------------------------------------------
// RNSmlDSHostClient::CommitChangeInfo
// This method is called after some changes are synchronized to Sync Partner. This method is used if
// all changes were synchronized correctly.
// ------------------------------------------------------------------------------------------------	
void RNSmlDSHostClient::CommitChangeInfo( const TSmlDataProviderId aId, const TDesC& aStoreName, TInt& aResultCode ) const
	{
	aResultCode = SendReceive( ENSmlDSCommitAllChanges, TIpcArgs( aId, &aStoreName ) );
	}

// ------------------------------------------------------------------------------------------------
// RNSmlDSHostClient::UpdateServerIdL
// Replaces the give old server ID with new one.
// ------------------------------------------------------------------------------------------------	
void RNSmlDSHostClient::UpdateServerIdL( TDesC& aOldServerId, TDesC& aNewValue, TInt& aResultCode  )
	{
	AdjustChunkIfNeededLC( aOldServerId.Length() + aNewValue.Length() + 2*sizeof(TInt32) + 5 );
	RMemWriteStream writeStream( iChunk.Base(), iChunk.Size() );
	CleanupClosePushL( writeStream );
	
	writeStream.WriteUint32L( aOldServerId.Length() );
	writeStream << aOldServerId;
	writeStream.WriteUint32L( aNewValue.Length() );
	writeStream << aNewValue;
	
	aResultCode = SendReceive( ENSmlDSUpdateServerId, TIpcArgs() );
	CleanupStack::PopAndDestroy(2); // writeStream, AdjustChunkIfNeededLC
	}

// ------------------------------------------------------------------------------------------------
// RNSmlDSHostClient - private methods
// ------------------------------------------------------------------------------------------------
// ------------------------------------------------------------------------------------------------
// RNSmlDSHostClient::LaunchServer
// Starts Host Server.
// ------------------------------------------------------------------------------------------------
TInt RNSmlDSHostClient::LaunchServer( const TDesC& aServerName ) const
	{
	TInt result( KErrNone );
	
	TParse parser;
	if ( aServerName.Compare( KNSmlDSHostServer1Name ) == 0 )
		{
		parser.Set( KNSmlDSHostServer1Exe, &KDC_PROGRAMS_DIR, NULL );
		}
	else if ( aServerName.Compare( KNSmlDSHostServer2Name ) == 0 )
		{
		parser.Set( KNSmlDSHostServer2Exe, &KDC_PROGRAMS_DIR, NULL );
		}
	else
		{
		return KErrNotSupported;
		}

	// DLL launch
	RProcess server;
	result = server.Create( parser.FullName(), KNullDesC );

	// Loading failed.
	if ( result != KErrNone )
		{
		return result;
		}
	
	TRequestStatus status;
	server.Rendezvous( status );

	if ( status != KRequestPending )
		{
		server.Kill( 0 );	// abort startup
		server.Close();
		return KErrGeneral;	// status can be KErrNone: don't return status.Int()
		}
	else
		{
		server.Resume();	// logon OK - start the server
		}
		
	User::WaitForRequest( status );
	
	result = status.Int();
	
	if ( status != KErrNone )
		{
		server.Close();
		}

	return result;
	}

	
// ------------------------------------------------------------------------------------------------
// RNSmlDSHostClient::StreamBufferLC
// ------------------------------------------------------------------------------------------------   
RNSmlDSHostClient::TStreamBuffers* RNSmlDSHostClient::StreamBufferLC() const
	{
	TStreamBuffers* cleanup = new ( ELeave ) TStreamBuffers;
	CleanupStack::PushL( TCleanupItem ( CleanupStreamBuffer, cleanup ) );
	cleanup->iBuffer = CBufFlat::NewL( 64 );
	cleanup->iWrite = new ( ELeave ) RBufWriteStream( *cleanup->iBuffer );
	cleanup->iRead = new ( ELeave ) RBufReadStream( *cleanup->iBuffer );
	return cleanup;
	}
	
// ------------------------------------------------------------------------------------------------
// RNSmlDSHostClient::CleanupStreamBuffer
// ------------------------------------------------------------------------------------------------
void RNSmlDSHostClient::CleanupStreamBuffer( TAny* aP )
	{
	TStreamBuffers* cleanup = reinterpret_cast<TStreamBuffers*>( aP );
	
	if ( cleanup->iWrite )
		{
		cleanup->iWrite->Close();
		delete cleanup->iWrite;
		}
		
	if ( cleanup->iRead )
		{
		cleanup->iRead->Close();
		delete cleanup->iRead;
		}
	
	delete cleanup->iBuffer;
	delete cleanup;
	}		

// ------------------------------------------------------------------------------------------------
// RNSmlDSHostClient::AdjustChunkLC
// Ensures that chunk has at least required size or max size of reserved memory.
// ------------------------------------------------------------------------------------------------	
void RNSmlDSHostClient::AdjustChunkLC( TInt aRequiredSize ) const
	{
	const RNSmlDSHostClient* ptr = NULL;
	
	if ( iChunk.Size() < aRequiredSize )
		{
		TInt maxSize( iChunk.MaxSize() );
	
		if ( aRequiredSize > maxSize )
			{
			aRequiredSize = maxSize;
			}
			
		User::LeaveIfError( iChunk.Adjust( aRequiredSize ) );
		ptr = this;
		}
		
	CleanupStack::PushL( TCleanupItem ( CancelAdjust, const_cast<RNSmlDSHostClient*>( ptr ) ) );
	}

// ------------------------------------------------------------------------------------------------
// RNSmlDSHostClient::AdjustChunkIfNeededLC
// Adjusts memory so that at least needed size is reserved.
// ------------------------------------------------------------------------------------------------    
void RNSmlDSHostClient::AdjustChunkIfNeededLC( TInt iNeededSize ) const
	{
	const RNSmlDSHostClient* ptr = NULL;
	
	if ( iChunk.Size() < iNeededSize )
		{
		User::LeaveIfError( iChunk.Adjust( iNeededSize ) );
		ptr = this;
		}
		
	CleanupStack::PushL( TCleanupItem ( CancelAdjust, const_cast<RNSmlDSHostClient*>( ptr ) ) );
	}
	
// ------------------------------------------------------------------------------------------------
// RNSmlDSHostClient::CancelAdjust
// restores chunk memory.
// ------------------------------------------------------------------------------------------------	
void RNSmlDSHostClient::CancelAdjust( TAny* aP )
	{
	if ( aP )
		{
		RNSmlDSHostClient* self = reinterpret_cast<RNSmlDSHostClient*>( aP );
		self->iChunk.Adjust( KNSmlDSHostChunkMinSize );
		}
	}		

// ------------------------------------------------------------------------------------------------
// RNSmlDSHostClient::InternalizeFiltersL
// Reads filters from stream.
// ------------------------------------------------------------------------------------------------
void RNSmlDSHostClient::InternalizeFiltersL( RReadStream& aStream, RPointerArray<CSyncMLFilter>& aFilters ) const
	{
	aFilters.ResetAndDestroy();
	
	TInt count( aStream.ReadInt32L() );
	
	for ( TInt i = 0; i < count; i++ )
		{
		CSyncMLFilter* filter = CSyncMLFilter::NewLC( aStream );
		aFilters.AppendL( filter );
		CleanupStack::Pop(); // filter
		}
	}
	
// ------------------------------------------------------------------------------------------------
// RNSmlDSHostClient::ExternalizeFiltersL
// Writes filters to stream.
// ------------------------------------------------------------------------------------------------	
void RNSmlDSHostClient::ExternalizeFiltersL( RWriteStream& aStream, const RPointerArray<CSyncMLFilter>& aFilters ) const
	{
	TInt count( aFilters.Count() );
	aStream.WriteInt32L( count );
	for ( TInt i(0); i < count; i++ )
		{
		aFilters[i]->ExternalizeL( aStream );
		}
	}		

// End of File
