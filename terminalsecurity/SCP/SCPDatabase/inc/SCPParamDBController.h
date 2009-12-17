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
#ifndef __CSCPARAMDBCONTROLLER_H_
#define __CSCPARAMDBCONTROLLER_H_

#include "SCPParamDB.h"

NONSHARABLE_CLASS(CSCPParamDBController) : public CBase {
public:
    ~CSCPParamDBController();
    
    IMPORT_C static CSCPParamDBController* NewL();
    
    IMPORT_C static CSCPParamDBController* NewLC();

    IMPORT_C TInt SetValueL(TInt aParamID, const TInt32 aValue, const TInt32 aApp);
    
    IMPORT_C TInt SetValuesL(TInt aParamID, const RPointerArray <HBufC>& aParamValues, const TInt32 aApp);

    IMPORT_C TInt GetValueL(TInt aParamID, TInt32& aValue, TInt32& aApp);
    
    IMPORT_C TInt GetValuesL(TInt aParamID, RPointerArray <HBufC>& aParamValues, const TInt32 aApp);
    
    IMPORT_C TInt ListApplicationsL(RArray <TUid>& aIDArray);
    
    IMPORT_C TBool IsParamValueSharedL(HBufC* aParamValue, const TInt32 aApp);
    
    IMPORT_C TInt DropValuesL(TInt aParamID, const TInt32 aApp=-1);
    
    IMPORT_C TInt DropValuesL(TInt aParamID, RPointerArray <HBufC>& aParamValues, const TInt32 aApp=-1);
    
    IMPORT_C TInt ListParamsUsedByAppL(RArray <TInt>& aParamIds, const TInt32 aApp=-1);

    IMPORT_C TInt ListEntriesL(RArray <TInt32>& aNumCols, RPointerArray <HBufC>& aDesCols, const TInt32 aApp=-1);

private:
    CSCPParamDBController() : iParamDB(NULL) { }
    void ConstructL();

private:
    CSCPParamDB* iParamDB;
};

#endif // __CSCPARAMDB_H_
