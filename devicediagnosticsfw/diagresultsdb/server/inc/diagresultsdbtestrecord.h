/*
* Copyright (c) 2007-2007 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Represents test record.
*  libraries   : 
*
*/


#ifndef DIAGRESULTSDBTESTRECORD_H
#define DIAGRESULTSDBTESTRECORD_H

#include "diagresultsdatabaseitem.h"
#include "diagresultsdatabasetestrecordinfo.h"

/**
* Class represents a test record. Contains N amount of 
* test results (CDiagResultsDatabaseItem).
*
* @since S60 v5.0
**/
class CDiagResultsDbTestRecord : public CBase
	{
public:

    /**
    * Destructor.
    **/
	~CDiagResultsDbTestRecord();
    
    /**
    * NewL.
    * 
    * @param aReadOnly Indicates are we creating already completed test record.
    * @param aRecordUid The Uid of this test record. The test record can be
    *                   searched based on the UID. Very important to remember.
    * @param aDbUid Identifies what DB file the test record should be written 
    *               to (when test record is completed).
    * @return New record.
    **/
	static CDiagResultsDbTestRecord* NewL( TBool aReadOnly, 
	                                       TUid aRecordUid );

    /**
    * Return overview of the test record. 
    *
    * @return Overview of the test record.
    **/
    //TDiagResultsDatabaseTestRecordInfo& RecordInfo();
    
    /**
    * Add one test result into the test record.
    *
    * @param aItem Test result to be added.
    **/
    void AddL( CDiagResultsDatabaseItem* aItem );
    
    /**
    * Operator that returns the database item. 
    * Panics if index is out of boundaries.
    *
    * @param aIndex Index of the item in the test record.
    * @return Test result / databaseitem.
    **/
    const CDiagResultsDatabaseItem& operator[] (TInt aIndex) const;
    
    /**
    * Returns a pointer to a database item. 
    *
    * @param aIndex Index of the item in the test record.
    * @return NULL or database item.
    **/
    CDiagResultsDatabaseItem* GetItem(TInt aIndex) const;
    
    /**
    * Return the number of test results in the test record.
    *
    * @return The number of items.
    **/
    TInt Count() const;
    
    /**
    * Indicates has the test record been written into the DB file or not.
    *
    * @return ETrue if the test record is written, otherwise EFalse.
    **/
    TBool ReadOnly() const;
    
    /**
    * Remove one item from the test record. Note that the removed 
    * item is not deleted.
    *
    * @param aIndex Index of the item to be removed.
    **/
    void RemoveL( TInt aIndex );
    
    /**
    * Remove one item from the test record. 
    *
    * @param aTestUid UID of the test.
    **/
    void RemoveL( TUid aTestUid );
    
    /**
    * Check is this kind of test result already in the test record.
    * Matching is done using test result UID.
    *
    * @param aItem The item to be searched.
    * @return Index in the record or -1 if not found.
    **/    
    TInt FindTestRecord( const CDiagResultsDatabaseItem& aItem ) const;
    
    CDiagResultsDatabaseItem* FindTestRecord( TUid aUid );
            
private:     

    /**
    * Constructor.
    *
    * @param aReadOnly Indicates are we creating already completed test record.
    * @param aRecordUid The Uid of this test record. The test record can be 
    *                   searched based on the UID. Very important to remember.
    * @param aDbUid Identifies what DB file the test record should be written
    *               to (when test record is completed).
    * @return New record.
    **/ 
    CDiagResultsDbTestRecord( TBool aReadOnly, TUid aRecordUid );
    
    /**
    * ConstructL.
    **/   
    void ConstructL();

private: // Data
    
    // Array containing the test results.
    RPointerArray<CDiagResultsDatabaseItem>  iTestArray;
    
    // The record uid.
    TUid iRecordUid;
    
    // Database file uid.
    //TUid iDbUid;
    
    // Indicates has the test record been written into the DB file.
    TBool iReadOnly;
	};

#endif //DIAGRESULTSDBTESTRECORD_H	
	