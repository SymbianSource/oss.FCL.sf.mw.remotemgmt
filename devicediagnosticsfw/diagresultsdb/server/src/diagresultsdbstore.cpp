/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Class definition of 
*
*/

#include <s32std.h>
#include <sysutil.h> 
#include <f32file.h> 
#include <centralrepository.h>

#include "diagresultsdbtestrecord.h"
#include "diagresultsdbstore.h"
#include "diagresultsdatabasecommon.h"
#include "diagresultsdbcrdc.h"
#include "diagresultsdatabase.h"
#include "diagresultsdbtestrecordhandle.h"
#include "diagresultsdbrecordengineparam.h"
#include "diagresultsdbprivatecrkeys.h"

_LIT( KFileType, ".dat" );
_LIT( KDriveC, "C:" );

const TInt KMininumDiskSpace = 500;

const TInt KFirstStreamId = 2;

// ---------------------------------------------------------------------------
// Constructor.
// ---------------------------------------------------------------------------
// 
CDiagResultsDbStore::CDiagResultsDbStore( TUid aDbUid ): 
        CActive( CActive::EPriorityStandard ), iStore( NULL ), 
        iDbUid( aDbUid ), iLoadingObserver( NULL ), iCompletionObserver( NULL ),
        iRecordArray( NULL ), iRecordIds(NULL), iRecordNumber(0), 
        iTestRecord( NULL ), iState( ENotRunning )
	{
	CActiveScheduler::Add(this);
	}

// ---------------------------------------------------------------------------
// NewL. Creates a DB file based on aDbUid if it does not exist.
// ---------------------------------------------------------------------------
// 
CDiagResultsDbStore* CDiagResultsDbStore::NewL( TUid aDbUid )
	{
	LOGME("CDiagResultsDbStore* CDiagResultsDbStore::NewL");
	CDiagResultsDbStore* store = new (ELeave) CDiagResultsDbStore( aDbUid );
	CleanupStack::PushL( store );
	store->ConstructL();
	CleanupStack::Pop();
	return store;
	}

// ---------------------------------------------------------------------------
// NewLC.
// ---------------------------------------------------------------------------
// 
CDiagResultsDbStore* CDiagResultsDbStore::NewLC( TUid aDbUid )
	{
	LOGME("CDiagResultsDbStore::NewLC");
	CDiagResultsDbStore* store = new (ELeave) CDiagResultsDbStore( aDbUid );
	CleanupStack::PushL( store );
	store->ConstructL();
	return store;
	}	

// ---------------------------------------------------------------------------
// Starts asynchronous loading of all test records from the DB.
// Observer is notified when test records have been loaded.
// ---------------------------------------------------------------------------
// 
void CDiagResultsDbStore::ExistingRecordsAsyncL( MRecordLoadingObserver& aObserver )
    {
    if ( IsActive() )
        {
        User::Leave( KErrInUse );
        }
    
    iLoadingObserver = &aObserver;
    
    iState = EGetRootStream;
    iRecordNumber = 0;
    
    CompleteOwnRequest();
    
    SetActive();
    }

// ---------------------------------------------------------------------------
// Cancel asynchronous functionality. Do not call this directly (use Cancel())
// ---------------------------------------------------------------------------
// 
void CDiagResultsDbStore::DoCancel()
    {
    if ( iRecordArray )
        {
        iRecordArray->ResetAndDestroy(); 
           
        delete iRecordArray;
        iRecordArray = NULL;   
        }
              
    //Note that Store is not responsible for deleting the iTestRecord.
    //iTestRecord is deleted by subsession.
            
    delete iRecordIds;
    iRecordIds = NULL;
    
    iState = ENotRunning;
    iRecordNumber = 0;
    }

// ---------------------------------------------------------------------------
// This is called if there are leaves or errors in RunL.
// ---------------------------------------------------------------------------
// 
TInt CDiagResultsDbStore::RunError(TInt aError)
    {
    TInt err = KErrNone;

    switch ( iState )
    	{
        case EGetRootStream:
        case EGetRecord:            
        case ERecordsReady:
            {
            TRAP( err, iLoadingObserver->ExistingRecordsL( aError, NULL ) );
            }
        break;
        default:
        	{
        	///@@@KSR: changes for Codescanner error val = High 
        	//User::Leave( aError );
        	}
    	}

    return err;
    }


// ---------------------------------------------------------------------------
// Keeps the database file small as possible. Compacting must be done often as
// possible keeping in mind that compacting could take a long time.
// ---------------------------------------------------------------------------
// 
void CDiagResultsDbStore::CompactDatabase()
    {    
    TRAPD( error, iStore->CompactL());
    
    if ( error != KErrNone )
        {
        User::Panic(_L("compact error"), 
            DiagResultsDbCommon::EDatabaseCompact );
        }    
    else 
        {
        //Commit must be called in order to really reduce the size of DB file.
        TRAP( error, iStore->CommitL() ); 
        if ( error != KErrNone )
           {
           User::Panic(_L("compact/commit error"), 
           DiagResultsDbCommon::EDatabaseCompactCommit );
           }
        }
    }

// ---------------------------------------------------------------------------
// Completes own request. This changes the value of TRequestStatus and causes
// RunL to be called afterwards (if CActive is active).
// ---------------------------------------------------------------------------
// 
void CDiagResultsDbStore::CompleteOwnRequest()
    {
    TRequestStatus* status = &iStatus;
    User::RequestComplete( status, KErrNone );
    }

// ---------------------------------------------------------------------------
// Asynchronous handling function. States are needed to separate two different
// tasks. The first one is loading test records from the db file. The second
// is writing a test record into the DB file.
// ---------------------------------------------------------------------------
//     
void CDiagResultsDbStore::RunL()
    {
    
    switch ( iState )
        {
        case EGetRootStream:
            {
            iRecordArray = new (ELeave) RPointerArray<CDiagResultsDbTestRecord>;
            iRecordIds = new (ELeave) RArray<TStreamId>;
            // Find available stream ids
            ReadRootStreamL( *iRecordIds );
                                   
            if ( iRecordIds->Count() == 0 ) //no records, no need to continue
                {
                 //Ownership is transferred
                iLoadingObserver->ExistingRecordsL( KErrNone, iRecordArray );
                iRecordArray = NULL; //do not delete
            
                iRecordIds->Close();
                delete iRecordIds;
                iRecordIds = NULL;
    
                iState = ENotRunning;
                iRecordNumber = 0;   
            
                iLoadingObserver = NULL;
                return;
                }
            
            iState = EGetRecord;
            
            CompleteOwnRequest();
            SetActive();
            }
            break;
            
        case EGetRecord:
            {            
            iRecordArray->Append ( OpenExistingRecordL( 
                            TUid::Uid( (*iRecordIds)[iRecordNumber].Value()), ETrue ) );
            iRecordNumber++;
            if ( iRecordNumber < iRecordIds->Count() )
                {
                iState = EGetRecord;
                }
            else 
                {
                iState = ERecordsReady;
                }
            
            CompleteOwnRequest();
            SetActive();
            }
            break;
            
        case ERecordsReady:
            {
             //Ownership is transferred
            iLoadingObserver->ExistingRecordsL( KErrNone, iRecordArray );
            iRecordArray = NULL; //do not delete
            
            iRecordIds->Close();
            delete iRecordIds;
            iRecordIds = NULL;
    
            iState = ENotRunning;
            iRecordNumber = 0;   
            
            iLoadingObserver = NULL;
            }
            break;
  
        
        default:
            User::Leave( KErrNotFound );
            break;
            
        }
        
    }

// ---------------------------------------------------------------------------
// Constructs the store. Creates the DB file if it does not exist or opens it. 
// Also creates an empty root stream into the store if file was created.
// There is some exception handling included for example if file exist but it
// cannot be opened.
// ---------------------------------------------------------------------------
// 
void CDiagResultsDbStore::ConstructL()
	{	
	LOGME("CDiagResultsDbStore::ConstructL");		
	//Read from central repository.
	TInt algorithm = GetDeletionAlgorithmL();
	
	if ( algorithm == 0 )
		{
		iDeletionAlgorithm = EMaxRecordCountAlgorithm;
		}
	else if ( algorithm == 1 )
		{
		iDeletionAlgorithm = ELastResultsAlgorithm;
		}
	else 
		{
		User::Panic( _L("Unknown Deletion Algorithm"), 
                DiagResultsDbCommon::EUnknownDeletionAlgorithm);  
		}
	
	LOGME("resultdbstore::Before irfs.connect");
    User::LeaveIfError(iRfs.Connect());
    LOGME("iRfs.connect");
    TBool tooLowSpace = SysUtil::DiskSpaceBelowCriticalLevelL( 
                                                    &iRfs, 
                                                    KMininumDiskSpace, 
                                                    EDriveC );
    LOGME1("tooLowSpace %d",tooLowSpace);
    if ( tooLowSpace )        
        {
        User::Leave( KErrDiskFull );
        }
    
    TInt err = iRfs.CreatePrivatePath( EDriveC );
    
    LOGME1("privatepathcreate %d",err);
    //Create private path if it does not exists
    if ( err != KErrAlreadyExists && err != KErrNone )
        {
        User::Leave ( err ); //Unexpected error
        }
    
    TBuf<256> filename;
    iRfs.PrivatePath( filename );
    filename.Insert( 0, KDriveC );
    filename.Append( iDbUid.Name() );
    filename.Append( KFileType );

    //Check is the File already there
    //open for reading and writing.
    err = iFile.Open( iRfs, filename, EFileShareAny|EFileWrite );  

    LOGME1("ifile.open %d",err);
    TInt fileError = KErrNotFound;
    TInt fileSize = 0;

    if ( err == KErrNone )
        {        
        fileError = iFile.Size( fileSize );    
        }
    
    
    //Checking is done when the file exists. 
    //File size reading error should never happen.
    //if the file is too large, the function will panic.
    if ( err == KErrNone && fileError == KErrNone )
    	{
    	CheckMaximumFileSizeLimitL( iRfs, filename, fileSize ); 
    	}
    
    iFile.Close();
    
    // File exist
    if ( err == KErrNone )
        {                           
        TRAPD( err, iStore = CPermanentFileStore::OpenL(iRfs, filename, 
                        EFileShareAny|EFileWrite|EFileRead|EFileStream ));
        
        if ( err != KErrNone )
            {           
            err = iRfs.Delete ( filename );
            
            if ( err != KErrNone )
                {
                User::Panic( _L("DiagServer corrupt file delete failed"), 
                        DiagResultsDbCommon::EServerCorruptFileDelete);
                }
                
            User::Leave( KErrCorrupt );
            }
            
        //This must be called, otherwise RStoreWriteStream write will fail.
        iStore->SetTypeL( iStore->Layout() );
        }
       
    else if ( err == KErrNotFound ) //Create new file  if the file is not found
        {
        LOGME("createnewfile");
        TRAPD( err, iStore = CPermanentFileStore::CreateL(iRfs, filename, 
                        EFileShareAny|EFileWrite|EFileRead|EFileStream ));
        
        LOGME1("createnewfile = %d",err);
        if ( err != KErrNone ) //File could not be created for some reason
            { 
             LOGME1("createnewfile panic = %d",err);
            User::Panic( _L("DiagServer unable to create DB file"), 
                            DiagResultsDbCommon::EServerFileCreationError);          
            }
            
        //This must be called, otherwise RStoreWriteStream write will fail.
        iStore->SetTypeL( iStore->Layout() );
        CreateEmptyRootStreamL( *iStore );
        
        if ( iDeletionAlgorithm == ELastResultsAlgorithm )
        	{
        	CreateLastResultsBufferStreamL();
        	}
        
        iStore->CommitL();
     
        }
    else 
        {
        LOGME1("else userleave = %d",err);
        User::Leave( err );
        }
	}


// ---------------------------------------------------------------------------
// Get central repository key that controls maximum file size.
// ---------------------------------------------------------------------------
//
TInt CDiagResultsDbStore::GetMaximumFileSizeL()
	{
	CRepository* cr = CRepository::NewLC( KCRUidDiagnosticsResults );

	TInt result = 0;	
	TInt error = cr->Get( KDiagDatabaseMaxFileSize, result );
	
	CleanupStack::PopAndDestroy();
	
	if ( error != KErrNone )
		{
		User::Leave(error);
		}
	return result;
	}


// ---------------------------------------------------------------------------
// Get central repository key that controls usage of the deletion algorithm.
// ---------------------------------------------------------------------------
//
TInt CDiagResultsDbStore::GetDeletionAlgorithmL()
	{
	CRepository* cr = CRepository::NewLC( KCRUidDiagnosticsResults );

	TInt result = 0;	
	TInt error = cr->Get( KDiagDatabaseDeletionAlgorithm, result );
	
	CleanupStack::PopAndDestroy();
	
	if ( error != KErrNone )
		{
		User::Leave(error);
		}
	return result;
	}


// ---------------------------------------------------------------------------
// Check DB file size and delete the file if it is too big.
// Checking is controlled by a central repository key.
//
// Note that this function deletes the file and after that panics, because
// the error is very serious. 
// ---------------------------------------------------------------------------
//
void CDiagResultsDbStore::CheckMaximumFileSizeLimitL( RFs& aRfs, 
													  const TDesC& aFileName,
													  TInt aFileSize )
	{
	//Read central repository value
	TInt maxFileSize = GetMaximumFileSizeL();
	
	//If == 0, feature is disabled i.e. there is no limitation
	//to the size of DB files.
	if ( maxFileSize == 0 )
		{
		return;		
		}
	else
		{
		if ( aFileSize > maxFileSize )
			{
			//Delete the file. 
			aRfs.Delete( aFileName );		
			User::Panic(_L("Maxfile Size exceeded"), 
			            DiagResultsDbCommon::EMaximumFileSizeExceeded );
			}
		}	
	}

// ---------------------------------------------------------------------------
// Create a stream for last results.
// The stream is created when DB file is created.
// 
// Because this is the first stream in the PermanentFileStore,
// we do not need to store the stream ID anywhere. Consider it as a constant.
// ---------------------------------------------------------------------------
//
void CDiagResultsDbStore::CreateLastResultsBufferStreamL()
    {        
    RArray<TUid>* uids = new (ELeave) RArray<TUid>;
    
    CDiagResultsDbRecordEngineParam* params = 
        CDiagResultsDbRecordEngineParam::NewL( uids, EFalse );
    CleanupStack::PushL( params );
                
    TStreamId id = iStore->ExtendL();
    
    //We assume that the permanent file store uses this as the first stream ID.
    __ASSERT_ALWAYS( id == TStreamId( KFirstStreamId ), 
                        User::Panic( _L("DiagServer Incorrect Last Results Buffer"), 
                        DiagResultsDbCommon::EIncorrectExtendNumberForLastResults)  );

    CleanupStack::Pop();
      
    CDiagResultsDbTestRecordHandle* handle = CDiagResultsDbTestRecordHandle::NewL( 
                                                    id, 
                                                    iDbUid, 
                                                    params );
                                                    
    CleanupStack::PushL( handle );
    
    WriteHandleIntoDbL( *handle ); //writes a new stream
         
    //Do not append on to the root stream!!
    //this should be visible only for the server.                  
    
    CleanupStack::PopAndDestroy();                                
        
    }

// ---------------------------------------------------------------------------
// Destructor.
// ---------------------------------------------------------------------------
// 	
CDiagResultsDbStore::~CDiagResultsDbStore()
	{
	if ( iRecordIds )
	    {
	    iRecordIds->Close();
	    delete iRecordIds;
        iRecordIds = NULL;    
	    }

    Cancel();
        
    delete iStore;
    iStore = NULL;
    
    iFile.Close();
    iRfs.Close();
	}

// ---------------------------------------------------------------------------
// Create a new record. 
// ---------------------------------------------------------------------------
// 
CDiagResultsDbTestRecordHandle* CDiagResultsDbStore::CreateNewRecordL( 
                                CDiagResultsDbRecordEngineParam* aEngineParam )
    {
    
    TBool tooLowSpace = SysUtil::DiskSpaceBelowCriticalLevelL( 
                                                    &iRfs, 
                                                    KMininumDiskSpace, 
                                                    EDriveC );
    if ( tooLowSpace )        
        {
        User::Leave( KErrDiskFull );
        }
    
    
    
    TStreamId id = iStore->ExtendL();
  
    CDiagResultsDbTestRecordHandle* handle = CDiagResultsDbTestRecordHandle::NewL( 
                                                    id, 
                                                    iDbUid, 
                                                    aEngineParam );
    CleanupStack::PushL( handle );
    
    WriteHandleIntoDbL( *handle ); //writes a new stream
    
    AppendRootStreamL( id ); //update root stream
    
    iStore->CommitL(); 
    
    CleanupStack::Pop();                                
    return handle;
    
    }

// ---------------------------------------------------------------------------
// Write test record handle into the store.
// ---------------------------------------------------------------------------
// 
void CDiagResultsDbStore::WriteHandleIntoDbL( CDiagResultsDbTestRecordHandle& aHandle )
    {
    RStoreWriteStream writestream;
    
    writestream.ReplaceLC( *iStore, aHandle.RecordId() );    
        
    aHandle.ExternalizeL( writestream );
    
    writestream.CommitL();
       
    CleanupStack::PopAndDestroy( &writestream );
    }

// ---------------------------------------------------------------------------
// Writes data into the DB file (.dat file). 
// ---------------------------------------------------------------------------
//     
TInt CDiagResultsDbStore::CompleteTestResult( TBool aCommit, 
                                    CDiagResultsDbTestRecordHandle& aHandle,  
                                    CDiagResultsDatabaseItem& aTestItem )
    {        
    TRAPD( err, DoCompleteTestResultL( aCommit, aHandle, aTestItem ));
    
    return err;
    }

// ---------------------------------------------------------------------------
// Writes handle into the DB. Test results items have been written already.
// see CompleteTestResult.
// ---------------------------------------------------------------------------
// 
TInt CDiagResultsDbStore::CompleteRecord(  
                                CDiagResultsDbTestRecordHandle& aHandle )
    {
    // ignore DRM error.
    TRAP_IGNORE( aHandle.RecordInfo().iDrmFinishTime = 
        TDiagResultsDatabaseTestRecordInfo::DRMTimeL() );

    aHandle.RecordInfo().iFinishTime.HomeTime();
    
    TRAPD( error, DoCompleteRecordL( aHandle ));
    
    return error;
    }

// ---------------------------------------------------------------------------
// Writes handle into the DB. Test results items have been written already.
// see CompleteTestResult.
// ---------------------------------------------------------------------------
// 
void CDiagResultsDbStore::DoCompleteRecordL(  
                                CDiagResultsDbTestRecordHandle& aHandle )
    {
    
    TBool tooLowSpace = SysUtil::DiskSpaceBelowCriticalLevelL( 
                                                    &iRfs, 
                                                    KMininumDiskSpace, 
                                                    EDriveC );
    if ( tooLowSpace )        
        {
        User::Leave( KErrDiskFull );
        }
    
    WriteHandleIntoDbL( aHandle );
    
    iStore->CommitL();
    }


// ---------------------------------------------------------------------------
// Write aTestItem and aHandle into the DB. These must be in sync.
// ---------------------------------------------------------------------------
//     
void CDiagResultsDbStore::DoCompleteTestResultL( TBool aCommit, 
                                        CDiagResultsDbTestRecordHandle& aHandle, 
                                        CDiagResultsDatabaseItem& aTestItem )
    {   
    
    TBool tooLowSpace = SysUtil::DiskSpaceBelowCriticalLevelL( 
                                                    &iRfs, 
                                                    aTestItem.Size() *2, 
                                                    EDriveC );
    if ( tooLowSpace )        
        {
        User::Leave( KErrDiskFull );
        }
    
    RStoreWriteStream writestream;
    
    CDiagResultsDbTestRecordHandle::TTestResultHandle testresulthandle;
    
    TBool found = aHandle.Find( aTestItem.TestUid() );
    
    
    if ( found ) //Replace old test result
        {
        TStreamId id;
        TInt error = aHandle.MatchingStreamId( aTestItem.TestUid(), id);
        User::LeaveIfError ( error );
        writestream.ReplaceLC( *iStore, id );
        }
    else //New test result
        {
        TStreamId newstreamid = writestream.CreateLC( *iStore );   
        testresulthandle.iStreamId = newstreamid;
        testresulthandle.iTestUid = aTestItem.TestUid();
        }

    aTestItem.ExternalizeL( writestream );
    writestream.CommitL();
    CleanupStack::PopAndDestroy( &writestream );
  
    if ( !found )
        {
        aHandle.AddL( testresulthandle );
        }
    
    WriteHandleIntoDbL( aHandle );
    
    if ( aCommit )
        {
        iStore->CommitL();       
        }      
    }

// ---------------------------------------------------------------------------
// Read a test record from the DB. The record should not be ever written into
// the same DB again. First read the number of items there are in the record.
// After that read the items and the record info. 
// ---------------------------------------------------------------------------
//     
CDiagResultsDbTestRecord* CDiagResultsDbStore::OpenExistingRecordL( TUid aUid, 
                                                            TBool aReadOnly )
    {    
    TStreamId id = TStreamId ( aUid.iUid );
    
    RStoreReadStream handlereadstream;
    handlereadstream.OpenLC( *iStore, id );
    
    CDiagResultsDbTestRecord* testrecord = CDiagResultsDbTestRecord::NewL( aReadOnly, 
	                                       aUid );
    CleanupStack::PushL ( testrecord );
    
    CDiagResultsDbTestRecordHandle* handle = 
                            CDiagResultsDbTestRecordHandle::NewL( handlereadstream );
                                                       
    CleanupStack::PushL ( handle );
    
    for ( TInt i = 0; i < handle->Count(); ++i )
        {
        RStoreReadStream readstream;
        readstream.OpenLC( *iStore, ((*handle)[i]) ); 
                
        testrecord->AddL( CDiagResultsDatabaseItem::NewL ( readstream ) );
        
        CleanupStack::PopAndDestroy( &readstream );  
        }

    CleanupStack::PopAndDestroy(); //handle
    CleanupStack::Pop(); //testrecord
    
    CleanupStack::PopAndDestroy( &handlereadstream ); 
    
    return testrecord;
    }


// ---------------------------------------------------------------------------
// Open last results buffer. It contains the test results that would have
// been otherwise deleted. 
// ---------------------------------------------------------------------------
//   
CDiagResultsDbTestRecord* CDiagResultsDbStore::OpenExistingLastResultsBufferL()
    {      
    CDiagResultsDbTestRecord* testrecord = 
                            OpenExistingRecordL( TUid::Uid(KFirstStreamId),
                                                 ETrue );                                                                                                                                       
    return testrecord;
    }


void CDiagResultsDbStore::OpenExistingRecordAndHandleL( TStreamId aId, 
                                                        CDiagResultsDbTestRecord*& aRecord,
                                                        CDiagResultsDbTestRecordHandle*& aHandle )
    {            
    RStoreReadStream handlereadstream;
    handlereadstream.OpenLC( *iStore, aId );
    
    CDiagResultsDbTestRecord* testrecord = CDiagResultsDbTestRecord::NewL( ETrue, 
	                                       TUid::Uid( aId.Value() ));
    CleanupStack::PushL ( testrecord );
    
    CDiagResultsDbTestRecordHandle* handle = 
                            CDiagResultsDbTestRecordHandle::NewL( handlereadstream );
                                                       
    CleanupStack::PushL ( handle );
    
    for ( TInt i = 0; i < handle->Count(); ++i )
        {
        RStoreReadStream readstream;
        readstream.OpenLC( *iStore, ((*handle)[i]) ); 
                
        testrecord->AddL( CDiagResultsDatabaseItem::NewL ( readstream ) );
        
        CleanupStack::PopAndDestroy( &readstream );  
        }

    CleanupStack::Pop(); //handle
    CleanupStack::Pop(); //testrecord
    
    aRecord = testrecord;
    aHandle = handle;
    
    CleanupStack::PopAndDestroy( &handlereadstream ); 
    
    
    }


// ---------------------------------------------------------------------------
// Retrieve all record infos from the store.
// ---------------------------------------------------------------------------
// 
RArray<TDiagResultsDatabaseTestRecordInfo>* CDiagResultsDbStore::ExistingRecordInfosL()
    {
    RArray<TStreamId> ids;
    CleanupClosePushL( ids );
    // Find available stream ids
    ReadRootStreamL( ids );
    
    RArray<TDiagResultsDatabaseTestRecordInfo>* infos = new (ELeave) 
                                            RArray<TDiagResultsDatabaseTestRecordInfo>;
    CleanupClosePushL( *infos );                                            
    
    
    for ( TInt i = 0; i < ids.Count(); ++i )
        {
        CDiagResultsDbTestRecordHandle* handle = OpenExistingHandleL( TUid::Uid( ids[i].Value() ));
      
        TDiagResultsDatabaseTestRecordInfo info = handle->RecordInfo();
        
        infos->Append( info );
        }  
        
    CleanupStack::PopAndDestroy( &ids );
    CleanupStack::Pop();
    
    return infos;
    }


// ---------------------------------------------------------------------------
// Open a test result from the DB.
// ---------------------------------------------------------------------------
// 
CDiagResultsDatabaseItem* CDiagResultsDbStore::OpenExistingTestResultL( TStreamId aId )
    {
    RStoreReadStream readstream;
    readstream.OpenLC( *iStore, aId ); 
    
    CDiagResultsDatabaseItem* item = CDiagResultsDatabaseItem::NewL ( readstream );
    
    CleanupStack::PopAndDestroy( &readstream );
    
    return item;
    }
    

// ---------------------------------------------------------------------------
// Open handle from the DB. Uid matches the handle's stream Id.
// ---------------------------------------------------------------------------
// 
CDiagResultsDbTestRecordHandle* CDiagResultsDbStore::OpenExistingHandleL( 
                                                    TUid aUid )
    {
    TStreamId id = TStreamId ( aUid.iUid );
    
    RStoreReadStream handlereadstream;
    handlereadstream.OpenLC( *iStore, id );
    
    
    CDiagResultsDbTestRecordHandle* handle = 
                            CDiagResultsDbTestRecordHandle::NewL( handlereadstream );
                            
    CleanupStack::PopAndDestroy( &handlereadstream );                           
    
    return handle;   
    }
    
// ---------------------------------------------------------------------------
// Cleanup Database if there are two many test records.    
// ---------------------------------------------------------------------------
// 
void CDiagResultsDbStore::CleanUpDatabaseL(TBool aCommit)
	{
	if ( iDeletionAlgorithm == EMaxRecordCountAlgorithm )
		{
		CleanUpDatabaseNoLastResultsL( aCommit );
		}
	else if ( iDeletionAlgorithm == ELastResultsAlgorithm )
		{
		CleanUpDatabaseUseLastResultsBufferL( aCommit );
		}
	else 
		{
		User::Panic( _L("Unknown Deletion Algorithm"), 
                DiagResultsDbCommon::EUnknownDeletionAlgorithm);  
		}
	}

// ---------------------------------------------------------------------------
// Cleanup Database if there are two many test records.    
// ---------------------------------------------------------------------------
// 
void CDiagResultsDbStore::CleanUpDatabaseNoLastResultsL(TBool aCommit)
    {    
    RArray<TStreamId> ids;
    CleanupClosePushL( ids );
    ReadRootStreamL( ids );
    
    //Fetch the maximum number of test records from Central Repository. Panic if
    //the value is not found. Also panic if value is less than zero.
    TInt maxsize = 0;
    TInt err = RDiagResultsDatabase::GetDatabaseMaximumSize( maxsize ) ;
    
    if ( err != KErrNone || maxsize < 0 )
        {
        User::Panic ( _L("Diag ResultsDB panic"), 
                    DiagResultsDbCommon::ECentralRepositoryFailure );
        }
            
    TBool cleanedUp = EFalse;   
        
    while ( ids.Count() > maxsize ) //Do cleanup
        {                
        DeleteOldestHandleL( ids );
                   
        cleanedUp = ETrue;
        }
      
    if ( cleanedUp && aCommit )
        {
        iStore->CommitL();       
        }      
      
    CleanupStack::PopAndDestroy( &ids );
    }


// ---------------------------------------------------------------------------
// Cleanup Database if there are two many test records.    
// ---------------------------------------------------------------------------
// 
void CDiagResultsDbStore::CleanUpDatabaseUseLastResultsBufferL(TBool aCommit)
    {    
    RArray<TStreamId> ids;
    CleanupClosePushL( ids );
    ReadRootStreamL( ids );

    //Fetch the maximum number of test records from Central Repository. Panic if
    //the value is not found. Also panic if value is less than zero.
    TInt maxsize = 0;
    TInt err = RDiagResultsDatabase::GetDatabaseMaximumSize( maxsize ) ;
    
    if ( err != KErrNone || maxsize < 0 )
        {
        User::Panic ( _L("Diag ResultsDB panic"), 
                    DiagResultsDbCommon::ECentralRepositoryFailure );
        }
            
    TBool cleanedUp = EFalse;   
        
    while ( ids.Count() > maxsize ) //Do cleanup
        {                     
        TInt count = ids.Count();
                   
        ///@@@KSR: changes for Codescanner error val = High 
        //CheckOverflowingTestResults( ids );
        CheckOverflowingTestResultsL( ids );
                                  
        cleanedUp = ETrue;
        
        __ASSERT_ALWAYS( ids.Count() == count - 1, User::Panic( _L("Diag ResultsDB panic"), 
                    DiagResultsDbCommon::EUnableToDelete ) );
        }
    
    if ( cleanedUp && aCommit )
        {
        iStore->CommitL();       
        }      
      
    CleanupStack::PopAndDestroy( &ids );
    }


// ---------------------------------------------------------------------------
// Load last results buffer and add test results if needed.
// 
// Test results are only moved to the last results buffer.
//
// We want to preserve only test results that have Pass/fail result.
//
// The algorithm.
//
// 1. Open the test record that is going to be removed (handle + test results)
// 2. Open last results buffer
// 3. Go through the test record and search for test results that have pass/fail
// 4. If found, then check last results buffer is there a similar result.
// 5. If similar test results are found from the last results buffer, delete the old
//    result and add the new one.
// 6. If similar test result is not found, then append the test result into the
//    buffer.
// 7. Finally update the last result buffer 
// 8. delete the test record and any of the test results that did not go into
//    the last results buffer.
// ---------------------------------------------------------------------------
//
//void CDiagResultsDbStore::CheckOverflowingTestResults( RArray<TStreamId>& aIds )
///@@@KSR: changes for Codescanner error val = High
void CDiagResultsDbStore::CheckOverflowingTestResultsL( RArray<TStreamId>& aIds )
    {    
    TStreamId id = aIds[0];
    
    CDiagResultsDbTestRecordHandle* handleToBeRemoved = NULL;
    CDiagResultsDbTestRecord* recordToBeRemoved = NULL;
    
    
    OpenExistingRecordAndHandleL( id, 
                                  recordToBeRemoved,
                                  handleToBeRemoved );
    
    CleanupStack::PushL( handleToBeRemoved );
    CleanupStack::PushL( recordToBeRemoved );
  
    
    CDiagResultsDbTestRecordHandle*  lastResultsBufferHandle = NULL;
                                                                                                         
    lastResultsBufferHandle = OpenExistingHandleL( TUid::Uid( KFirstStreamId ) );
          
    TInt testCount = recordToBeRemoved->Count();
   
    TInt movedCount = 0;
   
    CleanupStack::PushL( lastResultsBufferHandle );                                  
                                             
    for ( TInt x = 0; x < recordToBeRemoved->Count(); ++x )
        {
        
        CDiagResultsDatabaseItem* item = recordToBeRemoved->GetItem(x);
        
        //We are looking for only results that are either ESuccess or
        //EFailed.
        if ( item->TestResult() == CDiagResultsDatabaseItem::ESuccess ||
             item->TestResult() == CDiagResultsDatabaseItem::EFailed )
            {            
            TBool found = lastResultsBufferHandle->Find( item->TestUid() );
            TInt  index = lastResultsBufferHandle->FindIndex( item->TestUid() );
            
            CDiagResultsDbTestRecordHandle::TTestResultHandle resulthandle = 
                         handleToBeRemoved->Get( handleToBeRemoved->FindIndex( item->TestUid() ));
                                                
            if ( resulthandle.iTestUid != item->TestUid() ) 
                {
                User::Panic( _L("Diag ResultsDB panic"), DiagResultsDbCommon::EIncorrectStreamId);
                }
                                                        
            if ( found )
                {                                
                //remove old result from last results buffer, because we add new one.
                iStore->DeleteL ( lastResultsBufferHandle->Get(index).iStreamId );
                                
                lastResultsBufferHandle->RemoveL( item->TestUid() );
                lastResultsBufferHandle->AddL( resulthandle );                               
                }
            else 
                {
                lastResultsBufferHandle->AddL( resulthandle );                              
                }
                
            movedCount++;
            
            handleToBeRemoved->RemoveL( item->TestUid() );    
                
            }                                             
        }
            
    WriteHandleIntoDbL( *lastResultsBufferHandle );
                    
    iStore->DeleteL( aIds[0] ); //delete oldest first
    aIds.Remove( 0 );
        
    ReplaceRootStreamL( aIds );
                
    //Delete test results streams as well.    
    for (TInt i=0; i < handleToBeRemoved->Count(); ++i)
        {
        iStore->DeleteL ( handleToBeRemoved->Get(i).iStreamId );
        movedCount++;
        }        
        
    if ( movedCount != testCount )    
        {
        User::Panic( _L("EIncorrectStreamId"), DiagResultsDbCommon::EIncorrectStreamId);
        }
        
    CleanupStack::PopAndDestroy( lastResultsBufferHandle );
    CleanupStack::PopAndDestroy( recordToBeRemoved );
    CleanupStack::PopAndDestroy( handleToBeRemoved );       
    }


// ---------------------------------------------------------------------------
// Deletes the oldest handle from the store.
// ---------------------------------------------------------------------------
//  
void CDiagResultsDbStore::DeleteOldestHandleL( RArray<TStreamId>& aIds )
    {        
    CDiagResultsDbTestRecordHandle* handle = OpenExistingHandleL( 
                                                TUid::Uid( aIds[0].Value() ) );
    CleanupStack::PushL( handle );
        
    iStore->DeleteL( aIds[0] ); //delete oldest first
    aIds.Remove( 0 );
        
    ReplaceRootStreamL( aIds );
                
    //Delete test results streams as well.    
    for (TInt i=0; i < handle->Count(); ++i)
        {
        iStore->DeleteL ( handle->Get(i).iStreamId );
        }
                              
    CleanupStack::PopAndDestroy( handle );
    }


// ---------------------------------------------------------------------------
// Delete handle and its test results.
// ---------------------------------------------------------------------------
// 
void CDiagResultsDbStore::DeleteHandleL( TInt aIndex, RArray<TStreamId>& aIds )
    {        
    CDiagResultsDbTestRecordHandle* handle = OpenExistingHandleL( 
                                                TUid::Uid( aIds[aIndex].Value() ) );
    CleanupStack::PushL( handle );
        
    iStore->DeleteL( aIds[aIndex] ); 
    aIds.Remove( aIndex );
        
    ReplaceRootStreamL( aIds );
                
    //Delete test results streams as well.    
    for (TInt i=0; i < handle->Count(); ++i)
        {
        iStore->DeleteL ( handle->Get(i).iStreamId );
        }
                              
    CleanupStack::PopAndDestroy( handle );
    }


// ---------------------------------------------------------------------------
// Reads the root stream from the DB. Root stream contains uids of the 
// test records. TUid is convertible to TStreamId and opposite.
// ---------------------------------------------------------------------------
//             
void CDiagResultsDbStore::ReadRootStreamL( RArray<TStreamId>& aIds )
    {    
    
    if (!iStore)
        {
        User::Panic( _L("Store is null"), 
                        DiagResultsDbCommon::EStoreNullPointer );
        }
    
    RStoreReadStream readstream;
 
    TStreamId rootId = iStore->Root();
    readstream.OpenLC( *iStore, rootId );
    
    TInt count = readstream.ReadInt16L();
        
    for ( TInt i = 0; i < count; ++i )
        {
        TStreamId id;
        id.InternalizeL( readstream );
        aIds.AppendL ( id  );
               
        }
    
    CleanupStack::PopAndDestroy( &readstream );
    }
  
// ---------------------------------------------------------------------------
// Add one stream Id into the root stream.
// ---------------------------------------------------------------------------
//    
void CDiagResultsDbStore::AppendRootStreamL( TStreamId& aId )
    {    
    RArray<TStreamId> ids;
    CleanupClosePushL( ids );
    
    ReadRootStreamL( ids );
    
    ids.Append( aId );
    
    ReplaceRootStreamL( ids );

    CleanupStack::PopAndDestroy( &ids);
    }

// ---------------------------------------------------------------------------
// Returns TUids from the root stream.
// ---------------------------------------------------------------------------
//     
void CDiagResultsDbStore::RecordUidsL(RArray<TUid>& aUids)
    {
    RArray<TStreamId> ids;
    CleanupClosePushL ( ids );
 
    ReadRootStreamL( ids );
    
    for (TInt i = 0; i < ids.Count(); ++i)
        {
        aUids.Append( TUid::Uid( ids[i].Value() ));
        }
        
    CleanupStack::PopAndDestroy( &ids );
    }

        
// ---------------------------------------------------------------------------
// Replace the current root stream with a new array.
// ---------------------------------------------------------------------------
//     
void CDiagResultsDbStore::ReplaceRootStreamL( RArray<TStreamId>& aArray )
    {
    RStoreWriteStream writestream;
    writestream.ReplaceLC( *iStore, iStore->Root() );    
        
    TInt count =  aArray.Count();    
        
    writestream.WriteInt16L(  count );
    
    for ( TInt i = 0; i < count; ++i )
        {
        writestream << aArray[i];
        }
        
    writestream.CommitL();
       
    CleanupStack::PopAndDestroy( &writestream );
    }     

// ---------------------------------------------------------------------------
// Empty root stream is needed when DB file is created for the first time.
// ---------------------------------------------------------------------------
// 
void CDiagResultsDbStore::CreateEmptyRootStreamL(CPermanentFileStore& aStore)
    {
    RStoreWriteStream writestream;
   
    TStreamId id = writestream.CreateLC( *iStore );  
    writestream.WriteInt16L( 0 ); // stream id count = 0
    
    writestream.CommitL();  
    
    aStore.SetRootL( id );
    
    aStore.CommitL();
    
    CleanupStack::PopAndDestroy( &writestream );
    }
        
 
