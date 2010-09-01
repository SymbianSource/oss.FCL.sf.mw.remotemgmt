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
* Description:  Class definition of TDiagResultsDatabaseTestRecordInfo
*
*/


// SYSTEM INCLUDE FILES
#include <s32strm.h>
#include <s32std.h>
#include <drmserviceapi.h>

// USER INCLUDE FILES
#include "diagresultsdatabasetestrecordinfo.h"

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// Constructor.
// ---------------------------------------------------------------------------
//
EXPORT_C TDiagResultsDatabaseTestRecordInfo::TDiagResultsDatabaseTestRecordInfo()
    {
    iRecordId = TUid::Null();
    iDbUid = TUid::Null();
    iTestCount = 0;
    iCompleted = EFalse;
    }

// ---------------------------------------------------------------------------
// Read TDiagResultsDatabaseTestRecordInfo object from a stream.
// ---------------------------------------------------------------------------
//    
EXPORT_C void TDiagResultsDatabaseTestRecordInfo::InternalizeL( 
                                                RReadStream& aStream )
    {
    iRecordId = TUid::Uid( aStream.ReadInt32L() );
    
    ReadTimeFromStreamL( aStream, iDrmStartTime ); 
    ReadTimeFromStreamL( aStream, iDrmFinishTime );
    
    ReadTimeFromStreamL( aStream, iStartTime ); 
    ReadTimeFromStreamL( aStream, iFinishTime );
    
    iDbUid = TUid::Uid( aStream.ReadInt32L() );
    iTestCount = aStream.ReadInt32L();
    
    iRecordStatus = (TDbRecordStatus) aStream.ReadUint8L();
    
    iCompleted = (TBool) aStream.ReadUint8L();
    }    

// ---------------------------------------------------------------------------
// Write TDiagResultsDatabaseTestRecordInfo object into a stream.
// ---------------------------------------------------------------------------
//       
EXPORT_C void TDiagResultsDatabaseTestRecordInfo::ExternalizeL( 
                                                RWriteStream& aStream ) const
    {
    aStream.WriteInt32L( iRecordId.iUid );
    
    WriteTimeToStreamL( aStream, iDrmStartTime );
    WriteTimeToStreamL( aStream, iDrmFinishTime );
    
    WriteTimeToStreamL( aStream, iStartTime );
    WriteTimeToStreamL( aStream, iFinishTime );
    
    aStream.WriteInt32L( iDbUid.iUid );
    aStream.WriteInt32L( iTestCount );

    aStream.WriteUint8L( iRecordStatus );

    aStream.WriteUint8L( iCompleted );
    }   

// ---------------------------------------------------------------------------
// Helper function to write TTime into a stream.
// ---------------------------------------------------------------------------
//       
void TDiagResultsDatabaseTestRecordInfo::WriteTimeToStreamL( 
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
// Helper function to read TTime from a stream.
// ---------------------------------------------------------------------------
//  
void TDiagResultsDatabaseTestRecordInfo::ReadTimeFromStreamL( 
                                                RReadStream& aStream, 
                                                TTime& aTime ) 
    {
    TInt32 low_time = aStream.ReadInt32L();
    TInt32 high_time = aStream.ReadInt32L();
    
    TInt64 readTime = MAKE_TINT64(high_time,low_time);
    
    aTime = TTime(readTime);
    }      

// ---------------------------------------------------------------------------
// Get secure DRM time.
// ---------------------------------------------------------------------------
//    
EXPORT_C TTime TDiagResultsDatabaseTestRecordInfo::DRMTimeL()
    {
    // Get the current time.
    TTime currTime = 0;
    DRM::CDrmServiceApi* drmApi = DRM::CDrmServiceApi::NewLC();
    TInt timeZone = 0;
    DRMClock::ESecurityLevel secLevel = DRMClock::KInsecure;
    TInt err = drmApi->GetSecureTime( currTime, timeZone, secLevel );
    CleanupStack::PopAndDestroy( drmApi );
    User::LeaveIfError( err );
    return currTime;
    }
