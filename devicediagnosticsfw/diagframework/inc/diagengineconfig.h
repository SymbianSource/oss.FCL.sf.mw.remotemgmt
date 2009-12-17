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
* Description:  Class declaration for TDiagEngineConfig
*
*/


#ifndef DIAGENGINECONFIG_H
#define DIAGENGINECONFIG_H

// SYSTEM INCLUDE FILES
#include <e32base.h>                // NONSHARABLE_CLASS

// FORWARD DECLARATIONS
class CRepository;

/**
* Diagnostics Engine Configuration.
*
* This class implements TDiagEngineConfig class. This class stores configuration
* information.
*
* @since S60 v5.0
*
*/
NONSHARABLE_CLASS( TDiagEngineConfig )
    {
public:     // public interface
    /**
    * C++ default contructor
    */
    TDiagEngineConfig();

    /**
    * Read Cenrep key vaules
    */
    void ReadCenrepKeysL();

    /**
    * Get watchdog timeout vaule for interactive plug-ins.
    * 
    * @return Watchdog timeout value for interactive plug-ins. 
    */
    const TTimeIntervalMicroSeconds32& WatchdogTimeoutValueInteractive() const;

    /**
    * Get watchdog timeout vaule for automatic plug-ins.
    * 
    * @return Watchdog timeout value for automatic plug-ins. 
    */
    const TTimeIntervalMicroSeconds32& WatchdogTimeoutValueAutomatic() const;

    /**
    * Get test plug-in initial delay vaule.
    * 
    * @return Initial test plug-in delay vaule.
    *   Unit is in micro seconds
    */
    const TTimeIntervalMicroSeconds32& TestPluginInitialDelay() const;

    /**
    * Set whether engine is configured to disable dependency checks.
    *
    * @param aDisableDependency - ETrue to change configuration to disable
    *   dependency. EFalse to change configuration to enable dependency.
    */
    void SetDependencyDisabled( TBool aDisableDependency );

    /**
    * Get disable depndency value.
    *
    * @return - ETrue if dependency is disabled.
    *   EFalse if dependency is enabled.
    */
    TBool IsDependencyDisabled() const;

private:    // private methods
    /**
    * Read TTimeIntervalMicroSeconds32 value from cenrep.
    *   
    * @param aCenrep - Reference to Cenrep object to read timeout value from.
    * @param aKey - Cenrep key to read.
    * @param aValue - If successful, this variable will be updated with
    *   the value from cenrep.
    */
    void ReadTimeIntervalFromCenrepL( CRepository& aCenrep,
                                      TUint32 aKey,
                                      TTimeIntervalMicroSeconds32& aValue );

private:    // private data
    /**
    * iCenRepWatchdogTimeout - Watchdog timer value for Interactive steps.
    *   Unit is in microseconds.
    */
    TTimeIntervalMicroSeconds32 iCenRepWatchdogTimeoutInteractive;
    
    /**
    * iCenRepWatchdogTimeout - Watchdog timer value for automatic steps.
    *   Unit is in microseconds.
    */
    TTimeIntervalMicroSeconds32 iCenRepWatchdogTimeoutAutomatic;

    /**
    * iCenRepInitDelay - Initial delay for test plug-ins.
    *   Unit is in microseconds.
    */
    TTimeIntervalMicroSeconds32 iCenRepInitDelay;

    /**
    * iDisableDependency - If Enabled, dependency will not be executed.
    * From constructor input.
    */
    TBool iDisableDependency;

    };

#endif // DIAGENGINECONFIG_H

// End of File

