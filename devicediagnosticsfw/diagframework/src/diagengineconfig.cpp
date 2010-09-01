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
* Description:  Class definition of TDiagEngineConfig
*
*/


// CLASS DECLARATION
#include "diagengineconfig.h"       

// SYSTEM INCLUDE FILES
#include <centralrepository.h>              // CRepository
#include <DiagFrameworkDebug.h>             // LOGSTRING

// USER INCLUDE FILES
#include "diagnosticsfwprivatecrkeys.h"     // Private CenRep Key

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// TDiagEngineConfig::TDiagEngineConfig()
// ---------------------------------------------------------------------------
//
TDiagEngineConfig::TDiagEngineConfig()
    :   iCenRepWatchdogTimeoutInteractive( 0 ),
        iCenRepWatchdogTimeoutAutomatic( 0 ),
        iCenRepInitDelay( 0 ),
        iDisableDependency( EFalse )
    {
    // note that this is a T class, so all member must be initialized explicitly.
    }

// ---------------------------------------------------------------------------
// TDiagEngineConfig::ReadCenrepKeysL
// ---------------------------------------------------------------------------
//
void TDiagEngineConfig::ReadCenrepKeysL()
    {
    CRepository* cenrep = NULL;
    cenrep = CRepository::NewLC( KCRUidDiagnosticsFw );

    ReadTimeIntervalFromCenrepL( *cenrep, 
                                 KDiagFwWatchdogTimeoutInteractive, 
                                 iCenRepWatchdogTimeoutInteractive );
    ReadTimeIntervalFromCenrepL( *cenrep,
                                 KDiagFwWatchdogTimeoutAutomatic, 
                                 iCenRepWatchdogTimeoutAutomatic );
    ReadTimeIntervalFromCenrepL( *cenrep,
                                 KDiagFwTestInitDelay,
                                 iCenRepInitDelay );
       
    CleanupStack::PopAndDestroy( cenrep );
    cenrep = NULL;

    LOGSTRING2( "TDiagEngineConfig::ReadCenrepKeysL() "
        L"KDiagFwWatchdogTimeoutInteractive: %d micro seconds", 
        iCenRepWatchdogTimeoutInteractive.Int() )
    LOGSTRING2( "TDiagEngineConfig::ReadCenrepKeysL() "
        L"KDiagFwWatchdogTimeoutAutomatic: %d sec",
        iCenRepWatchdogTimeoutAutomatic.Int() )
    LOGSTRING2( "TDiagEngineConfig::ReadCenrepKeysL(): "
        L"KDiagFwTestInitDelay: %d microsec", 
        iCenRepInitDelay.Int() )
    }

// ---------------------------------------------------------------------------
// TDiagEngineConfig::ReadTimeIntervalFromCenrepL
// ---------------------------------------------------------------------------
//
void TDiagEngineConfig::ReadTimeIntervalFromCenrepL( 
        CRepository& aCenrep,
        TUint32 aKey,
        TTimeIntervalMicroSeconds32& aValue )
    {
    TInt timeoutValue = 0;
    User::LeaveIfError( aCenrep.Get( aKey, timeoutValue ) );
    aValue = timeoutValue;
    }

// ---------------------------------------------------------------------------
// TDiagEngineConfig::WatchdogTimeoutValueInteractive
// ---------------------------------------------------------------------------
//
const TTimeIntervalMicroSeconds32& TDiagEngineConfig::WatchdogTimeoutValueInteractive() const
    {
    return iCenRepWatchdogTimeoutInteractive;
    }

// ---------------------------------------------------------------------------
// TDiagEngineConfig::WatchdogTimeoutValueAutomatic
// ---------------------------------------------------------------------------
//
const TTimeIntervalMicroSeconds32& TDiagEngineConfig::WatchdogTimeoutValueAutomatic() const
    {
    return iCenRepWatchdogTimeoutAutomatic;
    }

// ---------------------------------------------------------------------------
// TDiagEngineConfig::TestPluginInitialDelay
// ---------------------------------------------------------------------------
//
const TTimeIntervalMicroSeconds32& TDiagEngineConfig::TestPluginInitialDelay() const
    {
    return iCenRepInitDelay;
    }

// ---------------------------------------------------------------------------
// TDiagEngineConfig::SetDependencyDisabled
// ---------------------------------------------------------------------------
//
void TDiagEngineConfig::SetDependencyDisabled( TBool aDisableDependency )
    {
    iDisableDependency = aDisableDependency;
    }

// ---------------------------------------------------------------------------
// TDiagEngineConfig::IsDependencyDisabled
// ---------------------------------------------------------------------------
//
TBool TDiagEngineConfig::IsDependencyDisabled() const
    {
    return iDisableDependency;
    }

// End of File

