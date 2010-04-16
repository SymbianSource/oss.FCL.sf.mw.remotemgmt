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
* Description:  Session for DS host server.
*
*/


// ------------------------------------------------------------------------------------------------
// Includes
// ------------------------------------------------------------------------------------------------

#include <s32mem.h>
#include <implementationinformation.h>
#include <ecom.h>
#include <badesca.h>
#include <nsmldebug.h>
#include <SmlDataFormat.h>

#include "NSmlAdapterLog.h"
#include "nsmldshostconstants.h"
#include "Nsmldshostserver.h"
#include "Nsmldshostsession.h"
#include "nsmldsdpinformation.h"
#include "Nsmldsdataproviderarray.h"

#ifdef __HOST_SERVER_MTEST__
#include "../../stif/DSHostServerTest/src/fakedataprovider.cpp"
#else
#include <nsmlchangefinder.h>	//for CNSmlDataItemUidSet
#endif

// ------------------------------------------------------------------------------------------------
// CNSmlDSHostSession::TMemPtr::WriteStreamLC
// ------------------------------------------------------------------------------------------------
RWriteStream& CNSmlDSHostSession::TMemPtr::WriteStreamLC() const
    {
    RMemWriteStream* writeStream = new ( ELeave ) RMemWriteStream( Ptr(), Size() );
    CleanupStack::PushL( TCleanupItem ( CleanupWriteStream, writeStream ) );
    return *writeStream;
    }

// ------------------------------------------------------------------------------------------------
// CNSmlDSHostSession::TMemPtr::StreamBufferLC
// ------------------------------------------------------------------------------------------------   
CNSmlDSHostSession::TMemPtr::TStreamBuffers* 
	CNSmlDSHostSession::TMemPtr::StreamBufferLC() const
	{
	TStreamBuffers* cleanup = new ( ELeave ) TStreamBuffers;
	CleanupStack::PushL( TCleanupItem ( CleanupStreamBuffer, cleanup ) );
	cleanup->iBuffer = CBufFlat::NewL( 64 );
	cleanup->iWrite = new ( ELeave ) RBufWriteStream( *cleanup->iBuffer );
	cleanup->iRead = new ( ELeave ) RBufReadStream( *cleanup->iBuffer );
	return cleanup;
	}

// ------------------------------------------------------------------------------------------------
// CNSmlDSHostSession::TMemPtr::ReadStreamLC
// ------------------------------------------------------------------------------------------------
RReadStream& CNSmlDSHostSession::TMemPtr::ReadStreamLC() const
    {
    RMemReadStream* readStream = new ( ELeave ) RMemReadStream( Ptr(), iChunk.Size() );
    CleanupStack::PushL( TCleanupItem ( CleanupReadStream, readStream ) );
    return *readStream;
    }
    
// ------------------------------------------------------------------------------------------------
// CNSmlDSHostSession::TMemPtr::AdjustChunkIfNeededL
// ------------------------------------------------------------------------------------------------    
void CNSmlDSHostSession::TMemPtr::AdjustChunkIfNeededL( TInt iNeededSize )
	{
	if ( Size() < iNeededSize )
		{
		User::LeaveIfError( Adjust(iNeededSize) );
		}
	}

// ------------------------------------------------------------------------------------------------
// CNSmlDSHostSession::TMemPtr::CleanupWriteStream
// ------------------------------------------------------------------------------------------------
void CNSmlDSHostSession::TMemPtr::CleanupWriteStream( TAny* aP )
	{
	RMemWriteStream* writeStream = reinterpret_cast<RMemWriteStream*>( aP );
	writeStream->Close();
	delete writeStream;
	}

// ------------------------------------------------------------------------------------------------
// CNSmlDSHostSession::TMemPtr::CleanupReadStream
// ------------------------------------------------------------------------------------------------
void CNSmlDSHostSession::TMemPtr::CleanupReadStream( TAny* aP )
	{
	RMemReadStream* readStream = reinterpret_cast<RMemReadStream*>( aP );
	readStream->Close();
	delete readStream;
	}

// ------------------------------------------------------------------------------------------------
// CNSmlDSHostSession::TMemPtr::CleanupStreamBuffer
// ------------------------------------------------------------------------------------------------
void CNSmlDSHostSession::TMemPtr::CleanupStreamBuffer( TAny* aP )
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
// CNSmlDSHostSession::TMemPtr::CancelAdjust
// ------------------------------------------------------------------------------------------------	
void CNSmlDSHostSession::TMemPtr::CancelAdjust( TAny* aP )
	{
	if ( aP )
		{
		TMemPtr* memptr = reinterpret_cast<TMemPtr*>( aP );
		memptr->Adjust( KNSmlDSHostChunkMinSize );
		}
	}
	
// ------------------------------------------------------------------------------------------------
// CNSmlDSHostSession::CNSmlServerDSHostItem::NewLC
// ------------------------------------------------------------------------------------------------
CNSmlDSHostSession::CNSmlServerDSHostItem* CNSmlDSHostSession::CNSmlServerDSHostItem::NewLC()
	{
	CNSmlServerDSHostItem* self = new ( ELeave ) CNSmlServerDSHostItem();
	CleanupStack::PushL( self );
	self->ConstructL();
	return self;
	}
	
// ------------------------------------------------------------------------------------------------
// CNSmlDSHostSession::NewL
// ------------------------------------------------------------------------------------------------
CNSmlDSHostSession* CNSmlDSHostSession::NewL( CNSmlDSHostServer& aServer )
	{
	_DBG_FILE( "CNSmlDSHostSession::NewL(): begin" );
	CNSmlDSHostSession* self= new ( ELeave ) CNSmlDSHostSession( aServer );
	CleanupStack::PushL( self );
	self->ConstructL();
	CleanupStack::Pop(); // self
	_DBG_FILE( "CNSmlDSHostSession::NewL(): end" );
	return self;
	}
	
// ------------------------------------------------------------------------------------------------
// CNSmlDSHostSession::~CNSmlDSHostSession
// ------------------------------------------------------------------------------------------------
CNSmlDSHostSession::~CNSmlDSHostSession()
	{
	_DBG_FILE( "CNSmlDSHostSession::~CNSmlDSHostSession(): begin" );
    iMemPtr.iChunk.Close();
    iDataProviders.ResetAndDestroy();
    iStringPool.Close();
    REComSession::FinalClose();
    iServer.DecSessionCount();
	_DBG_FILE( "CNSmlDSHostSession::~CNSmlDSHostSession(): end" );
	}

// ------------------------------------------------------------------------------------------------
// CNSmlDSHostSession::ServiceL
// ------------------------------------------------------------------------------------------------
void CNSmlDSHostSession::ServiceL( const RMessage2& aMessage )
	{
	_DBG_FILE( "CNSmlDSHostSession::ServiceL(): begin" );
    TBool completeRequest( ETrue );
	TRAPD( err, DispatchMessageL( aMessage, completeRequest ) );

    if ( completeRequest && !aMessage.IsNull() )
        {
        aMessage.Complete( err );
        }
	DBG_FILE_CODE( err, _S8( "CNSmlDSHostSession::ServiceL() returned" ) );
	}

// ------------------------------------------------------------------------------------------------
// CNSmlDSHostSession::DispatchMessageL
// ------------------------------------------------------------------------------------------------
void CNSmlDSHostSession::DispatchMessageL( const RMessage2& aMessage, TBool& aCompleteRequest )
	{
	_DBG_FILE( "CNSmlDSHostSession::DispatchMessageL(): begin" );
    aCompleteRequest = ETrue;

	switch( aMessage.Function() )
        {
	    case ENSmlDPOpen:
	    	_DBG_FILE("CreateDataProvidersL"); 
	        CreateDataProvidersL( aMessage );
	        break;
	    case ENSmlDPOpenExcept:
	        _DBG_FILE("CreateDataProvidersExceptL"); 
	        CreateDataProvidersExceptL( aMessage );
	        break;
	    case ENSmlDPInformation:
	    	_DBG_FILE("DPInformation"); 
	    	DPInformationL( aMessage );
	    	break;
		case ENSmlHandleChunk:
			_DBG_FILE("HandleChunk"); 
			HandleChunk( aMessage );
	        break;
		case ENSmlDPSupportsOperation:
			_DBG_FILE("SupportsOperationL"); 
	        SupportsOperationL( aMessage );
	        break;
		case ENSmlDPStoreFormat:
			_DBG_FILE("StoreFormatL"); 
	        StoreFormatL( aMessage );
	        break;
		case ENSmlDPlListStores:
	        _DBG_FILE("ListStoresL"); 
	        ListStoresL( aMessage );
	        break;
		case ENSmlDPDefaultStore:
	        _DBG_FILE("DefaultStoreL"); 
	        DefaultStoreL( aMessage );
	        break;
	    case ENSmlServerFilters:
	    	_DBG_FILE("SupportedServerFiltersL"); 
	        SupportedServerFiltersL( aMessage );
	        break;
        case ENSmlUpdateServerFilters:
        	_DBG_FILE("CheckServerFiltersL");
        	CheckServerFiltersL( aMessage );
        	break;
        case ENSmlCheckSupportedServerFilters:
        	_DBG_FILE("CheckSupportedServerFiltersL");
        	CheckSupportedServerFiltersL( aMessage );
        	break;
        case ENSmlFilters:
        	_DBG_FILE("GetFiltersL");
        	GetFiltersL( aMessage );
        	break;
		case ENSmlDSOpen:
	        _DBG_FILE("OpenL"); 
	        OpenL( aMessage ); aCompleteRequest = EFalse;
	        break;
		case ENSmlDSCancelRequest:
	        _DBG_FILE("CancelRequestL"); 
	        CancelRequestL( aMessage );
	        break;
		case ENSmlDSBeginTransaction:
	        _DBG_FILE("BeginTransactionL"); 
	        BeginTransactionL( aMessage );
	        break;
		case ENSmlDSCommitTransaction:
	        _DBG_FILE("CommitTransactionL"); 
	        CommitTransactionL( aMessage ); aCompleteRequest = EFalse;
	        break;
		case ENSmlDSRevertTransaction:
	        _DBG_FILE("RevertTransactionL"); 
	        RevertTransactionL( aMessage ); aCompleteRequest = EFalse;
	        break;
		case ENSmlDSBeginBatch:
	        _DBG_FILE("BeginBatchL"); 
	        BeginBatchL( aMessage );
	        break;
		case ENSmlDSCommitBatch:
	        _DBG_FILE("CommitBatchL"); 
	        CommitBatchL( aMessage ); aCompleteRequest = EFalse;
	        break;
		case ENSmlDSCancelBatch:
	        _DBG_FILE("CancelBatchL"); 
	        CancelBatchL( aMessage );
	        break;
		case ENSmlDSSetDataStoreFormat:
	        _DBG_FILE("SetRemoteDataStoreFormatL"); 
	        SetRemoteDataStoreFormatL( aMessage );
	        break;
		case ENSmlDSRemoteMaxObjSize:
	        _DBG_FILE("SetRemoteMaxObjectSizeL"); 
	        SetRemoteMaxObjectSizeL( aMessage );
	        break;
		case ENSmlMaxObjSize:
	        _DBG_FILE("MaxObjectSizeL"); 
	        MaxObjectSizeL( aMessage );
	        break;
		case ENSmlItemOpen:
	        _DBG_FILE("OpenItemL"); 
	        OpenItemL( aMessage ); aCompleteRequest = EFalse;
	        break;
		case ENSmlItemCreate:
	        _DBG_FILE("CreateItemL"); 
	        CreateItemL( aMessage ); aCompleteRequest = EFalse;
	        break;
		case ENSmlItemReplace:
	        _DBG_FILE("ReplaceItemL"); 
	        ReplaceItemL( aMessage ); aCompleteRequest = EFalse;
	        break;
		case ENSmlItemRead:
	        _DBG_FILE("ReadItemL"); 
	        ReadItemL( aMessage );
	        break;
		case ENSmlItemWrite:
	        _DBG_FILE("WriteItemL"); 
	        WriteItemL( aMessage );
	        break;
		case ENSmlItemCommit:
	        _DBG_FILE("CommitItemL"); 
	        CommitItemL( aMessage ); aCompleteRequest = EFalse;
	        break;
		case ENSmlItemClose:
	        _DBG_FILE("CloseItemL"); 
	        CloseItemL( aMessage );
	        break;
		case ENSmlItemMove:
	        _DBG_FILE("MoveItemL"); 
	        MoveItemL( aMessage ); aCompleteRequest = EFalse;
	        break;
		case ENSmlItemDelete:
	        _DBG_FILE("DeleteItemL"); 
	        DeleteItemL( aMessage ); aCompleteRequest = EFalse;
	        break;
		case ENSmlItemSoftDelete:
	        _DBG_FILE("SoftDeleteItemL"); 
	        SoftDeleteItemL( aMessage ); aCompleteRequest = EFalse;
	        break;
		case ENSmlItemDeleteAll:
	        _DBG_FILE("DeleteAllItemsL"); 
	        DeleteAllItemsL( aMessage ); aCompleteRequest = EFalse;
	        break;
		case ENSmlDSSyncHistory:
	        _DBG_FILE("HasSyncHistoryL"); 
	        HasSyncHistoryL( aMessage );
	        break;
		case ENSmlDSItemsAdded:
	        _DBG_FILE("AddedItemsL"); 
	        AddedItemsL( aMessage );
	        break;
		case ENSmlDSItemsDeleted:
	        _DBG_FILE("DeletedItemsL"); 
	        DeletedItemsL( aMessage );
	        break;
		case ENSmlDSItemsSofDeleted:
	        _DBG_FILE("SoftDeleteItemsL"); 
	        SoftDeleteItemsL( aMessage );
	        break;
		case ENSmlDSItemsModified:
	        _DBG_FILE("ModifiedItemsL"); 
	        ModifiedItemsL( aMessage );
	        break;
		case ENSmlDSItemsMoved:
	        _DBG_FILE("MovedItemsL"); 
	        MovedItemsL( aMessage );
	        break;
	    case ENSmlDSItemsAll:
	    	_DBG_FILE("AllItems"); 
	    	AllItemsL( aMessage ); aCompleteRequest = EFalse;
	    	break;
		case ENSmlDSResetChangeInfo:
	        _DBG_FILE("ResetChangeInfoL"); 
	        ResetChangeInfoL( aMessage ); aCompleteRequest = EFalse;
	        break;
		case ENSmlDSCommitChanges:
	        _DBG_FILE("CommitChangesL"); 
	        CommitChangesL( aMessage ); aCompleteRequest = EFalse;
	        break;
		case ENSmlDSCommitAllChanges:
	        _DBG_FILE("CommitAllChangesL"); 
	        CommitAllChangesL( aMessage ); aCompleteRequest = EFalse;
	        break;
        case ENSmlDSUpdateServerId:
        	_DBG_FILE("Update server id"); 
        	UpdateServerIdL( aMessage );
        	break;
		default:
			_DBG_FILE("Unknown function"); 
			PanicClient( aMessage, KErrNotSupported );
        }
	_DBG_FILE( "CNSmlDSHostSession::DispatchMessageL(): end" );
	}
	
// ------------------------------------------------------------------------------------------------
// CNSmlDSHostSession::CNSmlDSHostSession
// ------------------------------------------------------------------------------------------------
CNSmlDSHostSession::CNSmlDSHostSession( CNSmlDSHostServer& aServer ) : 
    iServer( aServer )
	{
	_DBG_FILE( "CNSmlDSHostSession::CNSmlDSHostSession(): begin" );
	iServer.IncSessionCount();
	_DBG_FILE( "CNSmlDSHostSession::CNSmlDSHostSession(): end" );
	}

// ------------------------------------------------------------------------------------------------
// CNSmlDSHostSession::ConstructL
// ------------------------------------------------------------------------------------------------
void CNSmlDSHostSession::ConstructL()
	{
	_DBG_FILE( "CNSmlDSHostSession::ConstructL(): begin" );
    iStringPool.OpenL();
	_DBG_FILE( "CNSmlDSHostSession::ConstructL(): end" );
	}
	

// ------------------------------------------------------------------------------------------------
// CNSmlDSHostSession::HandleChunk
// 
// ------------------------------------------------------------------------------------------------
void CNSmlDSHostSession::HandleChunk( const RMessage2& aMessage )
	{
	iMemPtr.iChunk.Close();
	const TInt error = iMemPtr.iChunk.Open(aMessage, 0, EFalse);
	}

// ------------------------------------------------------------------------------------------------
// CNSmlDSHostSession::CreateDataProvidersL
// Creates Data Providers.
// ------------------------------------------------------------------------------------------------
void CNSmlDSHostSession::CreateDataProvidersL( const RMessage2& /*aMessage*/ )
	{
	
	RArray<TInt> results;
    CleanupClosePushL( results );
    
    TMemPtr& ptr( MemPtrL() );
    RReadStream& readStream = ptr.ReadStreamLC();
    const TUint uidCount ( readStream.ReadInt32L() ) ;

    for( TUint i = 0; i < uidCount; i++ )
        {
        TSmlDataProviderId id;
        TPckg<TSmlDataProviderId> dpid( id );
        readStream.ReadL( dpid );
        TRAPD( err, CreateDataProviderL( dpid() ) );
        results.AppendL( err );
        }
    CleanupStack::PopAndDestroy(); //readStream

    //to chunk
    ptr.AdjustChunkIfNeededL( sizeof( TInt32 ) * ( results.Count() + 1 ) );
    RWriteStream& writeStream = ptr.WriteStreamLC();
    writeStream.WriteInt32L( results.Count() );

    for( TInt j = 0; j < results.Count(); j++ )
        {
        writeStream.WriteInt32L( results[j] );
        }
    writeStream.CommitL();
    CleanupStack::PopAndDestroy(2); //writeStream, results
	}

// ------------------------------------------------------------------------------------------------
// CNSmlDSHostSession::CreateDataProvidersExceptL
// ------------------------------------------------------------------------------------------------
void CNSmlDSHostSession::CreateDataProvidersExceptL( const RMessage2& /*aMessage*/ )
    {
    iDataProviders.ResetAndDestroy();
    
    RArray<TSmlDataProviderId> results;
    CleanupClosePushL( results );

    TMemPtr& ptr( MemPtrL() );
    RReadStream& readStream = ptr.ReadStreamLC();
    
    //copying except uids to array.
    const TInt exceptIdCount ( readStream.ReadInt32L() );
    	
    RArray<TSmlDataProviderId> exceptIds( Max<TInt>( 2, exceptIdCount ) );
    CleanupClosePushL( exceptIds );
    
    for ( TInt i = 0; i < exceptIdCount; i++ )
    	{
    	TSmlDataProviderId id;
    	TPckg<TSmlDataProviderId> dpid( id );
        readStream.ReadL( dpid );
        exceptIds.AppendL( dpid() );
    	}

	TUid DsUid = { KNSmlDSInterfaceUid };
	RImplInfoPtrArray implArray;
    CleanupStack::PushL( PtrArrCleanupItemRArr( 
			CImplementationInformation, &implArray ) );
	REComSession::ListImplementationsL( DsUid, implArray );
	
    for( TInt i = 0; i < implArray.Count(); i++ )
        {
        CImplementationInformation* implInfo = implArray[i];
        TSmlDataProviderId uid = implInfo->ImplementationUid().iUid;

        if ( exceptIds.Find( uid ) == KErrNotFound )
            {
            TRAPD( err, CreateDataProviderL( uid ) );
            if ( err == KErrNone )
                {
                results.AppendL( uid );
                }
            }
        }
        
    CleanupStack::PopAndDestroy(3); //implArray, exceptIds, readStream

    //to chunk
    TStreamBuffers* sb = ptr.StreamBufferLC();
    sb->iWrite->WriteInt32L( results.Count() );

    for( TInt j = 0; j < results.Count(); j++ )
        {
        TPckgC<TSmlDataProviderId> dpid( results[j] );
        sb->iWrite->WriteL( dpid );
        }
    StreamBufferToChunkL( ptr, sb );
	
    CleanupStack::PopAndDestroy(2); //sb, results
    }
    
// ------------------------------------------------------------------------------------------------
// CNSmlDSHostSession::CreateDataProviderL
// ------------------------------------------------------------------------------------------------
void CNSmlDSHostSession::CreateDataProviderL( TSmlDataProviderId aId )
    {
    CSmlDataProvider* dp = CSmlDataProvider::NewL( aId );
    CleanupStack::PushL( dp );
    TInt ret = iDataProviders.InsertL( dp );
    if ( ret == KErrNone )
        {
        CleanupStack::Pop( dp );    
        }
    else
        {
        CleanupStack::PopAndDestroy( dp );
        }
    }

// ------------------------------------------------------------------------------------------------
// CNSmlDSHostSession::DPInformationL
// ------------------------------------------------------------------------------------------------
void CNSmlDSHostSession::DPInformationL( const RMessage2& aMessage )
	{
	TSmlDataProviderId id( aMessage.Int0() );
	CSmlDataProvider* dp = DataProviderL( id );
	CNSmlDPInformation* dpi = CNSmlDPInformation::NewLC();
	TMemPtr& ptr( MemPtrL() );
	
	const CSmlDataStoreFormat& storeFormat = dp->StoreFormatL();
	dpi->SetId( id );
	
	CDesC8Array* mimetypes = new ( ELeave ) CDesC8ArrayFlat( storeFormat.MimeFormatCount() );
	CleanupStack::PushL( mimetypes );
	CDesC8Array* mimevers = new ( ELeave ) CDesC8ArrayFlat( storeFormat.MimeFormatCount() );
	CleanupStack::PushL( mimevers );
	
	for ( TInt i = 0; i < storeFormat.MimeFormatCount(); i++ )
		{
		const CSmlMimeFormat& mf = storeFormat.MimeFormat(i);
		mimetypes->AppendL( mf.MimeType().DesC() );
		mimevers->AppendL( mf.MimeVersion().DesC() );
		}
	
	dpi->SetMimeTypesL( mimetypes );
	dpi->SetMimeVersL( mimevers );
	
	CleanupStack::Pop( 2 ); //mimetypes, mimevers
	
	if ( storeFormat.IsSupported( CSmlDataStoreFormat::EOptionHierarchial ) )
		{
		dpi->SetProtocolVersion( ESmlVersion1_2 );
		}
	else
		{
		dpi->SetProtocolVersion( ESmlVersion1_1_2 );
		}
	
	TUid DsUid = { KNSmlDSInterfaceUid };
	RImplInfoPtrArray implArray;
    CleanupStack::PushL( PtrArrCleanupItemRArr( 
			CImplementationInformation, &implArray ) );
	REComSession::ListImplementationsL( DsUid, implArray );
	
    for( TInt i = 0; i < implArray.Count(); i++ )
        {
        CImplementationInformation* implInfo = implArray[i];
        if ( id == implInfo->ImplementationUid().iUid )
        	{
        	TVersion version( implInfo->Version(), 0, 0 );
        	dpi->SetVersion( version );
        	dpi->SetDisplayNameL( implInfo->DisplayName() );
        	}
        }
        
    CleanupStack::PopAndDestroy(); //implArray
    TStreamBuffers* sb = ptr.StreamBufferLC();
    dpi->ExternalizeL( *sb->iWrite );
    StreamBufferToChunkL( ptr, sb );
	CleanupStack::PopAndDestroy( 2 ); //sb, dpi
	}
	
// ------------------------------------------------------------------------------------------------
// CNSmlDSHostSession::SupportsOperationL
// Asks if Data Provider supports some operation.
// ------------------------------------------------------------------------------------------------
void CNSmlDSHostSession::SupportsOperationL( const RMessage2& aMessage )
	{
    CSmlDataProvider* dp = DataProviderL( aMessage.Int0() );
    TMemPtr& ptr( MemPtrL() );
    TInt32 tmp(0);
    TPckg<TInt32> uidpck( tmp );
    ptr.CopyTo( uidpck );
    TUid uid( TUid::Uid( uidpck() ) );
    TBool retVal( dp->SupportsOperation( uid ) );
    TPckgC<TBool> respck( retVal );
    aMessage.WriteL( 2, respck );
	}

// ------------------------------------------------------------------------------------------------
// CNSmlDSHostSession::StoreFormatL
// Creates Data Store format of Data Provider.
// ------------------------------------------------------------------------------------------------
void CNSmlDSHostSession::StoreFormatL( const RMessage2& aMessage )
	{
    CSmlDataProvider* dp = DataProviderL( aMessage.Int0() );
    TMemPtr& destPtr( MemPtrL() );
    
    TStreamBuffers* sb = destPtr.StreamBufferLC();
    const CSmlDataStoreFormat& dsf = dp->StoreFormatL();
    dsf.ExternalizeL( *sb->iWrite );
    StreamBufferToChunkL( destPtr, sb );
    
    CleanupStack::PopAndDestroy(); //sb
	}
	
// ------------------------------------------------------------------------------------------------
// CNSmlDSHostSession::ListStoresL
// Creates list of Data Store names of Data Provider.
// ------------------------------------------------------------------------------------------------	
void CNSmlDSHostSession::ListStoresL( const RMessage2& aMessage )
	{
    CSmlDataProvider* dp = DataProviderL( aMessage.Int0() );
    CDesCArray* stores = dp->ListStoresLC();

    TMemPtr& destPtr( MemPtrL() );
    TInt size( sizeof( TInt32 ) );
    
    if ( stores )
    	{
	    for ( TInt i = 0; i < stores->Count(); i++ )
	        {
	        size += ( *stores )[i].Size() + sizeof( TInt32 );
	        }
    	}
    	
    destPtr.AdjustChunkIfNeededL( size );
	RWriteStream& wStream = destPtr.WriteStreamLC();
	
	if ( stores )
		{
		wStream.WriteInt32L( stores->Count() );
	    for ( TInt i = 0; i < stores->Count(); i++ )
	        {
	        wStream.WriteInt32L( ( *stores )[i].Length() );
	        wStream << ( *stores )[i];
	        }
    	}
    else
    	{
    	wStream.WriteInt32L( 0 );
    	}

	wStream.CommitL();
    CleanupStack::PopAndDestroy(2); //wStream, stores
	}

// ------------------------------------------------------------------------------------------------
// CNSmlDSHostSession::DefaultStoreL
// Creates default name for Data Store of Data Provider.
// ------------------------------------------------------------------------------------------------	
void CNSmlDSHostSession::DefaultStoreL( const RMessage2& aMessage )
	{
    CSmlDataProvider* dp = DataProviderL( aMessage.Int0() );
    const TDesC& defStore = dp->DefaultStoreL();
    TMemPtr& dest( MemPtrL() );
    dest.AdjustChunkIfNeededL( defStore.Size() + sizeof( TInt32 ) + 2 );
    RWriteStream& wStream = dest.WriteStreamLC();
    wStream.WriteInt32L( defStore.Length() );
    wStream << defStore;
    wStream.CommitL();
    CleanupStack::PopAndDestroy(); //wStream
	}

// ------------------------------------------------------------------------------------------------
// CNSmlDSHostSession::SupportedServerFiltersL
// This method returns the set of filters that can be used to send to the Sync Partner.
// ------------------------------------------------------------------------------------------------	
void CNSmlDSHostSession::SupportedServerFiltersL( const RMessage2& aMessage )
	{
    CSmlDataProvider* dp = DataProviderL( aMessage.Int0() );
    
    TBool suppOp( dp->SupportsOperation( KUidSmlSupportsUserSelectableMatchType ) );
    TSyncMLFilterMatchType matchType( ESyncMLMatchDisabled );
    if ( suppOp )
    	{
    	matchType = ESyncMLMatchNotSelected;
    	}
    	
    TPckgC<TSyncMLFilterMatchType> matchTypePckg( matchType );
    aMessage.WriteL( 2, matchTypePckg );
    
    const RPointerArray<CSyncMLFilter>& filters = dp->SupportedServerFiltersL();
    
    TMemPtr& dest( MemPtrL() );
    TStreamBuffers* sb = dest.StreamBufferLC();
    ExternalizeFiltersL( *sb->iWrite, filters );
    StreamBufferToChunkL( dest, sb );
    
    CleanupStack::PopAndDestroy(); //sb
	}

// ------------------------------------------------------------------------------------------------
// CNSmlDSHostSession::CheckServerFiltersL
// This method updates dynamic filters up-to-date
// ------------------------------------------------------------------------------------------------		
void CNSmlDSHostSession::CheckServerFiltersL( const RMessage2& aMessage )
	{
	CSmlDataProvider* dp = DataProviderL( aMessage.Int0() );
	
	RPointerArray<CSyncMLFilter> *filters = new ( ELeave ) RPointerArray<CSyncMLFilter>();
	CleanupRPtrArrayPushL( filters );
	
	TMemPtr& memptr( MemPtrL() );
    RReadStream& rStream = memptr.ReadStreamLC();
    TSyncMLFilterChangeInfo changeInfo( static_cast<TSyncMLFilterChangeInfo>( rStream.ReadInt32L() ) );
    
    InternalizeFiltersL( rStream, *filters );
    CleanupStack::PopAndDestroy(); //rStream
    
    dp->CheckServerFiltersL( *filters, changeInfo );
    
    TStreamBuffers* sb = memptr.StreamBufferLC();
    sb->iWrite->WriteInt32L( changeInfo );
    ExternalizeFiltersL( *sb->iWrite, *filters );
    StreamBufferToChunkL( memptr, sb );

    CleanupStack::PopAndDestroy(2); //sb, filters
	}
	
// ------------------------------------------------------------------------------------------------
// CNSmlDSHostSession::CheckSupportedServerFiltersL
// Calls CSmlDataProvider::CheckSupportedServerFiltersL.
// ------------------------------------------------------------------------------------------------
void CNSmlDSHostSession::CheckSupportedServerFiltersL( const RMessage2& aMessage )
	{
	CSmlDataProvider* dp = DataProviderL( aMessage.Int0() );
	
	RPointerArray<CSyncMLFilter> *filters = new ( ELeave ) RPointerArray<CSyncMLFilter>();
	CleanupRPtrArrayPushL( filters );
	
	TMemPtr& memptr( MemPtrL() );
    RReadStream& rStream = memptr.ReadStreamLC();
    TSyncMLFilterChangeInfo changeInfo( static_cast<TSyncMLFilterChangeInfo>( rStream.ReadInt32L() ) );
    CSmlDataStoreFormat* dataStoreFormat = CSmlDataStoreFormat::NewLC( iStringPool, rStream );
    InternalizeFiltersL( rStream, *filters );
    
    dp->CheckSupportedServerFiltersL(*dataStoreFormat, *filters, changeInfo );    
    CleanupStack::PopAndDestroy(2); //rStream, dataStoreFormat
    
    TStreamBuffers* sb = memptr.StreamBufferLC();
    sb->iWrite->WriteInt32L( changeInfo );
    ExternalizeFiltersL( *sb->iWrite, *filters );
    StreamBufferToChunkL( memptr, sb );

    CleanupStack::PopAndDestroy(2); //sb, filters
	}
	
// ------------------------------------------------------------------------------------------------
// CNSmlDSHostSession::GetFiltersL
// This method updates dynamic filters up-to-date
// ------------------------------------------------------------------------------------------------		
void CNSmlDSHostSession::GetFiltersL( const RMessage2& aMessage )
	{
	CSmlDataProvider* dp = DataProviderL( aMessage.Int0() );
	
	HBufC* storename = DataStoreNameLC( aMessage );
	
	RPointerArray<CSyncMLFilter>* filters = new ( ELeave ) RPointerArray<CSyncMLFilter>();
	CleanupRPtrArrayPushL( filters );
	
	TMemPtr& memptr( MemPtrL() );
    RReadStream& rStream ( memptr.ReadStreamLC() );
    TSyncMLFilterMatchType matchType ( static_cast<TSyncMLFilterMatchType>( rStream.ReadInt32L() ) );
    InternalizeFiltersL( rStream, *filters );
    
    TBuf<64> recordMimeType;
    
    TSyncMLFilterType filterType( ESyncMLTypeInclusive );
    HBufC* query = dp->GenerateRecordFilterQueryLC( *filters, matchType, recordMimeType, filterType, *storename );
    
    RPointerArray<CSmlDataProperty> properties;
    CleanupStack::PushL( PtrArrCleanupItemRArr( 
			CSmlDataProperty, &properties ) );
			
	TBuf<64> fieldMimeType;
    
    dp->GenerateFieldFilterQueryL( *filters, fieldMimeType, properties, *storename );
    
    TStreamBuffers* sb = memptr.StreamBufferLC();
    sb->iWrite->WriteInt32L( filterType );
    if ( query )
    	{
    	sb->iWrite->WriteInt32L( query->Length() );
    	(*sb->iWrite) << *query;
    	}
    else
    	{
    	sb->iWrite->WriteInt32L( 0 );
    	(*sb->iWrite) << KNullDesC;
    	}
    
    sb->iWrite->WriteInt32L( recordMimeType.Length() );
    (*sb->iWrite) << recordMimeType;
    sb->iWrite->WriteInt32L( fieldMimeType.Length() );
    (*sb->iWrite) << fieldMimeType;
    
    sb->iWrite->WriteInt32L( properties.Count() );
    for ( TInt i = 0; i < properties.Count(); i++ )
    	{
    	properties[i]->ExternalizeL( *sb->iWrite );
    	}

	StreamBufferToChunkL( memptr, sb );
    
    CleanupStack::PopAndDestroy(6); //sb, properties, query, rStream, filters, storename
	
	}

// ------------------------------------------------------------------------------------------------
// CNSmlDSHostSession::OpenL
// Opens the data store specified by aStoreName.
// ------------------------------------------------------------------------------------------------	
void CNSmlDSHostSession::OpenL( const RMessage2& aMessage )
    {
    if ( HasDataStoreL( aMessage ) )
    	{
    	User::Leave( KErrAlreadyExists );
    	}
    
    CNSmlDSAsyncCallBackForOpen* p = new ( ELeave ) CNSmlDSAsyncCallBackForOpen( 
    	this, aMessage, &CNSmlDSHostSession::OpenFinishedL );
    CleanupStack::PushL( p );
    
    p->iDpi = DataProviderItemL( aMessage.Int0() );
    CSmlDataProvider* dp = p->iDpi->iDataprovider;
    TMemPtr& memptr( MemPtrL() );
    RReadStream& rStream = memptr.ReadStreamLC();
    
    //server id
    TInt tempLength = rStream.ReadUint32L();
	p->iServerId = HBufC::NewL( rStream, tempLength );
	
	//remotedb
	tempLength = rStream.ReadUint32L();
	p->iRemoteDB = HBufC::NewL( rStream, tempLength );
	
	CleanupStack::PopAndDestroy(); //rStream
    
    //Localdb same as storename 
    HBufC* storeName = DataStoreNameLC( aMessage );
    
    CNSmlAdapterLog* adpLog = CNSmlAdapterLog::NewL();
    CleanupStack::PushL( adpLog );
    
    adpLog->SetAdapterLogKeyL( dp->Identifier(), *storeName, *p->iServerId, *p->iRemoteDB );
    CSmlDataStore* ds = dp->NewStoreInstanceLC();
    p->iDSItem = iDataProviders.InsertL( p->iDpi, ds, storeName );
    p->iDSItem->iAdpLog = adpLog;
    CleanupStack::Pop( 3 ); //ds, adpLog, storeName. iDataProviders takes the ownership
    
    p->CallDSAsyncLC().OpenL( *storeName, *adpLog, p->iStatus );
    
    CleanupStack::Pop( 2 ); //CallDSAsyncLC, p
    }

// ------------------------------------------------------------------------------------------------
// CNSmlDSHostSession::OpenFinished
// called when OpenL request is finished.
// Note! when aOperation is CNSmlDSAsyncCallBack::ECanceled or CNSmlDSAsyncCallBack::EFree, 
// aDSAO->iDSItem might be NULL.
// ------------------------------------------------------------------------------------------------	
TInt CNSmlDSHostSession::OpenFinishedL( CNSmlDSAsyncCallBack* aDSAO, TCallBackOperation aOperation )
    {
    CNSmlDSAsyncCallBackForOpen* p = static_cast<CNSmlDSAsyncCallBackForOpen*> ( aDSAO );
    TInt err( aDSAO->iStatus.Int() );
    
    switch( aOperation )
    	{
		case CNSmlDSAsyncCallBack::EFinished:
			if ( err == KErrNone )
				{
				aDSAO->iDSItem->SetOpened();
				}
			else
				{
				//calling recursively
				OpenFinishedL( aDSAO, CNSmlDSAsyncCallBack::ECanceled );
				}
	    	break;
	    case CNSmlDSAsyncCallBack::ECanceled:
	    	iDataProviders.Remove( p->iDpi, 
	    		p->iDSItem ); //aDSAO->iDSItem might be NULL, but remove checks that.
	    	err = KErrNone;
	    	break;
	    case CNSmlDSAsyncCallBack::EFree:
			delete p->iServerId;
			p->iServerId = NULL;
			delete p->iRemoteDB;
			p->iRemoteDB = NULL;
			
	    	aDSAO->iPtr = NULL;
	    	break;
		default:
			User::Leave( KErrUnknown );
    	};

    return err;
    }

// ------------------------------------------------------------------------------------------------
// CNSmlDSHostSession::CancelRequest
// Cancel the current asynchronous request.
// ------------------------------------------------------------------------------------------------	
void CNSmlDSHostSession::CancelRequestL( const RMessage2& aMessage )
	{
    TNSmlDSDataStoreElement* dsi = DataStoreItemL( aMessage );
    CNSmlDSAsyncRequestHandler* dsao = dsi->iDSAO;
    //Note! it is possible, that after deleting dsao, 
    //dsi may not exist anymore.
    dsi = NULL;
    
    if ( dsao )
        {
    	delete dsao;
    	dsao = NULL;
        }
	}

// ------------------------------------------------------------------------------------------------
// CNSmlDSHostSession::BeginTransaction
// Starts the transaction mode.
// ------------------------------------------------------------------------------------------------	
void CNSmlDSHostSession::BeginTransactionL( const RMessage2& aMessage )
	{
    TNSmlDSDataStoreElement* dsi = DataStoreItemL( aMessage );

    CSmlDataStore* ds = dsi->iDataStore;
    ds->BeginTransactionL();
    dsi->iHostMode = ENSmlTransactionMode;
	}

// ------------------------------------------------------------------------------------------------
// CNSmlDSHostSession::CommitTransaction
// Method will be called at the end of a successful transaction.
// ------------------------------------------------------------------------------------------------	
void CNSmlDSHostSession::CommitTransactionL( const RMessage2& aMessage )
	{
    TNSmlDSDataStoreElement* dsi = DataStoreItemL( aMessage );
   
    CNSmlDSAsyncCallBack* dsao = new ( ELeave ) CNSmlDSAsyncCallBack( this, dsi, aMessage );
    CleanupStack::PushL( dsao );
    dsao->CallDSAsyncLC().CommitTransactionL( dsao->iStatus );
    dsi->iHostMode = ENSmlNormalMode;
    CleanupStack::Pop( 2 ); //CallDSAsyncLC(), dsao
	}

// ------------------------------------------------------------------------------------------------
// CNSmlDSHostSession::RevertTransaction
// Method will be called to abort an ongoing transaction.
// ------------------------------------------------------------------------------------------------	
void CNSmlDSHostSession::RevertTransactionL( const RMessage2& aMessage )
	{
    TNSmlDSDataStoreElement* dsi = DataStoreItemL( aMessage );
    
    CNSmlDSAsyncCallBack* dsao = new ( ELeave ) CNSmlDSAsyncCallBack( this, dsi, aMessage );
    CleanupStack::PushL( dsao );
    dsao->CallDSAsyncLC().RevertTransaction( dsao->iStatus );
    dsi->iHostMode = ENSmlNormalMode;
    CleanupStack::Pop( 2 ); //CallDSAsyncLC(), dsao
	}

// ------------------------------------------------------------------------------------------------
// CNSmlDSHostSession::BeginBatch
// Starts the batch mode.
// ------------------------------------------------------------------------------------------------	
void CNSmlDSHostSession::BeginBatchL( const RMessage2& aMessage )
	{
    TNSmlDSDataStoreElement* dsi = DataStoreItemL( aMessage );

    RBatchItemUidArray* biua= new ( ELeave ) RBatchItemUidArray();
    CleanupRPtrArrayPushL( biua );
    CSmlDataStore* ds = dsi->iDataStore;
    ds->BeginBatchL();
    dsi->iHostMode = ENSmlBatchMode;
    dsi->iBatchItemUids = biua;
    CleanupStack::Pop( biua );
	}
	
// ------------------------------------------------------------------------------------------------
// CNSmlDSHostSession::CommitBatchL
// Method will be called at the end of the batch mode.
// ------------------------------------------------------------------------------------------------	
void CNSmlDSHostSession::CommitBatchL( const RMessage2& aMessage )
	{
    TNSmlDSDataStoreElement* dsi = DataStoreItemL( aMessage );
   
    RArray<TInt>* resultArray = new ( ELeave ) RArray<TInt>;
    CleanupStack::PushL( resultArray );
    CNSmlDSAsyncCallBack* dsao = new ( ELeave ) CNSmlDSAsyncCallBack( 
    	this, dsi, aMessage, &CNSmlDSHostSession::CommitBatchRequestFinishedL, resultArray );
    CleanupStack::Pop( resultArray ); //dsao takes ownership
    
    CleanupStack::PushL( dsao );
    dsao->CallDSAsyncLC().CommitBatchL( *resultArray, dsao->iStatus );
    CleanupStack::Pop( 2 ); //CallDSAsyncLC(), dsao
	}

// ------------------------------------------------------------------------------------------------
// CNSmlDSHostSession::CommitBatchRequestFinished
// Called when commitBatchL request is finished
// ------------------------------------------------------------------------------------------------	
TInt CNSmlDSHostSession::CommitBatchRequestFinishedL( CNSmlDSAsyncCallBack* aDSAO, 
	TCallBackOperation aOperation )
    {
    TInt status ( aDSAO->iStatus.Int() );
    TNSmlDSDataStoreElement* dsi = aDSAO->iDSItem;
    RArray<TInt>* resultArray = reinterpret_cast<RArray<TInt>*>( aDSAO->iPtr );
    
    switch( aOperation )
    	{
		case CNSmlDSAsyncCallBack::EFinished:
			dsi->iHostMode = ENSmlNormalMode;
			if ( status == KErrNone )
				{
		        TMemPtr& dest( MemPtrL() );
		        TStreamBuffers* sb = dest.StreamBufferLC();
		        
		        // write results
				sb->iWrite->WriteInt32L( resultArray->Count() );
				for ( TInt i = 0; i < resultArray->Count(); i++ )
					{
					sb->iWrite->WriteInt32L( ( *resultArray )[i] );
					}
				// write uids that are added.
				if ( dsi->iBatchItemUids )
					{
					sb->iWrite->WriteInt32L( dsi->iBatchItemUids->Count() );
					for ( TInt i = 0; i < dsi->iBatchItemUids->Count(); i++ )
						{
						TPckgC<TSmlDbItemUid> uid( *( *dsi->iBatchItemUids )[i] );
						sb->iWrite->WriteL( uid );
						}
					}
				else
					{
					sb->iWrite->WriteInt32L( 0 );
					}
				StreamBufferToChunkL( dest, sb );
				CleanupStack::PopAndDestroy(); //sb
		        status = aDSAO->iStatus.Int();
				}
	    	break;
	    case CNSmlDSAsyncCallBack::ECanceled:
	    	break;
	    case CNSmlDSAsyncCallBack::EFree:
	    	dsi->FreeBatchItemArray();
	    	resultArray->Close();
	    	delete resultArray;
	    	aDSAO->iPtr = NULL;
	    	break;
		default:
			User::Leave( KErrUnknown );
    	}

    return status;
    }

// ------------------------------------------------------------------------------------------------
// CNSmlDSHostSession::CancelBatchL
// Method will be called to abort an ongoing batch mode.
// ------------------------------------------------------------------------------------------------	
void CNSmlDSHostSession::CancelBatchL( const RMessage2& aMessage )
	{
    TNSmlDSDataStoreElement* dsi = DataStoreItemL( aMessage );

    CSmlDataStore* ds = dsi->iDataStore;
    ds->CancelBatch();
    dsi->iHostMode = ENSmlNormalMode;
    dsi->FreeBatchItemArray();
	}
	
// ------------------------------------------------------------------------------------------------
// CNSmlDSHostSession::SetRemoteDataStoreFormatL
// Sets the Sync Partner Data Format.
// ------------------------------------------------------------------------------------------------	
void CNSmlDSHostSession::SetRemoteDataStoreFormatL( const RMessage2& aMessage )
	{
	TNSmlDSDataStoreElement* dsi = DataStoreItemL( aMessage );
    CSmlDataStore* ds = dsi->iDataStore;
    TMemPtr& ptr( MemPtrL() );
    RReadStream& readStream = ptr.ReadStreamLC();

    CSmlDataStoreFormat* serverDataStoreFormat = CSmlDataStoreFormat::NewLC( iStringPool, readStream );
    ds->SetRemoteStoreFormatL( *serverDataStoreFormat );
    dsi->setStoreFormat( serverDataStoreFormat );
    CleanupStack::Pop(); //serverDataStoreFormat
    CleanupStack::PopAndDestroy(); //readStream
	}
	
// ------------------------------------------------------------------------------------------------
// CNSmlDSHostSession::SetRemoteMaxObjectSizeL
// Sets the SyncML server Sync Partner maximum object size.
// ------------------------------------------------------------------------------------------------	
void CNSmlDSHostSession::SetRemoteMaxObjectSizeL( const RMessage2& aMessage )
	{
    CSmlDataStore* ds = DataStoreL( aMessage );
    
    TMemPtr& ptr( MemPtrL() );
    TInt tmp(0);
    TPckg<TInt> maxObjectSize( tmp );
    ptr.CopyTo( maxObjectSize );
    ds->SetRemoteMaxObjectSize( maxObjectSize() );
	}
	
// ------------------------------------------------------------------------------------------------
// CNSmlDSHostSession::MaxObjectSizeL
// Gets the Data Store maximum object size which is reported to the SyncML partner.
// ------------------------------------------------------------------------------------------------	
void CNSmlDSHostSession::MaxObjectSizeL( const RMessage2& aMessage )
	{
    CSmlDataStore* ds = DataStoreL( aMessage );

    TInt maxObjectSize = ds->MaxObjectSize();
    TPckgC<TInt> pck( maxObjectSize );
    aMessage.WriteL( 2, pck );
	}	

// ------------------------------------------------------------------------------------------------
// CNSmlDSHostSession::OpenItemL
// Opens item at Data Store.
// ------------------------------------------------------------------------------------------------	
void CNSmlDSHostSession::OpenItemL( const RMessage2& aMessage )
    {
    TNSmlDSDataStoreElement* dsi = DataStoreItemL( aMessage );
    CNSmlServerDSHostItem* dshi = DataStoreItemParamsLC();
    
    CNSmlDSAsyncCallBack* dsao = new ( ELeave ) CNSmlDSAsyncCallBack( 
    	this, dsi, aMessage, &CNSmlDSHostSession::OpenItemRequestFinishedL, dshi );
    CleanupStack::Pop( dshi ); //dsao takes ownership
    CleanupStack::PushL( dsao );
    
    dsao->CallDSAsyncLC().OpenItemL( 
    	dshi->Uid(), dshi->FieldChange(), dshi->Size(), dshi->ParentUid(), 
    	dshi->iMimeTypePtr, dshi->iMimeVerPtr, 
    	dsao->iStatus );
  
    CleanupStack::Pop(2); //CallDSAsyncLC(), dsao
    }

// ------------------------------------------------------------------------------------------------
// CNSmlDSHostSession::OpenItemRequestFinishedL
//Called when OpenItemL request is finished.
// ------------------------------------------------------------------------------------------------
TInt CNSmlDSHostSession::OpenItemRequestFinishedL( CNSmlDSAsyncCallBack* aDSAO, 
	TCallBackOperation aOperation )
    {
    TInt status( aDSAO->iStatus.Int() );
    CNSmlServerDSHostItem* dshi = reinterpret_cast<CNSmlServerDSHostItem*>( aDSAO->iPtr );
    
    switch( aOperation )
    	{
		case CNSmlDSAsyncCallBack::EFinished:
			if ( status == KErrNone )
				{
				dshi->SetMimeTypeL( dshi->iMimeTypePtr );
	    		dshi->SetMimeVerL( dshi->iMimeVerPtr );
	    		WriteDataStoreItemParamsL( dshi );
				}
	    	break;
	    case CNSmlDSAsyncCallBack::ECanceled:
	    	status = KErrNone;
	    	break;
	    case CNSmlDSAsyncCallBack::EFree:
	    	delete dshi;
	    	aDSAO->iPtr = NULL;
	    	break;
		default:
			User::Leave( KErrUnknown );
    	}

    return status;
    }

// ------------------------------------------------------------------------------------------------
// CNSmlDSHostSession::DataStoreItemParamsLC
// ------------------------------------------------------------------------------------------------
CNSmlDSHostSession::CNSmlServerDSHostItem* CNSmlDSHostSession::DataStoreItemParamsLC()
    {
    CNSmlServerDSHostItem* dshi = CNSmlServerDSHostItem::NewLC();
    TMemPtr& ptr( MemPtrL() );
    RReadStream& mrs = ptr.ReadStreamLC();
    
    dshi->InternalizeL( mrs );
    
    CleanupStack::PopAndDestroy(); //mrs
    return dshi;
    }

// ------------------------------------------------------------------------------------------------
// CNSmlDSHostSession::WriteDataStoreItemParamsL
// ------------------------------------------------------------------------------------------------
void CNSmlDSHostSession::WriteDataStoreItemParamsL( CNSmlDSHostItem* dshi )
    {
    TMemPtr& ptr( MemPtrL() );
    RWriteStream& mws = ptr.WriteStreamLC();
    dshi->ExternalizeL( mws );
    mws.CommitL();
    CleanupStack::PopAndDestroy(); //mws
    }

// ------------------------------------------------------------------------------------------------
// CNSmlDSHostSession::CreateItemL
// Creates new item to Data Store.
// ------------------------------------------------------------------------------------------------	
void CNSmlDSHostSession::CreateItemL( const RMessage2& aMessage )
    {
    TNSmlDSDataStoreElement* dsi = DataStoreItemL( aMessage );
    CNSmlServerDSHostItem* dshi = DataStoreItemParamsLC();
    CNSmlDSAsyncCallBack* dsao = new ( ELeave ) CNSmlDSAsyncCallBack( 
    	this, dsi, aMessage, &CNSmlDSHostSession::CreateItemRequestFinishedL, dshi );
    CleanupStack::Pop( dshi ); //dsao takes ownership
    CleanupStack::PushL( dsao );
    
    TSmlDbItemUid* uid = NULL;

    if ( dsi->iHostMode == ENSmlBatchMode )
        {
        uid = new ( ELeave ) TSmlDbItemUid();
        CleanupStack::PushL( uid );
        dsi->iBatchItemUids->AppendL( uid );
        CleanupStack::Pop( uid );
        }
    else{
        uid = &dsi->iCreatedUid;
        }
        
    *uid = dshi->Uid();
    dshi->iCreateItemUid = uid;

    dsao->CallDSAsyncLC().CreateItemL(
    	*uid, dshi->Size(), dshi->ParentUid(), *dshi->MimeType(), 
    	*dshi->MimeVer(), dsao->iStatus );
    	
    CleanupStack::Pop( 2 ); //CallDSAsyncLC(), dsao
    }

// ------------------------------------------------------------------------------------------------
// CNSmlDSHostSession::CreateItemRequestFinishedL
// Called when CreateItemL request is finished.
// ------------------------------------------------------------------------------------------------
TInt CNSmlDSHostSession::CreateItemRequestFinishedL( CNSmlDSAsyncCallBack* aDSAO, 
	TCallBackOperation aOperation )
    {
    TInt status( aDSAO->iStatus.Int() );
    CNSmlServerDSHostItem* dshi = reinterpret_cast<CNSmlServerDSHostItem*>( aDSAO->iPtr );
    
    switch( aOperation )
    	{
		case CNSmlDSAsyncCallBack::EFinished:
			if ( status == KErrNone )
				{
				dshi->SetUid( *dshi->iCreateItemUid );
				WriteDataStoreItemParamsL( dshi );
				}
	    	break;
	    case CNSmlDSAsyncCallBack::ECanceled:
	    	status = KErrNone;
	    	break;
	    case CNSmlDSAsyncCallBack::EFree:
	    	delete dshi;
	    	aDSAO->iPtr = NULL;
	    	break;
		default:
			User::Leave( KErrUnknown );
    	};

    return status;
    }
	
// ------------------------------------------------------------------------------------------------
// CNSmlDSHostSession::ReplaceItemL
// Replaces old item at Data Store.
// ------------------------------------------------------------------------------------------------	
void CNSmlDSHostSession::ReplaceItemL( const RMessage2& aMessage )
    {
    TNSmlDSDataStoreElement* dsi = DataStoreItemL( aMessage );
    CNSmlDSHostItem* dshi = DataStoreItemParamsLC();
    CNSmlDSAsyncCallBack* dsao = new ( ELeave ) CNSmlDSAsyncCallBack( this, dsi, aMessage );
    CleanupStack::PushL( dsao );
    dsao->CallDSAsyncLC().ReplaceItemL( 
    	dshi->Uid(), dshi->Size(), dshi->ParentUid(), dshi->FieldChange(), dsao->iStatus );
    CleanupStack::Pop( 2 ); //CallDSAsyncLC(), dsao
    CleanupStack::PopAndDestroy(); //dshi
    }

// ------------------------------------------------------------------------------------------------
// CNSmlDSHostSession::ReadItemL
// Reads data from item at Data Store. Item must be opened before this method can be called.
// This method is called until aBuffer is not used totally or method leaves with KErrEof.
// ------------------------------------------------------------------------------------------------	
void CNSmlDSHostSession::ReadItemL( const RMessage2& aMessage )
	{
    CSmlDataStore* ds = DataStoreL( aMessage );
    TMemPtr& mem( MemPtrL() );
    TInt32 sizeBytes(0);
    
    TPckg<TInt32> bytesToRead( sizeBytes );
    mem.CopyTo( bytesToRead );
    
    TInt chunkSize( mem.Size() - sizeof( TInt ) );
    
    if ( sizeBytes >  chunkSize )
    	{
    	sizeBytes = chunkSize;
    	}
    
    TPtr8 ptr( mem.Mid( sizeof( TInt ), sizeBytes ) );
    ds->ReadItemL( ptr );
    
    TPckgC<TInt32> ReadBytes( ptr.Size() );
    mem.Des().Copy( ReadBytes );
	}

// ------------------------------------------------------------------------------------------------
// CNSmlDSHostSession::WriteItemL
// Writes data to item to Data Provider. CreateItemL or ReplaceItemL method must be called before
// this method can be called. This method is called until all data to current item is written.
// ------------------------------------------------------------------------------------------------	
void CNSmlDSHostSession::WriteItemL( const RMessage2& aMessage )
	{
    CSmlDataStore* ds = DataStoreL( aMessage );
    TMemPtr& mem( MemPtrL() );
    
    TInt32 tmp(0);
    TPckg<TInt32> bytesToWrite( tmp );
    mem.CopyTo( bytesToWrite );
    
    TPtr8 ptr( mem.Mid( 4, bytesToWrite() ) );
    ptr.SetLength( ptr.MaxLength() );
    
    ds->WriteItemL( ptr );
    
    TPckgC<TInt32> bytesWritten( ptr.Size() );
    mem.Des().Copy( bytesWritten );
	}
	
// ------------------------------------------------------------------------------------------------
// CNSmlDSHostSession::CommitItemL
// After item is written to Data Provider it can be saved to the Data Store.
// This method can be called just after WriteItemL method.
// ------------------------------------------------------------------------------------------------	
void CNSmlDSHostSession::CommitItemL( const RMessage2& aMessage )
	{
    TNSmlDSDataStoreElement* dsi = DataStoreItemL( aMessage );

    CNSmlDSAsyncCallBack* dsao = new ( ELeave ) CNSmlDSAsyncCallBack( 
    	this, dsi, aMessage, &CNSmlDSHostSession::CommitItemRequestFinishedL );
    CleanupStack::PushL( dsao );
    dsao->CallDSAsyncLC().CommitItemL( dsao->iStatus );
    CleanupStack::Pop(2); //CallDSAsyncLC, dsao
	}

// ------------------------------------------------------------------------------------------------
// CNSmlDSHostSession::CommitItemRequestFinishedL
// Called when CommitItemL request is finished.
// ------------------------------------------------------------------------------------------------
TInt CNSmlDSHostSession::CommitItemRequestFinishedL( CNSmlDSAsyncCallBack* aDSAO, 
	TCallBackOperation aOperation )
    {
    TInt status( aDSAO->iStatus.Int() );
    
    switch( aOperation )
    	{
		case CNSmlDSAsyncCallBack::EFinished:
			if ( status == KErrNone )
				{
				TNSmlDSDataStoreElement* dsi = aDSAO->iDSItem;

	    		if ( dsi->iHostMode != ENSmlBatchMode )
	    	    	{
	        		TSmlDbItemUid uid = dsi->iCreatedUid;
	        		TPckgC<TSmlDbItemUid> pck( uid );
	        		aDSAO->iMessage.WriteL( 2, pck );
		        	}
				}
	    	break;
	    case CNSmlDSAsyncCallBack::ECanceled:
	    	status = KErrNone;
	    	break;
	    case CNSmlDSAsyncCallBack::EFree:
	    	break;
		default:
			User::Leave( KErrUnknown );
    	}

    return status;
    }
	
// ------------------------------------------------------------------------------------------------
// CNSmlDSHostSession::CloseItem
// Closes opened item.
// ------------------------------------------------------------------------------------------------	
void CNSmlDSHostSession::CloseItemL( const RMessage2& aMessage )
	{
    DataStoreL( aMessage )->CloseItem();
	}
	
// ------------------------------------------------------------------------------------------------
// CNSmlDSHostSession::MoveItemL
// Moves item to new location.
// ------------------------------------------------------------------------------------------------	
void CNSmlDSHostSession::MoveItemL( const RMessage2& aMessage )
	{
    TNSmlDSDataStoreElement* dsi = DataStoreItemL( aMessage );    
    
    CNSmlDSHostItem* dshi = DataStoreItemParamsLC();
    CNSmlDSAsyncCallBack* dsao = new ( ELeave ) CNSmlDSAsyncCallBack( this, dsi, aMessage );
    CleanupStack::PushL( dsao );
    dsao->CallDSAsyncLC().MoveItemL( dshi->Uid(), dshi->ParentUid(), dsao->iStatus );
    CleanupStack::Pop(2); //CallDSAsyncLC(), dsao
    CleanupStack::PopAndDestroy(); //dshi
	}
		
// ------------------------------------------------------------------------------------------------
// CNSmlDSHostSession::DeleteItemL
// Deletes one item at Data Store permanently.
// ------------------------------------------------------------------------------------------------	
void CNSmlDSHostSession::DeleteItemL( const RMessage2& aMessage )
	{
    TNSmlDSDataStoreElement* dsi = DataStoreItemL( aMessage );
    
    CNSmlDSHostItem* dshi = DataStoreItemParamsLC();
    CNSmlDSAsyncCallBack* dsao = new ( ELeave ) CNSmlDSAsyncCallBack( this, dsi, aMessage );
    CleanupStack::PushL( dsao );
    dsao->CallDSAsyncLC().DeleteItemL( dshi->Uid(), dsao->iStatus );
    CleanupStack::Pop(2); //CallDSAsyncLC(), dsao
    CleanupStack::PopAndDestroy(); //dshi
	}
	
// ------------------------------------------------------------------------------------------------
// CNSmlDSHostSession::SoftDeleteItemL
// Soft deletes one item at Data Store.
// ------------------------------------------------------------------------------------------------	
void CNSmlDSHostSession::SoftDeleteItemL( const RMessage2& aMessage )
	{
    TNSmlDSDataStoreElement* dsi = DataStoreItemL( aMessage );
    
    CNSmlDSHostItem* dshi = DataStoreItemParamsLC();
    CNSmlDSAsyncCallBack* dsao = new ( ELeave ) CNSmlDSAsyncCallBack( this, dsi, aMessage );
    CleanupStack::PushL( dsao );
    dsao->CallDSAsyncLC().SoftDeleteItemL( dshi->Uid(), dsao->iStatus );
    CleanupStack::Pop(2); //CallDSAsyncLC(), dsao
    CleanupStack::PopAndDestroy(); //dshi
	}
	
// ------------------------------------------------------------------------------------------------
// CNSmlDSHostSession::DeleteAllItemsL
// Deletes all items at Data Store permanently.
// ------------------------------------------------------------------------------------------------	
void CNSmlDSHostSession::DeleteAllItemsL( const RMessage2& aMessage )
	{
    TNSmlDSDataStoreElement* dsi = DataStoreItemL( aMessage );
    CNSmlDSAsyncCallBack* dsao = new ( ELeave ) CNSmlDSAsyncCallBack( this, dsi, aMessage );
    CleanupStack::PushL( dsao );
    dsao->CallDSAsyncLC().DeleteAllItemsL( dsao->iStatus );
    CleanupStack::Pop(2); //CallDSAsyncLC(), dsao
	}

// ------------------------------------------------------------------------------------------------
// CNSmlDSHostSession::HasSyncHistoryL
// Checks if the Data Store has sync history. If not then slow sync is proposed to Sync Partner.
// ------------------------------------------------------------------------------------------------	
void CNSmlDSHostSession::HasSyncHistoryL( const RMessage2& aMessage )
	{
    CSmlDataStore* ds = DataStoreL( aMessage );
    TPckgC<TBool> pck( TBool( ds->HasSyncHistory() ) );
    aMessage.WriteL( 2, pck );
	}
	
// ------------------------------------------------------------------------------------------------
// CNSmlDSHostSession::AddedItemsL
// The Data Provider returns UIDs of items that are added after previous synchronization.
// ------------------------------------------------------------------------------------------------	
void CNSmlDSHostSession::AddedItemsL( const RMessage2& aMessage )
	{
    CSmlDataStore* ds = DataStoreL( aMessage );
    
    const MSmlDataItemUidSet& dius = ds->AddedItems();
    RNSmlDbItemModificationSet dbims;
    CleanupClosePushL( dbims );
    
    dbims.AddGroupL( dius, TNSmlDbItemModification::ENSmlDbItemAdd );
    
    TMemPtr& dest( MemPtrL() );
    dest.AdjustChunkIfNeededL( dbims.StreamSize() );
    RWriteStream& wStream = dest.WriteStreamLC();
	dbims.ExternalizeL( wStream );
	wStream.CommitL();
	CleanupStack::PopAndDestroy(2); //wStream, dbims
	}
	
// ------------------------------------------------------------------------------------------------
// CNSmlDSHostSession::DeletedItemsL
// The Data Provider returns UIDs of items that are deleted after previous synchronization.
// ------------------------------------------------------------------------------------------------	
void CNSmlDSHostSession::DeletedItemsL( const RMessage2& aMessage )
	{
    CSmlDataStore* ds = DataStoreL( aMessage );
    
    const MSmlDataItemUidSet& dius = ds->DeletedItems();
    RNSmlDbItemModificationSet dbims;
    CleanupClosePushL( dbims );
    
    dbims.AddGroupL( dius, TNSmlDbItemModification::ENSmlDbItemDelete );
    
    TMemPtr& dest( MemPtrL() );
    dest.AdjustChunkIfNeededL( dbims.StreamSize() );
    RWriteStream& wStream = dest.WriteStreamLC();
	dbims.ExternalizeL( wStream );
	wStream.CommitL();
	CleanupStack::PopAndDestroy(2); //wStream, dbims
	}

// ------------------------------------------------------------------------------------------------
// CNSmlDSHostSession::SoftDeleteItemsL
// The Data Provider returns UIDs of items that are soft deleted after previous synchronization.
// ------------------------------------------------------------------------------------------------	
void CNSmlDSHostSession::SoftDeleteItemsL( const RMessage2& aMessage )
	{
    CSmlDataStore* ds = DataStoreL( aMessage );
    
    const MSmlDataItemUidSet& dius = ds->SoftDeletedItems();
    RNSmlDbItemModificationSet dbims;
    CleanupClosePushL( dbims );
    
    dbims.AddGroupL( dius, TNSmlDbItemModification::ENSmlDbItemSoftDelete );
    
    TMemPtr& dest( MemPtrL() );
    dest.AdjustChunkIfNeededL( dbims.StreamSize() );
    RWriteStream& wStream = dest.WriteStreamLC();
	dbims.ExternalizeL( wStream );
	wStream.CommitL();
	CleanupStack::PopAndDestroy(2); //wStream, dbims
	}

// ------------------------------------------------------------------------------------------------
// CNSmlDSHostSession::ModifiedItemsL
// The Data Provider returns UIDs of items that are modified after previous synchronization.
// ------------------------------------------------------------------------------------------------	
void CNSmlDSHostSession::ModifiedItemsL( const RMessage2& aMessage )
	{
    CSmlDataStore* ds = DataStoreL( aMessage );
    
    const MSmlDataItemUidSet& dius = ds->ModifiedItems();
    RNSmlDbItemModificationSet dbims;
    CleanupClosePushL( dbims );
    
    dbims.AddGroupL( dius, TNSmlDbItemModification::ENSmlDbItemModify );
    
    TMemPtr& dest( MemPtrL() );
    dest.AdjustChunkIfNeededL( dbims.StreamSize() );
    RWriteStream& wStream = dest.WriteStreamLC();
	dbims.ExternalizeL( wStream );
	wStream.CommitL();
	CleanupStack::PopAndDestroy(2); //wStream, dbims
	}

// ------------------------------------------------------------------------------------------------
// CNSmlDSHostSession::MovedItemsL
// The Data Provider returns UIDs of items that are moved after previous synchronization.
// ------------------------------------------------------------------------------------------------	
void CNSmlDSHostSession::MovedItemsL( const RMessage2& aMessage )
	{
    CSmlDataStore* ds = DataStoreL( aMessage );
    
    const MSmlDataItemUidSet& dius = ds->MovedItems();
    RNSmlDbItemModificationSet dbims;
    CleanupClosePushL( dbims );
    
    dbims.AddGroupL( dius, TNSmlDbItemModification::ENSmlDbItemMove );
    
    TMemPtr& dest( MemPtrL() );
    dest.AdjustChunkIfNeededL( dbims.StreamSize() );
    RWriteStream& wStream = dest.WriteStreamLC();
	dbims.ExternalizeL( wStream );
	wStream.CommitL();
	CleanupStack::PopAndDestroy(2); //wStream, dbims
	}

// ------------------------------------------------------------------------------------------------
// CNSmlDSHostSession::AllItemsL
// ------------------------------------------------------------------------------------------------	
void CNSmlDSHostSession::AllItemsL( const RMessage2& aMessage )
	{
	TNSmlDSDataStoreElement* dsi = DataStoreItemL( aMessage );
	
    CNSmlDSChangedItemsFetcher* dsao = CNSmlDSChangedItemsFetcher::NewLC( 
    	this, dsi, aMessage, &CNSmlDSHostSession::AllItemsRequestFinishedL );
    dsao->FetchAllChangedItemsL();
    CleanupStack::Pop(); //dsao
	}

// ------------------------------------------------------------------------------------------------
// CNSmlDSHostSession::AllItemsRequestFinished
// Called when AllItems request is finished.
// ------------------------------------------------------------------------------------------------	
void CNSmlDSHostSession::AllItemsRequestFinishedL( CNSmlDSChangedItemsFetcher* aDSAO )
	{
	TMemPtr& dest( MemPtrL() );
	const RNSmlDbItemModificationSet* mods = aDSAO->ChangedItems();
	dest.AdjustChunkIfNeededL( mods->StreamSize() );
	RWriteStream& wStream = dest.WriteStreamLC();
	mods->ExternalizeL( wStream );
	wStream.CommitL();
	CleanupStack::PopAndDestroy(); //wStream
	}

// ------------------------------------------------------------------------------------------------
// CNSmlDSHostSession::ResetChangeInfoL
// Reset change info from the Data Provider. 
// ------------------------------------------------------------------------------------------------	
void CNSmlDSHostSession::ResetChangeInfoL( const RMessage2& aMessage )
	{
    TNSmlDSDataStoreElement* dsi = DataStoreItemL( aMessage );

    CNSmlDSAsyncCallBack* dsao = new ( ELeave ) CNSmlDSAsyncCallBack( this, dsi, aMessage );
    CleanupStack::PushL( dsao );
    dsao->CallDSAsyncLC().ResetChangeInfoL( dsao->iStatus );
    CleanupStack::Pop(2); //CallDSAsyncLC(), dsao
	}
		
// ------------------------------------------------------------------------------------------------
// CNSmlDSHostSession::CommitChangeInfoL
// This method is called after some changes are synchronized to Sync Partner. If some changes
// were synchronized correctly then those UIDs are included to aItems.
// ------------------------------------------------------------------------------------------------	
void CNSmlDSHostSession::CommitChangesL( const RMessage2& aMessage )
	{
    TNSmlDSDataStoreElement* dsi = DataStoreItemL( aMessage );
    
    CNSmlDataItemUidSet* dius = new ( ELeave ) CNSmlDataItemUidSet();
    CleanupStack::PushL( dius );
    TMemPtr& ptr( MemPtrL() );
    RReadStream& readStream = ptr.ReadStreamLC();
    dius->InternalizeL( readStream );
    CleanupStack::PopAndDestroy(); //readStream
    
    CNSmlDSAsyncCallBack* dsao = new ( ELeave ) CNSmlDSAsyncCallBack( 
    	this, dsi, aMessage, &CNSmlDSHostSession::CommitChangesRequestFinishedL, dius );
    CleanupStack::Pop( dius ); //dsao takes the ownership.
    CleanupStack::PushL( dsao );
    dsao->CallDSAsyncLC().CommitChangeInfoL( dsao->iStatus, *dius );
    CleanupStack::Pop(2); //CallDSAsyncLC(), dsao
	}
// ------------------------------------------------------------------------------------------------
// CNSmlDSHostSession::CommitItemRequestFinishedL
// This method is called after CommitChangesL request is finished.
// ------------------------------------------------------------------------------------------------	
TInt CNSmlDSHostSession::CommitChangesRequestFinishedL( CNSmlDSAsyncCallBack* aDSAO, 
	TCallBackOperation aOperation )
	{
	TInt status( aDSAO->iStatus.Int() );
	CNSmlDataItemUidSet* dius = reinterpret_cast<CNSmlDataItemUidSet*>( aDSAO->iPtr );
    
    switch( aOperation )
    	{
		case CNSmlDSAsyncCallBack::EFinished:
	    	break;
	    case CNSmlDSAsyncCallBack::ECanceled:
	    	status = KErrNone;
	    	break;
	    case CNSmlDSAsyncCallBack::EFree:
	    	delete dius;
	    	aDSAO->iPtr = NULL;
	    	break;
		default:
			User::Leave( KErrUnknown );
    	}

    return status;
	
	}

// ------------------------------------------------------------------------------------------------
// CNSmlDSHostSession::CommitAllChangesL
// This method is called after some changes are synchronized to Sync Partner. This method is used if
// all changes were synchronized correctly.
// ------------------------------------------------------------------------------------------------	
void CNSmlDSHostSession::CommitAllChangesL( const RMessage2& aMessage )
	{
    TNSmlDSDataStoreElement* dsi = DataStoreItemL( aMessage );
    CNSmlDSAsyncCallBack* dsao = new ( ELeave ) CNSmlDSAsyncCallBack( this, dsi, aMessage );
    CleanupStack::PushL( dsao );
    dsao->CallDSAsyncLC().CommitChangeInfoL( dsao->iStatus );
    CleanupStack::Pop(2); //CallDSAsyncLC(), dsao
	}

// ------------------------------------------------------------------------------------------------
// CNSmlDSHostSession::UpdateServerIdL
// Updates server id to adapter log.
// ------------------------------------------------------------------------------------------------		
void CNSmlDSHostSession::UpdateServerIdL( const RMessage2& /*aMessage*/ )
	{
	TMemPtr& ptr( MemPtrL() );
    RReadStream& readStream = ptr.ReadStreamLC();
    
	HBufC* oldServerId = HBufC::NewLC( readStream, readStream.ReadUint32L() );
	HBufC* newValue = HBufC::NewLC( readStream, readStream.ReadUint32L() );
	
	CNSmlAdapterLog* adpLog = CNSmlAdapterLog::NewLC();
	
	adpLog->SetServerIdL( *oldServerId, *newValue );
	
	CleanupStack::PopAndDestroy( 4 ); //adpLog, newValue, oldServerId, readStream
	}

// ------------------------------------------------------------------------------------------------
// CNSmlDSHostSession::DataProviderL
// ------------------------------------------------------------------------------------------------
CSmlDataProvider* CNSmlDSHostSession::DataProviderL( TSmlDataProviderId aId )
    {
    CSmlDataProvider* dp = iDataProviders.DataProvider( aId );
    if ( !dp )
        {
        User::Leave( ENSmlDSHostServerErrDPNotCreated );
        }
    return dp;
    }

// ------------------------------------------------------------------------------------------------
// CNSmlDSHostSession::DataProviderItemL
// ------------------------------------------------------------------------------------------------
TNSmlDSDataProviderElement* CNSmlDSHostSession::DataProviderItemL( TSmlDataProviderId aId )
    {
    TNSmlDSDataProviderElement* dpi = iDataProviders.DataProviderElement( aId );
    if ( !dpi )
        {
        User::Leave( ENSmlDSHostServerErrDPNotCreated );
        }
    return dpi;
    }
    
// ------------------------------------------------------------------------------------------------
// CNSmlDSHostSession::DataStoreL
// ------------------------------------------------------------------------------------------------
CSmlDataStore* CNSmlDSHostSession::DataStoreL( const RMessage2& aMessage )
    {
    TNSmlDSDataStoreElement* dsi = DataStoreItemL( aMessage );
    return dsi->iDataStore;
    }

// ------------------------------------------------------------------------------------------------
// CNSmlDSHostSession::DataStoreItemL
// ------------------------------------------------------------------------------------------------
TNSmlDSDataStoreElement* CNSmlDSHostSession::DataStoreItemL( const RMessage2& aMessage )
    {
    HBufC* storeName = DataStoreNameLC( aMessage );
    TNSmlDSDataStoreElement* dsi = iDataProviders.DataStoreElement( 
        iDataProviders.DataProviderElement( aMessage.Int0() ), *storeName );
    
    if ( !dsi )
        {
        User::Leave( ENSmlDSHostServerErrDSNotOpened );
        }

    CleanupStack::PopAndDestroy(); //storeName

    return dsi;
    }

// ------------------------------------------------------------------------------------------------
// CNSmlDSHostSession::HasDataStoreL
// ------------------------------------------------------------------------------------------------    
TBool CNSmlDSHostSession::HasDataStoreL( const RMessage2& aMessage )
	{
	HBufC* storeName = DataStoreNameLC( aMessage );
    TNSmlDSDataStoreElement* dsi = iDataProviders.DataStoreElement( 
        iDataProviders.DataProviderElement( aMessage.Int0() ), *storeName );
        
    CleanupStack::PopAndDestroy(); //storeName
    return ( dsi != NULL );
	}

// ------------------------------------------------------------------------------------------------
// CNSmlDSHostSession::DataStoreNameLC
// ------------------------------------------------------------------------------------------------
HBufC* CNSmlDSHostSession::DataStoreNameLC( const RMessage2& aMessage )
    {
    TInt desLength = aMessage.GetDesLength( 1 );
    if ( desLength < 0 )
        {
        //if desLength negative, contains system-wide error code.
        User::Leave( desLength );
        }
    HBufC* storeName = HBufC::NewLC( desLength );
    TPtr store( storeName->Des() );
    aMessage.ReadL( 1, store );
    return storeName;
    }
    
// ------------------------------------------------------------------------------------------------
// CNSmlDSHostSession::InternalizeFiltersL
// Read filters from stream.
// ------------------------------------------------------------------------------------------------
void CNSmlDSHostSession::InternalizeFiltersL( RReadStream& aStream, RPointerArray<CSyncMLFilter>& aFilters )
	{
	
	TInt count( aStream.ReadInt32L() );
	
	for ( TInt i = 0; i < count; i++ )
		{
		CSyncMLFilter* filter = CSyncMLFilter::NewLC( aStream );
		aFilters.AppendL( filter );
		CleanupStack::Pop(); // filter
		}
	}
	
// ------------------------------------------------------------------------------------------------
// CNSmlDSHostSession::ExternalizeFiltersL
// Write filters to stream.
// ------------------------------------------------------------------------------------------------	
void CNSmlDSHostSession::ExternalizeFiltersL( RWriteStream& aStream, const RPointerArray<CSyncMLFilter>& aFilters )
	{
	TInt count( aFilters.Count() );
	aStream.WriteInt32L( count );
	for ( TInt i(0); i < count; i++ )
		{
		aFilters[i]->ExternalizeL( aStream );
		}
	}
	
// ------------------------------------------------------------------------------------------------
// CNSmlDSHostSession::PanicClient
// ------------------------------------------------------------------------------------------------
void CNSmlDSHostSession::PanicClient( const RMessage2& aMessage, TInt aReason ) const
	{
	_DBG_FILE( "CNSmlDSHostSession::PanicClient(): begin " );
	aMessage.Panic( _L( "CNSmlDSHostSession" ), aReason );
	_DBG_FILE( "CNSmlDSHostSession::PanicClient(): end" );
	}

// ------------------------------------------------------------------------------------------------
// CNSmlDSHostSession::MemPtrL
// ------------------------------------------------------------------------------------------------
CNSmlDSHostSession::TMemPtr& CNSmlDSHostSession::MemPtrL()
    {
    if ( iMemPtr.iChunk.Handle() == 0 || !iMemPtr.iChunk.IsWritable() )
    	{
    	User::Leave( KErrBadHandle );
    	}
    return iMemPtr;
    }

// ------------------------------------------------------------------------------------------------
// CNSmlDSHostSession::StreamBufferToChunkL
// ------------------------------------------------------------------------------------------------
void CNSmlDSHostSession::StreamBufferToChunkL( TMemPtr& aMemPtr, TStreamBuffers* aSb )
	{
	aSb->iWrite->CommitL();
	aMemPtr.AdjustChunkIfNeededL( aSb->iBuffer->Size() );
	RWriteStream& wStream = aMemPtr.WriteStreamLC();
	wStream.WriteL( *aSb->iRead );
	wStream.CommitL();
	CleanupStack::PopAndDestroy(); //wStream
	}

// End of File
