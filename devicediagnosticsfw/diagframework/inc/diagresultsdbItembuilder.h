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
* Description:  Diagnostics Results Database Item Builder
*
*/



#ifndef DIAGRESULTSDBITEMBUILDER_H
#define DIAGRESULTSDBITEMBUILDER_H

// INCLUDES
#include <DiagResultsDatabaseItem.h>        // CDiagResultsDatabaseItem::TResult

// FORWARD DECLARATIONS
namespace DRM
    {
    class CDrmServiceApi;
    }

class MDiagResultDetail;


/**
*  Diagnostics Result Database Item Builder.
*
*  This class can be used to build CDiagResultsDatabaseItem, without having
*  to have all the values avaliable.
*
*  @since S60 v5.0
*/
NONSHARABLE_CLASS( CDiagResultsDbItemBuilder ) : public CBase
    {
public:  // Constructors and Destructors
    /**
    * Two-phase constructor.
    *
    * This will set the test started time to now.
    *
    * @param aTestUid - Test Uid.
    * @param aDependencyExecution - Whether thsa was executed as dependency.
    * @return New instance of CDiagResultsDbItemBuilder
    */
    IMPORT_C static CDiagResultsDbItemBuilder* NewL( TUid  aTestUid,
                                                     TBool aDependencyExecution );
    IMPORT_C static CDiagResultsDbItemBuilder* NewLC( TUid  aTestUid,
                                                      TBool aDependencyExecution );

    /**
    * Destructor
    *
    */
    virtual ~CDiagResultsDbItemBuilder();

public: // New Interface

    /**
    * Set Test Uid
    *
    * @param aUid - Test uid.
    */
    IMPORT_C void SetTestUid( TUid aUid );

    /**
    * Get Test Uid
    *
    * @return Test uid.
    */
    IMPORT_C TUid TestUid() const;

    /**
    * Get current time. If DRM clock is available, it will use the DRM clock.
    *
    */
    IMPORT_C TTime GetCurrentTime() const;

    /**
    * Set whether test was executed as dependency or not.
    *
    * @param aWasDependency
    */
    IMPORT_C void SetWasDependency( TBool aWasDependency );

    /**
    * Get was dependency value
    *
    * @return ETrue if test was executed as a dependency.
    */
    IMPORT_C TBool WasDependency() const;

    /**
    * Update test result and also set completed time to now
    *
    * @param aResult - Test result.
    */
    IMPORT_C void SetTestCompleted( CDiagResultsDatabaseItem::TResult aResult );

    /**
    * Get Test result
    *
    * @return Test result.
    */
    IMPORT_C CDiagResultsDatabaseItem::TResult TestResult() const;

    /**
    * Set Time Started 
    *
    * @param aTime - time started.
    */
    IMPORT_C void SetTimeStarted( TTime aTime );

    /**
    * Get Time started.
    *
    * @return Time started.
    */
    IMPORT_C TTime TimeStarted() const;

    /**
    * Set Time Completed
    *
    * @param aTime - time completed.
    */
    IMPORT_C void SetTimeCompleted( TTime aTime );

    /**
    * Get Time Completed..
    *
    * @return Time Completed.
    */
    IMPORT_C TTime TimeCompleted() const;

    /**
    * Set result detail object. If it was already set, previous object is deleted.
    *
    * @param aResultDetail - Result detail object. Ownership is transferred.
    */
    IMPORT_C void SetResultDetail( MDiagResultDetail* aResultDetail );

    /**
    * Get result detail object. Use static_cast to change to specific instance.
    *   If result detail was not set previosuly, it will Panic with 
    *   EDiagFrameworkInvalidState
    *
    * @return Result detail object.
    */
    IMPORT_C MDiagResultDetail& ResultDetail() const;

    /**
    * Create CDiagResultsDatabaseItem from data available.
    *
    */
    IMPORT_C CDiagResultsDatabaseItem* ToResultsDatabaseItemL() const;

    /**
    * Create a simple CDiagResultsDatabaseItem. This function is meant to be
    * used to create default result data with just with minimal set of information.
    * It will use current time as both start and end time of test.
    * If manipulation of other information in needed, either instantiate
    * CDiagResultsDbItemBuilder or CDiagResultsDatabaseItem
    *
    * @param aTestUid - Uid of test plugin.
    * @param aAsDependency - Whether it was executed as dependency or not.
    * @param aResultType - Result of the test.
    * @return Instance of CDiagResultsDatabaseItem* with given info. 
    *   Ownership is transferred.
    */
    IMPORT_C static CDiagResultsDatabaseItem* CreateSimpleDbItemL( 
        TUid aTestUid,
        TBool aAsDependency,
        CDiagResultsDatabaseItem::TResult aResultType );


protected:  // constructors
    /**
    * C++ Constructor
    *
    * @param aTestUid - Uid of the test
    * @param aDependencyExecution - Whether this is being executed as dependency.
    */
    CDiagResultsDbItemBuilder( TUid  aTestUid,
                               TBool aDependencyExecution );
    /**
    * ConstructL
    *
    */
    void ConstructL();

private: // Data
    /**
    * iTestUid - Uid of the test plug-in that this result is for.
    */
    TUid iTestUid;

    /**
    * iDependencyExecution - Whether this test is being executed to 
    *   satisfy dependency or not.
    */
    TBool iDependencyExecution;

    /**
    * iResut - Actual result of the test.
    */
    CDiagResultsDatabaseItem::TResult iResult;

    /**
    * iStartTime - Time test started.
    */
    TTime iStartTime;

    /**
    * iEndTime - Time test finished.
    */
    TTime iEndTime;

    /**
    * iResultDetail - Extra details data.
    * Owns.
    */
    MDiagResultDetail* iResultDetail;

    /**
    * iDrmApi - Used to get accurate clock through DRM.
    * Owns.
    */
    DRM::CDrmServiceApi* iDrmApi;
    };


#endif // DIAGRESULTSDBITEMBUILDER_H

// End of File

