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
* Description:  Class definition of CDiagResultsDatabaseItem
*
*/


#include <s32std.h> 

#include "diagresultsdatabaseitem.h"

// ---------------------------------------------------------------------------
// NewL.Used when creating the object from a stream that contains the
// serialized object.
// ---------------------------------------------------------------------------
//  
EXPORT_C CDiagResultsDatabaseItem* CDiagResultsDatabaseItem::NewL ( 
                                                        RReadStream& aStream )
    {
    CDiagResultsDatabaseItem* self = new( ELeave ) CDiagResultsDatabaseItem();
    CleanupStack::PushL( self );
    self->ConstructL( aStream );
    CleanupStack::Pop();
    return self;
    }
    
// ---------------------------------------------------------------------------
// NewL. 
// ---------------------------------------------------------------------------
//        
EXPORT_C CDiagResultsDatabaseItem* CDiagResultsDatabaseItem::NewL (
                                        TUid            aTestUid,
                                        TBool           aDependencyExecution,
                                        TResult         aResult,
                                        TTime           aStartTime,
                                        TTime           aEndTime,
                                        CBufFlat*       aDetailsData )
    {
    
    CDiagResultsDatabaseItem* self = new( ELeave ) CDiagResultsDatabaseItem( 
                                    aTestUid, aDependencyExecution, aResult, 
                                    aStartTime, aEndTime, aDetailsData );
    return self;
    }

// ---------------------------------------------------------------------------
// NewLC.
// ---------------------------------------------------------------------------
//   
EXPORT_C CDiagResultsDatabaseItem* CDiagResultsDatabaseItem::NewLC (
                                        TUid            aTestUid,
                                        TBool           aDependencyExecution,
                                        TResult         aResult,
                                        TTime           aStartTime,
                                        TTime           aEndTime,
                                        CBufFlat*       aDetailsData )
    {
    CDiagResultsDatabaseItem* self = new( ELeave ) CDiagResultsDatabaseItem( 
                                        aTestUid, aDependencyExecution, aResult, 
                                        aStartTime, aEndTime, aDetailsData );
    CleanupStack::PushL( self );
    return self;
    }

// ---------------------------------------------------------------------------
// NewL. Copy constructor
// ---------------------------------------------------------------------------
//   
EXPORT_C CDiagResultsDatabaseItem* CDiagResultsDatabaseItem::NewL (
        const CDiagResultsDatabaseItem& aOriginal )
    {
    const CBufFlat* detailData = aOriginal.DetailsData();

    CBufFlat* detailDataCopy = NULL;

    if ( detailData )
        {
        detailDataCopy = CBufFlat::NewL( detailData->Size() );
        CleanupStack::PushL( detailDataCopy );

        TPtr8 ptr = detailDataCopy->Ptr( 0 );
        detailData->Read( 0, ptr );
        }

    CDiagResultsDatabaseItem* self = CDiagResultsDatabaseItem::NewL(
        aOriginal.TestUid(),
        aOriginal.WasDependency(),
        aOriginal.TestResult(),
        aOriginal.TimeStarted(),
        aOriginal.TimeCompleted(),
        detailDataCopy );

    if ( detailDataCopy )
        {
        CleanupStack::Pop( detailDataCopy );
        }

    return self;
    }

// ConstructL.
// ---------------------------------------------------------------------------
//
void CDiagResultsDatabaseItem::ConstructL ( RReadStream& aStream )
    {
    InternalizeL( aStream );
    }

// ---------------------------------------------------------------------------
// Destructor.
// ---------------------------------------------------------------------------
//
EXPORT_C CDiagResultsDatabaseItem::~CDiagResultsDatabaseItem()
    {
    if ( iDetailsData )
        {
        iDetailsData->Reset();
        }
        
    delete iDetailsData;
    iDetailsData = NULL;
    }

// ---------------------------------------------------------------------------
// C++ default constructor.
// ---------------------------------------------------------------------------
//    
CDiagResultsDatabaseItem::CDiagResultsDatabaseItem()
    {
    
    }

// ---------------------------------------------------------------------------
// Getter.
// ---------------------------------------------------------------------------
// 
EXPORT_C TUid CDiagResultsDatabaseItem::TestUid() const
    {
    return iTestUid;
    }
    
// ---------------------------------------------------------------------------
// Getter.
// ---------------------------------------------------------------------------
//
EXPORT_C TBool CDiagResultsDatabaseItem::WasDependency() const
    {
    return iDependencyExecution;
    }
    
// ---------------------------------------------------------------------------
// Getter.
// ---------------------------------------------------------------------------
//
EXPORT_C CDiagResultsDatabaseItem::TResult 
                   CDiagResultsDatabaseItem::TestResult() const
    {
    return iResult;
    }

// ---------------------------------------------------------------------------
// Getter.
// ---------------------------------------------------------------------------
// 
EXPORT_C TTime CDiagResultsDatabaseItem::TimeStarted() const
    {
    return iStartTime;
    }

// ---------------------------------------------------------------------------
// Getter.
// ---------------------------------------------------------------------------
//
EXPORT_C TTime CDiagResultsDatabaseItem::TimeCompleted() const
    {
    return iEndTime;
    }

// ---------------------------------------------------------------------------
// Getter.
// ---------------------------------------------------------------------------
//  
EXPORT_C const CBufFlat* CDiagResultsDatabaseItem::DetailsData() const
    {
    return iDetailsData;
    }

// ---------------------------------------------------------------------------
// Externalize the object into writestream.
// ---------------------------------------------------------------------------
//
EXPORT_C void CDiagResultsDatabaseItem::ExternalizeL(
                                             RWriteStream& aStream ) const
    {
    aStream.WriteInt32L ( iTestUid.iUid );
    aStream.WriteUint8L ( iDependencyExecution );
    aStream.WriteInt16L ( iResult );
    
    WriteTimeToStreamL( aStream, iStartTime );
    WriteTimeToStreamL( aStream, iEndTime );
    
    if ( iDetailsData == NULL )
        {
        aStream.WriteUint32L ( 0 );
        }
    else 
        {
        aStream.WriteUint32L ( iDetailsData->Size() );
    
        if ( iDetailsData->Size() > 0 )
            {
            aStream.WriteL( iDetailsData->Ptr(0), iDetailsData->Size() );    
            }
        }
    }


// ---------------------------------------------------------------------------
// Bytes are calculated from ExternalizeL method. (216 bits = 27 bytes).
// 32 + 8 + 16 + 64 + 64 + 32 = 216 bits.
// ---------------------------------------------------------------------------
//
EXPORT_C TInt CDiagResultsDatabaseItem::Size() const
    {
    if ( iDetailsData == NULL )
        {
        return 27;
        }
    
    return (27 + iDetailsData->Size() );
    }

// ---------------------------------------------------------------------------
// Internalize the object from a stream.
// ---------------------------------------------------------------------------
//
EXPORT_C void CDiagResultsDatabaseItem::InternalizeL( RReadStream& aStream )
    {
    iTestUid = TUid::Uid( aStream.ReadInt32L () );
    iDependencyExecution = aStream.ReadUint8L(); 
    iResult = (TResult ) aStream.ReadInt16L();
    
    ReadTimeFromStreamL( aStream, iStartTime );
    ReadTimeFromStreamL( aStream, iEndTime );
    
    TInt size = aStream.ReadUint32L();
    
    if ( size == 0 )
        {
        iDetailsData = NULL;
        }
    else 
        {
        iDetailsData = CBufFlat::NewL( 50 );
        iDetailsData->ResizeL( size );
        }
    
    
    if ( size > 0 )
        {
        TPtr8 ptr = iDetailsData->Ptr(0);
        aStream.ReadL( ptr, size);    
        }
    }

// ---------------------------------------------------------------------------
// Helper function.
// ---------------------------------------------------------------------------
//    
void CDiagResultsDatabaseItem::WriteTimeToStreamL( 
                                            RWriteStream& aStream, 
                                            const TTime& aTime ) const
    {
    TInt64 time = aTime.Int64(); 
 
    TInt32 low_time = I64LOW( time );
    TInt32 high_time = I64HIGH( time );
 
    aStream.WriteInt32L( low_time );
    aStream.WriteInt32L( high_time );
    }    

// ---------------------------------------------------------------------------
// Helper function.
// ---------------------------------------------------------------------------
//
void CDiagResultsDatabaseItem::ReadTimeFromStreamL( 
                                           RReadStream& aStream, 
                                           TTime& aTime ) 
    {
    TInt32 low_time = aStream.ReadInt32L();
    TInt32 high_time = aStream.ReadInt32L();
    
    TInt64 readTime = MAKE_TINT64(high_time,low_time);
    
    aTime = TTime(readTime);
    }  

// ---------------------------------------------------------------------------
// Constructor that sets the values.
// ---------------------------------------------------------------------------
//
CDiagResultsDatabaseItem::CDiagResultsDatabaseItem (
                        TUid            aTestUid,
                        TBool           aDependencyExecution,
                        TResult         aResult,
                        TTime           aStartTime,
                        TTime           aEndTime,
                        CBufFlat*       aDetailsData )
    {
    iTestUid = aTestUid;
    iDependencyExecution = aDependencyExecution;
    iResult = aResult;
    iStartTime = aStartTime;
    iEndTime = aEndTime;
    iDetailsData = aDetailsData;
    }
    
  
    

