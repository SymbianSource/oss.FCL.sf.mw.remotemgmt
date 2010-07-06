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
* Description:   Header file for fotadb
*
*/




#ifndef __CFOTADB_H___
#define __CFOTADB_H___

// INCLUDES
#include <d32dbms.h>
#include <fotaengine.h>
#include "FotaIPCTypes.h"


// CONSTANTS
// see fotaipctypes.h for field descriptions
_LIT(KDatabaseName,"FotaState");

_LIT(KSelectAll,"SELECT * FROM State" );
_LIT(KSelect_where_packageid,"SELECT * FROM State WHERE PkgID=%d" );
_LIT(KCreateTable, "CREATE TABLE State ( Id COUNTER, PkgID INTEGER NOT \
     NULL, Result INTEGER, State INTEGER NOT NULL, ProfileID INTEGER,\
	     PkgURL LONG VARCHAR, PkgName CHAR(255),Version CHAR(255), SmlTryCount INTEGER, \
         SessionType INTEGER, IapId INTEGER, PkgSize UNSIGNED INTEGER, UpdateLtr BIT)");
_LIT(KAlterTable, "ALTER TABLE State ADD (SessionType INTEGER, IapId INTEGER, PkgSize UNSIGNED INTEGER, UpdateLtr BIT)");
_LIT(KTblState,     "State");

_LIT(KColPkgId,     "PkgID");
_LIT(KColResult,    "Result");
_LIT(KColState,     "State");
_LIT(KColProfileId, "ProfileID");
_LIT(KColPkgUrl,    "PkgURL");
_LIT(KColPkgName,   "PkgName");
_LIT(KColVersion,   "Version");
_LIT(KColSmlTryCount, "SmlTryCount");
_LIT(KColSessionType, "SessionType");
_LIT(KColIapId, "IapId");
_LIT(KColPkgSize, "PkgSize");
_LIT(KColUpdateLtr, "UpdateLtr");

// DATATYPES
enum TFDBFields
{
    EFDBResult      = 0x01,
    EFDBState       = 0x02,
    EFDBProfileId   = 0x04,
    EFDBPkgUrl      = 0x08,
    EFDBPkgName     = 0x10,
    EFDBVersion     = 0x20,
    EFDBSmlTryCount = 0x40,
    EFDBSessionType = 0x80,
    EFDBIapId 		= 0x100,
    EFDBPkgSize 	= 0x200,
    EFDBUpdateLtr 	= 0x400
};
//Number of attributes in fotastate table
const TUint KNoOfDBFields = 13;

// FORWARD DECLARATION
class CPackageState;


/**
 *  Database class
 *
 *  @lib    fotaserver
 *  @since  S60 v3.1
 */
NONSHARABLE_CLASS(CFotaDB): public CBase
    {
    friend class CPackageState;
public:
	CFotaDB();

	virtual ~CFotaDB();

    static CFotaDB* NewL();

    /**
     * Create database
     *
     * @since   S60   v3.1
     * @param   None
     * @return  none
     */
    void CreateDatabaseL();
    

    /**
     * Open database
     *
     * @since   S60   v3.1
     * @param   None
     * @return  Error code
     */
    void OpenDBL();

    /**
     * Close database and commit changes
     *
     * @since   S60   v3.1
     * @param   None
     * @return  none
     */
    void CloseAndCommitDB();


    /**
     * Checks if DB is open
     *
     * @since   S60   v3.1
     * @param   None
     * @return  TBool - is database open
     */
    TBool IsOpen();
    

    /**
     * Add new package state to database
     *
     * @since   S60   v3.1
     * @param   aState      State
     * @param   aPkgURL     URL
     * @return  None
     */    
    void AddPackageStateL( const TPackageState& aState
                                ,const TDesC8& aPkgURL );


    /**
     * Get state from db
     *
     * @since   S60   v3.1
     * @param   aPkgId      Package ID
     * @return  State object
     */        
    TPackageState   GetStateL( const TInt aPkgId, TDes8& aPkgURL );


    /** Sets state of package specified by packageid. If state doesnt exist,
     * it is added to db.
     * @since   S60   v3.1
     * @param   aState   source data to be set
     * @param   aPkgURL  URL
     * @param   aChangedFields  Changed fields. Tells what columns from aState
     *                          are to be set.
     * @return  None
     */
    void SetStateL( TPackageState& aState
                        ,const TDesC8& aPkgURL, TUint aChangedFields );

    /**
     * Gets all states from database
     *
     * @since   S60   v3.1
     * @param   aStates     On return, all  states
     * @return  Error code
     */
    void GetAllL(RArray<TInt>& aStates);

private:

    void ConstructL();


    /**
     * Converts database row to TPackageState
     *
     * @since   S60   v3.1
     * @param  aPkgUrl  URL
     * @param  aView    Database view. Must be ready&prepared for data 
     *                  extraction
     * @return  State
     */
	TPackageState RowToStateL(HBufC8*& aPkgUrl,const RDbView& aView);


    /**
     * Inserts TPackageState to database view
     *
     * @since   S60   v3.1
     * @param  aPkg    Source package
     * @param  aPkgURL Url
     * @param  aView   Database view. Must be ready&prepared for data writing
     * @return  None
     */
    void StateToRowL (const TPackageState& aPkg, const TDesC8& aPkgURL
                    ,RDbView& aView);
    
    /**
     * Sets the retry count from the cenrep
     *
     * @since   S60   v5.0
     * @param   package state
     * @return  None
     */
    
    void SetRetryCount(TPackageState& aState);
    
    /**
     * Determines the char need for db queries
     *
     * @since   S60   v5.0
     * @param   aChangedFields change fields
     * @param   aState package states
     * @param   aPkgURL
     * @return  None
     */    
    TInt DetermineCharNeeded(TInt aChangedFields,TPackageState& aState,const TDesC8& aPkgURL);

private:

    /**
     * File server session
     */
    RFs iFSSession;

    
    /**
     * Reference to database
     */
    RDbNamedDatabase iStateDB;


    /**
     * Columns
     */
    CDbColSet* iColSet;


    /**
     * Table
     */
    RDbTable iTable;

    /**
    * Is the db open?
    */
    TBool iIsOpen;

    };

#endif // FOTADB_H
