/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  BT device and service searcher
*
*/



#ifndef CBTOBEXSEARCHER_H
#define CBTOBEXSEARCHER_H

//  INCLUDES
#include <ObexSearcherBase.h>
#include <bttypes.h>
#include <obex.h>
#include <btengsettings.h>
#include "Btconninfo.h"

// FORWARD DECLARATIONS
class CNSmlObexServiceSearcher;
class CObexClient;

// CLASS DECLARATION

/**
 *	Searches for Bluetooth devices
 */
class CBTObexSearcher : public CObexSearcherBase , public MBTEngSettingsObserver
    {
    public:  // Constructors and destructor
        
        /**
        * Two-phased constructor.
        */
        static CBTObexSearcher* NewL( const TBTConnInfo& aBTConnInfo );
        
        /**
        * Destructor.
        */
        virtual ~CBTObexSearcher();

    protected: // Functions from base classes
        
        /**
        * Search for a device
        */
        void SearchDeviceL();

        /**
        * Search for a service
        */
        void SearchServiceL();

        /**
        * Create OBEX client
        */
		CObexClient* CreateObexClientL();

        void RunL();
        TInt RunError(TInt aError);

        void DoCancel();

       /**
         * From MBTEngSettingsObserver.
         * Provides notification of changes in the power state 
         * of the Bluetooth hardware.
         *
         * 
         * @param aState EBTPowerOff if the BT hardware has been turned off, 
         *               EBTPowerOn if it has been turned on.
         */
        void PowerStateChanged( TBTPowerStateValue aState );

        /**
         * From MBTEngSettingsObserver.
         * Provides notification of changes in the discoverability 
         * mode of the Bluetooth hardware.
         *
         * 
         * @param aState EBTDiscModeHidden if the BT hardware is in hidden mode, 
         *               EBTDiscModeGeneral if it is in visible mode.
         */
        void VisibilityModeChanged( TBTVisibilityMode aState );

    private:

        /**
        * C++ default constructor.
        */
        CBTObexSearcher();

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL( const TBTConnInfo& aBTConnInfo );

        /**
         * Checks the offline mode and queries the user for turning BT on if
         * required.
         *
         * 
         * @return ETrue if the phone is in offline mode, otherwise EFalse.
         */
        TBool CheckOfflineModeL();

    private:    // Data

		CNSmlObexServiceSearcher* iServiceSearcher;

        TBool iWaitingForBTPower;

        TPckgBuf<TBool> iOffline;

 //       RNotifier iNotifier;

        CBTEngSettings* iSettings;
    };

#endif      // CBTOBEXSEARCHER_H   
            
// End of File
