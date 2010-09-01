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


#include "diagresultsdatabasecommon.h"
#include "diagresultsdbsession.h"
#include "diagresultsdbtestrecordsubsession.h"
#include "diagresultsdbtestrecord.h"
#include "diagresultsdbtestrecordhandle.h"
#include "diagresultsdbrecordengineparam.h"
#include "diagresultsdatabase.h"

#include <s32mem.h> 

//this matches the size in the client side.
const TInt KResultsDatabaseSubsessionBufferLength = 0x250;
const TInt KResultsDatabaseSubsessionGranuality = 50;

// ---------------------------------------------------------------------------
// NewL.
// ---------------------------------------------------------------------------
// 
CDiagResultsDbTestRecordSubsession* CDiagResultsDbTestRecordSubsession::NewL(
                             CDiagResultsDbSession* aSession, 
                             CDiagResultsDbTestRecordHandle* aTestRecordHandle,
                             TBool aReadonly
                             )
    {
    CDiagResultsDbTestRecordSubsession* self = new( ELeave ) 
                CDiagResultsDbTestRecordSubsession( aSession, 
                                                    aTestRecordHandle,
                                                    aReadonly
                                                    );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();
    return self;
    }

// ---------------------------------------------------------------------------
// Destructor. Checks has the test record been written into the DB.
// If it hasn't been written, destructor writes the data. 
// This situation happens when for example client dies suddenly.
// ---------------------------------------------------------------------------
// 
CDiagResultsDbTestRecordSubsession::~CDiagResultsDbTestRecordSubsession()
    {
    delete iTestRecordHandle;
    iTestRecordHandle = NULL;
    
    if ( iBuffer )
        {
        iBuffer->Reset();
        delete iBuffer;
        iBuffer = NULL;
        }
    }

// ---------------------------------------------------------------------------
// ConstructL. Creates the server side buffer.
// ---------------------------------------------------------------------------
// 
void CDiagResultsDbTestRecordSubsession::ConstructL()
    {
    // buffer for IPC. Objects are transmitted in this buffer.
	// Size of the buffer has to be selected carefully.
	// It should be 'large enough' but not too large.
	// If you see too many overflows, increase the size.
	// Subsession buffer can be smaller than the session buffer
	iBuffer = CBufFlat::NewL( KResultsDatabaseSubsessionGranuality  );
	iBuffer->ResizeL( KResultsDatabaseSubsessionBufferLength );
    }


// ---------------------------------------------------------------------------
// Handles subsession requests. 
// ---------------------------------------------------------------------------
// 
TBool CDiagResultsDbTestRecordSubsession::DispatchMessageL(
                                                    const RMessage2& aMessage)
    {
    TInt function = aMessage.Function();
    switch( function )
		{
        case DiagResultsDbCommon::ESubsessionGetTestRecordId:
            {            
            TPckgBuf<TUid> uid( iTestRecordHandle->RecordInfo().iRecordId);
            aMessage.Write( 0, uid );
            return EFalse;
            }
        
        case DiagResultsDbCommon::ESubsessionTestCompleted: //SYNC
            {
            iMsg = aMessage;
           
            CompleteTestRecordL( aMessage );
            
            aMessage.Complete( KErrNone );
            
            //Do compacting after subsession is completed to
            //minimize disk space usage
            if ( iSession->SessionHasWritten() )
                {
                iSession->Store().CompactDatabase();
                }
            
            //Not asynchronous, but this prevents calling Complete again.
            return ETrue;
            }
  
        case DiagResultsDbCommon::ESubsessionGetStatus:
            {
            GetStatusL ( aMessage );
            return EFalse;
            }
		case DiagResultsDbCommon::ESubsessionSuspend: //SYNC
		    {
		    iMsg = aMessage;
           
            SuspendTestRecordL( aMessage );
            
            aMessage.Complete( KErrNone );
            
            //Do compacting after subsession is completed to
            //minimize disk space usage
            if ( iSession->SessionHasWritten() )
                {
                iSession->Store().CompactDatabase();
                }
            
            //Not asynchronous, but this prevents calling Complete again.
            return ETrue;
		    }
            
        case DiagResultsDbCommon::ESubsessionIsTestCompleted:
            {
            TPckgBuf<TBool> pckg ( Completed() );
            aMessage.Write( 0, pckg );
            return EFalse;
            }
        case DiagResultsDbCommon::ESubsessionGetRecordInfo:
            {
            TPckgBuf<TDiagResultsDatabaseTestRecordInfo> pckg(
                                                 iTestRecordHandle->RecordInfo());
            aMessage.Write( 0, pckg );
            return EFalse;
            }
        case DiagResultsDbCommon::ESubsessionGetTestUids:
            {
            GetTestUidsL( aMessage );
            return EFalse;
            }
            
		case DiagResultsDbCommon::ESubsessionIsSuspended:
		    {
		    TBool suspended = EFalse;
            if ( iTestRecordHandle->RecordInfo().iRecordStatus ==
                    TDiagResultsDatabaseTestRecordInfo::ESuspended )
                {
                suspended = ETrue;
                }
                
            TPckgBuf<TBool> pckg ( suspended );
            aMessage.Write( 0, pckg );
            return EFalse;
		    }
		
		case DiagResultsDbCommon::ESubsessionGetEngineParam:
		    {
		    GetEngineParamL( aMessage );
            return EFalse;
		    }    
		    
        case DiagResultsDbCommon::ESubsessionLogTestResult: //ASYNC
            {
            iMsg = aMessage;
            iCompletedLogTest = EFalse;
            LogTestResultL( aMessage );
            return ETrue;
            }
        
        case DiagResultsDbCommon::ESubsessionCancelLogTestResult: 
            {
            CancelLogTestResult( aMessage );
            
            return EFalse;
            }

        case DiagResultsDbCommon::ESubsessionGetTestResult:
            {
            GetTestResultL( aMessage );
            return EFalse;
            }
            
        case DiagResultsDbCommon::ESubsessionGetTestResults:
            {
            GetTestResultsL( iTestRecordHandle, aMessage );
            return EFalse;
            }
        default:
            break;
		}
	aMessage.Panic( _L("DiagSrv panic: unknown command"), 
	                    DiagResultsDbCommon::EBadRequest );
	return EFalse;
	}


// ---------------------------------------------------------------------------
// Returns record complete status.
// ---------------------------------------------------------------------------
// 
TBool CDiagResultsDbTestRecordSubsession::Completed() const
    {
    TBool completed = EFalse;
     
    if ( iTestRecordHandle->RecordInfo().iCompleted )
        {
        completed = ETrue;
        }
    return completed;         
    }


// ---------------------------------------------------------------------------
// Maps server side status into user side status.
// ---------------------------------------------------------------------------
//  
void CDiagResultsDbTestRecordSubsession::GetStatusL ( const RMessage2& aMessage )
    {
    
    RDiagResultsDatabaseRecord::TRecordStatus status;
    
    if ( iTestRecordHandle->RecordInfo().iCompleted )
        {
        
        if ( iTestRecordHandle->RecordInfo().iRecordStatus == 
                                    TDiagResultsDatabaseTestRecordInfo::EOpen )
            {
            
            status = RDiagResultsDatabaseRecord::EPartiallyCompleted;
            }
        else if ( iTestRecordHandle->RecordInfo().iRecordStatus == 
                                    TDiagResultsDatabaseTestRecordInfo::ESuspended )
            {
            status = RDiagResultsDatabaseRecord::EPartiallyCompleted;
            }
        else if (  iTestRecordHandle->RecordInfo().iRecordStatus == 
                                    TDiagResultsDatabaseTestRecordInfo::ECompleted )   
            {
            status = RDiagResultsDatabaseRecord::ECompleted;
            }     
        else 
        	{        		  
            User::Panic( _L("DiagDbServer error"), DiagResultsDbCommon::EGetStatusPanic);                  
			status = RDiagResultsDatabaseRecord::ECrashed;	
        	}
        }
    else // iCompleted == EFalse
        {
         if ( iTestRecordHandle->RecordInfo().iRecordStatus == 
                                    TDiagResultsDatabaseTestRecordInfo::EOpen )
            {
            if ( iSession->SessionHasWritten() )
                {
                status = RDiagResultsDatabaseRecord::EOpen;
                }
            else
                {
                status = RDiagResultsDatabaseRecord::ECrashed;
                }
            }
        else if ( iTestRecordHandle->RecordInfo().iRecordStatus == 
                                    TDiagResultsDatabaseTestRecordInfo::ESuspended )
            {
            status = RDiagResultsDatabaseRecord::ESuspended;
            }
         // TDiagResultsDatabaseTestRecordInfo::ECompleted is not valid combination either
        else     
            {
            User::Panic( _L("DiagDbServer error"), DiagResultsDbCommon::EGetStatusPanic);
			status = RDiagResultsDatabaseRecord::ECrashed;
            }        
        }
        
    TPckgBuf<RDiagResultsDatabaseRecord::TRecordStatus> pckg( status );
    aMessage.Write( 0, pckg );    
    }

// ---------------------------------------------------------------------------
// Constructor.
// ---------------------------------------------------------------------------
//     
CDiagResultsDbTestRecordSubsession::CDiagResultsDbTestRecordSubsession(
                            CDiagResultsDbSession* aSession, 
                            CDiagResultsDbTestRecordHandle* aTestRecordHandle,
                            TBool aReadonly
                            ):
        iSession( aSession ), iTestRecordHandle( aTestRecordHandle ), 
        iReadonly (aReadonly), iCompletedLogTest( EFalse )
    {
	
    }

// ---------------------------------------------------------------------------
// Read-only subsession or not
// ---------------------------------------------------------------------------
// 
TBool CDiagResultsDbTestRecordSubsession::ReadonlySubsession() const
    {
    return iReadonly;
    }

// ---------------------------------------------------------------------------
// Retrieve session params.
// ---------------------------------------------------------------------------
// 
void CDiagResultsDbTestRecordSubsession::GetEngineParamL( const RMessage2& aMessage )
    {    
    iSession->ReadBufferL( aMessage, 0, iBuffer );

	RBufWriteStream stream ( *iBuffer );
	CleanupClosePushL( stream );

    iTestRecordHandle->GetEngineParam().ExternalizeL( stream );    
    
    stream.CommitL();
    
	CleanupStack::PopAndDestroy( &stream );
	
	 // if client buffer is not big enough	    
    if ( iBuffer->Ptr(0).Length() > aMessage.GetDesMaxLength(0) )
        {
        User::Leave( KErrOverflow );
        }
    
    aMessage.Write( 0, iBuffer->Ptr(0) ); //write to client's address space 
    }


// ---------------------------------------------------------------------------
// Log data into the store. This does not write data into the file.
// ---------------------------------------------------------------------------
// 
void CDiagResultsDbTestRecordSubsession::LogTestResultL( 
                                                const RMessage2& aMessage )
    {
    if ( Completed() )
        {
        User::Leave( KErrAlreadyExists );
        }
        
    if ( ReadonlySubsession() )
        {
        User::Leave( KErrAccessDenied  );
        }
        
    //Writing anymore results into suspended record is considered as resume   
    iTestRecordHandle->RecordInfo().iRecordStatus = 
                                    TDiagResultsDatabaseTestRecordInfo::EOpen;
        
    iSession->ReadBufferL( aMessage, 0, iBuffer );
    
    RBufReadStream stream( *iBuffer );
    CleanupClosePushL ( stream );
    
    CDiagResultsDatabaseItem * item = 
                            CDiagResultsDatabaseItem::NewL ( stream );
    CleanupStack::PushL( item );    

    //Update handle + write data into the file. Handle and DB must be in sync.
    TInt error = iSession->Store().CompleteTestResult( ETrue, 
                                                       *iTestRecordHandle, 
                                                       *item );
    
    //item has been written, so delete the object
    CleanupStack::PopAndDestroy( item ); 
    CleanupStack::PopAndDestroy( &stream );
    
    iCompletedLogTest = ETrue;
    
    // LogTestResult is asynchronous.
    if( error == KErrNone )
        {
        iSession->HasWritten();
        aMessage.Complete( KErrNone );   
        }
    else 
        {
        if ( error == KErrDiskFull )
            {
            iSession->DoNotCompact();
            }
            
        aMessage.Complete ( error );
        }
    }


// ---------------------------------------------------------------------------
// Cancel LogTestResultL.
// ---------------------------------------------------------------------------
// 
void CDiagResultsDbTestRecordSubsession::CancelLogTestResult( 
                                               const RMessage2& /*aMessage*/ )
    {
    if (!iMsg.IsNull() && !iCompletedLogTest )
        {  
        iMsg.Complete( KErrCancel ); //Complete client's request. 
        }
    }

    
// ---------------------------------------------------------------------------
// Returns the test record.
// ---------------------------------------------------------------------------
// 
CDiagResultsDbTestRecordHandle* CDiagResultsDbTestRecordSubsession::CurrentTestRecordHandle()
    {
    return iTestRecordHandle;
    }

// ---------------------------------------------------------------------------
// Finds one test result and serialize it. If it is not found, return KErrNone
// ---------------------------------------------------------------------------
// 
void CDiagResultsDbTestRecordSubsession::GetTestResultL( 
                                                   const RMessage2& aMessage )
    {
    iSession->ReadBufferL( aMessage, 1, iBuffer );
    
    TPckgBuf<TUid> uidpckg;
	aMessage.Read(0, uidpckg);
    TUid testUid = uidpckg(); 
	
	//Find right stream Id from the handle
	TStreamId id;
	TInt error = iTestRecordHandle->MatchingStreamId( testUid, id );
	
	User::LeaveIfError ( error );
	
	CDiagResultsDatabaseItem* item = 
	     iSession->Store().OpenExistingTestResultL( id );
	CleanupStack::PushL( item );
	  
	RBufWriteStream stream ( *iBuffer );
	CleanupClosePushL( stream );
	item->ExternalizeL( stream );
	stream.CommitL();
	CleanupStack::PopAndDestroy( &stream );
	
	CleanupStack::PopAndDestroy( item );
	          
	if ( iBuffer->Size() == 0 ) // Item was not found
	    {
	    User::Leave ( KErrNotFound );
	    }
	
	 // if client buffer is not big enough	    
    if ( iBuffer->Ptr(0).Length() > aMessage.GetDesMaxLength(1) )
        {
        User::Leave( KErrOverflow );
        }
    
    aMessage.Write( 1, iBuffer->Ptr(0) ); //write to client's address space 
   
    }

// ---------------------------------------------------------------------------
// Complete test record i.e. write it to file in parts.
// ---------------------------------------------------------------------------
// 
void CDiagResultsDbTestRecordSubsession::CompleteTestRecordL( 
                                                   const RMessage2& aMessage )
    {
    if ( Completed() )
        {
        User::Leave( KErrAlreadyExists );
        }
    if ( ReadonlySubsession() )
        {
        User::Leave( KErrAccessDenied  );
        }    
    
    TPckgBuf<TBool> completeFullyPckg;
	aMessage.Read(0, completeFullyPckg);
    TBool completeFully = completeFullyPckg();
    
    if ( completeFully )
        {
        iTestRecordHandle->RecordInfo().iRecordStatus = 
                                TDiagResultsDatabaseTestRecordInfo::ECompleted;
                                
        iTestRecordHandle->RecordInfo().iCompleted = ETrue;
        }
    else // partial complete. iRecordStatus is not changed.
        {
        iTestRecordHandle->RecordInfo().iCompleted = ETrue;
        }
    
    //Cleanup is not necessary if logging failed.
    TInt error = iSession->Store().CompleteRecord( *iTestRecordHandle );
    
    if ( error == KErrDiskFull )
            {
            iSession->DoNotCompact();
            }
   
    User::LeaveIfError ( error );         
    
    iSession->HasWritten();
    
    iSession->Store().CleanUpDatabaseL( ETrue );
    }


// ---------------------------------------------------------------------------
// Suspend test record i.e. write it to file in parts.
// ---------------------------------------------------------------------------
// 
void CDiagResultsDbTestRecordSubsession::SuspendTestRecordL( 
                                                   const RMessage2& /*aMessage*/ )
    {
    if ( Completed() )
        {
        User::Leave( KErrAlreadyExists );
        }
    if ( ReadonlySubsession() )
        {
        User::Leave( KErrAccessDenied  );
        }    
    
    iTestRecordHandle->RecordInfo().iRecordStatus = 
                                TDiagResultsDatabaseTestRecordInfo::ESuspended;

    iTestRecordHandle->RecordInfo().iCompleted = EFalse;
                                
    TInt error = iSession->Store().CompleteRecord( *iTestRecordHandle );
    
     if ( error == KErrDiskFull )
            {
            iSession->DoNotCompact();
            }
   
    User::LeaveIfError ( error );         
        
    iSession->Store().CleanUpDatabaseL( ETrue );
    
    iSession->HasWritten();
    }

// ---------------------------------------------------------------------------
// Observers function. Observers store after CompleteTestRecordL has been
// called.
// ---------------------------------------------------------------------------
// 
void CDiagResultsDbTestRecordSubsession::CompletedRecordL( TInt aError )
    {
    iMsg.Complete( aError ); // complete async function
    }


// ---------------------------------------------------------------------------
// Get test results of a test record.
// ---------------------------------------------------------------------------
// 
void CDiagResultsDbTestRecordSubsession::GetTestResultsL( 
                                        CDiagResultsDbTestRecordHandle* aHandle, 
                                        const RMessage2& aMessage )
    {
    iSession->ReadBufferL( aMessage, 0, iBuffer );
    
    	
    RBufWriteStream stream ( *iBuffer );
    CleanupClosePushL ( stream );
    stream.WriteInt16L( aHandle->Count() );
	
	CDiagResultsDbTestRecord* testrecord = 
	    iSession->Store().OpenExistingRecordL( TUid::Uid(aHandle->RecordId().Value()), ETrue );
	CleanupStack::PushL (testrecord);
	
	for ( TInt i = 0; i < aHandle->Count(); ++i )
	    {
	    const CDiagResultsDatabaseItem& item = (*testrecord)[i];
	    item.ExternalizeL( stream );
	    }
	    
    CleanupStack::PopAndDestroy( testrecord );
    
    // if client buffer is not big enough
    if ( iBuffer->Ptr(0).Length() > aMessage.GetDesMaxLength(0) ) 
        {
        User::Leave( KErrOverflow );
        }

    aMessage.Write( 0, iBuffer->Ptr(0) );  //write to client's address space
    
    CleanupStack::PopAndDestroy( &stream );
    } 

// ---------------------------------------------------------------------------
// Get UIDs of test results that are stored inside the test record.
// ---------------------------------------------------------------------------
//     
void CDiagResultsDbTestRecordSubsession::GetTestUidsL( 
                                            const RMessage2& aMessage )
    {
    iSession->ReadBufferL( aMessage, 0, iBuffer );
    
   	RBufWriteStream stream ( *iBuffer );
   	CleanupClosePushL( stream );

   	//Write how many uids there are.
   	stream.WriteInt16L( iTestRecordHandle->Count() );
   	
	for (TInt i = 0; i < iTestRecordHandle->Count(); ++i)
	    {
	    stream.WriteInt32L( iTestRecordHandle->Get(i).iTestUid.iUid ) ;        
	    }
	        
	// if client buffer is not big enough        
    if ( iBuffer->Ptr(0).Length() > aMessage.GetDesMaxLength(0) ) 
        {
        User::Leave( KErrOverflow );
        }
    
    
    aMessage.Write( 0, iBuffer->Ptr(0) ); //write to client's address space 
   
    CleanupStack::PopAndDestroy( &stream );
    }
