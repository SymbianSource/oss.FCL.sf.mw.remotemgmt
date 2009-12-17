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
* Description: 
*     Provisioning context list view
*
*/


#ifndef CWPCXVIEW_H
#define CWPCXVIEW_H

// INCLUDES
#include <aknview.h>
#include "MWPContextObserver.h"

// CONSTANTS
// UID of view
const TUid KProvisioningCxViewId = {1};

// FORWARD DECLARATIONS
class CWPCxContainer;
class CWPEngine;

// CLASS DECLARATION

/**
*  CProvisioningDumpView view class.
* 
*/
class CWPCxView : public CAknView, private MWPContextObserver
    {
    public: // Constructors and destructor

        /**
        * EPOC default constructor.
        */
        void ConstructL();

        /**
        * Destructor.
        */
        ~CWPCxView();

    public: // from CAknView
        
        TUid Id() const;
        void HandleCommandL(TInt aCommand);
        void HandleClientRectChange();

    private: // from CAknView

        void DoActivateL(const TVwsViewId& aPrevViewId,TUid aCustomMessageId,
            const TDesC8& aCustomMessage);
        void DoDeactivate();

    public: // From MWPContextObserver
        void ContextChangeL( RDbNotifier::TEvent aEvent );

    private:
        // From MEikMenuObserver
        void DynInitMenuPaneL(TInt aResourceId,CEikMenuPane* aMenuPane);

    private: // new methods
        void DeleteContextL();
        void LaunchPopupL();

    private: // Data
        /// The context list container. Owns.
        CWPCxContainer* iContainer;

        /// The ProvisioningEngine instance. Owns.
        CWPEngine* iEngine;

    };

#endif

// End of File
