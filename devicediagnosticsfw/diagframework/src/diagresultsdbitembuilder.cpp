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
* Description:  Class definition of CDiagResultsDbItemBuilder
*
*/


// CLASS DECLARATION
#include <DiagResultsDbItemBuilder.h>

// SYSTEM INCLUDE FILES
#include <DiagResultDetail.h>               // MDiagResultDetail

#ifndef __DIAGFW_DISABLE_DRM_API
#include <drmserviceapi.h>                  // DRM::CDrmServiceApi
#endif // __DIAGFW_DISABLE_DRM_API

// USER INCLUDE FILES
#include "diagframework.pan"                // Panic Codes


// CONSTANTS

// ======== MEMBER FUNCTIONS ========
// ---------------------------------------------------------------------------
// CDiagResultsDbItemBuilder::NewL
// ---------------------------------------------------------------------------
//
EXPORT_C CDiagResultsDbItemBuilder* CDiagResultsDbItemBuilder::NewL(
        TUid aTestUid,
        TBool aDependencyExecution )
    {
    CDiagResultsDbItemBuilder* self = CDiagResultsDbItemBuilder::NewLC(
        aTestUid,
        aDependencyExecution );
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// CDiagResultsDbItemBuilder::NewLC
// ---------------------------------------------------------------------------
//
EXPORT_C CDiagResultsDbItemBuilder* CDiagResultsDbItemBuilder::NewLC(
        TUid aTestUid,
        TBool aDependencyExecution )
    {
    CDiagResultsDbItemBuilder* self = new( ELeave )CDiagResultsDbItemBuilder(
        aTestUid,
        aDependencyExecution );
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

// ---------------------------------------------------------------------------
// CDiagResultsDbItemBuilder::CDiagResultsDbItemBuilder
// ---------------------------------------------------------------------------
//
CDiagResultsDbItemBuilder::CDiagResultsDbItemBuilder(
        TUid aTestUid,
        TBool aDependencyExecution )
    :   iTestUid( aTestUid ),
        iDependencyExecution( aDependencyExecution ),
        iResult( CDiagResultsDatabaseItem::ENotPerformed )
    {
    }

// ---------------------------------------------------------------------------
// CDiagResultsDbItemBuilder::ConstructL
// ---------------------------------------------------------------------------
//
void CDiagResultsDbItemBuilder::ConstructL()
    {
#ifndef __DIAGFW_DISABLE_DRM_API
    iDrmApi = DRM::CDrmServiceApi::NewL();
#endif // __DIAGFW_DISABLE_DRM_API

    SetTimeStarted( GetCurrentTime() );
    }

// ---------------------------------------------------------------------------
// CDiagResultsDbItemBuilder::~CDiagResultsDbItemBuilder
// ---------------------------------------------------------------------------
//
CDiagResultsDbItemBuilder::~CDiagResultsDbItemBuilder()
    {
#ifndef __DIAGFW_DISABLE_DRM_API
    delete iDrmApi;
    iDrmApi = NULL;
#endif // __DIAGFW_DISABLE_DRM_API
    delete iResultDetail;
    iResultDetail = NULL;
    }

// ---------------------------------------------------------------------------
// CDiagResultsDbItemBuilder::SetTestUid
// ---------------------------------------------------------------------------
//
EXPORT_C void CDiagResultsDbItemBuilder::SetTestUid( TUid aUid )
    {
    iTestUid = aUid;
    }

// ---------------------------------------------------------------------------
// CDiagResultsDbItemBuilder::TestUid
// ---------------------------------------------------------------------------
//
EXPORT_C TUid CDiagResultsDbItemBuilder::TestUid() const
    {
    return iTestUid;
    }

// ---------------------------------------------------------------------------
// CDiagResultsDbItemBuilder::GetCurrentTime
// ---------------------------------------------------------------------------
//
EXPORT_C TTime CDiagResultsDbItemBuilder::GetCurrentTime() const
    {
    TTime currTime = ( TInt64 )0;
#ifndef __DIAGFW_DISABLE_DRM_API
    TInt timeZone = 0;
    DRMClock::ESecurityLevel secLevel = DRMClock::KInsecure;
    if ( iDrmApi->GetSecureTime( currTime, timeZone, secLevel ) != KErrNone )
        {
        __ASSERT_DEBUG( 0, Panic( EDiagFrameworkInternal ) );
        // if we are unable to get DRM clock, use home clock.
        currTime.UniversalTime();
        }
#endif // __DIAGFW_DISABLE_DRM_API
		 currTime.UniversalTime(); // Added for Time Updation in HardwareSuiteView
    return currTime;
    }

// ---------------------------------------------------------------------------
// CDiagResultsDbItemBuilder::SetWasDependency
// ---------------------------------------------------------------------------
//
EXPORT_C void CDiagResultsDbItemBuilder::SetWasDependency( TBool aWasDependency )
    {
    iDependencyExecution = aWasDependency;
    }

// ---------------------------------------------------------------------------
// CDiagResultsDbItemBuilder::WasDependency
// ---------------------------------------------------------------------------
//
EXPORT_C TBool CDiagResultsDbItemBuilder::WasDependency() const
    {
    return iDependencyExecution;
    }

// ---------------------------------------------------------------------------
// CDiagResultsDbItemBuilder::SetTestCompleted
// ---------------------------------------------------------------------------
//
EXPORT_C void CDiagResultsDbItemBuilder::SetTestCompleted( 
        CDiagResultsDatabaseItem::TResult aResult )
    {
    iResult = aResult;
    SetTimeCompleted( GetCurrentTime() );
    }


// ---------------------------------------------------------------------------
// CDiagResultsDbItemBuilder::TestResult
// ---------------------------------------------------------------------------
//
EXPORT_C CDiagResultsDatabaseItem::TResult CDiagResultsDbItemBuilder::TestResult() const
    {
    return iResult;
    }

// ---------------------------------------------------------------------------
// CDiagResultsDbItemBuilder::SetTimeStarted
// ---------------------------------------------------------------------------
//
EXPORT_C void CDiagResultsDbItemBuilder::SetTimeStarted( TTime aTime )
    {
    iStartTime = aTime;
    }

// ---------------------------------------------------------------------------
// CDiagResultsDbItemBuilder::TimeStarted
// ---------------------------------------------------------------------------
//
EXPORT_C TTime CDiagResultsDbItemBuilder::TimeStarted() const
    {
    return iStartTime;
    }

// ---------------------------------------------------------------------------
// CDiagResultsDbItemBuilder::SetTimeCompleted
// ---------------------------------------------------------------------------
//
EXPORT_C void CDiagResultsDbItemBuilder::SetTimeCompleted( TTime aTime )
    {
    iEndTime = aTime;
    }

// ---------------------------------------------------------------------------
// CDiagResultsDbItemBuilder::TimeCompleted
// ---------------------------------------------------------------------------
//
EXPORT_C TTime CDiagResultsDbItemBuilder::TimeCompleted() const
    {
    return iStartTime;
    }

// ---------------------------------------------------------------------------
// CDiagResultsDbItemBuilder::SetResultDetail
// ---------------------------------------------------------------------------
//
EXPORT_C void CDiagResultsDbItemBuilder::SetResultDetail( 
        MDiagResultDetail* aResultDetail )
    {
    delete iResultDetail;
    iResultDetail = aResultDetail;
    }

// ---------------------------------------------------------------------------
// CDiagResultsDbItemBuilder::ResultDetail
// ---------------------------------------------------------------------------
//
EXPORT_C MDiagResultDetail& CDiagResultsDbItemBuilder::ResultDetail() const
    {
    __ASSERT_ALWAYS( iResultDetail, Panic( EDiagFrameworkInvalidState ) );
    return *iResultDetail;
    }

// ---------------------------------------------------------------------------
// CDiagResultsDbItemBuilder::ToResultsDatabaseItemL
// ---------------------------------------------------------------------------
//
EXPORT_C CDiagResultsDatabaseItem* CDiagResultsDbItemBuilder::ToResultsDatabaseItemL() const
    {
    CBufFlat* detailsData = NULL;

    if ( iResultDetail )
        {
        detailsData = iResultDetail->ExternalizeToBufferL();
        }

    return CDiagResultsDatabaseItem::NewL( iTestUid,
                                           iDependencyExecution,
                                           iResult,
                                           iStartTime,
                                           iEndTime,
                                           detailsData ); // ownership tranferred.
    }

// ---------------------------------------------------------------------------
// CDiagResultsDbItemBuilder::CreateSimpleDbItemL
// ---------------------------------------------------------------------------
//
EXPORT_C CDiagResultsDatabaseItem* CDiagResultsDbItemBuilder::CreateSimpleDbItemL(
        TUid aTestUid,
        TBool aAsDependency,
        CDiagResultsDatabaseItem::TResult aResultType )
    {
    CDiagResultsDbItemBuilder* resultBuilder = 
        CDiagResultsDbItemBuilder::NewL( aTestUid,
                                         aAsDependency );
    resultBuilder->SetTestCompleted( aResultType );

    CleanupStack::PushL( resultBuilder );
    CDiagResultsDatabaseItem* dbItem = resultBuilder->ToResultsDatabaseItemL();
    CleanupStack::PopAndDestroy( resultBuilder );

    return dbItem;
    }

// End of File

