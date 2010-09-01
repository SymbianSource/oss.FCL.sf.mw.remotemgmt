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
 * Description:  Methods for the SyncML Appserver starter notifier
 *
 */

#include "syncmlmmcwatcher.h"

// -----------------------------------------------------------------------------
// CSyncmlmmcwatcher::NewL()
// -----------------------------------------------------------------------------
CSyncmlmmcwatcher* CSyncmlmmcwatcher::NewL( MSyncMLQueryMmcObserver* aObserver )
    {
    CSyncmlmmcwatcher* self = new( ELeave ) CSyncmlmmcwatcher( aObserver );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// -----------------------------------------------------------------------------
// CSyncmlmmcwatcher::CSyncmlmmcwatcher()
// -----------------------------------------------------------------------------
inline CSyncmlmmcwatcher::CSyncmlmmcwatcher(MSyncMLQueryMmcObserver* aObserver)    
    {    
    immcobserver = aObserver;
    }

// -----------------------------------------------------------------------------
// CSyncmlmmcwatcher::ConstructL()
// -----------------------------------------------------------------------------
void CSyncmlmmcwatcher::ConstructL()
    {    
    TInt err = iMemoryCard.Connect();
    if ( err == KErrNone )
        {
        }

    else
        User::LeaveIfError(err);
    iDiskNotifyHandler = CDiskNotifyHandler::NewL( *this, iMemoryCard );
    }

// -----------------------------------------------------------------------------
// CSyncmlmmcwatcher::StartL()
// Logs a request to notify the disk events
// -----------------------------------------------------------------------------
void CSyncmlmmcwatcher::StartL()
    {   
    FLOG(_L("CSyncmlmmcwatcher::Start- begin"));

    if(iDiskNotifyHandler)
        {
        iDiskNotifyHandler->NotifyDisk();
        }
    }

// ----------------------------------------------------------------------------------------
// CSyncmlmmcwatcher::~CSyncmlmmcwatcher() 
// Destructor
// ----------------------------------------------------------------------------------------
CSyncmlmmcwatcher::~CSyncmlmmcwatcher()
    { 
    FLOG(_L("CSyncmlmmcwatcher::~CSyncmlmmcwatcher()"));  
    if(iDiskNotifyHandler)
        {
        iDiskNotifyHandler->CancelNotifyDisk();
        delete iDiskNotifyHandler;
        iDiskNotifyHandler = NULL;
        }

    if(iMemoryCard.Handle())
        {
        iMemoryCard.Close();
        }
    }

// ---------------------------------------------------------------------------
// CSyncmlmmcwatcher::HandleNotifyDisk
// Callback method to notify disk events
// ---------------------------------------------------------------------------
//
void CSyncmlmmcwatcher::HandleNotifyDisk( TInt aError, const TDiskEvent& aEvent )
    {
    FTRACE( FPrint(
            _L( "CSysApMMCObserver::HandleNotifyDisk: error: %d, type: %d, drive: %d," ),
            aError, aEvent.iType, aEvent.iDrive ) );

    if ( aError != KErrNone )
        {
        return;
        }
    else
        {
        if(immcobserver)
            {
            FLOG(_L("mmc removal check"));
            TInt diskinserted = EFalse;
            RProperty::Get(KPSUidUikon, KUikMMCInserted,diskinserted);
            if(diskinserted)//EDiskStatusChanged called for insert & hard removal
                {
                FLOG(_L("disk inserted"));
                }
            else
                {
                FLOG(_L("disk removed"));
                immcobserver->MmcRemoved();
                }
            FLOG(_L("mmcremoved call completed"));
            }
        }

    }

// ---------------------------------------------------------------------------
// CSyncmlmmcwatcher::CancelMmcwatch
// Cancels the notification
// ---------------------------------------------------------------------------
//
void CSyncmlmmcwatcher::CancelMmcwatch()
    {
    if(iDiskNotifyHandler)
        {
        iDiskNotifyHandler->CancelNotifyDisk();
        }
    }
