/*
* Copyright (c) 2002-2006 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Manages IMSI (DBMS) database 
*
*/



// INCLUDE FILES
#include    "CWPImsiDbHandler.h"
#include    <sysutil.h>
#include    "ProvisioningDebug.h"

// CONSTANTS

_LIT( KSimCardDb,   "wpsimcard.db" );
_LIT( KDbColImsi,   "imsi" ); 
_LIT( KDbSimTable,  "sim" );
_LIT( KDbImsiIndex, "I" );
_LIT( KDbNameDrive, "C:" );
const TInt KEmptyDbSizeEstimate     = 2048; // bytes to write
const TInt KImsiSize                = 16;

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CWPImsiDbHandler::CWPImsiDbHandler
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CWPImsiDbHandler::CWPImsiDbHandler()
    {
    }

// -----------------------------------------------------------------------------
// CWPImsiDbHandler::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CWPImsiDbHandler::ConstructL()
    {
    FLOG( _L( "[ProvisioningSC] CWPImsiDbHandler::ConstructL" ) );

	User::LeaveIfError( iFs.Connect() );
    // Get process privatepath
    TInt err = iFs.CreatePrivatePath( EDriveC );
    if( err != KErrAlreadyExists )
    	{
    	User::LeaveIfError( err );
    	}
    User::LeaveIfError( iFs.PrivatePath( iDataBasePath ) );
    iDataBasePath.Insert(0, KDbNameDrive );
    iDataBasePath.Append( KSimCardDb );
 
	// open imsi database
    TInt result( iDb.Open( iFs, iDataBasePath ) );
    FTRACE(RDebug::Print(_L("[ProvisioningSC] CWPImsiDbHandler db open 1 result (%d)"), result));
	if( result == KErrNotFound )
        {
        CreateDatabaseL();
        result = iDb.Open( iFs, iDataBasePath );
        FTRACE(RDebug::Print(_L("[ProvisioningSC] CWPImsiDbHandler db open 2 result (%d)"), result));
        }
    User::LeaveIfError( result );
    if( iDb.IsDamaged() )
        {
        User::LeaveIfError( iDb.Recover() );
        }
	User::LeaveIfError( iImsiTable.Open( iDb, KDbSimTable,
		RDbRowSet::TAccess( RDbRowSet::EUpdatable ) ) );
    }

// -----------------------------------------------------------------------------
// CWPImsiDbHandler::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CWPImsiDbHandler* CWPImsiDbHandler::NewL()
    {
	CWPImsiDbHandler* handler  = CWPImsiDbHandler::NewLC();
    CleanupStack::Pop();
    return handler;
    }

// -----------------------------------------------------------------------------
// CWPImsiDbHandler::NewLC
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CWPImsiDbHandler* CWPImsiDbHandler::NewLC()
    {
    CWPImsiDbHandler* self = new( ELeave ) CWPImsiDbHandler;    
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }
    
// -----------------------------------------------------------------------------
// CWPImsiDbHandler::~CWPImsiDbHandler
// Destructor
// -----------------------------------------------------------------------------
//
CWPImsiDbHandler::~CWPImsiDbHandler()
    {
	iImsiTable.Close();
	iDb.Close();
    iFs.Close();	
    }
// -----------------------------------------------------------------------------
// CWPImsiDbHandler::ImsiExistsL
// Check if imsi exists in the database
// -----------------------------------------------------------------------------
//
TBool CWPImsiDbHandler::ImsiExistsL( const TDesC& aPhoneImsi )
	{
    User::LeaveIfError( iImsiTable.SetIndex( KDbImsiIndex ) );
	TBool result( ETrue );
    if( !iImsiTable.SeekL( TDbSeekKey( aPhoneImsi ) ) )
        {
        result = EFalse;
        }
	return result;
	}
// -----------------------------------------------------------------------------
// CWPImsiDbHandler::StoreImsiL
// Store new imsi into database
// -----------------------------------------------------------------------------
//
void CWPImsiDbHandler::StoreImsiL( const TDesC& aPhoneImsi )
	{
	if( aPhoneImsi == KNullDesC )
		{
		User::Leave( KErrBadDescriptor );
		}
	if( SysUtil::FFSSpaceBelowCriticalLevelL( NULL, KImsiSize ) )
		{
		User::Leave( KErrDiskFull );
		}
	CDbColSet* colSet = iImsiTable.ColSetL();
    TInt imsisCol( colSet->ColNo( KDbColImsi ) );
	delete colSet;
	colSet = NULL;
	// create a view on the database
	RDbView view;
	CleanupClosePushL( view ); // 1
	_LIT( KSQLStatement,"select imsi from sim order by imsi" );
	User::LeaveIfError( view.Prepare( iDb,TDbQuery( KSQLStatement,
						                            EDbCompareNormal )));
	User::LeaveIfError( view.EvaluateAll() );
	
	// begin transaction
	User::LeaveIfError( iDb.Begin() ) ;
	CleanupStack::PushL( TCleanupItem( CleanupRollback, &iDb ) ); // 2
	// insert a row
	view.InsertL();	
	CleanupStack::PushL ( TCleanupItem( CleanupCancel, &view ) ); // 3
	view.SetColL( imsisCol, aPhoneImsi );
	view.PutL();
	CleanupStack::Pop(); // cleanupcancel, 3
	User::LeaveIfError( iDb.Commit() );
	CleanupStack::Pop(); // cleanuprollback, 2
	CleanupStack::PopAndDestroy(); // view, 1
	return;
	}	

// -----------------------------------------------------------------------------
// CWPImsiDbHandler::CreateDatabaseL
// -----------------------------------------------------------------------------
//
void CWPImsiDbHandler::CreateDatabaseL()
	{
	if( SysUtil::FFSSpaceBelowCriticalLevelL( &iFs,	KEmptyDbSizeEstimate ) )
        {
        User::Leave( KErrDiskFull );
        }

    // Get process privatepath
    iDataBasePath.Zero();
    User::LeaveIfError( iFs.PrivatePath( iDataBasePath ) );
    iDataBasePath.Insert(0, KDbNameDrive );
    iDataBasePath.Append( KSimCardDb );
    FLOG( _L( "[ProvisioningSC] CWPImsiDbHandler::CreateDatabaseL priv path" ) );

    FLOG( _L( "[ProvisioningSC] CWPImsiDbHandler::CreateDatabaseL" ) );
    User::LeaveIfError( iDb.Create( iFs, iDataBasePath ) );
    FLOG( _L( "[ProvisioningSC] CWPImsiDbHandler::CreateDatabaseL ok" ) );
	CreateTableL();
	CreateIndexL();
	iDb.Close();
    FLOG( _L( "[ProvisioningSC] CWPImsiDbHandler::CreateDatabaseL done" ) );
	}

// -----------------------------------------------------------------------------
// CWPImsiDbHandler::CreateTableL
// Create database table
// -----------------------------------------------------------------------------
//
void CWPImsiDbHandler::CreateTableL()
	{
    CDbColSet* colset = CDbColSet::NewLC(); // 1
    TDbCol idCol( KDbColImsi, EDbColText16 );
    idCol.iAttributes = TDbCol::ENotNull;
	colset->AddL( idCol );
	User::LeaveIfError( iDb.CreateTable( KDbSimTable, *colset ) );  
	CleanupStack::PopAndDestroy(); // colset
    } 
// -----------------------------------------------------------------------------
// CWPImsiDbHandler::CreateIndexL
// Create database index
// -----------------------------------------------------------------------------
//
void CWPImsiDbHandler::CreateIndexL()
	{
	CDbKey* key = CDbKey::NewLC(); // 1
	TDbKeyCol imsi( KDbColImsi );
	key->AddL( imsi );
	User::LeaveIfError(  iDb.CreateIndex( KDbImsiIndex, 
							KDbSimTable, *key ) ) ;
    CleanupStack::PopAndDestroy( ); // key
	}

// -----------------------------------------------------------------------------
// CWPImsiDbHandler::CleanupRollback
// Create database index
// -----------------------------------------------------------------------------
//
void CWPImsiDbHandler::CleanupRollback( TAny* aItem )
	{
	RDbDatabase* db = static_cast<RDbDatabase*>(aItem);
	db->Rollback();
	}

// -----------------------------------------------------------------------------
// CWPImsiDbHandler::CleanupCancel
// -----------------------------------------------------------------------------
//
void CWPImsiDbHandler::CleanupCancel( TAny* aItem )
    {
    RDbTable* table = static_cast<RDbTable*>(aItem);
    table->Cancel();
    }

//  End of File
