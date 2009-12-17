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
* Description:  Class definition of RDiagResultsDatabase
*
*/



// USER INCLUDE FILES

#include "diagresultsdatabase.h"
#include "diagresultsdatabaseitem.h"
#include "diagresultsdbrecordinfoarraypacked.h"
#include "diagresultsdatabasetestrecordinfo.h"
#include "diagresultsdbprivatecrkeys.h"
#include "diagresultsdbrecordengineparam.h"
#include "diagresultsdatabasecommon.h"
//#include <diagframeworkdebug.h>
// SYSTEM INCLUDE FILES
#include <s32mem.h> 
#include <centralrepository.h> 

const TInt KResultsDatabaseBufferLength = 0x700;
const TInt KResultsDatabaseSubsessionBufferLength = 0x250;
const TInt KResultsDatabaseGranuality = 50;

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// Constructor.
// ---------------------------------------------------------------------------
//
EXPORT_C RDiagResultsDatabase::RDiagResultsDatabase(): 
         iBuffer(NULL), iOpen(EFalse)
       , iPtr( NULL, 0 )
    {
    
    }

// ---------------------------------------------------------------------------
// Destructor.
// ---------------------------------------------------------------------------
//
EXPORT_C RDiagResultsDatabase::~RDiagResultsDatabase()
    {
    if ( iBuffer )
        {
        iBuffer->Reset();
        delete iBuffer;
        iBuffer = NULL;    
        }
    }

// ---------------------------------------------------------------------------
// Create connection to the results DB server
// ---------------------------------------------------------------------------
//
EXPORT_C TInt RDiagResultsDatabase::Connect( TUid aAppUid )
    {
    LOGME("RDiagResultsDatabase::Connect");
	TRAPD( err, DoConnectL( aAppUid));
	
	return err;
    }

// ---------------------------------------------------------------------------
// Start server, create flat buffer and send request.
// ---------------------------------------------------------------------------
//
void RDiagResultsDatabase::DoConnectL (TUid aAppUid)
    {
    LOGME("RDiagResultsDatabase::DoConnectL");
    if (iBuffer==NULL)
		{
		iBuffer = CBufFlat::NewL(KResultsDatabaseGranuality);
		iBuffer->ResizeL(KResultsDatabaseBufferLength);
		if (iBuffer==NULL)
			User::Leave( KErrNoMemory );
		}
    
    TInt r = DiagResultsDbCommon::StartServer();
    LOGME1("RDiagResultsDatabase::StartServer %d",r);
	if( r==KErrNone )
	    // Use default message slots
		r = CreateSession( KDiagResultsDatabaseServerName, Version() );
	LOGME1("RDiagResultsDatabase::Createsession %d",r); 
	if ( r != KErrNone )
	    {
	    User::Leave (r);
	    }
	
	iOpen = ETrue;
	
    TPckgBuf<TUid> uid( aAppUid );
    TInt srret = SendReceive (DiagResultsDbCommon::EConnect,TIpcArgs(&uid) );
    LOGME1("Rsession sendreceive %d",srret);
    if (srret != KErrNone)
	User::Leave( srret );
    }

// ---------------------------------------------------------------------------
// Close connection with the server.
// ---------------------------------------------------------------------------
//
EXPORT_C TInt RDiagResultsDatabase::Close()
    {
    LOGME("RDiagResultsDatabase::Close");
    if ( iOpen )
        {
        TInt err = SendReceive( DiagResultsDbCommon::EClose );
        RHandleBase::Close();
        iOpen = EFalse;
        delete iBuffer;
        iBuffer = NULL;
        return err;    
        }
    else 
        {
        iOpen = EFalse;
        return KErrNone;
        }
    }

// ---------------------------------------------------------------------------
// Returns the version of this server.
// ---------------------------------------------------------------------------
//
EXPORT_C TVersion RDiagResultsDatabase::Version() const
    {
    return( TVersion( KDiagResultsDatabaseServerMajor, 
                      KDiagResultsDatabaseServerMinor,
		              KDiagResultsDatabaseServerBuild ) );
    }

// ---------------------------------------------------------------------------
// Returns test record count.
// ---------------------------------------------------------------------------
//
EXPORT_C TInt RDiagResultsDatabase::GetRecordCount( TUint& aCount ) const
    {
	TPckgBuf<TUint> pckg;
	TInt error = SendReceive( DiagResultsDbCommon::EGetRecordCount, TIpcArgs(&pckg) );
	aCount = pckg();
	
	return error;
    }


// ---------------------------------------------------------------------------
// Fetch maximum number of test records from the central repository.
// ---------------------------------------------------------------------------
//
EXPORT_C TInt RDiagResultsDatabase::GetDatabaseMaximumSize( TInt& aMaxSize )
    {
    TRAPD( err, DoGetDatabaseMaximumSizeL( aMaxSize ) );
    return err;
    }

// ---------------------------------------------------------------------------
// Fetch maximum number of test records from the central repository.
// ---------------------------------------------------------------------------
//
void RDiagResultsDatabase::DoGetDatabaseMaximumSizeL( TInt& aMaxSize )
    {
    CRepository* cr = CRepository::NewLC( KCRUidDiagnosticsResults );
        
    User::LeaveIfError( cr->Get( KDiagDatabaseMaxRecordCount, aMaxSize ) );
    
    CleanupStack::PopAndDestroy( cr );
    }

// ---------------------------------------------------------------------------
// Get available record uids.
// ---------------------------------------------------------------------------
//
EXPORT_C TInt RDiagResultsDatabase::GetRecordUids( 
            CArrayFixFlat<TUid>& aSortedRecordUidArray ) const
    {
    TRAPD( err, DoGetRecordUidsL( aSortedRecordUidArray ));
    return err;
    }

// ---------------------------------------------------------------------------
// Resize buffer and then start query. Overflow means that the buffer was not
// large enough.
// ---------------------------------------------------------------------------
//    
void RDiagResultsDatabase::DoGetRecordUidsL( 
                           CArrayFixFlat<TUid>& aSortedRecordUidArray ) const
    {
    iBuffer->Delete( 0, iBuffer->Size() );
    iBuffer->ResizeL( KResultsDatabaseBufferLength );
    
    TPtr8 ptr ( iBuffer->Ptr(0) );
    //Now ask server to serialize object into this buffer
    TInt ret = SendReceive( DiagResultsDbCommon::EGetRecordList, 
                            TIpcArgs(&ptr) );
    
    while ( ret == KErrOverflow )
        {
        iBuffer->ResizeL( iBuffer->Size() + KResultsDatabaseBufferLength );
        ptr.Set( iBuffer->Ptr(0) );
        ret = SendReceive( DiagResultsDbCommon::EGetRecordList, 
                           TIpcArgs(&ptr) );
        }
    if ( ret != KErrNone )
        {
        User::Leave (ret);
        }
        
    RBufReadStream readStream ( *iBuffer );   
    CleanupClosePushL( readStream ) ;
    TInt count = readStream.ReadInt16L();
    
    for ( TInt i = 0; i < count; ++i )
        {
        aSortedRecordUidArray.AppendL ( TUid::Uid( readStream.ReadInt32L()) );
        }
   
   CleanupStack::PopAndDestroy( &readStream );
    }    

// ---------------------------------------------------------------------------
// Returns last completed record.
// ---------------------------------------------------------------------------
//
EXPORT_C TInt RDiagResultsDatabase::GetLastRecord( TUid& aRecordUid ) const
    {
    TPckgBuf<TUid> pckg;
	TInt err = SendReceive( DiagResultsDbCommon::EGetLastRecord, 
	                        TIpcArgs(&pckg) );
	aRecordUid = pckg();
	return err;
    }
    

// ---------------------------------------------------------------------------
// Get last incomplete record. It could suspended or application crashed
// when running the test.
// ---------------------------------------------------------------------------
//       
EXPORT_C TInt RDiagResultsDatabase::GetLastNotCompletedRecord ( 
                                                    TUid& aRecordUid ) const
    {
    TPckgBuf<TUid> pckg;
    TInt err = SendReceive( DiagResultsDbCommon::EGetLastNotCompletedRecord, 
	                        TIpcArgs(&pckg) );
	aRecordUid = pckg();
	return err;
                                                        
    }

// ---------------------------------------------------------------------------
// Get all test record overviews.
// ---------------------------------------------------------------------------
//
EXPORT_C TInt RDiagResultsDatabase::GetAllRecordInfos ( 
            CArrayFixFlat<TDiagResultsDatabaseTestRecordInfo>& aInfoArray) 
    {
    TRAPD( err, DoGetAllRecordInfosL( aInfoArray ));
    return err;
    }

// ---------------------------------------------------------------------------
// Send client request to the server. If overflow, then resize buffer and
// try again. 
// ---------------------------------------------------------------------------
//
void RDiagResultsDatabase::DoGetAllRecordInfosL ( 
            CArrayFixFlat<TDiagResultsDatabaseTestRecordInfo>& aInfoArray) 
    {
    iBuffer->Delete( 0, iBuffer->Size() );
    iBuffer->ResizeL( KResultsDatabaseBufferLength );
    
    TPtr8 ptr ( iBuffer->Ptr(0) );
    
    TInt ret = SendReceive( DiagResultsDbCommon::EGetRecordInfoList, 
                            TIpcArgs(&ptr) );
    
    while ( ret == KErrOverflow )
        {
        iBuffer->ResizeL( iBuffer->Size() + KResultsDatabaseBufferLength );
        ptr.Set( iBuffer->Ptr(0) );
        ret = SendReceive( DiagResultsDbCommon::EGetRecordInfoList, 
                           TIpcArgs(&ptr) );
        }
    
    if ( ret != KErrNone )
        {
        User::Leave (ret);
        }
            
    TDiagResultsDbRecordInfoArrayPacked packedArray( iBuffer );    
    packedArray.UnpackArrayL( aInfoArray );
    }

// ---------------------------------------------------------------------------
// Asynchronous get last results.
// ---------------------------------------------------------------------------
//
EXPORT_C void RDiagResultsDatabase::InitiateGetLastResults ( 
                                          const CArrayFixFlat<TUid>& aUidArray,
                                          TRequestStatus& aStatus )
    {    
    TRAPD( error, WriteArrayIntoBufferL( aUidArray ));
    
    if ( error != KErrNone )
        {         
        TRequestStatus* status = &aStatus;
        User::RequestComplete( status, error );
        return;
        }
    
            
    iPtr.Set( iBuffer->Ptr(0) );
    
	SendReceive( DiagResultsDbCommon::EInitiateGetLastResults, 
	             TIpcArgs( &iPtr ), 
	             aStatus);
    }

// ---------------------------------------------------------------------------
// Write Array into Buffer:
// ---------------------------------------------------------------------------
//    
void RDiagResultsDatabase::WriteArrayIntoBufferL( const CArrayFixFlat<TUid>& aUidArray )
    {
    iBuffer->Delete( 0, iBuffer->Size() );
    iBuffer->ResizeL( KResultsDatabaseBufferLength );     

    RBufWriteStream writeStream ( *iBuffer );
    CleanupClosePushL( writeStream );
        
    writeStream.WriteInt8L( aUidArray.Count() );
    
    for ( TInt i=0; i < aUidArray.Count(); ++i )
        {
        writeStream.WriteInt32L( aUidArray[i].iUid );
        }
     
    writeStream.CommitL();  
    
    CleanupStack::PopAndDestroy();
    }
    
// ---------------------------------------------------------------------------
// Cancel asynchronous request.
// ---------------------------------------------------------------------------
//
EXPORT_C void RDiagResultsDatabase::CancelInitiateGetLastResults () const
    {
    SendReceive( DiagResultsDbCommon::ECancelInitiateGetLastResults );
    }

// ---------------------------------------------------------------------------
// See InitiateGetLastResults. After InitiateGetLastResults finishes ok, 
// this method can be called to retrieve data.
// ---------------------------------------------------------------------------
//    
EXPORT_C TInt RDiagResultsDatabase::GetLastResults ( 
                    RPointerArray<CDiagResultsDatabaseItem>& aResults ) const
    {
    TRAPD( err, DoGetLastResultsL( aResults ) );
    return err;
    }

// ---------------------------------------------------------------------------
// Resize buffer and make the request. Data is serialized in the buffer so it 
// must be read using a read stream.
// ---------------------------------------------------------------------------
//
void RDiagResultsDatabase::DoGetLastResultsL ( 
                    RPointerArray<CDiagResultsDatabaseItem>& aResults ) const
    {
    iBuffer->Delete( 0, iBuffer->Size() );
    iBuffer->ResizeL( KResultsDatabaseBufferLength );
    
    TPtr8 ptr ( iBuffer->Ptr(0));
    //Now ask server to serialize object into this buffer
    TInt ret = SendReceive( DiagResultsDbCommon::EGetLastResults, 
                            TIpcArgs(&ptr) );
    
    while ( ret == KErrOverflow )
        {
        iBuffer->ResizeL( iBuffer->Size() + KResultsDatabaseBufferLength );
        ptr.Set ( iBuffer->Ptr(0) );
        ret = SendReceive( DiagResultsDbCommon::EGetLastResults, 
                           TIpcArgs(&ptr) );
        }
        
    if ( ret != KErrNone )
        {
        User::Leave (ret);
        }
        
    RBufReadStream readStream ( *iBuffer );    
    TInt count = readStream.ReadInt16L();
    
    for ( TInt i = 0; i < count; ++i )
        {
        
        TUint8 value = readStream.ReadUint8L();
        if ( value == 0 )
            {
            aResults.AppendL( NULL );
            }
        else 
            {
            aResults.AppendL (CDiagResultsDatabaseItem::NewL( readStream ) );
            }
        }
        
    readStream.Close();    
    }


// ---------------------------------------------------------------------------
// Initiate asynchronous operation. 
// ---------------------------------------------------------------------------
//
EXPORT_C void RDiagResultsDatabase::InitiateGetLastResult ( TUid aTestPluginUid, 
                                      TRequestStatus& aStatus )
    {
    TPckgBuf<TUid> uidpckg( aTestPluginUid );
    
    SendReceive( DiagResultsDbCommon::EInitiateGetSingleLastResult, 
	             TIpcArgs( &uidpckg ), 
	             aStatus);
    }
    

// ---------------------------------------------------------------------------
// Get last result that was fetched using InitiateGetLastResult method.
// ---------------------------------------------------------------------------
//
EXPORT_C TInt RDiagResultsDatabase::GetLastResult ( 
                                       CDiagResultsDatabaseItem*& aItem )    
    {
    
    TRAPD( err, DoGetLastResultL( aItem ) );
    return err;            
    }
    

// ---------------------------------------------------------------------------
// Leaving version of the function.
// ---------------------------------------------------------------------------
//    
void RDiagResultsDatabase::DoGetLastResultL ( 
                                        CDiagResultsDatabaseItem*& aItem )    
    {    
    iBuffer->Delete( 0, iBuffer->Size() );
    iBuffer->ResizeL( KResultsDatabaseBufferLength );
     	              
    TPtr8 ptr ( iBuffer->Ptr(0));
    //Now ask server to serialize object into this buffer
    TInt ret = SendReceive( DiagResultsDbCommon::EGetSingleLastResult, 
                            TIpcArgs(&ptr) );
    
    while ( ret == KErrOverflow )
        {
        iBuffer->ResizeL( iBuffer->Size() + KResultsDatabaseBufferLength );
        ptr.Set ( iBuffer->Ptr(0) );
        ret = SendReceive( DiagResultsDbCommon::EGetSingleLastResult, 
                           TIpcArgs(&ptr) );
        }
        
    if ( ret != KErrNone )
        {
        User::Leave (ret);
        }
        
    RBufReadStream readStream ( *iBuffer );  
    
    TInt count = readStream.ReadInt8L();  
   
    if ( count == 0 )
        {
        aItem = NULL;
        }
    else 
        {
        aItem = CDiagResultsDatabaseItem::NewL ( readStream );    
        }
          
    readStream.Close();
    }
    
// ---------------------------------------------------------------------------
// Subsession constructor.
// ---------------------------------------------------------------------------
//
EXPORT_C RDiagResultsDatabaseRecord::RDiagResultsDatabaseRecord(): 
                                        iBuffer(NULL), 
                                        iOpen(EFalse)
                                        ,iPtr( NULL, 0 )
    {
    }

// ---------------------------------------------------------------------------
// Subsession destructor.
// ---------------------------------------------------------------------------
//
EXPORT_C RDiagResultsDatabaseRecord::~RDiagResultsDatabaseRecord()
    {
     if ( iBuffer )
        {
        iBuffer->Reset();
        delete iBuffer;
        iBuffer = NULL;
        }
    }

// ---------------------------------------------------------------------------
// Checks if connection is already open.
// ---------------------------------------------------------------------------
//
EXPORT_C TInt RDiagResultsDatabaseRecord::Connect( 
            RDiagResultsDatabase& aSession,
            TUid aRecordId,
            TBool aReadOnly ) 
    {
    LOGME("RDiagResultsDatabaseRecord::Connect");
    if ( iOpen )
        {
        return KErrAlreadyExists;
        }
    
    TRAPD( err, DoConnectL( aSession, aRecordId, aReadOnly ) );
    return err;
    }

// ---------------------------------------------------------------------------
// Creates the flat buffer and connects the subsession to a test record.
// The test record must exist.
// ---------------------------------------------------------------------------
//
void RDiagResultsDatabaseRecord::DoConnectL( 
            RDiagResultsDatabase& aSession,
            TUid aRecordId,
            TBool aReadOnly ) 
    {
    LOGME("RDiagResultsDatabaseRecord::DoConnectL");
    if (iBuffer==NULL)
		{
		iBuffer = CBufFlat::NewL(KResultsDatabaseGranuality);
		iBuffer->ResizeL(KResultsDatabaseSubsessionBufferLength);
		if (iBuffer==NULL)
			User::Leave( KErrNoMemory );
		}
    
    iOpen = ETrue;
    
    TPckgBuf<TUid> uid( aRecordId );
    TPckgBuf<TBool> readonlyPckg( aReadOnly );
    User::LeaveIfError( CreateSubSession( aSession, 
                                      DiagResultsDbCommon::EConnectSubsession, 
                                      TIpcArgs(&uid, &readonlyPckg) ));
    }


// ---------------------------------------------------------------------------
// Create a new subsession and create a new test record.
// ---------------------------------------------------------------------------
//
EXPORT_C TInt RDiagResultsDatabaseRecord::CreateNewRecord (
                                            RDiagResultsDatabase& aSession, 
                                            TUid& aRecordId,
                                            CDiagResultsDbRecordEngineParam& aEngineParam )
    {
    if ( iOpen )
        {
        return KErrAlreadyExists;
        }
        
    TRAPD( err, DoCreateNewRecordL( aSession, aRecordId, aEngineParam) );
    return err;
    }

// ---------------------------------------------------------------------------
// Get parameters that are needed when resuming test session.
// ---------------------------------------------------------------------------
//
EXPORT_C TInt RDiagResultsDatabaseRecord::GetEngineParam( 
                        CDiagResultsDbRecordEngineParam*& aEngineParam ) const
    {
    TRAPD(err, DoGetEngineParamL( aEngineParam ) );
    return err;
    }


// ---------------------------------------------------------------------------
// Leaving version of GetEngineParam. Serialize EngineParam class.
// ---------------------------------------------------------------------------
//
void RDiagResultsDatabaseRecord::DoGetEngineParamL( 
                        CDiagResultsDbRecordEngineParam*& aEngineParam ) const
    {
    iBuffer->Delete( 0, iBuffer->Size() );
    iBuffer->ResizeL( KResultsDatabaseSubsessionBufferLength );
	
    TPtr8 ptr ( iBuffer->Ptr(0) );
    //Now ask server to serialize object into this buffer
    TInt ret = SendReceive( DiagResultsDbCommon::ESubsessionGetEngineParam, 
                                                 TIpcArgs(&ptr) );
    
    while ( ret == KErrOverflow )
        {
        iBuffer->ResizeL( iBuffer->Size() 
                                + KResultsDatabaseSubsessionBufferLength );
        ptr.Set( iBuffer->Ptr(0) );                                
        ret = SendReceive( DiagResultsDbCommon::ESubsessionGetEngineParam, 
                           TIpcArgs(&ptr) );
        }
    
    if ( ret != KErrNone )
        {
        User::Leave (ret);
        }
     
    //Stream contains the serialized array
    RBufReadStream readStream ( *iBuffer );
    CleanupClosePushL ( readStream );
    
    //Construct new test result.
    CDiagResultsDbRecordEngineParam* params = 
                                CDiagResultsDbRecordEngineParam::NewL( readStream );
    aEngineParam = params;
   
    CleanupStack::PopAndDestroy( &readStream );
    
    iBuffer->Delete(0, iBuffer->Size() );      
    }     
// ---------------------------------------------------------------------------
// Create the flat buffer and create the subsession.
// ---------------------------------------------------------------------------
//
void RDiagResultsDatabaseRecord::DoCreateNewRecordL (
            RDiagResultsDatabase& aSession,
            TUid& aRecordId,
            CDiagResultsDbRecordEngineParam& aEngineParam )
    {
     if (iBuffer==NULL)
		{
		iBuffer = CBufFlat::NewL(KResultsDatabaseGranuality);
		iBuffer->ResizeL(KResultsDatabaseSubsessionBufferLength);
		if (iBuffer==NULL)
			User::Leave( KErrNoMemory );
		}
        
    iBuffer->Delete( 0, iBuffer->Size() );
    iBuffer->ResizeL( KResultsDatabaseSubsessionBufferLength );
    
    TPtr8 ptr ( iBuffer->Ptr(0) );    
        
    TRAPD( error, WriteEngineParamIntoBufferL( aEngineParam ) );
     
    while ( error == KErrOverflow )
        {
        iBuffer->ResizeL( iBuffer->Size() + KResultsDatabaseSubsessionBufferLength ); 
        ptr.Set( iBuffer->Ptr(0) );
        TRAP( error, WriteEngineParamIntoBufferL( aEngineParam) );
        }
        
    User::LeaveIfError ( error );
    
    TPckgBuf<TUid> pckg;
    
    TInt err = CreateSubSession( aSession, 
                              DiagResultsDbCommon::ESubsessionCreateNewRecord, 
                              TIpcArgs(&pckg, &ptr) );
    aRecordId = pckg();
    if ( err == KErrNone )
        {
        iOpen = ETrue;
        }
        
    User::LeaveIfError( err );
    }

// ---------------------------------------------------------------------------
// Write engine parameters into the buffer.
// ---------------------------------------------------------------------------
//
void RDiagResultsDatabaseRecord::WriteEngineParamIntoBufferL( 
                CDiagResultsDbRecordEngineParam& aEngineParam ) 
    {
    RBufWriteStream writeStream ( *iBuffer );
    CleanupClosePushL( writeStream );
    
    aEngineParam.ExternalizeL( writeStream );
    
    writeStream.CommitL();
    CleanupStack::PopAndDestroy();    
    }

// ---------------------------------------------------------------------------
// Close the subsession.
// ---------------------------------------------------------------------------
//
EXPORT_C void RDiagResultsDatabaseRecord::Close()
    {
    if ( iOpen )
        {
        CloseSubSession( DiagResultsDbCommon::ECloseSubsession );    
        }
        
    iOpen = EFalse;
    }

// ---------------------------------------------------------------------------

// See ResumeTestRecord.
// ---------------------------------------------------------------------------
//
EXPORT_C TInt RDiagResultsDatabaseRecord::Suspend()
    {
    return SendReceive ( DiagResultsDbCommon::ESubsessionSuspend );
    }
    
// ---------------------------------------------------------------------------
// Has this test record been suspended. 
// ---------------------------------------------------------------------------
//  
EXPORT_C TInt RDiagResultsDatabaseRecord::IsSuspended( TBool& aSuspended ) const
    {
    TPckgBuf<TBool> pckg;
	TInt err = SendReceive( DiagResultsDbCommon::ESubsessionIsSuspended, 
	                        TIpcArgs(&pckg) );
	aSuspended = pckg();
	return err;
    }

// ---------------------------------------------------------------------------
// Returns test record Id that this subsession represents.
// ---------------------------------------------------------------------------
//
EXPORT_C TInt RDiagResultsDatabaseRecord::GetTestRecordId( 
                                                TUid& aRecordUid ) const
    {
    TPckgBuf<TUid> pckg;
	TInt err = SendReceive( DiagResultsDbCommon::ESubsessionGetTestRecordId, 
	                        TIpcArgs(&pckg) );
	aRecordUid = pckg();
	return err;
    }


// ---------------------------------------------------------------------------
// Test record completion. 
// ---------------------------------------------------------------------------
//
EXPORT_C TInt RDiagResultsDatabaseRecord::TestCompleted( 
                                                TBool aFullyComplete )
    {
    TPckgBuf<TBool> pckg (aFullyComplete);
    return SendReceive ( DiagResultsDbCommon::ESubsessionTestCompleted, 
                         TIpcArgs(&pckg) );
    }

// ---------------------------------------------------------------------------
// GetStatus returns current record status.
// ---------------------------------------------------------------------------
//
EXPORT_C TInt RDiagResultsDatabaseRecord::GetStatus( 
                                        TRecordStatus& aRecordStatus ) const
    {
    TPckgBuf<TRecordStatus> pckg;
	TInt err = SendReceive( DiagResultsDbCommon::ESubsessionGetStatus, 
	                        TIpcArgs(&pckg) );
	aRecordStatus = pckg();
	return err;
    }

// ---------------------------------------------------------------------------
// Indicates has the subsession been written into the DB file.
// ---------------------------------------------------------------------------
//
EXPORT_C TInt RDiagResultsDatabaseRecord::IsTestCompleted( 
                                                    TBool& aCompleted ) const
    {
    TPckgBuf<TBool> pckg;
	TInt err = SendReceive( DiagResultsDbCommon::ESubsessionIsTestCompleted, 
	                        TIpcArgs(&pckg) );
	aCompleted = pckg();
	return err;
    }


// ---------------------------------------------------------------------------
// Returns record overview.
// ---------------------------------------------------------------------------
//
EXPORT_C TInt RDiagResultsDatabaseRecord::GetRecordInfo ( 
        TDiagResultsDatabaseTestRecordInfo& aInfo ) const
    {
    TPckgBuf<TDiagResultsDatabaseTestRecordInfo> pckg;
	TInt err = SendReceive( DiagResultsDbCommon::ESubsessionGetRecordInfo, 
	                        TIpcArgs(&pckg) );
	aInfo = pckg();
	return err;
    }

// ---------------------------------------------------------------------------
// Get all test plug-in uids that are stored in the test record. 
// ---------------------------------------------------------------------------
//
EXPORT_C TInt RDiagResultsDatabaseRecord::GetTestUids ( 
                                    CArrayFixFlat<TUid>& aTestUidArray ) const
    {    
    TRAPD( err, DoGetTestUidsL( aTestUidArray ));
    return err;
    }

// ---------------------------------------------------------------------------
// Resize flat buffer, make the request. If flat buffer is not large enough
// grow the size and make the request again.
// ---------------------------------------------------------------------------
//
void RDiagResultsDatabaseRecord::DoGetTestUidsL ( 
                                    CArrayFixFlat<TUid>& aTestUidArray ) const
    {
    iBuffer->Delete( 0, iBuffer->Size() );
    iBuffer->ResizeL( KResultsDatabaseSubsessionBufferLength );
    
    TPtr8 ptr ( iBuffer->Ptr(0) );
    //Now ask server to serialize object into this buffer
    TInt ret = SendReceive( DiagResultsDbCommon::ESubsessionGetTestUids, 
                            TIpcArgs(&ptr) );
    
    while ( ret == KErrOverflow )
        {
        iBuffer->ResizeL( iBuffer->Size() + 
                                KResultsDatabaseSubsessionBufferLength );
        ptr.Set( iBuffer->Ptr(0) );                                
        ret = SendReceive( DiagResultsDbCommon::ESubsessionGetTestUids, 
                           TIpcArgs(&ptr) );
        }
    if ( ret != KErrNone )
        {
        User::Leave (ret);
        }
        
    RBufReadStream readStream ( *iBuffer );  
    CleanupClosePushL( readStream );
    TInt count = readStream.ReadInt16L();
    
    for ( TInt i = 0; i < count; ++i )
        {
        aTestUidArray.AppendL ( TUid::Uid( readStream.ReadInt32L() ));
        }
   
    CleanupStack::PopAndDestroy( &readStream );
    } 

// ---------------------------------------------------------------------------
// Store one test result into the test record. The method does not write 
// anything.
// ---------------------------------------------------------------------------
//
EXPORT_C TInt RDiagResultsDatabaseRecord::LogTestResult ( 
            TRequestStatus& aStatus,  
            const CDiagResultsDatabaseItem& aResultItem )
    {
    TRAPD(err, DoLogTestResultL( aStatus, aResultItem ) );
    return err;
    }


// ---------------------------------------------------------------------------
// Cancel LogTestResultL.
// ---------------------------------------------------------------------------
//
EXPORT_C void RDiagResultsDatabaseRecord::CancelLogTestResult() const
    {
    SendReceive( DiagResultsDbCommon::ESubsessionCancelLogTestResult );
    }

// ---------------------------------------------------------------------------
// Resize flat buffer and serialize the object that needs to be transferred.
// ---------------------------------------------------------------------------
//
void RDiagResultsDatabaseRecord::DoLogTestResultL ( 
            TRequestStatus& aStatus, 
            const CDiagResultsDatabaseItem& aResultItem )
    {
    iBuffer->Delete( 0, iBuffer->Size() );
    
    //Make sure that the buffer is big enough for the item.
    iBuffer->ResizeL( KResultsDatabaseSubsessionBufferLength );  
                                               
    TRAPD( error, WriteDatabaseItemIntoBufferL( aResultItem ));
    
    while ( error == KErrOverflow )
        {
        iBuffer->ResizeL( iBuffer->Size() + aResultItem.Size() ); 
        TRAP( error, WriteDatabaseItemIntoBufferL( aResultItem ));
        }
            
    if ( error != KErrNone )
        {
        TRequestStatus* status = &aStatus;
        User::RequestComplete( status, error );
        return;
        }
    
    //Note that iPtr has to be member variable. If a new TPtr8 is defined in 
    //here it can go out of scope i.e. reading will fail in the server side.
    //Synchronous SendReceive would not have that problem.
    iPtr.Set( iBuffer->Ptr(0) );
	SendReceive( DiagResultsDbCommon::ESubsessionLogTestResult, 
	                        TIpcArgs(&iPtr), aStatus );

    }


// ---------------------------------------------------------------------------
// Write CDiagResultsDatabaseItem into a buffer.
// ---------------------------------------------------------------------------
//  
void RDiagResultsDatabaseRecord::WriteDatabaseItemIntoBufferL( 
                                 const CDiagResultsDatabaseItem& aResultItem )
    {
    RBufWriteStream writeStream ( *iBuffer );
    CleanupClosePushL( writeStream );
    aResultItem.ExternalizeL( writeStream );
    writeStream.CommitL();
    CleanupStack::PopAndDestroy();
    }

// ---------------------------------------------------------------------------
// Get test results of the test record. 
// ---------------------------------------------------------------------------
//    
 EXPORT_C TInt RDiagResultsDatabaseRecord::GetTestResults ( 
                RPointerArray<CDiagResultsDatabaseItem>& aResultsArray ) const
    {

    TRAPD( err, DoGetTestResultsL( aResultsArray ) );
    return err;
    }  

// ---------------------------------------------------------------------------
// Retrieve test results. Server serializes data into the flat buffer. 
// Read stream can be used to deserialize data.
// ---------------------------------------------------------------------------
//
void RDiagResultsDatabaseRecord::DoGetTestResultsL ( 
                RPointerArray<CDiagResultsDatabaseItem>& aResultsArray ) const
    {
    iBuffer->Delete( 0, iBuffer->Size() );
    iBuffer->ResizeL( KResultsDatabaseSubsessionBufferLength );
    
    TPtr8 ptr ( iBuffer->Ptr(0));
    //Now ask server to serialize object into this buffer
    TInt ret = SendReceive( DiagResultsDbCommon::ESubsessionGetTestResults, 
                            TIpcArgs(&ptr) );
    
    while ( ret == KErrOverflow )
        {
        iBuffer->ResizeL( iBuffer->Size() + 
                          KResultsDatabaseSubsessionBufferLength );
        ptr.Set( iBuffer->Ptr(0) ); 
        ret = SendReceive( DiagResultsDbCommon::ESubsessionGetTestResults, 
                           TIpcArgs(&ptr) );
        }
    if ( ret != KErrNone )
        {
        User::Leave (ret);
        }
        
    RBufReadStream readStream ( *iBuffer );    
    TInt count = readStream.ReadInt16L();
    
    for ( TInt i = 0; i < count; ++i )
        {
        aResultsArray.AppendL (CDiagResultsDatabaseItem::NewL( readStream ) );
        }
    }

// ---------------------------------------------------------------------------
// Get only one result from the test record.
// ---------------------------------------------------------------------------
//
EXPORT_C TInt RDiagResultsDatabaseRecord::GetTestResult ( 
            TUid aPluginUid, 
            CDiagResultsDatabaseItem*& aResultItem ) const
    {    
    TRAPD( err, DoGetTestResultL ( aPluginUid, aResultItem ) );
    return err;
    }

// ---------------------------------------------------------------------------
// Resize flat buffer and use read stream to retrieve data into DB item.
// ---------------------------------------------------------------------------
//
void RDiagResultsDatabaseRecord::DoGetTestResultL ( 
            TUid aPluginUid, 
            CDiagResultsDatabaseItem*& aResultItem ) const
    {
    iBuffer->Delete( 0, iBuffer->Size() );
    iBuffer->ResizeL( KResultsDatabaseSubsessionBufferLength );
	
    TPtr8 ptr ( iBuffer->Ptr(0) );
    TPckgBuf<TUid> uid( aPluginUid );
    //Now ask server to serialize object into this buffer
    TInt ret = SendReceive( DiagResultsDbCommon::ESubsessionGetTestResult, 
                                                 TIpcArgs(&uid, &ptr) );
    
    while ( ret == KErrOverflow )
        {
        iBuffer->ResizeL( iBuffer->Size() 
                                + KResultsDatabaseSubsessionBufferLength );
        ptr.Set( iBuffer->Ptr(0) );                              
        ret = SendReceive( DiagResultsDbCommon::ESubsessionGetTestResult, 
                           TIpcArgs(&uid, &ptr) );
        }
    
    if ( ret != KErrNone )
        {
        User::Leave (ret);
        }
     
    //Stream contains the serialized array
    RBufReadStream readStream ( *iBuffer );
    CleanupClosePushL( readStream );
    
    //Construct new test result.
    CDiagResultsDatabaseItem* item = 
                                CDiagResultsDatabaseItem::NewL( readStream );
    aResultItem = item;
    
    CleanupStack::PopAndDestroy( &readStream );
    
    iBuffer->Delete(0, iBuffer->Size() );      
    }               
                              
// ---------------------------------------------------------------------------
// Starts server if it's not already running. 
// ---------------------------------------------------------------------------
//
TInt DiagResultsDbCommon::StartServer()
    {
    LOGME("DiagResultsDbCommon::StartServer");
	TInt res(KErrNone);
	// create server - if one of this name does not already exist
	TFindServer findServer(KDiagResultsDatabaseServerName);
	TFullName name;
	if (findServer.Next(name)!=KErrNone) // we don't exist already
		{
		TRequestStatus status;
		RProcess server;
		// Create the server process
		res = server.Create(KDiagResultsDatabaseServerExe, KNullDesC);		

		if( res!=KErrNone ) // thread created ok - now start it going
			{
			return res;
			}

		server.Rendezvous( status );
		if( status != KRequestPending )
		    {
		    server.Kill(0); // abort start-up
		    }
        else
            {
            server.Resume(); // // wait for server start-up.
            }
                
		// Wait until the completion of the server creation
		User::WaitForRequest( status );
		if( status != KErrNone )
			{
			server.Close();
			return status.Int();
			}
		// Server created successfully
		server.Close(); // we're no longer interested in the other process
		}
	LOGME1("startserver %d",res);
    return res;
    }
    
// ---------------------------------------------------------------------------
// DLL entry point
// ---------------------------------------------------------------------------
//
#ifndef EKA2
GLDEF_C TInt E32Dll(TDllReason /*aReason*/)
	{
	return(KErrNone);
   	}
#endif

   
