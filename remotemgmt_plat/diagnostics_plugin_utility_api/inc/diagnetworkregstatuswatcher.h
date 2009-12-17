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
* Description:  This is the header for the Diagnostics Network registration
*                 status watcher.
*
*/


#ifndef DIAGNETWORKREGSTATUSWATCHER_H
#define DIAGNETWORKREGSTATUSWATCHER_H

// Include files
#include <e32base.h>        // CActive
#include <etel3rdparty.h>   // CTelephony

//Forward declaration
class MDiagNetworkRegStatusObserver;


/**
* Diagnostics Network registration status watcher
*
* This class can be used as an observer for network status. After 
* instantiating the class and calling StartObserver(), the class firstly 
* informs the client of the current registration status and then goes into
* an observing mode for any changes in the network registration status.
*
*/
class CDiagNetworkRegStatusWatcher : public CActive
    {
public:

    /**
    * Symbian OS constructor.
    *
    * @param aCallBack A reference to the callback for this network
             registration status watcher
    * @return An pointer to the new instance of CDiagNetworkRegStatusWatcher.
    */
    IMPORT_C static CDiagNetworkRegStatusWatcher* NewL(
                    MDiagNetworkRegStatusObserver& aCallBack );
    
    /**
    * Default C++ virtual destructor
    */               
    IMPORT_C virtual ~CDiagNetworkRegStatusWatcher();
    
    /**
    * This function is used by client to start the watcher. Firstly, the
    * watcher will inform the clients via call
    * MDiagNetworkRegStatusObserver::InitialNetworkRegistrationStatusL()
    * and then go into an observing mode for any changes in the network
    * registration status. The changes status is informed via 
    * MDiagNetworkRegStatusObserver::NetworkRegistrationStatusChangeL()
    */    
    IMPORT_C void StartObserver();
    

    /**
    * This function is used by client to stop the observer.
    */     
    IMPORT_C void StopObserver();

private:

    /**
    * C++ constructor.
    *
    * @param aCallBack A reference to the callback for Network
    * registration status observer
    */
    CDiagNetworkRegStatusWatcher( MDiagNetworkRegStatusObserver&
                                                            aCallBack );
   
    /**
    * Symbian C++ 2nd phase constructor.
    *
    */    
    void ConstructL();

private: // From base class CActive

    /**
    * Handle active object completion event.
    */
    virtual void RunL();

    /**
    * Handle active object cancel event.
    */
    virtual void DoCancel();
    
private: // private functions

    /**
    * Handler for the internal state EStateWaitForInitialStatus
    */
    void HandleInitialStatusL();
    
    /**
    * Handler for the internal state EStateWaitForStatusChange
    */    
    void HandleNetworkStatusChangeL();

    /**
    * This function wraps the logic to decide if the device is currently 
    * registered or not.
    *
    * @return ETrue if the devide is registered, EFalse if the device is 
    * not registered.
    */
    TBool IsDeviceRegisteredOnNetwork();
    
private:

    /** States of the Network registration status watcher */
    enum TDiagNetwRegStatusWatcherState
        {
        EStateInit = 0,
        EStateWaitForInitialStatus,
        EStateWaitForStatusChange,
        EStateStopping
        };
    
    /** variable to store the internal state of the network registration
    *   status watcher
    */
    TDiagNetwRegStatusWatcherState iState;        

    /** Interface to phone's telephony system.  Own. */
    CTelephony* iTelephony;
    
    /** Defines the current network registration status */    
    CTelephony::TNetworkRegistrationV1 iRegV1;
    
    /** A typedef'd packaged CTelephony::TNetworkRegistrationV1 for
    passing through a generic API method. */    
    CTelephony::TNetworkRegistrationV1Pckg iRegV1Pckg;        
                        
    /** The callback interface for this instance of Network
    registration status observer */
    MDiagNetworkRegStatusObserver& iCallBack; 

    /** This variable remembers whether in previous state the 
    * the device was registered or not
    */
    TBool iPreviouslyRegistered;
    };

#endif // DIAGNETWORKREGSTATUSWATCHER_H

//End of file

