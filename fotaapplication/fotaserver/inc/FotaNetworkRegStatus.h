/*
 * Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies). 
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
 * Description:   GPRS and Wlan status getter for sending Generic Alerts
 *
 */

#ifndef __FOTANETWORKREGSTATUS_H__
#define __FOTANETWORKREGSTATUS_H__

// SYSTEM INCLUDES
#include <e32base.h>
#include <rconnmon.h>

//Forward declarations
class CFotaServer;

//Constants used in this class

const TTimeIntervalMicroSeconds32 KTimeInterval = 1000000; //1 second, duration between each retry.
const TInt KRetries = 10; //Maximum number of retries.

/**
 * Actice object class that runs a timer for probing network status (GPRS & WLAN)
 *  @lib    fotaserver
 *  @since  S60 v3.2
 */
NONSHARABLE_CLASS (CFotaNetworkRegStatus) : public CActive
    {
public:
    //Symbian 2-Phase construction

    static CFotaNetworkRegStatus* NewL(CFotaServer* aObserver);
    static CFotaNetworkRegStatus* NewLC(CFotaServer* aObserver);

    /**
     * Destructor.
     */
    virtual ~CFotaNetworkRegStatus();

public:
    /**
     * Starts monitoring for Network status before sending Generic Alert
     *
     * @since   S60   v3.2
     * @param   None
     * @return  None
     */

    void StartMonitoringL();

    /** 
     * Checks whether the network connection possible with the IAP Id provided
     *
     * @since   S60   v3.2
     * @param   None
     * @return  ETrue if yes, EFalse when not possible
     */
    //TBool IsConnectionPossibleL(TInt aIapid);

public:
    // Functions from base classes
    /**
     * From CActive,DoCancel.
     */
    void DoCancel();

    /**
     * From CActive,RunL.
     */
    void RunL();

    /**
     * From CActive,RunError.
     */
    TInt RunError(TInt aError);

private:
    //functions

    //Constructors

    CFotaNetworkRegStatus();
    CFotaNetworkRegStatus(CFotaServer* aObserver);

    //Symbian 2-Phase construction 
    void ConstructL();

    /**
     * Checks GlobalRFs status
     *
     * @since   S60   v3.2
     * @param   None
     * @return  ETrue if success, EFalse if failure
     */
    TBool CheckGlobalRFState();

    /**
     * Checks Network status, basically Network Registration
     *
     * @since   S60   v3.2
     * @param   None
     * @return  ETrue if success, EFalse if failure
     */
    TBool CheckNetworkRegStateL();

    /**
     * Checks Wlan status
     *
     * @since   S60   v3.2
     * @param   None
     * @return  ETrue if success, EFalse if failure
     */
    TBool IsWlanSupportedL();

    /**
     * To find the bearer of the IapId
     *
     * @since   S60   v3.2
     * @param   aIapId
     * @return  ETrue/EFalse
     */
    //TUint32 FindBearerL(TInt aIapId);

private:
    // data

    /**
     * timer used for monitoring
     */
    RTimer iTimer;

    /**
     * S60 monitory class used for monitoring network status
     */
    RConnectionMonitor iMonitor;

    /**
     * FotaServer
     */
    CFotaServer* iObserver;

    /**
     * Number of retries
     */
    TInt iRetriesLeft;

    /**
     * GlobalRFs state
     */
    TBool iGlobalRFState;
    };

#endif // __FOTANETWORKREGSTATUS_H__
// End of File
