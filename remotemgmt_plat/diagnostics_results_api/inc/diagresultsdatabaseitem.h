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
* Description:  Results database item
*
*/


#ifndef DIAGRESULTSDATABASEITEM_H
#define DIAGRESULTSDATABASEITEM_H

// INCLUDES
#include <e32base.h>            // CBase

// FORWARD DECLARATIONS
class RWriteStream;
class RReadStream;
class TTime;
class TStreamId;
class CStreamStore;
class CBufFlat;

/**
* Results database item.
*
* @since S60 v5.0
**/
class CDiagResultsDatabaseItem : public CBase
    {
public:
    /** 
    * Result of test.
    */
    enum TResult
        {
        ESuccess = 0,
        EFailed,            // Test failed
        ESkipped,           // Test was never executed because it was skipped 
                            // before it is started. (e.g. during initial delay timer)
        ECancelled,         // Test session was cancelled. ( ECancelAll )
        EInterrupted,       // Interrupted. Test was already running, but interrupted 
                            // by end user, or other external cause.
        ENotPerformed,      // Not performed due to invalid set up. 
        EDependencyFailed,  // Failed because dependent test failed.
        EWatchdogCancel,    // Watchdog cancelled testing.
        ESuspended,         // Test is scheduled to run later.
        EQueuedToRun,       // Test is waiting for execution. It is possible that 
                            // EQueuedToRun is never overwritten.
        EDependencySkipped  // Skipped because common dependency was not executed
                            // either due to it was skipped or interrupted.
        };

public:

    /**
    * NewL. Construct from a stream.
    * @param aStream   Stream to read from.
    */
    IMPORT_C static CDiagResultsDatabaseItem* NewL ( RReadStream& aStream );
        
    /**
    * NewL. 
    */
    IMPORT_C static CDiagResultsDatabaseItem* NewL (
                                        TUid            aTestUid,
                                        TBool           aDependencyExecution,
                                        TResult         aResult,
                                        TTime           aStartTime,
                                        TTime           aEndTime,
                                        CBufFlat*       aDetailsData );
    /**
    * NewLC.
    */
    IMPORT_C static CDiagResultsDatabaseItem* NewLC (
                                        TUid            aTestUid,
                                        TBool           aDependencyExecution,
                                        TResult         aResult,
                                        TTime           aStartTime,
                                        TTime           aEndTime,
                                        CBufFlat*       aDetailsData );

    /**
    * NewL. Copy constructor. This will do a deep copy.
    *
    * @param aOriginal - Original to copy data from.
    * @return a new instance of CDiagResultsDatabaseItem that is a deep
    *   copy of the original given. Ownership is transferred.
    */
    IMPORT_C static CDiagResultsDatabaseItem* NewL ( 
        const CDiagResultsDatabaseItem& aOriginal );
    /**
    * Destructor.
    */
    IMPORT_C virtual ~CDiagResultsDatabaseItem();
   
    /**
    * Get Uid of test plug-in. 
    *
    * @return TUid - uid of the test plug-in.
    */
    IMPORT_C TUid TestUid() const;
    
    /**
    * Whether the test was executed to satisfy dependency or not.
    *
    * @return ETrue if executed as dependency.
    *         EFalse otherwise.
    */
    IMPORT_C TBool WasDependency() const;
    
    /**
    * Get Test Result.
    *
    * @return TResult.
    */
    IMPORT_C TResult TestResult() const;

    /**
    * Time that test execution was started.
    * Universal time is used.
    *
    * @return Time that execution was started.
    */
    IMPORT_C TTime TimeStarted() const;

    /**
    * Time that test execution was completed.
    * Universal time is used.
    *
    * @return Time that execution was completed.
    */
    IMPORT_C TTime TimeCompleted() const;

    /**
    * Details data. This must be interpreted by the plug-ins before
    * it can be used. Data is in object form.
    */
    IMPORT_C const CBufFlat* DetailsData() const;

    /**
    * Calculate stream size in bytes. Size can be used to evaluate needed
    * stream size. 
    *
    * @return Size in bytes.
    */
    IMPORT_C TInt Size() const;

public: 

    /**
    * Externalize the test result to a stream.
    *
    * @param aStream   Stream to write to.
    */
    IMPORT_C void ExternalizeL( RWriteStream& aStream ) const; 
    
    
private: //internalization methods   

    /**
    * Internalize the test result from a stream.
    *
    * @param aStream   Stream to read from.
    */
    IMPORT_C void InternalizeL( RReadStream& aStream );
    
    /**
    * Write TTime into write stream.
    * 
    * @param aStream Stream to write to.
    * @param aTime TTime that is written into the stream.
    **/
    void WriteTimeToStreamL( RWriteStream& aStream, const TTime& aTime ) const;
    
    /**
    * Read TTime from read stream.
    * 
    * @param aStream read stream.
    * @param aTime Contains time after stream is read.
    **/
    void ReadTimeFromStreamL( RReadStream& aStream, TTime& aTime );

private:
    
    /**
    * C++ Constructors.
    */
    CDiagResultsDatabaseItem (
                        TUid            aTestUid,
                        TBool           aDependencyExecution,
                        TResult         aResult,
                        TTime           aStartTime,
                        TTime           aEndTime,
                        CBufFlat*       aDetailsData );
    
    /**
    * Construct from a stream.
    *
    * @param aStream Read stream.
    */ 
    void ConstructL ( RReadStream& aStream );
    
    /**
    * Default C++ constructor.
    **/
    CDiagResultsDatabaseItem ();
    
private:    // data

    // Uid of the test plug-in.
    TUid            iTestUid;
    
    // Has this been executed as a part of another execution (precondition).
    TBool           iDependencyExecution;
    
    // Test result
    TResult         iResult;
    
    // When the test began.
    TTime           iStartTime;
    
    // When the test ended
    TTime           iEndTime;
    
    // Contains flat buffer that can hold additional data 
    // For example debugging data or further analysis.
    CBufFlat*       iDetailsData;

    };

#endif // DIAGRESULTSDATABASEITEM_H
