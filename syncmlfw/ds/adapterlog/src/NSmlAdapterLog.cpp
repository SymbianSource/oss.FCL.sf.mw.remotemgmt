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
* Description:  AdapterLog source
*
*/



// INCLUDE FILES
#include <s32file.h>
#include <s32stor.h>
#include <bautils.h>

#include <nsmlconstants.h>
#include<nsmlconstantdefs.h>
#include <nsmldsconstants.h>
#include "NSmlAdapterLog.h"


// STATIC CONSTANTS
const TInt CNSmlAdapterLog::KMaxPathLength;


// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CNSmlAdapterLog::CNSmlAdapterLog
// C++ default constructor.
// -----------------------------------------------------------------------------
//
CNSmlAdapterLog::CNSmlAdapterLog()
    {
    }

// -----------------------------------------------------------------------------
// CNSmlAdapterLog::ConstructL
// Symbian 2nd phase constructor.
// -----------------------------------------------------------------------------
//
void CNSmlAdapterLog::ConstructL()
    {
    User::LeaveIfError(iFsSession.Connect());
    User::LeaveIfError(iRdbSession.Connect());

    iPath = HBufC::NewL(CNSmlAdapterLog::KMaxPathLength);
    TPtr path = iPath->Des();

    User::LeaveIfError(iFsSession.SetSessionToPrivate(KNSmlAdapterLogDrive));
    User::LeaveIfError(iFsSession.SessionPath(path));

    BaflUtils::EnsurePathExistsL(iFsSession, *iPath);
    delete iPath;
    iPath = NULL;
    
    TParse name;
    
#ifdef SYMBIAN_SECURE_DBMS
	name.Set(KNSmlAdapterLogDbName(), NULL, NULL);
#else
    name.Set(KNSmlAdapterLogDbName(), KNSmlDatabasesNonSecurePath, NULL);
#endif
    TInt err(KErrNone);
    RProcess currentProcess;
    const TSecureId secureId = currentProcess.SecureId();
    currentProcess.Close();
    if ( secureId.iId == KNSmlDSHostRWServerSecureID.iId )
        {
        err = iDatabase.Open(iRdbSession,
                             name.FullName(), KNSmlDBMSSecureDSHostRWID);
        if ( err == KErrNotFound )
            {
            CreateDatabaseL(name.FullName());
            err = iDatabase.Open(iRdbSession,
                               name.FullName(), KNSmlDBMSSecureDSHostRWID);
            }
        }
    else if (secureId.iId == KNSmlDSHostNULLServerSecureID.iId)
        {
        err = iDatabase.Open(iRdbSession,
                             name.FullName(), KNSmlDBMSSecureDSHostNULLID);
        if ( err == KErrNotFound )
            {
            CreateDatabaseL(name.FullName());
            err = iDatabase.Open(iRdbSession, name.FullName(), 
                                 KNSmlDBMSSecureDSHostNULLID);
            
            }
        }
    else
        {
        User::Leave( KErrAccessDenied );
        }

    User::LeaveIfError(err);            
    User::LeaveIfError(iTable.Open(iDatabase, KNSmlTableAdapterLog));
    iColSet = iTable.ColSetL();
    }

// -----------------------------------------------------------------------------
// CNSmlAdapterLog::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CNSmlAdapterLog* CNSmlAdapterLog::NewL()
    {
    CNSmlAdapterLog* self = CNSmlAdapterLog::NewLC();
    CleanupStack::Pop(self);

    return self;
    }

// -----------------------------------------------------------------------------
// CNSmlAdapterLog::NewLC
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CNSmlAdapterLog* CNSmlAdapterLog::NewLC()
    {
    CNSmlAdapterLog* self = new (ELeave) CNSmlAdapterLog;
    CleanupStack::PushL(self);
    self->ConstructL();
 
    return self;
    }
    
// -----------------------------------------------------------------------------
// CNSmlAdapterLog::~CNSmlAdapterLog()
// Destructor
// -----------------------------------------------------------------------------
CNSmlAdapterLog::~CNSmlAdapterLog()
    {
    delete iPath;
    
    if (iDictionaryStore)
        {
        iDictionaryStore->Commit();
        delete iDictionaryStore;
        }
    
   	iView.Close();
    delete iColSet;
    
    iTable.Close();    
    iDatabase.Close();
    iFsSession.Close();
    iRdbSession.Close();
    }

// -----------------------------------------------------------------------------
// CNSmlAdapterLog::SetAdapterLogKeyL
// Sets the sync relationship key.
// -----------------------------------------------------------------------------
//
EXPORT_C void CNSmlAdapterLog::SetAdapterLogKeyL(
             const TInt aImplUid, const TDesC& aLocalDB, 
             const TDesC& aServerId, const TDesC& aRemoteDB)
    {
    HBufC* sql = HBufC::NewLC(KSqlGetAdapterLogRow().Length() + 5 + 
        aLocalDB.Length() + aServerId.Length() + aRemoteDB.Length());
        
    TPtr tmp = sql->Des();
    tmp.Format(KSqlGetAdapterLogRow, aImplUid, &aLocalDB, &aServerId,
        &aRemoteDB);
    
    iView.Close();
    User::LeaveIfError(iView.Prepare(iDatabase, TDbQuery(*sql),
                                     RDbView::/*TAccess::*/EUpdatable));
    
    if (iView.FirstL())
        {
        // existing row
        iView.GetL();
        iID = iView.ColInt(iColSet->ColNo(KNSmlAdapterLogId));
        }
    else
        {
        // new row
        iID = GenerateIdL();
        iDatabase.Begin();
        iView.InsertL();
        iView.SetColL(iColSet->ColNo(KNSmlAdapterLogId), iID);
        iView.SetColL(iColSet->ColNo(KNSmlAdapterLogImplementationUID), aImplUid);
        iView.SetColL(iColSet->ColNo(KNSmlAdapterLogLocalDatabase), aLocalDB);
        iView.SetColL(iColSet->ColNo(KNSmlAdapterLogServerId), aServerId);
        iView.SetColL(iColSet->ColNo(KNSmlAdapterLogRemoteDatabase), aRemoteDB);   
        iView.PutL();
        iDatabase.Commit();
        iDatabase.Compact();
        }
                
    CleanupStack::PopAndDestroy(sql);

    OpenDictionaryStoreL();
    }

// -----------------------------------------------------------------------------
// CNSmlAdapterLog::SetServerIdL
// Sets server id.
// -----------------------------------------------------------------------------
//    
EXPORT_C void CNSmlAdapterLog::SetServerIdL( TDesC& aOldServerId, TDesC& aNewValue )
	{
	HBufC* sql = HBufC::NewLC( KSqlGetAdapterLogRowFromServerId().Length() + aOldServerId.Length() );
	
	TPtr tmp = sql->Des();
    tmp.Format( KSqlGetAdapterLogRowFromServerId,  &aOldServerId );
	
	iView.Close();
	User::LeaveIfError( iView.Prepare( iDatabase, TDbQuery(*sql),
                                     RDbView::EUpdatable ) );
	iDatabase.Begin();
	if ( iView.FirstL() )
		{
		//existing row
		iView.GetL();
		iView.UpdateL();
		iView.SetColL( iColSet->ColNo( KNSmlAdapterLogServerId ), aNewValue );
		iView.PutL();
		
		iDatabase.Commit();
		iDatabase.Compact();
		}

	CleanupStack::PopAndDestroy( sql );
	}

// -----------------------------------------------------------------------------
// CNSmlAdapterLog::SyncTaskKey
// Returns the unique identifier for the synchronization relationship. The
// default value for the identifier (zero) is returned, if the adapter log key
// has not been set.
// -----------------------------------------------------------------------------
//
EXPORT_C TSmlSyncTaskKey CNSmlAdapterLog::SyncTaskKey() const
    {
    return static_cast<TSmlSyncTaskKey>(iID);    
    }

// -----------------------------------------------------------------------------
// CNSmlAdapterLog::OpenReadStreamLC
// Opens the dictionary stream associated with the given UID, leaving the 
// cleanup item on cleanup stack. The adapter log key has to be set before this
// method can be used. Method leaves, if association with a stream and the
// given uid does not exist in the store.
// -----------------------------------------------------------------------------
//
EXPORT_C void CNSmlAdapterLog::OpenReadStreamLC(RReadStream& aReadStream,
                                                TUid aStreamUid)
    {
    if (!IsStreamPresentL(aStreamUid))
        {
        User::Leave(KErrNotFound);
        }

    static_cast<RDictionaryReadStream&>(aReadStream).OpenLC(
        *iDictionaryStore, aStreamUid);
    }

// -----------------------------------------------------------------------------
// CNSmlAdapterLog::OpenWriteStreamLC
// Opens the dictionary stream associated with the given UID, leaving the
// cleanup item on cleanup stack. If no stream is associated with the UID, new
// stream is created with association to given UID. The adapter log key has to
// be set before this method can be used.
// -----------------------------------------------------------------------------
//
EXPORT_C void CNSmlAdapterLog::OpenWriteStreamLC(RWriteStream& aWriteStream,
                                                 TUid aStreamUid)
    {
    if (iID == NULL)
        {
        User::Leave(KErrNotFound);
        }
        
    static_cast<RDictionaryWriteStream&>(aWriteStream).AssignLC(
        *iDictionaryStore, aStreamUid);
    }

// -----------------------------------------------------------------------------
// CNSmlAdapterLog::IsStreamPresentL
// Checks if a stream associated with given UID is present in the opened
// dictionary store. The adapter log key has to be set before this
// method can be used.
// -----------------------------------------------------------------------------
//
EXPORT_C TBool CNSmlAdapterLog::IsStreamPresentL(TUid aStreamUid) const
    {
    if (iID == NULL)
        {
        User::Leave(KErrNotFound);
        }
        
    return iDictionaryStore->IsPresentL(aStreamUid);
    }

// -----------------------------------------------------------------------------
// CNSmlAdapterLog::CreateDatabaseL
// Creates new database with AdapterLog table in the specified file.
// -----------------------------------------------------------------------------
//
void CNSmlAdapterLog::CreateDatabaseL(const TDesC& aFullName)
    {
	_LIT(KCreateAdapterLogTable, "CREATE TABLE AdapterLog (Id \
	INTEGER NOT NULL, ImplementationUID INTEGER NOT NULL, LocalDatabase CHAR(%d) \
	NOT NULL, ServerId CHAR(%d), RemoteDatabase CHAR(%d) NOT NULL)");
    
    HBufC* createAdapterLogTable = 
        HBufC::NewLC(KCreateAdapterLogTable().Length() + 20);
        
    TPtr AdapterLogTablePtr = createAdapterLogTable->Des();
    
    AdapterLogTablePtr.Format(KCreateAdapterLogTable, KNSmlMaxDataLength,
                              KNSmlMaxServerIdLength, KNSmlMaxRemoteNameLength);
    
    RProcess currentProcess;
    const TSecureId secureId = currentProcess.SecureId();
    currentProcess.Close();
    if ( secureId.iId == KNSmlDSHostRWServerSecureID.iId )
        {
        User::LeaveIfError(iDatabase.Create(iRdbSession,
                                            aFullName,
                                            KNSmlDBMSSecureDSHostRWID));
        }
    else if (secureId.iId == KNSmlDSHostNULLServerSecureID.iId)
        {
        User::LeaveIfError(iDatabase.Create(iRdbSession,
                                            aFullName,
                                            KNSmlDBMSSecureDSHostNULLID));
        }
    else
        {
        User::Leave( KErrAccessDenied );
        }
    iDatabase.Begin();
    iDatabase.Execute(*createAdapterLogTable);                                          
    iDatabase.Commit();
	iDatabase.Compact();
	iDatabase.Close();
	
	CleanupStack::PopAndDestroy(createAdapterLogTable);
    }
    
// -----------------------------------------------------------------------------
// CNSmlAdapterLog::GenerateIdL
// Generates a unique identifier for the table by searching the adapter log ID
// with highest value and increments it.
// -----------------------------------------------------------------------------
//
TInt CNSmlAdapterLog::GenerateIdL()
    {
    TInt highestKey(0);
    if (iTable.LastL())
        {
        TInt currentKey(0);
        do
            {
            iTable.GetL();
            currentKey = iTable.ColInt(iColSet->ColNo(KNSmlAdapterLogId));
            if (highestKey < currentKey)
                {
                highestKey = currentKey;
                }
            }
        while (iTable.PreviousL());
        }
    return highestKey + 1;    
    }
        
// -----------------------------------------------------------------------------
// CNSmlAdapterLog::OpenDictionaryStoreL
// Opens the dictionary file store associated with the adapter log ID.
// If dictionary store has already been opened, method establishes new commit
// point for opened store and closes the store before opening another.
// -----------------------------------------------------------------------------
//
void CNSmlAdapterLog::OpenDictionaryStoreL()
    {
    if (iDictionaryStore)
        {
        iDictionaryStore->CommitL();
        delete iDictionaryStore;
        iDictionaryStore = NULL;
        }
          
    HBufC* storeName = HBufC::NewLC(KNSmlAdapterLogStore().Length() + 10);
    TPtr tmp = storeName->Des();
    tmp.Format(KNSmlAdapterLogStore(), iID);

    iPath = HBufC::NewL(CNSmlAdapterLog::KMaxPathLength);
    TPtr path = iPath->Des();
 
    TParse file;
    User::LeaveIfError(file.Set(*storeName, iPath, NULL));
    
    iDictionaryStore = CDictionaryFileStore::OpenL(iFsSession, file.FullName(),
                                                   TUid::Uid(iID));
    
    CleanupStack::PopAndDestroy(storeName);
    }
    

//  End of File
