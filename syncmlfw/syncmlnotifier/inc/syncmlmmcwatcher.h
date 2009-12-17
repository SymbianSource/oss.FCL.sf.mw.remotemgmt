/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Synchronisation server alert notifier.
*
*/

#ifndef SYNCMLMMCWATCHER_H_
#define SYNCMLMMCWATCHER_H_
#include <e32base.h>
#include <e32property.h>
#include <UikonInternalPSKeys.h>
#include <f32file.h> 
#include <disknotifyhandler.h> //if RD_MULTIPLE_DRIVE
#include "SyncMLNotifDebug.h"

/**
*  MMC event observer class for SyncML notifier queries.
*  @lib SyncMLNotifier
*  @since Series 60 5.2
*/
NONSHARABLE_CLASS ( MSyncMLQueryMmcObserver )
    {
    public: // New functions
        /**
        * Is called when the mmc removes.
        * @since Series 60 5.2
        * @param None
        * @return None
        */
        virtual void MmcRemoved() = 0;
    };

/**
*  MMC event watcher class for SyncML notifier queries.
*  @lib SyncMLNotifier
*  @since Series 60 5.2
*/
class CSyncmlmmcwatcher :  public CBase, public MDiskNotifyHandlerCallback /*public CActive,*/
    {                
public:  // Constructors and destructor

    /**
     * Two-phased constructor.
     */
    static CSyncmlmmcwatcher* NewL( MSyncMLQueryMmcObserver* aObserver );

    /**
     * Destructor.
     */     
    virtual ~CSyncmlmmcwatcher();
private:   
    /**
     * Constructor.
     */
    inline CSyncmlmmcwatcher( MSyncMLQueryMmcObserver* aObserver);       

public:
    /**
     * Logs a request to notify the disk events
     * @since Series 60 5.2
     * @param None
     * @return None
     */
    void StartL();
    
    /**
     * Callback method to notify disk events
     * @since Series 60 5.2
     * @param aError,System wide error code from file server
     * @param aEvent,The disk event data data specified by TDiskEvent
     * @return None
     */
    void HandleNotifyDisk( TInt aError, const TDiskEvent& aEvent );
    
    /**
     * Cancels the disk notification
     * @since Series 60 5.2
     * @param None     
     * @return None
     */
    void CancelMmcwatch();

private:

    /**
     * Symbian 2nd phase constructor.
     */
    void ConstructL();            

private:            
    //Instance to RFs
    RFs iMemoryCard;
    
    // Pointer to disk notify handler. Own.
    CDiskNotifyHandler* iDiskNotifyHandler;
    
    // Pointer to MMC observer
    MSyncMLQueryMmcObserver* immcobserver;
    }; 

#endif /* SYNCMLMMCWATCHER_H_ */

