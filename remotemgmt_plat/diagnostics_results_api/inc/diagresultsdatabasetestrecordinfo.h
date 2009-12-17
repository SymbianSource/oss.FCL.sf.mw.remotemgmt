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
* Description:  Contains overview of the test record, not actual test results.
*  libraries   : DiagResultsDatabase.lib DiagResultsDatabase.dll
*
*/


#ifndef DIAGNOSTICS_RESULTS_DATABASE_TEST_RECORD_INFO_H
#define DIAGNOSTICS_RESULTS_DATABASE_TEST_RECORD_INFO_H

#include <e32base.h>

class RReadStream;
class RWriteStream;

/**
* Results database record info. Contains information about one record.
*
* @since S60 v5.0
**/
class TDiagResultsDatabaseTestRecordInfo
    {
public:

    enum TDbRecordStatus
        {      
        EOpen,                // Currently open for writing.
        ESuspended,   	      // ::Suspend() is called. 
        ECompleted	          // Record is completed (not resumable), and all 
                              // tests were completed. 
        };


    /**
    * Constructor
    **/
    IMPORT_C TDiagResultsDatabaseTestRecordInfo();
    
    /**
    * Internalize from a stream.
    *
    * @param aStream Stream to be read from.
    **/
    IMPORT_C void InternalizeL( RReadStream& aStream );

    /**
    * Externalize into a stream.
    *
    * @param aStream Stream to be written into.
    **/
    IMPORT_C void ExternalizeL( RWriteStream& aStream ) const;
    
    /**
    * Returns DRM time.
    *
    * @return DRM time.
    **/
    IMPORT_C static TTime DRMTimeL();

    //Public member data.
    TUid            iRecordId;   // Identifies this record.
    TTime 		    iDrmStartTime;  //When the record was opened (DRM time).
    TTime 		    iDrmFinishTime; //when the record was completed (DRM time).
    TTime 		    iStartTime;  //When the record was opened (Home time).
    TTime 		    iFinishTime; //when the record was completed (Home time).
    TUid 	        iDbUid; 	 //UID3 of the database.
    TUint 		    iTestCount;  //The number of tests in this record.
    TDbRecordStatus iRecordStatus; //Stored record status. 
    TBool           iCompleted; //Writable or read-only test record
    

private:

    /**
    * Writes time(TTime) into a stream. Time has to be written into a stream 
    * using two 32-bit values since there is no method to write 64-bit value.
    *
    * @param aStream Stream to be written into.
    * @param aTime Time that is written into the stream.
    **/
    void WriteTimeToStreamL( RWriteStream& aStream, const TTime& aTime ) const;
    
    /**
    * Reads time (TTime) from a stream. 
    *
    * @param aStream Stream to be read from.
    * @param aTime Time that was read from the stream.
    **/
    void ReadTimeFromStreamL( RReadStream& aStream, TTime& aTime );
    
    };
    
#endif    