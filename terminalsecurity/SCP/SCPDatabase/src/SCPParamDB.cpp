/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:
*
*/
#include "SCPDBDebug.h"
#include "SCPParamDB.h"
#include "SCPDBCommon.h"
#include "SCPConfiguration.h"

#define __LEAVE_IF_ERROR(x) if(KErrNone!=x) {_SCPDB_LOG(_L("LEAVE (%d) at %s : %d"), x, __FILE__, __LINE__); User::Leave(x); }

CSCPParamDB :: ~CSCPParamDB() {
    _SCPDB_LOG(_L("[CSCPParamDB]-> >>> ~CSCPDeviceLockdb"));

    if(iColSet) {
        delete iColSet;
        iColSet = NULL;
    }

    iTable.Close();
    iParameterDB.Close();
    iRdbSession.Close();
    _SCPDB_LOG(_L("[CSCPParamDB]-> <<< ~CSCPDeviceLockdb"));
}

CSCPParamDB* CSCPParamDB :: NewL() {
    _SCPDB_LOG(_L("[CSCPParamDB]-> >>> NewL()"));
    CSCPParamDB* self = new(ELeave) CSCPParamDB();
    CleanupStack :: PushL(self);
    self->ConstructL();
    CleanupStack :: Pop();
    _SCPDB_LOG(_L("[CSCPParamDB]-> <<< NewL()"));
    return self;
}

CSCPParamDB* CSCPParamDB :: NewLC() {
    _SCPDB_LOG(_L("[CSCPParamDB]-> >>> NewLC()"));
    CSCPParamDB* self = NewL();
    CleanupStack :: PushL(self);
    _SCPDB_LOG(_L("[CSCPParamDB]-> <<< NewLC()"));
    return self;
}

void CSCPParamDB :: ConstructL() {
    __LEAVE_IF_ERROR(iRdbSession.Connect());

    _SCPDB_LOG(_L("[CSCPParamDB]-> >>> ConstructL"));

    TParse lDBName;
    lDBName.Set(KDatabaseName(), NULL, NULL);
    
    _SCPDB_LOG(_L("[CSCPParamDB]-> Opening the database..."));
    TInt lErr = iParameterDB.Open ( iRdbSession, lDBName.FullName(), KDBMSOwnerProcess);
    _SCPDB_LOG(_L("[CSCPParamDB]-> lErr=%d"), lErr);
    
    switch(lErr) {
        case KErrNone:
            break;    
        default: {
            _SCPDB_LOG(_L("[CSCPParamDB]-> Deleting the database (if any)..."));
            TInt x = iRdbSession.DeleteDatabase(lDBName.FullName(), KOwnerProcess);
            
            _SCPDB_LOG(_L("[CSCPParamDB]-> Creating the database afresh..."));
            __LEAVE_IF_ERROR( iParameterDB.Create( iRdbSession, lDBName.FullName(), KDBMSOwnerProcess ) );
            __LEAVE_IF_ERROR( iParameterDB.Begin() );
            _SCPDB_LOG(_L("[CSCPParamDB]-> Creating the table PARAMETER..."));
            __LEAVE_IF_ERROR( iParameterDB.Execute(KCreateTable) );
            _SCPDB_LOG(_L("[CSCPParamDB]-> calling commit..."));
            __LEAVE_IF_ERROR( iParameterDB.Commit() );
            _SCPDB_LOG(_L("[CSCPParamDB]-> calling compact..."));
            __LEAVE_IF_ERROR( iParameterDB.Compact() );
        }
        break;
    }

    _SCPDB_LOG(_L("[CSCPParamDB]-> Opening the table PARAMETER..."));
    lErr = iTable.Open (iParameterDB, KTblParameter);
    _SCPDB_LOG(_L("[CSCPParamDB]-> lErr=%d"), lErr);
    
    if(lErr != KErrNone) {
        User :: Leave(lErr);
    }
    
    iColSet = iTable.ColSetL();
    _SCPDB_LOG(_L("[CSCPParamDB]-> ConstructL <<"));
}

TInt CSCPParamDB :: SetValueForParameterL(TInt aParamID, const TInt32 aValue, const TInt32 aApp) {
    _SCPDB_LOG(_L("[CSCPParamDB]-> SetValueForParameterL >>>"));

    HBufC* sql = HBufC :: NewLC(256);
    sql->Des().Format(KUpdateValIntWhereParamId, aApp, aValue, aParamID);

    _SCPDB_LOG(_L("[CSCPParamDB]-> SQL Query: %S"), &sql->Des());

    __LEAVE_IF_ERROR(iParameterDB.Begin());

    TInt rows = (iParameterDB.Execute(*sql));

    __LEAVE_IF_ERROR(iParameterDB.Commit());
    __LEAVE_IF_ERROR(iParameterDB.Compact());

    if(rows == 0) {
        sql->Des().Format(KInsertValInt, aParamID, aValue, aApp);

        __LEAVE_IF_ERROR(iParameterDB.Begin());

        TInt rows = (iParameterDB.Execute(*sql));

        __LEAVE_IF_ERROR(iParameterDB.Commit());
        __LEAVE_IF_ERROR(iParameterDB.Compact());

        if(rows == 0) {
            _SCPDB_LOG(_L("[CSCPParamDB]-> ERROR: Unable to insert value for the parameter '%d'..."), aParamID);
            User :: Leave(KErrCorrupt);
        }
    }
    else if(rows > 1) {
        _SCPDB_LOG(_L("[CSCPParamDB]-> ERROR: More than one record meets the criterion. Db is corrupt! Leaving..."));
        User :: Leave(KErrCorrupt);
    }

    CleanupStack :: PopAndDestroy();
    _SCPDB_LOG(_L("[CSCPParamDB]-> SetValueForParameterL <<<"));
    return KErrNone;
}

TInt CSCPParamDB :: SetValuesForParameterL(TInt aParamID, const RPointerArray <HBufC>& aParamValues, const TInt32 aApp) {
    _SCPDB_LOG(_L("[CSCPParamDB]-> SetValuesForParameterL >>>"));
    
    TInt lCount = aParamValues.Count();
    
    for(TInt i=0; i < lCount; i++) {
        HBufC * lDataPtr = aParamValues[i];
        HBufC* sql = HBufC :: NewLC(KInsertValDes().Length() + lDataPtr->Des().Length() + 40);        
        sql->Des().Format(KInsertValDes, aParamID, &lDataPtr->Des(), aApp);

        __LEAVE_IF_ERROR(iParameterDB.Begin());

        TInt rows = (iParameterDB.Execute(*sql));

        __LEAVE_IF_ERROR(iParameterDB.Commit());
        __LEAVE_IF_ERROR(iParameterDB.Compact());

        if(rows == 0) {
            _SCPDB_LOG(_L("[CSCPParamDB]-> ERROR: Unable to insert value for the parameter '%d'..."), aParamID);
            User :: Leave(KErrCorrupt);
        }
        
        CleanupStack :: PopAndDestroy();
    }
    
    _SCPDB_LOG(_L("[CSCPParamDB]-> SetValuesForParameterL <<<"));
    return KErrNone;
}

TInt CSCPParamDB :: GetValueForParameterL(TInt aParamID, TInt32& aValue, TInt32& aApp) {
    _SCPDB_LOG(_L("[CSCPParamDB]-> GetValueForParameterL >>>"));

    RDbView lDBView;
    CleanupClosePushL(lDBView);

    HBufC* lSelectQry  = HBufC :: NewLC(KSelectWhereParamId().Length() + 15);
    lSelectQry->Des().Format(KSelectWhereParamId, aParamID);

    __LEAVE_IF_ERROR(lDBView.Prepare(iParameterDB, TDbQuery(*lSelectQry)));
    __LEAVE_IF_ERROR(lDBView.EvaluateAll());
    if( lDBView.FirstL())
    {
    	TInt lRowCount = lDBView.CountL();

    	if(lRowCount == 0) {
        _SCPDB_LOG(_L("[CSCPParamDB]-> No Rows found for this parameter"));
        CleanupStack :: PopAndDestroy(2);
        return KErrNotFound;
    	}
    	else if(lRowCount > 1) {
        _SCPDB_LOG(_L("[CSCPParamDB]-> ERROR: More than one record matches the criterion. Db is corrupt! Leaving..."));
        User :: Leave(KErrCorrupt);
    	}

    	lDBView.GetL();
    	aApp = lDBView.ColInt(iColSet->ColNo(KColAppId));
    	aValue = lDBView.ColInt(iColSet->ColNo(KColValueInt));
    }
    CleanupStack :: PopAndDestroy(2);

    _SCPDB_LOG(_L("[CSCPParamDB]-> GetValueForParameterL <<<"));
    return KErrNone;
}

TInt CSCPParamDB :: GetValuesForParameterL(TInt aParamID, RPointerArray <HBufC>& aParamValues, const TInt32 aApp) {
    _SCPDB_LOG(_L("[CSCPParamDB]-> GetValueForParameterL >>>"));

    RDbView lDBView;
    CleanupClosePushL(lDBView);

    HBufC* lSelectQry  = HBufC :: NewLC(KSelectWhereParamIdAppID().Length() + 40);
    lSelectQry->Des().Format (KSelectWhereParamIdAppID, aParamID, aApp);

    __LEAVE_IF_ERROR(lDBView.Prepare(iParameterDB, TDbQuery(*lSelectQry)));
    __LEAVE_IF_ERROR(lDBView.EvaluateAll());
    
    if (lDBView.FirstL())
    {
    	TInt size(0);
    	TInt lRowCount = lDBView.CountL();

    	if(lRowCount == 0) {
        _SCPDB_LOG(_L("[CSCPParamDB]-> No Rows found for this parameter"));
        CleanupStack :: PopAndDestroy(2);
        return KErrNotFound;
    	}
    
    	TInt lErr(KErrNone);
    
    	do {
        lDBView.GetL();
        size = lDBView.ColDes(iColSet->ColNo(KColValueDes)).Size();

        if(size) {
            HBufC* lBuff = HBufC :: NewL(size);
            TPtr lPtr(lBuff->Des());
            lPtr.Copy(lDBView.ColDes(iColSet->ColNo(KColValueDes)));
          
            TRAP(lErr, aParamValues.AppendL(lBuff));
            
            if(lErr != KErrNone) {
                aParamValues.ResetAndDestroy();
                User :: Leave(lErr);
            }
        }
    	}
    	while(lDBView.NextL());
    }

    CleanupStack :: PopAndDestroy(2);
    _SCPDB_LOG(_L("[CSCPParamDB]-> GetValuesForParameterL <<<"));
    return KErrNone;   
}

TInt CSCPParamDB :: GetApplicationIDListL(RArray <TUid>& aIDArray) {
    _SCPDB_LOG(_L("[CSCPParamDB]-> GetApplicationIDList() >>>"));
    
    RDbView lDBView;
    CleanupClosePushL(lDBView);
    
    HBufC* lSelectQry  = HBufC :: NewLC(KSelectAll().Length());
    lSelectQry->Des().Append(KSelectAll);
    
    __LEAVE_IF_ERROR(lDBView.Prepare(iParameterDB, TDbQuery(*lSelectQry)));
    __LEAVE_IF_ERROR(lDBView.EvaluateAll());
    
    TBool lStatus(EFalse);
    TInt lRet(KErrNone);
    
    TRAP(lRet, lStatus = lDBView.FirstL());
    
    if(lRet == KErrNone && lStatus) {
        do {
            lDBView.GetL();
            TUid lAppUID = TUid :: Uid(lDBView.ColInt(iColSet->ColNo(KColAppId)));
            
            if(aIDArray.Find(lAppUID) == KErrNotFound) {
                aIDArray.AppendL(lAppUID);
            }
        }
        while(lDBView.NextL());
    }
    
    _SCPDB_LOG(_L("[CSCPParamDB]-> GetApplicationIDList() >>>"));
    CleanupStack :: PopAndDestroy(2);
    return KErrNone;
}

TBool CSCPParamDB :: IsParamValueSharedL(HBufC* aParamValue, const TInt32 aApp) {
    _SCPDB_LOG(_L("[CSCPParamDB]-> IsParamValueShared >>>"));
    
    if(aParamValue == NULL) {
        return KErrArgument;
    }
    
    RDbView lDBView;
    CleanupClosePushL(lDBView);
    
    HBufC* lSelectQry  = HBufC :: NewLC(KSelectAppIDParamValNotIn().Length() + aParamValue->Des().Length() + 50);
    lSelectQry->Des().Format(KSelectAppIDParamValNotIn, aApp, &aParamValue->Des());
    
    __LEAVE_IF_ERROR(lDBView.Prepare(iParameterDB, TDbQuery(*lSelectQry)));
    __LEAVE_IF_ERROR(lDBView.EvaluateAll());
    
    TInt lRowCnt(0);
    TInt lStatus(KErrNone);
    
    TRAP(lStatus, lRowCnt = lDBView.CountL());
    
    if(lStatus != KErrNone) {
        _SCPDB_LOG(_L("[CSCPParamDB]-> ERROR: Unable to determine row count..."));    
    }
    else {
        if(lRowCnt > 0) {
            CleanupStack :: PopAndDestroy(2);
            return ETrue;
        }
    }
    
    _SCPDB_LOG(_L("[CSCPParamDB]-> IsParamValueShared <<<"));
    CleanupStack :: PopAndDestroy(2);
    return EFalse;
}

TInt CSCPParamDB :: DropValuesL(TInt aParamID, RPointerArray <HBufC>& aParamValues, const TInt32 aApp) {
    _SCPDB_LOG(_L("[CSCPParamDB]-> DropValuesL() >>>"));
    
    TInt lCount = aParamValues.Count();
    
    if(lCount < 1) {
        _SCPDB_LOG(_L("[CSCPParamDB]-> WARNING: Nothing to do!!"));
        return KErrNone;  
    }
    
    HBufC* lDelQuery(NULL);
    
    if(iParameterDB.InTransaction()) {
        _SCPDB_LOG(_L("[CSCPParamDB]-> ERROR: The DB in use! leaving..."));
        User :: Leave(KErrInUse);
    }
        
    __LEAVE_IF_ERROR(iParameterDB.Begin());
    
    for(TInt i=0; i < lCount; i++) {
        if(aApp == -1) {
            lDelQuery = HBufC :: NewLC(KDeleteWhereParamIDValueDes().Length() + (aParamValues[i])->Length());
            lDelQuery->Des().Format(KDeleteWhereParamIDValueDes, aParamID, &(aParamValues[i])->Des());
        }
        else {
            lDelQuery = HBufC :: NewLC(KDeleteWhereParamIDValueDesAppID().Length() + (aParamValues[i])->Des().Length() + 100);
            lDelQuery->Des().Format(KDeleteWhereParamIDValueDesAppID, aParamID, &(aParamValues[i])->Des(), aApp);
        }

        TInt lAffRows = iParameterDB.Execute(*lDelQuery);
        _SCPDB_LOG(_L("[CSCPParamDB]-> INFO: Total rows affected=%d"), lAffRows);
        CleanupStack :: PopAndDestroy();
    }
    
    TInt lErr = iParameterDB.Commit();
    
    if(lErr != KErrNone) {
        _SCPDB_LOG(_L("[CSCPParamDB]-> ERROR: Commit failed!!"));
        iParameterDB.Rollback();
        User :: Leave(lErr);
    }
    
    _SCPDB_LOG(_L("[CSCPParamDB]-> DropValuesL() <<<"));
    return KErrNone;
}

TInt CSCPParamDB :: ListParamsUsedByAppL(RArray <TInt>& aParamIds, const TInt32 aApp) {
    _SCPDB_LOG(_L("[CSCPParamDB]-> ListParamsUsedByApp() >>>"));
    
    RDbView lDBView;
    CleanupClosePushL(lDBView);
    
    HBufC* lSelectQry(NULL);
    
    if(aApp == -1) {
        lSelectQry = HBufC :: NewLC(KSelectAllAscParamID().Length());
        lSelectQry->Des().Append(KSelectAllAscParamID);
    }
    else {
        lSelectQry = HBufC :: NewLC(KSelectWhereAppIdByAscParamID().Length() + 15);
        lSelectQry->Des().Format(KSelectWhereAppIdByAscParamID, aApp);
    }
    
    _SCPDB_LOG(_L("[CSCPParamDB]-> SQL Query: %S"), &lSelectQry->Des());
    _SCPDB_LOG(_L("[CSCPParamDB]-> Executing the query..."));
    __LEAVE_IF_ERROR(lDBView.Prepare(iParameterDB, TDbQuery(lSelectQry->Des())));
    __LEAVE_IF_ERROR(lDBView.EvaluateAll());
    
    TInt lErr(KErrNone);
    TInt lStatus(KErrNone);    
    
    TRAP(lErr, lStatus = lDBView.FirstL());
    _SCPDB_LOG(_L("[CSCPParamDB]-> Fetching values for the column KColParamId"));
    
    if(lErr == KErrNone && lStatus) {
        do {
            lDBView.GetL();
            TInt lPID = lDBView.ColInt(iColSet->ColNo(KColParamId));
            
            if(aParamIds.Find(lPID) == KErrNotFound) {
                aParamIds.AppendL(lPID);
            }
        }
        while(lDBView.NextL());
    }
    
    _SCPDB_LOG(_L("[CSCPParamDB]-> Query execution complete..."));
    CleanupStack :: PopAndDestroy(2);
    _SCPDB_LOG(_L("[CSCPParamDB]-> ListParamsUsedByApp() <<<"));
    return KErrNone;
}

TInt CSCPParamDB :: DropValuesL(TInt aParamID, const TInt32 aApp) {
    _SCPDB_LOG(_L("[CSCPParamDB]-> DropValuesL() >>>"));
    
    HBufC* lDelQuery(NULL);
    
    if(iParameterDB.InTransaction()) {
        _SCPDB_LOG(_L("[CSCPParamDB]-> ERROR: The DB in use! leaving..."));
        User :: Leave(KErrInUse);
    }
        
    __LEAVE_IF_ERROR(iParameterDB.Begin());
    
    if(aApp == -1) {
        lDelQuery = HBufC :: NewLC(KDeleteWhereParamID().Length() + 50);
        lDelQuery->Des().Format(KDeleteWhereParamID, aParamID);
    }
    else {
        lDelQuery = HBufC :: NewLC(KDeleteWhereParamIDAppID().Length() + 100);
        lDelQuery->Des().Format(KDeleteWhereParamIDAppID, aParamID, aApp);
    }
    _SCPDB_LOG(_L("[CSCPParamDB]-> SQL Query: %S"), &lDelQuery->Des());

    TInt lAffRows = iParameterDB.Execute(*lDelQuery);
    _SCPDB_LOG(_L("[CSCPParamDB]-> INFO: Total rows affected=%d"), lAffRows);
    CleanupStack :: PopAndDestroy();
    
    TInt lErr = iParameterDB.Commit();
    
    if(lErr != KErrNone) {
        _SCPDB_LOG(_L("[CSCPParamDB]-> ERROR: Commit failed!!"));
        iParameterDB.Rollback();
        User :: Leave(lErr);
    }
    
    _SCPDB_LOG(_L("[CSCPParamDB]-> DropValuesL() <<<"));
    return KErrNone;
}

TInt CSCPParamDB :: ListEntriesL(RArray <TInt32>& aNumCols, RPointerArray <HBufC>& aDesCols, const TInt32 aApp) {
    _SCPDB_LOG(_L("[CSCPParamDB]-> ListEntries() >>>"));
    
    RDbView lDBView;
    CleanupClosePushL(lDBView);
    
    HBufC* lSelectQry(NULL);
    
    if(aApp == -1) {
        lSelectQry = HBufC :: NewLC(KSelectAll().Length() + 50);
        lSelectQry->Des().Format(KSelectAll);
    }
    else {
        lSelectQry = HBufC :: NewLC(KSelectWhereAppId().Length() + 30);
        lSelectQry->Des().Format(KSelectWhereAppId, aApp);
    }
    
    __LEAVE_IF_ERROR(lDBView.Prepare(iParameterDB, TDbQuery(*lSelectQry)));
    __LEAVE_IF_ERROR(lDBView.EvaluateAll());

    TInt lRet(KErrNone);
    TBool lStatus(EFalse);
    
    TRAP(lRet, lStatus = lDBView.FirstL());
    
    if(lRet == KErrNone && lStatus) {
        do {
            lDBView.GetL();

            aNumCols.AppendL(lDBView.ColInt(iColSet->ColNo(KColParamId)));
            aNumCols.AppendL(lDBView.ColInt(iColSet->ColNo(KColAppId)));
            aNumCols.AppendL(lDBView.ColInt(iColSet->ColNo(KColValueInt)));           

            TPtrC lValueDes = lDBView.ColDes16(iColSet->ColNo(KColValueDes));
            HBufC* lBuff = HBufC :: NewL(lValueDes.Length());
            lBuff->Des().Append(lValueDes);
            aDesCols.AppendL(lBuff);
        }
        while(lDBView.NextL());
    }
    
    _SCPDB_LOG(_L("[CSCPParamDB]-> ListEntries() <<<"));
    CleanupStack :: PopAndDestroy(2);
    return KErrNone;
}
