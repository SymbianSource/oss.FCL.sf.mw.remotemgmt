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
* Description:  Methods to parse the server alert messages for data sync and device
*                Management frameworks. Launches respective application 
*                in appropriate conditions
*
*/

#include "SyncMLAknPopUplist.h"

const TInt  KSyncMLuSecsInSec   = 1000000; // Microseconds in a second

// -----------------------------------------------------------------------------
// CSyncMLAknPopUpList::CSyncMLAknPopUpList()
// Constructor
// -----------------------------------------------------------------------------
//
CSyncMLAknPopUpList::CSyncMLAknPopUpList(TInt aTimeout ):CAknPopupList()
    {
    iTimeout = aTimeout;
    iMmcwatcher = NULL;
    iTimer = NULL;
	immcremoved = EFalse;
    }

// -----------------------------------------------------------------------------
// CSyncMLAknPopUpList::NewL()
// -----------------------------------------------------------------------------
//
CSyncMLAknPopUpList* CSyncMLAknPopUpList::NewL(CAknSingleHeadingPopupMenuStyleListBox* listBox,
        TInt aResId,AknPopupLayouts::TAknPopupLayouts aPopuplayout,
        TInt   aTimeout  )
    {
    CSyncMLAknPopUpList* self = new( ELeave ) CSyncMLAknPopUpList(aTimeout);
    CleanupStack::PushL( self );
    self->ConstructL(listBox,aResId,aPopuplayout );
    CleanupStack::Pop( self );
    return self;
    }

// -----------------------------------------------------------------------------
// CSyncMLAknPopUpList::ConstructL()
// Second phase constructor
// -----------------------------------------------------------------------------
//
void CSyncMLAknPopUpList::ConstructL(CAknSingleHeadingPopupMenuStyleListBox* aListBox, 
                                     TInt aCbaResource,
                                     AknPopupLayouts::TAknPopupLayouts aType )
    {
    CAknPopupList::ConstructL(aListBox,aCbaResource,aType);
    }

// -----------------------------------------------------------------------------
// CSyncMLAknPopUpList::~CSyncMLAknPopUpList()
// Destructor.
// -----------------------------------------------------------------------------
//
CSyncMLAknPopUpList::~CSyncMLAknPopUpList()
    {
    if ( iTimer )
        {
        iTimer->Cancel();
        delete iTimer;
        iTimer = NULL;
        }
   
    if(iMmcwatcher)
        {
        iMmcwatcher->CancelMmcwatch();
        delete iMmcwatcher;
        iMmcwatcher = NULL;
        }
    }
 
// -----------------------------------------------------------------------------
// CSyncMLAknPopUpList::TimerExpired
// -----------------------------------------------------------------------------
//
void CSyncMLAknPopUpList::TimerExpired()
    {
    if ( iTimer )
        {
        iTimer->Cancel();
        delete iTimer;
        iTimer = NULL;
        iTimeout = KSyncMLNNoListTimeout;
        }
    if(iMmcwatcher)
        {
        iMmcwatcher->CancelMmcwatch();
        }
    CancelPopup();
       
    }
    
// -----------------------------------------------------------------------------
// CSyncMLAknPopUpList::ExecuteLD
// -----------------------------------------------------------------------------
//
TBool CSyncMLAknPopUpList::ExecuteLD()
    {
    if ( iTimeout > KSyncMLNNoListTimeout )
        {
        iTimer = CSyncMLQueryTimer::NewL( this );
        iTimer->After( iTimeout * KSyncMLuSecsInSec );
        }
    iMmcwatcher = CSyncmlmmcwatcher::NewL(this);
    iMmcwatcher->StartL();
    return CAknPopupList::ExecuteLD();
    }

// -----------------------------------------------------------------------------
// CSyncMLAknPopUpList::MmcRemoved
// -----------------------------------------------------------------------------
//
void CSyncMLAknPopUpList::MmcRemoved()
    {
    FLOG(_L("CSyncMLAknPopUpList::MmcRemoved- cancelling the pop up"));
	immcremoved = ETrue;
    CancelPopup();
    FLOG(_L("CSyncMLAknPopUpList::MmcRemoved- cancelling the pop up done"));
    }
	
// -----------------------------------------------------------------------------
// CSyncMLAknPopUpList::IsMmcRemoved
// -----------------------------------------------------------------------------
//
TBool CSyncMLAknPopUpList::IsMmcRemoved()
    {
    return immcremoved;
    }
