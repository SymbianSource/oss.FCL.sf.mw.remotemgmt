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
* Description:  AgentLog header
*
*/



#ifndef __AGENTLOG_H
#define __AGENTLOG_H


// INCLUDE FILES
#include <e32std.h>
#include <e32base.h>
#include <d32dbms.h>
#include <f32file.h>
#include <s32file.h>

// CONSTANTS

//Database name & drive
const TInt KNSmlAgentLogDbDrive = EDriveC;
_LIT( KNSmlAgentLogDbName, "c:nsmlagentlog.db" );

const TInt  KNSmlAgentLogFatMinSize =  1000;
const TInt  KNSmlAgentLogFatMaxSize = 10000;

//Database tables
_LIT( KNSmlTableAgentLog, "AgentLog" );
_LIT( KNSmlTableAuthentication, "Authentication" );

// <MAPINFO_RESEND_BEGIN>
_LIT( KNSmlTableDatastore, "Datastore" );
// <MAPINFO_RESEND_END>

//table AgentLog columns
_LIT( KNSmlAgentLogId, "Id" );
_LIT( KNSmlAgentLogImplementationUID, "ImplementationUID" );
_LIT( KNSmlAgentLogLocalDatabase, "LocalDatabase" );
_LIT( KNSmlAgentLogServerId, "ServerId" );
_LIT( KNSmlAgentLogRemoteDatabase, "RemoteDatabase" );
_LIT( KNSmlAgentLogLastSyncronised, "LastSyncronised" );
_LIT( KNSmlAgentLogSlowSyncRequest, "SlowSyncRequest" );
_LIT( KNSmlAgentLogLastSyncAnchor, "SyncAnchor" );
_LIT( KNSmlAgentLogServerDeviceInfo, "ServerDeviceInfo" );
_LIT( KNSmlAgentLogServerFilterInfo, "ServerFilterInfo" );
_LIT( KNSmlAgentLogServerSupportsHierarchicalSync, "ServerSupportsHierarchicalSync" );
_LIT( KNSmlAgentLogServerSupportsNOC, "ServerSupportsNOC" );
_LIT( KNSmlAgentLogServerSyncTypes, "ServerSyncTypes" );
//RD_SUSPEND_RESUME
_LIT( KNSmlAgentLogSuspendedState ,"SyncSuspendedState");
_LIT( KNSmlAgentLogPreviousSyncType, "PreviousSyncType");
_LIT( KNSmlAgentLogPreviousClientSyncType,"PreviousClientSyncType");
//RD_SUSPEND_RESUME
//table Authentication columns
_LIT( KNSmlAuthenticationServerId, "ServerId" );
_LIT( KNSmlAuthenticationNonce, "Nonce" );
_LIT( KNSmlAuthenticationType, "Type" );
_LIT( KNSmlAuthenticationSessionId, "SessionId" );

// SQL statements
_LIT( KSQLGetAgentLogRow, "SELECT * FROM AgentLog WHERE ImplementationUID = %d \
AND LocalDatabase = '%S' AND ServerId = '%S'" );
_LIT( KSQLGetAgentLogRowWithId, "SELECT * FROM AgentLog WHERE Id = %d" );
_LIT( KSQLGetAgentLogRowWithServerId, "SELECT * FROM AgentLog WHERE ServerId = '%S'" );
_LIT( KSQLGetAgentLogAll, "SELECT * FROM AgentLog" );
_LIT( KSQLGetAuthenticationRow, "SELECT * FROM Authentication WHERE ServerId = '%S'" );
_LIT( KSQLGetAuthenticationAll, "SELECT * FROM Authentication" );
_LIT( KColumn, "%S");

// DATA TYPES

enum TNSmlAgentLogData
	{
	EAgentLogImplementationUID,
	EAgentLogLocalDatabase,
	EAgentLogServerId,
	EAgentLogRemoteDatabase,
	EAgentLogLastSyncronised,
	EAgentLogSlowSyncRequest,
	EAgentLogLastSyncAnchor,
	EAgentLogServerDeviceInfo,
	EAgentLogServerSupportsNOC,
	EAgentLogServerSyncTypes,
	EAgentLogServerFilterInfo,
	EAgentLogServerSupportsHierarchicalSync,
	EAgentLogSyncSuspendedState,
	EAgentLogPreviousSyncType,
	EAgentLogPreviousClientSyncType
	};

enum TNSmlAgentLogAuthenticationType
	{
	EAgentLogNoAuth,
	EAgentLogBasic,
	EAgentLogMD5
	};
	
//RD_SUSPEND_RESUME
//Suspend state column values
enum TNSmlSyncSuspendedState
	{
	EStateNotStarted = 0,
	EStateClientModification,
	EStateServerModification,
	EStateDataUpdateStatus,
	EStateMapAcknowledge
	
	};
	
//Sync type column values
enum TNSmlPreviousSyncType
{ 
   ETypeNotSet=0,
   ESyncDSTwoWay=200,
   ESyncDSSlowSync,
   ESyncDSOneWayFromClient,
   ESyncDSRefreshFromClient,
   ESyncDSOneWayFromServer,
   ESyncDSRefreshFromServer,
   ESyncDSTwoWayByServer,
   ESyncDSOneWayFromClientByServer,
   ESyncDSRefreshFromClientByServer,
   ESyncDSOneWayFromServerByServer,
   ESyncDSRefreshFromServerByServer
};
//RD_SUSPEND_RESUME

// FORWARD DECLARATIONS
struct TNSmlCtCapData;
class CNSmlDbCaps;
struct TNSmlFilterCapData;

// <MAPINFO_RESEND_BEGIN>
// Table Modification columns
_LIT( KNSmlDatastoreRowID, "DatastoreRowID" );
_LIT( KNSmlDatastoreAgentlogID, "DatastoreAID" );
_LIT( KNSmlDatastoreStreamID, "DatastoreSID" );
_LIT( KNSmlDatastoreStreamData, "DatastoreData" );
_LIT( KSQLGetDatastoreAll, "SELECT * FROM Datastore" );
// <MAPINFO_RESEND_END>

// CLASS DECLARATION

/**
*  CNSmlDSAgentLog provides an interface for accessing AgentLog database.
*
*  @lib nsmlagentlog.lib
*/
class CNSmlDSAgentLog : public CBase
{
	public:
		enum TNSmlAgentDataStoreElement
			{
			EDataStore = 0x20,
			ESourceRef,
			EDisplayName,
			EMaxGUIDSize,
			ERxPref,
			ERx,
			ETxPref,
			ETx,
			EDSMem,
			ESupportHierarchicalSync,
			ESyncCap
			};
			
    public:  // Constructors and destructor
        
        /**
        * Two-phased constructor.
        */
		IMPORT_C static CNSmlDSAgentLog* NewL();	

        /**
        * Two-phased constructor.
        */
		IMPORT_C static CNSmlDSAgentLog* NewLC();

        /**
        * Destructor.
        */
		~CNSmlDSAgentLog();
		
    public: // New functions
		
		/**
        * Sets AgentLog key with given string values.
        * @param aImplUid Implementation UID.
        * @param aLocalDB Local database name.
        * @param aServerId Remote server ID.
        * @param aRemoteDB Remote database name.
        */
		IMPORT_C void SetAgentLogKeyL( const TInt aImplUid, const TDesC& aLocalDB, const TDesC& aServerId, const TDesC& aRemoteDB );
							
        /**
        * Sets the given integer value for the given AgentLog datatype.
        * @param aType Type of AgentLog data.
        * @param aNewValue Integer value to be set.
        */
		IMPORT_C void SetIntValueL( TNSmlAgentLogData aType, const TInt aNewValue );

        /**
        * Gets an integer value for the given AgentLog datatype.
        * @param aType Type of AgentLog data.
        * @return Integer value.
        */
		IMPORT_C TInt IntValueL( TNSmlAgentLogData aType );

        /**
        * Sets the given time value for the given AgentLog datatype.
        * @param aType Type of AgentLog data.
        * @param aNewValue Time value to be set.
        */
		IMPORT_C void SetTimeValueL( TNSmlAgentLogData aType, const TTime& aNewValue );

        /**
        * Gets the time value for the given AgentLog datatype. If the returned
        * value is TTime(0), given value was not found.
        * @param aType Type of AgentLog data.
        * @return Time value.
        */
		IMPORT_C TTime TimeValueL( TNSmlAgentLogData aType );

        /**
        * Sets the nonce data for given server ID.
        * @param aServerId Remote server ID.
        * @param aNewValue The nonce value to be set.
        */
		IMPORT_C void SetNonceL( const TDesC& aServerId, const TDesC& aNewValue );
		
		/**
		* Gets the nonce data for given server ID. If length of the returned
		* string is 0, nonce was not found.
		* @param aServerId Remote server ID.
		* @return Nonce data.
		*/
		IMPORT_C HBufC* NonceL( const TDesC& aServerId );

        /**
        * Sets the given session ID for the given server ID.
        * @param aServerId Remote server ID.
        * @param aSessionID Session ID.
        */
		IMPORT_C void SetSessionIDL( const TDesC& aServerId, TInt aSessionID );
		
		/**
		* Gets the session ID for the given server ID.
        * @param aServerId Remote server ID.
        * @return Session ID.
        */
		IMPORT_C TInt SessionIDL( const TDesC& aServerId );

        /**
        * Sets the authentication type for the given server ID.
        * @param aServerId Remote server ID.
        * @param aNewValue Authentication type.
        */
		IMPORT_C void SetAuthTypeL( const TDesC& aServerId, const TNSmlAgentLogAuthenticationType aNewValue );
		
	    /**
	    * Gets the authentication type for the given server ID.
        * @param aServerId Remote server ID.
        * @return Authentication type.
        */
		IMPORT_C TInt AuthTypeL( const TDesC& aServerId );
		
		/**
		* Replaces the give old server ID with new one.
        * @param aOldServerId Old remote server ID.
		* @param aNewValue New remote server ID.
		*/
		IMPORT_C void SetServerIdL( TDesC& aOldServerId, TDesC& aNewValue );

		/**
		* Sets the server device info for AgentLog.
		* @param aDbCaps Server's database capabilities.
		*/
		IMPORT_C void SetServerDeviceInfoL( const RPointerArray<CNSmlDbCaps>& aDbCaps );
		
		/**
		* Gets the server device info for AgentLog.
		* @param aDbCaps Server's database capabilities.
		*/
		IMPORT_C void GetServerDeviceInfoL( RPointerArray<CNSmlDbCaps>& aDbCaps );

        /**
        * Sets the server filter info for AgentLog.
        * @param aFilterInfoArr Server's filter capabilities.
        */
		IMPORT_C void SetServerFilterInfoL( const CArrayFix<TNSmlFilterCapData>* aFilterInfoArr );

        /**
        * Gets the server filter info for AgentLog.
        * @param aFilterInfoArr Server's filter capabilities.
        */
		IMPORT_C void GetServerFilterInfoL( CArrayFix<TNSmlFilterCapData>* aFilterInfoArr );

// <MAPINFO_RESEND_MOD_BEGIN>
        /**
        * Opens the read stream. 
        * @param aUid UID of the stream.
        * @return Pointer to the opened stream buffer. 
        */
		IMPORT_C MStreamBuf* OpenReadStreamL( TUid aUid );
        /**
        * Opens the write stream
        * @param aUid UID of the stream.
        * @return Pointer to the opened stream buffer. 
        */
		IMPORT_C MStreamBuf* OpenWriteStreamL( TUid aUid );
        /**
        * Checks if stream uid is already in use.
        * @param aUid UID of the stream.
        * @return ETrue if in use. Otherwise EFalse.
        */
		IMPORT_C TBool IsPresentL( TUid aUid );
        /**
        * Delete stream for Uid. 
        * @param aUid UID of the stream.
        */
		IMPORT_C void DeleteStreamL( TUid aUid );
        /**
        * Delete stream for datastore.
        */
		IMPORT_C void DeleteStoreL();
        /**
        * Closes write stream. 
        * @param aCommit Boolean value, which indicates if stream need to be committed before closing. 
        */
		IMPORT_C void CloseWriteStreamL( TBool aCommit = ETrue );
	 /**
        * Closes read stream.
        */
		IMPORT_C void CloseReadStreamL();
// <MAPINFO_RESEND_MOD_END>

	private:
	
        /**
        * Creates Agent Log database.
        * @param aFullName Name and path of database.
        */
		void CreateDatabaseL( const TDesC& aFullName );
		
		/**
		* Generates a unique Id to a given table.
		*/
		TInt GenerateIdL();
		
		/**
		* Initialises new row in AgentLog table with set key values.
		*/
		void InitNewRowL();
		
		/**
		* Makes an SQL query string for getting a row with currently set
		* AgentLog key from AgentLog table.
		*/
		HBufC* AgentLogRowSqlLC() const;
		
		/**
		* Makes an SQL query string for getting a row with given server ID
		* from Authentication table
		* @param aServerId Remote server ID.
		*/
		HBufC* AuthenticationSqlLC( const TDesC& aServerId ) const;
		
		/**
		* Closes the view and then prepares it with the given values.
		* @param aSql SQL statement defining the view.
		* @param aAccess Permitted operations for the view.
		*/
		void PrepareViewL( const TDesC& aSql, RDbRowSet::TAccess aAccess );
		
		/**
		* Commits update and compacts the database.
		*/
		void CommitAndCompact();

        /**
        * Symbian 2nd phase constructor.
        */
		void ConstructL();

    private:    // Data
		TInt iID;

		TInt iImplUid;
		HBufC* iLocalDB;
		HBufC* iServerId;
		HBufC* iRemoteDB;

		RDbNamedDatabase iDatabase;
		RFs iFsSession;
		RDbs iRdbSession;

		RDbTable iTableAgentLog;
		CDbColSet* iColSet;
		RDbView iView;

		RDbTable iTableAuthentication;	
		CDbColSet* iColSetTableAuth;
		
// <MAPINFO_RESEND_BEGIN>
		RDbTable iTableDatastore;	
		CDbColSet* iColSetTableDS;
		RDbView iWSView;
		RDbView iRSView;
		TBool iWriteStreamOpen;
		TBool iReadStreamOpen;
		RDbColWriteStream iWs;
		RDbColReadStream iRs;
// <MAPINFO_RESEND_END>
};


#endif      // __AGENTLOG_H   
            
// End of File
