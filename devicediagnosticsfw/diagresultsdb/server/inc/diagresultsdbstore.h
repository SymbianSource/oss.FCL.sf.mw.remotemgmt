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
* Description:  Permanent File Store for diagnostics test records.
*  libraries   : 
*
*/


#ifndef DIAGRESULTSDBSTORE_H
#define DIAGRESULTSDBSTORE_H

//System includes
#include <s32file.h> //CPermanentFileStore
#include <f32file.h> //Rfs, RFile
#include <e32base.h>

class CDiagResultsDatabaseItem;
class CDiagResultsDbTestRecord;
class CDiagResultsDbTestRecordHandle;
class TDiagResultsDatabaseTestRecordInfo;
class CDiagResultsDbRecordEngineParam;

/**
* An interface that must be implemented in order to call
* ExistingRecordsAsyncL that is an asynchronous method.
*
* @since S60 v5.0
**/
class MRecordLoadingObserver
    {
public:
    /**
    * Returns an array of test records. Contains all test records from a 
    * single database file.
    *
    * @param aError Indicates if there were any errors.
    * @param aArray An array of test records. Ownership is transferred.
    **/
    virtual void ExistingRecordsL( TInt aError, 
                       RPointerArray<CDiagResultsDbTestRecord>* aArray ) = 0;
   
    };
    
/**
* An interface that must be implemented in order to call CompleteRecordAsyncL.
*
* @since S60 v5.0
**/    
class MRecordCompletionObserver
    {
public:

    /**
    * This is called when test record is completed.
    *
    * @param aError Symbian error code or KErrNone.
    **/
    virtual void CompletedRecordL( TInt aError ) = 0;
    };    

/**
* Permanent file store class that manipulates results database file.
* Database files are saved under server's private directory. 
* For example epoc32\WINSCW\C\private\10282cd9\ in WINSCW.
* Provides functions to load, save, compact and cleanup data.
*
* Files have format [UID].dat e.g. [21234563].dat
*
* Stream Ids of the test record handle are stored into the root stream.
* Handles contain the stream ids to the test results.
*
* Implementation uses 3 kinds of streams:
* 1) root stream 
* 2) Test record handle stream
* 3) Test result stream
* 4) Last results buffer (buffer for overflowing test results)
*
* In order to retrieve a test result, we must first know which record
* is needed and then ask from the handle where test result is. 
* Of course we can browse all test records, because root stream
* contains stream ids into test record handles.
*
* @since S60 v5.0
**/
class CDiagResultsDbStore : public CActive
	{
public:

    /**
    * Destructor
    **/
	~CDiagResultsDbStore();
	
	/**
	* NewL.
	*
	* @param aUid UID of the database file. This class manipulates only one 
	*             database file at a time. If multiple database files needs
	*             to be open, use another instance.
	* @return New object.
	**/
	static CDiagResultsDbStore* NewL( TUid aUid );
    static CDiagResultsDbStore* NewLC( TUid aUid );
	
	/**
	* Create and return a new record. Ownership is transferred.
	*
	* @return a new test record.
	**/
    CDiagResultsDbTestRecordHandle* CreateNewRecordL( 
                             CDiagResultsDbRecordEngineParam* aEngineParam );
    
    /**
    * Write test record into the database.
    *
    * @param aHandle The test record to be written into the database.
    * @return KErrNone or symbian error code.                        
    **/
    TInt CompleteRecord( CDiagResultsDbTestRecordHandle& aHandle );
               
    /**
    * Write one test item into the DB.
    *
    * @param aCommit Indicates should we commit the store.
    * @param aHandle Handle that is updated to contain the item.
    * @param aTestItem An item to be added.
    *
    * @return Symbian error code or KErrNone.
    **/                         
    TInt CompleteTestResult( TBool aCommit, 
                             CDiagResultsDbTestRecordHandle& aHandle, 
                             CDiagResultsDatabaseItem& aTestItem );                         
                                                                                                           
    /**
    * Search database for a test record handle.
    * Handle does not contain test results directly. It only contains
    * stream ids to the test results and general record info.
    *
    * @param aUid test record is searched based on the uid.
    * @return The found test record or NULL.
    **/
    CDiagResultsDbTestRecordHandle* OpenExistingHandleL( TUid aUid );
    
    
    /**
    * Search database for a test record.
    *
    * @param aUid test record is searched based on the uid.
    * @param aReadOnly indicates should we allow writing.
    * @return The found test record or NULL.
    */
    CDiagResultsDbTestRecord* OpenExistingRecordL( TUid aUid, 
                                                   TBool aReadOnly );
    
    /**
    * Opens a single database item. 
    * @param aId Stream id that represents the item.
    *
    * @return The found item or NULL.
    **/
    CDiagResultsDatabaseItem* OpenExistingTestResultL( TStreamId aId );
       
    /**
    * Returns all record infos. Client is responsible for deleting the array.
    *
    * @return Record info array.
    **/
    RArray<TDiagResultsDatabaseTestRecordInfo>* ExistingRecordInfosL();
    
    /**
    * Retrieve all test records from the database asynchronously.
    *
    * @param aObserver Observer is notified after test records have 
    *                  been loaded.
    **/
    void ExistingRecordsAsyncL( MRecordLoadingObserver& aObserver );
    

        
    /**
    * Open last results buffer. It contains the test results that would have
    * been otherwise deleted. These are needed when all last results are
    * retrieved. Use with CleanUpDatabaseUseLastResultsBufferL.
    *
    * @return Test record. 
    **/
    CDiagResultsDbTestRecord* OpenExistingLastResultsBufferL(); 
    
    /**
    * Compacts database to have only necessary information. This should be 
    * done as often as necessary after writing. Compacting should not be
    * done after reading!
    * However note that this operation could take a long time. 
    **/
    void CompactDatabase();
    
	/**
	 * Cleanup database. Used deletion algorithm is taken from the
	 * Central Repository.
	 *
	 * @param aCommit ETrue commits the store.
	 **/ 	   
	void CleanUpDatabaseL( TBool aCommit );
    
    /**
    * Read available test record handle uids.
    *
    * @param aUids Contains available test record handle uids.
    **/
    void RecordUidsL( RArray<TUid>& aUids );
    
protected: //From CActive
    
    /**
    * Called from CActive::Cancel.
    **/
    virtual void DoCancel();

    /**
    * Asynchronous service function.
    **/
    virtual void RunL();
    
    /**
    * Called if error happens in RunL.
    **/
    virtual TInt RunError(TInt aError);
    
private:     
    
	/**
	 * Cleanup database to have only certain amount of test records 
	 * inside per each database file.
	 *
	 * @param aCommit ETrue commits the store.
	 **/ 	   
	void CleanUpDatabaseNoLastResultsL( TBool aCommit );
       
 	/**
 	 * Cleanup database and move some of the test results into the
 	 * last results buffer. This function guarantees that the DB
 	 * keeps last results. CleanUpDatabase does not do that.    
 	 *
 	 * @param aCommit ETrue commits the store. 
 	 **/ 
 	void CleanUpDatabaseUseLastResultsBufferL( TBool aCommit );

    /**
    * Read all stream ids from the root stream.
    * 
    * @param aIds StreamId array.
    **/    
    void ReadRootStreamL( RArray<TStreamId>& aIds );
        
    /**
    * Create empty root stream.
    *
    * @param aStore Root stream is created into this store.
    **/
    void CreateEmptyRootStreamL( CPermanentFileStore& aStore );
    
    /**
    * Replace root stream with an array.
    *
    * @param aArray Array that replaces any existing root stream.
    **/
    void ReplaceRootStreamL( RArray<TStreamId>& aArray );
    
    /**
    * Write test record into the database file.
    *
    * @param aCompletedRecord is the record closed/completed or not.
    * @param aCommit Commit the store or not.
    * @param aTestRecord The test record to be written into the file.
    **/
    void DoCompleteRecordL( CDiagResultsDbTestRecordHandle& aHandle );
                        
    /**
    * Complete test result i.e. write it into the store.
    * @param aHandle Handle to be written.
    * @param aTestItem Contains test results that are written into the store.
    **/                            
    void DoCompleteTestResultL( TBool aCommit, 
                                CDiagResultsDbTestRecordHandle& aHandle, 
                                CDiagResultsDatabaseItem& aTestItem );                            
                                   
    
    /**
    * Constructor.
    * 
    * @param aDbUid Database file uid.
    **/
    CDiagResultsDbStore( TUid aDbUid );
    
    /**
    * ConstructL.
    **/
    void ConstructL();
    
    /**
    * Complete own request is needed in some cases because this class uses 
    * CActive to partition tasks into smaller pieces. This function completes
    * TRequestStatus so that the RunL is called.
    **/
    void CompleteOwnRequest();
    
    //State of this class. These are used only in asynchronous methods.
    enum EState
        {
        ENotRunning,
        EGetRootStream,
        EGetRecord,
        ERecordsReady,
        };
    
    enum EDeletionAlgorithm
        {
        EMaxRecordCountAlgorithm,
        ELastResultsAlgorithm,
        };
     
     /**
     * Write handle into the store. Handle knows where test results are.
     * @param aHandle The handle that is written into the store.
     **/   
     void WriteHandleIntoDbL( CDiagResultsDbTestRecordHandle& aHandle );
     
     
     /**
     * Append one ID into the root stream.
     * @param aId ID to be added.
     **/
     void AppendRootStreamL( TStreamId& aId );
     
          
     /**
     * Deletes the oldest handle from the store.
     *
     * @param aIds Root stream.
     **/
     void DeleteOldestHandleL( RArray<TStreamId>& aIds );
             
     /**
     * Delete handle and its test results.
     *
     * @param aIndex Index of the handle in the root stream.
     * @param aIds Root stream.
     * 
     **/             
     void DeleteHandleL( TInt aIndex, RArray<TStreamId>& aIds );
     
     /**
     * Create last results buffer. It can be used to store test results
     * that would be deleted otherwise. 
     **/
     void CreateLastResultsBufferStreamL();
     
     /**
     * Open test record handle and its test results.
     *
     * @param aId Handle root stream ID.
     * @param aRecord Returned test record.
     * @param aHandle Returned test record handle.
     **/
     void OpenExistingRecordAndHandleL( TStreamId aId, 
                                        CDiagResultsDbTestRecord*& aRecord,
                                        CDiagResultsDbTestRecordHandle*& aHandle );

    /**
    * Check last results buffer and add test results into it if needed.
    *
    * @param aIds Root stream.
    **/                                        
     ///@@@KSR: changes for Codescanner error val = High 
     //void CheckOverflowingTestResults( RArray<TStreamId>& aIds );
     void CheckOverflowingTestResultsL( RArray<TStreamId>& aIds );

    /**
     * Return maximum file size defined in the Central Repository.
     * This can be used to prevent a DB file from using too much disk space.
     * 
     * @return Maximum file size in bytes.
     **/
    TInt GetMaximumFileSizeL();
    
    /**
     * Return the deletion algorithm defined in the central repository.
     * 
     * @return The type of the deletion algorithm.
     **/
    TInt GetDeletionAlgorithmL();
    
    /**
     * Get maximum file size from the central repository and check the file size.
     * 
     * @param aRfs Opened file server session.
     * @param aFileName File name of the DB file.
     * @param aFileSize Size of the DB file in bytes.
     **/
    void CheckMaximumFileSizeLimitL( RFs& aRfs, 
			  						 const TDesC& aFileName,
			  						 TInt aFileSize );

private: // Data

    // Store that is owned by this class. Performs many file manipulation operations.
	CPermanentFileStore* iStore;  
    
    // File DB uid. [UID].dat
    TUid iDbUid;

    // File server session
    RFs iRfs;
    
    // The database file.
    RFile iFile;
    
    // Loading observer to be notified async.
    MRecordLoadingObserver* iLoadingObserver;
    
    // Completion observer (notified async).
    MRecordCompletionObserver* iCompletionObserver;
    
    // Pointer to test record arrays. Used in async methods.
    RPointerArray<CDiagResultsDbTestRecord>* iRecordArray;
    
    // Contains available record ids. Used in async methods.
    RArray<TStreamId>* iRecordIds;
    
    // Used in async methods to remember what record was loaded.
    TInt iRecordNumber;
    
    // Contains the test record to be completed (Used only in async methods).
    CDiagResultsDbTestRecord* iTestRecord;
    
    // State of the asynchronous operation.
    EState iState;
    
    EDeletionAlgorithm iDeletionAlgorithm;
	};

#endif // DIAGRESULTSDBSTORE_H
