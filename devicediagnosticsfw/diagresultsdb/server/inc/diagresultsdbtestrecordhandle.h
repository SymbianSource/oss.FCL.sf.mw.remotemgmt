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
* Description:  Represents test record handle. The handle contains 
* 			     stream ids to test results.
*  libraries   : 
*
*/


#ifndef DIAGRESULTSDBTESTRECORDHANDLE_H
#define DIAGRESULTSDBTESTRECORDHANDLE_H

#include "diagresultsdatabasetestrecordinfo.h"

#include <s32std.h> //TStreamId 

class CDiagResultsDbRecordEngineParam;

/**
* Test handle contains test result stream IDs.
* Contains also general record info.
* Stream ids can be used to store and load 
* test results ( CDiagResultsDatabaseItem ). 
* 
* This class is only in the server side.
* 
* Handle contains one stream Id per test result that is stored.
* Handle is stored into the root stream.
*
* @since S60 v5.0
**/
class CDiagResultsDbTestRecordHandle : public CBase
	{
public:

    /**
    * Maps stream ids into test case Uids.
    **/
    struct TTestResultHandle
        {
        TStreamId iStreamId;
        TUid iTestUid;
        };

    /**
    * Destructor.
    **/
	~CDiagResultsDbTestRecordHandle();
    
    /**
    * NewL.
    **/
	static CDiagResultsDbTestRecordHandle* NewL(
	                           TStreamId aRecordUid, 
	                           TUid aDbUid,
	                           CDiagResultsDbRecordEngineParam* aEngineParam );
	                                                 
    /**
    * NewL.
    **/
	static CDiagResultsDbTestRecordHandle* NewL( RReadStream& aStream );	                                                 

    /**
    * Return overview of the test record. 
    *
    * @return Overview of the test record.
    **/
    TDiagResultsDatabaseTestRecordInfo& RecordInfo();
    
    
    /**
    * Return Engine parameters. These are needed by the
    * Diagnostics framework.
    *
    * @return Engine parameters.
    **/
    const CDiagResultsDbRecordEngineParam& GetEngineParam() const;
    
    /**
    * Root stream ID.
    *
    * @return StreamID that represents this handle in the root stream.
    **/
    TStreamId RecordId() const;
    
    /**
    * Add one test result into the test record.
    *
    * @param aItem Test result to be added.
    **/
    void AddL( TTestResultHandle& aItem );
    
    /**
    * Update one test result handle. Assumes
    * that this kind of result handle already exists. 
    * Leaves with KErrNotFound if the resulthandle is not found
    * (checked using the UID).
    * 
    * @param aResultHandle Contains updated stream ID.
    **/
    void UpdateL( TTestResultHandle& aResultHandle );
        
    /**
    * Operator that returns the database item. 
    * Panics if index is out of boundaries.
    *
    * @param aIndex Index of the item in the test record.
    * @return Test result / databaseitem.
    **/
    const TStreamId& operator[] (TInt aIndex) const;
    
    
    /**
    * Get result handle.
    *
    * @param aIndex Index of the handle.
    * @return Return handle that corresponds the index.
    * 
    **/
    TTestResultHandle Get( TInt aIndex ) const;
    
    /**
    * Convert Test Uids into stream Ids. Stream ids can be used
    * to load actual results from the store.
    *
    * @param aTestUid Plug-in Uid.
    * @param aStreamId StreamId that matches plug-in Uid.
    * @return KErrNotFound or KErrNone.
    *
    **/
    TInt MatchingStreamId( TUid aTestUid, TStreamId& aStreamId ) const;
    
    /**
    * Return the number of test results in the test record.
    *
    * @return The number of items.
    **/
    TInt Count() const;
    
    /**
    * Remove one item from the test record. 
    *
    * @param aItem ID to be removed.
    **/
    void RemoveL( TStreamId& aItem );
    
    /**
    * Remove one item from the test record. 
    *
    * @param aItemUid UID to be removed.
    **/
    void RemoveL( TUid aItemUid );
    
    /**
    * Check does the handle already exist.
    *
    * @param aTestUid Plug-in uid to be searched.
    * @return Index of the found plug-in uid.
    **/
    TInt FindIndex ( TUid aTestUid ) const;
    
    /**
    * Check does the handle already exist.
    *
    * @param aTestUid Plug-in Uid to be searched.
    * @return ETrue if plug-in Uid is found, EFalse otherwise.
    **/
    TBool Find ( TUid aTestUid ) const;
        
    /**
    * Externalize the test result to a stream.
    *
    * @param aStream   Stream to write to.
    */
    void ExternalizeL( RWriteStream& aStream ) const; 
            
private:     

    /**
    * Constructor.
    **/ 
    CDiagResultsDbTestRecordHandle(
                              TStreamId aRecordUid, 
	                          TUid aDbUid,
	                          CDiagResultsDbRecordEngineParam* aEngineParam );
	                                   
	/**
    * Constructor.
    **/ 
    CDiagResultsDbTestRecordHandle();
	                                   
    /**
    * ConstructL.
    **/   
    void ConstructL();
    
    /**
    * ConstructL.
    **/   
    void ConstructL( RReadStream& aStream );
    
    /**
    * Internalize the test result from a stream.
    *
    * @param aStream   Stream to read from.
    */
    void InternalizeL( RReadStream& aStream );

private: // Data

    // Test record overview.
    TDiagResultsDatabaseTestRecordInfo  iTestRecordInfo;
    
    // Array containing the streans IDs of test results.
    // These can be used to reload test records.
    RArray<TTestResultHandle>  iHandleArray;
    
    // Represents root stream ID.
    TStreamId iRecordId;
    
    // Engine parameters. 
    CDiagResultsDbRecordEngineParam* iEngineParam;
	};

#endif //DIAGRESULTSDBTESTRECORDHANDLE_H	
	