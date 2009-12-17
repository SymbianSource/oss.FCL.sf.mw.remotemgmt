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
* Description:  AdapterLog header
*
*/



#ifndef NSMLADAPTERLOG_H
#define NSMLADAPTERLOG_H

//  INCLUDES
#include <f32file.h>
#include <d32dbms.h>
#include <SmlDataSyncDefs.h>

// CONSTANTS

// Default drive, where adapter log is stored
const TInt KNSmlAdapterLogDrive = EDriveC;


// Dictionary stores' filenames
// These are store to private path of process
_LIT(KNSmlAdapterLogStore, "nsmladapterlog%d.dat");

// Database name
// This is stored to DBMS server
_LIT(KNSmlAdapterLogDbName, "c:nsmladapterlog.db");

// Database tables
_LIT(KNSmlTableAdapterLog, "AdapterLog");

// AdapterLog table columns
_LIT(KNSmlAdapterLogId, "Id");
_LIT(KNSmlAdapterLogImplementationUID, "ImplementationUID");
_LIT(KNSmlAdapterLogLocalDatabase, "LocalDatabase");
_LIT(KNSmlAdapterLogServerId, "ServerId");
_LIT(KNSmlAdapterLogRemoteDatabase, "RemoteDatabase");

// SQL query
_LIT(KSqlGetAdapterLogRow, "SELECT * FROM AdapterLog WHERE ImplementationUID \
= %d AND LocalDatabase = '%S' AND ServerId = '%S' AND RemoteDatabase = '%S'" );

// row from known server id
_LIT(KSqlGetAdapterLogRowFromServerId, "SELECT * FROM AdapterLog WHERE ServerId = '%S'" );


// FORWARD DECLARATIONS
class CDictionaryStore;


// CLASS DECLARATION

/**
*  CNSmlAdapterLog.
*
*  @lib NSmlAdapterLog.lib
*  @since Series 60 3.0 
*/
class CNSmlAdapterLog : public CBase, public MSmlSyncRelationship
    {
    public:  // Constructors and destructor
        
        /**
        * Creates a new instance of the CNSmlAdapterLog class.
        * The method also creates the AdapterLog database if it does not
        * exist.
        */
        IMPORT_C static CNSmlAdapterLog* NewL();
        
        /**
        * Creates a new instance of the CNSmlAdapterLog class. Method
        * pushes and leaves the created instance on the cleanup stack.
        * The method also creates the AdapterLog database if it does not
        * exist.
        */
        IMPORT_C static CNSmlAdapterLog* NewLC();
        
        /**
        * Destructor.
        */
        ~CNSmlAdapterLog();

    public: // New functions
        
        /**
        * Sets the adapter log key with given values.
        * @param aImplUid Implementation UID.
        * @param aLocalDB Local database name.
        * @param aServerId Remote server ID.
        * @param Remote database name.
        */
        IMPORT_C void SetAdapterLogKeyL(const TInt aImplUid,
                                        const TDesC& aLocalDB,
                                        const TDesC& aServerId,
                                        const TDesC& aRemoteDB);
		/**
		* Replaces the give old server ID with new one.
        * @param aOldServerId Old remote server ID.
		* @param aNewValue New remote server ID.
		*/
		IMPORT_C void SetServerIdL( TDesC& aOldServerId, TDesC& aNewValue );
                                              
    public: // Functions from base classes
    
      	/**
      	* From MSmlSyncRelationship. Returns the unique identifier for the
      	* synchronisation relationship.
      	* @return Unique synchronisation task key.
        */
        IMPORT_C TSmlSyncTaskKey SyncTaskKey() const;
        
        /**
        * From MSmlSyncRelationship. Opens a specified stream for reading, and
        * places an item on the cleanup stack to close it.
        * @param aReadStream Stream to be opened for reading.
        * @param aStreamUid UID specifying the stream.
		*/
		IMPORT_C void OpenReadStreamLC(RReadStream& aReadStream,
		                               TUid aStreamUid);
		
		/**
		* From MSmlSyncRelationship. Opens the specified stream, or creates a
		* new one if it does not exist, and places an item on the cleanup stack
		* to close it.
		* @param aWriteStream Stream to be opened for writing.
		* @param aStreamUid UID specifying the stream.
		*/
		IMPORT_C void OpenWriteStreamLC(RWriteStream& aWriteStream,
		                                TUid aStreamUid);
		
		/**
		* From MSmlSyncRelationship. Tests if the specified stream identified
		* exists in the store.
		* @param aStreamUid UID specifying the stream.
		* @return ETrue, if specified stream is present.
		*/
		IMPORT_C TBool IsStreamPresentL(TUid aStreamUid) const;
      
    private:

        /**
        * C++ default constructor.
        */
        CNSmlAdapterLog();

        /**
        * Symbian 2nd phase constructor.
        */
        void ConstructL();

        void CreateDatabaseL(const TDesC& aFullName);
        TInt GenerateIdL();
        void OpenDictionaryStoreL();
        
    private:    // Data
        
        TInt iID;

        RDbNamedDatabase iDatabase;
        RFs iFsSession;
        RDbs iRdbSession;

        RDbTable iTable;
        CDbColSet* iColSet;
        RDbView iView;
        
        CDictionaryStore* iDictionaryStore;
        
        HBufC* iPath;
        static const TInt KMaxPathLength = 256;
    };

#endif      // NSMLADAPTERLOG_H   
            
// End of File
