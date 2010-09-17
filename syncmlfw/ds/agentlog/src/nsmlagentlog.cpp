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
* Description:  AgentLog db-interface
*
*/



// INCLUDE FILES
#include <sysutil.h>
#include <bautils.h>
#include <utf.h>
#include <nsmlconstants.h>
#include <nsmldebug.h>

#include "nsmldbcaps.h"
#include "nsmldsdefines.h"
#include "nsmlagentlog.h"
#include "smldevinfdtd.h"


// CONSTANTS

// Size of empty AgentLog database in bytes.
const TInt KNSmlAgentLogEmptyDatabaseSize = 580;
// Estimated maximum size of AgentLog table row without the unrestricted
// ServerDeviceInfo and ServerFilterInfo fields.
const TInt KNSmlAgentLogMaxFixedLogRowSize = 860;
// Maximum size of Authentication table row without unrestricted Nonce field.
const TInt KNSmlAgentLogMaxFixedAuthRowSize = 320;
// Additional treshold used with OOD checks.
const TInt KNSmlAgentLogTreshold = 50;

_LIT8( KTabSeparator, " ");

//=============================================
//
//		CNSmlDSAgentLog
//
//=============================================

//=============================================
//		CNSmlDSAgentLog::NewL()
//		Creates a new instance of CNSmlDSAgentLog object.
//=============================================

EXPORT_C CNSmlDSAgentLog* CNSmlDSAgentLog::NewL()
	{
	DBG_FILE(_S8("CNSmlDSAgentLog::NewL"));
	CNSmlDSAgentLog* self = CNSmlDSAgentLog::NewLC();	
	CleanupStack::Pop();
	return self;
	}	

//=============================================
//		CNSmlDSAgentLog::NewLC()
//		Creates a new instance of CNSmlDSAgentLog object. 
//		Pushes and leaves new instance into CleanupStack.
//=============================================
EXPORT_C CNSmlDSAgentLog* CNSmlDSAgentLog::NewLC()
	{
	DBG_FILE(_S8("CNSmlDSAgentLog::NewLC"));
	CNSmlDSAgentLog* self = new ( ELeave ) CNSmlDSAgentLog;
	CleanupStack::PushL( self );
	self->ConstructL();	
	return self;
	}

//=============================================
//		CNSmlDSAgentLog::ConstructL()
//		Second phase constructor.
//=============================================

void CNSmlDSAgentLog::ConstructL()
	{
	DBG_FILE(_S8("CNSmlDSAgentLog::ConstructL(): begin"));
	iLocalDB = HBufC::NewL( KNSmlMaxStringColumnLength );
	iServerId = HBufC::NewL( KNSmlMaxServerIdLength );
	iRemoteDB = HBufC::NewL( KNSmlMaxStringColumnLength );

	User::LeaveIfError( iFsSession.Connect() );	
	User::LeaveIfError( iRdbSession.Connect() );
    iFsSession.SetSessionToPrivate( KNSmlAgentLogDbDrive );

    TParse name;
    
    TBool createdDatabase = EFalse;

#ifdef SYMBIAN_SECURE_DBMS
	name.Set( KNSmlAgentLogDbName(), NULL, NULL );
#else
    name.Set( KNSmlAgentLogDbName(), KNSmlDatabasesNonSecurePath, NULL );
#endif
	// PP: data compatibility fix: If incompatible database is 
    // found, it will be deleted & recreated.
    TInt retry0(5);
    while ( retry0>0 )
        {
        retry0 = retry0 - 1;
	    TInt err = iDatabase.Open( iRdbSession /*iFsSession*/,
	                               name.FullName(),
	                               KNSmlDBMSSecureSOSServerID );
	    if (err == KErrNone)
	    	{
	    	User::LeaveIfError( iTableAgentLog.Open( iDatabase, KNSmlTableAgentLog ) );
	    	iColSet = iTableAgentLog.ColSetL();
	    
	    	//RD_SUSPEND_RESUME	
	    	if (iColSet->ColNo( KNSmlAgentLogSuspendedState ) == KDbNullColNo
	    			|| iColSet->ColNo( KNSmlAgentLogPreviousSyncType ) == KDbNullColNo 
	    			|| iColSet->ColNo( KNSmlAgentLogPreviousClientSyncType ) == KDbNullColNo)
       			{
	    		iTableAgentLog.Close();
	    		iDatabase.Close();
	    		TInt error = iRdbSession.DeleteDatabase( name.FullName(), KNSmlSOSServerPolicyUID );
       			if (error == KErrNone)
	       			{
	       			err = KErrNotFound;	
	       			}
       			
       			}
	    	}
	    	
	    if ( err == KErrNotFound )
		    {
		    CreateDatabaseL( name.FullName() );
		    User::LeaveIfError( iDatabase.Open( iRdbSession,
		                                        name.FullName(),
		                                        KNSmlDBMSSecureSOSServerID ) );
		    createdDatabase = ETrue;
		    }
	    else
		    {
		    User::LeaveIfError( err );
		    }
		    
		if (createdDatabase) 
			{
			User::LeaveIfError( iTableAgentLog.Open( iDatabase, KNSmlTableAgentLog ) );
		    iColSet = iTableAgentLog.ColSetL();
			}
	      	    
	    User::LeaveIfError( iTableAuthentication.Open( iDatabase, KNSmlTableAuthentication ) );
	    iColSetTableAuth = iTableAuthentication.ColSetL();
        TInt err2 = iTableDatastore.Open( iDatabase, KNSmlTableDatastore() );
        // Table datastore missing => must delete db
        if ( err2==KErrNotFound )
            {
	        delete iColSetTableDS;
	        iColSetTableDS = NULL;
	        delete iColSetTableAuth;
	        iColSetTableAuth = NULL;
            iTableAgentLog.Close();
            iTableAuthentication.Close();
            User::LeaveIfError(iDatabase.Destroy());
            }
        else
            {
            retry0=0;
	        User::LeaveIfError( err2 );
            }
        }
	iColSetTableDS = iTableDatastore.ColSetL();
	        
	DBG_FILE(_S8("CNSmlDSAgentLog::ConstructL(): end"));
	}

//=============================================
//		CNSmlDSAgentLog::~CNSmlDSAgentLog()
//		Destructor.
//=============================================

CNSmlDSAgentLog::~CNSmlDSAgentLog()
	{
	DBG_FILE(_S8("CNSmlDSAgentLog::~CNSmlDSAgentLog begins"));

	delete iLocalDB;
	delete iServerId;
	delete iRemoteDB;

	iView.Close();
	delete iColSet;
	iTableAgentLog.Close();
	
// <MAPINFO_RESEND_MOD_BEGIN>
	delete iColSetTableDS;
	iTableDatastore.Close();
// <MAPINFO_RESEND_MOD_END>
		
	delete iColSetTableAuth;
	iTableAuthentication.Close();

	iDatabase.Close();
	iFsSession.Close();
	iRdbSession.Close();
	
	DBG_FILE(_S8("CNSmlDSAgentLog::~CNSmlDSAgentLog ends"));
	}

//=============================================
//		CNSmlDSAgentLog::CreateDatabaseL()
//		Creates Agent Log database.
//=============================================

void CNSmlDSAgentLog::CreateDatabaseL(const TDesC& aFullName)
	{
	DBG_FILE(_S8("CNSmlDSAgentLog::CreateDatabaseL begins"));

    // Check OOD before crating new AgentLog database
    if ( SysUtil::FFSSpaceBelowCriticalLevelL( &iFsSession,
            KNSmlAgentLogEmptyDatabaseSize + KNSmlAgentLogTreshold ) )
	    {
        User::Leave( KErrDiskFull );
		}

	_LIT( KCreateAgentLogTable, "CREATE TABLE AgentLog ( Id INTEGER NOT NULL, \
ImplementationUID INTEGER NOT NULL,  LocalDatabase CHAR(%d) NOT NULL, \
ServerId CHAR(%d), RemoteDatabase CHAR(%d) NOT NULL, \
LastSyncronised TIMESTAMP, SyncAnchor TIMESTAMP, SlowSyncRequest INTEGER, \
ServerDeviceInfo LONG VARCHAR, ServerSupportsNOC BIT, ServerSyncTypes INTEGER, \
ServerFilterInfo LONG VARCHAR, ServerSupportsHierarchicalSync BIT ,\
SyncSuspendedState INTEGER,PreviousSyncType INTEGER,\
PreviousClientSyncType INTEGER)" );

//RD_SUSPEND_RESUME- 3 new columns added
	_LIT( KCreateAuthenticationTable, "CREATE TABLE Authentication ( ServerId \
CHAR(%d), Nonce LONG VARCHAR, Type INTEGER, SessionId INTEGER )" );

// <MAPINFO_RESEND_MOD_BEGIN>
	_LIT( KCreateDatastoreTable, "CREATE TABLE %S ( %S COUNTER NOT NULL, \
	%S INTEGER NOT NULL, %S INTEGER NOT NULL, %S LONG VARBINARY )" );
// <MAPINFO_RESEND_MOD_END>

	_LIT( KCreateVersionTable, "CREATE TABLE Version ( \
VerMajor UNSIGNED TINYINT, VerMinor UNSIGNED TINYINT )" );

	HBufC* createAgentLogTable = HBufC::NewLC( KCreateAgentLogTable().Length() + 20 );
	TPtr agentLogTablePtr = createAgentLogTable->Des();
	HBufC* createAuthenticationTable = HBufC::NewLC( KCreateAuthenticationTable().Length() + 10);
	TPtr authenticationTablePtr = createAuthenticationTable->Des();

// <MAPINFO_RESEND_MOD_BEGIN>
	HBufC* createDatastoreTable = HBufC::NewLC( KCreateDatastoreTable().Length() * 3);
	TPtr datastoreTablePtr = createDatastoreTable->Des();
// <MAPINFO_RESEND_MOD_END>

	agentLogTablePtr.Format( KCreateAgentLogTable, KNSmlMaxDataLength,
	    KNSmlMaxServerIdLength, KNSmlMaxRemoteNameLength );
	authenticationTablePtr.Format( KCreateAuthenticationTable, KNSmlMaxServerIdLength );

// <MAPINFO_RESEND_MOD_BEGIN>
	datastoreTablePtr.Format(KCreateDatastoreTable(), &KNSmlTableDatastore(), &KNSmlDatastoreRowID(), &KNSmlDatastoreAgentlogID(), &KNSmlDatastoreStreamID(), &KNSmlDatastoreStreamData());
// <MAPINFO_RESEND_MOD_END>

	User::LeaveIfError( iDatabase.Create( iRdbSession,
	                                      aFullName,
	                                      KNSmlDBMSSecureSOSServerID ) );
	iDatabase.Begin();
	iDatabase.Execute( *createAgentLogTable );
	iDatabase.Execute( *createAuthenticationTable );
// <MAPINFO_RESEND_MOD_BEGIN>
	iDatabase.Execute( *createDatastoreTable );
// <MAPINFO_RESEND_MOD_END>
	iDatabase.Execute( KCreateVersionTable() );

	RDbTable table;
	User::LeaveIfError( table.Open( iDatabase, KNSmlTableVersion() ) );
	CleanupClosePushL( table );
	CDbColSet* colSet = table.ColSetL();
	CleanupStack::PushL( colSet );
	table.InsertL();
	table.SetColL( colSet->ColNo( KNSmlVersionColumnMajor ), KNSmlAgentLogCurrentVersionMajor );
	table.SetColL( colSet->ColNo( KNSmlVersionColumnMinor ), KNSmlAgentLogCurrentVersionMinor );
	table.PutL();
	
	CommitAndCompact();
	iDatabase.Close();
	
	DBG_FILE(_S8("CNSmlDSAgentLog::CreateDatabaseL ends"));
// <MAPINFO_RESEND_MOD_BEGIN>
	CleanupStack::PopAndDestroy( 5 ); // createAgentLogTable, createAuthenticationTable, colset, table
	//CleanupStack::PopAndDestroy( 4 ); // createAgentLogTable, createAuthenticationTable, colset, table
// <MAPINFO_RESEND_MOD_END>
	}	

//=============================================
//		CNSmlDSAgentLog::SetAgentLogKeyL()
//		Sets AgentLog key with given string values.
//=============================================

EXPORT_C void CNSmlDSAgentLog::SetAgentLogKeyL( const TInt aImplUid, const TDesC& aLocalDB, const TDesC& aServerId, const TDesC& aRemoteDB )
	{
	DBG_FILE(_S8("CNSmlDSAgentLog::SetAgentLogKeyL begins"));
	HBufC* sql = HBufC::NewLC( KSQLGetAgentLogRow().Length() + 5 + aLocalDB.Length() + aServerId.Length() );
    
	TPtr sqlPtr = sql->Des();
	sqlPtr.Format( KSQLGetAgentLogRow, aImplUid, &aLocalDB, &aServerId );
	
	PrepareViewL( *sql, iView.EReadOnly );
	
	if ( iView.FirstL() )
		{
			for(TInt i = 0; i < iView.CountL() ; i++ )
			{
		iView.GetL();
				if( aRemoteDB.CompareF( iView.ColDes16( iColSet->ColNo( KNSmlAgentLogRemoteDatabase  ) ) ) == 0 )
				{
		iID = iView.ColInt( iColSet->ColNo( KNSmlAgentLogId ) );
					i = iView.CountL() ; // terminates the loop
				}
				else
				{
					iView.NextL();	
				}
				
			}
		}
	else
		{
		iID = KNSmlNewObject;
		}
	
	CleanupStack::PopAndDestroy( 1 ); // sql

	iImplUid = aImplUid;
	*iLocalDB = aLocalDB;
	*iServerId = aServerId;
	*iRemoteDB = aRemoteDB;

	DBG_FILE(_S8("CNSmlDSAgentLog::SetAgentLogKeyL ends"));
	}


//=============================================
//		CNSmlDSAgentLog::SetIntValueL()
//		Sets given integer value.
//=============================================

EXPORT_C void CNSmlDSAgentLog::SetIntValueL( TNSmlAgentLogData aType, const TInt aNewValue )
	{
	DBG_FILE(_S8("CNSmlDSAgentLog::SetIntValueL begins"));
	// BPSS-7NZESW : return if remote database value is empty
	if ( iLocalDB == NULL || iServerId == NULL ||iRemoteDB == NULL || iRemoteDB->Length() < 1  )
		{
		return;
		}

	TBuf<32> columnName;
	TInt setValue( aNewValue );

	switch ( aType )
		{
		case ( EAgentLogSlowSyncRequest ) :
			columnName = KNSmlAgentLogSlowSyncRequest;
			break;

		case ( EAgentLogServerSupportsNOC ) :
			columnName = KNSmlAgentLogServerSupportsNOC;
			// Make sure that value is 1 or 0
			if ( setValue )
				{
				setValue = 1;
				}
			else
				{
				setValue = 0;
				}
			break;

		case ( EAgentLogServerSupportsHierarchicalSync ) :
			columnName = KNSmlAgentLogServerSupportsHierarchicalSync ;
			// Make sure that value is 1 or 0
			if ( setValue )
				{
				setValue = 1;
				}
			else
				{
				setValue = 0;
				}
			break;

		case ( EAgentLogServerSyncTypes ) :
			columnName = KNSmlAgentLogServerSyncTypes;
			break;
		//RD_SUSPEND_RESUME
		case (EAgentLogSyncSuspendedState) :
			columnName = KNSmlAgentLogSuspendedState;
			break;
			
		case (EAgentLogPreviousSyncType) :
		     columnName=KNSmlAgentLogPreviousSyncType;
		     break;
		case (EAgentLogPreviousClientSyncType):
		     columnName=KNSmlAgentLogPreviousClientSyncType;
		     break;
		
		default:
			User::Panic( KNSmlIndexOutOfBoundStr, KNSmlPanicIndexOutOfBound );
		}

	HBufC* sql = AgentLogRowSqlLC();
	PrepareViewL( *sql, iView.EUpdatable );
	
	iDatabase.Begin();
	if ( iView.FirstL() )
		{
		//existing row
		iView.GetL();
		iView.UpdateL();
		iView.SetColL( iColSet->ColNo( columnName ), setValue );
		iView.PutL();
		}
	else
		{
		//new row
		InitNewRowL();
		iTableAgentLog.SetColL( iColSet->ColNo( columnName ), setValue );
		iTableAgentLog.PutL();		
		}	
	CommitAndCompact();
	
	CleanupStack::PopAndDestroy( 1 ); // sql
	DBG_FILE(_S8("CNSmlDSAgentLog::SetIntValueL ends"));
	}

//=============================================
//		CNSmlDSAgentLog::IntValueL()
//		Gets given integer value.
//=============================================

EXPORT_C TInt CNSmlDSAgentLog::IntValueL( TNSmlAgentLogData aType )
	{
	DBG_FILE(_S8("CNSmlDSAgentLog::IntValueL begins"));
	HBufC* sql = AgentLogRowSqlLC();

	TBuf<30>columnName;

	switch ( aType )
		{
		case ( EAgentLogSlowSyncRequest ) :
			columnName = KNSmlAgentLogSlowSyncRequest;
			break;

		case ( EAgentLogServerSupportsNOC ) :
			columnName = KNSmlAgentLogServerSupportsNOC;
			break;
		
		case ( EAgentLogServerSyncTypes ) :
			columnName = KNSmlAgentLogServerSyncTypes;
			break;
			
		case EAgentLogServerSupportsHierarchicalSync:
			columnName = KNSmlAgentLogServerSupportsHierarchicalSync;
			break;
		//RD_SUSPEND_RESUME
		case (EAgentLogSyncSuspendedState) :
			columnName = KNSmlAgentLogSuspendedState;
			break;
		case (EAgentLogPreviousSyncType) :
		     columnName=KNSmlAgentLogPreviousSyncType;
             break;
		case(EAgentLogPreviousClientSyncType):
		    columnName=KNSmlAgentLogPreviousClientSyncType;
		    break;
		 //RD_SUSPEND_RESUME
		default:
			{
			User::Panic( KNSmlIndexOutOfBoundStr, KNSmlPanicIndexOutOfBound );
			}
		}
	
	PrepareViewL( *sql, iView.EReadOnly );

	TInt status( 0 );

	if ( iView.FirstL() )
		{
		iView.GetL();
		status = iView.ColInt( iColSet->ColNo( columnName ) );
		}

	CleanupStack::PopAndDestroy( 1 ); // sql 
	DBG_FILE(_S8("CNSmlDSAgentLog::IntValueL ends"));
	return status;
	}

//=============================================
//		CNSmlDSAgentLog::SetTimeValueL()
//		Sets given datetime value.
//=============================================

EXPORT_C void CNSmlDSAgentLog::SetTimeValueL( TNSmlAgentLogData aType, const TTime& aNewValue )
	{
	DBG_FILE(_S8("CNSmlDSAgentLog::SetTimeValueL begins"));
	if ( iLocalDB == NULL || iServerId == NULL || iRemoteDB == NULL )
		{
		return;
		}
	
	HBufC* sql = AgentLogRowSqlLC();

	TBuf<20> columnName;

	switch ( aType )
		{
		case ( EAgentLogLastSyncronised ) :
			{
			columnName.Format( KColumn, &KNSmlAgentLogLastSyncronised );
			}
		break;

		case ( EAgentLogLastSyncAnchor ) :
			{
			columnName.Format( KColumn, &KNSmlAgentLogLastSyncAnchor );
			}
		break;
		
		default:
			{
			User::Panic( KNSmlIndexOutOfBoundStr, KNSmlPanicIndexOutOfBound );
			}
		}
	
	PrepareViewL( *sql, iView.EUpdatable );
		
	iDatabase.Begin();
	if ( iView.FirstL() )
		{
		//existing row
		iView.GetL();
		iView.UpdateL();
		iView.SetColL( iColSet->ColNo( columnName ), aNewValue );  //check
		iView.PutL();
		}
	else
		{
		//new row
		InitNewRowL();
		iTableAgentLog.SetColL( iColSet->ColNo( columnName ), aNewValue );
		iTableAgentLog.PutL();	
		}
	
	CommitAndCompact();
	CleanupStack::PopAndDestroy( 1 ); // sql, 
	DBG_FILE(_S8("CNSmlDSAgentLog::SetTimeValueL ends"));
	}

//=============================================
//		CNSmlDSAgentLog::TimeValueL()
//		Gets given datetime value.
//		If the returned value is TTime(0),
//		given value was not found.
//=============================================

EXPORT_C TTime CNSmlDSAgentLog::TimeValueL( TNSmlAgentLogData aType )
	{
	DBG_FILE(_S8("CNSmlDSAgentLog::TimeValueL begins"));
	HBufC* sql = AgentLogRowSqlLC();
	TBuf<20> columnName;

	switch ( aType )
		{
		case ( EAgentLogLastSyncronised ) :
			{
			columnName.Format( KColumn, &KNSmlAgentLogLastSyncronised );
			}
		break;

		case ( EAgentLogLastSyncAnchor ) :
			{
			columnName.Format( KColumn, &KNSmlAgentLogLastSyncAnchor );
			}
		break;
		
		default:
			{
			User::Panic( KNSmlIndexOutOfBoundStr, KNSmlPanicIndexOutOfBound );
			}
		}
	
	PrepareViewL( *sql, iView.EReadOnly );
	
	TTime dateTimeValue( 0 );

	if ( iView.FirstL() )
		{
		iView.GetL();
		dateTimeValue = iView.ColTime( iColSet->ColNo( columnName ) );
		}
	
	CleanupStack::PopAndDestroy( 1 ); // sql
	DBG_FILE(_S8("CNSmlDSAgentLog::TimeValueL ends"));
	return dateTimeValue;
	}

//=============================================
//		CNSmlDSAgentLog::SetNonceL()
//		Sets nonce.
//=============================================

EXPORT_C void CNSmlDSAgentLog::SetNonceL( const TDesC& aServerId, const TDesC& aNewValue )
	{	
	DBG_FILE(_S8("CNSmlDSAgentLog::SetNonceL begins"));
	HBufC* sql = AuthenticationSqlLC( aServerId );
	
	PrepareViewL( *sql, iView.EUpdatable );
	iDatabase.Begin();

    RDbColReadStream in;
    TInt oldNonceSize(0);
   	if ( iView.FirstL() )
        {
        iView.GetL();
        in.OpenLC( iView, iColSetTableAuth->ColNo( KNSmlAuthenticationNonce ) );
        oldNonceSize = in.Source()->SizeL();
		CleanupStack::PopAndDestroy(); // in
        }
		
	RDbColWriteStream out;
	if ( iView.FirstL() )
		{
		//existing row
				
		// Check OOD before updating existing nonce value
        if ((aNewValue.Size() > oldNonceSize ) && (SysUtil::FFSSpaceBelowCriticalLevelL( &iFsSession,
                ( aNewValue.Size() - oldNonceSize ) + KNSmlAgentLogTreshold )))
    	    {
            iDatabase.Rollback();
            User::Leave( KErrDiskFull );
    		}
		
		// Replace nonce
		iView.GetL();
		iView.UpdateL();
		out.OpenLC( iView, iColSetTableAuth->ColNo( KNSmlAuthenticationNonce ) );
		out.WriteL( aNewValue );
		out.Close();
		iView.PutL();
		}
	else
		{
		//new row
		
		// Check OOD before inserting new row into Authentication table
        if ( SysUtil::FFSSpaceBelowCriticalLevelL( &iFsSession, aNewValue.Size() +
                KNSmlAgentLogMaxFixedAuthRowSize + KNSmlAgentLogTreshold ) )
       	    {
            iDatabase.Rollback();
            User::Leave( KErrDiskFull );
    		}
		
		// Insert new row
		PrepareViewL( KSQLGetAuthenticationAll, iView.EUpdatable );
		iView.InsertL();
		out.OpenLC( iView, iColSetTableAuth->ColNo( KNSmlAuthenticationNonce ) );
		out.WriteL( aNewValue );
		out.Close();
		iView.SetColL( iColSetTableAuth->ColNo( KNSmlAuthenticationServerId ), aServerId );		
		iView.PutL();
		}
	
	CommitAndCompact();
	CleanupStack::PopAndDestroy( 2 ); // sql, out
	DBG_FILE(_S8("CNSmlDSAgentLog::SetNonceL ends"));
	}

//=============================================
//		CNSmlDSAgentLog::NonceL()
//		Gets nonce.
//		If length of the returned string is 0,
//		nonce was not found.
//=============================================

EXPORT_C HBufC* CNSmlDSAgentLog::NonceL( const TDesC& aServerId )
{
	DBG_FILE(_S8("CNSmlDSAgentLog::NonceL begins"));
	
	HBufC* sql = AuthenticationSqlLC( aServerId );
		
	PrepareViewL( *sql, iView.EReadOnly );
	
	HBufC* stringValue;

	if ( iView.FirstL() )
		{
		iView.GetL();		
		RDbColReadStream in;
		in.OpenLC( iView, iColSetTableAuth->ColNo( KNSmlAuthenticationNonce ) );
		stringValue = HBufC::NewLC( iView.ColLength( iColSetTableAuth->ColNo( KNSmlAuthenticationNonce ) ) );
		TPtr ptr = stringValue->Des();
		in.ReadL( ptr, iView.ColLength( iColSetTableAuth->ColNo( KNSmlAuthenticationNonce ) ) );
		CleanupStack::Pop( 1 ); // stringValue
		CleanupStack::PopAndDestroy( 1 ); //in
		}
	else
		{
		//couldn't find
		stringValue = HBufC::NewL( 0 );
		}
	
	CleanupStack::PopAndDestroy( 1 ); // sql
	DBG_FILE(_S8("CNSmlDSAgentLog::NonceL ends"));
	return stringValue;
	}

//=============================================
//		CNSmlDSAgentLog::SetSessionIDL()
//		Sets session Id.
//=============================================

EXPORT_C void CNSmlDSAgentLog::SetSessionIDL( const TDesC& aServerId, const TInt aSessionID )
	{	
	DBG_FILE(_S8("CNSmlDSAgentLog::SetSessionIDL begins"));
	
	HBufC* sql = AuthenticationSqlLC( aServerId );

	PrepareViewL( *sql, iView.EUpdatable );
		
	iDatabase.Begin();

	if ( iView.FirstL() )
		{
		//existing row
		iView.GetL();
		iView.UpdateL();
		iView.SetColL( iColSetTableAuth->ColNo( KNSmlAuthenticationSessionId ), ( TInt ) aSessionID );
		iView.PutL();
		}
	else
		{
		//new row
		
		// Check OOD before inserting new row into Authentication table
        if ( SysUtil::FFSSpaceBelowCriticalLevelL( &iFsSession,
                KNSmlAgentLogMaxFixedAuthRowSize + KNSmlAgentLogTreshold ) )
            {
            iDatabase.Rollback();
            User::Leave( KErrDiskFull );
            }
		
		iTableAuthentication.InsertL();
		iTableAuthentication.SetColL( iColSetTableAuth->ColNo( KNSmlAuthenticationServerId ), aServerId );
		iTableAuthentication.SetColL( iColSetTableAuth->ColNo( KNSmlAuthenticationSessionId ), aSessionID );
		iTableAuthentication.PutL();
		}
		
	CommitAndCompact();
	CleanupStack::PopAndDestroy( 1 ); // sql
	DBG_FILE(_S8("CNSmlDSAgentLog::SetSessionIDL ends"));
	}

//=============================================
//		CNSmlDSAgentLog::SessionIDL()
//		Get Session Id
//=============================================

EXPORT_C TInt CNSmlDSAgentLog::SessionIDL( const TDesC& aServerId )
	{
	DBG_FILE(_S8("CNSmlDSAgentLog::SessionIDL begins"));
		 
	HBufC* sql = AuthenticationSqlLC( aServerId );
	
	PrepareViewL( *sql, iView.EReadOnly );
	
	TInt id( 0 );

	if ( iView.FirstL() )
		{
		iView.GetL();
		id = iView.ColInt( iColSetTableAuth->ColNo( KNSmlAuthenticationSessionId ) );
		}

	CleanupStack::PopAndDestroy( 1 ); // sql
	DBG_FILE(_S8("CNSmlDSAgentLog::SessionIDL ends"));
	return id;
	}

//=============================================
//		CNSmlDSAgentLog::SetAuthTypeL()
//		Set Authentication type
//=============================================

EXPORT_C void CNSmlDSAgentLog::SetAuthTypeL( const TDesC& aServerId, const TNSmlAgentLogAuthenticationType aNewValue )
{
	DBG_FILE(_S8("CNSmlDSAgentLog::SetAuthTypeL begins"));
	
	HBufC* sql = AuthenticationSqlLC( aServerId );
	
	PrepareViewL( *sql, iView.EUpdatable );
		
	iDatabase.Begin();

	if ( iView.FirstL() )
		{
		//existing row
		iView.GetL();
		iView.UpdateL();
		iView.SetColL( iColSetTableAuth->ColNo( KNSmlAuthenticationType ), ( TInt ) aNewValue );
		iView.PutL();
		}
	else
		{
		//new row
		
        // Check OOD before inserting new row into Authentication table
        if ( SysUtil::FFSSpaceBelowCriticalLevelL( &iFsSession,
                KNSmlAgentLogMaxFixedAuthRowSize + KNSmlAgentLogTreshold ) )
            {
            iDatabase.Rollback();
            User::Leave( KErrDiskFull );
            }		
		
		iTableAuthentication.InsertL();
		iTableAuthentication.SetColL( iColSetTableAuth->ColNo( KNSmlAuthenticationServerId ), aServerId );
		iTableAuthentication.SetColL( iColSetTableAuth->ColNo( KNSmlAuthenticationType ), (TInt)aNewValue );
		iTableAuthentication.PutL();
		}

	CommitAndCompact();
	CleanupStack::PopAndDestroy( 1 ); // sql
	DBG_FILE(_S8("CNSmlDSAgentLog::SetAuthTypeL ends"));
	}

//=============================================
//		CNSmlDSAgentLog::AuthTypeL()
//		Get authentication type
//=============================================

EXPORT_C TInt CNSmlDSAgentLog::AuthTypeL( const TDesC& aServerId )
	{
	DBG_FILE(_S8("CNSmlDSAgentLog::AuthTypeL begins"));
		 
	HBufC* sql = AuthenticationSqlLC( aServerId );
	
	PrepareViewL( *sql, iView.EReadOnly );

	TInt type( 0 );

	if ( iView.FirstL() )
		{
		iView.GetL();
		type = iView.ColInt( iColSetTableAuth->ColNo( KNSmlAuthenticationType ) );
		}

	CleanupStack::PopAndDestroy( 1 ); // sql
	DBG_FILE(_S8("CNSmlDSAgentLog::AuthTypeL ends"));
	return type;
	}

//=============================================
//		CNSmlDSAgentLog::SetServerIdL()
//		Sets ServerId.
//=============================================

EXPORT_C void CNSmlDSAgentLog::SetServerIdL( TDesC& aOldServerId, TDesC& aNewValue )
	{	
	DBG_FILE(_S8("CNSmlDSAgentLog::SetServerIdL begins"));
	HBufC* sql = AuthenticationSqlLC( aOldServerId );
	
	PrepareViewL( *sql, iView.EUpdatable );
	TInt replaceCount(0); // Number of replaced Server Ids
		
	iDatabase.Begin();
	if ( iView.FirstL() )
		{
	    ++replaceCount;
		
		//existing row
		iView.GetL();
		iView.UpdateL();
		iView.SetColL( iColSetTableAuth->ColNo( KNSmlAuthenticationServerId ), aNewValue );
		iView.PutL();
		}
	else
		{
		// Given ServerId was not found. Update unsuccessful.
		DBG_FILE(_S8("CNSmlDSAgentLog::AuthenticationTable update unsuccessful"));
		}
	CleanupStack::PopAndDestroy(); // sql
	
	// Update all old Server Ids also in the agentlog table
	*iServerId = aNewValue;

	HBufC* sqlAgentLog = HBufC::NewLC( KSQLGetAgentLogRowWithServerId().Length() + aOldServerId.Length() );
	TPtr sqlPtr = sqlAgentLog->Des();
	sqlPtr.Format( KSQLGetAgentLogRowWithServerId, &aOldServerId );

	
	PrepareViewL( *sqlAgentLog, iView.EUpdatable );

	if ( iView.FirstL() )
		{
		replaceCount += iView.CountL();
		
		do 
			{
			//existing row
			iView.GetL();
			iView.UpdateL();
			iView.SetColL( iColSet->ColNo( KNSmlAgentLogServerId ), aNewValue );
			iView.PutL();
			}
		while ( iView.NextL() );
		}
	else
		{
		// Given ServerId was not found. Update unsuccessful.
		DBG_FILE(_S8("CNSmlDSAgentLog::AgentLogTable update unsuccessful"));
		}

    // Check OOD before commiting
    if ( (aNewValue.Size() > aOldServerId.Size()) && (SysUtil::FFSSpaceBelowCriticalLevelL( &iFsSession, (replaceCount * 
            (aNewValue.Size() - aOldServerId.Size())) + KNSmlAgentLogTreshold )))
        {
        iDatabase.Rollback();
        User::Leave( KErrDiskFull );
        }

	CommitAndCompact();
	CleanupStack::PopAndDestroy(); // sqlAgentLog
	DBG_FILE(_S8("CNSmlDSAgentLog::SetServerIdL ends"));
	}

//=============================================
//		CNSmlDSAgentLog::SetServerDeviceInfoL()
//		Sets Server Device Info given in parameter.
//=============================================

EXPORT_C void CNSmlDSAgentLog::SetServerDeviceInfoL( const RPointerArray<CNSmlDbCaps>& aDbCaps )
	{
	DBG_FILE(_S8("CNSmlDSAgentLog::SetServerDeviceInfoL begins"));

	_LIT8( KSeparator, ";");

	CBufFlat* deviceData = CBufFlat::NewL(8);
	CleanupStack::PushL( deviceData );
	TInt pos( 0 );
	TBuf8<6> buffer;  //for num part

	HBufC* stringValue16=0;
	
	TInt arrayCount(0);
	while (arrayCount<aDbCaps.Count())
		{
		
		SmlDevInfDatastore_t* dataStore = aDbCaps[arrayCount]->DatastoreL();
		CleanupStack::PushL(dataStore);

		buffer.Num( ( TInt )CNSmlDSAgentLog::EDataStore  );	
		deviceData->InsertL( pos, buffer );
		pos += buffer.Length();

		deviceData->InsertL( pos, KSeparator );
		pos++;
		deviceData->InsertL( pos, KSeparator );
		pos++;
		

		if (dataStore->sourceref)
			{
			buffer.Num( ( TInt )CNSmlDSAgentLog::ESourceRef  );	
			deviceData->InsertL( pos, buffer );
			pos += buffer.Length();

			deviceData->InsertL( pos, KSeparator );
			pos++;

			deviceData->InsertL( pos,  dataStore->sourceref->Data());
			pos += dataStore->sourceref->Data().Length();

			deviceData->InsertL( pos, KSeparator );
			pos++;
			}

		if (dataStore->displayname)
			{
			buffer.Num( ( TInt )CNSmlDSAgentLog::EDisplayName  );	
			deviceData->InsertL( pos, buffer );
			pos += buffer.Length();

			deviceData->InsertL( pos, KSeparator );
			pos++;

			deviceData->InsertL( pos,  dataStore->displayname->Data());
			pos += dataStore->displayname->Data().Length();

			deviceData->InsertL( pos, KSeparator );
			pos++;
			}

		if (dataStore->maxguidsize)
			{
			buffer.Num( ( TInt )CNSmlDSAgentLog::EMaxGUIDSize  );	
			deviceData->InsertL( pos, buffer );
			pos += buffer.Length();

			deviceData->InsertL( pos, KSeparator );
			pos++;

			deviceData->InsertL( pos,  dataStore->maxguidsize->Data());
			pos += dataStore->maxguidsize->Data().Length();

			deviceData->InsertL( pos, KSeparator );
			pos++;
			}

		if (dataStore->rxpref)
			{
			buffer.Num( ( TInt )CNSmlDSAgentLog::ERxPref  );	
			deviceData->InsertL( pos, buffer );
			pos += buffer.Length();

			deviceData->InsertL( pos, KSeparator );
			pos++;

			deviceData->InsertL( pos,  dataStore->rxpref->cttype->Data());
			pos += dataStore->rxpref->cttype->Data().Length();

			deviceData->InsertL( pos, KTabSeparator );
			pos++;

			deviceData->InsertL( pos,  dataStore->rxpref->verct->Data());
			pos += dataStore->rxpref->verct->Data().Length();

			deviceData->InsertL( pos, KSeparator );
			pos++;
			}

		if (dataStore->rx)
			{
			SmlDevInfXmitListPtr_t rx= dataStore->rx;

			while (rx)
				{
				buffer.Num( ( TInt )CNSmlDSAgentLog::ERx);	
				deviceData->InsertL( pos, buffer );
				pos += buffer.Length();

				deviceData->InsertL( pos, KSeparator );
				pos++;

				deviceData->InsertL( pos,  rx->data->cttype->Data());
				pos += rx->data->cttype->Data().Length();

				deviceData->InsertL( pos, KTabSeparator );
				pos++;

				deviceData->InsertL( pos,  rx->data->verct->Data());
				pos += rx->data->verct->Data().Length();

				deviceData->InsertL( pos, KSeparator );
				pos++;

				rx=rx->next;
				}
			}

		if (dataStore->txpref)
			{
			buffer.Num( ( TInt )CNSmlDSAgentLog::ETxPref  );	
			deviceData->InsertL( pos, buffer );
			pos += buffer.Length();

			deviceData->InsertL( pos, KSeparator );
			pos++;

			deviceData->InsertL( pos,  dataStore->txpref->cttype->Data());
			pos += dataStore->txpref->cttype->Data().Length();

			deviceData->InsertL( pos, KTabSeparator );
			pos++;

			deviceData->InsertL( pos,  dataStore->txpref->verct->Data());
			pos += dataStore->txpref->verct->Data().Length();

			deviceData->InsertL( pos, KSeparator );
			pos++;
			}

		if (dataStore->tx)
			{
			SmlDevInfXmitListPtr_t tx= dataStore->tx;

			while (tx)
				{
				buffer.Num( ( TInt )CNSmlDSAgentLog::ETx);	
				deviceData->InsertL( pos, buffer );
				pos += buffer.Length();

				deviceData->InsertL( pos, KSeparator );
				pos++;

				deviceData->InsertL( pos,  tx->data->cttype->Data());
				pos += tx->data->cttype->Data().Length();

				deviceData->InsertL( pos, KTabSeparator );
				pos++;

				deviceData->InsertL( pos,  tx->data->verct->Data());
				pos += tx->data->verct->Data().Length();

				deviceData->InsertL( pos, KSeparator );
				pos++;

				tx=tx->next;
				}
			}

		if (dataStore->dsmem)
			{
			buffer.Num( ( TInt )CNSmlDSAgentLog::EDSMem );	
			deviceData->InsertL( pos, buffer );
			pos += buffer.Length();

			deviceData->InsertL( pos, KSeparator );
			pos++;

			deviceData->InsertL( pos,  dataStore->dsmem->Data());
			pos += dataStore->dsmem->Data().Length();

			deviceData->InsertL( pos, KSeparator );
			pos++;
			}

		if (dataStore->supportHierarchicalSync)
			{
			buffer.Num( ( TInt )CNSmlDSAgentLog::ESupportHierarchicalSync );	
			deviceData->InsertL( pos, buffer );
			pos += buffer.Length();

			deviceData->InsertL( pos, KSeparator );
			pos++;

			deviceData->InsertL( pos, KSeparator );
			pos++;
			}

		if (dataStore->synccap)
			{
			SmlPcdataListPtr_t syncType=dataStore->synccap->synctype;

			while (syncType)
				{
				buffer.Num( ( TInt )CNSmlDSAgentLog::ESyncCap);	
				deviceData->InsertL( pos, buffer );
				pos += buffer.Length();

				deviceData->InsertL( pos, KSeparator );
				pos++;

				deviceData->InsertL( pos,  dataStore->synccap->synctype->data->Data());
				pos += dataStore->synccap->synctype->data->Data().Length();

				deviceData->InsertL( pos, KSeparator );
				pos++;

				syncType=syncType->next;
				}
			}

		sml_devinf_ctcaplist_s *ctCaps = dataStore->ctcap;

		CNSmlCtCapsHandler* ctCapsHandler = new(ELeave) CNSmlCtCapsHandler( ctCaps );
		CleanupStack::PushL( ctCapsHandler );
		CArrayFix<TNSmlCtCapData>* ctCapArray = new( ELeave ) CArrayFixFlat<TNSmlCtCapData>(8);
		CleanupStack::PushL( ctCapArray );
		ctCapsHandler->GetCtCapsL( *ctCapArray );

		for ( TInt i = 0; i < ctCapArray->Count(); i++ )
			{
			buffer.Num( ( TInt ) ctCapArray->At( i ).iTag );	
			deviceData->InsertL( pos, buffer );
			pos += buffer.Length();

			deviceData->InsertL( pos, KSeparator );
			pos++;

			deviceData->InsertL( pos, ctCapArray->At( i ).iValue );
			pos += ctCapArray->At( i ).iValue.Length();
			
			deviceData->InsertL( pos, KSeparator );
			pos++;
			}

		CleanupStack::PopAndDestroy(3); //dataStore,ctCapsHandler,ctCapArray
		
		arrayCount++;
		}
	if (arrayCount>0)
		{
		TPtr8 stringValue = deviceData->Ptr( 0 );
		stringValue16 = HBufC::NewLC( stringValue.Size() * 2 );
		TPtr ptrStringValue16( stringValue16->Des() ); 
		CnvUtfConverter::ConvertToUnicodeFromUtf8( ptrStringValue16, stringValue );
		}
	else
		{
		stringValue16 = KNullDesC().AllocLC();
		}

	
	HBufC* sql = AgentLogRowSqlLC();
	
	PrepareViewL( *sql, iView.EUpdatable );
	iDatabase.Begin();
	
	RDbColReadStream in;
	TInt oldDeviceInfoSize(0);
	if ( iView.FirstL() )
	    {
	    iView.GetL();
	    in.OpenLC( iView, iColSet->ColNo( KNSmlAgentLogServerDeviceInfo ) );
	    oldDeviceInfoSize = in.Source()->SizeL();
	    CleanupStack::PopAndDestroy(&in);
	    }
	
	RDbColWriteStream out;
	if ( iView.FirstL() )
		{
		//existing row

        // Check OOD before writing device info on existing row
        if ((stringValue16->Size() > oldDeviceInfoSize ) && (SysUtil::FFSSpaceBelowCriticalLevelL( &iFsSession,
                ( stringValue16->Size() - oldDeviceInfoSize ) + KNSmlAgentLogTreshold )))
	    {
        iDatabase.Rollback();
        User::Leave( KErrDiskFull );
		}

		// Update device info
		iView.GetL();
		iView.UpdateL();
		out.OpenLC( iView, iColSet->ColNo( KNSmlAgentLogServerDeviceInfo ) );
		out.WriteL( *stringValue16 );
		out.Close();
		iView.PutL();
		}
	else
		{
		//new row

        // Check OOD before inserting new row into AgentLog table
        if ( SysUtil::FFSSpaceBelowCriticalLevelL( &iFsSession,
                KNSmlAgentLogMaxFixedLogRowSize + stringValue16->Size() +
                KNSmlAgentLogTreshold ) )
    	    {
            iDatabase.Rollback();
            User::Leave( KErrDiskFull );
    		}

        // Insert new row		
		PrepareViewL( KSQLGetAgentLogAll, iView.EUpdatable );
		iID = GenerateIdL();
		iView.InsertL();
		iView.SetColL( iColSet->ColNo( KNSmlAgentLogId ), iID );
		out.OpenLC( iView, iColSet->ColNo( KNSmlAgentLogServerDeviceInfo ) );
		out.WriteL( *stringValue16 );
		out.Close();
		iView.SetColL( iColSet->ColNo( KNSmlAgentLogImplementationUID ), iImplUid );
		iView.SetColL( iColSet->ColNo( KNSmlAgentLogLocalDatabase ), *iLocalDB );
		iView.SetColL( iColSet->ColNo( KNSmlAgentLogServerId ), *iServerId );
		iView.SetColL( iColSet->ColNo( KNSmlAgentLogRemoteDatabase ), *iRemoteDB );
		iView.PutL();
		}
	
	CommitAndCompact();
	CleanupStack::PopAndDestroy( 4 ); // sql, out, stringValue16, deviceData

	DBG_FILE(_S8("CNSmlDSAgentLog::SetServerDeviceInfoL ends"));
	}

//=============================================
//		CNSmlDSAgentLog::GetServerDeviceInfoL()
//		Gets Server Device Info.
//=============================================

EXPORT_C void CNSmlDSAgentLog::GetServerDeviceInfoL( RPointerArray<CNSmlDbCaps>& aDbCaps )
	{
	DBG_FILE(_S8("CNSmlDSAgentLog::GetServerDeviceInfoL begins"));
	

	HBufC* sql = AgentLogRowSqlLC();
	
	PrepareViewL( *sql, iView.EReadOnly );

	HBufC* stringValue;

	if ( iView.FirstL() )
		{
		iView.GetL();		
		RDbColReadStream in;
		stringValue = HBufC::NewLC( iView.ColLength( iColSet->ColNo( KNSmlAgentLogServerDeviceInfo ) ) );
		in.OpenLC( iView, iColSet->ColNo( KNSmlAgentLogServerDeviceInfo ) );		
		TPtr ptr = stringValue->Des();
		in.ReadL( ptr, iView.ColLength( iColSet->ColNo( KNSmlAgentLogServerDeviceInfo ) ) );
		CleanupStack::PopAndDestroy( 1 ); // in
		}
	else
		{
		//couldn't find
		CleanupStack::PopAndDestroy( 1 ); // sql
		return;
		}
	
	TInt start( 0 );
	TInt firstSeparator( 0 );
	TBool firstFound( EFalse );
	TLex lex;
	TInt numPart( 0 );
	TPtr ptrStringValue = stringValue->Des();


	if (stringValue->Length()>0)
		{
		CNSmlDbCaps* dbCaps = NULL; //

		CNSmlCtCap* cap=NULL;
		CNSmlDevInfProp* prop=NULL;
		CNSmlPropParam* param=NULL;
		TInt pc(0);
		for ( TInt i = 0; i < stringValue->Length(); i++ )
			{
			if ( ptrStringValue[i] == ';' ) //; = separator
				{
				if ( !firstFound )
					{
					firstSeparator = i;
					firstFound = ETrue;
					}
				else
					{
					lex.Assign( stringValue->Mid( start, firstSeparator - start ) );
					lex.Val( numPart );

					TPtrC ptrTemp = stringValue->Mid( firstSeparator + 1, i - firstSeparator - 1 );
					
					HBufC8* stringPart = HBufC8::NewLC( ptrTemp.Length() );
					pc++;
					TPtr8 ptrStringPart = stringPart->Des();
					
					CnvUtfConverter::ConvertFromUnicodeToUtf8( ptrStringPart, ptrTemp );				

					TInt separator;
					
					switch(numPart)
						{
						case TNSmlCtCapData::ECtType:
							cap = dbCaps->AddCtCapLC(); pc++;
							cap->SetCtTypeL(ptrStringPart);
							prop=0;
							param=0;
						break;

						case TNSmlCtCapData::EVerCt:
							if (cap)
								{
								cap->SetVerCtL(ptrStringPart);
								}
						break;

						case TNSmlCtCapData::EFieldLevel:
							if (cap)
								{
								cap->SetFieldLevelL();
								}
						break;

						case TNSmlCtCapData::EPropName:
							if (cap)
								{
								prop = cap->AddDevInfPropLC(ptrStringPart);
								pc++;
								param=0;
								}
						break;

						case TNSmlCtCapData::EPropDataType:
							if (prop)
								{
								prop->SetDataTypeL(ptrStringPart);
								}
						break;

						case TNSmlCtCapData::EPropMaxOccur:
							if (prop)
								{
								prop->SetMaxOccurL(ptrStringPart);
								}
						break;

						case TNSmlCtCapData::EPropNoTruncate:
							if (prop)
								{
								prop->SetNoTruncateL();
								}
						break;

						case TNSmlCtCapData::EPropValEnum:
							if (prop)
								{
								prop->AddValEnumL(ptrStringPart);
								}
						break;

						case TNSmlCtCapData::EPropDisplayName:
							if (prop)
								{
								prop->SetDisplayNameL(ptrStringPart);
								}
						break;

						case TNSmlCtCapData::EParamName:
							if (prop)
								{
								param = prop->AddParamLC(ptrStringPart);
								pc++;
								}
						break;

						case TNSmlCtCapData::EParamValEnum:
							if (param)
								{
								param->AddValEnumL(ptrStringPart);
								}
						break;

						case TNSmlCtCapData::EParamDataType:
							if (param)
								{
								param->SetDataTypeL(ptrStringPart);
								}
						break;

						case TNSmlCtCapData::EParamDisplayName:
							if (param)
								{
								param->SetDisplayNameL(ptrStringPart);
								}
						break;
						//////////////////////////

						case EDataStore:
							dbCaps = CNSmlDbCaps::NewL();
							TRAPD(err, aDbCaps.AppendL(dbCaps));
							if(err != KErrNone)
							    {
							    delete dbCaps;
								User::Leave( err );								
							    }
						break;

						case ESourceRef:
						    if (dbCaps)
						        {
							    dbCaps->SetSourceRefL(ptrStringPart);
						        }							
						break;

						case EDisplayName:
						    if (dbCaps)
						        {
							    dbCaps->SetDisplayNameL(ptrStringPart);
						        }
						break;

						case EMaxGUIDSize:
						    if (dbCaps)
						        {
							    dbCaps->SetMaxGuidSizeL(ptrStringPart);
						        }
						break;

						case ERxPref:
							separator = ptrStringPart.Locate(KTabSeparator()[0]);
							if (dbCaps)
							   {
							   dbCaps->SetRxPrefL(ptrStringPart.Left(separator),ptrStringPart.Right(ptrStringPart.Length()-separator-1));
							   }
						break;

						case ERx:
							separator = ptrStringPart.Locate(KTabSeparator()[0]);
							if (dbCaps)
							   {
							   dbCaps->AddRxL(ptrStringPart.Left(separator),ptrStringPart.Right(ptrStringPart.Length()-separator-1));
							   }
						break;

						case ETxPref:
							separator = ptrStringPart.Locate(KTabSeparator()[0]);
							if (dbCaps)
							   {
							   dbCaps->SetTxPrefL(ptrStringPart.Left(separator),ptrStringPart.Right(ptrStringPart.Length()-separator-1));
							   }
						break;


						case ETx:
							separator = ptrStringPart.Locate(KTabSeparator()[0]);
							if (dbCaps)
							    {
							    dbCaps->AddTxL(ptrStringPart.Left(separator),ptrStringPart.Right(ptrStringPart.Length()-separator-1));
							    }
						break;


						case EDSMem:
						break;

						case ESupportHierarchicalSync:
						    if (dbCaps)
						       {
							    dbCaps->SetSupportHierarchicalSyncL();
						       }
						break;

						case ESyncCap:
						break;

						default:
						break;	

						}
					
					start = i + 1;
					firstFound = EFalse;
					}
				}
			}

		CleanupStack::PopAndDestroy( pc ); // pc

		}
	
	CleanupStack::PopAndDestroy( 2 ); // sql, stringValue

	DBG_FILE(_S8("CNSmlDSAgentLog::GetServerDeviceInfoL ends"));
	}

//=============================================
//		CNSmlDSAgentLog::SetServerFilterInfoL()
//		Sets Server Filter Info given in parameter.
//=============================================

EXPORT_C void CNSmlDSAgentLog::SetServerFilterInfoL( const CArrayFix<TNSmlFilterCapData>* aFilterInfoArr )
	{
	DBG_FILE(_S8("CNSmlDSAgentLog::SetServerFilterInfoL begins"));
	_LIT8( KSeparator, ";");

	CBufFlat* filterData = CBufFlat::NewL(8);
	CleanupStack::PushL( filterData );
	TInt pos( 0 );
	TBuf8<6> buffer;  //for num part

	for ( TInt i = 0; i < aFilterInfoArr->Count(); i++ )
		{
		buffer.Num( ( TInt ) aFilterInfoArr->At( i ).iTag );	
		filterData->InsertL( pos, buffer );
		pos += buffer.Length();

		filterData->InsertL( pos, KSeparator );
		pos++;

		filterData->InsertL( pos, aFilterInfoArr->At( i ).iValue );
		pos += aFilterInfoArr->At( i ).iValue.Length();
		
		filterData->InsertL( pos, KSeparator );
		pos++;
		}
	
	TPtr8 stringValue = filterData->Ptr( 0 );

	HBufC* stringValue16;
	stringValue16 = HBufC::NewLC( stringValue.Size() * 2 );

	TPtr ptrStringValue16( stringValue16->Des() ); 
	CnvUtfConverter::ConvertToUnicodeFromUtf8( ptrStringValue16, stringValue );
	
	HBufC* sql = AgentLogRowSqlLC();
	
	PrepareViewL( *sql, iView.EUpdatable );		
	iDatabase.Begin();
	
	RDbColReadStream in;
	TInt oldFilterInfoSize(0);
	if ( iView.FirstL() )
	    {
	    iView.GetL();
	    in.OpenLC( iView, iColSet->ColNo( KNSmlAgentLogServerFilterInfo ) );
	    oldFilterInfoSize = in.Source()->SizeL();
        CleanupStack::PopAndDestroy(&in);
	    }
	
	RDbColWriteStream out;
	if ( iView.FirstL() )
		{
		//existing row

        // Check OOD before writing filter info on existing row
        if ( (stringValue16->Size() > oldFilterInfoSize) &&  (SysUtil::FFSSpaceBelowCriticalLevelL( &iFsSession,
                (stringValue16->Size() - oldFilterInfoSize) + KNSmlAgentLogTreshold )))
	    {
        iDatabase.Rollback();
        User::Leave( KErrDiskFull );
		}

        // Update filter info
		iView.GetL();
		iView.UpdateL();
		out.OpenLC( iView, iColSet->ColNo( KNSmlAgentLogServerFilterInfo ) );
		out.WriteL( ptrStringValue16 );
		out.Close();
		iView.PutL();
		}
	else
		{
		//new row

        // Check OOD before inserting new row into AgentLog table
        if ( SysUtil::FFSSpaceBelowCriticalLevelL( &iFsSession, 
                KNSmlAgentLogMaxFixedLogRowSize + stringValue16->Size() +
                KNSmlAgentLogTreshold ) )
    	    {
            iDatabase.Rollback();
            User::Leave( KErrDiskFull );
    		}

        // Insert new row
		PrepareViewL( KSQLGetAgentLogAll, iView.EUpdatable );
		iID = GenerateIdL();
		iView.InsertL();
		iView.SetColL( iColSet->ColNo( KNSmlAgentLogId ), iID );
		out.OpenLC( iView, iColSet->ColNo( KNSmlAgentLogServerFilterInfo ) );
		out.WriteL( ptrStringValue16 );
		out.Close();
		iView.SetColL( iColSet->ColNo( KNSmlAgentLogImplementationUID ), iImplUid );
		iView.SetColL( iColSet->ColNo( KNSmlAgentLogLocalDatabase ), *iLocalDB );
		iView.SetColL( iColSet->ColNo( KNSmlAgentLogServerId ), *iServerId );
		iView.SetColL( iColSet->ColNo( KNSmlAgentLogRemoteDatabase ), *iRemoteDB );
		iView.PutL();
		}
	
	CommitAndCompact();
	CleanupStack::PopAndDestroy( 4 ); // sql, out, stringValue16, filterData
	DBG_FILE(_S8("CNSmlDSAgentLog::SetServerFilterInfoL ends"));
	}

//=============================================
//		CNSmlDSAgentLog::GetServerFilterInfoL()
//		Gets Server Device Info.
//=============================================

EXPORT_C void CNSmlDSAgentLog::GetServerFilterInfoL( CArrayFix<TNSmlFilterCapData>* aFilterInfoArr )
	{
	DBG_FILE(_S8("CNSmlDSAgentLog::GetServerFilterInfoL begins"));
	
	HBufC* sql = AgentLogRowSqlLC();
	
	PrepareViewL( *sql, iView.EReadOnly );

	HBufC* stringValue;

	if ( iView.FirstL() )
		{
		iView.GetL();		
		RDbColReadStream in;
		stringValue = HBufC::NewLC( iView.ColLength( iColSet->ColNo( KNSmlAgentLogServerFilterInfo ) ) );
		in.OpenLC( iView, iColSet->ColNo( KNSmlAgentLogServerFilterInfo ) );		
		TPtr ptr = stringValue->Des();
		in.ReadL( ptr, iView.ColLength( iColSet->ColNo( KNSmlAgentLogServerFilterInfo ) ) );
		CleanupStack::PopAndDestroy( 1 ); // in
		}
	else
		{
		//couldn't find
		CleanupStack::PopAndDestroy( 1 ); // sql
		return;
		}
	
	TInt start( 0 );
	TInt firstSeparator( 0 );
	TBool firstFound( EFalse );
	TLex lex;
	TInt numPart( 0 );
	TPtr ptrStringValue = stringValue->Des();

	for ( TInt i = 0; i < stringValue->Length(); i++ )
		{
		if ( ptrStringValue[i] == ';' ) //; = separator
			{
			if ( !firstFound )
				{
				firstSeparator = i;
				firstFound = ETrue;
				}
			else
				{
				lex.Assign( stringValue->Mid( start, firstSeparator - start ) );
				lex.Val( numPart );

				TPtrC ptrTemp = stringValue->Mid( firstSeparator + 1, i - firstSeparator - 1 );
				
				HBufC8* stringPart = HBufC8::NewLC( ptrTemp.Length() );
				TPtr8 ptrStringPart = stringPart->Des();
				
				CnvUtfConverter::ConvertFromUnicodeToUtf8( ptrStringPart, ptrTemp );				

				aFilterInfoArr->AppendL( TNSmlFilterCapData(  TNSmlFilterCapData::TNSmlFilterCapTag(numPart), ptrStringPart ) );

				CleanupStack::PopAndDestroy( 1 ); //stringPart
				
				start = i + 1;
				firstFound = EFalse;
				}
			}
		}
	CleanupStack::PopAndDestroy( 2 ); // sql, stringValue
	DBG_FILE(_S8("CNSmlDSAgentLog::GetServerFilterInfoL ends"));
	}

//=============================================
//		CNSmlDSAgentLog::GenerateIdL()
//		Generates a unique Id (Id's highest value before + 1)
//		to a given table.
//=============================================

TInt CNSmlDSAgentLog::GenerateIdL()
	{
	TInt lastKey( 0 );
	if ( iTableAgentLog.LastL() )
		{
		iTableAgentLog.GetL();
		lastKey = iTableAgentLog.ColInt( iColSet->ColNo( KNSmlAgentLogId ) );
		iTableAgentLog.FirstL();
		iTableAgentLog.GetL();
		while ( iTableAgentLog.NextL() )
			{
			iTableAgentLog.GetL();
			if ( lastKey < iTableAgentLog.ColInt( iColSet->ColNo( KNSmlAgentLogId ) ) )
				{
				lastKey = iTableAgentLog.ColInt( iColSet->ColNo( KNSmlAgentLogId ) );
				}
			}
		}
	return ( lastKey + 1 );
	}

//=============================================
//		CNSmlDSAgentLog::InitNewRowL()
//		Initialises new row in Agent Log table
//		
//=============================================

void CNSmlDSAgentLog::InitNewRowL()
	{
    // Check OOD before inserting new row into AgentLog table
    if ( SysUtil::FFSSpaceBelowCriticalLevelL( &iFsSession,
            KNSmlAgentLogMaxFixedLogRowSize + KNSmlAgentLogTreshold ) )
	    {
        iDatabase.Rollback();
        User::Leave( KErrDiskFull );
		}
	
	// Insert new row
	iID = GenerateIdL();
	iTableAgentLog.InsertL();
	iTableAgentLog.SetColL( iColSet->ColNo( KNSmlAgentLogId ), iID );
	iTableAgentLog.SetColL( iColSet->ColNo( KNSmlAgentLogImplementationUID ), iImplUid );
	iTableAgentLog.SetColL( iColSet->ColNo( KNSmlAgentLogLocalDatabase ), *iLocalDB );
	iTableAgentLog.SetColL( iColSet->ColNo( KNSmlAgentLogServerId ), *iServerId );
	iTableAgentLog.SetColL( iColSet->ColNo( KNSmlAgentLogRemoteDatabase ), *iRemoteDB );
	}

//=============================================
//		CNSmlDSAgentLog::AgentLogRowSqlLC()
//		Makes SQL string for Agent Log table
//		
//=============================================

HBufC* CNSmlDSAgentLog::AgentLogRowSqlLC() const
	{
	HBufC* sql = HBufC::NewLC( KSQLGetAgentLogRowWithId().Length() + 5 );    
	TPtr sqlPtr = sql->Des();
	sqlPtr.Format( KSQLGetAgentLogRowWithId, iID );
	return sql;
	}

//=============================================
//		CNSmlDSAgentLog::AgentLogRowSqlLC()
//		Makes SQL string for Authentication table
//		
//=============================================

HBufC* CNSmlDSAgentLog::AuthenticationSqlLC( const TDesC& aServerId ) const
	{
	HBufC* sql = HBufC::NewLC( KSQLGetAuthenticationRow().Length() + aServerId.Length() );    
	TPtr sqlPtr = sql->Des();
	sqlPtr.Format( KSQLGetAuthenticationRow, &aServerId );
	return sql;
	}

//=============================================
//		CNSmlDSAgentLog::PrepareViewL()
//		Closes and prepares the view
//		
//=============================================

void CNSmlDSAgentLog::PrepareViewL( const TDesC& aSql, RDbRowSet::TAccess aAccess )
	{
	iView.Close();
	User::LeaveIfError( iView.Prepare( iDatabase, TDbQuery( aSql ), aAccess ) );
	}

//=============================================
//		CNSmlDSAgentLog::CommitAndCompact
//		Commits update and compacts the database
//		
//=============================================

void CNSmlDSAgentLog::CommitAndCompact() 
	{
	iDatabase.Commit();
	iDatabase.Compact();
	}

// <MAPINFO_RESEND_MOD_BEGIN>
	
//=============================================
//		
//=============================================
EXPORT_C MStreamBuf* CNSmlDSAgentLog::OpenReadStreamL( TUid aUid )
	{
	DBG_FILE(_S8("CNSmlDSAgentLog::OpenReadStreamL(): begin"));
	if( iReadStreamOpen )
		{
		DBG_FILE(_S8("CNSmlDSAgentLog::OpenReadStreamL(): end (readstream already open)"));
		User::Leave(KErrInUse);
		}

	if( !IsPresentL(aUid) )
		{
		DBG_FILE(_S8("CNSmlDSAgentLog::OpenReadStreamL(): end (readstream not found)"));
		User::Leave(KErrNotFound);
		}

	_LIT(KSQLStatement, "SELECT DatastoreData FROM Datastore WHERE DatastoreAID = %d AND DatastoreSID = %d");

	HBufC* sqlStatement = HBufC::NewLC( KSQLStatement().Length() + 15);
	TPtr sqlStatementPtr = sqlStatement->Des();

	sqlStatementPtr.Format(KSQLStatement(), iID, aUid.iUid);
	
	// create a view on the database
	User::LeaveIfError(iRSView.Prepare(iDatabase, TDbQuery(sqlStatementPtr, EDbCompareNormal), RDbRowSet::EReadOnly));
	User::LeaveIfError(iRSView.EvaluateAll());
	
	// Get the structure of rowset
	CDbColSet* colSet = iRSView.ColSetL(); 
	TDbColNo col = colSet->ColNo(KNSmlDatastoreStreamData()); // Ordinal position of long column
	delete colSet;
	
	// get row
	if( iRSView.FirstL() )
		{
		iRSView.GetL();
		iRs.OpenL(iRSView, col);
		}

	iReadStreamOpen = ETrue;

	DBG_FILE(_S8("CNSmlDSAgentLog::OpenReadStreamL(): end"));
	
	CleanupStack::PopAndDestroy(); // sqlStatement

	return iRs.Source();
	}

//=============================================
//		Open writestream for Uid		
//=============================================
EXPORT_C MStreamBuf* CNSmlDSAgentLog::OpenWriteStreamL( TUid aUid )
	{
	DBG_FILE(_S8("CNSmlDSAgentLog::OpenWriteStreamL(): begin"));
	if( iWriteStreamOpen )
		{
		DBG_FILE(_S8("CNSmlDSAgentLog::OpenWriteStreamL(): end (writestream already open)"));
		User::Leave(KErrInUse);
		}

	if( !IsPresentL(aUid) )
		{
		DBG_FILE(_S8("CNSmlDSAgentLog::OpenWriteStreamL(): creating new stream begin"));
		iDatabase.Begin();
		RDbView view;
		User::LeaveIfError(view.Prepare(iDatabase, TDbQuery(KSQLGetDatastoreAll()), RDbView::EUpdatable));
		DBG_FILE(_S8("CNSmlDSAgentLog::OpenWriteStreamL(): Preparing view successful"));
		CleanupClosePushL(view);
		view.InsertL();
		view.SetColL( iColSetTableDS->ColNo( KNSmlDatastoreAgentlogID ), iID );
		view.SetColL( iColSetTableDS->ColNo( KNSmlDatastoreStreamID ), aUid.iUid );
		view.PutL();
		CleanupStack::PopAndDestroy(); // view
		CommitAndCompact();
		DBG_FILE(_S8("CNSmlDSAgentLog::OpenWriteStreamL(): creating new stream end"));
		}

	_LIT(KSQLStatement, "SELECT DatastoreData FROM Datastore WHERE DatastoreAID = %d AND DatastoreSID = %d");

	HBufC* sqlStatement = HBufC::NewLC( KSQLStatement().Length() + 15 );
	TPtr sqlStatementPtr = sqlStatement->Des();
	sqlStatementPtr.Format(KSQLStatement(), iID, aUid.iUid);
	
	// create a view on the database
	User::LeaveIfError(iWSView.Prepare(iDatabase, TDbQuery(sqlStatementPtr, EDbCompareNormal), RDbRowSet::EUpdatable));
	DBG_FILE(_S8("CNSmlDSAgentLog::OpenWriteStreamL(): Preparing view successful (second)"));
	User::LeaveIfError(iWSView.EvaluateAll());
	DBG_FILE(_S8("CNSmlDSAgentLog::OpenWriteStreamL(): EvaluateAll successful..."));
	
	// Get the structure of rowset
	CDbColSet* colSet = iWSView.ColSetL(); 
	TDbColNo col = colSet->ColNo(KNSmlDatastoreStreamData()); // Ordinal position of long column
	delete colSet;
	
	// get row
	if( iWSView.FirstL() )
		{
		iWSView.UpdateL();
		iWs.OpenL(iWSView, col);
		}
	
	iWriteStreamOpen = ETrue;

	DBG_FILE(_S8("CNSmlDSAgentLog::OpenWriteStreamL(): end"));
	
	CleanupStack::PopAndDestroy(); // sqlStatement

	return iWs.Sink();
	}

//=============================================
//		Check if Uid already in use
//=============================================
EXPORT_C TBool CNSmlDSAgentLog::IsPresentL( TUid aUid )
	{
	DBG_FILE(_S8("CNSmlDSAgentLog::IsPresentL(): begin"));
	_LIT(KSQLStatement, "SELECT DatastoreData FROM Datastore WHERE DatastoreAID = %d AND DatastoreSID = %d");

	HBufC* sqlStatement = HBufC::NewLC( KSQLStatement().Length() + 15);
	TPtr sqlStatementPtr = sqlStatement->Des();

	sqlStatementPtr.Format(KSQLStatement(), iID, aUid.iUid);
	RDbView view;
	User::LeaveIfError(view.Prepare(iDatabase, TDbQuery(sqlStatementPtr, EDbCompareNormal), RDbRowSet::EReadOnly));
	CleanupClosePushL(view);
	User::LeaveIfError(view.EvaluateAll());
	TBool result(!view.IsEmptyL());
	DBG_FILE_CODE( result, _S8("CNSmlDSAgentLog::IsPresentL(): IsEmpty called...") );

	CleanupStack::PopAndDestroy(2); // view, sqlStatement

	DBG_FILE(_S8("CNSmlDSAgentLog::IsPresentL(): end"));
	return result;
	}

//=============================================
//		Delete stream for Uid		
//=============================================
EXPORT_C void CNSmlDSAgentLog::DeleteStreamL( TUid aUid )
	{
	DBG_FILE(_S8("CNSmlDSAgentLog::DeleteStreamL(): begin"));
	_LIT( KSQLDeleteDatastoreRow, "DELETE FROM Datastore WHERE DatastoreAID = %d AND DatastoreSID = %d" );

	HBufC* sqlStatement = HBufC::NewLC( KSQLDeleteDatastoreRow().Length() + 15);
	TPtr sqlStatementPtr = sqlStatement->Des();

	sqlStatementPtr.Format(KSQLDeleteDatastoreRow(), iID, aUid.iUid);
	iDatabase.Begin();
	TInt err = iDatabase.Execute(sqlStatementPtr);
	if( err < KErrNone )
		{
		iDatabase.Rollback();
		}
	else
		{
		iDatabase.Commit();
		}
	iDatabase.Compact();
	
	CleanupStack::PopAndDestroy(); // sqlStatement
	DBG_FILE(_S8("CNSmlDSAgentLog::DeleteStreamL(): end"));
	}

//=============================================
//		Delete stream for datastore
//=============================================
EXPORT_C void CNSmlDSAgentLog::DeleteStoreL()
	{
	DBG_FILE(_S8("CNSmlDSAgentLog::DeleteStoreL(): begin"));
	_LIT( KSQLDeleteDatastore, "DELETE FROM Datastore WHERE DatastoreAID = %d" );

	HBufC* sqlStatement = HBufC::NewLC( KSQLDeleteDatastore().Length() + 15);
	TPtr sqlStatementPtr = sqlStatement->Des();

	sqlStatementPtr.Format(KSQLDeleteDatastore(), iID);
	iDatabase.Begin();
	TInt err = iDatabase.Execute(sqlStatementPtr);
	if( err < KErrNone )
		{
		iDatabase.Rollback();
		}
	else
		{
		iDatabase.Commit();
		}
	iDatabase.Compact();

	CleanupStack::PopAndDestroy(); // sqlStatement

	DBG_FILE(_S8("CNSmlDSAgentLog::DeleteStoreL(): end"));
	}

//=============================================
//		Close write stream
//=============================================
EXPORT_C void CNSmlDSAgentLog::CloseWriteStreamL( TBool aCommit )
	{
	DBG_FILE(_S8("CNSmlDSAgentLog::CloseWriteStreamL(): begin"));
	if( !iWriteStreamOpen )
		{
		DBG_FILE(_S8("CNSmlDSAgentLog::CloseWriteStreamL(): end (no writestream open)"));
		return;
		}
	iWs.Close();
	if( aCommit )
		{
		// Check OOD before saving
		if (SysUtil::FFSSpaceBelowCriticalLevelL(&iFsSession, KNSmlAgentLogFatMinSize))
			{
			iWSView.Close();
			iWriteStreamOpen = EFalse;
			User::Leave( KErrDiskFull );
			}
		iWSView.PutL();
		}
	else
		{
		iWSView.Cancel();
		}
	iWSView.Close();
	iWriteStreamOpen = EFalse;
	DBG_FILE(_S8("CNSmlDSAgentLog::CloseWriteStreamL(): end"));
	}

//=============================================
//		Close read stream
//=============================================
EXPORT_C void CNSmlDSAgentLog::CloseReadStreamL()
	{
	DBG_FILE(_S8("CNSmlDSAgentLog::CloseReadStreamL(): begin"));
	if( !iReadStreamOpen )
		{
		return;
		}
	iRs.Close();
	iRSView.Close();
	iReadStreamOpen = EFalse;
	DBG_FILE(_S8("CNSmlDSAgentLog::CloseReadStreamL(): end"));
	}

// <MAPINFO_RESEND_MOD_END>

//End of File
