/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Multiple context manager.
*
*/


//  INCLUDE FILES
#include "CWPMultiContextManager.h"
#include <f32file.h>
#include <d32dbms.h>
#include <badesca.h>
#include <sysutil.h>
#include "MWPContextObserver.h"
#include "ProvisioningDebug.h"

// CONSTANTS
_LIT( KDbFullName, "C:\\system\\data\\wpcontext.db" );
_LIT( KDbNameDBMS, "c:wpcontext.db" );
_LIT( KDbTableNames, "A" );
_LIT( KDbIndexNamesUid, "AA" );
_LIT( KDbIndexNamesTPS, "AB" );
_LIT( KDbColumnNamesId, "AC" );
_LIT( KDbColumnNamesName, "AD" );
_LIT( KDbColumnNamesTPS, "AE" );
_LIT( KDbTableProxies, "B" );
_LIT( KDbIndexProxiesContextId, "BA" );
_LIT( KDbColumnProxiesContextId, "BB" );
_LIT( KDbColumnProxiesProxy, "BC" );
const TInt KProxiesGranularity = 3;
const TInt KNewContextSizeEstimate = 128;

#ifdef SYMBIAN_SECURE_DBMS
_LIT(KWPSecureDBMSFormatSpecifier, "SECURE[101F84D6]");
#endif

const TInt KWPDBMSFileValue = 0x101F84D6;
const TUid KWPDBMSFileUid = { KWPDBMSFileValue };

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CWPMultiContextManager::CWPMultiContextManager
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CWPMultiContextManager::CWPMultiContextManager()
: CActive( EPriorityStandard )
    {
    CActiveScheduler::Add( this );
    }

// -----------------------------------------------------------------------------
// CWPMultiContextManager::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CWPMultiContextManager::ConstructL()
    {
    User::LeaveIfError( iDbSession.Connect() );
    OpenDatabaseL();
    }

// -----------------------------------------------------------------------------
// CWPMultiContextManager::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CWPMultiContextManager* CWPMultiContextManager::NewL()
    {
    CWPMultiContextManager* self = NewLC();
    CleanupStack::Pop();
    
    return self;
    }

// -----------------------------------------------------------------------------
// CWPMultiContextManager::NewLC
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CWPMultiContextManager* CWPMultiContextManager::NewLC()
    {
    CWPMultiContextManager* self = new( ELeave ) CWPMultiContextManager;
    
    CleanupStack::PushL( self );
    self->ConstructL();
    
    return self;
    }

// Destructor
CWPMultiContextManager::~CWPMultiContextManager()
    {
    Cancel();

    iNotifier.Close();
    iDataBase.Close();
    iDbSession.Close();
    iObservers.Close();
    }

// -----------------------------------------------------------------------------
// CWPMultiContextManager::OpenDatabaseL
// -----------------------------------------------------------------------------
//
void CWPMultiContextManager::OpenDatabaseL()
    {
    FLOG( _L( "[Provisioning] CWPMultiContextManager::OpenDatabaseL" ) );
    TInt result = KErrNone;

#ifdef SYMBIAN_SECURE_DBMS
	result = iDataBase.Open( iDbSession, KDbNameDBMS, KWPSecureDBMSFormatSpecifier );
	FTRACE(RDebug::Print(_L("[Provisioning] CWPMultiContextManager::OpenDatabaseL: 1 result (%d)"), result));
#else
    iDataBasePath.Zero();
    iDataBasePath.Append( KDbFullName );
    result = iDataBase.Open( iDbSession, iDataBasePath );
    FTRACE(RDebug::Print(_L("[Provisioning] CWPMultiContextManager::OpenDatabaseL: 1 result (%d)"), result));
    
#endif
    if( result == KErrNotFound || result == KErrArgument )
        {
        FLOG( _L( "[Provisioning] CWPMultiContextManager::OpenDatabaseL close and recreate" ) );
        iDataBase.Close();
        CreateDatabaseL();
     #ifdef SYMBIAN_SECURE_DBMS
	    result = iDataBase.Open( iDbSession, KDbNameDBMS, KWPSecureDBMSFormatSpecifier );
     #else
        result = iDataBase.Open( iDbSession, iDataBasePath );
     #endif
        FTRACE(RDebug::Print(_L("[Provisioning] CWPMultiContextManager::OpenDatabaseL: 2 result (%d)"), result));
        }
    User::LeaveIfError( result );
    CheckDatabaseL();

    User::LeaveIfError( iNotifier.Open( iDataBase ) );
    IssueRequest();
    }

// -----------------------------------------------------------------------------
// CWPMultiContextManager::CreateDatabaseL
// -----------------------------------------------------------------------------
//
void CWPMultiContextManager::CreateDatabaseL()
    {
    FLOG( _L( "[Provisioning] CWPMultiContextManager::CreateDatabaseL" ) );
    
    const TName WPContextDbmsTypeName = 
        #ifdef SYMBIAN_SECURE_DBMS
            KWPSecureDBMSFormatSpecifier();
        #else
            KWPDBMSFileUid.Name();
        #endif
    
    RDbNamedDatabase db;
#ifdef SYMBIAN_SECURE_DBMS
    TInt retval = iDbSession.DeleteDatabase( KDbNameDBMS, KWPDBMSFileUid );
    
    retval = iDataBase.Create( iDbSession, KDbNameDBMS, WPContextDbmsTypeName );
    FTRACE(RDebug::Print(_L("[Provisioning] CWPMultiContextManager::CreateDatabaseL: retval (%d)"), retval));
    CleanupClosePushL( iDataBase );

    CreateNamesTableL( iDataBase );
    CreateProxiesTableL( iDataBase );
    CreateDataTableL( iDataBase );

    CleanupStack::PopAndDestroy(); // iDataBase
    
#else
    RFs fs;
    User::LeaveIfError( fs.Connect() );
    CleanupClosePushL( fs );

    if( SysUtil::FFSSpaceBelowCriticalLevelL( &fs, KEmptyDbSizeEstimate ) )
        {
        User::Leave( KErrDiskFull );
        }
    iDataBasePath.Zero();
    iDataBasePath.Append( KDbFullName );
    // Make sure the database file does not exist
    fs.Delete( iDataBasePath );
    CleanupStack::PushL( TCleanupItem( CleanupDeleteDb, &fs ) );

    User::LeaveIfError( db.Create( fs, iDataBasePath ) );
    
    CleanupClosePushL( db );

    CreateNamesTableL( db );
    CreateProxiesTableL( db );
    CreateDataTableL( db );

    CleanupStack::PopAndDestroy(); // db
    CleanupStack::Pop(); // deleteDb
    CleanupStack::PopAndDestroy(); // fs   
#endif
    FLOG( _L( "[Provisioning] CWPMultiContextManager::CreateDatabaseL done:" ) );
    }

// -----------------------------------------------------------------------------
// CWPMultiContextManager::CheckDatabaseL
// -----------------------------------------------------------------------------
//
void CWPMultiContextManager::CheckDatabaseL()
    {
    FLOG( _L( "[Provisioning] CWPMultiContextManager::CheckDatabaseL" ) );
    if( iDataBase.IsDamaged() )
        {
        FLOG( _L( "[Provisioning] CWPMultiContextManager::CheckDatabaseL damaged" ) );
        User::LeaveIfError( iDataBase.Recover() );
        }
    FLOG( _L( "[Provisioning] CWPMultiContextManager::CheckDatabaseL OK" ) );
    }

// -----------------------------------------------------------------------------
// CWPMultiContextManager::CreateNamesTableL
// -----------------------------------------------------------------------------
//
void CWPMultiContextManager::CreateNamesTableL( RDbDatabase& aDb )
    {
    FLOG( _L( "[Provisioning] CWPMultiContextManager::CreateNamesTableL" ) );
    
    // Create the actual table
    CDbColSet* colset = CDbColSet::NewLC();
    TDbCol idCol( KDbColumnNamesId, EDbColUint32 );
    idCol.iAttributes = TDbCol::EAutoIncrement;
    colset->AddL( idCol );
    TDbCol nameCol( KDbColumnNamesName, EDbColText16 );
    colset->AddL( nameCol );
    TDbCol tpsCol( KDbColumnNamesTPS, EDbColText16 );
    tpsCol.iAttributes = TDbCol::ENotNull;
    colset->AddL( tpsCol );
    User::LeaveIfError( aDb.CreateTable( KDbTableNames, *colset ) );
    CleanupStack::PopAndDestroy(); // colset

    // Index for searching by UID
    CDbKey* key = CDbKey::NewLC();
    TDbKeyCol idkeyCol( KDbColumnNamesId );
    key->AddL( idkeyCol );
    User::LeaveIfError( 
        aDb.CreateIndex( KDbIndexNamesUid, KDbTableNames, *key ) );
    CleanupStack::PopAndDestroy(); // key

    // Index for searching by TPS
    key = CDbKey::NewLC();
    TDbKeyCol tpskeyCol( KDbColumnNamesTPS );
    key->AddL( tpskeyCol );
    User::LeaveIfError( 
        aDb.CreateIndex( KDbIndexNamesTPS, KDbTableNames, *key ) );
    CleanupStack::PopAndDestroy(); // key
    }

// -----------------------------------------------------------------------------
// CWPMultiContextManager::CreateProxiesTableL
// -----------------------------------------------------------------------------
//
void CWPMultiContextManager::CreateProxiesTableL( RDbDatabase& aDb )
    {
    FLOG( _L( "[Provisioning] CWPMultiContextManager::CreateProxiesTableL" ) );
    
    // Create the actual table
    CDbColSet* colset = CDbColSet::NewLC();
    TDbCol contextIdCol( KDbColumnProxiesContextId, EDbColUint32 );
    contextIdCol.iAttributes = TDbCol::ENotNull;
    colset->AddL( contextIdCol );
    TDbCol proxyCol( KDbColumnProxiesProxy, EDbColText );
    proxyCol.iAttributes = TDbCol::ENotNull;
    colset->AddL( proxyCol );
    User::LeaveIfError( aDb.CreateTable( KDbTableProxies, *colset ) );
    CleanupStack::PopAndDestroy(); // colset

    // Index for searching by context UID
    CDbKey* key = CDbKey::NewLC();
    TDbKeyCol contextkeyCol( KDbColumnProxiesContextId );
    key->AddL( contextkeyCol );
    User::LeaveIfError( 
        aDb.CreateIndex( KDbIndexProxiesContextId, KDbTableProxies, *key ) );
    CleanupStack::PopAndDestroy(); // key
    }

// -----------------------------------------------------------------------------
// CWPMultiContextManager::CreateDataTableL
// -----------------------------------------------------------------------------
//
void CWPMultiContextManager::CreateDataTableL( RDbDatabase& aDb )
    {
    FLOG( _L( "[Provisioning] CWPMultiContextManager::CreateDataTableL" ) );
    
    // Create the actual table
    CDbColSet* colset = CDbColSet::NewLC();
    TDbCol contextIdCol( KDbColumnDataContextId, EDbColUint32 );
    contextIdCol.iAttributes = TDbCol::ENotNull;
    colset->AddL( contextIdCol );
    TDbCol adapterIdCol( KDbColumnDataAdapterId, EDbColUint32 );
    adapterIdCol.iAttributes = TDbCol::ENotNull;
    colset->AddL( adapterIdCol );
    TDbCol saveDataCol( KDbColumnDataSaveData, EDbColBinary );
    colset->AddL( saveDataCol );
    User::LeaveIfError( aDb.CreateTable( KDbTableData, *colset ) );
    CleanupStack::PopAndDestroy(); // colset

    // Index for searching by context UID and adapter UID
    CDbKey* key = CDbKey::NewLC();
    TDbKeyCol contextkeyCol( KDbColumnDataContextId );
    key->AddL( contextkeyCol );
    TDbKeyCol adapterkeyCol( KDbColumnDataAdapterId );
    key->AddL( adapterkeyCol );
    User::LeaveIfError( 
        aDb.CreateIndex( KDbIndexDataContextId, KDbTableData, *key ) );
    CleanupStack::PopAndDestroy(); // key
    }

// -----------------------------------------------------------------------------
// CWPMultiContextManager::CreateContextL
// -----------------------------------------------------------------------------
//
TUint32 CWPMultiContextManager::CreateContextL( const TDesC& aName,
                                                const TDesC& aTPS, 
                                                const MDesCArray& aProxies )
    {
    FLOG( _L( "[Provisioning] CWPMultiContextManager::CreateContextL" ) );

    CheckDatabaseL();

    // Check if a context has been created for the particular TPS
    if( ContextExistsL( aTPS ) )
        {
        User::Leave( KErrAlreadyExists );
        }

    if( SysUtil::FFSSpaceBelowCriticalLevelL( NULL, KNewContextSizeEstimate ) )
        {
        User::Leave( KErrDiskFull );
        }

    RDbTable contexts;
    OpenLC( KDbTableNames, contexts );

    CDbColSet* colSet = contexts.ColSetL();
    TInt contextNameCol( colSet->ColNo( KDbColumnNamesName ) );
    TInt contextTpsCol( colSet->ColNo( KDbColumnNamesTPS ) );
    TInt contextColIdCol( colSet->ColNo( KDbColumnNamesId ) );
    delete colSet;
    colSet = NULL;

    RDbTable proxies;
    OpenLC( KDbTableProxies, proxies );
    colSet = proxies.ColSetL();
    TInt proxiesContextIdCol( colSet->ColNo( KDbColumnProxiesContextId ) );
    TInt proxiesProxyCol( colSet->ColNo( KDbColumnProxiesProxy ) );
    delete colSet;
    colSet = NULL;

    // All checks done. Now just write the data.
    DatabaseBeginLC();
    InsertBeginLC( contexts );
    contexts.SetColL( contextNameCol, aName );
    contexts.SetColL( contextTpsCol, aTPS );
    TUint32 uid( contexts.ColUint32( contextColIdCol ) );
    InsertCommitLP( contexts );
    
    for( TInt i( 0 ); i < aProxies.MdcaCount(); i++ )
        {
        InsertBeginLC( proxies );
        proxies.SetColL( proxiesContextIdCol, uid );
        proxies.SetColL( proxiesProxyCol, aProxies.MdcaPoint(i) );
        InsertCommitLP( proxies );
        }
    
    DatabaseCommitLP();

    CleanupStack::PopAndDestroy(2); // proxies, contexts
    return uid;
    }

// -----------------------------------------------------------------------------
// CWPMultiContextManager::ContextUidsL
// -----------------------------------------------------------------------------
//
CArrayFix<TUint32>* CWPMultiContextManager::ContextUidsL()
    {
    FLOG( _L( "[Provisioning] CWPMultiContextManager::ContextUidsL" ) );

    CheckDatabaseL();

    RDbTable contexts;
    OpenLC( KDbTableNames, contexts, RDbRowSet::EReadOnly );
    CDbColSet* colset = contexts.ColSetL();
    TInt uidCol( colset->ColNo( KDbColumnNamesId ) );
    delete colset;

    CArrayFix<TUint32>* array = 
        new(ELeave) CArrayFixFlat<TUint32>( Max( 1, contexts.CountL() ) );
    CleanupStack::PushL( array );
    FTRACE(RDebug::Print(_L("[Provisioning] CWPMultiContextManager::ContextUidsL: count (%d)"), contexts.CountL()));
    if ( contexts.FirstL())
    {    	
    	while( contexts.AtRow() )
        {
        FLOG( _L( "[Provisioning] CWPMultiContextManager::ContextUidsL contexts.GetL()" ) );
        contexts.GetL();

        TUint32 uid( contexts.ColUint32( uidCol ) );
        array->AppendL( uid );
        contexts.NextL();
        }
     }

    CleanupStack::Pop(); // array
    CleanupStack::PopAndDestroy(); // contexts

    return array;
    }

// -----------------------------------------------------------------------------
// CWPMultiContextManager::ContextNameL
// -----------------------------------------------------------------------------
//
HBufC* CWPMultiContextManager::ContextNameL( TUint32 aUid )
    {
    FLOG( _L( "[Provisioning] CWPMultiContextManager::ContextNameL" ) );
    
    CheckDatabaseL();

    RDbTable contexts;
    OpenLC( KDbTableNames, contexts, RDbRowSet::EReadOnly );

    User::LeaveIfError( contexts.SetIndex( KDbIndexNamesUid ) );
    if( !contexts.SeekL( TDbSeekKey( TUint( aUid ) ) ) )
        {
        User::Leave( KErrNotFound );
        }

    CDbColSet* colset = contexts.ColSetL();
    TInt nameCol( colset->ColNo( KDbColumnNamesName ) );
    delete colset;
    
    contexts.GetL();
    HBufC* name = contexts.ColDes16( nameCol ).AllocL();

    CleanupStack::PopAndDestroy(); // contexts
    return name;
    }

// -----------------------------------------------------------------------------
// CWPMultiContextManager::ContextTPSL
// -----------------------------------------------------------------------------
//
HBufC* CWPMultiContextManager::ContextTPSL( TUint32 aUid )
    {
    FLOG( _L( "[Provisioning] CWPMultiContextManager::ContextTPSL" ) );
    CheckDatabaseL();

    RDbTable contexts;
    OpenLC( KDbTableNames, contexts, RDbRowSet::EReadOnly );

    User::LeaveIfError( contexts.SetIndex( KDbIndexNamesUid ) );
    if( !contexts.SeekL( TDbSeekKey( TUint( aUid ) ) ) )
        {
        User::Leave( KErrNotFound );
        }

    CDbColSet* colset = contexts.ColSetL();
    TInt tpsCol( colset->ColNo( KDbColumnNamesTPS ) );
    delete colset;
    
    contexts.GetL();
    HBufC* tps = contexts.ColDes16( tpsCol ).AllocL();

    CleanupStack::PopAndDestroy(); // contexts
    FLOG( _L( "[Provisioning] CWPMultiContextManager::ContextTPSL done" ) );
    return tps;
    }

// -----------------------------------------------------------------------------
// CWPMultiContextManager::ContextProxiesL
// -----------------------------------------------------------------------------
//
CDesCArray* CWPMultiContextManager::ContextProxiesL( TUint32 aUid )
    {
    FLOG( _L( "[Provisioning] CWPMultiContextManager::ContextProxiesL" ) );
    
    CheckDatabaseL();

    RDbTable proxies;
    OpenLC( KDbTableProxies, proxies, RDbRowSet::EReadOnly );

    CDbColSet* colset = proxies.ColSetL();
    TInt contextCol( colset->ColNo( KDbColumnProxiesContextId ) );
    TInt proxyCol( colset->ColNo( KDbColumnProxiesProxy ) );
    delete colset;

    User::LeaveIfError( proxies.SetIndex( KDbIndexProxiesContextId ) );
    proxies.SeekL( TDbSeekKey( TUint( aUid ) ) );

    CDesCArray* array = new(ELeave) CDesCArrayFlat( KProxiesGranularity );
    CleanupStack::PushL( array );

    while( proxies.AtRow()
        && (proxies.GetL(), proxies.ColUint32( contextCol ) == aUid ) )
        {
        array->AppendL( proxies.ColDes16( proxyCol ) );
        proxies.NextL();
        }
    
    CleanupStack::Pop(); // array
    CleanupStack::PopAndDestroy(); // proxies
    
    FLOG( _L( "[Provisioning] CWPMultiContextManager::ContextProxiesL done" ) );
    return array;
    }

// -----------------------------------------------------------------------------
// CWPMultiContextManager::DeleteContextL
// -----------------------------------------------------------------------------
//
void CWPMultiContextManager::DeleteContextL( 
                                          MWPContextExtensionArray& /*aArray*/, 
                                          TUint32 aUid )
    {
    FLOG( _L( "[Provisioning] CWPMultiContextManager::DeleteContextL" ) );
    
    CheckDatabaseL();

    // If we're deleting the current context, set the current context to null
    if( CurrentContextL() == aUid )
        {
        SetCurrentContextL( KWPMgrUidNoContext );
        }

    RDbTable contexts;
    OpenLC( KDbTableNames, contexts );
    
    // Find the line in names table and delete it
    User::LeaveIfError( contexts.SetIndex( KDbIndexNamesUid ) );
    if( !contexts.SeekL( TDbSeekKey( TUint( aUid ) ) ) )
        {
        User::Leave( KErrNotFound );
        }
    contexts.DeleteL();
    CleanupStack::PopAndDestroy(); // contexts
    }

// -----------------------------------------------------------------------------
// CWPMultiContextManager::DeleteContextDataL
// -----------------------------------------------------------------------------
//
TBool CWPMultiContextManager::DeleteContextDataL( 
                                          MWPContextExtensionArray& /*aArray*/,
                                          TUint32 /*aUid*/ )
    {
    FLOG( _L( "[Provisioning] CWPMultiContextManager::DeleteContextDataL" ) );
    
    // Deleting not supported by MultiContextManager
    return EFalse;
    }

// -----------------------------------------------------------------------------
// CWPMultiContextManager::ContextDataCountL
// -----------------------------------------------------------------------------
//
TInt CWPMultiContextManager::ContextDataCountL( TUint32 /*aUid*/ )
    {
    FLOG( _L( "[Provisioning] CWPMultiContextManager::ContextDataCountL" ) );
    
    // Context data gathering not supported by MultiContextManager
    return 0;
    }

// -----------------------------------------------------------------------------
// CWPMultiContextManager::CurrentContextL
// -----------------------------------------------------------------------------
//
TUint32 CWPMultiContextManager::CurrentContextL()
    {
    FLOG( _L( "[Provisioning] CWPMultiContextManager::CurrentContextL" ) );
    
    return iCurrentContext;
    }

// -----------------------------------------------------------------------------
// CWPMultiContextManager::SetCurrentContextL
// -----------------------------------------------------------------------------
//
void CWPMultiContextManager::SetCurrentContextL( TUint32 aUid )
    {
    FLOG( _L( "[Provisioning] CWPMultiContextManager::SetCurrentContextL" ) );
    
    iCurrentContext = aUid;
    }

// -----------------------------------------------------------------------------
// CWPMultiContextManager::ContextExistsL
// -----------------------------------------------------------------------------
//
TBool CWPMultiContextManager::ContextExistsL( TUint32 aUid )
    {
    FLOG( _L( "[Provisioning] CWPMultiContextManager::ContextExistsL(uid)" ) );
    CheckDatabaseL();

    RDbTable contexts;
    OpenLC( KDbTableNames, contexts, RDbRowSet::EReadOnly );

    User::LeaveIfError( contexts.SetIndex( KDbIndexNamesUid ) );
    TBool result( contexts.SeekL( TDbSeekKey( TUint( aUid ) ) ) );

    CleanupStack::PopAndDestroy(); // contexts
    return result;
    }

// -----------------------------------------------------------------------------
// CWPMultiContextManager::ContextExistsL
// -----------------------------------------------------------------------------
//
TBool CWPMultiContextManager::ContextExistsL( const TDesC& aTPS )
    {
    FLOG( _L( "[Provisioning] CWPMultiContextManager::ContextExistsL(aTPS)" ) );
    CheckDatabaseL();

    RDbTable contexts;
    OpenLC( KDbTableNames, contexts, RDbRowSet::EReadOnly );

    User::LeaveIfError( contexts.SetIndex( KDbIndexNamesTPS ) );
    TBool result( contexts.SeekL( TDbSeekKey( aTPS ) ) );

    CleanupStack::PopAndDestroy(); // contexts
    return result;
    }

// -----------------------------------------------------------------------------
// CWPMultiContextManager::ContextL
// -----------------------------------------------------------------------------
//
TUint32 CWPMultiContextManager::ContextL( const TDesC& aTPS )
    {
    FLOG( _L( "[Provisioning] CWPMultiContextManager::ContextL" ) );
    CheckDatabaseL();

    RDbTable contexts;
    OpenLC( KDbTableNames, contexts, RDbRowSet::EReadOnly );

    User::LeaveIfError( contexts.SetIndex( KDbIndexNamesTPS ) );
    if( !contexts.SeekL( TDbSeekKey( aTPS ) ) )
        {
        User::Leave( KErrNotFound );
        }
    contexts.GetL();

    CDbColSet* colset = contexts.ColSetL();
    TUint32 uid( contexts.ColUint32( colset->ColNo( KDbColumnNamesId ) ) );
    delete colset;

    CleanupStack::PopAndDestroy(); // contexts
    return uid;
    }

// -----------------------------------------------------------------------------
// CWPMultiContextManager::SaveL
// -----------------------------------------------------------------------------
//
void CWPMultiContextManager::SaveL( MWPContextExtension& /*aExtension*/,
                                    TInt /*aItem*/ )
    {
    // Context save data not gathered by MultiContextManager
    }

// -----------------------------------------------------------------------------
// CWPMultiContextManager::DatabaseBeginLC
// -----------------------------------------------------------------------------
//
void CWPMultiContextManager::DatabaseBeginLC()
    {
    FLOG( _L( "[Provisioning] CWPMultiContextManager::DatabaseBeginLC" ) );
    
    User::LeaveIfError( iDataBase.Begin() );
    CleanupStack::PushL( TCleanupItem( CleanupRollback, &iDataBase ) );
    }

// -----------------------------------------------------------------------------
// CWPMultiContextManager::DatabaseCommitLP
// -----------------------------------------------------------------------------
//
void CWPMultiContextManager::DatabaseCommitLP()
    {
    FLOG( _L( "[Provisioning] CWPMultiContextManager::DatabaseCommitLP" ) );
    
    User::LeaveIfError( iDataBase.Commit() );
    CleanupStack::Pop(); // CleanupRollback
    }

// -----------------------------------------------------------------------------
// CWPMultiContextManager::CleanupRollback
// -----------------------------------------------------------------------------
//
void CWPMultiContextManager::CleanupRollback( TAny* aItem )
    {
    FLOG( _L( "[Provisioning] CWPMultiContextManager::CleanupRollback" ) );
    
    RDbDatabase* db = static_cast<RDbDatabase*>(aItem);
    db->Rollback();
    }

// -----------------------------------------------------------------------------
// CWPMultiContextManager::InsertBeginLC
// -----------------------------------------------------------------------------
//
void CWPMultiContextManager::InsertBeginLC( RDbTable& aTable ) const
    {
    FLOG( _L( "[Provisioning] CWPMultiContextManager::InsertBeginLC" ) );
    
    aTable.InsertL();
    CleanupStack::PushL( TCleanupItem( CleanupCancel, &aTable ) );
    }

// -----------------------------------------------------------------------------
// CWPMultiContextManager::InsertCommitLP
// -----------------------------------------------------------------------------
//
void CWPMultiContextManager::InsertCommitLP( RDbTable& aTable ) const
    {
    FLOG( _L( "[Provisioning] CWPMultiContextManager::InsertCommitLP" ) );
    
    aTable.PutL();
    CleanupStack::Pop(); // Cleanup
    }

// -----------------------------------------------------------------------------
// CWPMultiContextManager::CleanupCancel
// -----------------------------------------------------------------------------
//
void CWPMultiContextManager::CleanupCancel( TAny* aItem )
    {
    FLOG( _L( "[Provisioning] CWPMultiContextManager::CleanupCancel" ) );
    
    RDbTable* table = static_cast<RDbTable*>(aItem);
    table->Cancel();
    }

// -----------------------------------------------------------------------------
// CWPMultiContextManager::CleanupDeleteDb
// -----------------------------------------------------------------------------
//
void CWPMultiContextManager::CleanupDeleteDb( TAny* aItem )
    {
    FLOG( _L( "[Provisioning] CWPMultiContextManager::CleanupDeleteDb" ) );
    
    RFs* fs = static_cast<RFs*>(aItem);
    TFileName path;
    path.Append( KDbFullName ); 
    fs->Delete( path );
    }

// -----------------------------------------------------------------------------
// CWPMultiContextManager::OpenLC
// -----------------------------------------------------------------------------
//
void CWPMultiContextManager::OpenLC( const TDesC& aName, RDbTable& aTable, 
                                    RDbTable::TAccess aAccess )
    {
    FLOG( _L( "[Provisioning] CWPMultiContextManager::OpenLC" ) );
    User::LeaveIfError( aTable.Open( iDataBase, aName, aAccess ) );
    FLOG( _L( "[Provisioning] CWPMultiContextManager::OpenLC OK" ) );
    CleanupClosePushL( aTable );
    }

// -----------------------------------------------------------------------------
// CWPMultiContextManager::RunL
// -----------------------------------------------------------------------------
//
void CWPMultiContextManager::RunL()
    {
    FLOG( _L( "[Provisioning] CWPMultiContextManager::RunL" ) );
    
    for( TInt i( 0 ); i < iObservers.Count(); i++ )
        {
        iObservers[i]->ContextChangeL( RDbNotifier::TEvent( iStatus.Int() ) );
        }
    IssueRequest();
    }

TInt CWPMultiContextManager::RunError ( TInt /*aError*/ )
    {
        return KErrNone;
    }	
// -----------------------------------------------------------------------------
// CWPMultiContextManager::DoCancel
// -----------------------------------------------------------------------------
//
void CWPMultiContextManager::DoCancel()
    {
    FLOG( _L( "[Provisioning] CWPMultiContextManager::DoCancel" ) );
    
    iNotifier.Cancel();
    }

// -----------------------------------------------------------------------------
// CWPMultiContextManager::IssueRequest
// -----------------------------------------------------------------------------
//
void CWPMultiContextManager::IssueRequest()
    {
    FLOG( _L( "[Provisioning] CWPMultiContextManager::IssueRequest" ) );

    iNotifier.NotifyChange( iStatus );
    SetActive();
    }

// -----------------------------------------------------------------------------
// CWPMultiContextManager::RegisterObserverL
// -----------------------------------------------------------------------------
//
void CWPMultiContextManager::RegisterContextObserverL( 
    MWPContextObserver* aObserver )
    {
    FLOG( _L( "[Provisioning] CWPMultiContextManager::RegisterContextObserverL" ) );
    
    User::LeaveIfError( iObservers.Append( aObserver ) );
    }

// -----------------------------------------------------------------------------
// CWPMultiContextManager::UnregisterContextObserver
// -----------------------------------------------------------------------------
//
TInt CWPMultiContextManager::UnregisterContextObserver( 
    MWPContextObserver* aObserver )
    {
    FLOG( _L( "[Provisioning] CWPMultiContextManager::UnregisterContextObserver" ) );
    
    TInt result( iObservers.Find( aObserver ) );

    if( result >= 0 )
        {
        iObservers.Remove( result );
        result = KErrNone;
        }

    return result;
    }


//  End of File  
