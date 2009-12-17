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

#include <d32dbms.h>
#include <e32base.h>

//database name
//_LIT(KtxDatabaseName, "DBS_INTERNET_ConnectionMethods.db");
_LIT(KtxDatabaseName, "c:InternetConnectionMethods.db");
_LIT( KNonSecureDbFullName, "C:\\system\\data\\InternetConnectionMethods.db" );

//Originator max length
#define KOriginatorMaxLength 50 

// database column names
_LIT(NCol1, "Index");
_LIT(NCol2, "APID");
_LIT(NCol3, "BearerType");
_LIT(NCol4, "Originator");

// database table name
_LIT(KtxtItemlist, "itemlist");

class TAccessPointItem
    {
public:
    TAccessPointItem() :
        iIndex(-1)
        {
        }
    ;
public:
    TInt iIndex;
    TUint iAPID;
    TUint iBearerType;
    TBuf<KOriginatorMaxLength> iOriginator;
    };

class CWPInternetAPDB : public CBase
    {
public:
    /**
     * Two-phased constructor.
     */
    static CWPInternetAPDB* NewL();

    /**
     * Two-phased constructor.
     */
    static CWPInternetAPDB* NewLC();

public:
    void ConstructL();
    ~CWPInternetAPDB();
public:
    /** 
     * CreateandOpenL
     * Creates and Opens the database.
     * @param name Path of the database
     * @return void
     **/
    inline void CreateandOpenL(TParse& name);
    /** 
     * ReadItemsL
     * Reads the Access Point data from a 
     * database and store the data in a Item array.
     * @param aItemArray Item Array
     * @param QueryBuffer SQL Query
     * @return void
     **/
    void
            ReadItemsL(RArray<TAccessPointItem>& aItemArray,
                    TFileName QueryBuffer);
    /** 
     * SaveToDatabaseL
     * Saves the Access Point data in a database
     * database and stores the data in a Item array.
     * @param aIndex Index
     * @param aAPId Access Point ID
     * @param aBearerType Bearer Type
     * @param aOriginator Operator
     * @return void
     **/
    void SaveToDatabaseL(TInt& aIndex, TUint aAPId, TUint32 aBearerType,
            const TDesC& aOriginator);
    /** 
     * ReadDbItemsL
     * Reads the Access Point data from a 
     * database and stores the data in a Item array.
     * @param aItemArray Item Array
     * @return void
     **/
    void ReadDbItemsL(RArray<TAccessPointItem>& aItemArray);

    /** 
     * ReadDbItemsForOrigL
     * Reads the Access Point data  for an operartor  
     * from a database and stores the data in a Item array.
     * @param aItemArray Item Array
     * @param aValue Operator
     * @return void
     **/
    void ReadDbItemsForOrigL(RArray<TAccessPointItem>& aItemArray,
            TDesC& aValue);
    
    /** 
     * DeleteFromDatabaseL
     * Deletes the existings rows which have same Access Point ID  
     * @param aAPId Access Point ID
     * @return TBool deleted rows
     **/
    TBool DeleteFromDatabaseL(TUint aAPId);
private:
    /** 
     * CreateTableL
     * Creates table for a database  
     * @param aDatabase Database Name
     * @return void
     **/
    void CreateTableL(RDbDatabase& aDatabase);
private:
    // Database Name
    RDbNamedDatabase iItemsDatabase;
    // File session
    RFs iFsSession;
    //Datbase session
    RDbs iRdbSession;
    };
