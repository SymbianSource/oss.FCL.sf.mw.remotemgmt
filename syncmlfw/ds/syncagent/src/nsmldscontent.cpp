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
* Description:  Database (content) specific fata and functionality
*
*/


// INCLUDE FILES
#include <nsmlconstants.h>
#include <nsmldebug.h>
#include "nsmldbcaps.h"
#include <nsmlchangefinder.h>
#include "nsmldserror.h"
#include "nsmlcliagconstants.h"
#include "nsmldscontent.h"
#include "NSmlDSCmds.h"
#include "nsmldsluidbuffer.h"
#include "nsmldsmapcontainer.h"
#include "nsmlagentlog.h"
#include "NSmlURI.h"
#include "nsmldshostclient.h"
#include "NsmlDSContentItem.h"
#include "nsmldssettings.h"
#include <centralrepository.h> 

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CNSmlDSContent::CNSmlDSContent()
// C++ default constructor.
// -----------------------------------------------------------------------------
//
CNSmlDSContent::CNSmlDSContent()
	{
	}

// -----------------------------------------------------------------------------
// CNSmlDSContent::ConstructL
// Symbian 2nd phase constructor.
// -----------------------------------------------------------------------------
//
void CNSmlDSContent::ConstructL()
	{
// <MAPINFO_RESEND_MOD_BEGIN>
	SetMapStreamUID();
	iOldMapExists=EFalse;
	
	TInt keyVal;
	TRAPD (err ,ReadRepositoryL(KNsmlDsSuspendResume, keyVal));
	if(err==KErrNone && keyVal==1)
		{
		SetResendUsed(ETrue);	
		}
	else
		{
		SetResendUsed(EFalse);	
		}
// <MAPINFO_RESEND_MOD_END>
	}
	
// -----------------------------------------------------------------------------
// CNSmlDSContent::NewL
// Symbian two-phased constructor.
// -----------------------------------------------------------------------------
//
CNSmlDSContent* CNSmlDSContent::NewL()
	{
	CNSmlDSContent* self = new ( ELeave ) CNSmlDSContent();
	CleanupStack::PushL( self );
	self->ConstructL();
	CleanupStack::Pop(); // self
	return( self );
	}
	
// -----------------------------------------------------------------------------
// CNSmlDSContent::~CNSmlDSContent
// Destructor.
// -----------------------------------------------------------------------------
//
CNSmlDSContent::~CNSmlDSContent()
	{
	iStores.ResetAndDestroy();		
	iStores.Close();
	delete iOldServerId;
	delete iHostClient;
	iHierarchicalSupportTable.ResetAndDestroy();
	iHierarchicalSupportTable.Close();
	}

void CNSmlDSContent::InitContentDataL()
	{
	iHostClient = CNSmlDSHostClient::NewL();
	}
	
// -----------------------------------------------------------------------------
// CNSmlDSContent::CurrentIndex
// Returns the index of the current content (datastore). 
// -----------------------------------------------------------------------------
//
TInt CNSmlDSContent::CurrentIndex() const
	{
	return iIndex;
	}

// -----------------------------------------------------------------------------
// CNSmlDSContent::ContentIndexIsSet
// Checks whether some content is selected.
// -----------------------------------------------------------------------------
//
TBool CNSmlDSContent::ContentIndexIsSet() const
	{
	if ( iIndex == -1 )
		{
		return EFalse;
		}

	return ETrue;
	}

// -----------------------------------------------------------------------------
// CNSmlDSContent::Count
// Returns the number of contents in the content array.
// -----------------------------------------------------------------------------
//
TInt CNSmlDSContent::Count() const
	{
	return iStores.Count();
	}

// -----------------------------------------------------------------------------
// CNSmlDSContent::ImplementationUID
// Returns current content's unique identifier.
// -----------------------------------------------------------------------------
//
TInt CNSmlDSContent::ImplementationUID() const
	{
	return iStores[iIndex]->iImplementationUID;
	}

// ---------------------------------------------------------
// CNSmlDSContent::LocalDatabase()
// 
// ---------------------------------------------------------
CNSmlURI* CNSmlDSContent::LocalDatabase() const
	{
	return iStores[iIndex]->iLocalDatabase;
	}

// ---------------------------------------------------------
// CNSmlDSContent::RemoteDatabase()
// 
// ---------------------------------------------------------
CNSmlURI* CNSmlDSContent::RemoteDatabase() const
	{
	return iStores[iIndex]->iRemoteDatabase;
	}
	
//RD_SUSPEND_RESUME
// -----------------------------------------------------------------------------
// CNSmlDSContent::SyncType
// Returns the datastore's currently selected synchronisation type.
// -----------------------------------------------------------------------------
//
TPtrC8 CNSmlDSContent::SyncType() const
	{
	if ( iStores[iIndex]->iSyncType )
		{
		return iStores[iIndex]->iSyncType->Des();		
		}

	return TPtrC8( KNullDesC8 );
	}

//RD_SUSPEND_RESUME
// -----------------------------------------------------------------------------
// CNSmlDSContent::ClientSyncType
// Returns the Sync Type proposed by the Client in the Sync session
// -----------------------------------------------------------------------------
//
TPtrC8 CNSmlDSContent::ClientSyncType() 
	{
	
		if ( iStores[iIndex]->iClientSyncType )
		{
		  	return iStores[iIndex]->iClientSyncType->Des();
		}
	
	
	return TPtrC8( KNullDesC8 );
	}
// -----------------------------------------------------------------------------
// CNSmlDSContent::SupportsOperationL
// Checks if an operation is supported by the data provider.
// -----------------------------------------------------------------------------
//
TBool CNSmlDSContent::SupportsOperationL( TUid aOperation )
	{
	TInt resultCode;

// <MAPINFO_RESEND_MOD_BEGIN>
	if( aOperation == KUidSmlSupportBatch && ResendUsed() ) 
		{
		return EFalse;	
		}
// <MAPINFO_RESEND_MOD_END>

	return iHostClient->SupportsOperationL( aOperation, iStores[iIndex]->iImplementationUID, resultCode );
	}

// -----------------------------------------------------------------------------
// CNSmlDSContent::BeginTransActionL
// Sets the current datastore to the transaction mode.
// -----------------------------------------------------------------------------
//
void CNSmlDSContent::BeginTransActionL()
	{
	TInt resultCode;
	iStores[iIndex]->iAtomicModeOn = ETrue;
	iHostClient->BeginTransactionL( iStores[iIndex]->iImplementationUID, *iStores[iIndex]->iStoreName, resultCode );
	}
	
// -----------------------------------------------------------------------------
// CNSmlDSContent::CommitTransactionL
// Orders the current datastore to apply pending atomic operations.
// -----------------------------------------------------------------------------
//
void CNSmlDSContent::CommitTransactionL( TInt& aResultCode )
	{
	iStores[iIndex]->iAtomicModeOn = EFalse;
	iHostClient->CommitTransactionL( iStores[iIndex]->iImplementationUID, *iStores[iIndex]->iStoreName, aResultCode );
	}
	
// -----------------------------------------------------------------------------
// CNSmlDSContent::RevertTransactionL
// Aborts the ongoing transaction.
// -----------------------------------------------------------------------------
//
void CNSmlDSContent::RevertTransactionL( TInt& aResultCode )
	{
	iStores[iIndex]->iAtomicModeOn = EFalse;
	iHostClient->RevertTransactionL( iStores[iIndex]->iImplementationUID, *iStores[iIndex]->iStoreName, aResultCode );
	}
	
// -----------------------------------------------------------------------------
// CNSmlDSContent::AtomicModeOn
// Checks if the current datastore is in the batch mode.
// -----------------------------------------------------------------------------
//
TBool CNSmlDSContent::AtomicModeOn() const
	{
	return iStores[iIndex]->iAtomicModeOn;
	}
	
// -----------------------------------------------------------------------------
// CNSmlDSContent::OpenNextItemL
// Opens the next item in the LUID buffer.
// -----------------------------------------------------------------------------
//
TInt CNSmlDSContent::OpenNextItemL( TSmlDbItemUid& aLocalUid, TSmlDbItemUid& aParentUid, TDes8& aCmdName, TInt& aSize, CNSmlDbMeta& aMeta ) const
	{
	TInt ret( iStores[iIndex]->iLUIDBuffer->NextUIDL( aLocalUid, aCmdName ) );
	
	if ( ( ret == KErrNone ) && ( aCmdName != KNSmlAgentDelete ) )
		{
		TBool fieldChange;
		HBufC8* mimeType;
		HBufC8* mimeVersion;
		
		iHostClient->OpenItemL( aLocalUid,
								fieldChange, 
								aSize, 
								aParentUid,
								mimeType, 
								mimeVersion, 
								iStores[iIndex]->iImplementationUID, 
								*iStores[iIndex]->iStoreName, 
								ret );
								
		if ( ret == KErrNone )			
			{
			CleanupStack::PushL( mimeType );
			CleanupStack::PushL( mimeVersion );
			
			if ( fieldChange )
				{
				aCmdName = KNSmlAgentPartialReplace;			
				}

			aMeta.SetTypeL( *mimeType );
			aMeta.SetVersionL( *mimeVersion );
			
			CleanupStack::PopAndDestroy( 2 ); // mimeVersion, mimeType
			}
		}		
	
	return ret;
	}
	
// -----------------------------------------------------------------------------
// CNSmlDSContent::ReadItemL
// Reads data from the currently open item.
// -----------------------------------------------------------------------------
//
void CNSmlDSContent::ReadItemL( TDes8& aBuffer )
	{
	TInt resultCode;
	iHostClient->ReadItemL( aBuffer, iStores[iIndex]->iImplementationUID, *iStores[iIndex]->iStoreName, resultCode );
	}
	
// -----------------------------------------------------------------------------
// CNSmlDSContent::CloseItemL
// Closes the currently open item.
// -----------------------------------------------------------------------------
//
void CNSmlDSContent::CloseItemL()
	{
	TInt resultCode;
	iHostClient->CloseItemL( iStores[iIndex]->iImplementationUID, *iStores[iIndex]->iStoreName, resultCode );
	}	

// -----------------------------------------------------------------------------
// CNSmlDSContent::CurrentUID
// Returns the UID of the current item. 
// -----------------------------------------------------------------------------
//
void CNSmlDSContent::CurrentUID( TSmlDbItemUid& aLocalUID ) const
	{
	iStores[iIndex]->iLUIDBuffer->CurrentUID( aLocalUID );
	}
	
// ---------------------------------------------------------
// CNSmlDSContent::DbCapabilitiesL()
// 
// ---------------------------------------------------------
TInt CNSmlDSContent::DbCapabilitiesL( CNSmlDbCaps*& aDbCaps ) const
	{
	TInt resultCode( KErrNone );
	aDbCaps = iHostClient->StoreFormatL( iStores[iIndex]->iImplementationUID, resultCode );

	return resultCode;
	}

// -----------------------------------------------------------------------------
// CNSmlDSContent::HasSyncHistoryL
// Checks if the current datastore has been synced previously.
// -----------------------------------------------------------------------------
//
TBool CNSmlDSContent::HasSyncHistoryL() const
	{
	TInt resultCode;
	return iHostClient->HasSyncHistoryL( iStores[iIndex]->iImplementationUID, *iStores[iIndex]->iStoreName, resultCode );
	}
	
// -----------------------------------------------------------------------------
// CNSmlDSContent::IsSupportedRxL
// Checks if the given RxType is supported by the datastore.
// -----------------------------------------------------------------------------
//
TBool CNSmlDSContent::IsSupportedRxL( const TDesC8& aType ) const
	{
	TBool ret( EFalse );
	CNSmlDbCaps* dbCaps;
	
	TInt result = DbCapabilitiesL( dbCaps );
	User::LeaveIfError( result );
	
	CleanupStack::PushL( dbCaps );

	// dbCaps ownership is moved to a caller
	SmlDevInfDatastore_t*  datastore = dbCaps->DatastoreL();
	
	if ( !datastore )
		{
		CleanupStack::PopAndDestroy(); // dbCaps
		return ret;
		}
		
	// datastore ownership is moved to a caller
	CleanupStack::PushL( datastore );
	
	if ( aType == datastore->rxpref->cttype->Data() )
		{
		ret = ETrue;
		}
	else
		{
		SmlDevInfXmitList_t* rxList = datastore->rx;
		
		while ( rxList )
			{
			if ( aType == rxList->data->cttype->Data() )
				{
				ret = ETrue;
				break;
				}
				
			rxList = rxList->next;
			}
		}
		
	CleanupStack::PopAndDestroy( 2 ); // datastore, dbCaps

	return ret;
	}
	
// -----------------------------------------------------------------------------
// CNSmlDSContent::IsSupportedTxL
// Checks if the given TxType is supported by the datastore.
// -----------------------------------------------------------------------------
//
TBool CNSmlDSContent::IsSupportedTxL( const TDesC8& aType ) const
	{
	TBool ret = EFalse;
	CNSmlDbCaps* dbCaps;

	TInt result = DbCapabilitiesL( dbCaps );	
	User::LeaveIfError( result );
	
	CleanupStack::PushL( dbCaps );

	// dbCaps ownership is moved to a caller
	SmlDevInfDatastore_t*  datastore = dbCaps->DatastoreL();
	
	if ( !datastore )
		{
		CleanupStack::PopAndDestroy(); // dbCaps
		return ret;
		}
		
	// datastore ownership is moved to a caller
	CleanupStack::PushL( datastore );
	if ( aType == datastore->txpref->cttype->Data() )
		{
		ret = ETrue;
		}
	else
		{
		SmlDevInfXmitList_t* txList = datastore->tx;

		while ( txList )
			{
			if ( aType == txList->data->cttype->Data() )
				{
				ret = ETrue;
				break;
				}
				
			txList = txList->next;
			}
		}
		
	CleanupStack::PopAndDestroy( 2 ); // datastore, dbCaps

	return ret;
	}

// ---------------------------------------------------------
// CNSmlDSContent::IsSupportedSyncTypeL()
// 
// ---------------------------------------------------------
TBool CNSmlDSContent::IsSupportedSyncTypeL( CNSmlDbCaps::ENSmlSyncTypes aSyncType ) const
	{
	CNSmlDbCaps* dbCaps( NULL );
	
	TInt result = DbCapabilitiesL( dbCaps );
	User::LeaveIfError( result );
	
	TBool ret( dbCaps->SupportsSyncType( aSyncType ) );
	delete dbCaps;
	
	return ret;
	}

// ---------------------------------------------------------
// CNSmlDSContent::IsSupportedSyncTypeL()
// 
// ---------------------------------------------------------
TBool CNSmlDSContent::IsSupportedSyncTypeL( const TDesC8& aSyncType ) const
	{
	CNSmlDbCaps::ENSmlSyncTypes syncType;
	
	if ( SyncTypeConv ( aSyncType, syncType ) )
		{
		return IsSupportedSyncTypeL( syncType );
		}
		
	return EFalse;
	}

// ---------------------------------------------------------
// CNSmlDSContent::IsHierarchicalSyncSupportedL()
// 
// ---------------------------------------------------------
TBool CNSmlDSContent::IsHierarchicalSyncSupportedL( const TPtr8 aMediaType ) 
	{
	TBool* doesExist = iHierarchicalSupportTable.Find(aMediaType);
	if(doesExist)
	{
		return *doesExist;
	}
	TBool ret( EFalse );
	CNSmlDbCaps* dbCaps( NULL );
	
	TInt result = DbCapabilitiesL( dbCaps );
	User::LeaveIfError( result );
	
	CleanupStack::PushL( dbCaps );
	
	SmlDevInfDatastorePtr_t datastore = dbCaps->DatastoreL();
	CleanupStack::PushL( datastore );
	
	if ( datastore->rxpref->cttype->Data().Compare( aMediaType ) == 0 )
		{
		if ( datastore->supportHierarchicalSync )
			{
			ret = ETrue;
			}
		}
		else if ( datastore->rx )
			{
			SmlDevInfXmitListPtr_t rx = datastore->rx;
			
			while ( rx )
				{
				if ( rx->data && rx->data->Data().Compare( aMediaType ) == 0 )
					{
					if ( datastore->supportHierarchicalSync )
						{
						ret = ETrue;
						}
						
					break;
					}
					
				rx = rx->next;
				}
			}
	
	CleanupStack::PopAndDestroy( 2 ); // datastore, dbCaps
	HBufC8* targetType = aMediaType.AllocLC();
	doesExist = new (ELeave) TBool(ret);
	CleanupStack::PushL(doesExist);
	if(iHierarchicalSupportTable.Insert(targetType,doesExist)!=KErrNone)
		{
		// Insertion failed, ignore but free memory
		CleanupStack::PopAndDestroy(2); // doesExist, targetType
		}
	else
		{
		CleanupStack::Pop(2); // doesExist and targetType are owned by the hash table now
		}
	
	return ret;
	}

// ---------------------------------------------------------
// CNSmlDSContent::IsSupportedSyncTypeByServerL()
// 
// ---------------------------------------------------------
TBool CNSmlDSContent::IsSupportedSyncTypeByServerL() const
	{
	TUint flag;
	
	if ( SyncType() == KNSmlDSOneWayFromClient )
		{
		flag = KNSmlOneWayFromClientFlag;
		}
	else if ( SyncType() == KNSmlDSRefreshFromClient )
		{
		flag = KNSmlRefreshFromClientFlag;
		}
	else if ( SyncType() == KNSmlDSOneWayFromServer )
		{
		flag = KNSmlOneWayFromServerFlag;
		}
	else if ( SyncType() == KNSmlDSRefreshFromServer )
		{
		flag = KNSmlRefreshFromServerFlag;
		}
	else
		{
		return ETrue;
		}
		
	TUint syncTypeFlags( iStores[iIndex]->iAgentLog->IntValueL( EAgentLogServerSyncTypes ) );
	
	if ( syncTypeFlags != 0 )
		{
		if ( !IsFlagSet( syncTypeFlags, flag ) )
			{
			return EFalse;
			}
		}
		
	return ETrue;
	}
	
// -----------------------------------------------------------------------------
// CNSmlDSContent::Interrupted
// Checks if the synchronisation of the current datastore has been interrupted.
// -----------------------------------------------------------------------------
//
TBool CNSmlDSContent::Interrupted() const
	{
	return iStores[iIndex]->iInterrupted;
	}
	
// -----------------------------------------------------------------------------
// CNSmlDSContent::ServerStatusError
// Checks if there is a server error in the synchronisation log.
// -----------------------------------------------------------------------------
//
TBool CNSmlDSContent::ServerStatusError() const
	{
	return iStores[iIndex]->iStatusErrorInSyncLog;
	}
	
// -----------------------------------------------------------------------------
// CNSmlDSContent::WaitingInitAlert
// Checks if the current datastore is waiting for the initial alert.
// -----------------------------------------------------------------------------
//
TBool CNSmlDSContent::WaitingInitAlert() const
	{
	return iStores[iIndex]->iWaitingInitAlert;
	}
	
// -----------------------------------------------------------------------------
// CNSmlDSContent::SlowSyncRequestL
// Checks if slow synchronisation has been requested for the current datastore.
// -----------------------------------------------------------------------------
//
TBool CNSmlDSContent::SlowSyncRequestL() const
	{
	return iStores[iIndex]->iAgentLog->IntValueL( EAgentLogSlowSyncRequest );
	}
	
// -----------------------------------------------------------------------------
// CNSmlDSContent::AnyDatabaseIsInterrupted
// Checks if the sychronisation of any datastore has been interrupted.
// -----------------------------------------------------------------------------
//
TBool CNSmlDSContent::AnyDatabaseIsInterrupted() const
	{
	for ( TInt i = 0; i < iStores.Count(); i++ )
		{
		if ( iStores[i]->iInterrupted )
			{
			return ETrue;
			}
		}
		
	return EFalse;
	}
	
// -----------------------------------------------------------------------------
// CNSmlDSContent::AllDatabasesAreInterrupted
// Checks if all synchronisations are interrupted.
// -----------------------------------------------------------------------------
//
TBool CNSmlDSContent::AllDatabasesAreInterrupted()	const
	{
	for ( TInt i = 0; i < iStores.Count(); i++ )
		{
		if ( !iStores[i]->iInterrupted )
			{
			return EFalse;
			}
		}
	
	return ETrue;
	}
	
// -----------------------------------------------------------------------------
// CNSmlDSContent::AllDatabasesAreInterruptedImmediately
// Checks if all synchronisations are interrupted immediately.
// -----------------------------------------------------------------------------
//
TBool CNSmlDSContent::AllDatabasesAreInterruptedImmediately() const	
	{
	for ( TInt i = 0; i < iStores.Count(); i++ )
		{
		if ( !iStores[i]->iImmediatelyInterruption )
			{
			return EFalse;
			}
		}
		
	return ETrue;
	}

// -----------------------------------------------------------------------------
// CNSmlDSContent::AllDatabasesOneWayFromClient
// Checks if synchronisation type is client-initiated one-way synchronisation 
// for all datastores. 
// -----------------------------------------------------------------------------
//
TBool CNSmlDSContent::AllDatabasesOneWayFromClient() const
	{
    for ( TInt i = 0; i < iStores.Count(); i++ )
    	{
    	if ( *iStores[i]->iSyncType != KNSmlDSOneWayFromClient &&
    	     *iStores[i]->iSyncType != KNSmlDSOneWayFromClientByServer &&
    	     *iStores[i]->iSyncType != KNSmlDSRefreshFromClient &&
    	     *iStores[i]->iSyncType != KNSmlDSRefreshFromClientByServer )
    		{
    		return EFalse;
    		}
    	}
    
    return ETrue;
	}

// -----------------------------------------------------------------------------
// CNSmlDSContent::MapExists
// Checks if the mapped items buffer exists for the current datastore.
// -----------------------------------------------------------------------------
//
TBool CNSmlDSContent::MapExists() const	
	{
	if ( !iStores[iIndex]->iMapContainer )
		{
		return EFalse;
		}
		
	return iStores[iIndex]->iMapContainer->MapListExists();
	}
	
// ---------------------------------------------------------
// CNSmlDSContent::ClientItemCountAsked()
// 
// ---------------------------------------------------------
TBool CNSmlDSContent::ClientItemCountAsked() const
	{
	return iStores[iIndex]->iClientItemCountAsked;
	}

// ---------------------------------------------------------
// CNSmlDSContent::ServerItemCountAsked()
// 
// ---------------------------------------------------------
TBool CNSmlDSContent::ServerItemCountAsked() const
	{
	return iStores[iIndex]->iServerItemCountAsked;
	}
	
// ---------------------------------------------------------
// CNSmlDSContent::ClientItemCount()
// 
// ---------------------------------------------------------
TInt CNSmlDSContent::ClientItemCount() const
	{
	return iStores[iIndex]->iLUIDBuffer->ClientItemCount();
	}
	
// ---------------------------------------------------------
// CNSmlDSContent::ServerItemCount()
// 
// ---------------------------------------------------------
TInt CNSmlDSContent::ServerItemCount() const
	{
	iStores[iIndex]->iServerItemCountAsked = ETrue;
	
	return iStores[iIndex]->iServerItemCount;
	}

// ---------------------------------------------------------
// CNSmlDSContent::MapItemList()
// 
// ---------------------------------------------------------
SmlMapItemList_t* CNSmlDSContent::MapItemListL() const
	{
	return iStores[iIndex]->iMapContainer->MapItemListL();
	}

// ---------------------------------------------------------
// CNSmlDSContent::MaxObjSizeInServer()
// 
// ---------------------------------------------------------
TInt CNSmlDSContent::MaxObjSizeInServer() const
	{
	return iStores[iIndex]->iMaxObjSizeInServer;
	}
	
// ---------------------------------------------------------
// CNSmlDSContent::LastSyncAnchorL()
// 
// ---------------------------------------------------------
TTime CNSmlDSContent::LastSyncAnchorL() const
	{
	return iStores[iIndex]->iAgentLog->TimeValueL( EAgentLogLastSyncAnchor );
	}

// ---------------------------------------------------------
// CNSmlDSContent::GetSavedFilterCapsL()
// 
// ---------------------------------------------------------
void CNSmlDSContent::GetSavedFilterCapsL( CArrayFix<TNSmlFilterCapData>* aFilterCaps ) const
	{
	iStores[iIndex]->iAgentLog->GetServerFilterInfoL( aFilterCaps );
	}

// ---------------------------------------------------------
// CNSmlDSContent::InitInterruptionFlags()
// 
// ---------------------------------------------------------
void CNSmlDSContent::InitInterruptionFlags() const
	{
	for ( TInt i = 0; i < iStores.Count(); i++ )
		{
		if ( !iStores[i]->iInterruptedBeforeSync )
			{
			iStores[i]->iInterrupted = EFalse;
			iStores[i]->iImmediatelyInterruption = EFalse;
			iStores[i]->iStatusErrorInSyncLog = EFalse;
			}
		}
	}
	
// ---------------------------------------------------------
// CNSmlDSContent::CreateNewMapItemL() 
// 
// ---------------------------------------------------------
void CNSmlDSContent::CreateNewMapItemL( const TSmlDbItemUid aLUID, const TDesC8& aGUID, const TInt aAtomicId ) const
	{
	if ( !iStores[iIndex]->iMapContainer )
		{
		iStores[iIndex]->iMapContainer = CNSmlDSMapContainer::NewL();
		}

	iStores[iIndex]->iMapContainer->CreateNewMapItemL( aLUID, aGUID, aAtomicId );
	}
	
// ---------------------------------------------------------
// CNSmlDSContent::SetMapItemList()
// 
// ---------------------------------------------------------
void CNSmlDSContent::SetMapItemList( SmlMapItemList_t* aMapItemList ) const
	{
	iStores[iIndex]->iMapContainer->SetMapItemList( aMapItemList );
	}
	
// ---------------------------------------------------------
// CNSmlDSContent::RemoveMap()
// 
// ---------------------------------------------------------
void CNSmlDSContent::RemoveMap() const
	{
	iStores[iIndex]->iMapContainer->MarkAllItemsSent();
	}

//RD_SUSPEND_RESUME
// ---------------------------------------------------------
// CNSmlDSContent::SetSyncTypeL()
// 
// ---------------------------------------------------------
void CNSmlDSContent::SetSyncTypeL( const TDesC8& aSyncType ) const
	{
	delete iStores[iIndex]->iSyncType;
	iStores[iIndex]->iSyncType = NULL;
	iStores[iIndex]->iSyncType = aSyncType.AllocL();
	}

//RD_SUSPEND_RESUME
// ---------------------------------------------------------
// CNSmlDSContent::SetClientSyncTypeL()
// 
// ---------------------------------------------------------
void CNSmlDSContent::SetClientSyncTypeL( const TDesC8& aSyncType ) const
{
	delete iStores[iIndex]->iClientSyncType;
	iStores[iIndex]->iClientSyncType = NULL;
	iStores[iIndex]->iClientSyncType = aSyncType.AllocL();
}
// CNSmlDSContent::SetSyncTypeL()
// 
// ---------------------------------------------------------
void CNSmlDSContent::SetSyncTypeL( CNSmlDbCaps::ENSmlSyncTypes aSyncType ) const
	{
	TBuf8<3> syncType;
	
	switch ( aSyncType )
		{
		case CNSmlDbCaps::ETwoWaySync:
			syncType = KNSmlDSTwoWay;
			break;
			
		case CNSmlDbCaps::ESlowTwoWaySync:
			syncType = KNSmlDSSlowSync;
			break;
			
		case CNSmlDbCaps::EOneWaySyncFromClient:
			syncType = KNSmlDSOneWayFromClient;
			break;
			
		case CNSmlDbCaps::ERefreshSyncFromClient:
			syncType = KNSmlDSRefreshFromClient;
			break;
			
		case CNSmlDbCaps::EOneWaySyncFromServer:
			syncType = KNSmlDSOneWayFromServer;
			break;
			
		case CNSmlDbCaps::ERefreshSyncFromServer:
			syncType = KNSmlDSRefreshFromServer;
			break;
			
		default:
			break;
		}

	SetSyncTypeL( syncType );
	SetClientSyncTypeL( syncType );//RD_SUSPEND_RESUME
	}

// ---------------------------------------------------------
// CNSmlDSContent::SetDbAndTimeInitsL()
// 
// ---------------------------------------------------------
void CNSmlDSContent::SetDbAndTimeInitsL( TBool& aEndOfInits, TInt& aResultCode, TBool aStartOfInits )
	{
	DBG_FILE(_S8("CNSmlDSContent:::SetDbAndTimeInitsL begins"));

	if ( !aStartOfInits )
		{
		aEndOfInits = !SetToFirst();
		}
	else
		{
		aEndOfInits = !SetToNext();
		}
		
	if ( !aEndOfInits )
		{
		if ( !Interrupted() )
			{
			if ( *iStores[iIndex]->iSyncType == KNSmlDSSlowSync || 
				 *iStores[iIndex]->iSyncType == KNSmlDSRefreshFromClient ||
				 *iStores[iIndex]->iSyncType == KNSmlDSRefreshFromClientByServer )
				{
				iHostClient->ResetChangeInfoL( iStores[iIndex]->iImplementationUID, *iStores[iIndex]->iStoreName, aResultCode );
// <MAPINFO_RESEND_MOD_BEGIN>
			//	CleanMapInfo();
// <MAPINFO_RESEND_MOD_END>
				iStores[iIndex]->iLUIDBuffer->FetchModificationsL( aResultCode );
				}
			if ( *iStores[iIndex]->iSyncType == KNSmlDSOneWayFromServer ||
				 *iStores[iIndex]->iSyncType == KNSmlDSOneWayFromServerByServer || 
				 *iStores[iIndex]->iSyncType == KNSmlDSRefreshFromServer || 
			  	 *iStores[iIndex]->iSyncType == KNSmlDSRefreshFromServerByServer )
				{
				iStores[iIndex]->iLUIDBuffer->FetchModificationsL( aResultCode );
				}
			if ( *iStores[iIndex]->iSyncType == KNSmlDSRefreshFromServer || 
				 *iStores[iIndex]->iSyncType == KNSmlDSRefreshFromServerByServer )
				{
				iHostClient->DeleteAllItemsL( iStores[iIndex]->iImplementationUID, *iStores[iIndex]->iStoreName, aResultCode );
				User::LeaveIfError(aResultCode);
				iHostClient->ResetChangeInfoL( iStores[iIndex]->iImplementationUID, *iStores[iIndex]->iStoreName, aResultCode );
// <MAPINFO_RESEND_MOD_BEGIN>

				DBG_FILE(_S8("CNSmlDSContent:::SetDbAndTimeInitsL - SyncType was KNSmlDSRefreshFromServer || KNSmlDSRefreshFromServerByServer"));
			//	CleanMapInfo();
// <MAPINFO_RESEND_MOD_END>
				}	
			if ( *iStores[iIndex]->iSyncType == KNSmlDSTwoWay ||
				 *iStores[iIndex]->iSyncType == KNSmlDSTwoWayByServer ||
				 *iStores[iIndex]->iSyncType == KNSmlDSOneWayFromClient ||
			 	 *iStores[iIndex]->iSyncType == KNSmlDSOneWayFromClientByServer )
				{
				iStores[iIndex]->iLUIDBuffer->FetchModificationsL( aResultCode );
				}
			}
		}

	DBG_FILE(_S8("CNSmlDSContent:::SetDbAndTimeInitsL ends"));
	}

// ---------------------------------------------------------
// CNSmlDSContent::InitSyncLogEventL()
// 
// ---------------------------------------------------------
void CNSmlDSContent::InitSyncLogEventL( CSyncMLHistoryJob& aSyncLog )
	{
	InitEventL( aSyncLog, iIndex );
	}

// ---------------------------------------------------------
// CNSmlDSContent::InitAllSyncLogEventsL()
// 
// ---------------------------------------------------------
void CNSmlDSContent::InitAllSyncLogEventsL( CSyncMLHistoryJob& aSyncLog )
	{
	for ( TInt i = 0; i < iStores.Count(); i++ )
		{
		InitEventL( aSyncLog, i );
		}
	}

// ---------------------------------------------------------
// CNSmlDSContent::FinalizeSyncLogEvents()
// 
// ---------------------------------------------------------
void CNSmlDSContent::FinalizeSyncLogEvents() const
	{
	for ( TInt i = 0; i < iStores.Count(); i++ )
		{
		if ( iStores[i]->iTaskInfo )
			{
			if ( iStores[i]->iSyncStatus == CNSmlDSAgent::ECompletedOK && iStores[i]->iWarnings > 0 )
				{
				iStores[i]->iSyncStatus = CNSmlDSAgent::ECompletedWithInfo;
				}
			
			
			iStores[i]->iTaskInfo->iError = iStores[i]->iSyncStatus;
			
			TSmlSyncType syncType = ESmlTwoWay; // set a value to avoid compiler warning
			TPtr8 dsSyncType = iStores[i]->iSyncType->Des();
			
			if ( dsSyncType == KNSmlDSTwoWay || dsSyncType == KNSmlDSTwoWayByServer )
				{
				syncType = ESmlTwoWay;
				}
			else 
			if ( dsSyncType == KNSmlDSSlowSync )
				{
				syncType = ESmlSlowSync;
				}
			else
			if ( dsSyncType == KNSmlDSOneWayFromClient || dsSyncType == KNSmlDSOneWayFromClientByServer )
				{
				syncType = ESmlOneWayFromClient;
				}
			else
			if ( dsSyncType == KNSmlDSRefreshFromClient || dsSyncType == KNSmlDSRefreshFromClientByServer )
				{
				syncType = ESmlRefreshFromClient;
				}
			else
			if ( dsSyncType == KNSmlDSOneWayFromServer ||  dsSyncType == KNSmlDSOneWayFromServerByServer )
				{
				syncType = ESmlOneWayFromServer;
				}
			else
			if ( dsSyncType == KNSmlDSRefreshFromServer || dsSyncType == KNSmlDSRefreshFromServerByServer )
				{
				syncType = ESmlRefreshFromServer;
				}
			
			iStores[i]->iTaskInfo->iSyncType = syncType;
			}
		}
	}

// -----------------------------------------------------------------------------
// CNSmlDSContent::AddNewContentL
// Adds a new datastore to the datastore list.
// -----------------------------------------------------------------------------
//
void CNSmlDSContent::AddNewContentL() 
	{
	CNSmlDSContentItem* contentItem = new ( ELeave ) CNSmlDSContentItem;
	
	CleanupStack::PushL( contentItem );
	User::LeaveIfError( iStores.Append( contentItem ) );
	CleanupStack::Pop(); // contentItem
	
	iIndex = iStores.Count() - 1;
	iStores[iIndex]->iSyncStatus = CNSmlDSAgent::ENothingIsSent;
	}
	
// -----------------------------------------------------------------------------
// CNSmlDSContent::SetImplementationUID
// Sets the current datastore's implementation UID.
// -----------------------------------------------------------------------------
//
void CNSmlDSContent::SetImplementationUID( TInt aImplUID ) const
	{
	iStores[iIndex]->iImplementationUID = aImplUID;
	}
	
// ---------------------------------------------------------
// CNSmlDSContent::SetLocalDatabaseL()
// 
// ---------------------------------------------------------
void CNSmlDSContent::SetLocalDatabaseL( CNSmlURI& aLocalDatabase ) const
	{
	
	if ( aLocalDatabase.Database() == KNullDesC() 
					|| ImplementationUID() == KUidNSmlAdapterEMail.iUid)
		{
		CNSmlDSHostClient* dsClient = CNSmlDSHostClient::NewL();
    	CleanupStack::PushL(dsClient);

    	RArray<TInt> idArray;
    	RArray<TInt> resultArray;
    	CleanupClosePushL(resultArray);
    	CleanupClosePushL(idArray);
    	idArray.AppendL(ImplementationUID() );
    
    	dsClient->CreateDataProvidersL( idArray, resultArray );

    	TInt res = resultArray[0];
    	User::LeaveIfError( res );

    	CleanupStack::PopAndDestroy(2);
    	TInt result(KErrNone);
    	
    	HBufC* localDatabase = dsClient->DefaultStoreL( ImplementationUID(), result );
    	User::LeaveIfNull( localDatabase );
    	
    	CleanupStack::PushL( localDatabase );
    	
    	if ( localDatabase->Des() == KNullDesC() )
    	    {
    	    User::Leave( KErrNotFound );
    	    }
    	
    	aLocalDatabase.SetDatabaseL( localDatabase->Des() );
    	CleanupStack::PopAndDestroy(); //localDatabase;
    	
    	CleanupStack::PopAndDestroy(); //dsClient    	
		}
	
	iStores[iIndex]->iLocalDatabase = CNSmlURI::NewL();
	*iStores[iIndex]->iLocalDatabase = aLocalDatabase;
	iStores[iIndex]->iStoreName = aLocalDatabase.DataBaseWithoutRelativePrefix().AllocL();
	}
	
// ---------------------------------------------------------
// CNSmlDSContent::SetRemoteDatabaseL()
// 
// ---------------------------------------------------------
void CNSmlDSContent::SetRemoteDatabaseL( const CNSmlURI& aRemoteDatabase ) const
	{
	iStores[iIndex]->iRemoteDatabase = CNSmlURI::NewL();
	*iStores[iIndex]->iRemoteDatabase = aRemoteDatabase;
	}

// -----------------------------------------------------------------------------
// CNSmlDSContent::InitContentInstanceL
// Initialises the current datastore.
// -----------------------------------------------------------------------------
//
TBool CNSmlDSContent::InitContentInstanceL( TDesC& aServerId )
	{
	TBool retval( ETrue );
	
	iStores[iIndex]->iAgentLog = CNSmlDSAgentLog::NewL();
	iStores[iIndex]->iAgentLog->SetAgentLogKeyL( iStores[iIndex]->iImplementationUID,
													iStores[iIndex]->iLocalDatabase->Database(),					
													aServerId,
													iStores[iIndex]->iRemoteDatabase->Database() );
	
	// old Server Id needs to be examined, if the server has sent a new id in the device info
	delete iOldServerId;
	iOldServerId = NULL;
	iOldServerId = aServerId.AllocL();
	iStores[iIndex]->iServerId = aServerId.AllocL();

	RArray<TSmlDataProviderId> dataProvider;
	RArray<TInt> result;
	
	dataProvider.Append( iStores[iIndex]->iImplementationUID );
	
	iHostClient->CreateDataProvidersL( dataProvider, result );

	if ( result[0] != KErrNone )
		{
		retval = EFalse;		
		}

	dataProvider.Close();
	result.Close();
	
	return retval;
	}

// ---------------------------------------------------------
// CNSmlDSContent::OpenContentInstanceL()
// 
// ---------------------------------------------------------
void CNSmlDSContent::OpenContentInstanceL( TInt& aResultCode ) const
	{
	DBG_FILE(_S8("CNSmlDSContent::OpenContentInstanceL begins"));

	iStores[iIndex]->iStoreOpened = ETrue;
	iHostClient->OpenL( iStores[iIndex]->iImplementationUID, *iStores[iIndex]->iStoreName, *iStores[iIndex]->iServerId, iStores[iIndex]->iRemoteDatabase->DataBaseWithoutRelativePrefix(), aResultCode );
	
	DBG_FILE(_S8("CNSmlDSContent::OpenContentInstanceL ends"));
	}

// -----------------------------------------------------------------------------
// CNSmlDSContent::CreateLUIDBufferL
// Creates a LUID buffer for the current datastore. 
// -----------------------------------------------------------------------------
//
void CNSmlDSContent::CreateLUIDBufferL( TInt& aResultCode ) const
	{
	DBG_FILE(_S8("CNSmlDSContent::CreateLUIDBufferL begins"));

	iStores[iIndex]->iLUIDBuffer = new ( ELeave ) CNSmlDSLUIDBuffer( *iHostClient, *iStores[iIndex] );
	iStores[iIndex]->iLUIDBuffer->FetchModificationsL( aResultCode );

	DBG_FILE(_S8("CNSmlDSContent::CreateLUIDBufferL ends"));
	}

// -----------------------------------------------------------------------------
// CNSmlDSContent::CancelRequestL
// Cancels all ongoing asynchronous operations.
// -----------------------------------------------------------------------------
//
void CNSmlDSContent::CancelRequestL() const
	{
	TInt resultCode;
	
	for ( TInt i = 0; i < iStores.Count(); i++ )
		{
		if ( iStores[i]->iStoreOpened )
			{
			iHostClient->CancelRequestL( iStores[i]->iImplementationUID, *iStores[i]->iStoreName, resultCode );
			}
		}
	}	

// ---------------------------------------------------------
// CNSmlDSContent::SetInitAlertWaiting() 
// 
// ---------------------------------------------------------
void CNSmlDSContent::SetInitAlertWaiting() const
	{
	iStores[iIndex]->iWaitingInitAlert = ETrue;
	}
	
// ---------------------------------------------------------
// CNSmlDSContent::SetInitAlertReceived()
// 
// ---------------------------------------------------------
void CNSmlDSContent::SetInitAlertReceived() const
	{
	iStores[iIndex]->iWaitingInitAlert = EFalse;
	}
	
// -----------------------------------------------------------------------------
// CNSmlDSContent::SetSlowSyncReqForNextSyncL
// Sets the next synchronisation type to slow sync. 
// -----------------------------------------------------------------------------
//
void CNSmlDSContent::SetSlowSyncReqForNextSyncL( TBool aRequest ) const
	{
	if ( aRequest )
		{	
		iStores[iIndex]->iAgentLog->SetIntValueL( EAgentLogSlowSyncRequest, 1 );
		}
	else
		{
		iStores[iIndex]->iAgentLog->SetIntValueL( EAgentLogSlowSyncRequest, 0 );
		}
	}
	
// -----------------------------------------------------------------------------
// CNSmlDSContent::SetRefreshRequiredL
// Sets if the Resume command is Rejected by the Server
// -----------------------------------------------------------------------------
//
void  CNSmlDSContent::SetRefreshRequiredL(TBool aRefreshRequired)
{
	iStores[iIndex]->iRefreshRequired=aRefreshRequired;
}
// -----------------------------------------------------------------------------
// CNSmlDSContent::GetRefreshRequired
// Gets if the Resume command is Rejected by the Server or not
// -----------------------------------------------------------------------------
//
TBool CNSmlDSContent::GetRefreshRequired()
{
	return(iStores[iIndex]->iRefreshRequired);
}

//RD_SUSPEND_RESUME
// -----------------------------------------------------------------------------
// CNSmlDSContent::SetSessionResumedL
// Sets if the Resume command is sent by the client
// -----------------------------------------------------------------------------
//
void  CNSmlDSContent::SetSessionResumedL(TBool aSessionResumed)
{
	iStores[iIndex]->iSessionResumed=aSessionResumed;
}

//RD_SUSPEND_RESUME
// ------------------------------------------------------------------------------------------------
// CNSmlDSContent::GetRefreshRequired
// Gets if the Resume command is sent by the client or not, ETrue is returned if session is resumed
// ------------------------------------------------------------------------------------------------
//
TBool CNSmlDSContent::GetSessionResumed()
{
	return(iStores[iIndex]->iSessionResumed);
}

// ---------------------------------------------------------
// CNSmlDSContent::SetIndexByLocalDatabaseL()
// 
// ---------------------------------------------------------
TBool CNSmlDSContent::SetIndexByLocalDatabaseL( CNSmlURI& aDatabase )
	{
	iIndex = -1;
	
	for ( TInt i = 0; i < iStores.Count(); i++ )
		{
		if ( aDatabase.IsEqualL( *iStores[i]->iLocalDatabase ) )
			{
			// Sync was bound to a database
			iIndex = i;
			return ETrue;
			}
		}
		
	return EFalse;
	}

// ---------------------------------------------------------
// CNSmlDSContent::SetIndexByRemoteDatabaseL()
// 
// ---------------------------------------------------------

TBool CNSmlDSContent::SetIndexByRemoteDatabaseL( CNSmlURI& aDatabase, TBool aRespURIExists )
	{
	iIndex = -1;
	
	for ( TInt i = 0; i < iStores.Count() ; i++ )
		{
		if ( aDatabase.IsEqualL( *iStores[i]->iRemoteDatabase , !aRespURIExists) )
			{
			// Sync was bound to a database
			iIndex = i;
			return ETrue;
			}
		else
			{
			if ( aRespURIExists )
				{
				if ( aDatabase.DatabaseMatchesL( iStores[i]->iRemoteDatabase->Database() ) )
					{
					// Sync was bound to a database
					iIndex = i;
					return ETrue;
					}
				}
			}
		}
		
	return EFalse;
	}

// ---------------------------------------------------------
// CNSmlDSContent::SetIndexByDatabasesAndTypeL()
// 
// ---------------------------------------------------------
TBool CNSmlDSContent::SetIndexByDatabasesAndTypeL( CNSmlURI& aRemoteDatabase, CNSmlURI& aLocalDatabase, const TDesC8& aType )
	{
	iIndex = -1;
	
	for ( TInt i = 0; i < iStores.Count(); i++ )
		{
		if ( ( aLocalDatabase.Database().Length() > 0 ) && ( aLocalDatabase.Database() != _L("/") ) )
			{
			if ( !aLocalDatabase.IsEqualL( *iStores[i]->iLocalDatabase ) )
				{
				continue;
				}
			}
			
		if ( !aRemoteDatabase.IsEqualL( *iStores[i]->iRemoteDatabase ) )
			{
			continue;
			}
			
		iIndex = i;
		
		if ( IsSupportedRxL( aType ) && IsSupportedTxL( aType ) )
			{
			return ETrue;
			}
		else
			{
			iIndex = -1;
			}
		}
		
	return EFalse;
	}
	
// -----------------------------------------------------------------------------
// CNSmlDSContent::SetAsServerAlerted
// Raises the server alert flag.
// -----------------------------------------------------------------------------
//
void CNSmlDSContent::SetAsServerAlerted() const
	{
	iStores[iIndex]->iServerAlerted = ETrue;
	}
	
// -----------------------------------------------------------------------------
// CNSmlDSContent::SetToFirst
// Sets the index to the first datastore.
// -----------------------------------------------------------------------------
TBool CNSmlDSContent::SetToFirst()
	{
	iIndex = 0;
	
	if ( iIndex < iStores.Count() )
		{
		return ETrue;
		}
		
	iIndex = -1;
	
	return EFalse;	
	}
	
// -----------------------------------------------------------------------------
// CNSmlDSContent::SetToNext
// Advances the index to the next datastore.
// -----------------------------------------------------------------------------
//
TBool CNSmlDSContent::SetToNext()
	{
	if ( ++iIndex < iStores.Count() )
		{
		return ETrue;
		}
		
	iIndex = -1;
	
	return EFalse;
	}
	
// -----------------------------------------------------------------------------
// CNSmlDSContent::SetIndex
// Sets the index to the given datastore.
// -----------------------------------------------------------------------------
//
TBool CNSmlDSContent::SetIndex( TInt aInd )
	{
	iIndex = aInd;
	
	if ( iIndex >= 0 && iIndex < iStores.Count() )
		{
		return ETrue;
		}
	
	iIndex = -1;
	
	return EFalse;
	}
	
// -----------------------------------------------------------------------------
// CNSmlDSContent::SaveCurrentIndex
// Saves the current index.
// -----------------------------------------------------------------------------
//
void CNSmlDSContent::SaveCurrentIndex() 
	{
	iSavedIndex = iIndex;
	}

// -----------------------------------------------------------------------------
// CNSmlDSContent::SetIndexToSaved
// Sets the index to the previously saved index.
// -----------------------------------------------------------------------------
//
void CNSmlDSContent::SetIndexToSaved() 
	{
	iIndex = iSavedIndex;
	}
	
// -----------------------------------------------------------------------------
// CNSmlDSContent::SetCurrentItemWritten
// Sets the current item's state in the LUID buffer processed.
// -----------------------------------------------------------------------------
//
void CNSmlDSContent::SetCurrentItemWritten() const
	{
	iStores[iIndex]->iLUIDBuffer->SetCurrentItemWritten();
	}

// ---------------------------------------------------------
// CNSmlDSContent::SetInterrupted()
// 
// ---------------------------------------------------------
void CNSmlDSContent::SetInterrupted() 
	{
	iStores[iIndex]->iInterrupted = ETrue;
	}
	
// ---------------------------------------------------------
// CNSmlDSContent::SetImmediatelyInterrupted()
// 
// ---------------------------------------------------------
void CNSmlDSContent::SetImmediatelyInterrupted() 
	{
	iStores[iIndex]->iImmediatelyInterruption = ETrue;
	}
	
// ---------------------------------------------------------
// CNSmlDSContent::SetServerStatusError()
// 
// ---------------------------------------------------------
void CNSmlDSContent::SetServerStatusError() 
	{
	iStores[iIndex]->iStatusErrorInSyncLog = ETrue;
	}
	
// ---------------------------------------------------------
// CNSmlDSContent::SetInterruptedBeforeSync()
// 
// ---------------------------------------------------------
void CNSmlDSContent::SetInterruptedBeforeSync() const
	{
	iStores[iIndex]->iInterruptedBeforeSync = ETrue;
	}

// ---------------------------------------------------------
// CNSmlDSContent::UpdateInterruptedFlags()
// 
// ---------------------------------------------------------
void CNSmlDSContent::UpdateInterruptedFlags()
	{
	for ( TInt i = 0; i < iStores.Count(); i++ )
		{
		if ( !iStores[i]->iInterrupted )
			{
			iStores[i]->iImmediatelyInterruption = EFalse;
			}
		}
	}
	
// ---------------------------------------------------------
// CNSmlDSContent::SetSyncStatus()
// 
// ---------------------------------------------------------
void CNSmlDSContent::SetSyncStatus( CNSmlDSAgent::TNSmlAgentSyncStatus aSyncStatus ) const
	{
	iStores[iIndex]->iSyncStatus = aSyncStatus;
	}
	
// ---------------------------------------------------------
// CNSmlDSContent::SetSyncStatusAll()
// 
// ---------------------------------------------------------
void CNSmlDSContent::SetSyncStatusToAll( CNSmlDSAgent::TNSmlAgentSyncStatus aSyncStatus ) const
	{
	for ( TInt i = 0; i < iStores.Count(); i++ )
		{
		if( !( iStores[i]->iSyncStatus == CNSmlDSAgent::EServerDataPartiallyUpdated && aSyncStatus == CNSmlDSAgent::ENoServerDataUpdated ) )
			{
			iStores[i]->iSyncStatus = aSyncStatus;
			}
		}
	}

// ---------------------------------------------------------
// CNSmlDSContent::SetTypeAndDevCapsL()
// 
// ---------------------------------------------------------
TInt CNSmlDSContent::SetTypeAndDevCapsL() const
	{
	TInt resultCode( KErrNotFound );
	
	for ( TInt i = 0; i < iStores.Count(); i++ )
		{
		if ( iStores[i]->iStoreOpened )
			{
			CNSmlDbCaps* dbCaps( NULL );
			GetDevCapsFromAgentLogL( i, dbCaps );
			if ( dbCaps )
				{
				CleanupStack::PushL( dbCaps );
				iHostClient->SetRemoteDataStoreFormatL( *dbCaps, iStores[i]->iImplementationUID, *iStores[i]->iStoreName, resultCode );
				CleanupStack::PopAndDestroy(); // dbCaps
				}
			}
		}
		
	return resultCode;
	}
	
// ---------------------------------------------------------
// CNSmlDSContent::CheckValidRemoteDb()
// Checks if the remote database is same as source ref
// ---------------------------------------------------------
TBool CNSmlDSContent::CheckValidRemoteDbL(RPointerArray<CNSmlURI>& aSourceRefArr, TBool aResp) const
	{
	TBool dbFound;
	for ( TInt i = 0; i < iStores.Count(); i++ )
	{
	    dbFound = EFalse;
		for ( TInt j = 0; j < aSourceRefArr.Count(); j++ )
		{
		if ( (aSourceRefArr[j])->IsEqualL( *iStores[i]->iRemoteDatabase,!aResp) )
			{
				dbFound = ETrue;
				continue;			
			}
		else
			{
			if ( aResp )
				{
				if ( (aSourceRefArr[j])->DatabaseMatchesL( iStores[i]->iRemoteDatabase->Database() ) )
					{
						dbFound = ETrue;
						continue;
					}
				}
			}
		}
		
		if( !dbFound )
		{
			return EFalse;
		}
	}
	return ETrue;
	}
// ---------------------------------------------------------
// CNSmlDSContent::SetSavedPartnerDevCapsL()
// 
// ---------------------------------------------------------
TInt CNSmlDSContent::SetSavedPartnerDevCapsL() 
	{
	TInt resultCode( KErrNone );
	
	CNSmlDbCaps* dbCaps;
	GetDevCapsFromAgentLogL( iIndex, dbCaps );
	if ( dbCaps )
		{
		CleanupStack::PushL( dbCaps );
		iHostClient->SetRemoteDataStoreFormatL( *dbCaps, iStores[iIndex]->iImplementationUID, *iStores[iIndex]->iStoreName, resultCode );
		CleanupStack::PopAndDestroy(); // dbCaps
		}
		
	return resultCode;
	}
	
// ---------------------------------------------------------
// CNSmlDSContent::SetClientItemCountAsked()
// 
// ---------------------------------------------------------
void CNSmlDSContent::SetClientItemCountAsked() const
	{
	iStores[iIndex]->iClientItemCountAsked = ETrue;
	}
	
// ---------------------------------------------------------
// CNSmlDSContent::SetServerItemCount()
// 
// ---------------------------------------------------------
void CNSmlDSContent::SetServerItemCount( TInt aCount ) const
	{
	iStores[iIndex]->iServerItemCount = aCount;
	}
	
// ---------------------------------------------------------
// CNSmlDSContent::SetMaxObjSizeInServer()
// 
// ---------------------------------------------------------
void CNSmlDSContent::SetMaxObjSizeInServer( TInt aSize ) const
	{
	iStores[iIndex]->iMaxObjSizeInServer = aSize;
	}
	
// ---------------------------------------------------------
// CNSmlDSContent::MaxObjSizeL()
// 
// ---------------------------------------------------------
TInt CNSmlDSContent::MaxObjSizeL() const
	{
	TInt resultCode( KErrNone );
	TInt value( KNSmlMaxObjSize );
	value = iHostClient->MaxObjectSizeL( iStores[iIndex]->iImplementationUID, *iStores[iIndex]->iStoreName, resultCode );
	if ( value == 0 )
	    {
	    value = KNSmlMaxObjSize;
	    }
	return value;
	}
	
// -----------------------------------------------------------------------------
// CNSmlDSContent::SetAsReceivedL
// Signals the current datastore that the given item has been received by the
// server.
// -----------------------------------------------------------------------------
//
void CNSmlDSContent::SetAsReceivedL( TSmlDbItemUid aLUID ) const
	{
	TInt resultCode;
	CNSmlDataItemUidSet* uidSet = new ( ELeave ) CNSmlDataItemUidSet;
	CleanupStack::PushL( uidSet );
	uidSet->AddItem( aLUID );
	iHostClient->CommitChangeInfoL( *uidSet, iStores[iIndex]->iImplementationUID, *iStores[iIndex]->iStoreName, resultCode );
	CleanupStack::PopAndDestroy( uidSet );
	}

// -----------------------------------------------------------------------------
// CNSmlDSContent::BeginBatchL
// Sets the current datastore to batch mode.
// -----------------------------------------------------------------------------
//
void CNSmlDSContent::BeginBatchL()
	{
	TInt resultCode;
	iStores[iIndex]->iBatchModeOn = ETrue;
	iHostClient->BeginBatchL( iStores[iIndex]->iImplementationUID, *iStores[iIndex]->iStoreName, resultCode );
	}
	
// -----------------------------------------------------------------------------
// CNSmlDSContent::CancelBatchL
// Cancels the ongoing batch mode.
// -----------------------------------------------------------------------------
//
void CNSmlDSContent::CancelBatchL()
	{
	TInt resultCode;
	iStores[iIndex]->iBatchModeOn = EFalse;
	iHostClient->CancelBatchL( iStores[iIndex]->iImplementationUID, *iStores[iIndex]->iStoreName, resultCode );
	}
	
// -----------------------------------------------------------------------------
// CNSmlDSContent::CommitBatchL
// Commits all operations done during the batch mode. Batch mode ends after this
// call is completed.
// -----------------------------------------------------------------------------
//
void CNSmlDSContent::CommitBatchL( RArray<TInt>& aResultArray, TInt& aResultCode )
	{
	iStores[iIndex]->iBatchModeOn = EFalse;
	iHostClient->CommitBatchL( aResultArray, iStores[iIndex]->iImplementationUID, *iStores[iIndex]->iStoreName, aResultCode );	
	}

// -----------------------------------------------------------------------------
// CNSmlDSContent::BatchModeOn
// Checks if the current datastore is in the batch mode.
// -----------------------------------------------------------------------------
//
TBool CNSmlDSContent::BatchModeOn() const
	{
	return iStores[iIndex]->iBatchModeOn;
	}
	
// -----------------------------------------------------------------------------
// CNSmlDSContent::CreateItemL
// Creates new item to the current datastore.
// -----------------------------------------------------------------------------
//
TInt CNSmlDSContent::CreateItemL( TSmlDbItemUid& aUid, TInt aSize, const TPtrC8 aMimeType, const TPtrC8 aMimeVersion, TSmlDbItemUid aParent ) const
	{
	TInt resultCode;
	
	iHostClient->CreateItemL( aUid,
							  aSize,
							  aParent,
							  aMimeType,
							  aMimeVersion,
							  iStores[iIndex]->iImplementationUID,
							  *iStores[iIndex]->iStoreName,
							  resultCode );
							  
	return resultCode;
	}
		
// -----------------------------------------------------------------------------
// CNSmlDSContent::DeleteItemL
// Deletes an item from the current datastore.
// -----------------------------------------------------------------------------
//
TInt CNSmlDSContent::DeleteItemL( const TSmlDbItemUid aUid, TBool aSoftDelete ) const
	{
	TInt resultCode;
	
	if ( aSoftDelete )	
		{
		iHostClient->SoftDeleteItemL( aUid,
									  iStores[iIndex]->iImplementationUID,
									  *iStores[iIndex]->iStoreName, 
									  resultCode );
		}
	else
		{
		iHostClient->DeleteItemL( aUid, 
								  iStores[iIndex]->iImplementationUID,
								  *iStores[iIndex]->iStoreName, 
								  resultCode );
		}
		
	return resultCode;
	}
	
// -----------------------------------------------------------------------------
// CNSmlDSContent::MoveItemL
// Moves an item in the current datastore. 
// -----------------------------------------------------------------------------
//
TInt CNSmlDSContent::MoveItemL( const TSmlDbItemUid aUid, const TSmlDbItemUid aParent ) const
	{
	TInt resultCode;
	
	iHostClient->MoveItemL( aUid, 
							aParent,
							iStores[iIndex]->iImplementationUID, 
							*iStores[iIndex]->iStoreName, 
							resultCode );

	return resultCode;
	}

// -----------------------------------------------------------------------------
// CNSmlDSContent::CommitItemL
// Commits the currently opened item.
// -----------------------------------------------------------------------------
//
TInt CNSmlDSContent::CommitItemL()
	{
	TInt resultCode;
	
	iHostClient->CommitItemL( iStores[iIndex]->iImplementationUID,
							  *iStores[iIndex]->iStoreName, 
							  resultCode );
							  
	return resultCode;
	}
		
// -----------------------------------------------------------------------------
// CNSmlDSContent::WriteItemL
// Writes data to the currently opened item.
// -----------------------------------------------------------------------------
//
void CNSmlDSContent::WriteItemL( const TDesC8& aData )
	{
	TInt resultCode;
	
	iHostClient->WriteItemL( aData,
							 iStores[iIndex]->iImplementationUID,
							 *iStores[iIndex]->iStoreName, resultCode );
	}

// -----------------------------------------------------------------------------
// CNSmlDSContent::ReplaceItemL
// Replaces an item in the current datastore with a new one.
// -----------------------------------------------------------------------------
//
TInt CNSmlDSContent::ReplaceItemL( const TSmlDbItemUid aUid, TInt aSize, TSmlDbItemUid aParent, TBool aPartialUpdate ) const
	{
	TInt resultCode;
	
	iHostClient->ReplaceItemL( aUid, aSize, aParent, aPartialUpdate, iStores[iIndex]->iImplementationUID, *iStores[iIndex]->iStoreName, resultCode );
							  
	return resultCode;
	}
	
// -----------------------------------------------------------------------------
// CNSmlDSContent::GetFilterL
// Gets filters from adapter. 
// -----------------------------------------------------------------------------
//
TInt CNSmlDSContent::GetFilterL( CNSmlFilter*& aFilter )
	{
    TInt resultCode( KErrNone );
    
    iHostClient->GetFilterL( iStores[iIndex]->iImplementationUID, *iStores[iIndex]->iStoreName, iStores[iIndex]->iFilters, aFilter, iStores[iIndex]->iFilterMatchType, resultCode );
    
    return resultCode;
	}

// -----------------------------------------------------------------------------
// CNSmlDSContent::IncreaseWarnings
// 
// -----------------------------------------------------------------------------
//
void CNSmlDSContent::IncreaseWarnings() const
	{
	++iStores[iIndex]->iWarnings;
	}
	
// -----------------------------------------------------------------------------
// CNSmlDSContent::UpdateSyncTypeL
// 
// -----------------------------------------------------------------------------
//
void CNSmlDSContent::UpdateSyncTypeL( const CArrayFix<HBufC8*>& aSyncTypeArray ) const
	{
	TInt syncTypeFlags( 0 );
	
	for ( TInt i = 0; i < aSyncTypeArray.Count() ; i++ )
		{
		TLex8 lexicalValue( *aSyncTypeArray[i] );
		TInt syncTypeInt;
		
		if ( lexicalValue.Val( syncTypeInt ) == KErrNone )
			{
			switch ( syncTypeInt )
				{
				case CNSmlDbCaps::ETwoWaySync:
					syncTypeFlags += KNSmlTwoWayFlag;
					break;
					
				case CNSmlDbCaps::ESlowTwoWaySync:
					syncTypeFlags += KNSmlSlowTwoWayFlag;
					break;
					
				case CNSmlDbCaps::EOneWaySyncFromClient:
					syncTypeFlags += KNSmlOneWayFromClientFlag;
					break;
					
				case CNSmlDbCaps::ERefreshSyncFromClient:
					syncTypeFlags += KNSmlRefreshFromClientFlag;
					break;
					
				case CNSmlDbCaps::EOneWaySyncFromServer:
					syncTypeFlags += KNSmlOneWayFromServerFlag;
					break;
					
				case CNSmlDbCaps::ERefreshSyncFromServer:
					syncTypeFlags += KNSmlRefreshFromServerFlag;
					break;
					
				case CNSmlDbCaps::EServerAlertedSync:
					syncTypeFlags += KNSmlServerAlertedFlag;
					break;
					
				default:
					User::Leave( KErrGeneral );
				}
			}
		}
		
	iStores[iIndex]->iAgentLog->SetIntValueL( EAgentLogServerSyncTypes, syncTypeFlags );
	}

// ---------------------------------------------------------
// CNSmlDSContent::UpdateSupportHierarchicalSyncL()
// 
// ---------------------------------------------------------
void CNSmlDSContent::UpdateSupportHierarchicalSyncL( TInt aSupport ) const
	{
	iStores[iIndex]->iAgentLog->SetIntValueL( EAgentLogServerSupportsHierarchicalSync, aSupport );
	}

// ---------------------------------------------------------
// CNSmlDSContent::UpdateCtCapsL()
// 
// ---------------------------------------------------------
void CNSmlDSContent::UpdateCtCapsL( const RPointerArray<CNSmlDbCaps>& aDbCaps ) const
	{
	RPointerArray<CNSmlDbCaps> dbCapArray;
	CleanupStack::PushL( TCleanupItem( DeleteRPointerArray, &dbCapArray ) );
	
	dbCapArray.AppendL( aDbCaps[aDbCaps.Count() - 1] );
	
	iStores[iIndex]->iAgentLog->SetServerDeviceInfoL( dbCapArray );

	dbCapArray.Reset();
	
	CleanupStack::Pop(); // dbCapArray
	}

// ---------------------------------------------------------
// CNSmlDSContent::UpdateFilterCapsL()
// 
// ---------------------------------------------------------
void CNSmlDSContent::UpdateFilterCapsL( const CArrayFix<TNSmlFilterCapData>* aFilterCaps ) const
	{
	iStores[iIndex]->iAgentLog->SetServerFilterInfoL( aFilterCaps );
	}

// ---------------------------------------------------------
// CNSmlDSContent::RemoveCtCapsL
// Empty CTCaps for initialising in Agent log
// ---------------------------------------------------------
void CNSmlDSContent::RemoveCtCapsL() const
	{
	RPointerArray<CNSmlDbCaps> emptyArray;

	iStores[iIndex]->iAgentLog->SetServerDeviceInfoL( emptyArray );
	emptyArray.ResetAndDestroy();
	emptyArray.Close();
	}

// ---------------------------------------------------------
// CNSmlDSContent::RemoveFilterCapsL
// Empty FilterCaps for initialising in Agent log
// ---------------------------------------------------------
void CNSmlDSContent::RemoveFilterCapsL() const
	{
	CArrayFix<TNSmlFilterCapData>* emptyArray = new( ELeave ) CArrayFixFlat<TNSmlFilterCapData>( 1 );	
	CleanupStack::PushL( emptyArray );	
	iStores[iIndex]->iAgentLog->SetServerFilterInfoL( emptyArray );
	CleanupStack::PopAndDestroy(); // emptyArray
	}

// -----------------------------------------------------------------------------
// CNSmlDSContent::FinalizeDatabasesL
// Makes final modifications to all opened datastores.
// -----------------------------------------------------------------------------
//
void CNSmlDSContent::FinalizeDatabasesL() const
	{
	for ( TInt i = 0; i < iStores.Count(); i++ )
		{
		if ( iStores[i]->iStoreOpened )
			{
			if ( iStores[i]->iAgentLog->IntValueL( EAgentLogSlowSyncRequest ) == 1 )
				{
				iStores[i]->iAgentLog->SetIntValueL( EAgentLogSlowSyncRequest, 0 );
				}
			}
			
			iStores[i]->iStoreOpened = EFalse;
		}
	}

// ---------------------------------------------------------
// CNSmlDSContent::UpdateSyncAnchorsL()
// Updates Client's Sync Anchors in Agent Log
// ---------------------------------------------------------
void CNSmlDSContent::UpdateSyncAnchorsL( const TTime& aAnchor ) const
	{
	for ( TInt i = 0; i < iStores.Count(); i++ )
		{
		if ( !iStores[i]->iInterrupted )
			{
		    iStores[i]->iAgentLog->SetTimeValueL( EAgentLogLastSyncAnchor, aAnchor );
			}
		}
	}

// -----------------------------------------------------------------------------
// CNSmlDSContent::RemoveContent
// Removes the current content (datastore) from the content array. 
// -----------------------------------------------------------------------------
//
void CNSmlDSContent::RemoveContent()
	{
	delete iStores[iIndex];
	iStores.Remove( iIndex );
	iStores.Compress();
	}

// -----------------------------------------------------------------------------
// CNSmlDSContent::RemoveNotServerAlertedContents
// Removes non-server alerted contents from the content array.
// -----------------------------------------------------------------------------
//
void CNSmlDSContent::RemoveNotServerAlertedContents()
	{
	TInt i( 0 )	;
	
	while ( i < iStores.Count() )
		{
		if ( !iStores[i]->iServerAlerted )
			{
			delete iStores[i];
			iStores.Remove( i );
			iStores.Compress();			
			}
		else
			{
			++i;
			}
		}
	}
	
// ---------------------------------------------------------
// CNSmlDSContent::GetDevCapsFromAgentLogL()
// Supported fields by the server are read from Agent Log
// ---------------------------------------------------------
void CNSmlDSContent::GetDevCapsFromAgentLogL( TInt aInd, CNSmlDbCaps*& aDbCaps ) const
	{
	RPointerArray<CNSmlDbCaps> dbCaps;
	aDbCaps = NULL;
	
	iStores[aInd]->iAgentLog->GetServerDeviceInfoL( dbCaps );
	
	if ( dbCaps.Count() == 1 )
		{
		aDbCaps = dbCaps[0];
		dbCaps.Remove( 0 );
		TInt syncTypeFlags = iStores[aInd]->iAgentLog->IntValueL( EAgentLogServerSyncTypes );
    	if ( syncTypeFlags & KNSmlTwoWayFlag)
    	    {
    	    aDbCaps->SetSyncType( CNSmlDbCaps::ETwoWaySync );
    	    }
    	if ( syncTypeFlags & KNSmlSlowTwoWayFlag)
    	    {
    	    aDbCaps->SetSyncType( CNSmlDbCaps::ESlowTwoWaySync );
    	    }
    	if ( syncTypeFlags & KNSmlOneWayFromClientFlag)
    	    {
    	    aDbCaps->SetSyncType( CNSmlDbCaps::EOneWaySyncFromClient );
    	    }
    	if ( syncTypeFlags & KNSmlRefreshFromClientFlag)
    	    {
    	    aDbCaps->SetSyncType( CNSmlDbCaps::ERefreshSyncFromClient );
    	    }
    	if ( syncTypeFlags & KNSmlOneWayFromServerFlag)
    	    {
    	    aDbCaps->SetSyncType( CNSmlDbCaps::EOneWaySyncFromServer );
    	    }
    	if ( syncTypeFlags & KNSmlRefreshFromServerFlag)
    	    {
    	    aDbCaps->SetSyncType( CNSmlDbCaps::ERefreshSyncFromServer );
    	    }
    	if ( syncTypeFlags & KNSmlServerAlertedFlag)
    	    {
    	    aDbCaps->SetSyncType( CNSmlDbCaps::EServerAlertedSync );
    	    }
		}
	dbCaps.ResetAndDestroy();
	}

// ---------------------------------------------------------
// CNSmlDSContent::SyncTypeConv
// 
// ---------------------------------------------------------
TBool CNSmlDSContent::SyncTypeConv( const TDesC8& aSyncType, CNSmlDbCaps::ENSmlSyncTypes& aDbCapsSyncType ) const 
	{
	if ( aSyncType == KNSmlDSTwoWay || aSyncType == KNSmlDSTwoWayByServer )
		{
		aDbCapsSyncType = CNSmlDbCaps::ETwoWaySync;
		}
	else 
	if ( aSyncType == KNSmlDSSlowSync )
		{
		aDbCapsSyncType = CNSmlDbCaps::ESlowTwoWaySync;
		}
	else
	if ( aSyncType == KNSmlDSOneWayFromClient || aSyncType == KNSmlDSOneWayFromClientByServer )
		{
		aDbCapsSyncType = CNSmlDbCaps::EOneWaySyncFromClient;
		}
	else
	if ( aSyncType == KNSmlDSRefreshFromClient || aSyncType == KNSmlDSRefreshFromClientByServer )
		{
		aDbCapsSyncType = CNSmlDbCaps::ERefreshSyncFromClient;
		}
	else
	if ( aSyncType == KNSmlDSOneWayFromServer ||  aSyncType == KNSmlDSOneWayFromServerByServer )
		{
		aDbCapsSyncType = CNSmlDbCaps::EOneWaySyncFromServer;
		}
	else
	if ( aSyncType == KNSmlDSRefreshFromServer || aSyncType == KNSmlDSRefreshFromServerByServer )
		{
		aDbCapsSyncType = CNSmlDbCaps::ERefreshSyncFromServer;
		}
	else
		{
		return EFalse;
		}
		
	return ETrue;
	}

// -----------------------------------------------------------------------------
// CNSmlDSContent::IsFlagSet
// Checks if a bit is set in the given TUint.
// -----------------------------------------------------------------------------
//
TBool CNSmlDSContent::IsFlagSet( const TUint& aValue, const TUint& aBit ) const
	{
	return( ( aValue & aBit ) == aBit );
	}

// -----------------------------------------------------------------------------
// CNSmlDSContent::InitEventL
// Inits the history log task info.
// -----------------------------------------------------------------------------
//
void CNSmlDSContent::InitEventL( CSyncMLHistoryJob& aSyncLog, TInt aInd )
	{
	iStores[aInd]->iTaskInfo = &aSyncLog.TaskById( iStores[aInd]->iTaskId );
	iStores[aInd]->iTaskInfo->iItemsAdded = 0;
	iStores[aInd]->iTaskInfo->iItemsChanged = 0;
	iStores[aInd]->iTaskInfo->iItemsDeleted = 0;
	iStores[aInd]->iTaskInfo->iItemsMoved = 0;
	iStores[aInd]->iTaskInfo->iItemsFailed = 0;
	iStores[aInd]->iTaskInfo->iServerItemsAdded = 0;
	iStores[aInd]->iTaskInfo->iServerItemsChanged = 0;
	iStores[aInd]->iTaskInfo->iServerItemsDeleted = 0;
	iStores[aInd]->iTaskInfo->iServerItemsMoved = 0;
	iStores[aInd]->iTaskInfo->iServerItemsFailed = 0;
	}

// -----------------------------------------------------------------------------
// CNSmlDSContent::UpdateServerId
// Updates current synchronisation profile's server ID.
// -----------------------------------------------------------------------------
//
TInt CNSmlDSContent::UpdateServerIdL( TDesC& aServerId ) const
	{
	TInt status = KErrNone;
	
	if (iOldServerId->Size() == 0 )
		{
		if ( *iOldServerId != aServerId )
			{
			TInt resultCode;
			iStores[0]->iAgentLog->SetServerIdL( *iOldServerId, aServerId );
			iHostClient->UpdateServerIdL( *iOldServerId, aServerId, resultCode );
			}
		}
	else if ( *iOldServerId != aServerId )
		{
		status = TNSmlDSError::ESmlDSServerIdNotMatching;
		}
		
	return status;
	}

// -----------------------------------------------------------------------------
// CNSmlDSContent::RemoveFailedMappingsL
// Removes those mappings that were created under a failed atomic command.
// -----------------------------------------------------------------------------
//
void CNSmlDSContent::RemoveFailedMappingsL( const TInt aAtomicId )
	{
	if ( iStores[iIndex]->iMapContainer )
		{
		iStores[iIndex]->iMapContainer->RemoveFailedAtomics( aAtomicId );
		}
	}

// -----------------------------------------------------------------------------
// CNSmlDSContent::TaskId
// Returns current content's task id.
// -----------------------------------------------------------------------------
//
TSmlTaskId CNSmlDSContent::TaskId() const
	{
	if ( !ContentIndexIsSet() )
		{
		return 0;
		}
		
	return iStores[iIndex]->iTaskId;
	}
	
// -----------------------------------------------------------------------------
// CNSmlDSContent::TaskId
// Sets current content's task id.
// -----------------------------------------------------------------------------
//
void CNSmlDSContent::TaskId( const TSmlTaskId aId )
	{
	iStores[iIndex]->iTaskId = aId;
	}
	
// -----------------------------------------------------------------------------
// CNSmlDSContent::IncreaseItemsAdded
// Increases the counter of added items.
// -----------------------------------------------------------------------------
//
void CNSmlDSContent::IncreaseItemsAdded() const
	{
	++iStores[iIndex]->iTaskInfo->iItemsAdded;
	}

// -----------------------------------------------------------------------------
// CNSmlDSContent::IncreaseItemsChanged
// Increases the counter of changed items.
// -----------------------------------------------------------------------------
//
void CNSmlDSContent::IncreaseItemsChanged() const
	{
	++iStores[iIndex]->iTaskInfo->iItemsChanged;
	}
	
// -----------------------------------------------------------------------------
// CNSmlDSContent::IncreaseItemsDeleted
// Increases the counter of deleted items.
// -----------------------------------------------------------------------------
//
void CNSmlDSContent::IncreaseItemsDeleted() const
	{
	++iStores[iIndex]->iTaskInfo->iItemsDeleted;
	}
	
// -----------------------------------------------------------------------------
// CNSmlDSContent::IncreaseItemsMoved
// Increases the counter of moved items.
// -----------------------------------------------------------------------------
//
void CNSmlDSContent::IncreaseItemsMoved() const
	{
	++iStores[iIndex]->iTaskInfo->iItemsMoved;
	}
	
// -----------------------------------------------------------------------------
// CNSmlDSContent::IncreaseItemsFailed
// Increases the counter of failed items.
// -----------------------------------------------------------------------------
//
void CNSmlDSContent::IncreaseItemsFailed() const
	{
	++iStores[iIndex]->iTaskInfo->iItemsFailed;
	}
	
// -----------------------------------------------------------------------------
// CNSmlDSContent::IncreaseServerItemsAdded
// Increases the counter of items added on the server.
// -----------------------------------------------------------------------------
//
void CNSmlDSContent::IncreaseServerItemsAdded() const	
	{
	++iStores[iIndex]->iTaskInfo->iServerItemsAdded;
	}
	
// -----------------------------------------------------------------------------
// CNSmlDSContent::IncreaseServerItemsChanged
// Increases the counter of items changed on the server.
// -----------------------------------------------------------------------------
//
void CNSmlDSContent::IncreaseServerItemsChanged() const
	{
	++iStores[iIndex]->iTaskInfo->iServerItemsChanged;
	}
	
// -----------------------------------------------------------------------------
// CNSmlDSContent::IncreaseServerItemsDeleted
// Increases the counter of items deleted on the server.
// -----------------------------------------------------------------------------
//
void CNSmlDSContent::IncreaseServerItemsDeleted() const
	{
	++iStores[iIndex]->iTaskInfo->iServerItemsDeleted;
	}
	
// -----------------------------------------------------------------------------
// CNSmlDSContent::IncreaseServerItemsMoved
// Increases the counter of items moved on the server.
// -----------------------------------------------------------------------------
//
void CNSmlDSContent::IncreaseServerItemsMoved() const
	{
	++iStores[iIndex]->iTaskInfo->iServerItemsMoved;
	}
	
// -----------------------------------------------------------------------------
// CNSmlDSContent::IncreaseServerItemsFailed
// Increases the counter of items failed on the server.
// -----------------------------------------------------------------------------
//
void CNSmlDSContent::IncreaseServerItemsFailed() const
	{
	++iStores[iIndex]->iTaskInfo->iServerItemsFailed;
	}

// -----------------------------------------------------------------------------
// CNSmlDSContent::PartiallyUpdated
// Checks if at least one change is applied to the local datastore.
// -----------------------------------------------------------------------------
//
TBool CNSmlDSContent::PartiallyUpdated() const
	{
	return ( iStores[iIndex]->iTaskInfo->iItemsAdded |
			 iStores[iIndex]->iTaskInfo->iItemsChanged |
			 iStores[iIndex]->iTaskInfo->iItemsDeleted |
			 iStores[iIndex]->iTaskInfo->iItemsMoved );
	}

// -----------------------------------------------------------------------------
// CNSmlDSContent::MapSourceParent
// Maps a SourceParent to an existing LUID.
// -----------------------------------------------------------------------------
//
TBool CNSmlDSContent::MapSourceParent( const TDesC8& aSourceParent, TSmlDbItemUid& aUid )
	{
	if ( aUid == KDbItemUidRoot )
		{
		return ETrue;
		}
		
	if ( !iStores[iIndex]->iMapContainer )
		{
		return EFalse;
		}
		
	return iStores[iIndex]->iMapContainer->MapSourceParent( aSourceParent, aUid );
	}

// -----------------------------------------------------------------------------
// CNSmlDSContent::StoreNamesL
// Gets a list of current content's store names.
// -----------------------------------------------------------------------------
//
CDesCArray* CNSmlDSContent::StoreNamesL() const
	{
	TInt result( KErrNone );
	CDesCArrayFlat* storeNames = new ( ELeave ) CDesCArrayFlat( 1 );
	
	iHostClient->ListStoresL( storeNames, iStores[iIndex]->iImplementationUID, result );
	
	return storeNames;
	}
	
// -----------------------------------------------------------------------------
// CNSmlDSContent::SetUsedFiltersL
// Stores the set of filters used with the current content.
// -----------------------------------------------------------------------------
//
void CNSmlDSContent::SetUsedFiltersL( RPointerArray<CSyncMLFilter>& aFilters, TSyncMLFilterMatchType aFilterMatchType )
    {
    iStores[iIndex]->iFilterMatchType = aFilterMatchType;
    
    for ( TInt i = 0; i < aFilters.Count(); i++ )
        {
        iStores[iIndex]->iFilters.AppendL( aFilters[i] );
        }
    }
		
// -----------------------------------------------------------------------------
// CNSmlDSContent::HasFilters
// Checks if filters are used.
// -----------------------------------------------------------------------------
//
TBool CNSmlDSContent::HasFilters() const
    {
    return ( iStores[iIndex]->iFilters.Count() > 0 );
    }

// <MAPINFO_RESEND_MOD_BEGIN>

// ---------------------------------------------------------
// CNSmlDSContent::ScanMapContentL()
// Calls map loader function
// ---------------------------------------------------------
TInt CNSmlDSContent::ScanMapContentL( TRequestStatus &aStatus )
	{
	DBG_FILE(_S8("CNSmlDSContent::ScanMapContentL begins"));
	iCallerStatus = &aStatus;
	*iCallerStatus = KRequestPending;
	TRAPD( err, LoadMapInfoL());
	if( err != KErrNone)
		{
		iOldMapExists = EFalse;
		if ( iStores[iIndex]->iMapContainer )
			{
			delete iStores[iIndex]->iMapContainer;
			iStores[iIndex]->iMapContainer = NULL;
			}
		}
	User::RequestComplete(iCallerStatus, err);
	DBG_FILE(_S8("CNSmlDSContent::ScanMapContentL ends"));
	return err;
	}
	
// ---------------------------------------------------------
// CNSmlDSContent::PackupRequestL()
// To save the map information to the physical storage
// ---------------------------------------------------------
void CNSmlDSContent::PackupRequestL( const TTime& aAnchor )
	{
	DBG_FILE(_S8("CNSmlDSContent::PackupRequestL begins"));
	RMutex mutex;
	if(mutex.OpenGlobal( KNSmlDSContentAtomicOperationName ) != KErrNone )
		mutex.CreateGlobal( KNSmlDSContentAtomicOperationName );
	mutex.Wait();
	SaveMapInfoL( aAnchor );
	mutex.Signal();
	mutex.Close();
	DBG_FILE(_S8("CNSmlDSContent::PackupRequestL ends"));
	}

// ----------------------------------------------------------------------------------------------------------------------------------------------------
// CNSmlDSContent::SaveMapInfoL()
// Saves the map information to the physical storage and update the sync flags required for next sync to continue properly but not the last sync anchor
// ----------------------------------------------------------------------------------------------------------------------------------------------------
void CNSmlDSContent::SaveMapInfoL( const TTime& aAnchor )
	{
	DBG_FILE(_S8("CNSmlDSContent::SaveMapInfoL() begins"));
	TBool found=EFalse;
	SaveCurrentIndex();
	if(SetToFirst())
		{
		do
			{
			if( iStores[iIndex]->iMapContainer )
				{
				if( iStores[iIndex]->iAgentLog )
					{
					if( iStores[iIndex]->iAgentLog->IsPresentL( iMapStreamUid ) )
						{
						iStores[iIndex]->iAgentLog->DeleteStreamL( iMapStreamUid );
						}
					MStreamBuf* sb = iStores[iIndex]->iAgentLog->OpenWriteStreamL( iMapStreamUid );
					RWriteStream ws( sb );
					DBG_FILE_CODE( iStores[iIndex]->iMapContainer->MapItemListSize(), _S8("CNSmlDSAgent::SaveMapInfoL(): MapItemListSize!"));
					if( iStores[iIndex]->iMapContainer->MapItemListSize() > 0 )
						{
						found = ETrue;
						TTime now;
						now.UniversalTime();
						iStores[iIndex]->iMapContainer->setMapAnchor( now );
						iStores[iIndex]->iMapContainer->setAppendToStore( EFalse );
						ws << *iStores[iIndex]->iMapContainer;
						}
					iStores[iIndex]->iAgentLog->CloseWriteStreamL( ETrue );
					}
				}
			} while(SetToNext());
		}
	SetIndexToSaved();
	if( found )
		{
		//UpdateSyncAnchorsL( aAnchor );
		}
	DBG_FILE(_S8("CNSmlDSContent::SaveMapInfoL() ends"));
	}

// ---------------------------------------------------------------------------------------------
// CNSmlDSContent::CleanAllMapInfo()
// Cleans up the map information in the cash and physical storage for all the databases
// // ------------------------------------------------------------------------------------------
void CNSmlDSContent::CleanAllMapInfo()
	{
	DBG_FILE(_S8("CNSmlDSAgent:: CleanAllMapInfo() begins"));
	//Generally we will have map informaiton for only one type of database to be deleted in any sync
	//In any case if more than one database has map information this will be useful
	SaveCurrentIndex();
	if(SetToFirst())
		{
		do
			{
			delete iStores[iIndex]->iMapContainer;
			iStores[iIndex]->iMapContainer = NULL;
			if( iStores[iIndex]->iAgentLog )
				{
				TInt err;
				TRAP(err,
						{
						if( iStores[iIndex]->iAgentLog->IsPresentL( iMapStreamUid ) )
							iStores[iIndex]->iAgentLog->DeleteStreamL( iMapStreamUid );
						}
					 );
				DBG_FILE_CODE( err, _S8("CNSmlDSContent::CleanAllMapInfo(): IsPresentL and DeleteStream executed!") );
				}
			}while(SetToNext());
		}
	SetIndexToSaved();
	DBG_FILE(_S8("CNSmlDSAgent:: CleanAllMapInfo() ends"));
	}

// ------------------------------------------------------------------
// CNSmlDSContent::CheckDbIndex()
// Checks if a given index is a valid index of a database in a sync
// ------------------------------------------------------------------
TBool CNSmlDSContent::CheckDbIndex(TInt aIndex){
	DBG_FILE(_S8("CNSmlDSContent::CheckDbIndex() starts"));
	TBool ret =EFalse;
	for ( TInt i = 0; i < iStores.Count(); i++){
		if ( i==aIndex )
		{
			ret = ETrue;
			break;
		}
	}
	DBG_FILE(_S8("CNSmlDSContent::CheckDbIndex() ends"));
	return ret;
}

// --------------------------------------------------------------------------------------------------
// CNSmlDSContent::CleanMapInfo()
// To clean up the map information in the cache as well as from physical storage for current database
// --------------------------------------------------------------------------------------------------
void CNSmlDSContent::CleanMapInfo()
	{
	DBG_FILE(_S8("CNSmlDSContent::CleanMapInfo() starts"));
	if( CheckDbIndex(iIndex) )
		{
		delete iStores[iIndex]->iMapContainer;
		iStores[iIndex]->iMapContainer = NULL;
		if( iStores[iIndex]->iAgentLog )
			{
			DBG_FILE(_S8("CNSmlDSContent::CleanMapInfo() - Trying to delete the present stream!"));
			TInt err;
			TRAP(err,
					{
					if( iStores[iIndex]->iAgentLog->IsPresentL(iMapStreamUid))
						{
						DBG_FILE(_S8("CNSmlDSContent::CleanMapInfo(): iMapStreamUid is present!"));
						iStores[iIndex]->iAgentLog->DeleteStreamL( iMapStreamUid );
						}
					}
				 );
			DBG_FILE_CODE( err, _S8("CNSmlDSContent::CleanMapInfo(): IsPresentL and DeleteStream executed!") );
			}
		}
	else
		{
		DBG_FILE(_S8("CNSmlDSContent::CleanMapInfo() - Cleaning all mapinfo!"));
		CleanAllMapInfo();
		}
	DBG_FILE(_S8("CNSmlDSContent::CleanMapInfo() ends"));
	}

// -------------------------------------------------------------------
// CNSmlDSContent::SetMapStreamUID()
// To Set the stream uid to be used for the map information streaming
// -------------------------------------------------------------------
void CNSmlDSContent::SetMapStreamUID(TInt aMapStreamUid)
	{
	iMapStreamUid.iUid = aMapStreamUid;
	}

// ------------------------------------------------------------------
// CNSmlDSContent::LoadMapInfoL()
// To Load the map information form the physical storage to the cache
// ------------------------------------------------------------------
void CNSmlDSContent::LoadMapInfoL()
	{
	DBG_FILE(_S8("CNSmlDSContent::LoadMapInfoL() starts"));
	if( iStores[iIndex]->iAgentLog && iStores[iIndex]->iAgentLog->IsPresentL(iMapStreamUid) )
		{
		if ( iStores[iIndex]->iMapContainer )
			{
			DBG_FILE(_S8("CNSmlDSContent::LoadMapInfoL(): deleting mapcontainer"));
			delete iStores[iIndex]->iMapContainer;
			iStores[iIndex]->iMapContainer = NULL;
			}
		DBG_FILE(_S8("CNSmlDSContent::LoadMapInfoL(): Creating new map container!"));
		iStores[iIndex]->iMapContainer = CNSmlDSMapContainer::NewL();
		MStreamBuf* sb = iStores[iIndex]->iAgentLog->OpenReadStreamL( iMapStreamUid );
		RReadStream rs(sb);
		TInt sz = rs.Source()->SizeL();
		if( sz > 0 )
			{
			TRAPD( err1,rs >> *iStores[iIndex]->iMapContainer);
			DBG_FILE_CODE( err1, _S8("CNSmlDSContent::LoadMapInfoL(): Tried to load mapcontainer!") );
			if( err1 != KErrNone )
				{
				DBG_FILE(_S8("CNSmlDSContent::LoadMapInfoL() - Reading mapcontainer from stream successful!"));
				iOldMapExists = ETrue;
				}
			}
		iStores[iIndex]->iAgentLog->CloseReadStreamL();
		}
	DBG_FILE(_S8("CNSmlDSContent::LoadMapInfoL() ends"));
	}


// --------------------------------------------------------------------------------------
// CNSmlDSContent::SetRemoveMap()
// To Delay the removal of map information from the cache to be removed at a later stage
// --------------------------------------------------------------------------------------
void CNSmlDSContent::SetRemoveMap(TBool aRemoveMap)
	{
	if(iStores[iIndex]->iMapContainer)
		{
		iStores[iIndex]->iMapContainer->setRemoveMap(aRemoveMap);
		}
	}
// ---------------------------------------------------------
// CNSmlDSContent::SetRemoveMap()
// To Delay the removal of map information from the cache
// ---------------------------------------------------------

TBool CNSmlDSContent::isMapRemovable()
	{
	if(iStores[iIndex]->iMapContainer)
		{
		return iStores[iIndex]->iMapContainer->isMapRemovable();
		}
	return EFalse;
	}
// -----------------------------------------------------------------------------------------------------------------------------------
// CNSmlDSContent::RemoveRemovableMap()
// Removes the map information form the cache for only set removable map and corresponding to the current database under consideration
// -----------------------------------------------------------------------------------------------------------------------------------
void CNSmlDSContent::RemoveRemovableMap() const
	{
	if ( iStores[iIndex]->iMapContainer && iStores[iIndex]->iMapContainer->isMapRemovable())
		{
		delete iStores[iIndex]->iMapContainer;
		iStores[iIndex]->iMapContainer = NULL;
		}
	}

// -------------------------------------------------------------------------------------------
// CNSmlDSContent::RemoveAllRemovableMap()
// Removes the map information form the cache for only set removable map for all the databases
// -------------------------------------------------------------------------------------------
void CNSmlDSContent::RemoveAllRemovableMap() const
	{
	for ( TInt i = 0; i < iStores.Count() ; i++ )
		{
		if ( iStores[i]->iMapContainer && iStores[i]->iMapContainer->isMapRemovable())
			{
			delete iStores[i]->iMapContainer;
			iStores[i]->iMapContainer = NULL;
			}
		}
	}

void CNSmlDSContent::SetResendUsed( TBool aResendUsed ) 
	{
	iResendUsed = aResendUsed;
	}

TBool CNSmlDSContent::ResendUsed() const 
	{
	return iResendUsed;	
	}
// <MAPINFO_RESEND_MOD_END>

//RD_SUSPEND_RESUME
// ---------------------------------------------------------
// CNSmlDSContent::SetMapRemovable()
// 
// ---------------------------------------------------------
	
void CNSmlDSContent::SetMapRemovable()
	{
	for ( TInt i = 0; i < iStores.Count() ; i++ )
		{
		if (iStores[i]->iMapContainer)
			{
		  	iStores[i]->iMapContainer->setRemoveMap(ETrue);
			}
		}
	}
	
//RD_SUSPEND_RESUME	
// ---------------------------------------------------------
// CNSmlDSContent::SetSuspendedStateL()
// 
// ---------------------------------------------------------

void CNSmlDSContent::SetSuspendedStateL(TNSmlSyncSuspendedState aState)
	{
		iStores[iIndex]->iAgentLog->SetIntValueL( EAgentLogSyncSuspendedState, aState);		
	}

//RD_SUSPEND_RESUME
// ---------------------------------------------------------
// CNSmlDSContent::SuspendedStateL()
// 
// ---------------------------------------------------------
TNSmlSyncSuspendedState CNSmlDSContent::SuspendedStateL()
	{
	return (TNSmlSyncSuspendedState)iStores[iIndex]->iAgentLog->IntValueL( EAgentLogSyncSuspendedState );
	}
//RD_SUSPEND_RESUME
// ---------------------------------------------------------
// CNSmlDSContent::SetPreviousSyncType()
// 
// ---------------------------------------------------------	
	
void CNSmlDSContent::SetPreviousSyncTypeL(TNSmlPreviousSyncType aPrevSyncType)
{
	iStores[iIndex]->iAgentLog->SetIntValueL( EAgentLogPreviousSyncType, aPrevSyncType);
}
//RD_SUSPEND_RESUME
// ---------------------------------------------------------
// CNSmlDSContent::PreviousSyncType()
// 
// ---------------------------------------------------------	
	
TNSmlPreviousSyncType CNSmlDSContent::PreviousSyncTypeL()
	{
   		return(TNSmlPreviousSyncType)iStores[iIndex]->iAgentLog->IntValueL(EAgentLogPreviousSyncType);
      
	}
	
//RD_SUSPEND_RESUME	
// ---------------------------------------------------------
// CNSmlDSContent::SetPreviousClientSyncType()
// 
// ---------------------------------------------------------
void CNSmlDSContent::SetPreviousClientSyncTypeL(TNSmlPreviousSyncType aPrevSyncType)
{
	iStores[iIndex]->iAgentLog->SetIntValueL( EAgentLogPreviousClientSyncType, aPrevSyncType);
}

//RD_SUSPEND_RESUME
// ---------------------------------------------------------
// CNSmlDSContent::PreviousClientSyncType()
// 
// ---------------------------------------------------------
TNSmlPreviousSyncType CNSmlDSContent::PreviousClientSyncTypeL()
{
  		return(TNSmlPreviousSyncType)iStores[iIndex]->iAgentLog->IntValueL(EAgentLogPreviousClientSyncType);
}	
// ---------------------------------------------------------
// CNSmlDSContent::ReadRepositoryL()
// 
// ---------------------------------------------------------

void CNSmlDSContent::ReadRepositoryL(TInt aKey, TInt& aValue)
{
	const TUid KRepositoryId = KCRUidDataSyncInternalKeys;
	
    CRepository* rep = CRepository::NewLC(KRepositoryId);
    TInt err = rep->Get(aKey, aValue);
	User::LeaveIfError(err);
	CleanupStack::PopAndDestroy(rep);
}
// End of File
