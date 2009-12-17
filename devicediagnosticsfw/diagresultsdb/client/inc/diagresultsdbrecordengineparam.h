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
* Description:  Capsulates parameters of the diagnostics engine.
*
*/


#ifndef DIAGRESULTSDBRECORDENGINEPARAM_H
#define DIAGRESULTSDBRECORDENGINEPARAM_H

// INCLUDES
#include <e32base.h>            // CBase

// FORWARD DECLARATIONS
class RWriteStream;
class RReadStream;

/**
* Contains parameters of the diagnostics engine.
* Parameters are needed only in suspend/resume functionality.
*
* @since S60 v5.0
**/
class CDiagResultsDbRecordEngineParam : public CBase
    {
public:


    /**
    * NewL. Construct from a stream.
    * @param aStream   Stream to read from.
    */
    IMPORT_C static CDiagResultsDbRecordEngineParam* NewL ( RReadStream& aStream );
        
    /**
    * NewL. Ownership of RArray is transferred.
    *
    * @param aInitialUids Execution Uid array. Should not be null. 
    *                     Use empty array if there are no uids.
    * @param aDependencyExecution This value is needed when resuming 
    *                             in order to create proper test resume plan.
    */
    IMPORT_C static CDiagResultsDbRecordEngineParam* NewL (
                                        RArray<TUid>*   aInitialUids,
                                        TBool           aDependencyExecution
                                        );
    /**
    * NewLC. Ownership of RArray is transferred.
    *
    * @param aInitialUids Execution Uid array. Should not be null. 
    *                     Use empty array if there are no uids.
    * @param aDependencyExecution This value is needed when resuming 
    *                             in order to create proper test resume plan.
    */
    IMPORT_C static CDiagResultsDbRecordEngineParam* NewLC (
                                        RArray<TUid>*   aInitialUids,
                                        TBool           aDependencyExecution
                                        );

    /**
    * Destructor.
    */
    IMPORT_C virtual ~CDiagResultsDbRecordEngineParam();
   
    /**
    * Return execution uids. These can be used to store for example 
    * user selections. User selections are needed when test run is suspended 
    * or test session crashed the phone.
    *
    * @return Array of uids. Empty if none.
    */
    IMPORT_C const RArray<TUid>& ExecutionsUidArray() const;
    
    /**
    * Indicates were dependencies resolved when diagnostics engine executed tests.
    *
    * @return ETrue if dependencies were resolved, EFalse otherwise.
    */
    IMPORT_C TBool DependencyExecution() const;
    
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
    
  
private:
    
    /**
    * C++ Constructors.
    */
    CDiagResultsDbRecordEngineParam ( RArray<TUid>* aInitialUids,
                                      TBool         aDependencyExecution );
    
    /**
    * Construct from a stream.
    *
    * @param aStream Read stream.
    */ 
    void ConstructL ( RReadStream& aStream );
    
    /**
    * Default constructL. Does nothing.
    */ 
    void ConstructL ();
    
    /**
    * Default C++ constructor.
    **/
    CDiagResultsDbRecordEngineParam ();
    
private:    // data

    // Execution uid array
    RArray<TUid>*               iExecutionUids;
    
    //Dependency resolving state.
    TBool                       iDependencyExecution;
    
    };

#endif // DIAGRESULTSDBRECORDENGINEPARAM_H
