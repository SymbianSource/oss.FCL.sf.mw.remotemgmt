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
* Description:  Class definition of CDiagResultsDbSession
*
*/


#include "diagresultsdatabasecommon.h"
#include "diagresultsdbsession.h"
#include "diagresultsdbserver.h"
#include "diagresultsdbtestrecordsubsession.h"
#include "diagresultsdatabasetestrecordinfo.h"
#include "diagresultsdbtestrecordhandle.h"
#include "diagresultsdbtestrecord.h"
#include "diagresultsdatabaseitem.h"
#include "diagresultsdbrecordinfoarraypacked.h"
#include "diagresultsdbcrdc.h"
#include "diagresultsdbrecordengineparam.h"

//System includes
#include <s32mem.h>

const TInt KArrayGranuality = 50;
const TInt KResultsDatabaseBufferLength=0x700;

// ---------------------------------------------------------------------------
// constructor - must pass client to CSession
// ---------------------------------------------------------------------------
// 
CDiagResultsDbSession::CDiagResultsDbSession(CDiagResultsDbServer * aServer): 
       iServer(aServer), iSubsessionContainer( NULL ), iSubsessionIndex(NULL), 
       iLastResultCommand( 0 ),
       iStore(NULL), iBufferedLastResults(NULL), iBufferedSingleResult(NULL),
       iHasWrittenData(EFalse)
	{
	aServer->IncreaseSessionCount();
	}

// ---------------------------------------------------------------------------
// NewL.
// ---------------------------------------------------------------------------
// 
CDiagResultsDbSession* CDiagResultsDbSession::NewL(CDiagResultsDbServer * aServer)
	{
	CDiagResultsDbSession* pSession= new (ELeave) CDiagResultsDbSession( aServer );
	CleanupStack::PushL( pSession );
	pSession->ConstructL();
	CleanupStack::Pop();
	return pSession;
	}

// ---------------------------------------------------------------------------
// ConstructL.
// ---------------------------------------------------------------------------
// 
void CDiagResultsDbSession::ConstructL()
	{
	iSubsessionContainer = iServer->NewContainerL();
	iSubsessionIndex = CObjectIx::NewL();
	
	// buffer for IPC. Objects are transmitted in this buffer.
	// Size of the buffer has to be selected carefully.
	// It should be 'large enough' but not too large.
	// If you see too many overflows, increase the size.
	iBuffer = CBufFlat::NewL(KArrayGranuality);
	iBuffer->ResizeL(KResultsDatabaseBufferLength);
	}

// ---------------------------------------------------------------------------
// Destructor.
// ---------------------------------------------------------------------------
// 	
CDiagResultsDbSession::~CDiagResultsDbSession()
	{
    if ( iBuffer )
        {
        iBuffer->Reset();
        delete iBuffer;
        iBuffer = NULL;
        }
    
	// NOTE!
	// The deletion order is important here. You must
	// delete object indexes first, because this zeros the
	// number of references to any CObject type objects. Trying 
	// to delete object container first, you get panic
	// E32USER-CBase 33: an attempt is made to delete the CObject 
	// when the reference count is not zero.
	delete iSubsessionIndex;

    if ( iSubsessionContainer )
        {
        iServer->RemoveContainer( iSubsessionContainer );
        }
	
	delete iStore;
	
    if ( iServer )
        {
        iServer->DecreaseSessionCount();
        }
	
	if ( iBufferedLastResults )
	    {
	    iBufferedLastResults->ResetAndDestroy();
	    iBufferedLastResults->Close();
	    delete iBufferedLastResults;
	    iBufferedLastResults = 0;
	    }
	    
    if ( iBufferedSingleResult )
        {
        delete iBufferedSingleResult;
        iBufferedSingleResult = NULL;
        }
	}

// ---------------------------------------------------------------------------
// Service client requests.
// ---------------------------------------------------------------------------
// 
void CDiagResultsDbSession::ServiceL(const RMessage2& aMessage)
	{
	LOGME("CDiagResultsDbSession::ServiceL");
	TBool async = EFalse;
	TRAPD( err, async = DispatchMessageL( aMessage ) );
	LOGME1("CDiagResultsDbSession::ServiceL %d",err);
	if ( !async ) 
	    {
	    aMessage.Complete( err );
	    }
	}

// ---------------------------------------------------------------------------
// service a client request; test the opcode and then do appropriate servicing
// ---------------------------------------------------------------------------
//     
TBool CDiagResultsDbSession::DispatchMessageL(const RMessage2 &aMessage)
	{
	LOGME("CDiagResultsDbSession::DispatchMessageL");
	iMsg = aMessage;
	TInt function = aMessage.Function();
	LOGME1("CDiagResultsDbSession::DispatchMessageL - %d",function);
	TInt handle(0);
	CObject* subsession = NULL;
	switch( function )
        {
	case DiagResultsDbCommon::EConnect:
	    {
	    LOGME("CDiagResultsDbSession::EConnect");
	    TPckgBuf<TUid> uidpckg;
	    aMessage.Read(0, uidpckg);
        iDbUid = uidpckg();
        
        iStore = CDiagResultsDbStore::NewL( iDbUid );
		return EFalse;
	    }
        
    case DiagResultsDbCommon::EClose:
        {        
        LOGME("CDiagResultsDbSession::EClose");
        aMessage.Complete (KErrNone);
                      
        return ETrue;
        }
        
    case DiagResultsDbCommon::EGetRecordCount:
        {        
        LOGME("CDiagResultsDbSession::EGetRecordCount");
        RArray<TUid> uids;
        CleanupClosePushL ( uids );
        iStore->RecordUidsL( uids );
        
        TPckgBuf<TUint> pckg( uids.Count() );
        aMessage.Write(0, pckg);
        
        CleanupStack::PopAndDestroy( &uids );
        return EFalse;
        }
        
	case DiagResultsDbCommon::EConnectSubsession:
			LOGME("CDiagResultsDbSession::EConnectSubsession");
	    ConnectSubsessionL( aMessage );
		return EFalse;			
		
	case DiagResultsDbCommon::ESubsessionCreateNewRecord:
			LOGME("CDiagResultsDbSession::ESubsessionCreateNewRecord");
	    CreateNewRecordL( aMessage );
	    HasWritten();
		return EFalse;
		
	case DiagResultsDbCommon::ECloseSubsession:
		LOGME("CDiagResultsDbSession::ECloseSubsession");
		CloseSubsessionL();
		return EFalse;
		
    case DiagResultsDbCommon::EGetLastRecord:  
    		LOGME("CDiagResultsDbSession::EGetLastRecord");
        GetLastRecordL( aMessage );
        return EFalse;
        
    case DiagResultsDbCommon::EGetLastNotCompletedRecord:
    		LOGME("CDiagResultsDbSession::EGetLastNotCompletedRecord");
        GetLastNotCompletedRecordL( aMessage );
        return EFalse;
    
    case DiagResultsDbCommon::EGetRecordList: //record uids 
    		LOGME("CDiagResultsDbSession::EGetRecordList");
        GetRecordListL( aMessage );
        return EFalse;

    case DiagResultsDbCommon::EGetRecordInfoList:
    		LOGME("CDiagResultsDbSession::EGetRecordInfoList");
        GetRecordInfoListL( aMessage );
        return EFalse;

    case DiagResultsDbCommon::EInitiateGetLastResults:  //Async
        {     
        LOGME("CDiagResultsDbSession::EInitiateGetLastResults");
        ReadBufferL( aMessage, 0, iBuffer );
               
        iLastResultsMsg = iMsg;
        	
        iLastResultCommand = DiagResultsDbCommon::EInitiateGetLastResults;	
        	
        if ( iBufferedLastResults )
	        {
	        iBufferedLastResults->ResetAndDestroy();
	        iBufferedLastResults->Close();
	        delete iBufferedLastResults;
	        iBufferedLastResults = 0;
	        }
	        
        iStore->ExistingRecordsAsyncL( *this );
        return ETrue;
        }
    
    case DiagResultsDbCommon::EInitiateGetSingleLastResult: //Async
        {           
        LOGME("CDiagResultsDbSession::EInitiateGetSingleLastResult");     
        iLastSingleResultsMsg = iMsg;
        
        iLastResultCommand = DiagResultsDbCommon::EInitiateGetSingleLastResult;	
        
        if ( iBufferedSingleResult )
            {
            delete iBufferedSingleResult;
            iBufferedSingleResult = NULL;
            }
            
        iStore->ExistingRecordsAsyncL( *this );
        return ETrue;  
        }
    
    
    case DiagResultsDbCommon::EGetLastResults:    
    		LOGME("CDiagResultsDbSession::EGetLastResults");     
        GetLastResultsL( aMessage );
        return EFalse;
        
    case DiagResultsDbCommon::EGetSingleLastResult: 
    		LOGME("CDiagResultsDbSession::EGetSingleLastResult");        
        GetSingleLastResultL( aMessage );
        return EFalse;    
        
    case DiagResultsDbCommon::ECancelInitiateGetLastResults:
        {
        LOGME("CDiagResultsDbSession::ECancelInitiateGetLastResults");        
        CancelLastResultsL( aMessage );
        return EFalse;
        }
       
	// Sub-session requests. See CDiagResultsDbTestRecordSubsession.
    case DiagResultsDbCommon::ESubsessionGetTestRecordId:
    case DiagResultsDbCommon::ESubsessionTestCompleted:
    case DiagResultsDbCommon::ESubsessionIsTestCompleted:
    case DiagResultsDbCommon::ESubsessionGetRecordInfo:
    case DiagResultsDbCommon::ESubsessionGetTestUids:
    case DiagResultsDbCommon::ESubsessionSuspend:
    case DiagResultsDbCommon::ESubsessionIsSuspended:
    case DiagResultsDbCommon::ESubsessionLogTestResult:
    case DiagResultsDbCommon::ESubsessionGetTestResult:
    case DiagResultsDbCommon::ESubsessionGetTestResults:
    case DiagResultsDbCommon::ESubsessionGetEngineParam:
    case DiagResultsDbCommon::ESubsessionGetStatus:
    case DiagResultsDbCommon::ESubsessionCancelLogTestResult:
    LOGME("CDiagResultsDbSession::ESubsessionCancelLogTestResult");        
		handle = aMessage.Int3();
		subsession = iSubsessionIndex->At( handle );
		return static_cast<CDiagResultsDbTestRecordSubsession*>(subsession)
		                                    ->DispatchMessageL( aMessage );
	default:
		aMessage.Panic( _L("DiagSrv panic: unknown command"), 
		                    DiagResultsDbCommon::EBadRequest );
		return EFalse;
		}
	}

// ---------------------------------------------------------------------------
// Get function.
// ---------------------------------------------------------------------------
// 
TBool CDiagResultsDbSession::SessionHasWritten() const
    {
    return iHasWrittenData;
    }
            
// ---------------------------------------------------------------------------
// Connect to a subsession. Test record represents the connected subsession.
// ---------------------------------------------------------------------------
//     
void CDiagResultsDbSession::ConnectSubsessionL( const RMessage2 &aMessage )    
    {
    TPckgBuf<TUid> uidpckg;
	aMessage.Read(0, uidpckg);
    TUid recordUid = uidpckg(); 
    
    TPckgBuf<TBool> readOnlyPckg;
	aMessage.Read(1, readOnlyPckg);
    TBool readonly = readOnlyPckg();
    
	CDiagResultsDbTestRecordHandle* handle = iStore->OpenExistingHandleL( 
	                                           recordUid );
	                                           
    if ( readonly ) 
        {
        // keep the record as it is.
        }
    else
        {
        if ( handle->RecordInfo().iCompleted )
            {
            delete handle;
            handle = 0;
            User::Leave( KErrAlreadyExists );
            }
        
        handle->RecordInfo().iRecordStatus = 
                TDiagResultsDatabaseTestRecordInfo::EOpen;
        }
	                                           
    CreateSubsessionL( handle, readonly );
    }

// ---------------------------------------------------------------------------
// Create a new record. This does not write data into the db file.
// ---------------------------------------------------------------------------
//     
void CDiagResultsDbSession::CreateNewRecordL ( const RMessage2 &aMessage )
    {
    
    ReadBufferL( aMessage, 1, iBuffer );
    
    RBufReadStream stream( *iBuffer );
    CleanupClosePushL ( stream );
    
    CDiagResultsDbRecordEngineParam* params = 
                            CDiagResultsDbRecordEngineParam::NewL ( stream );
    CleanupStack::PushL( params );                         

    CDiagResultsDbTestRecordHandle* handle = iStore->CreateNewRecordL( params );

    handle->RecordInfo().iRecordStatus = 
                                TDiagResultsDatabaseTestRecordInfo::EOpen;

    CleanupStack::Pop(); //params    
    CleanupStack::PopAndDestroy( &stream );

	CreateSubsessionL(handle, EFalse);
		
	TPckgBuf<TUid> recorduidpckg( handle->RecordInfo().iRecordId );
		
	aMessage.Write(0, recorduidpckg );
    }
    
// ---------------------------------------------------------------------------
// Return the database file uid.
// ---------------------------------------------------------------------------
//      
TUid CDiagResultsDbSession::DbUid() const
    {
    return iDbUid;
    }

// ---------------------------------------------------------------------------
// Create a new subsession.
// ---------------------------------------------------------------------------
//     
void CDiagResultsDbSession::CreateSubsessionL( 
                            CDiagResultsDbTestRecordHandle* aTestRecordHandle,
                            TBool aReadonly )
	{
    TInt handle(0);
    CObject* subsession = NULL;
    
	// Create sub-session object
	subsession = CDiagResultsDbTestRecordSubsession::NewL( this, 
	                                                    aTestRecordHandle,
	                                                    aReadonly );
    CleanupStack::PushL( subsession );
    iSubsessionContainer->AddL( subsession );
    CleanupStack::Pop();

	// Create sub-session handle
	TRAPD( err, handle = iSubsessionIndex->AddL( subsession ) );

	// Remember to remove session object from object container
	if( err != KErrNone )
		{
		iSubsessionContainer->Remove( subsession ); 
		User::Leave( DiagResultsDbCommon::ESvrCreateSubsession );
		}

    // Package to pass information to the client
    TPckgC<TInt> handlePckg(handle);
    
    // Send handle to the client
    TRAP( err, iMsg.WriteL( 3, handlePckg ) );
	if( err != KErrNone )
		{
		iSubsessionIndex->Remove(handle);
		User::Leave( DiagResultsDbCommon::ESvrCreateSubsession );
		}

	return;
	}

// ---------------------------------------------------------------------------
// Close existing subsession.
// ---------------------------------------------------------------------------
// 
void CDiagResultsDbSession::CloseSubsessionL()
	{
	TInt handle = iMsg.Int3();
	iSubsessionIndex->Remove(handle);
	}

// ---------------------------------------------------------------------------
// Return store that is responsible for handling the database file.
// ---------------------------------------------------------------------------
// 	
CDiagResultsDbStore& CDiagResultsDbSession::Store()
    {
    return *iStore;
    }

// ---------------------------------------------------------------------------
// Service function. Searches for newest test results. 
//    Related functions: 
//    ExistingRecordsAsyncL (initiates async fetch of test records)
//    CancelLastResultsL (cancel async fetch)
//    ExistingRecordsL (retrieves test records)
// ---------------------------------------------------------------------------
// 
void CDiagResultsDbSession::GetLastResultsL( const RMessage2 &aMessage )
    {
    if ( iBufferedLastResults == NULL )
        {
        User::Leave ( KErrNotFound );
        }
       
    ReadBufferL( aMessage, 0, iBuffer );
      
    RBufWriteStream stream ( *iBuffer );
    CleanupClosePushL( stream );
    
    stream.WriteInt16L( iBufferedLastResults->Count() );
    
    for ( TInt i = 0; i < iBufferedLastResults->Count(); ++i )
        {    
        CDiagResultsDatabaseItem* item = (*iBufferedLastResults)[i];
        
        if ( item == NULL )
            {
            stream.WriteUint8L(0); 
            }
        else 
            {
            stream.WriteUint8L(1);
            (*iBufferedLastResults)[i]->ExternalizeL( stream );
            }
        }

    if ( iBuffer->Ptr(0).Length() > aMessage.GetDesMaxLength(0) )
        {
        User::Leave( KErrOverflow );
        }
   
    stream.CommitL();
     
    CleanupStack::PopAndDestroy( &stream );
    
    aMessage.Write( 0, iBuffer->Ptr(0) ); //write to client's address space 
         
    iBufferedLastResults->ResetAndDestroy();
    iBufferedLastResults->Close();
    delete iBufferedLastResults;
    iBufferedLastResults = NULL;   
    }


// ---------------------------------------------------------------------------
// Service function. Get single test result (the newest).
// ---------------------------------------------------------------------------
//   
void CDiagResultsDbSession::GetSingleLastResultL( const RMessage2 &aMessage )
    {
    ReadBufferL( aMessage, 0, iBuffer );
          
    RBufWriteStream stream ( *iBuffer );
    CleanupClosePushL( stream );
    
    if ( iBufferedSingleResult )
        {
        stream.WriteInt8L( 1 );
        
        iBufferedSingleResult->ExternalizeL( stream );
        
        }
    else //NULL
        {
        stream.WriteInt8L( 0 );
        }
    
    stream.CommitL();
    CleanupStack::PopAndDestroy( &stream );
    
    aMessage.Write( 0, iBuffer->Ptr(0) );
            
    delete iBufferedSingleResult;
    iBufferedSingleResult = NULL;
    }


// ---------------------------------------------------------------------------
// Service function. Cancel InitiateGetLastResults method.
// ---------------------------------------------------------------------------
//     
void CDiagResultsDbSession::CancelLastResultsL( const RMessage2 & /*aMessage*/ )
    {
      if ( !iLastResultsMsg.IsNull() )
        {
        iStore->Cancel();    
        iLastResultsMsg.Complete( KErrCancel );  
        
        if ( iBufferedLastResults )
            {
            iBufferedLastResults->ResetAndDestroy();
            delete iBufferedLastResults;
            iBufferedLastResults = NULL;
            }
        }
    }

// ---------------------------------------------------------------------------
// Service function. Retrieve uid of the newest test record.
// ---------------------------------------------------------------------------
// 
void CDiagResultsDbSession::GetLastRecordL( const RMessage2 &aMessage )
    {
    RArray<TUid> uids;
    CleanupClosePushL ( uids );
    iStore->RecordUidsL( uids );
    
    if ( uids.Count() == 0 )
        {
        User::Leave( KErrNotFound );
        }
    
    TPckgBuf<TUid> pckg( uids[uids.Count() -1] ); //newest record is the last
    aMessage.Write( 0, pckg );
    
    CleanupStack::PopAndDestroy( &uids );
    }


// ---------------------------------------------------------------------------
// Service function. Try to find a test record that was suspended.
// Leave with KErrNotFound if such test record is not found.
// ---------------------------------------------------------------------------
// 
void CDiagResultsDbSession::GetLastNotCompletedRecordL( const RMessage2 &aMessage )
    {
    RArray<TUid> uids;
    CleanupClosePushL ( uids );
    iStore->RecordUidsL( uids );
    
    TBool found = EFalse;
    
    // newest record are on the top.
	for (TInt i = uids.Count() -1; i >= 0; --i)
	    {
	    CDiagResultsDbTestRecordHandle* handle = 
	                                iStore->OpenExistingHandleL( uids[i] );
	    
	    CleanupStack::PushL (handle);
        
        if( handle->RecordInfo().iRecordStatus != 
                                 TDiagResultsDatabaseTestRecordInfo::ECompleted &&
               !handle->RecordInfo().iCompleted )
                                 
            {
            TPckgBuf<TUid> pckg( handle->RecordInfo().iRecordId ); 
            aMessage.Write( 0, pckg );
            found = ETrue;
            CleanupStack::PopAndDestroy( handle );
            break;
            }     
        
        CleanupStack::PopAndDestroy( handle );
	    }
	
    CleanupStack::PopAndDestroy( &uids );
	 
    if ( !found )
        {
        User::Leave ( KErrNotFound );
        }  
    }

// ---------------------------------------------------------------------------
// Service function. Retrieve all test record uids that there are.
// ---------------------------------------------------------------------------
//     
void CDiagResultsDbSession::GetRecordListL( const RMessage2 &aMessage )
    {    
    ReadBufferL( aMessage, 0, iBuffer );
   
    RArray<TUid> uids; 
    CleanupClosePushL( uids );
    iStore->RecordUidsL( uids );
    
    RBufWriteStream stream ( *iBuffer );
    CleanupClosePushL( stream );
    stream.WriteInt16L( uids.Count() );
    
    for ( TInt i = 0; i < uids.Count(); ++i )
        {    
        stream.WriteInt32L( uids[i].iUid );  
        }

    if ( iBuffer->Ptr(0).Length() > aMessage.GetDesMaxLength(0) )
        {
        User::Leave( KErrOverflow );
        }
    
    aMessage.Write( 0, iBuffer->Ptr(0) ); //write to client's address space 
   
    CleanupStack::PopAndDestroy( &stream );
    CleanupStack::PopAndDestroy( &uids );

    }

// ---------------------------------------------------------------------------
// Service function. Return overviews of test records.
// ---------------------------------------------------------------------------
//     
void CDiagResultsDbSession::GetRecordInfoListL( const RMessage2 &aMessage )
    {
    ReadBufferL( aMessage, 0, iBuffer );
    
    CArrayFixFlat<TDiagResultsDatabaseTestRecordInfo>* array = new (ELeave) 
        CArrayFixFlat<TDiagResultsDatabaseTestRecordInfo>(KArrayGranuality);
    CleanupStack::PushL (array);
    
    RArray<TUid> uids;
    CleanupClosePushL ( uids );
    iStore->RecordUidsL( uids );
    
	for (TInt i = 0; i < uids.Count(); ++i)
	    {
	    CDiagResultsDbTestRecordHandle* handle = 
	                                iStore->OpenExistingHandleL( uids[i] );
	    
	    CleanupStack::PushL (handle);
        array->AppendL( handle->RecordInfo() );
        
        CleanupStack::PopAndDestroy( handle );
	    }
	
	CleanupStack::PopAndDestroy( &uids );
	
    TDiagResultsDbRecordInfoArrayPacked packedArray ( iBuffer );
    packedArray.PackArrayL ( *array );
    
    if ( iBuffer->Ptr(0).Length() > aMessage.GetDesMaxLength(0) )
        {
        User::Leave( KErrOverflow );
        }
	
    aMessage.Write( 0, iBuffer->Ptr(0) ); //write to client's address space 
   
    CleanupStack::PopAndDestroy( array );
    
    }
 
// ---------------------------------------------------------------------------
// Helper function to read a buffer from client side. Leaves the buffer
// onto cleanup stack IF it has to create a new one.
// ---------------------------------------------------------------------------
//    
void CDiagResultsDbSession::ReadBufferL(const RMessage2& aMessage, TInt aParam, 
                                                            CBufFlat*& aBuffer)
	{
	TInt desLen = aMessage.GetDesLengthL(aParam);

	if(desLen >= 0)
		{		
		if (aBuffer==NULL)
			{
			aBuffer = CBufFlat::NewL(KArrayGranuality);
			aBuffer->ResizeL(desLen);
			CleanupStack::PushL(aBuffer);
			}
		else if (desLen > aBuffer->Ptr(0).MaxLength())
			{
			iBuffer->Delete( 0, iBuffer->Size() ); // delete old data.
			// we have to increase the size of aBuffer	
			aBuffer->ResizeL(desLen);		
			}

		TPtr8 desPtr = aBuffer->Ptr(0);
		aMessage.ReadL(aParam, desPtr);

		}
	else
		{
		// desLen is negative leave with an error.
		User::Leave(KErrArgument);
		}
	}   

// ---------------------------------------------------------------------------
// Store observer method. This is called after store has retrieved
// test records from the DB file.
// ---------------------------------------------------------------------------
// 
void CDiagResultsDbSession::ExistingRecordsL( TInt aError, 
                            RPointerArray<CDiagResultsDbTestRecord>* aArray )
    {
    CleanupStack::PushL ( aArray );
    CleanupResetAndDestroyClosePushL( *aArray );

    //Check if there were any errors during loading the test records.
    if ( aError != KErrNone )
        {
        
        if ( iLastResultCommand == 
             DiagResultsDbCommon::EInitiateGetLastResults )
            {
            iLastResultsMsg.Complete ( aError );            
            }
        else if ( iLastResultCommand == 
                  DiagResultsDbCommon::EInitiateGetSingleLastResult  )
            {
            iLastSingleResultsMsg.Complete ( aError );
            }        
        
        CleanupStack::PopAndDestroy( aArray );
        CleanupStack::PopAndDestroy( aArray ); //delete the pointer  
        return;
        }
        	
	switch( iLastResultCommand )
        {
	
	//Find multiple last results
	//buffer contains the uids to be searched.
	//See RDiagResultsDatabase::InitiateGetLastResults.
	case DiagResultsDbCommon::EInitiateGetLastResults:
	    {
        //Trap is needed so that we can complete client's request
        //if any errors occur.
        TRAPD(error, FindLastResultsL( *aArray ));	        
	        	    
	    if ( error != KErrNone )
	        {	        
	        iLastResultsMsg.Complete (error);	        
	        break;
	        }
	    else 
	        {
	        iLastResultsMsg.Complete (KErrNone);    
	        }
	    	    	  
	    break;
	    }
	    
	  // Find single test result  
	  // see RDiagResultsDatabase::InitiateGetLastResult.
     case DiagResultsDbCommon::EInitiateGetSingleLastResult:
        {        
        TPckgBuf<TUid> uidpckg;
	    iLastSingleResultsMsg.Read(0, uidpckg);
        TUid resultsItemUid = uidpckg();
                 	    	            
        CDiagResultsDatabaseItem* item = NULL;
        
        // Trap any errors and complete client's request if there
        // are any errors.
        TRAPD(error, item = FindDatabaseItemL( resultsItemUid, aArray)) ;	                            	                           

        if ( error != KErrNone )
            {
            delete item;
            item = NULL;
            
            iLastSingleResultsMsg.Complete (error);
            break;
            }
	        
	    if ( item == NULL ) //Not found
	        {
	        iBufferedSingleResult = NULL;
	        
	        //Check also the last results buffer  
	        CheckLastResultsBufferL( resultsItemUid, iBufferedSingleResult );
	        }
	    else 
	        {	                	                
	        iBufferedSingleResult = item;
	        }          
	       
	   
	       
	    iLastSingleResultsMsg.Complete (KErrNone);
	    break;
        }  
	  
	  default:
	    {
	    User::Panic ( _L("Diag results DB"), 
	                  DiagResultsDbCommon::EUnknownLastResultState );
	    }	    	    
      }
    
    iLastResultCommand = 0;  
      
    CleanupStack::PopAndDestroy( aArray ); //call reset and destroy + close
    CleanupStack::PopAndDestroy( aArray ); //delete the pointer
    }


// ---------------------------------------------------------------------------
// Read UIDs from a stream and search last results.
// ---------------------------------------------------------------------------
// 
void CDiagResultsDbSession::FindLastResultsL( 
                            RPointerArray<CDiagResultsDbTestRecord>& aArray )
    {
    
	CArrayFixFlat<TUid>* uidArray = new (ELeave) 
	                                    CArrayFixFlat<TUid>(KArrayGranuality);
	CleanupStack::PushL( uidArray );
	     
    RBufReadStream stream( *iBuffer );
    CleanupClosePushL ( stream );
        
    TInt8 count = stream.ReadInt8L();
        
    for ( TInt i = 0; i < count; ++i )
        {
        TInt32 uid = stream.ReadInt32L();
            
        uidArray->AppendL( TUid::Uid( uid ));
        }
	    
    CleanupStack::PopAndDestroy( &stream );	 
	    	    
	iBufferedLastResults = new (ELeave) RPointerArray<CDiagResultsDatabaseItem>;
	    	    
	SearchLastResultsL( *uidArray, aArray, *iBufferedLastResults  );
	
	//last results could be also in the last results buffer.
	CheckLastResultsBufferL( *uidArray, *iBufferedLastResults );
	    
	  // there must be exactly the same number of cells in both arrays. 
	if ( uidArray->Count() != iBufferedLastResults->Count() ) 
	    {
	    User::Panic ( _L("Diag results DB"), 
	                  DiagResultsDbCommon::EGetLastResultsMismatch );
	    }
	    
	CleanupStack::PopAndDestroy( uidArray );   
    }


// ---------------------------------------------------------------------------
// Check last result buffer for test results.
// ---------------------------------------------------------------------------
// 
void CDiagResultsDbSession::CheckLastResultsBufferL( 
                         const CArrayFixFlat<TUid>& aUidArray,                           
                         RPointerArray<CDiagResultsDatabaseItem>& aResultsArray )
    {        
    CDiagResultsDbTestRecord* buffer = iStore->OpenExistingLastResultsBufferL(); 
    CleanupStack::PushL( buffer );
    
    for (TInt i = 0; i < aResultsArray.Count(); ++i)
        {        
        if ( aResultsArray[i] == NULL )
            {
            TUid uid = aUidArray[i];
            
            CDiagResultsDatabaseItem* item = buffer->FindTestRecord( uid );
            
            if ( item )
                {
                buffer->RemoveL( uid );
                aResultsArray[i] = item;
                }            
            }        
        }
    
    CleanupStack::PopAndDestroy( buffer );
    }


// ---------------------------------------------------------------------------
// Check last result buffer for test result.
// ---------------------------------------------------------------------------
// 
void CDiagResultsDbSession::CheckLastResultsBufferL( 
                         TUid aTestUid,                           
                         CDiagResultsDatabaseItem*& aResult )
    {
    
    
    CDiagResultsDbTestRecord* buffer = iStore->OpenExistingLastResultsBufferL(); 
    CleanupStack::PushL( buffer );
 
    CDiagResultsDatabaseItem* item = buffer->FindTestRecord( aTestUid );
            
    if ( item )
        {
        buffer->RemoveL( aTestUid );
        aResult = item;                                                
        }
    
    CleanupStack::PopAndDestroy( buffer );
    }


// ---------------------------------------------------------------------------
// Searches for the newest test results.
// ---------------------------------------------------------------------------
// 
void CDiagResultsDbSession::SearchLastResultsL( 
                         const CArrayFixFlat<TUid>& aUidArray, 
                         RPointerArray<CDiagResultsDbTestRecord>& aTestRecordArray, 
                         RPointerArray<CDiagResultsDatabaseItem>& aResultsArray )
            
    {   
    //Search all records for certain uid.
	for ( TInt i = 0; i < aUidArray.Count(); ++i )
	    {
	      
	    CDiagResultsDatabaseItem* item = 
	                            FindDatabaseItemL( aUidArray[i], &aTestRecordArray );	                            	                           
	        
	    if ( item == NULL ) //Not found
	        {
	        aResultsArray.Append(NULL);
	        }
	     else 
	        {	                	                
	        aResultsArray.Append( item );
	        }          
	    }
    }
	     

// ---------------------------------------------------------------------------
// Indicates has session written any data into the DB file. 
// To be exact only subsession writes data into the file.
// ---------------------------------------------------------------------------
//     
void CDiagResultsDbSession::HasWritten()
    {
    iHasWrittenData = ETrue;
    }

// ---------------------------------------------------------------------------
// Turn off compacting.
// ---------------------------------------------------------------------------
//  
void CDiagResultsDbSession::DoNotCompact()
    {
    iHasWrittenData = EFalse;
    }

// ---------------------------------------------------------------------------
// Helper function to seach an item from a pointer array.
// Starts from the newest record to search for an UID.
// ---------------------------------------------------------------------------
// 
CDiagResultsDatabaseItem* CDiagResultsDbSession::FindDatabaseItemL( TUid aUid, 
                             RPointerArray<CDiagResultsDbTestRecord>* aArray )    
    {
    
    //Check that there is a test record.
    if ( !aArray || aArray->Count() == 0 )
        {
        return NULL;
        }
    
    // start from the newest record
    for (TInt x = aArray->Count() -1; x >= 0 ; --x ) 
	    {
	    CDiagResultsDbTestRecord* record = (*aArray)[x];
	    
	    //Assumes that there is only MAX one specific UID in the test record.        
	    for ( TInt y = 0; y < record->Count(); ++y )
	        {
	        CDiagResultsDatabaseItem* item = record->GetItem( y );
	                 
             //	 Search for a test result that is not skipped / cancelled.                 
	         if (  item->TestUid() == aUid && 
	              (item->TestResult() == CDiagResultsDatabaseItem::ESuccess ||
	               item->TestResult() == CDiagResultsDatabaseItem::EFailed ))
	            {
	            //Remove the found item to speed up look up times.
	            //This does not remove it from the DB file.
	            record->RemoveL( y ); 
	            return item;
	            }
	        }     
	    }
	     
	return NULL;  
    }
