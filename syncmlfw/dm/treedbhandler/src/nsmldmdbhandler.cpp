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
* Description:  source file for dm tree db handler
*
*/


#include <utf.h>
#include <s32file.h>
#include <sysutil.h>
#include <bautils.h>
#include <utf.h>
#include <data_caging_path_literals.hrh>

#include <nsmlconstants.h>
#include <nsmldebug.h>
#include <nsmldmdbhandler.h>
#include <nsmldmuri.h>
//For P&S keys
#include <e32property.h>
#include "nsmldminternalpskeys.h"
#include "nsmldmaclparser.h"



// ===========================================================================
// CNSmlDmDbHandler
// ===========================================================================


// ---------------------------------------------------------------------------
// CNSmlDmDbHandler::~CNSmlDmDbHandler()
// ---------------------------------------------------------------------------
EXPORT_C CNSmlDmDbHandler::~CNSmlDmDbHandler()
	{
	iAdapters.ResetAndDestroy();
	iView.Close();
	iDatabase.Close();
	if ( iFileStore )
		{
		delete iFileStore;
		}
	iFsSession.Close();
	delete iServer;
	iAclURIs.ResetAndDestroy();
	iAcls.ResetAndDestroy();

	}

// ---------------------------------------------------------------------------
// CNSmlDmDbHandler* CNSmlDmDbHandler::NewLC()
// ---------------------------------------------------------------------------
EXPORT_C CNSmlDmDbHandler* CNSmlDmDbHandler::NewLC()
	{
	CNSmlDmDbHandler* self = new (ELeave) CNSmlDmDbHandler();
	CleanupStack::PushL( self );
	self->ConstructL();
	return self;
	}

// ---------------------------------------------------------------------------
// CNSmlDmDbHandler* CNSmlDmDbHandler::NewL()
// ---------------------------------------------------------------------------
EXPORT_C CNSmlDmDbHandler* CNSmlDmDbHandler::NewL()
	{
	CNSmlDmDbHandler* self = CNSmlDmDbHandler::NewLC();
	CleanupStack::Pop();
	return self;
	}


// ---------------------------------------------------------------------------
// void CNSmlDmDbHandler::ConstructL()
// ---------------------------------------------------------------------------
void CNSmlDmDbHandler::ConstructL()
	{
	_DBG_FILE("CNSmlDmDbHandler::ConstructL() : begin");
	User::LeaveIfError( iFsSession.Connect() );
	
	HBufC* treeDbPath = HBufC::NewLC( KMaxFileName );
	TPtr treeDbPtr = treeDbPath->Des();
	User::LeaveIfError(iFsSession.PrivatePath( treeDbPtr ) );
	treeDbPtr.Append(KSmlDmModuleDbName);
	
	if ( !BaflUtils::FileExists ( iFsSession, *treeDbPath ) )
		{
		CreateDatabaseL( *treeDbPath );		
		}
	else
		{
		
 	   	iFileStore = CPermanentFileStore::OpenL ( iFsSession,
	   		*treeDbPath, EFileRead|EFileWrite );
 	   		
    	iFileStore->SetTypeL ( iFileStore->Layout() );
		}
	CleanupStack::PopAndDestroy(); //treeDbPath	
	_DBG_FILE("CNSmlDmDbHandler::ConstructL() : end");
	}


// ---------------------------------------------------------------------------
// void CNSmlDmDbHandler::CreateDatabaseL( const TDesC& aFullName )
// Creates database and tables
// ---------------------------------------------------------------------------
void CNSmlDmDbHandler::CreateDatabaseL( const TDesC& aFullName )
	{
	_DBG_FILE("CNSmlDmDbHandler::CreateDatabaseL() : begin");
	
    // Create empty database file.
  	iFsSession.CreatePrivatePath( EDriveC );

    iFileStore = CPermanentFileStore::ReplaceL ( iFsSession,
    	aFullName, EFileRead|EFileWrite );
    	
    iFileStore->SetTypeL ( iFileStore->Layout() );
    TStreamId id = iDatabase.CreateL ( iFileStore );
    iFileStore->SetRootL ( id );
    iFileStore->CommitL();
	
	iDatabase.Begin();

	CDbColSet* luidMappingColumns = CDbColSet::NewLC();
	luidMappingColumns->AddL(TDbCol ( KNSmlDmAdapterId, EDbColUint32 ) ); 		//adapter impluid
	luidMappingColumns->AddL(TDbCol ( KNSmlDmMappingURI, EDbColLongText8 ) ); 	//mapping uries in string separated with '&'
	luidMappingColumns->AddL(TDbCol ( KNSmlDmMappingLuid, EDbColLongText8 ) ); 	//table of mapping luids
	
	//Create LuidMapping table
	User::LeaveIfError ( iDatabase.CreateTable ( KNSmlDmLuidMappingTable,
		*luidMappingColumns ) );

	CDbColSet* aclColumns = CDbColSet::NewLC();

	//adapter impluid
	aclColumns->AddL ( TDbCol ( KNSmlDmAdapterId, EDbColUint32 ) ); 
	//uries in string separated with '&'
	aclColumns->AddL ( TDbCol ( KNSmlDmAclURI, EDbColLongText8 ) );
	//table of acls
	aclColumns->AddL ( TDbCol ( KNSmlDmAcl, EDbColLongText8 ) );
	
	//Create AclTable table
	User::LeaveIfError ( iDatabase.CreateTable ( KNSmlDmAclTable,
		*aclColumns ) );

	iDatabase.Commit();
	iDatabase.Close();
	CleanupStack::PopAndDestroy( 2 ); // luidMappingColumns, aclColumns
	_DBG_FILE("CNSmlDmDbHandler::CreateDatabaseL() : end");
	}

// ---------------------------------------------------------------------------
// void CNSmlDmDbHandler::SetServerL(const TDesC& aServer)
// Set the iServer member variable, which is used for ACL handling
// ---------------------------------------------------------------------------
EXPORT_C void CNSmlDmDbHandler::SetServerL(const TDesC8& aServer)
	{
	_DBG_FILE("CNSmlDmDbHandler::SetServerL() : begin");
	if(iServer)
		{
		delete iServer;
		iServer = 0;
		}
	iServer = aServer.AllocL();
	_DBG_FILE("CNSmlDmDbHandler::SetServerL() : end");
	}


// ---------------------------------------------------------------------------
// void CNSmlDmDbHandler::ReadAllURIMappingsFromDbL()
// Get the URI mapping info from database to member variables.
// ---------------------------------------------------------------------------

void CNSmlDmDbHandler::ReadAllURIMappingsFromDbL()
	{
	iAdapters.ResetAndDestroy();
	iDatabase.OpenL ( iFileStore, iFileStore->Root() );

	_LIT ( KNSmlDmSQLStatement, "SELECT * FROM LuidMappingTable");
	TPtrC sql ( KNSmlDmSQLStatement() );

	User::LeaveIfError ( iView.Prepare ( iDatabase, sql,
		TDbWindow( TDbWindow::EUnlimited ) ) );
		
	User::LeaveIfError ( iView.EvaluateAll() );

	if ( iView.FirstL() )
		{
		do 
			{
			iView.GetL();
			ReadOneLineFromDbL();
			} while ( iView.NextL() );	
		}
	iView.Close();
	iDatabase.Close();
	}


// ---------------------------------------------------------------------------
// void CNSmlDmDbHandler::ReadOneLineFromDbL()
// Reads one line at the time from db.
// Cursor is kept at valid line outside of this function.
// ---------------------------------------------------------------------------

void CNSmlDmDbHandler::ReadOneLineFromDbL()
	{
	TUint32 adapterId(0);			
	CDbColSet* colSet = iView.ColSetL();
	CleanupStack::PushL ( colSet );
	
	RDbColReadStream colReadStream;
		
	adapterId = iView.ColUint32 ( colSet->ColNo ( KNSmlDmAdapterId ) );
	TDbColNo uri = colSet->ColNo ( KNSmlDmMappingURI );
	TDbColNo luid = colSet->ColNo ( KNSmlDmMappingLuid );

	colReadStream.OpenLC ( iView, uri );
	HBufC8* uri8 = HBufC8::NewLC ( iView.ColLength( uri ) );
	TPtr8 uriPtr = uri8->Des();
	colReadStream.ReadL ( uriPtr, iView.ColLength ( uri ) );
	CleanupStack::Pop();  // uriPtr
	CleanupStack::PopAndDestroy();  // colReadStream
	CleanupStack::PushL( uri8 ); 

	colReadStream.OpenLC ( iView, luid );
	HBufC8* luid8 = HBufC8::NewLC ( iView.ColLength( luid ) );
	TPtr8 luidPtr = luid8->Des();
	colReadStream.ReadL ( luidPtr, iView.ColLength ( luid ) );
	CleanupStack::Pop();  // luidPtr
	CleanupStack::PopAndDestroy();  // colReadStream
	CleanupStack::PushL ( luid8 ); 
		
	UpdateInternalArrayL (
		adapterId, uriPtr, luidPtr, EMapNoAction, ENormalSearch );
		
	CleanupStack::PopAndDestroy(3); // luidPtr, uriPtr, colSet				
	}
	
// ---------------------------------------------------------------------------
// void CNSmlDmDbHandler::UpdateInternalArrayL()
// Adds (or marks) entries to member variable array.
// Given values are either read from db or function call parameters.
// ---------------------------------------------------------------------------
void CNSmlDmDbHandler::UpdateInternalArrayL(TUint32 aAdapterId,
	const TDesC8& aURI,
	const TDesC8& aLUID,
	TNSmlDmMapOperation aOperation,
	TNSmlDmSearchMethod aMethod,
	const TDesC8& aNewUri )
	{
	CNSmlDmURIMapping* mapElement =
		CNSmlDmURIMapping::NewLC( aURI, aLUID, aOperation );
		
	TBool newAdapter(ETrue);
	for ( TInt i=0; i<iAdapters.Count(); i++ )
		{
		if ( iAdapters[i]->iAdapterId == aAdapterId )
			{
			newAdapter = EFalse;
			if ( aOperation == EMapNoAction || aMethod == ENoSearch )
				{
				iAdapters[i]->iMapArray.AppendL ( mapElement );
				CleanupStack::Pop();  // mapElement			
				}
			else
				{
				TInt adIndex(0);
				RArray<TInt> uriIndex;
				SearchUriL( aAdapterId, aURI, aMethod, adIndex, uriIndex );
				if ( aOperation == EMapDelete )
					{
					for ( TInt u=0; u<uriIndex.Count(); u++ )
						{
						TInt mapIndex ( uriIndex[u] );
						iAdapters[adIndex]->iMapArray[mapIndex]->iOperation
							= aOperation;
						}
					CleanupStack::PopAndDestroy();  // mapElement
					}
				else if ( aOperation == EMapInsert )
					{
					if ( uriIndex.Count() > 0 )
						{
						CleanupStack::PopAndDestroy();  // mapElement
						for ( TInt u=0; u<uriIndex.Count(); u++ )
							{
							if ( aLUID.Compare (*iAdapters[adIndex]->
								iMapArray[uriIndex[u]]->iLUID ) != 0 ||
							    iAdapters[adIndex]->iMapArray[uriIndex[u]]->
							    iOperation == EMapDelete ) 
								{
								iAdapters[adIndex]->iMapArray[uriIndex[u]]->
									iOperation = EMapUpdate;
									
								delete iAdapters[adIndex]->
									iMapArray[uriIndex[u]]->iLUID;
									
								iAdapters[adIndex]->iMapArray[uriIndex[u]]->
									iLUID = NULL;							
									
								iAdapters[adIndex]->iMapArray[uriIndex[u]]->
									iLUID = aLUID.AllocL();
								}
							}
						}
					else
						{
						iAdapters[i]->iMapArray.AppendL ( mapElement );
						CleanupStack::Pop();  // mapElement
						}
					}
				else if ( aOperation == EMapRename )
					{
					for ( TInt u=0; u<uriIndex.Count(); u++ )
						{
						iAdapters[adIndex]->iMapArray[uriIndex[u]]->
							iOperation = EMapDelete;
							
						HBufC8* oldLUIDSaved = iAdapters[adIndex]->
							iMapArray[uriIndex[u]]->iLUID;
							
						TPtr8 newURI = HBufC8::NewLC( aNewUri.Length() + ( iAdapters[adIndex]->iMapArray[uriIndex[u]]->iURI->Length() - aURI.Length() ) )->Des();
						newURI.Copy ( aNewUri );
						
						if (  iAdapters[adIndex]->iMapArray[uriIndex[u]]->
							iURI->Length() - aURI.Length() > 0 )
							{
							newURI.Append ( iAdapters[adIndex]->
								iMapArray[uriIndex[u]]->iURI->
								Mid( aURI.Length() ) );
							}
							
						UpdateInternalArrayL ( aAdapterId,
							newURI,
							*oldLUIDSaved,
							EMapInsert,
							ENoSearch );
							
						CleanupStack::PopAndDestroy();  // newURI
						}
					CleanupStack::PopAndDestroy();  // mapElement
					}
				else 
					{
					CleanupStack::PopAndDestroy(); // mapElement
					}
				uriIndex.Reset();
				}
			break;
			}
		}
	if ( newAdapter && aOperation != EMapDelete )
		{
		CNSmlDmAdapterElement* adapterElement =
			CNSmlDmAdapterElement::NewLC ( aAdapterId );
			
		iAdapters.AppendL ( adapterElement );
		iAdapters[iAdapters.Count()-1]->iMapArray.AppendL ( mapElement );
		CleanupStack::Pop(2);  // adapterElement, mapElement
		}	
//ADD-S for DMS00652653
	else if ( newAdapter)
		{
		CleanupStack::PopAndDestroy(); // mapElement
		}
//ADD-E for DMS00652653
	}
	
	
// ---------------------------------------------------------------------------
// void CNSmlDmDbHandler::UpdateRowInDbL(
//	TUint32 aAdapterId,
// 	const TDesC8& aURI, 
//	const TDesC8& aLUID,
//	TNSmlDmMapOperation aOperation,
//	TBool aPrepareView )
// ---------------------------------------------------------------------------
void CNSmlDmDbHandler::UpdateRowInDbL(TUint32 aAdapterId,
	const TDesC8& aURI,
	const TDesC8& aLUID,
	TNSmlDmMapOperation aOperation )
	{
	iDatabase.Begin();
	TBool insertNewEntry(EFalse);
	if ( aOperation == EMapDelete )
		{
		if ( SearchRowInDbL ( aAdapterId, aURI ) == KErrNone )
			{
			iView.GetL();
			iView.DeleteL();
			iPrepareView = ETrue;

			}
		}
	else if ( aOperation == EMapUpdate )
		{
		if ( SearchRowInDbL ( aAdapterId, aURI ) == KErrNone )
			{
			CDbColSet* colSet = iView.ColSetL();
			CleanupStack::PushL ( colSet );
			iView.GetL();
			iView.UpdateL();
			RDbColWriteStream writeStream;
			writeStream.OpenLC ( iView, colSet->ColNo( KNSmlDmMappingLuid ));
			writeStream.WriteL ( aLUID );
			writeStream.CommitL();
			CleanupStack::PopAndDestroy(); //writeStream
			iView.PutL();
			CleanupStack::PopAndDestroy(); //colSet
			iPrepareView = EFalse;
			}
		else
			{
			insertNewEntry = ETrue;
			}
		}		
	if ( aOperation == EMapInsert  || insertNewEntry )
		{
		RDbTable uriTable;
		User::LeaveIfError ( uriTable.Open ( iDatabase,
			KNSmlDmLuidMappingTable ) );
			
		CDbColSet* columns = uriTable.ColSetL();
		CleanupStack::PushL ( columns );

		uriTable.Reset();
		uriTable.InsertL();
		uriTable.SetColL ( columns->ColNo ( KNSmlDmAdapterId ), aAdapterId );
		RDbColWriteStream writeStream;
		writeStream.OpenLC ( uriTable, columns->ColNo( KNSmlDmMappingLuid ) );
		writeStream.WriteL ( aLUID );
		writeStream.CommitL();
		CleanupStack::PopAndDestroy();  // writestream for Luid
		writeStream.OpenLC ( uriTable, columns->ColNo( KNSmlDmMappingURI ) );
		writeStream.WriteL ( aURI );
		writeStream.CommitL();
		CleanupStack::PopAndDestroy();  // writestream for Uri		
		uriTable.PutL();
		CleanupStack::PopAndDestroy(); // columns
		uriTable.Close();
		iPrepareView = EFalse;
		}
	iView.Close();	
	iDatabase.Commit();
	}

// ---------------------------------------------------------------------------
// TInt CNSmlDmDbHandler::SearchRowInDbL()
// ---------------------------------------------------------------------------
TInt CNSmlDmDbHandler::SearchRowInDbL(TUint32 aAdapterId,
	const TDesC8& aURI )
	{
	TInt ret(KErrNotFound);
	
	if ( iPrepareView )
		{
		_LIT( KNSmlDmUpdateRow,
			"SELECT * FROM LuidMappingTable WHERE AdapterId=%u" );
			
		TPtr sqlPtr = HBufC::NewLC( KNSmlDmUpdateRow().Length() +
			KNSmlDmIdLength )->Des(); 
			
		sqlPtr.Format ( KNSmlDmUpdateRow(), aAdapterId );
		
		User::LeaveIfError ( iView.Prepare ( iDatabase, sqlPtr,
			TDbWindow( TDbWindow::EUnlimited ) ) );
			
		User::LeaveIfError ( iView.EvaluateAll() );

		CleanupStack::PopAndDestroy();  // sqlPtr
		}

	if ( iView.FirstL() )
		{
		do 
			{
			CDbColSet* colSet = iView.ColSetL();
			CleanupStack::PushL ( colSet );
			TDbColNo uri = colSet->ColNo ( KNSmlDmMappingURI );

			iView.GetL();
			RDbColReadStream colReadStream;
			colReadStream.OpenLC ( iView, uri );
			HBufC8* uri8 = HBufC8::NewLC ( iView.ColLength( uri ) );
			TPtr8 uriPtr = uri8->Des();
			colReadStream.ReadL ( uriPtr, iView.ColLength ( uri ) );
			
			if ( aURI.Compare ( uriPtr ) == 0 )
				{
				CleanupStack::PopAndDestroy(3);//uri8, colReadStream, colSet
				ret = KErrNone;
				break;
				}
			CleanupStack::PopAndDestroy(3);//uri8, colReadStream, colSet			
			} while ( iView.NextL() );	
		}
	return ret;
	}
	

// ---------------------------------------------------------------------------
// TInt CNSmlDmDbHandler::SearchUriL()
// Searches the given URI from the internal array.
// There are four different search methods:
// - ENormalSearch: lengths and contents must match
// - EUsingParents: e.g. given Email/NN/XXX finds and stops to Email/NN
// - EAmongChidren: e.g. given Email/NN finds: Email/NN/AA,
//		Email/NN/3 and Email/NN
// - EAmongParents: e.g. given Email/NN finds: Email/NN/AA and Email/NN/3
//		but NOT Email/NN
// ---------------------------------------------------------------------------

TInt CNSmlDmDbHandler::SearchUriL( TUint32 aAdapterId,
	const TDesC8& aURI,
	TNSmlDmSearchMethod aSearchMethod,
	TInt& aAdIndex,
	RArray<TInt>& aUriIndex)
	{
	TInt found(KErrNotFound);
	TInt i;	
	for ( i=0; i<iAdapters.Count(); i++ )
		{
		if ( iAdapters[i]->iAdapterId == aAdapterId )
			{
			if ( aSearchMethod == ENormalSearch )
				{
				for ( TInt u=0; u<iAdapters[i]->iMapArray.Count(); u++ )
					{
					if ( aURI.Compare
						( *iAdapters[i]->iMapArray[u]->iURI ) == 0 ) 
						{
						found = KErrNone;
						aAdIndex = i;
						aUriIndex.Append ( u );
						break;
						}
					}
				}
			else if ( aSearchMethod == EUsingParents )
				{
				TPtr8 uriPtr = aURI.AllocLC()->Des();
				while ( uriPtr.Length() > 0 && found != KErrNone )
					{
					for ( TInt u=0; u<iAdapters[i]->iMapArray.Count(); u++ )
						{
						if ( uriPtr.Compare
							( *iAdapters[i]->iMapArray[u]->iURI ) == 0 ) 
							{
							found = KErrNone;
							aAdIndex = i;
							aUriIndex.Append ( u );
                            if( uriPtr.Compare(aURI) == 0 )
                                {
                                //Checks the cmd is Add or not
                                TInt val = KErrNotFound;
                                TInt err = RProperty::Get( KPSUidNSmlDMSyncAgent, 
                                        KNSmlDMCmdAddOnExistingNodeorLeafKey,
                                        val );      
                                if( err == KErrNone && EAddCmd == val )//means Add cmd
                                    {           
                                    //Node exists in DB, set P&S key to EAddOnExistingNode 
                                    RProperty::Set( KPSUidNSmlDMSyncAgent,
                                            KNSmlDMCmdAddOnExistingNodeorLeafKey,
                                            EAddOnExistingNode );
                                    }
                                }
							}
						}
					uriPtr = NSmlDmURI::RemoveLastSeg ( uriPtr );
					}
				CleanupStack::PopAndDestroy(); //uriPtr
				}
			else if ( aSearchMethod == EAmongChildren )
				{
				for ( TInt u=0; u<iAdapters[i]->iMapArray.Count(); u++ )
					{
					if ( iAdapters[i]->iMapArray[u]->iURI->Find( aURI ) == 0 )
						{
						found = KErrNone;
						aAdIndex = i;
						aUriIndex.Append ( u );
						}
					}				
				}
			else if ( aSearchMethod == EAmongParents )
				{
				for ( TInt u=0; u<iAdapters[i]->iMapArray.Count(); u++ )
					{
					if ( aURI.Compare (NSmlDmURI::ParentURI(
						*iAdapters[i]->iMapArray[u]->iURI )  ) == 0 )
						{
						found = KErrNone;
						aAdIndex = i;
						aUriIndex.Append ( u );
						}
					}				
				}
			break;
			}	
		}
	return found;
	}

// ---------------------------------------------------------------------------
// TInt CNSmlDmDbHandler::GetMappingInfoL()
// Get the mapping info either by reading first mappings from db or 
// directly from member variables.
// ---------------------------------------------------------------------------
EXPORT_C TInt CNSmlDmDbHandler::GetMappingInfoLC(TUint32 aAdapterId,
	const TDesC8& aURI,	HBufC8*& aLuid )
	{
	_DBG_FILE("CNSmlDmDbHandler::GetMappingInfoL() : begin");
	if ( !iURIMappingsReadFromDb )
		{
		ReadAllURIMappingsFromDbL();
		iURIMappingsReadFromDb = ETrue;
		}
		
	TInt adIndex(0);
	RArray<TInt> uriIndex;
	TInt found = SearchUriL( aAdapterId, aURI, EUsingParents,
		adIndex, uriIndex );
		
	if ( found == KErrNone )
		{
		aLuid = iAdapters[adIndex]->iMapArray[uriIndex[0]]->iLUID->AllocLC();
		}
	else
		{
		aLuid = KNullDesC8().AllocLC();		
		}
	uriIndex.Reset();
	_DBG_FILE("CNSmlDmDbHandler::GetMappingInfoL() : end");
	return found;
	}

// ---------------------------------------------------------------------------
// TInt CNSmlDmDbHandler::AddMappingInfoL ( TUint32 aAdapterId,
// const TDesC8& aURI, const TDesC8& aLuid )
// Adds the  mapping info to member variables, if it doesn't already exist.
// ---------------------------------------------------------------------------
EXPORT_C TInt CNSmlDmDbHandler::AddMappingInfoL ( TUint32 aAdapterId,
	const TDesC8& aURI, const TDesC8& aLuid )
	{
	_DBG_FILE("CNSmlDmDbHandler::AddMappingInfoL() : begin");
	if (SysUtil::FFSSpaceBelowCriticalLevelL ( &iFsSession,
		aURI.Size() + aLuid.Size() ) ) 
		{
		User::Leave(KErrDiskFull);
		}

	if ( !iURIMappingsReadFromDb )
		{
		ReadAllURIMappingsFromDbL();
		iURIMappingsReadFromDb = ETrue;
		}

	UpdateInternalArrayL ( aAdapterId, aURI, aLuid,
		EMapInsert, ENormalSearch );
	
	_DBG_FILE("CNSmlDmDbHandler::AddMappingInfoL() : end");
	return KErrNone;
	}


// ---------------------------------------------------------------------------
// TInt CNSmlDmDbHandler::RemoveMappingInfoL ( TUint32 aAdapterId,
//		const TDesC8& aURI, TBool aChildAlso )
// Removes mapping info from member variables
// ---------------------------------------------------------------------------
EXPORT_C TInt CNSmlDmDbHandler::RemoveMappingInfoL ( TUint32 aAdapterId,
	const TDesC8& aURI,	TBool aChildAlso )
	{
	_DBG_FILE("CNSmlDmDbHandler::RemoveMappingInfoL() : begin");
	if ( !iURIMappingsReadFromDb )
		{
		ReadAllURIMappingsFromDbL();
		iURIMappingsReadFromDb = ETrue;
		}

	//delete mapping items if found
	TNSmlDmSearchMethod method =
		( aChildAlso ) ? EAmongChildren : ENormalSearch;
		
	HBufC8* luid = HBufC8::NewLC(0);
	UpdateInternalArrayL ( aAdapterId, aURI, *luid, EMapDelete, method );
	CleanupStack::PopAndDestroy();  // luid

	_DBG_FILE("CNSmlDmDbHandler::RemoveMappingInfoL() : end");
	return KErrNone;
	}


// ---------------------------------------------------------------------------
// TInt CNSmlDmDbHandler::RenameMappingL ( TUint32 aAdapterId,
//		const TDesC8& aURI, const TDesC8& aObject )
// ---------------------------------------------------------------------------
EXPORT_C TInt CNSmlDmDbHandler::RenameMappingL ( TUint32 aAdapterId,
	const TDesC8& aURI, const TDesC8& aObject )
	{
	_DBG_FILE("CNSmlDmDbHandler::RenameMappingL() : begin");
	TInt ret = KErrNotFound;

	if ( !iURIMappingsReadFromDb )
		{
		ReadAllURIMappingsFromDbL();
		iURIMappingsReadFromDb = ETrue;
		}

	if ( aObject.Compare ( NSmlDmURI::LastURISeg ( aURI ) ) == 0 )
		{
		return KErrNone;
		}

	TPtr8 newUriPtr = HBufC8::NewLC( NSmlDmURI::RemoveLastSeg(aURI).Length() +
		aObject.Length() + 1 )->Des();
		
	newUriPtr.Copy ( NSmlDmURI::RemoveLastSeg(aURI) );
	newUriPtr.Append ( KNSmlDmLitSeparator );
	newUriPtr.Append ( aObject );
	
	//Does the renamed uri already exist..
	TInt adIndex(0);
	RArray<TInt> uriIndex;
	if ( SearchUriL( aAdapterId, newUriPtr, EAmongChildren, adIndex,
		uriIndex ) == KErrNone )
		{
		ret = KErrAlreadyExists;
		}
	else 
		{
		adIndex = 0;
		uriIndex.Reset();		
		if ( SearchUriL( aAdapterId, aURI, ENormalSearch, adIndex,
			uriIndex ) == KErrNone )
			{
			HBufC8* luid = HBufC8::NewLC(0);
			
			UpdateInternalArrayL ( aAdapterId, aURI, *luid, EMapRename,
				EAmongChildren, newUriPtr );
				
			CleanupStack::PopAndDestroy();  // luid
			RenameAclL ( aURI, newUriPtr );
			ret = KErrNone;
			}		
		}
	uriIndex.Reset();
	CleanupStack::PopAndDestroy();  // newUriPtr
	_DBG_FILE("CNSmlDmDbHandler::RenameMappingL() : end");
	return ret;
	}


// ---------------------------------------------------------------------------
// TInt CNSmlDmDbHandler::GetURISegmentListL ( TUint32 aAdapterId,
//	const TDesC8& aURI,
//	CArrayFix<TNSmlDmMappingInfo>& aPreviousURISegmentList)
// Gets child uri list of aURI.
// ---------------------------------------------------------------------------
EXPORT_C TInt CNSmlDmDbHandler::GetURISegmentListL ( TUint32 aAdapterId,
	const TDesC8& aURI, CArrayFix<TSmlDmMappingInfo>& aURISegList)
	{
	_DBG_FILE("CNSmlDmDbHandler::GetURISegmentListL() : begin");
	TInt found(KErrNone);
	if ( !iURIMappingsReadFromDb )
		{
		ReadAllURIMappingsFromDbL();
		iURIMappingsReadFromDb = ETrue;
		}
	TInt adIndex;
	RArray<TInt> uriIndex;
	found = SearchUriL ( aAdapterId, aURI, EAmongParents, adIndex, uriIndex );
	TSmlDmMappingInfo mappingInfo;
	for ( TInt i=0; i<uriIndex.Count(); i++ )
		{
		mappingInfo.iURISeg = NSmlDmURI::LastURISeg( *iAdapters[adIndex]->
			iMapArray[uriIndex[i]]->iURI );
			
		mappingInfo.iURISegLUID.Set( *iAdapters[adIndex]->
			iMapArray[uriIndex[i]]->iLUID );
			
		aURISegList.AppendL ( mappingInfo );
		}
	uriIndex.Reset();
	_DBG_FILE("CNSmlDmDbHandler::GetURISegmentListL() : end");
//	return KErrNone;
	return found;
	}


// ---------------------------------------------------------------------------
// TInt CNSmlDmDbHandler::WriteMappingInfoToDbL()
// Write mapping info from member variables to db.
// ---------------------------------------------------------------------------
EXPORT_C TInt CNSmlDmDbHandler::WriteMappingInfoToDbL()
	{
	_DBG_FILE("CNSmlDmDbHandler::WriteMappingInfoToDbL() : begin");	
	TBool dbUpdated(EFalse);
	iDatabase.OpenL ( iFileStore, iFileStore->Root() );
	
	for ( TInt i=0; i<iAdapters.Count(); i++ )
		{
		iPrepareView = ETrue;
		for ( TInt u=0; u<iAdapters[i]->iMapArray.Count(); u++ )
			{
			if ( iAdapters[i]->iMapArray[u]->iOperation != EMapNoAction )
				{
				UpdateRowInDbL ( iAdapters[i]->iAdapterId,
					*iAdapters[i]->iMapArray[u]->iURI,
					*iAdapters[i]->iMapArray[u]->iLUID,
					iAdapters[i]->iMapArray[u]->iOperation );
					
				if ( iAdapters[i]->iMapArray[u]->iOperation == EMapDelete )
					{
					delete iAdapters[i]->iMapArray[u];
					iAdapters[i]->iMapArray.Remove(u);
					u--;				
					}
				else
					{
					iAdapters[i]->iMapArray[u]->iOperation = EMapNoAction;					
					}
				dbUpdated = ETrue;								
				}
			}
		}
	if ( dbUpdated )
		{
		iView.Close();
		iDatabase.Compact();
		}
	iDatabase.Close();
	_DBG_FILE("CNSmlDmDbHandler::WriteMappingInfoToDbL() : end");		
	return KErrNone;		
	}


// ---------------------------------------------------------------------------
// TInt CNSmlDmDbHandler::UpdateMappingInfoL ( TUint32 aAdapterId,
//		const TDesC8& aURI, CBufBase &aCurrentList )
// Updates mapping info. 
// ---------------------------------------------------------------------------
EXPORT_C void CNSmlDmDbHandler::UpdateMappingInfoL(TUint32 aAdapterId,
	const TDesC8& aURI,
	CBufBase &aCurrentList)
	{
	_DBG_FILE("CNSmlDmDbHandler::UpdateMappingInfoL() : begin");

	if ( !iURIMappingsReadFromDb )
		{
		ReadAllURIMappingsFromDbL();
		iURIMappingsReadFromDb = ETrue;
		}
	
	TInt startPos(0);
	TInt segEnds(0);
	RPointerArray<HBufC8> currentUris;
	CleanupStack::PushL ( PtrArrCleanupItemRArr ( HBufC8, &currentUris ) );
	
	while ( startPos < ( aCurrentList.Size()  ) )
		{
		TPtrC8 tmpDesc = aCurrentList.Ptr(startPos);
		segEnds = tmpDesc.Find ( KNSmlDmLitSeparator );
		if ( segEnds == KErrNotFound )
		    {
			segEnds = tmpDesc.Length();
		    }
		
		HBufC8* childUri = HBufC8::NewL ( aURI.Length() +
			KNSmlDmLitSeparator().Length() + segEnds );
			
		TPtr8 childUriPtr = childUri->Des();
		childUriPtr.Append ( aURI );
		childUriPtr.Append ( KNSmlDmLitSeparator );
		childUriPtr.Append ( tmpDesc.Left ( segEnds ) );	
		
		TRAPD(err, currentUris.AppendL ( childUri ));
		if(err != KErrNone)
		    {
		    delete childUri;
		    User::Leave( err );
		    }
		        
		startPos += segEnds + 1;
		}
	
	// Search all the uris which have the parent uri matching with the given aURI 
	TBool found(EFalse);
	TInt adIndex(0);
	RArray<TInt> uriIndex;
	SearchUriL ( aAdapterId, aURI, EAmongChildren, adIndex, uriIndex );
	
	for ( TInt i(0); i<uriIndex.Count(); i++ )
		{
		for ( TInt u(0); u<currentUris.Count(); u++ )
			{
			if ( iAdapters[adIndex]->iMapArray[uriIndex[i]]->iURI->
				Compare ( *currentUris[u] ) >= 0  ||
				iAdapters[adIndex]->iMapArray[uriIndex[i]]->iURI->Length() <=
				( aURI.Length() + 1 ) )
				{
				found = ETrue;
				break;
				}
			}
		if ( !found )
			{
			// Mark deleted all the uris which have different child uri
			iAdapters[adIndex]->iMapArray[uriIndex[i]]->iOperation =
				EMapDelete;
				
			DeleteAclL ( *iAdapters[adIndex]->iMapArray[uriIndex[i]]->iURI );			
			}
		found = EFalse;
		}
	
	uriIndex.Reset();
	CleanupStack::PopAndDestroy();  // currentUris
	
	_DBG_FILE("CNSmlDmDbHandler::UpdateMappingInfoL() : end");
	}

// ---------------------------------------------------------------------------
// CNSmlDmDbHandler::DeleteAclL()
// Delete Acl information belongin to the aURI
// ---------------------------------------------------------------------------
EXPORT_C TInt CNSmlDmDbHandler::DeleteAclL(const TDesC8& aURI)
	{
	GetAclDataFromDbL();

	for(TInt i=0;i<iAclURIs.Count();i++)
		{
		if(iAclURIs[i]->Find(aURI)==0)
			{
			delete iAclURIs[i];
			iAclURIs.Remove(i);
			delete iAcls[i];
			iAcls.Remove(i);
			i--;
			}
		}
	WriteAclInfoToDbL();

	return 0;
	}

// ---------------------------------------------------------------------------
// CNSmlDmDbHandler::UpdateAclL()
// Update Acl information
// ---------------------------------------------------------------------------
EXPORT_C TInt CNSmlDmDbHandler::UpdateAclL(const TDesC8& aURI,
	const TDesC8& aACL)
	{
	if (SysUtil::FFSSpaceBelowCriticalLevelL(&iFsSession,
		aURI.Size()+aACL.Size()))
		{
		User::Leave(KErrDiskFull);
		}

	if(aURI.Length()==0||aURI.Compare(KNSmlDmRootUri)==0)
		{
		if(!(aACL.Find(KNSmlDmAclAddForAll)==KErrNone||
			aACL.Compare(KNSmlDmAclAll)==0))
			{
			return KErrAccessDenied;
			}
		}
	if(aACL.Locate(TChar(KNSmlDMAclUriSeparator))>=0)
		{
		return KErrCorrupt;
		}
	GetAclDataFromDbL();

	HBufC8* uri;
	if(aURI.Length()==0)
		{
		uri = HBufC8::NewLC( 1 );
		uri->Des().Format(KNSmlDmRootUri);
		}
	else
		{
		uri = aURI.AllocLC();
		}
	TPtr8 uriPtr = uri->Des();


	TInt uriIndex = KErrNotFound;

	//check if aURI allready exists
	for(TInt i=0;i<iAclURIs.Count();i++)
		{
		if(iAclURIs[i]->Compare(uriPtr)==0)
			{
			uriIndex=i;
			break;
			}
		}

	if(uriIndex!=KErrNotFound)
		{
		delete iAclURIs[uriIndex];
		iAclURIs.Remove(uriIndex);
		delete iAcls[uriIndex];
		iAcls.Remove(uriIndex);
		}


	if(aACL.Length()>0&&aACL.Compare(KNSmlDmLitAclNull)!=0)
		{
		HBufC8* acl = HBufC8::NewLC(aACL.Length() );
		TPtr8 aclPtr = acl->Des();
		aclPtr.Format(aACL);
		
		CleanupStack::Pop(); // acl
		iAcls.AppendL(acl);
		CleanupStack::Pop(); // uri
		iAclURIs.AppendL(uri);
		}
	else
		{
		CleanupStack::PopAndDestroy(); //uri
		}

	WriteAclInfoToDbL();
	return KErrNone;
	}
	
// ---------------------------------------------------------------------------
// CNSmlDmDbHandler::GetAclL()
// Get the acl belonging to aURI
// ---------------------------------------------------------------------------
EXPORT_C TInt CNSmlDmDbHandler::GetAclL(const TDesC8& aURI,
	CBufBase& aACL,
	TBool aInherited)
	{
	_DBG_FILE("CNSmlDmDbHandler::GetAclL() : begin");

	GetAclDataFromDbL();

	TInt ret(KErrNone);
	HBufC8* uri;
	if(aURI.Length()==0)
		{
		uri = HBufC8::NewLC( 1 );
		uri->Des().Format(KNSmlDmRootUri);
		}
	else
		{
		uri = aURI.AllocLC();
		}
	TPtr8 uriPtr = uri->Des();

	if(iAcls.Count()==0||iAclURIs.Count()==0)
		{
		if(aInherited||uriPtr.Compare(KNSmlDmRootUri)==0)
			{
			//no ACL info in database, return default ACL
			aACL.InsertL(0,KNSmlDmAclDefaultRoot);
			ret = KErrNone;
			}
		}
	else
		{
		TBool found = EFalse;
		ret = KErrNotFound;
		//acl is inherited to childs also, in this loop all the parents
		//are checked
		while(uriPtr.Length()!=0 && !found)
			{
			for(TInt i=0;i<iAclURIs.Count();i++)
				{
				if(uriPtr.Compare(iAclURIs[i]->Des())==0)
					{
					//acl belonging to aURI or to any parent is found and
					//returned
					found = ETrue;
					aACL.InsertL(0,iAcls[i]->Des());
					ret = KErrNone;
					break;
					}
				}
			if(found||!aInherited)
				{
				break;
				}
			uriPtr.Format(NSmlDmURI::RemoveLastSeg(uriPtr));
			} //end while

		if(!found&&(aInherited||uriPtr.Compare(KNSmlDmRootUri)==0))
			{
			TBool rootAclFound = EFalse;
			//check if root root acl has changed
			uriPtr.Format(KNSmlDmRootUri);
			for(TInt i=0;i<iAclURIs.Count();i++)
				{
				if(uriPtr.Compare(*iAclURIs[i])==0)
					{
					//acl belonging to root
					rootAclFound = ETrue;
					aACL.InsertL(0,*iAcls[i]);
					ret = KErrNone;
					break;
					}
				}
			if(!rootAclFound)
				{
				//acl not found, return  default
				ret = KErrNone;
				aACL.InsertL(0,KNSmlDmAclDefaultRoot);
				}
			}
		}
	CleanupStack::PopAndDestroy(); //uri

	_DBG_FILE("CNSmlDmDbHandler::GetAclL() : end");
	return ret;
	}


// ---------------------------------------------------------------------------
// TInt CNSmlDmDbHandler::WriteAclInfoToDbL()
// Updates the acl info to database
// ---------------------------------------------------------------------------
EXPORT_C TInt CNSmlDmDbHandler::WriteAclInfoToDbL()
	{
	_DBG_FILE("CNSmlDmDbHandler::WriteAclInfoToDbL() : begin");

	TInt ret = KErrNone;
	if(!iAclUpToDate)
		{
		_DBG_FILE("CNSmlDmDbHandler::WriteAclInfoToDbL() : end (upToDate)");
		return ret;
		}
	if(iAclURIs.Count()!=iAcls.Count())
		{
		_DBG_FILE("CNSmlDmDbHandler::WriteAclInfoToDbL(): end (ACL Count");
		User::Leave(KErrGeneral);
		}


	_LIT(KNSmlDmSQLStatement, "select * from AclTable where AdapterId = %d");
	TInt pushed=0;

	iDatabase.OpenL ( iFileStore, iFileStore->Root() );

	HBufC8* aclURIs=KNullDesC8().AllocLC();
	pushed++;

	TInt i;	
	//write aclURIs from array to HBufC*
	for(i=0;i<iAclURIs.Count();i++)
		{
		CleanupStack::Pop();
		aclURIs = aclURIs->ReAllocL( aclURIs->Length() +
			iAclURIs[i]->Length() + 1 );
		CleanupStack::PushL(aclURIs);
		aclURIs->Des().Append( *iAclURIs[i] );
		aclURIs->Des().Append( KNSmlDmLitMappingSeparator16 );
		}

	HBufC8* acls=KNullDesC8().AllocLC();
	pushed++;
	//write acls from array to HBufC*
	for(i = 0;i<iAcls.Count();i++)
		{
		CleanupStack::Pop();
		acls = acls->ReAllocL( acls->Length() + iAcls[i]->Length() + 1 );
		CleanupStack::PushL(acls);
		acls->Des().Append( *iAcls[i] );
		acls->Des().Append(TChar(KNSmlDMAclUriSeparator));
		}
	
	HBufC* sql = HBufC::NewLC( KNSmlDmSQLStatement().Length()+10);
	pushed++;
	TPtr sqlPtr = sql->Des();
	sqlPtr.Format( KNSmlDmSQLStatement, KNSmlAclDbId );

	ret = iView.Prepare(iDatabase,TDbQuery(sqlPtr,EDbCompareNormal));

	CDbColSet* colSet = iView.ColSetL();
	CleanupStack::PushL(colSet);
	pushed++;
	User::LeaveIfError(iView.EvaluateAll());

	if(ret==KErrNone)
		{
		iView.FirstL();
		if(iView.AtRow())
			{
			iView.GetL();
			iView.UpdateL();
			}
		else
			{
			iView.InsertL();
			iView.SetColL(colSet->ColNo(KNSmlDmAdapterId), KNSmlAclDbId );
			}
		}

	RDbColWriteStream out[2];
	TDbColNo col[2];
	//write data to db
	for(i=0;i<2;i++)
		{
		switch(i)
			{
			case 0:
			col[i] = colSet->ColNo(KNSmlDmAclURI);
			out[i].OpenL(iView,col[i]);
			CleanupClosePushL(out[i]);
			out[i].WriteL(*aclURIs);
			break;

			case 1:
			col[i] = colSet->ColNo(KNSmlDmAcl);
			out[i].OpenL(iView,col[i]);
			CleanupClosePushL(out[i]);
			out[i].WriteL(*acls);
			break;
			
			default:
			User::Panic(KSmlDmTreeDbHandlerPanic,KErrArgument);
			break;
			}
		out[i].CommitL();
		CleanupStack::PopAndDestroy(); //out[i]
		} //for

	iView.PutL();
	iView.Close();
	iDatabase.Compact();
	iDatabase.Close();
	
	//store, colSet, adapters, versions, acls, aclURis
	CleanupStack::PopAndDestroy(pushed);
	
	_DBG_FILE("CNSmlDmDbHandler::WriteAclInfoToDbL() : end");
	return ret;
	}


// ---------------------------------------------------------------------------
// TInt CNSmlDmDbHandler::CheckAclL()
// Check if server has the acl rigths to current uri
// ---------------------------------------------------------------------------
EXPORT_C TBool CNSmlDmDbHandler::CheckAclL(const TDesC8& aURI,
	TNSmlDmCmdType aCmdType)
	{
	_DBG_FILE("CNSmlDmDbHandler::CheckAclL() : begin");
	if(!iServer)
		{
		return EFalse;
		}

	GetAclDataFromDbL();
	
	HBufC8* uri;
	if(aURI.Length()==0)
		{
		uri = HBufC8::NewLC( 1 );
		uri->Des().Format(KNSmlDmRootUri);
		}
	else
		{
		uri = aURI.AllocLC();
		}
	TPtr8 uriPtr = uri->Des();

	TBool found = EFalse;
	TBool ret = EFalse;
	//check if acl is set to this uri or any of it's parents
	while(uriPtr.Length()!=0 && !found)
		{
		for(TInt i=0;i<iAcls.Count();i++)
			{
			if(uriPtr.Compare(*iAclURIs[i])==0)
				{
				found = ETrue;
				break;
				}
			}
		if(found)
			{
			break;
			}
		uriPtr.Format(NSmlDmURI::RemoveLastSeg(uriPtr));
		}

	//check if root acl has been changed
	if(!found)
		{
		uriPtr.Format(KNSmlDmRootUri);
		for(TInt i=0;i<iAcls.Count();i++)
			{
			if(uriPtr.Compare(*iAclURIs[i])==0)
				{
				found = ETrue;
				break;
				}
			}
		}
	
	if(found)
		{
		//acl is found from db
		CBufBase* acl;
		acl = CBufFlat::NewL(1);
		CleanupStack::PushL(acl);

		GetAclL(uriPtr,*acl);

		TPtr8 aclPtr = acl->Ptr(0);
		
		CNSmlDmACLParser* aclParser = CNSmlDmACLParser::NewLC();
		aclParser->ParseL(aclPtr);
		ret = aclParser->HasRights(*iServer,(TNSmlDmCmdType)aCmdType);
		CleanupStack::PopAndDestroy(2);//aclParser, acl
		} //end if(found)
	else
		{
		//acls not found, use default
		switch(aCmdType)
			{
			case EAclAdd:
				ret = ETrue;
				break;

			case EAclGet:
				ret = ETrue;
				break;

			case EAclReplace:
				ret = EFalse;
				break;

			case EAclDelete:
				ret = EFalse;
				break;

			case EAclExecute:
				ret = ETrue;
				break;

			default:
				User::Panic(KSmlDmTreeDbHandlerPanic,KErrArgument);
				break;

			}
		} //end else 


	CleanupStack::PopAndDestroy(); //uri

	_DBG_FILE("CNSmlDmDbHandler::CheckAclL() : end");
	return ret;
	}


// ---------------------------------------------------------------------------
// TInt CNSmlDmDbHandler::DefaultACLsToServerL()
// Add default acl values to current server
// ---------------------------------------------------------------------------
EXPORT_C TInt CNSmlDmDbHandler::DefaultACLsToServerL( const TDesC8& aURI)
	{
	_DBG_FILE("CNSmlDmDbHandler::DefaultACLsToServerL() : begin");
	TPtr8 serverPtr = iServer->Des();

	const TInt length = 5*serverPtr.Length()+KNSmlDmAclAddEqual().Length() +
		KNSmlDmAclGetEqual().Length() +
		KNSmlDmAclReplaceEqual().Length() +
		KNSmlDmAclDeleteEqual().Length() +
		KNSmlDmAclExecEqual().Length() + 4;
		
	HBufC8 *defaultACL = HBufC8::NewLC(length);
	TPtr8 defaultACLptr = defaultACL->Des();
	defaultACLptr.Format(KNSmlDmAclAddEqual);
	defaultACLptr.Append(serverPtr);
	defaultACLptr.Append(KNSmlDmAclSeparator);
	defaultACLptr.Append(KNSmlDmAclDeleteEqual);
	defaultACLptr.Append(serverPtr);
	defaultACLptr.Append(KNSmlDmAclSeparator);
	defaultACLptr.Append(KNSmlDmAclReplaceEqual);
	defaultACLptr.Append(serverPtr);
	defaultACLptr.Append(KNSmlDmAclSeparator);
	defaultACLptr.Append(KNSmlDmAclGetEqual);
	defaultACLptr.Append(serverPtr);
	defaultACLptr.Append(KNSmlDmAclSeparator);
	defaultACLptr.Append(KNSmlDmAclExecEqual);
	defaultACLptr.Append(serverPtr);

	UpdateAclL(aURI,defaultACLptr);
	
	CleanupStack::PopAndDestroy(); //defaultACL
	_DBG_FILE("CNSmlDmDbHandler::DefaultACLsToServerL() : end");
	return KErrNone;
	}

// ---------------------------------------------------------------------------
// CNSmlDmDbHandler::EraseServerIdL
// Removes ACL information for this server id
// ---------------------------------------------------------------------------
EXPORT_C TInt CNSmlDmDbHandler::EraseServerIdL(const TDesC8& aServerId)
	{
	GetAclDataFromDbL();

	for(TInt i=0;i<iAcls.Count();i++)
		{
		CNSmlDmACLParser* acl = CNSmlDmACLParser::NewLC();
		acl->ParseL(*iAcls[i]);
		acl->RemoveAllReferences(aServerId);
		if(acl->ShouldDelete())
			{
			delete iAclURIs[i];
			iAclURIs.Remove(i);
			delete iAcls[i];
			iAcls.Remove(i);
			i--;
			}
		else
			{
			HBufC8* aclBuf=acl->GenerateL();
			delete iAcls[i];
			iAcls[i] = aclBuf;
			}
		CleanupStack::PopAndDestroy(); //acl		
		}
	WriteAclInfoToDbL();

	return 0;
	}


// ---------------------------------------------------------------------------
// TInt CNSmlDmDbHandler::GetAclDataFromDbL(TUint32 aAdapterId)
// Gets the data from tthe db to membervariables
// ---------------------------------------------------------------------------
TInt CNSmlDmDbHandler::GetAclDataFromDbL()
	{
	_DBG_FILE("CNSmlDmDbHandler::GetAclDataFromDbL() : begin");

	TInt ret=KErrNone;
	if(!iAclUpToDate)
		{
		_DBG_FILE("CNSmlDmDbHandler::GetAclDataFromDbL() : Get data from db");
		_LIT(KNSmlDmSQLStatement,
			"select * from AclTable where AdapterId = %d");
			
		TInt pushed=0;

		//get data from database
		iAcls.ResetAndDestroy();
		iAclURIs.ResetAndDestroy();

		iDatabase.OpenL ( iFileStore, iFileStore->Root() );

		HBufC* sql = HBufC::NewLC( KNSmlDmSQLStatement().Length()+10);
		pushed++;
		TPtr sqlPtr = sql->Des();
		sqlPtr.Format( KNSmlDmSQLStatement, KNSmlAclDbId );

		ret = iView.Prepare( iDatabase,TDbQuery(sqlPtr,EDbCompareNormal) );
		HBufC8* aclURIs=0;
		HBufC8* acls=0;
		
		if(ret==KErrNone)
			{
			ret = iView.EvaluateAll();
			//Get the structure of rowset
			CDbColSet* colSet = iView.ColSetL();
			CleanupStack::PushL(colSet);
			pushed++;
			
			if(iView.FirstL())
				{
				iView.GetL();
				}
			else
				{
				iView.Close();
				iDatabase.Close();
				iAclUpToDate = ETrue;
				CleanupStack::PopAndDestroy(pushed);
				return KErrNotFound;
				}
		
			RDbColReadStream in[2];
			TDbColNo col[2];

			//read data from db
			for(TInt i=0;i<2;i++)
				{
				if(i==0)
					{
					col[i] = colSet->ColNo(KNSmlDmAcl);
					in[i].OpenL(iView,col[i]);
					CleanupClosePushL(in[i]);
					acls = HBufC8::NewLC(iView.ColLength(col[i]));
					TPtr8 aclPtr = acls->Des();
					in[i].ReadL(aclPtr,iView.ColLength(col[i]));
					CleanupStack::Pop(); //acls
					CleanupStack::PopAndDestroy(); //in[i]
					CleanupStack::PushL(acls); 
					pushed++;
					}
				else
					{
					col[i] = colSet->ColNo(KNSmlDmAclURI);
					in[i].OpenL(iView,col[i]);
					CleanupClosePushL(in[i]);
					aclURIs = HBufC8::NewLC(iView.ColLength(col[i]));
					TPtr8 uriPtr = aclURIs->Des();
					in[i].ReadL(uriPtr,iView.ColLength(col[i]));
					CleanupStack::Pop(); //aclURIs
					CleanupStack::PopAndDestroy(); //in[i]
					CleanupStack::PushL(aclURIs);
					pushed++;
					}
				} //for
			} //end if(ret==KErrNone)

		//write data to arrays from HBufC*
		if(acls!=0)
			{
			TInt startIndex=0;
			for(TInt i=0;i<acls->Length();i++)
				{
				if(acls[0][i]==KNSmlDMAclUriSeparator)
					{
					HBufC8* tmpAcl = HBufC8::NewLC(i-startIndex);
					tmpAcl->Des() = acls->Mid(startIndex, i-startIndex);
					iAcls.AppendL(tmpAcl);
					CleanupStack::Pop ( tmpAcl );
					startIndex=i+1;
					}
				}
			} //end if(acls!=0)

		//write data to arrays from HBufC*
		if(aclURIs!=0)
			{
			TInt startIndex=0;
			for(TInt i=0;i<aclURIs->Length();i++)
				{
				if(aclURIs[0][i]==KNSmlDMMappingSeparator)
					{
					HBufC8* tmpUri = HBufC8::NewLC(i-startIndex);
					tmpUri->Des() = aclURIs->Mid(startIndex, i-startIndex);
					iAclURIs.AppendL(tmpUri);
					CleanupStack::Pop ( tmpUri );
					startIndex=i+1;
					}
				}
			} //end if(aclUris!=0)


		//colset, store, sql, acls, aclUris
		CleanupStack::PopAndDestroy(pushed);
		
		iView.Close();
		iDatabase.Close();
		iAclUpToDate = ETrue;
		} //end if(aAdapterId!=iAdapterIdAcl)
	
	return ret;
	}


// ---------------------------------------------------------------------------
// CNSmlDmDbHandler::RenameAclL()
// Rename Acl uri and also the children
// ---------------------------------------------------------------------------
TInt CNSmlDmDbHandler::RenameAclL(const TDesC8& aURI, const TDesC8& aNewURI)
	{
	GetAclDataFromDbL();
	TInt ret = KErrNotFound;

	TBool found = EFalse;

	//rename mapping item if found
	for(TInt i=0;i<iAclURIs.Count();i++)
		{
		if( iAclURIs[i]->Find(aURI) == 0 )
			{
			found = ETrue;
			}
		if(iAclURIs[i]->Find(aNewURI)==0 )
			{
			return KErrAlreadyExists;
			}
		}

	if(found)
		{
		for(TInt i=0;i<iAclURIs.Count();i++)
			{
			if( iAclURIs[i]->Find(aURI) == 0 )
				{
				HBufC8* newUri;
				if( iAclURIs[i]->Compare(aURI) == 0 )
					{
					newUri = aNewURI.AllocLC();
					}
				else
					{
					newUri = HBufC8::NewLC(aNewURI.Length() +
						(iAclURIs[i]->Length()-aURI.Length()));
						
					TPtr8 newUriPtr = newUri->Des();
					newUriPtr.Format(aNewURI);
					newUriPtr.Append(iAclURIs[i]->Mid(aURI.Length()));
					}
				HBufC8* acl = iAcls[i];
				delete iAclURIs[i];
				iAclURIs.Remove(i);
				iAcls.Remove(i);

				iAclURIs.AppendL(newUri);
				CleanupStack::Pop(newUri);
				iAcls.AppendL(acl);
				iAclURIs.Compress();
				iAcls.Compress();
				i--;
				}
			}
		ret = KErrNone;
		}
	return ret;	
	}


// ===========================================================================
// CNSmlDmURIMapping
// ===========================================================================

// ---------------------------------------------------------------------------
// CNSmlDmURIMapping::~CNSmlDmURIMapping()
// ---------------------------------------------------------------------------
CNSmlDmURIMapping::~CNSmlDmURIMapping()
	{
	delete iURI;
	delete iLUID;
	}
	
// ---------------------------------------------------------------------------
// CNSmlDmURIMapping* CNSmlDmURIMapping::NewLC()
// ---------------------------------------------------------------------------
CNSmlDmURIMapping* CNSmlDmURIMapping::NewLC()
	{
	CNSmlDmURIMapping* self = new (ELeave) CNSmlDmURIMapping();

	CleanupStack::PushL( self );
	self->ConstructL();

	return self;
	}

// ---------------------------------------------------------------------------
// CNSmlDmURIMapping* CNSmlDmURIMapping::NewLC( const TDesC8& aURI,
//	const TDesC8& aLUID, TNSmlDmMapOperation aOperation )
// ---------------------------------------------------------------------------
CNSmlDmURIMapping* CNSmlDmURIMapping::NewLC( const TDesC8& aURI,
	const TDesC8& aLUID,
	TNSmlDmMapOperation aOperation )
	{
	CNSmlDmURIMapping* self = new (ELeave) CNSmlDmURIMapping();

	CleanupStack::PushL( self );
	self->ConstructL ( aURI, aLUID, aOperation );

	return self;
	}
	
// ---------------------------------------------------------------------------
// void CNSmlDmURIMapping::ConstructL()
// ---------------------------------------------------------------------------
void CNSmlDmURIMapping::ConstructL()
	{
	iURI = HBufC8::NewL(0);
	iLUID = HBufC8::NewL(0);
	iOperation = EMapNoAction;
	}

// ---------------------------------------------------------------------------
// void CNSmlDmURIMapping::ConstructL( const TDesC8& aURI,
//	const TDesC8& aLUID, TNSmlDmMapOperation aOperation )
// ---------------------------------------------------------------------------
void CNSmlDmURIMapping::ConstructL( const TDesC8& aURI,
	const TDesC8& aLUID,
	TNSmlDmMapOperation aOperation )
	{
	iURI = aURI.AllocLC();
	iLUID = aLUID.AllocL();
	iOperation = aOperation;
	CleanupStack::Pop();
	}



// ===========================================================================
// CNSmlDmAdapterElement
// ===========================================================================

// ---------------------------------------------------------------------------
// CNSmlDmAdapterElement::CNSmlDmAdapterElement()
// ---------------------------------------------------------------------------
CNSmlDmAdapterElement::~CNSmlDmAdapterElement()
	{
	iMapArray.ResetAndDestroy();
	}
	
// ---------------------------------------------------------------------------
// CNSmlDmAdapterElement* CNSmlDmAdapterElement::NewLC( TUint32 aAdapterId )
// ---------------------------------------------------------------------------
CNSmlDmAdapterElement* CNSmlDmAdapterElement::NewLC ( TUint32 aAdapterId )
	{
	CNSmlDmAdapterElement* self = new (ELeave) CNSmlDmAdapterElement(aAdapterId);
	CleanupStack::PushL( self );
	return self;
	}
	
// ---------------------------------------------------------------------------
// CNSmlDmAdapterElement::CNSmlDmAdapterElement ( TUint32 aAdapterId )
// ---------------------------------------------------------------------------
CNSmlDmAdapterElement::CNSmlDmAdapterElement ( TUint32 aAdapterId )
	{
	iAdapterId = aAdapterId;
	}

