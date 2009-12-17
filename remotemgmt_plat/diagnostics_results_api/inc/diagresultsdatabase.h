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
* Description:  Results database client interfaces.
*                Provides results database session and subsession.
*  libraries   : DiagResultsDatabase.lib DiagResultsDatabase.dll
*
*/


#ifndef DIAGNOSTICS_RESULTS_DATABASE_CLIENT_H
#define DIAGNOSTICS_RESULTS_DATABASE_CLIENT_H

#include <e32base.h>

class CDiagResultsDatabaseItem;
class TDiagResultsDatabaseTestRecordInfo;
class CDiagResultsDbRecordEngineParam;

/**
* Diagnostics Results Database. Stores test results and more information.
* Provides methods to retrieve and fetch new data.
* Requires ReadDeviceData and WriteDeviceData capabilities even if the
* session does not write anything.
*
* Results database is a permanent file store based solution.
* Database files are stored under WINSCW\C\private\10282cd9 in emulator
* environment.
*
* It is recommended to have only one open RDiagResultsDatabase per thread.
* There should be only one writer and multiple readers at the same time
* accessing the same database file. It is possible that there are multiple
* DB files for example when there are two diagnostics applications in the
* system.
*
* @since S60 v5.0
**/
class RDiagResultsDatabase : public RSessionBase
    {
public:

    /**
    * Constructor
    **/
    IMPORT_C RDiagResultsDatabase();
    
    /**
    * Destructor
    **/
    IMPORT_C ~RDiagResultsDatabase();

    /**
    * Connect to the results database to a DB file. This must be called before 
    * trying to use this class, otherwise KERN-EXEC 0 will crash your app. 
    * Only one RDiagResultsDatabase connection is recommended per application,
    * because server sessions consume significant amount of system resources.
    * Share the session inside your thread! Subsessions are much more lighter. 
    *
    * Calling connect creates the DB file inside server's private directory.
    * If for some reason database is corrupted, the DB file should be deleted
    * manually.
    *
    * @param aAppUid Unique identifier of the application. Each application has 
    *   its own store.  Applications can access to another 
    *   applications's database if they know their UID.
    * @return KErrNone, if successful, otherwise one of the other system-wide
    *   error codes. If calling process does not have access rights, 
    *   KErrPermissionDenied is returned. ReadDeviceData+WriteDeviceData are
    *   required. KErrDiskFull is returned if there is not enough space 
    *   on c-drive.
    **/
    IMPORT_C TInt Connect( TUid aAppUid );

    /**
    * Close the session with the server. Handle is not valid anymore.
    *
    * @return KErrNone, if successful, otherwise one of the other system-wide
    *   error codes.
    **/
    IMPORT_C TInt Close();

    /**
    * Returns the version number of the server.
    *
    * @return The version number.
    **/
    IMPORT_C TVersion Version() const;

    /**
    * Get the current number of test records in the database.
    *
    * @param aCount the number of records.
    * @return KErrNone, if successful, otherwise one of the other system-wide 
    *   error codes.
    **/
    IMPORT_C TInt GetRecordCount ( TUint& aCount ) const;

    /**
    * Returns the maximum number of records for each application. The number 
    * is defined in the central repository and it is static for all applications. 
    * It is guarenteed that the DB file contains only the maxsize records and
    * not more. Old test records are deleted when new test records are 
    * completed.
    *
    * @param aMaxSize The maximum number of test records.
    * @return KErrNone, if successful, otherwise one of the other system-wide
    *   error codes.
    **/
    IMPORT_C static TInt GetDatabaseMaximumSize( TInt& aMaxSize );

    /**
    * Returns an array of record Ids. The oldest records are at the beginning
    * of the array. Test record are sorted based on creation time.
    *
    * @param aSortedRecordUidArray An array of UIds. The array is sorted 
    *   based on time.
    * @return KErrNone, if successful, otherwise one of the other system-wide
    *   error codes.
    **/
    IMPORT_C TInt GetRecordUids ( CArrayFixFlat<TUid>& aSortedRecordUidArray ) const;

    /**
    * Returns id to last test record. The last record cannot be 
    * still open for writing! Test record are sorted based on creation time.
    *
    * @param aRecordUid Unique identifier of the record.
    * @return KErrNone, if successful, otherwise one of the other system-wide
    *   error codes. Returns KErrNotFound if there are no records.
    **/
    IMPORT_C TInt GetLastRecord ( TUid& aRecordUid ) const;
    
    /**
    * Returns id to last suspended or incompleted test record. 
    * The last record cannot be still open for writing! Returns KErrNotFound 
    * if there are no suspended or incomplete test records.
    *
    * @param aRecordUid Unique identifier of the record.
    * @return KErrNone, if successful, otherwise one of the other system-wide
    *   error codes. Returns KErrNotFound if there are no suspended test records.
    **/
    IMPORT_C TInt GetLastNotCompletedRecord ( TUid& aRecordUid ) const;
    
    /**
    * Returns an overview from all records. 
    *
    * @param aInfoArray returns an array of record infos.
    * @see TDiagResultsDatabaseTestRecordInfo.
    **/
    IMPORT_C TInt GetAllRecordInfos ( 
               CArrayFixFlat<TDiagResultsDatabaseTestRecordInfo>& aInfoArray );

    /**
    * Initiate retrieving of last results.  After request is completed,
    * data can be fetched using GetLastResults method.
    *
    * @param aUidArray An array of uid used to identify plug-ins.
    * @param aStatus Completion status. 
    **/
    IMPORT_C void InitiateGetLastResults ( const CArrayFixFlat<TUid>& aUidArray,
                                                TRequestStatus& aStatus );
                                                
    /**
    * Cancel InitiateGetLastResults.
    **/                                                
    IMPORT_C void CancelInitiateGetLastResults () const;
    
    /**
    * Get list of last results. Last result means that the plug-in has passed / failed
    * test result. 
    * 
    * GetLastResults deletes data after this function 
    * is called, so InitiateGetLastResults must be called if data is needed 
    * again. Client is responsible for deleting the returned array. Contains
    * null values if the searched uid was not found. There should be the same
    * number of cells in UidArray and in the results array.   
    *
    * @param aResults Returned results array. Client is responsible for 
    *                 deleting the array.
    *
    * @return KErrNotReady if this is called before InitiateGetLastResults.
    */
    IMPORT_C TInt GetLastResults ( RPointerArray<CDiagResultsDatabaseItem>& aResults ) const;
    
    /**
    * Get last (the newest) test result of a test plug-in. The algorithm searches for a 
    * test result that is either passed or failed. This is also asynchronous operation,
    * because all records have to be browsed in worst case scenario (that is, when
    * the plug-in uid is not found). Call GetLastResult when test result is needed.
    *
    *
    * @param aTestPluginUid UID of a plug-in.    
    * @param aStatus Asynchronous request status.
    * 
    */
    IMPORT_C void InitiateGetLastResult ( TUid aTestPluginUid, 
                                          TRequestStatus& aStatus );
    

    /**
    * Get last result of a test plug-in.
    *
    * @param aItem Test result or NULL if not found.
    * @retrun KErrNotReady if this is called before InitiateGetLastResult.
    **/                                          
    IMPORT_C TInt GetLastResult ( CDiagResultsDatabaseItem*& aItem );

private:
    
    /**
    * Leaving version of the service functions. Look above for explanations.
    **/                    
    void DoConnectL (TUid aAppUid);
    
    void DoGetRecordUidsL( CArrayFixFlat<TUid>& aSortedRecordUidArray ) const;

    /*
    * @see GetDatabaseMaximumSize()
    */
    static void DoGetDatabaseMaximumSizeL( TInt& aMaxSize );
    
    void DoGetAllRecordInfosL ( CArrayFixFlat<TDiagResultsDatabaseTestRecordInfo>& aInfoArray);
    
    void DoGetLastResultsL ( RPointerArray<CDiagResultsDatabaseItem>& aResults ) const;
    
    void WriteArrayIntoBufferL( const CArrayFixFlat<TUid>& aUidArray );   
    
    void DoGetLastResultL ( CDiagResultsDatabaseItem*& aItem );                                    

private: 

    // Flat dynamic buffer.
    CBufFlat* iBuffer;
    
    // is the connection already open.
    TBool iOpen;     
    
    //Buffer pointer
    TPtr8 iPtr;                 
    };
   
/**
* Sub-session to Diagnostics Results Database. This API is used to
* handle a Test record. A test record is a collection of test results.
*
* Test record is always written into the file, when updates are necessary. 
* 
*
* @since S60 v5.0 
**/
class RDiagResultsDatabaseRecord : public RSubSessionBase
    {
public:

    /**
    * Test record status. 
    **/
     enum TRecordStatus
        {    	      
        EOpen,                // Currently open for writing.
        ESuspended,   	      // ::Suspend() is called. 
        ECrashed,		      // It was open previously, and it was not 
                              // closed properly.
        EPartiallyCompleted,  // Record is completed (not resumable), but not
                              // all tests were completed.
        ECompleted	          // Record is completed (not resumable), and all 
                              // tests were completed. 
        };

    /**
    * Constructor
    **/
    IMPORT_C RDiagResultsDatabaseRecord();
    
    /**
    * Destructor
    **/
    IMPORT_C ~RDiagResultsDatabaseRecord();

    /**
    * Connects to a test record. There must be at least one created record.
    * If the specified record is not found, KErrNotFound is returned.
    * Opened test record can be modified if the test record has not been
    * completed (= TestCompleted called).
    *
    * @param aSession open database session.
    * @param aRecordId specifies the record where we want to connect.
    * @param aReadOnly Indicates is the test record opened in read-only mode.
    *                  If read-only = EFalse, the test record can overwrite
    *                  test records that are not completed already. If 
    *                  the test record is completed it can be opened only 
    *                  in read-mode. 
    *
    * @return KErrNone, if successful, otherwise one of the other system-wide
    *   error codes. Returns KErrAlreadyExists if subsession is connected.
    **/
    IMPORT_C TInt Connect( RDiagResultsDatabase& aSession, 
                           TUid aRecordId, 
                           TBool aReadOnly );
    
    /**
    * Creates a new record and returns the uid of that record. 
    * Writes empty test record into the file. Create new record opens the record
    * always in Write mode.
    *
    * @param aSession open database session.
    * @param aRecordId a new unique identifier is returned that is used to
    *   identify particular test record. Client can use the record ID to access
    *   the same record again.
    * @param aEngineParam Engine parameters. 
    *
    * @return KErrNone, if successful, otherwise one of the other system-wide
    *   error codes. Returns KErrAlreadyExists if subsession is connected.
    *   KErrDiskFull is returned if there is not enough space on c-drive.
    **/
    IMPORT_C TInt CreateNewRecord( RDiagResultsDatabase& aSession, 
                                   TUid& aRecordId,
                                   CDiagResultsDbRecordEngineParam& aEngineParam );
    
    /**
    * Retrieve parameters of the diagnostics engine. These are needed in
    * Suspend/Resume functionality. 
    *
    * @param aEngineParam Engine parameters. Ownership is transferred.
    *                     Client is responsible for deleting the object.
    * @return KErrNone, if successful, otherwise one of the other system-wide
    *   error codes.
    **/
    IMPORT_C TInt GetEngineParam( CDiagResultsDbRecordEngineParam*& aEngineParam ) const;


    /**
    * Get record status.
    *
    * @param aRecordStatus The status of the record. 
    * @see TRecordStatus in the beginning of the class declaration.
    *
    * @return KErrNone, if successful, otherwise one of the other system-wide
    *   error codes.
    **/
    IMPORT_C TInt GetStatus( TRecordStatus& aRecordStatus ) const;

    /**
    * Close subsession. Does not write test record into the file.
    *
    * @return Symbian error code or KErrNone.
    **/
    IMPORT_C void Close();
    
    /**
    * Suspend test record. Same as complete, but the test record can be modified
    * on next connect.Writes test suspend time into test record info.
    *
    * @return Symbian error code or KErrNone.
    **/
    IMPORT_C TInt Suspend();
    
    /**
    * Indicates has this test record been suspended.
    * @param aSuspended Returns has this test record been suspended(ETrue), 
    *                   otherwise EFalse.
    * @return KErrNone, if successful, otherwise one of the other system-wide
    *   error codes.
    **/
    IMPORT_C TInt IsSuspended( TBool& aSuspended ) const;

    /**
    * Returns the record UID of this subsession.
    *
    * @param aRecordUid Record ID of the subsession.
    * @return KErrNone, if successful, otherwise one of the other system-wide
    *   error codes.
    **/
    IMPORT_C TInt GetTestRecordId( TUid& aRecordUid ) const;

    /**
    * Write test completion time/date to the record and prevent further 
    * writing. Handle is still valid. However, it cannot be used for adding
    * any more test results. During completion database file is cleaned up from
    * old test records (limited by GetDatabaseMaximumSize method).
    *
    * Test completion is synchronous operation because only record handle must be
    * updated (fixed size stream).
    *
    * @param aFullyComplete If ETrue (default), test record is completed with
    *                       status fully completed. Otherwise the test record is 
    *                       considered partially completed. 
    *
    * @return Symbian error code or KErrNone. KErrDiskFull is returned if there 
    |         is not enough space on c-drive.   
    **/
    IMPORT_C TInt TestCompleted( TBool aFullyComplete = ETrue );
    
    /**
    * Indicates has this record been written into the DB file or not. 
    *
    * @param aCompleted Has the test record been completed.
    * @return KErrNone, if successful, otherwise one of the other system-wide
    *   error codes.
    **/ 
    IMPORT_C TInt IsTestCompleted( TBool& aCompleted ) const;
    
    /**
    * Returns information about the record. If the record is not finalized,
    * iFinishTimes contains unknown values. 
    *
    * @param aInfo returns an overview of the test record.
    * @return KErrNone, if successful, otherwise one of the other system-wide
    *   error codes.
    **/
    IMPORT_C TInt GetRecordInfo ( TDiagResultsDatabaseTestRecordInfo& aInfo ) const;
    
    /**
    * Get list of test UIDs inside the test record.
    *
    * @param aTestUidArray An array of test uids.
    * @return KErrNone, if successful, otherwise one of the other system-wide
    *   error codes.
    **/
    IMPORT_C TInt GetTestUids ( CArrayFixFlat<TUid>& aTestUidArray ) const;
    
    /**
    * Logs results of a test into the database including more information
    * that there might be. See class CDiagResultsDatabaseItem. Result Item
    * is written into the file immediately after TRequestStatus completes.
    *
    * @param aResultItem This structure is stored into the database. 
    *   Client is responsible for creating the object. Also client is responsible 
    *   for setting item's values.
    *
    * @param aStatus Asynchronous status. Completes when process is finished.
    *
    * @return KErrNone, if successful, otherwise one of the other system-wide
    *   error codes. Returns KErrAlreadyExists if record is already completed.
    *   KErrDiskFull is returned if there is not enough space on c-drive. 
    *   If KErrDiskFull is received, the test result is not logged.
    *
    **/ 
    IMPORT_C TInt LogTestResult ( TRequestStatus& aStatus, 
                                  const CDiagResultsDatabaseItem& aResultItem );

    /**
    * Cancel LogTestResult. Server continues to write result item into the DB.
    * Cancelling only completes client's request.
    **/                                  
    IMPORT_C void CancelLogTestResult() const;                                  
    
    /**
    * Get single test result from this test record identified by an uid. 
    * 
    * @param aPluginUid Unique identifier of the plug-in whose results are needed.
    * @param aResultItem Contains test results.
    * @return KErrNone, if successful, otherwise one of the other system-wide 
    *   error codes.KErrNotFound is returned if the plug-in is not found
    *   from the record.
    **/
    IMPORT_C TInt GetTestResult ( TUid aPluginUid, 
                                  CDiagResultsDatabaseItem*& aResultItem ) const;
   
    /**
    * Get all test results inside the test record. Client is responsible for 
    * reseting/destroying the array.
    *
    * @param aResultsArray Returns test results for this record. 
    *
    * @return KErrNone, if successful, otherwise one of the other system-wide
    *   error codes.
    */
    IMPORT_C TInt GetTestResults ( RPointerArray<CDiagResultsDatabaseItem>& aResultsArray ) const;
 
private:
    
    /**
    * Leaving version of the functions and helper functions.
    **/ 
    void DoConnectL ( RDiagResultsDatabase& aSession, 
                      TUid aRecordId,
                      TBool aReadOnly );
        
    void DoCreateNewRecordL ( RDiagResultsDatabase& aSession, TUid& aRecordId,
                              CDiagResultsDbRecordEngineParam& aEngineParam );
    
    void DoGetTestUidsL ( CArrayFixFlat<TUid>& aTestUidArray ) const;
    
    void DoLogTestResultL ( TRequestStatus& aStatus, 
                            const CDiagResultsDatabaseItem& aResultItem );
    
    void DoGetTestResultsL ( RPointerArray<CDiagResultsDatabaseItem>& aResultsArray ) const;
    
    void DoGetTestResultL ( TUid aPluginUid, CDiagResultsDatabaseItem*& aResultItem ) const;
    
    void WriteDatabaseItemIntoBufferL( const CDiagResultsDatabaseItem& aResultItem );
    
    void WriteEngineParamIntoBufferL( CDiagResultsDbRecordEngineParam& aEngineParam );
    
    void DoGetEngineParamL( CDiagResultsDbRecordEngineParam*& aEngineParam ) const;
    
private: 

    // Dynamic flat buffer.
    CBufFlat* iBuffer;  
    
    // Is the connection already open.
    TBool iOpen;  
    
    //Buffer pointer
    TPtr8 iPtr;
    };

#endif  // DIAGNOSTICS_RESULTS_DATABASE_CLIENT_H

