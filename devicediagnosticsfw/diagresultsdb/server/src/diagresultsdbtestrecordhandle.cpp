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


#include "diagresultsdbtestrecordhandle.h"
#include "diagresultsdbrecordengineparam.h"


// ---------------------------------------------------------------------------
// Constructor. 
// ---------------------------------------------------------------------------
// 
CDiagResultsDbTestRecordHandle::CDiagResultsDbTestRecordHandle(
                               TStreamId aRecordUid, 
	                           TUid aDbUid,
	                           CDiagResultsDbRecordEngineParam* aEngineParam )
	{
	iRecordId = aRecordUid;
    RecordInfo().iRecordId = TUid::Uid(aRecordUid.Value());
	RecordInfo().iDbUid = aDbUid;
	iEngineParam = aEngineParam;
	}

// ---------------------------------------------------------------------------
// Constructor. 
// ---------------------------------------------------------------------------
// 
CDiagResultsDbTestRecordHandle::CDiagResultsDbTestRecordHandle()
	{
	}

// ---------------------------------------------------------------------------
// Return record ID.
// ---------------------------------------------------------------------------
// 
TStreamId CDiagResultsDbTestRecordHandle::RecordId() const
    {
    return iRecordId;
    }

// ---------------------------------------------------------------------------
// NewL.
// ---------------------------------------------------------------------------
// 
CDiagResultsDbTestRecordHandle* CDiagResultsDbTestRecordHandle::NewL(
                               TStreamId aRecordUid, 
	                           TUid aDbUid,
	                           CDiagResultsDbRecordEngineParam* aEngineParam )
	{
	CDiagResultsDbTestRecordHandle* testrecord =	
	    new (ELeave) CDiagResultsDbTestRecordHandle( 
	                                            aRecordUid, 
	                                            aDbUid,
	                                            aEngineParam );
	CleanupStack::PushL( testrecord );
	testrecord->ConstructL();
	CleanupStack::Pop();
	return testrecord;
	}

// ---------------------------------------------------------------------------
// NewL.
// ---------------------------------------------------------------------------
// 
CDiagResultsDbTestRecordHandle* CDiagResultsDbTestRecordHandle::NewL( RReadStream& aStream )
    {
    
    CDiagResultsDbTestRecordHandle* testrecord =	
	    new (ELeave) CDiagResultsDbTestRecordHandle( );
	CleanupStack::PushL( testrecord );
	testrecord->ConstructL( aStream );
	CleanupStack::Pop();
	return testrecord;
    }
    

// ---------------------------------------------------------------------------
// ConstructL
// ---------------------------------------------------------------------------
//    
void CDiagResultsDbTestRecordHandle::ConstructL( RReadStream& aStream )
    {
    InternalizeL ( aStream );
    }
   
   
// ---------------------------------------------------------------------------
// ConstructL. Set test record overview's DRM and home times.
// ---------------------------------------------------------------------------
// 
void CDiagResultsDbTestRecordHandle::ConstructL()
	{
	//Time when this test record was created.
	iTestRecordInfo.iDrmStartTime = 
	                            TDiagResultsDatabaseTestRecordInfo::DRMTimeL();
	iTestRecordInfo.iStartTime.HomeTime();
	}



// ---------------------------------------------------------------------------
// Destructor. 
// ---------------------------------------------------------------------------
// 
CDiagResultsDbTestRecordHandle::~CDiagResultsDbTestRecordHandle()
    {
    iHandleArray.Close();
    
    delete iEngineParam;
    iEngineParam = NULL;
    }

// ---------------------------------------------------------------------------
// Return record info.
// ---------------------------------------------------------------------------
//    
TDiagResultsDatabaseTestRecordInfo& CDiagResultsDbTestRecordHandle::RecordInfo()
    {
    return iTestRecordInfo;
    }


// ---------------------------------------------------------------------------
// Add one result handle into the record.
// We can use aItem to search the item from the store.
// ---------------------------------------------------------------------------
//  
void CDiagResultsDbTestRecordHandle::AddL( TTestResultHandle& aItem )
    {
    iTestRecordInfo.iTestCount++;
    iHandleArray.AppendL( aItem );
    }


// ---------------------------------------------------------------------------
// Return matching streamId.
// ---------------------------------------------------------------------------
//        
const TStreamId& CDiagResultsDbTestRecordHandle::operator[] (TInt aIndex) const
    {
    return iHandleArray[aIndex].iStreamId;
    }
   

// ---------------------------------------------------------------------------
// Get handle to a test result.
// ---------------------------------------------------------------------------
//     
CDiagResultsDbTestRecordHandle::TTestResultHandle 
                        CDiagResultsDbTestRecordHandle::Get( TInt aIndex ) const
    {
    return iHandleArray[aIndex];
    }   
   
// ---------------------------------------------------------------------------
// Convert plug-in uid into stream id.
// ---------------------------------------------------------------------------
//  
 TInt CDiagResultsDbTestRecordHandle::MatchingStreamId( TUid aTestUid, TStreamId& aStreamId ) const
    {
     
    for ( TInt i=0; i < iHandleArray.Count(); ++i )
        {
        if ( iHandleArray[i].iTestUid == aTestUid )
            {
            aStreamId = iHandleArray[i].iStreamId;
            return KErrNone;
            }
        }    
    
    aStreamId = 0;
    return KErrNotFound;    
    }   
    

// ---------------------------------------------------------------------------
// Returns count.
// ---------------------------------------------------------------------------
//     
TInt CDiagResultsDbTestRecordHandle::Count() const
    {
    return iHandleArray.Count();
    }
   

// ---------------------------------------------------------------------------
// Return the index of uid.
// ---------------------------------------------------------------------------
//       
TInt CDiagResultsDbTestRecordHandle::FindIndex ( TUid aTestUid ) const
    {
    
    for ( TInt i=0; i < iHandleArray.Count(); ++i )
        {
        if ( iHandleArray[i].iTestUid == aTestUid )
            {
            return i;
            }
        }
        
    return KErrNotFound;
    }    

// ---------------------------------------------------------------------------
// Check does the uid exists in the array.
// ---------------------------------------------------------------------------
//     
TBool CDiagResultsDbTestRecordHandle::Find ( TUid aTestUid ) const
    {
    
    for ( TInt i=0; i < iHandleArray.Count(); ++i )
        {
        if ( iHandleArray[i].iTestUid == aTestUid )
            {
            return ETrue;
            }
        }
        
    return EFalse;
    }      
   
   
// ---------------------------------------------------------------------------
// Remove result handle from the array.
// ---------------------------------------------------------------------------
//    
void CDiagResultsDbTestRecordHandle::RemoveL( TStreamId& aItem )
    {
    
    for (TInt i=0; i < iHandleArray.Count(); ++i)
        {
        if ( iHandleArray[i].iStreamId == aItem )
            {
            iHandleArray.Remove( i );
            break;
            }
        }
    }


// ---------------------------------------------------------------------------
// Remove result handle from the array.
// ---------------------------------------------------------------------------
//    
void  CDiagResultsDbTestRecordHandle::RemoveL( TUid aItemUid )
    {
     for (TInt i=0; i < iHandleArray.Count(); ++i)
        {
        if ( iHandleArray[i].iTestUid == aItemUid )
            {
            iHandleArray.Remove( i );
            break;
            }
        }
    }


// ---------------------------------------------------------------------------
// Update one result handle. This can be done when one stream(test result)
// is moved into the last results buffer.
// ---------------------------------------------------------------------------
//  
void CDiagResultsDbTestRecordHandle::UpdateL( TTestResultHandle& aResultHandle )
    {
    TInt index = FindIndex( aResultHandle.iTestUid );
    
    if ( index != KErrNone )
        {
        User::Leave( KErrNotFound );
        }
        
    iHandleArray[index] = aResultHandle;    
    }


// ---------------------------------------------------------------------------
// Simple Get.
// ---------------------------------------------------------------------------
//  
const CDiagResultsDbRecordEngineParam& CDiagResultsDbTestRecordHandle::GetEngineParam() const
    {
    return *iEngineParam;
    }
  
  
// ---------------------------------------------------------------------------
// Externalize Handle into a stream. Typically this is used when
// handle is externalized into the store.
// ---------------------------------------------------------------------------
//   
void CDiagResultsDbTestRecordHandle::ExternalizeL( RWriteStream& aStream ) const
    {
    iRecordId.ExternalizeL( aStream );
    iTestRecordInfo.ExternalizeL( aStream );
    
    TInt count =  Count();
    aStream.WriteInt16L( count );
    
    for (TInt i = 0; i < count; ++i)
        {
        // Write the streamId
        iHandleArray[i].iStreamId.ExternalizeL( aStream ); 
        // Write the plug-uid
        aStream.WriteInt32L( iHandleArray[i].iTestUid.iUid ); 
        }
        
    iEngineParam->ExternalizeL(aStream);
    } 


// ---------------------------------------------------------------------------
// Internalize from a stream. Typically this is used when handle is read
// from the store.
// ---------------------------------------------------------------------------
//     
void CDiagResultsDbTestRecordHandle::InternalizeL( RReadStream& aStream )
    {
    iRecordId.InternalizeL( aStream );
    iTestRecordInfo.InternalizeL( aStream );
    
    TInt count = aStream.ReadInt16L();
    
    for (TInt i = 0; i < count; ++i)
        {
        TTestResultHandle handle;
        handle.iStreamId.InternalizeL( aStream );
        handle.iTestUid.iUid = aStream.ReadInt32L();
        iHandleArray.Append( handle );
        }
    
    iEngineParam = CDiagResultsDbRecordEngineParam::NewL ( aStream );
    
    }
            

