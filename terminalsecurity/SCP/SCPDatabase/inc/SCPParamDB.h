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
#ifndef __CSCPARAMDB_H_
#define __CSCPARAMDB_H_

#include <e32base.h>
#include <e32std.h>
#include <badesca.h>
#include <e32def.h>
#include <e32base.h>
#include <f32file.h>
#include <d32dbms.h>

_LIT(KDatabaseName,             "c:scp.db");
_LIT(KTblParameter,             "PARAMETER");
_LIT(KColParamId,               "PARAMID");
_LIT(KColAppId,                 "APPID");
_LIT(KColValueInt,              "VALUEINT");
_LIT(KColValueDes,              "VALUEDES");

_LIT(KSelectAll,                        "SELECT * FROM PARAMETER");
_LIT(KSelectAllAscParamID,               "SELECT * FROM PARAMETER ORDER BY PARAMID ASC");
_LIT(KSelectWhereParamId,               "SELECT * FROM PARAMETER WHERE PARAMID=%d");
_LIT(KSelectWhereParamIdAppID,          "SELECT * FROM PARAMETER WHERE PARAMID=%d AND APPID=%d");
_LIT(KSelectAppIDParamValNotIn,         "SELECT * FROM PARAMETER WHERE NOT APPID=%d AND VALUEDES='%S'");
_LIT(KSelectWhereAppId,                 "SELECT * FROM PARAMETER WHERE APPID=%d");
_LIT(KSelectWhereAppIdByAscParamID,    "SELECT * FROM PARAMETER WHERE APPID=%d ORDER BY PARAMID ASC");
_LIT(KDeleteWhereParamIDValueDes,       "DELETE FROM PARAMETER WHERE PARAMID=%d AND VALUEDES='%S'");
_LIT(KDeleteWhereParamIDValueDesAppID,  "DELETE FROM PARAMETER WHERE PARAMID=%d AND VALUEDES='%S' AND APPID=%d");
_LIT(KDeleteWhereParamIDAppID,          "DELETE FROM PARAMETER WHERE PARAMID=%d AND APPID=%d");
_LIT(KDeleteWhereParamID,               "DELETE FROM PARAMETER WHERE PARAMID=%d");
_LIT(KUpdateValIntWhereParamId,         "UPDATE PARAMETER SET APPID=%d, VALUEINT=%d WHERE PARAMID=%d");
_LIT(KInsertValInt,                     "INSERT INTO PARAMETER (PARAMID, VALUEINT, APPID) VALUES(%d, %d, %d)");
_LIT(KInsertValDes,                     "INSERT INTO PARAMETER (PARAMID, VALUEDES, APPID) VALUES(%d, '%S', %d)");
_LIT(KUpdateValDesWhereParamId,         "UPDATE PARAMETER SET APPID=%d, VALUEDES='%S' WHERE PARAMID=%d");
_LIT(KCreateTable,                      "CREATE TABLE PARAMETER (PARAMID INTEGER NOT NULL,\
                                            APPID INTEGER, VALUEINT INTEGER, VALUEDES CHAR(255))");

/**
 *  Database class
 *
 *  @lib
 *  @since
 */
NONSHARABLE_CLASS(CSCPParamDB) : public CBase {
public:
    ~CSCPParamDB();
    static CSCPParamDB* NewL();
    static CSCPParamDB* NewLC();

    /*
     * Store value for a parameter into the database. The parameter value that can be stored through this function
     * can only be of type numeric/boolean (The 16 shared parameters excepting EPasscodeClearSpecificStrings, 
     * EPasscodeDisallowSpecific and EPasscodeAllowSpecific)
     * 
     * @Params - aParamID: The Parameter ID 
     *                     (Has to be one from the enum list, RTerminalControl3rdPartySession :: TTerminalControl3rdPartyMessages)
     *           aValue: The value of the parameter identified by aParamID
     *           aApp: The UID of the calling application.
     * 
     * @returns -   KErrNone: on successful completion.
     *              
     */
    TInt SetValueForParameterL(TInt aParamID, const TInt32 aValue, const TInt32 aApp);
    
    /*
     * Store values of a literal/string type parameter into the database. Currently the function supports set operations on
     * only EPasscodeDisallowSpecific
     * 
     * @Params - aParamID: The Parameter ID 
     *                     (Currently it has to be EPasscodeDisallowSpecific, for anything else an KErrNotSupported is returned)
     *           aParamValues: Multiple values (if any) can be specified through this array.
     *              NOTE: The values contained in this array has to be cleaned explicitly by the caller. (by a call to ResetAndDestroy())
     *              
     *           aApp: The UID of the calling application.
     *           
     * @returns -   KErrNone: on successful completion.           
     */
	TInt SetValuesForParameterL(TInt aParamID, const RPointerArray <HBufC>& aParamValues, const TInt32 aApp);

    /*
     * Returns the value associated with the parameter ID (The 16 shared parameters excepting EPasscodeClearSpecificStrings, 
     * EPasscodeDisallowSpecific and EPasscodeAllowSpecific)
     * 
     * @Params - aParamID: The Parameter ID 
     *                     (Has to be one from the enum list, RTerminalControl3rdPartySession :: TTerminalControl3rdPartyMessages)
     *           aValue: The value of the parameter identified by aParamID
     *           aApp: The UID of the calling application.
     *           
     * @returns -   KErrNone: on successful completion.           
     */
    TInt GetValueForParameterL(TInt aParamID, TInt32& aValue, TInt32& aApp);
    
    
    /*
     * Returns the string/descriptor values associated with the parameter.
     * 
     * @Params - aParamID: The Parameter ID 
     *                     (Currently it has to be EPasscodeDisallowSpecific, for anything else KErrNotSupported is returned)
     *                     
     *           aParamValues: Parameter value(s) as an array
     *              NOTE: The values contained in this array has to be cleaned explicitly by the caller. (by a call to ResetAndDestroy())
     *              
     *           aApp: The UID of the calling application.
     *           
     * @returns -   KErrNone: on successful completion.
     * 
     */
    TInt GetValuesForParameterL(TInt aParamID, RPointerArray <HBufC>& aParamValues, const TInt32 aApp);
    
    /*
     * Returns the list of applications that have currently set one or more values
     * 
     * @Params - aIDArray: Contains the UID's of the applications
     *           
     * @returns -   KErrNone: on successful completion.           
     */    
    TInt GetApplicationIDListL(RArray <TUid>& aIDArray);
    
    /*
     * This function can be called to check if any of the entries made for the descriptor column matches. The case might arise if 
     * multiple applications have set 'DisallowString' values that are same. This function can be called before DropValuesL()
     * to make sure that a shared value is not dropped from the DB.
     * 
     * @Params - aParamValue: The parameter value that needs to be checked.
     *           aApp: The UID of the calling application.
     *           
     * @returns - ETrue if the value is shared, EFalse otherwise (EFalse is returned also if the entry is not availble in the DB).
     */
    TBool IsParamValueSharedL(HBufC* aParamValue, const TInt32 aApp);
    
    /*
     * Removes all the entries corresponding to the values mentioned in the input array from the DB.
     * 
     * @Params - aParamValues: Parameter value(s) that need to be dropped from the DB.
     * 
     *           aApp:  The UID of the calling application (Only the entries of this application are dropped).
     *                  By default, if aApp is not specified, the function drops all the entries that match
     *                  the values mentioned in the input array.
     *           
     * @returns -   KErrNone: on successful completion.
     * 
     * NOTE:    The function by default (if aApp=-1) does not check if the value is shared accross multiple applications. As a safer approach, 
     *          IsParamValueSharedL() can be called on each of the parameter values being passed in the input 
     *          array before a call to this function.     *          
     *          If the function leaves, the parameter aParamValues has to be cleaned explicitly by a call to ResetAndDestroy.           
     * 
     * Currently this function can be called only to drop values set on EPasscodeDisallowSpecific.
     */
    TInt DropValuesL(TInt aParamID, RPointerArray <HBufC>& aParamValues, const TInt32 aApp=-1);
    
    /*
     * Lists the parameters (ID's) that the specified application has set.
     * 
     * @Params -    aParamIds: Contains the list of parameter ID's set by the specified application. 
     *              (All unique parameter ID's is aApp is not specified)
     * 
     *              aApp:  The UID of the calling application. (Optional)
     *           
     * @returns -   KErrNone: on successful completion.
     * 
     */   
    TInt ListParamsUsedByAppL(RArray <TInt>& aParamIds, const TInt32 aApp=-1);
    
    /*
     * Removes all entries of the parameter for the specified application.
     * 
     * @Params - aParamID: ID of the parameter that needs to be dropped.
     * 
     *           aApp:  The UID of the calling application (Only the entries of this application are dropped).
     *                  By default, if aApp is not specified, the function drops all the entries that match
     *                  the parameter ID.
     *           
     * @returns -   KErrNone: on successful completion.
     * 
     */   
    TInt DropValuesL(TInt aParamID, const TInt32 aApp=-1);
    
    /*
     * Provides a dump of all the entries currently available in the DB (For debug purposes only)
     * 
     * @Params - aNumCols:  On return, contains values of all the numeric type columns (PARAMID, APPID, VALUEINT) 
     *                      in column major fashion.
     *           
     *           aDesCols: Contains the values of the descriptor column (VALUEDES).
     * 
     *           aApp:  The UID of the calling application (Only the entries of this application are listed).
     *                  By default, if aApp is not specified, the application lists the entire DB)
     *           
     * @returns -   KErrNone: on successful completion.
     * 
     */
    TInt ListEntriesL(RArray <TInt32>& aNumCols, RPointerArray <HBufC>& aDesCols, const TInt32 aApp=-1);

private:
    CSCPParamDB() {}
    void ConstructL();

    TInt OpenDB();

private:
    RDbs iRdbSession;
    RDbNamedDatabase iParameterDB;
    CDbColSet* iColSet;
    RDbTable iTable;
};

#endif // __CSCPARAMDB_H_
