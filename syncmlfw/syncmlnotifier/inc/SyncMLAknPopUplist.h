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

#ifndef CSYNCMLAKNPOPUPLIST_H
#define CSYNCMLAKNPOPUPLIST_H

//  INCLUDES
#include <aknPopup.h>   // CAknPopupList
#include <aknlists.h>
#include <SyncMLNotifierParams.h>
#include "SyncMLQueryTimer.h"
#include "syncmlmmcwatcher.h"
#include "SyncMLNotifDebug.h"
// CONSTANTS
const TInt  KSyncMLNNoListTimeout  = 0; // The query does not time out

// CLASS DECLARATION

/**
*  List query with timeout.
*
*  @lib SyncMLNotifier
*  @since Series 60 5.2
*/
NONSHARABLE_CLASS ( CSyncMLAknPopUpList ) : public CAknPopupList,
                                                 public MSyncMLQueryTimerObserver,
                                                 public MSyncMLQueryMmcObserver
    {
    public:  
        
        /**
        * Two-phased constructor.
        */
        static CSyncMLAknPopUpList* NewL(CAknSingleHeadingPopupMenuStyleListBox* listBox,
                TInt aResId,AknPopupLayouts::TAknPopupLayouts aPopuplayout,
                TInt   aTimeout = KSyncMLNNoListTimeout);		           
        
        /**
         * Constructor.
         */
        CSyncMLAknPopUpList(TInt aTimeout = KSyncMLNNoListTimeout);
        
        /**
        * Destructor.
        */
        virtual ~CSyncMLAknPopUpList();
        
        /**
         * From CEikDialog.
         *
         * Loads, displays, and destroys the dialog.
         */
        TBool ExecuteLD();

    public: // Functions from base classes

        /**
        * From MSyncMLQueryTimerObserver Is called when the timer
        * runs out.
        * @since Series 60 3.0
        * @param None
        * @return None
        */
        void TimerExpired();
        
        /**
         * From MSyncMLQueryMmcObserver is called when mmc removed
         * @since Series 60 5.2
         * @param None
         * @return None 
         */
        void MmcRemoved();
        
        /**
         * returns the status of mmc removal
         * @since Series 60 5.2
         * @param None
         * @return ETrue/EFalse
         */
		TBool IsMmcRemoved();
        
    protected:                               
        /**
         * Constructs a pop up list box 
         * @since Series 60 5.2
         * @param aListBox, reference to CAknSingleHeadingPopupMenuStyleListBox
         * @param aCbaResource, resource id of CBA
         * @param aType, popup layout of the list
         * @return None
         */
        void ConstructL(CAknSingleHeadingPopupMenuStyleListBox* aListBox, 
                                     TInt aCbaResource,
                                     AknPopupLayouts::TAknPopupLayouts aType );
        
    private:    
        // Number of seconds before the query is timed out. 0 = no timeout.
        TInt               iTimeout;
        
        //Current state of MMC removal
        TBool immcremoved;
        
        // Pointer to the timer instance
        CSyncMLQueryTimer*  iTimer;
        
        //Pointer to MMC watcher
        CSyncmlmmcwatcher*  iMmcwatcher;       
    };

#endif      // CSYNCMLTIMEDMESSAGEQUERY_H   
            
// End of File
