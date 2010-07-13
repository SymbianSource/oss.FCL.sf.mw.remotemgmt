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

// INCLUDE FILES

//System Includes
#include <startupdomainpskeys.h>	//GlobalRFsStates
#include <rconnmon.h>	//RConnectionMonitor
#include <featmgr.h>	//Feature Manager
#include <features.hrh>	//Feature Manager
#include <cmconnectionmethoddef.h>
#include <cmmanagerext.h>
#include <cmpluginwlandef.h>
#include <cmpluginpacketdatadef.h>

//User Includes
#include "FotaServer.h"
#include "FotaNetworkRegStatus.h"

// -----------------------------------------------------------------------------
// CFotaNetworkRegStatus::NewL
// Symbian 2-Phase construction, NewL used for creating object of this class
// This method can leave
// -----------------------------------------------------------------------------

CFotaNetworkRegStatus* CFotaNetworkRegStatus::NewL(CFotaServer* aObserver)
    {
    CFotaNetworkRegStatus* self = CFotaNetworkRegStatus::NewLC(aObserver);
    CleanupStack::Pop(self);
    return self;
    }

// -----------------------------------------------------------------------------
// CFotaNetworkRegStatus::NewLC
// Symbian 2-Phase construction, NewLC used for creating object of this class
// This method can leave
// -----------------------------------------------------------------------------

CFotaNetworkRegStatus* CFotaNetworkRegStatus::NewLC(CFotaServer* aObserver)
    {
    CFotaNetworkRegStatus* self = new (ELeave) CFotaNetworkRegStatus(
            aObserver);
    CleanupStack::PushL(self);
    self->ConstructL();

    return self;
    }

// -----------------------------------------------------------------------------
// CFotaNetworkRegStatus::ConstructL
// Symbian 2-Phase construction, ConstructL used for constructing the members of this class
// This method can leave
// -----------------------------------------------------------------------------

void CFotaNetworkRegStatus::ConstructL()
    {
    iTimer.CreateLocal();
    iMonitor.ConnectL();
    }

// -----------------------------------------------------------------------------
// CFotaNetworkRegStatus::CFotaNetworkRegStatus
// C++ Constructor
// This method shouldn't leave
// -----------------------------------------------------------------------------

CFotaNetworkRegStatus::CFotaNetworkRegStatus(CFotaServer* aObserver) :
    CActive(CActive::EPriorityStandard), iObserver(aObserver), iRetriesLeft(
            KRetries), iGlobalRFState(EFalse)
    {
    CActiveScheduler::Add(this); // Add AO to current active scheduler
    }

// -----------------------------------------------------------------------------
// CFotaNetworkRegStatus::~CFotaNetworkRegStatus
// C++ Desctructor
// This method shouldn't leave
// -----------------------------------------------------------------------------

CFotaNetworkRegStatus::~CFotaNetworkRegStatus()
    {
    Cancel();

    iTimer.Close();
    iMonitor.Close();
    }

// -----------------------------------------------------------------------------
// CFotaNetworkRegStatus::DoCancel()
// Cancels currently active notifier, if such exists
// -----------------------------------------------------------------------------
//
void CFotaNetworkRegStatus::DoCancel()
    {
    FLOG(_L("CFotaNetworkRegStatus::DoCancel >>"));

    if (IsActive())
        {
        iTimer.Cancel();
        //		Cancel();
        }

    FLOG(_L("CFotaNetworkRegStatus::DoCancel <<"));
    }

// -----------------------------------------------------------------------------
// CFotaNetworkRegStatus::StartMonitoringL
// Monitors for connection status
// This method don't leave
// -----------------------------------------------------------------------------

void CFotaNetworkRegStatus::StartMonitoringL()
    {
    FLOG(_L("CFotaNetworkRegStatus::StartMonitoringL >>"));

    //Check offline state
    FLOG(_L("Check GlobalRF state..."));
    if (!iGlobalRFState)
        {
        iGlobalRFState = CheckGlobalRFState();
        }

    if (iGlobalRFState)
        {
        //Check registration state only if iGlobalRFState is true
        FLOG(_L("Check registration state..."));
        if (CheckNetworkRegStateL())
            {
            //If both are successful report complete
            FLOG(
                    _L("Network Registration is successful, sending Status as success to FotaServer"));
            iObserver->ReportNetworkStatus(ETrue);
            return;
            }
        }
    //If one of them is not successful, start timer and retry KRetries times...

    if (--iRetriesLeft >= 0)
        {
        FLOG(_L("Retry count... [%d]"), KRetries - iRetriesLeft);
        iTimer.After(iStatus, KTimeInterval);
        if (!IsActive())
            SetActive();
        }
    else
        {
        FLOG(
                _L("Maximum retries (%d) reached, sending Status as failure to FotaServer"),
                KRetries);
        iObserver->ReportNetworkStatus(IsWlanSupportedL());
        }

    FLOG(_L("CFotaNetworkRegStatus::StartMonitoringL <<"));
    }

// -----------------------------------------------------------------------------
// CFotaNetworkRegStatus::RunL()
// Called when event accomplished
// -----------------------------------------------------------------------------
//
void CFotaNetworkRegStatus::RunL()
    {
    FLOG(_L("CFotaNetworkRegStatus::RunL >>"));

    if (iStatus == KErrNone)
        {
        StartMonitoringL();
        }
    else
        {
        iObserver->ReportNetworkStatus(EFalse);
        }

    FLOG(_L("CFotaNetworkRegStatus::RunL <<"));
    }

// -----------------------------------------------------------------------------
// CFotaNetworkRegStatus::RunError
// Called when RunL leaves
// This method can't leave
// -----------------------------------------------------------------------------

TInt CFotaNetworkRegStatus::RunError(TInt aError)
    {
    FLOG(_L("CFotaNetworkRegStatus::RunL >>"));
    iObserver->ReportNetworkStatus(EFalse);
    FLOG(_L("CFotaNetworkRegStatus::RunL <<"));
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CFotaNetworkRegStatus::CheckGlobalRFState
// Checks for GlobalRFState, set by Starter
// This method can't leave
// -----------------------------------------------------------------------------

TBool CFotaNetworkRegStatus::CheckGlobalRFState()
    {
    FLOG(_L("CFotaNetworkRegStatus::CheckGlobalRFState >>"));

    RProperty prop;
    TInt val = KErrNone;
    TInt err = KErrNone;
    TInt status = EFalse;

    err = prop.Get(KPSUidStartup, KPSGlobalSystemState, val);
    if (err == KErrNone && val == ESwStateNormalRfOn)
        {
        //Phone is not offline. Check for Network Registration status
        FLOG(_L("Phone is online. Check for Network Registration status"));
        status = ETrue;
        }
    else
        {
        //Phone is offline. No Network activities allowed.
        FLOG(_L("Phone is offline. No Network activities allowed."));
        status = EFalse;
        }
    FLOG(_L("CFotaNetworkRegStatus::CheckGlobalRFState, status = %d <<"),
            status);
    return status;
    }

// -----------------------------------------------------------------------------
// CFotaNetworkRegStatus::CheckNetworkRegStatusL
// Checks for Network registration status
// This method can leave
// -----------------------------------------------------------------------------

TBool CFotaNetworkRegStatus::CheckNetworkRegStateL()
    {
    FLOG(_L("CFotaNetworkRegStatus::CheckNetworkRegStateL >>"));

    TBool status = EFalse;

    TInt registrationStatus(KErrNone);
    TRequestStatus status1;
    iMonitor.GetIntAttribute(EBearerIdGSM, // See bearer ids from TConnMonBearerId
            0, KNetworkRegistration, registrationStatus, status1);
    User::WaitForRequest(status1);

    if (status1.Int() == KErrNone)
        {
        switch (registrationStatus)
            {
            case ENetworkRegistrationHomeNetwork:
            case ENetworkRegistrationRoaming:
                {
                status = ETrue;
                break;
                }
            default:
                {
                /* Includes - 	ENetworkRegistrationNotAvailable:
                 ENetworkRegistrationUnknown:
                 ENetworkRegistrationNoService:
                 ENetworkRegistrationEmergencyOnly:
                 ENetworkRegistrationSearching:
                 ENetworkRegistrationBusy:
                 ENetworkRegistrationDenied:*/
                status = EFalse;
                break;
                }
            }
        }

    FLOG(_L("CFotaNetworkRegStatus::CheckNetworkRegStateL, status = %d <<"),
            status);
    return status;
    }

// -----------------------------------------------------------------------------
// CFotaNetworkRegStatus::IsWlanSupportedL
// Checks whether Wlan supported on device and active
// This method can leave
// -----------------------------------------------------------------------------

TBool CFotaNetworkRegStatus::IsWlanSupportedL()
    {
    FLOG(_L("CFotaNetworkRegStatus::IsWlanSupportedL >>"));

    TBool status = EFalse;

    FeatureManager::InitializeLibL();
    if (FeatureManager::FeatureSupported(KFeatureIdProtocolWlan)) // check for feature enabled
        {
        status = ETrue;
        }

    FeatureManager::UnInitializeLib();

#if defined(__WINS__)
    status = ETrue;
#endif

    FLOG(_L("CFotaNetworkRegStatus::IsWlanSupportedL, status = %d <<"),
            status);
    return status;
    }

// -----------------------------------------------------------------------------
// CFotaNetworkRegStatus::IsConnectionPossibleL
// Checks whether the network connection is possible in the given IAP Id
// This method can leave
// -----------------------------------------------------------------------------
/*TBool CFotaNetworkRegStatus::IsConnectionPossibleL(TInt aIapid)
    {
    FLOG(_L("CFotaNetworkRegStatus::IsConnectionPossibleL >>"));
    TBool status(EFalse);

    TUint32 bearer = FindBearerL(aIapid);
    if (bearer == KUidPacketDataBearerType)
        {
        FLOG(_L("Bearer is Packet data"));
        if (CheckGlobalRFState() && CheckNetworkRegStateL())
            {
            FLOG(_L("Network is up and connection is possible "));
            status = ETrue;
            }
        else
            {
            FLOG(_L("Network is not up and connection is not possible "));
            status = EFalse;
            }
        }
    else if (bearer == KUidWlanBearerType)//for wlan or other bearers
        {
        FLOG(_L("Bearer is wlan and proceeding for download "));
        //proceed & this else loop to be removed
        status = ETrue;
        }
    else
        {
        FLOG(_L("Bearer is not packet data or WLAN"));
        }

    FLOG(_L("CFotaNetworkRegStatus::IsConnectionPossibleL, status = %d <<"),
            status);
    return status;
    }*/

// ----------------------------------------------------------------------------------------
// CFotaDownload::FindBearerId
// Finds the Bearer Id for a given IAP Id
// ----------------------------------------------------------------------------------------
/*TUint32 CFotaNetworkRegStatus::FindBearerL(TInt aIapId)
    {
    FLOG(_L("CFotaNetworkRegStatus::FindBearerL: %d"), aIapId);
    TUint32 bearer = 0;
    TInt err(KErrNone);
    // Query CM Id
    TInt cmId(aIapId);
    RCmManagerExt CmManagerExt;
    TRAP( err, CmManagerExt.OpenL() );
    FLOG(_L("CmManagerExt.OpenL() with error as  %d"), err);
    if (err == KErrNone)
        {
        RCmConnectionMethodExt cm;
        TRAP( err, cm = CmManagerExt.ConnectionMethodL( cmId ) );
        FLOG(_L("CmManagerExt.ConnectionMethodL with error as  %d"), err);
        if (err == KErrNone)
            {
            CleanupClosePushL(cm);
            FLOG(_L("cm pushed to cleanupstack "));
            bearer = cm.GetIntAttributeL(CMManager::ECmBearerType);
            FLOG(_L("bearer is %d "), bearer);
            CleanupStack::PopAndDestroy(); // cm	  
            FLOG(_L("cm poped & destroyed from cleanupstack "));
            }

        CmManagerExt.Close();
        FLOG(_L("CmManagerExt closed "));
        }
    FLOG(_L("CFotaNetworkRegStatus::FindBearerL end with bearer: %d"), bearer);
    return bearer;
    }*/

// End of File 
