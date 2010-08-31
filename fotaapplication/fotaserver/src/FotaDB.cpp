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
 * Description:   Fota package state preservation 
 *
 */

#include <centralrepository.h>
#include "FotaDB.h"
#include "FotaSrvDebug.h"
#include "fotaserverPrivateCRKeys.h"

#define __LEAVE_IF_ERROR(x) if(KErrNone!=x) {FLOG(_L("LEAVE in %s: %d"), __FILE__, __LINE__); User::Leave(x); }

// ====================== MEMBER FUNCTIONS ===================================

// ---------------------------------------------------------------------------
// CFotaDB::CFotaDB()
// ---------------------------------------------------------------------------
CFotaDB::CFotaDB() :
    iIsOpen(EFalse)
    {
    }

// ---------------------------------------------------------------------------
// CFotaDB::~CFotaDB()
// ---------------------------------------------------------------------------
CFotaDB::~CFotaDB()
    {
    //Delete columns set
    if (iColSet)
        {
        delete iColSet;
        iColSet = NULL;
        }
    //Close table
    iTable.Close();

    //Close database
    iStateDB.Close();

    //Close file server session
    if (iFSSession.Handle())
        iFSSession.Close();
    }

// ---------------------------------------------------------------------------
// CFotaDB::NewL()
// ---------------------------------------------------------------------------
CFotaDB* CFotaDB::NewL()
    {
    CFotaDB* self = new (ELeave) CFotaDB();
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop();
    return self;
    }

// ---------------------------------------------------------------------------
// CFotaDB::ConstructL()
// ---------------------------------------------------------------------------
void CFotaDB::ConstructL()
    {
    TInt err;
    TBuf<KMaxPath> tmp;
    User::LeaveIfError(iFSSession.Connect());
    err = iFSSession.CreatePrivatePath(EDriveC);
    if (err != KErrNone && err != KErrAlreadyExists)
        User::Leave(err);
    User::LeaveIfError(iFSSession.SetSessionToPrivate(EDriveC));
    User::LeaveIfError(iFSSession.SessionPath(tmp));
    }

// ---------------------------------------------------------------------------
// CFotaDB::CreateDatabaseL()
// Creates db in private dir of fotaserver
// ---------------------------------------------------------------------------
void CFotaDB::CreateDatabaseL()
    {
    FLOG(_L("CFotaDB::CreateDatabaseL()  >>"));
    TInt err;
    User::LeaveIfError(iStateDB.Create(iFSSession, KDatabaseName));
    User::LeaveIfError(iStateDB.Begin());
    User::LeaveIfError(iStateDB.Execute(KCreateTable));
    err = iStateDB.Commit();
    if (err)
        {
        FLOG(_L("      cdb err %d"), err);
        iStateDB.Rollback();
        User::Leave(err);
        }
    User::LeaveIfError(iStateDB.Compact());
    FLOG(_L("CFotaDB::CreateDatabaseL()  <<"));
    }

// ---------------------------------------------------------------------------
// CFotaDB::AddPackageStateL
// Adds state to db
// ---------------------------------------------------------------------------
void CFotaDB::AddPackageStateL(const TPackageState& aState,
        const TDesC8& aPkgURL)
    {
    FLOG(_L("  CFotaDB::AddPackageStateL   >>"));
    TInt err;
    RDbView view;
    CleanupClosePushL(view);
    TPackageState pkgstate(aState);
    pkgstate.iResult = -1; // result should be -1 if no Execs have been done
    err = view.Prepare(iStateDB, TDbQuery(KSelectAll), RDbView::EInsertOnly);
    __LEAVE_IF_ERROR(err);
    FLOG(_L("  CFotaDB::AddPackageStateL   inserting. pkgid:%d result:%d  state:%d"),
            pkgstate.iPkgId, pkgstate.iResult, pkgstate.iState);
    view.InsertL();
    StateToRowL(pkgstate, aPkgURL, view);
    view.PutL();
    CleanupStack::PopAndDestroy(); //view
    FLOG(_L("  CFotaDB::AddPackageStateL   <<"));
    }

// ---------------------------------------------------------------------------
// CFotaDB::GetAllL
// Get all states
// ---------------------------------------------------------------------------
void CFotaDB::GetAllL(RArray<TInt>& aStates)
    {
    RDbView view;
    CleanupClosePushL(view);

    TInt err = view.Prepare(iStateDB, TDbQuery(KSelectAll));
    __LEAVE_IF_ERROR(err);
    view.EvaluateAll();

    view.FirstL();
    FLOG(_L("[fota DB] --- rows ------------------------------------------------------------------- v"));
    while (view.AtRow())
        {
        view.GetL();
        HBufC8* url;
        TPackageState s = RowToStateL(url, view);
        aStates.Append(s.iPkgId);
        CleanupStack::PushL(url);
        FLOG(_L("[fotaDB] pkgid: %d profid:%d state:%d  result:%d \
                url: %d chars sessiontype:%d IapId:%d Pkgsize:%d UpdateLtr:%d"),
                s.iPkgId, s.iProfileId, s.iState, s.iResult,
                url->Des().Length(), s.iSessionType, s.iIapId, s.iPkgSize,
                s.iUpdateLtr);
        CleanupStack::PopAndDestroy(); // url
        view.NextL();
        }
    FLOG(_L("[fota DB] --- rows ------------------------------------------------------------------- ^"));
    view.Close();
    CleanupStack::PopAndDestroy(); //view
    }

// ---------------------------------------------------------------------------
// CFotaDB::OpenDBL()
// Opens database
// ---------------------------------------------------------------------------
void CFotaDB::OpenDBL()
    {
    FLOG(_L("CFotaDB::OpenDBL()"));
    if (!iIsOpen) //Prevents accidental opening of database twice
        {
        TInt err;
        err = iStateDB.Open(iFSSession, KDatabaseName);

        if (err == KErrNotFound)
            {
            CreateDatabaseL();
            }
        else if (err != KErrNone)
            {
            FLOG(_L("[fota DB openDB]\t db open error: %d"), err);
            FLOG(_L("deleting fota DB and creating it again..."));
            err = iFSSession.Delete(KDatabaseName);
            CreateDatabaseL();
            User::LeaveIfError(err);
            }
        User::LeaveIfError(iTable.Open(iStateDB, KTblState));
        iColSet = iTable.ColSetL();

        iIsOpen = ETrue;
        }
    }

// ---------------------------------------------------------------------------
// CFotaDB::CloseAndCommitDB
// Closes and commits DB
// ---------------------------------------------------------------------------
void CFotaDB::CloseAndCommitDB()
    {
    if (iColSet)
        {
        delete iColSet;
        iColSet = NULL;
        }
    iTable.Close();
    iStateDB.Close();
    iIsOpen = EFalse;
    }

// ---------------------------------------------------------------------------
// CFotaDB::IsOpen()
// Chekcs if db is open
// ---------------------------------------------------------------------------
TBool CFotaDB::IsOpen()
    {
    return iIsOpen;
    }

// ---------------------------------------------------------------------------
// CFotaDB::GetStateL
// Gets pkg state from db
// ---------------------------------------------------------------------------
TPackageState CFotaDB::GetStateL(const TInt aPkgId, TDes8& aPkgURL)
    {
    RDbView view;
    TPackageState s;
    CleanupClosePushL(view);

    s.iState = RFotaEngineSession::EIdle; // default state is idle

    HBufC* select = HBufC::NewLC(KSelect_where_packageid().Length() + 10);
    select->Des().Format(KSelect_where_packageid, aPkgId);

    TInt err = view.Prepare(iStateDB, TDbQuery(*select));
    __LEAVE_IF_ERROR(err);
    view.EvaluateAll();
    view.FirstL();

    while (view.AtRow())
        {
        view.GetL();
        HBufC8* url;
        s = RowToStateL(url, view);
        CleanupStack::PushL(url);
        aPkgURL.Copy(url->Des());
        CleanupStack::PopAndDestroy(); // url
        view.NextL();
        }
    CleanupStack::PopAndDestroy(); //select
    CleanupStack::PopAndDestroy(); //view
    return s;
    }

// ---------------------------------------------------------------------------
// CFotaDB::SetStateL
// Writes package state to DB. 
// ---------------------------------------------------------------------------
void CFotaDB::SetStateL(TPackageState& aState, const TDesC8& aPkgURL,
        TUint aChangedFields)
    {
    FLOG(_L("  CFotaDB::SetStateL  >>  id %d result %d  state %d sessiontype %d iapid %d pkgsize %d updateltr %d"), aState.iPkgId, aState.iResult, aState.iState);
    __ASSERT_ALWAYS( aChangedFields!=0, User::Leave(KErrArgument) );
    TBuf8<KMaxFileName> temp;
    TPackageState found = GetStateL(aState.iPkgId, temp);
    if (found.iPkgId == KErrNotFound)
        {
        AddPackageStateL(aState, aPkgURL);
        }
    else
        {
        // sml try count must be reset, if state is set
        if (aChangedFields & EFDBState)
            {
            aChangedFields = aChangedFields | EFDBSmlTryCount;
            SetRetryCount(aState);
            }

        // Construct a SQL string for update. 
        // Example: UPDATE State SET Result=40,State=4 WHERE pkgID=5
        // 
        TInt sqlsize = 0;
        _LIT8( KSqlbegin, "UPDATE State SET " );
        TBuf<21> sqlEnd;
        HBufC8* sql(NULL);
        // determine characters needed 
        sqlsize = DetermineCharNeeded(aChangedFields, aState, aPkgURL);
        sqlEnd.AppendFormat(_L(" WHERE pkgID=%d"), aState.iPkgId);

        sql = HBufC8::NewLC(((TDesC8) KSqlbegin).Length() + sqlsize
                + sqlEnd.Length());

        sql->Des().Append(KSqlbegin);

        if (aChangedFields & EFDBResult)
            {
            // check FUMO compability
            __ASSERT_ALWAYS( aState.iResult>=KErrNotFound
                    && aState.iResult<=602, User::Leave(KErrArgument) );
            sql->Des().AppendFormat(_L8("Result=%d,"), aState.iResult);
            }
        if (aChangedFields & EFDBState)
            {
            // check FUMO compability
            __ASSERT_ALWAYS( aState.iState>=0 && aState.iState<=100
                    , User::Leave(KErrArgument) );
            sql->Des().AppendFormat(_L8("State=%d,"), aState.iState);
            }
        if (aChangedFields & EFDBProfileId)
            {
            sql->Des().AppendFormat(_L8("profileid=%d,"), aState.iProfileId);
            }
        if (aChangedFields & EFDBPkgUrl)
            {
            sql->Des().AppendFormat(_L8("pkgurl='%S',"), &aPkgURL);
            }
        if (aChangedFields & EFDBPkgName)
            {
            sql->Des().AppendFormat(_L8("pkgname='%S',"), &(aState.iPkgName));
            }
        if (aChangedFields & EFDBVersion)
            {
            sql->Des().AppendFormat(_L8("Version='%S',"),
                    &(aState.iPkgVersion));
            }
        if (aChangedFields & EFDBSmlTryCount)
            {
            __ASSERT_ALWAYS( aState.iSmlTryCount>=0
                    , User::Leave(KErrArgument) );
            sql->Des().AppendFormat(_L8("SmlTryCount=%d,"),
                    aState.iSmlTryCount);

            }
        if (aChangedFields & EFDBSessionType)
            {
            sql->Des().AppendFormat(_L8("SessionType=%d,"),
                    aState.iSessionType);
            }
        if (aChangedFields & EFDBIapId)
            {
            // validate IAP ID
            __ASSERT_ALWAYS( aState.iIapId>=-1 ,User::Leave(KErrArgument) );
            sql->Des().AppendFormat(_L8("IapId=%d,"), aState.iIapId);
            }
        if (aChangedFields & EFDBPkgSize)
            {
            sql->Des().AppendFormat(_L8("PkgSize=%d,"), aState.iPkgSize);
            }
        if (aChangedFields & EFDBUpdateLtr)
            {
            // validate bit
            sql->Des().AppendFormat(_L8("UpdateLtr=%d,"), aState.iUpdateLtr);
            }
        // remove trailing ,
        if (aChangedFields)
            {
            sql->Des().SetLength(sql->Des().Length() - 1);
            }
        sql->Des().Append(sqlEnd);
        HBufC* sql2 = HBufC::NewLC(sql->Length()); // to cleanupstack
        sql2->Des().Copy(sql->Des());
        FLOG(_L("  sql:%S"), sql2);

        User::LeaveIfError(iStateDB.Begin());
        User::LeaveIfError(iStateDB.Execute(*sql2));
        User::LeaveIfError(iStateDB.Commit());
        User::LeaveIfError(iStateDB.Compact());

        CleanupStack::PopAndDestroy(sql2); //sql2
        CleanupStack::PopAndDestroy(sql); //sql
        }
    FLOG(_L("  CFotaDB::SetStateL  <<"));
    }

// ---------------------------------------------------------------------------
// CFotaDB::DetermineCharNeeded 
// Returns the char needed fro the query 
// ---------------------------------------------------------------------------

TInt CFotaDB::DetermineCharNeeded(TInt aChangedFields, TPackageState& aState,
        const TDesC8& aPkgURL)

    {

    TInt sqlsize = 0;
    if (aChangedFields & EFDBResult)
        sqlsize += 4 + 7 + 4;
    if (aChangedFields & EFDBState)
        sqlsize += 4 + 5 + 4;
    if (aChangedFields & EFDBProfileId)
        sqlsize += 4 + 9 + 4;
    if (aChangedFields & EFDBPkgUrl)
        sqlsize += aPkgURL.Length() + 6 + 4;
    if (aChangedFields & EFDBPkgName)
        sqlsize += aState.iPkgName.Length() + 7 + 4;
    if (aChangedFields & EFDBVersion)
        sqlsize += aState.iPkgVersion.Length() + 7 + 4;
    if (aChangedFields & EFDBSmlTryCount)
        sqlsize += 4 + 11 + 4;

    if (aChangedFields & EFDBSessionType)
        sqlsize += 4 + 11 + 4;
    if (aChangedFields & EFDBIapId)
        sqlsize += 4 + 5 + 4;
    if (aChangedFields & EFDBPkgSize)
        sqlsize += 4 + 7 + 10;
    if (aChangedFields & EFDBUpdateLtr)
        sqlsize += 4 + 11 + 4;

    return sqlsize;

    }

// ---------------------------------------------------------------------------
// CFotaDB::SetRetryCount
// Sets the retry count
// ---------------------------------------------------------------------------
void CFotaDB::SetRetryCount(TPackageState& aState)
    {
    CRepository* centrep(NULL);
    TInt err = KErrNone;
    TInt retry = 0;

    TRAP(err, centrep = CRepository::NewL( KCRUidFotaServer ) );
    if (centrep)
        {
        err = centrep->Get(KGenericAlertRetries, retry);
        delete centrep;
        centrep = NULL;
        }
    if (err == KErrNone)
        {
        if (retry < 0)
            {
            aState.iSmlTryCount = KDefaultSmlTryCount;
            }
        else if (retry == 0)
            {
            aState.iSmlTryCount = 2;
            }
        else if (retry > KMaximumSmlTryCount)
            {
            aState.iSmlTryCount = KMaximumSmlTryCount + 1;
            }
        else
            {
            aState.iSmlTryCount = retry + 1;
            }
        }
    else
        {
        aState.iSmlTryCount = KDefaultSmlTryCount;
        }
    }

// ---------------------------------------------------------------------------
// CFotaDB::RowToStateL
// Extracts db row contents to package state object and aPkgUrl
// Returns url in aPkgURL parameter
// ---------------------------------------------------------------------------
TPackageState CFotaDB::RowToStateL(HBufC8*& aPkgUrl, const RDbView& aView)
    {
    TPackageState s;
    TInt pkgid = aView.ColInt(iColSet->ColNo(KColPkgId));
    TInt state = aView.ColInt(iColSet->ColNo(KColState));
    TInt result = aView.ColInt(iColSet->ColNo(KColResult));
    TSmlProfileId profileid(aView.ColInt(iColSet->ColNo(KColProfileId)));
    TPtrC pkgname = aView.ColDes(iColSet->ColNo(KColPkgName));
    TPtrC version = aView.ColDes(iColSet->ColNo(KColVersion));
    TInt smltrycount = aView.ColInt(iColSet->ColNo(KColSmlTryCount));
    TInt sessiontype = aView.ColInt(iColSet->ColNo(KColSessionType));
    TInt iapid = aView.ColInt(iColSet->ColNo(KColIapId));
    TUint pkgsize = aView.ColUint(iColSet->ColNo(KColPkgSize));
    TBool updateltr = aView.ColUint8(iColSet->ColNo(KColUpdateLtr));

    s.iPkgId = pkgid;
    s.iPkgName.Copy(pkgname);
    s.iPkgVersion.Copy(version);
    s.iProfileId = profileid;
    s.iResult = result;
    s.iState = RFotaEngineSession::TState(state);
    s.iSmlTryCount = smltrycount;
    s.iSessionType = sessiontype;
    s.iIapId = iapid;
    s.iPkgSize = pkgsize;
    s.iUpdateLtr = updateltr;

    RDbColReadStream rstream;
    TInt len = aView.ColLength(iColSet->ColNo(KColPkgUrl));
    rstream.OpenLC(aView, iColSet->ColNo(KColPkgUrl));
    HBufC* pkgurl = HBufC::NewLC(len);
    TPtr ptr = pkgurl->Des();
    rstream.ReadL(ptr, len);

    HBufC8* tmp = HBufC8::NewL(pkgurl->Des().Length());
    tmp->Des().Copy(pkgurl->Des());
    aPkgUrl = tmp;

    CleanupStack::PopAndDestroy(pkgurl);
    CleanupStack::PopAndDestroy(&rstream);
    return s;
    }

// ---------------------------------------------------------------------------
// CFotaDB::StateToRowL
// Converts state object to database row (into view object)
// ---------------------------------------------------------------------------
void CFotaDB::StateToRowL(const TPackageState& aPkg, const TDesC8& aPkgURL,
        RDbView& aView)
    {
    HBufC* pkgname = HBufC::NewLC(aPkg.iPkgName.Length());
    HBufC* version = HBufC::NewLC(aPkg.iPkgVersion.Length());

    pkgname->Des().Copy(aPkg.iPkgName);
    version->Des().Copy(aPkg.iPkgVersion);

    aView.SetColL(iColSet->ColNo(KColPkgId), aPkg.iPkgId);
    aView.SetColL(iColSet->ColNo(KColResult), aPkg.iResult);
    aView.SetColL(iColSet->ColNo(KColState), aPkg.iState);
    aView.SetColL(iColSet->ColNo(KColProfileId), aPkg.iProfileId);
    aView.SetColL(iColSet->ColNo(KColPkgName), *pkgname);
    aView.SetColL(iColSet->ColNo(KColVersion), *version);
    aView.SetColL(iColSet->ColNo(KColSmlTryCount), aPkg.iSmlTryCount);
    aView.SetColL(iColSet->ColNo(KColSessionType), aPkg.iSessionType);
    aView.SetColL(iColSet->ColNo(KColIapId), aPkg.iIapId);
    aView.SetColL(iColSet->ColNo(KColPkgSize), aPkg.iPkgSize);
    aView.SetColL(iColSet->ColNo(KColUpdateLtr), aPkg.iUpdateLtr);

    RDbColWriteStream wstream;
    CleanupClosePushL(wstream);
    wstream.OpenL(aView, iColSet->ColNo(KColPkgUrl));
    // Cannot write 8 bit descriptors to databae
    HBufC* buf = HBufC::NewLC(aPkgURL.Length());
    buf->Des().Copy(aPkgURL);
    wstream.WriteL(buf->Des());

    FLOG(_L("CFotaDB::StateToRowL  id:%d result:%d state:%d profileid:%d \
    		name:%d chars version: %d chars url: %d chars sessiontype:%d iapid:%d pkgsize:%d updateltr = %d"),
            aPkg.iPkgId, aPkg.iResult, aPkg.iState, aPkg.iProfileId,
            pkgname->Des().Length(), version->Des().Length(),
            buf->Des().Length(), aPkg.iSessionType, aPkg.iIapId,
            aPkg.iPkgSize, aPkg.iUpdateLtr);

    CleanupStack::PopAndDestroy(buf);
    CleanupStack::PopAndDestroy(&wstream);
    CleanupStack::PopAndDestroy(version);
    CleanupStack::PopAndDestroy(pkgname);
    }
