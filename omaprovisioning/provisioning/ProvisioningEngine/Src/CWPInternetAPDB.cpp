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
* Description: create/save/get Internet connection methods from database.
*
*/

//Include files
#include "CWPInternetAPDB.h"
#include "ProvisioningDebug.h"
#include <sysutil.h>

#ifdef SYMBIAN_SECURE_DBMS
_LIT(KInternetSecureDBMShostID, "SECURE[101F84D6]");
#endif

const TInt KWPDBMSFileValue = 0x101F84D6;
const TUid KWPDBMSFileUid =
    {
    KWPDBMSFileValue
    };

CWPInternetAPDB* CWPInternetAPDB::NewL()
    {
    CWPInternetAPDB* self = NewLC();
    CleanupStack::Pop();

    return self;
    }

// -----------------------------------------------------------------------------
// CWPInternetAPDB::NewLC
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CWPInternetAPDB* CWPInternetAPDB::NewLC()
    {
    FLOG( _L( "[Provisioning] CWPInternetAPDB::NewLC:" ) );
    CWPInternetAPDB* self = new( ELeave ) CWPInternetAPDB;

    CleanupStack::PushL(self);
    self->ConstructL();

    FLOG( _L( "[Provisioning] CWPInternetAPDB::NewLC:done" ) );
    return self;
    }

// -----------------------------------------------------------------------------
// CWPInternetAPDB::~CWPInternetAPDB
// -----------------------------------------------------------------------------
//
CWPInternetAPDB::~CWPInternetAPDB()
    {
    FLOG( _L( "[Provisioning] CWPInternetAPDB::~CWPInternetAPDB" ) );
    iItemsDatabase.Close();
    iRdbSession.Close();
    iFsSession.Close();

    FLOG( _L( "[Provisioning] CWPInternetAPDB::~CWPInternetAPDB: done" ) );
    }

// -----------------------------------------------------------------------------
// CWPInternetAPDB::ConstructL
// -----------------------------------------------------------------------------
// 
void CWPInternetAPDB::ConstructL()
    {
    FLOG( _L( "[Provisioning] CWPInternetAPDB::ConstructL" ) );
    TParse name;
    TInt err;

    User::LeaveIfError(iFsSession.Connect());
    User::LeaveIfError(iRdbSession.Connect() );

    iFsSession.SetSessionToPrivate(EDriveC);

#ifdef SYMBIAN_SECURE_DBMS
    name.Set(KtxDatabaseName, NULL, NULL);
    err = iItemsDatabase.Open(iRdbSession, name.FullName(),
            KInternetSecureDBMShostID);
#else
    name.Set( KtxDatabaseName, KNonSecureDbFullName, NULL );
    err = iItemsDatabase.Open(iRdbSession, DBFileName);
#endif

    if (err == KErrNotFound)
        {
        CreateandOpenL(name);
        }
    else
        {

        if ( (err == KErrEof ) || (err == KErrCorrupt ) || (err
                == KErrArgument ))
            {
            // something seriously wrong with the db, delete it and try 
            // to create new
            iRdbSession.DeleteDatabase(name.FullName(), KWPDBMSFileUid);
            CreateandOpenL(name);
            }

        }

    FLOG( _L( "[Provisioning] CWPInternetAPDB::ConstructL: done" ) );
    }

// -----------------------------------------------------------------------------
// CWPInternetAPDB::CreateandOpenL
// -----------------------------------------------------------------------------
//    
inline void CWPInternetAPDB::CreateandOpenL(TParse& name)
    {
    FLOG( _L( "[Provisioning] CWPInternetAPDB::CreateandOpenL" ) );
    TInt err;

#ifdef SYMBIAN_SECURE_DBMS
    iItemsDatabase.Create(iRdbSession, name.FullName(),
            KInternetSecureDBMShostID);
#else
    if( SysUtil::FFSSpaceBelowCriticalLevelL( &iFsSession, KEmptyDbSizeEstimate ) )
        {
        User::Leave( KErrDiskFull );
        }
    iItemsDatabase.Create(iFsSession, name.FullName());
#endif	

    CreateTableL(iItemsDatabase);
    iItemsDatabase.Close();
#ifdef SYMBIAN_SECURE_DBMS

    err = iItemsDatabase.Open(iRdbSession, name.FullName(),
            KInternetSecureDBMShostID);

#else
    err = iItemsDatabase.Open(iFsSession, DBFileName);
#endif

    //Debug
    if (err != KErrNone)
        {
        FLOG(_L("Error man?" ));
        User::LeaveIfError(err);
        }

    FLOG( _L( "[Provisioning] CWPInternetAPDB::CreateandOpenL:done" ) );
    }

// -----------------------------------------------------------------------------
// CWPInternetAPDB::CreateTableL
// -----------------------------------------------------------------------------
//  
void CWPInternetAPDB::CreateTableL(RDbDatabase& aDatabase)
    {
    FLOG( _L( "[Provisioning] CWPInternetAPDB::CreateTableL" ) );

    // Create a table definition
    CDbColSet* columns=CDbColSet::NewLC();

    // Add Columns
    TDbCol id(NCol1, EDbColInt32);

    // automatic indexing for items,it is our key field.
    id.iAttributes=id.EAutoIncrement;
    columns->AddL(id);

    columns->AddL(TDbCol(NCol2, EDbColInt32));
    columns->AddL(TDbCol(NCol3, EDbColInt32));
    columns->AddL(TDbCol(NCol4, EDbColText, KOriginatorMaxLength));

    // Create a table
    User::LeaveIfError(aDatabase.CreateTable(KtxtItemlist, *columns) );

    // cleanup the column set
    CleanupStack::PopAndDestroy(columns);
    FLOG( _L( "[Provisioning] CWPInternetAPDB::CreateTableL: done" ) );
    }

// -----------------------------------------------------------------------------
// CWPInternetAPDB::ReadDbItemsForOrigL
// -----------------------------------------------------------------------------
//   
void CWPInternetAPDB::ReadDbItemsForOrigL(
        RArray<TAccessPointItem>& aItemArray, TDesC& aValue)
    {
    FLOG( _L( "[Provisioning] CWPInternetAPDB::ReadDbItemsForOrigL" ) );
    aItemArray.Reset();// first reset the array

    _LIT(KEqualToString,"='%S'");
    _LIT(KQuery, "SELECT * FROM %S WHERE %S");

    TBuf<100> bufsql;
    bufsql.Append(KQuery);
    bufsql.Append(KEqualToString);
    TBuf<100> bufsql1;
    bufsql1.Format(bufsql, &KtxtItemlist, &NCol4, &aValue);

    ReadItemsL(aItemArray, bufsql1);

    FLOG( _L( "[Provisioning] CWPInternetAPDB::ReadDbItemsForOrigL:done" ) );
    }

// -----------------------------------------------------------------------------
// CWPInternetAPDB::ReadDbItemsL
// -----------------------------------------------------------------------------
//   
void CWPInternetAPDB::ReadDbItemsL(RArray<TAccessPointItem>& aItemArray)
    {
    FLOG( _L( "[Provisioning] CWPInternetAPDB::ReadDbItemsL" ) );
    aItemArray.Reset();// first reset the array

    TFileName QueryBuffer;

    // just get all columns & rows of same originator
    QueryBuffer.Copy(_L("SELECT * FROM "));
    QueryBuffer.Append(KtxtItemlist);

    ReadItemsL(aItemArray, QueryBuffer);

    FLOG( _L( "[Provisioning] CWPInternetAPDB::ReadDbItemsL:done" ) );
    }

// -----------------------------------------------------------------------------
// CWPInternetAPDB::ReadItemsL
// -----------------------------------------------------------------------------
// 
void CWPInternetAPDB::ReadItemsL(RArray<TAccessPointItem>& aItemArray,
        TFileName QueryBuffer)
    {
    FLOG( _L( "[Provisioning] CWPInternetAPDB::ReadItemsL" ) );
    RDbView Myview;
    Myview.Prepare(iItemsDatabase, TDbQuery(QueryBuffer));
    CleanupClosePushL(Myview);
    Myview.EvaluateAll();
    if ( Myview.FirstL())
    {
    	while (Myview.AtRow())
        {
        Myview.GetL();

        TAccessPointItem AccessPointItem;
        AccessPointItem.iIndex = Myview.ColInt(1);
        AccessPointItem.iAPID = Myview.ColInt(2);
        AccessPointItem.iBearerType = Myview.ColInt(3);
        AccessPointItem.iOriginator.Copy(Myview.ColDes(4));

        aItemArray.Append(AccessPointItem);
        Myview.NextL();
        }
    }

    CleanupStack::PopAndDestroy(); // Myview
    FLOG( _L( "[Provisioning] CWPInternetAPDB::ReadItemsL: done" ) );
    }

// -----------------------------------------------------------------------------
// CWPInternetAPDB::SaveToDatabaseL
// -----------------------------------------------------------------------------
//   
void CWPInternetAPDB::SaveToDatabaseL(TInt& aIndex, TUint aAPId,
        TUint32 aBearerType, const TDesC& aOriginator)
    {
    FLOG( _L( "[Provisioning] CWPInternetAPDB::SaveToDatabaseL" ) );

    //If there is any existing rows for the same APID, delete those
    if (DeleteFromDatabaseL(aAPId))
        {
        iItemsDatabase.Begin();
        }

    TFileName QueryBuffer;
    QueryBuffer.Copy(_L("SELECT * FROM "));
    QueryBuffer.Append(KtxtItemlist);

    RDbView Myview;
    Myview.Prepare(iItemsDatabase, TDbQuery(QueryBuffer));
    CleanupClosePushL(Myview);

    Myview.InsertL();

    Myview.SetColL(2, aAPId);
    Myview.SetColL(3, aBearerType);
    Myview.SetColL(4, aOriginator);

    Myview.PutL();

    aIndex = Myview.ColInt(1);// autoincrement gives us unique index.

    CleanupStack::PopAndDestroy(1); // Myview
    iItemsDatabase.Commit();
    FLOG( _L( "[Provisioning] CWPInternetAPDB::SaveToDatabaseL:done" ) );
    }

// -----------------------------------------------------------------------------
// CWPInternetAPDB::DeleteFromDatabaseL
// -----------------------------------------------------------------------------
//
TBool CWPInternetAPDB::DeleteFromDatabaseL(TUint aAPId)
    {
    FLOG( _L( "[Provisioning] CWPInternetAPDB::DeleteFromDatabaseL" ) );
    TBool rowsdeleted = EFalse;
    TFileName QueryBuffer;
    QueryBuffer.Copy(_L("SELECT * FROM "));
    QueryBuffer.Append(KtxtItemlist);
    QueryBuffer.Append(_L(" WHERE "));
    QueryBuffer.Append(NCol2);
    QueryBuffer.Append(_L(" = "));
    QueryBuffer.AppendNum(aAPId);

    iItemsDatabase.Begin();

    RDbView Myview;
    // query buffer with index finds only the selected item row.
    Myview.Prepare(iItemsDatabase, TDbQuery(QueryBuffer));
    CleanupClosePushL(Myview);

    Myview.EvaluateAll();
    if( Myview.FirstL())
    {
    	if (!Myview.IsEmptyL())
        {
        // we have autoincrement in index so it should be unique
        // but just to make sure, we use 'while', instead of 'if'
        while (Myview.AtRow())
            {
            Myview.GetL();
            Myview.DeleteL();
            Myview.NextL();
            }

        iItemsDatabase.Commit();
        // compacts the databse, by physically removig deleted data.
        iItemsDatabase.Compact();
        rowsdeleted = ETrue;
        }
    }
    CleanupStack::PopAndDestroy(1); // Myview

    FLOG( _L( "[Provisioning] CWPInternetAPDB::DeleteFromDatabaseL: done" ) );
    return rowsdeleted;
    }

//End of the file
